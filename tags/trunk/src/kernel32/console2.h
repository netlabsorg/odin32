/* $Id: console2.h,v 1.11 2001-10-01 01:43:35 bird Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Console Subsystem for OS/2
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) HandleManager.Cpp       1.0.0   1998/02/11 PH start
 */

#ifndef _CONSOLESUBSYSTEM_H_
#define _CONSOLESUBSYSTEM_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#pragma pack(4)

#ifndef _OS2WIN_H_
typedef struct _COORD
{
    SHORT X;
    SHORT Y;
} COORD, *PCOORD;
#endif


typedef struct _SMALL_RECT
{
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
} SMALL_RECT, *PSMALL_RECT;


typedef struct _KEY_EVENT_RECORD
{
    BOOL bKeyDown;
    WORD wRepeatCount;
    WORD wVirtualKeyCode;
    WORD wVirtualScanCode;
    union
    {
        WCHAR UnicodeChar;
        CHAR   AsciiChar;
    } uChar;
    DWORD dwControlKeyState;
} KEY_EVENT_RECORD, *PKEY_EVENT_RECORD;


//
// ControlKeyState flags
//

#define RIGHT_ALT_PRESSED     0x0001 // the right alt key is pressed.
#define LEFT_ALT_PRESSED      0x0002 // the left alt key is pressed.
#define RIGHT_CTRL_PRESSED    0x0004 // the right ctrl key is pressed.
#define LEFT_CTRL_PRESSED     0x0008 // the left ctrl key is pressed.
#define SHIFT_PRESSED         0x0010 // the shift key is pressed.
#define NUMLOCK_ON            0x0020 // the numlock light is on.
#define SCROLLLOCK_ON         0x0040 // the scrolllock light is on.
#define CAPSLOCK_ON           0x0080 // the capslock light is on.
#define ENHANCED_KEY          0x0100 // the key is enhanced.

typedef struct _MOUSE_EVENT_RECORD
{
    COORD dwMousePosition;
    DWORD dwButtonState;
    DWORD dwControlKeyState;
    DWORD dwEventFlags;
} MOUSE_EVENT_RECORD, *PMOUSE_EVENT_RECORD;


//
// ButtonState flags
//

#define FROM_LEFT_1ST_BUTTON_PRESSED    0x0001
#define RIGHTMOST_BUTTON_PRESSED        0x0002
#define FROM_LEFT_2ND_BUTTON_PRESSED    0x0004
#define FROM_LEFT_3RD_BUTTON_PRESSED    0x0008
#define FROM_LEFT_4TH_BUTTON_PRESSED    0x0010

//
// EventFlags
//

#define MOUSE_MOVED   0x0001
#define DOUBLE_CLICK  0x0002

typedef struct _WINDOW_BUFFER_SIZE_RECORD
{
  COORD dwSize;
} WINDOW_BUFFER_SIZE_RECORD, *PWINDOW_BUFFER_SIZE_RECORD;

typedef struct _MENU_EVENT_RECORD
{
  UINT dwCommandId;
} MENU_EVENT_RECORD, *PMENU_EVENT_RECORD;

typedef struct _FOCUS_EVENT_RECORD
{
  BOOL bSetFocus;
} FOCUS_EVENT_RECORD, *PFOCUS_EVENT_RECORD;

typedef struct _INPUT_RECORD
{
  WORD EventType;
  union
  {
    KEY_EVENT_RECORD          KeyEvent;
    MOUSE_EVENT_RECORD        MouseEvent;
    WINDOW_BUFFER_SIZE_RECORD WindowBufferSizeEvent;
    MENU_EVENT_RECORD         MenuEvent;
    FOCUS_EVENT_RECORD        FocusEvent;
  } Event;
} INPUT_RECORD, *PINPUT_RECORD;


//
//  EventType flags:
//

#define KEY_EVENT         0x0001 // Event contains key event record
#define MOUSE_EVENT       0x0002 // Event contains mouse event record
#define WINDOW_BUFFER_SIZE_EVENT 0x0004 // Event contains window change event record
#define MENU_EVENT 0x0008 // Event contains menu event record
#define FOCUS_EVENT 0x0010 // event contains focus change

typedef struct _CHAR_INFO
{
  union
  {
    WCHAR UnicodeChar;
    CHAR   AsciiChar;
   } Char;
   WORD Attributes;
} CHAR_INFO, *PCHAR_INFO;

//
// Attributes flags:
//

#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.


