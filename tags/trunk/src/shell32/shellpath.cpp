/* $Id: shellpath.cpp,v 1.10 2000-05-18 14:16:04 sandervl Exp $ */

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

/*************************************************************************
 * SheGetDirW [SHELL32.281]
 *
 */
ODINFUNCTION2(HRESULT, SheGetDirW,
              LPWSTR, u,
              LPWSTR, v)
{  FIXME("%p %p stub\n",u,v);
   return 0;
}

/*************************************************************************
 * SheChangeDirW [SHELL32.274]
 *
 */
ODINFUNCTION1(HRESULT, SheChangeDirW,
              LPWSTR, u)
{  FIXME("(%s),stub\n",debugstr_w(u));
   return 0;
}
