/* $Id: hmcomm.cpp,v 1.9 2001-01-29 23:42:25 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 COM device access class
 *
 * 1999 Achim Hasenmueller <achimha@innotek.de>
 *
 */



#include <os2win.h>
#include <string.h>
#include <handlemanager.h>
#include <heapstring.h>
#include "hmdevice.h"
#include "hmcomm.h"
#include "oslibdos.h"

#define DBG_LOCALLOG  DBG_hmcomm
#include "dbglocal.h"

#define MAGIC_COM 0x12abcd34

#define IOCTL_ASYNC          0x01
#define ASYNC_GETDCBINFO     0x73
#define ASYNC_SETDCBINFO     0x53
#define ASYNC_SETLINECTRL    0x42
#define ASYNC_GETCOMMEVENT   0x72
#define ASYNC_EXTGETBAUDRATE 0x63
#define ASYNC_EXTSETBAUDRATE 0x43
#define ASYNC_GETCOMMERROR   0x6D
#define ASYNC_GETCOMMSTATUS  0x65
#define ASYNC_GETINQUECOUNT  0x68
#define ASYNC_GETOUTQUECOUNT 0x69
#define ASYNC_GETMODEMINPUT  0x67
#define ASYNC_TRANSMITIMM    0x44
#define ASYNC_SETBREAKON     0x4B
#define ASYNC_SETBREAKOFF    0x45
#define ASYNC_SETMODEMCTRL   0x46
#define ASYNC_STARTTRANSMIT  0x48
#define ASYNC_STOPTRANSMIT   0x47
#define ASYNC_GETMODEMOUTPUT 0x66


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





typedef struct _HMDEVCOMDATA
{
  ULONG ulMagic;
  // Win32 Device Control Block
  COMMCONFIG   CommCfg;
  COMMTIMEOUTS CommTOuts;
  DWORD dwInBuffer, dwOutBuffer;
  DWORD dwEventMask;
  //OS/2 Device Control Block
  DCBINFO dcbOS2;
} HMDEVCOMDATA, *PHMDEVCOMDATA;

VOID *CreateDevData()
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

HMDeviceCommClass::HMDeviceCommClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName)
{
  VOID *pData;
  dprintf(("HMDeviceCommClass: Register COM1 to COM8 with Handle Manager\n"));
  pData = CreateDevData();
  if(pData!= NULL)
    HMDeviceRegisterEx("COM1", this, pData);
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
    if(namelength > 5)
        return FALSE;  //can't be com name

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

DWORD HMDeviceCommClass::CreateFile(LPCSTR lpFileName,
                                    PHMHANDLEDATA pHMHandleData,
                                    PVOID lpSecurityAttributes,
                                    PHMHANDLEDATA pHMHandleDataTemplate)
{
 char comname[6];

  dprintf(("HMComm: Serial communication port %s open request\n", lpFileName));

  if(strlen(lpFileName) > 5) {
    return -1;  //safety check (unnecessary..)
  }
  pHMHandleData->hHMHandle = 0;

  strcpy(comname, lpFileName);
  comname[4] = 0;   //get rid of : (if present) (eg COM1:)

  //AH: TODO parse Win32 security handles
  OSLibDosDisableHardError(TRUE);
  pHMHandleData->hHMHandle = OSLibDosOpen(comname,
                                          OSLIB_ACCESS_READWRITE |
                                          OSLIB_ACCESS_SHAREDENYREAD |
                                          OSLIB_ACCESS_SHAREDENYWRITE);
  OSLibDosDisableHardError(FALSE);
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
}


                      /* this is a handler method for calls to CloseHandle() */
DWORD HMDeviceCommClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{
  dprintf(("HMComm: Serial communication port close request\n"));
  delete pHMHandleData->lpHandlerData;
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

BOOL HMDeviceCommClass::SetupComm( PHMHANDLEDATA pHMHandleData,
                                   DWORD dwInQueue,
                                   DWORD dwOutQueue)
{
  dprintf(("HMDeviceCommClass::SetupComm "));
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }
  pDevData->dwInBuffer  = dwInQueue;
  pDevData->dwOutBuffer = dwOutQueue;

  return(TRUE);
}

BOOL HMDeviceCommClass::WaitCommEvent( PHMHANDLEDATA pHMHandleData,
                                       LPDWORD lpfdwEvtMask,
                                       LPOVERLAPPED lpo)
{
  APIRET rc;
  ULONG ulLen;
  USHORT COMEvt;
  DWORD dwEvent,dwMask;

  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::WaitCommEvent"));
  ulLen = sizeof(CHAR);

  dwEvent = 0;
  rc = 0;
  ulLen = sizeof(COMEvt);
  dwMask = pDevData->dwEventMask;
  while( (0==rc) &&
        !(dwEvent & dwMask) &&
        (dwMask ==pDevData->dwEventMask) ) // Exit if the Mask gets changed
  {
    rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
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
      //dwEvent |= (COMEvt&0x0020)? 0:0; DCS = RLSD?
      dwEvent |= (COMEvt&0x0040)? EV_BREAK:0;
      dwEvent |= (COMEvt&0x0080)? EV_ERR:0;
      dwEvent |= (COMEvt&0x0100)? EV_RING:0;
    }
    DosSleep(100);
  }
  *lpfdwEvtMask = rc==0?dwEvent:0;
  return(rc==0);
}


