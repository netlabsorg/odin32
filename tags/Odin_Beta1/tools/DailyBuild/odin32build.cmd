/* $Id: odin32build.cmd,v 1.1 2000-04-27 11:32:24 bird Exp $
 *
 * Builds debug and release editions of Odin32.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    /* debug build */
    'SET DEBUG=1';
    'nmake dep';
    if (RC <> 0) then call failure rc, 'Make failed (dep).';
    'nmake NODEBUGINFO=1';
    if (RC <> 0) then call failure rc, 'Make debug failed.';

    /* release build */
    'SET DEBUG=';
    'nmake ';
    if (RC <> 0) then call failure rc, 'Make release failed .';

    exit(0);

failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

