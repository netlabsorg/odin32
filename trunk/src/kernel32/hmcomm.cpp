/* $Id: hmcomm.cpp,v 1.41 2004-01-30 22:16:59 bird Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 COM device access class
 *
 * 1999 Achim Hasenmueller <achimha@innotek.de>
 * 2001 Sander van Leeuwen <sandervl@xs4all.nl>
 *
 *
 */



#include <os2win.h>
#include <string.h>
#include <stdio.h>
#include <handlemanager.h>
#include "handlenames.h"
#include <heapstring.h>
#include "hmcomm.h"
#include "oslibdos.h"

#define DBG_LOCALLOG  DBG_hmcomm
#include "dbglocal.h"

//#define TESTING_COMM
#ifdef TESTING_COMM
#undef dprintf
#define dprintf(a) WriteLog a
#undef dprintf2
#define dprintf2(a) WriteLog a
#endif

BAUDTABLEENTRY BaudTable[] =
{
  {75,BAUD_075},
  {110,BAUD_110},
  {134,BAUD_134_5},
  {150,BAUD_150},
  {300,BAUD_300},
  {600,BAUD_600},
  {1200,BAUD_1200},
  {1800,BAUD_1800},
  {2400,BAUD_2400},
  {4800,BAUD_4800},
  {7200,BAUD_7200},
  {9600,BAUD_9600},
  {14400,BAUD_14400},
  {19200,BAUD_19200},
  {38400,BAUD_38400},
  {56000,BAUD_56K},
  {57600,BAUD_57600},
  {115200,BAUD_115200},
  {128000,BAUD_128K}
};

#define BaudTableSize (sizeof(BaudTable)/sizeof(BAUDTABLEENTRY))

static DWORD CommReadIOHandler(LPASYNCIOREQUEST lpRequest, DWORD *lpdwResult, DWORD *lpdwTimeOut);
static DWORD CommWriteIOHandler(LPASYNCIOREQUEST lpRequest, DWORD *lpdwResult, DWORD *lpdwTimeOut);
static DWORD CommPollIOHandler(LPASYNCIOREQUEST lpRequest, DWORD *lpdwResult, DWORD *lpdwTimeOut);
#ifdef DEBUG_LOGGING
static char *DebugCommEvent(DWORD dwEvents);
static char *DebugModemStatus(DWORD dwModemStatus);
#ifdef DEBUG_COMOUTPUT
static FILE *comlog = NULL;
#endif
#endif
//******************************************************************************
//******************************************************************************
static VOID *CreateDevData()
{
    PHMDEVCOMDATA pData;

    pData = new HMDEVCOMDATA();
    if(NULL!=pData)
    {
        memset(pData,0,sizeof(HMDEVCOMDATA));
        pData->ulMagic = MAGIC_COM;
        pData->CommCfg.dwSize   = sizeof(COMMCONFIG);
        pData->CommCfg.wVersion = 1;
        pData->CommCfg.dwProviderSubType = PST_RS232;
        pData->CommCfg.dcb.DCBlength = sizeof(DCB);
        pData->CommCfg.dcb.BaudRate  = CBR_1200;
        pData->CommCfg.dcb.ByteSize = 8;
        pData->CommCfg.dcb.Parity   = NOPARITY;
        pData->CommCfg.dcb.StopBits = ONESTOPBIT;
        pData->dwInBuffer  = 16;
        pData->dwOutBuffer = 16;
    }
    return pData;
}
//******************************************************************************
//******************************************************************************
HMDeviceCommClass::HMDeviceCommClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName)
{
    VOID *pData;

    dprintf(("HMDeviceCommClass: Register COM1 to COM8 with Handle Manager\n"));
    pData = CreateDevData();
    if(pData!= NULL)
        HMDeviceRegisterEx("COM1", this, pData);

    // add symbolic links to the "real name" of the device
    // @@@PH what's the long device name: SerialPortx ?
    // HandleNamesAddSymbolicLink("\\Device\\ParallelPort3", "COM3");
    // Note: \\.\COMx: is invalid (NT4SP6)

    PSZ pszCOM = strdup("\\\\.\\COMx");
    for (char ch = '1'; ch < '9'; ch++)
    {
        pszCOM[7] = ch;
        HandleNamesAddSymbolicLink(pszCOM, pszCOM+4);
    }
    free(pszCOM);

    // add "AUX" device
    HandleNamesAddSymbolicLink("AUX",        "COM1");
    HandleNamesAddSymbolicLink("AUX:",       "COM1");
    HandleNamesAddSymbolicLink("\\\\.\\AUX", "COM1");
#ifdef DEBUG_COMOUTPUT
    comlog = fopen("comlog", "w");
#endif
}

