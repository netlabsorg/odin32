/* $Id: odin32env.cmd,v 1.31 2003-04-14 22:08:04 bird Exp $
 *
 * Sets the build environment.
 *
 * Copyright (c) 1999-2003 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*
 * Source dir.
 */
parse source . . sSrc
sDir = filespec('drive', sSrc) || filespec('path', sSrc);

/* base env */
sOldDir = directory();
call directory sDir||'\bin';
call BuildEnv 'warpin mysql perl~ cvs ddkbase mscv6-16 emx gcc321 vac365õ watcomc11cõ vac40õ vac308 toolkit40 debug'
call directory sOldDir;

/* minor adjustments. */
'Set CVSROOT=:pserver:bird@www.netlabs.org:/netlabs.cvs/odin32';
'Set MULTIJOBS=1';
'Set BUILD_SETUP_MAK=';
'Set EMX=%PATH_EMXPGCC%';

exit(0);