typedef struct _CONSOLE_SCREEN_BUFFER_INFO
{
  COORD      dwSize;
  COORD      dwCursorPosition;
  WORD       wAttributes;
  SMALL_RECT srWindow;
  COORD      dwMaximumWindowSize;
} CONSOLE_SCREEN_BUFFER_INFO, *PCONSOLE_SCREEN_BUFFER_INFO;

typedef struct _CONSOLE_CURSOR_INFO
{
  DWORD  dwSize;
  BOOL   bVisible;
} CONSOLE_CURSOR_INFO, *PCONSOLE_CURSOR_INFO;

//
// typedef for ctrl-c handler routines
//

typedef BOOL (* __stdcall PHANDLER_ROUTINE)(DWORD CtrlType);    /*PLF Fri  97-06-20 17:17:42*/

#define CTRL_C_EVENT        0
#define CTRL_BREAK_EVENT    1
#define CTRL_CLOSE_EVENT    2
// 3 is reserved!
// 4 is reserved!
#define CTRL_LOGOFF_EVENT   5
#define CTRL_SHUTDOWN_EVENT 6

//
//  Input Mode flags:
//

#define ENABLE_PROCESSED_INPUT 0x0001
#define ENABLE_LINE_INPUT      0x0002
#define ENABLE_ECHO_INPUT      0x0004
#define ENABLE_WINDOW_INPUT    0x0008
#define ENABLE_MOUSE_INPUT     0x0010

//
// Output Mode flags:
//

#define ENABLE_PROCESSED_OUTPUT    0x0001
#define ENABLE_WRAP_AT_EOL_OUTPUT  0x0002

#pragma pack()


/*****************************************************************************
 * Defines & Macros                                                          *
 *****************************************************************************/

#define CONSOLE_TEXTMODE_BUFFER 1

#define SZ_CONSOLE_CLASS "WIN32CONSOLECLASS"


/* according to the updated AVIO documentation of Warp 4 */
#define MAX_OS2_ROWS           255
#define MAX_OS2_COLUMNS        255
#define FORMAT_CGA             1
#define CONSOLE_TIMER_ID       1
#define CONSOLE_INPUTQUEUESIZE 256


#define CONSOLECURSOR_HIDE         1
#define CONSOLECURSOR_SHOW         2
#define CONSOLECURSOR_BLINK        3
#define CONSOLECURSOR_OVERWRITTEN  4

#define UM_CONSOLE_CREATE WM_USER + 0x1000


