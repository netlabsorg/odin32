/* $Id: buildenv.cmd,v 1.48 2003-03-20 19:52:27 bird Exp $
 *
 * This is the master tools environment script. It contains environment
 * configurations for many development tools. Each tool can be installed
 * and uninstalled from the environment interchangably.
 *
 * Note:    Of historic reasons, there are some environments here which
 *          isn't normally used by normal code trees.
 *
 *
 * Known problems:
 *     - LANG is set to en_US by both VAC36 and TOOLKIT45. When unsetting
 *       those the original value, for example of no_NO, is not restored.
 *     - Same goes for some other stuff, we have no stack of previous values.
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 *
 * GPL v2
 *
 */

    Address CMD '@echo off';

    signal on novalue name NoValueHandler

    /*
     * Version
     */
    sVersion = '1.0.19 [2003-03-20]';

    /*
     * Create argument array with lowercase arguments.
     */
    parse arg sEnv.1 sEnv.2 sEnv.3 sEnv.4 sEnv.5 sEnv.6 sEnv.7 sEnv.8 sEnv.9 sEnv.10 sEnv.11 sEnv.12 sEnv.13 sEnv.14 sEnv.15 sEnv.16 sEnv.17 sEnv.18 sEnv.19 sEnv.20 sEnv.21 sEnv.22 sEnv.23

    i = 1;
    do while (sEnv.i <> '')
        sEnv.i = translate(strip(sEnv.i), 'abcdefghijklmnopqrstuvwxyzëõÜ', 'ABCDEFGHIJKLMNOPQRSTUVWXYZíùè');
        i = i + 1;
    end
    sEnv.0 = i - 1;

    /*
     * Syntax
     */
    if (sEnv.0 = 0) then
    do
        say 'BuildEnv v'||sVersion
        say '-------------------------------'
        say ''
        say 'Synopsis: Environment configuration utility written to maintain'
        say 'many different versions of compilers and toolkits on the same'
        say 'workstation. '
        say ''
        say 'Syntax: BuildEnv.cmd <environments>[action]'
        say ''
        say 'Actions:'
        say '   +   Install tool in environment. Default action.'
        say '   ~   Install tool in environment if it''s configured.'
        say '   -   Remove tool from environment.'
        say '   ı   Remove tool from environment if it''s configured.'
        say '   *   Configure tool if needed.'
        say '   !   Forced tool configuretion.'
        say '   @   Verify tool configuration.'
        say '   ?   Query if a tool is configured.'
        say ''
        say 'Special environments (commands):'
        say '   allconfig       Configure all tools which fails verify.'
        say '   allreconfig     Reconfigure all tools.'
        say '   allverify       Verify all configured tools.'
        say '   alluninstall    Removed all configured tools from environment.'
        say '   showall         Show all tools.'
        say '   showconfigured  Show all configured tools.'
        say '   shownotconfigured   Show all tools which isn''t configured.'
        say ''
        say 'Copyright (c) 1999-2003 knut st. osmundsen'
        say 'Published under GPL v2'
        return 8;
    end

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
     * Configuration - sorted please!
     */
    parse source . . sPathFile .
    sPathFile = sPathFile||'.paths';
    aPath.0 = 0;

    i = 1;
    /* Tool id                      The tool's group            The function with args.                 Optional verify data.                       increment index */
    aCfg.i.sId = 'cvs';             aCfg.i.sGrp = 'version';    aCfg.i.sSet = 'CVS';                    aCfg.i.sDesc = 'CVS v1.10 or later';        i = i + 1;
    aCfg.i.sId = 'db2v52';          aCfg.i.sGrp = 'database';   aCfg.i.sSet = 'db2v52';                 aCfg.i.sDesc = 'DB2 v5.2 Dev Edition';      i = i + 1;
    aCfg.i.sId = 'ddk';             aCfg.i.sGrp = 'ddk';        aCfg.i.sSet = 'DDK';                    aCfg.i.sDesc = 'OS/2 DDK (recent)';         i = i + 1;
    aCfg.i.sId = 'ddkbase';         aCfg.i.sGrp = 'ddk';        aCfg.i.sSet = 'DDKBase';                aCfg.i.sDesc = 'DDK Base (recent)';         i = i + 1;
    aCfg.i.sId = 'ddkvideo';        aCfg.i.sGrp = 'ddk';        aCfg.i.sSet = 'DDKVideo';               aCfg.i.sDesc = 'DDK Video (recent)';        i = i + 1;
    aCfg.i.sId = 'doxygen';         aCfg.i.sGrp = 'doc';        aCfg.i.sSet = 'DoxyGen';                aCfg.i.sDesc = 'Doxygen v1.2.11 for OS/2';        i = i + 1;
    aCfg.i.sId = 'emx';             aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'EMX';                    aCfg.i.sDesc = 'EMX v0.9d fixpack 04';      i = i + 1;
    aCfg.i.sId = 'emxpgcc';         aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'EMXPGCC';                aCfg.i.sDesc = 'Pentium Optimized GCC/EMX v1.1.1 r2 with binutils 2.9.1'; i = i + 1;
    aCfg.i.sId = 'freetypeemx';     aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'FreeTypeEMX';            aCfg.i.sDesc = 'FreeType v1.3.1 for EMX.'; i = i + 1;
    aCfg.i.sId = 'gcc302';          aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'GCC3xx,''gcc302''';      aCfg.i.sDesc = 'GCC/EMX v3.0.2beta with binutils 2.11.2'; i = i + 1;
    aCfg.i.sId = 'gcc303';          aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'GCC3xx,''gcc303''';      aCfg.i.sDesc = 'GCC/EMX v3.0.3beta with binutils 2.11.2'; i = i + 1;
    aCfg.i.sId = 'gcc321';          aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'GCC3xx,''gcc321''';      aCfg.i.sDesc = 'GCC/EMX v3.2.1beta with binutils 2.11.2'; i = i + 1;
    aCfg.i.sId = 'icatgam';         aCfg.i.sGrp = 'debugger';   aCfg.i.sSet = 'ICATGam';                aCfg.i.sDesc = 'ICAT for OS/2 latest';      i = i + 1;
    aCfg.i.sId = 'icatgam406rc1';   aCfg.i.sGrp = 'debugger';   aCfg.i.sSet = 'ICATGam406RC1';          aCfg.i.sDesc = 'ICAT for OS/2 v4.0.6 release candidate 1'; i = i + 1;
    aCfg.i.sId = 'icatpe';          aCfg.i.sGrp = 'debugger';   aCfg.i.sSet = 'ICATPe';                 aCfg.i.sDesc = 'ICAT for OS/2 with PE support (test version)'; i = i + 1;
    aCfg.i.sId = 'ida';             aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'IDA414';                 aCfg.i.sDesc = 'Interactive DisAssembler (IDA) (latest)'; i = i + 1;
    aCfg.i.sId = 'ida38';           aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'IDA38';                  aCfg.i.sDesc = 'Interactive DisAssembler (IDA) v3.80 (historical)'; i = i + 1;
    aCfg.i.sId = 'ida40';           aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'IDA40';                  aCfg.i.sDesc = 'Interactive DisAssembler (IDA) v4.0 (historical)'; i = i + 1;
    aCfg.i.sId = 'ida414';          aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'IDA414';                 aCfg.i.sDesc = 'Interactive DisAssembler (IDA) v4.14'; i = i + 1;
    aCfg.i.sId = 'idasdk';          aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'IDASDK';                 aCfg.i.sDesc = 'Interactive DisAssembler (IDA) SDK'; i = i + 1;
    aCfg.i.sId = 'java131';         aCfg.i.sGrp = 'java';       aCfg.i.sSet = 'Java131';                aCfg.i.sDesc = 'Java v1.3.1 (co131-20020710)'; i = i + 1;
    aCfg.i.sId = 'jpeg';            aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'JPEG';                   aCfg.i.sDesc = '(lib)JPEG v6b';             i = i + 1;
    aCfg.i.sId = 'mode12050';       aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'Mode,120,50';            aCfg.i.sDesc = 'mode 120,50';               i = i + 1;
    aCfg.i.sId = 'mode8050';        aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'Mode,80,50';             aCfg.i.sDesc = 'mode 80,50';                i = i + 1;
    aCfg.i.sId = 'mscv6';           aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'MSCV6_32';               aCfg.i.sDesc = 'MicroSoft C v6.0 32-bit';   i = i + 1;
    aCfg.i.sId = 'mscv6-16';        aCfg.i.sGrp = 'comp16';     aCfg.i.sSet = 'MSCV6_16';               aCfg.i.sDesc = 'MicroSoft C v6.0a 16-bit';  i = i + 1;
    aCfg.i.sId = 'mscv7-16';        aCfg.i.sGrp = 'comp16';     aCfg.i.sSet = 'MSCV7_16';               aCfg.i.sDesc = 'MicroSoft C v7.0 16-bit with OS/2 support';  i = i + 1;
    aCfg.i.sId = 'mysql';           aCfg.i.sGrp = 'database';   aCfg.i.sSet = 'mySQL';                  aCfg.i.sDesc = 'MySql any version (latest from Yuri is recommended)'; i = i + 1;
    aCfg.i.sId = 'nasm9833';        aCfg.i.sGrp = 'asm';        aCfg.i.sSet = 'NASM,''nasm9833''';      aCfg.i.sDesc = 'NASM version 0.98.33 compiled on May 28 2002'; i = i + 1;
    aCfg.i.sId = 'netqos2';         aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'NetQOS2';                aCfg.i.sDesc = 'NetQOS2 - help system for VAC40,VAC365,DB2 and more.'; i = i + 1;
    aCfg.i.sId = 'odin32testcase';  aCfg.i.sGrp = 'tests';      aCfg.i.sSet = 'Odin32Testcase';         aCfg.i.sDesc = 'Odin32 testcase setup'; i = i + 1;
    aCfg.i.sId = 'perl';            aCfg.i.sGrp = 'script';     aCfg.i.sSet = 'Perl580';                aCfg.i.sDesc = 'Perl v5.8.0'; i = i + 1;
    aCfg.i.sId = 'perl580';         aCfg.i.sGrp = 'script';     aCfg.i.sSet = 'Perl580';                aCfg.i.sDesc = 'Perl v5.8.0'; i = i + 1;
    aCfg.i.sId = 'perl50053';       aCfg.i.sGrp = 'script';     aCfg.i.sSet = 'Perl50xxx';              aCfg.i.sDesc = 'Perl v5.0053'; i = i + 1;
    aCfg.i.sId = 'python';          aCfg.i.sGrp = 'script';     aCfg.i.sSet = 'Python';                 aCfg.i.sDesc = 'Python v1.5'; i = i + 1;
    aCfg.i.sId = 'toolkit40';       aCfg.i.sGrp = 'tlktos2';    aCfg.i.sSet = 'Toolkit40';              aCfg.i.sDesc = 'Toolkit v4.0 CSD 4'; i = i + 1;
    aCfg.i.sId = 'toolkit45';       aCfg.i.sGrp = 'tlktos2';    aCfg.i.sSet = 'Toolkit45';              aCfg.i.sDesc = 'Toolkit v4.5'; i = i + 1;
    aCfg.i.sId = 'toolkit451';      aCfg.i.sGrp = 'tlktos2';    aCfg.i.sSet = 'Toolkit451';             aCfg.i.sDesc = 'Toolkit v4.5.1'; i = i + 1;
    aCfg.i.sId = 'toolkit452';      aCfg.i.sGrp = 'tlktos2';    aCfg.i.sSet = 'Toolkit452';             aCfg.i.sDesc = 'Toolkit v4.5.2'; i = i + 1;
    aCfg.i.sId = 'unix';            aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'Unix';                   aCfg.i.sDesc = 'Misc unix stuff.'; i = i + 1;
    aCfg.i.sId = 'vac308';          aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'VAC308';                 aCfg.i.sDesc = 'VisualAge for C++ v3.08'; i = i + 1;
    aCfg.i.sId = 'vac365';          aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'VAC365';                 aCfg.i.sDesc = 'VisualAge for C++ v3.6.5 FP2 with latest optimizer fixes.'; i = i + 1;
    aCfg.i.sId = 'vac40';           aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'VAC40';                  aCfg.i.sDesc = 'VisualAge for C++ v4.0 FP2(??)'; i = i + 1;
    aCfg.i.sId = 'warpin';          aCfg.i.sGrp = 'misc';       aCfg.i.sSet = 'WarpIn';                 aCfg.i.sDesc = 'WarpIn 0.9.18+ (for Odin32 18 with fix is required)'; i = i + 1;
    aCfg.i.sId = 'watcomc11';       aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'WatComC11,32,';          aCfg.i.sDesc = 'Watcom C/C++ v11.0 32-bit (no fixes)'; i = i + 1;
    aCfg.i.sId = 'watcomc11-16';    aCfg.i.sGrp = 'comp16';     aCfg.i.sSet = 'WatComC11,16';           aCfg.i.sDesc = 'Watcom C/C++ v11.0 16-bit (no fixes)'; i = i + 1;
    aCfg.i.sId = 'watcomc11c';      aCfg.i.sGrp = 'comp32';     aCfg.i.sSet = 'WatComC11c,32';          aCfg.i.sDesc = 'Watcom C/C++ v11.0c 32-bit (beta)'; i = i + 1;
    aCfg.i.sId = 'watcomc11c-16';   aCfg.i.sGrp = 'comp16';     aCfg.i.sSet = 'WatComC11c,16';          aCfg.i.sDesc = 'Watcom C/C++ v11.0c 16-bit (beta)'; i = i + 1;
    aCfg.0 = i - 1;



    /*
     * Parse arguments
     */
    do i = 1 to sEnv.0
        /* uses dash to mark end of arguments */
        if ((sEnv.i = '-') | (sEnv.i = '*')) then
            leave;

        /*
         * Get last char.
         * Dash means remove, pluss means add, asterix means verify and configure.
         * Pluss is default and optional.
         *
         */
        ch = substr(sEnv.i, length(sEnv.i), 1);
        if (pos(ch, '+~-ı*!?@') > 0) then
            sEnv.i = substr(sEnv.i, 1, length(sEnv.i) - 1);
        else
            ch = '+';
        fRM = (ch = '-' | ch = 'ı');
        fOptional = (ch = '~' | ch = 'ı')
        fCfg = (ch = '*');
        fForcedCfg = (ch = '!');
        fVerify = (ch = '@')
        fQuery = (ch = '?')

        /*
         * do the switch.
         */
        rc = 0;
        select

            /*
             * Multi tool operations.
             */
            when (sEnv.i = 'allconfig') then do
                do j = 1 to aCfg.0
                    if (CfgVerify(j, 0, 1) <> 0) then
                    do
                        rc = CfgConfigure(j, 1);
                        if (rc >= 8) then
                            exit(rc);
                    end
                end
            end

            when (sEnv.i = 'allreconfig') then do
                do j = 1 to aCfg.0
                    rc = CfgConfigure(j, 1);
                    if (rc >= 8) then
                        exit(rc);
                end
            end

            when (sEnv.i = 'allverify') then do
                do j = 1 to aCfg.0
                    if (CfgIsConfigured(j)) then
                        call CfgVerify j, 0, 1;
                end
            end

            when (sEnv.i = 'alluninstall') then do
                do j = 1 to aCfg.0
                    if (CfgIsConfigured(j)) then
                        call CfgInstallUninstall j, 1;
                end
            end

            when (sEnv.i = 'showall') then do
                do j = 1 to aCfg.0
                    say left(aCfg.j.sId, 15) '-' left(aCfg.j.sGrp, 8) '-' aCfg.j.sDesc
                end
            end
            when (sEnv.i = 'showconfigured') then do
                do j = 1 to aCfg.0
                    if (CfgIsConfigured(j)) then
                        say left(aCfg.j.sId, 15) '-' left(aCfg.j.sGrp, 8) '-' aCfg.j.sDesc
                end
            end

            when (sEnv.i = 'shownotconfigured') then do
                do j = 1 to aCfg.0
                    if (\CfgIsConfigured(j)) then
                        say left(aCfg.j.sId, 15) '-' left(aCfg.j.sGrp, 8) '-' aCfg.j.sDesc
                end
            end


            /*
             * Special 'tools'.
             */
            when (sEnv.i = 'debug') then do
                rc = EnvSet(0, 'DEBUG','1');
                rc = EnvSet(0, 'RELEASE','');
                rc = EnvSet(0, 'BUILD_MODE','DEBUG');
            end
            when (sEnv.i = 'profile') then do
                rc = EnvSet(0, 'DEBUG','1');
                rc = EnvSet(0, 'RELEASE','');
                rc = EnvSet(0, 'BUILD_MODE','PROFILE');
            end
            when (sEnv.i = 'release') then do
                rc = EnvSet(0, 'DEBUG','');
                rc = EnvSet(0, 'RELEASE','1');
                rc = EnvSet(0, 'BUILD_MODE','RELEASE');
            end

            when (sEnv.i = 'buildsetup') then
                rc = EnvSet(0, 'BUILD_SETUP_MAK','make\setup.mak');

            /*
             * Generic
             */
            otherwise
            do
                fFound = 0;
                do j = 1 to aCfg.0
                    if (aCfg.j.sId = sEnv.i) then
                    do
                        /*
                         * Found the environment.
                         */
                        fFound = 1;

                        /*
                         * Take requested action.
                         */
                        rc = -16;
                        if (fCfg | fForcedCfg) then
                            rc = CfgConfigure(j, fForcedCfg);
                        else if (fVerify) then
                            rc = CfgVerify(j, 0, 1);
                        else if (fQuery) then
                        do
                            rc = 0;
                            if (\CfgIsConfigured(j)) then
                                return 3;
                        end
                        else
                        do
                            if (\fOptional) then
                                rc = CfgInstallUninstall(j, fRM);
                            else if (CfgIsConfigured(j)) then
                                rc = CfgInstallUninstall(j, fRM);
                        end
                        leave;
                    end
                end /* loop */

                if (\fFound) then
                do
                    say 'error: unknown tool! - 'sEnv.i;
                    call SysSleep 2;
                    exit(16)
                end
            end /* otherwise */
        end /* select */
    end /* sEnv.i loop */


    /*
     * Check for command to execute.
     * (I.e. if there are more arguments left. after the dash/star.)
     */
    if (i < sEnv.0) then
    do
        chType = sEnv.i;

        sCmd = '';
        do while (i < sEnv.0)
            i = i + 1;
            sCmd = sCmd ||' '||sEnv.i;
        end

        if (chType = '-') then
        do
            Address CMD 'start /F' sCMD;
            Address CMD 'exit';
        end
        else
            Address CMD sCMD;
        exit(rc);
    end

exit(0);


/**
 * No value handler
 */
NoValueHandler:
    say 'NoValueHandler: line 'SIGL;
exit(16);



/**
 * Get the description of an tool.
 * @returns Description string.
 *          '' if not found.
 * @param   sToolId      Tool id.
 */
CfgDesc: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId
    do i = 1 to aCfg.0
        if (aCfg.i.sId = sToolId) then
            return aCfg.i.sDesc;
    end
return sToolId;


/**
 * Lookups up an env. config in the aCfg. array.
 * @return  Index of sToolId.
 *          aCfg.0+1 on error.
 * @param   sToolId      Tool id.
 */
CfgLookup: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId
    iTool = 1;
    do while ((iTool <= aCfg.0) & (aCfg.iTool.sId <> sToolId))
        iTool = iTool + 1;
    end
return iTool;


