/* $Id: configure.cmd,v 1.26 2002-12-06 03:29:45 bird Exp $
 *
 * Configuration script.
 * Generates makefile.inc and an empty .depend file.
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysLoadFuncs;

    /* Argument defaults */
    fInteractive    = 1;
    fWin32k         = 1;
    fWithKLib       = 0;
    fUseOldLib      = 0;

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
                when (ch = '-') then
                do
                    parse var sArg sParm'='sOpt
                    select
                        when (sParm = '-WITH-KLIB') then
                            fWithKLib = 1;
                        when (sParm = '-USE-OLDLIB') then
                            fUseOldLib = 1;
                        otherwise
                            say 'syntax error ('asArgs.i')';
                            exit(2);
                    end
                end
                when (ch = '?' | ch = 'H' | substr(sArg, 1, 2) = '-H') then
                do
                    say 'syntax: Configure.cmd [options]'
                    say '  -n           Noninteractive.'
                    say '  -w           Don''t build Win32k.'
                    say '  --with-klib  Build with kLib. (Will checkout kLib for you.)'
                    say '  --use-oldlib Use old LIB tool to build emxomfld-compatible LIB files.'
                    say '  -h           This text.'
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
        call lineout sIncFile, '# Generated by 'directory()'\configure.cmd'
        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, ''
        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# BASE DIRECTORIES - Win32K'
        call lineout sIncFile, '# Note! These should be absolute paths!'
        call lineout sIncFile, '################################################################################'
        sWin32kBase = directory();
        call lineout sIncFile, 'PATH_WIN32K      =' sWin32kBase;
        call lineout sIncFile, 'PATH_KKRNLLIB    =' sWin32kBase'\kKrnlLib';
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
        call lineout sIncFile, 'WIN32KBIN        =' sWin32kBase'\bin\debug.$(CCENV)'
        call lineout sIncFile, 'WIN32KLIST       =' sWin32kBase'\list\debug.$(CCENV)'
        call lineout sIncFile, 'WIN32KOBJ        =' sWin32kBase'\object\debug.$(CCENV)'
        call lineout sIncFile, '!else'
        call lineout sIncFile, 'WIN32KBIN        =' sWin32kBase'\bin\release.$(CCENV)'
        call lineout sIncFile, 'WIN32KLIST       =' sWin32kBase'\list\release.$(CCENV)'
        call lineout sIncFile, 'WIN32KOBJ        =' sWin32kBase'\object\release.$(CCENV)'
        call lineout sIncFile, '!endif'
        call lineout sIncFile, 'WIN32KOBJECT     = $(WIN32KOBJ)'
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# DDKPATH: base directory of ddk base (i.e. \DDK\BASE not \DDK for DDK v4.0+).'
        call lineout sIncFile, '################################################################################'
        sDDK = SearchPaths('PATH_DDKBASE', 'h\dhcalls.h', 'INCLUDE', '..\h\dhcalls.h', 'PATH', '..\h\dhcalls.h', 'BOOKSHELF', '..\base\.\h\dhcalls.h', 'Path to DDK "\BASE" directory (i.e. F:\DDK\BASE):');
        sDDK = ValidatePath(sDDK, 'h\devhdr.h', 'DDK Base Path');
        call lineout sIncFile, 'DDKPATH          =' sDDK
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# OS/2 TOOLKIT'
        call lineout sIncFile, '################################################################################'
        sTLKT = SearchPaths('PATH_TOOLKIT', 'h\os2.h', 'INCLUDE', '..\h\os2.h', 'PATH', '..\h\os2.h', 'BOOKSHELF', '..\h\os2.h', 'Path to OS/2 Toolkit directory:');
        sTLKT = ValidatePath(sTLKT, 'h\bsememf.h', 'OS/2 Toolkit Path');
        call lineout sIncFile, 'TOOLKIT          =' sTLKT
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# IBM VisualAge for C++ base path'
        call lineout sIncFile, '################################################################################'
        sVA = Value('CPPMAIN',, 'OS2ENVIRONMENT'); /* not sure if CPPMAIN or CPPLOCAL is the one to search... */
        if (sVA <> '' & stream(sVA'\bin\icc.exe', 'c', 'query exists') = '') then
            sVA = ''
        if (sVA = '') then
            sVA = SearchPaths('PATH', '..\bin\icc.exe', 'BOOKSHELF', '..\bin\icc.exe', 'Path to IBM VisualAge for C++ or IBM C and C++ Compilers, and set its environment you compile:');
        sVA = ValidatePath(sVA, 'bin\icc.exe', 'VisualAge Path');
        call lineout sIncFile, 'VACPATH          =' sVA
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# Microsoft C v6.0, v6.0a or v7.0'
        call lineout sIncFile, '################################################################################'
        sMSC = SearchPaths('PATH_MSC','binp\c2l.exe','PATH', '..\binp\c3l.exe', 'INCLUDE', '..\binp\c3l.exe','LIB', '..\binp\c3l.exe', 'Path to Microsoft C v6.0, v6.0a or v7.0 directory:');
        sMSC = ValidatePath(sMSC, 'binp\cl.exe', 'Microsoft C Path');
        call lineout sIncFile, 'MSCPATH          =' sMSC
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# Grep'
        call lineout sIncFile, '################################################################################'
        sGrep = SearchPaths('PATH', 'grep.exe', 'Path to UNIX-like grep utility which supports -v.');
        if (sGrep <> '') then
            call lineout sIncFile, 'GREP             =' sGrep || '\grep.exe'
        call lineout sIncFile, ''

        call stream sIncFile, 'c', 'close';

        /*
         * Create output directories.
         */
        '@mkdir Bin > nul 2> nul'
        '@mkdir Bin\Release.vac3 > nul 2> nul'
        '@mkdir Bin\Release.vac36 > nul 2> nul'
        '@mkdir Bin\Debug.vac3 > nul 2> nul'
        '@mkdir Bin\Debug.vac36 > nul 2> nul'

        '@mkdir List > nul 2> nul'
        '@mkdir List\Release.vac3 > nul 2> nul'
        '@mkdir List\Release.vac36 > nul 2> nul'
        '@mkdir List\Debug.vac3 > nul 2> nul'
        '@mkdir List\Debug.vac36 > nul 2> nul'

        '@mkdir Object > nul 2> nul'
        '@mkdir Object\Debug.vac3 > nul 2> nul'
        '@mkdir Object\Debug.vac36 > nul 2> nul'
        '@mkdir Object\Release.vac3 > nul 2> nul'
        '@mkdir Object\Release.vac36 > nul 2> nul'

        /*
         * Display output.
         */
        if (0 /*fInteractive = 1*/) then /* currently disabled since we're now called from main config. */
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

    /*
     * Checkout K-Lib
     */
    if (fWithKLib) then
    do
        say 'Configuring kLib...'
        sDir = directory();
        if (chdir('kKrnlLib')) then
        do
            '@mkdir kLib 2> nul > nul'
            if (chdir('kLib')) then
            do
                if (fInteractive) then
                do
                    say 'Log in to the kLib cvs repository? (Y/N)';
                    pull sAnswer
                    if (substr(sAnswer, 1, 1) = 'Y') then
                        'cvs -d:pserver:readonly@www.netlabs.org:/netlabs.cvs/ktaskmanager login'
                end
                'cvs -d:pserver:readonly@www.netlabs.org:/netlabs.cvs/ktaskmanager co kLibSrc kLibInclude'
            end
        end
        call directory sDir;
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
        sPath = strip(substr(sPath, 1, i-1), 'T', '\');
    end

return sPath;


/**
 * Validate if a file exists to ensure that the given path is valid.
 * @returns The path if valid. If invalid an empty string is returned.
 */
ValidatePath: procedure expose fWin32k
    parse arg sPath, sFile, sDesc

    if (strip(sPath) = "" & \fWin32k) then
        return "";

    rc = stream(sPath'\'sFile, 'c', 'query exists')
    if (rc <> '') then
        return sPath;
    say 'Warning: Validatation of the' sDesc 'failed. Win32k.sys will not be built.'
    say '         path='sPath;
return '';



/**
 * Changes the directory.
 * On error we will call failure.
 * @returns success indicator.
 */
ChDir: procedure expose sStartDir;
    parse arg sDir

    'cd' sDir
    if (rc <> 0) then
    do
        call failure rc, 'Failed to ChDir into' sDir '(CWD='directory()').'
        return 0;
    end
return 1;