/* device request codes for use in the pseudo-device handlers */
#define DRQ_CONSOLE                        0x00010000
#define DRQ_FILLCONSOLEOUTPUTATTRIBUTE     DRQ_CONSOLE +  0
#define DRQ_FILLCONSOLEOUTPUTCHARACTERA    DRQ_CONSOLE +  1
#define DRQ_FILLCONSOLEOUTPUTCHARACTERW    DRQ_CONSOLE +  2
#define DRQ_FLUSHCONSOLEINPUTBUFFER        DRQ_CONSOLE +  3
#define DRQ_GETCONSOLECURSORINFO           DRQ_CONSOLE +  4
#define DRQ_GETCONSOLEMODE                 DRQ_CONSOLE +  5
#define DRQ_GETCONSOLESCREENBUFFERINFO     DRQ_CONSOLE +  6
#define DRQ_GETLARGESTCONSOLEWINDOWSIZE    DRQ_CONSOLE +  7
#define DRQ_GETNUMBEROFCONSOLEINPUTEVENTS  DRQ_CONSOLE +  8
#define DRQ_PEEKCONSOLEINPUTW              DRQ_CONSOLE +  9
#define DRQ_PEEKCONSOLEINPUTA              DRQ_CONSOLE + 10
#define DRQ_READCONSOLEA                   DRQ_CONSOLE + 11
#define DRQ_READCONSOLEW                   DRQ_CONSOLE + 12
#define DRQ_READCONSOLEINPUTA              DRQ_CONSOLE + 14
#define DRQ_READCONSOLEINPUTW              DRQ_CONSOLE + 15
#define DRQ_READCONSOLEOUTPUTA             DRQ_CONSOLE + 16
#define DRQ_READCONSOLEOUTPUTW             DRQ_CONSOLE + 17
#define DRQ_READCONSOLEOUTPUTATTRIBUTE     DRQ_CONSOLE + 18
#define DRQ_READCONSOLEOUTPUTCHARACTERA    DRQ_CONSOLE + 19
#define DRQ_READCONSOLEOUTPUTCHARACTERW    DRQ_CONSOLE + 20
#define DRQ_SCROLLCONSOLESCREENBUFFERA     DRQ_CONSOLE + 21
#define DRQ_SCROLLCONSOLESCREENBUFFERW     DRQ_CONSOLE + 22
#define DRQ_SETCONSOLEACTIVESCREENBUFFER   DRQ_CONSOLE + 23
#define DRQ_SETCONSOLECURSORINFO           DRQ_CONSOLE + 24
#define DRQ_SETCONSOLECURSORPOSITION       DRQ_CONSOLE + 25
#define DRQ_SETCONSOLEMODE                 DRQ_CONSOLE + 26
#define DRQ_SETCONSOLESCREENBUFFERSIZE     DRQ_CONSOLE + 27
#define DRQ_SETCONSOLETEXTATTRIBUTE        DRQ_CONSOLE + 28
#define DRQ_SETCONSOLEWINDOWINFO           DRQ_CONSOLE + 29
#define DRQ_WRITECONSOLEA                  DRQ_CONSOLE + 30
#define DRQ_WRITECONSOLEW                  DRQ_CONSOLE + 31
#define DRQ_WRITECONSOLEINPUTA             DRQ_CONSOLE + 32
#define DRQ_WRITECONSOLEINPUTW             DRQ_CONSOLE + 33
#define DRQ_WRITECONSOLEOUTPUTA            DRQ_CONSOLE + 34
#define DRQ_WRITECONSOLEOUTPUTW            DRQ_CONSOLE + 35
#define DRQ_WRITECONSOLEOUTPUTATTRIBUTE    DRQ_CONSOLE + 36
#define DRQ_WRITECONSOLEOUTPUTCHARACTERA   DRQ_CONSOLE + 37
#define DRQ_WRITECONSOLEOUTPUTCHARACTERW   DRQ_CONSOLE + 38
#define DRQ_INTERNAL_CONSOLEBUFFERMAP      DRQ_CONSOLE + 39
#define DRQ_INTERNAL_CONSOLECURSORSHOW     DRQ_CONSOLE + 40
#define DRQ_INTERNAL_CONSOLEADJUSTWINDOW   DRQ_CONSOLE + 41


#define COORD2ULONG(c) ((ULONG)( ((ULONG)c.X << 16) + (ULONG)c.Y))
#define ULONG2COORD(c,u) c.X = u >> 16; c.Y = u & 0x0000FFFF;


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


typedef struct _ConsoleOptions
{
  HMODULE hmodResources;                   /* module handle for PM resources */

  BOOL  fTerminateAutomatically;       /* wait for window termination or not */
  BOOL  fSpeakerEnabled;               /* whether we process BEL 0x07 or not */
  ULONG ulSpeakerDuration;        /* duration and frequency for speaker beep */
  ULONG ulSpeakerFrequency;
  ULONG ulUpdateLimit;        /* when exceeding this line counter, automatic */
                                               /* screen update is performed */

  BOOL  fSetWindowPosition;           /* TRUE if window has to be positioned */

  COORD coordDefaultPosition;      /* default position of the console window */
  COORD coordDefaultSize;          /* default size     of the console window */
  COORD coordBufferSize;           /* size of the console buffer             */

  BOOL  fQuickInsert;                         /* like NT's quick insert mode */
  BOOL  fInsertMode;                                          /* insert mode */
  BOOL  fMouseActions;                  /* like Warp 4's "mouse action" mode */
  BOOL  fToolbarActive;                   /* windows95-like toolbar on / off */

  ULONG ulTabSize;                                         /* tabulator size */

  UCHAR ucDefaultAttribute;                    /* the default text attribute */

  UCHAR ucCursorDivisor;                /* cursor timer divisor for blinking */

  ULONG ulConsoleThreadPriorityClass;       /* priority settings for message */
  ULONG ulConsoleThreadPriorityDelta;                              /* thread */
  ULONG ulAppThreadPriorityClass;       /* priority settings for application */
  ULONG ulAppThreadPriorityDelta;                                  /* thread */

} ICONSOLEOPTIONS, *PICONSOLEOPTIONS;