/*****************************************************************************
 * Name      : HMDeviceCommClass::FindDevice
 * Purpose   : Checks if lpDeviceName belongs to this device class
 * Parameters: LPCSTR lpClassDevName
 *             LPCSTR lpDeviceName
 *             int namelength
 * Variables :
 * Result    : checks if name is COMx or COMx: (x=1..8)
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceCommClass::FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength)
{
    dprintf2(("HMDeviceCommClass::FindDevice %s %s", lpClassDevName, lpDeviceName));

    //first 3 letters 'COM'?
    if(lstrncmpiA(lpDeviceName, lpClassDevName, 3) != 0) {
        return FALSE;
    }

    if(namelength == 5 && lpDeviceName[4] != ':') {
        return FALSE;
    }
    switch(lpDeviceName[3]) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
        return TRUE;    //we support up to COM8
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
DWORD HMDeviceCommClass::CreateFile(LPCSTR lpFileName,
                                    PHMHANDLEDATA pHMHandleData,
                                    PVOID lpSecurityAttributes,
                                    PHMHANDLEDATA pHMHandleDataTemplate)
{
 char  comname[6];
 DWORD ret = ERROR_SUCCESS;

    dprintf(("HMComm: Serial communication port %s open request\n", lpFileName));

    if(strlen(lpFileName) > 5) {
        lpFileName += 4; //skip prefix
    }

    pHMHandleData->hHMHandle = 0;

    strcpy(comname, lpFileName);
    comname[4] = 0;   //get rid of : (if present) (eg COM1:)

    //AH: TODO parse Win32 security handles
    ULONG oldmode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
    pHMHandleData->hHMHandle = OSLibDosOpen(comname,
                                            OSLIB_ACCESS_READWRITE |
                                            OSLIB_ACCESS_SHAREDENYREAD |
                                            OSLIB_ACCESS_SHAREDENYWRITE);
    ::SetErrorMode(oldmode);
    if (pHMHandleData->hHMHandle != 0)
    {
        ULONG ulLen;
        APIRET rc;
        pHMHandleData->lpHandlerData = new HMDEVCOMDATA();
        // Init The handle instance with the default default device config
        memcpy( pHMHandleData->lpHandlerData,
                pHMHandleData->lpDeviceData,
                sizeof(HMDEVCOMDATA));

        ulLen = sizeof(DCBINFO);

        rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                               IOCTL_ASYNC,
                               ASYNC_GETDCBINFO,
                               0,0,0,
                               &((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2,ulLen,&ulLen);
        dprintf(("DCB Of %s :\n"
                 " WriteTimeout           : %d\n"
                 " ReadTimeout            : %d\n"
                 " CtlHandshake           : 0x%x\n"
                 " FlowReplace            : 0x%x\n"
                 " Timeout                : 0x%x\n"
                 " Error replacement Char : 0x%x\n"
                 " Break replacement Char : 0x%x\n"
                 " XON Char               : 0x%x\n"
                 " XOFF Char              : 0x%x\n",
                 comname,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.usWriteTimeout,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.usReadTimeout,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.fbCtlHndShake,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.fbFlowReplace,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.fbTimeOut,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.bErrorReplacementChar,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.bBreakReplacementChar,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.bXONChar,
                 ((PHMDEVCOMDATA)pHMHandleData->lpHandlerData)->dcbOS2.bXOFFChar));

        if(rc)
        {
            OSLibDosClose(pHMHandleData->hHMHandle);
            delete pHMHandleData->lpHandlerData;
            return rc;
        }

        rc = SetBaud(pHMHandleData,9600);
        dprintf(("Init Baud to 9600 rc = %d",rc));
        rc = SetLine(pHMHandleData,8,0,0);
        dprintf(("Set Line to 8/N/1 rc = %d",rc));

        if(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED)
        {
            PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
            int           comnr;

            comnr = comname[3] - '1';

            if(handler[comnr] == NULL)
            {
                try {
                    handler[comnr] = new OverlappedIOHandler(CommReadIOHandler, CommWriteIOHandler, CommPollIOHandler, ASYNC_TYPE_FULLDUPLEX);
                }
                catch(...)
                {
                    ret = ERROR_NOT_ENOUGH_MEMORY;
                    goto fail;
                }
                if(handler[comnr] == NULL) {
                    ret = ERROR_NOT_ENOUGH_MEMORY;
                    goto fail;
                }
                handler[comnr]->AddRef();
            }
            pDevData->iohandler = handler[comnr];
        }
        return ERROR_SUCCESS;
    }
    else
        return ERROR_ACCESS_DENIED;

fail:

    delete pHMHandleData->lpHandlerData;
    OSLibDosClose(pHMHandleData->hHMHandle);
    return ret;
}
/*****************************************************************************
 * Name      : DWORD HMDeviceCommClass::GetFileType
 * Purpose   : determine the handle type
 * Parameters: PHMHANDLEDATA pHMHandleData
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMDeviceCommClass::GetFileType(PHMHANDLEDATA pHMHandleData)
{
    dprintf(("KERNEL32: HMDeviceCommClass::GetFileType %s(%08x)\n",
             lpHMDeviceName, pHMHandleData));

    return FILE_TYPE_CHAR;
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
    PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
    dprintf(("HMComm: Serial communication port close request"));

    delete pHMHandleData->lpHandlerData;
    return OSLibDosClose(pHMHandleData->hHMHandle);
}
//******************************************************************************
//Overlapped read handler
//******************************************************************************
DWORD CommReadIOHandler(LPASYNCIOREQUEST lpRequest, DWORD *lpdwResult, DWORD *lpdwTimeOut)
{
    PHMHANDLEDATA pHMHandleData;
    BOOL          ret;
    ULONG         ulBytesRead, len;

    pHMHandleData = HMQueryHandleData(lpRequest->hHandle);
    if(pHMHandleData == NULL) {
        dprintf(("!ERROR!: CommReadIOHandler handle %x not valid", lpRequest->hHandle));
        DebugInt3();
        return ERROR_INVALID_HANDLE;
    }

#ifdef DEBUG
    RXQUEUE qInfo;
    ULONG ulLen = sizeof(qInfo);
    ULONG rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                                IOCTL_ASYNC,
                                ASYNC_GETINQUECOUNT,
                                0,0,0,
                                &qInfo,ulLen,&ulLen);
    dprintf(("ASYNC_GETINQUECOUNT -> qInfo.cch %d (queue size %d) rc %d", qInfo.cch, qInfo.cb, rc));
#endif

    ret = OSLibDosRead(pHMHandleData->hHMHandle, (LPVOID)lpRequest->lpBuffer, lpRequest->nNumberOfBytes, &ulBytesRead);

    *lpdwResult = (ret) ? ulBytesRead : 0;
    dprintf(("KERNEL32: CommReadIOHandler %d bytes read", *lpdwResult));

    if(ret == FALSE) {
        dprintf(("!ERROR!: CommReadIOHandler failed with rc %d", GetLastError()));
    }
    else {
#ifdef DEBUG
        if(ulBytesRead < qInfo.cch) {
            dprintf(("WARNING: Not all data removed from queue!!"));
        }
        dprintf2(("%d Bytes read:", ulBytesRead));
        for(int i=0;i<min(ulBytesRead, 16);i++) {
            dprintf2(("%x %c", ((char *)lpRequest->lpBuffer)[i], ((char *)lpRequest->lpBuffer)[i]));
        }
#endif
    }
    return GetLastError();
}
//******************************************************************************
//Overlapped write handler
//******************************************************************************
DWORD CommWriteIOHandler(LPASYNCIOREQUEST lpRequest, DWORD *lpdwResult, DWORD *lpdwTimeOut)
{
    PHMHANDLEDATA pHMHandleData;
    BOOL          ret;
    ULONG         ulBytesWritten;

    pHMHandleData = HMQueryHandleData(lpRequest->hHandle);
    if(pHMHandleData == NULL) {
        dprintf(("!ERROR!: CommWriteIOHandler handle %x not valid", lpRequest->hHandle));
        DebugInt3();
        return ERROR_INVALID_HANDLE;
    }

#ifdef DEBUG
    dprintf2(("Bytes to write:"));
    for(int i=0;i<min(lpRequest->nNumberOfBytes, 16);i++) {
        dprintf2(("%x %c", ((char *)lpRequest->lpBuffer)[i], ((char *)lpRequest->lpBuffer)[i]));
    }
#endif

    ret = OSLibDosWrite(pHMHandleData->hHMHandle, (LPVOID)lpRequest->lpBuffer, lpRequest->nNumberOfBytes,
                        &ulBytesWritten);

    *lpdwResult = (ret) ? ulBytesWritten : 0;
    dprintf2(("KERNEL32:CommWriteIOHandler %d byte(s) written", *lpdwResult));

    if(ulBytesWritten != lpRequest->nNumberOfBytes) {
        dprintf(("WARNING: ulBytesWritten (%d) != lpRequest->nNumberOfBytes (%d)", ulBytesWritten, lpRequest->nNumberOfBytes));
    }
    if(ret == FALSE) {
       dprintf(("!ERROR!: CommWriteIOHandler failed with rc %d", GetLastError()));
    }
    return GetLastError();
}
//******************************************************************************
//Overlapped WaitCommEvent handler
//******************************************************************************
DWORD CommPollIOHandler(LPASYNCIOREQUEST lpRequest, DWORD *lpdwResult, DWORD *lpdwTimeOut)
{
    APIRET rc;
    ULONG ulLen;
    USHORT COMEvt;
    DWORD dwEvent,dwMask;
    PHMHANDLEDATA pHMHandleData;
    PHMDEVCOMDATA pDevData;

    pHMHandleData = HMQueryHandleData(lpRequest->hHandle);
    if(pHMHandleData == NULL || pHMHandleData->lpHandlerData == NULL) {
        dprintf(("!ERROR!: CommWriteIOHandler handle %x not valid", lpRequest->hHandle));
        DebugInt3();
        return ERROR_INVALID_HANDLE;
    }

    pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

////    dprintf(("CommPollIOHandler %x %x", pHMHandleData->hHMHandle, lpRequest->dwEventMask));

    *lpdwTimeOut = TIMEOUT_COMM;

    ulLen   = sizeof(CHAR);
    dwEvent = 0;
    rc      = 0;
    ulLen   = sizeof(COMEvt);
    dwMask  = lpRequest->dwEventMask;

    if(lpRequest->dwEventMask != pDevData->dwEventMask) {
        dprintf(("!WARNING!: CommPollIOHandler: operation aborted (event mask changed)"));
        return ERROR_OPERATION_ABORTED; // Exit if the Mask gets changed
    }
    rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                          IOCTL_ASYNC,
                          ASYNC_GETCOMMEVENT,
                          0,0,0,
                          &COMEvt,ulLen,&ulLen);
    if(!rc)
    {
#ifdef DEBUG
        if(COMEvt) dprintf(("ASYNC_GETCOMMEVENT %x", COMEvt));
#endif
        dwEvent |= (COMEvt&0x0001)? EV_RXCHAR:0;
        //dwEvent |= (COMEvt&0x0002)? 0:0;
        dwEvent |= (COMEvt&0x0004)? EV_TXEMPTY:0;
        dwEvent |= (COMEvt&0x0008)? EV_CTS:0;
        dwEvent |= (COMEvt&0x0010)? EV_DSR:0;
        dwEvent |= (COMEvt&0x0020)? EV_RLSD:0;
        dwEvent |= (COMEvt&0x0040)? EV_BREAK:0;
        dwEvent |= (COMEvt&0x0080)? EV_ERR:0;
        dwEvent |= (COMEvt&0x0100)? EV_RING:0;

#ifdef DEBUG
        if(dwEvent & EV_ERR) {
            ULONG COMErr = 0;

            rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_GETCOMMERROR,
                    0,0,0,
                    &COMErr,2,&ulLen);

            dprintf(("!!!!!!!----> ERROR OCCURRED (EV_ERR) reason %x <-----!!!!!", COMErr));

        }
#endif
        if((dwEvent & EV_RXCHAR) && (dwMask & EV_RXCHAR))
        {
            //check if there's really data in the in queue
            RXQUEUE qInfo;
            ULONG ulLen = sizeof(qInfo);
            ULONG rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                                        IOCTL_ASYNC,
                                        ASYNC_GETINQUECOUNT,
                                        0,0,0,
                                        &qInfo,ulLen,&ulLen);
            if(qInfo.cch == 0) {
                dprintf(("!WARNING!: CommPollIOHandler -> EV_RXCHAR but no DATA"));
                dwEvent &= ~EV_RXCHAR;
            }
        }
        if((dwEvent & dwMask)) {
            dprintf(("CommPollIOHandler: event(s) %s %x occured", DebugCommEvent((dwEvent & dwMask)), (dwEvent & dwMask)));
            *lpdwResult = (dwEvent & dwMask);
            return ERROR_SUCCESS;
        }
    }
    else {
        dprintf(("!ERROR!: CommPollIOHandler: OSLibDosDevIOCtl failed with rc %d", rc));
        *lpdwResult = 0;
        return ERROR_OPERATION_ABORTED;
    }
    return ERROR_IO_PENDING;
}
/*****************************************************************************
 * Name      : BOOL HMDeviceCommClass::WriteFile
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
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceCommClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                  LPCVOID       lpBuffer,
                                  DWORD         nNumberOfBytesToWrite,
                                  LPDWORD       lpNumberOfBytesWritten,
                                  LPOVERLAPPED  lpOverlapped,
                                  LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
    PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
    BOOL  ret;
    ULONG ulBytesWritten;

    dprintf(("KERNEL32:HMDeviceCommClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x)",
             lpHMDeviceName, pHMHandleData->hHMHandle, lpBuffer, nNumberOfBytesToWrite,
             lpNumberOfBytesWritten, lpOverlapped));


    if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
    {
        ::SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
        dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && lpOverlapped) {
        dprintf(("Warning: lpOverlapped != NULL & !FILE_FLAG_OVERLAPPED; sync operation"));
    }
    if(lpCompletionRoutine) {
        dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
    }

#ifdef DEBUG
    dprintf2(("WriteFile: bytes to write:"));
    for(int i=0;i<min(nNumberOfBytesToWrite, 16);i++) {
        dprintf2(("%x %c", ((char *)lpBuffer)[i], ((char *)lpBuffer)[i]));
    }
#endif

    if(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) {
        return pDevData->iohandler->WriteFile(pHMHandleData->hWin32Handle, lpBuffer, nNumberOfBytesToWrite,
                                              lpNumberOfBytesWritten, lpOverlapped, lpCompletionRoutine, (DWORD)pDevData);
    }

    ret = OSLibDosWrite(pHMHandleData->hHMHandle, (LPVOID)lpBuffer, nNumberOfBytesToWrite,
                        &ulBytesWritten);

    if(lpNumberOfBytesWritten) {
       *lpNumberOfBytesWritten = (ret) ? ulBytesWritten : 0;
       dprintf2(("KERNEL32:HMDeviceCommClass::WriteFile %d byte(s) written", *lpNumberOfBytesWritten));
    }
    if(ret == FALSE) {
       dprintf(("!ERROR!: WriteFile failed with rc %d", GetLastError()));
    }

    return ret;
}
/*****************************************************************************
 * Name      : BOOL HMDeviceCommClass::ReadFile
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
 * Author    : SvL
 *****************************************************************************/

