/* $Id: odin32db.cmd,v 1.1 2000-04-27 11:32:25 bird Exp $
 *
 * Updates the Odin32 API database.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    /* load rexxutils functions */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysloadFuncs;

    'cd tools\database';
    if rc <> 0 then call failure rc, 'cd db failed';
    'nmake apiimport.exe ..\bin\APIImport.exe';
    if rc <> 0 then call failure rc, 'nmake db failed';
    'nmake stateupd.exe ..\bin\stateupd.exe';
    if rc <> 0 then call failure rc, 'nmake db failed';
    'cd ..\..\src';
    if rc <> 0 then call failure rc, 'cd src failed';
    'nmake apiimport';
    if rc <> 0 then call failure rc, 'apiimport failed';
    'nmake stateupd 2>&1';
    if rc <> 0 then call failure rc, 'stateupd failed';

    /* create database backup */
    'c:'
    if rc <> 0 then call failure rc, 'c: failed';
    'cd c:\emx\mysql\data\odin32'
    if rc <> 0 then call failure rc, 'cd <> failed';
    'mysqladmin refresh';
    'rar a -m5 d:\temp\db'||date(S)||'.rar *'
    if rc <> 0 then call failure rc, 'rar db failed';
    'cd \';
    'd:';
    'cd ..';
    exit(0);

failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

