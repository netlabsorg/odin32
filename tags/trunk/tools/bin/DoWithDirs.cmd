/* $Id: DoWithDirs.cmd,v 1.1 2000-02-09 23:51:34 bird Exp $
 *
 *  Syntax: dowithdirs.cmd [-e<list of excludes>] [-cd] <cmd with args...>
 *      <list of excludes> is a INCLUDE-path styled list of directories.
 *
 */

    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
    call SysLoadFuncs

    /* init options */
    fIgnoreFailure = 0;
    asIgnore.0 = 0;
    fCD = 0;

    /* parse arguments */
    parse arg sArg.1 sArg.2 sArg.3 sArg.4
    sArg.0 = 4;
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

    /* process directories */
    rc = SysFileTree('*.', 'asDirs', 'DO');
    if rc <> 0 then do
        say 'SysFileTree failed rc='rc;
        exit(rc);
    end

    sArgDirs = ' ';
    do i = 1 to asDirs.0
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
        do  /* execute the command */
            if (fCD) then
            do
                say '# entering directory:' asDirs.i;
                sOldDir = directory();
                call directory asDirs.i;
                sCmds;
            end
            else
                sCmds filespec('name', asDirs.i);

            if (rc <> 0) then
            do
                say 'rc='rc;
                if (\fIgnoreFailure) then
                    exit(rc);
            end

            if (fCD) then
                call directory sOldDir;
        end
    end

    exit(rc);


syntax:
    say 'Syntax: dowithdirs.cmd [-e<list of excludes>] <cmd with args...>';
    say '   <list of excludes> is a INCLUDE-path styled list of directories.';
