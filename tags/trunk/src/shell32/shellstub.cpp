/* $Id: shellstub.cpp,v 1.6 2003-10-02 10:39:22 sandervl Exp $ */

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


DWORD WIN32API SHCreateStdEnumFmtEtc(DWORD x1, DWORD x2, DWORD x3)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API SHFindFiles( DWORD x1, DWORD x2)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API SHFindComputer(DWORD x1, DWORD x2)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API DAD_AutoScroll(DWORD x1, DWORD x2, DWORD x3)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API DAD_DragEnter(DWORD x1)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API DAD_DragEnterEx(DWORD x1, DWORD x2, DWORD x3)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API DAD_DragLeave()
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API DAD_DragMove(DWORD x1, DWORD x2)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API Desktop_UpdateBriefcaseOnEvent(DWORD x1)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API InvalidateDriveType(DWORD x1)
{
  dprintf(("STUB!"));
  return 0;
}


DWORD WIN32API SHGlobalDefect(DWORD x1)
{
  dprintf(("STUB!"));
  return 0;
}

DWORD WIN32API SHHandleDiskFull(DWORD x1, DWORD x2)
{
  dprintf(("STUB!"));
  return 0;
}

BOOL WIN32API SHGetNewLinkInfoA(LPCTSTR pszLinkTo, LPCTSTR pszDir,
                                LPTSTR pszName, BOOL *pfMustCopy,
                                UINT uFlags)
{
  dprintf(("NOT IMPLEMENTED: SHGetNewLinkInfoA %s %s %s %x %x", pszLinkTo, pszDir, pszName, pfMustCopy, uFlags));
  return FALSE;
}

BOOL WIN32API SHGetNewLinkInfoW(LPCWSTR pszLinkTo, LPCWSTR pszDir,
                                LPWSTR pszName, BOOL *pfMustCopy,
                                UINT uFlags)
{
  dprintf(("NOT IMPLEMENTED: SHGetNewLinkInfoA %ls %ls %ls %x %x", pszLinkTo, pszDir, pszName, pfMustCopy, uFlags));
  return FALSE;
}
