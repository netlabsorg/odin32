/* $Id: odin32env.cmd,v 1.28 2003-02-06 20:46:18 bird Exp $
 *
 * Sets the build environment.
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

call BuildEnv 'warpin mysql perl cvs ddkbase mscv6-16 emx gcc303 vac365õ watcomc11cõ vac40õ vac308 toolkit40 debug'
'Set CVSROOT=:pserver:bird@www.netlabs.org:/netlabs.cvs/odin32';
'Set MULTIJOBS=1';
'Set BUILD_SETUP_MAK=';
'Set EMX=%PATH_EMXPGCC%';

exit(0);


