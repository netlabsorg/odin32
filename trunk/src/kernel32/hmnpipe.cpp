/* $Id: hmnpipe.cpp,v 1.2 2001-04-26 13:22:46 sandervl Exp $ */
/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 Named Pipes device access class
 *
 * Copyright 2000 Przemyslaw Dobrowolski <dobrawka@asua.org.pl>
 *
 * TODO: OVERLAPPED results!!!!
 *
 * PD: All my work for Odin I dedicate to my deceased mother.
 *
 */
#include <odin.h>
#include <os2win.h>
#include <misc.h>
#include "HMDevice.h"
#include "HMFile.h"
#include "HMNPipe.h"
#include "oslibdos.h"
#include <string.h>

#define DBG_LOCALLOG	DBG_hmnpipe
#include "dbglocal.h"

#undef DEBUG_LOCAL
//#define DEBUG_LOCAL

#ifdef DEBUG_LOCAL
#  define dprintfl(a) dprintf(a)
#else
inline void ignore_dprintf(...){}
#  define dprintfl(a) ignore_dprintf(a)
#endif

//******************************************************************************
//******************************************************************************
BOOL HMDeviceNamedPipeClass::PeekNamedPipe(PHMHANDLEDATA pHMHandleData,
                                      LPVOID lpvBuffer,
                                      DWORD   cbBuffer,
                                      LPDWORD lpcbRead,
                                      LPDWORD lpcbAvail,
                                      LPDWORD lpcbMessage)
{
  dprintfl(("KERNEL32: HMDeviceNamedPipeClass::PeekNamedPipe %s(%08x)\n",
           lpHMDeviceName, pHMHandleData));

  return OSLibDosPeekNamedPipe(pHMHandleData->hHMHandle,
                            lpvBuffer,
                            cbBuffer,
                            lpcbRead,
                            lpcbAvail,
                            lpcbMessage);
}

