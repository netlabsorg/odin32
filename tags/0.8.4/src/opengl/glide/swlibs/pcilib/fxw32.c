/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
** 
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.  
** 
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
*/
#include <stdlib.h>
#include <stdio.h>

#define WIN32_LEANER_AND_MEANER
#include <windows.h>

#include <3dfx.h>
#define FX_DLL_DEFINITION
#include <fxdll.h>
#include <fxmemmap.h>
#include "fxpci.h"
#include "pcilib.h"

#include "fxw32.h"

/* Stuff shared across all usable win32 platforms */
HANDLE hMemmapFile;

FxBool 
pciPlatformInit(void)
{
  OSVERSIONINFO
    osInfo;
  FxBool
    retVal;

  osInfo.dwOSVersionInfoSize = sizeof(osInfo);
  retVal = GetVersionEx(&osInfo);
  if (retVal) {
    switch(osInfo.dwPlatformId) {
    case VER_PLATFORM_WIN32_WINDOWS:
      gCurPlatformIO = ioProcs9x;
      break;

    case VER_PLATFORM_WIN32_NT:
      gCurPlatformIO = ioProcsNT;
      break;

    default:
    case VER_PLATFORM_WIN32s:
      gCurPlatformIO = NULL;
      break;
    }

    retVal = (gCurPlatformIO != NULL);
  }

  return retVal;
}
