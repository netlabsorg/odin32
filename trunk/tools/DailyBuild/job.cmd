/* $Id: job.cmd,v 1.10 2003-04-14 22:08:03 bird Exp $
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
    sStateDir = sScriptDir||'State'||sType;
    sLogFile = sScriptDir || 'Logs\' || sDate || '.log';
    sTree    = sScriptDir || '..\tree' || sDate;
    'call' sScriptDir||'bin\CreatePath.cmd 'sScriptDir||'Logs'
    'call' sScriptDir||'bin\CreatePath.cmd 'sScriptDir||'DBBackup'
    'call' sScriptDir||'bin\CreatePath.cmd 'sStateDir;

    /*
     * Clean tree, get it and build it.
     */
    'mkdir' sTree
    filespec('drive', sScript);
    'cd' sTree;
    if (rc <> 0) then call failure rc, '', 'cd 'sTree 'failed.';
    'call' sScriptDir || 'odin32env.cmd'
    if (rc <> 0) then call failure rc, '', 'Env failed.';
    if (IsChangeLogModified(sStateDir)) then
    do
        say 'Nothing to do. ChangeLog unmodified.'
        'echo ChangeLog unmodified >' sLogFile;
        exit(0);
    end
    'call' sScriptDir || 'odin32clean.cmd'
    if (rc <> 0) then call failure rc, sStateDir, 'Clean failed.';
    'call' sScriptDir || 'odin32get.cmd'
    if (rc <> 0) then call failure rc, sStateDir, 'Get failed.';
    'call' sScriptDir || 'odin32bldnr.cmd inc'
    if (rc <> 0) then call failure rc, sStateDir, 'Build Nr inc failed.';
    'call' sScriptDir || 'odin32build.cmd 2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, sStateDir, 'Build failed.';
    'call' sScriptDir || 'odin32bldnr.cmd commit'
    if (rc <> 0) then call failure rc, sStateDir, 'Build Nr commit failed.';

    /*
     * Pack and upload it.
     */
    'call' sScriptDir || 'odin32pack.cmd  2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, sStateDir, 'Packing failed.';
    'call' sScriptDir || 'odin32ftp2.cmd';
    if (rc <> 0) then call failure rc, sStateDir, 'Upload failed!';


    /*
     * database update
     */
    /*
    sScriptDir || 'odin32db.cmd  2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, '', 'db failed.';
    */

    /* successfull exit */
    exit(0);


/*
 * fatal failures terminates here!.
 */
failure: procedure
parse arg rc, sStateDir, sText;
    say 'rc='rc sText
    if (sStateDir <> '') then
        call ForceNextBuild sStateDir;
    exit(rc);


/*
 * Checks if the change log is up to date or not.
 */
IsChangeLogModified: procedure
parse arg sStateDir;

    sDir = directory();
    'cd' sStateDir
    if (rc <> 0) then call failure rc, 'cd 'sStateDir' failed!';

    if (stream(sStateDir'\ChangeLog', 'c', 'query exist') == '') then
    do
        /* no such file: check it out. */
        fUpToDate = 0;
    end
    else
    do
        /* check if up to date. */
        'cvs status ChangeLog | grep -q "Status: Up-to-date"';
        if (rc <> 0) then
            fUpToDate = 0;
        else
            fUpToDate = 1;
    end

    /*
     * Check out the latest ChangeLog.
     */
    if (\fUpToDate) then
    do
        /* check if up to date. */
        'if exist ChangeLog del ChangeLog';
        'call cvs checkout ChangeLog';
    end

    call directory sDir;
return fUpToDate;


/*
 * Force build next time.
 * Called when we fail.
 */
ForceNextBuild: procedure
parse arg sStateDir;
    'if exist 'sStateDir'\ChangeLog del 'sStateDir'\ChangeLog';
return rc;
