/* $Id: hmtoken.cpp,v 1.1 1999-12-18 21:45:55 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 */


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
#include "HMToken.h"


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::OpenThreadToken
 * Purpose   : 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMDeviceTokenClass::OpenThreadToken(PHMHANDLEDATA pHMHandleData, 
                                          DWORD dwUserData,
                                          HANDLE  ThreadHandle,
                                          BOOL    OpenAsSelf)
{
  pHMHandleData->dwUserData = dwUserData;
  pHMHandleData->dwInternalType = HMTYPE_THREADTOKEN;
  return STATUS_SUCCESS;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::OpenThreadToken
 * Purpose   : 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMDeviceTokenClass::OpenProcessToken(PHMHANDLEDATA pHMHandleData, 
                                           DWORD dwUserData,
                                           HANDLE  ProcessHandle)
{
  pHMHandleData->dwUserData = dwUserData;
  pHMHandleData->dwInternalType = HMTYPE_PROCESSTOKEN;
  return STATUS_SUCCESS;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::CloseHandle
 * Purpose   : 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMDeviceTokenClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  return STATUS_SUCCESS;
}
