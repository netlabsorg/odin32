/* $Id: shellstub.cpp,v 1.3 2001-04-28 13:33:47 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Project Odin Software License can be found in LICENSE.TXT
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


ODINDEBUGCHANNEL(SHELL32-SHELLSTUB)

ODINFUNCTION3(DWORD, SHCreateStdEnumFmtEtc, DWORD, x1, DWORD, x2, DWORD, x3)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION4(DWORD, CIDLData_CreateFromIDArray, DWORD, x1, DWORD, x2, DWORD, x3, DWORD, x4)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION2(DWORD, SHFindFiles, DWORD, x1, DWORD, x2)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION2(DWORD, SHFindComputer, DWORD, x1, DWORD, x2)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION3(DWORD, DAD_AutoScroll, DWORD, x1, DWORD, x2, DWORD, x3)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION1(DWORD, DAD_DragEnter, DWORD, x1)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION3(DWORD, DAD_DragEnterEx, DWORD, x1, DWORD, x2, DWORD, x3)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION0(DWORD, DAD_DragLeave)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION2(DWORD, DAD_DragMove, DWORD, x1, DWORD, x2)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION1(DWORD, Desktop_UpdateBriefcaseOnEvent, DWORD, x1)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION1(DWORD, InvalidateDriveType, DWORD, x1)
{
  dprintf(("STUB!"));
  return 0;
}


ODINFUNCTION1(DWORD, SHGlobalDefect, DWORD, x1)
{
  dprintf(("STUB!"));
  return 0;
}

ODINFUNCTION2(DWORD, SHHandleDiskFull, DWORD, x1, DWORD, x2)
{
  dprintf(("STUB!"));
  return 0;
}
