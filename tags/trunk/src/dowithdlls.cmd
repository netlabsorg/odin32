/* $Id: dowithdlls.cmd,v 1.1 1999-12-06 23:51:41 bird Exp $ */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
    call SysLoadFuncs

    parse arg sArgs

    asIgnore.0 = 7;
    asIgnore.1 = 'CVS';
    asIgnore.2 = 'odin32api';
    asIgnore.3 = 'odincrt';
    asIgnore.4 = 'pe2lx';
    asIgnore.5 = 'peldr';
    asIgnore.6 = 'spy';
    asIgnore.7 = 'win32k';

    rc = SysFileTree('*.', 'asDirs', 'DO');
    if rc <> 0 then do
        say 'SysFileTree failed rc='rc;
        exit(rc);
    end

    sArgDirs = ' ';
    do i = 1 to asDirs.0
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
            sArgs filespec('name',asDirs.i)
            say 'rc='rc;
        end
    end

    exit(rc);