typedef struct _iConsoleGlobals
{
    TID    tidConsole;                             /* console message thread */
    HEV    hevConsole;                            /* console event semaphore */
    APIRET rcConsole;                /* initialization status of the console */
    HAB    hab;                                       /* anchor block handle */
    HMQ    hmq;               /* message queue handle for the console window */
    QMSG   qmsg;                           /* message for the console window */
    ULONG  flFrameFlags;                      /* frame window creation flags */
    PSZ    pszWindowTitle;                             /* name of the window */
    HWND   hwndFrame;                                 /* frame window handle */
    HWND   hwndClient;                               /* client window handle */

    HWND   hwndHorzScroll;                /* handle of horizontal scroll bar */
    HWND   hwndVertScroll;                  /* handle of vertical scroll bar */
    BOOL   fHasVertScroll;            /* indicates if scrollbars are visible */
    BOOL   fHasHorzScroll;

    HDC    hdcConsole;                             /* console device context */
    PFNWP  pfnwpFrameOriginal;            /* original frame window procedure */

    HWND     hwndMenuConsole;                          /* console popup menu */
    HMODULE  hmodResource;           /* resources are stored in KERNEL32.DLL */
    HPOINTER hPtrConsole;                                    /* console icon */

    HANDLE hConsoleBuffer;            /* handle of the active console buffer */
    HANDLE hConsoleBufferDefault;    /* handle of the default console buffer */

    HVPS   hvpsConsole;                   /* console AVIO presentation space */

    COORD  coordMaxWindowPels;              /* maximum window size in pixels */
    COORD  coordWindowSize;                   /* current console window size */
    COORD  coordWindowPos;                           /* scroller's positions */

    SHORT  sCellCX; /* height and width of a avio cell with the current font */
    SHORT  sCellCY;

    BOOL   fUpdateRequired; /* set to TRUE if next WM_TIMER shall update the */
                           /* AVIO presentation space from the consolebuffer */

    ULONG  idTimer;                                      /* Timer identifier */
    ULONG  ulTimerFrequency;             /* cursor + blitter timer frequency */
    ULONG  ulTimerCursor;                 /* cursor loop counter for divisor */

    ICONSOLEOPTIONS Options;           /* the console's options / properties */

} ICONSOLEGLOBALS, *PICONSOLEGLOBALS;


typedef struct _iConsoleInput
{
  INPUT_RECORD arrInputRecord[CONSOLE_INPUTQUEUESIZE];       /* packet queue */
  HEV          hevInputQueue;             /* becomes posted if input arrives */

  ULONG        ulIndexFree;              /* index of first free event record */
  ULONG        ulIndexEvent;          /* index of first valid event in queue */
  ULONG        ulEvents;                        /* number of events in queue */
  DWORD        dwConsoleMode;                          /* input console mode */

  HMTX         hmtxInputQueue;        /* console input queue mutex semaphore */
} ICONSOLEINPUT, *PICONSOLEINPUT;



/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


                                                   /* console message thread */
VOID /*_Optlink*/ iConsoleMsgThread    (void *pParameters);

MRESULT EXPENTRY iConsoleWindowProc(HWND   hwnd,   /* window procedure */
                                          ULONG  msg,
                                          MPARAM mp1,
                                          MPARAM mp2);

MRESULT EXPENTRY iConsoleFrameWindowProc(HWND   hwnd,
                                               ULONG  msg,
                                               MPARAM mp1,
                                               MPARAM mp2);

void   iConsoleBufferMap           (PCONSOLEBUFFER pConsoleBuffer);

void   iConsoleBufferScrollUp      (PCONSOLEBUFFER pConsoleBuffer,
                                          ULONG          ulLines);

void   iConsoleBufferFillLine      (ULONG   ulPattern,
                                          PUSHORT pusTarget,
                                          ULONG   ulSize);

APIRET iConsoleInputEventPush      (PINPUT_RECORD pInputRecord);

APIRET iConsoleInputEventPop       (PINPUT_RECORD pInputRecord);

APIRET iConsoleInputEventPushKey   (MPARAM mp1,
                                          MPARAM mp2);

APIRET iConsoleInputEventPushMouse (ULONG  ulMessage,
                                          MPARAM mp1,
                                          MPARAM mp2);

APIRET iConsoleInputEventPushWindow(COORD coordWindowSize);

APIRET iConsoleInputEventPushMenu  (DWORD dwCommandId);

APIRET iConsoleInputEventPushFocus (BOOL bSetFocus);

#define QUERY_EVENT_PEEK	0
#define QUERY_EVENT_WAIT	1
ULONG  iConsoleInputQueryEvents    (PICONSOLEINPUT pConsoleInput, int fWait);

void   iConsoleCursorShow          (PCONSOLEBUFFER pConsoleBuffer,
                                    ULONG          ulCursorMode);

APIRET iConsoleFontQuery           (void);

void   iConsoleAdjustWindow        (PCONSOLEBUFFER pConsoleBuffer);