#pragma pack(1)
typedef struct
{
  ULONG ulCurrBaud;
  UCHAR ucCurrFrac;
  ULONG ulMinBaud;
  UCHAR ucMinFrac;
  ULONG ulMaxBaud;
  UCHAR ucMaxFrac;
} EXTBAUDGET, *PEXTBAUDGET;

typedef struct
{
  ULONG ulBaud;
  UCHAR ucFrac;
} EXTBAUDSET, *PEXTBAUDSET;
#pragma pack()

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

BOOL HMDeviceCommClass::GetCommProperties( PHMHANDLEDATA pHMHandleData,
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

BOOL HMDeviceCommClass::GetCommMask( PHMHANDLEDATA pHMHandleData,
                                     LPDWORD lpfdwEvtMask)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::GetCommMask"));

  *lpfdwEvtMask = pDevData->dwEventMask;
  return(TRUE);
}

BOOL HMDeviceCommClass::SetCommMask( PHMHANDLEDATA pHMHandleData,
                                     DWORD fdwEvtMask)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  dprintf(("HMDeviceCommClass::SetCommMask"));

  pDevData->dwEventMask = fdwEvtMask & ~(EV_RLSD|EV_RXFLAG); // Clear the 2 not supported Flags.
  return(TRUE);
}

BOOL HMDeviceCommClass::PurgeComm( PHMHANDLEDATA pHMHandleData,
                                   DWORD fdwAction)
{
  dprintf(("HMDeviceCommClass::PurgeComm (flags 0x%x) unimplemented stub!",fdwAction));
  // ToDo: find a way to stop the current transmision didn't find
  // any clue how to in Control Program Guide and reference

  return(TRUE);
}
BOOL HMDeviceCommClass::ClearCommError( PHMHANDLEDATA pHMHandleData,
                                        LPDWORD lpdwErrors,
                                        LPCOMSTAT lpcst)
{
  APIRET rc;
  ULONG ulLen;
  USHORT COMErr;

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
BOOL HMDeviceCommClass::SetCommState( PHMHANDLEDATA pHMHandleData,
                                      LPDCB lpDCB)
{
  APIRET rc;
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  DCB *pCurDCB = &pDevData->CommCfg.dcb;
  dprintf(("HMDeviceCommClass::SetCommState"));

  rc = 0;
  if(pCurDCB->BaudRate != lpDCB->BaudRate)
    rc = SetBaud( pHMHandleData,
                  lpDCB->BaudRate);

  if(!rc)
  {
    if( (pCurDCB->ByteSize != lpDCB->ByteSize) ||
        (pCurDCB->Parity   != lpDCB->Parity) ||
        (pCurDCB->StopBits != lpDCB->StopBits))
      rc = SetLine( pHMHandleData,
                    lpDCB->ByteSize,
                    lpDCB->Parity,
                    lpDCB->StopBits);
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
      SetOS2DCB( pHMHandleData,
                 lpDCB->fOutxCtsFlow, lpDCB->fOutxDsrFlow,
                 lpDCB->fDtrControl,  lpDCB->fDsrSensitivity,
                 lpDCB->fTXContinueOnXoff, lpDCB->fOutX,
                 lpDCB->fInX, lpDCB->fErrorChar,
                 lpDCB->fNull, lpDCB->fRtsControl,
                 lpDCB->fAbortOnError, lpDCB->XonChar,
                 lpDCB->XoffChar,lpDCB->ErrorChar);
  }

  return(rc==0);
}
BOOL HMDeviceCommClass::GetCommState( PHMHANDLEDATA pHMHandleData,
                                      LPDCB lpdcb)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::GetCommState"));
  memcpy(lpdcb,&pDevData->CommCfg.dcb,sizeof(DCB));

  return(TRUE);
}
BOOL HMDeviceCommClass::GetCommModemStatus( PHMHANDLEDATA pHMHandleData,
                                          LPDWORD lpModemStat )
{
  APIRET rc;
  ULONG ulLen;
  USHORT COMErr;
  UCHAR ucStatus;

  dprintf(("HMDeviceCommClass::TransmitCommChar partly implemented"));
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
    //*lpModemStat |= (ucStatus & 0x80)? MS_RSLD_ON:0;
  }

  return(rc==0);
}

