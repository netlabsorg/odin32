/* $Id: Check.cmd,v 1.3 2000-04-17 02:32:34 bird Exp $
 *
 * Dirty hack to get around the nmake feature of passing on MAKEFLAGS.
 * This simply executes it's arguments and returns the errorcode.
 */
    parse arg sArgs
    '@echo off';
    '@SET MAKEFLAGS='
    sArgs;
    exit RC ;
