/* $Id: hmcomm.h,v 1.8 2001-04-26 13:22:44 sandervl Exp $ */

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
typedef struct
{
  DWORD dwBaudRate;
  DWORD dwBaudFlag;
}BAUDTABLEENTRY;

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

                           /* this is a handler method for calls to ReadFile() */
  virtual BOOL   ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED  lpOverlapped);

                        /* this is a handler method for calls to ReadFileEx() */
  virtual BOOL  ReadFileEx(PHMHANDLEDATA pHMHandleData,
                           LPVOID       lpBuffer,
                           DWORD        nNumberOfBytesToRead,
                           LPOVERLAPPED lpOverlapped,
                           LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

                        /* this is a handler method for calls to WriteFile() */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped);

                        /* this is a handler method for calls to WriteFileEx() */
  virtual BOOL  WriteFileEx(PHMHANDLEDATA pHMHandleData,
                            LPVOID       lpBuffer,
                            DWORD        nNumberOfBytesToWrite,
                            LPOVERLAPPED lpOverlapped,
                            LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

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
};


#endif // _HM_COMM_H_
