/* $Id: odin32pack.cmd,v 1.9 2001-01-09 18:46:28 bird Exp $
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
    call packdir3dx 'debug', 'debug'
    call packdir 'release', 'release'
    call packdir3dx 'release', 'release'
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
    'del /Q /Y /Z /X .\Voodoo1\CVS > nul 2>&1'
    'del /Q /Y /Z /X .\Voodoo1 > nul 2>&1'
    'del /Q /Y /Z /X .\Voodoo2\CVS > nul 2>&1'
    'del /Q /Y /Z /X .\Voodoo2 > nul 2>&1'
    'rmdir .\Voodoo2 > nul 2>&1'
    'rmdir .\Voodoo1 > nul 2>&1'

    /*
     * Create a pack directory in /bin and go into it.
     * (Don't test on return from mkdir while the directory might exist.)
     */
    'mkdir ..\pack'
    call ChDir '..\pack'
    'del /Q /Y /Z *' /* Perform some cleanup */

    /* Copy root files into the pack directory. */
    call copy '..\wgss50.dll', 'system32\wgss50.dll';
    call copy '..\odin.ini'
    call copy '..\..\doc\odin.ini.txt'
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
    'ren .\system32\glide ..\glide_tmp'
    if (RC <> 0) then
    do
        'ren .\system32 ..\'||sDir
        call failure rc, 'renaming system32\glide -> ..\glide_tmp failed';
    end

    say 'zip -9 -R' sZipFile '* -xCVS';
    'zip -9 -R' sZipFile '* -xCVS';
    if (RC <> 0) then
    do
        rc2 = rc;
        'ren ..\glide_tmp .\system32\glide'
        'ren .\system32 ..\'||sDir
        call failure rc2, 'zip...';
    end
    'ren ..\glide_tmp .\system32\glide'
    if (RC <> 0) then call failure rc, 'renaming glide_tmp failed';
    'ren .\system32 ..\'||sDir
    if (RC <> 0) then call failure rc, 'renaming' sDir'->system32 failed';

    /* restore directory */
    call ChDir '..';
    return;

/*
 * Pack the 3dx dlls.
 */
packdir3dx: procedure expose sStartDir;
parse arg sDir, sType;
    sZipFile = filespec('path', directory())||'odin32bin-' || DATE(S) || '-glide-' || sType || '.zip';
    sDir = directory();

    call ChDir sDir
    say 'zip -9 -R' sZipFile 'glide\* -xCVS';
    'zip -9 -R' sZipFile 'glide\* -xCVS';
    if (rc <> 0) then call failure rc, 'zip...';

    /* restore directory */
    call directory(sDir);
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
    parse arg sSrc, sDst, fNotFatal

    /* if no sDst set default */
    if (sDst = '') then sDst='.';
    if (fNotFatal = '') then fNotFatal = 0;

    'copy' sSrc sDst
    if (rc <> 0 & \fNotFatal) then
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