/**
 * Verifies a configuration.
 * @returns 0 on success.
 *          4 on error/warnings which is continuable.
 *          8 or higher or on fatal errors.
 * @param   iTool   The tool index in aCfg.
 * @param   fRM     If set we'll uninstall the tool from the environment.
 */
CfgInstallUninstall: procedure expose aCfg. aPath. sPathFile
    parse arg iTool, fRM

    /* make rexx expression */
    if (pos(',', aCfg.iTool.sSet) > 0) then
        sRexx = substr(aCfg.iTool.sSet, 1, pos(',', aCfg.iTool.sSet) - 1) || '(aCfg.iTool.sId,sOperation,fRM,fQuiet',
             || substr(aCfg.iTool.sSet, pos(',', aCfg.iTool.sSet)) || ')';
    else
        sRexx = aCfg.iTool.sSet || '(aCfg.iTool.sId,sOperation,fRM,fQuiet)';
    fQuiet = 0;
    if (\fRM) then  sOperation = 'install';
    else            sOperation = 'uninstall';

    /* call the tool procedure with a verify operation. */
    interpret 'iRc = '||sRexx;

    /* On failure we'll complain and quietly uninstall the tool. */
    if (iRc <> 0) then
    do
        /* complain */
        if (\fQuiet) then
        do
            select
                when (iRc = 1) then
                    say 'error - 'aCfg.iTool.sId': 'sOperation' not configured - ie. no path.';
                when (iRc = 2) then
                    say 'error - 'aCfg.iTool.sId': 'sOperation' failed ''cause some vital file/dir wasn''t found.';
                when (iRc = 49) then
                    say 'error - 'aCfg.iTool.sId': 'sOperation' failed ''cause some vital command didn''t return as expected.';
                when (iRc = 99) then
                    say 'error - 'aCfg.iTool.sId': 'sOperation' failed ''cause some vital command didn''t return the expected output.';
                otherwise
                    say 'internal error- 'aCfg.iTool.sId': bad return code from '''sRexx''' rc=' iRc'.';
            end
        end

        /* uninstall silently */
        fRM = 1;
        fQuiet = 1;
        sOperation = 'quietuninstall';
        interpret 'rcignore = '||sRexx;
    end
return iRc;




/**
 * Configures an tool.
 * @returns 0 on success.
 *          4 on error/warnings which is continuable.
 *          8 or higher or on fatal errors.
 * @param   iTool       The tool configuration to configure.
 * @param   fForced     If set, we'll force a reconfiguration of the tool.
 */
CfgConfigure: procedure expose aCfg. aPath. sPathFile
    parse arg iTool, fForced

    /*
     * First verfiy the configuration quietly, we don't have to do anything if it's ok.
     */
    if (\fForced & (CfgVerify(iTool, 1, 1) = 0)) then
        return 0;

    /*
     * We have to configure it!
     */
    say '- Config of the 'aCfg.iTool.sId' ('CfgDesc(aCfg.iTool.sId)') tool.';

    /* make rexx expression */
    if (pos(',', aCfg.iTool.sSet) > 0) then
        sRexx = substr(aCfg.iTool.sSet, 1, pos(',', aCfg.iTool.sSet) - 1) || '(aCfg.iTool.sId,sOperation,fRM,fQuiet',
             || substr(aCfg.iTool.sSet, pos(',', aCfg.iTool.sSet)) || ')';
    else
        sRexx = aCfg.iTool.sSet || '(aCfg.iTool.sId,sOperation,fRM,fQuiet)';
    if (fForced) then   sOperation = 'forcedconfig';
    else                sOperation = 'config';
    fRM = 0;
    fQuiet = 0;


    /*
     * Loop till rc=0 or user gives up.
     */
    rc = -1
    do while (rc <> 0)
        /* configure */
        interpret 'rc = '||sRexx;

        if (rc <> 0) then do
            say 'warning: The user refused to give a path, continuing.';
            return 4;
        end

        /* verifying */
        rc = CfgVerify(iTool, 0, 1);
        sOperation = 'verify';
        if (rc = 0) then
            leave;

        /* Retry the config if the user wanna do so. */
        say ''
        say 'Retry configuring the tool' aCfg.iTool.sId '('CfgDesc(aCfg.iTool.sId)')? (y/N)';
        sAnswer = PullUser(1);
        if (substr(strip(sAnswer),1,1) <> 'Y') then
            return 4;
        sOperation = 'forcedconfig';
    end

    /*
     * Write path file and return successfully.
     */
    call PathWrite;
return 0;


/**
 * Verifies a configuration.
 * @returns Return code from the environment procedure.
 * @param   iTool       The tool index in aCfg.
 * @param   fQuiet      If set we'll to a quiet verify.
 * @param   fCleanup    If set we'll clean properly.
 */
CfgVerify: procedure expose aCfg. aPath. sPathFile
    parse arg iTool, fQuiet, fCleanup

    /* make rexx expression */
    if (pos(',', aCfg.iTool.sSet) > 0) then
        sRexx = substr(aCfg.iTool.sSet, 1, pos(',', aCfg.iTool.sSet) - 1) || '(aCfg.iTool.sId,sOperation,fRM,fQuiet',
             || substr(aCfg.iTool.sSet, pos(',', aCfg.iTool.sSet)) || ')';
    else
        sRexx = aCfg.iTool.sSet || '(aCfg.iTool.sId,sOperation,fRM,fQuiet)';
    if (fQuiet) then    sOperation = 'quietverify';
    else                sOperation = 'verify';
    fRM = 0;

    /* call the tool procedure with a verify operation. */
    interpret 'iRc = '||sRexx;

    /* On failure we'll complain and quietly uninstall the tool. */
    if (iRc <> 0) then
    do
        /* complain */
        if (\fQuiet) then
        do
            select
                when (iRc = 1) then
                    say 'warning - 'aCfg.iTool.sId': The user refused to give a path, continuing.';
                when (iRc = 2) then
                    say 'error - 'aCfg.iTool.sId': verify failed ''cause some vital file/dir wasn''t found.';
                when (iRc = 49) then
                    say 'error - 'aCfg.iTool.sId': verify failed ''cause some vital command didn''t return as expected.';
                when (iRc = 99) then
                    say 'error - 'aCfg.iTool.sId': verify failed ''cause some vital command didn''t return the expected output.';
                otherwise
                    say 'internal error- 'aCfg.iTool.sId': bad return code from '''sRexx''' iRc=' iRc'.';
            end
        end
        fCleanup = 1;
    end

    /* uninstall */
    if (fCleanup) then
    do
        fRM = 1;
        fQuiet = 1;
        sOperation = 'quietuninstall';
        interpret 'rcignore = '||sRexx;
    end
return iRc;


/**
 * Verifies a configuration.
 * @returns True if configured.
 *          False if not configured.
 * @param   iTool   The tool index in aCfg.
 * @param   fQuiet  If set we'll to a quiet verify.
 */
CfgIsConfigured: procedure expose aCfg. aPath. sPathFile
    parse arg iTool

    /* make rexx expression */
    if (pos(',', aCfg.iTool.sSet) > 0) then
        sRexx = substr(aCfg.iTool.sSet, 1, pos(',', aCfg.iTool.sSet) - 1) || '(aCfg.iTool.sId,''quietisconfig'',0,1',
             || substr(aCfg.iTool.sSet, pos(',', aCfg.iTool.sSet)) || ')';
    else
        sRexx = aCfg.iTool.sSet || '(aCfg.iTool.sId,''quietisconfig'',0,1)';
    interpret 'iRc = '||sRexx;
return (iRc = 0);



/**
 * Checks if a file exists.
 * @param   sFile       Name of the file to look for.
 * @param   fQuiet      Flag which tells whether to be quiet or not.
 * @param   fOptional   Flag to say that this file is optional.
 * @returns TRUE if file exists.
 *          FALSE if file doesn't exists.
 */
CfgVerifyFile: procedure expose aCfg. aPath. sPathFile
    parse arg sFile, fQuiet, fOptional
    if (fOptional = '') then fOptional = 0;
    rc = stream(sFile, 'c', 'query exist');
    if ((rc = '') & \fQuiet) then
    do
        if (fOptional) then
            say 'Warning: Installation is missing '''sFile'''.';
        else
            say 'Verify existance of '''sFile''' failed.';
    end
return rc <> '' | fOptional;


/**
 * Checks if a directory exists.
 * @param   sDir   Name of the dir to look for.
 * @param   fQuiet  Flag which tells whether to be quiet or not.
 * @returns TRUE if file exists.
 *          FALSE if file doesn't exists.
 */
CfgVerifyDir: procedure expose aCfg. aPath. sPathFile
    parse arg sDir, fQuiet
    rc = SysFileTree(sDir, 'sDirs', 'DO');
    if (rc = 0 & sDirs.0 = 1) then
        return 1;
    if (\fQuiet) then
        say 'Verify existance of '''sDir''' failed.';
return 0;





/**
 * The Directory Configuration Function.
 *
 * @returns Lower cased, absolute, backward slashed, path to program.
 * @param   sPathId     Program identifier. (lowercase!)
 */
PathQuery: procedure expose aCfg. aPath. sPathFile
    parse arg sPathId, sToolId, sOperation, fOptional

    if (fOptional = '') then
        fOptional = 0;

    if (aPath.0 = 0) then
    do  /*
         * Read path config file
         */
        call PathRead;

        /*
         * If no data found fill in defaults (if known host).
         */
        if (aPath.0 = 0) then
        do
            call PathSetDefault;
            call PathWrite;
        end
    end

    /*
     * Check for forced config.
     */
    if (sOperation = 'forcedconfig') then
        call PathRemove sPathId;
    else
    do
        /*
         * Search for the path.
         */
        do i = 1 to aPath.0
            if (aPath.i.sPId = sPathId) then
            do
                return aPath.i.sPath;
                leave;
            end
        end
    end

    /*
     * Path wasn't found!
     */

    /* for quiet verify, configured test and uninstall, fail sliently. */
    if ((sOperation = 'quietisconfig') | (sOperation = 'quietverify') | (sOperation = 'quietuninstall')) then
        return '';

    /* if configure operation the configure it. */
    if (pos('config', sOperation) > 0) then
        return PathConfig(sOperation, sPathId, sToolId);

    /* elsewise this is an fatal error */
    if (\fOptional) then
    do
        say 'Fatal error: Path information for '''sPathId''' was not found.';
        call SysSleep 5;
        exit(16);
    end

return '';


/**
 * Reads the path file into the 'aPath.' stem.
 */
PathRead: procedure expose aCfg. aPath. sPathFile

    i = 1;                              /* Path index */
    iLine = 0;                          /* Line # in file */

    /*
     * Read loop.
     */
    do while (lines(sPathFile) > 0)
        iLine = iLine + 1;
        sLine = strip(linein(sPathFile));

        /*
         * Skip empty lines and comment lines, ie. starting with '#' or ';'.
         */
        if ((sLine <> '') & (substr(sLine, 1, 1) <> '#') & (substr(sLine, 1, 1) <> ';')) then
        do
            /*
             * Parse the line.
             */
            parse var sLine aPath.i.sPId '=' aPath.i.sPath
            aPath.i.sPId = strip(aPath.i.sPId);
            aPath.i.sPath = strip(aPath.i.sPath);

            /*
             * Validate the input.
             */
            if ((aPath.i.sPath = '') | (aPath.i.sPId = '') | (translate(sLine,'','#!$@%|<>;ı&œ') <> sLine) ) then
            do
                say 'fatal error: missformed line in path file at line 'iLine'!'
                call stream sPathFile, 'c', 'close';
                call SysSleep 5;
                exit(16);
            end
            i = i + 1;
        end
    end
    call stream sPathFile, 'c', 'close';
    aPath.0 = i - 1;
return 0;


/**
 * Writes the path file from what's in the 'aPath.' stem.
 */
PathWrite: procedure expose aCfg. aPath. sPathFile
    call SysFileDelete(sPathFile);
    do i = 1 to aPath.0
        /* skip if already written */
        j = 1;
        do while (aPath.j.sPId <> aPath.i.sPId)
            j = j + 1;
        end
        if (j >= i) then
            call lineout sPathFile, aPath.i.sPId'='aPath.i.sPath;
    end
    call stream sPathFile, 'c', 'close';
return 0;


/**
 * Remove a path from the 'aPath.' stem.
 * @returns 0
 * @param   sPathId     The id of the path to remove.
 */
PathRemove: procedure expose aCfg. aPath. sPathFile
    parse arg sPathId

    /*
     * Find.
     */
    i = 1;
    do while (i <= aPath.0)
        if (aPath.i.sPId = sPathId) then
            leave;
        i = i + 1;
    end

    /*
     * Move.
     */
    if (i <= aPath.0) then
    do
        j = i + 1;
        do while (j <= aPath.0)
            aPath.i.sPId = aPath.j.sPId;
            aPath.i.sPath = aPath.j.sPath;
            j = j + 1;
            i = i + 1;
        end
        aPath.0 = aPath.0 - 1;
    end
return 0;


/**
 * Sets a given path.
 * @param   sPathId     Path id.
 * @param   sNewPath    Path.
 */
PathSet: procedure expose aCfg. aPath. sPathFile
parse arg sPathId, sNewPath

    /*
     * Search for the path.
     */
    do i = 1 to aPath.0
        if (aPath.i.sPId = sPathId) then
        do
            aPath.i.sPath = sNewPath;
            return 0;
        end
    end

    /*
     * Not found, so add it.
     */
    i = aPath.0 + 1;
    aPath.i.sPId = sPathId;
    aPath.i.sPath = sNewPath;
    aPath.0 = i;
return 0;



/**
 * Fills 'aPath.' with default settings overwriting anything in the table.
 */
PathSetDefault: procedure expose aCfg. aPath. sPathFile
    i = 1;

    /*
     * Bird: home boxes.
     */
    if ((translate(EnvGet('HOSTNAME')) = 'UNIVAC') | (translate(EnvGet('HOSTNAME')) = 'ENIAC')) then
    do
        say 'Info: No or empty path file, using birds defaults.';
        aPath.i.sPId = 'cvs';                       aPath.i.sPath = 'f:\cvs';                       i = i + 1;
        aPath.i.sPId = 'db2v52';                    aPath.i.sPath = 'f:\sqllib52';                  i = i + 1;
        aPath.i.sPId = 'emx';                       aPath.i.sPath = 'f:\emx';                       i = i + 1;
        aPath.i.sPId = 'emxpgcc';                   aPath.i.sPath = 'f:\emxpgcc';                   i = i + 1;
        aPath.i.sPId = 'freetypeemx';               aPath.i.sPath = 'f:\Freetype\v1.3.1-emx\emx';   i = i + 1;
        aPath.i.sPId = 'gcc302';                    aPath.i.sPath = 'f:\gcc302beta';                i = i + 1;
        aPath.i.sPId = 'gcc303';                    aPath.i.sPath = 'f:\gcc\v3.0.3beta\emx';        i = i + 1;
        aPath.i.sPId = 'gcc321';                    aPath.i.sPath = 'f:\gcc\v3.2.1beta\emx';        i = i + 1;
        aPath.i.sPId = 'icatgam';                   aPath.i.sPath = 'f:\icatos2';                   i = i + 1;
        aPath.i.sPId = 'icatgam406rc1';             aPath.i.sPath = 'f:\icatos2.4.0.6.rc1';         i = i + 1;
        aPath.i.sPId = 'icatpe';                    aPath.i.sPath = 'f:\icatpe';                    i = i + 1;
        aPath.i.sPId = 'ida38';                     aPath.i.sPath = 'f:\ida38';                     i = i + 1;
        aPath.i.sPId = 'ida40';                     aPath.i.sPath = 'f:\ida401';                    i = i + 1;
        aPath.i.sPId = 'ida414';                    aPath.i.sPath = 'f:\ida414';                    i = i + 1;
        aPath.i.sPId = 'idasdk';                    aPath.i.sPath = 'f:\idasdk';                    i = i + 1;
        aPath.i.sPId = 'java131';                   aPath.i.sPath = 'e:\java131';                   i = i + 1;
        aPath.i.sPId = 'jpeg';                      aPath.i.sPath = 'f:\jpeg\v6b';                  i = i + 1;
        aPath.i.sPId = 'ddk';                       aPath.i.sPath = 'f:\ddk\april02';               i = i + 1;
        aPath.i.sPId = 'ddkbase';                   aPath.i.sPath = 'f:\ddk\april02\base';          i = i + 1;
        aPath.i.sPId = 'ddkvideo';                  aPath.i.sPath = 'f:\ddk\april02\video';         i = i + 1;
        aPath.i.sPId = 'doxygen';                   aPath.i.sPath = 'f:\doxygen\v1.2.11-OS2';       i = i + 1;
        aPath.i.sPId = 'home';                      aPath.i.sPath = 'e:\user\kso';                  i = i + 1;
        aPath.i.sPId = 'mscv6-16';                  aPath.i.sPath = 'f:\ddktools\toolkits\msc60';   i = i + 1;
        aPath.i.sPId = 'mscv7-16';                  aPath.i.sPath = 'f:\msc\v7.0';                  i = i + 1;
        aPath.i.sPId = 'mysql';                     aPath.i.sPath = 'f:\mysql2';                    i = i + 1;
        aPath.i.sPId = 'nasm9833';                  aPath.i.sPath = 'f:\nasm\os2\0.98.33';          i = i + 1;
        aPath.i.sPId = 'netqos2';                   aPath.i.sPath = 'f:\netqos2';                   i = i + 1;
        aPath.i.sPId = 'perl50xxx';                 aPath.i.sPath = 'f:\perllib';                   i = i + 1;
        aPath.i.sPId = 'perl580';                   aPath.i.sPath = 'f:\perl\v5.8.0';               i = i + 1;
        aPath.i.sPId = 'python';                    aPath.i.sPath = 'f:\python';                    i = i + 1;
        aPath.i.sPId = 'toolkit40';                 aPath.i.sPath = 'f:\toolkit\v40csd4';           i = i + 1;
        aPath.i.sPId = 'toolkit45';                 aPath.i.sPath = 'f:\toolkit\v45';               i = i + 1;
        aPath.i.sPId = 'toolkit451';                aPath.i.sPath = 'f:\toolkit\v451';              i = i + 1;
        aPath.i.sPId = 'toolkit452';                aPath.i.sPath = 'f:\toolkit\v452';              i = i + 1;
        aPath.i.sPId = 'unixroot';                  aPath.i.sPath = 'e:\unix';                      i = i + 1;
        aPath.i.sPId = 'xfree86';                   aPath.i.sPath = 'e:\xfree86';                   i = i + 1;
        aPath.i.sPId = 'vac308';                    aPath.i.sPath = 'f:\vacpp\v308';                i = i + 1;
        aPath.i.sPId = 'vac365';                    aPath.i.sPath = 'f:\vacpp\v365';                i = i + 1;
        aPath.i.sPId = 'vac40';                     aPath.i.sPath = 'f:\ibmcpp40';                  i = i + 1;
        aPath.i.sPId = 'warpin';                    aPath.i.sPath = 'f:\warpin';                    i = i + 1;
        aPath.i.sPId = 'watcom11';                  aPath.i.sPath = 'f:\watcom';                    i = i + 1;
        aPath.i.sPId = 'watcom11c';                 aPath.i.sPath = 'f:\watcom11c';                 i = i + 1;
        aPath.i.sPId = 'testcase_drive_unused';     aPath.i.sPath = 'l'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_fixed';      aPath.i.sPath = 'c'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_floppy';     aPath.i.sPath = 'a'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_cdrom';      aPath.i.sPath = 'k'; /* optional */             i = i + 1;
        aPath.i.sPId = 'testcase_drive_network';    aPath.i.sPath = 's'; /* optional */             i = i + 1;
        aPath.i.sPId = 'testcase_drive_ramdisk';    aPath.i.sPath = 'r'; /* optional */             i = i + 1;
        /*aPath.i.sPId = '';          aPath.i.sPath =      i = i + 1;*/
    end


    /*
     * Bird: laptop box.
     */
    if (translate(EnvGet('HOSTNAME')) = 'DELIRIUM') then
    do
        say 'Info: No or empty path file, using birds work defaults.';
        aPath.i.sPId = 'cvs';                       aPath.i.sPath = 'e:\dev\cvs\v11.1';             i = i + 1;
        aPath.i.sPId = 'emx';                       aPath.i.sPath = 'e:\emx';                       i = i + 1;
        aPath.i.sPId = 'emxpgcc';                   aPath.i.sPath = 'e:\dev\emxpgcc\v2.95.2';       i = i + 1;
        aPath.i.sPId = 'gcc303';                    aPath.i.sPath = 'e:\dev\gcc\v3.0.3\emx';        i = i + 1;
        aPath.i.sPId = 'gcc321';                    aPath.i.sPath = 'e:\dev\gcc\v3.2.1\emx';        i = i + 1;
      /*aPath.i.sPId = 'db2v52';                    aPath.i.sPath = 'e:\sqllib52';                  i = i + 1;
        aPath.i.sPId = 'icatgam';                   aPath.i.sPath = 'e:\icatos2';                   i = i + 1;
        aPath.i.sPId = 'icatgam406rc1';             aPath.i.sPath = 'e:\icatos2.4.0.6.rc1';         i = i + 1;
        aPath.i.sPId = 'icatpe';                    aPath.i.sPath = 'e:\icatpe';                    i = i + 1;
        aPath.i.sPId = 'ida38';                     aPath.i.sPath = 'e:\ida38';                     i = i + 1;
        aPath.i.sPId = 'ida40';                     aPath.i.sPath = 'e:\ida401';                    i = i + 1; */
        aPath.i.sPId = 'ida414';                    aPath.i.sPath = 'e:\dev\ida\v414';                    i = i + 1;
      /*aPath.i.sPId = 'idasdk';                    aPath.i.sPath = 'e:\idasdk';                    i = i + 1; */
        aPath.i.sPId = 'ddk';                       aPath.i.sPath = 'e:\dev\ddk\june02';                       i = i + 1;
        aPath.i.sPId = 'ddkbase';                   aPath.i.sPath = 'e:\dev\ddk\june02\base';                  i = i + 1;
        aPath.i.sPId = 'ddkvideo';                  aPath.i.sPath = 'e:\dev\ddk\june02\video';                 i = i + 1;
        aPath.i.sPId = 'home';                      aPath.i.sPath = 'e:\home';                      i = i + 1;
        aPath.i.sPId = 'mscv6-16';                  aPath.i.sPath = 'e:\dev\ddktools\toolkits\msc60';   i = i + 1;
      /*aPath.i.sPId = 'mscv7-16';                  aPath.i.sPath = 'e:\msc\v7.0';                  i = i + 1;
        aPath.i.sPId = 'mysql';                     aPath.i.sPath = 'e:\mysql2';                    i = i + 1;
        aPath.i.sPId = 'netqos2';                   aPath.i.sPath = 'e:\netqos2';                   i = i + 1;
        aPath.i.sPId = 'perl50xxx';                 aPath.i.sPath = 'e:\perllib';                   i = i + 1;
        aPath.i.sPId = 'perl580';                   aPath.i.sPath = 'e:\dev\perl\v5.8.0';           i = i + 1;
        aPath.i.sPId = 'python';                    aPath.i.sPath = 'e:\python';                    i = i + 1;
        aPath.i.sPId = 'toolkit40';                 aPath.i.sPath = 'e:\toolkit';                   i = i + 1;
        aPath.i.sPId = 'toolkit45';                 aPath.i.sPath = 'e:\toolkit45';                 i = i + 1;
        aPath.i.sPId = 'toolkit451';                aPath.i.sPath = 'e:\toolkit451';                i = i + 1; */
        aPath.i.sPId = 'toolkit452';                aPath.i.sPath = 'e:\dev\toolkit\v452';                i = i + 1;
        aPath.i.sPId = 'unixroot';                  aPath.i.sPath = 'e:\unix';                      i = i + 1;
        aPath.i.sPId = 'xfree86';                   aPath.i.sPath = 'e:\xfree86';                   i = i + 1;
        aPath.i.sPId = 'vac308';                    aPath.i.sPath = 'e:\dev\vacpp\v308';                 i = i + 1;
        aPath.i.sPId = 'vac365';                    aPath.i.sPath = 'e:\dev\vacpp\v365';                   i = i + 1;
      /*aPath.i.sPId = 'vac40';                     aPath.i.sPath = 'e:\ibmcpp40';                  i = i + 1;*/
        aPath.i.sPId = 'warpin';                    aPath.i.sPath = 'e:\warpin';                    i = i + 1;
      /*aPath.i.sPId = 'watcom11';                  aPath.i.sPath = 'e:\watcom';                    i = i + 1;*/
/*        aPath.i.sPId = 'watcom11c';                 aPath.i.sPath = 'e:\dev\watcom\v11c';                 i = i + 1; */
        aPath.i.sPId = 'testcase_drive_unused';     aPath.i.sPath = 't'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_fixed';      aPath.i.sPath = 'd'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_floppy';     aPath.i.sPath = 'a'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_cdrom';      aPath.i.sPath = 'f'; /* optional */             i = i + 1;
        aPath.i.sPId = 'testcase_drive_network';    aPath.i.sPath = 'x'; /* optional */             i = i + 1;
        /*aPath.i.sPId = 'testcase_drive_ramdisk';    aPath.i.sPath = '';  /* optional */             i = i + 1;*/
        /*aPath.i.sPId = '';          aPath.i.sPath =      i = i + 1;*/
    end


    /*
     * Bird: work boxes.
     */
    if ((translate(EnvGet('HOSTNAME')) = 'DREAM')  | (translate(EnvGet('HOSTNAME')) = 'DESPAIR')) then
    do
        say 'Info: No or empty path file, using birds work defaults.';
        aPath.i.sPId = 'cvs';                       aPath.i.sPath = 'd:\dev\cvs\v11.1';             i = i + 1;
        aPath.i.sPId = 'emx';                       aPath.i.sPath = 'd:\emx';                       i = i + 1;
        aPath.i.sPId = 'emxpgcc';                   aPath.i.sPath = 'd:\dev\emxpgcc\v2.95.2';       i = i + 1;
        aPath.i.sPId = 'gcc303';                    aPath.i.sPath = 'd:\dev\gcc\v3.0.3\emx';        i = i + 1;
        aPath.i.sPId = 'gcc321';                    aPath.i.sPath = 'd:\dev\gcc\v3.2.1\emx';        i = i + 1;
      /*aPath.i.sPId = 'db2v52';                    aPath.i.sPath = 'e:\sqllib52';                  i = i + 1;
        aPath.i.sPId = 'icatgam';                   aPath.i.sPath = 'e:\icatos2';                   i = i + 1;
        aPath.i.sPId = 'icatgam406rc1';             aPath.i.sPath = 'e:\icatos2.4.0.6.rc1';         i = i + 1;
        aPath.i.sPId = 'icatpe';                    aPath.i.sPath = 'e:\icatpe';                    i = i + 1;
        aPath.i.sPId = 'ida38';                     aPath.i.sPath = 'e:\ida38';                     i = i + 1;
        aPath.i.sPId = 'ida40';                     aPath.i.sPath = 'e:\ida401';                    i = i + 1; */
        aPath.i.sPId = 'ida414';                    aPath.i.sPath = 'd:\dev\ida\v414';              i = i + 1;
      /*aPath.i.sPId = 'idasdk';                    aPath.i.sPath = 'e:\idasdk';                    i = i + 1; */
        aPath.i.sPId = 'java131';                   aPath.i.sPath = 'd:\java131';                   i = i + 1;
        aPath.i.sPId = 'ddk';                       aPath.i.sPath = 'd:\dev\ddk\june02';            i = i + 1;
        aPath.i.sPId = 'ddkbase';                   aPath.i.sPath = 'd:\dev\ddk\june02\base';       i = i + 1;
        aPath.i.sPId = 'ddkvideo';                  aPath.i.sPath = 'd:\dev\ddk\june02\video';      i = i + 1;
        aPath.i.sPId = 'home';                      aPath.i.sPath = 'd:\home\bird';                 i = i + 1;
        aPath.i.sPId = 'mscv6-16';                  aPath.i.sPath = 'd:\dev\ddktools\toolkits\msc60'; i = i + 1;
        aPath.i.sPId = 'mscv7-16';                  aPath.i.sPath = 'd:\dev\msc\v7.0';              i = i + 1;
        aPath.i.sPId = 'mysql';                     aPath.i.sPath = 'd:\apps\mysql\v3.23.50b1';     i = i + 1;
      /*aPath.i.sPId = 'netqos2';                   aPath.i.sPath = 'e:\netqos2';                   i = i + 1;*/
        aPath.i.sPId = 'perl50xxx';                 aPath.i.sPath = 'd:\dev\perl\v5.00455';         i = i + 1;
        aPath.i.sPId = 'perl580';                   aPath.i.sPath = 'd:\dev\perl\v5.8.0';           i = i + 1;
      /*aPath.i.sPId = 'python';                    aPath.i.sPath = 'e:\python';                    i = i + 1;*/
        aPath.i.sPId = 'toolkit40';                 aPath.i.sPath = 'd:\dev\toolkit\v40csd1';       i = i + 1;
      /*aPath.i.sPId = 'toolkit45';                 aPath.i.sPath = 'e:\toolkit45';                 i = i + 1;
        aPath.i.sPId = 'toolkit451';                aPath.i.sPath = 'e:\toolkit451';                i = i + 1; */
        aPath.i.sPId = 'toolkit452';                aPath.i.sPath = 'd:\dev\toolkit\v452';          i = i + 1;
        aPath.i.sPId = 'unixroot';                  aPath.i.sPath = 'd:\unix';                      i = i + 1;
        aPath.i.sPId = 'xfree86';                   aPath.i.sPath = 'd:\xfree86';                   i = i + 1;
        aPath.i.sPId = 'vac308';                    aPath.i.sPath = 'd:\dev\VACpp\v308';            i = i + 1;
        aPath.i.sPId = 'vac365';                    aPath.i.sPath = 'd:\dev\VACpp\v365';            i = i + 1;
        aPath.i.sPId = 'vac40';                     aPath.i.sPath = 'd:\dev\VACpp\v40ga';           i = i + 1;
        aPath.i.sPId = 'warpin';                    aPath.i.sPath = 'c:\warpin';                    i = i + 1;
        aPath.i.sPId = 'watcom11';                  aPath.i.sPath = 'd:\dev\watcom\v110';           i = i + 1;
        aPath.i.sPId = 'watcom11c';                 aPath.i.sPath = 'd:\dev\watcom\v110c';          i = i + 1;
        aPath.i.sPId = 'testcase_drive_unused';     aPath.i.sPath = 't'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_fixed';      aPath.i.sPath = 'f'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_floppy';     aPath.i.sPath = 'a'; /* reqired */              i = i + 1;
        aPath.i.sPId = 'testcase_drive_cdrom';      aPath.i.sPath = 'g'; /* optional */             i = i + 1;
        aPath.i.sPId = 'testcase_drive_network';    aPath.i.sPath = 'x'; /* optional */             i = i + 1;
        aPath.i.sPId = 'testcase_drive_ramdisk';    aPath.i.sPath = 'r'; /* optional */             i = i + 1;
        /*aPath.i.sPId = '';          aPath.i.sPath =      i = i + 1;*/
    end

    /* add your own stuff here.. */
    aPath.0 = i - 1;
return 0;


/**
 * Configure a path.
 * @returns Path on success.
 *          '' on failure.
 * @param   sOperation  The operation - 'config' or 'forcedconfig'
 * @param   sPathId     The path to configure.
 * @param   sToolId     The tool Id.
 */
PathConfig: procedure expose aCfg. aPath. sPathFile
    parse arg sOperation, sPathId, sToolId

    /*
     * If not forced we'll ask first.
     */
    if (sOperation <> 'forcedconfig') then
    do
        say 'Do you want to configure the path '''sPathId''/* for the '''sToolId'''('CfgDesc(sToolId)') tool?*/ '(y/N)';
        sAnswer = PullUser(1);
        if (substr(strip(sAnswer),1,1) <> 'Y') then
            return '';
    end

    /*
     * Config loop.
     */
    do i = 1 to 128

        say 'Give us the path for '''sPathId'''('''sToolId'''/'CfgDesc(sToolId)'):'
        sThePath = translate(strip(strip(strip(PullUser()), 'T','\'),'T','/'), '\', '/');
        /*say 'Debug: sThePath='sThePath;*/
        if ((sThePath <> '') & (sThePath = translate(sThePath,'','#!$@%|<>;ı&œ='))) then
        do
            /*
             * Add it to internal struct.
             */
            call PathRemove(sPathId);
            j = aPath.0 + 1;
            aPath.j.sPId = strip(sPathId);
            aPath.j.sPath = translate(strip(strip(strip(sThePath), 'T','\'),'T','/'), '\', '/');
            aPath.0 = j;
            return sThePath;
        end
        else
            say 'error: invalid path name.';
        say 'Debug 9'

        /* ask if retry */
        if (i >= 2) then
            say 'You''re not trying hard, are you?';
        say 'Wanna try giving us an *valid* path for the path '''sPathId''' for the '''sToolId'''('CfgDesc(sToolId)') tool? (y/N)';
        sAnswer = PullUser(1);
        if (substr(strip(sAnswer),1,1) <> 'Y') then
            leave;
    end

    say 'Giving up!';
return '';


/**
 * Get user response and empties the input queue.
 * @returns     User input.
 * @param       fUpper  If present and set uppercase the user response.
 */
PullUser: procedure
    parse arg fUpper
    if (fUpper = '') then
        fUpper = 0;

    signal on halt name PullUser_Handler
    signal on syntax name PullUser_Handler
    signal on notready name PullUser_Handler
    parse pull sAnswer;
    signal off syntax
    signal off halt
    signal off notready
    /*say 'Debug: sAnswer='c2x(sAnswer);
    sAnswer = strip(strip(sAnswer, 'T', '0A'x), 'T', '0D'x);*/

    if (fUpper) then
        sAnswer = translate(sAnswer);
    /* flush input */
    do while (Queued())
        pull dummy;
    end
return sAnswer;


/**
 * No value handler
 */
PullUser_Handler:
    say 'fatal error: Believe Ctrl-Break/C might have been pressed.';
    signal off syntax
    signal off halt
    signal off syntax
    signal off notready
    do while (Queued())
        pull dummy;
    end
exit(16);


/**
 * Checks if a file exists.
 * @param   sFile       Name of the file to look for.
 * @param   sComplain   Complaint text. Complain if non empty and not found.
 * @returns TRUE if file exists.
 *          FALSE if file doesn't exists.
 */
FileExists: procedure
    parse arg sFile, sComplain
    rc = stream(sFile, 'c', 'query exist');
    if ((rc = '') & (sComplain <> '')) then
        say sComplain ''''sFile'''.';
