/* $Id: DoWithDirs.cmd,v 1.15 2002-08-29 11:49:37 bird Exp $
 *
 * Syntax: dowithdirs.cmd [-e<list of excludes>] [-c] [-i] [-l] [-r] <cmd with args...>
 *    -e      Exclude directories.
 *    <list of excludes> is a INCLUDE-path styled list of directories.
 *    -c      CD into the directory and execute the command.
 *            Default action is to pass the directory name as last argument.
 *    -i      Ignore command failure (rc=0)
 *    -r      Process diretories in reverse order.
 *    -l<tag> Lock directories for other dowithdirs.cmd processes. (-c required!)
 *            Processing stops (rc=0) on the first locked directory.
 *            <tag> is a name of the lock.
 *    -s      Skip locked directories in stead of stopping.
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 */
signal on novalue name NoValueHandler
Address CMD '@echo off'

if (RxFuncQuery('SysLoadFuncs') = 1) then
do
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
    call SysLoadFuncs
end


/*
 * Color config.
 */
if ((getenv('BUILD_NOCOLORS') = '') & (getenv('SLKRUNS') = '')) then
do
    sClrMak = '[35;1m'
    sClrErr = '[31;1m'
    sClrRst = '[0m'
end
else
do
    sClrMak = ''
    sClrErr = ''
    sClrRst = ''
end


/*
 * Build Pass
 */
sPass = getenv('_BUILD_PASS');
if (sPass <> '') then
    sPass = 'Pass '||sPass||' - '


/* init options */
fIgnoreFailure = 0;
asIgnore.0 = 0;
fCD = 0;
fLocking = 0;
fDontStop = 0;
fReverse = 0;
fExitOnLock = 1;
sLockTag = '';

/* parse arguments */
parse arg sArg.1 sArg.2 sArg.3 sArg.4 sArg.5 sArg.6 sArg.7 sArg.8 sArg.9
sArg.0 = 9;
do i = 1 to sArg.0
    if (sArg.i <> '') then
    do
        if (substr(sArg.i, 1, 1) = '-') then
        do
            ch = translate(substr(sArg.i, 2, 1));
            select
                when ch = 'E' then
                do
                    sLeft = substr(sArg.i, 3);
                    do while (sLeft <> '' & sLeft <> ';')
                        j = asIgnore.0 + 1;
                        iPos = pos(';', sLeft);
                        if (iPos < 1) then do
                            asIgnore.j = sLeft;
                            sLeft = '';
                            asIgnore.0 = j;
                        end
                        else do
                            asIgnore.j = substr(sLeft, 1, iPos - 1);
                            sLeft = substr(sLeft, iPos + 1);
                            asIgnore.0 = j;
                        end
                    end
                    /*
                    do j = 1 to asIgnore.0
                        say 'dbg:' asIgnore.j;
                    end
                    */
                end

                when ch = 'C' then
                do
                    fCD = 1;
                end

                when ch = 'I' then
                do
                    fIgnoreFailure = 1;
                end

                when ch = 'R' then
                do
                    fReverse = 1;
                end

                when ch = 'L' then
                do
                    fLocking = 1;
                    sLockTag = substr(sArg.i, 3);
                end

                when ch = 'S' then
                do
                    fExitOnLock = 0;
                end

                otherwise
                    say 'unknown argument:' sArg.i;
                    call syntax;
            end
        end
        else
        do  /* the rest of the args is part of the cmd */
            sCmds = '';
            do j = i to sArg.0;
                if (sArg.j <> '') then
                sCmds = sCmds || ' ' || sArg.j;
            end
            i = sArg.0;
        end
    end
    else
    do
        say 'missing cmd.';
        call syntax;
    end
end

/* sanity check */
if (fLocking & \fCD) then
do
    say '-l (Locking) requires -cd to be specified!';
    call syntax;
end

/* process directories */
rc = SysFileTree('*.', 'asDirs', 'DO');
if rc <> 0 then do
    say sClrErr||'SysFileTree failed rc='rc||sClrRst;
    exit(rc);
end

