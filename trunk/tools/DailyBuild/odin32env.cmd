/* $Id: odin32env.cmd,v 1.18 2001-04-02 01:54:11 bird Exp $
 *
 * Sets the build environment.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

parse arg sCompiler

    /*
     * To use this script you'll have to adjust some paths to match your local
     * installation. These paths are (mainly) called s<>Main or s<>Base, and usually
     * the only assignment statements in the configuration procedures below.
     *
     * In addition the CVSROOT, USER and HOME directories right below have to corrected.
     */
    call EnvVar_Set 0, 'CVSROOT',   ':pserver:bird@www.netlabs.org:e:/netlabs.cvs/odin32'
    call EnvVar_Set 0, 'HOME',      'd:\knut\home'
    call EnvVar_Set 0, 'USER',      'bird'
    call EnvVar_Set 0, 'HOSTNAME',  'Univac-Job'


    /*
     * Call the procedures which configure each tool.
     * (The order this is done is _very_ important!)
     */
    call Jade 0;
    call DocBookXML 0;
    call WarpIn 0;
    call mySQL 0;
    call Perl 0;
    call CVS 0;
    call DDKBase 0;
    call MSC60 0;
    call EMX 0;
    if (sCompiler = 'watcom') then
    do /* watcom */
        call VAC30 1;
        call VAC36 1;
        call Watcom 0;
    end
    else if (sCompiler = 'vac36') then
    do /* vac36 */
        call VAC30 1;
        call Watcom 1;
        call NetQOS2 0;
        call VAC36 0;
    end
    else
    do /* default is vac30 */
        call VAC36 1;
        call Watcom 1;
        call VAC30 0;
    end
    call Toolkit40 0;
    call EnvVar_Set 0, 'DEBUG',      '1'

    exit(0);




/**
 * Individual tools configurations
 **/


/*
 * Concurrent Versions System (CVS)
 */
CVS: procedure
    parse arg fRM

    /*
     * Concurrent Versions System (CVS) main directory.
     */
    sCVSMain    = 'd:\knut\tools\cvs';
    call EnvVar_Set      fRM, 'cvsmain',     sCVSMain;
    call EnvVar_AddFront fRM, 'path',        sCVSMain'\bin;'
    call EnvVar_AddFront fRM, 'bookshelf',   sCVSMain'\book;'
    return 0;


/*
 * EMX
 */
EMX: procedure
    parse arg fRM

    /*
     * EMX/GCC main directory.
     */
    sEMXBack    = 'c:\emx';
    sEMXForw    = 'c:/emx';

    call EnvVar_Set      fRM, 'emx', sEMXBack;

    call EnvVar_AddFront fRM, 'BEGINLIBPATH',       sEMXBack'\dll;'
    call EnvVar_AddFront fRM, 'PATH',               sEMXBack'\bin;'
    call EnvVar_AddFront fRM, 'DPATH',              sEMXBack'\book;'
    call EnvVar_AddFront fRM, 'BOOKSHELF',          sEMXBack'\book;'
    call EnvVar_AddFront fRM, 'HELP',               sEMXBack'\help;'
    call EnvVar_AddFront fRM, 'C_INCLUDE_PATH',     sEMXForw'/include'
    call EnvVar_AddFront fRM, 'LIBRARY_PATH',       sEMXForw'/lib'
    call EnvVar_AddFront fRM, 'CPLUS_INCLUDE_PATH', sEMXForw'/include/cpp;'sEMXForw'/include'
    call EnvVar_Set      fRM, 'PROTODIR',           sEMXForw'/include/cpp/gen'
    call EnvVar_Set      fRM, 'OBJC_INCLUDE_PATH',  sEMXForw'/include'
    call EnvVar_Set      fRM, 'GCCLOAD',            '5'
    call EnvVar_Set      fRM, 'GCCOPT',             '-pipe'
    call EnvVar_AddFront fRM, 'INFOPATH',           sEMXForw'/info'
    call EnvVar_Set      fRM, 'EMXBOOK',            'emxdev.inf+emxlib.inf+emxgnu.inf+emxbsd.inf'
    call EnvVar_AddFront fRM, 'HELPNDX',            'emxbook.ndx', '+', 1
    call EnvVar_Set      fRM, 'EMXOPT',             '-c -n -h1024'
    if EnvVar_Get('TERM') = '' then do
        call EnvVar_Set  fRM, 'TERM',               'ansi'
        call EnvVar_Set  fRM, 'TERMCAP',            sEMXForw'/etc/termcap.dat'
    end
    return 0;


/*
 *  Device Driver Kit (DDK) base.
 */
