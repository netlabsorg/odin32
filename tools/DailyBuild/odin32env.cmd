/* $Id: odin32env.cmd,v 1.24 2002-04-11 16:27:03 bird Exp $
 *
 * Sets the build environment.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

parse arg sCompiler
    sCompiler = translate(sCompiler, 'abcdeghijklmnopqrstuvwxyz', 'ABCDEGHIJKLMNOPQRSTUVWXYZ');

    /*
     * To use this script you'll have to adjust some paths to match your local
     * installation. These paths are (mainly) called s<>Main or s<>Base, and usually
     * the only assignment statements in the configuration procedures below.
     *
     * In addition the CVSROOT, USER and HOME directories right below have to corrected.
     */
    call EnvVar_Set 0, 'CVSROOT',   ':pserver:bird@www.netlabs.org:/netlabs.cvs/odin32'
    call EnvVar_Set 0, 'HOME',      'e:/user/kso'
    call EnvVar_Set 0, 'USER',      'bird'
    call EnvVar_Set 0, 'HOSTNAME',  'Eniac'
    call EnvVar_Set 0, 'MULTIJOBS', '1'


    /*
     * Call the procedures which configure each tool.
     * (The order this is done is _very_ important!)
     */
    /*call Jade 0;
    call DocBookXML 0;*/
    call WarpIn 0;
    call mySQL 0;
    call Perl 0;
    call CVS 0;
    call DDKBase 0;
    call MSCV6 0;
    call EMX 0;
    if (sCompiler = 'watcom') then
    do /* watcom */
        say 'Watcom';
        call VAC308 1;
        call VAC365 1;
        call VAC40 1;
        call VAC365 0;
        call WatComC11c 0;
        call Toolkit40 0;
    end
    else if ((sCompiler = 'vac36') | (sCompiler = 'vac365')) then
    do /* vac36 */
        say 'VAC36(5)';
        call VAC308 1;
        call WatComC11c 1;
        call NetQOS2 0;
        call VAC40 1;
        call VAC365 0;
        call Toolkit40 0;
    end
    else if (sCompiler = 'vac40') then
    do /* vac40 */
        say 'VAC40';
        call VAC308 1;
        call WatComC11c 1;
        call NetQOS2 0;
        call VAC365 0;
        call Toolkit40 0;
        call VAC40 0;
    end
    else
    do /* default is vac30 */
        say 'VAC30';
        call VAC365 1;
        call WatComC11c 1;
        call VAC40 1;
        call VAC308 0;
        call Toolkit40 0;
    end

    call EnvVar_Set 0, 'DEBUG',      '1'
    call EnvVar_Set 0, 'BUILD_MODE', 'DEBUG'
    call EnvVar_Set 0, 'BUILD_PLATFORM', 'OS2'

    exit(0);


/**
 * The Directory Configuration Function.
 *
 * @returns Lower cased, absolute, backward slashed, path to program.
 * @param   sProgram    Program identifier. (lowercase!)
 *
 * @remark
 * Unless this causes a lot of trouble, we'll keep it in the script.
 * Having this externaly will slow down the script execution
 * somewhat I'm afraid. And the script is slow enought as it is.
 *
 */
