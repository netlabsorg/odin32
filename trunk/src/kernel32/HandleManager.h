/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 Unified Handle Manager for OS/2
 *
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) HandleManager.Cpp       1.0.0   1998/02/11 PH start
 */

#ifndef _HANDLEMANAGER_H_
#define _HANDLEMANAGER_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 1998/02/11 PH The interface to the handle manager is twofold:
               a) HMxxx routines to be called from the "top" from the
                  KERNEL32 stubs
               b) the HMDeviceHandler class is actually a workaround for
                  the device driver that would handle the requests on Win32.
                  To implement a new pseudo-device, one has create a new
                  HMDeviceHandler class and link it into the table in the
                  HANDLEMANAGER.CPP file.
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#ifdef _OS2WIN_H
#include <winos2def.h>
#else
typedef struct {
    DWORD    Internal;
    DWORD    InternalHigh;
    DWORD    Offset;
    DWORD    OffsetHigh;
    HANDLE   hEvent;
} OVERLAPPED, *POVERLAPPED, *LPOVERLAPPED;
#endif


/*****************************************************************************
 * Defines & Macros                                                          *
 *****************************************************************************/

      /* all handles to our special pseudo-devices are ORed with this define */
   /* this allows us to determine quickly how to where to route requests to. */
#define HM_HANDLE_ID   0xDEAD0000
#define HM_HANDLE_MASK 0x0000FFFF


#define IS_HM_HANDLE(hHandle) ( (hHandle & ~HM_HANDLE_MASK) == HM_HANDLE_ID )


                                     /* 1998/02/12 PH Correction of os2win.h */
#undef FILE_TYPE_UNKNOWN
#define FILE_TYPE_UNKNOWN   0x0000

#undef FILE_TYPE_DISK
#define FILE_TYPE_DISK      0x0001

#undef FILE_TYPE_CHAR
#define FILE_TYPE_CHAR      0x0002

#undef FILE_TYPE_PIPE
#define FILE_TYPE_PIPE      0x0003

#undef FILE_TYPE_REMOTE
#define FILE_TYPE_REMOTE    0x8000



/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct _HMHANDLEDATA
{
  HANDLE          hHandle;                           /* a copy of the handle */

  DWORD           dwType;                          /* handle type identifier */

  DWORD           dwAccess;                     /* access mode of the handle */
  DWORD           dwShare;                       /* share mode of the handle */
  DWORD           dwCreation;                       /* dwCreationDisposition */
  DWORD           dwFlags;                           /* flags and attributes */

  LPVOID          lpHandlerData;    /* for private use of the device handler */
} HMHANDLEDATA, *PHMHANDLEDATA;


#ifdef __cplusplus
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
};

#endif


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

DWORD  HMInitialize(void);          /* initialize the HandleManager */

DWORD  HMTerminate(void);            /* terminate the HandleManager */


#ifdef __cplusplus
                            /* register a new device with the handle manager */
DWORD  HMDeviceRegister(PSZ pszDeviceName, HMDeviceHandler *pDeviceHandler);
#endif



 /* handle manager version of GetStdHandle, Open32 can't really help us here */
HANDLE  HMGetStdHandle(DWORD nStdHandle);

 /* handle manager version of GetStdHandle, Open32 can't really help us here */
BOOL    HMSetStdHandle(DWORD  nStdHandle,
                               HANDLE hHandle);

                       /* this is a handler method for calls to CreateFile() */
HANDLE  HMCreateFile (LPCSTR       lpFileName,
                              DWORD        dwDesiredAccess,
                              DWORD        dwShareMode,
                              PVOID        lpSecurityAttributes,
                              DWORD        dwCreationDisposition,
                              DWORD        dwFlagsAndAttributes,
                              HANDLE       hTemplateFile);

                      /* this is a handler method for calls to CloseHandle() */
BOOL    HMCloseHandle(HANDLE       hObject);

                         /* this is a handler method for calls to ReadFile() */
BOOL    HMReadFile   (HANDLE       hFile,
                              LPCVOID      lpBuffer,
                              DWORD        nNumberOfBytesToRead,
                              LPDWORD      lpNumberOfBytesRead,
                              LPOVERLAPPED lpOverlapped);

                        /* this is a handler method for calls to WriteFile() */
BOOL    HMWriteFile  (HANDLE       hFile,
                              LPCVOID      lpBuffer,
                              DWORD        nNumberOfBytesToWrite,
                              LPDWORD      lpNumberOfBytesWritten,
                              LPOVERLAPPED lpOverlapped);

                      /* this is a handler method for calls to GetFileType() */
DWORD   HMGetFileType(HANDLE       hFile);

                              /* this is for special non-standard device I/O */
DWORD   HMDeviceRequest (HANDLE hFile,
                                 ULONG  ulRequestCode,
                                 ULONG  arg1,
                                 ULONG  arg2,
                                 ULONG  arg3,
                                 ULONG  arg4);


/*****************************************************************************/
/* handle translation buffer management                                      */
/*                                                                           */
/* Since some Win32 applications rely (!) on 16-bit handles, we've got to do */
/* 32-bit to 16-bit and vs vsa translation here.                             */
/* Filehandle-based functions should be routed via the handlemanager instead */
/* of going to Open32 directly.                                              */
/*****************************************************************************/

DWORD  HMHandleAllocate       (PULONG phHandle16,
                                       ULONG  hHandle32);

DWORD  HMHandleFree           (ULONG  hHandle16);

DWORD  HMHandleValidate       (ULONG  hHandle16);

DWORD  HMHandleTranslateToWin (ULONG  hHandle32,
                                       PULONG phHandle16);

DWORD  HMHandleTranslateToOS2 (ULONG  hHandle16,
                                       PULONG hHandle32);

DWORD  HMHandleTranslateToOS2i(ULONG  hHandle16);


/*****************************************************************************
 * Forwarders                                                                *
 *****************************************************************************/


#endif /* _HANDLEMANAGER_H_ */