//******************************************************************************
//******************************************************************************
DWORD HMDeviceNamedPipeClass::CreateNamedPipe(PHMHANDLEDATA pHMHandleData,
                                         LPCTSTR lpName, 
                                         DWORD  dwOpenMode, 
                                         DWORD  dwPipeMode,
                                         DWORD  nMaxInstances, 
                                         DWORD  nOutBufferSize,
                                         DWORD  nInBufferSize,  
                                         DWORD  nDefaultTimeOut,
                                         LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{ 
  pHMHandleData->dwInternalType = HMTYPE_PIPE;

  dprintfl(("KERNEL32: HMDeviceNamedPipeClass::CreateNamedPipe %s\n",
           lpName));


  pHMHandleData->hHMHandle = OSLibDosCreateNamedPipe( lpName,
                                                  dwOpenMode,
                                                  dwPipeMode, 
                                                  nMaxInstances,
                                                  nOutBufferSize,
                                                  nInBufferSize,
                                                  nDefaultTimeOut,
                                                  lpSecurityAttributes );


  return (pHMHandleData->hHMHandle);
}
/*****************************************************************************
 * Name      : DWORD HMDeviceNamedPipeClass::CreateFile
 * Purpose   : this is called from the handle manager if a CreateFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMDeviceNamedPipeClass::CreateFile (LPCSTR        lpFileName,
                                          PHMHANDLEDATA pHMHandleData,
                                          PVOID         lpSecurityAttributes,
                                          PHMHANDLEDATA pHMHandleDataTemplate)
{
   pHMHandleData->hHMHandle = OSLibDosOpenPipe(lpFileName, 
                                               pHMHandleData->dwAccess,
                                               pHMHandleData->dwShare,
                                               (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
                                               pHMHandleData->dwCreation,
                                               pHMHandleData->dwFlags);
   if(pHMHandleData->hHMHandle == -1) {
      return GetLastError();
   }
   return ERROR_SUCCESS;
}

/*****************************************************************************
 * Name      : DWORD HMDeviceNamedPipeClass::CloseHandle
 * Purpose   : close the handle
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

BOOL HMDeviceNamedPipeClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("KERNEL32: HMDeviceNamedPipeClass::CloseHandle(%08x)", pHMHandleData->hHMHandle));

  return OSLibDosClose(pHMHandleData->hHMHandle);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceNamedPipeClass::CreatePipe(PHMHANDLEDATA pHMHandleDataRead,
                                         PHMHANDLEDATA pHMHandleDataWrite,
                                         LPSECURITY_ATTRIBUTES lpsa, 
                                         DWORD         cbPipe)
{ 
  pHMHandleDataRead->dwInternalType  = HMTYPE_PIPE;
  pHMHandleDataWrite->dwInternalType = HMTYPE_PIPE;

  dprintfl(("KERNEL32: HMDeviceNamedPipeClass::CreatePipe"));

  if(!OSLibDosCreatePipe(&pHMHandleDataRead->hHMHandle,
                         &pHMHandleDataWrite->hHMHandle,
                         lpsa,
                         cbPipe))
    return TRUE;
  else
    return(FALSE);
}


//******************************************************************************
//******************************************************************************
BOOL HMDeviceNamedPipeClass::ConnectNamedPipe( PHMHANDLEDATA pHMHandleData, 
                                          LPOVERLAPPED lpOverlapped)
{
  dprintfl(("KERNEL32: HMDeviceNamedPipeClass::ConnectNamedPipe %s(%08x) [%08x]\n",
           lpHMDeviceName, pHMHandleData,lpOverlapped));

  return OSLibDosConnectNamedPipe(pHMHandleData->hHMHandle,lpOverlapped);
}

//******************************************************************************
//******************************************************************************
BOOL HMDeviceNamedPipeClass::DisconnectNamedPipe(PHMHANDLEDATA pHMHandleData)
{
  dprintfl(("KERNEL32: HMDeviceNamedPipeClass::DisconnectNamedPipe %s(%08x)\n",
           lpHMDeviceName, pHMHandleData));
  
  return OSLibDosDisconnectNamedPipe(pHMHandleData->hHMHandle);
}

//******************************************************************************
//******************************************************************************
BOOL HMDeviceNamedPipeClass::GetNamedPipeHandleState(PHMHANDLEDATA pHMHandleData,
                                                LPDWORD lpState,
                                                LPDWORD lpCurInstances,
                                                LPDWORD lpMaxCollectionCount,
                                                LPDWORD lpCollectDataTimeout,
                                                LPTSTR  lpUserName,
                                                DWORD   nMaxUserNameSize)
{
  dprintf(("KERNEL32: HMDeviceNamedPipeClass::GetNamedPipeHandleStateA (%s) (%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) NIY\n",
           lpHMDeviceName,
           lpState,
           lpCurInstances,
           lpMaxCollectionCount,
           lpCollectDataTimeout,
           lpUserName,
           nMaxUserNameSize));

  return (FALSE);
}

//******************************************************************************
//******************************************************************************
BOOL HMDeviceNamedPipeClass::GetNamedPipeInfo(PHMHANDLEDATA pHMHandleData,
                                LPDWORD lpFlags,
                                LPDWORD lpOutBufferSize,
                                LPDWORD lpInBufferSize,
                                LPDWORD lpMaxInstances)
{

  dprintf(("KERNEL32: HMDeviceNamedPipeClass::GetNamedPipeInfo (%s) (%08xh,%08xh,%08xh,%08xh) NIY\n",
           lpHMDeviceName,
           lpFlags,
           lpOutBufferSize,
           lpInBufferSize,
           lpMaxInstances));

  return (FALSE);
}

//******************************************************************************
//******************************************************************************
DWORD HMDeviceNamedPipeClass::TransactNamedPipe(PHMHANDLEDATA pHMHandleData,
                                           LPVOID        lpvWriteBuf,
                                           DWORD         cbWriteBuf,
                                           LPVOID        lpvReadBuf,
                                           DWORD         cbReadBuf,
                                           LPDWORD       lpcbRead,
                                           LPOVERLAPPED  lpo)
{

  dprintfl(("KERNEL32: HMDeviceNamedPipeClass::TransactNamedPipe %s(%08x) - [%08x,%08x,%08x,%08x,%08x,%08x]\n",
            lpHMDeviceName, pHMHandleData,lpvWriteBuf,cbWriteBuf,lpvReadBuf,cbReadBuf,lpcbRead,lpo));

  return(OSLibDosTransactNamedPipe( pHMHandleData->hHMHandle,
                                    lpvWriteBuf,
                                    cbWriteBuf,
                                    lpvReadBuf,
                                    cbReadBuf,
                                    lpcbRead,
                                    lpo));
}

BOOL HMDeviceNamedPipeClass::SetNamedPipeHandleState(PHMHANDLEDATA pHMHandleData,
                                                LPDWORD lpdwMode,
                                                LPDWORD lpcbMaxCollect,
                                                LPDWORD lpdwCollectDataTimeout)
{
  BOOL ret;

  dprintf(("KERNEL32: HMDeviceNamedPipeClass::SetNamedPipeInfo (%s) (%08xh,%08xh,%08xh)",
           lpdwMode,lpcbMaxCollect,lpdwCollectDataTimeout));

  if(lpdwMode) {
     ret = OSLibSetNamedPipeState(pHMHandleData->hHMHandle, *lpdwMode);
  }
  if(lpcbMaxCollect || lpdwCollectDataTimeout) {
     dprintf(("WARNING: Not supported -> lpcbMaxCollect & lpdwCollectDataTimeout"));
  }
  return ret;
}


BOOL HMDeviceNamedPipeClass::GetOverlappedResult(PHMHANDLEDATA pHMHandleData,
                                                 LPOVERLAPPED  arg2,
                                                 LPDWORD       arg3,
                                                 BOOL          arg4)
{
   return (FALSE); 
}
