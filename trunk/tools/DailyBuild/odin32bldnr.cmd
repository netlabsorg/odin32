/* $Id: odin32bldnr.cmd,v 1.5 2002-06-26 22:07:15 bird Exp $
 *
 * Build number update script.
 *
 * Two operations:
 *      1. Increment the build number.
 *      2. Commit the build number file.
 *
 * Assumes that current directory is the root.
 *
 * Copyright (c) 2001-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
call SysLoadFuncs;

/* get build settings */
sDate = value('BUILD_DATE',, 'OS2ENVIRONMENT');
sType = value('BUILD_TYPE',, 'OS2ENVIRONMENT');
if ((sDate = '') | (sType = '')) then do say 'BUILD_DATE/BUILD_TYPE unset, you didn''t start job.cmd.'; exit(16); end


/*
 * Parse parameters.
 */
parse arg sOperation sIgnore

/*
 * Save and change directory.
 */
sTree = directory();
'cd include';
if (rc <> 0) then call failure rc, 'cd include failed.';


/*
 * Do operation.
 */
if (substr(sOperation, 1, 3) = 'inc') then
do
    /*
     * Scan the odinbuild.h file for ODIN32_BUILD_NR.
     */
    sOut = 'odinbuild.h';
    sIn = 'odinbuild.h.backup';
    call SysFileDelete('odinbuild.h.backup');
    'copy' sOut sIn;
    if (rc) then call failure rc, 'backup copy failed';
    call SysFileDelete('odinbuild.h');

    rcIn = stream(sIn, 'c', 'open read');
    rcOut = stream(sOut, 'c', 'open write');
    if (pos('READY', rcIn) <> 1 | pos('READY', rcOut) <> 1) then
    do
        call stream(sIn, 'c', 'close');
        call stream(sOut, 'c', 'close');
        call failure 5, 'failed to open in or/and out file. rcIn='rcIn 'rcOut='rcOut;
    end

    /*
     * Copy loop which updates ODIN32_BUILD_NR when found.
     */
    fFound = 0;
    do while (lines(sIn))
        sLine = linein(sIn);
        if (\fFound & substr(strip(sLine), 1, 24) = '#define ODIN32_BUILD_NR ') then
        do
            parse var sLine '#define ODIN32_BUILD_NR' iBuildNr sComment;
            iBuildNr = strip(iBuildNr);
            sComment = strip(sComment);
            iBuildNr = iBuildNr + 1;
            sLine = '#define ODIN32_BUILD_NR       '||iBuildNr||'        '||sComment;
            say 'newln:' sLine;
            fFound = 1;
        end
        call lineout sOut, sLine;
    end
    call lineout sOut, '';
    call stream sIn, 'c', 'close';
    call stream sOut, 'c', 'close';
    rc = 0;
end
else if (sOperation = 'svn') then
do

    /* get svn revision */
    'rxqueue /clear'
    'svn info | grep "Revision:" | cut -d" " -f2 | rxqueue'
    iBuildNr = LineIn("QUEUE:")

    /*
     * Scan the odinbuild.h file for ODIN32_BUILD_NR.
     */
    sOut = 'odinbuild.h';
    sIn = 'odinbuild.h.backup';
    call SysFileDelete('odinbuild.h.backup');
    'copy' sOut sIn;
    if (rc) then call failure rc, 'backup copy failed';
    call SysFileDelete('odinbuild.h');

    rcIn = stream(sIn, 'c', 'open read');
    rcOut = stream(sOut, 'c', 'open write');
    if (pos('READY', rcIn) <> 1 | pos('READY', rcOut) <> 1) then
    do
        call stream(sIn, 'c', 'close');
        call stream(sOut, 'c', 'close');
        call failure 5, 'failed to open in or/and out file. rcIn='rcIn 'rcOut='rcOut;
    end

    /*
     * Copy loop which updates ODIN32_BUILD_NR when found.
     */
    fFound = 0;
    do while (lines(sIn))
        sLine = linein(sIn);
        if (\fFound & substr(strip(sLine), 1, 24) = '#define ODIN32_BUILD_NR ') then
        do
            parse var sLine '#define ODIN32_BUILD_NR' iOldBuildNr sComment;
            sComment = strip(sComment);
            sLine = '#define ODIN32_BUILD_NR       '||iBuildNr||'        '||sComment;
            say 'newln:' sLine;
            fFound = 1;
        end
        call lineout sOut, sLine;
    end
    call lineout sOut, '';
    call stream sIn, 'c', 'close';
    call stream sOut, 'c', 'close';
    rc = 0;
end
else if (sOperation = 'commit') then
do
    /*
     * Commit the build nr. file.
     */
    if (sType = 'W') then
        'cvs commit -m "Weekly build - 'sDate'" odinbuild.h';
    else
        'cvs commit -m "Daily build - 'sDate'" odinbuild.h';
    if (rc <> 0) then call failed rc, 'failed to commit odinbuild.h'
end
else
do
    say 'invalid operation' sOperation'. Should be commit or inc.'
    rc = 87;
end


/*
 * Restore directory.
 */
call directory(sTree);

exit(rc);


/*
 * Fatal failures terminates here!.
 */
failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

