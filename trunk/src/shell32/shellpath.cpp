/* $Id: shellpath.cpp,v 1.11 2000-08-18 02:01:23 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Path Functions
 *
 * Many of this functions are in SHLWAPI.DLL also
 *
 * Corel WINE 20000324 level (without CRTDLL_* calls)
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#define HAVE_WCTYPE_H
#include <odin.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"
#include "crtdll.h"

#include "shlobj.h"
#include "shell32_main.h"

#include <heapstring.h>
#include <misc.h>


ODINDEBUGCHANNEL(SHELL32-SHELLPATH)

