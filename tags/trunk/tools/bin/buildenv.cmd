/* $Id: buildenv.cmd,v 1.7 2002-05-16 11:50:22 bird Exp $
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
 * Known problems:
 *     - LANG is set to en_US by both VAC36 and TOOLKIT45. When unsetting
 *       those the original value, for example of no_NO, is not restored.
 */

    Address CMD '@echo off';

    signal on novalue name NoValueHandler

    /*
     * Create argument array with lowercase arguments.
     */
    parse arg sEnv.1 sEnv.2 sEnv.3 sEnv.4 sEnv.5 sEnv.6 sEnv.7 sEnv.8 sEnv.9 sEnv.10 sEnv.11 sEnv.12 sEnv.13 sEnv.14 sEnv.15 sEnv.16 sEnv.17 sEnv.18 sEnv.19 sEnv.20 sEnv.21

    i = 1;
    do while (sEnv.i <> '')
        sEnv.i = translate(strip(sEnv.i), 'abcdefghijklmnopqrstuvwxyz‘›†', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ’');
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
     * Configuration - sorted please!
     */
    parse source . . sPathFile .
    sPathFile = sPathFile||'.paths';
    aPath.0 = 0;

    i = 1;
    /* environment id               The function with args.                 Optional verify data.                       increment index */
    aCfg.i.sId = 'cvs';             aCfg.i.sSet = 'CVS';                    aCfg.i.sDesc = 'CVS v1.10 or later';        i = i + 1;
    aCfg.i.sId = 'db2v52';          aCfg.i.sSet = 'db2v52';                 aCfg.i.sDesc = 'DB2 v5.2 Dev Edition';      i = i + 1;
    aCfg.i.sId = 'ddkbase';         aCfg.i.sSet = 'DDKBase';                aCfg.i.sDesc = 'DDK base (recent)';  i = i + 1;
    aCfg.i.sId = 'emx';             aCfg.i.sSet = 'EMX';                    aCfg.i.sDesc = 'EMX v0.9d fixpack 04';      i = i + 1;
    aCfg.i.sId = 'emxpgcc';         aCfg.i.sSet = 'EMXPGCC';                aCfg.i.sDesc = 'Pentium Optimized GCC/EMX v1.1.1 r2 with binutils 2.9.1'; i = i + 1;
    aCfg.i.sId = 'icatgam';         aCfg.i.sSet = 'ICATGam';                aCfg.i.sDesc = 'ICAT for OS/2 latest'; i = i + 1;
    aCfg.i.sId = 'icatgam406rc1';   aCfg.i.sSet = 'ICATGam406RC1';          aCfg.i.sDesc = 'ICAT for OS/2 v4.0.6 release candidate 1'; i = i + 1;
    aCfg.i.sId = 'icatpe';          aCfg.i.sSet = 'ICATPe';                 aCfg.i.sDesc = 'ICAT for OS/2 with PE support (test version)'; i = i + 1;
    aCfg.i.sId = 'ida';             aCfg.i.sSet = 'IDA414';                 aCfg.i.sDesc = 'Interactive DisAssembler (IDA) (latest)'; i = i + 1;
    aCfg.i.sId = 'ida38';           aCfg.i.sSet = 'IDA38';                  aCfg.i.sDesc = 'Interactive DisAssembler (IDA) v3.80 (historical)'; i = i + 1;
    aCfg.i.sId = 'ida40';           aCfg.i.sSet = 'IDA40';                  aCfg.i.sDesc = 'Interactive DisAssembler (IDA) v4.0 (historical)'; i = i + 1;
    aCfg.i.sId = 'ida414';          aCfg.i.sSet = 'IDA414';                 aCfg.i.sDesc = 'Interactive DisAssembler (IDA) v4.14'; i = i + 1;
    aCfg.i.sId = 'idasdk';          aCfg.i.sSet = 'IDASDK';                 aCfg.i.sDesc = 'Interactive DisAssembler (IDA) SDK'; i = i + 1;
    aCfg.i.sId = 'mode12050';       aCfg.i.sSet = 'Mode,120,50';            aCfg.i.sDesc = 'mode 120,50'; i = i + 1;
    aCfg.i.sId = 'mode8050';        aCfg.i.sSet = 'Mode,80,50';             aCfg.i.sDesc = 'mode 80,50'; i = i + 1;
/* obsolete id   aCfg.i.sId = 'msc60';           aCfg.i.sSet = 'MSCV6_16';               aCfg.i.sDesc = 'MicroSoft C v6.0a 16-bit'; i = i + 1; */
    aCfg.i.sId = 'mscv6';           aCfg.i.sSet = 'MSCV6_32';               aCfg.i.sDesc = 'MicroSoft C v6.0 32-bit'; i = i + 1;
    aCfg.i.sId = 'mscv6-16';        aCfg.i.sSet = 'MSCV6_16';               aCfg.i.sDesc = 'MicroSoft C v6.0a 16-bit'; i = i + 1;
    aCfg.i.sId = 'mysql';           aCfg.i.sSet = 'mySQL';                  aCfg.i.sDesc = 'MySql any version (latest from Yuri is recommended)'; i = i + 1;
    aCfg.i.sId = 'netqos2';         aCfg.i.sSet = 'NetQOS2';                aCfg.i.sDesc = 'NetQOS2 - help system for VAC40,VAC365,DB2 and more.'; i = i + 1;
    aCfg.i.sId = 'odin32testcase';  aCfg.i.sSet = 'Odin32Testcase';         aCfg.i.sDesc = 'Odin32 testcase setup'; i = i + 1;
    aCfg.i.sId = 'perl';            aCfg.i.sSet = 'Perl';                   aCfg.i.sDesc = 'Perl v5.0053'; i = i + 1;
    aCfg.i.sId = 'python';          aCfg.i.sSet = 'Python';                 aCfg.i.sDesc = 'Python v1.5'; i = i + 1;
    aCfg.i.sId = 'toolkit40';       aCfg.i.sSet = 'Toolkit40';              aCfg.i.sDesc = 'Toolkit v4.0 CSD 4'; i = i + 1;
    aCfg.i.sId = 'toolkit45';       aCfg.i.sSet = 'Toolkit45';              aCfg.i.sDesc = 'Toolkit v4.5'; i = i + 1;
    aCfg.i.sId = 'toolkit451';      aCfg.i.sSet = 'Toolkit451';             aCfg.i.sDesc = 'Toolkit v4.5.1'; i = i + 1;
    aCfg.i.sId = 'unix';            aCfg.i.sSet = 'Unix';                   aCfg.i.sDesc = 'Misc unix stuff.'; i = i + 1;
/* obsolete id   aCfg.i.sId = 'vac30';           aCfg.i.sSet = 'VAC308';                 aCfg.i.sDesc = 'VisualAge for C++ v3.08'; i = i + 1; */
    aCfg.i.sId = 'vac308';           aCfg.i.sSet = 'VAC308';                 aCfg.i.sDesc = 'VisualAge for C++ v3.08'; i = i + 1;
/* osbolete id    aCfg.i.sId = 'vac36';           aCfg.i.sSet = 'VAC365';                 aCfg.i.sDesc = ''; i = i + 1; */
    aCfg.i.sId = 'vac365';          aCfg.i.sSet = 'VAC365';                 aCfg.i.sDesc = 'VisualAge for C++ v3.6.5 FP2 with latest optimizer fixes.'; i = i + 1;
    aCfg.i.sId = 'vac40';           aCfg.i.sSet = 'VAC40';                  aCfg.i.sDesc = 'VisualAge for C++ v4.0 FP2(??)'; i = i + 1;
    aCfg.i.sId = 'warpin';          aCfg.i.sSet = 'WarpIn';                 aCfg.i.sDesc = 'WarpIn 0.9.18+ (for Odin32 18 with fix is required)'; i = i + 1;
    aCfg.i.sId = 'watcomc11';       aCfg.i.sSet = 'WatComC11,32,';          aCfg.i.sDesc = 'Watcom C/C++ v11.0 32-bit (no fixes)'; i = i + 1;
    aCfg.i.sId = 'watcomc11-16';    aCfg.i.sSet = 'WatComC11,16';           aCfg.i.sDesc = 'Watcom C/C++ v11.0 16-bit (no fixes)'; i = i + 1;
    aCfg.i.sId = 'watcomc11c';      aCfg.i.sSet = 'WatComC11c,32';          aCfg.i.sDesc = 'Watcom C/C++ v11.0c 32-bit (beta)'; i = i + 1;
    aCfg.i.sId = 'watcomc11c-16';   aCfg.i.sSet = 'WatComC11c,16';          aCfg.i.sDesc = 'Watcom C/C++ v11.0c 16-bit (beta)'; i = i + 1;
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
        if (ch = '-' | ch = '+' | ch = '*') then
            sEnv.i = substr(sEnv.i, 1, length(sEnv.i) - 1);
        fRM = (ch = '-');
        fCfg = (ch = '*');

        /*
         * do the switch.
         */
        rc = 0;
        select
            when (sEnv.i = 'debug') then do
                rc = EnvVar_Set(0, 'DEBUG','1');
                rc = EnvVar_Set(0, 'RELEASE','');
                rc = EnvVar_Set(0, 'BUILD_MODE','DEBUG');
            end
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

            when (sEnv.i = 'buildsetup') then
                rc = EnvVar_Set(0, 'BUILD_SETUP_MAK','make\setup.mak');

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
                        if (fCfg) then; say 'Verify & Config of the 'sEnv.i' ('CfgDesc(sEnv.i)') environment...';
                        if (pos(',', aCfg.j.sSet) > 0) then
                            sRexx = substr(aCfg.j.sSet, 1, pos(',', aCfg.j.sSet) - 1) || '(aCfg.j.sSet,fRM,fCfg',
                                 || substr(aCfg.j.sSet, pos(',', aCfg.j.sSet)) || ')';
                        else
                            sRexx = aCfg.j.sSet || '(sEnv.i,fRM,fCfg)';
                        interpret 'rc = '||sRexx;

                        /*
                         * Loop till rc=0 or leave.
                         */
                        do while (rc <> 0)
                            select
                                when (rc = 1) then
                                do
                                    if (fCfg = 0) then
                                    do
                                        say 'fatal error: One of the paths for environment '''sEnv.i''' wasn'' found.';
                                        say '             Please (re-)configure the environment.';
                                        call SysSleep 5;
                                        exit(16);
                                    end
                                    say 'warning: The user refused to give a path, continuing.';
                                end

                                when (rc = 2) then
                                    say 'error: verify failed ''cause some vital file wasn''t found.';

                                when (rc = 49) then
                                    say 'error: verify failed ''cause some vital command didn''t return as expected.';

                                when (rc = 99) then
                                    say 'error: verify failed ''cause some vital command didn''t return the expected output.';

                                otherwise
                                do
                                    say 'internal error: bad return code from '''sRexx''' rc=' rc'.';
                                    call SysSleep 5;
                                    exit(16);
                                end
                            end

                            /*
                             * Retry the config if the user wan't so.
                             */
                            fCfg = 3; /* forced reconfig. */
                            say '(Re)Try to config the environment' sEnv.i '('CfgDesc(sEnv.i)')? (y/N)';
                            sAnswer = PullUser(1);
                            if (substr(strip(sAnswer),1,1) <> 'Y') then
                                leave;
                            interpret 'rc = '||sRexx;
                        end
                        rc = 0;
                        leave;
                    end
                end

                if (\fFound) then
                do
                    say 'error: unknown environment! - 'sEnv.i;
                    call SysSleep 5;
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
return


/**
 * The Directory Configuration Function.
 *
 * @returns Lower cased, absolute, backward slashed, path to program.
 * @param   sPathId     Program identifier. (lowercase!)
 *
 * @remark
 * Unless this causes a lot of trouble, we'll keep it in the script.
 * Having this externaly will slow down the script execution
 * somewhat I'm afraid. And the script is slow enought as it is.
 *
 */
PathQuery: procedure expose aCfg. aPath. sPathFile
    parse arg sPathId, sEnvId, fCfg, fOptional

    if (fOptional = '') then
        fOptional = 0;

    if (aPath.0 = 0) then
    do  /*
         * Read path config file
         */
        call PathRead;

        /*
         * If no data found fill in defaults.
         */
        if (aPath.0 = 0) then
        do
            say 'Info: No or empty path file, using defaults.';
            i = 1;
            aPath.i.sPId = 'cvs';                       aPath.i.sPath = 'f:\cvs';                       i = i + 1;
            aPath.i.sPId = 'emx';                       aPath.i.sPath = 'f:\emx';                       i = i + 1;
            aPath.i.sPId = 'emxpgcc';                   aPath.i.sPath = 'f:\emxpgcc';                   i = i + 1;
            aPath.i.sPId = 'db2v52';                    aPath.i.sPath = 'f:\sqllib52';                  i = i + 1;
            aPath.i.sPId = 'icatgam';                   aPath.i.sPath = 'f:\icatos2';                   i = i + 1;
            aPath.i.sPId = 'icatgam406rc1';             aPath.i.sPath = 'f:\icatos2.4.0.6.rc1';         i = i + 1;
            aPath.i.sPId = 'icatpe';                    aPath.i.sPath = 'f:\icatpe';                    i = i + 1;
            aPath.i.sPId = 'ida38';                     aPath.i.sPath = 'f:\ida38';                     i = i + 1;
            aPath.i.sPId = 'ida40';                     aPath.i.sPath = 'f:\ida401';                    i = i + 1;
            aPath.i.sPId = 'ida414';                    aPath.i.sPath = 'f:\ida414';                    i = i + 1;
            aPath.i.sPId = 'idasdk';                    aPath.i.sPath = 'f:\idasdk';                    i = i + 1;
            aPath.i.sPId = 'ddkbase';                   aPath.i.sPath = 'f:\ddk\base';                  i = i + 1;
            aPath.i.sPId = 'home';                      aPath.i.sPath = 'e:\user\kso';                  i = i + 1;
            aPath.i.sPId = 'mscv6-16';                  aPath.i.sPath = 'f:\ddktools\toolkits\msc60';   i = i + 1;
            aPath.i.sPId = 'mysql';                     aPath.i.sPath = 'f:\mysql2';                    i = i + 1;
            aPath.i.sPId = 'netqos2';                   aPath.i.sPath = 'f:\netqos2';                   i = i + 1;
            aPath.i.sPId = 'perl';                      aPath.i.sPath = 'f:\perllib';                   i = i + 1;
            aPath.i.sPId = 'python';                    aPath.i.sPath = 'f:\python';                    i = i + 1;
            aPath.i.sPId = 'toolkit40';                 aPath.i.sPath = 'f:\toolkit';                   i = i + 1;
            aPath.i.sPId = 'toolkit45';                 aPath.i.sPath = 'f:\toolkit45';                 i = i + 1;
            aPath.i.sPId = 'toolkit451';                aPath.i.sPath = 'f:\toolkit451';                i = i + 1;
            aPath.i.sPId = 'unixroot';                  aPath.i.sPath = 'e:\unix';                      i = i + 1;
            aPath.i.sPId = 'xfree86';                   aPath.i.sPath = 'e:\xfree86';                   i = i + 1;
            aPath.i.sPId = 'vac308';                    aPath.i.sPath = 'f:\ibmcpp';                    i = i + 1;
            aPath.i.sPId = 'vac365';                    aPath.i.sPath = 'f:\ibmcxxo';                   i = i + 1;
            aPath.i.sPId = 'vac40';                     aPath.i.sPath = 'f:\ibmcpp40';                  i = i + 1;
            aPath.i.sPId = 'warpin';                    aPath.i.sPath = 'f:\warpin';                    i = i + 1;
            aPath.i.sPId = 'watcom11';                  aPath.i.sPath = 'f:\watcom';                    i = i + 1;
            aPath.i.sPId = 'watcom11c';                 aPath.i.sPath = 'f:\watcom11c';                 i = i + 1;
            aPath.i.sPId = 'testcase_drive_unused';     aPath.i.sPath = 'l'; /* reqired */              i = i + 1;
            aPath.i.sPId = 'testcase_drive_fixed';      aPath.i.sPath = 'c'; /* reqired */              i = i + 1;
            aPath.i.sPId = 'testcase_drive_floppy';     aPath.i.sPath = 'a'; /* reqired */              i = i + 1;
            aPath.i.sPId = 'testcase_drive_cdrom';      aPath.i.sPath = 'k'; /* optional */             i = i + 1;
            aPath.i.sPId = 'testcase_drive_network';    aPath.i.sPath = 's'; /* optional */             i = i + 1;
            /*aPath.i.sPId = 'testcase_drive_ramdisk';    aPath.i.sPath = '';  /* optional */             i = i + 1;*/
            /*aPath.i.sPId = '';          aPath.i.sPath =      i = i + 1;*/
            aPath.0 = i - 1;

            /*
             * Write to file.
             */
            call PathWrite;
        end
    end

    /*
     * Check for forced config.
     */
    if (fCfg = 3) then
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
     * Configure then we'll ask the user.
     */
    if (fCfg > 0) then
        return PathConfig(fCfg, sPathId, sEnvId);

    /*
     * Error
     */
    if (\fOptional) then
    do
        say 'Fatal error: Path information for program '''sPathId''' was not found.';
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
            if ((aPath.i.sPath = '') | (aPath.i.sPId = '') | (translate(sLine,'','#!$@%|<>;õ&Ï') <> sLine) ) then
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
 * Configure a path.
 * @returns Path on success.
 *          '' on failure.
 * @param   fCfg        The config flags.
 *                      1 = configure if needed.
 *                      3 = force config.
 * @param   sPathId     The path to configure.
 * @param   sEnvId      The environment Id.
 */
PathConfig: procedure expose aCfg. aPath. sPathFile
    parse arg fCfg, sPathId, sEnvId

    /*
     * Config loop.
     */
    if (fCfg = 1) then
    do
        say 'Do you want to configure the path '''sPathId''' for the '''sEnvId'''('CfgDesc(sEnvId)') environment? (y/N)';
        sAnswer = PullUser(1);
        if (substr(strip(sAnswer),1,1) <> 'Y') then
            return '';
    end
    do i = 1 to 128

        say 'Give us the path then:'
        sThePath = translate(strip(strip(strip(PullUser()), 'T','\'),'T','/'), '\', '/');
        /*say 'Debug: sThePath='sThePath;*/
        if ((sThePath <> '') & (sThePath = translate(sThePath,'','#!$@%|<>;õ&Ï='))) then
        do
            /*
             * Add it to internal struct.
             */
            call PathRemove(sPathId);
            j = aPath.0 + 1;
            aPath.j.sPId = strip(sPathId);
            aPath.j.sPath = translate(strip(strip(strip(sThePath), 'T','\'),'T','/'), '\', '/');
            aPath.0 = j;

            /*
             * Write to file and return successfully.
             */
            call PathWrite;
            return sThePath;
        end
        else
            say 'error: invalid path name.';

        /* ask if retry */
        if (i >= 2) then
            say 'You''re not trying hard, are you?';
        say 'Wanna try giving us an *valid* path for the path '''sPathId''' for the '''sEnvId'''('CfgDesc(sEnvId)') environment? (y/N)';
        sAnswer = PullUser(1);
        if (substr(strip(sAnswer),1,1) <> 'Y') then
            leave;
    end

    say 'Giving up!';
return '';


/**
 * Get the description of an environment.
 * @returns Description string.
 *          '' if not found.
 * @param   sEnvId      Environment id.
 */
CfgDesc: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId
    do i = 1 to aCfg.0
        if (aCfg.i.sId = sEnvId) then
            return aCfg.i.sDesc;
    end
return sEnvId;


/**
 * Lookups up an env. config in the aCfg. array.
 * @return  Index of sEnvId.
 *          aCfg.0+1 on error.
 * @param   sEnvId      Environment id.
 */
CfgLookup: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId
    iEnv = 1;
    do while ((iEnv <= aCfg.0) & (aCfg.iEnv.sId <> sEnvId))
        iEnv = iEnv + 1;
    end
return iEnv;


/**
 * Get user response and empties the input queue.
 * @returns     User input.
 * @param       fUpper  If present and set uppercase the user response.
 */
PullUser: procedure
    parse arg fUpper
    if (fUpper = '') then
        fUpper = 0;

    if (fUpper) then
        pull sAnswer;
    else
        parse pull sAnswer;

    do while (Queued())
        pull dummy;
    end
return sAnswer;


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
    say sComplain ''''sDir'''.';
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
    parse arg sCmd, rcCmdExpected, sOutputPartExpected

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
    rc = 0
    if (rcCmd = rcCmdExpected) then
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
    if (rc <> 0) then
        say 'Debug:' sCmd 'rc='rc' rcCmd='rcCmd;
return rc;


/**
 * Environment procedures section
 * @returns 0 on success.
 *          1 if PathQuery() failed.
 *          2 if some vital file wasn't found in the config verify.
 *          49 if verify command rc mismatched.
 *          99 if verify command output mismatched.
 **/


/*
 * Concurrent Versions System (CVS)
 */
CVS: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg


    /*
     * Concurrent Versions System (CVS) main directory.
     */
    sPathCVS    = PathQuery('cvs', sEnvId, fCfg);
    if (sPathCVS = '') then
        return 1;
    sPathHome   = PathQuery('home', sEnvId, fCfg);
    if (sPathHome = '') then
        return 1;

    call EnvVar_Set      fRM, 'PATH_CVS',    sPathCVS;
    call EnvVar_AddFront fRM, 'path',        sPathCVS'\bin;'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathCVS'\book;'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathCVS'\book;'
    call EnvVar_Set      fRM, 'home',        translate(sPathHome, '/','\');

    /* verify */
    if (fCfg > 0) then
    do
        if (\FileExists(sPathCVS'\bin\cvs.exe', 'error: verify didn''t find')) then
            return 2;
        if (length(sPathHome) <= 2) then
        do
            say 'Error: The home directory is to short!';
            return 2;
        end
        if (\DirExists(sPathHome)) then
        do
            say 'Error: The home directory must exist!';
            return 2;
        end
        return CheckCmdOutput('cvs --version', 0, 'Concurrent Versions System (CVS) 1.1');
    end
return 0;


/*
 * EMX
 */
EMX: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * EMX/GCC main directory.
     */
    sEMX        = PathQuery('emx', sEnvId, fCfg);
    if (sEMX = '') then
        return 1;

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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sEmxBack'\bin\gcc.exe', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\bin\emxomf.exe', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\bin\emxrev.cmd', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\mt\c.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\mt\c.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\mt\sys.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\mt\emx.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\mt\emx.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\mt\c_import.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\mt\c_import.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\c_alias.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\c_alias.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\emx2.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\emx2.lib', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('gcc --version', 0, '2.8.1');
        if (rc = 0) then
            rc = CheckCmdOutput('emxrev.cmd', 0,,
                                'EMX : revision = 61'||'0d0a'x ||,
                                'EMXIO : revision = 60'||'0d0a'x||,
                                'EMXLIBC : revision = 63'||'0d0a'x||,
                                'EMXLIBCM : revision = 64'||'0d0a'x||,
                                'EMXLIBCS : revision = 64'||'0d0a'x||,
                                'EMXWRAP : revision = 60'||'0d0a'x);
        return rc;
    end
return 0;


/*
 * EMX PGCC - must be installed on to the ordinar EMX.
 */
EMXPGCC: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * EMX/GCC main directory.
     */
    sEMXPGCC    = PathQuery('emxpgcc', sEnvId, fCfg);
    if (sEMXPGCC = '') then
        return 1;
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sEmxBack'\bin\gcc.exe', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\bin\g++.exe', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\bin\as.exe', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\bin\emxomf.exe', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\gcc29160.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\gcc29160.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\iberty.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\iberty.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\iberty_s.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\iberty_s.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\opcodes.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\opcodes.lib', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\opcodes_s.a', 'error: verify didn''t find'),
            |   \FileExists(sEmxBack'\lib\opcodes_s.lib', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('gcc --version', 0, 'pgcc-2.95.2');
        if (rc = 0) then
            rc = CheckCmdOutput('g++ --version', 0, 'pgcc-2.95.2');
        if (rc = 0) then
            rc = CheckCmdOutput('as --version', 0, 'GNU assembler 2.9.1');
        return rc;
    end
return 0;

/*
 * IBM DB2 v5.2
 */
db2v52: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    sPathDB2   = PathQuery('db2v52', sEnvId, fCfg);
    if (sPathDB2 = '') then
        return 1;
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathDB2'\bin\db2.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\bin\sqlbind.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\bin\sqlprep.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\lib\db2api.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\lib\db2cli.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\lib\db2gmf32.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\include\sql.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\include\sqlcodes.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\include\sqlsystm.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDB2'\include\sqlcli.h', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('echo quit | db2', 0, 'Command Line Processor for DB2 SDK 5.2.0');
        return rc;
    end
return 0;


/*
 *  Device Driver Kit (DDK) base.
 */
DDKBase: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * Device Driver Kit (DDK) (v4.0+) base (important not main) directory.
     */
    sPathDDKBase    = PathQuery('ddkbase', sEnvId, fCfg);
    if (sPathDDKBase = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_DDKBASE',sPathDDKBase;
    call EnvVar_AddFront fRM, 'path',        sPathDDKBase'\tools;'
    call EnvVar_AddFront fRM, 'include',     sPathDDKBase'\h;'sPathDDKBase'\inc;'sPathDDKBase'\inc32;'
    call EnvVar_AddFront fRM, 'include16',   sPathDDKBase'\h;'
    call EnvVar_AddFront fRM, 'lib',         sPathDDKBase'\lib;'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathDDKBase'\..\docs;'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathDDKBase'\tools\link.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\tools\link386.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\tools\cl386.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\tools\masm.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\tools\h2inc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\tools\lib.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\os2286.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\os2286p.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\os2386.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\os2386p.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\doscalls.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\dhcalls.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\addcalls.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\rmcalls.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\lib\vdh.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\h\infoseg.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\h\include.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\h386\pmddi.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\h386\pmddim.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\h386\limits.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\h386\string.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\inc\v8086.inc', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\inc\sas.inc', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\inc\pmwinx.inc', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\inc\infoseg.inc', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\inc\devhlp.inc', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\inc\devhlpp.inc', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('cl386', 0, 'Microsoft (R) Microsoft 386 C Compiler. Version 6.00.054');
        if (rc = 0) then
            rc = CheckCmdOutput('masm nul,nul,nul,nul;', 2, 'Microsoft (R) Macro Assembler Version 5.10A.15 Jul 07 15:25:03 1989');
        if (rc = 0) then
            rc = CheckCmdOutput('h2inc -?', 0, 'h2inc - .H to .INC file translator (version 13.29)');
        if (rc = 0) then
            rc = CheckCmdOutput('type' sPathDDKBase'\inc\devhlp.inc', 0, 'DevHlp_ReadFileAt');
        return rc;
    end
return 0;


/*
 * ICAT Debugger
 */
ICATGam: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    sPathICAT   = PathQuery('icatgam', sEnvId, fCfg);
    if (sPathICAT = '') then
        return 1;
    call EnvVar_Set      fRm, 'PATH_ICATGAM', sPathICAT;
    call EnvVar_AddFront fRm, 'beginlibpath',sPathICAT'\dll;'
    call EnvVar_AddFront fRm, 'path',        sPathICAT'\bin;'
    call EnvVar_AddFront fRm, 'dpath',       sPathICAT'\help;'
    call EnvVar_AddFront fRm, 'help',        sPathICAT'\help;'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathICAT'\bin\icatgam.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gamoou3.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gam5lde.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gam5cx.dll', 'error: verify didn''t find'),
            ) then
            return 2;
        return 0;
    end
return 0;


/*
 * ICAT Debugger
 */
ICATGam406RC1: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    sPathICAT   = PathQuery('icatgam406rc1', sEnvId, fCfg);
    if (sPathICAT = '') then
        return 1;
    call EnvVar_Set      fRm, 'PATH_ICATGAM', sPathICAT;
    call EnvVar_AddFront fRm, 'beginlibpath',sPathICAT'\dll;'
    call EnvVar_AddFront fRm, 'path',        sPathICAT'\bin;'
    call EnvVar_AddFront fRm, 'dpath',       sPathICAT'\help;'
    call EnvVar_AddFront fRm, 'help',        sPathICAT'\help;'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathICAT'\bin\icatgam.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gamoou3.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gam5lde.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gam5cx.dll', 'error: verify didn''t find'),
            ) then
            return 2;
        return 0;
    end
