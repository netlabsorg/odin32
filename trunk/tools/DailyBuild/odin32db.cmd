/* $Id: odin32db.cmd,v 1.3 2001-06-27 08:55:14 bird Exp $
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

    /*
     * Get source directory of this script
     */
    parse source sd1 sd2 sScript
    sScriptDir = filespec('drive', sScript) || filespec('path', sScript);

    'cd tools\database';
    if rc <> 0 then call failure rc, 'cd db failed';
    'nmake';
    if rc <> 0 then call failure rc, 'nmake db failed';
    'cd ..\..\src';
    if rc <> 0 then call failure rc, 'cd src failed';
    'nmake apiimport';
    if rc <> 0 then call failure rc, 'apiimport failed';
    'nmake stateupd 2>&1';
    if rc <> 0 then call failure rc, 'stateupd failed';

    /* create database backup */
    filespec('drive', getMySqlDataDir());
    if rc <> 0 then call failure rc, 'c: failed';
    'cd' getMySqlDataDir()||'\odin32';
    if rc <> 0 then call failure rc, 'cd <> failed';
    'mysqladmin refresh';
    'rar a -m5 ' || sScriptDir || 'dbbackup\db'||date(S)||'.rar *'
    if rc <> 0 then call failure rc, 'rar db failed';
    'cd \';
    'd:';
    'cd ..';
    exit(0);

failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);


/*
 * Get the MySql data directory.
 */
getMySqlDataDir: procedure

    /* Get mysql variables */
    '@mysqladmin variables | rxqueue /lifo'

    /* Get datadir */
    sDataDir = '';
    do queued()
        pull s;
        if (pos(' DATADIR ', s) > 0) then
        do
            sDataDir = strip( substr( s, pos('|',s,3) + 1 ) );
            sDataDir = strip( substr(sDataDir, 1, length(sDataDir)-1) );
            leave;
        end
    end

    /* Drain queue */
    do queued()
        pull s
    end
    drop s;

    /* If failure set default directory. */
    if (sDataDir = '') then
        sDataDir = 'd:\knut\Apps\MySql\data\';
    return sDataDir;

