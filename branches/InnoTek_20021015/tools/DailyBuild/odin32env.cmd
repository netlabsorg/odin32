/* $Id: odin32env.cmd,v 1.26 2002-09-05 01:06:07 bird Exp $
 *
 * Sets the build environment.
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

    call BuildEnv 'warpin mysql perl cvs ddkbase mscv6 emx vac365� watcomc11c� vac40� vac308 toolkit40 debug'
    'Set CVSROOT=:pserver:bird@www.netlabs.org:/netlabs.cvs/odin32';
    'Set MULTIJOBS=1';

    exit(0);


