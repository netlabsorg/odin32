/* $Id: hmparport.cpp,v 1.5 2001-11-23 00:50:06 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 Parallel Port device access class
 *
 * 2001 Patrick Haller <patrick.haller@innotek.de>
 *
 */



#include <os2win.h>
#include <string.h>
#include <handlemanager.h>
#include <heapstring.h>
#include <winioctl.h>
#include "hmdevice.h"
#include "hmparport.h"
#include "oslibdos.h"

#define DBG_LOCALLOG  DBG_hmparport
#include "dbglocal.h"


#define MAGIC_PARPORT 0x4c505431

#define IOCTL_PRINTER                      0x0005
#define PRT_QUERYJOBHANDLE                 0x0021
#define PRT_SETFRAMECTL                    0x0042
#define PRT_SETINFINITERETRY               0x0044
#define PRT_INITPRINTER                    0x0046
#define PRT_ACTIVATEFONT                   0x0048
#define PRT_SETPRINTJOBTITLE               0x004D
#define PRT_SETIRQTIMEOUT                  0x004E
#define PRT_SETCOMMMODE                    0x0052
#define PRT_SETDATAXFERMODE                0x0053
#define PRT_GETFRAMECTL                    0x0062
#define PRT_GETINFINITERETRY               0x0064
#define PRT_GETPRINTERSTATUS               0x0066
#define PRT_QUERYACTIVEFONT                0x0069
#define PRT_VERIFYFONT                     0x006A
#define PRT_QUERYIRQTIMEOUT                0x006E
#define PRT_QUERYCOMMMODE                  0x0072
#define PRT_QUERYDATAXFERMODE              0x0073
#define PRT_QUERDEVICEID                   0x0074


#if 0
#pragma pack(1)
typedef struct _DCBINFO
{
  USHORT   usWriteTimeout;         /*  Time period used for Write Timeout processing. */
  USHORT   usReadTimeout;          /*  Time period used for Read Timeout processing. */
  BYTE     fbCtlHndShake;          /*  HandShake Control flag. */
  BYTE     fbFlowReplace;          /*  Flow Control flag. */
  BYTE     fbTimeOut;              /*  Timeout flag. */
  BYTE     bErrorReplacementChar;  /*  Error Replacement Character. */
  BYTE     bBreakReplacementChar;  /*  Break Replacement Character. */
  BYTE     bXONChar;               /*  Character XON. */
  BYTE     bXOFFChar;              /*  Character XOFF. */
} DCBINFO;
typedef DCBINFO *PDCBINFO;


typedef struct _RXQUEUE
{
  USHORT   cch;  /*  Number of characters in the queue. */
  USHORT   cb;   /*  Size of receive/transmit queue. */
} RXQUEUE;

typedef RXQUEUE *PRXQUEUE;


typedef struct _MODEMSTATUS
{
  BYTE   fbModemOn;   /*  Modem Control Signals ON Mask. */
  BYTE   fbModemOff;  /*  Modem Control Signals OFF Mask. */
} MODEMSTATUS;

typedef MODEMSTATUS *PMODEMSTATUS;


#pragma pack()


#endif


#if 0

typedef struct _PARALLEL_PNP_INFORMATION {
  PHYSICAL_ADDRESS  OriginalEcpController;
  PUCHAR  EcpController;
  ULONG  SpanOfEcpController;
  ULONG  PortNumber;
  ULONG  HardwareCapabilities;
  PPARALLEL_SET_CHIP_MODE  TrySetChipMode;
  PPARALLEL_CLEAR_CHIP_MODE  ClearChipMode;
  ULONG  FifoDepth;
  ULONG  FifoWidth;
  PHYSICAL_ADDRESS  EppControllerPhysicalAddress;
  ULONG  SpanOfEppController;
  ULONG  Ieee1284_3DeviceCount;
  PPARALLEL_TRY_SELECT_ROUTINE  TrySelectDevice;
  PPARALLEL_DESELECT_ROUTINE  DeselectDevice;
  PVOID  Context;
  ULONG  CurrentMode;
  PWSTR  PortName;
} PARALLEL_PNP_INFORMATION, *PPARALLEL_PNP_INFORMATION;
#endif



