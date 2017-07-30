/* $Id: odin32pack.cmd,v 1.25 2003-04-15 00:34:35 bird Exp $
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
    sDate = value('BUILD_DATE',, 'OS2ENVIRONMENT');
    sType = value('BUILD_TYPE',, 'OS2ENVIRONMENT');
    if ((sDate = '') | (sType = '')) then do say 'BUILD_DATE/BUILD_TYPE unset, you didn''t start job.cmd.'; exit(16); end

    if (sType = 'W') then
        sTypeOdinCMD = '-Weekly';
    else
        sTypeOdinCMD = '-Daily';


    /*
     * Make .WPI files.
     */
    call ChDir 'tools\install';
    'call odin.cmd 'sTypeOdinCMD' debug'
    if (RC <> 0) then call failure rc, 'odin.cmd debug failed.';
    'call odin.cmd 'sTypeOdinCMD' release'
    if (RC <> 0) then call failure rc, 'odin.cmd release failed.';
    'move *.wpi' sStartDir;
    if (RC <> 0) then call failure rc, 'failed to move the *.wpi ->' sStartDir;
    call ChDir '..\..';

    /*
     * Make .ZIP files.
     */
    call packdir 'bin\debug', 'debug'
    /*call packdir3dx 'bin\debug\glide', 'glide-debug' */
    call packdir 'bin\release', 'release'
    /*call packdir3dx 'bin\release\glide', 'glide-release'*/

    /*
     * Make copy.
     */
     /*
    if (sType = 'W') then
        'copy *.wpi e:\DailyBuildArchive\'
    else
        'copy *.zip e:\DailyBuildArchive\'
      */

    /* return successfully */
    exit(0);


packdir: procedure expose sStartDir sDate sType;
parse arg sDir, sBldType;

    sZipFile = directory() || '\odin32bin-' || sDate || '-' || sBldType || '.zip';

    /*
     * Change into the directory we're to pack and do some fixups
     */
    sRoot = directory();
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
    call copy sRoot'\LICENSE.txt';
    call copy sRoot'\WGSS50.lic';
    call copy sRoot'\ChangeLog';
    call copy sRoot'\doc\Readme.txt';
    call copy sRoot'\doc\odin.ini.txt'
    call copy sRoot'\doc\Logging.txt';
    call copy sRoot'\doc\ReportingBugs.txt';
    call copy sRoot'\doc\ChangeLog-2001';
    call copy sRoot'\doc\ChangeLog-2000';
    call copy sRoot'\doc\ChangeLog-1999';

    /*
     * Move (=rename) the /bin/<release|debug> dir into /pack/system32
     * and pack it.
     */
    'ren' sRoot'\'sDir '.\system32'
    if (RC <> 0) then call failure rc, 'renaming' sDir'->system32 failed';
    'if exist .\system32\glide ren .\system32\glide ..\glide_tmp'
    /*
    if (RC <> 0) then
    do
        rc2 = rc;
        call backout sDir, sBldType, sRoot;
        call failure rc2, 'renaming system32\glide -> ..\glide_tmp failed';
    end
    */

    call copy sRoot'\bin\wgss50.dll', 'system32\wgss50.dll';
    /*call copy sRoot'\bin\odin.ini', 'system32\Odin.ini';*/
    if (pos('debug', sBldType) > 0) then
    do
        call copy sRoot'\bin\release\odincrt.dll', 'system32\odincrt.dll'
        call copy sRoot'\bin\release\odincrt.sym', 'system32\odincrt.sym'
    end

    say 'zip -9 -R' sZipFile '* -xCVS';
    'zip -9 -R' sZipFile '* -xCVS';
    if (RC <> 0) then
    do
        rc2 = rc;
        call backout sDir, sBldType, sRoot;
        call failure rc2, 'zip...';
    end

    /* resotre */
    call backout sDir, sBldType, sRoot;

    /* restore directory */
    call directory(sRoot);
    return;

/* backout procedure for packdir */
backout: procedure;
    parse arg sDir, sBldType, sRoot
    if (pos('debug', sBldType) > 0) then
    do
        'del system32\odincrt.dll'
        'del system32\odincrt.sym'
    end
    'ren ..\glide_tmp .\system32\glide'
    'ren .\system32' sRoot'\'sDir
    return;

/*
 * Pack the 3dx dlls.
 */
packdir3dx: procedure expose sStartDir sDate sType;
parse arg sDir, sBldType;
    sZipFile = directory()||'\odin32bin-' || sDate || '-' || sBldType || '.zip';

    sRoot = directory();
    call ChDir sDir
    say 'zip -9 -R' sZipFile '* -xCVS';
    'zip -9 -R' sZipFile '* -xCVS';
    if (rc <> 0) then call failure rc, 'zip...';

    /* restore directory */
    call directory(sRoot);
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