DDKBase: procedure
    parse arg fRM

    /*
     * Device Driver Kit (DDK) (v4.0+) base (important not main) directory.
     */
    sDDKBase    = 'd:\knut\ddk\base';
    call EnvVar_Set      fRM, 'ddkbase', sDDKBase;
    call EnvVar_AddFront fRM, 'path',        sDDKBase'\tools;'
    call EnvVar_AddFront fRM, 'include',     sDDKBase'\h;'
    call EnvVar_AddFront fRM, 'lib',         sDDKBase'\lib;'
    call EnvVar_AddFront fRM, 'bookshelf',   sDDKBase'\..\docs;'

    return 0;


/*
 * DocBook 4.1.x XML.
 */
DocBookXML: procedure
    parse arg fRM

    /*
     * DocBook XML dir.
     */
    sDocBookXML = 'e:\coding\DocBook\XML'

    call EnvVar_Set      fRM, 'docbookxml',         sDocBookXML
    call EnvVar_AddFront fRM, 'SGML_CATALOG_FILES', sDocBookXML'\docbook.cat;'

    return 0;


/*
 * Jade 1.2.1 for OS/2.
 */
Jade: procedure
    parse arg fRM

    /*
     * Jade install dir.
     */
    sJadeMain = 'e:\apps\jade'

    call EnvVar_Set      fRM, 'jademain',           sJadeMain
    call EnvVar_AddFront fRM, 'path',               sJadeMain';'
    call EnvVar_AddFront fRM, 'SGML_CATALOG_FILES', sJadeMain'\catalog;'
    call EnvVar_AddEnd   fRM, 'SGML_CATALOG_FILES', '.\catalog;'

    return 0;


/*
 * Microsoft C v6.0a
 */
MSC60: procedure
    parse arg fRM

    /*
     * Microsoft C v6.0a main directory.
     */
    sMSCMain    = 'd:\knut\ddk\ddktools\toolkits\msc60';
    call EnvVar_Set      fRM, 'mscmain',    sMSCMain;
    call EnvVar_AddFront fRM, 'path',       sMSCMain'\binp;'
    call EnvVar_AddFront fRM, 'include',    sMSCMain'\include;'
    call EnvVar_AddFront fRM, 'lib',        sMSCMain'\lib;'
    return 0;



/*
 * mySQL Database system
 */
mySQL: procedure
    parse arg fRM

    /*
     * mySQL Database system main directory.
     */
    sMySQLMain    = 'd:\knut\apps\MySql';
    call EnvVar_Set      fRM, 'mysqlmain',      sMySQLMain;
    call EnvVar_AddFront fRM, 'path',           sMySQLMain'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sMySQLMain'\dll;'
    call EnvVar_AddFront fRM, 'include',        sMySQLMain'\include;'
    call EnvVar_AddEnd   fRM, 'bookshelf',      sMySQLMain'\book;'
    /*call EnvVar_AddFront fRM, 'lib',            sMySQLMain'\lib;'*/
    return 0;



/*
 * NetQOS2 - help subsystem++ for VAC 3.6.5 and VAC 4.0
 */
NetQOS2: procedure
    parse arg fRM
    sNetQ11Main = 'e:\apps\imnnq';
    call EnvVar_Set      fRM, 'netq11main',     sNetQ11Main
    call EnvVar_AddFront fRM, 'beginlibpath',   sNetQ11Main';'
    call EnvVar_AddFront fRM, 'path',           sNetQ11Main';'
    call EnvVar_AddFront fRM, 'dpath',          sNetQ11Main';'
    call EnvVar_AddEnd   fRM, 'nlspath',        sNetQ11Main'\%N;'
    call EnvVar_Set      fRM, 'imndatasrv',     sNetQ11Main'\DATA'
    call EnvVar_Set      fRM, 'imndatacl',      sNetQ11Main'\DATA'
    call EnvVar_Set      fRM, 'imnworksrv',     sNetQ11Main'\WORK'
    call EnvVar_Set      fRM, 'imnworkcl',      sNetQ11Main'\WORK'
    call EnvVar_Set      fRM, 'imnnlpssrv',     sNetQ11Main
    call EnvVar_Set      fRM, 'imnnlpscl',      sNetQ11Main
    call EnvVar_Set      fRM, 'imncccfgfile',   'NETQ.CFG'
    call EnvVar_Set      fRM, 'imncscfgfile',   'NETQ.CFG'
    call EnvVar_Set      fRM, 'imqconfigsrv',   sNetQ11Main'\instance'
    call EnvVar_Set      fRM, 'imqconfigcl',    sNetQ11Main'\instance\dbcshelp'
    call EnvVar_Set      fRM, 'lite_locales',   sNetQ11Main
    return 0;



/*
 * PERL 5005_53
 */
