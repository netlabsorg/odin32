/* $Id: odin32clean.cmd,v 1.1 2000-04-27 11:32:24 bird Exp $
 *
 * Deletes the tree (current directory and all subdirectories).
 *
 * (assumes that rd is equal to "deltree"  (which it is in my installation).
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    'echo y > %temp%\y_odin32';
    'rd . < %temp%\y_odin32';
    if RC <> 0 then call failure rc, 'Del failed.';
    'del %temp%\y_odin32';
    exit(0);

failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