BOOL HMDeviceCommClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                 LPCVOID       lpBuffer,
                                 DWORD         nNumberOfBytesToRead,
                                 LPDWORD       lpNumberOfBytesRead,
                                 LPOVERLAPPED  lpOverlapped,
                                 LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
    PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
    BOOL  ret;
    ULONG ulBytesRead;

    dprintf(("KERNEL32:HMDeviceCommClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)",
             lpHMDeviceName, pHMHandleData->hHMHandle, lpBuffer, nNumberOfBytesToRead,
             lpNumberOfBytesRead, lpOverlapped));

    if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
    {
        ::SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
        dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && lpOverlapped) {
        dprintf(("!WARNING!: lpOverlapped != NULL & !FILE_FLAG_OVERLAPPED; sync operation"));
    }

    if(lpCompletionRoutine) {
        dprintf(("!WARNING!: lpCompletionRoutine not supported -> fall back to sync IO"));
    }

    RXQUEUE qInfo;
    ULONG ulLen = sizeof(qInfo);
    ULONG rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle, IOCTL_ASYNC, ASYNC_GETINQUECOUNT, 0,0,0, &qInfo,ulLen,&ulLen);
    dprintf(("ASYNC_GETINQUECOUNT -> qInfo.cch %d (queue size %d) rc %d", qInfo.cch, qInfo.cb, rc));

    if(!qInfo.cch && pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) {
        return pDevData->iohandler->ReadFile(pHMHandleData->hWin32Handle, lpBuffer, nNumberOfBytesToRead,
                                             lpNumberOfBytesRead, lpOverlapped, lpCompletionRoutine, (DWORD)pDevData);
    }

    ret = OSLibDosRead(pHMHandleData->hHMHandle, (LPVOID)lpBuffer, nNumberOfBytesToRead,
                       &ulBytesRead);

    if(lpNumberOfBytesRead) {
        *lpNumberOfBytesRead = (ret) ? ulBytesRead : 0;
        dprintf2(("KERNEL32:HMDeviceCommClass::ReadFile %d bytes read", *lpNumberOfBytesRead));
        if(qInfo.cch > ulBytesRead) {
            dprintf(("Warning: more bytes available!!"));
        }
    }
    if(ret == FALSE) {
        dprintf(("!ERROR!: ReadFile failed with rc %d", GetLastError()));
    }