Perl: procedure
    parse arg fRM

    /*
     * Perl main directory.
     */
    sPerlMain       = 'd:\perllib';
    sPerlMainForw   = 'd:/perllib';
    call EnvVar_Set      fRM, 'perlmain',       sPerlMain;
    call EnvVar_AddFront fRM, 'path',           sPerlMain'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPerlMain'\dll;'
    call EnvVar_AddEnd   fRM, 'bookshelf',      sPerlMain'\book;'
    call EnvVar_Set      fRM, 'perllib_prefix', sPerlMainForw'/lib;'sPerlMainForw'/lib'
    call EnvVar_Set      fRM, 'perl_sh_dir',    sPerlMainForw'/bin_sh'
    call EnvVar_Set      fRM, 'manpath',        sPerlMainForw'/man'
    call EnvVar_Set      fRM, 'perl5lib',       sPerlMainForw'/lib'
    return 0;



/*
 * OS/2 Programmers Toolkit v4.0 (CSD1/4)
 */
Toolkit40: procedure
    parse arg fRM

    /*
     * Toolkit (4.0) main directory.
     */
     sTKMain    = 'c:\appl\os2\toolkt40';

    call EnvVar_Set      fRM, 'tkmain',  sTkMain;

    call EnvVar_AddFront fRM, 'beginlibpath',   sTkMain'\BETA\DLL;'sTkMain'\SAMPLES\MM\DLL;'sTkMain'\SAMPLES\OPENDOC\PARTS\DLL;'sTkMain'\SOM\COMMON\DLL;'sTkMain'\SOM\LIB;'sTkMain'\OPENDOC\BASE\DLL;'sTkMain'\OPENDOC\BASE\LOCALE\EN_US;'sTkMain'\DLL;'
    call EnvVar_AddFront fRM, 'path',           sTkMain'\BETA\BIN;'sTkMain'\SOM\COMMON;'sTkMain'\SOM\BIN;.;'sTkMain'\OPENDOC\BASE\BIN;'sTkMain'\BIN;'
    call EnvVar_AddFront fRM, 'dpath',          sTkMain'\SOM\COMMON\SYSTEM;'sTkMain'\SOM\MSG;'sTkMain'\OPENDOC\BASE\MSG;'sTkMain'\BOOK;'sTkMain'\MSG;'
    call EnvVar_AddFront fRM, 'help',           sTkMain'\BETA\HELP;'sTkMain'\OPENDOC\BASE\LOCALE\EN_US;'sTkMain'\HELP;'
    call EnvVar_AddFront fRM, 'bookshelf',      sTkMain'\BETA\BOOK;'sTkMain'\BOOK;'sTkMain'\ARCHIVED;'
    call EnvVar_AddFront fRM, 'somir',          sTkMain'\SOM\COMMON\ETC\214\SOM.IR;'sTkMain'\OPENDOC\BASE\AVLSHELL.IR;'
    call EnvVar_AddEnd   fRM, 'somir',          sTkMain'\OPENDOC\CUSTOM\OD.IR;'sTkMain'\SAMPLES\REXX\SOM\ANIMAL\ORXSMP.IR;'
    call EnvVar_AddFront fRM, 'include',        sTkMain'\SPEECH\H;'sTkMain'\BETA\H;'sTkMain'\SAMPLES\OPENDOC\PARTS\INCLUDE;'sTkMain'\SOM\INCLUDE;'sTkMain'\INC;'sTkMain'\H\GL;'sTkMain'\H;'
    call EnvVar_AddEnd   fRM, 'include',        sTkMain'\H\LIBC;.;'
    call EnvVar_AddFront fRM, 'lib',            sTkMain'\SPEECH\LIB;'sTkMain'\SAMPLES\MM\LIB;'sTkMain'\LIB;'sTkMain'\SOM\LIB;'sTkMain'\OPENDOC\BASE\LIB;'
    call EnvVar_AddFront fRM, 'nlspath',        sTkMain'\OPENDOC\BASE\LOCALE\EN_US\%N;'sTkMain'\MSG\%N;'
    call EnvVar_AddFront fRM, 'locpath',        sTkMain'\OPENDOC\BASE\LOCALE;'
    call EnvVar_AddFront fRM, 'ipfc',           sTkMain'\IPFC;'
    call EnvVar_Set      fRM, 'odbase',         sTkMain'\OPENDOC\BASE'
    call EnvVar_Set      fRM, 'odlang',         'en_US'
    call EnvVar_AddFront fRM, 'odbasepaths',    sTkMain'\OPENDOC\BASE;'
    call EnvVar_Set      fRM, 'odcfg',          sTkMain'\OPENDOC\CUSTOM'
    call EnvVar_Set      fRM, 'odtmp',          EnvVar_Get('tmp');
    call EnvVar_Set      fRM, 'sombase',        sTkMain'\SOM'
    call EnvVar_Set      fRM, 'somruntime',     sTkMain'\SOM\COMMON'
    call EnvVar_AddEnd   fRM, 'helpndx',        'EPMKWHLP.NDX+', '+', 1

    call EnvVar_Set      fRM, 'cpref',          'CP1.INF+CP2.INF+CP3.INF'
    call EnvVar_Set      fRM, 'gpiref',         'GPI1.INF+GPI2.INF+GPI3.INF+GPI4.INF'
    call EnvVar_Set      fRM, 'mmref',          'MMREF1.INF+MMREF2.INF+MMREF3.INF'
    call EnvVar_Set      fRM, 'pmref',          'PM1.INF+PM2.INF+PM3.INF+PM4.INF+PM5.INF'
    call EnvVar_Set      fRM, 'wpsref',         'WPS1.INF+WPS2.INF+WPS3.INF'
    call EnvVar_AddFront fRM, 'sminclude',      sTkMain'\H;'sTkMain'\IDL;'sTkMain'\SOM\INCLUDE;.;'sTkMain'\OPENDOC\BASE\INCLUDE;'sTkMain'\SAMPLES\OPENDOC\PARTS\INCLUDE;'
    call EnvVar_Set      fRM, 'smaddstar',      '1'
    call EnvVar_Set      fRM, 'smemit',         'h;ih;c'
    call EnvVar_Set      fRM, 'smtmp',          EnvVar_Get('tmp');
    call EnvVar_Set      fRM, 'smclasses',      'WPTYPES.IDL'
    call EnvVar_Set      fRM, 'odparts',        sTkMain'\SAMPLES\OPENDOC\PARTS'
    call EnvVar_Set      fRM, 'odsrc',          sTkMain'\SAMPLES\OPENDOC\PARTS'
    call EnvVar_AddFront fRM, 'odpartspaths',   sTkMain'\SAMPLES\OPENDOC\PARTS;'
    call EnvVar_AddFront fRM, 'odsrcpaths',     sTkMain'\SAMPLES\OPENDOC\PARTS;'
    /*
    call EnvVar_Set      fRM, 'CAT_MACHINE=COM1:57600'
    call EnvVar_Set      fRM, 'CAT_HOST_BIN_PATH='sTkMain'\BIN'
    call EnvVar_Set      fRM, 'CAT_COMMUNICATION_TYPE=ASYNC_SIGBRK'
    call EnvVar_AddFront fRM, 'CAT_HOST_SOURCE_PATH='sTkMain'\BIN;'
    */
    return 0;



