/* $Id: Check.cmd,v 1.1 2000-03-09 20:30:43 bird Exp $
 *
 * Dirty hack to get around the nmake feature of passing on MAKEFLAGS.
 * This simply executes it's arguments and returns the errorcode.
 */
    parse arg sArgs
    sArgs;
    exit RC ;
