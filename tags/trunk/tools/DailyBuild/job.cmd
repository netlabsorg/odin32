/* $Id: job.cmd,v 1.9 2003-02-06 21:05:37 bird Exp $
 *
 * Main job for building OS/2.
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

    /* Load rexxutils functions */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysloadFuncs;

    /*
     * Get and set the build date.
     */
    parse arg sDate sType sDummy
    fOk = 1;
    if (sDate <> '') then
    do
        parse var sDate sYear'-'sMonth'-'sDay
        sType = substr(translate(sType), 1, 1);
        if (  (length(sYear) <> 4) | (strip(translate(sYear, '','0123456789')) <> ''),
            | (length(sMonth) <>2) | (strip(translate(sMonth, '','0123456789')) <> ''),
            | (length(sDay)  <> 2) | (strip(translate(sDay, '','0123456789')) <> ''),
            | ((sType <> 'W') & (sType <> 'D'))) then
            fOk = 0;
        else
            sDate = sYear||sMonth||sDay;
    end
    else
    do
        sDate = date('S');
        sType = 'D';
        if (Date('B')//7 = 3) then  /* weekly on Thursdays */
            sType = 'W';
    end

    if (\fOk) then
    do
        say 'Hey mister! you''ve given me a bad date or build type!!!';
        say 'Date='sYear'-'sMonth'-'sDay
        say 'Buildtype='sType;
        exit(16);
    end
    call value 'BUILD_DATE', sDate, 'OS2ENVIRONMENT';
    call value 'BUILD_TYPE', sType, 'OS2ENVIRONMENT';


    /*
     * Get source directory of this script
     */
    parse source sd1 sd2 sScript
    sScriptDir = filespec('drive', sScript) || filespec('path', sScript);
    sLogFile = sScriptDir || '\logs\' || sDate || '.log';
    sTree    = sScriptDir || '..\tree' || sDate;
    'mkdir 'sScriptDir||'\logs'
    'mkdir 'sScriptDir||'\DBBackup'

    /*
     * Clean tree, get it and build it.
     */
    'mkdir' sTree
    filespec('drive', sScript);
    'cd' sTree;
    if (rc <> 0) then call failure rc, 'cd ..\'sTree 'failed.';
    'call' sScriptDir || 'odin32env.cmd'
    if (rc <> 0) then call failure rc, 'Env failed.';
    'call' sScriptDir || 'odin32clean.cmd'
    if (rc <> 0) then call failure rc, 'Clean failed.';
    'call' sScriptDir || 'odin32get.cmd'
    if (rc <> 0) then call failure rc, 'Get failed.';
    'call' sScriptDir || 'odin32bldnr.cmd inc'
    if (rc <> 0) then call failure rc, 'Build Nr inc failed.';
    'call' sScriptDir || 'odin32build.cmd 2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, 'Build failed.';
    'call' sScriptDir || 'odin32bldnr.cmd commit'
    if (rc <> 0) then call failure rc, 'Build Nr commit failed.';

    /*
     * Pack and upload it.
     */
    'call' sScriptDir || 'odin32pack.cmd  2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, 'Packing failed.';
    'start /BG "Uploading Odin..." nice -f cmd /C' sScriptDir || 'odin32ftp2.cmd';
    if (rc <> 0) then say 'rc='rc' FTPing failed. i = ' || i;

    /*
     * database update
     */
    sScriptDir || 'odin32db.cmd  2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, 'db failed.';

    /*
     * Update local installation at d:\odin...
     */
/*
    'd:';
    if (rc <> 0) then call failure rc, 'd: failed.';
    'cd d:\odin';
    if (rc <> 0) then call failure rc, 'cd d:\odin failed.';
    'unzip -o ' || sTree || '\odin*debug.zip';
    if (rc <> 0) then call failure rc, 'unzip failed.';
    'd:'
    if (rc <> 0) then call failure rc, 'd: failed.';
*/
    /* successfull exit */
    exit(0);


/*
 * fatal failures terminates here!.
 */
failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