#ifndef _OS2WIN_H

BOOL   WIN32API AllocConsole                  (VOID);

HANDLE WIN32API CreateConsoleScreenBuffer     (DWORD         dwDesiredAccess,
                                                  DWORD         dwShareMode,
                                                  LPVOID        lpSecurityAttributes,
                                                  DWORD         dwFlags,
                                                  LPVOID        lpScreenBufferData);

BOOL   WIN32API FillConsoleOutputAttribute    (HANDLE        hConsoleOutput,
                                                  WORD          wAttribute,
                                                  DWORD         nLength,
                                                  COORD         dwWriteCoord,
                                                  LPDWORD       lpNumberOfAttrsWritten);

BOOL   WIN32API FillConsoleOutputCharacterW   (HANDLE        hConsoleOutput,
                                                  WCHAR         cCharacter,
                                                  DWORD         nLength,
                                                  COORD         dwWriteCoord,
                                                  LPDWORD       lpNumberOfCharsWritten);

BOOL   WIN32API FillConsoleOutputCharacterA   (HANDLE        hConsoleOutput,
                                                  UCHAR         cCharacter,
                                                  DWORD         nLength,
                                                  COORD         dwWriteCoord,
                                                  LPDWORD       lpNumberOfCharsWritten);


BOOL   WIN32API FlushConsoleInputBuffer       (HANDLE        hConsoleInput);

BOOL   WIN32API FreeConsole                   (VOID);

BOOL   WIN32API GenerateConsoleCtrlEvent      (DWORD         dwCtrlEvent,
                                                  DWORD         dwProcessGroupId);

UINT   WIN32API GetConsoleCP                  (VOID);

BOOL   WIN32API GetConsoleCursorInfo          (HANDLE               hConsoleOutput,
                                                  PCONSOLE_CURSOR_INFO lpConsoleCursorInfo);

BOOL   WIN32API GetConsoleMode                (HANDLE        hConsole,
                                                  LPDWORD       lpMode);

UINT   WIN32API GetConsoleOutputCP            (VOID);

BOOL   WIN32API GetConsoleScreenBufferInfo    (HANDLE                      hConsoleOutput,
                                                  PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo);

DWORD  WIN32API GetConsoleTitleA              (LPTSTR        lpConsoleTitle,
                                                  DWORD         nSize);

DWORD  WIN32API GetConsoleTitleW              (LPWSTR        lpConsoleTitle,
                                               DWORD         nSize);

COORD  WIN32API GetLargestConsoleWindowSize   (HANDLE        hConsoleOutput);

BOOL   WIN32API GetNumberOfConsoleInputEvents (HANDLE        hConsoleInput,
                                                  LPDWORD       lpNumberOfEvents);

BOOL   WIN32API GetNumberOfConsoleMouseButtons(LPDWORD       lpcNumberOfMouseButtons);

BOOL   WIN32API PeekConsoleInputA             (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcRead);

BOOL   WIN32API PeekConsoleInputW             (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcRead);

BOOL   WIN32API ReadConsoleA                  (HANDLE        hConsoleInput,
                                                  LPVOID        lpvBuffer,
                                                  DWORD         cchToRead,
                                                  LPDWORD       lpcchRead,
                                                  LPVOID        lpvReserved);

BOOL   WIN32API ReadConsoleInputA             (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcRead);

BOOL   WIN32API ReadConsoleInputW             (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcRead);

BOOL   WIN32API ReadConsoleOutputA            (HANDLE        hConsoleOutput,
                                                  PCHAR_INFO    pchiDestBuffer,
                                                  COORD         coordDestBufferSize,
                                                  COORD         coordDestBufferCoord,
                                                  PSMALL_RECT   psrctSourceRect);

BOOL   WIN32API ReadConsoleOutputW            (HANDLE        hConsoleOutput,
                                                  PCHAR_INFO    pchiDestBuffer,
                                                  COORD         coordDestBufferSize,
                                                  COORD         coordDestBufferCoord,
                                                  PSMALL_RECT   psrctSourceRect);

BOOL   WIN32API ReadConsoleOutputAttribute    (HANDLE        hConsoleOutput,
                                                  LPWORD        lpwAttribute,
                                                  DWORD         cReadCells,
                                                  COORD         coordReadCoord,
                                                  LPDWORD       lpcNumberRead);

BOOL   WIN32API ReadConsoleOutputCharacter    (HANDLE        hConsoleOutput,
                                                  LPTSTR        lpReadBuffer,
                                                  DWORD         cchRead,
                                                  COORD         coordReadCoord,
                                                  LPDWORD       lpcNumberRead);

