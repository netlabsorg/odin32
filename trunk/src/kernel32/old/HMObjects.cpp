/* $Id: HMObjects.cpp,v 1.1 1999-09-15 23:32:48 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 */

#undef DEBUG_LOCAL
//#define DEBUG_LOCAL


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "misc.h"

#include "HandleManager.H"
#include "HMObjects.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/



/*****************************************************************************
 * Name      : DWORD HMDeviceKernelObjectClass::CloseHandle
 * Purpose   : close the handle
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceKernelObjectClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  BOOL bRC;

  dprintf(("KERNEL32: HandleManager::KernelObject::CloseHandle(%08x)\n",
          pHMHandleData->hHMHandle));

  bRC = O32_CloseHandle(pHMHandleData->hHMHandle);

  dprintf(("KERNEL32: HandleManager::KernelObject::CloseHandle returned %08xh\n",
          bRC));

  return (DWORD)bRC;
}

