/* $Id: configure.cmd,v 1.18 2001-04-14 07:22:12 bird Exp $
 *
 * Configuration script.
 * Generates makefile.inc and an empty .depend file.
 *
 * Copyright (c) 1999-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysLoadFuncs;

    /* Argument defaults */
    fInteractive    = 1;
    fWin32k         = 1;

    /* parse arguments */
    parse arg asArgs.1 asArgs.2 asArgs.3 asArgs.4 asArgs.5 asArgs.6 asArgs.7 asArgs.8 asArgs.9
    asArgs.0 = 0;
    do i = 1 to 9
        if (asArgs.i <> '') then
            asArgs.0 = i;
    end
    do i = 1 to asArgs.0
        if (substr(asArgs.i, 1, 1) = '-' | substr(asArgs.i, 1, 1) = '/')  then
        do
            sArg = translate(substr(asArgs.i, 2));
            ch = substr(sArg, 1, 1);
            select
                when (ch = 'N') then
                    fInteractive = 0;
                when (ch = 'I') then
                    fInteractive = 1;
                when (ch = 'W') then
                    fWin32k = 0;
                when (ch = '?' | ch = 'H' | substr(sArg, 1, 2) = '-H') then
                do
                    say 'Odin32 Configure.cmd. $Revision: 1.18 $.'
                    say 'syntax: Configure.cmd [-n] [-w]'
                    say '  -n   Noninteractive.'
                    say '  -w   Don''t build Win32k.'
                    say '  -h   This text.'
                    exit(1);
                end
                otherwise
                    say 'syntax error ('asArgs.i')';
                    exit(2);
            end
        end
    end

    /* strings */
    sIncFile='Makefile.inc';

    /* delete old target files */
    call SysFileDelete sIncFile;

    /* open target file */
    if (stream(sIncFile, 'c', 'open write' ) <> '') then
    do
        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# Generated by 'directory()'configure.cmd'
        call lineout sIncFile, '################################################################################';
        call lineout sIncFile, ''
        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# BASEDIRECTORIES - Win32K'
        call lineout sIncFile, '# Note! These should be absolute paths!'
        call lineout sIncFile, '################################################################################'
        sWin32kBase = directory();
        call lineout sIncFile, 'WIN32KBASE       =' sWin32kBase;
        call lineout sIncFile, 'WIN32KDEV16      =' sWin32kBase'\dev16'
        call lineout sIncFile, 'WIN32KDEV32      =' sWin32kBase'\dev32'
        call lineout sIncFile, 'WIN32KELF2LX     =' sWin32kBase'\elf2lx'
        call lineout sIncFile, 'WIN32KINCLUDE    =' sWin32kBase'\include'
        call lineout sIncFile, 'WIN32KK32        =' sWin32kBase'\k32'
        call lineout sIncFile, 'WIN32KLDR        =' sWin32kBase'\ldr'
        call lineout sIncFile, 'WIN32KLIB        =' sWin32kBase'\lib'
        call lineout sIncFile, 'WIN32KMISC       =' sWin32kBase'\misc'
        call lineout sIncFile, 'WIN32KPE2LX      =' sWin32kBase'\pe2lx'
        call lineout sIncFile, '!ifdef DEBUG'
        call lineout sIncFile, 'WIN32KBIN        =' sWin32kBase'\bin\debug'
        call lineout sIncFile, 'WIN32KLIST       =' sWin32kBase'\list\debug'
        call lineout sIncFile, 'WIN32KOBJ        =' sWin32kBase'\object\debug'
        call lineout sIncFile, '!else'
        call lineout sIncFile, 'WIN32KBIN        =' sWin32kBase'\bin\release'
        call lineout sIncFile, 'WIN32KLIST       =' sWin32kBase'\list\release'
        call lineout sIncFile, 'WIN32KOBJ        =' sWin32kBase'\object\release'
        call lineout sIncFile, '!endif'
        call lineout sIncFile, 'WIN32KOBJECT     = $(WIN32KOBJ)'
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# DDKPATH: base directory of ddk base. (ie. \DDK\BASE not \DDK for DDK v4.0+)'
        call lineout sIncFile, '################################################################################'
        sDDK = SearchPaths('INCLUDE', '..\h\dhcalls.h', 'PATH', '..\h\dhcalls.h', 'BOOKSHELF', '..\base\.\h\dhcalls.h', 'Path to DDK "\BASE" directory (ie. F:\DDK\BASE):');
        call lineout sIncFile, 'DDKPATH          =' sDDK
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# OS/2 TOOLKIT'
        call lineout sIncFile, '################################################################################'
        sTLKT = SearchPaths('INCLUDE', '..\h\os2.h', 'PATH', '..\h\os2.h', 'BOOKSHELF', '..\h\os2.h', 'Path to OS/2 Toolkit directory:');
        call lineout sIncFile, 'TOOLKIT          =' sTLKT
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# IBM VisualAge for C++ base path'
        call lineout sIncFile, '################################################################################'
        sVA = Value('CPPMAIN',, 'OS2ENVIRONMENT'); /* no sure if CPPMAIN or CPPLOCAL is the one to search... */
        if (sVA = '') then
            sVA = SearchPaths('PATH', 'icc.exe', 'BOOKSHELF', '..\bin\icc.exe', 'Path to IBM VisualAge for C++ or IBM C and C++ Compilers, and set its environment you compile:');
        call lineout sIncFile, 'VACPATH          =' sVA
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# Microsoft C v6.0, v6.0a or v7.0'
        call lineout sIncFile, '################################################################################'
        sMSC = SearchPaths('PATH', '..\binp\c3l.exe', 'INCLUDE', '..\binp\c3l.exe','LIB', '..\binp\c3l.exe', 'Path to Microsoft C v6.0, v6.0a or v7.0 directory:');
        call lineout sIncFile, 'MSCPATH          =' sMSC
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# Grep'
        call lineout sIncFile, '################################################################################'
        sGrep = SearchPaths('PATH', 'grep.exe', 'Path to UNIX-like grep utility which supports -v.');
        if (sGrep <> '') then
            call lineout sIncFile, 'GREP             =' sGrep || 'grep.exe'
        call lineout sIncFile, ''

        call stream sIncFile, 'c', 'close';

        /*
         * Create output directories.
         */
        '@mkdir Bin > nul 2> nul'
        '@mkdir Bin\Release > nul 2> nul'
        '@mkdir Bin\Debug > nul 2> nul'

        '@mkdir List > nul 2> nul'
        '@mkdir List\Release > nul 2> nul'
        '@mkdir List\Debug > nul 2> nul'

        '@mkdir Object > nul 2> nul'
        '@mkdir Object\Debug > nul 2> nul'
        '@mkdir Object\Release > nul 2> nul'

        /*
         * Display output.
         */
        if (0 /*fInteractive = 1*/) then /* currently disabled since we're now called from mail config. */
        do
            say 'Configuration completed!'
            say ''
            say 'You should now verify that the settings are correct by listing the file';
            say ''''sIncFile'''';
            say ''
            say 'Remember to do a ''nmake dep'' before making win32k.'
        end
    end
    else
    do
        say 'oops, failed to open outputfile,' sIncFile;
        exit 1;
    end
    exit 0;