typedef struct _HMDEVPARPORTDATA
{
  ULONG ulMagic;
  // Win32 Device Control Block
  COMMCONFIG   CommCfg;
  //OS/2 Device Control Block
} HMDEVPARPORTDATA, *PHMDEVPARPORTDATA;

static VOID *CreateDevData()
{
  PHMDEVPARPORTDATA pData;
  pData = new HMDEVPARPORTDATA();
  if(NULL!=pData)
  {
    memset(pData,0,sizeof(HMDEVPARPORTDATA));
    pData->ulMagic = MAGIC_PARPORT;
    pData->CommCfg.dwSize   = sizeof(COMMCONFIG);
    pData->CommCfg.wVersion = 1;
    pData->CommCfg.dwProviderSubType = PST_PARALLELPORT;
  }
  return pData;
}


HMDeviceParPortClass::HMDeviceParPortClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName)
{
  dprintf(("HMDeviceParPortClass::HMDevParPortClass(%s)\n",
           lpDeviceName));
  
  VOID *pData;
  dprintf(("HMDeviceParPortClass: Register LPT1 to LPT3 with Handle Manager\n"));
  
  pData = CreateDevData();
  if(pData!= NULL)
    HMDeviceRegisterEx("LPT1", this, pData);
}

/*****************************************************************************
 * Name      : HMDeviceParPortClass::FindDevice
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
BOOL HMDeviceParPortClass::FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength)
{
  dprintf(("HMDeviceParPortClass::FindDevice(%s,%s)\n",
           lpClassDevName,
           lpDeviceName));
  
    if(namelength > 5)
        return FALSE;  //can't be lpt name

    //first 3 letters 'LPT'?
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
        return TRUE;    //we support up to LPT3
    }
    return FALSE;
}

DWORD HMDeviceParPortClass::CreateFile(LPCSTR lpFileName,
                                       PHMHANDLEDATA pHMHandleData,
                                       PVOID lpSecurityAttributes,
                                       PHMHANDLEDATA pHMHandleDataTemplate)
{
  dprintf(("HMDeviceParPortClass::CreateFile(%s,%08xh,%08xh,%08xh)\n",
           lpFileName,
           pHMHandleData,
           lpSecurityAttributes,
           pHMHandleDataTemplate));
  
 char comname[6];

  dprintf(("HMDeviceParPortClass: Parallel port %s open request\n", lpFileName));

  if(strlen(lpFileName) > 5) {
    return -1;  //safety check (unnecessary..)
  }
  pHMHandleData->hHMHandle = 0;

  strcpy(comname, lpFileName);
  comname[4] = 0;   //get rid of : (if present) (eg LPT1:)

  //AH: TODO parse Win32 security handles
  ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS);
  pHMHandleData->hHMHandle = OSLibDosOpen(comname,
                                          OSLIB_ACCESS_READWRITE |
                                          OSLIB_ACCESS_SHAREDENYREAD |
                                          OSLIB_ACCESS_SHAREDENYWRITE);
  SetErrorMode(oldmode);

#if 0
  if (pHMHandleData->hHMHandle != 0)
  {
    ULONG ulLen;
    APIRET rc;
    pHMHandleData->lpHandlerData = new HMDEVPARPORTDATA();
    // Init The handle instance with the default default device config
    memcpy( pHMHandleData->lpHandlerData,
            pHMHandleData->lpDeviceData,
            sizeof(HMDEVPARPORTDATA));
    
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
      return -1;
    }
    rc = SetBaud(pHMHandleData,9600);
    dprintf(("Init Baud to 9600 rc = %d",rc));
    rc = SetLine(pHMHandleData,8,0,0);
    dprintf(("Set Line to 8/N/1 rc = %d",rc));
    return 0;
  }
  else
    return  -1;
#endif
  
  return NO_ERROR;
}


                      /* this is a handler method for calls to CloseHandle() */
