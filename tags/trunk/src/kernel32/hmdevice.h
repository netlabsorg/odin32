/* $Id: hmdevice.h,v 1.2 1999-07-05 09:58:14 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Unified Handle Manager for OS/2
 * 1999/06/17 PH Patrick Haller (phaller@gmx.net)
 */

#ifndef _HM_DEVICE_H_
#define _HM_DEVICE_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct _HMHANDLEDATA
{
  HANDLE          hHMHandle;             /* a copy of the OS/2 system handle */

  DWORD           dwType;                          /* handle type identifier */

  DWORD           dwAccess;                     /* access mode of the handle */
  DWORD           dwShare;                       /* share mode of the handle */
  DWORD           dwCreation;                       /* dwCreationDisposition */
  DWORD           dwFlags;                           /* flags and attributes */

  LPVOID          lpHandlerData;    /* for private use of the device handler */
} HMHANDLEDATA, *PHMHANDLEDATA;



class HMDeviceHandler
{
  /***************************************************************************
   * The following methods are called by the handle manager request router.  *
   * They are exact replacements for the corresponding Win32 calls.          *
   ***************************************************************************/

public:
  LPCSTR lpHMDeviceName;                   /* a reference to the device name */

  HMDeviceHandler(LPCSTR lpDeviceName);      /* constructor with device name */


  /***********************************
   * handle generic standard methods *
   ***********************************/

  /* this is a special internal method to handle non-standard requests       */
  /* such as GetConsoleMode() for console devices                            */
  virtual DWORD  _DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                 ULONG         ulRequestCode,
                                 ULONG         arg1,
                                 ULONG         arg2,
                                 ULONG         arg3,
                                 ULONG         arg4);

                       /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

                       /* this is a handler method for calls to   OpenFile() */
  virtual DWORD  OpenFile   (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             OFSTRUCT*     pOFStruct,
                             UINT          fuMode);

                      /* this is a handler method for calls to CloseHandle() */
  virtual DWORD  CloseHandle(PHMHANDLEDATA pHMHandleData);

                           /* this is a handler method for calls to ReadFile() */
  virtual DWORD  ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED  lpOverlapped);

                        /* this is a handler method for calls to WriteFile() */
  virtual DWORD  WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED  lpOverlapped);

                      /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);


       /* this is a handler method for calls to GetFileInformationByHandle() */
  virtual DWORD GetFileInformationByHandle(PHMHANDLEDATA pHMHandleData,
                                           BY_HANDLE_FILE_INFORMATION *pHFI);

                     /* this is a handler method for calls to SetEndOfFile() */
  virtual BOOL  SetEndOfFile(PHMHANDLEDATA pHMHandleData);

                      /* this is a handler method for calls to SetFileTime() */
  virtual BOOL  SetFileTime (PHMHANDLEDATA pHMHandleData,
                             LPFILETIME    pFT1,
                             LPFILETIME    pFT2,
                             LPFILETIME    pFT3);

                      /* this is a handler method for calls to GetFileSize() */
  virtual DWORD GetFileSize(PHMHANDLEDATA pHMHandleData,
                            PDWORD        pSizeHigh);

                   /* this is a handler method for calls to SetFilePointer() */
  virtual DWORD SetFilePointer(PHMHANDLEDATA pHMHandleData,
                               LONG          lDistanceToMove,
                               PLONG         lpDistanceToMoveHigh,
                               DWORD         dwMoveMethod);

                         /* this is a handler method for calls to LockFile() */
  virtual DWORD LockFile(PHMHANDLEDATA pHMHandleData,
                         DWORD         arg2,
                         DWORD         arg3,
                         DWORD         arg4,
                         DWORD         arg5);

                       /* this is a handler method for calls to LockFileEx() */
  virtual DWORD LockFileEx(PHMHANDLEDATA pHMHandleData,
                           DWORD         dwFlags,
                           DWORD         dwReserved,
                           DWORD         nNumberOfBytesToLockLow,
                           DWORD         nNumberOfBytesToLockHigh,
                           LPOVERLAPPED  lpOverlapped);

                       /* this is a handler method for calls to UnlockFile() */
  virtual DWORD UnlockFile(PHMHANDLEDATA pHMHandleData,
                           DWORD         arg2,
                           DWORD         arg3,
                           DWORD         arg4,
                           DWORD         arg5);

                     /* this is a handler method for calls to UnlockFileEx() */
  virtual DWORD UnlockFileEx(PHMHANDLEDATA pHMHandleData,
                             DWORD         dwFlags,
                             DWORD         dwReserved,
                             DWORD         nNumberOfBytesToLockLow,
                             DWORD         nNumberOfBytesToLockHigh,
                             LPOVERLAPPED  lpOverlapped);
};


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

                            /* register a new device with the handle manager */
DWORD  HMDeviceRegister(PSZ             pszDeviceName,
                        HMDeviceHandler *pDeviceHandler);


#endif /* _HM_DEVICE_H_ */