/*
 * OS/2 Programmers Toolkit v4.5
 */
Toolkit45: procedure
    parse arg fRM

    /*
     * Toolkit (4.5) main directory.
     */
     sTKMain    = 'd:\knut\tools\toolkit45';

    call EnvVar_Set      fRM, 'tkmain',  sTkMain;

    call EnvVar_AddFront fRM, 'path',        sTkMain'\bin;'
    call EnvVar_AddFront fRM, 'dpath',       sTkMain'\book;'
    call EnvVar_AddFront fRM, 'dpath',       sTkMain'\msg;'
    call EnvVar_AddFront fRM, 'beginlibpath', sTkMain'\dll;'
    call EnvVar_AddFront fRM, 'help',        sTkMain'\help;'
    call EnvVar_AddFront fRM, 'bookshelf',   sTkMain'\archived;'
    call EnvVar_AddFront fRM, 'bookshelf',   sTkMain'\book;'
    call EnvVar_AddFront fRM, 'nlspath',     sTkMain'\msg\%N;'
    call EnvVar_AddEnd   fRM, 'ulspath',     sTkMain'\language;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\H;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\H\GL;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\SPEECH\H;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\H\RPC;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\H\NETNB;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\H\NETINET;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\H\NET;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\H\ARPA;'
    call EnvVar_AddFront fRM, 'include',     sTkMain'\INC;'
    call EnvVar_AddEnd   fRM, 'lib',         sTkMain'\SAMPLES\MM\LIB;'
    call EnvVar_AddEnd   fRM, 'lib',         sTkMain'\SPEECH\LIB;'
    call EnvVar_AddFront fRM, 'lib',         sTkMain'\lib;'
    call EnvVar_AddEnd   fRM, 'helpndx',     'EPMKWHLP.NDX', '+', 1
    call EnvVar_AddFront fRM, 'ipfc',        sTkMain'\ipfc;'
    call EnvVar_Set      fRM, 'LANG',        'en_us'
    call EnvVar_Set      fRM, 'CPREF',       'CP1.INF+CP2.INF+CP3.INF'
    call EnvVar_Set      fRM, 'GPIREF',      'GPI1.INF+GPI2.INF+GPI3.INF+GPI4.INF'
    call EnvVar_Set      fRM, 'MMREF',       'MMREF1.INF+MMREF2.INF+MMREF3.INF'
    call EnvVar_Set      fRM, 'PMREF',       'PM1.INF+PM2.INF+PM3.INF+PM4.INF+PM5.INF'
    call EnvVar_Set      fRM, 'WPSREF',      'WPS1.INF+WPS2.INF+WPS3.INF'
    /*
    call EnvVar_Set      fRM, 'CAT_MACHINE', 'COM1:57600'
    call EnvVar_Set      fRM, 'CAT_HOST_BIN_PATH', TKMAIN'\BIN'
    call EnvVar_Set      fRM, 'CAT_COMMUNICATION_TYPE', 'ASYNC_SIGBRK'
    call EnvVar_Set      fRM, 'CAT_HOST_SOURCE_PATH',TKMAIN'\BIN;'
    */

    return 0;



