/* $Id: odinbuild.h,v 1.133 2001-08-10 19:25:15 bird Exp $
 *
 * Odin32 version number and build numbers.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _ODINBUILD_H_
#define _ODINBUILD_H_

/*
 * API for querying the build number of Odin32. Well.. Kernel32 really...
 */
int     WIN32API Odin32GetBuildNumber(void);

/*
 * Current Odin version and build number.
 * Used for bldlevel info.
 */
#define ODIN32_VERSION_MAJOR    0
#define ODIN32_VERSION_MINOR    5
#define ODIN32_VERSION       "0.5"
#define ODIN32_BUILD_NR       438        /* This started at 300 on the 17th Des 2000. */

#endif









































































