return 0;



/*
 * ICAT Debugger for PE images.
 */
ICATPe: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    sPathICAT   = PathQuery('icatgam', sEnvId, fCfg);
    if (sPathICAT = '') then
        return 1;
    sPathICATPe = PathQuery('icatpe', sEnvId, fCfg);
    if (sPathICATPe = '') then
        return 1;
    call EnvVar_Set      fRm, 'PATH_ICATGAM',sPathICAT;
    call EnvVar_Set      fRm, 'PATH_ICATPE', sPathICATPe;
    call EnvVar_AddFront fRm, 'beginlibpath',sPathICATPe'\bin;'sPathICAT'\dll;'
    call EnvVar_AddFront fRm, 'path',        sPathICATPe'\bin;'sPathICAT'\bin;'
    call EnvVar_AddFront fRm, 'dpath',       sPathICATPe'\bin;'sPathICAT'\help;'
    call EnvVar_AddFront fRm, 'help',        sPathICATPe'\bin;'sPathICAT'\help;'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathICAT'\bin\icatgam.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gamoou3.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gam5lde.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICAT'\dll\gam5cx.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICATPe'\bin\icatgam.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathICATPe'\bin\gamoou3.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICATPe'\bin\gam5lde.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathICATPe'\bin\gam5cx.dll', 'error: verify didn''t find'),
            ) then
            return 2;
        return 0;
    end
