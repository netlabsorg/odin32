/* $Id: ordinal.cpp,v 1.3 1999-06-24 19:27:49 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 * Copyright 1997 Marcus Meissner
 * Copyright 1988 Patrick Haller (adapted for win32os2)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <shellapi.h>
#include <winreg.h>

#include <stdarg.h>
//#include <builtin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wcstr.h>

#include <misc.h>
#include <nameid.h>
#include <unicode.h>
#include <winnls.h>

#include "shell32.h"


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/


/*************************************************************************
 *           SHELL32_16             [SHELL32.16]
 * find_lastitem_in_itemidlist()
 */
LPSHITEMID WIN32API ILFindLastID(LPITEMIDLIST iil)
{
  LPSHITEMID lastsii,
             sii;

  dprintf(("SHELL32: ILFindLastID (%08xh)\n",
           iil));

  if (!iil)
    return NULL;

  sii = &(iil->mkid);
  lastsii = sii;

  while (sii->cb)
  {
    lastsii = sii;
    sii = (LPSHITEMID)(((char*)sii)+sii->cb);
  }

  return lastsii;
}


/*************************************************************************
 *           SHELL32_71             [SHELL32.71]
 * returns internal shell values in the passed pointers
 */
BOOL WIN32API Shell_GetImageList(LPDWORD x,
                                 LPDWORD y)
{
  dprintf(("SHELL32: Shell_GetImageList (%08xh, %08xh) not implemented.\n",
           x,
           y));

  return TRUE;
}


/*************************************************************************
 *           SHELL32_72             [SHELL32.72]
 * dunno. something with icons
 */
void WIN32API Shell_GetCachedImageIndex(LPSTR x,
                                        DWORD y,
                                        DWORD z)
{
  dprintf (("SHELL32: Shell_GetCachedImageIndex (%08xh, %08xh, %08xh) not implemented.\n",
            x,
            y,
            z));
}


/*************************************************************************
 *           SHELL32_89             [SHELL32.89]
 */
DWORD WIN32API SHCloneSpecialIDList(DWORD x1,
                                    DWORD x2,
                                    DWORD x3)
{
  dprintf(("SHELL32: SHCloneSpecialIDList (%08xh, %08xh, %08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return 0;
}


/*************************************************************************
 *              SHELL32_119 [SHELL32.119]
 * unknown
 */
BOOL WIN32API IsLFNDrive(LPVOID x)
{
  dprintf(("SHELL32: IsLFNDrive (%08xh) not implemented.\n",
           x));

  return TRUE;
}


/*************************************************************************
 *           SHELL32_77             [SHELL32.77]
 */
DWORD WIN32API SHMapPIDLToSystemImageListIndex(DWORD x,
                                               DWORD y,
                                               DWORD z)
{
  dprintf(("SHELL32: SHMapPIDLToSystemImageListIndex (%08xh, %08xh, %08xh) not implemented.\n",
           x,
           y,
           z));

  return 0;
}


/*************************************************************************
 *           SHELL32_152            [SHELL32.152]
 * itemlist_length
 */
DWORD WIN32API ILGetSize(LPITEMIDLIST iil)
{
  LPSHITEMID    si;
  DWORD     len;

  dprintf(("SHELL32: ILGetSize (%08xh)\n",
           iil));


  si = &(iil->mkid);
  len = 2;

  while (si->cb)
  {
    len += si->cb;
    si   = (LPSHITEMID)(((char*)si)+si->cb);
  }

  return len;
}



/*************************************************************************
 *           SHELL32_18             [SHELL32.18]
 * copy_itemidlist()
 */
LPITEMIDLIST WIN32API ILClone(LPITEMIDLIST iil)
{
  DWORD     len;
  LPITEMIDLIST  newiil;

  dprintf(("SHELL32: ILClone(%08xh)\n",
           iil));


  len = ILGetSize(iil);
  newiil = (LPITEMIDLIST)SHAlloc(len);
  memcpy(newiil,
         iil,
         len);

  return newiil;
}


/*************************************************************************
 *           SHELL32_25             [SHELL32.25]
 * merge_itemidlist()
 */
LPITEMIDLIST WIN32API ILCombine(LPITEMIDLIST iil1,
                                LPITEMIDLIST iil2)
{
  DWORD     len1,len2;
  LPITEMIDLIST  newiil;

  dprintf(("SHELL32: ILCombine(%08xh, %08xh)\n",
           iil1,
           iil2));


  len1   = ILGetSize(iil1)-2;
  len2   = ILGetSize(iil2);
  newiil = (LPITEMIDLIST)SHAlloc(len1+len2);

  memcpy(newiil,
         iil1,
         len1);

  memcpy(((char*)newiil)+len1,
         iil2,
         len2);

  return newiil;
}


/*************************************************************************
 *           SHELL32_155            [SHELL32.155]
 * free_check_ptr - frees memory (if not NULL) allocated by SHMalloc allocator
 */
DWORD WIN32API ILFree(LPVOID x)
{
  dprintf(("SHELL32: ILFree (%08xh)\n",
           x));

  if (!x)
    return 0;

  return SHFree(x);
}


/*************************************************************************
 * SHELL32_85 [SHELL32.85]
 * unknown
 */
DWORD WIN32API OpenRegStream(DWORD x1,
                             DWORD x2,
                             DWORD x3,
                             DWORD x4)
{
  dprintf(("SHELL32: OpenRegStream (%08x, %08xh, %08xh, %08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.162
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHSimpleIDListFromPathAW(DWORD x1)
{
  dprintf(("SHELL32: .162 (%08xh) not implemented.\n",
           x1));

  return (0);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.102
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHCoCreateInstance(DWORD x1,
                                  DWORD x2,
                                  DWORD x3,
                                  DWORD x4,
                                  DWORD x5)
{
  dprintf(("SHELL32: SHCoCreateInstance (%08xh, %08xh, %08xh, %08xh, %08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return (0);
}
