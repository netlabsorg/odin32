/* $Id: odin32get.cmd,v 1.3 2002-06-26 22:11:43 bird Exp $
 *
 * Gets the CVS tree from netlabs.
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

    /* get build settings */
    /*
    sDate = value('BUILD_DATE',, 'OS2ENVIRONMENT');
    sType = value('BUILD_TYPE',, 'OS2ENVIRONMENT');
    if ((sDate = '') | (sType = '')) then do say 'BUILD_DATE/BUILD_TYPE unset, you didn''t start job.cmd.'; exit(16); end
    */

    'cvs checkout .'
    if (RC <> 0) then call failure rc, 'CVS checkout . failed';
    exit(0);


failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