return 0;



/*
 * Interactive Disassembler (IDA) v3.80a
 */
IDA38: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    /*
     * IDA main directory.
     */
    sPathIDA = PathQuery('ida38', sEnvId, fCfg);
    if (sPathIDA = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_IDA',       sPathIDA
    call EnvVar_AddFront fRM, 'path',           sPathIDA
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathIDA

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathIDA'\ida2.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\idaw.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\ida.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\pc.dll', 'error: verify didn''t find'),
            ) then
            return 2;
        return 0;
    end
return 0;


/*
 * Interactive Disassembler (IDA) v4.01
 */
IDA40: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    /*
     * IDA main directory.
     */
    sPathIDA = PathQuery('ida40', sEnvId, fCfg);
    if (sPathIDA = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_IDA',       sPathIDA
    call EnvVar_AddFront fRM, 'path',           sPathIDA
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathIDA

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathIDA'\ida2.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\idaw.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\ida.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\pc.dll', 'error: verify didn''t find'),
            ) then
            return 2;
        return 0;
    end
return 0;


/*
 * Interactive Disassembler (IDA) v4.14
 */
IDA414: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    /*
     * IDA main directory.
     */
    sPathIDA = PathQuery('ida414', sEnvId, fCfg);
    if (sPathIDA = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_IDA',       sPathIDA
    call EnvVar_AddFront fRM, 'path',           sPathIDA
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathIDA

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathIDA'\ida2.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\idaw.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\ida.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathIDA'\pc.dll', 'error: verify didn''t find'),
            ) then
            return 2;
        return 0;
    end