#ifdef DEBUG
    else {
        int i;
        dprintf2(("%d Bytes read:", ulBytesRead));
#ifdef DEBUG_COMOUTPUT
        char *tmp = (char *)malloc(ulBytesRead+1);
        memcpy(tmp, lpBuffer, ulBytesRead);
        tmp[ulBytesRead] = 0;
        dprintf2(("RF: %s", tmp));
        WORD sel = RestoreOS2FS();
        for(i=0;i<ulBytesRead;i++) {
            fprintf(comlog, "%c", tmp[i]);
        }
        SetFS(sel);
        free(tmp);
#endif
        for(i=0;i<ulBytesRead;i++) {
            dprintf2(("%x %c", ((char *)lpBuffer)[i], ((char *)lpBuffer)[i]));
        }
    }
#endif

    if(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) {
        //reset overlapped semaphore to non-signalled
        ::ResetEvent(lpOverlapped->hEvent);

        //set event to make sure next GetOverlappedResult doesn't block
        ::SetEvent(lpOverlapped->hEvent);

        lpOverlapped->Internal     = GetLastError();
        lpOverlapped->InternalHigh = ulBytesRead;
    }
    return ret;
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

BOOL HMDeviceCommClass::SetupComm( PHMHANDLEDATA pHMHandleData,
                                   DWORD dwInQueue,
                                   DWORD dwOutQueue)
{
    PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

    dprintf(("HMDeviceCommClass::SetupComm "));
    if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
    {
        ::SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    pDevData->dwInBuffer  = dwInQueue;
    pDevData->dwOutBuffer = dwOutQueue;

    return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::WaitCommEvent( PHMHANDLEDATA pHMHandleData,
                                       LPDWORD lpfdwEvtMask,
                                       LPOVERLAPPED lpo)
{
    APIRET rc;
    ULONG ulLen;
    USHORT COMEvt;
    DWORD dwEvent,dwMask;

    PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

    dprintf(("HMDeviceCommClass::WaitCommEvent %x %x %x", pHMHandleData->hHMHandle, lpfdwEvtMask, lpo));

    if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpo) {
        dprintf(("!WARNING! pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpo"));
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED)
    {
        return pDevData->iohandler->WaitForEvent(pHMHandleData->hWin32Handle, pDevData->dwEventMask, lpfdwEvtMask,
                                                 lpo, NULL, (DWORD)pDevData);
    }

    ulLen = sizeof(CHAR);

    dwEvent = 0;
    rc = 0;
    ulLen = sizeof(COMEvt);
    dwMask = pDevData->dwEventMask;
    while( (0==rc) &&
           !(dwEvent & dwMask) &&
           (dwMask ==pDevData->dwEventMask) ) // Exit if the Mask gets changed
    {
        rc = OSLibDosDevIOCtl(pHMHandleData->hHMHandle,
                              IOCTL_ASYNC,
                              ASYNC_GETCOMMEVENT,
                              0,0,0,
                              &COMEvt,ulLen,&ulLen);
        if(!rc)
        {
            dwEvent |= (COMEvt&0x0001)? EV_RXCHAR:0;
            //dwEvent |= (COMEvt&0x0002)? 0:0;
            dwEvent |= (COMEvt&0x0004)? EV_TXEMPTY:0;
            dwEvent |= (COMEvt&0x0008)? EV_CTS:0;
            dwEvent |= (COMEvt&0x0010)? EV_DSR:0;
            dwEvent |= (COMEvt&0x0020)? EV_RLSD:0;
            dwEvent |= (COMEvt&0x0040)? EV_BREAK:0;
            dwEvent |= (COMEvt&0x0080)? EV_ERR:0;
            dwEvent |= (COMEvt&0x0100)? EV_RING:0;
            if((dwEvent & dwMask)) break;
        }
        else break;

        DosSleep(TIMEOUT_COMM);
    }
    if(dwMask == pDevData->dwEventMask) {
        *lpfdwEvtMask = (rc==0) ? (dwEvent & dwMask) : 0;
        dprintf(("WaitCommEvent returned %s %x", DebugCommEvent(*lpfdwEvtMask), *lpfdwEvtMask));
    }
    else  *lpfdwEvtMask = 0;

    ::SetLastError(rc);
    return (rc==0);
}
/*****************************************************************************
 * Name      : DWORD HMDeviceCommClass::CancelIo
 * Purpose   : cancel pending IO operation
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceCommClass::CancelIo(PHMHANDLEDATA pHMHandleData)
{
    PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

    dprintf(("HMDeviceCommClass::CancelIo"));
    if(pDevData == NULL || !(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED)) {
        ::SetLastError(ERROR_ACCESS_DENIED); //todo: wrong error?
        return FALSE;
    }

    return pDevData->iohandler->CancelIo(pHMHandleData->hWin32Handle);
}
/*****************************************************************************
 * Name      : DWORD HMDeviceFileClass::GetOverlappedResult
 * Purpose   : asynchronus I/O
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             LPOVERLAPPED  arg2
 *             LPDWORD       arg3
 *             BOOL          arg4
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMDeviceCommClass::GetOverlappedResult(PHMHANDLEDATA pHMHandleData,
                                            LPOVERLAPPED  lpOverlapped,
                                            LPDWORD       lpcbTransfer,
                                            BOOL          fWait)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

    dprintf(("KERNEL32-HMDeviceCommClass: HMDeviceCommClass::GetOverlappedResult(%08xh,%08xh,%08xh,%08xh)",
             pHMHandleData->hHMHandle, lpOverlapped, lpcbTransfer, fWait));

    if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED)) {
        dprintf(("!WARNING!: GetOverlappedResult called for a handle that wasn't opened with FILE_FLAG_OVERLAPPED"));
        return TRUE; //NT4, SP6 doesn't fail
    }

    if(pDevData == NULL) {
        ::SetLastError(ERROR_ACCESS_DENIED); //todo: wrong error?
        return FALSE;
    }
    if(!lpOverlapped) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return pDevData->iohandler->GetOverlappedResult(pHMHandleData->hWin32Handle, lpOverlapped, lpcbTransfer, fWait);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::GetCommProperties(PHMHANDLEDATA pHMHandleData,
                                          LPCOMMPROP lpcmmp)
{
  EXTBAUDGET BaudInfo;
  APIRET rc;
  ULONG ulLen;
  USHORT COMErr;
  int i;
  dprintf(("HMDeviceCommClass::GetCommProperties"));

  ulLen = sizeof(EXTBAUDGET);
  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_EXTGETBAUDRATE,
                    0,0,0,
                    &BaudInfo,ulLen,&ulLen);
  memset(lpcmmp,0,sizeof(COMMPROP));
  lpcmmp->wPacketLength  = sizeof(COMMPROP);
  lpcmmp->wPacketVersion = 1; //???
  lpcmmp->dwServiceMask  = SP_SERIALCOMM;
  for(i=0;i<BaudTableSize && BaudInfo.ulMaxBaud <= BaudTable[i].dwBaudRate;i++);
  lpcmmp->dwMaxBaud      = BaudTable[i].dwBaudFlag;
  lpcmmp->dwProvSubType =  PST_RS232;
  lpcmmp->dwProvCapabilities = PCF_DTRDSR | PCF_PARITY_CHECK |
                               PCF_RTSCTS | PCF_SETXCHAR |
                               PCF_XONXOFF;
  lpcmmp->dwSettableParams   = SP_BAUD | SP_DATABITS |
                               SP_HANDSHAKEING | SP_PARITY |
                               SP_PARITY_CHECK | SP_STOPBIT;
  lpcmmp->dwSettableBaud = 0;
  for(i=0;i<BaudTableSize;i++)
  {
    if ( (BaudTable[i].dwBaudRate>=BaudInfo.ulMinBaud) &&
         (BaudTable[i].dwBaudRate<=BaudInfo.ulMaxBaud) )
      lpcmmp->dwSettableBaud |= BaudTable[i].dwBaudFlag;
  }
  lpcmmp->dwSettableBaud |= BAUD_USER;
  lpcmmp->wSettableData = DATABITS_5 | DATABITS_6 | DATABITS_7 | DATABITS_8;
  lpcmmp->wSettableStopParity = STOPBITS_10 | STOPBITS_15 | STOPBITS_20 |
                                PARITY_NONE | PARITY_ODD | PARITY_EVEN |
                                PARITY_MARK | PARITY_SPACE;
  return(rc==0);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::GetCommMask( PHMHANDLEDATA pHMHandleData,
                                     LPDWORD lpfdwEvtMask)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::GetCommMask"));

  *lpfdwEvtMask = pDevData->dwEventMask;
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::SetCommMask( PHMHANDLEDATA pHMHandleData,
                                     DWORD fdwEvtMask)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  dprintf(("HMDeviceCommClass::SetCommMask %x", fdwEvtMask));

  if(fdwEvtMask & (EV_RXFLAG)) {
      dprintf(("!WARNING! SetCommMask: unsupported flag EV_RXFLAG!!"));
  }

  pDevData->dwEventMask = fdwEvtMask & ~(EV_RXFLAG); // Clear the not supported Flag.
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::PurgeComm( PHMHANDLEDATA pHMHandleData,
                                   DWORD fdwAction)
{
  ULONG ulParLen, ulDataLen, rc = ERROR_SUCCESS;
  BYTE par  = 0;
  WORD data = 0;

  dprintf(("HMDeviceCommClass::PurgeComm (flags 0x%x) partly implemented",fdwAction));
  // ToDo: find a way to stop the current transmision didn't find
  // any clue how to in Control Program Guide and reference

  ulParLen  = sizeof(par);
  ulDataLen = sizeof(data);
  if(fdwAction & PURGE_TXCLEAR) {
    rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                           IOCTL_GENERAL,
                           DEV_FLUSHOUTPUT,
                           &par,ulParLen,&ulParLen,
                           &data,ulDataLen,&ulDataLen);
  }
  if(fdwAction & PURGE_RXCLEAR) {
    rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                           IOCTL_GENERAL,
                           DEV_FLUSHINPUT,
                           &par,ulParLen,&ulParLen,
                           &data,ulDataLen,&ulDataLen);
  }
  if(rc) {
      dprintf(("!WARNING! OSLibDosDevIOCtl failed with rc %d", rc));
  }
  return (rc == ERROR_SUCCESS);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::ClearCommError( PHMHANDLEDATA pHMHandleData,
                                        LPDWORD lpdwErrors,
                                        LPCOMSTAT lpcst)
{
  APIRET rc;
  ULONG ulLen, dwError = 0;
  USHORT COMErr;

  if(lpdwErrors == NULL)
     lpdwErrors = &dwError;

  dprintf(("HMDeviceCommClass::ClearCommError"));
  ulLen = sizeof(USHORT);

  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_GETCOMMERROR,
                    0,0,0,
                    &COMErr,2,&ulLen);
  *lpdwErrors = 0;
  *lpdwErrors |= (COMErr & 0x0001)?CE_OVERRUN:0;
  *lpdwErrors |= (COMErr & 0x0002)?CE_RXOVER:0;
  *lpdwErrors |= (COMErr & 0x0004)?CE_RXPARITY:0;
  *lpdwErrors |= (COMErr & 0x0008)?CE_FRAME:0;

  dprintf(("Error %x", *lpdwErrors));
  if(lpcst)
  {
    UCHAR ucStatus;
    RXQUEUE qInfo;
    ulLen = 1;
    rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                      IOCTL_ASYNC,
                      ASYNC_GETCOMMSTATUS,
                      0,0,0,
                      &ucStatus,ulLen,&ulLen);
    if(!rc)
    {
      lpcst->fCtsHold  = ((ucStatus & 0x01)>0);
      lpcst->fDsrHold  = ((ucStatus & 0x02)>0);
      lpcst->fRlsdHold = FALSE;//(ucStatus & 0x04)>0);
      lpcst->fXoffHold = ((ucStatus & 0x08)>0);
      lpcst->fXoffSend = ((ucStatus & 0x10)>0);
      lpcst->fEof      = ((ucStatus & 0x20)>0);// Is break = Eof ??
      lpcst->fTxim     = ((ucStatus & 0x40)>0);

      ulLen = sizeof(qInfo);
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                        IOCTL_ASYNC,
                        ASYNC_GETINQUECOUNT,
                        0,0,0,
                        &qInfo,ulLen,&ulLen);
      if(!rc)
      {
        lpcst->cbInQue   = qInfo.cch;
        rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                          IOCTL_ASYNC,
                          ASYNC_GETOUTQUECOUNT,
                          0,0,0,
                          &qInfo,ulLen,&ulLen);
        if(!rc)
          lpcst->cbOutQue = qInfo.cch;
      }
    }
  }

  return(rc==0);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::SetCommState( PHMHANDLEDATA pHMHandleData,
                                      LPDCB lpDCB)
{
  APIRET rc;
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  DCB *pCurDCB = &pDevData->CommCfg.dcb;
  dprintf(("HMDeviceCommClass::SetCommState"));

  rc = 0;
  if(pCurDCB->BaudRate != lpDCB->BaudRate) {
      dprintf(("SetCommState: change baud rate from %d to %d", pCurDCB->BaudRate, lpDCB->BaudRate));
      rc = SetBaud( pHMHandleData,
                    lpDCB->BaudRate);
  }

  if(!rc)
  {
    if( (pCurDCB->ByteSize != lpDCB->ByteSize) ||
        (pCurDCB->Parity   != lpDCB->Parity) ||
        (pCurDCB->StopBits != lpDCB->StopBits))
    {
      dprintf(("SetCommState: change line %d %d %d", lpDCB->ByteSize, lpDCB->Parity, lpDCB->StopBits));
      rc = SetLine( pHMHandleData,
                    lpDCB->ByteSize,
                    lpDCB->Parity,
                    lpDCB->StopBits);
    }
  }

  if(!rc)
  {
    if( (pCurDCB->fOutxCtsFlow       != lpDCB->fOutxCtsFlow) ||
        (pCurDCB->fOutxDsrFlow       != lpDCB->fOutxDsrFlow) ||
        (pCurDCB->fDtrControl        != lpDCB->fDtrControl) ||
        (pCurDCB->fDsrSensitivity    != lpDCB->fDsrSensitivity) ||
        (pCurDCB->fTXContinueOnXoff  != lpDCB->fTXContinueOnXoff) ||
        (pCurDCB->fOutX              != lpDCB->fOutX) ||
        (pCurDCB->fInX               != lpDCB->fInX) ||
        (pCurDCB->fErrorChar         != lpDCB->fErrorChar) ||
        (pCurDCB->fNull              != lpDCB->fNull) ||
        (pCurDCB->fRtsControl        != lpDCB->fRtsControl) ||
        (pCurDCB->fAbortOnError      != lpDCB->fAbortOnError) ||
        (pCurDCB->XonChar            != lpDCB->XonChar) ||
        (pCurDCB->XoffChar           != lpDCB->XoffChar) ||
        (pCurDCB->ErrorChar          != lpDCB->ErrorChar))
    {
      dprintf(("SetCommState: change flags cts %d dsr %d dtr %d dsr %d tx %d out %d in %d ferror %d null %d rts %d abort %d xon %d xoff %d error %d", lpDCB->fOutxCtsFlow, lpDCB->fOutxDsrFlow,lpDCB->fDtrControl,lpDCB->fDsrSensitivity,lpDCB->fDsrSensitivity,lpDCB->fTXContinueOnXoff,lpDCB->fOutX, lpDCB->fInX,lpDCB->fErrorChar,lpDCB->fNull,lpDCB->fRtsControl,lpDCB->fAbortOnError,lpDCB->XonChar,lpDCB->XoffChar,lpDCB->ErrorChar));
      SetOS2DCB( pHMHandleData,
                 lpDCB->fOutxCtsFlow, lpDCB->fOutxDsrFlow,
                 lpDCB->fDtrControl,  lpDCB->fDsrSensitivity,
                 lpDCB->fTXContinueOnXoff, lpDCB->fOutX,
                 lpDCB->fInX, lpDCB->fErrorChar,
                 lpDCB->fNull, lpDCB->fRtsControl,
                 lpDCB->fAbortOnError, lpDCB->XonChar,
                 lpDCB->XoffChar,lpDCB->ErrorChar);
    }
  }

  return(rc==0);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::GetCommState( PHMHANDLEDATA pHMHandleData,
                                      LPDCB lpdcb)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::GetCommState %x", lpdcb));

  if(lpdcb == NULL) {
      ::SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }

  memcpy(lpdcb,&pDevData->CommCfg.dcb,sizeof(DCB));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::GetCommModemStatus( PHMHANDLEDATA pHMHandleData,
                                          LPDWORD lpModemStat )
{
  APIRET rc;
  ULONG ulLen;
  USHORT COMErr;
  UCHAR ucStatus;

  dprintf(("HMDeviceCommClass::GetCommModemStatus %x", lpModemStat));
  if(lpModemStat == NULL) {
      ::SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }

  ulLen = sizeof(CHAR);

  ulLen = 1;
  *lpModemStat = 0;

  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_GETMODEMINPUT,
                    0,0,0,
                    &ucStatus,ulLen,&ulLen);
  if(!rc)
  {
    *lpModemStat |= (ucStatus & 0x10)? MS_CTS_ON:0;
    *lpModemStat |= (ucStatus & 0x20)? MS_DSR_ON:0;
    *lpModemStat |= (ucStatus & 0x40)? MS_RING_ON:0;
    *lpModemStat |= (ucStatus & 0x80)? MS_RLSD_ON:0;
  }

  dprintf2(("HMDeviceCommClass::GetCommModemStatus -> %s %x rc=%d", DebugModemStatus(*lpModemStat), *lpModemStat, rc));
  return(rc==0);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::GetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                                         LPCOMMTIMEOUTS lpctmo)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::GetCommTimeouts %x stub", lpctmo));

  if(lpctmo == NULL) {
      ::SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }

  memcpy( lpctmo,
          &pDevData->CommTOuts,
          sizeof(COMMTIMEOUTS));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::SetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                                         LPCOMMTIMEOUTS lpctmo)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  DCBINFO os2dcb;
  ULONG ulLen;
  APIRET rc;
  UCHAR fbTimeOut;

  if(lpctmo == NULL) {
      dprintf(("!WARNING! HMDeviceCommClass::SetCommTimeouts %x -> invalid parameter", lpctmo));
      ::SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }

  dprintf(("HMDeviceCommClass::SetCommTimeouts\n"
           " ReadIntervalTimeout         : 0x%x\n"
           " ReadTotalTimeoutMultiplier  : %d\n"
           " ReadTotalTimeoutConstant    : %d\n"
           " WriteTotalTimeoutMultiplier : %d\n"
           " WriteTotalTimeoutConstant   : %d\n",
           lpctmo->ReadIntervalTimeout,
           lpctmo->ReadTotalTimeoutMultiplier,
           lpctmo->ReadTotalTimeoutConstant,
           lpctmo->WriteTotalTimeoutMultiplier,
           lpctmo->WriteTotalTimeoutConstant
  ));

  memcpy( &pDevData->CommTOuts,
          lpctmo,
          sizeof(COMMTIMEOUTS));

  memcpy(&os2dcb,&pDevData->dcbOS2,sizeof(DCBINFO));

  fbTimeOut = 0x02; //normal processing (wait until timout or buffer full)
  if(MAXDWORD==pDevData->CommTOuts.ReadIntervalTimeout)
  {
    if( (0==pDevData->CommTOuts.ReadTotalTimeoutMultiplier) &&
        (0==pDevData->CommTOuts.ReadTotalTimeoutConstant))
      fbTimeOut = 0x05; //no wait
    else
      fbTimeOut = 0x04; //wait for something
  }
  else
  {
    DWORD dwTimeout;
    dwTimeout = pDevData->CommTOuts.ReadIntervalTimeout/10;
    if(dwTimeout)
      dwTimeout--; // 0=10 ms unit is 10ms or .01s

    os2dcb.usWriteTimeout = 0x0000FFFF & dwTimeout;
    os2dcb.usReadTimeout  = 0x0000FFFF & dwTimeout;
  }
  if( (0==pDevData->CommTOuts.WriteTotalTimeoutMultiplier) &&
      (0==pDevData->CommTOuts.WriteTotalTimeoutConstant))
  {//no timeout used for writing
    os2dcb.fbTimeOut |= 1; //write infinite timeout
  }

  os2dcb.fbTimeOut = (os2dcb.fbTimeOut & 0xF8) | fbTimeOut;

  dprintf((" New DCB:\n"
           " WriteTimeout           : %d\n"
           " ReadTimeout            : %d\n"
           " CtlHandshake           : 0x%x\n"
           " FlowReplace            : 0x%x\n"
           " Timeout                : 0x%x\n"
           " Error replacement Char : 0x%x\n"
           " Break replacement Char : 0x%x\n"
           " XON Char               : 0x%x\n"
           " XOFF Char              : 0x%x\n",
           os2dcb.usWriteTimeout,
           os2dcb.usReadTimeout,
           os2dcb.fbCtlHndShake,
           os2dcb.fbFlowReplace,
           os2dcb.fbTimeOut,
           os2dcb.bErrorReplacementChar,
           os2dcb.bBreakReplacementChar,
           os2dcb.bXONChar,
           os2dcb.bXOFFChar));

  ulLen = sizeof(DCBINFO);
  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                          IOCTL_ASYNC,
                          ASYNC_SETDCBINFO,
                          &os2dcb,ulLen,&ulLen,
                          NULL,0,NULL);
  dprintf(("IOCRL returned %d",rc));
  return(0==rc);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::TransmitCommChar( PHMHANDLEDATA pHMHandleData,
                                          CHAR cChar )
{
  APIRET rc;
  ULONG ulLen;
  USHORT COMErr;

  dprintf(("HMDeviceCommClass::TransmitCommChar"));
  ulLen = sizeof(CHAR);

  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_TRANSMITIMM,
                    &cChar,ulLen,&ulLen,
                    NULL,0,NULL);

  return(rc==0);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::SetCommBreak( PHMHANDLEDATA pHMHandleData )
{
  APIRET rc;
  ULONG ulLen;
  USHORT COMErr;

  dprintf(("HMDeviceCommClass::SetCommBreak"));
  ulLen = sizeof(USHORT);

  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_SETBREAKON,
                    0,0,0,
                    &COMErr,2,&ulLen);

  return(rc==0);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::ClearCommBreak( PHMHANDLEDATA pHMHandleData)
{
  APIRET rc;
  ULONG ulLen;
  USHORT COMErr;

  dprintf(("HMDeviceCommClass::ClearCommBreak"));
  ulLen = sizeof(USHORT);

  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_SETBREAKOFF,
                    0,0,0,
                    &COMErr,2,&ulLen);

  return(rc==0);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::SetCommConfig( PHMHANDLEDATA pHMHandleData,
                                       LPCOMMCONFIG lpCC,
                                       DWORD dwSize )
{
  dprintf(("HMDeviceCommClass::SetCommConfig NOT IMPLEMENTED"));

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::GetCommConfig( PHMHANDLEDATA pHMHandleData,
                                       LPCOMMCONFIG lpCC,
                                       LPDWORD lpdwSize )
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::GetCommConfig"));

  if( O32_IsBadWritePtr(lpCC,sizeof(COMMCONFIG)) ||
      *lpdwSize< sizeof(COMMCONFIG) )
  {
    ::SetLastError(ERROR_INSUFFICIENT_BUFFER);
    *lpdwSize= sizeof(COMMCONFIG);
    return FALSE;
  }

  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
  {
    ::SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  memcpy(lpCC,&pDevData->CommCfg,sizeof(COMMCONFIG));
  *lpdwSize = sizeof(COMMCONFIG);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::EscapeCommFunction( PHMHANDLEDATA pHMHandleData,
                                            UINT dwFunc )
{
  APIRET rc;
  ULONG ulDLen,ulPLen;
  USHORT COMErr;
  MODEMSTATUS mdm;

  dprintf(("HMDeviceCommClass::EscapeCommFunction %x", dwFunc));

  ulDLen = sizeof(USHORT);
  ulPLen = sizeof(MODEMSTATUS);
  switch(dwFunc)
  {
    case CLRDTR:
      mdm.fbModemOn  = 0x00;
      mdm.fbModemOff = 0XFE;
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_SETMODEMCTRL,
                             &mdm,ulPLen,&ulPLen,
                             &COMErr,ulDLen,&ulDLen);
      dprintf(("CLRDTR rc = %d Comerror = 0x%x",rc,COMErr));
      rc = COMErr;
      if(rc==0)
      {
        BYTE bModem;
        ulDLen = sizeof(BYTE);
        rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                               IOCTL_ASYNC,
                               ASYNC_GETMODEMOUTPUT,
                               NULL,0,NULL,
                               &bModem,ulDLen,&ulDLen);
        dprintf(("Check DTR rc = %d Flags = 0x%x",rc,bModem));
        rc = bModem & 0x01;
      }
      break;
    case CLRRTS:
      mdm.fbModemOn  = 0x00;
      mdm.fbModemOff = 0XFD;
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_SETMODEMCTRL,
                             &mdm,ulPLen,&ulPLen,
                             &COMErr,ulDLen,&ulDLen);
      dprintf(("CLRRTS: rc = %d, Comm error %x", rc, COMErr));
      break;
    case SETDTR:
      mdm.fbModemOn  = 0x01;
      mdm.fbModemOff = 0XFF;
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_SETMODEMCTRL,
                             &mdm,ulPLen,&ulPLen,
                             &COMErr,ulDLen,&ulDLen);
      dprintf(("SETDTR: rc = %d, Comm error %x", rc, COMErr));
      break;
    case SETRTS:
      mdm.fbModemOn  = 0x02;
      mdm.fbModemOff = 0XFF;
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_SETMODEMCTRL,
                             &mdm,ulPLen,&ulPLen,
                             &COMErr,ulDLen,&ulDLen);
      dprintf(("SETRTS: rc = %d, Comm error %x", rc, COMErr));
      break;
    case SETXOFF:
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_STOPTRANSMIT,
                             0,0,0,
                             0,0,0);
      dprintf(("SETXOFF: rc = %d", rc));
      break;
    case SETXON:
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_STARTTRANSMIT,
                             0,0,0,
                             0,0,0);
      dprintf(("SETXON: rc = %d", rc));
      break;
    default:
      dprintf(("!ERROR!: EscapeCommFunction: unknown function"));
      ::SetLastError(ERROR_INVALID_PARAMETER);
      return(FALSE);
  }

  return(rc==0);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::SetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                              LPCOMMCONFIG lpCC,
                                              DWORD dwSize)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpDeviceData;
  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
  {
    ::SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  dprintf(("SetDefaultCommConfig %x %d", lpCC, dwSize));
  memset(&pDevData->CommCfg,0, sizeof(COMMCONFIG));
  memcpy(&pDevData->CommCfg,lpCC,dwSize>sizeof(COMMCONFIG)?sizeof(COMMCONFIG):dwSize);

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL HMDeviceCommClass::GetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                              LPCOMMCONFIG lpCC,
                                              LPDWORD lpdwSize)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpDeviceData;

  if( O32_IsBadWritePtr(lpCC,sizeof(COMMCONFIG)) ||
      *lpdwSize< sizeof(COMMCONFIG) )
  {
    ::SetLastError(ERROR_INSUFFICIENT_BUFFER);
    *lpdwSize= sizeof(COMMCONFIG);
    return FALSE;
  }

  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
  {
    ::SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }
  dprintf(("GetDefaultCommConfig %x %x", lpCC, lpdwSize));

  memcpy(lpCC,&pDevData->CommCfg,sizeof(COMMCONFIG));
  *lpdwSize = sizeof(COMMCONFIG);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
APIRET HMDeviceCommClass::SetLine( PHMHANDLEDATA pHMHandleData,
                                   UCHAR ucSize,
                                   UCHAR ucParity,
                                   UCHAR ucStop)
{
  APIRET rc;
  ULONG ulLen;
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  DCB *pCurDCB = &pDevData->CommCfg.dcb;
  struct
  {
    UCHAR ucSize;
    UCHAR ucParity;
    UCHAR ucStop;
    UCHAR ucPadding;
  }Param;

  ulLen = 3;
  Param.ucSize   = ucSize;
  Param.ucParity = ucParity;
  Param.ucStop   = ucStop;

  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_SETLINECTRL,
                    &Param,ulLen,&ulLen,
                    NULL,0,NULL);

  if(0==rc)
  {
    pCurDCB->ByteSize = ucSize;
    pCurDCB->Parity   = ucParity;
    pCurDCB->StopBits = ucStop;
  }
  else {
    dprintf(("!ERROR! SetLine: OSLibDosDevIOCtl failed with rc %d", rc));
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
APIRET HMDeviceCommClass::SetOS2DCB( PHMHANDLEDATA pHMHandleData,
                                     BOOL fOutxCtsFlow, BOOL fOutxDsrFlow,
                                     UCHAR ucDtrControl,  BOOL fDsrSensitivity,
                                     BOOL fTXContinueOnXoff, BOOL fOutX,
                                     BOOL fInX, BOOL fErrorChar,
                                     BOOL fNull, UCHAR ucRtsControl,
                                     BOOL fAbortOnError, BYTE XonChar,
                                     BYTE XoffChar,BYTE ErrorChar)
{
  APIRET rc;
  ULONG ulLen;
  DCBINFO os2dcb;
  UCHAR  fbTimeOut;
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  DCB *pCurDCB = &pDevData->CommCfg.dcb;

  memcpy(&os2dcb,&pDevData->dcbOS2,sizeof(DCBINFO));
  os2dcb.fbCtlHndShake = (ucDtrControl & 0x03) |
                         (fOutxCtsFlow?0x08:0x00) |
                         (fOutxDsrFlow?0x10:0x00) |
                         // No DCD support in Win32 ?!
                         (fDsrSensitivity?0x40:0x00);
  os2dcb.fbFlowReplace = (fOutX?0x01:0x00) |
                         (fInX?0x02:0x00) |
                         (fErrorChar?0x04:0x00)|
                         (fNull?0x08:0x00)|
                         (fTXContinueOnXoff?0x02:0x00)| // Not sure if thats the right flag to test
                         (ucRtsControl<<6);

  fbTimeOut = 0x02;
  if(MAXDWORD==pDevData->CommTOuts.ReadIntervalTimeout)
  {
    if( (0==pDevData->CommTOuts.ReadTotalTimeoutMultiplier) &&
        (0==pDevData->CommTOuts.ReadTotalTimeoutConstant))
      fbTimeOut = 0x05;
    else
      fbTimeOut = 0x04;
  }
  else
  {
    DWORD dwTimeout;
    dwTimeout = pDevData->CommTOuts.ReadIntervalTimeout/10;
    if(dwTimeout)
      dwTimeout--; // 0=10 ms unit is 10ms or .01s
    os2dcb.usWriteTimeout = 0x0000FFFF & dwTimeout;
    os2dcb.usReadTimeout  = 0x0000FFFF & dwTimeout;
  }
  os2dcb.fbTimeOut = (os2dcb.fbTimeOut & 0xF9) | fbTimeOut;
  os2dcb.bErrorReplacementChar = ErrorChar;
  os2dcb.bXONChar              = XonChar;
  os2dcb.bXOFFChar             = XoffChar;
  ulLen = sizeof(DCBINFO);
  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                          IOCTL_ASYNC,
                          ASYNC_SETDCBINFO,
                          &os2dcb,ulLen,&ulLen,
                          NULL,0,NULL);

  if(0==rc)
  {
    memcpy(&pDevData->dcbOS2,&os2dcb,sizeof(DCBINFO));
    pCurDCB->fOutxCtsFlow       = fOutxCtsFlow;
    pCurDCB->fOutxDsrFlow       = fOutxDsrFlow;
    pCurDCB->fDtrControl        = ucDtrControl;
    pCurDCB->fDsrSensitivity    = fDsrSensitivity;
    pCurDCB->fTXContinueOnXoff  = fTXContinueOnXoff;
    pCurDCB->fOutX              = fOutX;
    pCurDCB->fInX               = fInX;
    pCurDCB->fErrorChar         = fErrorChar;
    pCurDCB->fNull              = fNull;
    pCurDCB->fRtsControl        = ucRtsControl;
    pCurDCB->fAbortOnError      = fAbortOnError;
    pCurDCB->XonChar            = XonChar;
    pCurDCB->XoffChar           = XoffChar;
    pCurDCB->ErrorChar          = ErrorChar;

    dprintf(("OS/2 DCB:\n"
             " WriteTimeout           : %d\n"
             " ReadTimeout            : %d\n"
             " CtlHandshake           : 0x%x\n"
             " FlowReplace            : 0x%x\n"
             " Timeout                : 0x%x\n"
             " Error replacement Char : 0x%x\n"
             " Break replacement Char : 0x%x\n"
             " XON Char               : 0x%x\n"
             " XOFF Char              : 0x%x\n",
             os2dcb.usWriteTimeout,
             os2dcb.usReadTimeout,
             os2dcb.fbCtlHndShake,
             os2dcb.fbFlowReplace,
             os2dcb.fbTimeOut,
             os2dcb.bErrorReplacementChar,
             os2dcb.bBreakReplacementChar,
             os2dcb.bXONChar,
             os2dcb.bXOFFChar));

  }

  return rc;

}
//******************************************************************************
//******************************************************************************
APIRET HMDeviceCommClass::SetBaud( PHMHANDLEDATA pHMHandleData,
                                   DWORD dwNewBaud)
{
  APIRET rc;
  ULONG ulLen;
  EXTBAUDSET SetBaud;
  EXTBAUDGET GetBaud;
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  ulLen = sizeof(SetBaud);
  SetBaud.ulBaud = dwNewBaud;
  SetBaud.ucFrac = 0;
  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_EXTSETBAUDRATE,
                    &SetBaud,ulLen,&ulLen,
                    NULL,0,NULL);
  if(0==rc)
  {
    ulLen = sizeof(GetBaud);
    rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                      IOCTL_ASYNC,
                      ASYNC_EXTGETBAUDRATE,
                      NULL,0,NULL,
                      &GetBaud,ulLen,&ulLen);
    if(0==rc)
    {
      if(dwNewBaud != GetBaud.ulCurrBaud) {
        dprintf(("!WARNING! dwNewBaud (%d) != GetBaud.ulCurrBaud (%d)", dwNewBaud, GetBaud.ulCurrBaud));
        rc = 1; // ToDo set a proper Errorhandling
      }
      else
      {
        pDevData->CommCfg.dcb.BaudRate = dwNewBaud;
      }
    }
    else {
        dprintf(("!WARNING! SetBaud: (get) OSLibDosDevIOCtl failed with rc %d", rc));
    }
  }
  else {
    dprintf(("!WARNING! SetBaud: (set) OSLibDosDevIOCtl failed with rc %d", rc));
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
void HMDeviceCommClass::CloseOverlappedIOHandlers()
{
    for(int i=0;i<MAX_COMPORTS;i++) {
        if(handler[i]) {
            handler[i]->Release(TRUE);
            handler[i] = NULL;
        }
    }
}
#ifdef DEBUG_LOGGING
//******************************************************************************
//******************************************************************************
static char *DebugCommEvent(DWORD dwEvents)
{
    static char szCommEvents[128];

    szCommEvents[0] = 0;

    if(dwEvents & EV_RXCHAR) {
        strcat(szCommEvents, "EV_RXCHAR ");
    }
    if(dwEvents & EV_TXEMPTY) {
        strcat(szCommEvents, "EV_TXEMPTY ");
    }
    if(dwEvents & EV_CTS) {
        strcat(szCommEvents, "EV_CTS ");
    }
    if(dwEvents & EV_DSR) {
        strcat(szCommEvents, "EV_DSR ");
    }
    if(dwEvents & EV_RLSD) {
        strcat(szCommEvents, "EV_RLSD ");
    }
    if(dwEvents & EV_BREAK) {
        strcat(szCommEvents, "EV_BREAK ");
    }
    if(dwEvents & EV_ERR) {
        strcat(szCommEvents, "EV_ERR ");
    }
    if(dwEvents & EV_RING) {
        strcat(szCommEvents, "EV_RING ");
    }
    return szCommEvents;
}
//******************************************************************************
//******************************************************************************
static char *DebugModemStatus(DWORD dwModemStatus)
{
    static char szModemStatus[128];

    szModemStatus[0] = 0;

    if(dwModemStatus & MS_CTS_ON) {
        strcat(szModemStatus, "MS_CTS_ON ");
    }
    if(dwModemStatus & MS_DSR_ON) {
        strcat(szModemStatus, "MS_DSR_ON ");
    }
    if(dwModemStatus & MS_RING_ON) {
        strcat(szModemStatus, "MS_RING_ON ");
    }
    if(dwModemStatus & MS_RLSD_ON) {
        strcat(szModemStatus, "MS_RLSD_ON ");
    }
    return szModemStatus;
}
#endif
//******************************************************************************
//******************************************************************************
OverlappedIOHandler *HMDeviceCommClass::handler[MAX_COMPORTS] = {NULL};

