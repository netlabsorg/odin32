/* $Id: configure.cmd,v 1.1 1999-09-08 20:48:58 bird Exp $
 *
 * Configuration script.
 * Generates makefile.inc and an empty .depend file.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysLoadFuncs;

    /* strings */
    sIncFile='Makefile.inc';

    /* delete old target files */
    call SysFileDelete '.depend';
    call SysFileDelete sIncFile;

    /* create empty .depend file */
    'echo #run nmake depend > .depend'

    /* open target file */
    if (stream(sIncFile, 'c', 'open write' ) <> '') then do
        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '#'
        call lineout sIncFile, '# Copyright (c) 1998-1999 knut st. osmundsen'
        call lineout sIncFile, '#'
        call lineout sIncFile, '################################################################################';
        call lineout sIncFile, ''
        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# BASEDIRECTORIES - Win32-OS/2 project and Win32K -'
        call lineout sIncFile, '# Note these should be absolute paths!'
        call lineout sIncFile, '################################################################################'
        sWin32kBase = directory();
        call lineout sIncFile, 'WIN32KBASE       =' sWin32kBase;
        call lineout sIncFile, 'WIN32KOBJ        =' sWin32kBase'\object'
        call lineout sIncFile, 'WIN32KLIST       =' sWin32kBase'\list'
        sPDWin32Base = substr(sWin32kBase, 1, lastpos('\', sWin32kBase, lastpos('\', sWin32kBase)-1)-1);
        call lineout sIncFile, 'PDWIN32BASE      =' sPDWin32Base
        call lineout sIncFile, 'PDWIN32_INCLUDE  =' sPDWin32Base'\include'
        call lineout sIncFile, 'PDWIN32_BIN      =' sPDWin32Base'\bin'
        call lineout sIncFile, 'PDWIN32_LIB      =' sPDWin32Base'\lib'
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# DDKPATH : base directory of ddk base. (ie. \DDK\BASE not \DDK for DDK v4.0+)'
        call lineout sIncFile, '################################################################################'
        sDDK = SearchPaths('INCLUDE', '..\h\dhcalls.h', 'PATH', '..\h\dhcalls.h', 'BOOKSHELF', '..\h\dhcalls.h', 'Path to DDK "\BASE" directory (ie. F:\DDK\BASE):');
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
        if sVA = '' then
            sVA = SearchPaths('PATH', 'icc.exe', 'BOOKSHELF', '..\bin\icc.exe', 'Path to IBM VisualAge for C++ or IBM C and C++ Compilers, and set its environment you compile:');
        call lineout sIncFile, 'VACPATH          =' sVA
        call lineout sIncFile, ''

        call lineout sIncFile, '################################################################################'
        call lineout sIncFile, '# Microsoft C v6.0, v6.0a or v7.0'
        call lineout sIncFile, '################################################################################'
        sMSC = SearchPaths('PATH', '..\binp\c3l.exe', 'INCLUDE', '..\binp\c3l.exe','LIB', '..\binp\c3l.exe', 'Path to Microsoft C v6.0, v6.0a or v7.0 directory:');
        call lineout sIncFile, 'MSCPATH          =' sMSC
        call lineout sIncFile, ''

        call stream sIncFile, 'c', 'close';

        say 'Configuration compleated!'
        say ''
        say 'You should now verify that the settings are correct by listing the file';
        say ''''sIncFile'''';
    end
    else do
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
SearchPaths: procedure
    i = 1;
    sEnv  = arg(i);
    sFile = arg(i+1);
    sPath = '';
    do while (sPath == '' & sEnv <> '' & sFile <> '')
        sPath = SysSearchPath(sEnv, sFile);
        /* debug: say 'sEnv:'sEnv 'sFile:'sFile 'sPath:'sPath 'i:'i */
        i = i + 2;
        sEnv  = arg(i);
        sFile = arg(i+1);
    end

    if (sPath == '' & sEnv <> '' & sFile == '') then do
        say sEnv;
        pull sPath;
    end
    else do
        if sPath <> '' then do
            /*  */
            i = lastpos('..\', sFile);
            if i <> 0 then
                sFile = substr(sFile, i + 2);

            /* cut equally */
            i = lastpos(sFile, sPath);
            if i == 0 then
                say '!error! sFile:'sFile  'sPath:'sPath;
            /* debug: say 'sPath:'sPath  'sFile:'sFile */
            sPath = substr(sPath, 1, i-1);
        end
    end

    return sPath;