return rc <> '';


/**
 * Checks if a directory exists.
 * @param   sDir        Name of the directory to look for.
 * @param   sComplain   Complaint text. Complain if non empty and not found.
 * @returns TRUE if file exists.
 *          FALSE if file doesn't exists.
 */
DirExists: procedure
    parse arg sDir, sComplain
    rc = SysFileTree(sDir, 'sDirs', 'DO');
    if (rc = 0 & sDirs.0 = 1) then
        return 1;
    if (sComplain <> '') then do
        say sComplain ''''sDir'''.';
return 0;


/**
 * Add sToAdd in front of sEnvVar.
 * Note: sToAdd now is allowed to be alist!
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvAddFront: procedure
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
        rc = EnvAddFront2(fRM, sEnvVar, substr(sToAdd, j+1, i - j), sSeparator);
        i = j;
    end

return rc;

/**
 * Add sToAdd in front of sEnvVar.
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvAddFront2: procedure
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
    sOrgEnvVar = EnvGet(sEnvVar);

    /* Remove previously sToAdd if exists. (Changing sOrgEnvVar). */
    i = pos(translate(sToAdd), translate(sOrgEnvVar));
    if (i > 0) then
        sOrgEnvVar = substr(sOrgEnvVar, 1, i-1) || substr(sOrgEnvVar, i + length(sToAdd));

    /* set environment */
    if (fRM) then
        return EnvSet(0, sEnvVar, sOrgEnvVar);
return EnvSet(0, sEnvVar, sToAdd||sOrgEnvVar);


/**
 * Add sToAdd as the end of sEnvVar.
 * Note: sToAdd now is allowed to be alist!
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvAddEnd: procedure
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
        rc = EnvAddEnd2(fRM, sEnvVar, substr(sToAdd, j+1, i - j), sSeparator);
        i = j;
    end

return rc;

/**
 * Add sToAdd as the end of sEnvVar.
 *
 * Known features: Don't remove sToAdd from original value if sToAdd
 *                 is at the end and don't end with a ';'.
 */
EnvAddEnd2: procedure
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
    sOrgEnvVar = EnvGet(sEnvVar);

    if (sOrgEnvVar <> '') then
    do
        /* Remove previously sToAdd if exists. (Changing sOrgEnvVar). */
        i = pos(translate(sToAdd), translate(sOrgEnvVar));
        if (i > 0) then
            sOrgEnvVar = substr(sOrgEnvVar, 1, i-1) || substr(sOrgEnvVar, i + length(sToAdd));

        /* checks that sOrgEnvVar ends with a separator. Adds one if not. */
        if (sOrgEnvVar = '') then
            if (right(sOrgEnvVar,1) <> sSeparator) then
                sOrgEnvVar = sOrgEnvVar || sSeparator;
    end

    /* set environment */
    if (fRM) then return EnvSet(0, sEnvVar, sOrgEnvVar);
return EnvSet(0, sEnvVar, sOrgEnvVar||sToAdd);


/**
 * Sets sEnvVar to sValue.
 */
EnvSet: procedure
    parse arg fRM, sEnvVar, sValue

    /* if we're to remove this, make valuestring empty! */
    if (fRM) then
        sValue = '';
    sEnvVar = translate(sEnvVar);

    /*
     * Begin/EndLibpath fix:
     *      We'll have to set internal these using both commandline 'SET'
     *      and internal VALUE in order to export it and to be able to
     *      get it (with EnvGet) again.
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
EnvGet: procedure
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
        Address CMD '@set dummyenvvar'||i'=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    end
    do i = 1 to 100
        Address CMD '@set dummyenvvar'||i'=';
    end
return 0;


/**
 * Execute a command and match output and return code.
 *
 * @returns  0 on match.
 *          49 on return code mismatch.
 *          99 on output mistmatch.
 * @param   sCmd                    The command to execute.
 * @param   rcCmdExepcted           The expected return code from the command.
 * @param   sOutputPartExpected     A 'needle' of the output 'haystack'.
 */
