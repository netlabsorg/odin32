/* $Id: odin32ftp.cmd,v 1.2 2001-01-10 14:17:23 bird Exp $
 *
 * Old FTP routines using only RexxFTP.
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

do i = 1 to 7
    sFile   = 'odin32bin-'|| DATE(S);
    sDelete = 'odin32bin-'|| DateSub(DATE('S'), 7);
    sFileDbg   = sFile   || '-debug.zip';
    sDeleteDbg = sDelete || '-debug.zip';
    sFileRel   = sFile   || '-release.zip';
    sDeleteRel = sDelete || '-release.zip';

    if (sLoc = '' | sLoc = 'os2') then
    do
        /*              (sFile,                  sFileRemote,                     sDelete,         sLockFile,             sSite); */
        rc = putfunction(sFileDbg,         '/daily/'sFileDbg,         '/daily/'sDeleteDbg,       'os2-debug',     'ftp.os2.org');
        rc = putfunction(sFileRel,         '/daily/'sFileRel,         '/daily/'sDeleteRel,     'os2-release',     'ftp.os2.org');
        rc = putfunction('ChangeLog',     '/daily/Changelog',         '/daily/'sDeleteRel,      'os2-Change',     'ftp.os2.org');
    end

    if (sLoc = '' | sLoc = 'netlabs') then
    do
        /*              (sFile,                     sFileRemote,                     sDelete,          sLockFile,             sSite); */
        rc = putfunction(sFileDbg,    '/odinftp/daily/'sFileDbg, '/odinftp/daily/'sDeleteDbg,    'netlabs-debug', 'ftp.netlabs.org');
        rc = putfunction(sFileRel,    '/odinftp/daily/'sFileRel, '/odinftp/daily/'sDeleteRel,  'netlabs-release', 'ftp.netlabs.org');
        rc = putfunction('ChangeLog','/odinftp/daily/ChangeLog', '/odinftp/daily/'sDeleteRel,'netlabs-changelog', 'ftp.netlabs.org');
    end
end

exit(0);

putfunction: procedure
    parse arg sFile, sFileRemote, sDelete, sLockFile, sSite
    if stream(sLockFile,'c','query exists') = '' then
    do
        say '---'
        say sFile
        say sFileRemote;
        say sDelete
        say sLockFile
        say sSite

        /* get password */
        sPasswdString = GetPassword(sSite);
        if (sPasswdString = '') then
        do
            call failure rc, 'Can''t find userid/password for' sSite'.', -1;
            return -1;
        end
        parse var sPasswdString sUser':'sPasswd;
        /*
        say sUser
        say sPasswd */

        say sSite':' sFile '-> ' sFileRemote;
        rc = FtpSetUser(sSite, sUser, sPasswd);
        if (rc = 1) then
        do
            rc = 0;
            if (DATE('B')//7 <> 5) then
                rc = FtpDelete(sFileRemote);
            rcPut = FtpPut(sFile, sFileRemote, 'Binary');
            if (rcPut <> 0) then
                rc = FtpDelete(sFileRemote);
            rc = FtpDelete(sDelete)
            say sDelete ' - ' rc
            rc = FtpLogoff();
            if (rcPut = 0) then
                'echo ok > ' || sLockFile;
            else
                call failure rc, 'FtpPut failed -' sSite , FTPERRNO;
            rc = rcPut;
        end
        else
        do
            call failure rc, 'Logon failed -' sSite, FTPERRNO;
            if rc = 0 then rc = -1;
        end
    end
    else
        rc = 0;

    return 0;





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


