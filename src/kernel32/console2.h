/*
 * Win32 Console Subsystem for OS/2
 *
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) HandleManager.Cpp       1.0.0	1998/02/11 PH start
 */

#ifndef _CONSOLESUBSYSTEM_H_
#define _CONSOLESUBSYSTEM_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 
 */
 

/*****************************************************************************
 * Resources                                                                 *
 *****************************************************************************/

#include "consolerc.h"


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

//#include <win32type.h>
#include "handlemanager.h"
#include <console.h>


/*****************************************************************************
 * Defines & Macros                                                          *
 *****************************************************************************/


#define CONSOLE_TEXTMODE_BUFFER 1


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/


typedef struct _ConsoleBuffer
{
      /* A console buffer is a indexed array of lines. This technique allows */
      /* much faster scrolling than a single large textbuffer.               */
  PSZ   *ppszLine;                  /* an array of line pointers             */
  ULONG ulScrollLineOffset;          /* offset to the 1st line in the buffer */
  
                /* the following structures reflect the Win32 API structures */
  CONSOLE_CURSOR_INFO        CursorInfo;
  COORD                      coordCursorPosition;
  
                                     /* the mode the console is currently in */
  DWORD                      dwConsoleMode;
  
                              /* information about the current screen buffer */
  //  CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
  COORD                      coordBufferSize;
  UCHAR                      ucDefaultAttribute;   /* default text attribute */
  COORD                      coordWindowPosition;       /* viewport position */
  COORD                      coordWindowSize;               /* viewport size */
} CONSOLEBUFFER, *PCONSOLEBUFFER;



/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

  
DWORD EXPORT ConsoleInit(void);        /* creation of the console subsystem */  
DWORD EXPORT ConsoleDevicesRegister(void);        /* create console devices */  
void  EXPORT ConsoleWaitClose(void);      /* wait for user to close console */
BOOL  EXPORT ConsoleIsActive(void);  /* returns TRUE if console window open */



BOOL   WIN32API OS2AllocConsole                  (VOID);

HANDLE WIN32API OS2CreateConsoleScreenBuffer     (DWORD         dwDesiredAccess,
                                                  DWORD         dwShareMode,
                                                  LPVOID        lpSecurityAttributes,
                                                  DWORD         dwFlags,
                                                  LPVOID        lpScreenBufferData);

BOOL   WIN32API OS2FillConsoleOutputAttribute    (HANDLE        hConsoleOutput,
                                                  WORD          wAttribute,
                                                  DWORD         nLength,
                                                  COORD         dwWriteCoord,
                                                  LPDWORD       lpNumberOfAttrsWritten);

BOOL   WIN32API OS2FillConsoleOutputCharacterW   (HANDLE        hConsoleOutput,
                                                  WCHAR         cCharacter,
                                                  DWORD         nLength,
                                                  COORD         dwWriteCoord,
                                                  LPDWORD       lpNumberOfCharsWritten);

BOOL   WIN32API OS2FillConsoleOutputCharacterA   (HANDLE        hConsoleOutput,
                                                  UCHAR         cCharacter,
                                                  DWORD         nLength,
                                                  COORD         dwWriteCoord,
                                                  LPDWORD       lpNumberOfCharsWritten);


BOOL   WIN32API OS2FlushConsoleInputBuffer       (HANDLE        hConsoleInput);

BOOL   WIN32API OS2FreeConsole                   (VOID);

BOOL   WIN32API OS2GenerateConsoleCtrlEvent      (DWORD         dwCtrlEvent,
                                                  DWORD         dwProcessGroupId);

UINT   WIN32API OS2GetConsoleCP                  (VOID);

BOOL   WIN32API OS2GetConsoleCursorInfo          (HANDLE               hConsoleOutput,
                                                  PCONSOLE_CURSOR_INFO lpConsoleCursorInfo);

BOOL   WIN32API OS2GetConsoleMode                (HANDLE        hConsole,
                                                  LPDWORD       lpMode);

UINT   WIN32API OS2GetConsoleOutputCP            (VOID);