QueryPath: procedure
    parse arg sProgram

    select
        when (sProgram = 'cvs') then        return 'f:\cvs';
        when (sProgram = 'emx') then        return 'f:\emx';
        when (sProgram = 'emxpgcc') then    return 'f:\emxpgcc';
        when (sProgram = 'db2v52') then     return 'f:\sqllib52';
        when (sProgram = 'icatgam') then    return 'f:\icatos2';
        when (sProgram = 'icatgam406rc1') then return 'f:\icatos2.4.0.6.rc1';
        when (sProgram = 'icatpe') then     return 'f:\icatpe';
        when (sProgram = 'ida38') then      return 'f:\ida38';
        when (sProgram = 'ida40') then      return 'f:\ida401';
        when (sProgram = 'ida414') then     return 'f:\ida414';
        when (sProgram = 'idasdk') then     return 'f:\idasdk';
        when (sProgram = 'ddkbase') then    return 'f:\ddk\base';
        when (sProgram = 'mscv6') then      return 'f:\ddktools\toolkits\msc60';
        when (sProgram = 'mysql') then      return 'f:\mysql2';
        when (sProgram = 'netqos2') then    return 'f:\netqos2';
        when (sProgram = 'perl') then       return 'f:\perllib';
        when (sProgram = 'python') then     return 'f:\python';
        when (sProgram = 'toolkit40') then  return 'f:\toolkit';
        when (sProgram = 'toolkit45') then  return 'f:\toolkit45';
        when (sProgram = 'unixroot') then   return 'e:\unix';
        when (sProgram = 'xfree86') then    return 'e:\xfree86';
        when (sProgram = 'vac308') then     return 'f:\ibmcpp';
        when (sProgram = 'vac365') then     return 'f:\ibmcxxo';
        when (sProgram = 'vac40') then      return 'f:\ibmcpp40';
        when (sProgram = 'warpin') then     return 'f:\warpin';
        when (sProgram = 'watcom11') then   return 'f:\watcom';
        when (sProgram = 'watcom11c') then  return 'f:\watcom11c';

        otherwise
        do
            say 'Fatal error: Path information for program '''sProgram''' was not found.';
            exit(16);
        end
    end

    /* Should never get here */
return '';



/**
 * Environment procedures section
 **/


/*
 * Concurrent Versions System (CVS)
 */
CVS: procedure
    parse arg fRM

    /*
     * Concurrent Versions System (CVS) main directory.
     */
    sPathCVS    = QueryPath('cvs');
    call EnvVar_Set      fRM, 'PATH_CVS',    sPathCVS;
    call EnvVar_AddFront fRM, 'path',        sPathCVS'\bin;'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathCVS'\book;'
return 0;


/*
 * EMX
 */
EMX: procedure
    parse arg fRM

    /*
     * EMX/GCC main directory.
     */
    sEMX        = QueryPath('emx');
    sEMXBack    = translate(sEMX, '\', '/');
    sEMXForw    = translate(sEMX, '/', '\');
    call EnvVar_Set      fRM, 'PATH_EMX', sEMXBack;
    call EnvVar_Set      fRM, 'CCENV',      'EMX'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'EMX'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

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
 * EMX PGCC - must be installed on to the ordinar EMX.
 */
EMXPGCC: procedure
    parse arg fRM

    /*
     * EMX/GCC main directory.
     */
    sEMXPGCC    = QueryPath('emxpgcc');
    sEMXBack    = translate(sEMXPGCC, '\', '/');
    sEMXForw    = translate(sEMXPGCC, '/', '\');
    call EnvVar_Set      fRM, 'PATH_EMXPGCC',   sEMXBack;
    call EnvVar_Set      fRM, 'CCENV',          'EMX'
    call EnvVar_Set      fRM, 'BUILD_ENV',      'EMX'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

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
    call EnvVar_AddFront fRM, 'INFOPATH',           sEMXForw'/info'
    call EnvVar_Set      fRM, 'EMXBOOK',            'emxdev.inf+emxlib.inf+emxgnu.inf+emxbsd.inf'
    call EnvVar_AddFront fRM, 'HELPNDX',            'emxbook.ndx', '+', 1

return 0;

/*
 * IBM DB2 v5.2
 */
db2v52: procedure
    parse arg fRM
    sPathDB2   = QueryPath('db2v52');
    call EnvVar_Set      fRm, 'PATH_DB2',    sPathDB2;
    call EnvVar_Set      fRm, 'db2path',     sPathDB2;
    call EnvVar_AddFront fRm, 'beginlibpath',sPathDB2'\dll;'sPathDB2'\alt;'
    call EnvVar_AddFront fRm, 'path',        sPathDB2'\bin;'sPathDB2'\alt;'
    call EnvVar_AddFront fRm, 'dpath',       sPathDB2'\bin;'sPathDB2';'
    call EnvVar_AddFront fRm, 'help',        sPathDB2'\help;'
    call EnvVar_AddEnd   fRm, 'classpath',   '.;'sPathDB2'\JAVA\DB2JAVA.ZIP;'sPathDB2'\JAVA\RUNTIME.ZIP;'sPathDB2'\JAVA\SQLJ.ZIP;'
    call EnvVar_Set      fRM, 'db2instace',  'DB2'
    /*call EnvVar_Set      fRM, 'odbc_path',   'f:\odbc' -- huh? what's this? */
    call EnvVar_AddFront fRM, 'cobcpy',      sPathDB2'\include\cobol_mf'
    call EnvVar_Set      fRM, 'finclude',    sPathDB2'\include'
    call EnvVar_AddFront fRM, 'include',     sPathDB2'\include;'
    call EnvVar_AddFront fRM, 'lib',         sPathDB2'\lib;'
return 0;


/*
 * ICAT Debugger
 */
ICATGam: procedure
    parse arg fRM
    sPathICAT   = QueryPath('icatgam');
    call EnvVar_Set      fRm, 'PATH_ICATGAM', sPathICAT;
    call EnvVar_AddFront fRm, 'beginlibpath',sPathICAT'\dll;'
    call EnvVar_AddFront fRm, 'path',        sPathICAT'\bin;'
    call EnvVar_AddFront fRm, 'dpath',       sPathICAT'\help;'
    call EnvVar_AddFront fRm, 'help',        sPathICAT'\help;'
return 0;


/*
 * ICAT Debugger
 */
ICATGam406RC1: procedure
    parse arg fRM
    sPathICAT   = QueryPath('icatgam406rc1');
    call EnvVar_Set      fRm, 'PATH_ICATGAM', sPathICAT;
    call EnvVar_AddFront fRm, 'beginlibpath',sPathICAT'\dll;'
    call EnvVar_AddFront fRm, 'path',        sPathICAT'\bin;'
    call EnvVar_AddFront fRm, 'dpath',       sPathICAT'\help;'
    call EnvVar_AddFront fRm, 'help',        sPathICAT'\help;'
return 0;



/*
 * ICAT Debugger for PE images.
 */
ICATPe: procedure
    parse arg fRM
    sPathICAT   = QueryPath('icatgam');
    sPathICATPe = QueryPath('icatpe');
    call EnvVar_Set      fRm, 'PATH_ICATGAM',sPathICAT;
    call EnvVar_Set      fRm, 'PATH_ICATPE', sPathICATPe;
    call EnvVar_AddFront fRm, 'beginlibpath',sPathICATPe'\bin;'sPathICAT'\dll;'
    call EnvVar_AddFront fRm, 'path',        sPathICATPe'\bin;'sPathICAT'\bin;'
    call EnvVar_AddFront fRm, 'dpath',       sPathICATPe'\bin;'sPathICAT'\help;'
    call EnvVar_AddFront fRm, 'help',        sPathICATPe'\bin;'sPathICAT'\help;'
return 0;



/*
 * Interactive Disassembler (IDA) v3.80a
 */
IDA38: procedure
    parse arg fRM
    /*
     * IDA main directory.
     */
    sPathIDA = QueryPath('ida38');
    call EnvVar_Set      fRM, 'PATH_IDA',       sPathIDA
    call EnvVar_AddFront fRM, 'path',           sPathIDA
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathIDA
return 0;


/*
 * Interactive Disassembler (IDA) v4.01
 */
IDA40: procedure
    parse arg fRM
    /*
     * IDA main directory.
     */
    sPathIDA = QueryPath('ida40');
    call EnvVar_Set      fRM, 'PATH_IDA',       sPathIDA
    call EnvVar_AddFront fRM, 'path',           sPathIDA
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathIDA
return 0;


/*
 * Interactive Disassembler (IDA) v4.14
 */
IDA414: procedure
    parse arg fRM
    /*
     * IDA main directory.
     */
    sPathIDA = QueryPath('ida414');
    call EnvVar_Set      fRM, 'PATH_IDA',       sPathIDA
    call EnvVar_AddFront fRM, 'path',           sPathIDA
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathIDA
return 0;


/*
 * Interactive Disassembler (IDA) Plugin SDK (v5.0?)
 */
IDASDK: procedure
    parse arg fRM
    /*
     * IDA main directory.
     */
    sPathIDASDK = QueryPath('idasdk');
    call EnvVar_Set      fRM, 'PATH_IDASDK',    sPathIDASDK
    call EnvVar_AddFront fRM, 'include',        sPathIDASDK'\include;'
    call EnvVar_AddFront fRM, 'lib',            sPathIDASDK'\libwat.os2;'
    call EnvVar_AddFront fRM, 'path',           sPathIDASDK'\bin\os2;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathIDASDK'\bin\os2;'
return 0;


/*
 *  Device Driver Kit (DDK) base.
 */
DDKBase: procedure
    parse arg fRM

    /*
     * Device Driver Kit (DDK) (v4.0+) base (important not main) directory.
     */
    sPathDDKBase    = QueryPath('ddkbase');
    call EnvVar_Set      fRM, 'PATH_DDKBASE',sPathDDKBase;
    call EnvVar_AddFront fRM, 'path',        sPathDDKBase'\tools;'
    call EnvVar_AddFront fRM, 'include',     sPathDDKBase'\h;'sPathDDKBase'\inc;'sPathDDKBase'\inc32;'
    call EnvVar_AddFront fRM, 'include16',   sPathDDKBase'\h;'
    call EnvVar_AddFront fRM, 'lib',         sPathDDKBase'\lib;'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathDDKBase'\..\docs;'

return 0;



/*
 * Microsoft C v6.0a
 */
MSCV6: procedure
    parse arg fRM

    /*
     * Microsoft C v6.0a main directory.
     */
    sPathMSC    = QueryPath('mscv6');
    call EnvVar_Set      fRM, 'BUILD_ENV',  'MSCV6'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'
    call EnvVar_Set      fRM, 'PATH_MSC',   sPathMSC;
    call EnvVar_AddFront fRM, 'path',       sPathMSC'\binp;'
    call EnvVar_AddFront fRM, 'include',    sPathMSC'\include;'
    call EnvVar_AddFront fRM, 'include16',  sPathMSC'\include;'
    call EnvVar_AddFront fRM, 'lib',        sPathMSC'\lib;'
return 0;



/*
 * mySQL Database system
 */
mySQL: procedure
    parse arg fRM

    /*
     * mySQL Database system main directory.
     */
    sPathMySQL    = QueryPath('mysql');
    call EnvVar_Set      fRM, 'PATH_MYSQL',     sPathMySQL;
    call EnvVar_AddFront fRM, 'path',           sPathMySQL'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathMySQL'\dll;'
    call EnvVar_AddFront fRM, 'include',        sPathMySQL'\include;'
    call EnvVar_AddFront fRM, 'bookshelf',      sPathMySQL'\doc;'sPathMySQL'\book';
    /*call EnvVar_AddFront fRM, 'lib',            sPathMySQL'\lib;'*/
return 0;



/*
 * NetQOS2 - help subsystem++ for VAC 3.6.5 and VAC 4.0
 */
NetQOS2: procedure
    sPathNetQOS2 = QueryPath('netqos2');
    call EnvVar_Set      fRM, 'PATH_NETQOS2',   sPathNetQOS2
    call EnvVar_AddFront fRM, 'path',           sPathNetQOS2';'
    call EnvVar_AddFront fRM, 'dpath',          sPathNetQOS2';'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathNetQOS2';'
    call EnvVar_Set      fRM, 'imndatasrv',     sPathNetQOS2'\DATA'
    call EnvVar_Set      fRM, 'imndatacl',      sPathNetQOS2'\DATA'
    call EnvVar_Set      fRM, 'imnworksrv',     sPathNetQOS2'\WORK'
    call EnvVar_Set      fRM, 'imnworkcl',      sPathNetQOS2'\WORK'
    call EnvVar_Set      fRM, 'imnnlpssrv',     sPathNetQOS2
    call EnvVar_Set      fRM, 'imnnlpscl',      sPathNetQOS2
    call EnvVar_Set      fRM, 'imncccfgfile',   'NETQ.CFG'
    call EnvVar_Set      fRM, 'imncscfgfile',   'NETQ.CFG'
    call EnvVar_Set      fRM, 'imqconfigsrv',   sPathNetQOS2'\instance'
    call EnvVar_Set      fRM, 'imqconfigcl',    sPathNetQOS2'\instance\dbcshelp'
return 0;



/*
 * PERL 5005_53
 */
Perl: procedure
    parse arg fRM

    /*
     * Perl main directory.
     */
/* BAD
    sPathPerl       = QueryPath('perl');
    sPathPerlForw   = translate(sPathPerl, '/', '\');
    call EnvVar_Set      fRM, 'PATH_PERL',      sPathPerl;
    call EnvVar_AddFront fRM, 'path',           sPathPerl'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathPerl'\dll;'
    call EnvVar_AddEnd   fRM, 'bookshelf',      sPathPerl'\book;'
    call EnvVar_Set      fRM, 'perllib_prefix', sPathPerlForw'/lib;'sPathPerl'\lib'
    call EnvVar_Set      fRM, 'perl5lib',       sPathPerlForw'/lib/site_perl/5.00553/os2;'sPathPerlForw'/lib/site_perl/5.00553'
    call EnvVar_Set      fRM, 'perl5load',      '2'
    call EnvVar_Set      fRM, 'perl_sh_dir',    sPathPerlForw'/bin_sh/sh.exe'
    call EnvVar_Set      fRM, 'manpath',        sPathPerlForw'/man'
    call EnvVar_Set      fRM, 'perl_badlang',   '0'
/*    call EnvVar_Set      fRM, 'LANG',           'en_US' /* dirty fix... */*/
*/
/* seems ok */
    sPathPerl       = QueryPath('perl');
    sPathPerlForw   = translate(sPathPerl, '/', '\');
    call EnvVar_Set      fRM, 'PATH_PERL',      sPathPerl;
    call EnvVar_AddFront fRM, 'path',           sPathPerl'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathPerl'\dll;'
    call EnvVar_AddEnd   fRM, 'bookshelf',      sPathPerl'\book;'
    call EnvVar_Set      fRM, 'perllib_prefix', sPathPerlForw'/lib;'sPathPerlForw'/lib'
    call EnvVar_Set      fRM, 'perl_sh_dir',    sPathPerlForw'/bin_sh'
    call EnvVar_Set      fRM, 'manpath',        sPathPerlForw'/man'
    call EnvVar_Set      fRM, 'perl5lib',       sPathPerlForw'/lib'
    call EnvVar_Set      fRM, 'perl_badlang',   '0'
return 0;


/*
 * Python/2 v1.5.2
 */
Python: procedure
    parse arg fRM

    /*
     * The Python Home directory
     */
    sPythonHome = QueryPath('python');
    call EnvVar_Set      fRM, 'PATH_PYTHON',    sPythonHome
    call EnvVar_Set      fRM, 'pythonhome',     sPythonHome
    call EnvVar_Set      fRM, 'pythonpath',     '.;'sPythonHome'\Lib;'sPythonHome'\Lib\plat-win;'sPythonHome'\Lib\lib-tk;'sPythonHome'\Lib\lib-dynload;'sPythonHome'\Lib\site-packages;'sPythonHome'\Lib\dos-8x3'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPythonHome
    call EnvVar_AddFront fRM, 'path',           sPythonHome

return 0;


/*
 * OS/2 Programmers Toolkit v4.0 (CSD1/4)
 */
Toolkit40: procedure
    parse arg fRM

    /*
     * Toolkit (4.0) main directory.
     */
    sPathTK    = QueryPath('toolkit40');
    call EnvVar_Set      fRM, 'PATH_TOOLKIT',  sPathTK;

    call EnvVar_AddFront fRM, 'beginlibpath',   sPathTK'\BETA\DLL;'sPathTK'\SAMPLES\MM\DLL;'sPathTK'\SAMPLES\OPENDOC\PARTS\DLL;'sPathTK'\SOM\COMMON\DLL;'sPathTK'\SOM\LIB;'sPathTK'\OPENDOC\BASE\DLL;'sPathTK'\OPENDOC\BASE\LOCALE\EN_US;'sPathTK'\DLL;'
    call EnvVar_AddFront fRM, 'path',           sPathTK'\BETA\BIN;'sPathTK'\SOM\COMMON;'sPathTK'\SOM\BIN;.;'sPathTK'\OPENDOC\BASE\BIN;'sPathTK'\BIN;'
    call EnvVar_AddFront fRM, 'dpath',          sPathTK'\SOM\COMMON\SYSTEM;'sPathTK'\SOM\MSG;'sPathTK'\OPENDOC\BASE\MSG;'sPathTK'\BOOK;'sPathTK'\MSG;'
    call EnvVar_AddFront fRM, 'help',           sPathTK'\BETA\HELP;'sPathTK'\OPENDOC\BASE\LOCALE\EN_US;'sPathTK'\HELP;'
    call EnvVar_AddFront fRM, 'bookshelf',      sPathTK'\BETA\BOOK;'sPathTK'\BOOK;'sPathTK'\ARCHIVED;'
    call EnvVar_AddFront fRM, 'somir',          sPathTK'\SOM\COMMON\ETC\214\SOM.IR;'sPathTK'\OPENDOC\BASE\AVLSHELL.IR;'
    call EnvVar_AddEnd   fRM, 'somir',          sPathTK'\OPENDOC\CUSTOM\OD.IR;'sPathTK'\SAMPLES\REXX\SOM\ANIMAL\ORXSMP.IR;'
    call EnvVar_AddFront fRM, 'include',        sPathTK'\SPEECH\H;'sPathTK'\BETA\H;'sPathTK'\SAMPLES\OPENDOC\PARTS\INCLUDE;'sPathTK'\SOM\INCLUDE;'sPathTK'\INC;'sPathTK'\H\GL;'sPathTK'\H;'
    call EnvVar_AddEnd   fRM, 'include',        sPathTK'\H\LIBC;.;'
    call EnvVar_AddFront fRM, 'lib',            sPathTK'\SPEECH\LIB;'sPathTK'\SAMPLES\MM\LIB;'sPathTK'\LIB;'sPathTK'\SOM\LIB;'sPathTK'\OPENDOC\BASE\LIB;'
    call EnvVar_AddFront fRM, 'nlspath',        sPathTK'\OPENDOC\BASE\LOCALE\EN_US\%N;'sPathTK'\MSG\%N;C:\MPTN\MSG\NLS\%N;C:\TCPIP\msg\ENUS850\%N;'
    call EnvVar_AddFront fRM, 'locpath',        sPathTK'\OPENDOC\BASE\LOCALE;'
    call EnvVar_AddFront fRM, 'ipfc',           sPathTK'\IPFC;'
    call EnvVar_Set      fRM, 'odbase',         sPathTK'\OPENDOC\BASE'
    call EnvVar_Set      fRM, 'odlang',         'en_US'
    call EnvVar_AddFront fRM, 'odbasepaths',    sPathTK'\OPENDOC\BASE;'
    call EnvVar_Set      fRM, 'odcfg',          sPathTK'\OPENDOC\CUSTOM'
    call EnvVar_Set      fRM, 'odtmp',          EnvVar_Get('tmp');
    call EnvVar_Set      fRM, 'sombase',        sPathTK'\SOM'
    call EnvVar_Set      fRM, 'somruntime',     sPathTK'\SOM\COMMON'

    call EnvVar_Set      fRM, 'cpref',          'CP1.INF+CP2.INF+CP3.INF'
    call EnvVar_Set      fRM, 'gpiref',         'GPI1.INF+GPI2.INF+GPI3.INF+GPI4.INF'
    call EnvVar_Set      fRM, 'mmref',          'MMREF1.INF+MMREF2.INF+MMREF3.INF'
    call EnvVar_Set      fRM, 'pmref',          'PM1.INF+PM2.INF+PM3.INF+PM4.INF+PM5.INF'
    call EnvVar_Set      fRM, 'wpsref',         'WPS1.INF+WPS2.INF+WPS3.INF'
    call EnvVar_AddFront fRM, 'sminclude',      sPathTK'\H;'sPathTK'\IDL;'sPathTK'\SOM\INCLUDE;.;'sPathTK'\OPENDOC\BASE\INCLUDE;'sPathTK'\SAMPLES\OPENDOC\PARTS\INCLUDE;'
    call EnvVar_Set      fRM, 'smaddstar',      '1'
    call EnvVar_Set      fRM, 'smemit',         'h;ih;c'
    call EnvVar_Set      fRM, 'smtmp',          EnvVar_Get('tmp');
    call EnvVar_Set      fRM, 'smclasses',      'WPTYPES.IDL'
    call EnvVar_Set      fRM, 'odparts',        sPathTK'\SAMPLES\OPENDOC\PARTS'
    call EnvVar_Set      fRM, 'odsrc',          sPathTK'\SAMPLES\OPENDOC\PARTS'
    call EnvVar_AddFront fRM, 'odpartspaths',   sPathTK'\SAMPLES\OPENDOC\PARTS;'
    call EnvVar_AddFront fRM, 'odsrcpaths',     sPathTK'\SAMPLES\OPENDOC\PARTS;'
    /*
    call EnvVar_Set      fRM, 'CAT_MACHINE=COM1:57600'
    call EnvVar_Set      fRM, 'CAT_HOST_BIN_PATH='sPathTK'\BIN'
    call EnvVar_Set      fRM, 'CAT_COMMUNICATION_TYPE=ASYNC_SIGBRK'
    call EnvVar_AddFront fRM, 'CAT_HOST_SOURCE_PATH='sPathTK'\BIN;'
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
     sPathTK    = QueryPath('toolkit45');
    call EnvVar_Set      fRM, 'PATH_TOOLKIT',  sPathTK;

    call EnvVar_AddFront fRM, 'path',        sPathTK'\bin;'
    call EnvVar_AddFront fRM, 'dpath',       sPathTK'\book;'
    call EnvVar_AddFront fRM, 'dpath',       sPathTK'\msg;'
    call EnvVar_AddFront fRM, 'beginlibpath', sPathTK'\dll;'
    call EnvVar_AddFront fRM, 'help',        sPathTK'\help;'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathTK'\archived;'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathTK'\book;'
    call EnvVar_AddFront fRM, 'nlspath',     sPathTK'\msg\%N;'
    call EnvVar_AddEnd   fRM, 'ulspath',     sPathTK'\language;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\H;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\H\GL;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\SPEECH\H;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\H\RPC;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\H\NETNB;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\H\NETINET;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\H\NET;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\H\ARPA;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\INC;'
    call EnvVar_AddEnd   fRM, 'lib',         sPathTK'\SAMPLES\MM\LIB;'
    call EnvVar_AddEnd   fRM, 'lib',         sPathTK'\SPEECH\LIB;'
    call EnvVar_AddFront fRM, 'lib',         sPathTK'\lib;'
    call EnvVar_AddFront fRM, 'helpndx',     'EPMKWHLP.NDX+', '+'
    call EnvVar_AddFront fRM, 'ipfc',        sPathTK'\ipfc;'
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


/**
 * This will envolve into an full UNIX like environment some day perhaps...
 */
Unix: procedure
    parse arg fRM

    /*
     * Unix root directory.
     */
    sUnixBack = QueryPath('unixroot');
    sUnixForw = translate(sUnixBack, '/', '\');
    call EnvVar_Set      fRM, 'PATH_UNIX',          sUnixBack
    call EnvVar_Set      fRM, 'unixroot',           sUnixBack
    call EnvVar_AddFront fRM, 'path',               sUnixBack'\bin;'sUnixBack'\usr\local\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',       sUnixBack'\dll;'
    call EnvVar_Set      fRM, 'groff_font_path',    sUnixForw'/lib/groff/font'
    call EnvVar_Set      fRM, 'groff_tmac_path',    sUnixForw'/lib/groff/tmac'
    call EnvVar_Set      fRM, 'refer',              sUnixForw'/lib/groff/dict/papers/ind'

    call EnvVar_Set      fRM, 'editor',             'e:/verkty/boxer/b2.exe -Vr25'

    /*
     * XFree86 main directory.
     */
    sXF86Back = QueryPath('xfree86');
    sXF86Forw = translate(sXF86Back, '/', '\');
    call EnvVar_Set      fRM, 'PATH_XFREE86',       sXF86Back
    call EnvVar_AddFront fRM, 'C_INCLUDE_PATH',     sXF86Forw'/include'
    call EnvVar_AddFront fRM, 'CPLUS_INCLUDE_PATH', sXF86Forw'/include'
    call EnvVar_Set      fRM, 'OBJC_INCLUDE_PATH',  sXF86Forw'/include'
    call EnvVar_AddFront fRM, 'LIBRARY_PATH',       sXF86Forw'/lib'

return EMX(fRM);



/*
 * IBM Visual Age for C++ v3.08 for OS/2
 */
VAC308: procedure
    parse arg fRM

    /*
     * IBM Visual Age for C++ Version 3.08 main directory.
     */
    sPathCPP = QueryPath('vac308');
    call EnvVar_Set      fRM, 'PATH_VAC308',    sPathCPP
    call EnvVar_Set      fRM, 'CCENV',      'VAC3'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'VAC308'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_AddFront fRM, 'beginlibpath',   sPathCPP'\DLL;'sPathCPP'\SAMPLES\TOOLKIT\DLL;'
    call EnvVar_AddFront fRM, 'path',           sPathCPP'\BIN;'sPathCPP'\SMARTS\SCRIPTS;'sPathCPP'\HELP;'
    call EnvVar_AddFront fRM, 'dpath',          sPathCPP'\HELP;'sPathCPP';'sPathCPP'\LOCALE;'sPathCPP'\MACROS;'sPathCPP'\BND;'
    call EnvVar_AddFront fRM, 'help',           sPathCPP'\HELP;'sPathCPP'\SAMPLES\TOOLKIT\HELP;'
    call EnvVar_AddFront fRM, 'bookshelf',      sPathCPP'\HELP;'
    call EnvVar_AddFront fRM, 'somir',          sPathCPP'\ETC\SOM.IR;'
    call EnvVar_AddFront fRM, 'cpphelp_ini',    'C:\OS2\SYSTEM'
    call EnvVar_AddFront fRM, 'locpath',        sPathCPP'\LOCALE;%LOCPATH%;'
    call EnvVar_AddFront fRM, 'include',        sPathCPP'\INCLUDE;'sPathCPP'\INCLUDE\OS2;'sPathCPP'\INC;'sPathCPP'\INCLUDE\SOM;'
    call EnvVar_AddFront fRM, 'sminclude',      sPathCPP'\INCLUDE\OS2;'sPathCPP'\INCLUDE;'sPathCPP'\INCLUDE\SOM;'

    call EnvVar_AddFront fRM, 'vbpath',         '.;'sPathCPP'\DDE4VB;'
    call EnvVar_Set      fRM, 'tmpdir',         EnvVar_Get('tmp')
    call EnvVar_Set      fRM, 'lxevfref',       'EVFELREF.INF+LPXCREF.INF'
    call EnvVar_Set      fRM, 'lxevfhdi',       'EVFELHDI.INF+LPEXHDI.INF'
    call EnvVar_AddFront fRM, 'lpath',          sPathCPP'\MACROS;'
    call EnvVar_AddFront fRM, 'codelpath',      sPathCPP'\CODE\MACROS;'sPathCPP'\MACROS;'
    call EnvVar_Set      fRM, 'clref',          'CPPCLRF.INF+CPPDAT.INF+CPPAPP.INF+CPPWIN.INF+CPPCTL.INF+CPPADV.INF+CPP2DG.INF+CPPDDE.INF+CPPDM.INF+CPPMM.INF+CPPCLRB.INF'
    call EnvVar_AddFront fRM, 'ipfc',           sPathCPP'\IPFC'
    call EnvVar_AddFront fRM, 'lib',            sPathCPP'\LIB;'sPathCPP'\DLL;'
    call EnvVar_Set      fRM, 'cpplocal',       sPathCPP
    call EnvVar_Set      fRM, 'cppmain',        sPathCPP
    call EnvVar_Set      fRM, 'cppwork',        sPathCPP
    call EnvVar_Set      fRM, 'iwf.default_prj','CPPDFTPRJ'

    call EnvVar_Set      fRM, 'iwf.solution_lang_support', 'CPPIBS30;ENG'
    call EnvVar_Set      fRM, 'vacpp_shared'    'FALSE'
    call EnvVar_Set      fRM, 'iwfhelp',        'IWFHDI.INF'
    call EnvVar_Set      fRM, 'iwfopt',         sPathCPP

    call EnvVar_Set      fRM, 'somruntime',     sPathCPP'\DLL'
    call EnvVar_Set      fRM, 'smaddstar',      '1'
    call EnvVar_Set      fRM, 'smemit',         'h;ih;c'
    call EnvVar_Set      fRM, 'sombase',        sPathCPP
    call EnvVar_Set      fRM, 'smtmp',          EnvVar_Get('tmp')
    call EnvVar_Set      fRM, 'smclasses',      'WPTYPES.IDL'

    /* FIXME THESE USES '+' AS SEPERATOR NOT ';'!!! */
    call EnvVar_AddFront fRM, 'helpndx',        'EPMKWHLP.NDX+CPP.NDX+CPPBRS.NDX', '+'
    call EnvVar_AddFront fRM, 'ipf_keys',       'SHOWNAV'


return 0;



/*
 * Visual Age / C and C++ tools v3.6.5 for OS/2
 */
VAC365: procedure
    parse arg fRM

    /*
     * IBM C/C++ Compiler and Tools Version 3.6.5 main directory.
     */
    sPathCxx    = QueryPath('vac365');
    call EnvVar_Set      fRM, 'PATH_VAC365', sPathCxx;
    call EnvVar_Set      fRM, 'CCENV',      'VAC36'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'VAC365'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_Set      fRM, 'cxxmain',     sPathCxx;
    call EnvVar_AddFront fRM, 'path',        sPathCxx'\bin;'
    call EnvVar_AddFront fRM, 'dpath',       sPathCxx'\local;'sPathCxx'\help;'
    call EnvVar_AddFront fRM, 'beginlibpath',sPathCxx'\dll;'sPathCxx'\runtime;'
    call EnvVar_AddFront fRM, 'nlspath',     sPathCxx'\msg\%N;'
    call EnvVar_AddFront fRM, 'include',     sPathCxx'\include;'
    call EnvVar_AddFront fRM, 'lib',         sPathCxx'\lib;'
    call EnvVar_AddFront fRM, 'ipfc',        sPathCxx'\ipfc;'
    call EnvVar_Set      fRM, 'LANG',        'en_us'
    call EnvVar_Set      fRM, 'CPP_DBG_LANG', 'CPP'

return 0;


/*
 * Visual Age for C++ v4.0 for OS/2
 */
VAC40: procedure
    parse arg fRM

    /*
     * IBM VisualAge for C++ v4.0 main directory.
     */
    sPathCPP    = QueryPath('vac40');
    call EnvVar_Set      fRM, 'PATH_VAC40',  sPathCPP;
    call EnvVar_Set      fRM, 'CCENV',      'VAC40'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'VAC40'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_AddFront fRM, 'path',        sPathCPP'\bin;'
    call EnvVar_AddFront fRM, 'dpath',       sPathCPP'\etc;'sPathCPP'\help;'
    call EnvVar_AddFront fRM, 'beginlibpath',sPathCPP'\dll;'sPathCPP'\runtime;'
    call EnvVar_AddFront fRM, 'help',        sPathCPP'\help;'
    call EnvVar_AddFront fRM, 'nlspath',     sPathCPP'\msg\%N;'
    call EnvVar_AddFront fRM, 'locpath',     sPathCPP'\locale;'sPathCPP'\runtime\locale;'
    call EnvVar_AddFront fRM, 'include',     sPathCPP'\ivb;'sPathCPP'\include;'
    call EnvVar_AddFront fRM, 'lib',         sPathCPP'\lib;'
    call EnvVar_AddFront fRM, 'ipfc',        sPathCPP'\bin;'
    call EnvVar_AddFront fRM, 'cpplpath4',   sPathCPP'\macros;'
    call EnvVar_Set      fRM, 'system_ice',  sPathCPP'\etc\system.ice'
    call EnvVar_Set      fRM, 'vbpath',      sPathCPP'\ivb'
    call EnvVar_Set      fRM, 'vacppmain',   sPathCPP;
    call EnvVar_Set      fRM, 'os2',         '1'

return 0;




/*
 * WarpIn
 */
WarpIn: procedure
    parse arg fRM

    /*
     * WarpIn main directory.
     */
    sPathWarpIn = QueryPath('warpin');
    call EnvVar_Set      fRM, 'PATH_WARPIN', sPathWarpIn;
    call EnvVar_AddFront fRM, 'path',        sPathWarpIn';'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathWarpIn';'
return 0;



/*
 * WatCom C/C++ v11.0
 */
WatComC11: procedure
    parse arg fRM

    /*
     * Watcom C/C++ v11.0 main directory
     */
    sPathWatcom = QueryPath('watcom11');
    call EnvVar_Set      fRM, 'PATH_WATCOM', sPathWatcom
    call EnvVar_Set      fRM, 'CCENV',      'WAT'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'WAT11'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_Set      fRM, 'watcom',      sPathWatcom
    call EnvVar_AddFront fRM, 'path',        sPathWatcom'\binp;'sPathWatcom'\binw;'
    call EnvVar_AddFront fRM, 'beginlibpath',sPathWatcom'\binp\dll;'
    call EnvVar_AddFront fRM, 'help',        sPathWatcom'\binp\help;'
    call EnvVar_AddEnd   fRM, 'bookshelf',   sPathWatcom'\binp\help;'
    call EnvVar_AddFront fRM, 'include',     sPathWatcom'\h;'sPathWatcom'\h\os2;'sPathWatcom'\h\nt;'
    call EnvVar_AddFront fRM, 'lib',         sPathWatcom'\lib386;'sPathWatcom'\lib386\os2;'sPathWatcom'\lib286;'sPathWatcom'\lib286\os2;'
    call EnvVar_Set      fRM, 'edpath',      sPathWatcom'EDDAT;'
    /*
    rem detach %watcom%\BINP\BATSERV.EXE
    rem detach %watcom%\BINP\NMPBIND.EXE
    */

return 0;

/*
 * WatCom C/C++ v11.0c
 */
WatComC11c: procedure
    parse arg fRM

    /*
     * Watcom C/C++ v11.0c main directory
     */
    sPathWatcom = QueryPath('watcom11c');
    call EnvVar_Set      fRM, 'PATH_WATCOM', sPathWatcom
    call EnvVar_Set      fRM, 'CCENV',      'WAT'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'WAT11'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_Set      fRM, 'watcom',      sPathWatcom
    call EnvVar_AddFront fRM, 'path',        sPathWatcom'\binp;'sPathWatcom'\binw;'
    call EnvVar_AddFront fRM, 'beginlibpath',sPathWatcom'\binp\dll;'
    call EnvVar_AddFront fRM, 'help',        sPathWatcom'\binp\help;'
    call EnvVar_AddEnd   fRM, 'bookshelf',   sPathWatcom'\binp\help;'
    call EnvVar_AddFront fRM, 'include',     sPathWatcom'\h;'sPathWatcom'\h\os2;'sPathWatcom'\h\nt;'
    call EnvVar_AddFront fRM, 'lib',         sPathWatcom'\lib386;'sPathWatcom'\lib386\os2;'sPathWatcom'\lib286;'sPathWatcom'\lib286\os2;'
    call EnvVar_Set      fRM, 'edpath',      sPathWatcom'EDDAT;'
    /*
    rem detach %watcom%\BINP\BATSERV.EXE
    rem detach %watcom%\BINP\NMPBIND.EXE
    */

return 0;













/**
 * Helper Procedure section
 **/


/**
 * Add sToAdd in front of sEnvVar.
 * Note: sToAdd now is allowed to be alist!
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvVar_addFront: procedure
    parse arg fRM, sEnvVar, sToAdd, sSeparator

    /* sets default separator if not specified. */
    if (sSeparator = '') then sSeparator = ';';

    /* checks that sToAdd ends with an ';'. Adds one if not. */
    if (substr(sToAdd, length(sToAdd), 1) <> sSeparator) then
        sToAdd = sToAdd || sSeparator;

    /* check and evt. remove ';' at start of sToAdd */
    if (substr(sToAdd, 1, 1) = ';') then
        sToAdd = substr(sToAdd, 2);

    /* loop thru sToAdd */
    rc = 0;
    i = length(sToAdd);
    do while i > 1 & rc = 0
        j = lastpos(sSeparator, sToAdd, i-1);
        rc = EnvVar_AddFront2(fRM, sEnvVar, substr(sToAdd, j+1, i - j), sSeparator);
        i = j;
    end

return rc;

/**
 * Add sToAdd in front of sEnvVar.
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvVar_AddFront2: procedure
    parse arg fRM, sEnvVar, sToAdd, sSeparator

    /* sets default separator if not specified. */
    if (sSeparator = '') then sSeparator = ';';

    /* checks that sToAdd ends with a separator. Adds one if not. */
    if (substr(sToAdd, length(sToAdd), 1) <> sSeparator) then
        sToAdd = sToAdd || sSeparator;

    /* check and evt. remove the separator at start of sToAdd */
    if (substr(sToAdd, 1, 1) = sSeparator) then
        sToAdd = substr(sToAdd, 2);

    /* Get original variable value */
    sOrgEnvVar = EnvVar_Get(sEnvVar);

    /* Remove previously sToAdd if exists. (Changing sOrgEnvVar). */
    i = pos(translate(sToAdd), translate(sOrgEnvVar));
    if (i > 0) then
        sOrgEnvVar = substr(sOrgEnvVar, 1, i-1) || substr(sOrgEnvVar, i + length(sToAdd));

    /* set environment */
    if (fRM) then
        return EnvVar_Set(0, sEnvVar, sOrgEnvVar);
return EnvVar_Set(0, sEnvVar, sToAdd||sOrgEnvVar);


/**
 * Add sToAdd as the end of sEnvVar.
 * Note: sToAdd now is allowed to be alist!
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvVar_addEnd: procedure
    parse arg fRM, sEnvVar, sToAdd, sSeparator

    /* sets default separator if not specified. */
    if (sSeparator = '') then sSeparator = ';';

    /* checks that sToAdd ends with a separator. Adds one if not. */
    if (substr(sToAdd, length(sToAdd), 1) <> sSeparator) then
        sToAdd = sToAdd || sSeparator;

    /* check and evt. remove ';' at start of sToAdd */
    if (substr(sToAdd, 1, 1) = sSeparator) then
        sToAdd = substr(sToAdd, 2);

    /* loop thru sToAdd */
    rc = 0;
    i = length(sToAdd);
    do while i > 1 & rc = 0
        j = lastpos(sSeparator, sToAdd, i-1);
        rc = EnvVar_AddEnd2(fRM, sEnvVar, substr(sToAdd, j+1, i - j), sSeparator);
        i = j;
    end

return rc;

/**
 * Add sToAdd as the end of sEnvVar.
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvVar_AddEnd2: procedure
    parse arg fRM, sEnvVar, sToAdd, sSeparator

    /* sets default separator if not specified. */
    if (sSeparator = '') then sSeparator = ';';

    /* checks that sToAdd ends with a separator. Adds one if not. */
    if (substr(sToAdd, length(sToAdd), 1) <> sSeparator) then
        sToAdd = sToAdd || sSeparator;

    /* check and evt. remove separator at start of sToAdd */
    if (substr(sToAdd, 1, 1) = sSeparator) then
        sToAdd = substr(sToAdd, 2);

    /* Get original variable value */
    sOrgEnvVar = EnvVar_Get(sEnvVar);

    /* Remove previously sToAdd if exists. (Changing sOrgEnvVar). */
    i = pos(translate(sToAdd), translate(sOrgEnvVar));
    if (i > 0) then
        sOrgEnvVar = substr(sOrgEnvVar, 1, i-1) || substr(sOrgEnvVar, i + length(sToAdd));

    /* checks that sOrgEnvVar ends with a separator. Adds one if not. */
    if (substr(sOrgEnvVar, length(sOrgEnvVar), 1) <> sSeparator) then
        sOrgEnvVar = sOrgEnvVar || sSeparator;

    /* set environment */
    if (fRM) then return EnvVar_Set(0, sEnvVar, sOrgEnvVar);
return EnvVar_Set(0, sEnvVar, sOrgEnvVar||sToAdd);


/**
 * Sets sEnvVar to sValue.
 */
EnvVar_Set: procedure
    parse arg fRM, sEnvVar, sValue

    /* if we're to remove this, make valuestring empty! */
    if (fRM) then
        sValue = '';
    sEnvVar = translate(sEnvVar);

    /*
     * Begin/EndLibpath fix:
     *      We'll have to set internal these using both commandline 'SET'
     *      and internal VALUE in order to export it and to be able to
     *      get it (with EnvVar_Get) again.
     */
    if ((sEnvVar = 'BEGINLIBPATH') | (sEnvVar = 'ENDLIBPATH')) then
    do
        if (length(sValue) >= 1024) then
            say 'Warning: 'sEnvVar' is too long,' length(sValue)' char.';
        return SysSetExtLibPath(sValue, substr(sEnvVar, 1, 1));
    end

    if (length(sValue) >= 1024) then
    do
        say 'Warning: 'sEnvVar' is too long,' length(sValue)' char.';
        say '    This may make CMD.EXE unstable after a SET operation to print the environment.';
    end
    sRc = VALUE(sEnvVar, sValue, 'OS2ENVIRONMENT');
return 0;

/**
 * Gets the value of sEnvVar.
 */
EnvVar_Get: procedure
    parse arg sEnvVar
    if ((translate(sEnvVar) = 'BEGINLIBPATH') | (translate(sEnvVar) = 'ENDLIBPATH')) then
        return SysQueryExtLibPath(substr(sEnvVar, 1, 1));
return value(sEnvVar,, 'OS2ENVIRONMENT');


/**
 *  Workaround for bug in CMD.EXE.
 *  It messes up when REXX have expanded the environment.
 */
FixCMDEnv: procedure
    /* check for 4OS2 first */
    Address CMD 'set 4os2test_env=%@eval[2 + 2]';
    if (value('4os2test_env',, 'OS2ENVIRONMENT') = '4') then
        return 0;

    /* force environment expansion by setting a lot of variables and freeing them. */
    do i = 1 to 100
        Address CMD 'set dummyenvvar'||i'=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    end
    do i = 1 to 100
        Address CMD 'set dummyenvvar'||i'=';
    end
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



