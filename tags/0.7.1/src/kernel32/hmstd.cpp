/* $Id: hmstd.cpp,v 1.12 2002-06-30 09:57:13 sandervl Exp $ */

/*
 * Handle Manager class for standard in, out & error handles
 *
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
#include <stdio.h>
#include <string.h>
#include <unicode.h>
#include <dbglog.h>

#include "HandleManager.H"
#include "hmstd.h"
#include "winexebase.h"

#define DBG_LOCALLOG	DBG_hmstd
#include "dbglocal.h"

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
 * Name      : BOOL HMDeviceStandardClass::ReadFile
 * Purpose   : read data from handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToRead,
 *             LPDWORD       lpNumberOfBytesRead,
 *             LPOVERLAPPED  lpOverlapped
 * Variables :
 * Result    : Boolean
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMDeviceStandardClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                 LPCVOID       lpBuffer,
                                 DWORD         nNumberOfBytesToRead,
                                 LPDWORD       lpNumberOfBytesRead,
                                 LPOVERLAPPED  lpOverlapped,
                                 LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
    BOOL         bRC;
    DWORD        bytesread;

    dprintf2(("KERNEL32: HMDeviceStandardClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x) - NOT IMPLEMENTED\n",
              lpHMDeviceName, pHMHandleData, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped));

    if(lpCompletionRoutine) {
        dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
    }
    
    if(lpNumberOfBytesRead == NULL) {
        lpNumberOfBytesRead = &bytesread;
    }
    if(pHMHandleData->dwUserData != STD_INPUT_HANDLE) {
        return FALSE;
    }
    return O32_ReadFile(pHMHandleData->hHMHandle, (LPVOID)lpBuffer, nNumberOfBytesToRead,
                        lpNumberOfBytesRead, lpOverlapped);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceStandardClass::WriteFile
 * Purpose   : write data to handle / device
 * Parameters: PHMHANDLEDATA pHMHandleData,
 *             LPCVOID       lpBuffer,
 *             DWORD         nNumberOfBytesToWrite,
 *             LPDWORD       lpNumberOfBytesWritten,
 *             LPOVERLAPPED  lpOverlapped
 * Variables :
 * Result    : Boolean
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/
#define CARRIAGE_RETURN 0xD
#define LINE_FEED       0xA

BOOL HMDeviceStandardClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                      LPCVOID       lpBuffer,
                                      DWORD         nNumberOfBytesToWrite,
                                      LPDWORD       lpNumberOfBytesWritten,
                                      LPOVERLAPPED  lpOverlapped,
                                      LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
    DWORD  byteswritten;
    LPVOID lpLowMemBuffer;

    dprintf(("KERNEL32: HMDeviceStandardClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x)",
             lpHMDeviceName, pHMHandleData, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten,
             lpOverlapped));

    if(lpNumberOfBytesWritten == NULL)
        lpNumberOfBytesWritten = &byteswritten;

    if(lpCompletionRoutine)
    {
        dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
    }

    if(pHMHandleData->dwUserData == STD_INPUT_HANDLE)
        return FALSE;

    //count linefeed without carriage return occurances
    char *src, *dest;
    int  i         = 0;
    int  missingCR = 0;
    char prevchar  = 0;

    src = (char *)lpBuffer;
    while(i < nNumberOfBytesToWrite) {
        if(src[i] == LINE_FEED && prevchar != CARRIAGE_RETURN) {
            missingCR++;
        }
        prevchar = src[i];
        i++;
    }

    lpLowMemBuffer = alloca(nNumberOfBytesToWrite+missingCR);
    if(lpLowMemBuffer == NULL)
    {
        DebugInt3();
        return FALSE;
    }

    //convert linefeed without carriage return into LF+CR
    i         = 0;
    missingCR = 0;
    prevchar  = 0;
    src       = (char *)lpBuffer;
    dest      = (char *)lpLowMemBuffer;

    while(i < nNumberOfBytesToWrite) {
        if(src[i] == LINE_FEED && prevchar != CARRIAGE_RETURN) {
            dest[i+missingCR] = CARRIAGE_RETURN;
            missingCR++;
            dest[i+missingCR] = LINE_FEED;
        }
        else {
            dest[i+missingCR] = src[i];
        }
        prevchar = src[i];
        i++;
    }
    nNumberOfBytesToWrite += missingCR;

    if(WinExe && !WinExe->isConsoleApp() && O32_GetFileType(pHMHandleData->hHMHandle) == FILE_TYPE_UNKNOWN) /* kso */
    {
        //DosWrite returns error 436 when PM apps try to write to std out
        //kso - Jun 23 2002 2:54am:
        //Yeah, cause PM programs doesn't have working STD* handles unless you redirect them!
        //So, we should rather check if valid handle than !console.
        dprintf(("%s (GUI): %.*s", pHMHandleData->dwUserData == STD_ERROR_HANDLE ? "STDERR" : "STDOUT",
                 nNumberOfBytesToWrite, lpLowMemBuffer));
        return TRUE;
    }

    dprintf(("%s: %.*s", pHMHandleData->dwUserData == STD_ERROR_HANDLE ? "STDERR" : "STDOUT",
             nNumberOfBytesToWrite, lpLowMemBuffer));


    if(!O32_WriteFile(pHMHandleData->hHMHandle, lpLowMemBuffer, nNumberOfBytesToWrite,
                      lpNumberOfBytesWritten, lpOverlapped))
    {
        dprintf(("STD*: failed with lasterror=%d\n", GetLastError()));
        return FALSE;
    }
    return TRUE;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceStandardClass::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceStandardClass::GetFileType(PHMHANDLEDATA pHMHandleData)
{
  dprintf2(("KERNEL32: HMDeviceStandardClass::GetFileType %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData));
  #if 0
  return FILE_TYPE_CHAR;
  #else
  return O32_GetFileType(pHMHandleData->hHMHandle);
  #endif
}