BOOL   WIN32API OS2GetConsoleScreenBufferInfo    (HANDLE                      hConsoleOutput,
                                                  PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo);

DWORD  WIN32API OS2GetConsoleTitleA              (LPTSTR        lpConsoleTitle,
                                                  DWORD         nSize);

DWORD  WIN32API OS2GetConsoleTitleW              (LPTSTR        lpConsoleTitle,
                                                  DWORD         nSize);

COORD  WIN32API OS2GetLargestConsoleWindowSize   (HANDLE        hConsoleOutput);

BOOL   WIN32API OS2GetNumberOfConsoleInputEvents (HANDLE        hConsoleInput,
                                                  LPDWORD       lpNumberOfEvents);

BOOL   WIN32API OS2GetNumberOfConsoleMouseButtons(LPDWORD       lpcNumberOfMouseButtons);

BOOL   WIN32API OS2PeekConsoleInputA             (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcRead);

BOOL   WIN32API OS2PeekConsoleInputW             (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcRead);

BOOL   WIN32API OS2ReadConsoleA                  (HANDLE        hConsoleInput,
                                                  LPVOID        lpvBuffer,
                                                  DWORD         cchToRead,
                                                  LPDWORD       lpcchRead,
                                                  LPVOID        lpvReserved);

BOOL   WIN32API OS2ReadConsoleInputA             (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcRead);

BOOL   WIN32API OS2ReadConsoleInputW             (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcRead);

BOOL   WIN32API OS2ReadConsoleOutputA            (HANDLE        hConsoleOutput,
                                                  PCHAR_INFO    pchiDestBuffer,
                                                  COORD         coordDestBufferSize,
                                                  COORD         coordDestBufferCoord,
                                                  PSMALL_RECT   psrctSourceRect);

BOOL   WIN32API OS2ReadConsoleOutputW            (HANDLE        hConsoleOutput,
                                                  PCHAR_INFO    pchiDestBuffer,
                                                  COORD         coordDestBufferSize,
                                                  COORD         coordDestBufferCoord,
                                                  PSMALL_RECT   psrctSourceRect);

BOOL   WIN32API OS2ReadConsoleOutputAttribute    (HANDLE        hConsoleOutput,
                                                  LPWORD        lpwAttribute,
                                                  DWORD         cReadCells,
                                                  COORD         coordReadCoord,
                                                  LPDWORD       lpcNumberRead);

BOOL   WIN32API OS2ReadConsoleOutputCharacter    (HANDLE        hConsoleOutput,
                                                  LPTSTR        lpReadBuffer,
                                                  DWORD         cchRead,
                                                  COORD         coordReadCoord,
                                                  LPDWORD       lpcNumberRead);

BOOL   WIN32API OS2ScrollConsoleScreenBufferA    (HANDLE        hConsoleOutput,
                                                  PSMALL_RECT   psrctSourceRect,
                                                  PSMALL_RECT   psrctClipRect,
                                                  COORD         coordDestOrigin,
                                                  PCHAR_INFO    pchiFill);

BOOL   WIN32API OS2ScrollConsoleScreenBufferW    (HANDLE        hConsoleOutput,
                                                  PSMALL_RECT   psrctSourceRect,
                                                  PSMALL_RECT   psrctClipRect,
                                                  COORD         coordDestOrigin,
                                                  PCHAR_INFO    pchiFill);

BOOL   WIN32API OS2SetConsoleActiveScreenBuffer  (HANDLE        hConsoleOutput);

BOOL   WIN32API OS2SetConsoleCP                  (UINT          IDCodePage);

BOOL   WIN32API OS2SetConsoleCtrlHandler         (PHANDLER_ROUTINE pHandlerRoutine,
                                                  BOOL             fAdd);

BOOL   WIN32API OS2SetConsoleCursorInfo          (HANDLE               hConsoleOutput,
                                                  PCONSOLE_CURSOR_INFO lpConsoleCursorInfo);

BOOL   WIN32API OS2SetConsoleCursorPosition      (HANDLE        hConsoleOutput,
                                                  COORD         coordCursor);

