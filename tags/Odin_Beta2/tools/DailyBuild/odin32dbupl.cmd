/* $Id: odin32dbupl.cmd,v 1.1 2002-02-08 10:56:02 bird Exp $
 *
 * Updates the Odin32 API database.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    /* load rexxutils functions */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysloadFuncs;

    sPwdDummy = GetPassword('odin32dbupdate');
    parse var sPwdDummy sPasswd':'sDummy
    'wget -O /dev/con http://www.netlabs.org/odin/odin32dbtest/Odin32DBUpdate.phtml?sPasswd='||sPasswd
    if (rc <> 0) then call failure rc, 'wget failed';


    exit(0);

failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

failure2: procedure
parse arg rc, sText;
    say 'rc='rc sText
    return 0;


/*
 * Get the MySql data directory.
 */
getMySqlDataDir: procedure

    /* Get mysql variables */
    '@mysqladmin variables | rxqueue /lifo'

    /* Get datadir */
    sDataDir = '';
    do queued()
        pull s;
        if (pos(' DATADIR ', s) > 0) then
        do
            sDataDir = strip( substr( s, pos('|',s,3) + 1 ) );
            sDataDir = strip( substr(sDataDir, 1, length(sDataDir)-1) );
            leave;
        end
    end

    /* Drain queue */
    do queued()
        pull s
    end
    drop s;

    /* If failure set default directory. */
    if (sDataDir = '') then
        sDataDir = 'd:\knut\Apps\MySql\data\';
    return sDataDir;

/*
 *
 *  From odin32ftp2.cmd
 *  From odin32ftp2.cmd
 *  From odin32ftp2.cmd
 *  From odin32ftp2.cmd
 *  From odin32ftp2.cmd
 *  From odin32ftp2.cmd
 *
 */


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


/*
 * REXX FTP put function.
 */
putRexxFtp: procedure
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

        /* log on the ftp site */
        rc = FtpSetUser(sSite, sUser, sPasswd);
        if (rc = 1) then
        do
            /* put file, delete it if we fail */
            say sSite  sUser  sPasswd  sFile  sRemoteDir'/'filespec('name', sFile)  'Binary'
            rcPut = FtpPut(sFile, sRemoteDir'/'filespec('name',sFile), 'Binary');
            /*
            if (rcPut <> 0) then
                rc = FtpDelete(sRemoteDir'/'filespec('name', sFile));
            */

            /* Logoff and make lock file. */
            rc = FtpLogoff();
            if (rcPut = 0) then
                'echo ok  ' || sLockFile; /* changed - no lockfile */
            else
                call failure2 rc, 'FtpPut failed -' sSite , FTPERRNO;
            rc = rcPut;
        end
        else
        do
            call failure2 rc, 'Logon failed -' sSite, FTPERRNO;
            if rc = 0 then rc = -1;
        end
    end
    else
        rc = 0;
    return 0;