/*
 * IBM Visual Age for C++ v3.08 for OS/2
 */
VAC30: procedure
    parse arg fRM

    /*
     * IBM Visual Age for C++ Version 3.08 main directory.
     */
    sCPPMain = 'c:\appl\os2\ibmcpp30'
    call EnvVar_Set      fRM, 'CPPMAIN',        sCPPMain
    call EnvVar_AddFront fRM, 'beginlibpath',   sCPPMain'\DLL;'sCPPMain'\SAMPLES\TOOLKIT\DLL;'
    call EnvVar_AddFront fRM, 'path',           sCPPMain'\BIN;'sCPPMain'\SMARTS\SCRIPTS;'sCPPMain'\HELP;'
    call EnvVar_AddFront fRM, 'dpath',          sCPPMain'\HELP;'sCPPMain';'sCPPMain'\LOCALE;'sCPPMain'\MACROS;'sCPPMain'\BND;'
    call EnvVar_AddFront fRM, 'help',           sCPPMain'\HELP;'sCPPMain'\SAMPLES\TOOLKIT\HELP;'
    call EnvVar_AddFront fRM, 'bookshelf',      sCPPMain'\HELP;'
    call EnvVar_AddFront fRM, 'somir',          sCPPMain'\ETC\SOM.IR;'
    call EnvVar_AddFront fRM, 'cpphelp_ini',    'C:\OS2\SYSTEM'
    call EnvVar_AddFront fRM, 'locpath',        sCPPMain'\LOCALE;%LOCPATH%;'
    call EnvVar_AddFront fRM, 'include',        sCPPMain'\INCLUDE;'sCPPMain'\INCLUDE\OS2;'sCPPMain'\INC;'sCPPMain'\INCLUDE\SOM;'
    call EnvVar_AddFront fRM, 'sminclude',      sCPPMain'\INCLUDE\OS2;'sCPPMain'\INCLUDE;'sCPPMain'\INCLUDE\SOM;'

    call EnvVar_AddFront fRM, 'vbpath',         '.;'sCPPMain'\DDE4VB;'
    call EnvVar_Set      fRM, 'tmpdir',         EnvVar_Get('tmp')
    call EnvVar_Set      fRM, 'lxevfref',       'EVFELREF.INF+LPXCREF.INF'
    call EnvVar_Set      fRM, 'lxevfhdi',       'EVFELHDI.INF+LPEXHDI.INF'
    call EnvVar_AddFront fRM, 'lpath',          sCPPMain'\MACROS;'
    call EnvVar_AddFront fRM, 'codelpath',      sCPPMain'\CODE\MACROS;'sCPPMain'\MACROS;'
    call EnvVar_Set      fRM, 'clref',          'CPPCLRF.INF+CPPDAT.INF+CPPAPP.INF+CPPWIN.INF+CPPCTL.INF+CPPADV.INF+CPP2DG.INF+CPPDDE.INF+CPPDM.INF+CPPMM.INF+CPPCLRB.INF'
    call EnvVar_AddFront fRM, 'ipfc',           sCPPMain'\IPFC'
    call EnvVar_AddFront fRM, 'lib',            sCPPMain'\LIB;'sCPPMain'\DLL;'
    call EnvVar_Set      fRM, 'cpplocal',       sCPPMain
    call EnvVar_Set      fRM, 'cppmain',        sCPPMain
    call EnvVar_Set      fRM, 'cppwork',        sCPPMain
    call EnvVar_Set      fRM, 'iwf.default_prj','CPPDFTPRJ'

    call EnvVar_Set      fRM, 'iwf.solution_lang_support', 'CPPIBS30;ENG'
    call EnvVar_Set      fRM, 'vacpp_shared'    'FALSE'
    call EnvVar_Set      fRM, 'iwfhelp',        'IWFHDI.INF'
    call EnvVar_Set      fRM, 'iwfopt',         sCPPMain

    call EnvVar_Set      fRM, 'somruntime',     sCPPMain'\DLL'
    call EnvVar_Set      fRM, 'smaddstar',      '1'
    call EnvVar_Set      fRM, 'smemit',         'h;ih;c'
    call EnvVar_Set      fRM, 'sombase',        sCPPMain
    call EnvVar_Set      fRM, 'smtmp',          EnvVar_Get('tmp')
    call EnvVar_Set      fRM, 'smclasses',      'WPTYPES.IDL'

    /* FIXME THESE USES '+' AS SEPERATOR NOT ';'!!! */
    call EnvVar_AddFront fRM, 'helpndx',        'CPP.NDX+CPPBRS.NDX', '+',1
    call EnvVar_AddFront fRM, 'ipf_keys',       'SHOWNAV'
    return 0;



