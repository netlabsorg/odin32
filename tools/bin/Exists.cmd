/* $Id: Exists.cmd,v 1.1 2000-03-09 17:23:23 bird Exp $
 *
 * Simple rexx util which checks if an file exists.
 *
 * Syntax: exists.cmd <filename>
 * Return code: 0 - file exists
 *              1 - file don't exist.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    parse arg sFile

    if (stream(sFile, 'c', 'query exists') = '') then
        exit(1);
    exit(0);

