/* $Id: DoWithDirs.cmd,v 1.4 2000-03-11 17:11:30 bird Exp $
 *
 * Syntax: dowithdirs.cmd [-e<list of excludes>] [-c] [-i] [-l] [-r] <cmd with args...>
 *    -e      Exclude directories.
 *    <list of excludes> is a INCLUDE-path styled list of directories.
 *    -c      CD into the directory and execute the command.
 *            Default action is to pass the directory name as last argument.
 *    -i      Ignore command failure (rc=0)
 *    -r      Process diretories in reverse order.
 *    -l      Lock directories for other dowithdirs.cmd processes. (-c required!)
 */

    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
    call SysLoadFuncs

    /* init options */
    fIgnoreFailure = 0;
    asIgnore.0 = 0;
    fCD = 0;
    fLocking = 0;
    fReverse = 0;

    /* parse arguments */
    parse arg sArg.1 sArg.2 sArg.3 sArg.4 sArg.5 sArg.6 sArg.7
    sArg.0 = 7;
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
        say 'SysFileTree failed rc='rc;
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
                say '# entering directory:' asDirs.i;
                /* save old dir and enter the new dir. */
                sOldDir = directory();
                call directory asDirs.i;

                /* Lock the directory? */
                fOK = 1;
                if (fLocking) then
                    if (\lockdir()) then
                    do
                        say '# - warning: Skipping '|| asDirs.i || ' - directory was locked.';
                        fOK = 0;
                    end

                /* continue only if locking was successful. */
                if (fOK) then
                do
                    /* execute command */
                    sCmds;
                    ret = rc;

                    /* unlock directory */
                    if (fLocking & fOk) then
                        call unlockdir;

                    /* check for return? */
                    if (ret <> 0) then
                    do
                        /* complain and fail if errors aren't ignored. */
                        say '# - rc = 'ret;
                        if (\fIgnoreFailure) then
                            exit(rc);
                    end
                end

                /* restore old directory */
                call directory sOldDir;
            end
            else
            do
                /* execute the command with the directory as the last parameter */
                sCmds filespec('name', asDirs.i);
                if (rc <> 0) then
                do
                    say '# - rc = ' || rc;
                    if (\fIgnoreFailure) then
                        exit(rc);
                end
            end
        end /* loop */
    end

    exit(rc);


syntax:
    say 'Syntax: dowithdirs.cmd [-e<list of excludes>] [-c] [-i] [-l] [-r] <cmd with args...>';
    say '   -e      Exclude directories.';
    say '   <list of excludes> is a INCLUDE-path styled list of directories.';
    say '   -c      CD into the directory and execute the command.';
    say '           Default action is to pass the directory name as last argument.';
    say '   -i      Ignore command failure (rc=0)';
    say '   -r      Process diretories in reverse order.';
    say '   -l      Lock directories for other dowithdirs.cmd processes. (-c required!)';
    exit(-1)


/*
 * Locks the directory by creating a .dirlocked file in the directory.
 * Returns  1 on success
 *          0 on error
 */
lockdir: procedure
    rc = stream('.dirlocked', 'c', 'open write');
    return substr(rc, 1, 5) = 'READY';


/*
 * Unlocks thedirectory by deleting the .dirlocked file.
 */
unlockdir: procedure
    rc = stream('.dirlocked', 'c', 'close');
    call SysFileDelete '.dirlocked';
    return 0;