BOOL   WIN32API OS2SetConsoleMode                (HANDLE        hConsole,
                                                  DWORD         fdwMode);

BOOL   WIN32API OS2SetConsoleOutputCP            (UINT          IDCodePage);

BOOL   WIN32API OS2SetConsoleScreenBufferSize    (HANDLE        hConsoleOutput,
                                                  COORD         coordSize);

BOOL   WIN32API OS2SetConsoleTextAttribute       (HANDLE        hConsoleOutput,
                                                  WORD          wAttr);

BOOL   WIN32API OS2SetConsoleTitleA              (LPTSTR        lpszTitle);

BOOL   WIN32API OS2SetConsoleTitleW              (LPTSTR        lpszTitle);

BOOL   WIN32API OS2SetConsoleWindowInfo          (HANDLE        hConsoleOutput,
                                                  BOOL          fAbsolute,
                                                  PSMALL_RECT   psrctWindowRect);

BOOL   WIN32API OS2WriteConsoleA                 (HANDLE        hConsoleOutput,
                                                  CONST VOID*   lpvBuffer,
                                                  DWORD         cchToWrite,
                                                  LPDWORD       lpcchWritten,
                                                  LPVOID        lpvReserved);

BOOL   WIN32API OS2WriteConsoleInputA            (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcWritten);

BOOL   WIN32API OS2WriteConsoleInputW            (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcWritten);

BOOL   WIN32API OS2WriteConsoleOutputA           (HANDLE        hConsoleOutput,
                                                  PCHAR_INFO    pchiSrcBuffer,
                                                  COORD         coordSrcBufferSize,
                                                  COORD         coordSrcBufferCoord,
                                                  PSMALL_RECT   psrctDestRect);

BOOL   WIN32API OS2WriteConsoleOutputW           (HANDLE        hConsoleOutput,
                                                  PCHAR_INFO    pchiSrcBuffer,
                                                  COORD         coordSrcBufferSize,
                                                  COORD         coordSrcBufferCoord,
                                                  PSMALL_RECT   psrctDestRect);

BOOL   WIN32API OS2WriteConsoleOutputAttribute   (HANDLE        hConsoleOutput,
                                                  LPWORD        lpwAttribute,
                                                  DWORD         cWriteCells,
                                                  COORD         coordWriteCoord,
                                                  LPDWORD       lpcNumberWritten);

BOOL   WIN32API OS2WriteConsoleOutputCharacterA  (HANDLE        hConsoleOutput,
                                                  LPTSTR        lpWriteBuffer,
                                                  DWORD         cchWrite,
                                                  COORD         coordWriteCoord,
                                                  LPDWORD       lpcWritten);

BOOL   WIN32API OS2WriteConsoleOutputCharacterW  (HANDLE        hConsoleOutput,
                                                  LPTSTR        lpWriteBuffer,
                                                  DWORD         cchWrite,
                                                  COORD         coordWriteCoord,
                                                  LPDWORD       lpcWritten);

  
  
  
#ifdef __cplusplus
}
#endif


/*****************************************************************************
 * Forwarders                                                                *
 *****************************************************************************/

#ifdef __cplusplus

/*****************************************************************************
 * Win32 Console Subsystem Pseudo-Devices                                    *
 *****************************************************************************/


/*****************************************************************************
 * ConsoleInput - manages a queue of window events fro keyboard, mouse, etc. *
 *****************************************************************************/

class HMDeviceConsoleInClass : public HMDeviceHandler
{
  protected:
    /* input queue */
    DWORD dwNumberOfInputEvents;   /* number of unread events in input queue */
  
  public:
    HMDeviceConsoleInClass(LPCSTR lpDeviceName)
      : HMDeviceHandler(lpDeviceName) {}
  
