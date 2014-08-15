/* $Id: odin32build.cmd,v 1.2 2002-06-26 22:08:32 bird Exp $
 *
 * Builds debug and release editions of Odin32.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    /* get build settings */
    /*sDate = value('BUILD_DATE',, 'OS2ENVIRONMENT');
    sType = value('BUILD_TYPE',, 'OS2ENVIRONMENT');
    if ((sDate = '') | (sType = '')) then do say 'BUILD_DATE/BUILD_TYPE unset, you didn''t start job.cmd.'; exit(16); end
    */

/*
 * Parse parameters.
 */
parse arg sSMP

    /* debug build */
    'SET DEBUG=1';
    'nmake dep';
    if (RC <> 0) then call failure rc, 'Make failed (dep).';
    'nmake NODEBUGINFO=1 'sSMP;
    if (RC <> 0) then call failure rc, 'Make debug failed.';

    /* release build */
    'SET DEBUG=';
    'nmake 'sSMP;
    if (RC <> 0) then call failure rc, 'Make release failed .';

    exit(0);

failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

