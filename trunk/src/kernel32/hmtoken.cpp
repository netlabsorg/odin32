/* $Id: hmtoken.cpp,v 1.5 2001-04-26 13:22:47 sandervl Exp $ */

/*
 * Win32 process/thread token handle manager class
 *
 * TODO: Don't put pointer to security info struct in userdata
 *       Not valid when thread is done or terminated
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
#include <wprocess.h>

#define DBG_LOCALLOG	DBG_hmtoken
#include "dbglocal.h"

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::OpenThreadToken
 * Purpose   : 
 * Variables :
 * Result    : 
 * Remark    : TODO: OpenAsSelf
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMDeviceTokenClass::OpenThreadToken(PHMHANDLEDATA pHMHandleData, 
                                          HANDLE  ThreadHandle,
                                          BOOL    OpenAsSelf)
{
 TEB *teb = GetTEBFromThreadHandle(ThreadHandle);

  if(teb == 0) {
	dprintf(("HMDeviceTokenClass::OpenThreadToken: thread handle %x not found", ThreadHandle));
	return ERROR_INVALID_PARAMETER;
  }
  pHMHandleData->dwUserData = (DWORD)&teb->o.odin.threadinfo;
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
BOOL HMDeviceTokenClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  return TRUE;
}
