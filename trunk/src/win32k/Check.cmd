/* $Id: Check.cmd,v 1.2 2000-03-09 20:46:35 bird Exp $
 *
 * Dirty hack to get around the nmake feature of passing on MAKEFLAGS.
 * This simply executes it's arguments and returns the errorcode.
 */
    parse arg sArgs
    '@echo off';
    sArgs;
    exit RC ;