/*
 * Visual Age / C and C++ tools v3.6.5 for OS/2
 */
VAC36: procedure
    parse arg fRM

    /*
     * IBM C/C++ Compiler and Tools Version 3.6.5 main directory.
     */
    sCxxMain    = 'e:\apps\ibmcxxo';

    call EnvVar_Set      fRM, 'cxxmain', sCxxMain;

    call EnvVar_AddFront fRM, 'beginlibpath', sCxxMain'\dll;'sCxxMain'\runtime;'
    call EnvVar_AddFront fRM, 'dpath',       sCxxMain'\help;'sCxxMain'\local;'
    call EnvVar_AddFront fRM, 'path',        sCxxMain'\bin;'
    call EnvVar_AddFront fRM, 'help',        sCxxMain'\help;'
    call EnvVar_AddFront fRM, 'locpath',     sCxxMain'\locale;'
    call EnvVar_AddFront fRM, 'nlspath',     sCxxMain'\msg\%N;'
    call EnvVar_AddFront fRM, 'include',     sCxxMain'\include;'
    call EnvVar_AddFront fRM, 'lib',         sCxxMain'\lib;'
    call EnvVar_AddFront fRM, 'ipfc',        sCxxMain'\bin;'
    call EnvVar_Set      fRM, 'LANG',        'en_us'
    call EnvVar_Set      fRM, 'CPP_DBG_LANG', 'CPP'
    return 0;


/*
 * Visual Age for C++ v4.0 for OS/2.
 */
VAC36: procedure
    parse arg fRM

    /*
     * IBM VisualAge for C++ v4.0 main directory.
     */
    sVACppMain    = 'e:\apps\ibmcpp40';

    call EnvVar_Set      fRM, 'vacppmain', sVACppMain;

    call EnvVar_AddFront fRM, 'beginlibpath', sVACppMain'\dll;'sVACppMain'\runtime;'
    call EnvVar_AddFront fRM, 'dpath',       sVACppMain'\etc;'sVACppMain'\help;'sVACppMain'\runtime;'
    call EnvVar_AddFront fRM, 'path',        sVACppMain'\bin;'
    call EnvVar_AddFront fRM, 'help',        sVACppMain'\help;'
    call EnvVar_AddFront fRM, 'locpath',     sVACppMain'\runtime\locale;'
    call EnvVar_AddFront fRM, 'nlspath',     sVACppMain'\msg\%N;'
    call EnvVar_AddFront fRM, 'include',     sVACppMain'\ivb;'sVACppMain'\include;'
    call EnvVar_AddFront fRM, 'lib',         sVACppMain'\lib;'
    call EnvVar_AddFront fRM, 'ipfc',        sVACppMain'\bin;'
    call EnvVar_Set      fRM, 'cpplpath4',   sVACppMain'\macros'
    call EnvVar_Set      fRM, 'system_ice',  sVACppMain'\etc\system.ice'
    call EnvVar_Set      fRM, 'vbpath',      sVACppMain'\ivb'
    call EnvVar_Set      fRM, 'os2',         '1'
    call EnvVar_Set      fRM, 'LANG',        'en_us'
    return 0;


/*
 * Watcom C/C++ 11.0
 */
Watcom: procedure
    parse arg fRM

    /*
     * Watcom C/C++ 11.0 Main directory.
     */
    sWatcom = 'e:\apps\Watcom';

    call EnvVar_Set      fRM, 'watcom', sWatcom;

    call EnvVar_Set      fRM, 'edpath',      sWatcom'\eddat'
    call EnvVar_AddFront fRM, 'beginlibpath', sWatcom'\binp\dll;'
    call EnvVar_AddFront fRM, 'path',        sWatcom'\binp;'sWatcom'\binw;'
    call EnvVar_AddFront fRM, 'help',        sWatcom'\binp\help;'
    call EnvVar_AddFront fRM, 'bookshelf',   sWatcom'\binp\help;'
    call EnvVar_AddFront fRM, 'include',     sWatcom'\h;'
  /*call EnvVar_AddFront fRM, 'include',     sWatcom'\h\os2;'sWatcom'\h;'*/ /* h\os2 ? */