CheckCmdOutput: procedure
    parse arg sCmd, rcCmdExpected, fQuiet, sOutputPartExpected

    /*
     * Try execute the command
     */
    queTmp = RxQueue('Create');
    queOld = RxQueue('Set', queTmp);
    Address CMD sCmd || ' 2>&1 | RxQueue' queTmp;
    rcCmd = rc;

    /* get output */
    sOutput = '';
    do while (queued() > 0)
        parse pull sLine
        sOutput = sOutput || sLine || '0d0a'x
    end
    call RxQueue 'Delete', RxQueue('Set', queOld);

    /*
     * If command
     */
    rc = 0;
    if (/*rcCmd = rcCmdExpected*/ 1) then /* doesn't work with cmd.exe */
    do
        if (pos(sOutputPartExpected, sOutput) <= 0) then
        do
            say 'Debug - start'
            say 'Debug:' sOutputPartExpected
            say 'Debug: not found in:'
            say sOutput
            say 'Debug - end'
            rc = 99
        end
    end
    else
        rc = 49

    if (\fQuiet & rc <> 0) then
        say 'Debug:' sCmd 'rc='rc' rcCmd='rcCmd 'rcCmdExpected='rcCmdExpected;
return rc;


/**
 * Checks syslevel info.
 * @returns 0 if match.
 *          <>0 if mismatch.
 * @param   sFile           Name of the syslevel file.
 * @param   fQuiet          Quiet / verbose flag.
 * @param   sMatchCid       Component id. (optional)
 * @param   sMatchVer       Version id. (optional)
 * @param   sMatchLevel     Current Level. (optional)
 * @param   sMatchTitle     Product title. (optional)
 * @param   sMatchKind      Product kind. (optional)
 * @param   sMatchType      Product type. (optional)
 */