                       /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);
  
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

  virtual DWORD  _DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                 ULONG         ulRequestCode,
                                 ULONG         arg1,
                                 ULONG         arg2,
                                 ULONG         arg3,
                                 ULONG         arg4);

  /************************************
   * non-standard console I/O methods *
   ************************************/
  
  virtual BOOL  FlushConsoleInputBuffer(PHMHANDLEDATA pHMHandleData);
  
  virtual DWORD GetConsoleMode(PHMHANDLEDATA pHMHandleData,
                               LPDWORD       lpMode);
  
  virtual BOOL  GetNumberOfConsoleInputEvents(PHMHANDLEDATA pHMHandleData,
                                              LPDWORD       lpNumberOfEvents);

  virtual DWORD PeekConsoleInputA(PHMHANDLEDATA pHMHandleData,
                                  PINPUT_RECORD pirBuffer,
                                  DWORD         cInRecords,
                                  LPDWORD       lpcRead);
  
  virtual DWORD PeekConsoleInputW(PHMHANDLEDATA pHMHandleData,
                                  PINPUT_RECORD pirBuffer,
                                  DWORD         cInRecords,
                                  LPDWORD       lpcRead);
  
  virtual DWORD ReadConsoleA(PHMHANDLEDATA pHMHandleData,
                             CONST VOID*   lpvBuffer,
                             DWORD         cchToRead,
                             LPDWORD       lpcchRead,
                             LPVOID        lpvReserved);  
  
  virtual DWORD ReadConsoleW(PHMHANDLEDATA pHMHandleData,
                             CONST VOID*   lpvBuffer,
                             DWORD         cchToRead,
                             LPDWORD       lpcchRead,
                             LPVOID        lpvReserved);
  
  virtual DWORD ReadConsoleInputA(PHMHANDLEDATA pHMHandleData,
                                  PINPUT_RECORD pirBuffer,
                                  DWORD         cInRecords,
                                  LPDWORD       lpcRead);
  
  virtual DWORD ReadConsoleInputW(PHMHANDLEDATA pHMHandleData,
                                  PINPUT_RECORD pirBuffer,
                                  DWORD         cInRecords,
                                  LPDWORD       lpcRead);
  
  virtual DWORD SetConsoleMode(PHMHANDLEDATA pHMHandleData,
                               DWORD         dwMode);
  
  virtual DWORD WriteConsoleInputA (PHMHANDLEDATA pHMHandleData,
                                    PINPUT_RECORD pirBuffer,
                                    DWORD         cInRecords,
                                    LPDWORD       lpcWritten);  
  
  virtual DWORD WriteConsoleInputW (PHMHANDLEDATA pHMHandleData,
                                    PINPUT_RECORD pirBuffer,
                                    DWORD         cInRecords,
                                    LPDWORD       lpcWritten);
  
};


/*****************************************************************************
 * ConsoleOutput - redirects requests to the currently active buffer         *
 *****************************************************************************/

class HMDeviceConsoleOutClass : public HMDeviceHandler
{
  protected:
    HANDLE hConsoleBuffer;            /* handle to the active console buffer */
  
  public:
    HMDeviceConsoleOutClass(LPCSTR lpDeviceName)
      : HMDeviceHandler(lpDeviceName) {}
  
                       /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);
  
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
  
  
                                                /* handling non-standard I/O */
  virtual DWORD _DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                ULONG         ulRequestCode,
                                ULONG         arg1,
                                ULONG         arg2,
                                ULONG         arg3,
                                ULONG         arg4);
};


/*****************************************************************************
 * ConsoleBuffer - does all the writing to an internal line buffer array     *
 *                 if this buffer is the active one, also does a screen updt *
 *****************************************************************************/

class HMDeviceConsoleBufferClass : public HMDeviceHandler
{
  public:
    HMDeviceConsoleBufferClass(LPCSTR lpDeviceName)
      : HMDeviceHandler(lpDeviceName) {}
  
                       /* this is a handler method for calls to CreateFile() */
  virtual DWORD  CreateFile (LPCSTR        lpFileName,
                             PHMHANDLEDATA pHMHandleData,
                             PVOID         lpSecurityAttributes,
                             PHMHANDLEDATA pHMHandleDataTemplate);
  
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
  
  virtual DWORD  CloseHandle(PHMHANDLEDATA pHMHandleData);
  
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

#endif


#endif /* _CONSOLESUBSYSTEM_H_ */