/*
    'detach' sWatcom'\binp\batserv.exe'
    'detach' sWatcom'\binp\nmpbind.exe'
*/
    return 0;


/*
 * WarpIn
 */
WarpIn: procedure
    parse arg fRM

    /*
     * Concurrent Versions System (CVS) main directory.
     */
    sWarpInMain = 'd:\knut\tools\WarpIn';
    call EnvVar_Set      fRM, 'sWarpInMain', sWarpInMain;
    call EnvVar_AddFront fRM, 'path',        sWarpInMain';'
    call EnvVar_AddFront fRM, 'beginlibpath', sWarpInMain';'
    call EnvVar_AddFront fRM, 'bookshelf',   sWarpInMain';'
    return 0;



/**
 * Helper Procedure section
 **/


/*
 * Add sToAdd in front of sEnvVar.
 * Note: sToAdd now is allowed to be alist!
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvVar_addFront: procedure
    parse arg fRM, sEnvVar, sToAdd, sSeparator, fNoEnd

    /* Sets default separator and fNoEnd if not specified. */
    if (sSeparator = '') then sSeparator = ';';
    if (fNoEnd = '')     then fNoEnd = 0;

    /* Add evt. missing separator at end. */
    if (substr(sToAdd, length(sToAdd), 1) <> sSeparator) then
        sToAdd = sToAdd || sSeparator;

    /* Check and evt. remove separator at start of sToAdd */
    if (substr(sToAdd, 1, 1) = cSeparator) then
        sToAdd = substr(sToAdd, 2);

    /* Loop thru sToAdd */
    rc = 0;
    i = length(sToAdd);
    do while i > 1 & rc = 0
        j = lastpos(sSeparator, sToAdd, i-1);
        rc = _EnvVar_AddFront2(fRM, sEnvVar, substr(sToAdd, j+1, i - j), sSeparator, fNoEnd);
        i = j;
    end

    return rc;

/*
 * Add sToAdd in front of sEnvVar.
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
_EnvVar_AddFront2: procedure
    parse arg fRM, sEnvVar, sToAdd, sSeparator, fNoEnd

    /* Add evt. missing separator at end. */
    if (substr(sToAdd, length(sToAdd), 1) <> sSeparator) then
        sToAdd = sToAdd || sSeparator;

    /* Check and evt. remove the separator at start of sToAdd */
    if (substr(sToAdd, 1, 1) = sSeparator) then
        sToAdd = substr(sToAdd, 2);

    /* Get original variable without sToAdd. */
    sOrgEnvVar = _EnvVar_RemoveFrom(EnvVar_Get(sEnvVar),,
                                    sToAdd, sSeparator, fNoEnd);

    /* If only removing - nothing more to do */
    if (fRM) then
        return EnvVar_Set(0, sEnvVar, sOrgEnvVar);

    /* Create new value */
    sNewValue = sToAdd || sOrgEnvVar;

    /* Remove or add separator if necessary */
    if (fNoEnd  & substr(sNewValue, length(sNewValue), 1) =  sSeparator) then
        sNewValue = substr(sNewValue, 1, length(sNewValue) - 1);
    if (\fNoEnd & substr(sNewValue, length(sNewValue), 1) <> sSeparator) then
        sNewValue = sNewValue || sSeparator;

    /* Set environment */
    return EnvVar_Set(0, sEnvVar, sNewValue);


/*
 * Add sToAdd as the end of sEnvVar.
 * Note: sToAdd now is allowed to be alist!
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvVar_addEnd: procedure
    parse arg fRM, sEnvVar, sToAdd, sSeparator, fNoEnd

    /* Sets default separator and fNoEnd if not specified. */
    if (sSeparator = '') then sSeparator = ';';
    if (fNoEnd = '')     then fNoEnd = 0;

    /* Add evt. missing separator at end. */
    if (substr(sToAdd, length(sToAdd), 1) <> sSeparator) then
        sToAdd = sToAdd || sSeparator;

    /* Check and evt. remove separator at start of sToAdd */
    if (substr(sToAdd, 1, 1) = sSeparator) then
        sToAdd = substr(sToAdd, 2);

    /* Loop thru sToAdd */
    rc = 0;
    i = length(sToAdd);
    do while i > 1 & rc = 0
        j = lastpos(sSeparator, sToAdd, i-1);
        rc = _EnvVar_AddEnd2(fRM, sEnvVar, substr(sToAdd, j+1, i - j), sSeparator, fNoEnd);
        i = j;
    end

    return rc;

