/* $Id: ConfigureGCC.cmd,v 1.2 2003-04-08 12:46:04 sandervl Exp $
 *
 * Main configure file for Odin32. (For the New Build System.)
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 * Corrected by Platon Fomichev for MSVCRT project
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
 * Welcome 
 */

say 'ConfigureMSVCRT.cmd v'sVersion' for 'sProject'.'
say '==============================================='
say ''

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
        say 'ConfigureMSVCRT.cmd v'sVersion' for 'sProject'.'
        say '';
        say 'No options currently'
        say '';
        say 'Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)'
        say '';
        say 'Published under the GNU Public Lisence (GPL) version 2.'

        exit(16);
    end
end

/*
 * First check for EMX runtime exisistance 
 */

say 'Checking for EMX runtime availability...'

rc = CheckCmdOutput('tools\bin\which', 0, 1, 'which (v2.1.3 June-03-1997 updated by Wonkoo Kim) for OS/2, from Newtware');
if (rc = 99) then
   do 
      say ''
      say 'There was a problem launching EMX based application.'
      say 'Possibly you do not have EMX runtime installed on your system'
      say 'or EMX DLL are not in LIBPATH. Correct this and relaunch.'
      say ''
      exit(99)
   end

say 'EMX runtime found.'
say ''
say 'Checking EMX revision...'

rc = CheckCmdOutput('emxrev.cmd', 0, 1,,
                    'EMX : revision = 61'||'0d0a'x ||,
                    'EMXIO : revision = 60'||'0d0a'x||,
                    'EMXLIBC : revision = 63'||'0d0a'x||,
                    'EMXLIBCM : revision = 64'||'0d0a'x||,
                    'EMXLIBCS : revision = 64'||'0d0a'x||,
                    'EMXWRAP : revision = 60'||'0d0a'x);

if (rc = 99) then
   do 
      say ''
      say 'Revision mistmatch. You need to have latest EMX fixkit installed.'
      say 'Latest fixkit is 0.9d4. Type ''emxrev'' to check your current level.'
      say 'Install latest update and relaunch.'
      say ''
      exit(99)
   end


say 'Found runtime revision 0.9d4.'
say ''
say 'Checking developer revision...'

queTmp = RxQueue('Create');
queOld = RxQueue('Set', queTmp);
Address CMD 'tools\bin\which gcc' || ' 2>&1 | RxQueue' queTmp;
rcCmd = rc;

/* get output */
sOutput = '';
do while (queued() > 0)
      parse pull sLine
      sOutput = sOutput || sLine || '0d0a'x
end
call RxQueue 'Delete', RxQueue('Set', queOld);

j =lastpos('emx', sOutput);
if (j > 0) then
  do
     sOutput = substr(sOutput, 1, j+3);
     /* for later use */
     emxPath = sOutput;

    if (stream(emxPath'\lib\st\c.a', 'c', 'query size') <> 313492) then
    do
        say 'You do not have developer fixes 0.9d4 installed. Note that';
        say 'you do need not only runtime fixes for compiler suite. Install developer';
        say 'fixes, re-run ''make'' in emx\lib directory and try again';
        exit(99);
    end
  end
else
  do
      say ''
      say 'Error finding ''emx'' dir in GCC compiler suite structure. This directory'
      say 'is hardcoded and without it compiler can''t function normally.'
      say 'Reinstall compiler suite correctly.'
      say ''
      exit(99)
end

say 'Developer revision is 0.9d4.'
say ''

say 'Checking GCC compiler'

rc = CheckCmdOutput('tools\bin\which gcc', 0, 1,,
                    'gcc is NOT FOUND');
if (rc != 0) then rc = CheckCmdOutput('tools\bin\which g++', 0, 1,,
                       'g++ is NOT FOUND');
if (rc != 0) then rc = CheckCmdOutput('tools\bin\which as', 0, 1,,
                       'as is NOT FOUND');

if (rc = 0) then
   do 
      say ''
      say 'Can''t locate GCC compiler executable files. Check you have them'
      say 'in your PATH variable. Correct this issue and relaunch.'
      say ''
      exit(99)
   end

say 'Checking compiler version.'
rc = CheckCmdOutput('gcc --version', 0, 1,'3.2.1');
if (rc = 0) then rc = CheckCmdOutput('g++ --version', 0, 1,'3.2.1');
if (rc = 0) then rc = CheckCmdOutput('as --version', 0, 1,'GNU assembler 2.11.2');
if (rc = 99 ) then
   do 
      say ''
      say 'Version mistmatch, while checking Gnu Compiler Suite!'
      say 'You should have GCC 3.2.1 installed. Also you should set it as an'
      say 'active compiler before launching configure, if you have more than'
      say 'one GCC compiler available.'
      say ''
      exit(99)
   end

say 'GCC compiler 3.2.1 found and it is active.'
say 'Building test samples:'
call charout ,'C compiler - a.out mode ->'
rc = CheckGCC('');
if (rc = 0) then call charout ,'OK'
 else
  do
      say ''
      say 'Error checking C compiler in a.out mode. This is weird and unexpected.'
      say 'Please check files in current dir for errors. Configure can not continue.'
      say ''
      exit(99)

  end
say ''
call charout ,'C compiler - OMF mode ->'
rc = CheckGCC('-Zomf');
if (rc = 0) then  call charout ,'OK'
 else
  do
      say ''
      say 'Error checking C compiler in OMF mode. This is weird and unexpected.'
      say 'Please check files in current dir for errors. Configure can not continue.'
      say ''
      exit(99)

  end