return 0;


/*
 * Interactive Disassembler (IDA) Plugin SDK (v5.0?)
 */
IDASDK: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    /*
     * IDA main directory.
     */
    sPathIDASDK = PathQuery('idasdk', sEnvId, fCfg);
    if (sPathIDASDK = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_IDASDK',    sPathIDASDK
    call EnvVar_AddFront fRM, 'include',        sPathIDASDK'\include;'
    call EnvVar_AddFront fRM, 'lib',            sPathIDASDK'\libwat.os2;'
    call EnvVar_AddFront fRM, 'path',           sPathIDASDK'\bin\os2;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathIDASDK'\bin\os2;'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathIDASDK'\os2wat.cfg', 'error: verify didn''t find'),
            |   \FileExists(sPathIDASDK'\d32wat.cfg', 'error: verify didn''t find'),
            |   \FileExists(sPathIDASDK'\include\exehdr.h', 'error: verify didn''t find'),
            |   \FileExists(sPathIDASDK'\include\ida.hpp', 'error: verify didn''t find'),
            |   \FileExists(sPathIDASDK'\include\vm.hpp', 'error: verify didn''t find'),
            |   \FileExists(sPathIDASDK'\libwat.os2\ida.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathIDASDK'\libwat.d32\ida.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathIDASDK'\libwat.d32\INIRT386.OBJ', 'error: verify didn''t find'),
            |   \FileExists(sPathIDASDK'\libbor.d32\ida.lib', 'error: verify didn''t find'),
            ) then
            return 2;
        return 0;
    end