BOOL HMDeviceParPortClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("HMDeviceParPortClass: Parallel port close request(%08xh)\n",
          pHMHandleData));
  delete pHMHandleData->lpHandlerData;
  return OSLibDosClose(pHMHandleData->hHMHandle);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceParPortClass::WriteFile
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

BOOL HMDeviceParPortClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                     LPCVOID       lpBuffer,
                                     DWORD         nNumberOfBytesToWrite,
                                     LPDWORD       lpNumberOfBytesWritten,
                                     LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32:HMDeviceParPortClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x)",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped));

  BOOL  ret;
  ULONG ulBytesWritten;

  if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
    dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && lpOverlapped) {
    dprintf(("Warning: lpOverlapped != NULL & !FILE_FLAG_OVERLAPPED; sync operation"));
  }

  ret = OSLibDosWrite(pHMHandleData->hHMHandle, (LPVOID)lpBuffer, nNumberOfBytesToWrite,
                      &ulBytesWritten);

  if(lpNumberOfBytesWritten) {
       *lpNumberOfBytesWritten = (ret) ? ulBytesWritten : 0;
  }
  if(ret == FALSE) {
       dprintf(("ERROR: WriteFile failed with rc %d", GetLastError()));
  }

  return ret;
}

/*****************************************************************************
 * Name      : BOOL WriteFileEx
 * Purpose   : The WriteFileEx function writes data to a file. It is designed
 *             solely for asynchronous operation, unlike WriteFile, which is
 *             designed for both synchronous and asynchronous operation.
 *             WriteFileEx reports its completion status asynchronously,
 *             calling a specified completion routine when writing is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to write
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to write
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL HMDeviceParPortClass::WriteFileEx(PHMHANDLEDATA pHMHandleData,
                                       LPVOID       lpBuffer,
                                       DWORD        nNumberOfBytesToWrite,
                                       LPOVERLAPPED lpOverlapped,
                                       LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("ERROR: WriteFileEx %s (%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpOverlapped,
           lpCompletionRoutine));

  SetLastError(ERROR_INVALID_FUNCTION);
  return FALSE;
}

/*****************************************************************************
 * Name      : BOOL HMDeviceParPortClass::ReadFile
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

BOOL HMDeviceParPortClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                    LPCVOID       lpBuffer,
                                    DWORD         nNumberOfBytesToRead,
                                    LPDWORD       lpNumberOfBytesRead,
                                    LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32:HMDeviceParPortClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped));

  BOOL  ret;
  ULONG ulBytesRead;

  if((pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && !lpOverlapped) {
    dprintf(("FILE_FLAG_OVERLAPPED flag set, but lpOverlapped NULL!!"));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  if(!(pHMHandleData->dwFlags & FILE_FLAG_OVERLAPPED) && lpOverlapped) {
    dprintf(("Warning: lpOverlapped != NULL & !FILE_FLAG_OVERLAPPED; sync operation"));
  }

  ret = OSLibDosRead(pHMHandleData->hHMHandle, (LPVOID)lpBuffer, nNumberOfBytesToRead,
                     &ulBytesRead);

  if(lpNumberOfBytesRead) {
       *lpNumberOfBytesRead = (ret) ? ulBytesRead : 0;
  }
  if(ret == FALSE) {
       dprintf(("ERROR: ReadFile failed with rc %d", GetLastError()));
  }
  return ret;
}

/*****************************************************************************
 * Name      : BOOL ReadFileEx
 * Purpose   : The ReadFileEx function reads data from a file asynchronously.
 *             It is designed solely for asynchronous operation, unlike the
 *             ReadFile function, which is designed for both synchronous and
 *             asynchronous operation. ReadFileEx lets an application perform
 *             other processing during a file read operation.
 *             The ReadFileEx function reports its completion status asynchronously,
 *             calling a specified completion routine when reading is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to read
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to read
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/
BOOL HMDeviceParPortClass::ReadFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToRead,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("ERROR: ReadFileEx %s (%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpHMDeviceName,
           pHMHandleData->hHMHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpOverlapped,
           lpCompletionRoutine));

  SetLastError(ERROR_INVALID_FUNCTION);
  return FALSE;
}

BOOL HMDeviceParPortClass::GetCommProperties( PHMHANDLEDATA pHMHandleData,
                                             LPCOMMPROP lpcmmp)
{
  dprintf(("HMDeviceParPortClass::GetCommProperties(%08xh, %08xh)\n",
           pHMHandleData,
           lpcmmp));
  
  APIRET rc;
  ULONG ulLen;
  int i;
  
#if 0
  USHORT COMErr;
  EXTBAUDGET BaudInfo;
  ulLen = sizeof(EXTBAUDGET);
  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_EXTGETBAUDRATE,
                    0,0,0,
                        &BaudInfo,ulLen,&ulLen);
#endif
  rc = NO_ERROR;
  
  memset(lpcmmp,0,sizeof(COMMPROP));
  lpcmmp->wPacketLength  = sizeof(COMMPROP);
  lpcmmp->wPacketVersion = 1; //???
  lpcmmp->dwProvSubType =  PST_PARALLELPORT;
  
#if 0
  lpcmmp->dwServiceMask  = SP_SERIALCOMM;
  for(i=0;i<BaudTableSize && BaudInfo.ulMaxBaud <= BaudTable[i].dwBaudRate;i++);
  lpcmmp->dwMaxBaud      = BaudTable[i].dwBaudFlag;
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
#endif
  
  return(rc==0);
}

BOOL HMDeviceParPortClass::ClearCommError( PHMHANDLEDATA pHMHandleData,
                                          LPDWORD lpdwErrors,
                                          LPCOMSTAT lpcst)
{
  dprintf(("HMDeviceParPortClass::ClearCommError(%08xh,%08xh,%08xh)\n",
           pHMHandleData,
           lpdwErrors,
           lpcst));
  
  APIRET rc;
  ULONG ulLen;
  USHORT COMErr;

  ulLen = sizeof(USHORT);
  
  *lpdwErrors = 0;
  rc = NO_ERROR;
  
#if 0
  // ParPort: CE_DNS, CE_OOP CE_PTO
  
  rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                    IOCTL_ASYNC,
                    ASYNC_GETCOMMERROR,
                    0,0,0,
                    &COMErr,2,&ulLen);
  *lpdwErrors |= (COMErr & 0x0001)?CE_OVERRUN:0;
  *lpdwErrors |= (COMErr & 0x0002)?CE_RXOVER:0;
  *lpdwErrors |= (COMErr & 0x0004)?CE_RXPARITY:0;
  *lpdwErrors |= (COMErr & 0x0008)?CE_FRAME:0;

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
#endif
  
  return(rc==0);
}


BOOL HMDeviceParPortClass::DeviceIoControl(PHMHANDLEDATA pHMHandleData, 
                                           DWORD dwIoControlCode,
                                           LPVOID lpInBuffer, 
                                           DWORD nInBufferSize,
                                           LPVOID lpOutBuffer, 
                                           DWORD nOutBufferSize,
                                           LPDWORD lpBytesReturned, 
                                           LPOVERLAPPED lpOverlapped)
{
#ifdef DEBUG
    char *msg = NULL;

    switch(dwIoControlCode)
    {
      case IOCTL_INTERNAL_GET_PARALLEL_PORT_INFO:
        msg = "IOCTL_INTERNAL_GET_PARALLEL_PORT_INFO";
        break;
      
      case IOCTL_INTERNAL_GET_PARALLEL_PNP_INFO:
        msg = "IOCTL_INTERNAL_GET_PARALLEL_PNP_INFO";
        break;
    }
  
    if(msg) {
        dprintf(("HMDeviceParPortClass::DeviceIoControl %s %x %d %x %d %x %x", msg, lpInBuffer, nInBufferSize,
                 lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped));
    }
#endif

    switch(dwIoControlCode)
    {
      case IOCTL_INTERNAL_GET_PARALLEL_PORT_INFO:
      {
        PPARALLEL_PORT_INFORMATION pPPI = (PPARALLEL_PORT_INFORMATION)lpOutBuffer;
        
        if(nOutBufferSize < sizeof(PARALLEL_PORT_INFORMATION) || !pPPI) 
        {
          SetLastError(ERROR_INSUFFICIENT_BUFFER);
          return FALSE;
        }
        
        if(lpBytesReturned)
          *lpBytesReturned = sizeof(PARALLEL_PORT_INFORMATION);
        
        // fill in the data values
        
        // Specifies the bus relative base I/O address of the parallel port registers.
        pPPI->OriginalController.LowPart = 0;  // @@@PH
        pPPI->OriginalController.HighPart = 0;  // @@@PH
        
        // Pointer to the system-mapped base I/O location of the parallel port registers.
        pPPI->Controller = 0;          // @@@PH
        
        // Specifies the size, in bytes, of the I/O space, allocated to the parallel port.
        pPPI->SpanOfController = 0;    // @@@PH
        
        // Pointer to a callback routine that a kernel-mode driver can use to try to allocate the parallel port.
        pPPI->TryAllocatePort = NULL;
        
        // Pointer to a callback routine that a kernel-mode driver can use to free the parallel port.
        pPPI->FreePort = NULL;
        
        // Pointer to a callback routine that a kernel-mode driver can use to determine the number of requests on the work queue of the parallel port.
        pPPI->QueryNumWaiters = NULL;
        
        // Pointer to the device extension of parallel port.
        pPPI->Context = NULL;

        return TRUE;
      }
      
      
      case IOCTL_INTERNAL_GET_PARALLEL_PNP_INFO:
      {
        PPARALLEL_PNP_INFORMATION pPPI = (PPARALLEL_PNP_INFORMATION)lpOutBuffer;
        
        if(nOutBufferSize < sizeof(PARALLEL_PNP_INFORMATION) || !pPPI)
        {
          SetLastError(ERROR_INSUFFICIENT_BUFFER);
          return FALSE;
        }
        
        if(lpBytesReturned)
          *lpBytesReturned = sizeof(PARALLEL_PNP_INFORMATION);
        
        // fill in the data values
        
        // Specifies the base physical address that the system-supplied function driver for parallel ports uses to control the ECP operation of the parallel port.
        pPPI->OriginalEcpController.LowPart = 0;
        pPPI->OriginalEcpController.HighPart = 0;
        
        // Pointer to the I/O port resource that is used to control the port in ECP mode.
        pPPI->EcpController = 0;
        
        // Specifies the size, in bytes, of the I/O port resource.
        pPPI->SpanOfEcpController = 0;
        
        // Not used.
        pPPI->PortNumber = 0;
        
        // Specifies the hardware capabilities of the parallel port. The following capabilities can be set using a bitwise OR of the following constants:
        pPPI->HardwareCapabilities = 0;
        //  PPT_1284_3_PRESENT
        //  PPT_BYTE_PRESENT
        //  PPT_ECP_PRESENT
        //  PPT_EPP_32_PRESENT
        //  PPT_EPP_PRESENT
        //  PT_NO_HARDWARE_PRESENT
        
        // Pointer to a callback routine that a kernel-mode driver can use to change the operating mode of the parallel port.
        pPPI->TrySetChipMode = 0;
        
        // Pointer to a callback routine that a kernel-mode driver can use to clear the operating mode of the parallel port.
        pPPI->ClearChipMode = 0;
        
        // Specifies the size, in words, of the hardware first in/first out (FIFO) buffer. The FIFO word size, in bits, is the value of FifoWidth.
        pPPI->FifoDepth = 0;
        
        // Specifies the FIFO word size, in bits, which is the number of bits handled in parallel.
        pPPI->FifoWidth = 0;
        
        // Not used.
        pPPI->EppControllerPhysicalAddress.LowPart = 0;
        pPPI->EppControllerPhysicalAddress.HighPart = 0;
        
        // Not used.
        pPPI->SpanOfEppController = 0;
        
        // Specifies the number of daisy-chain devices currently attached to a parallel port. In Microsoft� Windows� XP, from zero to two devices can be simultaneously connected to a
        // parallel port. In Windows 2000, from zero to four devices can be simultaneously connected to a parallel port.
        pPPI->Ieee1284_3DeviceCount = 0;
        
        // Pointer to a callback routine that a kernel-mode driver can use to try to select an IEEE 1284.3 device.
        pPPI->TrySelectDevice = 0;
        
        // Pointer to a callback routine that a kernel-mode driver can use to deselect an IEEE 1284.3 device.
        pPPI->DeselectDevice = 0;
        
        // Pointer to the device extension of a parallel port's function device object (FDO).
        pPPI->Context = 0;
        
        // The current operating mode of the parallel port.
        pPPI->CurrentMode = 0;
        
        // The symbolic link name of the parallel port.
        pPPI->PortName = 0;
        
        return TRUE;
      }
    }
    dprintf(("HMDeviceParPortClass::DeviceIoControl: unimplemented dwIoControlCode=%08lx\n", dwIoControlCode));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}


BOOL HMDeviceParPortClass::SetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                                LPCOMMCONFIG lpCC,
                                                DWORD dwSize)
{
  dprintf(("HMDeviceParPortClass::SetDefaultCommConfig(%08xh,%08xh,%08xh)\n",
           pHMHandleData,
           lpCC,
           dwSize));
  
  PHMDEVPARPORTDATA pDevData = (PHMDEVPARPORTDATA)pHMHandleData->lpDeviceData;
  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_PARPORT) )
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }
  memset(&pDevData->CommCfg,0, sizeof(COMMCONFIG));
  memcpy(&pDevData->CommCfg,lpCC,dwSize>sizeof(COMMCONFIG)?sizeof(COMMCONFIG):dwSize);

  return(TRUE);
}


BOOL HMDeviceParPortClass::GetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                                LPCOMMCONFIG lpCC,
                                                LPDWORD lpdwSize)
{
  dprintf(("HMDeviceParPortClass::GetDefaultCommConfig(%08xh,%08xh,%08xh)\n",
           pHMHandleData,
           lpCC,
           lpdwSize));

  
  PHMDEVPARPORTDATA pDevData = (PHMDEVPARPORTDATA)pHMHandleData->lpDeviceData;

  if( O32_IsBadWritePtr(lpCC,sizeof(COMMCONFIG)) ||
      *lpdwSize< sizeof(COMMCONFIG) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    *lpdwSize= sizeof(COMMCONFIG);
    return FALSE;
  }

  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_PARPORT) )
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  memcpy(lpCC,&pDevData->CommCfg,sizeof(COMMCONFIG));
  *lpdwSize = sizeof(COMMCONFIG);
  return(TRUE);
}


BOOL HMDeviceParPortClass::SetCommConfig( PHMHANDLEDATA pHMHandleData,
                                         LPCOMMCONFIG lpCC,
                                         DWORD dwSize )
{
  dprintf(("HMDeviceParPortClass::SetCommConfig not implemented"));

  return(TRUE);
}


BOOL HMDeviceParPortClass::GetCommConfig( PHMHANDLEDATA pHMHandleData,
                                         LPCOMMCONFIG lpCC,
                                         LPDWORD lpdwSize )
{
  PHMDEVPARPORTDATA pDevData = (PHMDEVPARPORTDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceParPortClass::GetCommConfig(%08xh,%08xh,%08xh)\n",
           pHMHandleData,
           lpCC,
           lpdwSize));

  if( O32_IsBadWritePtr(lpCC,sizeof(COMMCONFIG)) ||
      *lpdwSize< sizeof(COMMCONFIG) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    *lpdwSize= sizeof(COMMCONFIG);
    return FALSE;
  }

  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_PARPORT) )
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  memcpy(lpCC,&pDevData->CommCfg,sizeof(COMMCONFIG));
  *lpdwSize = sizeof(COMMCONFIG);
  return(TRUE);
}
