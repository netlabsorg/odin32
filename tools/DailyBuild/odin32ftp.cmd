/* $Id: odin32ftp.cmd,v 1.1 2000-04-27 11:32:25 bird Exp $
 *
 * Old FTP routines using only RexxFTP.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
    sDelete = 'odin32bin-'|| DateSub7(DATE('S'));
    sFileDbg   = sFile   || '-debug.zip';
    sDeleteDbg = sDelete || '-debug.zip';
    sFileRel   = sFile   || '-release.zip';
    sDeleteRel = sDelete || '-release.zip';

    if (sLoc = '' | sLoc = 'os2') then
    do
        /*              (sFile,                  sFileRemote,                     sDelete,         sLockFile,             sSite,  sUser,    sPasswd); */
        rc = putfunction(sFileDbg,         '/daily/'sFileDbg,         '/daily/'sDeleteDbg,       'os2-debug',     'ftp.os2.org', 'user', 'xxxxxxxx');
        rc = putfunction(sFileRel,         '/daily/'sFileRel,         '/daily/'sDeleteRel,     'os2-release',     'ftp.os2.org', 'user', 'xxxxxxxx');
        rc = putfunction('ChangeLog',     '/daily/Changelog',         '/daily/'sDeleteRel,      'os2-Change',     'ftp.os2.org', 'user', 'xxxxxxxx');
    end

    if (sLoc = '' | sLoc = 'netlabs') then
    do
        /*              (sFile,                     sFileRemote,                     sDelete,          sLockFile,             sSite,  sUser,    sPasswd); */
        rc = putfunction(sFileDbg,    '/odinftp/daily/'sFileDbg, '/odinftp/daily/'sDeleteDbg,    'netlabs-debug', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
        rc = putfunction(sFileRel,    '/odinftp/daily/'sFileRel, '/odinftp/daily/'sDeleteRel,  'netlabs-release', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
        rc = putfunction('ChangeLog','/odinftp/daily/ChangeLog', '/odinftp/daily/'sDeleteRel,'netlabs-changelog', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
    end
end

exit(0);

putfunction: procedure
    parse arg sFile, sFileRemote, sDelete, sLockFile, sSite, sUser, sPasswd
    if stream(sLockFile,'c','query exists') = '' then
    do
        say '---'
        say sFile
        say sFileRemote;
        say sDelete
        say sLockFile
        say sSite
        /*
        say sUser
        say sPasswd */

        say sSite':' sFile '-> ' sFileRemote;
        rc = FtpSetUser(sSite, sUser, sPasswd);
        if rc = 1 then
        do
            rc = FtpDelete(sFileRemote);
            rcPut = FtpPut(sFile, sFileRemote, 'Binary');
            if (rcPut <> 0) then
                rc = FtpDelete(sFileRemote);
            rc = FtpDelete(sDelete)
            say sDelete ' - ' rc
            rc = FtpLogoff();
            if rcPut = 0 then
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

/* rexx */
    /* load rexxutils functions */

/**
 * @returns  yyyymmdd date
 * @param    iDate  Date on the yyyymmdd format.
 */
DateSub7: procedure
parse arg iDate

    iDayInMonth = substr(iDate, 7, 2);
    iMonth = substr(iDate, 5, 2);
    iYear = substr(iDate, 1, 4);

    if (iDayInMonth > 7) then
        return iDate - 7;
    else
    do
        if iMonth > 1 then
            iPrvMonth = iMonth - 1;
        else
            iPrvMonth = 12;

        select
            when iPrvMonth = 4 | iPrvMonth = 6 | iPrvMonth = 9 | iPrvMonth = 11 then
                iDaysInPrevMonth = 30;

            when iPrvMonth = 2 then
            do
                if ((iYear // 4) = 0) & (((iYear // 400) <> 0) | ((iYear // 2000) = 0)) then /*?*/
                    iDaysInPrevMonth = 29;
                else
                    iDaysInPrevMonth = 28;
            end

            otherwise
                iDaysInPrevMonth = 31;
        end /* select */

        say iDaysInPrevMonth
        if iMonth > 1 then
            return iDate - (100 - iDaysInPrevMonth) - 7;
        else
            return iDate - 8869 - 7;
        end
    end

    return 00010101;
