/* $Id: odin32ftp2.cmd,v 1.10 2001-02-23 14:41:51 bird Exp $
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

do i = 1 to 5 /* (Retries 5 times) */
    sFile           = 'odin32bin-'|| DATE(S);
    sDelete         = 'odin32bin-'|| DateSub(DATE('S'), 7);
    sFileDbg        = sFile   || '-debug.zip';
    sFileGlideDbg   = sFile   || '-glide-debug.zip';
    sFileDbgWPI     = sFile   || '-debug.wpi';
    sDeleteDbg      = sDelete || '-debug.zip';
    sDeleteGlideDbg = sDelete || '-glide-debug.zip';
    sDeleteDbgWPI   = sDelete || '-debug.wpi';
    sFileRel        = sFile   || '-release.zip';
    sFileGlideRel   = sFile   || '-glide-release.zip';
    sFileRelWPI     = sFile   || '-release.wpi';
    sDeleteRel      = sDelete || '-release.zip';
    sDeleteGlideRel = sDelete || '-glide-release.zip';
    sDeleteRelWPI   = sDelete || '-release.wpi';
    sChangeLog      = 'ChangeLog';

    /*
     * Cleanup the daily directory at source forge.
     */
    rc = cleanSF();

    /*
     * Put file to SourceForge.
     */
    rc = putSF(sFileDbg,        'SF-debug');
    rc = putSF(sFileGlideDbg,   'SF-glide-debug');
    rc = putSF(sFileDbgWPI,     'SF-debug-wpi');
    rc = putSF(sFileRel,        'SF-release');
    rc = putSF(sFileGlideRel,   'SF-glide-release');
    rc = putSF(sFileRelWPI,     'SF-release-wpi');
    rc = putSF(sChangeLog,      'SF-ChangeLog');

    /*
     * Forwards files from source forge to os2.ftp.org
     */
    if (sLoc = '' | sLoc = 'os2') then
    do
        rc = deletefunction(     '/daily/'sDeleteDbg,      '/daily/'sDeleteRel,       'os2-delete', 'www.os2.org');
/*      rc = deletefunction('/daily/'sDeleteGlideDbg, '/daily/'sDeleteGlideRel, 'os2-delete-glide', 'www.os2.org'); */
        rc = deletefunction(  '/daily/'sDeleteDbgWPI,   '/daily/'sDeleteRelWPI,   'os2-delete-wpi', 'www.os2.org');
        rc = forwardSF(sFileDbg,              'os2-debug', '/daily', 'www.os2.org');
/*      rc = forwardSF(sFileGlideDbg,   'os2-glide-debug', '/daily', 'www.os2.org'); */
        rc = forwardSF(sFileRel,            'os2-release', '/daily', 'www.os2.org');
/*      rc = forwardSF(sFileGlideRel, 'os2-glide-release', '/daily', 'www.os2.org'); */
        rc = forwardSF(sFileDbgWPI,       'os2-debug-wpi', '/daily', 'www.os2.org');
        rc = forwardSF(sFileRelWPI,     'os2-release-wpi', '/daily', 'www.os2.org');
        rc = forwardSF(sChangeLog,        'os2-ChangeLog', '/daily', 'www.os2.org');

    end

    if (sLoc = '' | sLoc = 'netlabs') then
    do
        /*                 (                    sDeleteFile1,             sDeleteFile2,             sLockFile,             sSite); */
        rc = deletefunction(             '/daily/'sDeleteDbg,      '/daily/'sDeleteRel,      'netlabs-delete', 'ftp.netlabs.org');
/*      rc = deletefunction('/odinftp/daily/'sDeleteGlideDbg, '/daily/'sDeleteGlideRel,'netlabs-delete-glide', 'ftp.netlabs.org'); */
        rc = deletefunction(          '/daily/'sDeleteDbgWPI,   '/daily/'sDeleteRelWPI,  'netlabs-delete-wpi', 'ftp.netlabs.org');
        if 1 then
        do
        /*              (sFile,      sFileRemote,              sLockFile,             sSite); */
        rc = putfunction(sFileDbg,      '/daily',        'netlabs-debug', 'ftp.netlabs.org');
