/* $Id: buildenv.cmd,v 1.1 2002-04-07 16:29:10 bird Exp $
 *
 * This is the master environment script. It contains settings for many
 * enviroment configurations. Environments may be set and unset
 * interchangably.
 *
 *
 * Note:    Of historic reasons, there are some environments here which
 *          isn't normally used by normal code trees.
 *
 *
 * Change the QueryPath function to fit you local environment.
 * Change the QueryPath function to fit you local environment.
 * Change the QueryPath function to fit you local environment.
 *
 *
 * Known problems:
 *     - LANG is set to en_US by both VAC36 and TOOLKIT45. When unsetting
 *       those the original value, for example of no_NO, is not restored.
 */


    /*
     * Create argument array with lowercase arguments.
     */
    parse arg sEnv.1 sEnv.2 sEnv.3 sEnv.4 sEnv.5 sEnv.6 sEnv.7 sEnv.8 sEnv.9 sEnv.10 sEnv.11 sEnv.12 sEnv.13 sEnv.14 sEnv.15 sEnv.16 sEnv.17 sEnv.18 sEnv.19 sEnv.20 sEnv.21

    i = 1;
    do while (sEnv.i <> '')
        sEnv.i = translate(strip(sEnv.i), 'abcdefghijklmnopqrstuvwxyzëõÜ', 'ABCDEFGHIJKLMNOPQRSTUVWXYZíùè');
        i = i + 1;
    end
    sEnv.0 = i - 1;


    /*
     * Load REXX Util Functions.
     * (Need Sys[Query|Set]ExtLibPath.)
     */
    if (RxFuncQuery('SysLoadFuncs') = 1) then
    do
        call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
        call SysLoadFuncs;
    end


    /*
     * Apply CMD.EXE workaround.
     */
    call FixCMDEnv;


    /*
     * Parse arguments
     */
    do i = 1 to sEnv.0
        /* uses dash to mark end of arguments */
        if (sEnv.i = '-') then
            leave;

        /*
         * Get last char.
         * Dash means remove, pluss means add.
         * Pluss is default and optional.
         */
        ch = substr(sEnv.i, length(sEnv.i), 1);
        if ch = '-' then
            fRM = 1;
        else
            fRM = 0;
        if (ch = '-' | ch = '+') then
            sEnv.i = substr(sEnv.i, 1, length(sEnv.i) - 1);

        /*
         * do the switch.
         */
        rc = 0;
        select
            when (sEnv.i = 'mode8050') then     Address CMD 'mode 80,50'
            when (sEnv.i = 'mode12050') then    Address CMD 'mode 120,50'
            when (sEnv.i = 'cvs') then          rc = CVS(fRM);
            when (sEnv.i = 'db2v52') then       rc = db2v52(fRM);
            when (sEnv.i = 'debug') then do
                rc = EnvVar_Set(0, 'DEBUG','1');
                rc = EnvVar_Set(0, 'RELEASE','');
                rc = EnvVar_Set(0, 'BUILD_MODE','DEBUG');
            end
            when (sEnv.i = 'ddkbase') then      rc = DDKBase(fRM);
            when (sEnv.i = 'emx') then          rc = EMX(fRM);
            when (sEnv.i = 'emxpgcc') then      rc = EMXPGCC(fRM);
            when (sEnv.i = 'icatgam') then      rc = ICATGam(fRM);
            when (sEnv.i = 'icatpe') then       rc = ICATPe(fRM);
            when (sEnv.i = 'ida') then          rc = IDA414(fRM);
            when (sEnv.i = 'ida414') then       rc = IDA414(fRM);
            when (sEnv.i = 'ida38') then        rc = IDA38(fRM);
            when (sEnv.i = 'ida40') then        rc = IDA40(fRM);
            when (sEnv.i = 'idasdk') then       rc = IDASDK(fRM);
            when (sEnv.i = 'msc60') then        rc = MSC60(fRM);
            when (sEnv.i = 'mysql') then        rc = mySQL(fRM);
            when (sEnv.i = 'netqos2') then      rc = NetQOS2(fRM);
            when (sEnv.i = 'python') then       rc = Python(fRM);
            when (sEnv.i = 'perl') then         rc = Perl(fRM);
            when (sEnv.i = 'profile') then do
                rc = EnvVar_Set(0, 'DEBUG','1');
                rc = EnvVar_Set(0, 'RELEASE','');
                rc = EnvVar_Set(0, 'BUILD_MODE','PROFILE');
            end
            when (sEnv.i = 'release') then do
                rc = EnvVar_Set(0, 'DEBUG','');
                rc = EnvVar_Set(0, 'RELEASE','1');
                rc = EnvVar_Set(0, 'BUILD_MODE','RELEASE');
            end
            when (sEnv.i = 'toolkit45') then    rc = Toolkit45(fRM);
            when (sEnv.i = 'toolkit40') then    rc = Toolkit40(fRM);
            when (sEnv.i = 'unix') then         rc = Unix(fRM);
            when (sEnv.i = 'vac30') then        rc = VAC308(fRM);
            when (sEnv.i = 'vac308') then       rc = VAC308(fRM);
            when (sEnv.i = 'vac36') then        rc = VAC365(fRM);
            when (sEnv.i = 'vac365') then       rc = VAC365(fRM);
            when (sEnv.i = 'vac40') then        rc = VAC40(fRM);
            when (sEnv.i = 'warpin') then       rc = WarpIn(fRM);
            when (sEnv.i = 'watcomc11') then    rc = WatComC11(fRM);
            when (sEnv.i = 'watcomc11c') then   rc = WatComC11c(fRM);

            otherwise
                say 'error: unknown environment! - 'sEnv.i;
                exit(1)
        end

        /*
         * Check for errors.
         */
        if (rc <> 0) then
        do
            say 'Processing '''sEnv.i''' failed with rc='rc'.';
            SysSleep(10);
            exit(rc);
        end
    end


    /*
     * Check for command to execute.
     * (I.e. if there are more arguments left. after the dash.)
     */
    if (i < sEnv.0) then
    do
        sCmd = '';
        do while (i < sEnv.0)
            i = i + 1;
            sCmd = sCmd ||' '||sEnv.i;
        end
        Address CMD 'start /F' sCMD;
        Address CMD 'exit';
        exit(rc);
    end

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
        when (sProgram = 'icatpe') then     return 'f:\icatpe';
        when (sProgram = 'ida38') then      return 'f:\ida38';
        when (sProgram = 'ida40') then      return 'f:\ida401';
        when (sProgram = 'ida414') then     return 'f:\ida414';
        when (sProgram = 'idasdk') then     return 'f:\idasdk';
        when (sProgram = 'ddkbase') then    return 'f:\ddk\base';
        when (sProgram = 'msc60') then      return 'f:\ddktools\toolkits\msc60';
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
    sCVSMain    = QueryPath('cvs');
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
    sEMX        = QueryPath('emx');
    sEMXBack    = translate(sEMX, '\', '/');
    sEMXForw    = translate(sEMX, '/', '\');

    call EnvVar_Set      fRM, 'emxmain', sEMXBack;
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

    call EnvVar_Set      fRM, 'emxpgcc', sEMXBack;
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
    call EnvVar_AddFront fRM, 'INFOPATH',           sEMXForw'/info'
    call EnvVar_Set      fRM, 'EMXBOOK',            'emxdev.inf+emxlib.inf+emxgnu.inf+emxbsd.inf'
    call EnvVar_AddFront fRM, 'HELPNDX',            'emxbook.ndx', '+', 1

    return 0;

/*
 * IBM DB2 v5.2
 */
db2v52: procedure
    parse arg fRM
    sDB2Main   = QueryPath('db2v52');
    call EnvVar_Set      fRm, 'db2path',     sDB2Main;
    call EnvVar_AddFront fRm, 'beginlibpath',sDB2Main'\dll;'sDB2Main'\alt;'
    call EnvVar_AddFront fRm, 'path',        sDB2Main'\bin;'sDB2Main'\alt;'
    call EnvVar_AddFront fRm, 'dpath',       sDB2Main'\bin;'sDB2Main';'
    call EnvVar_AddFront fRm, 'help',        sDB2Main'\help;'
    call EnvVar_AddEnd   fRm, 'classpath',   '.;'sDB2Main'\JAVA\DB2JAVA.ZIP;'sDB2Main'\JAVA\RUNTIME.ZIP;'sDB2Main'\JAVA\SQLJ.ZIP;'
    call EnvVar_Set      fRM, 'db2instace',  'DB2'
    /*call EnvVar_Set      fRM, 'odbc_path',   'f:\odbc' -- huh? what's this? */
    call EnvVar_AddFront fRM, 'cobcpy',      sDB2Main'\include\cobol_mf'
    call EnvVar_Set      fRM, 'finclude',    sDB2Main'\include'
    call EnvVar_AddFront fRM, 'include',     sDB2Main'\include;'
    call EnvVar_AddFront fRM, 'lib',         sDB2Main'\lib;'
return 0;


/*
 * ICAT Debugger
 */
ICATGam: procedure
    parse arg fRM
    sICATMain   = QueryPath('icatgam');
    call EnvVar_Set      fRm, 'icatgammain', sICATMain;
    call EnvVar_AddFront fRm, 'beginlibpath',sICATMain'\dll;'
    call EnvVar_AddFront fRm, 'path',        sICATMain'\bin;'
    call EnvVar_AddFront fRm, 'dpath',       sICATMain'\help;'
    call EnvVar_AddFront fRm, 'help',        sICATMain'\help;'
    return 0;


/*
 * ICAT Debugger for PE images.
 */
ICATPe: procedure
    parse arg fRM
    sICATMain   = QueryPath('icatgam');
    sICATPe     = QueryPath('icatpe');
    call EnvVar_Set      fRm, 'icatgammain', sICATMain;
    call EnvVar_Set      fRm, 'icatpemain',  sICATPe;
    call EnvVar_AddFront fRm, 'beginlibpath',sICATPe'\bin;'sICATMain'\dll;'
    call EnvVar_AddFront fRm, 'path',        sICATPe'\bin;'sICATMain'\bin;'
    call EnvVar_AddFront fRm, 'dpath',       sICATPe'\bin;'sICATMain'\help;'
    call EnvVar_AddFront fRm, 'help',        sICATPe'\bin;'sICATMain'\help;'
    return 0;



/*
 * Interactive Disassembler (IDA) v3.80a
 */
IDA38: procedure
    parse arg fRM
    /*
     * IDA main directory.
     */
    sIDAMain = QueryPath('ida38');
    call EnvVar_Set      fRM, 'idamain',        sIDAMain
    call EnvVar_AddFront fRM, 'path',           sIDAMain
    call EnvVar_AddFront fRM, 'beginlibpath',   sIDAMain
    return 0;


/*
 * Interactive Disassembler (IDA) v4.01
 */
IDA40: procedure
    parse arg fRM
    /*
     * IDA main directory.
     */
    sIDAMain = QueryPath('ida40');
    call EnvVar_Set      fRM, 'idamain',        sIDAMain
    call EnvVar_AddFront fRM, 'path',           sIDAMain
    call EnvVar_AddFront fRM, 'beginlibpath',   sIDAMain
    return 0;


/*
 * Interactive Disassembler (IDA) v4.14
 */
IDA414: procedure
    parse arg fRM
    /*
     * IDA main directory.
     */
    sIDAMain = QueryPath('ida414');
    call EnvVar_Set      fRM, 'idamain',        sIDAMain
    call EnvVar_AddFront fRM, 'path',           sIDAMain
    call EnvVar_AddFront fRM, 'beginlibpath',   sIDAMain
    return 0;


/*
 * Interactive Disassembler (IDA) Plugin SDK (v5.0?)
 */
IDASDK: procedure
    parse arg fRM
    /*
     * IDA main directory.
     */
    sIDASDKMain = QueryPath('idasdk');
    call EnvVar_Set      fRM, 'idasdkmain',     sIDASDKMain
    call EnvVar_AddFront fRM, 'include',        sIDASDKMain'\include;'
    call EnvVar_AddFront fRM, 'lib',            sIDASDKMain'\libwat.os2;'
    call EnvVar_AddFront fRM, 'path',           sIDASDKMain'\bin\os2;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sIDASDKMain'\bin\os2;'
    return 0;


/*
 *  Device Driver Kit (DDK) base.
 */
DDKBase: procedure
    parse arg fRM

    /*
     * Device Driver Kit (DDK) (v4.0+) base (important not main) directory.
     */
    sDDKBase    = QueryPath('ddkbase');
    call EnvVar_Set      fRM, 'ddkbase', sDDKBase;
    call EnvVar_AddFront fRM, 'path',        sDDKBase'\tools;'
    call EnvVar_AddFront fRM, 'include',     sDDKBase'\h;'sDDKBase'\inc;'sDDKBase'\inc32;'
    call EnvVar_AddFront fRM, 'include16',   sDDKBase'\h;'
    call EnvVar_AddFront fRM, 'lib',         sDDKBase'\lib;'
    call EnvVar_AddFront fRM, 'bookshelf',   sDDKBase'\..\docs;'

    return 0;



/*
 * Microsoft C v6.0a
 */
MSC60: procedure
    parse arg fRM

    /*
     * Microsoft C v6.0a main directory.
     */
    sMSCMain    = QueryPath('msc60');
    call EnvVar_Set      fRM, 'BUILD_ENV',  'MSCV6'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'
    call EnvVar_Set      fRM, 'mscmain',    sMSCMain;
    call EnvVar_AddFront fRM, 'path',       sMSCMain'\binp;'
    call EnvVar_AddFront fRM, 'include',    sMSCMain'\include;'
    call EnvVar_AddFront fRM, 'include16',  sMSCMain'\include;'
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
    sMySQLMain    = QueryPath('mysql');
    call EnvVar_Set      fRM, 'mysqlmain',      sMySQLMain;
    call EnvVar_AddFront fRM, 'path',           sMySQLMain'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sMySQLMain'\dll;'
    call EnvVar_AddFront fRM, 'include',        sMySQLMain'\include;'
    call EnvVar_AddFront fRM, 'bookshelf',      sMySQLMain'\doc;'sMySQLMain'\book';
    /*call EnvVar_AddFront fRM, 'lib',            sMySQLMain'\lib;'*/
    return 0;



/*
 * NetQOS2 - help subsystem++ for VAC 3.6.5 and VAC 4.0
 */
NetQOS2: procedure
    sNetQ11Main = QueryPath('netqos2');
    call EnvVar_Set      fRM, 'netq11main',     sNetQ11Main
    call EnvVar_AddFront fRM, 'path',           sNetQ11Main';'
    call EnvVar_AddFront fRM, 'dpath',          sNetQ11Main';'
    call EnvVar_AddFront fRM, 'beginlibpath',   sNetQ11Main';'
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
    sPerlMain       = QueryPath('perl');
    sPerlMainForw   = translate(sPerlMain, '/', '\');
    call EnvVar_Set      fRM, 'perlmain',       sPerlMain;
    call EnvVar_AddFront fRM, 'path',           sPerlMain'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPerlMain'\dll;'
    call EnvVar_AddEnd   fRM, 'bookshelf',      sPerlMain'\book;'
    call EnvVar_Set      fRM, 'perllib_prefix', sPerlMainForw'/lib;'sPerlMain'\lib'
    call EnvVar_Set      fRM, 'perl5lib',       sPerlMainForw'/lib/site_perl/5.00553/os2;'sPerlMainForw'/lib/site_perl/5.00553'
    call EnvVar_Set      fRM, 'perl5load',      '2'
    call EnvVar_Set      fRM, 'perl_sh_dir',    sPerlMainForw'/bin_sh/sh.exe'
    call EnvVar_Set      fRM, 'manpath',        sPerlMainForw'/man'
    call EnvVar_Set      fRM, 'perl_badlang',   '0'
/*    call EnvVar_Set      fRM, 'LANG',           'en_US' /* dirty fix... */*/
*/
/* seems ok */
    sPerlMain       = QueryPath('perl');
    sPerlMainForw   = translate(sPerlMain, '/', '\');
    call EnvVar_Set      fRM, 'perlmain',       sPerlMain;
    call EnvVar_AddFront fRM, 'path',           sPerlMain'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPerlMain'\dll;'
    call EnvVar_AddEnd   fRM, 'bookshelf',      sPerlMain'\book;'
    call EnvVar_Set      fRM, 'perllib_prefix', sPerlMainForw'/lib;'sPerlMainForw'/lib'
    call EnvVar_Set      fRM, 'perl_sh_dir',    sPerlMainForw'/bin_sh'
    call EnvVar_Set      fRM, 'manpath',        sPerlMainForw'/man'
    call EnvVar_Set      fRM, 'perl5lib',       sPerlMainForw'/lib'
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
    sTKMain    = QueryPath('toolkit40');
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
    call EnvVar_AddFront fRM, 'nlspath',        sTkMain'\OPENDOC\BASE\LOCALE\EN_US\%N;'sTkMain'\MSG\%N;C:\MPTN\MSG\NLS\%N;C:\TCPIP\msg\ENUS850\%N;'
    call EnvVar_AddFront fRM, 'locpath',        sTkMain'\OPENDOC\BASE\LOCALE;'
    call EnvVar_AddFront fRM, 'ipfc',           sTkMain'\IPFC;'
    call EnvVar_Set      fRM, 'odbase',         sTkMain'\OPENDOC\BASE'
    call EnvVar_Set      fRM, 'odlang',         'en_US'
    call EnvVar_AddFront fRM, 'odbasepaths',    sTkMain'\OPENDOC\BASE;'
    call EnvVar_Set      fRM, 'odcfg',          sTkMain'\OPENDOC\CUSTOM'
    call EnvVar_Set      fRM, 'odtmp',          EnvVar_Get('tmp');
    call EnvVar_Set      fRM, 'sombase',        sTkMain'\SOM'
    call EnvVar_Set      fRM, 'somruntime',     sTkMain'\SOM\COMMON'

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
     sTKMain    = QueryPath('toolkit45');
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
    call EnvVar_AddFront fRM, 'helpndx',     'EPMKWHLP.NDX+', '+'
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
    call EnvVar_Set      fRM, 'XF86Main',           sXF86Back
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
    sCPPMain = QueryPath('vac308');
    call EnvVar_Set      fRM, 'CCENV',      'VAC3'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'VAC308'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

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
    sCxxMain    = QueryPath('vac365');
    call EnvVar_Set      fRM, 'CCENV',      'VAC36'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'VAC365'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_Set      fRM, 'cxxmain', sCxxMain;

    call EnvVar_AddFront fRM, 'path',        sCxxMain'\bin;'
    call EnvVar_AddFront fRM, 'dpath',       sCxxMain'\local;'sCxxMain'\help;'
    call EnvVar_AddFront fRM, 'beginlibpath', sCxxMain'\dll;'sCxxMain'\runtime;'
    call EnvVar_AddFront fRM, 'nlspath',     sCxxMain'\msg\%N;'
    call EnvVar_AddFront fRM, 'include',     sCxxMain'\include;'
    call EnvVar_AddFront fRM, 'lib',         sCxxMain'\lib;'
    call EnvVar_AddFront fRM, 'ipfc',        sCxxMain'\ipfc;'
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
    sCPPMain    = QueryPath('vac40');
    call EnvVar_Set      fRM, 'CCENV',      'VAC40'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'VAC40'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_Set      fRM, 'vacppmain',   sCPPMain;

    call EnvVar_AddFront fRM, 'path',        sCPPMain'\bin;'
    call EnvVar_AddFront fRM, 'dpath',       sCPPMain'\etc;'sCPPMain'\help;'
    call EnvVar_AddFront fRM, 'beginlibpath', sCPPMain'\dll;'sCPPMain'\runtime;'
    call EnvVar_AddFront fRM, 'help',        sCPPMain'\help;'
    call EnvVar_AddFront fRM, 'nlspath',     sCPPMain'\msg\%N;'
    call EnvVar_AddFront fRM, 'locpath',     sCPPMain'\locale;'sCPPMain'\runtime\locale;'
    call EnvVar_AddFront fRM, 'include',     sCPPMain'\ivb;'sCPPMain'\include;'
    call EnvVar_AddFront fRM, 'lib',         sCPPMain'\lib;'
    call EnvVar_AddFront fRM, 'ipfc',        sCPPMain'\bin;'
    call EnvVar_AddFront fRM, 'cpplpath4',   sCPPMain'\macros;'
    call EnvVar_Set      fRM, 'system_ice',  sCPPMain'\etc\system.ice'
    call EnvVar_Set      fRM, 'vbpath',      sCPPMain'\ivb'
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
    sWarpInMain = QueryPath('warpin');
    call EnvVar_Set      fRM, 'sWarpInMain', sWarpInMain;
    call EnvVar_AddFront fRM, 'path',        sWarpInMain';'
    call EnvVar_AddFront fRM, 'bookshelf',   sWarpInMain';'
    return 0;



/*
 * WatCom C/C++ v11.0
 */
WatComC11: procedure
    parse arg fRM

    /*
     * Watcom C/C++ v11.0 main directory
     */
    sWatComMain = QueryPath('watcom11');
    call EnvVar_Set      fRM, 'CCENV',      'WAT'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'WAT11'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_Set      fRM, 'watcom',      sWatComMain
    call EnvVar_AddFront fRM, 'path',        sWatComMain'\binp;'sWatComMain'\binw;'
    call EnvVar_AddFront fRM, 'beginlibpath',sWatComMain'\binp\dll;'
    call EnvVar_AddFront fRM, 'help',        sWatComMain'\binp\help;'
    call EnvVar_AddEnd   fRM, 'bookshelf',   sWatComMain'\binp\help;'
    call EnvVar_AddFront fRM, 'include',     sWatComMain'\h;'sWatComMain'\h\os2;'sWatComMain'\h\nt;'
    call EnvVar_AddFront fRM, 'lib',         sWatComMain'\lib386;'sWatComMain'\lib386\os2;'sWatComMain'\lib286;'sWatComMain'\lib286\os2;'
    call EnvVar_Set      fRM, 'edpath',      sWatComMain'EDDAT;'
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
    sWatComMain = QueryPath('watcom11c');
    call EnvVar_Set      fRM, 'CCENV',      'WAT'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'WAT11'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_Set      fRM, 'watcom',      sWatComMain
    call EnvVar_AddFront fRM, 'path',        sWatComMain'\binp;'sWatComMain'\binw;'
    call EnvVar_AddFront fRM, 'beginlibpath',sWatComMain'\binp\dll;'
    call EnvVar_AddFront fRM, 'help',        sWatComMain'\binp\help;'
    call EnvVar_AddEnd   fRM, 'bookshelf',   sWatComMain'\binp\help;'
    call EnvVar_AddFront fRM, 'include',     sWatComMain'\h;'sWatComMain'\h\os2;'sWatComMain'\h\nt;'
    call EnvVar_AddFront fRM, 'lib',         sWatComMain'\lib386;'sWatComMain'\lib386\os2;'sWatComMain'\lib286;'sWatComMain'\lib286\os2;'
    call EnvVar_Set      fRM, 'edpath',      sWatComMain'EDDAT;'
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

