/* $Id: hmcomm.h,v 1.14 2002-06-11 16:36:06 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 COM device access class
 *
 * 1999 Achim Hasenmueller <achimha@innotek.de>
 *
 */

#ifndef _HM_COMM_H_
#define _HM_COMM_H_

#include <handlemanager.h>
#include "hmdevice.h"
#include <overlappedio.h>

#define MAX_COMPORTS        8
#define MAGIC_COM           0x12abcd34

#define TIMEOUT_COMM        10

typedef struct
{
  DWORD dwBaudRate;
  DWORD dwBaudFlag;
}BAUDTABLEENTRY;

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

#define IOCTL_GENERAL        0x0B
#define DEV_FLUSHINPUT       0x01
#define DEV_FLUSHOUTPUT      0x02


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
  ULONG                 ulMagic;
  // Win32 Device Control Block
  COMMCONFIG            CommCfg;
  COMMTIMEOUTS          CommTOuts;
  DWORD                 dwInBuffer;
  DWORD                 dwOutBuffer;
  DWORD                 dwEventMask;
  OverlappedIOHandler  *iohandler;
  //OS/2 Device Control Block
  DCBINFO               dcbOS2;
} HMDEVCOMDATA, *PHMDEVCOMDATA;

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



class HMDeviceCommClass : public HMDeviceHandler
{
  public:

  HMDeviceCommClass(LPCSTR lpDeviceName);

  //checks if device name belongs to this class
  virtual BOOL FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength);

  /* this is the handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

  /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);

  /* this is the handler method for calls to CloseHandle() */
  virtual BOOL CloseHandle(PHMHANDLEDATA pHMHandleData);

  /* this is the handler method for SetComm() */
  virtual BOOL WaitCommEvent( PHMHANDLEDATA pHMHandleData,
                              LPDWORD lpfdwEvtMask,
                              LPOVERLAPPED lpo);

  virtual BOOL GetCommProperties( PHMHANDLEDATA pHMHandleData,
                                  LPCOMMPROP lpcmmp);
  virtual BOOL GetCommMask( PHMHANDLEDATA pHMHandleData,
                            LPDWORD lpfdwEvtMask);
  virtual BOOL SetCommMask( PHMHANDLEDATA pHMHandleData,
                            DWORD fdwEvtMask);
  virtual BOOL PurgeComm( PHMHANDLEDATA pHMHandleData,
                          DWORD fdwAction);
  virtual BOOL ClearCommError( PHMHANDLEDATA pHMHandleData,
                               LPDWORD lpdwErrors,
                               LPCOMSTAT lpcst);
  virtual BOOL SetCommState( PHMHANDLEDATA pHMHandleData,
                             LPDCB lpdcb) ;
  virtual BOOL GetCommState( PHMHANDLEDATA pHMHandleData,
                             LPDCB lpdcb);
  virtual BOOL GetCommModemStatus( PHMHANDLEDATA pHMHandleData,
                                   LPDWORD lpModemStat );
  virtual BOOL GetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                                LPCOMMTIMEOUTS lpctmo);
  virtual BOOL SetCommTimeouts( PHMHANDLEDATA pHMHandleData,
                                LPCOMMTIMEOUTS lpctmo);
  virtual BOOL TransmitCommChar( PHMHANDLEDATA pHMHandleData,
                                 CHAR cChar );
  virtual BOOL SetCommConfig( PHMHANDLEDATA pHMHandleData,
                              LPCOMMCONFIG lpCC,
                              DWORD dwSize );
  virtual BOOL SetCommBreak( PHMHANDLEDATA pHMHandleData );
  virtual BOOL GetCommConfig( PHMHANDLEDATA pHMHandleData,
                              LPCOMMCONFIG lpCC,
                              LPDWORD lpdwSize );
  virtual BOOL EscapeCommFunction( PHMHANDLEDATA pHMHandleData,
                                   UINT dwFunc );
  virtual BOOL SetupComm( PHMHANDLEDATA pHMHandleData,
                          DWORD dwInQueue,
                          DWORD dwOutQueue);
  virtual BOOL ClearCommBreak( PHMHANDLEDATA pHMHandleData);
  virtual BOOL SetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                     LPCOMMCONFIG lpCC,
                                     DWORD dwSize);
  virtual BOOL GetDefaultCommConfig( PHMHANDLEDATA pHMHandleData,
                                     LPCOMMCONFIG lpCC,
                                     LPDWORD lpdwSize);

  /* this is a handler method for calls to ReadFile/Ex */
  virtual BOOL   ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  /* this is a handler method for calls to WriteFile/Ex */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  virtual BOOL   CancelIo           (PHMHANDLEDATA pHMHandleData);

                /* this is a handler method for calls to GetOverlappedResult */
  virtual BOOL GetOverlappedResult(PHMHANDLEDATA pHMHandleData,
                                   LPOVERLAPPED  arg2,
                                   LPDWORD       arg3,
                                   BOOL          arg4);

  static void CloseOverlappedIOHandlers();

  private:

  APIRET SetLine( PHMHANDLEDATA pHMHandleData,
                  UCHAR ucSize,UCHAR Parity, UCHAR Stop);
  APIRET SetOS2DCB( PHMHANDLEDATA pHMHandleData,
                    BOOL fOutxCtsFlow, BOOL fOutxDsrFlow,
                    UCHAR ucDtrControl,  BOOL fDsrSensitivity,
                    BOOL fTXContinueOnXoff, BOOL fOutX,
                    BOOL fInX, BOOL fErrorChar,
                    BOOL fNull, UCHAR ucRtsControl,
                    BOOL fAbortOnError, BYTE XonChar,
                    BYTE XoffChar,BYTE ErrorChar);
  APIRET SetBaud( PHMHANDLEDATA pHMHandleData,
                  DWORD dwNewBaud);


    static OverlappedIOHandler *handler[MAX_COMPORTS];
};


#endif // _HM_COMM_H_