/*      rc = putfunction(sFileGlideDbg, '/daily',  'netlabs-glide-debug', 'ftp.netlabs.org'); */
        rc = putfunction(sFileRel,      '/daily',      'netlabs-release', 'ftp.netlabs.org');
/*      rc = putfunction(sFileGlideRel, '/daily','netlabs-glide-release', 'ftp.netlabs.org'); */
        rc = putfunction(sFileDbgWPI,   '/daily',    'netlabs-debug-wpi', 'ftp.netlabs.org');
        rc = putfunction(sFileRelWPI,   '/daily',  'netlabs-release-wpi', 'ftp.netlabs.org');
        rc = putfunction('ChangeLog',   '/daily',    'netlabs-ChangeLog', 'ftp.netlabs.org');
        end
        else
        do
        /*            (sFile,                  sLockFile,  sRemoteDir,             sSite); */
        rc = forwardSF(sFileDbg,         'netlabs-debug',    '/daily', 'ftp.netlabs.org');
/*      rc = forwardSF(sFileGlideDbg,    'netlabs-debug',    '/daily', 'ftp.netlabs.org'); */
        rc = forwardSF(sFileRel,       'netlabs-release',    '/daily', 'ftp.netlabs.org');
/*      rc = forwardSF(sFileGlideRel,  'netlabs-release',    '/daily', 'ftp.netlabs.org'); */
        rc = forwardSF(sFileDbgWPI,      'netlabs-debug',    '/daily', 'ftp.netlabs.org');
        rc = forwardSF(sFileRelWPI,    'netlabs-release',    '/daily', 'ftp.netlabs.org');
        rc = forwardSF('ChangeLog',  'netlabs-ChangeLog',    '/daily', 'ftp.netlabs.org');
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
putfunction: procedure
    parse arg sFile, sRemoteDir, sLockFile, sSite

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

        /* debug logging */
        if 0 then
        do
            say sFile
            say sRemoteDir;
            say sLockFile
            say sSite
            say sUser
            say sPasswd
        end

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
                say 'NCFTPPut failed with rc='rc'-' asErrors.rc;
            else
                say 'NCFTPPut failed with rc='rc;
        end
    end
    else
        rc = 0;

    return 0;


/*
 * Delete two files on a given ftp site.
 */
deletefunction: procedure
    parse arg sDeleteFile1, sDeleteFile2, sLockFile, sSite

    /*
     * On fridays we don't perform any deletetions.
     */
    if (DATE('B')//7 = 5) then
        return 0;

    if stream(sLockFile,'c','query exists') = '' then
    do
        say '--- deleting old files('sDeleteFile1','sDeleteFile2') at 'sSite' ---'

        /* get password */
        sPasswdString = GetPassword(sSite);
        if (sPasswdString = '') then
        do
            call failure rc, 'Can''t find userid/password for' sSite'.', -1;
            return -1;
        end
        parse var sPasswdString sUser':'sPasswd;

        /* debug logging */
        if 0 then
        do
            say sDeleteFile1
            say sDeleteFile2
            say sLockFile
            say sSite
            say sUser
            say sPasswd
        end

        /* start ftp'ing */
        rc = FtpSetUser(sSite, sUser, sPasswd);
        if rc = 1 then
        do
            rc1 = FtpDelete(sDeleteFile1)
            if (rc1 <> 0 & FTPERRNO = 'FTPCOMMAND') then /* happens when the file don't exists... too. */
                rc1 = 0;
            if (rc1 <> 0) then
            do
                rc = rc1;
                call failure rc, 'FtpDelete failed -' sSite , FTPERRNO;
            end

            rc2 = FtpDelete(sDeleteFile2)
            if (rc2 <> 0 & FTPERRNO = 'FTPCOMMAND') then /* happens when the file don't exists... too. */
                rc2 = 0;
            if (rc1 <> 0) then
            do
                rc = rc2;
                call failure rc, 'FtpDelete failed -' sSite , FTPERRNO;
            end
            if (rc1 = 0 & rc2 = 0) then
            do
                'echo ok >' sLockFile;
                rc = 0;
            end
            else
                say 'delete failed with rc1='rc1 'and rc2='rc2;
            rc1 = FtpLogoff();
        end
        else
        do
            call failure rc, 'Logon failed -' sSite, FTPERRNO;
            if rc = 0 then rc = -1;
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

