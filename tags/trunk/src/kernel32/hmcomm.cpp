/* $Id: hmcomm.cpp,v 1.4 2000-02-16 14:23:58 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 COM device access class
 *
 * 1999 Achim Hasenmueller <achimha@innotek.de>
 *
 */

#include <odin.h>
#include <win32type.h>
#include <misc.h>
#include "handlemanager.h"
#include "hmdevice.h"
#include "hmcomm.h"
#include "oslibdos.h"

#define DBG_LOCALLOG	DBG_hmcomm
#include "dbglocal.h"


HMDeviceCommClass::HMDeviceCommClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName)
{
  dprintf(("HMDeviceCommClass: Register COM1 to COM4 with Handle Manager\n"));
  HMDeviceRegister("COM1", this);
  HMDeviceRegister("COM2", this);
  HMDeviceRegister("COM3", this);
  HMDeviceRegister("COM4", this);
  HMDeviceRegister("COM5", this);
  HMDeviceRegister("COM6", this);
  HMDeviceRegister("COM7", this);
  HMDeviceRegister("COM8", this);
}



DWORD HMDeviceCommClass::CreateFile(LPCSTR lpFileName,
                                    PHMHANDLEDATA pHMHandleData,
                                    PVOID lpSecurityAttributes,
                                    PHMHANDLEDATA pHMHandleDataTemplate)
{
  dprintf(("HMComm: Serial communication port %s open request\n", lpFileName));

  pHMHandleData->hHMHandle = 0;

  //AH: TODO parse Win32 security handles   
  OSLibDosDisableHardError(TRUE);
  pHMHandleData->hHMHandle = OSLibDosOpen((char*)lpFileName,
                                          OSLIB_ACCESS_READWRITE |
                                          OSLIB_ACCESS_SHAREDENYREAD |
                                          OSLIB_ACCESS_SHAREDENYWRITE);
  OSLibDosDisableHardError(FALSE);
  if (pHMHandleData->hHMHandle != 0)
    return 0;
  else
    return  -1;
}


                      /* this is a handler method for calls to CloseHandle() */
DWORD HMDeviceCommClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("HMComm: Serial communication port close request\n"));
  return OSLibDosClose(pHMHandleData->hHMHandle);
}

/*****************************************************************************
 * Name      : DWORD HMDeviceHandler::SetupComm      
 * Purpose   : set com port parameters (queue)
 * Variables :
 * Result    : 
 * Remark    :
 * Status    :
 *
 * Author    : Achim Hasenmueller
 *****************************************************************************/

BOOL HMDeviceCommClass::SetupComm(PHMHANDLEDATA pHMHandleData, DWORD dwInQueue, DWORD dwOutQueue)
{
  dprintf(("HMDeviceCommClass::SetupComm unimplemented stub!"));


  return(TRUE);
}