BOOL   WIN32API ScrollConsoleScreenBufferA    (HANDLE        hConsoleOutput,
                                                  PSMALL_RECT   psrctSourceRect,
                                                  PSMALL_RECT   psrctClipRect,
                                                  COORD         coordDestOrigin,
                                                  PCHAR_INFO    pchiFill);

BOOL   WIN32API ScrollConsoleScreenBufferW    (HANDLE        hConsoleOutput,
                                                  PSMALL_RECT   psrctSourceRect,
                                                  PSMALL_RECT   psrctClipRect,
                                                  COORD         coordDestOrigin,
                                                  PCHAR_INFO    pchiFill);

BOOL   WIN32API SetConsoleActiveScreenBuffer  (HANDLE        hConsoleOutput);

BOOL   WIN32API SetConsoleCP                  (UINT          IDCodePage);

BOOL   WIN32API SetConsoleCtrlHandler         (PHANDLER_ROUTINE pHandlerRoutine,
                                                  BOOL             fAdd);

BOOL   WIN32API SetConsoleCursorInfo          (HANDLE               hConsoleOutput,
                                                  PCONSOLE_CURSOR_INFO lpConsoleCursorInfo);

BOOL   WIN32API SetConsoleCursorPosition      (HANDLE        hConsoleOutput,
                                                  COORD         coordCursor);

BOOL   WIN32API SetConsoleMode                (HANDLE        hConsole,
                                                  DWORD         fdwMode);

BOOL   WIN32API SetConsoleOutputCP            (UINT          IDCodePage);

BOOL   WIN32API SetConsoleScreenBufferSize    (HANDLE        hConsoleOutput,
                                                  COORD         coordSize);

BOOL   WIN32API SetConsoleTextAttribute       (HANDLE        hConsoleOutput,
                                                  WORD          wAttr);

BOOL   WIN32API SetConsoleTitleA              (LPTSTR        lpszTitle);

BOOL   WIN32API SetConsoleTitleW              (LPWSTR        lpszTitle);

BOOL   WIN32API SetConsoleWindowInfo          (HANDLE        hConsoleOutput,
                                                  BOOL          fAbsolute,
                                                  PSMALL_RECT   psrctWindowRect);

BOOL   WIN32API WriteConsoleA                 (HANDLE        hConsoleOutput,
                                                  CONST VOID*   lpvBuffer,
                                                  DWORD         cchToWrite,
                                                  LPDWORD       lpcchWritten,
                                                  LPVOID        lpvReserved);

BOOL   WIN32API WriteConsoleInputA            (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcWritten);

BOOL   WIN32API WriteConsoleInputW            (HANDLE        hConsoleInput,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcWritten);

BOOL   WIN32API WriteConsoleOutputA           (HANDLE        hConsoleOutput,
                                                  PCHAR_INFO    pchiSrcBuffer,
                                                  COORD         coordSrcBufferSize,
                                                  COORD         coordSrcBufferCoord,
                                                  PSMALL_RECT   psrctDestRect);

BOOL   WIN32API WriteConsoleOutputW           (HANDLE        hConsoleOutput,
                                                  PCHAR_INFO    pchiSrcBuffer,
                                                  COORD         coordSrcBufferSize,
                                                  COORD         coordSrcBufferCoord,
                                                  PSMALL_RECT   psrctDestRect);

BOOL   WIN32API WriteConsoleOutputAttribute   (HANDLE        hConsoleOutput,
                                                  LPWORD        lpwAttribute,
                                                  DWORD         cWriteCells,
                                                  COORD         coordWriteCoord,
                                                  LPDWORD       lpcNumberWritten);

BOOL   WIN32API WriteConsoleOutputCharacterA  (HANDLE        hConsoleOutput,
                                                  LPTSTR        lpWriteBuffer,
                                                  DWORD         cchWrite,
                                                  COORD         coordWriteCoord,
                                                  LPDWORD       lpcWritten);

BOOL   WIN32API WriteConsoleOutputCharacterW  (HANDLE        hConsoleOutput,
                                                  LPTSTR        lpWriteBuffer,
                                                  DWORD         cchWrite,
                                                  COORD         coordWriteCoord,
                                                  LPDWORD       lpcWritten);


#endif // _OS2WIN_H

#ifdef __cplusplus
  }
#endif

#endif /* _CONSOLESUBSYSTEM_H_ */
