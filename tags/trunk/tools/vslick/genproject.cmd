/* $Id: genproject.cmd,v 1.1 2000-01-23 03:33:08 bird Exp $
 *
 * This script generates a Visual Slick project of the source and include
 * files found in the directory tree starting at the current directory.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysLoadFuncs;

    parse arg sArg

    sIncludes.0 = 0;
    sProjFile = '';

    sArgs.0 = 0;
    i = 0;
    do while sArg \= ''
        i = i + 1;
        ipos = pos(' ', sArg);
        if ipos == 0 then do
            sArgs.i = substr(sArg, 1);
            sArg = '';
        end
        else do
            sArgs.i = substr(sArg, 1, ipos);
            /* next */
            sArg = substr(sArg, ipos);
            do while substr(sArg, 1, 1) == ' ' | substr(sArg, 1, 1) == '\t'
                sArg = substr(sArg, 2);
            end
        end
    end
    sArgs.0 = i;

    /* check if no parameters */
    if i = 0 then
    do
        say 'error: no arguments!';
        call syntax;
    end

    /* parse arguments */
    do i = 1 to sArgs.0
        sArg = sArgs.i;
        if substr(sArg, 1, 1) == '-' | substr(sArg, 1, 1) == '/' then do
            /* option */
            sArg = substr(sArg, 2);
            chArg = substr(sArg, 1, 1);
            select
                when chArg = 'I' | chArg = 'i' then do

                end

                when chArg = 'h' | chArg = '?' | sArg = '-help' then do
                    call syntax;
                end

                otherwise  do
                    say 'illegal option: 'sArg
                    call syntax;
                end
            end
        end
        else do
            if sProjFile = '' then
                sProjFile = sArg;
            else do
                say 'error! multiple project names!'
                call syntax;
            end
        end
    end /* do */

    say 'Will now generate :' sProjFile
    sInclude='';
    do i = 1 to sIncludes.0
        if i = 1 then
            say 'Includes specified:' sIncludes.i
        else
            say '                   ' sIncludes.i
        sInclude=sInclude||sIncludes.i||';'
    end

    /* delete old target files */
    call SysFileDelete sProjFile;
    if lastpos('.', sProjFile) > 0 then /* tag file */
        call SysFileDelete substr(sProjFile, 1, lastpos('.', sProjFile))'vtg'

    /* open target file */
    if (stream(sProjFile, 'c', 'open write' ) <> '') then do
        call lineout sProjFile, '[COMPILER]'
        call lineout sProjFile, 'MACRO=odin32_maketagfile();\n'
        call lineout sProjFile, 'FILTEREXPANSION=1 1 0 0 1'
        call lineout sProjFile, 'compile=concur|capture|clear|:Compile:&Compile,nmake object\%n.obj'
        call lineout sProjFile, 'make=concur|capture|clear|:Build:&Build,nmake'
        call lineout sProjFile, 'rebuild=concur|capture|clear|:Rebuild:&Rebuild,nmake /A'
        call lineout sProjFile, 'debug=:Debug:&Debug,'
        call lineout sProjFile, 'execute=:Execute:E&xecute,'
        call lineout sProjFile, 'user1=hide|:User 1:User 1,'
        call lineout sProjFile, 'user2=hide|:User 2:User 2,'
        call lineout sProjFile, 'usertool_resource_editor=hide|:Resource Editor:Resource Editor,dlgedit'
        call lineout sProjFile, 'workingdir='directory()
        /* TODO */
        call lineout sProjFile, 'includedirs='||sInclude||'%(INCLUDE)'
        call lineout sProjFile, 'tagfiles='
        call lineout sProjFile, 'reffile='

        call lineout sProjFile, '[FILES]'
        call processDirTree sProjFile, directory();
        call lineout sProjFile, '[ASSOCIATION]'
        call lineout sProjFile, '[STATE]'
        call lineout sProjFile, 'FILEHIST: 0'
        call lineout sProjFile, 'PRINTER: 2'

        call stream sIncFile, 'c', 'close';
    end
    else do
        say 'oops, failed to open outputfile,' sProjFile;
        exit 1;
    end

    exit (0);






/*********************/
/* procedure section */
/*********************/

syntax: procedure
    say 'syntax: genproject.cmd  <project filename> [-I<include directories>]'
    say ''
    say 'Copyright (c) 1999 knut st. osmundsen'
    exit (1);

/* processes an directory tree */
processDirTree: procedure
    parse arg sProjFile, sDirectory

    rc = SysFileTree(sDirectory'\*', sFiles, 'FO');
    if rc == 0  then do
        do i = 1 to sFiles.0
            if filterFile(sFiles.i) then
                call lineout sProjFile, sFiles.i
        end
    end

    rc = SysFileTree(sDirectory'\*', sDirs, 'DO');
    if rc == 0  then do
        do i = 1 to sDirs.0
            if filterDirectory(sDirs.i) then
                call processDirTree sProjFile, sDirs.i
        end
    end


    return;


/* returns boolean, TRUE if include; false if exclude */
filterFile: procedure
    parse arg sFile

    if lastpos('\', sFile) < lastpos('.', sFile) then do
        sIncludeExt = 'c;cpp;h;hpp;inc;asm;rc;mak;cmd;mk;def;txt;'
        sExt = substr(sFile, lastpos('.', sFile)+1);

        /* look for sExt in sIncludeExt */
        do while pos(';', sIncludeExt) > 0
            ipos = pos(';', sIncludeExt)
            if sExt == substr(sIncludeExt, 1, ipos-1) then
                return 1;
            sIncludeExt = substr(sIncludeExt, ipos+1);
        end

    end
    else
        return 1; /* all file without extension is included. */

    return 0;

/* returns boolean, TRUE if include; false if exclude */
filterDirectory: procedure
    parse arg sDir

    i = lastpos('\', sDir)
    if i == 0 then
        i = length(sDir);
    else
        i = i + 1;
    sDir = substr(sDir, i);

    sExcludeDir = 'CVS;';

    /* look for sExt in sIncludeExt */
    do while pos(';', sExcludeDir) > 0
        ipos = pos(';', sExcludeDir)
        if sDir == substr(sExcludeDir, 1, ipos-1) then
            return 0;
        sExcludeDir = substr(sExcludeDir, ipos+1);
    end

    return 1;

