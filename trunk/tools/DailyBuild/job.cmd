/* $Id: job.cmd,v 1.2 2000-07-18 11:18:13 bird Exp $
 *
 * Main job for building OS/2.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

    /* Load rexxutils functions */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysloadFuncs;

    sLogFile = '..\logs\' || Date(S) || '.log';
    sTree    = 'tree' || Date(S);

    /*
     * Clean tree, get it and build it.
     */
    'mkdir ..\'||sTree
    'cd ..\'||sTree
    if (rc <> 0) then call failure rc, 'cd ..\'sTree 'failed.';
    'call ..\scripts\odin32env.cmd'
    if (rc <> 0) then call failure rc, 'Env failed.';
    'call ..\scripts\odin32clean.cmd'
    if (rc <> 0) then call failure rc, 'Clean failed.';
    'call ..\scripts\odin32get.cmd'
    if (rc <> 0) then call failure rc, 'Get failed.';
    'call ..\scripts\odin32build.cmd 2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, 'Build failed.';

    /*
     * Pack and upload it.
     */
    'call ..\scripts\odin32pack.cmd  2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, 'Packing failed.';
    'start /BG "Uploading Odin..." nice -f cmd /C ..\scripts\odin32ftp2.cmd';
    if (rc <> 0) then say 'rc='rc' FTPing failed. i = ' || i;

    /*
     * database update
     */
    '..\scripts\odin32db.cmd  2>&1 | tee /a ' || sLogFile; /* 4OS/2 tee command. */
    if (rc <> 0) then call failure rc, 'db failed.';

    /*
     * Update local installation at c:\odin32...
     */
    'c:';
    if (rc <> 0) then call failure rc, 'c: failed.';
    'cd c:\odin32';
    if (rc <> 0) then call failure rc, 'cd c:\odin32 failed.';
    'unzip -o d:\odin32\tree\*debug.zip';
    if (rc <> 0) then call failure rc, 'unzip failed.';
    'd:'
    if (rc <> 0) then call failure rc, 'd: failed.';

    /* successfull exit */
    exit(0);


/*
 * fatal failures terminates here!.
 */
failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);