BOOL HMDeviceCommClass::GetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                                         LPCOMMTIMEOUTS lpctmo)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::GetCommTimeouts stub"));
  memcpy( lpctmo,
          &pDevData->CommTOuts,
          sizeof(COMMTIMEOUTS));
  return(TRUE);
}
BOOL HMDeviceCommClass::SetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                                         LPCOMMTIMEOUTS lpctmo)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;
  DCBINFO os2dcb;
  ULONG ulLen;
  APIRET rc;
  UCHAR fbTimeOut;
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
#if 0
    if(dwTimeout)
      dwTimeout--; // 0=10 ms unit is 10ms or .01s
#endif
    os2dcb.usWriteTimeout = 0x0000FFFF & dwTimeout;
    os2dcb.usReadTimeout  = 0x0000FFFF & dwTimeout;
  }
  os2dcb.fbTimeOut = (os2dcb.fbTimeOut & 0xF9) | fbTimeOut;

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
                                  LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32:HMDeviceCommClass::WriteFile %s(%08x,%08x,%08x,%08x,%08x)",
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

BOOL HMDeviceCommClass::WriteFileEx(PHMHANDLEDATA pHMHandleData,
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
                                 LPOVERLAPPED  lpOverlapped)
{
  dprintf(("KERNEL32:HMDeviceCommClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)",
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
BOOL HMDeviceCommClass::ReadFileEx(PHMHANDLEDATA pHMHandleData,
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

BOOL HMDeviceCommClass::SetCommConfig( PHMHANDLEDATA pHMHandleData,
                                       LPCOMMCONFIG lpCC,
                                       DWORD dwSize )
{
  dprintf(("HMDeviceCommClass::SetCommConfig"));


  return(TRUE);
}

BOOL HMDeviceCommClass::GetCommConfig( PHMHANDLEDATA pHMHandleData,
                                       LPCOMMCONFIG lpCC,
                                       LPDWORD lpdwSize )
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpHandlerData;

  dprintf(("HMDeviceCommClass::GetCommConfig"));

  if( O32_IsBadWritePtr(lpCC,sizeof(COMMCONFIG)) ||
      *lpdwSize< sizeof(COMMCONFIG) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    *lpdwSize= sizeof(COMMCONFIG);
    return FALSE;
  }

  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  memcpy(lpCC,&pDevData->CommCfg,sizeof(COMMCONFIG));
  *lpdwSize = sizeof(COMMCONFIG);
  return(TRUE);


  return(TRUE);
}

BOOL HMDeviceCommClass::EscapeCommFunction( PHMHANDLEDATA pHMHandleData,
                                            UINT dwFunc )
{
  APIRET rc;
  ULONG ulDLen,ulPLen;
  USHORT COMErr;
  MODEMSTATUS mdm;

  dprintf(("HMDeviceCommClass::EscapeCommFunction"));

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
      break;
    case SETDTR:
      mdm.fbModemOn  = 0x01;
      mdm.fbModemOff = 0XFF;
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_SETMODEMCTRL,
                             &mdm,ulPLen,&ulPLen,
                             &COMErr,ulDLen,&ulDLen);
      break;
    case SETRTS:
      mdm.fbModemOn  = 0x02;
      mdm.fbModemOff = 0XFF;
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_SETMODEMCTRL,
                             &mdm,ulPLen,&ulPLen,
                             &COMErr,ulDLen,&ulDLen);
      break;
    case SETXOFF:
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_STOPTRANSMIT,
                             0,0,0,
                             0,0,0);
      break;
    case SETXON:
      rc = OSLibDosDevIOCtl( pHMHandleData->hHMHandle,
                             IOCTL_ASYNC,
                             ASYNC_STARTTRANSMIT,
                             0,0,0,
                             0,0,0);
      break;
    default:
      SetLastError(ERROR_INVALID_PARAMETER);
      return(FALSE);
  }

  return(rc==0);
}

