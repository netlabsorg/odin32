/* $Id: odin32get.cmd,v 1.2 2000-07-18 11:18:14 bird Exp $
 *
 * Gets the CVS tree from netlabs.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

    'cvs checkout .'
    if (RC <> 0) then call failure rc, 'CVS checkout . failed';
    exit(0);


failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

