/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 */

#ifndef __HMMAILSLOT_H__
#define __HMMAILSLOT_H__


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include "HMDevice.h"
#include "HMOpen32.h"


#define MAILSLOT_SIZE		65536

class HMMailSlotInfo
{
public:
  HMMailSlotInfo(LPCSTR lpszName, HANDLE hPipe, DWORD nMaxMessageSize, 
                 DWORD lReadTimeout, BOOL fServer,
                 LPSECURITY_ATTRIBUTES lpSecurityAttributes);
 ~HMMailSlotInfo();

  LPSTR lpszName;
  LPSECURITY_ATTRIBUTES lpSecurityAttributes;
  LPVOID lpMessage;
  DWORD  dwMessageCount;
  DWORD  nMaxMessageSize;
  DWORD  lReadTimeout;
  BOOL   fServer;
  HANDLE hPipe;
};

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

class HMMailslotClass : public HMDeviceHandler
{
public:
  HMMailslotClass(LPCSTR lpDeviceName);

  virtual BOOL FindDevice(LPCSTR lpClassDevName, LPCSTR lpDeviceName, int namelength);

          BOOL CreateMailslotA(PHMHANDLEDATA pHMHandleData,
                               LPCSTR lpName, DWORD nMaxMessageSize,
                               DWORD lReadTimeout,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes);

  virtual BOOL GetMailslotInfo(PHMHANDLEDATA pHMHandleData,
                               LPDWORD lpMaxMessageSize,
                               LPDWORD lpNextSize,
                               LPDWORD lpMessageCount,
                               LPDWORD lpReadTimeout);

  virtual BOOL SetMailslotInfo(PHMHANDLEDATA pHMHandleData,
                               DWORD  dwReadTimeout);


  /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

  /* this is a handler method for calls to CloseHandle() */
  virtual BOOL   CloseHandle(PHMHANDLEDATA pHMHandleData);


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

};


#endif /* __HMMAILSLOT_H__ */