BOOL HMDeviceCommClass::SetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                              LPCOMMCONFIG lpCC,
                                              DWORD dwSize)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpDeviceData;
  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }
  memset(&pDevData->CommCfg,0, sizeof(COMMCONFIG));
  memcpy(&pDevData->CommCfg,lpCC,dwSize>sizeof(COMMCONFIG)?sizeof(COMMCONFIG):dwSize);

  return(TRUE);
}
BOOL HMDeviceCommClass::GetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                              LPCOMMCONFIG lpCC,
                                              LPDWORD lpdwSize)
{
  PHMDEVCOMDATA pDevData = (PHMDEVCOMDATA)pHMHandleData->lpDeviceData;

  if( O32_IsBadWritePtr(lpCC,sizeof(COMMCONFIG)) ||
      *lpdwSize< sizeof(COMMCONFIG) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    *lpdwSize= sizeof(COMMCONFIG);
    return FALSE;
  }

  if((NULL==pDevData) || (pDevData->ulMagic != MAGIC_COM) )
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
  }

  memcpy(lpCC,&pDevData->CommCfg,sizeof(COMMCONFIG));
  *lpdwSize = sizeof(COMMCONFIG);
  return(TRUE);
}
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

  return rc;
}

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
  }

  return rc;

}

APIRET HMDeviceCommClass::SetBaud( PHMHANDLEDATA pHMHandleData,
                                   DWORD dwNewBaud)
{
  APIRET rc;
  ULONG ulLen;
  EXTBAUDSET SetBaud;
  EXTBAUDGET GetBaud;
  ulLen = sizeof(SetBaud);
  SetBaud.ulBaud = dwNewBaud;
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
      if(dwNewBaud !=GetBaud.ulCurrBaud)
        rc = 1; // ToDo set a proper Errorhandling
      else
      {
        ((PHMDEVCOMDATA)pHMHandleData->lpDeviceData)->CommCfg.dcb.BaudRate = dwNewBaud;
        ((PHMDEVCOMDATA)pHMHandleData->lpDeviceData)->CommCfg.dcb.BaudRate = dwNewBaud;
      }
    }
  }
  return rc;
}


