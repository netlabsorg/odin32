/* $Id: odin32ftp2.cmd,v 1.4 2000-07-19 12:08:50 bird Exp $
 *
 * Uploads the relase and debug builds to the FTP sites.
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

do i = 1 to 5 /* (Retries 5 times) */
    sFile           = 'odin32bin-'|| DATE(S);
    sDelete         = 'odin32bin-'|| DateSub7(DATE('S'));
    sFileDbg        = sFile   || '-debug.zip';
    sFileDbgWPI     = sFile   || '-debug.wpi';
    sDeleteDbg      = sDelete || '-debug.zip';
    sDeleteDbgWPI   = sDelete || '-debug.wpi';
    sFileRel        = sFile   || '-release.zip';
    sFileRelWPI     = sFile   || '-release.wpi';
    sDeleteRel      = sDelete || '-release.zip';
    sDeleteRelWPI   = sDelete || '-release.wpi';
    sChangeLog      = 'ChangeLog';

    /*
     * Cleanup the daily directory at source forge.
     */
    rc = cleanSF();

    /*
     * Put file to SourceForge.
     */
    rc = putSF(sFileDbg,    'SF-debug');
    rc = putSF(sFileDbgWPI, 'SF-debug-wpi');
    rc = putSF(sFileRel,    'SF-release');
    rc = putSF(sFileRelWPI, 'SF-release-wpi');
    rc = putSF(sChangeLog,  'SF-ChangeLog');

    /*
     * Forwards files from source forge to os2.ftp.org
     */
    if (sLoc = '' | sLoc = 'os2') then
    do
        rc = deletefunction(   '/daily/'sDeleteDbg,    '/daily/'sDeleteRel,     'os2-delete', 'www.os2.org', 'user', 'xxxxxxxx');
        rc = deletefunction('/daily/'sDeleteDbgWPI, '/daily/'sDeleteRelWPI, 'os2-delete-wpi', 'www.os2.org', 'user', 'xxxxxxxx');
        rc = forwardSF(sFileDbg,          'os2-debug', '/daily', 'www.os2.org', 'user', 'xxxxxxxx');
        rc = forwardSF(sFileRel,        'os2-release', '/daily', 'www.os2.org', 'user', 'xxxxxxxx');
        rc = forwardSF(sFileDbgWPI,   'os2-debug-wpi', '/daily', 'www.os2.org', 'user', 'xxxxxxxx');
        rc = forwardSF(sFileRelWPI, 'os2-release-wpi', '/daily', 'www.os2.org', 'user', 'xxxxxxxx');
        rc = forwardSF(sChangeLog,    'os2-ChangeLog', '/daily', 'www.os2.org', 'user', 'xxxxxxxx');

    end

    if (0 /*sLoc = '' | sLoc = 'netlabs'*/) then
    do
        /*                 (                  sDeleteFile1,                   sDeleteFile2,            sLockFile,             sSite,  sUser,    sPasswd); */
        rc = deletefunction(   '/odinftp/daily/'sDeleteDbg,    '/odinftp/daily/'sDeleteRel,     'netlabs-delete', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
        rc = deletefunction('/odinftp/daily/'sDeleteDbgWPI, '/odinftp/daily/'sDeleteRelWPI, 'netlabs-delete-wpi', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
        /*              (sFile,            sFileRemote,           sLockFile,             sSite,  sUser,    sPasswd); */
        rc = putfunction(sFileDbg,    '/odinftp/daily',     'netlabs-debug', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
        rc = putfunction(sFileRel,    '/odinftp/daily',   'netlabs-release', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
        rc = putfunction(sFileDbgWPI, '/odinftp/daily',     'netlabs-debug', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
        rc = putfunction(sFileRelWPI, '/odinftp/daily',   'netlabs-release', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
        rc = putfunction('ChangeLog', '/odinftp/daily', 'netlabs-ChangeLog', 'ftp.netlabs.org', 'user', 'xxxxxxxx');
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
    parse arg sFile, sLockFile, sRemoteDir, sSite, sUser, sPasswd

    sSFDir = '/home/groups/ftp/pub/kTaskMgr/daily/';

    if (stream(sLockFile,'c','query exists') = '') then
    do
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
    parse arg sFile, sRemoteDir, sLockFile, sSite, sUser, sPasswd
    if stream(sLockFile,'c','query exists') = '' then
    do
        say '--- put' sFile '->' sRemoteDir'/'sFile '---';
        if 0 then
        do
            say sFile
            say sRemoteDir;
            say sLockFile
            say sSite
            say sUser
            say sPasswd
        end

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
    parse arg sDeleteFile1, sDeleteFile2, sLockFile, sSite, sUser, sPasswd
    if stream(sLockFile,'c','query exists') = '' then
    do
        say '--- deleting old files('sDeleteFile1','sDeleteFile2') at 'sSite' ---'
        if 0 then
        do
            say sDeleteFile1
            say sDeleteFile2
            say sLockFile
            say sSite
            say sUser
            say sPasswd
        end

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



/**
 * Finds date seven days ago.
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

