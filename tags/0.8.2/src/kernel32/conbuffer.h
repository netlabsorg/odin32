/* $Id: conbuffer.h,v 1.6 2003-04-02 12:58:28 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Console Subsystem for OS/2
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 */

#ifndef _CONSOLE_BUFFER_H_
#define _CONSOLE_BUFFER_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Resources                                                                 *
 *****************************************************************************/


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

//#include <win32type.h>
#include "handlemanager.h"
#include "HMDevice.h"
#include "console2.h"


/*****************************************************************************
 * ConsoleBuffer - does all the writing to an internal line buffer array     *
 *                 if this buffer is the active one, also does a screen updt *
 *****************************************************************************/

class HMDeviceConsoleBufferClass : public HMDeviceHandler
{
  protected:
    PICONSOLEINPUT   pConsoleInput;         /* pointer to console input area */
    PICONSOLEGLOBALS pConsoleGlobals;      /* pointer to console global area */

  public:
    HMDeviceConsoleBufferClass(LPCSTR           lpDeviceName,
                               PICONSOLEINPUT   piConsoleInput,
                               PICONSOLEGLOBALS piConsoleGlobals)
      : HMDeviceHandler(lpDeviceName)
    {
      pConsoleInput   = piConsoleInput;          // save pointers to this data
      pConsoleGlobals = piConsoleGlobals;
    }