/*****************/
/* function area */
/*****************/


/**
 * SearchPaths
 * Parameters:
 *   Takes pair of <envionment variable name> and <filename> as parameters
 *   As final optional parameter you may specify a question to request the string from the user.
 * Returns:
 *   sPath-sFile ignoring '..\'s in sFile.
 */
SearchPaths: procedure expose fInteractive fWin32k;
    /*
     * Check if there is need to waste time searching.
     * Init local variables.
     */
    if (\fWin32k) then
        return '';
    i = 1;
    sPath = '';

    /*
     * Search for the given environments/files combinations.
     */
    do i = 2 to arg() by 2
        sEnv  = arg(i-1);
        sFile = arg(i);
        /* say i':' sEnv '-' sFile; */
        sPath = SysSearchPath(sEnv, sFile);
        /* say 'SysSearchPath('sEnv',' sFile') ->' sPath; */
        if (sPath <> '') then
           leave;
    end

    /* did we find it? */
    if (sPath = '') then
    do  /*
         * If interactive mode, we'll ask for the the path, else skip it.
         */
        say arg(arg());
        if (fInteractive = 1) then
            sPath = linein();
        else
            say '!ignored!';
    end
    else
    do  /*
         * Trim the result for the file part.
         */
        /* look for .\ or ..\ */
        i = lastpos('.\', sFile);
        if (i <> 0) then
            sFile = substr(sFile, i + 1);

        /* cut equally */
        i = lastpos(sFile, sPath);
        if (i = 0) then
            say '!error! sFile:'sFile  'sPath:'sPath;
        /* debug: say 'sPath:'sPath  'sFile:'sFile */
        sPath = substr(sPath, 1, i-1);
    end

    return sPath;
