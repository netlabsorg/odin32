/* $Id: Exists.cmd,v 1.3 2002-05-16 11:50:23 bird Exp $
 *
 * Simple rexx util which checks if a file or directory exists.
 *
 * Syntax: exists.cmd <filename>
 * Return code: 0 - file exists
 *              1 - file doesn't exist.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
 *
 */
    parse arg sFile

    if (sFile = '.' | sFile = '..') then
        exit(0);

    if (stream(sFile, 'c', 'query exists') = '') then
    do  /* directory ? */
        if (RxFuncQuery('SysFileTree') = 1) then
            call RxFuncAdd 'SysFileTree', 'RexxUtil', 'SysFileTree';
        rc = SysFileTree(sFile, 'sDirs', 'DO');
        if (rc = 0 & sDirs.0 = 1) then
            exit(0);
        exit(1);
    end
    exit(0);

