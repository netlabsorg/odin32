/* $Id: NewConfigure.cmd,v 1.2 2002-05-16 13:36:46 bird Exp $
 *
 * Main configure file for Odin32. (For the New Build System.)
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
 * USA.
 *
 */

Address CMD '@echo off';

sProject = 'Odin32';
sVersion = '1.0'

/*
 * Arg variables.
 */
fNoInitialEnv = 0;
fNoModeSwitch = 0;
fNoVerify = 0
fNoCheck = 0
fWithoutDDKBase = 0;
fWithoutEMX = 0;
fWithoutICAT = 0;
fWithoutWatcom = 0;
fWithEMXPGCC = 0;
fWithIDA = 0;
sCompiler = 'VAC308';


/*
 * Parse Args.
 */
parse arg sArg.1 sArg.2 sArg.3 sArg.4 sArg.5 sArg.6 sArg.7 sArg.8 sArg.9 sArg.10 sDummy
if (sDummy <> '') then
do
    say 'too many arguments.'
    exit(12);
end
i = 1;
do while (sArg.i <> '')
    i = i + 1;
end
sArg.0 = i - 1;

/*
 * Process Args.
 */
do i = 1 to sArg.0
    ch = translate(substr(sArg.i,1,1));
    ch2 = translate(substr(sArg.i,2,1));
    /* check for help request */
    if (    ( (ch = '?') | (ch = 'H') ),
        |   ( ((ch = '-') | (ch = '/')) &  ( (ch2 = '?') | (ch2 = 'H'))),
        |   (translate(substr(sArg.0, 1, 3)) = '--H'),
        ) then
    do
        say 'Configure.cmd v'sVersion' for 'sProject'.'
        say '';
        say 'Syntax: Configure.cmd [options]';
        say '';
        say 'options: (both ''-'' and ''/'' is accepted)';
        say '   ?|h|-?|-h|--h           This help info.';
        say '   -c | --no-check         Don''t check the compilers.';
        say '   -e | --env-only         Only setup work environment.';
        say '   -i | --no-initial-env   Don''t setup an initial environment.';
        say '                           The default is modify the current environment to';
        say '                           be suitable for 'sProject' development.';
        say '   -m | --no-mode-switch   Don''t switch to mode 120,50.';
        say '   -v | --no-verify        Don''t verify & config the environments.';
        say '   -w | --use-watcom       Use the Watcom v11.0c compiler as default.';
        say '                           The default is VisualAge for C++ v3.08.';
        say '   --use-vac365            Use the VisualAge for C++ v3.6.5 as default.';
        say '                           The default is VisualAge for C++ v3.08.';
        say '   --without-ddkbase       Do not include DDKBase in the environment.';
        say '   --without-emx           Do not include DDKBase in the environment.';
        say '   --without-icat          Do not include ICAT in the environment.';
        say '   --with-emxpgcc          With support for emx + emxpgcc.';
        say '   --with-ida              Do include IDA Pro in the environment.';
        say '';
        say 'Please note that the build environment is really picky about versions and '
        say 'tools. For example it doesn''t accept any toolkit, meaning, toolkit 4.0'
        say 'with some CSD is the only game in town at the moment.';
        say 'Also for watcom it checks explicitly for 11.0c and will fail if you give';
        say 'it anything else.';
        say '';
        say 'NMAKE v5+ will not work - as always.';
        say '';
        say 'A bug, emx is currently required allthough there is options for omitting it.';
        say 'This will be corrected later, no problem.'
        say 'Yeah, and path settings are stored in tools\bin\BuildEnv.cmd.paths.';
        say '';
        say 'Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)'
        say '';
        say 'Published under the GNU Public Lisence (GPL) version 2.'

        exit(16);
    end

    if ((ch = '-') | (ch = '/')) then
    do
        select
            when ((ch2 = 'E') | (sArg.i = '--env-only')) then
            do
                fNoCheck = 1;
                fNoVerify = 1;
                fNoInitialEnv = 0;
            end
            when ((ch2 = 'C') | (sArg.i = '--no-check')) then
                fNoCheck = 1;
            when ((ch2 = 'I') | (sArg.i = '--no-initial-env')) then
                fNoInitialEnv = 1;
            when ((ch2 = 'M') | (sArg.i = '--no-mode-switch')) then
                fNoModeSwitch = 1;
            when ((ch2 = 'V') | (sArg.i = '--no-verify')) then
                fNoVerify = 1;
            when ((ch2 = 'W') | (sArg.i = '--use-watcom')) then
                sCompiler = 'WAT11C';
            when (sArg.i = '--without-ddkbase') then
                fWithoutDDKBase = 1;
            when (sArg.i = '--without-emx') then
                fWithoutEMX = 1;
            when (sArg.i = '--without-icat') then
                fWithoutICAT = 1;
            when (sArg.i = '--without-watcom') then
                fWithoutWatcom = 1;
            when (sArg.i = '--with-emxpgcc') then
                fWithEMXPGCC = 1;
            when (sArg.i = '--with-ida') then
                fWithIDA = 1;
            when (sArg.i = '--use-vac365') then
                sCompiler = 'VAC365';

            otherwise
            do
                say 'Invalid argument '''sArg.i'''!';
                exit(16);
            end
        end
    end
    else
    do
        say 'Invalid argument '''sArg.i'''!';
        exit(16);
    end
end

/* sanity checks */
if (fNoInitialEnv & fNoVerify & fNoCheck) then
do
    say 'Warning: You''ve asked us to do nothing. How very clever of you!';
    exit(4);
end
if (fWithoutWatcom & (sCompiler = 'WAT11C')) then
do
    say 'Warning: You''ve asked for watcom compiler (-w/--use-watcom) without watcom (--without-watcom). How very clever of you!!';
    exit(4);
end
if (fWithoutEMX & fWithEMXPGCC) then
do
    say 'Warning: You''ve asked for emxpgcc compiler without emx. that doesn''t work.';
    exit(4);
end


/*
 * Calculate default compiler environment and remove environment arguments.
 */
sDefaultEnvs = 'buildsetup cvs'
if (fWithIDA) then
    sDefaultEnvs = sDefaultEnvs || ' ida';
if (\fWithoutICAT) then
    sDefaultEnvs = sDefaultEnvs || ' icatgam';
if (\fWithoutDDKBase) then
    sDefaultEnvs = sDefaultEnvs || ' ddkbase';
if (\fWithoutEMX) then
    sDefaultEnvs = sDefaultEnvs || ' emx';
if (sCompiler = 'WAT11C') then
    sDefaultEnvs = sDefaultEnvs || ' vac308 watcomc11c';
else
    sDefaultEnvs = sDefaultEnvs || ' ' || sCompiler;
sDefaultEnvs = sDefaultEnvs || ' toolkit40';

sRemoveEnvs = 'buildsetup- cvs- toolkit40- vac308-'
if (sCompiler = 'VAC365') then
    sRemoveEnvs = sRemoveEnvs || ' vac365-';
if (\fWithoutDDKBase) then
    sRemoveEnvs = sRemoveEnvs || ' ddkbase-';
if (\fWithoutEMX) then
    sRemoveEnvs = sRemoveEnvs || ' emx-';
if (\fWithoutICAT) then
    sRemoveEnvs = sRemoveEnvs || ' icatgam-';
if (\fWithoutWatcom) then
    sRemoveEnvs = sRemoveEnvs || ' watcomc11c- watcomc11c-16-';
if (fWithEMXPGCC) then
    sRemoveEnvs = sRemoveEnvs || ' emxpgcc-';
if (fWithIDA) then
    sRemoveEnvs = sRemoveEnvs || ' ida-';

/*say 'debug remove:' sRemoveEnvs
say 'debug setup: ' sDefaultEnvs*/



/*
 * Go to source directory.
 */
parse source . . sSource .
sDir = directory();
call directory strip(strip(filespec('Drive', sSource) || filespec('Path', sSource), 'T', '\'), 'T', '/');


/*
 * Verify & Configure the required environments.
 */
say 'Configuring 'sProject'...'
if (\fNoVerify) then
do
    Address CMD '@setlocal'
    Address CMD 'call tools\bin\BuildEnv.cmd buildsetup* toolkit40* cvs* vac308*'
    if (sCompiler = 'VAC365') then
        Address CMD 'call tools\bin\BuildEnv.cmd vac365*';
    if (\fWithoutDDKBase) then
        Address CMD 'call tools\bin\BuildEnv.cmd ddkbase*';
    if (\fWithoutEMX) then
        Address CMD 'call tools\bin\BuildEnv.cmd emx*';
    if (fWithEMXPGCC) then
        Address CMD 'call tools\bin\BuildEnv.cmd emxpgcc*';
    if (\fWithoutICAT) then
        Address CMD 'call tools\bin\BuildEnv.cmd icatgam*'
    if (\fWithoutWatcom) then
        Address CMD 'call tools\bin\BuildEnv.cmd watcomc11c* watcomc11c-16*'
    if (fWithIDA) then
        Address CMD 'call tools\bin\BuildEnv.cmd ida*'
    Address CMD '@endlocal'
end


/*
 * Check the different compiler configs.
 */
if (\fNoCheck) then
do
    Address CMD '@setlocal'
    Address CMD '@set BUILD_QUIET=1';
    Address CMD '@set BUILD_VERBOSE=';
    say 'Checking make configs...'

    Address CMD 'call tools\bin\BuildEnv.cmd 'sRemoveEnvs sDefaultEnvs 'debug+'
    call CheckMake 'VAC308', 'EXE';
    Address CMD 'call tools\bin\BuildEnv.cmd profile+'
    call CheckMake 'VAC308', 'EXE';
    Address CMD 'call tools\bin\BuildEnv.cmd release+'
    call CheckMake 'VAC308', 'EXE';

    if (sCompiler = 'VAC365') then
    do
        Address CMD 'call tools\bin\BuildEnv.cmd 'sRemoveEnvs sDefaultEnvs 'debug+'
        call CheckMake 'VAC308', 'EXE';
        Address CMD 'call tools\bin\BuildEnv.cmd profile+'
        call CheckMake 'VAC308', 'EXE';
        Address CMD 'call tools\bin\BuildEnv.cmd release+'
        call CheckMake 'VAC308', 'EXE';
    end

    if (\fWithoutWatcom) then
    do
        Address CMD 'call tools\bin\BuildEnv.cmd 'sRemoveEnvs sDefaultEnvs 'debug+'
        call CheckMake 'WAT11C', 'EXE';
        Address CMD 'call tools\bin\BuildEnv.cmd profile+'
        call CheckMake 'WAT11C', 'EXE';
        Address CMD 'call tools\bin\BuildEnv.cmd release+'
        call CheckMake 'WAT11C', 'EXE';
    end
/*
    Address CMD 'call tools\bin\BuildEnv.cmd 'sRemoveEnvs sDefaultEnvs 'debug+'
    call CheckMake 'WAT11C-16', 'EXE';
    Address CMD 'call tools\bin\BuildEnv.cmd profile+'
    call CheckMake 'WAT11C-16', 'EXE';
    Address CMD 'call tools\bin\BuildEnv.cmd release+'
    call CheckMake 'WAT11C-16', 'EXE';
*/
    if (fWithEMXPGCC) then
    do
        Address CMD 'call tools\bin\BuildEnv.cmd 'sRemoveEnvs sDefaultEnvs 'debug+'
        call CheckMake 'EMX', 'EXE';
        Address CMD 'call tools\bin\BuildEnv.cmd profile+'
        call CheckMake 'EMX', 'EXE';
        Address CMD 'call tools\bin\BuildEnv.cmd release+'
        call CheckMake 'EMX', 'EXE';
    end

    Address CMD '@endlocal'
end


/*
 * Setup the initial environment if requested.
 */
if (\fNoInitialEnv) then
do
    call charout ,'Setting up suitable development environment...'
    /*
     * Clean.
     */
    Address CMD 'call tools\bin\BuildEnv.cmd' sRemoveEnvs
    call charout ,'.';

    /*
     * Set.
     */
    Address CMD 'call tools\bin\BuildEnv.cmd' sDefaultEnvs

    if (\fNoModeSwitch) then
        Address CMD 'call tools\bin\BuildEnv.cmd mode12050'

    /*
     * 4OS2?
     */
    Address CMD 'set 4os2test_env=%@eval[2 + 2]';
    if (value('4os2test_env',, 'OS2ENVIRONMENT') = '4') then
        Address CMD 'set BUILD_SHELL=4OS2';
    /*    Address CMD 'alias nmake=`iff exist Makefile.os2 .and. %@INDEX[%@UPPER[ %$ ], -F ] == -1 then & nmake.exe -f Makefile.os2 %$ & else & nmake.exe %$ & endiff`'*/
end

say 'Configure completed successfully.';
call directory(sDir);
return 0;



/*
 * Checks a makefile configuration.
 */
CheckMake: procedure
    parse arg sEnv, sMode
    sBuildMode = value('BUILD_MODE',,'OS2ENVIRONMENT')

    fFailed = 1;
    call charout ,'Testing 'sBuildMode' build with 'sEnv' in 'sMode' mode...';

    sFnE = '.\bin\'||sBuildMode||'\CheckMake.exe';
    sFnM = 'CheckMake.mak';
    sFnC = 'CheckMake.c';
    sFnD = 'CheckMake.def';

    /*
     * Generate files.
     */
    call SysFileDelete(sFnM);
    call lineout sFnM, 'BUILD_ENV_FORCE='sEnv
    call lineout sFnM, 'PATH_ROOT=.'
    call lineout sFnM, '!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)'
    call lineout sFnM, ' '
    call lineout sFnM, 'TARGET_NAME=CheckMake'
    call lineout sFnM, 'MAKEFILE='sFnM
    call lineout sFnM, 'TARGET_MODE='sMode
    call lineout sFnM, 'TARGET_OBJS=$(PATH_TARGET)\CheckMake.$(EXT_OBJ)'
    call lineout sFnM, 'TARGET_LIBS=$(LIB_OS) $(LIB_C_OBJ)'
    call lineout sFnM, ' '
    call lineout sFnM, '!include $(MAKE_INCLUDE_PROCESS)'
    call stream sFnM, 'c', 'close';

    call SysFileDelete(sFnC);
    if (sMode = 'EXE') then
    do
        call lineout sFnC, '#include <stdio.h>'
        call lineout sFnC, 'int main(void) { printf("hello world\n"); return 0;}'
        call stream sFnC, 'c', 'close';
    end

    call SysFileDelete(sFnD);
    if (sMode = 'EXE') then
    do
        call lineout sFnD, 'NAME CheckMake WINDOWCOMPAT'
        call stream sFnD, 'c', 'close';
    end
    call charout ,'.'


    /*
     * Build it.
     */
    call SysFileDelete(sFnE);
    Address CMD '@nmake -f 'sFnM 'clean dep build install 2>&1 > nul';
    if (rc = 0) then
    do
        call charout ,'.'
        if (sMode = 'EXE') then
        do
            /*
             * Test run the program
             */
            queTmp = RxQueue('Create');
            queOld = RxQueue('Set', queTmp);
            Address CMD '@'sFnE || ' 2>&1 | RxQueue' queTmp;
            rcCmd = rc;
            /* get output */
            sOutput = '';
            do while (queued() > 0)
                parse pull sLine
                sOutput = sOutput || sLine || '0d0a'x
            end
            call RxQueue 'Delete', RxQueue('Set', queOld);
            call charout ,'.'

            /*
             * Check output and result.
             */
            if ((rcCmd <> 0) | (sOutput != 'Hello world'||'0d0a'x)) then
                say 'Failed! Bad output or rc!';
            else
                fFailed = 0;
        end
    end
    else
        say 'Make Failed! rc='rc

    /*
     * Cleanup.
     */
    if (\fFailed) then
    do
        Address CMD 'nmake -f 'sFnM 'clean 2>&1 > nul ';
        call SysFileDelete(sFnC);
        call SysFileDelete(sFnM);
        call SysFileDelete(sFnD);
        call SysFileDelete(sFnE);
    end

    /*
     * Return / Exit.
     */
    if (fFailed) then
    do
        Address CMD '@endlocal'
        exit(16);
    end
    say 'OK!'
return 0;
