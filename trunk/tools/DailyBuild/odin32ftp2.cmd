/* $Id: odin32ftp2.cmd,v 1.12 2001-04-02 01:55:41 bird Exp $
 *
 * Uploads the relase and debug builds to the FTP sites.
 *
 * Copyright (c) 1999-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*Trace 'A'*/

rc = RxFuncAdd('FtpLoadFuncs','rxFtp','FtpLoadFuncs');
rc = FtpLoadFuncs();

parse arg sLoc

/*
 * Determin files to upload and files to delete.
 */
if (DATE('B')//7 = 3) then              /* Thursdays only! */
do  /* weekly .wpi build */
    asUploads.0 = 3;
    asUploads.1 = 'ChangeLog';
    asUploads.2 = 'odin32bin-'DATE('S')'-release.wpi';
    asUploads.3 = 'odin32bin-'DATE('S')'-debug.wpi';
end
else
do  /* daily .zip build */
    asUploads.0 = 3;
    asUploads.1 = 'ChangeLog';
    asUploads.2 = 'odin32bin-'DATE('S')'-release.zip';
    asUploads.3 = 'odin32bin-'DATE('S')'-debug.zip';
end

asDelete.0 = 25;
do i = 1 to 12
    j = i * 2;
    asDelete.j = 'odin32bin-'DateSub(Date('S'), 31+i)'-release.zip';
    j = j + 1;
    asDelete.j = 'odin32bin-'DateSub(Date('S'), 31+i)'-debug.zip';
end


/*
 * Execution loop.
 */
do i = 1 to 5 /* (Retries 5 times) */
    /*
     * Put files to SourceForge.
     */
    rc = cleanSF();
    do j = 1 to asUploads.0
        rc = putSF(asUploads.j, 'SF-'||asUploads.j);
    end

    /*
     * Forwards files from sourceforge(t) to os2.ftp.org
     */
    if (sLoc = '' | sLoc = 'os2') then
    do
        rc = cleanFtp('os2-delete', '/daily', 'www.os2.org');
        do j = 1 to asUploads.0
            rc = forwardSF(asUploads.j, 'os2-'||asUploads.j, '/daily', 'www.os2.org');
        end
    end

    /*
     * Upload files to netlabs.
     */
    if (sLoc = '' | sLoc = 'netlabs') then
    do
        rc = cleanFtp('netlabs-delete', '/daily', 'ftp.netlabs.org');
        do j = 1 to asUploads.0
            if (1) then
                rc = putFtp(asUploads.j, 'netlabs-'||asUploads.j, '/daily', 'ftp.netlabs.org');
            else
                rc = forwardSF(asUploads.j, 'netlabs-'||asUploads.j, '/daily', 'ftp.netlabs.org');
        end
    end
end

exit(0);


/*
 * Deletes all the files in /pub/kTaskMgr/daily.
 */
cleanSF: procedure

    sLockFile = 'SF-delete';
    if (stream(sLockFile,'c','query exists') = '') then
    do
        sSFDir = '/home/groups/ftp/pub/kTaskMgr/daily/';
        'ssh -f -n -l stknut kTaskMgr.sourceforge.net rm -f' sSFDir||'*';
        if (rc <> 0) then
        do
            say 'rm -f <>/* failed. rc='rc;
            return rc;
        end
        'echo ok >' sLockFile
    end

    return 0;


/*
 * Upload a file to source forge
 */
putSF: procedure
    parse arg sFile, sLockFile

    sSFDir = '/home/groups/ftp/pub/kTaskMgr/daily/';

    if (stream(sLockFile,'c','query exists') = '') then
    do
        'ssh -f -n -l stknut kTaskMgr.sourceforge.net rm -f' sSFDir||sFile;
        'scp' sFile 'stknut@kTaskMgr.sourceforge.net:'||sSFDir||sFile;
        if (rc <> 0) then
        do
            say 'scp' sFile 'failed. rc='rc;
            return rc;
        end
        'echo ok >' sLockFile
    end

    return 0;


/*
 * Forward file from SourceForge to ftp site.
 */
forwardSF: procedure
    parse arg sFile, sLockFile, sRemoteDir, sSite

    sSFDir = '/home/groups/ftp/pub/kTaskMgr/daily/';

    if (stream(sLockFile,'c','query exists') = '') then
    do
        /* get password */
        sPasswdString = GetPassword(sSite);
        if (sPasswdString = '') then
        do
            call failure rc, 'Can''t find userid/password for' sSite'.', -1;
            return -1;
        end
        parse var sPasswdString sUser':'sPasswd;

        /* invoke forward process on SourceForge(t). */
        'cls'
        say 'ssh -l stknut kTaskMgr.sourceforge.net upload' sUser sPasswd sSite sRemoteDir sFile;
        'ssh -l stknut kTaskMgr.sourceforge.net ./upload' sUser sPasswd sSite sRemoteDir sFile;
        if (rc <> 0) then
        do
            say 'ssh failed with rc='rc;
            return rc;
        end
        'echo ok >' sLockFile;
    end

    return 0;



/*
 * Puts a file to a ftp site using ncftpput from ncftp v3.0 beta.
 */
putFtp: procedure
    parse arg sFile, sLockFile, sRemoteDir, sSite

    /* check for done-lock */
    if stream(sLockFile,'c','query exists') = '' then
    do
        say '--- put' sFile '->' sRemoteDir'/'sFile '---';

        /* get password */
        sPasswdString = GetPassword(sSite);
        if (sPasswdString = '') then
        do
            call failure rc, 'Can''t find userid/password for' sSite'.', -1;
            return -1;
        end
        parse var sPasswdString sUser':'sPasswd;

        /* do the put */
        say 'ncftpput -u' sUser '-p' sPasswd '-z' sSite sRemoteDir sFile;
        'ncftpput -u' sUser '-p' sPasswd '-z -F' sSite sRemoteDir sFile;
        if (rc == 0) then
            'echo ok >' sLockFile;
        else
        do
            asErrors.0 = 7;
            asErrors.1 = 'Could not connect to remote host.';
            asErrors.2 = 'Could not connect to remote host - timed out.';
            asErrors.3 = 'Transfer failed.';
            asErrors.4 = 'Transfer failed - timed out.';
            asErrors.5 = 'Directory change failed.';
            asErrors.6 = 'Directory change failed - timed out.';
            asErrors.7 = 'Malformed URL.';
            if (rc < asErrors.0) then
                say 'ncftpput failed with rc='rc'-' asErrors.rc;
            else
                say 'ncftpput failed with rc='rc;
        end
    end
    else
        rc = 0;

    return 0;


/*
 * Delete the files in asDelete on a given ftp site.
 */
cleanFtp: procedure expose asDelete.;
    parse arg sLockFile, sRemoteDir, sSite

    if (stream(sLockFile,'c','query exists') = '') then
    do
        say '--- deleting old files at 'sSite' ---'

        /* get password */
        sPasswdString = GetPassword(sSite);
        if (sPasswdString = '') then
        do
            call failure rc, 'Can''t find userid/password for' sSite'.', -1;
            return -1;
        end
        parse var sPasswdString sUser':'sPasswd;

        /* start ftp'ing */
        rc = FtpSetUser(sSite, sUser, sPasswd);
        if (rc = 1) then
        do
            cErrors = 0;
            do i = 1 to asDelete.0
                rc = FtpDelete(sRemoteDir||'/'||asDelete.i)
                if (rc <> 0 & FTPERRNO = 'FTPCOMMAND') then /* happens when the file don't exists... too. */
                    rc = 0;
                if (rc <> 0) then
                do
                    call failure rc, 'FtpDelete failed - 'sRemoteDir||'/'||asDelete.i' -' sSite , FTPERRNO;
                    cErrors = cErrors + 1;
                end
            end

            if (cErrors = 0) then
                'echo ok >' sLockFile;
            else
                say 'delete failed with 'cErrors' times.';
            call FtpLogoff;
        end
        else
        do
            call failure rc, 'Logon failed -' sSite, FTPERRNO;
            if (rc = 0) then rc = -1;
        end
    end
    else
        rc = 0;

    return rc;




failure: procedure
parse arg rc, sText, iftperrno;
    say 'rc='rc sText
    say 'FTPerrno:'||iftperrno
    return;


/*
 * Reads the password file (passwd) in the script directory to
 *   get a password and userid for a given site.
 *
 * Format of the passwd file is:
 *  <site> <user> <password>
 * Lines starting with '#' is ignored.
 *
 * @param   sSite   name of the site.
 * @returns String on the form '<userid>:<passwd>' if found.
 *          Empty string if not found.
 */
GetPassword: procedure;
    parse upper arg sSiteToFind
    parse source sd1 sd2 sScript
    sPasswd = filespec('drive', sScript) || filespec('path', sScript)||'\passwd';

    rc = stream(sPasswd, 'c', 'open read');
    if (pos('READY', rc) <> 1) then
    do
        say 'failed to open ftp password file - rc='rc;
        return '';
    end

    sRet = '';
    do while (lines(sPasswd) > 0)
        sLine = strip(linein(sPasswd));
        if (sLine = '' | substr(sLine, 1, 1) = '#') then
            iterate;

        parse var sLine sSite' 'sUser' 'sPassword' 'sDummy
        sSite = translate(strip(sSite));
        sUser = strip(sUser);
        sPassword = strip(sPassword);
        if (sSite = '' | sPassword = '' | sUser = '') then
            say 'warning! misformed password line!';
        if (sSite = sSiteToFind) then
        do
           sRet = sUser||':'||sPassword;
           leave
        end
    end
    call stream sPasswd, 'c', 'close';
    return sRet;


testdatesub:
    sDate='20010131';

    do i = 1 to 365*2+1
        say sDate '-' i '=' DateSub(sDate, i);
    end
    exit;


/**
 * Finds date seven days ago.
 * @returns  yyyymmdd date
 * @param    sDate  Date on the yyyymmdd format.
 * @param    cDays  Number of days to subtract.
 * @remark   Works only for dates between 1000-01-01 and 9999-12-31
 *           including the limits.
 */
DateSub: procedure
parse arg sDate, cDays

    /* subtraction loop which updates sDate and cDays for each iteration. */
    do while (cDays > 0)
        /*
         * Get the day in month of sDate.
         * Do a simple subtraction is this is higher than the number of days to subtract.
         */
        iDayInMonth = substr(sDate, 7, 2);
        if (iDayInMonth > cDays) then
            return sDate - cDays;

        /*
         * Determin previous month and the number of days in it.
         */
        iMonth = substr(sDate, 5, 2);
        iYear = substr(sDate, 1, 4);
        if (iMonth > 1) then
            iPrvMonth = iMonth - 1;
        else
            iPrvMonth = 12;
        cDaysPrvMonth = DateGetDaysInMonth(iYear, iPrvMonth);

        /*
         * Update date and days left to subtract.
         */
        cDays = cDays - iDayInMonth;
        if (iMonth > 1) then
            sDate = sDate - iDayInMonth - 100 + cDaysPrvMonth; /* last day of previous month */
        else
            sDate = sDate - iDayInMonth - 8869;   /* last day of last year */
    end

    return sDate;

/*
 * Gets the number of days in a given month.
 * @param       iYear   the year.
 * @param       iMonth  the month.
 */
DateGetDaysInMonth: procedure
    parse arg iYear, iMonth

    select
        when (iMonth = 4 | iMonth = 6 | iMonth = 9 | iMonth = 11) then
            cDays = 30;

        when (iMonth = 2) then
        do
            if ((iYear // 4) = 0) & (((iYear // 400) <> 0) | ((iYear // 2000) = 0)) then
                cDays = 29;
            else
                cDays = 28;
        end

        otherwise
            cDays = 31;
    end /* select */
    return cDays;