say ''
call charout ,'C++ compiler - a.out mode ->'
rc = CheckGPP('');
if (rc = 0) then call charout ,'OK'
 else
  do
      say ''
      say 'Error checking C++ compiler in a.out mode. This is weird and unexpected.'
      say 'Please check files in current dir for errors. Configure can not continue.'
      say ''
      exit(99)

  end
say ''

call charout ,'Weak symbol repository file:'
say ''

sOutput = emxPath
sOutput = sOutput || 'lib\gcc-lib\i386-pc-os2-emx\3.2.1\weaksyms.omf';
if (\CfgVerifyFile(sOutput,0)) then
  do
      say ''
      say 'Error finding weak symbols file in GCC compiler suite structure.'
      say 'This file is critical for OMF files. Please check whether you did'
      say ''make' command in ''...emx\lib\gcc-lib\i386-pc-os2-emx\3.2.1\'' directory'
      say 'Configure aborted.'
      exit(99)
  end
else
     do
        say sOutput;
        'cp' sOutput .
     end 


call charout ,'C++ compiler - OMF mode and weak symbol support ->'
rc = CheckGPP('-Zomf');
if (rc = 0) then call charout ,'OK'
 else
  do
      say ''
      say 'Error checking C++ compiler in a.out mode. This is weird and unexpected.'
      say 'Please check files in current dir for errors. Configure can not continue.'
      say ''
      'rm weaksyms.omf'
      exit(99)
  end
say ''
'rm weaksyms.omf'
say 'All test successfully passed. Headers path will be 'emxPath'include\'

if (\CfgVerifyFile('makefile.inc',0)) then
do
      say ''
      say 'Makefile.inc does not exist in the current directory. Run Odin32 configure'
      say 'first and relaunch after that.'
      say ''
      exit(99)
end

call stream 'makefile.inc','c','open write';
call lineout 'makefile.inc', 'EMX_INCLUDE	= 'emxPath'include\'
call lineout ''
call stream 'makefile.inc', 'c', 'close';

say 'Done.'
say ''

exit(0)


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
            if (\fQuiet) then
            do   
              say 'Debug - start'
              say 'Debug:' sOutputPartExpected
              say 'Debug: not found in:'
              say sOutput
              say 'Debug - end'
            end 
           rc = 99
        end
    end
    else
        rc = 49

    if (\fQuiet & rc <> 0) then
        say 'Debug:' sCmd 'rc='rc' rcCmd='rcCmd 'rcCmdExpected='rcCmdExpected;
return rc;


/*
 * Checks a makefile configuration.
 */
CheckGCC: procedure
    parse arg sFlags
    fFailed = 0;

    sFnE = 'CheckMake.exe';
    sFnM = 'CheckMake.mak';
    sFnC = 'CheckMake.c';
    sFnD = 'CheckMake.def';

    /*
     * Generate files.
     */
    call SysFileDelete(sFnC);
    call lineout sFnC, '#include <stdio.h>'
    call lineout sFnC, 'int main(void) { printf("Hello world\n"); return 0;}'
    call stream sFnC, 'c', 'close';

    /*
     * Build it.
     */
    call SysFileDelete(sFnE);
    Address CMD 'gcc CheckMake.c' sFlags '1>nul';
    if (rc = 0) then
    do
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

            /*
             * Check output and result.
             */
            if ((rcCmd <> 0) | (sOutput <> 'Hello world'||'0d0a'x)) then
            do
                say 'Error'
                say 'Compiler launch failed or test sample was not launched!';
                return 99;
            end
            else
                fFailed = 0;
        end
    end
    else
        do
        say 'There was a failure while compiling test sample';
        return 99;
        end
    /*
     * Cleanup.
     */
    if (\fFailed) then
    do
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
return 0;

/*
 * Checks a makefile configuration.
 */
CheckGPP: procedure
    parse arg sFlags
    fFailed = 0;

    sFnE = 'CheckMake.exe';
    sFnM = 'CheckMake.mak';
    sFnC = 'CheckMake.cpp';
    sFnD = 'CheckMake.def';

    /*
     * Generate files.
     */
    call SysFileDelete(sFnC);
    call lineout sFnC, '#include <vector>'
    call lineout sFnC, '#include <string>'
    call lineout sFnC, '#include <iostream>'
    call lineout sFnC, 'using namespace std;'
    call lineout sFnC, 'int main() { vector<int> a; a.push_back(2); string b("Hello World"); cout<<b<<*a.begin()<<endl; return 0;}';
    call stream sFnC, 'c', 'close';

    /*
     * Build it.
     */
    call SysFileDelete(sFnE);
    Address CMD 'g++ CheckMake.cpp' sFlags '1>nul';
    if (rc = 0) then
    do
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

            /*
             * Check output and result.
             */
            if ((rcCmd <> 0) | (sOutput <> 'Hello World2'||'0d0a'x)) then
            do
                say 'Error'
                say 'Compiler launch failed or test sample was not launched!';
                return 99;
            end
            else
                fFailed = 0;
        end
    end
    else
        do
          say 'There was a failure while compiling test sample';
          return 99;
        end

    /*
     * Cleanup.
     */
    if (\fFailed) then
    do
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
return 0;

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