sArgDirs = ' ';
do ii = 1 to asDirs.0
    /* calculate index */
    if (fReverse) then
        i = asDirs.0 - ii + 1;
    else
        i = ii;

    /* ignore the directory? */
    fFound = 0;
    do j = 1 to asIgnore.0
        if translate(asIgnore.j) = translate(filespec('name', asDirs.i)) then
        do
            fFound = 1;
            leave;
        end
    end

    if \fFound then
    do
        /* switch execution type. */
        if (fCD) then
        do
            /* exectute the command in the directory */
            say sClrMak||'['||sPass||'Entering Directory: '||asDirs.i']'||sClrRst;
            /* save old dir and enter the new dir. */
            sOldDir = directory();
            call directory asDirs.i;

            /* Lock the directory? */
            fOK = 1;
            if (fLocking) then
                if (\lockdir(sLockTag)) then
                do
                    if (fExitOnLock) then
                    do
                        /* restore old directory and return sucessfully */
                        call directory sOldDir;
                        say sClrMak||'['||sPass||'!Lock found, stops processing.'||']'||sClrRst;
                        exit(0);
                    end
                    say sClrMak||'['||sPass||'!Skipping '||asDirs.i||' - Directory was locked.'||']'||sClrRst;
                    fOK = 0;
                end

            /* continue only if locking was successful. */
            if (fOK) then
            do
                /* execute command */
                'call' sCmds;
                ret = rc;

                /* unlock directory */
                if (fLocking & fOk) then
                    call unlockdir sLockTag;

                /* check for return? */
                if (ret <> 0) then
                do
                    /* complain and fail if errors aren't ignored. */
                    if (\fIgnoreFailure) then
                    do
                        say sClrErr||'['||sPass||'rc = 'ret' '||asDirs.i||']'||sClrErr;
                        exit(rc);
                    end
                    say sClrMak||'['||sPass||'rc = 'ret' '||asDirs.i||']'||sClrRst;
                end
            end

            /* restore old directory */
            say sClrMak||'['||sPass||'Leaving  Directory:' directory()||']'||sClrRst;
            call directory sOldDir;
        end
        else
        do
            /* execute the command with the directory as the last parameter */
            'call' sCmds filespec('name', asDirs.i);
            if (rc <> 0) then
            do
                say sClrErr||'['||sPass||'rc = '||rc||']'||sClrRst;
                if (\fIgnoreFailure) then
                    exit(rc);
            end
        end
    end /* loop */
end

exit(rc);


syntax:
    say 'Syntax: dowithdirs.cmd [-e<list of excludes>] [-c] [-i] [-l<tag>] [-r] <cmd with args...>';
    say '   -e      Exclude directories.';
    say '   <list of excludes> is a INCLUDE-path styled list of directories.';
    say '   -c      CD into the directory and execute the command.';
    say '           Default action is to pass the directory name as last argument.';
    say '   -i      Ignore command failure (rc=0)';
    say '   -r      Process diretories in reverse order.';
    say '   -l<tag> Lock directories for other dowithdirs.cmd processes. (-c required!)';
    say '           Processing stops (rc=0) on the first locked directory.';
    say '           <tag> is a name of the lock.';
    say '   -s      Skip locked directories in stead of stopping.'
    exit(-1)


/*
 * Locks the directory by creating a .dirlocked file in the directory.
 * Returns  1 on success
 *          0 on error
 */
lockdir: procedure
    parse arg sTag
    rc = stream('.dirlocked' || sTag, 'c', 'open write');
    return substr(rc, 1, 5) = 'READY';


/*
 * Unlocks thedirectory by deleting the .dirlocked file.
 */
unlockdir: procedure
    parse arg sTag
    rc = stream('.dirlocked' || sTag, 'c', 'close');
    call SysFileDelete '.dirlocked' || sTag;
    return 0;


/**
 * Get environment variable value.
 * @returns Environment variable value if set.
 *          '' if not set.
 * @param   sVar    Variable name.
 */
getenv: procedure
parse arg sVar
return value(sVar,,'OS2ENVIRONMENT');

/**
 * No value handler
 */
NoValueHandler:
    say 'NoValueHandler: line 'SIGL;
return 0;