/*
 * Add sToAdd as the end of sEnvVar.
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
_EnvVar_AddEnd2: procedure
    parse arg fRM, sEnvVar, sToAdd, sSeparator, fNoEnd

    /* Check and evt. remove separator at start of sToAdd */
    if (substr(sToAdd, 1, 1) = sSeparator) then
        sToAdd = substr(sToAdd, 2);

    /* Get original variable without sToAdd. */
    sOrgEnvVar = _EnvVar_RemoveFrom(EnvVar_Get(sEnvVar),,
                                    sToAdd, sSeparator, fNoEnd);

    /* If we're only to remove the sToAdd value, we're finised now. */
    if (fRM) then
        reutrn EnvVar_Set(0, sEnvVar, sOrgEnvVar);

    /* fNoEnd: remove separator at end (if any) and remove previously sToAdd. */
    if (fNoEnd & substr(sToAdd, length(sToAdd), 1) = sSeparator) then
        sToAdd = substr(sToAdd, 1, length(sToAdd) - 1);

    /* Add separator to sOrgEnvVar if fNoEnd and sOrgEnvVar is not empty */
    if (fNoEnd & length(sOrgEnvVar) > 0) then
        return EnvVar_Set(0, sEnvVar, sOrgEnvVar||sSeparator||sToAdd);
    return EnvVar_Set(0, sEnvVar, sOrgEnvVar||sToAdd);


/*
 * Removes a value for a separated enviroment string.
 */
_EnvVar_RemoveFrom: procedure
    parse arg sOrgValue, sValue, sSeparator, fNoEnd

    /* test for empty org value */
    if (sOrgValue = '') then
        return '';

    /* remove any separators in front or end of sValue. */
    if (substr(sValue, 1, 1) = sSeparator) then
        sValue = substr(sValue, 2);
    if (substr(sValue, length(sValue), 1) = sSeparator) then
        sValue = substr(sValue, 1, length(sValue) - 1);

    /* look for sValue */
    i = pos(translate(sValue), translate(sOrgValue));
    do while (i > 0)
        cch = length(sValue);
        fEnd = length(sOrgValue) <= cch + i;

        /* check if ok */
        if (i <= 1) then fOkFront = 1;
        else    fOkFront = (substr(sOrgValue, i - 1, 1) = sSeparator);
        if (fOkFront & (fEnd | substr(sOrgValue, i + cch, 1) = sSeparator)) then
        do
            /* addjust length / index to remove separators */
            if (i > 1)         then
            do
                cch = cch + 1;
                i = i - 1;
            end
            if (\fEnd & i = 1) then
                cch = cch + 1;

            /* create new sOrgValue */
            if (i > 1) then
                sOrgValue = substr(sOrgValue, 1, i - 1) || substr(sOrgValue, i + cch);
            else if (cch < length(sOrgValue)) then
                sOrgValue = substr(sOrgValue, i + cch);
            else
                sOrgValue = '';
        end
        else
            i = i + 1;

        /* more occurences? */
        j = i;
        i = pos(translate(sValue), translate(sOrgValue), i);
    end

    /* remove any start separator */
    do while (length(sOrgValue) > 0)
        if (substr(sOrgValue, 1, 1) <> sSeparator) then
            leave
        sOrgValue = substr(sOrgValue, 2);
    end

    /* Remove all separators at end */
    do while (length(sOrgValue) > 1)
        if (substr(sOrgValue, length(sOrgValue), 1) <> sSeparator) then
            leave;
        sOrgValue = substr(sOrgValue, 1, length(sOrgValue) - 1);
    end

    /* Just in case fix for value with only separator. (paranoia) */
    if (length(sOrgValue) = 1 & sOrgValue = sSeparator) then
        sOrgValue = '';

    /* Add end separator */
    if (length(sOrgValue) > 0 & \fNoEnd) then
        sOrgValue = sOrgValue || sSeparator;

    return sOrgValue;


/*
 * Sets sEnvVar to sValue.
 */
EnvVar_Set: procedure
    parse arg fRM, sEnvVar, sValue

    /* if we're to remove this, make valuestring empty! */
    if fRM then
        sValue = '';
    sEnvVar = translate(sEnvVar);

    /*
     * Begin/EndLibpath fix:
     *      We'll have to set internal these using both commandline 'SET'
     *      and internal VALUE in order to export it and to be able to
     *      get it (with EnvVar_Get) again.
     */
    if sEnvVar = 'BEGINLIBPATH' | sEnvVar = 'ENDLIBPATH' then
        'set' sEnvVar'='sValue;
    sRc = VALUE(sEnvVar, sValue, 'OS2ENVIRONMENT');
    return 0;

/*
 * Gets the value of sEnvVar.
 */
EnvVar_Get: procedure
    parse arg sEnvVar
    return value(sEnvVar,, 'OS2ENVIRONMENT');



