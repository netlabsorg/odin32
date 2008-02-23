/* $Id: d16Globl.c,v 1.3 2002-12-16 01:36:57 bird Exp $
 *
 * Global data - everything in is 16-bit data segment!
 *
 * Copyright (c) 1999-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_NOPMAPI
#include <os2.h>
#include "..\..\..\..\include\odinbuild.h" /* FIXME */
#include "options.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/

/* Note: All global variables must be initialized!  *
 *       Uninitialized variables ends up in DATA32. */
PFN         Device_Help = NULL;
ULONG       pulTKSSBase32 = 0;
USHORT      R0FlatCS16  = 0;
USHORT      R0FlatDS16  = 0;
ULONG       linDT = 0;                  /* dostable */
ULONG       linDT2 = 0;                 /* dostable2 */
BOOL        fInitTime   = TRUE;
struct kKLOptions options = DEFAULT_OPTION_ASSIGMENTS;

char        szBuildDate[] = {__DATE__};
char        szBuildTime[] = {__TIME__};

/* current OS and kernel info */
ULONG       fKernel = 0xffffffff;       /* Smp or uni kernel. */
ULONG       ulKernelBuild = 0xffffffff; /* Kernel build. */
USHORT      usVerMajor = 0xffff;        /* OS/2 major ver - 20 */
USHORT      usVerMinor = 0xffff;        /* OS/2 minor ver - 30,40 */
ULONG       ulkKrnlLibBuild = ODIN32_BUILD_NR;
char        szSymbolFile[128] = {0};    /* The symbol file to load / loaded. */