CheckSyslevel: procedure
parse arg sFile, fQuiet, sMatchCId,sMatchVer,sMatchLevel,sMatchTitle,iMatchKind,sMatchType,dummy

    iRc = -1;

    /* Open the file */
    rc = stream(sFile, 'c', 'open read');
    if (pos('READY', rc) = 1) then
    do
        if (charin(sFile, 1, 11) = 'FF'x'FF'x'SYSLEVEL'||'00'x) then
        do
            /* read base offset (binary long) */
            iBase = c2x(charin(sFile, 34, 4));
            iBase = 1 + x2d(right(iBase,2)||substr(iBase,5,2)||substr(iBase,3,2)||left(iBase,2));

            /* Read fields...
             *
             *  typedef struct _SYSLEVELDATA {      offset
             *     unsigned char d_reserved1[2];     0
             *     unsigned char d_kind;             2
             *     unsigned char d_version[2];       3
             *     unsigned char d_reserved2[2];     5
             *     unsigned char d_clevel[7];        7
             *     unsigned char d_reserved3;       14
             *     unsigned char d_plevel[7];       15
             *     unsigned char d_reserved4;       22
             *     unsigned char d_title[80];       23
             *     unsigned char d_cid[9];         103
             *     unsigned char d_revision;       112
             *     unsigned char d_type[1];        113
             *  } SYSLEVELDATA;
             */
            iKind       =   c2d(charin(sFile, iBase+  2,  1));
            iVer        =       charin(sFile, iBase+  3,  2);
            sCurLevel   = strip(charin(sFile, iBase+  7,  7), 'T', '00'x);
            sPreLevel   = strip(charin(sFile, iBase+ 15,  7), 'T', '00'x);
            sTitle      = strip(charin(sFile, iBase+ 23, 80), 'T', '00'x);
            sCId        =       charin(sFile, iBase+103,  9);
            iRev        =       charin(sFile, iBase+112,  1);
            sType       = strip(charin(sFile, iBase+113, 10), 'T', '00'x);

            sVer = substr(c2x(substr(iVer, 1, 1)), 1, 1)||,
                   '.'||,
                   substr(c2x(substr(iVer, 1, 1)), 2, 1)||,
                   d2c(c2d(substr(iVer, 2, 1)) + 48);
            if (iRev <> 0) then
                sVer = sVer ||'.'|| d2c(c2d(iRev) + 48);

            /*
             * Compare.
             */
            iRc = 0;
            if (sMatchCId <> '' & sMatchCId <> sCid) then
            do
                if (\fQuiet) then
                    say 'syslevel '''sFile''': cid '''sCId''' != '''sMatchCId'''.';
                iRc = 2;
            end
            if (sMatchVer <> '' & sMatchVer <> sVer) then
            do
                if (\fQuiet) then
                    say 'syslevel '''sFile''': ver '''sVer''' != '''sMatchVer'''.';
                iRc = 3;
            end
            if (sMatchLevel <> '' & sMatchLevel <> sCurLevel) then
            do
                if (\fQuiet) then
                    say 'syslevel '''sFile''': level '''sCurLevel''' != '''sMatchLevel'''.';
                iRc = 4;
            end
            if (sMatchTitle <> '' & sMatchTitle <> sTitle) then
            do
                if (\fQuiet) then
                    say 'syslevel '''sFile''': title '''sTitle''' != '''sMatchTitle'''.';
                iRc = 5;
            end
            if (iMatchKind <> '' & iMatchKind <> iKind) then
            do
                if (\fQuiet) then
                    say 'syslevel '''sFile''': kind '''iKind''' != '''iMatchKind'''.';
                iRc = 6;
            end
            if (sMatchType <> '' & sMatchType <> sType) then
            do
                if (\fQuiet) then
                    say 'syslevel '''sFile''': type '''sType''' != '''sMatchType'''.';
                iRc = 7;
            end
            /*
            say 'debug:'
            say 'iKind       =' iKind
            say 'sCurLevel   =' sCurLevel
            say 'sPreLevel   =' sPreLevel
            say 'sTitle      =' sTitle
            say 'sCId        =' sCId
            say 'sType       =' sType
            say 'sVer        =' sVer
            */
        end
        else
            say 'bad signature';

        /* finished, close file */
        call stream sFile, 'c', 'close';
    end
    else say 'open failed, rc='rc;
return iRc;



/**
 * Tool procedures section
 * @returns 0 on success.
 *          1 if PathQuery() failed.
 *          2 if some vital file/dir wasn't found in the config verify.
 *          49 if verify command rc mismatched.
 *          99 if verify command output mismatched.
 **/


/*
 * Concurrent Versions System (CVS)
 */
CVS: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet


    /*
     * The directories.
     */
    sPathCVS = PathQuery('cvs', sToolId, sOperation);
    if (sPathCVS = '') then
        return 1;
    sPathHome = PathQuery('home', sToolId, sOperation);
    if (sPathHome = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_CVS',    sPathCVS;
    call EnvAddFront fRM, 'path',        sPathCVS'\bin;'
    call EnvAddFront fRM, 'bookshelf',   sPathCVS'\book;'
    call EnvAddFront fRM, 'bookshelf',   sPathCVS'\book;'
    call EnvSet      fRM, 'home',        translate(sPathHome, '/','\');

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;

    if (\CfgVerifyFile(sPathCVS'\bin\cvs.exe',fQuiet)) then
            return 2;
    if (length(sPathHome) <= 2) then
    do
        if (\fQuiet) then
            say 'Error: The home directory is to short!';
        return 2;
    end
    if (\CfgVerifyDir(sPathHome, fQuiet)) then
        return 2;
return CheckCmdOutput('cvs --version', 0, fQuiet, 'Concurrent Versions System (CVS) 1.1');


/*
 * EMX
 */
EMX: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * EMX/GCC main directory.
     */
    sEMX = PathQuery('emx', sToolId, sOperation);
    if (sEMX = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    sEMXBack    = translate(sEMX, '\', '/');
    sEMXForw    = translate(sEMX, '/', '\');
    call EnvSet      fRM, 'PATH_EMX', sEMXBack;
    call EnvSet      fRM, 'CCENV',      'EMX'
    call EnvSet      fRM, 'BUILD_ENV',  'EMX'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvAddFront fRM, 'BEGINLIBPATH',       sEMXBack'\dll;'
    call EnvAddFront fRM, 'PATH',               sEMXBack'\bin;'
    call EnvAddFront fRM, 'DPATH',              sEMXBack'\book;'
    call EnvAddFront fRM, 'BOOKSHELF',          sEMXBack'\book;'
    call EnvAddFront fRM, 'HELP',               sEMXBack'\help;'
    call EnvAddFront fRM, 'C_INCLUDE_PATH',     sEMXForw'/include'
    call EnvAddFront fRM, 'LIBRARY_PATH',       sEMXForw'/lib'
    call EnvAddFront fRM, 'CPLUS_INCLUDE_PATH', sEMXForw'/include/cpp;'sEMXForw'/include'
    call EnvSet      fRM, 'PROTODIR',           sEMXForw'/include/cpp/gen'
    call EnvSet      fRM, 'OBJC_INCLUDE_PATH',  sEMXForw'/include'
    call EnvSet      fRM, 'GCCLOAD',            '5'
    call EnvSet      fRM, 'GCCOPT',             '-pipe'
    call EnvAddFront fRM, 'INFOPATH',           sEMXForw'/info'
    call EnvSet      fRM, 'EMXBOOK',            'emxdev.inf+emxlib.inf+emxgnu.inf+emxbsd.inf'
    call EnvAddFront fRM, 'HELPNDX',            'emxbook.ndx', '+', 1
    call EnvSet      fRM, 'EMXOPT',             '-c -n -h1024'
    if EnvGet('TERM') = '' then do
        call EnvSet  fRM, 'TERM',               'ansi'
        call EnvSet  fRM, 'TERMCAP',            sEMXForw'/etc/termcap.dat'
    end

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sEmxBack'\bin\gcc.exe', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\bin\emxomf.exe', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\bin\emxrev.cmd', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\mt\c.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\mt\c.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\mt\sys.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\mt\emx.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\mt\emx.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\mt\c_import.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\mt\c_import.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\c_alias.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\c_alias.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\emx2.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\emx2.lib', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('gcc --version', 0, fQuiet, '2.8.1');
    if (rc = 0) then
        rc = CheckCmdOutput('emxrev.cmd', 0, fQuiet,,
                            'EMX : revision = 61'||'0d0a'x ||,
                            'EMXIO : revision = 60'||'0d0a'x||,
                            'EMXLIBC : revision = 63'||'0d0a'x||,
                            'EMXLIBCM : revision = 64'||'0d0a'x||,
                            'EMXLIBCS : revision = 64'||'0d0a'x||,
                            'EMXWRAP : revision = 60'||'0d0a'x);
    return rc;
return 0;


/*
 * EMX PGCC - must be installed on to the ordinar EMX.
 */
EMXPGCC: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * EMX/GCC main directory.
     */
    sEMXPGCC    = PathQuery('emxpgcc', sToolId, sOperation);
    if (sEMXPGCC = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    sEMXBack    = translate(sEMXPGCC, '\', '/');
    sEMXForw    = translate(sEMXPGCC, '/', '\');
    call EnvSet      fRM, 'PATH_EMXPGCC',   sEMXBack;
    call EnvSet      fRM, 'CCENV',          'EMX'
    call EnvSet      fRM, 'BUILD_ENV',      'EMX'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvAddFront fRM, 'BEGINLIBPATH',       sEMXBack'\dll;'
    call EnvAddFront fRM, 'PATH',               sEMXBack'\bin;'
    call EnvAddFront fRM, 'DPATH',              sEMXBack'\book;'
    call EnvAddFront fRM, 'BOOKSHELF',          sEMXBack'\book;'
    call EnvAddFront fRM, 'HELP',               sEMXBack'\help;'
    call EnvAddFront fRM, 'C_INCLUDE_PATH',     sEMXForw'/include'
    call EnvAddFront fRM, 'LIBRARY_PATH',       sEMXForw'/lib'
    call EnvAddFront fRM, 'CPLUS_INCLUDE_PATH', sEMXForw'/include/cpp;'sEMXForw'/include'
    call EnvSet      fRM, 'PROTODIR',           sEMXForw'/include/cpp/gen'
    call EnvSet      fRM, 'OBJC_INCLUDE_PATH',  sEMXForw'/include'
    call EnvAddFront fRM, 'INFOPATH',           sEMXForw'/info'
    call EnvSet      fRM, 'EMXBOOK',            'emxdev.inf+emxlib.inf+emxgnu.inf+emxbsd.inf'
    call EnvAddFront fRM, 'HELPNDX',            'emxbook.ndx', '+', 1

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sEmxBack'\bin\gcc.exe', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\bin\g++.exe', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\bin\as.exe', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\bin\emxomf.exe', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\gcc29160.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\gcc29160.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\iberty.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\iberty.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\iberty_s.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\iberty_s.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\opcodes.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\opcodes.lib', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\opcodes_s.a', fQuiet),
        |   \CfgVerifyFile(sEmxBack'\lib\opcodes_s.lib', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('gcc --version', 0, fQuiet, 'pgcc-2.95.2');
    if (rc = 0) then
        rc = CheckCmdOutput('g++ --version', 0, fQuiet, 'pgcc-2.95.2');
    if (rc = 0) then
        rc = CheckCmdOutput('as --version', 0, fQuiet, 'GNU assembler 2.9.1');
return rc;


/*
 * FreeType v1.3.1 EMX release.
 */
FreeTypeEMX: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    sPathFreeType = PathQuery('freetypeemx', sToolId, sOperation);
    if (sPathFreeType = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;
    call EnvSet      fRm, 'PATH_FREETYPE', sPathFreeType;
    call EnvAddFront fRm, 'beginlibpath',sPathFreeType'\dll;'
    call EnvAddFront fRm, 'path',        sPathFreeType'\bin;'
    call EnvAddFront fRM, 'include',     sPathFreeType'\include;'
    call EnvAddFront fRM, 'C_INCLUDE_PATH', sPathFreeType'\include;'
    call EnvAddFront fRM, 'lib',         sPathFreeType'\lib;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;

    if (    \CfgVerifyFile(sPathFreeType'\bin\ftsbit.exe', fQuiet),
        |   \CfgVerifyFile(sPathFreeType'\bin\ftzoom.exe', fQuiet),
        |   \CfgVerifyFile(sPathFreeType'\dll\ttf.dll', fQuiet),
        ) then
        return 2;
return 0;


/*
 * IBM DB2 v5.2
 */
db2v52: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    sPathDB2   = PathQuery('db2v52', sToolId, sOperation);
    if (sPathDB2 = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;
    call EnvSet      fRm, 'PATH_DB2',    sPathDB2;
    call EnvSet      fRm, 'db2path',     sPathDB2;
    call EnvAddFront fRm, 'beginlibpath',sPathDB2'\dll;'sPathDB2'\alt;'
    call EnvAddFront fRm, 'path',        sPathDB2'\bin;'sPathDB2'\alt;'
    call EnvAddFront fRm, 'dpath',       sPathDB2'\bin;'sPathDB2';'
    call EnvAddFront fRm, 'help',        sPathDB2'\help;'
    call EnvAddEnd   fRm, 'classpath',   '.;'sPathDB2'\JAVA\DB2JAVA.ZIP;'sPathDB2'\JAVA\RUNTIME.ZIP;'sPathDB2'\JAVA\SQLJ.ZIP;'
    call EnvSet      fRM, 'db2instace',  'DB2'
    /*call EnvSet      fRM, 'odbc_path',   'f:\odbc' -- huh? what's this? */
    call EnvAddFront fRM, 'cobcpy',      sPathDB2'\include\cobol_mf'
    call EnvSet      fRM, 'finclude',    sPathDB2'\include'
    call EnvAddFront fRM, 'include',     sPathDB2'\include;'
    call EnvAddFront fRM, 'lib',         sPathDB2'\lib;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;

    if (    \CfgVerifyFile(sPathDB2'\bin\db2.exe', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\bin\sqlbind.exe', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\bin\sqlprep.exe', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\lib\db2api.lib', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\lib\db2cli.lib', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\lib\db2gmf32.lib', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\include\sql.h', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\include\sqlcodes.h', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\include\sqlsystm.h', fQuiet),
        |   \CfgVerifyFile(sPathDB2'\include\sqlcli.h', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('echo quit | db2', 0, fQuiet, 'Command Line Processor for DB2 SDK 5.2.0');
return rc;



/*
 *  Device Driver Kit (DDK) base.
 */
DDK: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Device Driver Kit (DDK) (v4.0+) Main Directory.
     */
    sPathDDK    = PathQuery('ddk', sToolId, sOperation);
    if (sPathDDK = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
    do
        /* Set the ddk subpaths */
        if (PathQuery('ddkbase', 'ddkbase', 'quietisconfig') = '') then
            call PathSet 'ddkbase', sPathDDK'\base';
        if (PathQuery('ddkvideo', 'ddkvideo', 'quietisconfig') = '') then
            call PathSet 'ddkvideo', sPathDDK'\video';
        if (PathQuery('ddkprint', 'ddkvideo', 'quietisconfig') = '') then
            call PathSet 'ddkprint', sPathDDK'\print';
        return 0;
    end
    call EnvSet      fRM, 'PATH_DDK',    sPathDDK;
    rc = DDKBase('ddkbase',sOperation,fRM,fQuiet)
    if (rc = 0) then
        rc = DDKVideo('ddkvideo',sOperation,fRM,fQuiet)
return rc;


/*
 *  Device Driver Kit (DDK) base.
 */
DDKBase: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Device Driver Kit (DDK) (v4.0+) base (important not main) directory.
     */
    sPathDDKBase    = PathQuery('ddkbase', sToolId, sOperation);
    if (sPathDDKBase = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;
    call EnvSet      fRM, 'PATH_DDKBASE',sPathDDKBase;
    call EnvAddFront fRM, 'path',        sPathDDKBase'\tools;'
    call EnvAddFront fRM, 'include',     sPathDDKBase'\h;'sPathDDKBase'\inc;'sPathDDKBase'\inc32;'
    call EnvAddFront fRM, 'include16',   sPathDDKBase'\h;'
    call EnvAddFront fRM, 'lib',         sPathDDKBase'\lib;'
    call EnvAddFront fRM, 'bookshelf',   sPathDDKBase'\..\docs;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathDDKBase'\tools\link.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\tools\link386.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\tools\cl386.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\tools\masm.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\tools\h2inc.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\tools\lib.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\os2286.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\os2286p.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\os2386.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\os2386p.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\doscalls.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\dhcalls.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\addcalls.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\rmcalls.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\lib\vdh.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\h\infoseg.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\h\include.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\h386\pmddi.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\h386\pmddim.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\h386\limits.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\h386\string.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\inc\v8086.inc', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\inc\sas.inc', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\inc\pmwinx.inc', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\inc\infoseg.inc', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\inc\devhlp.inc', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\inc\devhlpp.inc', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('cl386', 0, fQuiet, 'Microsoft (R) Microsoft 386 C Compiler. Version 6.00.054');
    if (rc = 0) then
        rc = CheckCmdOutput('masm nul,nul,nul,nul;', 2, fQuiet, 'Microsoft (R) Macro Assembler Version 5.10A.15 Jul 07 15:25:03 1989');
    if (rc = 0) then
        rc = CheckCmdOutput('h2inc -?', 0, fQuiet, 'h2inc - .H to .INC file translator (version 13.29)');
    if (rc = 0) then
        rc = CheckCmdOutput('type' sPathDDKBase'\inc\devhlp.inc', 0, fQuiet, 'DevHlp_ReadFileAt');
return rc;


/*
 *  Device Driver Kit (DDK) Video.
 */
DDKVideo: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Device Driver Kit (DDK) (v4.0+) Video (important not main) directory.
     */
    sPathDDKVideo   = PathQuery('ddkvideo', sToolId, sOperation);
    if (sPathDDKVideo = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;
    call EnvSet      fRM, 'PATH_DDKVIDEO',sPathDDKVideo;
    call EnvAddFront fRM, 'path',        sPathDDKVideo'\tools\os2.386\bin;'sPathDDKVideo'\tools\os2.386\lx.386\bin;' /* might not need this... */
    call EnvAddFront fRM, 'include',     sPathDDKVideo'\rel\os2c\include\base\os2;'/*sPathDDKVideo'\rel\os2c\include\base\os2\16bit;'sPathDDKVideo'\rel\os2c\include\base\os2\inc;'sPathDDKVideo'\rel\os2c\include\base\os2\inc32;' /* might be over kill!! */  - it is! */
    call EnvAddFront fRM, 'include16',   sPathDDKVideo'\rel\os2c\include\base\os2\16bit;'
    call EnvAddFront fRM, 'lib',         sPathDDKVideo'\rel\os2c\lib\os2;'sPathDDKVideo'\rel\os2c\lib\os2\priv;'
    call EnvAddFront fRM, 'bookshelf',   sPathDDKVideo'\..\docs;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\doscalls.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\gradd.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\os2386.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\libh.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\vdh.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\thunkrt.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\dbcs32.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\priv\pmwp.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\priv\os2286p.lib', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\vvga.def', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\lib\os2\vvga.def', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\include\base\os2\gradd.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\include\base\os2\pmwp.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\include\base\os2\os2p.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\include\base\os2\pmgpip.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\include\base\os2\pmdevp.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\rel\os2c\include\base\os2\inc32\pmp.inc', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\tools\os2.386\bin\rc.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\tools\os2.386\bin\nmake.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\tools\os2.386\bin\h2inc.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\tools\os2.386\lx.386\bin\link386.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\tools\os2.386\lx.386\bin\masm.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\tools\os2.386\lx.386\bin\masm.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKVideo'\tools\os2.386\lx.386\bin\mcl386\bin\c3_386.exe', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('nmake -?', 0, fQuiet, 'Version 2.001.000 Jan 28 1994');
    if (rc = 0) then
        rc = CheckCmdOutput('masm nul,nul,nul,nul;', 2, fQuiet, 'Microsoft (R) Macro Assembler Version 5.10A.15 Jul 07 15:25:03 1989');
    if (rc = 0) then
        rc = CheckCmdOutput('h2inc -?', 0, fQuiet, 'h2inc - .H to .INC file translator (version 13.29)');
    if (rc = 0) then
        rc = CheckCmdOutput('type 'sPathDDKVideo'\rel\os2c\include\base\os2\gradd.h', 0, fQuiet, 'GHI_CMD_POLYGON');
return rc;


/*
 * Doxygen v1.2.11.1 for OS/2.
 */
DoxyGen: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Get base directory.
     */
    sPathDoxyGen   = PathQuery('doxygen', sToolId, sOperation);
    if (sPathDoxyGen = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;
    call EnvSet      fRM, 'PATH_DOXYGEN',sPathDoxyGen;
    call EnvAddFront fRM, 'path',        sPathDoxyGen'\bin;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathDoxyGen'\bin\dot.exe', fQuiet),
        |   \CfgVerifyFile(sPathDoxyGen'\bin\doxygen.exe', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('doxygen', 1, fQuiet, 'Doxygen version 1.2.11.1');
return rc;


/*
 * EMX/GCC 3.x.x - this environment must be used 'on' the ordinary EMX.
 * Note! bin.new has been renamed to bin!
 * Note! make .lib of every .a! in 4OS2: for /R %i in (*.a) do if not exist %@NAME[%i].lib emxomf %i
 */
GCC3xx: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet,sPathId

    /*
     * EMX/GCC main directory.
     */
    sGCC = PathQuery(sPathId, sToolId, sOperation);
    if (sGCC = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    sGCCBack    = translate(sGCC, '\', '/');
    sGCCForw    = translate(sGCC, '/', '\');
    call EnvSet      fRM, 'PATH_EMXPGCC',   sGCCBack;
    call EnvSet      fRM, 'CCENV',          'EMX'
    call EnvSet      fRM, 'BUILD_ENV',      'EMX'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvAddFront fRM, 'BEGINLIBPATH',       sGCCBack'\dll;'
    call EnvAddFront fRM, 'PATH',               sGCCBack'\bin.new;'sGCCBack'\bin;'
    call EnvAddFront fRM, 'DPATH',              sGCCBack'\book;'
    call EnvAddFront fRM, 'BOOKSHELF',          sGCCBack'\book;'
    call EnvAddFront fRM, 'HELP',               sGCCBack'\help;'
    call EnvAddFront fRM, 'C_INCLUDE_PATH',     sGCCForw'/include'
    call EnvAddFront fRM, 'LIBRARY_PATH',       sGCCForw'/lib'
    call EnvAddFront fRM, 'CPLUS_INCLUDE_PATH', sGCCForw'/include/cpp;'sGCCForw'/include'
    call EnvSet      fRM, 'PROTODIR',           sGCCForw'/include/cpp/gen'
    call EnvSet      fRM, 'OBJC_INCLUDE_PATH',  sGCCForw'/include'
    call EnvAddFront fRM, 'INFOPATH',           sGCCForw'/info'
    call EnvSet      fRM, 'EMXBOOK',            'emxdev.inf+emxlib.inf+emxgnu.inf+emxbsd.inf'
    call EnvAddFront fRM, 'HELPNDX',            'emxbook.ndx', '+', 1

    /*
     * Verify.
     */
    chMajor = '3';
    chMinor = left(right(sToolId, 2), 1);
    chRel   = right(sToolId, 1);
    sVer = chMajor'.'chMinor'.'chRel
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sGCCBack'\bin.new\gcc.exe', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\bin.new\g++.exe', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\bin.new\as.exe', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\bin.new\readelf.exe', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\bin.new\emxomf.exe', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\dll\bfd211.dll', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\lib\iberty.a', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\lib\iberty.lib', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\lib\iberty_s.a', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\lib\iberty_s.lib', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\lib\opcodes.a', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\lib\opcodes.lib', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\lib\opcodes_s.a', fQuiet),
        |   \CfgVerifyFile(sGCCBack'\lib\opcodes_s.lib', fQuiet),
        ) then
        return 2;

    if (chMinor > 0) then
    do
        if (    \CfgVerifyFile(sGCCBack'\lib\gcc-lib\i386-pc-os2-emx\'sVer'\st\'sToolId'.lib', fQuiet),
            |   \CfgVerifyFile(sGCCBack'\lib\gcc-lib\i386-pc-os2-emx\'sVer'\st\stdcxx.lib', fQuiet),
            |   \CfgVerifyFile(sGCCBack'\lib\gcc-lib\i386-pc-os2-emx\'sVer'\st\stdcxx.a', fQuiet),
            ) then
            return 2;
    end
    else
    do
        if (    \CfgVerifyFile(sGCCBack'\lib\gcc-lib\i386-pc-os2_emx\'sVer'\st\gcc_dll.lib', fQuiet),
            |   \CfgVerifyFile(sGCCBack'\lib\gcc-lib\i386-pc-os2_emx\'sVer'\st\stdcxx.lib', fQuiet),
            |   \CfgVerifyFile(sGCCBack'\lib\gcc-lib\i386-pc-os2_emx\'sVer'\st\stdcxx.a', fQuiet),
            ) then
            return 2;
    end


    rc = CheckCmdOutput('gcc --version', 0, fQuiet, sVer);
    if (rc = 0) then
        rc = CheckCmdOutput('g++ --version', 0, fQuiet, sVer);
    if (rc = 0) then
    do
        sVerAS = '2.11.2';
        rc = CheckCmdOutput('as --version', 0, fQuiet, 'GNU assembler 'sVerAS);
    end
return rc;


/*
 * ICAT Debugger
 */
ICATGam: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    sPathICAT   = PathQuery('icatgam', sToolId, sOperation);
    if (sPathICAT = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRm, 'PATH_ICATGAM', sPathICAT;
    call EnvAddFront fRm, 'beginlibpath',sPathICAT'\dll;'
    call EnvAddFront fRm, 'path',        sPathICAT'\bin;'
    call EnvAddFront fRm, 'dpath',       sPathICAT'\help;'
    call EnvAddFront fRm, 'help',        sPathICAT'\help;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathICAT'\bin\icatgam.exe', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gamoou3.dll', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gam5lde.dll', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gam5cx.dll', fQuiet),
        ) then
        return 2;
return 0;


/*
 * ICAT Debugger
 */
ICATGam406RC1: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    sPathICAT   = PathQuery('icatgam406rc1', sToolId, sOperation);
    if (sPathICAT = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRm, 'PATH_ICATGAM', sPathICAT;
    call EnvAddFront fRm, 'beginlibpath',sPathICAT'\dll;'
    call EnvAddFront fRm, 'path',        sPathICAT'\bin;'
    call EnvAddFront fRm, 'dpath',       sPathICAT'\help;'
    call EnvAddFront fRm, 'help',        sPathICAT'\help;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathICAT'\bin\icatgam.exe', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gamoou3.dll', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gam5lde.dll', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gam5cx.dll', fQuiet),
        ) then
        return 2;
return 0;



/*
 * ICAT Debugger for PE images.
 */
ICATPe: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    sPathICAT   = PathQuery('icatgam', sToolId, sOperation);
    if (sPathICAT = '') then
        return 1;
    sPathICATPe = PathQuery('icatpe', sToolId, sOperation);
    if (sPathICATPe = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRm, 'PATH_ICATGAM',sPathICAT;
    call EnvSet      fRm, 'PATH_ICATPE', sPathICATPe;
    call EnvAddFront fRm, 'beginlibpath',sPathICATPe'\bin;'sPathICAT'\dll;'
    call EnvAddFront fRm, 'path',        sPathICATPe'\bin;'sPathICAT'\bin;'
    call EnvAddFront fRm, 'dpath',       sPathICATPe'\bin;'sPathICAT'\help;'
    call EnvAddFront fRm, 'help',        sPathICATPe'\bin;'sPathICAT'\help;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathICAT'\bin\icatgam.exe', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gamoou3.dll', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gam5lde.dll', fQuiet),
        |   \CfgVerifyFile(sPathICAT'\dll\gam5cx.dll', fQuiet),
        |   \CfgVerifyFile(sPathICATPe'\bin\icatgam.exe', fQuiet),
        |   \CfgVerifyFile(sPathICATPe'\bin\gamoou3.dll', fQuiet),
        |   \CfgVerifyFile(sPathICATPe'\bin\gam5lde.dll', fQuiet),
        |   \CfgVerifyFile(sPathICATPe'\bin\gam5cx.dll', fQuiet),
        ) then
        return 2;
return 0;



/*
 * Interactive Disassembler (IDA) v3.80a
 */
IDA38: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    /*
     * IDA main directory.
     */
    sPathIDA = PathQuery('ida38', sToolId, sOperation);
    if (sPathIDA = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_IDA',       sPathIDA
    call EnvAddFront fRM, 'path',           sPathIDA
    call EnvAddFront fRM, 'beginlibpath',   sPathIDA

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathIDA'\ida2.exe', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\idaw.exe', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\ida.dll', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\pc.dll', fQuiet),
        ) then
        return 2;
return 0;


/*
 * Interactive Disassembler (IDA) v4.01
 */
IDA40: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    /*
     * IDA main directory.
     */
    sPathIDA = PathQuery('ida40', sToolId, sOperation);
    if (sPathIDA = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_IDA',       sPathIDA
    call EnvAddFront fRM, 'path',           sPathIDA
    call EnvAddFront fRM, 'beginlibpath',   sPathIDA

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathIDA'\ida2.exe', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\idaw.exe', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\ida.dll', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\pc.dll', fQuiet),
        ) then
        return 2;
return 0;


/*
 * Interactive Disassembler (IDA) v4.14
 */
IDA414: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    /*
     * IDA main directory.
     */
    sPathIDA = PathQuery('ida414', sToolId, sOperation);
    if (sPathIDA = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_IDA',       sPathIDA
    call EnvAddFront fRM, 'path',           sPathIDA
    call EnvAddFront fRM, 'beginlibpath',   sPathIDA

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathIDA'\ida2.exe', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\idaw.exe', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\ida.dll', fQuiet),
        |   \CfgVerifyFile(sPathIDA'\pc.dll', fQuiet),
        ) then
        return 2;
return 0;


/*
 * Interactive Disassembler (IDA) Plugin SDK (v5.0?)
 */
IDASDK: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    /*
     * IDA main directory.
     */
    sPathIDASDK = PathQuery('idasdk', sToolId, sOperation);
    if (sPathIDASDK = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_IDASDK',    sPathIDASDK
    call EnvAddFront fRM, 'include',        sPathIDASDK'\include;'
    call EnvAddFront fRM, 'lib',            sPathIDASDK'\libwat.os2;'
    call EnvAddFront fRM, 'path',           sPathIDASDK'\bin\os2;'
    call EnvAddFront fRM, 'beginlibpath',   sPathIDASDK'\bin\os2;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathIDASDK'\os2wat.cfg', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\d32wat.cfg', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\include\exehdr.h', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\include\ida.hpp', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\include\vm.hpp', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\libwat.os2\ida.lib', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\libwat.d32\ida.lib', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\libwat.d32\INIRT386.OBJ', fQuiet),
      /*  |   \CfgVerifyFile(sPathIDASDK'\libbor.d32\ida.lib', fQuiet)*/,
        ) then
        return 2;
return 0;


/*
 * Interactive Disassembler (IDA) Plugin SDK (v5.0?)
 */
IDASDK: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    /*
     * IDA main directory.
     */
    sPathIDASDK = PathQuery('idasdk', sToolId, sOperation);
    if (sPathIDASDK = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_IDASDK',    sPathIDASDK
    call EnvAddFront fRM, 'include',        sPathIDASDK'\include;'
    call EnvAddFront fRM, 'lib',            sPathIDASDK'\libwat.os2;'
    call EnvAddFront fRM, 'path',           sPathIDASDK'\bin\os2;'
    call EnvAddFront fRM, 'beginlibpath',   sPathIDASDK'\bin\os2;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathIDASDK'\os2wat.cfg', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\d32wat.cfg', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\include\exehdr.h', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\include\ida.hpp', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\include\vm.hpp', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\libwat.os2\ida.lib', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\libwat.d32\ida.lib', fQuiet),
        |   \CfgVerifyFile(sPathIDASDK'\libwat.d32\INIRT386.OBJ', fQuiet),
      /*  |   \CfgVerifyFile(sPathIDASDK'\libbor.d32\ida.lib', fQuiet)*/,
        ) then
        return 2;
return 0;


/*
 * JAVA v1.3.1 (latest)
 */
Java131: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    /*
     * JAVA main directory.
     */
    sPathJava = PathQuery('java131', sToolId, sOperation);
    if (sPathJava = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_JAVA',      sPathJava
    call EnvSet      fRM, 'PATH_JAVA131',   sPathJava
    call EnvAddFront fRM, 'path',           sPathJava'\bin;'sPathJava'\jre\bin;'
    call EnvAddFront fRM, 'beginlibpath',   sPathJava'\jre\dll;'sPathJava'\jre\bin;'sPathJava'\icatjava\dll;'
/*    call EnvAddFront fRM, 'classpath',      sPathJava'\jre\dll;'sPathJava'\jre\bin;'sPathJava'\icatjava\dll;'
*/
    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathJava'\bin\javac.exe', fQuiet),
        |   \CfgVerifyFile(sPathJava'\bin\jar.exe', fQuiet),
        |   \CfgVerifyFile(sPathJava'\lib\tools.jar', fQuiet),
        |   \CfgVerifyFile(sPathJava'\lib\javai.lib', fQuiet),
        |   \CfgVerifyFile(sPathJava'\jre\dll\jv12mi36.dll', fQuiet),
        |   \CfgVerifyFile(sPathJava'\jre\bin\java.exe', fQuiet),
        |   \CfgVerifyFile(sPathJava'\jre\bin\jitc.dll', fQuiet),
        |   \CfgVerifyFile(sPathJava'\jre\bin\javaw.exe', fQuiet),
        |   \CfgVerifyFile(sPathJava'\jre\bin\rmid.exe', fQuiet),
        |   \CfgVerifyFile(sPathJava'\jre\bin\classic\jvm.dll', fQuiet),
        |   \CfgVerifyFile(sPathJava'\include\int64_md.h', fQuiet),
        |   \CfgVerifyFile(sPathJava'\include\jawt.h', fQuiet),
        |   \CfgVerifyFile(sPathJava'\include\jawt_md.h', fQuiet),
        |   \CfgVerifyFile(sPathJava'\include\jni.h', fQuiet),
        |   \CfgVerifyFile(sPathJava'\include\jniproto_md.h', fQuiet),
        |   \CfgVerifyFile(sPathJava'\include\jni_md.h', fQuiet),
        |   \CfgVerifyFile(sPathJava'\include\jvmdi.h', fQuiet),
        |   \CfgVerifyFile(sPathJava'\include\jvmpi.h', fQuiet),
        |   \CfgVerifyFile(sPathJava'\jre\bin\jitc_g.dll', fQuiet, 1),
        |   \CfgVerifyFile(sPathJava'\jre\bin\classic\jvm_g.dll', fQuiet, 1),
        ) then
        return 2;

return 0;


/*
 * (lib) JPEG v6b port.
 */
JPEG: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    sPathJPEG = PathQuery('jpeg', sToolId, sOperation);
    if (sPathJPEG = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;
    call EnvSet      fRm, 'PATH_JPEG',   sPathJPEG;
    call EnvAddFront fRm, 'beginlibpath',sPathJPEG'\dll;'
    call EnvAddFront fRm, 'path',        sPathJPEG'\bin;'
    call EnvAddFront fRM, 'include',     sPathJPEG'\include;'
    call EnvAddFront fRM, 'C_INCLUDE_PATH', sPathJPEG'\include;'
    call EnvAddFront fRM, 'lib',         sPathJPEG'\lib;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;

    if (    \CfgVerifyFile(sPathJPEG'\bin\cjpeg.exe', fQuiet),
        |   \CfgVerifyFile(sPathJPEG'\dll\jpeg.dll', fQuiet),
        |   \CfgVerifyFile(sPathJPEG'\include\jpeglib.h', fQuiet),
        |   \CfgVerifyFile(sPathJPEG'\lib\jpeg.a', fQuiet),
        |   \CfgVerifyFile(sPathJPEG'\lib\jpeg.lib', fQuiet),
        ) then
        return 2;
return 0;




/*
 * Mode commandline.
 */
Mode: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet,cols,rows

    if (pos('install', sOperation) > 0 & pos('uninstall', sOperation) <= 0) then
    do
        say "ok!"
        Address CMD 'mode' cols','rows
    end
    /* TODO
    else if ((pos('uninstall', sOperation) > 0) | \fRM) then
    do
        say 'Huh?'
        cols = 80;
        rows = 25;
    end
    */
return 0;


/*
 * Microsoft C v6.0a 16-bit
 */
MSCV6_16: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Microsoft C v6.0a main directory.
     */
    sPathMSC    = PathQuery('mscv6-16', sToolId, sOperation);
    if (sPathMSC = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'BUILD_ENV',  'MSCV6-16'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'
    call EnvSet      fRM, 'PATH_MSC',   sPathMSC;
    call EnvAddFront fRM, 'path',       sPathMSC'\binp;'
    call EnvAddFront fRM, 'endlibpath', sPathMSC'\dll;'
    call EnvAddFront fRM, 'helpfiles',  sPathMSC'\help;'
    call EnvAddFront fRM, 'include',    sPathMSC'\include;'
    call EnvAddFront fRM, 'include16',  sPathMSC'\include;'
    call EnvAddFront fRM, 'lib',        sPathMSC'\lib;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathMSC'\binp\cl.exe', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\lib\clibcep.lib', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\lib\llibcep.lib', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\lib\mlibcep.lib', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\lib\slibcep.lib', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\sysbits.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\dos.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\bios.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\string.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\stdio.h', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('cl', 0, fQuiet, 'Microsoft (R) C Optimizing Compiler Version 6.00A.04');
return rc;


/*
 * Microsoft C v6.0a 32-bit
 */
MSCV6_32: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Microsoft C v6.0a 32-bit main directory.
     */
    sPathDDKBase = PathQuery('ddkbase', sToolId, sOperation);
    if (sPathDDKBase = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * This is where the compiler really is.
     */
    call DDKBase 'ddkbase',sOperation,fRM,fQuiet;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'BUILD_ENV',  'MSCV6'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'
    call EnvSet      fRM, 'PATH_MSC',   sPathDDKBase;
    call EnvAddFront fRM, 'include',    sPathDDKBase'\h386;'
    call EnvAddFront fRM, 'lib',        sPathDDKBase'\lib;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathDDKBase'\tools\cl386.exe', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\h386\limits.h', fQuiet),
        |   \CfgVerifyFile(sPathDDKBase'\h386\string.h', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('cl386', 0, fQuiet, 'Microsoft (R) Microsoft 386 C Compiler. Version 6.00.054');
return rc;


/*
 * Microsoft C v7.0 16-bit with OS/2 support.
 */
MSCV7_16: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Microsoft C v6.0a main directory.
     */
    sPathMSC    = PathQuery('mscv7-16', sToolId, sOperation);
    if (sPathMSC = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'BUILD_ENV',  'MSCV7-16'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'
    call EnvSet      fRM, 'PATH_MSC',   sPathMSC;
    call EnvAddFront fRM, 'path',       sPathMSC'\binp;'
    call EnvAddFront fRM, 'endlibpath', sPathMSC'\dll;'
    call EnvAddFront fRM, 'helpfiles',  sPathMSC'\help;'
    call EnvAddFront fRM, 'include',    sPathMSC'\include;'
    call EnvAddFront fRM, 'include16',  sPathMSC'\include;'
    call EnvAddFront fRM, 'lib',        sPathMSC'\lib;'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathMSC'\binp\cl.exe', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\binp\link.exe', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\binp\ilink.exe', fQuiet),
       /* |   \CfgVerifyFile(sPathMSC'\lib\clibcep.lib', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\lib\llibcep.lib', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\lib\mlibcep.lib', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\lib\slibcep.lib', fQuiet)*/,
        |   \CfgVerifyFile(sPathMSC'\include\dos.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\bios.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\locale.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\stdiostr.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\string.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\vmemory.h', fQuiet),
        |   \CfgVerifyFile(sPathMSC'\include\stdio.h', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('cl', 0, fQuiet, 'Microsoft (R) C/C++ Optimizing Compiler Version 7.00');
return rc;




/*
 * mySQL Database system
 */
mySQL: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * mySQL Database system main directory.
     */
    sPathMySQL    = PathQuery('mysql', sToolId, sOperation);
    if (sPathMySQL = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_MYSQL',     sPathMySQL;
    call EnvAddFront fRM, 'path',           sPathMySQL'\bin;'
    call EnvAddFront fRM, 'beginlibpath',   sPathMySQL'\dll;'
    call EnvAddFront fRM, 'include',        sPathMySQL'\include;'
    call EnvAddFront fRM, 'bookshelf',      sPathMySQL'\doc;'sPathMySQL'\book';
    /*call EnvAddFront fRM, 'lib',            sPathMySQL'\lib;'*/

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathMySQL'\bin\mysql.exe', fQuiet),
        |   \CfgVerifyFile(sPathMySQL'\bin\mysqld.exe', fQuiet),
        |   \CfgVerifyFile(sPathMySQL'\bin\mysqladmin.exe', fQuiet),
        |   \CfgVerifyFile(sPathMySQL'\dll\mysql.dll', fQuiet),
        |   \CfgVerifyFile(sPathMySQL'\include\mysql.h', fQuiet),
        |   \CfgVerifyFile(sPathMySQL'\include\mysql_com.h', fQuiet),
        |   \CfgVerifyFile(sPathMySQL'\include\mysql_version.h', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('mysql --version', 0, fQuiet, ', for ');
return rc;



/*
 * NASM - NetWide Assembler (all versions)
 */
NASM: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet,sPathId

    /*
     * Get NASM directory
     */
    sPathNASM = PathQuery(sPathId, sToolId, sOperation);
    if (sPathNASM = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_NASM',      sPathNASM;
    call EnvAddFront fRM, 'path',           sPathNASM

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathNASM'\nasm.exe', fQuiet),
        |   \CfgVerifyFile(sPathNASM'\ndisasm.exe', fQuiet),
        ) then
        return 2;
    select
        when (sPathId = 'nasm9833') then    sVer = '0.98.33 compiled';
        otherwise do; say 'internal error invalid pathid! sPathId='sPathId; exit(16); end
    end
    rc = CheckCmdOutput('nasm -version', 0, fQuiet, 'NASM version '||sVer);
    if (rc = 0) then
        rc = CheckCmdOutput('ndisasm -version', 0, fQuiet, 'NDISASM version '||sVer);
return rc;



/*
 * NetQOS2 - help subsystem++ for VAC 3.6.5 and VAC 4.0
 */
NetQOS2: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    sPathNetQOS2 = PathQuery('netqos2', sToolId, sOperation);
    if (sPathNetQOS2 = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_NETQOS2',   sPathNetQOS2
    call EnvAddFront fRM, 'path',           sPathNetQOS2';'
    call EnvAddFront fRM, 'dpath',          sPathNetQOS2';'
    call EnvAddFront fRM, 'beginlibpath',   sPathNetQOS2';'
    call EnvSet      fRM, 'imndatasrv',     sPathNetQOS2'\DATA'
    call EnvSet      fRM, 'imndatacl',      sPathNetQOS2'\DATA'
    call EnvSet      fRM, 'imnworksrv',     sPathNetQOS2'\WORK'
    call EnvSet      fRM, 'imnworkcl',      sPathNetQOS2'\WORK'
    call EnvSet      fRM, 'imnnlpssrv',     sPathNetQOS2
    call EnvSet      fRM, 'imnnlpscl',      sPathNetQOS2
    call EnvSet      fRM, 'imncccfgfile',   'NETQ.CFG'
    call EnvSet      fRM, 'imncscfgfile',   'NETQ.CFG'
    call EnvSet      fRM, 'imqconfigsrv',   sPathNetQOS2'\instance'
    call EnvSet      fRM, 'imqconfigcl',    sPathNetQOS2'\instance\dbcshelp'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathNetQOS2'\netq.exe', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('netq', 999, fQuiet, 'NETQ {START | STOP');
return rc;


/*
 * Odin32 testcase setup.
 */
Odin32Testcase: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet
    if (  PathQuery('testcase_drive_unused', sToolId, sOperation) = '',
        | PathQuery('testcase_drive_fixed', sToolId, sOperation) = '',
        | PathQuery('testcase_drive_floppy', sToolId, sOperation) = '',
        | PathQuery('testcase_drive_cdrom', sToolId, sOperation) = '',
        ) then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'odin32_testcase_drive_unused',       PathQuery('testcase_drive_unused', sToolId, sOperation);
    call EnvSet      fRM, 'odin32_testcase_drive_fixed',        PathQuery('testcase_drive_fixed', sToolId, sOperation);
    call EnvSet      fRM, 'odin32_testcase_drive_floppy',       PathQuery('testcase_drive_floppy', sToolId, sOperation);
    call EnvSet      fRM, 'odin32_testcase_drive_cdrom',        PathQuery('testcase_drive_cdrom', sToolId, sOperation);
    call EnvSet      fRM, 'odin32_testcase_drive_network',      PathQuery('testcase_drive_network', sToolId, sOperation, 1);
    call EnvSet      fRM, 'odin32_testcase_drive_ramdisk',      PathQuery('testcase_drive_ramdisk', sToolId, sOperation, 1);

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
return 0;


/*
 * PERL 5005_53 or 5.004_55
 */
Perl50xxx: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Perl main directory.
     */
    sPathPerl       = PathQuery('perl50xxx', sToolId, sOperation);
    if (sPathPerl = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    sPathPerlForw   = translate(sPathPerl, '/', '\');
    call EnvSet      fRM, 'PATH_PERL',      sPathPerl;
    call EnvAddFront fRM, 'path',           sPathPerl'\bin;'
    call EnvAddFront fRM, 'beginlibpath',   sPathPerl'\dll;'
    call EnvAddEnd   fRM, 'bookshelf',      sPathPerl'\book;'
    call EnvSet      fRM, 'perllib_prefix', sPathPerlForw'/lib;'sPathPerlForw'/lib'
    call EnvSet      fRM, 'perl_sh_dir',    sPathPerlForw'/bin_sh'
    call EnvSet      fRM, 'manpath',        sPathPerlForw'/man'
    call EnvSet      fRM, 'perl5lib',       sPathPerlForw'/lib'
    call EnvSet      fRM, 'perl_badlang',   '0'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;

    sPerlDLL = 'perl.dll';
    sVer     = '5.004_55';
    f5005_53 = FileExists(sPathPerl'\dll\perlE0AC.dll');
    if (f5005_53) then
    do
        sPerlDLL = 'perlE0AC.dll';
        sVer     = '5.005_53';
    end
    if (    \CfgVerifyFile(sPathPerl'\bin\perl.exe', fQuiet),
        |   \CfgVerifyFile(sPathPerl'\dll\'||sPerlDLL, fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('perl --version', 0, fQuiet, 'This is perl, version '||sVer||' built for os2');
return rc;


/*
 * PERL v5.8.0
 */
Perl580: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Perl main directory.
     */
    sPathPerl       = PathQuery('perl580', sToolId, sOperation);
    if (sPathPerl = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    sPathPerlForw   = translate(sPathPerl, '/', '\');
    call EnvSet      fRM, 'PATH_PERL',      sPathPerl;
    call EnvAddFront fRM, 'path',           sPathPerl'\bin\5.8.0;'
    call EnvAddFront fRM, 'beginlibpath',   sPathPerl'\lib;'
    call EnvAddEnd   fRM, 'bookshelf',      sPathPerl'\doc;'
    call EnvSet      fRM, 'perllib_prefix', 'L:/Perl/lib;'sPathPerlForw'/lib'
    call EnvSet      fRM, 'perl_sh_dir',    sPathPerlForw'/bin/5.8.0'
    call EnvSet      fRM, 'manpath',        sPathPerlForw'/man'
    call EnvSet      fRM, 'perl_badlang',   '0'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;

    if (    \CfgVerifyFile(sPathPerl'\bin\5.8.0\perl.exe', fQuiet),
        |   \CfgVerifyFile(sPathPerl'\lib\perlB12E.dll', fQuiet),
        |   \CfgVerifyFile(sPathPerl'\bin\5.8.0\sh.exe', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('perl --version', 0, fQuiet, 'This is perl, v5.8.0 built for os2_emx');

return rc;


/*
 * Python/2 v1.5.2
 */
Python: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * The Python Home directory
     */
    sPythonHome = PathQuery('python', sToolId, sOperation);
    if (sPythonHome = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_PYTHON',    sPythonHome
    call EnvSet      fRM, 'pythonhome',     sPythonHome
    call EnvSet      fRM, 'pythonpath',     '.;'sPythonHome'\Lib;'sPythonHome'\Lib\plat-win;'sPythonHome'\Lib\lib-tk;'sPythonHome'\Lib\lib-dynload;'sPythonHome'\Lib\site-packages;'sPythonHome'\Lib\dos-8x3'
    call EnvAddFront fRM, 'beginlibpath',   sPythonHome
    call EnvAddFront fRM, 'path',           sPythonHome

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPythonHome'\Python.exe', fQuiet),
        |   \CfgVerifyFile(sPythonHome'\Python15.dll', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('echo print "hello world" | python', 0, fQuiet, 'hello world');
return rc;



/*
 * OS/2 Programmers Toolkit v4.0 (CSD1/4)
 */
Toolkit40: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Toolkit (4.0) main directory.
     */
    sPathTK    = PathQuery('toolkit40', sToolId, sOperation);
    if (sPathTK = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_TOOLKIT',  sPathTK;
    call EnvAddFront fRM, 'beginlibpath',   sPathTK'\archived;'sPathTK'\SAMPLES\MM\DLL;'sPathTK'\SAMPLES\OPENDOC\PARTS\DLL;'sPathTK'\SOM\COMMON\DLL;'sPathTK'\SOM\LIB;'sPathTK'\OPENDOC\BASE\DLL;'sPathTK'\OPENDOC\BASE\LOCALE\EN_US;'sPathTK'\DLL;'
    call EnvAddFront fRM, 'path',           sPathTK'\archived;'sPathTK'\SOM\COMMON;'sPathTK'\SOM\BIN;.;'sPathTK'\OPENDOC\BASE\BIN;'sPathTK'\BIN;'
    call EnvAddFront fRM, 'dpath',          sPathTK'\SOM\COMMON\SYSTEM;'sPathTK'\SOM\MSG;'sPathTK'\OPENDOC\BASE\MSG;'sPathTK'\BOOK;'sPathTK'\MSG;'
    call EnvAddFront fRM, 'help',           sPathTK'\archived;'sPathTK'\OPENDOC\BASE\LOCALE\EN_US;'sPathTK'\HELP;'
    call EnvAddFront fRM, 'bookshelf',      sPathTK'\archived;'sPathTK'\BOOK;'sPathTK'\ARCHIVED;'
    call EnvAddFront fRM, 'somir',          sPathTK'\SOM\COMMON\ETC\214\SOM.IR;'sPathTK'\OPENDOC\BASE\AVLSHELL.IR;'
    call EnvAddEnd   fRM, 'somir',          sPathTK'\OPENDOC\CUSTOM\OD.IR;'sPathTK'\SAMPLES\REXX\SOM\ANIMAL\ORXSMP.IR;'
/*    call EnvAddFront fRM, 'include',        sPathTK'\SPEECH\H;''sPathTK'\SAMPLES\OPENDOC\PARTS\INCLUDE;'sPathTK'\SOM\INCLUDE;'sPathTK'\INC;'sPathTK'\H\GL;'sPathTK'\H;' */
    call EnvAddFront fRM, 'include',        /*sPathTK'\SPEECH\H;'*/sPathTK'\SAMPLES\OPENDOC\PARTS\INCLUDE;'sPathTK'\SOM\INCLUDE;'sPathTK'\INC;'sPathTK'\H\GL;'sPathTK'\H;'
    call EnvAddEnd   fRM, 'include',        sPathTK'\H\LIBC;.;'
    call EnvAddFront fRM, 'lib',            sPathTK'\SPEECH\LIB;'sPathTK'\SAMPLES\MM\LIB;'sPathTK'\LIB;'sPathTK'\SOM\LIB;'sPathTK'\OPENDOC\BASE\LIB;'
    call EnvAddFront fRM, 'nlspath',        sPathTK'\OPENDOC\BASE\LOCALE\EN_US\%N;'sPathTK'\MSG\%N;C:\MPTN\MSG\NLS\%N;C:\TCPIP\msg\ENUS850\%N;'
    call EnvAddFront fRM, 'locpath',        sPathTK'\OPENDOC\BASE\LOCALE;'
    call EnvAddFront fRM, 'ipfc',           sPathTK'\IPFC;'
    call EnvSet      fRM, 'odbase',         sPathTK'\OPENDOC\BASE'
    call EnvSet      fRM, 'odlang',         'en_US'
    call EnvAddFront fRM, 'odbasepaths',    sPathTK'\OPENDOC\BASE;'
    call EnvSet      fRM, 'odcfg',          sPathTK'\OPENDOC\CUSTOM'
    call EnvSet      fRM, 'odtmp',          EnvGet('tmp');
    call EnvSet      fRM, 'sombase',        sPathTK'\SOM'
    call EnvSet      fRM, 'somruntime',     sPathTK'\SOM\COMMON'

    call EnvSet      fRM, 'cpref',          'CP1.INF+CP2.INF+CP3.INF'
    call EnvSet      fRM, 'gpiref',         'GPI1.INF+GPI2.INF+GPI3.INF+GPI4.INF'
    call EnvSet      fRM, 'mmref',          'MMREF1.INF+MMREF2.INF+MMREF3.INF'
    call EnvSet      fRM, 'pmref',          'PM1.INF+PM2.INF+PM3.INF+PM4.INF+PM5.INF'
    call EnvSet      fRM, 'wpsref',         'WPS1.INF+WPS2.INF+WPS3.INF'
    call EnvAddFront fRM, 'sminclude',      sPathTK'\H;'sPathTK'\IDL;'sPathTK'\SOM\INCLUDE;.;'sPathTK'\OPENDOC\BASE\INCLUDE;'sPathTK'\SAMPLES\OPENDOC\PARTS\INCLUDE;'
    call EnvSet      fRM, 'smaddstar',      '1'
    call EnvSet      fRM, 'smemit',         'h;ih;c'
    call EnvSet      fRM, 'smtmp',          EnvGet('tmp');
    call EnvSet      fRM, 'smclasses',      'WPTYPES.IDL'
    call EnvSet      fRM, 'odparts',        sPathTK'\SAMPLES\OPENDOC\PARTS'
    call EnvSet      fRM, 'odsrc',          sPathTK'\SAMPLES\OPENDOC\PARTS'
    call EnvAddFront fRM, 'odpartspaths',   sPathTK'\SAMPLES\OPENDOC\PARTS;'
    call EnvAddFront fRM, 'odsrcpaths',     sPathTK'\SAMPLES\OPENDOC\PARTS;'
    /*
    call EnvSet      fRM, 'CAT_MACHINE=COM1:57600'
    call EnvSet      fRM, 'CAT_HOST_BIN_PATH='sPathTK'\BIN'
    call EnvSet      fRM, 'CAT_COMMUNICATION_TYPE=ASYNC_SIGBRK'
    call EnvAddFront fRM, 'CAT_HOST_SOURCE_PATH='sPathTK'\BIN;'
    */

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathTK'\bin\alp.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\rc.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\ipfc.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\implib.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\mkmsgf.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\mapsym.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\os2386.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\pmbidi.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\tcpip32.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\os2.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\os2win.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\stack16\pmwsock.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\som\bin\sc.exe', fQuiet),
        ) then
        return 2;

    rc = CheckSyslevel(sPathTK||'\bin\syslevel.tlk', fQuiet,,,,,
                       'IBM Developer''s Toolkit for OS/2 Warp Version 4',,
                       15, '0');
    if (rc = 0) then
        rc = CheckCmdOutput('sc -V', -1, fQuiet, '", Version: 2.54.');
    if (rc = 0) then
        rc = CheckCmdOutput('rc', 0, fQuiet, 'IBM RC (Resource Compiler) Version 5.00.00');
    if (rc = 0) then
        rc = CheckCmdOutput('ipfc', 0, fQuiet, 'Version 4.00.00');

return rc;



/*
 * OS/2 Programmers Toolkit v4.5
 */
Toolkit45: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Toolkit (4.5) main directory.
     */
     sPathTK    = PathQuery('toolkit45', sToolId, sOperation);
    if (sPathTK = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_TOOLKIT',  sPathTK;
    call EnvAddFront fRM, 'path',        sPathTK'\bin;'
    call EnvAddFront fRM, 'dpath',       sPathTK'\book;'
    call EnvAddFront fRM, 'dpath',       sPathTK'\msg;'
    call EnvAddFront fRM, 'beginlibpath', sPathTK'\dll;'
    call EnvAddFront fRM, 'help',        sPathTK'\help;'
    call EnvAddFront fRM, 'bookshelf',   sPathTK'\archived;'
    call EnvAddFront fRM, 'bookshelf',   sPathTK'\book;'
    call EnvAddFront fRM, 'nlspath',     sPathTK'\msg\%N;'
    call EnvAddEnd   fRM, 'ulspath',     sPathTK'\language;'
    call EnvAddFront fRM, 'include',     sPathTK'\H;'
/*    call EnvAddFront fRM, 'include',     sPathTK'\H\GL;' */
/*    call EnvAddFront fRM, 'include',     sPathTK'\SPEECH\H;' includes tend to get too long :-( */
    call EnvAddFront fRM, 'include',     sPathTK'\H\RPC;'
    call EnvAddFront fRM, 'include',     sPathTK'\H\NETNB;'
    call EnvAddFront fRM, 'include',     sPathTK'\H\NETINET;'
    call EnvAddFront fRM, 'include',     sPathTK'\H\NET;'
    call EnvAddFront fRM, 'include',     sPathTK'\H\ARPA;'
    call EnvAddFront fRM, 'include',     sPathTK'\INC;'
    call EnvAddEnd   fRM, 'lib',         sPathTK'\SAMPLES\MM\LIB;'
    call EnvAddEnd   fRM, 'lib',         sPathTK'\SPEECH\LIB;'
    call EnvAddFront fRM, 'lib',         sPathTK'\lib;'
    call EnvAddFront fRM, 'helpndx',     'EPMKWHLP.NDX+', '+'
    call EnvAddFront fRM, 'ipfc',        sPathTK'\ipfc;'
    call EnvSet      fRM, 'LANG',        'en_us'
    call EnvSet      fRM, 'CPREF',       'CP1.INF+CP2.INF+CP3.INF'
    call EnvSet      fRM, 'GPIREF',      'GPI1.INF+GPI2.INF+GPI3.INF+GPI4.INF'
    call EnvSet      fRM, 'MMREF',       'MMREF1.INF+MMREF2.INF+MMREF3.INF'
    call EnvSet      fRM, 'PMREF',       'PM1.INF+PM2.INF+PM3.INF+PM4.INF+PM5.INF'
    call EnvSet      fRM, 'WPSREF',      'WPS1.INF+WPS2.INF+WPS3.INF'
    /*
    call EnvSet      fRM, 'CAT_MACHINE', 'COM1:57600'
    call EnvSet      fRM, 'CAT_HOST_BIN_PATH', TKMAIN'\BIN'
    call EnvSet      fRM, 'CAT_COMMUNICATION_TYPE', 'ASYNC_SIGBRK'
    call EnvSet      fRM, 'CAT_HOST_SOURCE_PATH',TKMAIN'\BIN;'
    */

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathTK'\bin\alp.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\rc.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\ipfc.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\implib.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\mkmsgf.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\mapsym.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\os2386.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\pmbidi.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\tcpip32.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\os2.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\os2win.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\stack16\pmwsock.h', fQuiet),
        |    FileExists(sPathTK'\som\bin\sc.exe'),
        ) then
        return 2;

    rc = CheckSyslevel(sPathTK||'\bin\syslevel.tlk', fQuiet,,
                       '5639F9300', '4.50.0', 'XR04500',,
                       'IBM OS/2 Developer''s Toolkit Version 4.5',,
                       15, '0');
    if (rc = 0) then
        rc = CheckCmdOutput('rc', 0, fQuiet, 'IBM RC (Resource Compiler) Version 5.00.004');
    if (rc = 0) then
        rc = CheckCmdOutput('ipfc', 0, fQuiet, 'Version 4.00.006 July 21 1998');
return rc;


/*
 * OS/2 Programmers Toolkit v4.5.1
 */
Toolkit451: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Toolkit (4.5.1) main directory.
     */
     sPathTK    = PathQuery('toolkit451', sToolId, sOperation);
    if (sPathTK = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_TOOLKIT',  sPathTK;
    call EnvAddFront fRM, 'path',        sPathTK'\bin;'sPathTK'\som\common;'sPathTK'\som\bin'
    call EnvAddFront fRM, 'dpath',       sPathTK'\msg;'sPathTK'\book;'sPathTK'\SOM\COMMON\SYSTEM;'sPathTK'\SOM\MSG;'
    call EnvAddFront fRM, 'beginlibpath', sPathTK'\dll;'sPathTK'\som\common\dll;'sPathTK'\som\lib;'
    call EnvAddFront fRM, 'help',        sPathTK'\help;'
    call EnvAddFront fRM, 'bookshelf',   sPathTK'\book;'sPathTK'\archived;'
    call EnvAddFront fRM, 'somir',       sPathTK'\SOM\COMMON\ETC\214\SOM.IR;'
    call EnvAddEnd   fRM, 'somir',       sPathTK'\SAMPLES\REXX\SOM\ANIMAL\ORXSMP.IR;'
    call EnvAddFront fRM, 'nlspath',     sPathTK'\msg\%N;'
    call EnvAddEnd   fRM, 'ulspath',     sPathTK'\language;'
    /*call EnvAddFront fRM, 'include',     sPathTK'\H\ARPA;'sPathTK'\H\NET;'sPathTK'\H\NETINET;'sPathTK'\H\NETNB;'sPathTK'\H\RPC;'sPathTK'\SPEECH\H;'sPathTK'\H\GL;'sPathTK'\H;'sPathTK'\SOM\INCLUDE;'sPathTK'\INC;'*/
    call EnvAddFront fRM, 'include',     sPathTK'\H\ARPA;'sPathTK'\H\NET;'sPathTK'\H\NETINET;'sPathTK'\H\NETNB;'sPathTK'\H\RPC;'/*sPathTK'\SPEECH\H;'sPathTK'\H\GL;'*/sPathTK'\H;'sPathTK'\SOM\INCLUDE;'sPathTK'\INC;'
    call EnvAddFront fRM, 'lib',         sPathTK'\lib;'sPathTK'\som\lib;'
    call EnvAddEnd   fRM, 'lib',         sPathTK'\SAMPLES\MM\LIB;'sPathTK'\SPEECH\LIB;'
    call EnvAddFront fRM, 'helpndx',     'EPMKWHLP.NDX+', '+'
    call EnvAddFront fRM, 'ipfc',        sPathTK'\ipfc;'
    call EnvSet      fRM, 'sombase',     sPathTK'\SOM'
    call EnvSet      fRM, 'somruntime',  sPathTK'\SOM\COMMON'
    call EnvSet      fRM, 'LANG',        'en_us'

    call EnvSet      fRM, 'CPREF',       'CP1.INF+CP2.INF+CP3.INF'
    call EnvSet      fRM, 'GPIREF',      'GPI1.INF+GPI2.INF+GPI3.INF+GPI4.INF'
    call EnvSet      fRM, 'MMREF',       'MMREF1.INF+MMREF2.INF+MMREF3.INF'
    call EnvSet      fRM, 'PMREF',       'PM1.INF+PM2.INF+PM3.INF+PM4.INF+PM5.INF'
    call EnvSet      fRM, 'WPSREF',      'WPS1.INF+WPS2.INF+WPS3.INF'
    call EnvAddFront fRM, 'sminclude',   sPathTK'\H;'sPathTK'\IDL;'sPathTK'\SOM\INCLUDE;.;'
    call EnvSet      fRM, 'smaddstar',   '1'
    call EnvSet      fRM, 'smemit',      'h;ih;c'
    call EnvSet      fRM, 'smtmp',       EnvGet('tmp');
    call EnvSet      fRM, 'smclasses',   'WPTYPES.IDL'
    /*
    call EnvSet      fRM, 'CAT_MACHINE', 'COM1:57600'
    call EnvSet      fRM, 'CAT_HOST_BIN_PATH', TKMAIN'\BIN'
    call EnvSet      fRM, 'CAT_COMMUNICATION_TYPE', 'ASYNC_SIGBRK'
    call EnvSet      fRM, 'CAT_HOST_SOURCE_PATH',TKMAIN'\BIN;'
    */

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathTK'\bin\alp.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\rc.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\ipfc.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\implib.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\mkmsgf.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\mapsym.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\nmake.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\nmake32.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\os2386.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\pmbidi.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\tcpip32.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\os2.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\os2win.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\stack16\pmwsock.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\som\bin\sc.exe', fQuiet),
        ) then
        return 2;

    rc = CheckSyslevel(sPathTK||'\bin\syslevel.tlk', fQuiet,,
                       '5639F9300', '4.50.1', 'XR04510',,
                       'IBM OS/2 Developer''s Toolkit Version 4.5',,
                       15, '0');
    if (rc = 0) then
        rc = CheckCmdOutput('sc -V', -1, fQuiet, '", Version: 2.54.');
    if (rc = 0) then
        rc = CheckCmdOutput('rc', 0, fQuiet, 'IBM RC (Resource Compiler) Version 5.00.006 Oct 20 2000');
    if (rc = 0) then
        rc = CheckCmdOutput('ipfc', 0, fQuiet, 'Version 4.00.007 Oct 02 2000');
    if (rc = 0) then
        rc = CheckCmdOutput('nmake -?', 0, fQuiet, 'Version 4.00.000 Oct 20 2000');
    if (rc = 0) then
        rc = CheckCmdOutput('nmake32 -?', 0, fQuiet, 'Version 5.00.003 Oct 20 2000');
return rc;



/*
 * OS/2 Programmers Toolkit v4.5.2
 */
Toolkit452: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Toolkit (4.5.1) main directory.
     */
     sPathTK    = PathQuery('toolkit452', sToolId, sOperation);
    if (sPathTK = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_TOOLKIT',  sPathTK;
    call EnvAddFront fRM, 'path',        sPathTK'\bin;'sPathTK'\som\common;'sPathTK'\som\bin'
    call EnvAddFront fRM, 'dpath',       sPathTK'\msg;'sPathTK'\book;'sPathTK'\SOM\COMMON\SYSTEM;'sPathTK'\SOM\MSG;'
    call EnvAddFront fRM, 'beginlibpath', sPathTK'\dll;'sPathTK'\som\common\dll;'sPathTK'\som\lib;'
    call EnvAddFront fRM, 'help',        sPathTK'\help;'
    call EnvAddFront fRM, 'bookshelf',   sPathTK'\book;'sPathTK'\archived;'
    call EnvAddFront fRM, 'somir',       sPathTK'\SOM\COMMON\ETC\214\SOM.IR;'
    call EnvAddEnd   fRM, 'somir',       sPathTK'\SAMPLES\REXX\SOM\ANIMAL\ORXSMP.IR;'
    call EnvAddFront fRM, 'nlspath',     sPathTK'\msg\%N;'
    call EnvAddEnd   fRM, 'ulspath',     sPathTK'\language;'
    /*call EnvAddFront fRM, 'include',     sPathTK'\H\ARPA;'sPathTK'\H\NET;'sPathTK'\H\NETINET;'sPathTK'\H\NETNB;'sPathTK'\H\RPC;'sPathTK'\SPEECH\H;'sPathTK'\H\GL;'sPathTK'\H;'sPathTK'\SOM\INCLUDE;'sPathTK'\INC;'*/
    /* the include mustn't be too long :-/ */
    call EnvAddFront fRM, 'include',     sPathTK'\H\ARPA;'sPathTK'\H\NET;'sPathTK'\H\NETINET;'sPathTK'\H\NETNB;'sPathTK'\H\RPC;'/*sPathTK'\SPEECH\H;'sPathTK'\H\GL;'*/sPathTK'\H;'sPathTK'\SOM\INCLUDE;'sPathTK'\INC;'
    call EnvAddFront fRM, 'lib',         sPathTK'\lib;'sPathTK'\som\lib;'
    call EnvAddEnd   fRM, 'lib',         sPathTK'\SAMPLES\MM\LIB;'sPathTK'\SPEECH\LIB;'
    call EnvAddFront fRM, 'helpndx',     'EPMKWHLP.NDX+', '+'
    call EnvAddFront fRM, 'ipfc',        sPathTK'\ipfc;'
    call EnvSet      fRM, 'sombase',     sPathTK'\SOM'
    call EnvSet      fRM, 'somruntime',  sPathTK'\SOM\COMMON'
    call EnvSet      fRM, 'LANG',        'en_us'

    call EnvSet      fRM, 'CPREF',       'CP1.INF+CP2.INF+CP3.INF'
    call EnvSet      fRM, 'GPIREF',      'GPI1.INF+GPI2.INF+GPI3.INF+GPI4.INF'
    call EnvSet      fRM, 'MMREF',       'MMREF1.INF+MMREF2.INF+MMREF3.INF'
    call EnvSet      fRM, 'PMREF',       'PM1.INF+PM2.INF+PM3.INF+PM4.INF+PM5.INF'
    call EnvSet      fRM, 'WPSREF',      'WPS1.INF+WPS2.INF+WPS3.INF'
    call EnvAddFront fRM, 'sminclude',   sPathTK'\H;'sPathTK'\IDL;'sPathTK'\SOM\INCLUDE;.;'
    call EnvSet      fRM, 'smaddstar',   '1'
    call EnvSet      fRM, 'smemit',      'h;ih;c'
    call EnvSet      fRM, 'smtmp',       EnvGet('tmp');
    call EnvSet      fRM, 'smclasses',   'WPTYPES.IDL'
    /*
    call EnvSet      fRM, 'CAT_MACHINE', 'COM1:57600'
    call EnvSet      fRM, 'CAT_HOST_BIN_PATH', TKMAIN'\BIN'
    call EnvSet      fRM, 'CAT_COMMUNICATION_TYPE', 'ASYNC_SIGBRK'
    call EnvSet      fRM, 'CAT_HOST_SOURCE_PATH',TKMAIN'\BIN;'
    */

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathTK'\bin\alp.exe', fQuiet),
        /*|   \CfgVerifyFile(sPathTK'\bin\rc.exe', fQuiet)*/,
        |   \CfgVerifyFile(sPathTK'\bin\rc16.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\ipfc.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\implib.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\mkmsgf.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\mapsym.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\nmake.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\bin\nmake32.exe', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\os2386.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\pmbidi.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\lib\tcpip32.lib', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\os2.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\os2win.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\h\stack16\pmwsock.h', fQuiet),
        |   \CfgVerifyFile(sPathTK'\som\bin\sc.exe', fQuiet),
        ) then
        return 2;

    rc = CheckSyslevel(sPathTK||'\bin\syslevel.tlk', fQuiet,,
                       '5639F9300', '4.50.2', 'XR04520',,
                       'IBM OS/2 Developer''s Toolkit Version 4.5',,
                       15, '0');
    if (rc = 0) then
        rc = CheckCmdOutput('sc -V', -1, fQuiet, '", Version: 2.54.');
    /*if (rc = 0) then
        rc = CheckCmdOutput('rc', 1, fQuiet, 'Version 4.00.011 Oct 04 2001');*/
    if (rc = 0) then
        rc = CheckCmdOutput('rc16', 1, fQuiet, 'Version 4.00.011 Oct 04 2001');
    if (rc = 0) then
        rc = CheckCmdOutput('ipfc', 0, fQuiet, 'Version 4.00.007 Oct 02 2000');
    if (rc = 0) then
        rc = CheckCmdOutput('nmake -?', 0, fQuiet, 'Version 4.00.001 Oct  4 2001');
    if (rc = 0) then
        rc = CheckCmdOutput('nmake32 -?', 0, fQuiet, 'Version 5.00.003 Oct  4 2001');
return rc;



/**
 * This will envolve into an full UNIX like environment some day perhaps...
 */
Unix: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * Unix root directory and XFree86 main directory.
     */
    sUnixBack = PathQuery('unixroot', sToolId, sOperation);
    if (sUnixBack = '') then
        return 1;
    sXF86Back = PathQuery('xfree86', sToolId, sOperation);
    if (sXF86Back = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    sUnixForw = translate(sUnixBack, '/', '\');
    call EnvSet      fRM, 'PATH_UNIX',          sUnixBack
    call EnvSet      fRM, 'unixroot',           sUnixBack
    call EnvAddFront fRM, 'path',               sUnixBack'\bin;'sUnixBack'\usr\local\bin;'
    call EnvAddFront fRM, 'beginlibpath',       sUnixBack'\dll;'
    call EnvSet      fRM, 'groff_font_path',    sUnixForw'/lib/groff/font'
    call EnvSet      fRM, 'groff_tmac_path',    sUnixForw'/lib/groff/tmac'
    call EnvSet      fRM, 'refer',              sUnixForw'/lib/groff/dict/papers/ind'

/*    call EnvSet      fRM, 'editor',             'TEDIT' - don't change it */

    sXF86Forw = translate(sXF86Back, '/', '\');
    call EnvSet      fRM, 'PATH_XFREE86',       sXF86Back
    call EnvAddFront fRM, 'C_INCLUDE_PATH',     sXF86Forw'/include'
    call EnvAddFront fRM, 'CPLUS_INCLUDE_PATH', sXF86Forw'/include'
    call EnvSet      fRM, 'OBJC_INCLUDE_PATH',  sXF86Forw'/include'
    call EnvAddFront fRM, 'LIBRARY_PATH',       sXF86Forw'/lib'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sUnixBack'\bin\bash.exe', fQuiet, 1),
        |   \CfgVerifyFile(sUnixBack'\bin\sh.exe', fQuiet),
        |   \CfgVerifyFile(sUnixBack'\bin\yes.exe', fQuiet),
        |   \CfgVerifyFile(sUnixBack'\bin\rm.exe', fQuiet),
        |   \CfgVerifyFile(sUnixBack'\bin\cp.exe', fQuiet, 1),
        |   \CfgVerifyFile(sUnixBack'\bin\mv.exe', fQuiet. 1),
        |   \CfgVerifyFile(sXF86Back'\bin\xf86config.exe', fQuiet, 1),
        ) then
        return 2;
return 0;



/*
 * IBM Visual Age for C++ v3.08 for OS/2
 */
VAC308: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * IBM Visual Age for C++ Version 3.08 main directory.
     */
    sPathCPP = PathQuery('vac308', sToolId, sOperation);
    if (sPathCPP = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_VAC308',    sPathCPP
    call EnvSet      fRM, 'CCENV',      'VAC3'
    call EnvSet      fRM, 'BUILD_ENV',  'VAC308'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvAddFront fRM, 'beginlibpath',   sPathCPP'\DLL;'sPathCPP'\SAMPLES\TOOLKIT\DLL;'
    call EnvAddFront fRM, 'path',           sPathCPP'\BIN;'sPathCPP'\SMARTS\SCRIPTS;'sPathCPP'\HELP;'
    call EnvAddFront fRM, 'dpath',          sPathCPP'\HELP;'sPathCPP';'sPathCPP'\LOCALE;'sPathCPP'\MACROS;'sPathCPP'\BND;'
    call EnvAddFront fRM, 'help',           sPathCPP'\HELP;'sPathCPP'\SAMPLES\TOOLKIT\HELP;'
    call EnvAddFront fRM, 'bookshelf',      sPathCPP'\HELP;'
    call EnvAddFront fRM, 'somir',          sPathCPP'\ETC\SOM.IR;'
    call EnvAddFront fRM, 'cpphelp_ini',    'C:\OS2\SYSTEM'
    call EnvAddFront fRM, 'locpath',        sPathCPP'\LOCALE;%LOCPATH%;'
    call EnvAddFront fRM, 'include',        sPathCPP'\INCLUDE;'sPathCPP'\INCLUDE\OS2;'sPathCPP'\INC;'sPathCPP'\INCLUDE\SOM;'
    call EnvAddFront fRM, 'sminclude',      sPathCPP'\INCLUDE\OS2;'sPathCPP'\INCLUDE;'sPathCPP'\INCLUDE\SOM;'

    call EnvAddFront fRM, 'vbpath',         '.;'sPathCPP'\DDE4VB;'
    call EnvSet      fRM, 'tmpdir',         EnvGet('tmp')
    call EnvSet      fRM, 'lxevfref',       'EVFELREF.INF+LPXCREF.INF'
    call EnvSet      fRM, 'lxevfhdi',       'EVFELHDI.INF+LPEXHDI.INF'
    call EnvAddFront fRM, 'lpath',          sPathCPP'\MACROS;'
    call EnvAddFront fRM, 'codelpath',      sPathCPP'\CODE\MACROS;'sPathCPP'\MACROS;'
    call EnvSet      fRM, 'clref',          'CPPCLRF.INF+CPPDAT.INF+CPPAPP.INF+CPPWIN.INF+CPPCTL.INF+CPPADV.INF+CPP2DG.INF+CPPDDE.INF+CPPDM.INF+CPPMM.INF+CPPCLRB.INF'
    call EnvAddFront fRM, 'ipfc',           sPathCPP'\IPFC'
    call EnvAddFront fRM, 'lib',            sPathCPP'\LIB;'sPathCPP'\DLL;'
    call EnvSet      fRM, 'cpplocal',       sPathCPP
    call EnvSet      fRM, 'cppmain',        sPathCPP
    call EnvSet      fRM, 'cppwork',        sPathCPP
    call EnvSet      fRM, 'iwf.default_prj','CPPDFTPRJ'

    call EnvSet      fRM, 'iwf.solution_lang_support', 'CPPIBS30;ENG'
    call EnvSet      fRM, 'vacpp_shared'    'FALSE'
    call EnvSet      fRM, 'iwfhelp',        'IWFHDI.INF'
    call EnvSet      fRM, 'iwfopt',         sPathCPP

    call EnvSet      fRM, 'somruntime',     sPathCPP'\DLL'
    call EnvSet      fRM, 'smaddstar',      '1'
    call EnvSet      fRM, 'smemit',         'h;ih;c'
    call EnvSet      fRM, 'sombase',        sPathCPP
    call EnvSet      fRM, 'smtmp',          EnvGet('tmp')
    call EnvSet      fRM, 'smclasses',      'WPTYPES.IDL'

    call EnvAddFront fRM, 'helpndx',        'EPMKWHLP.NDX+CPP.NDX+CPPBRS.NDX', '+'
    call EnvAddFront fRM, 'ipf_keys',       'SHOWNAV'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathCPP'\bin\icc.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\ilib.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\ilink.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\icsperf.exe', fQuiet,1),
        |   \CfgVerifyFile(sPathCPP'\bin\icsdebug.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\cppfilt.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\dllrname.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\demangl.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cppom30.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cppom30i.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cppom30o.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cppon30i.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cppon30o.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\_doscall.lib', fQuiet, 1),
        |   \CfgVerifyFile(sPathCPP'\lib\_pmwin.lib', fQuiet, 1),
        |   \CfgVerifyFile(sPathCPP'\include\builtin.h', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\include\conio.h', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\include\ismkss.h', fQuiet),
        |    FileExists(sPathCPP'\include\os2.h'),
        |    FileExists(sPathCPP'\include\os2win.h'),
        |    FileExists(sPathCPP'\include\pm.h'),
        |   \CfgVerifyFile(sPathCPP'\include\sys\utime.h', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\help\cpplib.inf', fQuiet),
        ) then
        return 2;


    rc = CheckSyslevel(sPathCPP||'\syslevel\syslevel.ct3', fQuiet,'562201703',,'CTC308',);
    if (rc = 0) then
        rc = CheckSyslevel(sPathCPP||'\syslevel\syslevel.ct4', fQuiet,'562201704',,'CTU308',);
    /*if (rc = 0) then
        rc = CheckCmdOutput('syslevel '||sPathCPP||'\syslevel', 0, fQuiet, 'Version 3.00     Component ID 562201707'||'0d0a'x||'Current CSD level: CTV308');
    if (rc = 0) then
        rc = CheckSyslevel(sPathCPP||'\syslevel\syslevel.ct8', fQuiet,'562201708',,'CTD308',);
        */
    if (rc = 0) then
        rc = CheckSyslevel(sPathCPP||'\syslevel\syslevel.wf5', fQuiet,'562201605',,'CTC308',);
    /*if (rc = 0) then
        rc = CheckSyslevel(sPathCPP||'\syslevel\syslevel.wf2', fQuiet,'562201602',,'CTO308',);
        */
    if (rc = 0) then
        rc = CheckCmdOutput('icc', 0, fQuiet, 'IBM VisualAge C++ for OS/2, Version 3');
    if (rc = 0) then
        rc = CheckCmdOutput('ilink', 16, fQuiet, 'IBM(R) Linker for OS/2(R), Version 01.08.r1a_CTC308c');
    if (rc = 0) then
        rc = CheckCmdOutput('ilib /?', 8, fQuiet, 'IBM(R) Library Manager for OS/2(R), Version 01.00.03 cc_CTC308c');
return rc;



/*
 * Visual Age / C and C++ tools v3.6.5 for OS/2
 */
VAC365: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * IBM C/C++ Compiler and Tools Version 3.6.5 main directory.
     */
    sPathCxx    = PathQuery('vac365', sToolId, sOperation);
    if (sPathCxx = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_VAC365', sPathCxx;
    call EnvSet      fRM, 'CCENV',      'VAC36'
    call EnvSet      fRM, 'BUILD_ENV',  'VAC365'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvSet      fRM, 'cxxmain',     sPathCxx;
    call EnvAddFront fRM, 'path',        sPathCxx'\bin;'
    call EnvAddFront fRM, 'dpath',       sPathCxx'\local;'sPathCxx'\help;'
    call EnvAddFront fRM, 'beginlibpath',sPathCxx'\dll;'sPathCxx'\runtime;'
    call EnvAddFront fRM, 'nlspath',     sPathCxx'\msg\%N;'
    call EnvAddFront fRM, 'include',     sPathCxx'\include;'
    call EnvAddFront fRM, 'lib',         sPathCxx'\lib;'
    call EnvAddFront fRM, 'ipfc',        sPathCxx'\ipfc;'
    call EnvSet      fRM, 'LANG',        'en_us'
    call EnvSet      fRM, 'CPP_DBG_LANG', 'CPP'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathCxx'\bin\icc.exe', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\bin\ilib.exe', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\bin\ilink.exe', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\bin\idebug.exe', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\bin\cppfilt.exe', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\bin\dllrname.exe', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\bin\cppcbe36.exe', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\lib\cpprms36.lib', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\lib\cpprmi36.lib', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\lib\cpprmo36.lib', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\lib\cpprni36.lib', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\lib\cpprds36.lib', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\include\builtin.h', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\include\conio.h', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\include\ismavl.h', fQuiet),
        |    FileExists(sPathCxx'\include\os2.h'),
        |    FileExists(sPathCxx'\include\os2win.h'),
        |    FileExists(sPathCxx'\include\pm.h'),
        |   \CfgVerifyFile(sPathCxx'\include\sys\utime.h', fQuiet),
        |   \CfgVerifyFile(sPathCxx'\help\cpplbm36.msg', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('icc', 0, fQuiet, 'IBM* C and C++ Compilers for OS/2*, AIX* and for Windows NT**, Version 3.6');
    if (rc = 0) then
        rc = CheckCmdOutput('ilink', 16, fQuiet, 'IBM(R) Linker for OS/2(R), Version 03.06.PPK1010917.011116ilink');
    if (rc = 0) then
        rc = CheckCmdOutput('ilib /?', 0, fQuiet, 'IBM Librarian for OS/2(R) Version 03.99.PPK1001123');
    if (stream(sPathCxx'\bin\cppcbe36.exe', 'c', 'query size') <> 603122) then
    do
        say 'Error!!! Get latest vac365 optimizer fixes from the OS2 Mozilla project.';
        say '         http://www.mozilla.org/ports/os2/setup.html';
        rc = 99;
    end
return rc;


/*
 * Visual Age for C++ v4.0 for OS/2
 */
VAC40: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * IBM VisualAge for C++ v4.0 main directory.
     */
    sPathCPP    = PathQuery('vac40', sToolId, sOperation);
    if (sPathCPP = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_VAC40',  sPathCPP;
    call EnvSet      fRM, 'CCENV',      'VAC40'
    call EnvSet      fRM, 'BUILD_ENV',  'VAC40'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvAddFront fRM, 'path',        sPathCPP'\bin;'
    call EnvAddFront fRM, 'dpath',       sPathCPP'\etc;'sPathCPP'\help;'
    call EnvAddFront fRM, 'beginlibpath',sPathCPP'\dll;'sPathCPP'\runtime;'
    call EnvAddFront fRM, 'help',        sPathCPP'\help;'
    call EnvAddFront fRM, 'nlspath',     sPathCPP'\msg\%N;'
    call EnvAddFront fRM, 'locpath',     sPathCPP'\locale;'sPathCPP'\runtime\locale;'
    call EnvAddFront fRM, 'include',     sPathCPP'\ivb;'sPathCPP'\include;'
    call EnvAddFront fRM, 'lib',         sPathCPP'\lib;'
    call EnvAddFront fRM, 'ipfc',        sPathCPP'\bin;'
    call EnvAddFront fRM, 'cpplpath4',   sPathCPP'\macros;'
    call EnvSet      fRM, 'system_ice',  sPathCPP'\etc\system.ice'
    call EnvSet      fRM, 'vbpath',      sPathCPP'\ivb'
    call EnvSet      fRM, 'vacppmain',   sPathCPP;
    call EnvSet      fRM, 'os2',         '1'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathCPP'\bin\iccv4.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\ilib.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\ire.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\vacide.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\cppfilt.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\dllrname.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\bin\patrace.exe', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cpprms40.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cpprmi40.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cpprmo40.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cpprni40.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\lib\cpprds40.lib', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\include\builtin.h', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\include\conio.h', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\include\ismavl.h', fQuiet),
        |    FileExists(sPathCPP'\include\os2.h'),
        |    FileExists(sPathCPP'\include\os2win.h'),
        |    FileExists(sPathCPP'\include\pm.h'),
        |   \CfgVerifyFile(sPathCPP'\include\sys\utime.h', fQuiet),
        |   \CfgVerifyFile(sPathCPP'\help\cpplbm40.msg', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('iccv4', 0, fQuiet, 'IBM VisualAge for C++ Version 4.0 C Compiler');
    if (rc = 0) then
        rc = CheckCmdOutput('ilib /?', 0, fQuiet, 'IBM Librarian for OS/2(R) Version 03.99.cc_981110'); /* is this really FP2?????? */
    if (rc = 0) then
        rc = CheckCmdOutput('vacbld -?', 0, fQuiet, 'IBM(R) VisualAge(R) C++ Professional, Version 4.0 (981117)'); /* is this really FP2?????? */
return rc;




/*
 * WarpIn
 */
WarpIn: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet

    /*
     * WarpIn main directory.
     */
    sPathWarpIn = PathQuery('warpin', sToolId, sOperation);
    if (sPathWarpIn = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_WARPIN', sPathWarpIn;
    call EnvAddFront fRM, 'path',        sPathWarpIn';'
    call EnvAddFront fRM, 'beginlibpath',sPathWarpIn';'
    call EnvAddFront fRM, 'bookshelf',   sPathWarpIn';'

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathWarpIn'\wic.exe', fQuiet),
        |   \CfgVerifyFile(sPathWarpIn'\wpirtl.dll', fQuiet),
        |   \CfgVerifyFile(sPathWarpIn'\warpin.exe', fQuiet),
        ) then
        return 2;
    rc = CheckCmdOutput('wic', 1, fQuiet, '2002 - WarpIn archive creation and maintenance');
return rc;



/*
 * WatCom C/C++ v11.0
 */
WatComC11: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet, iBits

    /*
     * Watcom C/C++ v11.0 main directory
     */
    sPathWatcom = PathQuery('watcom11', sToolId, sOperation);
    if (sPathWatcom = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_WATCOM', sPathWatcom
    call EnvSet      fRM, 'CCENV',      'WAT'
    call EnvSet      fRM, 'BUILD_ENV',  'WAT11'
    if (iBits == 16) then
        call EnvSet      fRM, 'BUILD_ENV',  'WAT11-16'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvSet      fRM, 'watcom',      sPathWatcom
    call EnvAddFront fRM, 'path',        sPathWatcom'\binp;'sPathWatcom'\binw;'
    call EnvAddFront fRM, 'beginlibpath',sPathWatcom'\binp\dll;'
    call EnvAddFront fRM, 'help',        sPathWatcom'\binp\help;'
    call EnvAddEnd   fRM, 'bookshelf',   sPathWatcom'\binp\help;'
    call EnvAddFront fRM, 'include',     sPathWatcom'\h;'sPathWatcom'\h\os2;'sPathWatcom'\h\nt;'
    call EnvAddFront fRM, 'lib',         sPathWatcom'\lib386;'sPathWatcom'\lib386\os2;'sPathWatcom'\lib286;'sPathWatcom'\lib286\os2;'
    call EnvSet      fRM, 'edpath',      sPathWatcom'EDDAT;'
    /*
    rem detach %watcom%\BINP\BATSERV.EXE
    rem detach %watcom%\BINP\NMPBIND.EXE
    */

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathWatcom'\binp\wpp.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\binp\wcc.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\binp\wcc386.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\binp\wpp386.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\binp\wlink.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\lib286\os2\os2.lib', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\lib386\os2\clbrdll.lib', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\lib386\os2\clib3r.lib', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\lib386\nt\kernel32.lib', fQuiet,1),
        |   \CfgVerifyFile(sPathWatcom'\lib386\nt\clbrdll.lib', fQuiet,1),
        |   \CfgVerifyFile(sPathWatcom'\lib386\nt\clib3r.lib', fQuiet,1),
        ) then
        return 2;
    rc = CheckCmdOutput('wcc', 8, fQuiet, 'Watcom C16 Optimizing Compiler  Version 11.0 '||'0d0a'x);
    if (rc = 0) then
        rc = CheckCmdOutput('wpp', 8, fQuiet, 'Watcom C++16 Optimizing Compiler  Version 11.0 '||'0d0a'x);
    if (rc = 0) then
        rc = CheckCmdOutput('wcc386', 8, fQuiet, 'Watcom C32 Optimizing Compiler  Version 11.0 '||'0d0a'x);
    if (rc = 0) then
        rc = CheckCmdOutput('wpp386', 8, fQuiet, 'Watcom C++32 Optimizing Compiler  Version 11.0 '||'0d0a'x);
    if (rc = 0) then
        rc = CheckCmdOutput('wlink form ELF', 1, fQuiet, 'WATCOM Linker Version 11.0'||'0d0a'x);
return rc;


/*
 * WatCom C/C++ v11.0c
 */
WatComC11c: procedure expose aCfg. aPath. sPathFile
    parse arg sToolId,sOperation,fRM,fQuiet, iBits

    /*
     * Watcom C/C++ v11.0c main directory
     */
    sPathWatcom = PathQuery('watcom11c', sToolId, sOperation);
    if (sPathWatcom = '') then
        return 1;
    /* If config operation we're done now. */
    if (pos('config', sOperation) > 0) then
        return 0;

    /*
     * Installing the environment variables.
     */
    call EnvSet      fRM, 'PATH_WATCOM', sPathWatcom
    call EnvSet      fRM, 'CCENV',      'WAT'
    call EnvSet      fRM, 'BUILD_ENV',  'WAT11C'
    if (iBits = 16) then
        call EnvSet      fRM, 'BUILD_ENV',  'WAT11C-16'
    call EnvSet      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvSet      fRM, 'watcom',      sPathWatcom
    call EnvAddFront fRM, 'path',        sPathWatcom'\binp;'sPathWatcom'\binw;'
    call EnvAddFront fRM, 'beginlibpath',sPathWatcom'\binp\dll;'
    call EnvAddFront fRM, 'help',        sPathWatcom'\binp\help;'
    call EnvAddEnd   fRM, 'bookshelf',   sPathWatcom'\binp\help;'
    if (iBits = 16) then
        call EnvAddFront fRM, 'include',     sPathWatcom'\h;'sPathWatcom'\h\os21x;'
    else
        call EnvAddFront fRM, 'include',     sPathWatcom'\h;'sPathWatcom'\h\os2;'sPathWatcom'\h\nt;'
    call EnvAddFront fRM, 'lib',         sPathWatcom'\lib386;'sPathWatcom'\lib386\os2;'sPathWatcom'\lib286;'sPathWatcom'\lib286\os2;'
    call EnvSet      fRM, 'edpath',      sPathWatcom'EDDAT;'
    /*
    rem detach %watcom%\BINP\BATSERV.EXE
    rem detach %watcom%\BINP\NMPBIND.EXE
    */

    /*
     * Verify.
     */
    if (pos('verify', sOperation) <= 0) then
        return 0;
    if (    \CfgVerifyFile(sPathWatcom'\binp\wpp.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\binp\wcc.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\binp\wcc386.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\binp\wpp386.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\binp\wlink.exe', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\lib286\os2\os2.lib', fQuiet, 1),
        |   \CfgVerifyFile(sPathWatcom'\lib386\os2\clbrdll.lib', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\lib386\os2\clib3r.lib', fQuiet),
        |   \CfgVerifyFile(sPathWatcom'\lib386\nt\kernel32.lib', fQuiet, 1),
        |   \CfgVerifyFile(sPathWatcom'\lib386\nt\clbrdll.lib', fQuiet, 1),
        |   \CfgVerifyFile(sPathWatcom'\lib386\nt\clib3r.lib', fQuiet, 1),
        ) then
        return 2;
    rc = CheckCmdOutput('wcc', 8, fQuiet, 'Watcom C16 Optimizing Compiler  Version 11.0c');
    if (rc = 0) then
        rc = CheckCmdOutput('wpp', 8, fQuiet, 'Watcom C++16 Optimizing Compiler  Version 11.0c');
    if (rc = 0) then
        rc = CheckCmdOutput('wcc386', 8, fQuiet, 'Watcom C32 Optimizing Compiler  Version 11.0c');
    if (rc = 0) then
        rc = CheckCmdOutput('wpp386', 8, fQuiet, 'Watcom C++32 Optimizing Compiler  Version 11.0c');
    if (rc = 0) then
        rc = CheckCmdOutput('wlink form ELF', 1, fQuiet, 'WATCOM Linker Version 11.0c');
return rc;

