/* $Id: odin32pack.cmd,v 1.6 2000-12-03 07:24:37 bird Exp $
 *
 * Make the two zip files.
 *
 * NOTE! This requires 4OS/2 for the DEL commands.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    sStartDir = directory();

    /*
     * Make .WPI files.
     */
    call ChDir 'tools\install';
    'call odin.cmd debug'
    if (RC <> 0) then call failure rc, 'odin.cmd debug failed.';
    'call odin.cmd release'
    if (RC <> 0) then call failure rc, 'odin.cmd release failed.';
    'move *.wpi' sStartDir;
    if (RC <> 0) then call failure rc, 'failed to move the *.wpi ->' sStartDir;
    call ChDir '..\..';

    /*
     * Make .ZIP files.
     */
    call ChDir 'bin';
    call packdir 'debug', 'debug'
    call packdir 'release', 'release'
    call ChDir '..';

    /* return successfully */
    exit(0);


packdir: procedure expose sStartDir;
parse arg sDir, sType;

    sZipFile = filespec('path', directory())||'odin32bin-' || DATE(S) || '-' || sType || '.zip';

    /*
     * Change into the directory we're to pack and do some fixups
     */
    call ChDir sDir
    'del /Q /Y /Z *.cmd > nul 2>&1'
    'del /Q /Y /Z /X CVS > nul 2>&1'
    'del /Q /Y /Z /X .\glide\CVS > nul 2>&1'
    'del /Q /Y /Z /X .\glide\Voodoo1\CVS > nul 2>&1'
    'del /Q /Y /Z /X .\glide\Voodoo2\CVS > nul 2>&1'
    call copy '..\odin.ini'
    call copy '..\..\doc\odin.ini.txt'

    /*
     * Create a pack directory in /bin and go into it.
     * (Don't test on return from mkdir while the directory might exist.)
     */
    'mkdir ..\pack'
    call ChDir '..\pack'
    'del /Q /Y /Z *' /* Perform some cleanup */

    /* Copy root files into the pack directory. */
    call copy '..\..\LICENSE.txt';
    call copy '..\..\ChangeLog';
    call copy '..\..\doc\Readme.txt';
    call copy '..\..\doc\Logging.txt';
    call copy '..\..\doc\ReportingBugs.txt';
    call copy '..\..\doc\ChangeLog-2000';
    call copy '..\..\doc\ChangeLog-1999';

    /*
     * Move (=rename) the /bin/<release|debug> dir into /pack/system32
     * and pack it.
     */
    'ren ..\'||sDir '.\system32'
    if (RC <> 0) then call failure rc, 'renaming' sDir'->system32 failed';

    say 'zip -9 -R' sZipFile '* -xCVS';
    'zip -9 -R' sZipFile '* -xCVS';
    if (RC <> 0) then
    do
        rc2 = rc;
        'ren .\system32 ..\'||sDir
        call failure rc2, 'zip...';
    end
    'ren .\system32 ..\'||sDir
    if (RC <> 0) then call failure rc, 'renaming' sDir'->system32 failed';

    /* restore directory */
    call ChDir '..';
    return;

/*
 * Changes the directory.
 * On error we will call failure.
 */
ChDir: procedure expose sStartDir;
    parse arg sDir

    'cd' sDir
    if (rc <> 0) then
    do
        call failure rc, 'Failed to ChDir into' sDir '(CWD='directory()').'
        return rc;
    end
    return 0;


/*
 * Copy a file.
 * On error we will call failure.
 */
Copy: procedure expose sStartDir
    parse arg sSrc, sDst

    /* if no sDst set default */
    if (sDst = '') then sDst='.';
    'copy' sSrc sDst
    if (rc <> 0) then
    do
        call failure rc, 'Copying' sSrc 'to' sDst 'failed.'
        return rc;
    end
    return 0;


/*
 * Complain about a failure and exit the script.
 * Note. Uses the global variable sStartDir to restore the current
 *       directory where the script was started from.
 * @param rc    Error code to write. (usually RC)
 * @param sText Description.
 */
failure: procedure expose sStartDir;
parse arg rc, sText;
    say 'rc='rc sText
    call directory sStartDir;
    exit(rc);