return 0;


/*
 * Mode commandline.
 */
Mode: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg,cols,rows
    if (fRM) then
    do
        cols = 80;
        rows = 25;
    end
    Address CMD 'mode' cols','rows
return 0;


/*
 * Microsoft C v6.0a 16-bit
 */
MSCV6_16: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * Microsoft C v6.0a main directory.
     */
    sPathMSC    = PathQuery('mscv6-16', sEnvId, fCfg);
    if (sPathMSC = '') then
        return 1;
    call EnvVar_Set      fRM, 'BUILD_ENV',  'MSCV6-16'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'
    call EnvVar_Set      fRM, 'PATH_MSC',   sPathMSC;
    call EnvVar_AddFront fRM, 'path',       sPathMSC'\binp;'
    call EnvVar_AddFront fRM, 'include',    sPathMSC'\include;'
    call EnvVar_AddFront fRM, 'include16',  sPathMSC'\include;'
    call EnvVar_AddFront fRM, 'lib',        sPathMSC'\lib;'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathMSC'\binp\cl.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\lib\clibcep.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\lib\llibcep.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\lib\mlibcep.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\lib\slibcep.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\include\sysbits.h', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\include\dos.h', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\include\bios.h', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\include\string.h', 'error: verify didn''t find'),
            |   \FileExists(sPathMSC'\include\stdio.h', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('cl', 0, 'Microsoft (R) C Optimizing Compiler Version 6.00A.04');
        return rc;
    end
return 0;


/*
 * Microsoft C v6.0a 32-bit
 */
MSCV6_32: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * This is where the compiler really is.
     */
    call DDKBase 'ddkbase',fRM,fCfg;

    /*
     * Microsoft C v6.0a 32-bit main directory.
     */
    sPathDDKBase    = PathQuery('ddkbase', sEnvId, fCfg);
    if (sPathDDKBase = '') then
        return 1;
    call EnvVar_Set      fRM, 'BUILD_ENV',  'MSCV6'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'
    call EnvVar_Set      fRM, 'PATH_MSC',   sPathDDKBase;
    call EnvVar_AddFront fRM, 'include',    sPathDDKBase'\h386;'
    call EnvVar_AddFront fRM, 'lib',        sPathDDKBase'\lib;'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathDDKBase'\tools\cl386.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\h386\limits.h', 'error: verify didn''t find'),
            |   \FileExists(sPathDDKBase'\h386\string.h', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('cl386', 0, 'Microsoft (R) Microsoft 386 C Compiler. Version 6.00.054');
        return rc;
    end
return 0;


/*
 * mySQL Database system
 */
mySQL: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * mySQL Database system main directory.
     */
    sPathMySQL    = PathQuery('mysql', sEnvId, fCfg);
    if (sPathMySQL = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_MYSQL',     sPathMySQL;
    call EnvVar_AddFront fRM, 'path',           sPathMySQL'\bin;'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPathMySQL'\dll;'
    call EnvVar_AddFront fRM, 'include',        sPathMySQL'\include;'
    call EnvVar_AddFront fRM, 'bookshelf',      sPathMySQL'\doc;'sPathMySQL'\book';
    /*call EnvVar_AddFront fRM, 'lib',            sPathMySQL'\lib;'*/

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathMySQL'\bin\mysql.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathMySQL'\bin\mysqld.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathMySQL'\bin\mysqladmin.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathMySQL'\dll\mysql.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathMySQL'\include\mysql.h', 'error: verify didn''t find'),
            |   \FileExists(sPathMySQL'\include\mysql_com.h', 'error: verify didn''t find'),
            |   \FileExists(sPathMySQL'\include\mysql_version.h', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('mysql --version', 0, ', for ');
        return rc;
    end
return 0;



/*
 * NetQOS2 - help subsystem++ for VAC 3.6.5 and VAC 4.0
 */
NetQOS2: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    sPathNetQOS2 = PathQuery('netqos2', sEnvId, fCfg);
    if (sPathNetQOS2 = '') then
        return 1;
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathNetQOS2'\netq.exe', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('netq', 999, 'NETQ {START | STOP');
        return rc;
    end
return 0;


/*
 * Odin32 testcase setup.
 */
Odin32Testcase: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg
    if (  PathQuery('testcase_drive_unused', sEnvId, fCfg) = '',
        | PathQuery('testcase_drive_fixed', sEnvId, fCfg) = '',
        | PathQuery('testcase_drive_floppy', sEnvId, fCfg) = '',
        | PathQuery('testcase_drive_cdrom', sEnvId, fCfg) = '',
        ) then
        return 1;

    call EnvVar_Set      fRM, 'odin32_testcase_drive_unused',       PathQuery('testcase_drive_unused', sEnvId, fCfg);
    call EnvVar_Set      fRM, 'odin32_testcase_drive_fixed',        PathQuery('testcase_drive_fixed', sEnvId, fCfg);
    call EnvVar_Set      fRM, 'odin32_testcase_drive_floppy',       PathQuery('testcase_drive_floppy', sEnvId, fCfg);
    call EnvVar_Set      fRM, 'odin32_testcase_drive_cdrom',        PathQuery('testcase_drive_cdrom', sEnvId, fCfg);
    call EnvVar_Set      fRM, 'odin32_testcase_drive_network',      PathQuery('testcase_drive_network', sEnvId, fCfg, 1);
    call EnvVar_Set      fRM, 'odin32_testcase_drive_ramdisk',      PathQuery('testcase_drive_ramdisk', sEnvId, fCfg, 1);
return 0;

/*
 * PERL 5005_53
 */
Perl: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * Perl main directory.
     */
/* BAD
    sPathPerl       = PathQuery('perl', sEnvId, fCfg);
    if (sPathPerl = '') then
        return 1;
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
    sPathPerl       = PathQuery('perl', sEnvId, fCfg);
    if (sPathPerl = '') then
        return 1;
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathPerl'\bin\perl.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathPerl'\dll\perlE0AC.dll', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('perl --version', 0, 'This is perl, version 5.005_53 built for os2');
        return rc;
    end
return 0;


/*
 * Python/2 v1.5.2
 */
Python: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * The Python Home directory
     */
    sPythonHome = PathQuery('python', sEnvId, fCfg);
    if (sPythonHome = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_PYTHON',    sPythonHome
    call EnvVar_Set      fRM, 'pythonhome',     sPythonHome
    call EnvVar_Set      fRM, 'pythonpath',     '.;'sPythonHome'\Lib;'sPythonHome'\Lib\plat-win;'sPythonHome'\Lib\lib-tk;'sPythonHome'\Lib\lib-dynload;'sPythonHome'\Lib\site-packages;'sPythonHome'\Lib\dos-8x3'
    call EnvVar_AddFront fRM, 'beginlibpath',   sPythonHome
    call EnvVar_AddFront fRM, 'path',           sPythonHome

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPythonHome'\Python.exe', 'error: verify didn''t find'),
            |   \FileExists(sPythonHome'\Python15.dll', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('echo print "hello world" | python', 0, 'hello world');
        return rc;
    end
return 0;


/*
 * OS/2 Programmers Toolkit v4.0 (CSD1/4)
 */
Toolkit40: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * Toolkit (4.0) main directory.
     */
    sPathTK    = PathQuery('toolkit40', sEnvId, fCfg);
    if (sPathTK = '') then
        return 1;
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathTK'\bin\alp.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\rc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\ipfc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\implib.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\mkmsgf.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\mapsym.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\os2386.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\pmbidi.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\tcpip32.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\os2.h', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\os2win.h', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\stack16\pmwsock.h', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\som\bin\sc.exe', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('syslevel '||sPathTK||'\bin', 0, 'IBM Developer''s Toolkit for OS/2 Warp Version 4'||'0d0a'x||'Version 4.00.');
        if (rc = 0) then
            rc = CheckCmdOutput('sc -V', -1, '", Version: 2.54.');
        if (rc = 0) then
            rc = CheckCmdOutput('rc', 0, 'IBM RC (Resource Compiler) Version 5.00.00');
        if (rc = 0) then
            rc = CheckCmdOutput('ipfc', 0, 'Version 4.00.00');

        return rc;
    end
return 0;



/*
 * OS/2 Programmers Toolkit v4.5
 */
Toolkit45: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * Toolkit (4.5) main directory.
     */
     sPathTK    = PathQuery('toolkit45', sEnvId, fCfg);
    if (sPathTK = '') then
        return 1;
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathTK'\bin\alp.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\rc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\ipfc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\implib.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\mkmsgf.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\mapsym.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\os2386.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\pmbidi.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\tcpip32.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\os2.h', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\os2win.h', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\stack16\pmwsock.h', 'error: verify didn''t find'),
            |    FileExists(sPathTK'\som\bin\sc.exe'),
            ) then
            return 2;
        rc = CheckCmdOutput('syslevel '||sPathTK||'\bin', 0, 'IBM OS/2 Developer''s Toolkit Version 4.5'||'0d0a'x||'Version 4.50     Component ID 5639F9300');
        if (rc = 0) then
            rc = CheckCmdOutput('rc', 0, 'IBM RC (Resource Compiler) Version 5.00.004');
        if (rc = 0) then
            rc = CheckCmdOutput('ipfc', 0, 'Version 4.00.006 July 21 1998');
        return rc;
    end
return 0;


/*
 * OS/2 Programmers Toolkit v4.5.1
 */
Toolkit451: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * Toolkit (4.5.1) main directory.
     */
     sPathTK    = PathQuery('toolkit451', sEnvId, fCfg);
    if (sPathTK = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_TOOLKIT',  sPathTK;

    call EnvVar_AddFront fRM, 'path',        sPathTK'\bin;'sPathTK'\som\common;'sPathTK'\som\bin'
    call EnvVar_AddFront fRM, 'dpath',       sPathTK'\msg;'sPathTK'\book;'sPathTK'\SOM\COMMON\SYSTEM;'sPathTK'\SOM\MSG;'
    call EnvVar_AddFront fRM, 'beginlibpath', sPathTK'\dll;'sPathTK'\som\common\dll;'sPathTK'\som\lib;'
    call EnvVar_AddFront fRM, 'help',        sPathTK'\help;'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathTK'\book;'sPathTK'\archived;'
    call EnvVar_AddFront fRM, 'somir',       sPathTK'\SOM\COMMON\ETC\214\SOM.IR;'
    call EnvVar_AddEnd   fRM, 'somir',       sPathTK'\SAMPLES\REXX\SOM\ANIMAL\ORXSMP.IR;'
    call EnvVar_AddFront fRM, 'nlspath',     sPathTK'\msg\%N;'
    call EnvVar_AddEnd   fRM, 'ulspath',     sPathTK'\language;'
    call EnvVar_AddFront fRM, 'include',     sPathTK'\H\ARPA;'sPathTK'\H\NET;'sPathTK'\H\NETINET;'sPathTK'\H\NETNB;'sPathTK'\H\RPC;'sPathTK'\SPEECH\H;'sPathTK'\H\GL;'sPathTK'\H;'sPathTK'\SOM\INCLUDE;'sPathTK'\INC;'
    call EnvVar_AddFront fRM, 'lib',         sPathTK'\lib;'
    call EnvVar_AddEnd   fRM, 'lib',         sPathTK'\SAMPLES\MM\LIB;'sPathTK'\SPEECH\LIB;'
    call EnvVar_AddFront fRM, 'helpndx',     'EPMKWHLP.NDX+', '+'
    call EnvVar_AddFront fRM, 'ipfc',        sPathTK'\ipfc;'
    call EnvVar_Set      fRM, 'sombase',     sPathTK'\SOM'
    call EnvVar_Set      fRM, 'somruntime',  sPathTK'\SOM\COMMON'
    call EnvVar_Set      fRM, 'LANG',        'en_us'

    call EnvVar_Set      fRM, 'CPREF',       'CP1.INF+CP2.INF+CP3.INF'
    call EnvVar_Set      fRM, 'GPIREF',      'GPI1.INF+GPI2.INF+GPI3.INF+GPI4.INF'
    call EnvVar_Set      fRM, 'MMREF',       'MMREF1.INF+MMREF2.INF+MMREF3.INF'
    call EnvVar_Set      fRM, 'PMREF',       'PM1.INF+PM2.INF+PM3.INF+PM4.INF+PM5.INF'
    call EnvVar_Set      fRM, 'WPSREF',      'WPS1.INF+WPS2.INF+WPS3.INF'
    call EnvVar_AddFront fRM, 'sminclude',   sPathTK'\H;'sPathTK'\IDL;'sPathTK'\SOM\INCLUDE;.;'
    call EnvVar_Set      fRM, 'smaddstar',   '1'
    call EnvVar_Set      fRM, 'smemit',      'h;ih;c'
    call EnvVar_Set      fRM, 'smtmp',       EnvVar_Get('tmp');
    call EnvVar_Set      fRM, 'smclasses',   'WPTYPES.IDL'
    /*
    call EnvVar_Set      fRM, 'CAT_MACHINE', 'COM1:57600'
    call EnvVar_Set      fRM, 'CAT_HOST_BIN_PATH', TKMAIN'\BIN'
    call EnvVar_Set      fRM, 'CAT_COMMUNICATION_TYPE', 'ASYNC_SIGBRK'
    call EnvVar_Set      fRM, 'CAT_HOST_SOURCE_PATH',TKMAIN'\BIN;'
    */

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathTK'\bin\alp.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\rc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\ipfc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\implib.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\mkmsgf.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\mapsym.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\nmake.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\bin\nmake32.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\os2386.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\pmbidi.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\lib\tcpip32.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\os2.h', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\os2win.h', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\h\stack16\pmwsock.h', 'error: verify didn''t find'),
            |   \FileExists(sPathTK'\som\bin\sc.exe', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('syslevel '||sPathTK||'\bin', 0, 'IBM OS/2 Developer''s Toolkit Version 4.5'||'0d0a'x||'Version 4.50.1     Component ID 5639F9300');
        if (rc = 0) then
            rc = CheckCmdOutput('sc -V', -1, '", Version: 2.54.');
        if (rc = 0) then
            rc = CheckCmdOutput('rc', 0, 'IBM RC (Resource Compiler) Version 5.00.006 Oct 20 2000');
        if (rc = 0) then
            rc = CheckCmdOutput('ipfc', 0, 'Version 4.00.007 Oct 02 2000');
        if (rc = 0) then
            rc = CheckCmdOutput('nmake', 2, 'Version 4.00.000 Oct 20 2000');
        if (rc = 0) then
            rc = CheckCmdOutput('nmake32', 0, 'Version 5.00.003 Oct 20 2000');
        return rc;
    end
return 0;



/**
 * This will envolve into an full UNIX like environment some day perhaps...
 */
Unix: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * Unix root directory.
     */
    sUnixBack = PathQuery('unixroot', sEnvId, fCfg);
    if (sUnixBack = '') then
        return 1;
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
    sXF86Back = PathQuery('xfree86', sEnvId, fCfg);
    if (sXF86Back = '') then
        return 1;
    sXF86Forw = translate(sXF86Back, '/', '\');
    call EnvVar_Set      fRM, 'PATH_XFREE86',       sXF86Back
    call EnvVar_AddFront fRM, 'C_INCLUDE_PATH',     sXF86Forw'/include'
    call EnvVar_AddFront fRM, 'CPLUS_INCLUDE_PATH', sXF86Forw'/include'
    call EnvVar_Set      fRM, 'OBJC_INCLUDE_PATH',  sXF86Forw'/include'
    call EnvVar_AddFront fRM, 'LIBRARY_PATH',       sXF86Forw'/lib'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sUnixBack'\bin\bash.exe', 'error: verify didn''t find'),
            |   \FileExists(sUnixBack'\bin\sh.exe', 'error: verify didn''t find'),
            |   \FileExists(sUnixBack'\bin\yes.exe', 'error: verify didn''t find'),
            |   \FileExists(sUnixBack'\bin\rm.exe', 'error: verify didn''t find'),
            |   \FileExists(sUnixBack'\bin\cp.exe', 'error: verify didn''t find'),
            |   \FileExists(sUnixBack'\bin\mv.exe', 'error: verify didn''t find'),
            |   \FileExists(sXF86Back'\bin\xf86config.exe', 'error: verify didn''t find'),
            ) then
            return 2;
        return rc;
    end
return EMX(fRM);



/*
 * IBM Visual Age for C++ v3.08 for OS/2
 */
VAC308: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * IBM Visual Age for C++ Version 3.08 main directory.
     */
    sPathCPP = PathQuery('vac308', sEnvId, fCfg);
    if (sPathCPP = '') then
        return 1;
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

    call EnvVar_AddFront fRM, 'helpndx',        'EPMKWHLP.NDX+CPP.NDX+CPPBRS.NDX', '+'
    call EnvVar_AddFront fRM, 'ipf_keys',       'SHOWNAV'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathCPP'\bin\icc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\ilib.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\ilink.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\icsperf.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\icsdebug.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\cppfilt.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\dllrname.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\demangl.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cppom30.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cppom30i.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cppom30o.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cppon30i.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cppon30o.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\_doscall.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\_pmwin.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\include\builtin.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\include\conio.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\include\ismkss.h', 'error: verify didn''t find'),
            |    FileExists(sPathCPP'\include\os2.h'),
            |    FileExists(sPathCPP'\include\os2win.h'),
            |    FileExists(sPathCPP'\include\pm.h'),
            |   \FileExists(sPathCPP'\include\sys\utime.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\help\cpplib.inf', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('syslevel '||sPathCPP||'\syslevel', 0, 'Version 3.00     Component ID 562201703'||'0d0a'x||'Current CSD level: CTC308');
        if (rc = 0) then
            rc = CheckCmdOutput('syslevel '||sPathCPP||'\syslevel', 0, 'Version 3.00     Component ID 562201704'||'0d0a'x||'Current CSD level: CTU308');
        if (rc = 0) then
            rc = CheckCmdOutput('syslevel '||sPathCPP||'\syslevel', 0, 'Version 3.00     Component ID 562201707'||'0d0a'x||'Current CSD level: CTV308');
        if (rc = 0) then
            rc = CheckCmdOutput('syslevel '||sPathCPP||'\syslevel', 0, 'Version 3.00     Component ID 562201708'||'0d0a'x||'Current CSD level: CTD308');
        if (rc = 0) then
            rc = CheckCmdOutput('syslevel '||sPathCPP||'\syslevel', 0, 'Version 3.00     Component ID 562201605'||'0d0a'x||'Current CSD level: CTC308');
        if (rc = 0) then
            rc = CheckCmdOutput('syslevel '||sPathCPP||'\syslevel', 0, 'Version 3.00     Component ID 562201602'||'0d0a'x||'Current CSD level: CTO308');
        if (rc = 0) then
            rc = CheckCmdOutput('icc', 0, 'IBM VisualAge C++ for OS/2, Version 3');
        if (rc = 0) then
            rc = CheckCmdOutput('ilink', 16, 'IBM(R) Linker for OS/2(R), Version 01.08.r1a_CTC308c');
        if (rc = 0) then
            rc = CheckCmdOutput('ilib /?', 8, 'IBM(R) Library Manager for OS/2(R), Version 01.00.03 cc_CTC308c');
        return rc;
    end
return 0;



/*
 * Visual Age / C and C++ tools v3.6.5 for OS/2
 */
VAC365: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * IBM C/C++ Compiler and Tools Version 3.6.5 main directory.
     */
    sPathCxx    = PathQuery('vac365', sEnvId, fCfg);
    if (sPathCxx = '') then
        return 1;
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathCxx'\bin\icc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\bin\ilib.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\bin\ilink.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\bin\idebug.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\bin\cppfilt.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\bin\dllrname.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\bin\cppcbe36.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\lib\cpprms36.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\lib\cpprmi36.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\lib\cpprmo36.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\lib\cpprni36.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\lib\cpprds36.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\include\builtin.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\include\conio.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\include\ismavl.h', 'error: verify didn''t find'),
            |    FileExists(sPathCxx'\include\os2.h'),
            |    FileExists(sPathCxx'\include\os2win.h'),
            |    FileExists(sPathCxx'\include\pm.h'),
            |   \FileExists(sPathCxx'\include\sys\utime.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCxx'\help\cpplbm36.msg', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('icc', 0, 'IBM* C and C++ Compilers for OS/2*, AIX* and for Windows NT**, Version 3.6');
        if (rc = 0) then
            rc = CheckCmdOutput('ilink', 16, 'IBM(R) Linker for OS/2(R), Version 03.06.PPK1990125');
        if (rc = 0) then
            rc = CheckCmdOutput('ilib /?', 0, 'IBM Librarian for OS/2(R) Version 03.99.PPK1990310');
        if (stream(sPathCxx'\bin\cppcbe36.exe', 'c', 'query size') <> 603122) then
        do
            say 'Error!!! Get latest vac365 optimizer fixes from the OS2 Mozilla project.';
            say '         http://www.mozilla.org/ports/os2/setup.html';
            rc = 99;
        end
        return rc;
    end
return 0;


/*
 * Visual Age for C++ v4.0 for OS/2
 */
VAC40: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * IBM VisualAge for C++ v4.0 main directory.
     */
    sPathCPP    = PathQuery('vac40', sEnvId, fCfg);
    if (sPathCPP = '') then
        return 1;
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathCPP'\bin\iccv4.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\ilib.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\ire.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\vacide.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\cppfilt.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\dllrname.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\bin\patrace.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cpprms40.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cpprmi40.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cpprmo40.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cpprni40.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\lib\cpprds40.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\include\builtin.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\include\conio.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\include\ismavl.h', 'error: verify didn''t find'),
            |    FileExists(sPathCPP'\include\os2.h'),
            |    FileExists(sPathCPP'\include\os2win.h'),
            |    FileExists(sPathCPP'\include\pm.h'),
            |   \FileExists(sPathCPP'\include\sys\utime.h', 'error: verify didn''t find'),
            |   \FileExists(sPathCPP'\help\cpplbm40.msg', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('iccv4', 0, 'IBM VisualAge for C++ Version 4.0 C Compiler');
        if (rc = 0) then
            rc = CheckCmdOutput('ilib /?', 0, 'IBM Librarian for OS/2(R) Version 03.99.cc_981110'); /* is this really FP2?????? */
        if (rc = 0) then
            rc = CheckCmdOutput('vacbld -?', 0, 'IBM(R) VisualAge(R) C++ Professional, Version 4.0 (981117)'); /* is this really FP2?????? */
        return rc;
    end
return 0;




/*
 * WarpIn
 */
WarpIn: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg

    /*
     * WarpIn main directory.
     */
    sPathWarpIn = PathQuery('warpin', sEnvId, fCfg);
    if (sPathWarpIn = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_WARPIN', sPathWarpIn;
    call EnvVar_AddFront fRM, 'path',        sPathWarpIn';'
    call EnvVar_AddFront fRM, 'beginlibpath',sPathWarpIn';'
    call EnvVar_AddFront fRM, 'bookshelf',   sPathWarpIn';'

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathWarpIn'\wic.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWarpIn'\wpirtl.dll', 'error: verify didn''t find'),
            |   \FileExists(sPathWarpIn'\warpin.exe', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('wic', 1, '2002 - WarpIn archive creation and maintenance');
        return rc;
    end
return 0;



/*
 * WatCom C/C++ v11.0
 */
WatComC11: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg, iBits

    /*
     * Watcom C/C++ v11.0 main directory
     */
    sPathWatcom = PathQuery('watcom11', sEnvId, fCfg);
    if (sPathWatcom = '') then
        return 1;
    call EnvVar_Set      fRM, 'PATH_WATCOM', sPathWatcom
    call EnvVar_Set      fRM, 'CCENV',      'WAT'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'WAT11'
    if (iBits == 16) then
        call EnvVar_Set      fRM, 'BUILD_ENV',  'WAT11-16'
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

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathWatcom'\binp\wpp.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\binp\wcc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\binp\wcc386.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\binp\wpp386.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\binp\wlink.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib286\os2\os2.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\os2\clbrdll.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\os2\clib3r.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\nt\kernel32.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\nt\clbrdll.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\nt\clib3r.lib', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('wcc', 8, 'Watcom C16 Optimizing Compiler  Version 11.0 '||'0d0a'x);
        if (rc = 0) then
            rc = CheckCmdOutput('wpp', 8, 'Watcom C++16 Optimizing Compiler  Version 11.0 '||'0d0a'x);
        if (rc = 0) then
            rc = CheckCmdOutput('wcc386', 8, 'Watcom C32 Optimizing Compiler  Version 11.0 '||'0d0a'x);
        if (rc = 0) then
            rc = CheckCmdOutput('wpp386', 8, 'Watcom C++32 Optimizing Compiler  Version 11.0 '||'0d0a'x);
        if (rc = 0) then
            rc = CheckCmdOutput('wlink form ELF', 1, 'WATCOM Linker Version 11.0'||'0d0a'x);
        return rc;
    end
return 0;

/*
 * WatCom C/C++ v11.0c
 */
WatComC11c: procedure expose aCfg. aPath. sPathFile
    parse arg sEnvId,fRM,fCfg, iBits

    /*
     * Watcom C/C++ v11.0c main directory
     */
    sPathWatcom = PathQuery('watcom11c', sEnvId, fCfg);
    if (sPathWatcom = '') then
        return 1;

    call EnvVar_Set      fRM, 'PATH_WATCOM', sPathWatcom
    call EnvVar_Set      fRM, 'CCENV',      'WAT'
    call EnvVar_Set      fRM, 'BUILD_ENV',  'WAT11C'
    if (iBits = 16) then
        call EnvVar_Set      fRM, 'BUILD_ENV',  'WAT11C-16'
    call EnvVar_Set      fRM, 'BUILD_PLATFORM', 'OS2'

    call EnvVar_Set      fRM, 'watcom',      sPathWatcom
    call EnvVar_AddFront fRM, 'path',        sPathWatcom'\binp;'sPathWatcom'\binw;'
    call EnvVar_AddFront fRM, 'beginlibpath',sPathWatcom'\binp\dll;'
    call EnvVar_AddFront fRM, 'help',        sPathWatcom'\binp\help;'
    call EnvVar_AddEnd   fRM, 'bookshelf',   sPathWatcom'\binp\help;'
    if (iBits = 16) then
        call EnvVar_AddFront fRM, 'include',     sPathWatcom'\h;'sPathWatcom'\h\os21x;'
    else
        call EnvVar_AddFront fRM, 'include',     sPathWatcom'\h;'sPathWatcom'\h\os2;'sPathWatcom'\h\nt;'
    call EnvVar_AddFront fRM, 'lib',         sPathWatcom'\lib386;'sPathWatcom'\lib386\os2;'sPathWatcom'\lib286;'sPathWatcom'\lib286\os2;'
    call EnvVar_Set      fRM, 'edpath',      sPathWatcom'EDDAT;'
    /*
    rem detach %watcom%\BINP\BATSERV.EXE
    rem detach %watcom%\BINP\NMPBIND.EXE
    */

    /* verify */
    if (fCfg > 0) then
    do
        if (    \FileExists(sPathWatcom'\binp\wpp.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\binp\wcc.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\binp\wcc386.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\binp\wpp386.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\binp\wlink.exe', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib286\os2\os2.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\os2\clbrdll.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\os2\clib3r.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\nt\kernel32.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\nt\clbrdll.lib', 'error: verify didn''t find'),
            |   \FileExists(sPathWatcom'\lib386\nt\clib3r.lib', 'error: verify didn''t find'),
            ) then
            return 2;
        rc = CheckCmdOutput('wcc', 8, 'Watcom C16 Optimizing Compiler  Version 11.0c');
        if (rc = 0) then
            rc = CheckCmdOutput('wpp', 8, 'Watcom C++16 Optimizing Compiler  Version 11.0c');
        if (rc = 0) then
            rc = CheckCmdOutput('wcc386', 8, 'Watcom C32 Optimizing Compiler  Version 11.0c');
        if (rc = 0) then
            rc = CheckCmdOutput('wpp386', 8, 'Watcom C++32 Optimizing Compiler  Version 11.0c');
        if (rc = 0) then
            rc = CheckCmdOutput('wlink form ELF', 1, 'WATCOM Linker Version 11.0c');
        return rc;
    end
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
        Address CMD '@set dummyenvvar'||i'=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    end
    do i = 1 to 100
        Address CMD '@set dummyenvvar'||i'=';
    end
return 0;

