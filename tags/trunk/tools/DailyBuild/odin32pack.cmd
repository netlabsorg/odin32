/* $Id: odin32pack.cmd,v 1.1 2000-04-27 11:32:25 bird Exp $
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
    'cd bin\debug'
    if RC <> 0 then call failure rc, 'ChDir to bin\debug failed.';
    call packdir 'debug'

    'cd ..\..\bin\release'
    if RC <> 0 then call failure rc, 'ChDir to bin\release failed.';
    call packdir 'release'
    'cd ..\..';

    exit(0);

packdir: procedure
parse arg stype;
    'del /Q /Y /Z *.cmd'
    'del /Q /Y /Z /X CVS'
    'del /Q /Y /Z /X .\glide\CVS'
    'del /Q /Y /Z /X .\glide\Voodoo1\CVS'
    'del /Q /Y /Z /X .\glide\Voodoo2\CVS'
    'copy ..\odin.ini odin.ini'
    if (RC <> 0) then call failure rc, 'copy odin.ini to bin failed.';
    'copy ..\..\license.txt'
    if (RC <> 0) then call failure rc, 'copy license.txt to bin failed.';
    'copy ..\..\readme.txt'
    if (RC <> 0) then call failure rc, 'copy readme.txt to bin failed.';
    'copy ..\..\logging.txt logging.txt'
    if (RC <> 0) then call failure rc, 'copy logging.txt to bin failed.';
    'copy ..\..\odin.ini.txt .\odin_ini.txt'
    if (RC <> 0) then call failure rc, 'copy odin.ini.txt to bin failed.';
    'copy ..\..\ChangeLog'
    if (RC <> 0) then call failure rc, 'copy ChangeLog to bin failed.';
    'copy ..\Glide\readme.txt Glide\readme.txt'
    if (RC <> 0) then call failure rc, 'copy a Glide\readme.txt to bin failed.';
    'copy ..\Glide\Voodoo1\readme.txt Glide\Voodoo1\readme.txt'
    if (RC <> 0) then call failure rc, 'copy a Glide\Voodoo1\readme.txt to bin failed.';
    'copy ..\Glide\Voodoo2\readme.txt Glide\Voodoo2\readme.txt'
    if (RC <> 0) then call failure rc, 'copy a Glide\Voodoo2\readme.txt to bin failed.';

    'zip -9 -R ..\..\odin32bin-' || DATE(S) || '-' || stype || '.zip * -xCVS';
    if (RC <> 0) then call failure rc, 'zip...';
    return;

failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    'cd ..\..';
    exit(rc);

