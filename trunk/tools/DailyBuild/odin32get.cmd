/* $Id: odin32get.cmd,v 1.1 2000-04-27 11:32:25 bird Exp $
 *
 * Gets the CVS tree from netlabs.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    'cd ..\tree'
    if (RC <> 0) then call failure rc, 'Change Dir ..\tree failed';
    'cvs checkout .'
    if (RC <> 0) then call failure rc, 'CVS checkout . failed';
    exit(0);


failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

