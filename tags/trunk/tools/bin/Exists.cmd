/* $Id: Exists.cmd,v 1.2 2001-05-16 01:17:03 bird Exp $
 *
 * Simple rexx util which checks if a file or directory exists.
 *
 * Syntax: exists.cmd <filename>
 * Return code: 0 - file exists
 *              1 - file doesn't exist.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    parse arg sFile

    if (sFile = '.' | sFile = '..') then
        exit(0);

    if (stream(sFile, 'c', 'query exists') = '') then
    do  /* directory ? */
        if (RxFuncQuery('SysFileTree') = 1) then
            call RxFuncAdd 'SysFileTree', 'RexxUtil', 'SysFileTree';
        rc = SysFileTree(sFile, sDirs, 'DO');
        if (rc = 0 & sDirs.0 = 1) then
            exit(0);
        exit(1);
    end
    exit(0);