                       /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);

                           /* this is a handler method for calls to ReadFile() */
  virtual BOOL   ReadFile   (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToRead,
                             LPDWORD       lpNumberOfBytesRead,
                             LPOVERLAPPED lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

                        /* this is a handler method for calls to WriteFile() */
  virtual BOOL   WriteFile  (PHMHANDLEDATA pHMHandleData,
                             LPCVOID       lpBuffer,
                             DWORD         nNumberOfBytesToWrite,
                             LPDWORD       lpNumberOfBytesWritten,
                             LPOVERLAPPED lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

  /* this is a handler method for calls to GetFileType() */
  virtual DWORD GetFileType (PHMHANDLEDATA pHMHandleData);

  virtual BOOL  CloseHandle(PHMHANDLEDATA pHMHandleData);

  virtual DWORD  _DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                 ULONG         ulRequestCode,
                                 ULONG         arg1,
                                 ULONG         arg2,
                                 ULONG         arg3,
                                 ULONG         arg4);


  /************************************
   * non-standard console I/O methods *
   ************************************/

  virtual DWORD  FillConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                            WORD          wAttribute,
                                            DWORD         nLength,
                                            COORD         dwWriteCoord,
                                            LPDWORD       lpNumberOfAttrsWritten);

  virtual DWORD  FillConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                             UCHAR         ucCharacter,
                                             DWORD         nLength,
                                             COORD         dwWriteCoord,
                                             LPDWORD       lpNumberOfCharsWritten);

  virtual DWORD  FillConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                             WCHAR         wcCharacter,
                                             DWORD         nLength,
                                             COORD         dwWriteCoord,
                                             LPDWORD       lpNumberOfCharsWritten);

  virtual DWORD  GetConsoleCursorInfo(PHMHANDLEDATA        pHMHandleData,
                                      PCONSOLE_CURSOR_INFO pCCI);

  virtual DWORD  GetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                LPDWORD       lpMode);

  virtual DWORD  GetLargestConsoleWindowSize(PHMHANDLEDATA pHMHandleData);

  virtual DWORD  GetConsoleScreenBufferInfo(PHMHANDLEDATA               pHMHandleData,
                                            PCONSOLE_SCREEN_BUFFER_INFO pCSBI);

  virtual DWORD  ReadConsoleOutputA(PHMHANDLEDATA pHMHandleData,
                                    PCHAR_INFO    pchiDestBuffer,
                                    COORD         coordDestBufferSize,
                                    COORD         coordDestBufferCoord,
                                    PSMALL_RECT   psrctSourceRect);

  virtual DWORD  ReadConsoleOutputW(PHMHANDLEDATA pHMHandleData,
                                    PCHAR_INFO    pchiDestBuffer,
                                    COORD         coordDestBufferSize,
                                    COORD         coordDestBufferCoord,
                                    PSMALL_RECT   psrctSourceRect);

  virtual DWORD  ReadConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                            LPWORD        lpwAttribute,
                                            DWORD         cReadCells,
                                            COORD         dwReadCoord,
                                            LPDWORD       lpcNumberRead);

  virtual DWORD  ReadConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                             LPTSTR        lpwReadBuffer,
                                             DWORD         cchRead,
                                             COORD         coordReadCoord,
                                             LPDWORD       lpcNumberRead);

  virtual DWORD  ReadConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                             LPWSTR        lpwReadBuffer,
                                             DWORD         cchRead,
                                             COORD         coordReadCoord,
                                             LPDWORD       lpcNumberRead);

  virtual DWORD  ScrollConsoleScreenBufferA(PHMHANDLEDATA pHMHandleData,
                                            PSMALL_RECT   psrctSourceRect,
                                            PSMALL_RECT   psrctClipRect,
                                            COORD         coordDestOrigin,
                                            PCHAR_INFO    pchiFill);

  virtual DWORD  ScrollConsoleScreenBufferW(PHMHANDLEDATA pHMHandleData,
                                            PSMALL_RECT   psrctSourceRect,
                                            PSMALL_RECT   psrctClipRect,
                                            COORD         coordDestOrigin,
                                            PCHAR_INFO    pchiFill);

  virtual DWORD  SetConsoleScreenBufferSize (PHMHANDLEDATA pHMHandleData,
                                             COORD         coordSize);

  virtual DWORD  SetConsoleCursorInfo(PHMHANDLEDATA        pHMHandleData,
                                      PCONSOLE_CURSOR_INFO pCCI);

  virtual DWORD  SetConsoleCursorPosition(PHMHANDLEDATA pHMHandleData,
                                          COORD         coordCursorPosition);

  virtual DWORD  SetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                DWORD         dwMode);

  virtual DWORD  SetConsoleTextAttribute    (PHMHANDLEDATA pHMHandleData,
                                             WORD          wAttr);

  virtual DWORD  SetConsoleActiveScreenBuffer(PHMHANDLEDATA pHMHandleData);

  virtual BOOL   SetConsoleWindowInfo(PHMHANDLEDATA pHMHandleData,
                                      BOOL          fAbsolute,
                                      PSMALL_RECT   psrctWindowRect);

  virtual DWORD  WriteConsoleA(PHMHANDLEDATA pHMHandleData,
                               CONST VOID*   lpvBuffer,
                               DWORD         cchToWrite,
                               LPDWORD       lpcchWritten,
                               LPVOID        lpvReserved);

  virtual DWORD  WriteConsoleW(PHMHANDLEDATA pHMHandleData,
                               CONST VOID*   lpvBuffer,
                               DWORD         cchToWrite,
                               LPDWORD       lpcchWritten,
                               LPVOID        lpvReserved);

  virtual DWORD  WriteConsoleOutputA(PHMHANDLEDATA pHMHandleData,
                                     PCHAR_INFO    pchiSrcBuffer,
                                     COORD         coordSrcBufferSize,
                                     COORD         coordSrcBufferCoord,
                                     PSMALL_RECT   psrctDestRect);

  virtual DWORD  WriteConsoleOutputW(PHMHANDLEDATA pHMHandleData,
                                     PCHAR_INFO    pchiSrcBuffer,
                                     COORD         coordSrcBufferSize,
                                     COORD         coordSrcBufferCoord,
                                     PSMALL_RECT   psrctDestRect);

  virtual DWORD  WriteConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                             LPWORD        lpwAttribute,
                                             DWORD         cWriteCells,
                                             COORD         dwWriteCoord,
                                             LPDWORD       lpcWritten);

  virtual DWORD  WriteConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                              LPTSTR        lpWriteBuffer,
                                              DWORD         cchWrite,
                                              COORD         dwWriteCoord,
                                              LPDWORD       lpcWritten);

  virtual DWORD  WriteConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                              LPWSTR        lpWriteBuffer,
                                              DWORD         cchWrite,
                                              COORD         dwWriteCoord,
                                              LPDWORD       lpcWritten);
};

#endif /* _CONSOLE_BUFFER_H_ */

