/* $Id: APIImport.cmd,v 1.1 2000-02-11 14:21:19 bird Exp $
 *
 * Helper script which invokes APIImport.exe with the correct .def file.
 *
 * Copyright (c) 2000 knut st. osmundsen
 *
 */

    call RxFuncAdd 'SysFileDelete', 'RexxUtil', 'SysFileDelete';
    call RxFuncAdd 'SysFileTree', 'RexxUtil', 'SysFileTree';

    sDllName = filespec('name', directory());
    call MakeTempDeffile sDllName, 'APIImport.def';

    parse source sD1 sD2 sSrc;

    sSrc = filespec('drive', sSrc) || filespec('path', sSrc);
    sSrc||'APIImport.exe APIImport.def';

    exit(rc);


MakeTempDeffile: procedure;
    parse arg sDllName, sTmpName

    call SysFileDelete sTmpName;
    sOrgDef = sDllName||'.def';
    rc = stream(sOrgDef, 'c', 'open read');
    if (pos('READY', rc) <> 1) then
    do
        rc = SysFileTree('*.def', 'asFiles', 'FO');
        if rc == 0 then
        do
            do i = 1 to asFiles.0
                if translate(substr(asFiles.i, length(asFiles.i) - 7, 7)) <> 'EXP.DEF' then
                do
                    sOrgDef = asFiles.i;
                    rc = stream(sOrgDef, 'c', 'open read');
                    if (pos('READY', rc) <> 1) then
                        say 'APIImport.cmd: failiure (2) rc='||rc;
                    else
                        i = asFiles.0;
                end
            end
        end
        else
            say 'APIImport.cmd: failiure (1) rc='||rc;
    end
    rc = stream(sTmpName, 'c', 'open write');

    do while (lines(sOrgDef) > 0)
        sLine = linein(sOrgDef);
        if (pos('LIBRARY ',  translate(strip(sLine))) = 1 ) then
            call lineout sTmpName, 'LIBRARY '||sDllName;
        else
            call lineout sTmpName, sLine;
    end

    rc = stream(sOrgDef, 'c', 'close');
    rc = stream(sTmpName, 'c', 'close');
    return 0;
