/*
 * Win32 Console API Translation for OS/2
 *
 * 1998/02/10 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) console.cpp         1.0.0   1998/02/10 PH Start from scratch
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifdef DEBUG
#define DEBUG_LOCAL
#define DEBUG_LOCAL2
#endif

#undef DEBUG_LOCAL
#undef DEBUG_LOCAL2


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 - DWORD HandlerRoutine (DWORD dwCtrlType)
   basically an exception handler routine. handles a few signals / excpts.
   should be somewhere near the exception handling code ... :)

   Hmm, however as PM applications don't really get a ctrl-c signal,
   I'll have to do this on my own ...

 - supply unicode<->ascii conversions for all the _A and _W function pairs.

 - problem: we can't prevent thread1 from blocking the message queue ?
            what will happen if a WinTerminate() is issued there ?
            will the message queue be closed and provide smooth tasking ?
            how will open32 react on this ?

 - ECHO_LINE_INPUT / ReadFile blocks till CR

 - scrollbars
 * do some flowchart to exactly determine WHEN to use WHICH setting
   and perform WHAT action

 - clipboard support
*/


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#define  INCL_WIN
#define  INCL_DOSMEMMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#define  INCL_DOSPROCESS
#define  INCL_DOSMODULEMGR
#define  INCL_VIO
#define  INCL_AVIO
#include <os2.h>
#include <builtin.h>

#include <stdlib.h>
#include <string.h>
#include "win32type.h"
#include "misc.h"
#include "console.h"
#include "console2.h"
#include "conprop.h"
#include "unicode.h"
#include "HandleManager.h"


/***********************************
 * PH: fixups for missing os2win.h *
 ***********************************/

#include <os2sel.h>

extern "C"
{
  void   _System _O32_SetLastError(DWORD  dwError);
  DWORD  _System _O32_GetLastError(void);
  LPSTR  _System _O32_GetCommandLine(void);
  void   _System _O32_ExitProcess(UINT exitcode);
  HANDLE _System _O32_GetStdHandle(DWORD dwDevice);
  DWORD  _System _O32_GetFileType(HANDLE hFile);

inline void SetLastError(DWORD a)
{
 USHORT sel = GetFS();

    _O32_SetLastError(a);
    SetFS(sel);
} 

inline DWORD GetLastError()
{
 DWORD yyrc;
 USHORT sel = GetFS();

    yyrc = _O32_GetLastError();
    SetFS(sel);

    return yyrc;
} 

inline LPSTR GetCommandLine()
{
 LPSTR yyrc;
 USHORT sel = GetFS();

    yyrc = _O32_GetCommandLine();
    SetFS(sel);

    return yyrc;
} 

inline void ExitProcess(UINT a)
{
 USHORT sel = GetFS();

    _O32_ExitProcess(a);
    SetFS(sel);
} 

inline HANDLE GetStdHandle(DWORD a)
{
 HANDLE yyrc;
 USHORT sel = GetFS();

    yyrc = _O32_GetStdHandle(a);
    SetFS(sel);

    return yyrc;
} 

inline DWORD GetFileType(HANDLE a)
{
 DWORD yyrc;
 USHORT sel = GetFS();

    yyrc = _O32_GetFileType(a);
    SetFS(sel);

    return yyrc;
} 

}

#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#define STD_INPUT_HANDLE         ((DWORD)-10)
#define STD_OUTPUT_HANDLE        ((DWORD)-11)
#define STD_ERROR_HANDLE         ((DWORD)-12)
#define GENERIC_READ                 0x80000000
#define GENERIC_WRITE                0x40000000
#define FILE_SHARE_READ              0x00000001
#define FILE_SHARE_WRITE             0x00000002



/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

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


/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/


static APIRET ConsoleTerminate(void);/* termination of the console subsystem */

                                                   /* console message thread */
static VOID _Optlink ConsoleMsgThread    (void *pParameters);

static MRESULT EXPENTRY ConsoleWindowProc(HWND   hwnd,   /* window procedure */
                                          ULONG  msg,
                                          MPARAM mp1,
                                          MPARAM mp2);

static MRESULT EXPENTRY ConsoleFrameWindowProc(HWND   hwnd,
                                               ULONG  msg,
                                               MPARAM mp1,
                                               MPARAM mp2);

static void   ConsoleBufferMap           (PCONSOLEBUFFER pConsoleBuffer);

static void   ConsoleBufferScrollUp      (PCONSOLEBUFFER pConsoleBuffer,
                                          ULONG          ulLines);

static void   ConsoleBufferFillLine      (ULONG   ulPattern,
                                          PUSHORT pusTarget,
                                          ULONG   ulSize);

static APIRET ConsoleInputEventPush      (PINPUT_RECORD pInputRecord);

static APIRET ConsoleInputEventPop       (PINPUT_RECORD pInputRecord);

static APIRET ConsoleInputEventPushKey   (MPARAM mp1,
                                          MPARAM mp2);

static APIRET ConsoleInputEventPushMouse (ULONG  ulMessage,
                                          MPARAM mp1,
                                          MPARAM mp2);

static APIRET ConsoleInputEventPushWindow(COORD coordWindowSize);

static APIRET ConsoleInputEventPushMenu  (DWORD dwCommandId);

static APIRET ConsoleInputEventPushFocus (BOOL bSetFocus);

static ULONG  ConsoleInputQueryEvents    (void);

static void   ConsoleCursorShow          (PCONSOLEBUFFER pConsoleBuffer,
                                          ULONG          ulCursorMode);

static APIRET ConsoleFontQuery           (void);

static void   ConsoleAdjustWindow        (PCONSOLEBUFFER pConsoleBuffer);


/*****************************************************************************
 * Process Global Structures                                                 *
 *****************************************************************************/


struct _ConsoleGlobals
{
  public:
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

    CONSOLEOPTIONS Options;            /* the console's options / properties */

} ConsoleGlobals;


struct _ConsoleInput
{
  INPUT_RECORD arrInputRecord[CONSOLE_INPUTQUEUESIZE];       /* packet queue */
  HEV          hevInputQueue;             /* becomes posted if input arrives */

  /* HMUTEX hmtxInputQueue; if queue corruption should occur ... */

  ULONG        ulIndexFree;              /* index of first free event record */
  ULONG        ulIndexEvent;          /* index of first valid event in queue */
  ULONG        ulEvents;                        /* number of events in queue */
  DWORD        dwConsoleMode;                          /* input console mode */
} ConsoleInput;


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

APIRET ConsoleInit(void)                /* creation of the console subsystem */
{
  APIRET rc;                                              /* API return code */
  ULONG  ulPostCount;                              /* semaphore post counter */



  if (ConsoleGlobals.hevConsole != NULLHANDLE) /* we're already initialized ?*/
    return (NO_ERROR);                             /* then abort immediately */

                                 /* create console synchronization semaphore */
  rc = DosCreateEventSem (NULL,
                          &ConsoleGlobals.hevConsole,
                          0L,                        /* semaphore is private */
                          FALSE);                             /* reset state */
  if (rc != NO_ERROR)                                       /* other error ? */
    return (rc);                                    /* raise error condition */


                                     /* create console input queue semaphore */
  rc = DosCreateEventSem (NULL,
                          &ConsoleInput.hevInputQueue,
                          0L,                        /* semaphore is private */
                          FALSE);                             /* reset state */
  if (rc != NO_ERROR)                                       /* other error ? */
  {
    DosCloseEventSem(ConsoleGlobals.hevConsole);    /* close other semaphore */
    return (rc);                                    /* raise error condition */
  }


  /***************************************************************************
   * Create pseudo-devices and initialize ConsoleGlobals                     *
   ***************************************************************************/

  rc = ConsoleDevicesRegister();                 /* ensure devices are there */
  if (rc != NO_ERROR)                                    /* check for errors */
  {
    DosCloseEventSem(ConsoleGlobals.hevConsole);    /* close other semaphore */
    DosCloseEventSem(ConsoleInput.hevInputQueue);   /* close other semaphore */
    return (rc);                                    /* raise error condition */
  }


  /***************************************************************************
   * Presentation Manager Initialization phase                               *
   ***************************************************************************/

    /* OK, we're about to initialize the console subsystem for this process. */
                           /* start message thread for console object window */
  ConsoleGlobals.tidConsole = _beginthread(ConsoleMsgThread,
                                           NULL,
                                           16384,
                                           NULL);
                                   /* has the thread been created properly ? */
  if (ConsoleGlobals.tidConsole == -1)
  {
    DosCloseEventSem(ConsoleInput.hevInputQueue);   /* close other semaphore */
    DosCloseEventSem(ConsoleGlobals.hevConsole);    /* close event semaphore */
    ConsoleGlobals.hevConsole = NULLHANDLE;         /* for ConsoleIsActive() */
    return (rc);                                    /* raise error condition */
  }
  else
    DosSetPriority(PRTYS_THREAD,                             /* set priority */
                   ConsoleGlobals.Options.ulConsoleThreadPriorityClass,
                   ConsoleGlobals.Options.ulConsoleThreadPriorityDelta,
                   ConsoleGlobals.tidConsole);


                      /* wait for the child thread to do it's initialization */
                                              /* timeout isn't really useful */
  rc = DosWaitEventSem(ConsoleGlobals.hevConsole,
                       SEM_INDEFINITE_WAIT);
  if (rc != NO_ERROR)                                    /* check for errors */
  {
    DosCloseEventSem(ConsoleInput.hevInputQueue);   /* close other semaphore */
    DosCloseEventSem(ConsoleGlobals.hevConsole);    /* close event semaphore */
    ConsoleGlobals.hevConsole = NULLHANDLE;         /* for ConsoleIsActive() */
    return (rc);                                    /* raise error condition */
  }

  DosResetEventSem(ConsoleGlobals.hevConsole,       /* reset event semaphore */
                   &ulPostCount);

  rc = ConsoleGlobals.rcConsole;   /* pass thru console thread's return code */

  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : ConsoleDevicesRegister
 * Purpose   : creates and registers console devices if the standard handles
 *             are not redirected to a file
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/17 01:55]
 *****************************************************************************/

APIRET ConsoleDevicesRegister(void)
{
  DWORD  dwType;                                       /* device handle type */
  HANDLE hStandardIn;                              /* stdin handle to CONIN$ */
  HANDLE hStandardOut;                           /* stdout handle to CONOUT$ */
  HANDLE hStandardError;                         /* stderr handle to CONOUT$ */

  HMDeviceConsoleInClass     *pHMDeviceConsoleIn;
  HMDeviceConsoleOutClass    *pHMDeviceConsoleOut;
  HMDeviceConsoleBufferClass *pHMDeviceConsoleBuffer;

  DWORD rc;

  static fDevicesInitialized;          /* have we been initialized already ? */

  if (fDevicesInitialized == TRUE)                 /* OK, we're already done */
    return (NO_ERROR);
  else
    fDevicesInitialized = TRUE;

  dprintf(("KERNEL32:ConsoleDevicesRegister\n"));


  /*************************************
   * Initialize Console Window Options *
   *************************************/

  ConsoleGlobals.Options.fTerminateAutomatically = FALSE;
  ConsoleGlobals.Options.fSpeakerEnabled         = TRUE;

  ConsoleGlobals.Options.fSetWindowPosition      = FALSE;
  ConsoleGlobals.Options.coordDefaultPosition.X  = 0;
  ConsoleGlobals.Options.coordDefaultPosition.Y  = 0;
  ConsoleGlobals.Options.coordDefaultSize.X      = 80;
  ConsoleGlobals.Options.coordDefaultSize.Y      = 35;
  ConsoleGlobals.coordWindowSize.X               = ConsoleGlobals.Options.coordDefaultSize.X;
  ConsoleGlobals.coordWindowSize.Y               = ConsoleGlobals.Options.coordDefaultSize.Y;
  ConsoleGlobals.coordWindowPos.X                = 0;
  ConsoleGlobals.coordWindowPos.Y                = 0;

  ConsoleGlobals.Options.fQuickInsert            = FALSE;
  ConsoleGlobals.Options.fInsertMode             = FALSE;
  ConsoleGlobals.Options.fMouseActions           = FALSE;
  ConsoleGlobals.Options.fToolbarActive          = FALSE;

  ConsoleGlobals.Options.ucDefaultAttribute = 0x0007;  /* 07 = grey on black */

  ConsoleGlobals.Options.ulTabSize               = 8;      /* tabulator size */
  ConsoleGlobals.Options.ulUpdateLimit           = 8; /* scroll max. n lines */

                                     /* priority settings for message thread */
  ConsoleGlobals.Options.ulConsoleThreadPriorityClass = PRTYC_REGULAR;
  ConsoleGlobals.Options.ulConsoleThreadPriorityDelta = +10;

  ConsoleGlobals.Options.ucCursorDivisor = 10; /* timer divisor for blinking */

  ConsoleGlobals.ulTimerFrequency = 10;  /* cursor + blitter timer frequency */


  ConsoleGlobals.flFrameFlags = FCF_SIZEBORDER   |   /* frame creation flags */
                                FCF_TITLEBAR     |
                                FCF_SHELLPOSITION|
                                FCF_SYSMENU      |
                                FCF_TASKLIST     |
                                FCF_AUTOICON     |
                                FCF_HORZSCROLL   |
                                FCF_VERTSCROLL   |
                                FCF_MINMAX;

                                                   /* generate copy of title */
  ConsoleGlobals.pszWindowTitle = strdup(GetCommandLine());

                                    /* obtain module handle to our resources */
  rc = DosQueryModuleHandle("KERNEL32",
                            &ConsoleGlobals.hmodResource);
  if (rc != NO_ERROR)
    WriteLog("KERNEL32/CONSOLE: Can't get handle to KERNEL32 (%u).\n",
             rc);

                                             /* standard console input modes */
  ConsoleInput.dwConsoleMode = ENABLE_LINE_INPUT      |
                               ENABLE_PROCESSED_INPUT;
  /* @@@PH ENABLE_ECHO_INPUT || ENABLE_MOUSE_INPUT; */

  ConsoleGlobals.hConsoleBufferDefault = INVALID_HANDLE_VALUE;
  ConsoleGlobals.hConsoleBuffer        = INVALID_HANDLE_VALUE;


  /***************************************************************************
   * Standard handles     Initialization phase                               *
   ***************************************************************************/

                   /* create devices and register devices with handlemanager */

    pHMDeviceConsoleIn  = new HMDeviceConsoleInClass("CONIN$");
    rc = HMDeviceRegister ("CONIN$",
                           pHMDeviceConsoleIn);
    if (rc != NO_ERROR)                                  /* check for errors */
      dprintf(("KERNEL32:ConsoleDevicesRegister: registering CONIN$ failed with %u.\n",
               rc));


    pHMDeviceConsoleOut = new HMDeviceConsoleOutClass("CONOUT$");
    rc = HMDeviceRegister ("CONOUT$",
                           pHMDeviceConsoleOut);
    if (rc != NO_ERROR)                                  /* check for errors */
      dprintf(("KERNEL32:ConsoleDevicesRegister: registering CONOUT$ failed with %u.\n",
               rc));


    pHMDeviceConsoleBuffer = new HMDeviceConsoleBufferClass("CONBUFFER$");
    rc = HMDeviceRegister ("CONBUFFER$",
                           pHMDeviceConsoleBuffer);
    if (rc != NO_ERROR)                                  /* check for errors */
      dprintf(("KERNEL32:ConsoleDevicesRegister: registering CONBUFFER$ failed with %u.\n",
               rc));


      /***********************************************************************
       * initialize stdin handle                                             *
       ***********************************************************************/
      hStandardIn = GetStdHandle(STD_INPUT_HANDLE);
      dwType = GetFileType(hStandardIn);
      if (dwType == FILE_TYPE_CHAR)                /* is handle redirected ? */
        hStandardIn = HMCreateFile("CONIN$",
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL,
                                   0,
                                   CONSOLE_TEXTMODE_BUFFER,
                                   0);

      HMSetStdHandle(STD_INPUT_HANDLE,
                     hStandardIn);

      /***********************************************************************
       * initialize stdout handle                                            *
       ***********************************************************************/
      hStandardOut = GetStdHandle(STD_OUTPUT_HANDLE);
      dwType = GetFileType(hStandardOut);
      if (dwType == FILE_TYPE_CHAR)                /* is handle redirected ? */
        hStandardOut = HMCreateFile("CONOUT$",
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL,
                                   0,
                                   CONSOLE_TEXTMODE_BUFFER,
                                   0);

      HMSetStdHandle(STD_OUTPUT_HANDLE,
                     hStandardOut);


      /***********************************************************************
       * initialize stderr handle                                            *
       ***********************************************************************/
      hStandardError = GetStdHandle(STD_ERROR_HANDLE);
      dwType = GetFileType(hStandardError);
      if (dwType == FILE_TYPE_CHAR)                /* is handle redirected ? */
        hStandardError = HMCreateFile("CONOUT$",
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL,
                                   0,
                                   CONSOLE_TEXTMODE_BUFFER,
                                   0);

      HMSetStdHandle(STD_ERROR_HANDLE,
                     hStandardError);

  return (NO_ERROR);                                                   /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConsoleTerminate
 * Purpose   : Shuts the OS/2 console subsystem down
 * Parameters: VOID
 * Variables :
 * Result    : OS/2 API return code
 * Remark    : That consolebuffer handle that became allocated as default
 *             screen buffer is lost here. This would be a serious memory
 *             leak if an application keeps opening and closing the console
 *             frequently.
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

static APIRET ConsoleTerminate(VOID)
{
  APIRET rc;

  WinPostMsg (ConsoleGlobals.hwndFrame,         /* force thread to terminate */
              WM_CLOSE,
              (MPARAM)NULL,
              (MPARAM)NULL);

  rc = DosWaitThread(&ConsoleGlobals.tidConsole,/* wait until thd terminates */
                     DCWW_WAIT);

                                           /* close the consolebuffer handle */
  HMCloseHandle(ConsoleGlobals.hConsoleBufferDefault);
  free(ConsoleGlobals.pszWindowTitle);   /* free previously allocated memory */

  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : static void ConsoleWaitClose
 * Purpose   : Wait for the user to close console window
 * Parameters: VOID
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

void ConsoleWaitClose(void)
{
  CHAR szBuffer[128];                                /* buffer for the title */
  BOOL fResult;                /* result from subsequent calls to Win32 APIs */

                                /* check if there is a console window at all */
  if (ConsoleIsActive() == FALSE)
    return;                                                          /* nope */

  strcpy (szBuffer,               /* indicate console process has terminated */
          "Completed: ");

  fResult = GetConsoleTitleA(szBuffer + 11,/* 11 is length of Completed:_ */
                                sizeof(szBuffer) - 11);


                                /* Set new title: Win32 Console - Terminated */
  fResult = SetConsoleTitleA(szBuffer);

                                          /* terminate console immediately ? */
  if (ConsoleGlobals.Options.fTerminateAutomatically == FALSE)
    DosWaitThread(&ConsoleGlobals.tidConsole,   /* wait until thd terminates */
                  DCWW_WAIT);
}


/*****************************************************************************
 * Name      : static BOOL ConsoleIsActive
 * Purpose   : Check if Console window is opened
 * Parameters: VOID
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL ConsoleIsActive(void)
{
  return (NULLHANDLE != ConsoleGlobals.hevConsole);
}


/*****************************************************************************
 * Name      : static VOID ConsoleMsgThread
 * Purpose   : Message handler thread for the console object window
 * Parameters: PVOID pDummy
 * Variables :
 * Result    : is placed in Globals.rcConsole
 * Remark    : the main thread has to wait for this thread
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 02:49]
 *****************************************************************************/

static VOID ConsoleMsgThread(PVOID pParameters)
{
  APIRET rc;                                              /* API return code */


  ConsoleGlobals.rcConsole = NO_ERROR;                     /* initialization */

  ConsoleGlobals.hab = WinInitialize(0);             /* enable thread for PM */
  if (ConsoleGlobals.hab == NULLHANDLE) /* if anchor block allocation failed */
    ConsoleGlobals.rcConsole = ERROR_NOT_ENOUGH_MEMORY;
  else
  {
                                                     /* create message queue */
    ConsoleGlobals.hmq = WinCreateMsgQueue(ConsoleGlobals.hab,
                                           0);
    if (ConsoleGlobals.hmq == NULLHANDLE)  /* if msg queue allocation failed */
    {
      WinTerminate(ConsoleGlobals.hab);     /* stop thread from accessing PM */
      ConsoleGlobals.rcConsole = ERROR_NOT_ENOUGH_MEMORY;
    }
    else
    {
      if (WinRegisterClass(ConsoleGlobals.hab, /* register our class with PM */
                           SZ_CONSOLE_CLASS,
                           ConsoleWindowProc,
                           CS_SIZEREDRAW,
                           0)
          == FALSE)
      {
        WinDestroyMsgQueue(ConsoleGlobals.hmq);     /* destroy message queue */
        WinTerminate(ConsoleGlobals.hab);   /* stop thread from accessing PM */
        ConsoleGlobals.rcConsole = ERROR_NOT_ENOUGH_MEMORY;
      }
      else
      {
        ConsoleGlobals.hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                                      WS_VISIBLE,
                                                      &ConsoleGlobals.flFrameFlags,
                                                      SZ_CONSOLE_CLASS,
                                                      ConsoleGlobals.pszWindowTitle,
                                                      0,
                                                      ConsoleGlobals.hmodResource,
                                                      ID_CONSOLE_MAIN,
                                                      &ConsoleGlobals.hwndClient);
        if (ConsoleGlobals.hwndFrame == NULLHANDLE) /* check window creation */
        {
          WinDestroyMsgQueue(ConsoleGlobals.hmq);   /* destroy message queue */
          WinTerminate(ConsoleGlobals.hab); /* stop thread from accessing PM */
          ConsoleGlobals.rcConsole = ERROR_NOT_ENOUGH_MEMORY;
        } /* WinCreateStdWindow */
      } /* WinRegisterClass */
    } /* WinCreateMsgQueue */
  } /* WinInitialize */


  DosPostEventSem(ConsoleGlobals.hevConsole);      /* signal the main thread */


  if (ConsoleGlobals.rcConsole != NO_ERROR)      /* if we ran into a problem */
  {
    DosCloseEventSem(ConsoleInput.hevInputQueue);   /* close other semaphore */
    DosCloseEventSem(ConsoleGlobals.hevConsole); /* subsystem not running .. */
    ConsoleGlobals.hevConsole = NULLHANDLE;         /* for ConsoleIsActive() */
    return;                    /* abort the message queue thread immediately */
  }


  while( WinGetMsg(ConsoleGlobals.hab,                       /* message loop */
                   &ConsoleGlobals.qmsg,
                   NULLHANDLE,
                   0,
                   0) )
    WinDispatchMsg(ConsoleGlobals.hab,               /* dispatch the message */
                   &ConsoleGlobals.qmsg);

                                    /* do the cleanup, destroy window, queue */
                                    /* and stop thread from using PM         */
  WinDestroyWindow  (ConsoleGlobals.hwndFrame);
  WinDestroyMsgQueue(ConsoleGlobals.hmq);
  WinTerminate      (ConsoleGlobals.hab);

                   /* destruction of semaphore indicates console is shutdown */
  DosCloseEventSem(ConsoleInput.hevInputQueue);     /* close other semaphore */
  DosCloseEventSem(ConsoleGlobals.hevConsole);
  ConsoleGlobals.hevConsole  = NULLHANDLE;          /* for ConsoleIsActive() */
  ConsoleInput.hevInputQueue = NULLHANDLE;

  /* @@@PH we've got to exit the process here ! */
  ExitProcess(1);
}


/*****************************************************************************
 * Name      : static MRESULT EXPENTRY ConsoleWindowProc
 * Purpose   : Window Procedure for OUR console window
 * Parameters: HWND   hwnd - window handle
 *             ULONG  msg  - message identifier
 *             MPARAM mp1  - message parameter 1
 *             MPARAM mp2  - message parameter 2
 * Variables :
 * Result    : MRESULT for PM
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 03:24]
 *****************************************************************************/

static MRESULT EXPENTRY ConsoleWindowProc(HWND   hwnd,
                                          ULONG  msg,
                                          MPARAM mp1,
                                          MPARAM mp2)
{
  static RECTL rcl;                                      /* window rectangle */
  static HPS   hps;

  switch(msg)
  {
    /*************************************************************************
     * WM_CREATE window creation                                             *
     *************************************************************************/

    case WM_CREATE:
      WinPostMsg(hwnd,                            /* deferred initialization */
                 UM_CONSOLE_CREATE,
                 (MPARAM)NULL,
                 (MPARAM)NULL);
      break;


    case UM_CONSOLE_CREATE:
    {
      APIRET    rc;                                       /* API return code */
      HWND      hwndFrame;                            /* frame window handle */

                                                /* subclass the frame window */
      hwndFrame = ConsoleGlobals.hwndFrame;
      ConsoleGlobals.pfnwpFrameOriginal = WinSubclassWindow(hwndFrame,
                                                            ConsoleFrameWindowProc);

      ConsoleGlobals.hwndMenuConsole
                      = WinLoadMenu (hwnd,             /* load context menue */
                                     ConsoleGlobals.hmodResource,
                                     ID_CONSOLE_MAIN);

                                               /* set an icon for the dialog */
      ConsoleGlobals.hPtrConsole = WinLoadPointer(HWND_DESKTOP,
                                                  ConsoleGlobals.hmodResource,
                                                  ID_CONSOLE_MAIN );
      WinSendMsg(ConsoleGlobals.hwndFrame,
                 WM_SETICON,
                 (MPARAM)ConsoleGlobals.hPtrConsole,
                 0L );

               /* determine handles of the horizontal / vertical scroll bars */
      ConsoleGlobals.hwndVertScroll = WinWindowFromID(ConsoleGlobals.hwndFrame,
                                                      FID_VERTSCROLL);

      ConsoleGlobals.hwndHorzScroll = WinWindowFromID(ConsoleGlobals.hwndFrame,
                                                      FID_HORZSCROLL);

                            /* the initial state of the controls is DETACHED */
      WinSetParent(ConsoleGlobals.hwndHorzScroll,          /* detach control */
                   HWND_OBJECT,
                   FALSE);

      WinSetParent(ConsoleGlobals.hwndVertScroll,          /* detach control */
                   HWND_OBJECT,
                   FALSE);


                                           /* create AVIO presentation space */
      rc = VioCreatePS(&ConsoleGlobals.hvpsConsole,
                       ConsoleGlobals.coordWindowSize.Y,
                       ConsoleGlobals.coordWindowSize.X,
                       0,                               /* format, must be 0 */
                       FORMAT_CGA,
                       0);                       /* template hvps, must be 0 */
      if (rc != NO_ERROR)                                    /* check errors */
        WriteLog("KERNEL32/CONSOLE:VioCreatePS=%u\n",
                 rc);

 /* PH 1998/02/12 this seems to be an OS/2 PM bug:
    when doing a WinOpenWindowDC here, PM hangs. Seems it never gets back into
    the message loop. To investigate and report to IBM
  */

                               /* get a device context for the client window */
      ConsoleGlobals.hdcConsole = WinOpenWindowDC(hwnd);
                                           /* associate AVIO PS with the HDC */
      rc = VioAssociate(ConsoleGlobals.hdcConsole,
                        ConsoleGlobals.hvpsConsole);
      if (rc != NO_ERROR)                                    /* check errors */
        WriteLog("KERNEL32/CONSOLE:VioAssociate=%u\n",
                 rc);

      ConsoleFontQuery();                        /* query current cell sizes */

                                          /* adjust window size and position */
      HMDeviceRequest(ConsoleGlobals.hConsoleBuffer,
                      DRQ_INTERNAL_CONSOLEADJUSTWINDOW,
                      0,
                      0,
                      0,
                      0);

            /* @@@PH if console is maximized - now switched on per default ! */
      WinSetWindowPos (ConsoleGlobals.hwndFrame,
                       HWND_DESKTOP,
                       0,
                       0,
                       ConsoleGlobals.Options.coordDefaultPosition.X *
                         ConsoleGlobals.sCellCX +
                         2 * WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER),
                       ConsoleGlobals.Options.coordDefaultPosition.Y *
                         ConsoleGlobals.sCellCY +
                         WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) +
                         2 * WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER),
                       SWP_SIZE);

                             /* do we have to set the window position also ? */
      if (ConsoleGlobals.Options.fSetWindowPosition == TRUE)
        WinSetWindowPos (ConsoleGlobals.hwndFrame,
                         HWND_DESKTOP,
                         ConsoleGlobals.Options.coordDefaultPosition.X,
                         ConsoleGlobals.Options.coordDefaultPosition.Y,
                         0,
                         0,
                         SWP_MOVE);

                     /* start timer service for blitting and cursor blinking */
      ConsoleGlobals.idTimer = WinStartTimer (ConsoleGlobals.hab,
                                              hwnd,
                                              CONSOLE_TIMER_ID,  /* timer id */
                                              ConsoleGlobals.ulTimerFrequency);

      WinPostMsg (hwnd,   /* send myself a start message so we get the first */
                  WM_TIMER,                             /* frame immediately */
                  (MPARAM)NULL,
                  (MPARAM)NULL);
    }
    break;


    /*************************************************************************
     * WM_DESTROY window destruction                                         *
     *************************************************************************/

    case WM_DESTROY:
      WinStopTimer (ConsoleGlobals.hab,                      /* anchor block */
                    hwnd,
                    ConsoleGlobals.idTimer);                     /* timer ID */

      VioAssociate(NULL,
                   ConsoleGlobals.hvpsConsole); /* disassociates the AVIO PS */
      VioDestroyPS(ConsoleGlobals.hvpsConsole); /* destroys the AVIO PS      */

      WinDestroyWindow(ConsoleGlobals.hwndMenuConsole);
      WinDestroyPointer(ConsoleGlobals.hPtrConsole);
      break;


    /*************************************************************************
     * WM_TIMER display cursor and update AVIO PS if required                *
     *************************************************************************/

    case WM_TIMER:
                                       /* check if console has to be updated */
      if (ConsoleGlobals.fUpdateRequired == TRUE)
      {
        ConsoleGlobals.fUpdateRequired = FALSE;     /* as soon as possible ! */

                                /* as device to map itself to the VIO buffer */
        HMDeviceRequest(ConsoleGlobals.hConsoleBuffer,
                        DRQ_INTERNAL_CONSOLEBUFFERMAP,
                        0,
                        0,
                        0,
                        0);

        { /* DEBUG */
          APIRET rc;
                                           /* and now bring it to the screen */
        rc = VioShowPS(ConsoleGlobals.coordWindowSize.Y,
                       ConsoleGlobals.coordWindowSize.X,
                       0,
                       ConsoleGlobals.hvpsConsole);

          dprintf(("KERNEL32::Console::1 VioShowPS(%u,%u,%u)=%u\n",
                   ConsoleGlobals.coordWindowSize.Y,
                   ConsoleGlobals.coordWindowSize.X,
                   ConsoleGlobals.hvpsConsole,
                   rc));
        }

                                             /* cursor is overwritten here ! */
        HMDeviceRequest(ConsoleGlobals.hConsoleBuffer,
                        DRQ_INTERNAL_CONSOLECURSORSHOW,
                        CONSOLECURSOR_OVERWRITTEN,
                        0,
                        0,
                        0);
      }
      else
      {
        ConsoleGlobals.ulTimerCursor++;                  /* increase counter */
        if (ConsoleGlobals.ulTimerCursor > ConsoleGlobals.Options.ucCursorDivisor)
        {
          ConsoleGlobals.ulTimerCursor = 0;                 /* reset counter */
                                                     /* let our cursor blink */
          HMDeviceRequest(ConsoleGlobals.hConsoleBuffer,
                          DRQ_INTERNAL_CONSOLECURSORSHOW,
                          CONSOLECURSOR_BLINK,
                          0,
                          0,
                          0);
        }
      }
      break;


    /*************************************************************************
     * WM_MINMAXFRAME handle window repaint in case of iconized window       *
     *************************************************************************/

    case WM_MINMAXFRAME :
    {
      BOOL  fShow = ! (((PSWP) mp1)->fl & SWP_MINIMIZE);
      HENUM henum;
      HWND  hwndChild;

      WinEnableWindowUpdate ( hwnd, FALSE );

      for (henum=WinBeginEnumWindows(hwnd);
           (hwndChild = WinGetNextWindow (henum)) != 0; )
      WinShowWindow ( hwndChild, fShow );

      WinEndEnumWindows ( henum );
      WinEnableWindowUpdate ( hwnd, TRUE );
    }
    break;


    /*************************************************************************
     * WM_PAINT repaint the window                                           *
     *************************************************************************/
    case WM_PAINT:
      hps = WinBeginPaint(hwnd,
                          (HPS)NULL,
                          &rcl);
                                   /* blit the whole AVIO presentation space */
      { /* DEBUG */
        APIRET rc;
                                     /* and now bring it to the screen */
        rc = VioShowPS(ConsoleGlobals.coordWindowSize.Y,
                       ConsoleGlobals.coordWindowSize.X,
                       0,
                       ConsoleGlobals.hvpsConsole);

        dprintf(("KERNEL32::Console::2 VioShowPS(%u,%u,%u)=%u\n",
                 ConsoleGlobals.coordWindowSize.Y,
                 ConsoleGlobals.coordWindowSize.X,
                 ConsoleGlobals.hvpsConsole,
                 rc));
      }

      WinEndPaint(hps);
      break;


    /*************************************************************************
     * WM_SIZE resize the window                                             *
     *************************************************************************/
    case WM_SIZE:
    {
                                       /* calculate scrollbars if neccessary */
      HMDeviceRequest(ConsoleGlobals.hConsoleBuffer,
                      DRQ_INTERNAL_CONSOLEADJUSTWINDOW,
                      0,
                      0,
                      0,
                      0);

      return WinDefAVioWindowProc(hwnd,
                                  (USHORT)msg,
                                  (ULONG)mp1,
                                  (ULONG)mp2);
    }


    /*************************************************************************
     * context menue                                                         *
     *************************************************************************/
    case WM_CONTEXTMENU:
    {
      WinPopupMenu (hwnd,
                    hwnd,
                    ConsoleGlobals.hwndMenuConsole,
                    SHORT1FROMMP(mp1),
                    SHORT2FROMMP(mp1),
                    CM_CONSOLE_PROPERTIES,                        /* item id */
                    PU_HCONSTRAIN |
                    PU_VCONSTRAIN |
                    PU_KEYBOARD   |
                    PU_MOUSEBUTTON1 |
                    PU_MOUSEBUTTON2 |
                    PU_POSITIONONITEM);
    }
    return (MPARAM)FALSE;


    /*************************************************************************
     * WM_COMMAND command processing                                         *
     *************************************************************************/
    case WM_COMMAND:
    {
      switch(SHORT1FROMMP(mp1))             /* select appropriate identifier */
      {
        /* close console window, however we can't call ConsoleTerminate here!*/
        case CM_CONSOLE_EXIT:
          WinPostMsg (ConsoleGlobals.hwndFrame,
                      WM_CLOSE,
                      (MPARAM)NULL,
                      (MPARAM)NULL);

          return (MPARAM)FALSE;


        case CM_CONSOLE_REPAINT:
          WinInvalidateRect(ConsoleGlobals.hwndClient,/* redraw frame window */
                            NULL,
                            TRUE);
          return (MPARAM)FALSE;

                                               /* open the properties dialog */
        case CM_CONSOLE_PROPERTIES:
        {
          ULONG ulResult;                              /* response from user */

          ConsoleGlobals.Options.hmodResources =       /* save module handle */
            ConsoleGlobals.hmodResource;

          ulResult = WinDlgBox(HWND_DESKTOP,
                               ConsoleGlobals.hwndClient,
                               &ConsolePropertyDlgProc,
                               ConsoleGlobals.hmodResource,
                               DLG_CONSOLE_PROPERTIES,
                               (PVOID)&ConsoleGlobals.Options);
          return (MPARAM) FALSE;
        }
      }
    }
    break;


    /*************************************************************************
     * WM_CHAR keyboard char processing                                      *
     *************************************************************************/

    case WM_CHAR:
      ConsoleInputEventPushKey(mp1,                 /* push event into queue */
                               mp2);
      break;                                  /* enable further processing ! */


    /*************************************************************************
     * WM_SETFOCUS focus changing processing                                 *
     *************************************************************************/

    case WM_SETFOCUS:
      ConsoleInputEventPushFocus((BOOL)mp2);        /* push event into queue */
      break;                                  /* enable further processing ! */


    /*************************************************************************
     * WM_MOUSEMOVE mouse event processing                                   *
     *************************************************************************/

    case WM_MOUSEMOVE:
    case WM_BUTTON1UP:
    case WM_BUTTON1DOWN:
    case WM_BUTTON2UP:
    case WM_BUTTON2DOWN:
    case WM_BUTTON3UP:
    case WM_BUTTON3DOWN:
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON2DBLCLK:
    case WM_BUTTON3DBLCLK:
      ConsoleInputEventPushMouse(msg,
                                 mp1,               /* push event into queue */
                                 mp2);
      break;                                  /* enable further processing ! */
  }

  return WinDefWindowProc(hwnd,                     /* to default processing */
                          msg,
                          mp1,
                          mp2);
}


/*****************************************************************************
 * Name      : static MRESULT EXPENTRY ConsoleFrameWindowProc
 * Purpose   : Window Procedure for OUR console frame window
 * Parameters: HWND   hwnd - window handle
 *             ULONG  msg  - message identifier
 *             MPARAM mp1  - message parameter 1
 *             MPARAM mp2  - message parameter 2
 * Variables :
 * Result    : MRESULT for PM
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 03:24]
 *****************************************************************************/

static MRESULT EXPENTRY ConsoleFrameWindowProc(HWND   hwnd,
                                               ULONG  msg,
                                               MPARAM mp1,
                                               MPARAM mp2)
{
  switch(msg)
  {
    /*************************************************************************
     * WM_QUERYTRACKINFO handling                                            *
     *************************************************************************/
    case WM_QUERYTRACKINFO:
    {
       MRESULT    mr;                                      /* message result */
       PTRACKINFO pTrackInfo;           /* frame window tracking information */

                             /* let the original code do the hard work first */
       mr = ConsoleGlobals.pfnwpFrameOriginal(hwnd,
                                              msg,
                                              mp1,
                                              mp2);

       pTrackInfo = (PTRACKINFO)mp2;                /* get track information */

       /* @@@PH */
       pTrackInfo->ptlMinTrackSize.x = max(pTrackInfo->ptlMinTrackSize.x, 200);
       pTrackInfo->ptlMinTrackSize.y = max(pTrackInfo->ptlMinTrackSize.y, 100);
       pTrackInfo->ptlMaxTrackSize.x = min(pTrackInfo->ptlMaxTrackSize.x, ConsoleGlobals.coordMaxWindowPels.X);
       pTrackInfo->ptlMaxTrackSize.y = min(pTrackInfo->ptlMaxTrackSize.y, ConsoleGlobals.coordMaxWindowPels.Y);

       return (mr);                                        /* deliver result */
    }
  }

                                /* call original frame window procedure code */
  return (ConsoleGlobals.pfnwpFrameOriginal(hwnd,
                                            msg,
                                            mp1,
                                            mp2));
}


/*****************************************************************************
 * Name      : static void ConsoleBufferMap
 * Purpose   : draw a whole consolebuffer into the VIO space
 * Parameters: PCONSOLEBUFFER pConsoleBuffer
 * Variables :
 * Result    : none
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/17 12:57]
 *****************************************************************************/

static void ConsoleBufferMap(PCONSOLEBUFFER pConsoleBuffer)
{
  ULONG ulLine;

  ULONG ulSizeX;                               /* blitting length and height */
  ULONG ulSizeY;

  ulSizeX = 2 * min(ConsoleGlobals.coordWindowSize.X,
                    pConsoleBuffer->coordBufferSize.X -
                    ConsoleGlobals.coordWindowPos.X);

  ulSizeY = min(ConsoleGlobals.coordWindowSize.Y,
                pConsoleBuffer->coordBufferSize.Y -
                ConsoleGlobals.coordWindowPos.Y);

                      /* check if we're called with non-existing line buffer */
  if (pConsoleBuffer->ppszLine == NULL)
    return;

  for (ulLine = ConsoleGlobals.coordWindowPos.Y;
       ulLine < ulSizeY;
       ulLine++)
    VioWrtCellStr(pConsoleBuffer->ppszLine[ulLine] +
                    ConsoleGlobals.coordWindowPos.X,
                  ulSizeX,
                  ulLine,
                  0,
                  ConsoleGlobals.hvpsConsole);
}


/*****************************************************************************
 * Name      : static void ConsoleBufferFillLine
 * Purpose   : fills a line with a certain output pattern
 * Parameters: ULONG ulPattern, PUSHORT pusTarget, ULONG ulSize
 * Variables :
 * Result    : none
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/17 12:57]
 *****************************************************************************/

static void ConsoleBufferFillLine(ULONG   ulPattern,
                                  PUSHORT pusTarget,
                                  ULONG   ulSize)
{
  ULONG  ulCounter;

  for (ulCounter = 0;
       ulCounter < (ulSize >> 1);
       ulCounter++,
       pusTarget+=2)
    *(PULONG)pusTarget = ulPattern;

  if (ulSize & 0x00000001)
    *pusTarget = (USHORT)ulPattern;
}


/*****************************************************************************
 * Name      : static void ConsoleBufferScrollUp
 * Purpose   : scroll whole buffer n lines up,
 *             fill new lines with default attribute
 * Parameters: PCONSOLEBUFFER pConsoleBuffer
 *             ULONG          ulLines
 * Variables :
 * Result    : none
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/17 12:57]
 *****************************************************************************/

static void ConsoleBufferScrollUp(PCONSOLEBUFFER pConsoleBuffer,
                                  ULONG          ulLines)
{
  ULONG ulLine;
  ULONG ulPosition;
  ULONG ulScrollLine;

  static ULONG ulUpdateCounter;                /* counter for jump-scrolling */

                              /* calculate new line offset to the first line */
  pConsoleBuffer->ulScrollLineOffset += ulLines;
  pConsoleBuffer->ulScrollLineOffset %= pConsoleBuffer->coordBufferSize.Y;

                                                   /* do we have to scroll ? */
  if (ulLines < pConsoleBuffer->coordBufferSize.Y)
  {
    for (ulLine = 0;                                     /* do the scrolling */
         ulLine < ConsoleGlobals.coordWindowSize.Y;
         ulLine++)
    {
      ulScrollLine = (ulLine + pConsoleBuffer->ulScrollLineOffset)
                     % pConsoleBuffer->coordBufferSize.Y;

      ulPosition = (ULONG)pConsoleBuffer->ppszLine
                   + (pConsoleBuffer->coordBufferSize.Y * sizeof (PSZ) )
                   + (pConsoleBuffer->coordBufferSize.X * 2 * ulScrollLine);

      pConsoleBuffer->ppszLine[ulLine] = (PSZ)ulPosition;
    }
  }

                                                  /* enforce the upper limit */
  if (ulLines > pConsoleBuffer->coordBufferSize.Y)
    ulLines = pConsoleBuffer->coordBufferSize.Y;

  ulPosition = ( ((ULONG)(pConsoleBuffer->ucDefaultAttribute) << 8) +
                 ((ULONG)' ') +
                 ((ULONG)(pConsoleBuffer->ucDefaultAttribute) << 24) +
                 ((ULONG)' ' << 16) );

                                                    /* scroll the line index */
  for (ulLine = pConsoleBuffer->coordBufferSize.Y - ulLines;
       ulLine < pConsoleBuffer->coordBufferSize.Y;
       ulLine++)
    ConsoleBufferFillLine(ulPosition,
                          (PUSHORT)(pConsoleBuffer->ppszLine[ulLine]),
                          pConsoleBuffer->coordBufferSize.X);

    /* this code ensures frequent screen updating, even if the timer prooves */
                                                            /* to be to slow */
  ulUpdateCounter++;
  if (ulUpdateCounter > ConsoleGlobals.Options.ulUpdateLimit)
  {
    ulUpdateCounter = 0;                                /* reset the counter */
    ConsoleBufferMap(pConsoleBuffer);
    VioShowPS(ConsoleGlobals.coordWindowSize.Y,
              ConsoleGlobals.coordWindowSize.X,
              0,
              ConsoleGlobals.hvpsConsole);
    ConsoleGlobals.fUpdateRequired = FALSE;          /* no more required ... */
  }
}


/*****************************************************************************
 * Name      : static APIRET ConsoleInputEventPush
 * Purpose   : add an element to the console input queue
 * Parameters: PINPUT_RECORD pInputRecord
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static APIRET ConsoleInputEventPush(PINPUT_RECORD pInputRecord)
{
  PINPUT_RECORD pirFree;                           /* pointer to free record */
  APIRET        rc;                                        /* API-returncode */

#ifdef DEBUG_LOCAL2
  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPush(%08x).\n",
           pInputRecord));
#endif
                                                           /* get free event */
  pirFree = &ConsoleInput.arrInputRecord[ConsoleInput.ulIndexFree];
  if (pirFree->EventType != 0x0000)
    return (ERROR_QUE_NO_MEMORY);                         /* queue is full ! */

                                                       /* put event in queue */
  memcpy(pirFree,                                               /* copy data */
         pInputRecord,
         sizeof (INPUT_RECORD) );

  ConsoleInput.ulIndexFree++;                        /* update index counter */
  if (ConsoleInput.ulIndexFree >= CONSOLE_INPUTQUEUESIZE)
    ConsoleInput.ulIndexFree = 0;

  ConsoleInput.ulEvents++;                   /* increate queue event counter */

                                                  /* unblock reading threads */
  rc = DosPostEventSem(ConsoleInput.hevInputQueue);
  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConsoleInputEventPop
 * Purpose   : read first element from the console input queue
 * Parameters: PINPUT_RECORD pInputRecord
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static APIRET ConsoleInputEventPop(PINPUT_RECORD pInputRecord)
{
  PINPUT_RECORD pirEvent;                         /* pointer to event record */
  APIRET        rc;                                        /* API-returncode */

#ifdef DEBUG_LOCAL2
  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPop(%08x).\n",
           pInputRecord));
#endif

  if (ConsoleInput.ulEvents == 0)                         /* empty console ? */
    return (ERROR_QUE_EMPTY);                            /* queue is empty ! */

                                                          /* get first event */
  pirEvent = &ConsoleInput.arrInputRecord[ConsoleInput.ulIndexEvent];
  if (pirEvent->EventType == 0x0000)
    return (ERROR_QUE_EMPTY);                            /* queue is empty ! */

                                                       /* put event in queue */
  memcpy(pInputRecord,                                          /* copy data */
         pirEvent,
         sizeof (INPUT_RECORD) );

  pirEvent->EventType = 0x0000;                 /* mark event as read = free */

  if (ConsoleInput.ulEvents >= 0)       /* decrease number of console events */
    ConsoleInput.ulEvents--;

  ConsoleInput.ulIndexEvent++;                       /* update index counter */
  if (ConsoleInput.ulIndexEvent >= CONSOLE_INPUTQUEUESIZE)
    ConsoleInput.ulIndexEvent = 0;

  return (NO_ERROR);                                                   /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConsoleInputEventPushKey
 * Purpose   : push key event into the queue
 * Parameters: MPARAM mp1, MPARAM mp2 from WM_CHAR processing
 * Variables :
 * Result    : API returncode
 * Remark    : @@@PH: 2nd table that learns codes automatically from "down"
 *                    messages from PM. With Alt-a, etc. it is 0 for "up" ?
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

#define TABVIRTUALKEYCODES 0x6D
static tabVirtualKeyCodes[TABVIRTUALKEYCODES] =
{
/* --- PM key -- NT key --- */
   /*              0x00 */ 0,
   /* VK_BUTTON1   0x01 */ 0x01, /* WIN_VK_LBUTTON ??? */
   /* VK_BUTTON2   0x02 */ 0x02, /* WIN_VK_RBUTTON ??? */
   /* VK_BUTTON3   0x03 */ 0x04, /* WIN_VK_MBUTTON ??? */
   /* VK_BREAK     0x04 */ 0x03, /* WIN_VK_CANCEL  ??? */
   /* VK_BACKSPACE 0x05 */ 0x08, /* WIN_VK_BACK    */
   /* VK_TAB       0x06 */ 0x09, /* WIN_VK_TAB     */
   /* VK_BACKTAB   0x07 */ 0,
   /* VK_NEWLINE   0x08 */ 0,
   /* VK_SHIFT     0x09 */ 0x10, /* WIN_VK_SHIFT   */
   /* VK_CTRL      0x0A */ 0x11, /* WIN_VK_CONTROL */
   /* VK_ALT       0x0B */ 0x12, /* WIN_VK_MENU    */
   /* VK_ALTGRAF   0x0C */ 0,
   /* VK_PAUSE     0x0D */ 0x13, /* WIN_VK_PAUSE   */
   /* VK_CAPSLOCK  0x0E */ 0x14, /* WIN_VK_CAPITAL ??? */
   /* VK_ESC       0x0F */ 0x1b, /* WIN_VK_ESCAPE  */
   /* VK_SPACE     0x10 */ 0x20, /* WIN_VK_SPACE   */
   /* VK_PAGEUP    0x11 */ 0x21, /* WIN_VK_PRIOR   ??? */
   /* VK_PAGEDOWN  0x12 */ 0x22, /* WIN_VK_NEXT    ??? */
   /* VK_END       0x13 */ 0x23, /* WIN_VK_END     */
   /* VK_HOME      0x14 */ 0x24, /* WIN_VK_HOME    */
   /* VK_LEFT      0x15 */ 0x25, /* WIN_VK_LEFT    */
   /* VK_UP        0x16 */ 0x26, /* WIN_VK_UP      */
   /* VK_RIGHT     0x17 */ 0x27, /* WIN_VK_RIGHT   */
   /* VK_DOWN      0x18 */ 0x28, /* WIN_VK_DOWN    */
   /* VK_PRINTSCRN 0x19 */ 0x2A, /* WIN_VK_PRINT   */
   /* VK_INSERT    0x1A */ 0x2D, /* WIN_VK_INSERT  */
   /* VK_DELETE    0x1B */ 0x2E, /* WIN_VK_DELETE  */
   /* VK_SCRLLOCK  0x1C */ 0x91, /* WIN_VK_SCROLL  */
   /* VK_NUMLOCK   0x1D */ 0x90, /* WIN_VK_NUMLOCK */
   /* VK_ENTER     0x1E */ 0x0D, /* WIN_VK_RETURN  */
   /* VK_SYSRQ     0x1F */ 0,
   /* VK_F1        0x20 */ 0x70, /* WIN_VK_F1      */
   /* VK_F2        0x21 */ 0x71, /* WIN_VK_F2      */
   /* VK_F3        0x22 */ 0x72, /* WIN_VK_F3      */
   /* VK_F4        0x23 */ 0x73, /* WIN_VK_F4      */
   /* VK_F5        0x24 */ 0x74, /* WIN_VK_F5      */
   /* VK_F6        0x25 */ 0x75, /* WIN_VK_F6      */
   /* VK_F7        0x26 */ 0x76, /* WIN_VK_F7      */
   /* VK_F8        0x27 */ 0x77, /* WIN_VK_F8      */
   /* VK_F9        0x28 */ 0x78, /* WIN_VK_F9      */
   /* VK_F10       0x29 */ 0x79, /* WIN_VK_F10     */
   /* VK_F11       0x2A */ 0x7A, /* WIN_VK_F11     */
   /* VK_F12       0x2B */ 0x7B, /* WIN_VK_F12     */
   /* VK_F13       0x2C */ 0x7C, /* WIN_VK_F13     */
   /* VK_F14       0x2D */ 0x7D, /* WIN_VK_F14     */
   /* VK_F15       0x2E */ 0x7E, /* WIN_VK_F15     */
   /* VK_F16       0x2F */ 0x7F, /* WIN_VK_F16     */
   /* VK_F17       0x30 */ 0x80, /* WIN_VK_F17     */
   /* VK_F18       0x31 */ 0x81, /* WIN_VK_F18     */
   /* VK_F19       0x32 */ 0x82, /* WIN_VK_F19     */
   /* VK_F20       0x33 */ 0x83, /* WIN_VK_F20     */
   /* VK_F21       0x34 */ 0x84, /* WIN_VK_F21     */
   /* VK_F22       0x35 */ 0x85, /* WIN_VK_F22     */
   /* VK_F23       0x36 */ 0x86, /* WIN_VK_F23     */
   /* VK_F24       0x37 */ 0x87, /* WIN_VK_F24     */
   /* VK_ENDDRAG   0x38 */ 0,
   /* VK_CLEAR     0x39 */ 0x0C, /* WIN_VK_CLEAR   */
   /* VK_EREOF     0x3A */ 0xF9, /* WIN_VK_EREOF   */
   /* VK_PA1       0x3B */ 0xFD, /* WIN_VK_PA1     */
   /* VK_ATTN      0x3C */ 0xF6, /* WIN_VK_ATTN    */
   /* VK_CRSEL     0x3D */ 0xF7, /* WIN_VK_CRSEL   */
   /* VK_EXSEL     0x3E */ 0xF8, /* WIN_VK_EXSEL   */
   /* VK_COPY      0x3F */ 0,
   /* VK_BLK1      0x40 */ 0,
   /* VK_BLK2      0x41 */ 0,
   /*              0x42 */ 0,
   /*              0x43 */ 0,
   /*              0x44 */ 0,
   /*              0x45 */ 0,
   /*              0x46 */ 0,
   /*              0x47 */ 0,
   /*              0x48 */ 0,
   /*              0x49 */ 0,
   /*              0x4A */ 0,
   /*              0x4B */ 0,
   /*              0x4C */ 0,
   /*              0x4D */ 0,
   /*              0x4E */ 0,
   /*              0x4F */ 0,
   /* from UNIKBD.H:             */
   /* VK_PA2              0x0050 */ 0,
   /* VK_PA3              0x0051 */ 0,
   /* VK_GROUP            0x0052 */ 0,
   /* VK_GROUPLOCK        0x0053 */ 0,
   /* VK_APPL             0x0054 */ 0x5D, /* WIN_VK_APPS ??? */
   /* VK_WINLEFT          0x0055 */ 0x5B, /* WIN_VK_LWIN */
   /* VK_WINRIGHT         0x0056 */ 0x5C, /* WIN_VK_RWIN */
   /*                     0x0057 */ 0,
   /*                     0x0058 */ 0,
   /*                     0x0059 */ 0,
   /*                     0x005A */ 0,
   /*                     0x005B */ 0,
   /*                     0x005C */ 0,
   /*                     0x005D */ 0,
   /*                     0x005E */ 0,
   /*                     0x005F */ 0,
   /*                     0x0060 */ 0,
   /* VK_M_DOWNLEFT       0x0061 */ 0,
   /* VK_M_DOWN           0x0062 */ 0,
   /* VK_M_DOWNRIGHT      0x0063 */ 0,
   /* VK_M_LEFT           0x0064 */ 0,
   /* VK_M_CENTER         0x0065 */ 0,
   /* VK_M_RIGHT          0x0066 */ 0,
   /* VK_M_UPLEFT         0x0067 */ 0,
   /* VK_M_UP             0x0068 */ 0,
   /* VK_M_UPRIGHT        0x0069 */ 0,
   /* VK_M_BUTTONLOCK     0x006A */ 0,
   /* VK_M_BUTTONRELEASE  0x006B */ 0,
   /* VK_M_DOUBLECLICK    0x006C */ 0,

#if 0
0xA4, /* WIN_VK_LMENU   ??? */
0xA5, /* WIN_VK_RMENU   ??? */
#define VK_SELECT         0x29
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_HELP           0x2F
#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_PROCESSKEY     0xE5
#define VK_PLAY           0xFA
#define VK_ZOOM           0xFB
#define VK_NONAME         0xFC
#define VK_OEM_CLEAR      0xFE
#endif

};


static APIRET ConsoleInputEventPushKey(MPARAM mp1,
                                       MPARAM mp2)
{
  INPUT_RECORD InputRecord;                    /* the input record structure */
  APIRET       rc;                                         /* API-returncode */
  USHORT       fsFlags    = ((ULONG)mp1 & 0x0000ffff);             /* get key flags */
  UCHAR        ucRepeat   = ((ULONG)mp1 & 0x00ff0000) >> 16;
  UCHAR        ucScanCode = ((ULONG)mp1 & 0xff000000) >> 24;
  UCHAR        usCh       = ((ULONG)mp2 & 0x0000ffff);
  USHORT       usVk       = ((ULONG)mp2 & 0xffff0000) >> 16;
  UCHAR        ucChar     = usCh & 0x00ff;

#ifdef DEBUG_LOCAL2
  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPushKey(%08x,%08x).\n",
           mp1,
           mp2));
#endif


  InputRecord.EventType = KEY_EVENT;                 /* fill event structure */
  InputRecord.Event.KeyEvent.dwControlKeyState = 0;

  if (fsFlags & KC_SHIFT) InputRecord.Event.KeyEvent.dwControlKeyState |= SHIFT_PRESSED;
  if (fsFlags & KC_ALT)   InputRecord.Event.KeyEvent.dwControlKeyState |= LEFT_ALT_PRESSED;
  if (fsFlags & KC_CTRL)  InputRecord.Event.KeyEvent.dwControlKeyState |= LEFT_CTRL_PRESSED;

  /* @@@PH no support for RIGHT_ALT_PRESSED,
                          RIGHT_CTRL_PRESSED,
                          NUMLOCK_ON,
                          SCROLLLOCK_ON,
                          CAPSLOCK_ON,
                          ENHANCED_KEY
   */

  InputRecord.Event.KeyEvent.bKeyDown         = !(fsFlags & KC_KEYUP);
  InputRecord.Event.KeyEvent.wRepeatCount     = ucRepeat;
  InputRecord.Event.KeyEvent.wVirtualKeyCode  = usVk;
  InputRecord.Event.KeyEvent.wVirtualScanCode = ucScanCode;

             /* check if ascii is valid, if so then wVirtualKeyCode = ascii, */
             /* else go through the table                                    */
  if (fsFlags & KC_CHAR) /* usCh valid ? */
  {
          /* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
          /* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */
    if ( ( (usCh >= 'a') && (usCh <= 'z') ) ||                /* lowercase ? */
         ( (usCh >= '0') && (usCh <= '9') )
       )
      InputRecord.Event.KeyEvent.wVirtualKeyCode = usCh & 0xDF;
    else
      InputRecord.Event.KeyEvent.wVirtualKeyCode = usCh;
  }
  else
    if (fsFlags & KC_VIRTUALKEY)          /* translate OS/2 virtual key code */
    {
      if (usVk < TABVIRTUALKEYCODES)                  /* limit to table size */
        InputRecord.Event.KeyEvent.wVirtualKeyCode =
          tabVirtualKeyCodes[usVk];                     /* translate keycode */
    }

                /* this is a workaround for empty / invalid wVirtualKeyCodes */
  if (InputRecord.Event.KeyEvent.wVirtualKeyCode == 0x0000)
  {
    if ( ( (usCh >= 'a') && (usCh <= 'z') ) ||                /* lowercase ? */
         ( (usCh >= '0') && (usCh <= '9') )
       )
      InputRecord.Event.KeyEvent.wVirtualKeyCode = usCh & 0xDF;
    else
      InputRecord.Event.KeyEvent.wVirtualKeyCode = usCh;
  }


  /* @@@PH handle special keys */
  if ( (ucChar != 0xe0) && (ucChar != 0x00) )
    InputRecord.Event.KeyEvent.uChar.AsciiChar  = ucChar;
  else
  {
    /* extended key ! */
    InputRecord.Event.KeyEvent.dwControlKeyState |= ENHANCED_KEY;
    InputRecord.Event.KeyEvent.uChar.AsciiChar  = (ucChar >> 8);
  }

              /* further processing according the current input console mode */
  if (ConsoleInput.dwConsoleMode & ENABLE_PROCESSED_INPUT)
  {
    /* filter ctrl-c, etc. */
  }

#if 0
  /* DEBUG */
  dprintf(("DEBUG: mp1=%08x mp2=%08x\n",
           mp1,
           mp2));
  dprintf(("DEBUG: fsFlags = %04x repeat=%u hwscan=%2x",
           fsFlags,
           ucRepeat,
           ucScanCode ));
  dprintf((" uscc=%04x usvk=%04x\n",
           SHORT1FROMMP(mp2),
           SHORT2FROMMP(mp2)));

  dprintf(("DEBUG: ascii=[%c] (%02x)",
           InputRecord.Event.KeyEvent.uChar.AsciiChar,
           InputRecord.Event.KeyEvent.uChar.AsciiChar));
#endif

  rc = ConsoleInputEventPush(&InputRecord);           /* add it to the queue */
  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConsoleInputEventPushMouse
 * Purpose   : push mouse event into the queue
 * Parameters: MPARAM mp1, MPARAM mp2 from WM_MOUSEMOVE processing
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static APIRET ConsoleInputEventPushMouse(ULONG  ulMessage,
                                         MPARAM mp1,
                                         MPARAM mp2)
{
  INPUT_RECORD     InputRecord;                /* the input record structure */
  APIRET             rc;                                   /* API-returncode */
  USHORT             fsFlags = SHORT2FROMMP(mp2);           /* get key flags */
  static USHORT      usButtonState;     /* keeps track of mouse button state */

                                      /* do we have to process mouse input ? */
  if ( !(ConsoleInput.dwConsoleMode & ENABLE_MOUSE_INPUT))
    return (NO_ERROR);                                 /* return immediately */

  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPushMouse(%08x,%08x,%08x).\n",
           ulMessage,
           mp1,
           mp2));

  memset(&InputRecord,                                 /* zero the structure */
         0,
         sizeof (INPUT_RECORD) );

  InputRecord.EventType = MOUSE_EVENT;               /* fill event structure */

  switch (ulMessage)
  {
    case WM_MOUSEMOVE:
      InputRecord.Event.MouseEvent.dwEventFlags      = MOUSE_MOVED;
      InputRecord.Event.MouseEvent.dwMousePosition.X = SHORT1FROMMP(mp1);
      InputRecord.Event.MouseEvent.dwMousePosition.Y = SHORT2FROMMP(mp1);

      InputRecord.Event.MouseEvent.dwButtonState     = usButtonState;

      if (fsFlags & KC_SHIFT) InputRecord.Event.MouseEvent.dwControlKeyState |= SHIFT_PRESSED;
      if (fsFlags & KC_ALT)   InputRecord.Event.MouseEvent.dwControlKeyState |= LEFT_ALT_PRESSED;
      if (fsFlags & KC_CTRL)  InputRecord.Event.MouseEvent.dwControlKeyState |= LEFT_CTRL_PRESSED;

      /* @@@PH no support for RIGHT_ALT_PRESSED,
                              RIGHT_CTRL_PRESSED,
                              NUMLOCK_ON,
                              SCROLLLOCK_ON,
                              CAPSLOCK_ON,
                              ENHANCED_KEY
       */
      break;

    case WM_BUTTON1UP:
      usButtonState            &= ~FROM_LEFT_1ST_BUTTON_PRESSED;
      InputRecord.Event.MouseEvent.dwButtonState = usButtonState;
      break;

    case WM_BUTTON1DOWN:
      usButtonState            |=  FROM_LEFT_1ST_BUTTON_PRESSED;
      InputRecord.Event.MouseEvent.dwButtonState = usButtonState;
      break;

    case WM_BUTTON2UP:
      usButtonState &= ~FROM_LEFT_2ND_BUTTON_PRESSED;
      InputRecord.Event.MouseEvent.dwButtonState = usButtonState;
      break;

    case WM_BUTTON2DOWN:
      usButtonState |= FROM_LEFT_2ND_BUTTON_PRESSED;
      InputRecord.Event.MouseEvent.dwButtonState = usButtonState;
      break;

    case WM_BUTTON3UP:
      usButtonState &= ~FROM_LEFT_3RD_BUTTON_PRESSED;
      InputRecord.Event.MouseEvent.dwButtonState = usButtonState;
      break;

    case WM_BUTTON3DOWN:
      usButtonState |=  FROM_LEFT_3RD_BUTTON_PRESSED;
      InputRecord.Event.MouseEvent.dwButtonState = usButtonState;
      break;

    case WM_BUTTON1DBLCLK:
      InputRecord.Event.MouseEvent.dwEventFlags = DOUBLE_CLICK;
      InputRecord.Event.MouseEvent.dwButtonState = FROM_LEFT_1ST_BUTTON_PRESSED;
      usButtonState &= ~FROM_LEFT_1ST_BUTTON_PRESSED;
      break;

    case WM_BUTTON2DBLCLK:
      InputRecord.Event.MouseEvent.dwEventFlags = DOUBLE_CLICK;
      InputRecord.Event.MouseEvent.dwButtonState = FROM_LEFT_2ND_BUTTON_PRESSED;
      usButtonState &= ~FROM_LEFT_2ND_BUTTON_PRESSED;
      break;

    case WM_BUTTON3DBLCLK:
      InputRecord.Event.MouseEvent.dwEventFlags = DOUBLE_CLICK;
      InputRecord.Event.MouseEvent.dwButtonState = FROM_LEFT_3RD_BUTTON_PRESSED;
      usButtonState &= ~FROM_LEFT_3RD_BUTTON_PRESSED;
      break;
  }

                        /* @@@PH pseudo-support for RIGHTMOST_BUTTON_PRESSED */
  if (InputRecord.Event.MouseEvent.dwButtonState & FROM_LEFT_3RD_BUTTON_PRESSED)
    InputRecord.Event.MouseEvent.dwButtonState |= RIGHTMOST_BUTTON_PRESSED;

  rc = ConsoleInputEventPush(&InputRecord);           /* add it to the queue */
  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConsoleInputEventPushWindow
 * Purpose   : push menu event into the queue
 * Parameters: DWORD dwCommandId
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static APIRET ConsoleInputEventPushWindow(COORD coordWindowSize)
{
  INPUT_RECORD     InputRecord;                /* the input record structure */
  APIRET           rc;                                     /* API-returncode */

                                     /* do we have to process window input ? */
  if ( !(ConsoleInput.dwConsoleMode & ENABLE_WINDOW_INPUT))
    return (NO_ERROR);                                 /* return immediately */

  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPushWindow(x = %u, y = %u).\n",
           coordWindowSize.X,
           coordWindowSize.Y));

  InputRecord.EventType = WINDOW_BUFFER_SIZE_EVENT;  /* fill event structure */

  InputRecord.Event.WindowBufferSizeEvent.dwSize = coordWindowSize;

  rc = ConsoleInputEventPush(&InputRecord);           /* add it to the queue */
  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConsoleInputEventPushMenu
 * Purpose   : push window event into the queue
 * Parameters: COORD coordWindowSize
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static APIRET ConsoleInputEventPushMenu(DWORD dwCommandId)
{
  INPUT_RECORD     InputRecord;                /* the input record structure */
  APIRET           rc;                                     /* API-returncode */

  /* @@@PH this is unknown to me - there's no separate bit for menu events ? */
                                     /* do we have to process window input ? */
  if ( !(ConsoleInput.dwConsoleMode & ENABLE_WINDOW_INPUT))
    return (NO_ERROR);                                 /* return immediately */

  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPushMenu(%08x).\n",
           dwCommandId));

  InputRecord.EventType = MENU_EVENT;                /* fill event structure */

  InputRecord.Event.MenuEvent.dwCommandId = dwCommandId;

  rc = ConsoleInputEventPush(&InputRecord);           /* add it to the queue */
  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : static APIRET ConsoleInputEventPushFocus
 * Purpose   : push focus event into the queue
 * Parameters: BOOL bSetFocus
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static APIRET ConsoleInputEventPushFocus(BOOL bSetFocus)
{
  INPUT_RECORD     InputRecord;                /* the input record structure */
  APIRET           rc;                                     /* API-returncode */

  /* @@@PH this is unknown to me - there's no separate bit for menu events ? */
                                     /* do we have to process window input ? */
  if ( !(ConsoleInput.dwConsoleMode & ENABLE_WINDOW_INPUT))
    return (NO_ERROR);                                 /* return immediately */

  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPushFocus(%08x).\n",
           bSetFocus));

  InputRecord.EventType = FOCUS_EVENT;               /* fill event structure */

  InputRecord.Event.FocusEvent.bSetFocus = bSetFocus;

  rc = ConsoleInputEventPush(&InputRecord);           /* add it to the queue */
  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : static ULONG ConsoleInputQueueEvents
 * Purpose   : query number of events in the queue
 * Parameters:
 * Variables :
 * Result    : number of events
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static ULONG ConsoleInputQueryEvents (void)
{
  return (ConsoleInput.ulEvents);        /* return number of events in queue */
}


/*****************************************************************************
 * Name      : static void ConsoleCursorShow
 * Purpose   : query number of events in the queue
 * Parameters:
 * Variables :
 * Result    : number of events
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static void ConsoleCursorShow (PCONSOLEBUFFER pConsoleBuffer,
                               ULONG          ulCursorMode)
{
  HPS   hps;                                    /* presentation space handle */
  RECTL rclCursor;                                   /* the cursor rectangle */
  static BOOL fState;                                /* current cursor state */
  RECTL rclWindow;                                    /* current window size */

#ifdef DEBUG_LOCAL2
  dprintf(("KERNEL32:Console:ConsoleCursorShow(%u)\n",
           ulCursorMode));
#endif

  if (pConsoleBuffer->CursorInfo.bVisible == FALSE)/* cursor is switched off */
    return;                                            /* return immediately */

  switch (ulCursorMode)
  {
    case CONSOLECURSOR_HIDE:
      if (fState == FALSE)                       /* cursor currently shown ? */
        return;                                     /* no, abort immediately */
      else
        fState = FALSE;       /* set to invisible and invert our cursor rect */
      break;

    case CONSOLECURSOR_SHOW:
      if (fState == TRUE)                        /* cursor currently shown ? */
        return;                                     /* yes,abort immediately */
      else
        fState = TRUE;          /* set to visible and invert our cursor rect */
      break;

    case CONSOLECURSOR_BLINK:
      fState = !fState;      /* let there be on off on off on off on off ... */
      break;

    case CONSOLECURSOR_OVERWRITTEN:       /* our cursor has been overwritten */
      fState = TRUE;                       /* so show the cursor immediately */
      break;
  }


                                              /* query current window's size */
  WinQueryWindowRect(ConsoleGlobals.hwndClient,
                     &rclWindow);

                                      /* calculate coordinates of the cursor */
  rclCursor.xLeft   = ConsoleGlobals.sCellCX * pConsoleBuffer->coordCursorPosition.X;
  rclCursor.xRight  = rclCursor.xLeft + ConsoleGlobals.sCellCX;
  rclCursor.yBottom = rclWindow.yTop
                      - ConsoleGlobals.sCellCY * (pConsoleBuffer->coordCursorPosition.Y + 1);
  rclCursor.yTop    = rclCursor.yBottom +        /* cursor height in percent */
                      (ConsoleGlobals.sCellCY *
                       pConsoleBuffer->CursorInfo.dwSize /
                       100);

  hps = WinGetPS(ConsoleGlobals.hwndClient);                      /* get HPS */

  /* @@@PH invert coordinates here ... */
  WinInvertRect(hps,                  /* our cursor is an inverted rectangle */
                &rclCursor);

  WinReleasePS(hps);                                /* release the hps again */
}


/*****************************************************************************
 * Name      : static APIRET ConsoleFontQuery
 * Purpose   : queries the current font cell sizes
 * Parameters:
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/03/07 16:55]
 *****************************************************************************/

static APIRET ConsoleFontQuery (void)
{
  return(VioGetDeviceCellSize(&ConsoleGlobals.sCellCY,  /* query VIO manager */
                              &ConsoleGlobals.sCellCX,
                              ConsoleGlobals.hvpsConsole));
}


/*****************************************************************************
 * Name      : static void ConsoleCursorShow
 * Purpose   : query number of events in the queue
 * Parameters:
 * Variables :
 * Result    : number of events
 * Remark    : called during INIT, FONTCHANGE, RESIZE, BUFFERCHANGE
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/04/29 16:55]
 *****************************************************************************/

static void ConsoleAdjustWindow (PCONSOLEBUFFER pConsoleBuffer)
{
  LONG   lX, lY;                                    /* temporary long values */
  RECTL  rcl;
  PRECTL pRcl = &rcl;
  ULONG  flStyle;                              /* window frame control style */

  BOOL fNeedVertScroll;                      /* indicates need of scrollbars */
  BOOL fNeedHorzScroll;

  LONG lScrollX;                           /* width and height of scrollbars */
  LONG lScrollY;

                                         /* now calculate actual window size */
  lX = ConsoleGlobals.sCellCX * ConsoleGlobals.coordWindowSize.X;
  lY = ConsoleGlobals.sCellCY * ConsoleGlobals.coordWindowSize.Y;

  if ( (ConsoleGlobals.sCellCX == 0) ||          /* prevent division by zero */
       (ConsoleGlobals.sCellCY == 0) )
    return;

         /* calculate maximum console window size in pixels for the tracking */
  ConsoleGlobals.coordMaxWindowPels.X = ConsoleGlobals.sCellCX * pConsoleBuffer->coordWindowSize.X
                                        + WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER) * 2;

  ConsoleGlobals.coordMaxWindowPels.Y = ConsoleGlobals.sCellCY * pConsoleBuffer->coordWindowSize.Y
                                        + WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER) * 2
                                        + WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);

  /***************************/
  /* @@@PH broken code below */
  /***************************/
  return;

                             /* add the window border height and width, etc. */
  WinQueryWindowRect (ConsoleGlobals.hwndClient,
                      pRcl);

                                                   /* calculate visible area */
   /* calculate real client window rectangle and take care of the scrollbars */
  lScrollX = WinQuerySysValue(HWND_DESKTOP, SV_CXVSCROLL);
  lScrollY = WinQuerySysValue(HWND_DESKTOP, SV_CYHSCROLL);
  if (ConsoleGlobals.fHasHorzScroll)
  {
    lY += lScrollY;
    ConsoleGlobals.coordMaxWindowPels.Y += lScrollY;
  }

  if (ConsoleGlobals.fHasVertScroll)
  {
    lX += lScrollX;
    ConsoleGlobals.coordMaxWindowPels.X += lScrollX;
  }

  /* @@@PH might NOT exceed maximum VioPS size ! */
  ConsoleGlobals.coordWindowSize.X = (pRcl->xRight - pRcl->xLeft)
                                     / ConsoleGlobals.sCellCX;

  ConsoleGlobals.coordWindowSize.Y = (pRcl->yTop   - pRcl->yBottom)
                                     / ConsoleGlobals.sCellCY;

                                    /* do we have to enable the scrollbars ? */
  fNeedHorzScroll = lX < pConsoleBuffer->coordWindowSize.X * ConsoleGlobals.sCellCX;
  fNeedVertScroll = lY < pConsoleBuffer->coordWindowSize.Y * ConsoleGlobals.sCellCY;


  if ( (ConsoleGlobals.fHasVertScroll != fNeedVertScroll) ||
       (ConsoleGlobals.fHasHorzScroll != fNeedHorzScroll) )
  {
    flStyle = WinQueryWindowULong(ConsoleGlobals.hwndFrame,
                                  QWL_STYLE);

                                           /* now set or remove the controls */
    if (ConsoleGlobals.fHasHorzScroll != fNeedHorzScroll)
      if (fNeedHorzScroll)
      {
        flStyle |= FCF_HORZSCROLL;
        WinSetParent(ConsoleGlobals.hwndHorzScroll,        /* attach control */
                     ConsoleGlobals.hwndFrame,
                     FALSE);
      }
      else
      {
        flStyle &= ~FCF_HORZSCROLL;
        WinSetParent(ConsoleGlobals.hwndHorzScroll,        /* detach control */
                     HWND_OBJECT,
                     FALSE);
        ConsoleGlobals.coordWindowPos.X = 0;    /* we can see the whole buffer */
      }

    if (ConsoleGlobals.fHasVertScroll != fNeedVertScroll)
      if (fNeedVertScroll)
      {
        flStyle |= FCF_VERTSCROLL;
        WinSetParent(ConsoleGlobals.hwndVertScroll,        /* attach control */
                     ConsoleGlobals.hwndFrame,
                     FALSE);
      }
      else
      {
        flStyle &= ~FCF_VERTSCROLL;
        WinSetParent(ConsoleGlobals.hwndVertScroll,        /* detach control */
                     HWND_OBJECT,
                     FALSE);
        ConsoleGlobals.coordWindowPos.Y = 0;  /* we can see the whole buffer */
      }


    WinSendMsg(ConsoleGlobals.hwndFrame,                     /* update frame */
               WM_UPDATEFRAME,
               MPFROMLONG(flStyle),
               MPVOID);

    WinInvalidateRect(ConsoleGlobals.hwndFrame,       /* redraw frame window */
                      NULL,
                      TRUE);

    ConsoleGlobals.fHasVertScroll = fNeedVertScroll;       /* update globals */
    ConsoleGlobals.fHasHorzScroll = fNeedHorzScroll;       /* update globals */
  }


                                    /* setup the scrollbars and scrollranges */
  if (ConsoleGlobals.fHasVertScroll)
  {
    /* setup vertical scrollbar */
  }


  if (ConsoleGlobals.fHasHorzScroll)
  {
    /* setup horizonal scrollbar */
  }


  WinCalcFrameRect(ConsoleGlobals.hwndFrame,    /* calculate frame rectangle */
                   pRcl,
                   FALSE);

  /* @@@PH client may not overlap frame ! */
  /* @@@PH write values to TRACKINFO      */

#if 0
  /* @@@PH this results in recursion */
  WinSetWindowPos (ConsoleGlobals.hwndClient,   /* adjust client window size */
                   ConsoleGlobals.hwndFrame,
                   0,
                   0,
                   lX,
                   lY,
                   SWP_SIZE);

  WinSetWindowPos (ConsoleGlobals.hwndFrame,    /* adjust client window size */
                   HWND_DESKTOP,
                   pRcl->xLeft,
                   pRcl->yBottom,
                   pRcl->xRight,
                   pRcl->yTop,
                   SWP_SIZE);
#endif
}


/*****************************************************************************
 * Name      : BOOL WIN32API AllocConsole
 * Purpose   : The AllocConsole function allocates a new console
 *             for the calling process
 * Parameters: VOID
 * Variables :
 * Result    : BOOL: TRUE  - function succeeded
 *                   FALSE - function failed. Extended error information
 *                           obtainable via GetLastError
 * Remark    :
 * Status    : REWRITTEN UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API AllocConsole(VOID)
{
  APIRET rc;                                               /* API returncode */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2AllocConsole() called.\n");
#endif

  rc = ConsoleInit();                    /* initialize subsystem if required */
  if (rc != NO_ERROR)                                    /* check for errors */
  {
    SetLastError(rc);                            /* pass thru the error code */
    return FALSE;                                          /* signal failure */
  }
  else
    return TRUE;                                                /* Fine ! :) */
}


/*****************************************************************************
 * Name      : HANDLE WIN32API CreateConsoleScreenBuffer
 * Purpose   : The CreateConsoleScreenBuffer function creates a console
 *             screen buffer and returns a handle of it.
 * Parameters: DWORD  dwDesiredAccess    - access flag
 *             DWORD  dwShareMode        - buffer share more
 *             PVOID  pIgnored           - LPSECURITY_ATTRIBUTES -> NT
 *             DWORD  dwFlags            - type of buffer to create
 *             LPVOID lpScreenBufferData - reserved
 * Variables :
 * Result    :
 * Remark    : a console buffer is a kernel heap object equipped with
 *             share modes, access rights, etc.
 *             we can't really map this to OS/2 unless we build a
 *             console device driver for it ... maybe this turns out to
 *             be necessary since we've got to handle CONIN$ and CONOUT$, too.
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 03:55]
 *****************************************************************************/

HANDLE WIN32API CreateConsoleScreenBuffer(DWORD  dwDesiredAccess,
                                             DWORD  dwShareMode,
                                             LPVOID lpSecurityAttributes,
                                             DWORD  dwFlags,
                                             LPVOID lpScreenBufferData)
{
  HANDLE hResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:OS2CreateConsoleScreenBuffer(%08x,%08x,%08x,%08x,%08x).\n",
           dwDesiredAccess,
           dwShareMode,
           lpSecurityAttributes,
           dwFlags,
           lpScreenBufferData);
#endif

  hResult = HMCreateFile("CONBUFFER$",         /* create a new buffer handle */
                         dwDesiredAccess,
                         dwShareMode,
                         lpSecurityAttributes,
                         0,
                         dwFlags,
                         INVALID_HANDLE_VALUE);

  return hResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API FillConsoleOutputAttribute(HANDLE  hConsoleOutput,
                                            WORD    wAttribute,
                                            DWORD   nLength,
                                            COORD   dwWriteCoord,
                                            LPDWORD lpNumberOfAttrsWritten)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2FillConsoleOutputAttribute(%08x,%04x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           wAttribute,
           nLength,
           dwWriteCoord,
           lpNumberOfAttrsWritten);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_FILLCONSOLEOUTPUTATTRIBUTE,
                                  (ULONG)wAttribute,
                                  (ULONG)nLength,
                                  COORD2ULONG(dwWriteCoord),
                                  (ULONG)lpNumberOfAttrsWritten);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API FillConsoleOutputCharacterA(HANDLE  hConsoleOutput,
                                             UCHAR   cCharacter,
                                             DWORD   nLength,
                                             COORD   dwWriteCoord,
                                             LPDWORD lpNumberOfCharsWritten )
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2FillConsoleOutputCharacterA(%08x,%c,%08x,%08x,%08x).\n",
           hConsoleOutput,
           cCharacter,
           nLength,
           dwWriteCoord,
           lpNumberOfCharsWritten);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_FILLCONSOLEOUTPUTCHARACTERA,
                                  (ULONG)cCharacter,
                                  (ULONG)nLength,
                                  COORD2ULONG(dwWriteCoord),
                                  (ULONG)lpNumberOfCharsWritten);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API FillConsoleOutputCharacterW(HANDLE  hConsoleOutput,
                                             WCHAR   cCharacter,
                                             DWORD   nLength,
                                             COORD   dwWriteCoord,
                                             LPDWORD lpNumberOfCharsWritten )
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2FillConsoleOutputCharacterW(%08x,%c,%08x,%08x,%08x) .\n",
           hConsoleOutput,
           cCharacter,
           nLength,
           dwWriteCoord,
           lpNumberOfCharsWritten);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_FILLCONSOLEOUTPUTCHARACTERW,
                                  (ULONG)cCharacter,
                                  (ULONG)nLength,
                                  COORD2ULONG(dwWriteCoord),
                                  (ULONG)lpNumberOfCharsWritten);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API FlushConsoleInputBuffer( HANDLE hConsoleInput )
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2FlushConsoleInputBuffer(%08x).\n",
           hConsoleInput);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_FLUSHCONSOLEINPUTBUFFER,
                                  0,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      : BOOL WIN32API FreeConsole
 * Purpose   : The FreeConsole function detaches the calling process
 *             from its console.
 * Parameters: VOID
 * Variables :
 * Result    : BOOL: TRUE  - function succeeded
 *                   FALSE - function failed. Extended error information
 *                           obtainable via GetLastError
 * Remark    :
 * Status    : REWRITTEN UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 03:35]
 *****************************************************************************/

BOOL WIN32API FreeConsole( VOID )
{
  APIRET rc;                                               /* API returncode */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2FreeConsole() called.\n");
#endif

  rc = ConsoleTerminate();                /* terminate subsystem if required */
  if (rc != NO_ERROR)                                    /* check for errors */
  {
    SetLastError(rc);                            /* pass thru the error code */
    return FALSE;                                          /* signal failure */
  }
  else
    return TRUE;                                                /* Fine ! :) */

  return TRUE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GenerateConsoleCtrlEvent(DWORD dwCtrlEvent,
                                          DWORD dwProcessGroupId)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GenerateConsoleCtrlEvent(%08x,%08x) not implemented.\n",
           dwCtrlEvent,
           dwProcessGroupId);
#endif

  return TRUE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

UINT WIN32API GetConsoleCP(VOID)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetConsoleCP not implemented.\n");
#endif

  return 1;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetConsoleCursorInfo(HANDLE               hConsoleOutput,
                                      PCONSOLE_CURSOR_INFO lpConsoleCursorInfo)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetConsoleCursorInfo(%08x,%08x).\n",
           hConsoleOutput,
           lpConsoleCursorInfo);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_GETCONSOLECURSORINFO,
                                  (ULONG)lpConsoleCursorInfo,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetConsoleMode(HANDLE  hConsole,
                                LPDWORD lpMode)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetConsoleMode(%08x,%08x).\n",
           hConsole,
           lpMode);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsole,
                                  DRQ_GETCONSOLEMODE,
                                  (ULONG) lpMode,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

UINT WIN32API GetConsoleOutputCP(VOID)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetConsoleOutputCP not implemented.\n");
#endif

  return 1;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetConsoleScreenBufferInfo(HANDLE                      hConsoleOutput,
                                            PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetConsoleScreenBufferInfo(%08x,%08x).\n",
           hConsoleOutput,
           lpConsoleScreenBufferInfo);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_GETCONSOLESCREENBUFFERINFO,
                                  (ULONG)lpConsoleScreenBufferInfo,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      : DWORD WIN32API GetConsoleTitle
 * Purpose   : Query the current console window title
 * Parameters: LPTSTR lpConsoleTitle
 *             DWORD  nSize
 * Variables :
 * Result    : number of copied bytes
 * Remark    :
 * Status    : REWRITTEN UNTESTED
 *
 * Author    : Patrick Haller [Thu, 1998/02/12 23:31]
 *****************************************************************************/

DWORD WIN32API GetConsoleTitleA(LPTSTR lpConsoleTitle,
                                   DWORD  nSize)
{
  ULONG ulLength;                                          /* length of text */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetConsoleTitleA(%08x,%08x).\n",
           lpConsoleTitle,
           nSize);
#endif

  if (ConsoleGlobals.pszWindowTitle == NULL)    /* is there a window title ? */
    return 0;                                           /* abort immediately */

  ulLength = strlen(ConsoleGlobals.pszWindowTitle);        /* length of text */

  strncpy(lpConsoleTitle,
          ConsoleGlobals.pszWindowTitle,
          nSize);

  return (nSize < ulLength) ? nSize : ulLength;
}


/*****************************************************************************
 * Name      : DWORD WIN32API GetConsoleTitle
 * Purpose   : Query the current console window title
 * Parameters: LPTSTR lpConsoleTitle
 *             DWORD  nSize
 * Variables :
 * Result    : number of copied bytes
 * Remark    :
 * Status    : REWRITTEN UNTESTED
 *
 * Author    : Patrick Haller [Thu, 1998/02/12 23:31]
 *****************************************************************************/

DWORD WIN32API GetConsoleTitleW(LPTSTR lpConsoleTitle,
                                   DWORD  nSize)
{
  ULONG ulLength;                                          /* length of text */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetConsoleTitleW(%08x,%08x).\n",
           lpConsoleTitle,
           nSize);
#endif

  if (ConsoleGlobals.pszWindowTitle == NULL)    /* is there a window title ? */
    return 0;                                           /* abort immediately */

  ulLength = strlen(ConsoleGlobals.pszWindowTitle);        /* length of text */

  strncpy(lpConsoleTitle,
          ConsoleGlobals.pszWindowTitle,
          nSize);

  /* @@@PH Ascii2Unicode */

  return (nSize < ulLength) ? nSize : ulLength;
}


/*****************************************************************************
 * Name      : COORD WIN32API GetLargestConsoleWindowSize
 * Purpose   : Determine maximum AVIO size
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

COORD WIN32API GetLargestConsoleWindowSize(HANDLE hConsoleOutput)
{
  DWORD dwResult;
  COORD coordResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetLargestConsoleWindowSize(%08x).\n",
           hConsoleOutput);
#endif

  dwResult = HMDeviceRequest(hConsoleOutput,
                             DRQ_GETLARGESTCONSOLEWINDOWSIZE,
                             0,
                             0,
                             0,
                             0);

  ULONG2COORD(coordResult,dwResult)
  return ( coordResult );
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetNumberOfConsoleInputEvents(HANDLE  hConsoleInput,
                                               LPDWORD lpNumberOfEvents)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetNumberOfConsoleInputEvents(%08x,%08x).\n",
           hConsoleInput,
           lpNumberOfEvents);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_GETNUMBEROFCONSOLEINPUTEVENTS,
                                  (ULONG)lpNumberOfEvents,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API GetNumberOfConsoleMouseButtons(LPDWORD lpcNumberOfMouseButtons)
{
  LONG lMouseButtons;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2GetNumberOfConsoleMouseButtons(%08x).\n",
           lpcNumberOfMouseButtons);
#endif

  lMouseButtons = WinQuerySysValue(HWND_DESKTOP,        /* query PM for that */
                                   SV_CMOUSEBUTTONS);

  *lpcNumberOfMouseButtons = (DWORD)lMouseButtons;

  return TRUE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API PeekConsoleInputW(HANDLE        hConsoleInput,
                                   PINPUT_RECORD pirBuffer,
                                   DWORD         cInRecords,
                                   LPDWORD       lpcRead)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2PeekConsoleInputW(%08x,%08x,%08x,%08x).\n",
           hConsoleInput,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_PEEKCONSOLEINPUTW,
                                  (ULONG)pirBuffer,
                                  (ULONG)cInRecords,
                                  (ULONG)lpcRead,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API PeekConsoleInputA(HANDLE        hConsoleInput,
                                   PINPUT_RECORD pirBuffer,
                                   DWORD         cInRecords,
                                   LPDWORD       lpcRead)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2PeekConsoleInputA(%08x,%08x,%08x,%08x).\n",
           hConsoleInput,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_PEEKCONSOLEINPUTA,
                                  (ULONG)pirBuffer,
                                  (ULONG)cInRecords,
                                  (ULONG)lpcRead,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleA(HANDLE  hConsoleInput,
                              LPVOID  lpvBuffer,
                              DWORD   cchToRead,
                              LPDWORD lpcchRead,
                              LPVOID  lpvReserved)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleA(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleInput,
           lpvBuffer,
           cchToRead,
           lpcchRead,
           lpvReserved);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_READCONSOLEA,
                                  (ULONG)lpvBuffer,
                                  (ULONG)cchToRead,
                                  (ULONG)lpcchRead,
                                  (ULONG)lpvReserved);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleW(HANDLE  hConsoleInput,
                              LPVOID  lpvBuffer,
                              DWORD   cchToRead,
                              LPDWORD lpcchRead,
                              LPVOID  lpvReserved)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleW(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleInput,
           lpvBuffer,
           cchToRead,
           lpcchRead,
           lpvReserved);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_READCONSOLEW,
                                  (ULONG)lpvBuffer,
                                  (ULONG)cchToRead,
                                  (ULONG)lpcchRead,
                                  (ULONG)lpvReserved);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleInputA(HANDLE        hConsoleInput,
                                   PINPUT_RECORD pirBuffer,
                                   DWORD         cInRecords,
                                   LPDWORD       lpcRead)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleInputA(%08x,%08x,%08x,%08x).\n",
           hConsoleInput,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_READCONSOLEINPUTA,
                                  (ULONG)pirBuffer,
                                  (ULONG)cInRecords,
                                  (ULONG)lpcRead,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleInputW(HANDLE        hConsoleInput,
                                   PINPUT_RECORD pirBuffer,
                                   DWORD         cInRecords,
                                   LPDWORD       lpcRead)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleInputW(%08x,%08x,%08x,%08x).\n",
           hConsoleInput,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_READCONSOLEINPUTW,
                                  (ULONG)pirBuffer,
                                  (ULONG)cInRecords,
                                  (ULONG)lpcRead,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleOutputA(HANDLE      hConsoleOutput,
                                    PCHAR_INFO  pchiDestBuffer,
                                    COORD       coordDestBufferSize,
                                    COORD       coordDestBufferCoord,
                                    PSMALL_RECT psrctSourceRect)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleOutputA(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           pchiDestBuffer,
           coordDestBufferSize,
           coordDestBufferCoord,
           psrctSourceRect);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_READCONSOLEOUTPUTA,
                                  (ULONG)pchiDestBuffer,
                                  COORD2ULONG(coordDestBufferSize),
                                  COORD2ULONG(coordDestBufferCoord),
                                  (ULONG)psrctSourceRect);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleOutputW(HANDLE      hConsoleOutput,
                                    PCHAR_INFO  pchiDestBuffer,
                                    COORD       coordDestBufferSize,
                                    COORD       coordDestBufferCoord,
                                    PSMALL_RECT psrctSourceRect)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleOutputW(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           pchiDestBuffer,
           coordDestBufferSize,
           coordDestBufferCoord,
           psrctSourceRect);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_READCONSOLEOUTPUTW,
                                  (ULONG)pchiDestBuffer,
                                  COORD2ULONG(coordDestBufferSize),
                                  COORD2ULONG(coordDestBufferCoord),
                                  (ULONG)psrctSourceRect);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleOutputAttribute(HANDLE  hConsoleOutput,
                                            LPWORD  lpwAttribute,
                                            DWORD   cReadCells,
                                            COORD   coordReadCoord,
                                            LPDWORD lpcNumberRead)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleOutputAttribute(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           lpwAttribute,
           cReadCells,
           coordReadCoord,
           lpcNumberRead);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_READCONSOLEOUTPUTATTRIBUTE,
                                  (ULONG)lpwAttribute,
                                  (ULONG)cReadCells,
                                  COORD2ULONG(coordReadCoord),
                                  (ULONG)lpcNumberRead);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleOutputCharacterA(HANDLE  hConsoleOutput,
                                             LPTSTR  lpReadBuffer,
                                             DWORD   cchRead,
                                             COORD   coordReadCoord,
                                             LPDWORD lpcNumberRead)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleOutputCharacterA(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           lpReadBuffer,
           cchRead,
           coordReadCoord,
           lpcNumberRead);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_READCONSOLEOUTPUTCHARACTERA,
                                  (ULONG)lpReadBuffer,
                                  (ULONG)cchRead,
                                  COORD2ULONG(coordReadCoord),
                                  (ULONG)lpcNumberRead);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ReadConsoleOutputCharacterW(HANDLE  hConsoleOutput,
                                             LPTSTR  lpReadBuffer,
                                             DWORD   cchRead,
                                             COORD   coordReadCoord,
                                             LPDWORD lpcNumberRead)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ReadConsoleOutputCharacterW(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           lpReadBuffer,
           cchRead,
           coordReadCoord,
           lpcNumberRead);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_READCONSOLEOUTPUTCHARACTERW,
                                  (ULONG)lpReadBuffer,
                                  (ULONG)cchRead,
                                  COORD2ULONG(coordReadCoord),
                                  (ULONG)lpcNumberRead);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ScrollConsoleScreenBufferA(HANDLE      hConsoleOutput,
                                            PSMALL_RECT psrctSourceRect,
                                            PSMALL_RECT psrctClipRect,
                                            COORD       coordDestOrigin,
                                            PCHAR_INFO  pchiFill)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ScrollConsoleScreenBufferA(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           psrctSourceRect,
           psrctClipRect,
           coordDestOrigin,
           pchiFill);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_SCROLLCONSOLESCREENBUFFERA,
                                  (ULONG)psrctSourceRect,
                                  (ULONG)psrctClipRect,
                                  COORD2ULONG(coordDestOrigin),
                                  (ULONG)pchiFill);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API ScrollConsoleScreenBufferW(HANDLE      hConsoleOutput,
                                            PSMALL_RECT psrctSourceRect,
                                            PSMALL_RECT psrctClipRect,
                                            COORD       coordDestOrigin,
                                            PCHAR_INFO  pchiFill)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2ScrollConsoleScreenBufferW(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           psrctSourceRect,
           psrctClipRect,
           coordDestOrigin,
           pchiFill);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_SCROLLCONSOLESCREENBUFFERW,
                                  (ULONG)psrctSourceRect,
                                  (ULONG)psrctClipRect,
                                  COORD2ULONG(coordDestOrigin),
                                  (ULONG)pchiFill);

  return fResult;
}

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleActiveScreenBuffer(HANDLE hConsoleOutput)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleActiveScreenBuffer(%08x).\n",
           hConsoleOutput);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_SETCONSOLEACTIVESCREENBUFFER,
                                  0,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleCP(UINT IDCodePage)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleCP(%08x) not implemented.\n",
           IDCodePage);
#endif

  return TRUE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleCtrlHandler(PHANDLER_ROUTINE pHandlerRoutine,
                                       BOOL             fAdd)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleCtrlHandler(%08x,%08x) not implemented.\n",
           pHandlerRoutine,
           fAdd);
#endif

  return TRUE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleCursorInfo(HANDLE               hConsoleOutput,
                                      PCONSOLE_CURSOR_INFO lpConsoleCursorInfo)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleCursorInfo(%08x,%08x).\n",
           hConsoleOutput,
           lpConsoleCursorInfo);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_SETCONSOLECURSORINFO,
                                  (ULONG)lpConsoleCursorInfo,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]

 *****************************************************************************/

BOOL WIN32API SetConsoleCursorPosition(HANDLE hConsoleOutput,
                                          COORD  coordCursor)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleCursorPosition(%08x,%08x).\n",
           hConsoleOutput,
           coordCursor);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_SETCONSOLECURSORPOSITION,
                                  COORD2ULONG(coordCursor),
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleMode(HANDLE hConsole,
                                DWORD  fdwMode)
{
 BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleMode(%08x,%08x).\n",
           hConsole,
           fdwMode);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsole,
                                  DRQ_SETCONSOLEMODE,
                                  (ULONG)fdwMode,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleOutputCP(UINT IDCodePage)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleOutputCP(%08x) not implemented.\n",
           IDCodePage);
#endif

  return TRUE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleScreenBufferSize(HANDLE hConsoleOutput,
                                            COORD  coordSize)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleScreenBufferSize(%08x,%08x).\n",
           hConsoleOutput,
           coordSize);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_SETCONSOLESCREENBUFFERSIZE,
                                  COORD2ULONG(coordSize),
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleTextAttribute(HANDLE hConsoleOutput,
                                         WORD   wAttr)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleTextAttribute(%08x,%04x).\n",
           hConsoleOutput,
           wAttr);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_SETCONSOLETEXTATTRIBUTE,
                                  (ULONG)wAttr,
                                  0,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetConsoleTitleA
 * Purpose   : Set new title text for the console window
 * Parameters: LPTSTR lpszTitle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : REWRITTEN UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/12 23:28]
 *****************************************************************************/

BOOL WIN32API SetConsoleTitleA(LPTSTR lpszTitle)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleTitleA(%s).\n",
           lpszTitle);
#endif

  if (ConsoleGlobals.pszWindowTitle != NULL)           /* previously set name */
    free (ConsoleGlobals.pszWindowTitle);                     /* then free it */

  ConsoleGlobals.pszWindowTitle = strdup(lpszTitle);     /* copy the new name */

  WinSetWindowText(ConsoleGlobals.hwndFrame,           /* set new title text */
                   ConsoleGlobals.pszWindowTitle);

  return TRUE;
}


/*****************************************************************************
 * Name      : BOOL WIN32API SetConsoleTitleW
 * Purpose   : Set new title text for the console window
 * Parameters: LPTSTR lpszTitle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : REWRITTEN UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/12 23:28]
 *****************************************************************************/

BOOL WIN32API SetConsoleTitleW(LPTSTR lpszTitle)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleTitleW(%s) not implemented.\n",
           lpszTitle);
#endif

  /* @@@PH Unicode2Ascii */

  if (ConsoleGlobals.pszWindowTitle != NULL)           /* previously set name */
    free (ConsoleGlobals.pszWindowTitle);                     /* then free it */

  ConsoleGlobals.pszWindowTitle = strdup(lpszTitle);     /* copy the new name */

  WinSetWindowText(ConsoleGlobals.hwndFrame,           /* set new title text */
                   ConsoleGlobals.pszWindowTitle);

  return TRUE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API SetConsoleWindowInfo(HANDLE      hConsoleOutput,
                                      BOOL        fAbsolute,
                                      PSMALL_RECT psrctWindowRect)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleWindowInfo(%08x,%08x,%08x).\n",
           hConsoleOutput,
           fAbsolute,
           psrctWindowRect);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_SETCONSOLEWINDOWINFO,
                                  (ULONG)fAbsolute,
                                  (ULONG)psrctWindowRect,
                                  0,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleA(HANDLE      hConsoleOutput,
                               CONST VOID* lpvBuffer,
                               DWORD       cchToWrite,
                               LPDWORD     lpcchWritten,
                               LPVOID      lpvReserved)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleA(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           lpvBuffer,
           cchToWrite,
           lpcchWritten,
           lpvReserved);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_WRITECONSOLEA,
                                  (ULONG)lpvBuffer,
                                  (ULONG)cchToWrite,
                                  (ULONG)lpcchWritten,
                                  (ULONG)lpvReserved);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleW(HANDLE      hConsoleOutput,
                               CONST VOID* lpvBuffer,
                               DWORD       cchToWrite,
                               LPDWORD     lpcchWritten,
                               LPVOID      lpvReserved)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleW(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           lpvBuffer,
           cchToWrite,
           lpcchWritten,
           lpvReserved);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_WRITECONSOLEW,
                                  (ULONG)lpvBuffer,
                                  (ULONG)cchToWrite,
                                  (ULONG)lpcchWritten,
                                  (ULONG)lpvReserved);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleInputA(HANDLE        hConsoleInput,
                                    PINPUT_RECORD pirBuffer,
                                    DWORD         cInRecords,
                                    LPDWORD       lpcWritten)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleInputA(%08x,%08x,%08x,%08x).\n",
           hConsoleInput,
           pirBuffer,
           cInRecords,
           lpcWritten);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_WRITECONSOLEINPUTA,
                                  (ULONG)pirBuffer,
                                  (ULONG)cInRecords,
                                  (ULONG)lpcWritten,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleInputW(HANDLE        hConsoleInput,
                                    PINPUT_RECORD pirBuffer,
                                    DWORD         cInRecords,
                                    LPDWORD       lpcWritten)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleInputW(%08x,%08x,%08x,%08x).\n",
           hConsoleInput,
           pirBuffer,
           cInRecords,
           lpcWritten);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleInput,
                                  DRQ_WRITECONSOLEINPUTW,
                                  (ULONG)pirBuffer,
                                  (ULONG)cInRecords,
                                  (ULONG)lpcWritten,
                                  0);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleOutputA(HANDLE      hConsoleOutput,
                                     PCHAR_INFO  pchiSrcBuffer,
                                     COORD       coordSrcBufferSize,
                                     COORD       coordSrcBufferCoord,
                                     PSMALL_RECT psrctDestRect)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleOutputA(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           pchiSrcBuffer,
           coordSrcBufferSize,
           coordSrcBufferCoord,
           psrctDestRect);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_WRITECONSOLEOUTPUTA,
                                  (ULONG)pchiSrcBuffer,
                                  COORD2ULONG(coordSrcBufferSize),
                                  COORD2ULONG(coordSrcBufferCoord),
                                  (ULONG)psrctDestRect);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleOutputW(HANDLE      hConsoleOutput,
                                     PCHAR_INFO  pchiSrcBuffer,
                                     COORD       coordSrcBufferSize,
                                     COORD       coordSrcBufferCoord,
                                     PSMALL_RECT psrctDestRect)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleOutputW(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           pchiSrcBuffer,
           coordSrcBufferSize,
           coordSrcBufferCoord,
           psrctDestRect);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_WRITECONSOLEOUTPUTW,
                                  (ULONG)pchiSrcBuffer,
                                  COORD2ULONG(coordSrcBufferSize),
                                  COORD2ULONG(coordSrcBufferCoord),
                                  (ULONG)psrctDestRect);

  return fResult;
}

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleOutputAttribute(HANDLE  hConsoleOutput,
                                             LPWORD  lpwAttribute,
                                             DWORD   cWriteCells,
                                             COORD   coordWriteCoord,
                                             LPDWORD lpcNumberWritten)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleOutputAttribute(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           lpwAttribute,
           cWriteCells,
           coordWriteCoord,
           lpcNumberWritten);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_WRITECONSOLEOUTPUTATTRIBUTE,
                                  (ULONG)lpwAttribute,
                                  (ULONG)cWriteCells,
                                  COORD2ULONG(coordWriteCoord),
                                  (ULONG)lpcNumberWritten);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleOutputCharacterA(HANDLE  hConsoleOutput,
                                              LPTSTR  lpWriteBuffer,
                                              DWORD   cchWrite,
                                              COORD   coordWriteCoord,
                                              LPDWORD lpcWritten)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleOutputCharacterA(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           lpWriteBuffer,
           cchWrite,
           coordWriteCoord,
           lpcWritten);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_WRITECONSOLEOUTPUTCHARACTERA,
                                  (ULONG)lpWriteBuffer,
                                  (ULONG)cchWrite,
                                  COORD2ULONG(coordWriteCoord),
                                  (ULONG)lpcWritten);

  return fResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

BOOL WIN32API WriteConsoleOutputCharacterW(HANDLE  hConsoleOutput,
                                              LPTSTR  lpWriteBuffer,
                                              DWORD   cchWrite,
                                              COORD   coordWriteCoord,
                                              LPDWORD lpcWritten)
{
  BOOL fResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2WriteConsoleOutputCharacterW(%08x,%08x,%08x,%08x,%08x).\n",
           hConsoleOutput,
           lpWriteBuffer,
           cchWrite,
           coordWriteCoord,
           lpcWritten);
#endif

  fResult = (BOOL)HMDeviceRequest(hConsoleOutput,
                                  DRQ_WRITECONSOLEOUTPUTCHARACTERW,
                                  (ULONG)lpWriteBuffer,
                                  (ULONG)cchWrite,
                                  COORD2ULONG(coordWriteCoord),
                                  (ULONG)lpcWritten);

  return fResult;
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::CreateFile
 * Purpose   : this is called from the handle manager if a CreateFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    : @@@PH CONIN$ handles should be exclusive
 *                   reject other requests to this device
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::CreateFile (LPCSTR        lpFileName,
                                          PHMHANDLEDATA pHMHandleData,
                                          PVOID         lpSecurityAttributes,
                                          PHMHANDLEDATA pHMHandleDataTemplate)
{
#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleInClass::CreateFile %s(%s,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData->hHandle,
           lpSecurityAttributes,
           pHMHandleDataTemplate);
#endif

  pHMHandleData->dwType = FILE_TYPE_CHAR;        /* we're a character device */

  return(NO_ERROR);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                       LPCVOID       lpBuffer,
                                       DWORD         nNumberOfBytesToRead,
                                       LPDWORD       lpNumberOfBytesRead,
                                       LPOVERLAPPED  lpOverlapped)
{
  ULONG  ulCounter;                  /* character counter for the queue loop */
  PSZ    pszTarget;                              /* pointer to target buffer */
  APIRET rc;                                               /* API returncode */
  INPUT_RECORD InputRecord;               /* buffer for the event to be read */
  ULONG  ulPostCounter;                            /* semaphore post counter */

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleInClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped);
#endif

  ulCounter = 0;                              /* read ascii chars from queue */
  pszTarget = (PSZ)lpBuffer;

  /* @@@PH: ConsoleMode: ENABLE_LINE_INPUT - blocks until CR is read */

                                  /* block if no key events are in the queue */
  for (;ulCounter==0;)                       /* until we got some characters */
  {
    if (ConsoleInputQueryEvents() == 0)       /* if queue is currently empty */
    {
      rc = DosWaitEventSem(ConsoleInput.hevInputQueue,     /* wait for input */
                           SEM_INDEFINITE_WAIT);
      DosResetEventSem(ConsoleInput.hevInputQueue,        /* reset semaphore */
                       &ulPostCounter);            /* post counter - ignored */
    }

    do
    {
      rc = ConsoleInputEventPop(&InputRecord);       /* get event from queue */
      if (rc == NO_ERROR)         /* if we've got a valid event in the queue */
      {
        if (InputRecord.EventType == KEY_EVENT)          /* check event type */
        {
          *pszTarget = InputRecord.Event.KeyEvent.uChar.AsciiChar;
          pszTarget++;
          ulCounter++;

                                                     /* local echo enabled ? */
          if (ConsoleInput.dwConsoleMode & ENABLE_ECHO_INPUT)
            HMWriteFile(ConsoleGlobals.hConsoleBuffer,
                        &InputRecord.Event.KeyEvent.uChar.AsciiChar,
                        1,
                        &ulPostCounter,                      /* dummy result */
                        NULL);

          if (ulCounter >= nNumberOfBytesToRead)        /* at buffer's end ? */
            goto __readfile_exit;
        }
                                         /* Note: other events are discarded */
      }
    }
    while (rc == NO_ERROR);
  }

__readfile_exit:

  *lpNumberOfBytesRead = ulCounter;                          /* write result */

  return(TRUE);                                                        /* OK */
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                        LPCVOID       lpBuffer,
                                        DWORD         nNumberOfBytesToWrite,
                                        LPDWORD       lpNumberOfBytesWritten,
                                        LPOVERLAPPED  lpOverlapped)
{

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleInClass:WriteFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped);
#endif

  return(ERROR_ACCESS_DENIED);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMDeviceConsoleInClass::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                               ULONG         ulRequestCode,
                                               ULONG         arg1,
                                               ULONG         arg2,
                                               ULONG         arg3,
                                               ULONG         arg4)
{
  switch (ulRequestCode)
  {
    case DRQ_FLUSHCONSOLEINPUTBUFFER:
      return (HMDeviceConsoleInClass::
              FlushConsoleInputBuffer(pHMHandleData));

    case DRQ_GETNUMBEROFCONSOLEINPUTEVENTS:
      return (HMDeviceConsoleInClass::
              GetNumberOfConsoleInputEvents(pHMHandleData,
                                            (LPDWORD)arg1));

    case DRQ_PEEKCONSOLEINPUTA:
      return (HMDeviceConsoleInClass::
              PeekConsoleInputA(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)        arg2,
                                (LPDWORD)      arg3));

    case DRQ_PEEKCONSOLEINPUTW:
      return (HMDeviceConsoleInClass::
              PeekConsoleInputW(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)        arg2,
                                (LPDWORD)      arg3));


    case DRQ_READCONSOLEA:
      return (HMDeviceConsoleInClass::
              ReadConsoleA(pHMHandleData,
                           (CONST VOID*) arg1,
                           (DWORD)       arg2,
                           (LPDWORD)     arg3,
                           (LPVOID)      arg4));

    case DRQ_READCONSOLEW:
      return (HMDeviceConsoleInClass::
              ReadConsoleW(pHMHandleData,
                           (CONST VOID*) arg1,
                           (DWORD)       arg2,
                           (LPDWORD)     arg3,
                           (LPVOID)      arg4));

    case DRQ_READCONSOLEINPUTA:
      return (HMDeviceConsoleInClass::
              ReadConsoleInputA(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)arg2,
                                (LPDWORD)arg3));

    case DRQ_READCONSOLEINPUTW:
      return (HMDeviceConsoleInClass::
              ReadConsoleInputW(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)arg2,
                                (LPDWORD)arg3));

    case DRQ_WRITECONSOLEINPUTA:
      return (HMDeviceConsoleInClass::
              WriteConsoleInputA(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)arg2,
                                (LPDWORD)arg3));

    case DRQ_WRITECONSOLEINPUTW:
      return (HMDeviceConsoleInClass::
              WriteConsoleInputW(pHMHandleData,
                                (PINPUT_RECORD)arg1,
                                (DWORD)arg2,
                                (LPDWORD)arg3));

  }

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleInClass:_DeviceRequest %s(%08x,%08x,%08x,%08x,%08x,%08x) unknown request\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           ulRequestCode,
           arg1,
           arg2,
           arg3,
           arg4);
#endif

  SetLastError(ERROR_INVALID_FUNCTION);           /* request not implemented */
  return(FALSE);                 /* we assume this indicates API call failed */
}


/*****************************************************************************
 * Name      : BOOL HMDeviceConsoleInClass::FlushConsoleInputBuffer
 * Purpose   : flushes all events from the input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

BOOL HMDeviceConsoleInClass::FlushConsoleInputBuffer(PHMHANDLEDATA pHMHandleData)
{
  ULONG ulCounter;                                           /* loop counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::FlushConsoleInputBuffer(%08x).\n",
           pHMHandleData);
#endif

  ConsoleInput.ulIndexFree  = 0;
  ConsoleInput.ulIndexEvent = 0;
  ConsoleInput.ulEvents     = 0;

  for (ulCounter = 0;
       ulCounter < CONSOLE_INPUTQUEUESIZE;
       ulCounter++)
    ConsoleInput.arrInputRecord[ulCounter].EventType = 0x0000; /* free event */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::GetConsoleMode
 * Purpose   : queries the current console mode
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPDWORD lpMode
 * Variables :
 * Result    :

 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::GetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                             LPDWORD       lpMode)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::GetConsoleMode(%08x,%08x).\n",
           pHMHandleData,
           lpMode);
#endif

  *lpMode = ConsoleInput.dwConsoleMode;       /* return current console mode */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::GetNumberOfConsoleInputEvents
 * Purpose   : queries the current number of events in the input queue
 * Parameters: PHMHANDLEDATA pHMHandleData    - handle specific data
 *             LPDWORD       lpNumberOfEvents - return number of events
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

BOOL HMDeviceConsoleInClass::GetNumberOfConsoleInputEvents(PHMHANDLEDATA pHMHandleData,
                                                           LPDWORD       lpNumberOfEvents)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::GetNumberOfConsoleInputEvents(%08x,%08x).\n",
           pHMHandleData,
           lpNumberOfEvents);
#endif

  *lpNumberOfEvents = ConsoleInput.ulEvents;      /* return number of events */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::PeekConsoleInputA
 * Purpose   : peeks events placed in the console input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - current handle data
 *             PINPUT_RECORD pirBuffer     - target buffer for events
 *             DWORD         cInRecords    - number of input records
 *             LPDWORD       lpcRead       - returns number of events stored
 * Variables :
 * Result    : TRUE if successful, FALSE otherwise
 * Remark    : if queue is completely filled and no event is free,
 *             loop will scan over queue multiple times, until target
 *             buffer is filled. It does not check ulCounter to stop
 *             when one scan of the queue is complete.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::PeekConsoleInputA(PHMHANDLEDATA pHMHandleData,
                                                PINPUT_RECORD pirBuffer,
                                                DWORD         cInRecords,
                                                LPDWORD       lpcRead)
{
  ULONG         ulCounter;                                   /* loop counter */
  ULONG         ulCurrentEvent;       /* index of current event in the queue */
  PINPUT_RECORD pirEvent;                /* pointer to current queue element */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: HMDeviceConsoleInClass::PeekConsoleInputA(%08x,%08x,%08x,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  if (ConsoleInputQueryEvents() == 0)         /* if queue is currently empty */
  {
    *lpcRead = 0;                               /* no events read from queue */
    return (TRUE);                                         /* OK, we're done */
  }


  for (ulCounter = 0,
       ulCurrentEvent = ConsoleInput.ulIndexEvent,
       pirEvent = &ConsoleInput.arrInputRecord[ConsoleInput.ulIndexEvent];

       ulCounter < cInRecords;

       ulCounter++,
       ulCurrentEvent++,
       pirEvent++,
       pirBuffer++)
  {
    if (ulCurrentEvent > CONSOLE_INPUTQUEUESIZE) /* reaching after end of que*/
    {
      ulCurrentEvent = 0;         /* then start over from beginning of queue */
      pirEvent       = ConsoleInput.arrInputRecord;
    }

    if (pirEvent->EventType == 0x0000)                   /* no more events ? */
      break;                                              /* leave loop then */

    memcpy(pirEvent,                                      /* copy event data */
           pirBuffer,
           sizeof(INPUT_RECORD));
  }

  *lpcRead = ulCounter;                      /* return number of events read */
  return (TRUE);                                           /* OK, we're done */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::PeekConsoleInputW
 * Purpose   : peeks events placed in the console input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - current handle data
 *             PINPUT_RECORD pirBuffer     - target buffer for events
 *             DWORD         cInRecords    - number of input records
 *             LPDWORD       lpcRead       - returns number of events stored
 * Variables :
 * Result    : TRUE if successful, FALSE otherwise
 * Remark    : if queue is completely filled and no event is free,
 *             loop will scan over queue multiple times, until target
 *             buffer is filled. It does not check ulCounter to stop
 *             when one scan of the queue is complete.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::PeekConsoleInputW(PHMHANDLEDATA pHMHandleData,
                                                PINPUT_RECORD pirBuffer,
                                                DWORD         cInRecords,
                                                LPDWORD       lpcRead)
{
  ULONG         ulCounter;                                   /* loop counter */
  ULONG         ulCurrentEvent;       /* index of current event in the queue */
  PINPUT_RECORD pirEvent;                /* pointer to current queue element */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: HMDeviceConsoleInClass::PeekConsoleInputW(%08x,%08x,%08x,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  if (ConsoleInputQueryEvents() == 0)         /* if queue is currently empty */
  {
    *lpcRead = 0;                               /* no events read from queue */
    return (TRUE);                                         /* OK, we're done */
  }


  for (ulCounter = 0,
       ulCurrentEvent = ConsoleInput.ulIndexEvent,
       pirEvent = &ConsoleInput.arrInputRecord[ConsoleInput.ulIndexEvent];

       ulCounter < cInRecords;

       ulCounter++,
       ulCurrentEvent++,
       pirEvent++,
       pirBuffer++)
  {
    if (ulCurrentEvent > CONSOLE_INPUTQUEUESIZE) /* reaching after end of que*/
    {
      ulCurrentEvent = 0;         /* then start over from beginning of queue */
      pirEvent       = ConsoleInput.arrInputRecord;
    }

    if (pirEvent->EventType == 0x0000)                   /* no more events ? */
      break;                                              /* leave loop then */

    memcpy(pirEvent,                                      /* copy event data */
           pirBuffer,
           sizeof(INPUT_RECORD));
  }

  *lpcRead = ulCounter;                      /* return number of events read */
  return (TRUE);                                           /* OK, we're done */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::ReadConsoleA
 * Purpose   : read a string from the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cWriteCells
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadConsoleA(PHMHANDLEDATA pHMHandleData,
                                           CONST VOID*   lpvBuffer,
                                           DWORD         cchToRead,
                                           LPDWORD       lpcchRead,
                                           LPVOID        lpvReserved)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::ReadConsoleA(%08x,%08x,%u,%08x,%08x).\n",
           pHMHandleData,
           lpvBuffer,
           cchToRead,
           lpcchRead,
           lpvReserved);
#endif

                               /* simply forward the request to that routine */
  return (HMDeviceConsoleInClass::ReadFile(pHMHandleData,
                                           lpvBuffer,
                                           cchToRead,
                                           lpcchRead,
                                           NULL));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::ReadConsoleW
 * Purpose   : write a string to the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cWriteCells
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadConsoleW(PHMHANDLEDATA pHMHandleData,
                                           CONST VOID*   lpvBuffer,
                                           DWORD         cchToRead,
                                           LPDWORD       lpcchRead,
                                           LPVOID        lpvReserved)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  DWORD          dwResult;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::ReadConsoleW(%08x,%08x,%u,%08x,%08x).\n",
           pHMHandleData,
           lpvBuffer,
           cchToRead,
           lpcchRead,
           lpvReserved);
#endif

                               /* simply forward the request to that routine */
  dwResult = HMDeviceConsoleInClass::ReadFile(pHMHandleData,
                                              lpvBuffer,
                                              cchToRead,
                                              lpcchRead,
                                              NULL);
  /* @@@PH AScii -> unicode translation */

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::ReadConsoleInputA
 * Purpose   : read events placed in the console input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - current handle data
 *             PINPUT_RECORD pirBuffer     - target buffer for events
 *             DWORD         cInRecords    - number of input records
 *             LPDWORD       lpcRead       - returns number of events stored
 * Variables :
 * Result    : TRUE if successful, FALSE otherwise
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadConsoleInputA(PHMHANDLEDATA pHMHandleData,
                                                PINPUT_RECORD pirBuffer,
                                                DWORD         cInRecords,
                                                LPDWORD       lpcRead)
{
  ULONG  ulPostCounter;                  /* semaphore post counter - ignored */
  APIRET rc;                                               /* API returncode */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: HMDeviceConsoleInClass::ReadConsoleInputA(%08x,%08x,%08x,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  if (ConsoleInputQueryEvents() == 0)         /* if queue is currently empty */
  {
    rc = DosWaitEventSem(ConsoleInput.hevInputQueue,       /* wait for input */
                         SEM_INDEFINITE_WAIT);
    DosResetEventSem(ConsoleInput.hevInputQueue,          /* reset semaphore */
                     &ulPostCounter);              /* post counter - ignored */
  }


  /* now read events into target buffer */
  for (ulPostCounter = 0;
       ulPostCounter < cInRecords;
       ulPostCounter++,
       pirBuffer++)
  {
    rc = ConsoleInputEventPop(pirBuffer);            /* get event from queue */
    if (rc != NO_ERROR)                  /* if read error occurs, break look */
      break;
  }

  *lpcRead = ulPostCounter;                 /* return number of records read */
  return (TRUE);                                                       /* OK */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::ReadConsoleInputW
 * Purpose   : read events placed in the console input queue
 * Parameters: PHMHANDLEDATA pHMHandleData - current handle data
 *             PINPUT_RECORD pirBuffer     - target buffer for events
 *             DWORD         cInRecords    - number of input records
 *             LPDWORD       lpcRead       - returns number of events stored
 * Variables :
 * Result    : TRUE if successful, FALSE otherwise
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::ReadConsoleInputW(PHMHANDLEDATA pHMHandleData,
                                                PINPUT_RECORD pirBuffer,
                                                DWORD         cInRecords,
                                                LPDWORD       lpcRead)
{
  ULONG ulPostCounter;                   /* semaphore post counter - ignored */
  APIRET rc;                                               /* API returncode */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: HMDeviceConsoleInClass::ReadConsoleInputW(%08x,%08x,%08x,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcRead);
#endif

  if (ConsoleInputQueryEvents() == 0)         /* if queue is currently empty */
  {
    rc = DosWaitEventSem(ConsoleInput.hevInputQueue,       /* wait for input */
                         SEM_INDEFINITE_WAIT);
    DosResetEventSem(ConsoleInput.hevInputQueue,          /* reset semaphore */
                     &ulPostCounter);              /* post counter - ignored */
  }


  /* now read events into target buffer */
  for (ulPostCounter = 0;
       ulPostCounter < cInRecords;
       ulPostCounter++,
       pirBuffer++)
  {
    rc = ConsoleInputEventPop(pirBuffer);            /* get event from queue */
    if (rc != NO_ERROR)                  /* if read error occurs, break look */
      break;
  }

  *lpcRead = ulPostCounter;                 /* return number of records read */
  return (TRUE);                                                       /* OK */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::SetConsoleMode
 * Purpose   : sets the current console mode
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             DWORD         dwMode        - console mode
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::SetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                             DWORD         dwMode)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::SetConsoleMode(%08x,%08x).\n",
           pHMHandleData,
           dwMode);
#endif

  ConsoleInput.dwConsoleMode = dwMode;           /* set current console mode */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::WriteConsoleInputA
 * Purpose   : this writes event records directly into the queue
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PINPUT_RECORD pirBuffer
 *             DWORD         cInRecords
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::WriteConsoleInputA (PHMHANDLEDATA pHMHandleData,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  APIRET         rc;                                       /* API returncode */
  ULONG          ulCounter;                                  /* loop counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::WriteConsoleInputA(%08x,%08x,%u,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcWritten);
#endif

  for (ulCounter = 0;
       ulCounter < cInRecords;
       ulCounter++,
       pirBuffer++)
  {
    rc = ConsoleInputEventPush(pirBuffer);             /* push current event */
    if (rc != NO_ERROR)                     /* oops ? queue full ? problem ? */
      break;
  }

  *lpcWritten = ulCounter;                /* return number of events written */
  return (TRUE);                                                       /* OK */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleInClass::WriteConsoleInputW
 * Purpose   : this writes event records directly into the queue
 * Parameters: PHMHANDLEDATA pHMHandleData
 *             PINPUT_RECORD pirBuffer
 *             DWORD         cInRecords
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleInClass::WriteConsoleInputW (PHMHANDLEDATA pHMHandleData,
                                                  PINPUT_RECORD pirBuffer,
                                                  DWORD         cInRecords,
                                                  LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  APIRET         rc;                                       /* API returncode */
  ULONG          ulCounter;                                  /* loop counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONIN$::WriteConsoleInputW(%08x,%08x,%u,%08x).\n",
           pHMHandleData,
           pirBuffer,
           cInRecords,
           lpcWritten);
#endif

  for (ulCounter = 0;
       ulCounter < cInRecords;
       ulCounter++,
       pirBuffer++)
  {
    rc = ConsoleInputEventPush(pirBuffer);             /* push current event */
    if (rc != NO_ERROR)                     /* oops ? queue full ? problem ? */
      break;
  }

  *lpcWritten = ulCounter;                /* return number of events written */
  return (TRUE);                                                       /* OK */
}



/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleOutClass::CreateFile
 * Purpose   : this is called from the handle manager if a CreateFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleOutClass::CreateFile (LPCSTR        lpFileName,
                                           PHMHANDLEDATA pHMHandleData,
                                           PVOID         lpSecurityAttributes,
                                           PHMHANDLEDATA pHMHandleDataTemplate)
{
  APIRET rc;
  BOOL   fResult;
  HANDLE hConsole;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleOutClass %s(%s,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData->hHandle,
           lpSecurityAttributes,
           pHMHandleDataTemplate);
#endif

  pHMHandleData->dwType = FILE_TYPE_CHAR;        /* we're a character device */


                 /* if no default buffer is available, then do default setup */
  if (ConsoleGlobals.hConsoleBuffer == INVALID_HANDLE_VALUE)
  {
                /* now we need a default screen buffer with the default size */
    hConsole = CreateConsoleScreenBuffer(0,
                                            0,
                                            NULL,
                                            CONSOLE_TEXTMODE_BUFFER,
                                            NULL);
    if (hConsole == INVALID_HANDLE_VALUE)
    {
#ifdef DEBUG_LOCAL
      WriteLog("KERNEL32/CONSOLE:OS2CreateConsoleScreenBuffer = %u.\n",
               GetLastError());
#endif
      return INVALID_HANDLE_VALUE;   /* abort further processing immediately */
    }

    fResult = SetConsoleTextAttribute(hConsole,
                                         ConsoleGlobals.Options.ucDefaultAttribute);
#ifdef DEBUG_LOCAL
    if (fResult == FALSE)                                    /* check errors */
      WriteLog("KERNEL32/CONSOLE:OS2SetConsoleTextAttribute=%u.\n",
               GetLastError());
#endif

    fResult = SetConsoleScreenBufferSize(hConsole,
                                            ConsoleGlobals.Options.coordDefaultSize);
    if (fResult == FALSE)
    {
#ifdef DEBUG_LOCAL
      WriteLog("KERNEL32/CONSOLE:OS2SetConsoleScreenBufferSize=%u.\n",
               GetLastError());
#endif
      HMCloseHandle(hConsole);                          /* free handle again */
      return (INVALID_HANDLE_VALUE);            /* abort further processing */
    }

    fResult = SetConsoleActiveScreenBuffer(hConsole);
    if (fResult == FALSE)
    {
#ifdef DEBUG_LOCAL
      WriteLog("KERNEL32/CONSOLE:OS2SetConsoleActiveScreenBuffer=%u.\n",
               GetLastError());
#endif
      HMCloseHandle(hConsole);                          /* free handle again */
      return (INVALID_HANDLE_VALUE);            /* abort further processing */
    }
    else
    {
      ConsoleGlobals.hConsoleBufferDefault = hConsole;        /* save handle */
      ConsoleGlobals.hConsoleBuffer        = hConsole;
    }
  }

  return(NO_ERROR);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleOutClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                        LPCVOID       lpBuffer,
                                        DWORD         nNumberOfBytesToRead,
                                        LPDWORD       lpNumberOfBytesRead,
                                        LPOVERLAPPED  lpOverlapped)
{

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleOutClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped);
#endif

  return(ERROR_ACCESS_DENIED);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleOutClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                         LPCVOID       lpBuffer,
                                         DWORD         nNumberOfBytesToWrite,
                                         LPDWORD       lpNumberOfBytesWritten,
                                         LPOVERLAPPED  lpOverlapped)
{
  DWORD dwResult;                        /* result from subsequent WriteFile */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleOutClass:WriteFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped);
#endif

        /* just prevent an endless loop, although this condition might never */
                                                                /* be true ! */
  if (pHMHandleData->hHandle != ConsoleGlobals.hConsoleBuffer)
  {
#if 0
    HMDeviceRequest(ConsoleGlobals.hConsoleBuffer,        /* hide the cursor */
                    DRQ_INTERNAL_CONSOLECURSORSHOW,
                    CONSOLECURSOR_HIDE,
                    0,
                    0,
                    0);
#endif

    dwResult = HMWriteFile(ConsoleGlobals.hConsoleBuffer,
                           lpBuffer,
                           nNumberOfBytesToWrite,
                           lpNumberOfBytesWritten,
                           lpOverlapped);

#if 0
    HMDeviceRequest(ConsoleGlobals.hConsoleBuffer,        /* show the cursor */
                    DRQ_INTERNAL_CONSOLECURSORSHOW,
                    CONSOLECURSOR_SHOW,
                    0,
                    0,
                    0);
#endif

    return (dwResult);                                 /* return result code */
  }
  else
    return (ERROR_SYS_INTERNAL);                    /* raise error condition */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleOutClass::_DeviceRequest
 * Purpose   : we just forward those device requests to the console buffer
 *             currently associated with the console itself.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/03/35 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleOutClass::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                               ULONG         ulRequestCode,
                                               ULONG         arg1,
                                               ULONG         arg2,
                                               ULONG         arg3,
                                               ULONG         arg4)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleOutClass:_DeviceRequest %s(%08x,%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           ulRequestCode,
           arg1,
           arg2,
           arg3,
           arg4);
#endif
        /* just prevent an endless loop, although this condition might never */
                                                                /* be true ! */
  if (pHMHandleData->hHandle != ConsoleGlobals.hConsoleBuffer)
    return (HMDeviceRequest(ConsoleGlobals.hConsoleBuffer,
                            ulRequestCode,
                            arg1,
                            arg2,
                            arg3,
                            arg4));
  else
    return (ERROR_SYS_INTERNAL);                    /* raise error condition */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::CreateFile
 * Purpose   : this is called from the handle manager if a CreateFile() is
 *             performed on a handle
 * Parameters: LPCSTR        lpFileName            name of the file / device
 *             PHMHANDLEDATA pHMHandleData         data of the NEW handle
 *             PVOID         lpSecurityAttributes  ignored
 *             PHMHANDLEDATA pHMHandleDataTemplate data of the template handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NO_ERROR - API succeeded
 *             other    - what is to be set in SetLastError
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::CreateFile (LPCSTR        lpFileName,
                                              PHMHANDLEDATA pHMHandleData,
                                              PVOID         lpSecurityAttributes,
                                              PHMHANDLEDATA pHMHandleDataTemplate)
{
  PCONSOLEBUFFER pConsoleBuffer;                 /* console buffer structure */

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass %s(%s,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           lpFileName,
           pHMHandleData->hHandle,
           lpSecurityAttributes,
           pHMHandleDataTemplate);
#endif

  pHMHandleData->dwType = FILE_TYPE_CHAR;        /* we're a character device */

  pHMHandleData->lpHandlerData = malloc ( sizeof(CONSOLEBUFFER) );

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:CheckPoint1: %s pHMHandleData=%08xh, lpHandlerData=%08xh\n",
           lpFileName,
           pHMHandleData,
           pHMHandleData->lpHandlerData);
#endif


  if (pHMHandleData->lpHandlerData == NULL)              /* check allocation */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);          /* set error information */
    return (INVALID_HANDLE_VALUE);                  /* raise error condition */
  }
  else
  {
    pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

    memset(pHMHandleData->lpHandlerData,             /* initialize structure */
           0,
           sizeof (CONSOLEBUFFER) );

                                                      /* set buffer defaults */
    pConsoleBuffer->dwConsoleMode = ENABLE_PROCESSED_OUTPUT |
                                    ENABLE_WRAP_AT_EOL_OUTPUT;

    pConsoleBuffer->CursorInfo.dwSize   = 2;                  /* 2 scanlines */
    pConsoleBuffer->CursorInfo.bVisible = TRUE;
  }

  return(NO_ERROR);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::CloseHandle(PHMHANDLEDATA pHMHandleData)
{

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass::CloseHandle %s(%08x)\n",
           lpHMDeviceName,
           pHMHandleData);
#endif

  if (pHMHandleData->lpHandlerData != NULL)                 /* check pointer */
  {
    PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;


    if (pConsoleBuffer->ppszLine != NULL)        /* free line buffer array ! */
      free (pConsoleBuffer->ppszLine);

    free (pHMHandleData->lpHandlerData);          /* free device object data */
    pHMHandleData->lpHandlerData = NULL;
  }

  return(NO_ERROR);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadFile(PHMHANDLEDATA pHMHandleData,
                                           LPCVOID       lpBuffer,
                                           DWORD         nNumberOfBytesToRead,
                                           LPDWORD       lpNumberOfBytesRead,
                                           LPOVERLAPPED  lpOverlapped)
{

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass::ReadFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           lpBuffer,
           nNumberOfBytesToRead,
           lpNumberOfBytesRead,
           lpOverlapped);
#endif

  return(ERROR_ACCESS_DENIED);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteFile(PHMHANDLEDATA pHMHandleData,
                                            LPCVOID       lpBuffer,
                                            DWORD         nNumberOfBytesToWrite,
                                            LPDWORD       lpNumberOfBytesWritten,
                                            LPOVERLAPPED  lpOverlapped)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
           ULONG ulCounter;                 /* counter for the byte transfer */
           PSZ   pszBuffer = (PSZ)lpBuffer;
  register UCHAR ucChar;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass:WriteFile %s(%08x,%08x,%08x,%08x,%08x)\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpNumberOfBytesWritten,
           lpOverlapped);
#endif

                      /* check if we're called with non-existing line buffer */
  if (pConsoleBuffer->ppszLine == NULL)
    return (ERROR_SYS_INTERNAL);

  for (ulCounter = 0;
       ulCounter < nNumberOfBytesToWrite;
       ulCounter++)
  {
    ucChar = pszBuffer[ulCounter];                        /* map to register */

    if ( (pConsoleBuffer->dwConsoleMode & ENABLE_PROCESSED_OUTPUT) &&
         (ucChar < 32) )     /* this is faster than a large switch statement */
    {
      switch (ucChar)
      {
        case 7: /* BEL */
          if (ConsoleGlobals.Options.fSpeakerEnabled == TRUE)
            DosBeep(ConsoleGlobals.Options.ulSpeakerFrequency,
                    ConsoleGlobals.Options.ulSpeakerDuration);
          break;

        case 8: /* Backspace */
          if (pConsoleBuffer->coordCursorPosition.X > 0)
            pConsoleBuffer->coordCursorPosition.X--;
          break;

        case 9: /* Tab */
          pConsoleBuffer->coordCursorPosition.X =
            (pConsoleBuffer->coordCursorPosition.X
             / ConsoleGlobals.Options.ulTabSize
             + 1)
            * ConsoleGlobals.Options.ulTabSize;

          if (pConsoleBuffer->coordCursorPosition.X >=
              pConsoleBuffer->coordBufferSize.X)
          {
            pConsoleBuffer->coordCursorPosition.X = 0;
            pConsoleBuffer->coordCursorPosition.Y++;

            if (pConsoleBuffer->coordCursorPosition.Y >=
                pConsoleBuffer->coordBufferSize.Y)
            {
              if (pConsoleBuffer->dwConsoleMode & ENABLE_WRAP_AT_EOL_OUTPUT)
              {
                ConsoleBufferScrollUp(pConsoleBuffer,    /* scroll one line up */
                                      1);
                pConsoleBuffer->coordCursorPosition.Y--;
              }
            }
          }
          break;

        case 10: /* LINEFEED */
          pConsoleBuffer->coordCursorPosition.Y++;

          if (pConsoleBuffer->coordCursorPosition.Y >=
              pConsoleBuffer->coordBufferSize.Y)
          {
            ConsoleBufferScrollUp(pConsoleBuffer,      /* scroll one line up */
                                  1);
            pConsoleBuffer->coordCursorPosition.Y--;
          }
          break;

        case 13: /* CARRIAGE RETURN */
          pConsoleBuffer->coordCursorPosition.X = 0;
          break;

        default:
          break;
      }
    }
    else
    {
                                                          /* write character */
      *(pConsoleBuffer->ppszLine[pConsoleBuffer->coordCursorPosition.Y] +
        pConsoleBuffer->coordCursorPosition.X * 2) = pszBuffer[ulCounter];

      pConsoleBuffer->coordCursorPosition.X++;

      if (pConsoleBuffer->coordCursorPosition.X >=
          pConsoleBuffer->coordBufferSize.X)
      {
        pConsoleBuffer->coordCursorPosition.X = 0;
        pConsoleBuffer->coordCursorPosition.Y++;

        if (pConsoleBuffer->coordCursorPosition.Y >=
            pConsoleBuffer->coordBufferSize.Y)
        {
          if (pConsoleBuffer->dwConsoleMode & ENABLE_WRAP_AT_EOL_OUTPUT)
          {
            ConsoleBufferScrollUp(pConsoleBuffer,      /* scroll one line up */
                                  1);
            pConsoleBuffer->coordCursorPosition.Y--;
          }
          else
          {
                                              /* just stay on last character */
            pConsoleBuffer->coordCursorPosition.X = pConsoleBuffer->coordBufferSize.X - 1;
            pConsoleBuffer->coordCursorPosition.Y = pConsoleBuffer->coordBufferSize.Y - 1;
          }
        }
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  *lpNumberOfBytesWritten = ulCounter;

  return(ulCounter);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMDeviceConsoleBufferClass::_DeviceRequest (PHMHANDLEDATA pHMHandleData,
                                                   ULONG         ulRequestCode,
                                                   ULONG         arg1,
                                                   ULONG         arg2,
                                                   ULONG         arg3,
                                                   ULONG         arg4)
{
  switch (ulRequestCode)
  {
    case DRQ_FILLCONSOLEOUTPUTATTRIBUTE:
    {
      COORD coordWrite;

      ULONG2COORD(coordWrite,arg3);

      return (HMDeviceConsoleBufferClass
              ::FillConsoleOutputAttribute(pHMHandleData,
                                           (WORD)arg1,
                                           (DWORD)arg2,
                                           coordWrite,
                                           (LPDWORD)arg4));
    }


    case DRQ_FILLCONSOLEOUTPUTCHARACTERA:
    {
      COORD coordWrite;

      ULONG2COORD(coordWrite,arg3);

      return (HMDeviceConsoleBufferClass
              ::FillConsoleOutputCharacterA(pHMHandleData,
                                            (UCHAR)arg1,
                                            (DWORD)arg2,
                                            coordWrite,
                                            (LPDWORD)arg4));
    }


    case DRQ_FILLCONSOLEOUTPUTCHARACTERW:
    {
      COORD coordWrite;

      ULONG2COORD(coordWrite,arg3);

      return (HMDeviceConsoleBufferClass
              ::FillConsoleOutputCharacterW(pHMHandleData,
                                            (WCHAR)arg1,
                                            (DWORD)arg2,
                                            coordWrite,
                                            (LPDWORD)arg4));
    }


    case DRQ_GETCONSOLECURSORINFO:
      return (HMDeviceConsoleBufferClass
              ::GetConsoleCursorInfo(pHMHandleData,
                                     (PCONSOLE_CURSOR_INFO)arg1));


    case DRQ_GETCONSOLEMODE:
      return (HMDeviceConsoleBufferClass
              ::GetConsoleMode(pHMHandleData,
                               (LPDWORD)arg1));


    case DRQ_GETCONSOLESCREENBUFFERINFO:
      return (HMDeviceConsoleBufferClass
              ::GetConsoleScreenBufferInfo(pHMHandleData,
                                           (PCONSOLE_SCREEN_BUFFER_INFO)arg1));


    case DRQ_GETLARGESTCONSOLEWINDOWSIZE:
      return (HMDeviceConsoleBufferClass
              ::GetLargestConsoleWindowSize(pHMHandleData));


    case DRQ_READCONSOLEOUTPUTA:
    {
      COORD coordDestBufferSize;
      COORD coordDestBufferCoord;

      ULONG2COORD(coordDestBufferSize,  arg2);
      ULONG2COORD(coordDestBufferCoord, arg3);

      return (HMDeviceConsoleBufferClass
              ::ReadConsoleOutputA(pHMHandleData,
                                   (PCHAR_INFO)arg1,
                                   coordDestBufferSize,
                                   coordDestBufferCoord,
                                   (PSMALL_RECT)arg4));
    }


    case DRQ_READCONSOLEOUTPUTW:
    {
      COORD coordDestBufferSize;
      COORD coordDestBufferCoord;

      ULONG2COORD(coordDestBufferSize,  arg2);
      ULONG2COORD(coordDestBufferCoord, arg3);

      return (HMDeviceConsoleBufferClass
              ::ReadConsoleOutputW(pHMHandleData,
                                   (PCHAR_INFO)arg1,
                                   coordDestBufferSize,
                                   coordDestBufferCoord,
                                   (PSMALL_RECT)arg4));
    }


    case DRQ_READCONSOLEOUTPUTATTRIBUTE:
    {
      COORD coordReadCoord;

      ULONG2COORD(coordReadCoord, arg3);

      return (HMDeviceConsoleBufferClass
              ::ReadConsoleOutputAttribute(pHMHandleData,
                                   (LPWORD)arg1,
                                   (DWORD)arg2,
                                   coordReadCoord,
                                   (LPDWORD)arg4));
    }


    case DRQ_READCONSOLEOUTPUTCHARACTERA:
    {
      COORD coordReadCoord;

      ULONG2COORD(coordReadCoord, arg3);

      return (HMDeviceConsoleBufferClass
              ::ReadConsoleOutputCharacterA(pHMHandleData,
                                            (LPTSTR)arg1,
                                            (DWORD)arg2,
                                            coordReadCoord,
                                            (LPDWORD)arg4));
    }


    case DRQ_READCONSOLEOUTPUTCHARACTERW:
    {
      COORD coordReadCoord;

      ULONG2COORD(coordReadCoord, arg3);

      return (HMDeviceConsoleBufferClass
              ::ReadConsoleOutputCharacterW(pHMHandleData,
                                            (LPWSTR)arg1,
                                            (DWORD)arg2,
                                            coordReadCoord,
                                            (LPDWORD)arg4));
    }


    case DRQ_SCROLLCONSOLESCREENBUFFERA:
    {
      COORD coordDestOrigin;

      ULONG2COORD(coordDestOrigin, arg3);

      return (HMDeviceConsoleBufferClass
              ::ScrollConsoleScreenBufferA(pHMHandleData,
                                           (PSMALL_RECT)arg1,
                                           (PSMALL_RECT)arg2,
                                           coordDestOrigin,
                                           (PCHAR_INFO)arg4));
    }


    case DRQ_SCROLLCONSOLESCREENBUFFERW:
    {
      COORD coordDestOrigin;

      ULONG2COORD(coordDestOrigin, arg3);

      return (HMDeviceConsoleBufferClass
              ::ScrollConsoleScreenBufferW(pHMHandleData,
                                           (PSMALL_RECT)arg1,
                                           (PSMALL_RECT)arg2,
                                           coordDestOrigin,
                                           (PCHAR_INFO)arg4));
    }


    case DRQ_SETCONSOLEACTIVESCREENBUFFER:
      return (HMDeviceConsoleBufferClass
              ::SetConsoleActiveScreenBuffer(pHMHandleData));


    case DRQ_SETCONSOLECURSORINFO:
      return (HMDeviceConsoleBufferClass
              ::SetConsoleCursorInfo(pHMHandleData,
                                     (PCONSOLE_CURSOR_INFO)arg1));


    case DRQ_SETCONSOLECURSORPOSITION:
    {
      COORD coordCursor;

      ULONG2COORD(coordCursor, arg1);

      return (HMDeviceConsoleBufferClass
              ::SetConsoleCursorPosition(pHMHandleData,
                                         coordCursor));
    }


    case DRQ_SETCONSOLEMODE:
      return (HMDeviceConsoleBufferClass
              ::SetConsoleMode(pHMHandleData,
                               (DWORD)arg1));


    case DRQ_SETCONSOLESCREENBUFFERSIZE:
    {
      COORD coordSize;

      ULONG2COORD(coordSize,arg1);

      return (HMDeviceConsoleBufferClass::
                SetConsoleScreenBufferSize(pHMHandleData,
                                           coordSize));
    }


    case DRQ_SETCONSOLETEXTATTRIBUTE:
      return (HMDeviceConsoleBufferClass::
                SetConsoleTextAttribute(pHMHandleData,
                                        (WORD)arg1));


    case DRQ_SETCONSOLEWINDOWINFO:
      return (HMDeviceConsoleBufferClass
              ::SetConsoleWindowInfo(pHMHandleData,
                                     (BOOL)arg1,
                                     (PSMALL_RECT)arg2));


    case DRQ_WRITECONSOLEA:
      return (HMDeviceConsoleBufferClass
              ::WriteConsoleA(pHMHandleData,
                              (CONST VOID*)arg1,
                              (DWORD)arg2,
                              (LPDWORD)arg3,
                              (LPVOID)arg4));


    case DRQ_WRITECONSOLEW:
      return (HMDeviceConsoleBufferClass
              ::WriteConsoleW(pHMHandleData,
                              (CONST VOID*)arg1,
                              (DWORD)arg2,
                              (LPDWORD)arg3,
                              (LPVOID)arg4));


    case DRQ_WRITECONSOLEOUTPUTA:
    {
      COORD coordSrcBufferSize;
      COORD coordSrcBufferCoord;

      ULONG2COORD(coordSrcBufferSize,  arg2);
      ULONG2COORD(coordSrcBufferCoord, arg3);

      return (HMDeviceConsoleBufferClass
              ::WriteConsoleOutputA(pHMHandleData,
                                    (PCHAR_INFO)arg1,
                                    coordSrcBufferSize,
                                    coordSrcBufferCoord,
                                    (PSMALL_RECT)arg4));
    }


    case DRQ_WRITECONSOLEOUTPUTW:
    {
      COORD coordSrcBufferSize;
      COORD coordSrcBufferCoord;

      ULONG2COORD(coordSrcBufferSize,  arg2);
      ULONG2COORD(coordSrcBufferCoord, arg3);

      return (HMDeviceConsoleBufferClass
              ::WriteConsoleOutputA(pHMHandleData,
                                    (PCHAR_INFO)arg1,
                                    coordSrcBufferSize,
                                    coordSrcBufferCoord,
                                    (PSMALL_RECT)arg4));
    }


    case DRQ_WRITECONSOLEOUTPUTATTRIBUTE:
    {
      COORD coordWriteCoord;

      ULONG2COORD(coordWriteCoord,  arg3);

      return (HMDeviceConsoleBufferClass
              ::WriteConsoleOutputAttribute(pHMHandleData,
                                            (LPWORD)arg1,
                                            (DWORD)arg2,
                                            coordWriteCoord,
                                            (LPDWORD)arg4));
    }


    case DRQ_WRITECONSOLEOUTPUTCHARACTERA:
    {
      COORD coordWriteCoord;

      ULONG2COORD(coordWriteCoord,  arg3);

      return (HMDeviceConsoleBufferClass
              ::WriteConsoleOutputCharacterA(pHMHandleData,
                                             (LPTSTR)arg1,
                                             (DWORD)arg2,
                                             coordWriteCoord,
                                             (LPDWORD)arg4));
    }


    case DRQ_WRITECONSOLEOUTPUTCHARACTERW:
    {
      COORD coordWriteCoord;

      ULONG2COORD(coordWriteCoord,  arg3);

      return (HMDeviceConsoleBufferClass
              ::WriteConsoleOutputCharacterW(pHMHandleData,
                                             (LPWSTR)arg1,
                                             (DWORD)arg2,
                                             coordWriteCoord,
                                             (LPDWORD)arg4));
    }


    case DRQ_INTERNAL_CONSOLEBUFFERMAP:
      ConsoleBufferMap((PCONSOLEBUFFER)pHMHandleData->lpHandlerData);
      return (NO_ERROR);


    case DRQ_INTERNAL_CONSOLECURSORSHOW:
      ConsoleCursorShow((PCONSOLEBUFFER)pHMHandleData->lpHandlerData,
                        (ULONG)arg1);
      return (NO_ERROR);


    case DRQ_INTERNAL_CONSOLEADJUSTWINDOW:
      ConsoleAdjustWindow((PCONSOLEBUFFER)pHMHandleData->lpHandlerData);
      return (NO_ERROR);
  }


#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE:HMDeviceConsoleBufferClass:_DeviceRequest %s(%08x,%08x,%08x,%08x,%08x,%08x) unknown request\n",
           lpHMDeviceName,
           pHMHandleData->hHandle,
           ulRequestCode,
           arg1,
           arg2,
           arg3,
           arg4);
#endif

  SetLastError(ERROR_INVALID_FUNCTION);           /* request not implemented */
  return(FALSE);                 /* we assume this indicates API call failed */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::FillConsoleOutputAttribute
 * Purpose   : fills the console buffer with a specified attribute
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             WORD          wAttribute
 *             DWORD         nLength
 *             COORD         dwWriteCoord
 *             LPDWORD       lpNumberOfAttrsWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::FillConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                                             WORD    wAttribute,
                                                             DWORD   nLength,
                                                             COORD   dwWriteCoord,
                                                             LPDWORD lpNumberOfAttrsWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::FillConsoleOutputAttribute(%08x,attr=%04x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           wAttribute,
           nLength,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpNumberOfAttrsWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpNumberOfAttrsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfAttrsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }

                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpNumberOfAttrsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfAttrsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < nLength;
       ulCounter++)
  {
                                                /* write attribute into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2 + 1)
     ) = (UCHAR)(wAttribute & 0xFF);
                                 /* write attribute, don't change characters */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpNumberOfAttrsWritten != NULL)       /* ensure pointer is valid */
          *lpNumberOfAttrsWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
          ConsoleGlobals.fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpNumberOfAttrsWritten != NULL)             /* ensure pointer is valid */
    *lpNumberOfAttrsWritten = nLength;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::FillConsoleOutputCharacterA
 * Purpose   : fills the console buffer with a specified ASCII character
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             UCHAR         ucCharacter
 *             DWORD         nLength
 *             COORD         dwWriteCoord
 *             LPDWORD       lpNumberOfCharsWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::FillConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                                              UCHAR   ucCharacter,
                                                              DWORD   nLength,
                                                              COORD   dwWriteCoord,
                                                              LPDWORD lpNumberOfCharsWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::FillConsoleOutputCharacterA(%08x,char=%02x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           ucCharacter,
           nLength,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpNumberOfCharsWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpNumberOfCharsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfCharsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpNumberOfCharsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfCharsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < nLength;
       ulCounter++)
  {
                                                /* write character into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2)
     ) = ucCharacter;

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpNumberOfCharsWritten != NULL)       /* ensure pointer is valid */
          *lpNumberOfCharsWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
          ConsoleGlobals.fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpNumberOfCharsWritten != NULL)             /* ensure pointer is valid */
    *lpNumberOfCharsWritten = nLength;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::FillConsoleOutputCharacterW
 * Purpose   : fills the console buffer with a specified ASCII character
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             WCHAR         wcCharacter
 *             DWORD         nLength
 *             COORD         dwWriteCoord
 *             LPDWORD       lpNumberOfCharsWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::FillConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                                              WCHAR   wcCharacter,
                                                              DWORD   nLength,
                                                              COORD   dwWriteCoord,
                                                              LPDWORD lpNumberOfCharsWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::FillConsoleOutputCharacterW(%08x,char=%02x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           wcCharacter,
           nLength,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpNumberOfCharsWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpNumberOfCharsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfCharsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpNumberOfCharsWritten != NULL)           /* ensure pointer is valid */
      *lpNumberOfCharsWritten = 0;                /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < nLength;
       ulCounter++)
  {
                                                /* write character into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2)
     ) = (UCHAR)wcCharacter;          /* @@@PH unicode to ascii conversion ! */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpNumberOfCharsWritten != NULL)       /* ensure pointer is valid */
          *lpNumberOfCharsWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
          ConsoleGlobals.fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpNumberOfCharsWritten != NULL)             /* ensure pointer is valid */
    *lpNumberOfCharsWritten = nLength;

  return (TRUE);
}



/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetConsoleMode
 * Purpose   : queries the current console mode
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPDWORD lpMode
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::GetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                                 LPDWORD       lpMode)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::GetConsoleMode(%08x,%08x).\n",
           pHMHandleData,
           lpMode);
#endif

  *lpMode = pConsoleBuffer->dwConsoleMode;    /* return current console mode */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetConsoleCursorInfo
 * Purpose   : queries the current console's cursor information
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCONSOLE_CURSOR_INFO pCCI
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::GetConsoleCursorInfo(PHMHANDLEDATA        pHMHandleData,
                                                       PCONSOLE_CURSOR_INFO pCCI)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::GetConsoleCursorInfo(%08x,%08x).\n",
           pHMHandleData,
           pCCI);
#endif

  memcpy(pCCI,                      /* just copy the whole information block */
         &pConsoleBuffer->CursorInfo,
         sizeof (pConsoleBuffer->CursorInfo) );

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetConsoleScreenBufferInfo
 * Purpose   : queries the current console screen buffer's info
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCONSOLE_SCREEN_BUFFER_INFO pCSBI
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::GetConsoleScreenBufferInfo(PHMHANDLEDATA               pHMHandleData,
                                                             PCONSOLE_SCREEN_BUFFER_INFO pCSBI)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::GetConsoleScreenBufferInfo(%08x,%08x).\n",
           pHMHandleData,
           pCSBI);
#endif

  pCSBI->dwSize           = pConsoleBuffer->coordBufferSize;
  pCSBI->dwCursorPosition = pConsoleBuffer->coordCursorPosition;
  pCSBI->wAttributes      = (USHORT)pConsoleBuffer->ucDefaultAttribute;

  /* @@@PH unsure, but should be OK */
  pCSBI->srWindow.Left   = pConsoleBuffer->coordWindowPosition.X;
  pCSBI->srWindow.Top    = pConsoleBuffer->coordWindowPosition.Y;
  pCSBI->srWindow.Right  = pConsoleBuffer->coordWindowPosition.X +
                           pConsoleBuffer->coordWindowSize.X - 1;
  pCSBI->srWindow.Bottom = pConsoleBuffer->coordWindowPosition.Y +
                           pConsoleBuffer->coordWindowSize.Y - 1;

  pCSBI->dwMaximumWindowSize = pConsoleBuffer->coordBufferSize;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::GetLargestConsoleWindowSize
 * Purpose   : Determine maximum AVIO size
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::GetLargestConsoleWindowSize(PHMHANDLEDATA pHMHandleData)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  COORD          coordSize;                             /* maximum avio size */
  LONG           lScreenCX;                   /* width and height of display */
  LONG           lScreenCY;
  APIRET         rc;                                       /* API returncode */


#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::GetLargestConsoleWindowSize(%08x).\n",
           pHMHandleData);
#endif

  /* @@@PH determine maximum console window size in characters
    based on display size and current avio font */

  lScreenCX = WinQuerySysValue(HWND_DESKTOP,            /* query PM for that */
                               SV_CXSCREEN);

  lScreenCY = WinQuerySysValue(HWND_DESKTOP,            /* query PM for that */
                               SV_CYFULLSCREEN);

  if (rc != NO_ERROR)
  {
    WriteLog("KERNEL32/CONSOLE: VioGetDeviceCellSize failed with #%u.\n",
             rc);

    return (FALSE);                                        /* say API failed */
  }

  if ( (ConsoleGlobals.sCellCX == 0) ||          /* prevent division by zero */
       (ConsoleGlobals.sCellCY == 0) )
  {
    WriteLog("KERNEL32/CONSOLE: VioGetDeviceCellSize returned 0 value.\n");

    return (FALSE);                                        /* say API failed */
  }

  coordSize.X = lScreenCX / ConsoleGlobals.sCellCX;                            /* calculate */
  coordSize.Y = lScreenCY / ConsoleGlobals.sCellCY;

                /* these limitations are due to OS/2's current VIO subsystem */
  coordSize.X = min(coordSize.X, MAX_OS2_COLUMNS);
  coordSize.Y = min(coordSize.Y, MAX_OS2_ROWS);

  return (COORD2ULONG(coordSize));                           /* return value */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputA
 * Purpose   : reads character and color attribute data from screen rectangle
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCHAR_INFO    pchiDestBuffer
 *             COORD         coordDestBufferSize
 *             COORD         coordDestBufferCoord
 *             PSMALL_RECT   psrctSourceRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputA(PHMHANDLEDATA pHMHandleData,
                                                     PCHAR_INFO    pchiDestBuffer,
                                                     COORD         coordDestBufferSize,
                                                     COORD         coordDestBufferCoord,
                                                     PSMALL_RECT   psrctSourceRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulX,  ulY;                          /* current coordinate to be read */
  ULONG ulCX, ulCY;                       /* width and height of target area */
  ULONG ulReadX, ulReadY;                      /* position data is read from */
  WORD  wCell;                                        /* currently read data */

  PCHAR_INFO pchi;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputA(%08x,%08x,x=%u y=%u,x=%u y=%u, %08x).\n",
           pHMHandleData,
           pchiDestBuffer,
           coordDestBufferSize.X,
           coordDestBufferSize.Y,
           coordDestBufferCoord.X,
           coordDestBufferCoord.Y,
           psrctSourceRect);
#endif


  /* verify psrctSourceRect first */
  psrctSourceRect->Left  = max(psrctSourceRect->Left,  0);
  psrctSourceRect->Top   = max(psrctSourceRect->Top,   0);
  psrctSourceRect->Right = min(psrctSourceRect->Right, pConsoleBuffer->coordBufferSize.X - 1);
  psrctSourceRect->Bottom= min(psrctSourceRect->Bottom,pConsoleBuffer->coordBufferSize.Y - 1);

                                                     /* verify target buffer */
  if ( (coordDestBufferSize.X < coordDestBufferCoord.X) ||
       (coordDestBufferSize.Y < coordDestBufferCoord.Y) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);        /* set detailed error info */
    return (FALSE);                                            /* API failed */
  }

  ulCX = coordDestBufferSize.X - coordDestBufferCoord.X;
  ulCY = coordDestBufferSize.Y - coordDestBufferCoord.Y;

  ulCX = min(ulCX, (psrctSourceRect->Right  - psrctSourceRect->Left));
  ulCY = min(ulCY, (psrctSourceRect->Bottom - psrctSourceRect->Top));

                                  /* final calculation of the copy rectangle */
  psrctSourceRect->Right  = psrctSourceRect->Left + ulCX;
  psrctSourceRect->Bottom = psrctSourceRect->Top  + ulCY;


  for (ulY = 0,
       ulReadY = psrctSourceRect->Top;

       ulY <= ulCY;

       ulY++,
       ulReadY++)
  {
    pchi = pchiDestBuffer + sizeof(CHAR_INFO) * coordDestBufferCoord.X
                          + sizeof(CHAR_INFO) * (coordDestBufferCoord.Y + ulY)
                            * coordDestBufferSize.X;
    for (ulX = 0,
         ulReadX = psrctSourceRect->Left;

         ulX <= ulCX;

         ulX++,
         ulReadX++,
         pchi++)
    {
                                                           /* read character */
      wCell = *(pConsoleBuffer->ppszLine[ulReadY] + ulReadX * 2);

      pchi->Char.AsciiChar = (UCHAR)(wCell & 0x00FF);
      pchi->Attributes     = wCell >> 8;
    }
  }

  return (TRUE);                                            /* OK, that's it */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputW
 * Purpose   : reads character and color attribute data from screen rectangle
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCHAR_INFO    pchiDestBuffer
 *             COORD         coordDestBufferSize
 *             COORD         coordDestBufferCoord
 *             PSMALL_RECT   psrctSourceRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputW(PHMHANDLEDATA pHMHandleData,
                                                     PCHAR_INFO    pchiDestBuffer,
                                                     COORD         coordDestBufferSize,
                                                     COORD         coordDestBufferCoord,
                                                     PSMALL_RECT   psrctSourceRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulX,  ulY;                          /* current coordinate to be read */
  ULONG ulCX, ulCY;                       /* width and height of target area */
  ULONG ulReadX, ulReadY;                      /* position data is read from */
  WORD  wCell;                                        /* currently read data */

  PCHAR_INFO pchi;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputW(%08x,%08x,x=%u y=%u,x=%u y=%u, %08x).\n",
           pHMHandleData,
           pchiDestBuffer,
           coordDestBufferSize.X,
           coordDestBufferSize.Y,
           coordDestBufferCoord.X,
           coordDestBufferCoord.Y,
           psrctSourceRect);
#endif


  /* verify psrctSourceRect first */
  psrctSourceRect->Left  = max(psrctSourceRect->Left,  0);
  psrctSourceRect->Top   = max(psrctSourceRect->Top,   0);
  psrctSourceRect->Right = min(psrctSourceRect->Right, pConsoleBuffer->coordBufferSize.X - 1);
  psrctSourceRect->Bottom= min(psrctSourceRect->Bottom,pConsoleBuffer->coordBufferSize.Y - 1);

                                                     /* verify target buffer */
  if ( (coordDestBufferSize.X < coordDestBufferCoord.X) ||
       (coordDestBufferSize.Y < coordDestBufferCoord.Y) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);        /* set detailed error info */
    return (FALSE);                                            /* API failed */
  }

  ulCX = coordDestBufferSize.X - coordDestBufferCoord.X;
  ulCY = coordDestBufferSize.Y - coordDestBufferCoord.Y;

  ulCX = min(ulCX, (psrctSourceRect->Right  - psrctSourceRect->Left));
  ulCY = min(ulCY, (psrctSourceRect->Bottom - psrctSourceRect->Top));

                                  /* final calculation of the copy rectangle */
  psrctSourceRect->Right  = psrctSourceRect->Left + ulCX;
  psrctSourceRect->Bottom = psrctSourceRect->Top  + ulCY;


  for (ulY = 0,
       ulReadY = psrctSourceRect->Top;

       ulY <= ulCY;

       ulY++,
       ulReadY++)
  {
    pchi = pchiDestBuffer + sizeof(CHAR_INFO) * coordDestBufferCoord.X
                          + sizeof(CHAR_INFO) * (coordDestBufferCoord.Y + ulY)
                            * coordDestBufferSize.X;
    for (ulX = 0,
         ulReadX = psrctSourceRect->Left;

         ulX <= ulCX;

         ulX++,
         ulReadX++,
         pchi++)
    {
                                                           /* read character */
      wCell = *(pConsoleBuffer->ppszLine[ulReadY] + ulReadX * 2);

                                                     /* @@@PH Ascii->Unicode */
      pchi->Char.UnicodeChar = (UCHAR)(wCell & 0x00FF);
      pchi->Attributes     = wCell >> 8;
    }
  }

  return (TRUE);                                            /* OK, that's it */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputAttribute
 * Purpose   : read an array with specified attributes from the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cReadCells
 *             COORD         dwReadCoord
 *             LPDWORD       lpcNumberRead
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                                             LPWORD        lpwAttribute,
                                                             DWORD         cReadCells,
                                                             COORD         dwReadCoord,
                                                             LPDWORD       lpcNumberRead)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputAttribute(%08x,pattr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpwAttribute,
           cReadCells,
           dwReadCoord.X,
           dwReadCoord.Y,
           lpcNumberRead);
#endif

  if ( (dwReadCoord.X < 0) ||
       (dwReadCoord.Y < 0) )
  {
    if (lpcNumberRead != NULL)                       /* ensure pointer is valid */
      *lpcNumberRead = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }

                                    /* check if dwReadCoord is within specs */
  if ( (dwReadCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcNumberRead != NULL)                       /* ensure pointer is valid */
      *lpcNumberRead = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < cReadCells;
       ulCounter++,
       lpwAttribute++)
  {
                                                /* write attribute into cell */
    *lpwAttribute = (UCHAR)
      *(pConsoleBuffer->ppszLine[dwReadCoord.Y] +
                                (dwReadCoord.X * 2 + 1));

    dwReadCoord.X++;                                 /* move write position */
    if (dwReadCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwReadCoord.X = 0;                               /* skip to next line */
      dwReadCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcNumberRead != NULL)                   /* ensure pointer is valid */
          *lpcNumberRead = ulCounter;

        return (TRUE);
      }
    }
  }

  if (lpcNumberRead != NULL)                         /* ensure pointer is valid */
    *lpcNumberRead = cReadCells;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputCharacterA
 * Purpose   : read an array with specified characters from the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpReadBuffer
 *             DWORD         cRead
 *             COORD         coordReadCoord
 *             LPDWORD       lpcNumberRead
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                                              LPTSTR        lpwReadBuffer,
                                                              DWORD         cchRead,
                                                              COORD         coordReadCoord,
                                                              LPDWORD       lpcNumberRead)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputCharacterA(%08x,pattr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpwReadBuffer,
           cchRead,
           coordReadCoord.X,
           coordReadCoord.Y,
           lpcNumberRead);
#endif

  if ( (coordReadCoord.X < 0) ||
       (coordReadCoord.Y < 0) )
  {
    if (lpcNumberRead != NULL)                    /* ensure pointer is valid */
      *lpcNumberRead = 0;                         /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }

                                  /* check if coordReadCoord is within specs */
  if ( (coordReadCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (coordReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcNumberRead != NULL)                    /* ensure pointer is valid */
      *lpcNumberRead = 0;                         /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < cchRead;
       ulCounter++,
       lpwReadBuffer++)
  {
                                                /* write character into cell */
    *lpwReadBuffer =
      *(pConsoleBuffer->ppszLine[coordReadCoord.Y] +
                                (coordReadCoord.X * 2));

    coordReadCoord.X++;                               /* move write position */
    if (coordReadCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      coordReadCoord.X = 0;                             /* skip to next line */
      coordReadCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (coordReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcNumberRead != NULL)                /* ensure pointer is valid */
          *lpcNumberRead = ulCounter;

        return (TRUE);
      }
    }
  }

  if (lpcNumberRead != NULL)                         /* ensure pointer is valid */
    *lpcNumberRead = cchRead;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputCharacterW
 * Purpose   : read an array with specified characters from the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpReadBuffer
 *             DWORD         cRead
 *             COORD         coordReadCoord
 *             LPDWORD       lpcNumberRead
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ReadConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                                              LPWSTR        lpwReadBuffer,
                                                              DWORD         cchRead,
                                                              COORD         coordReadCoord,
                                                              LPDWORD       lpcNumberRead)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ReadConsoleOutputCharacterW(%08x,pattr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpwReadBuffer,
           cchRead,
           coordReadCoord.X,
           coordReadCoord.Y,
           lpcNumberRead);
#endif

  if ( (coordReadCoord.X < 0) ||
       (coordReadCoord.Y < 0) )
  {
    if (lpcNumberRead != NULL)                    /* ensure pointer is valid */
      *lpcNumberRead = 0;                         /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }

                                  /* check if coordReadCoord is within specs */
  if ( (coordReadCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (coordReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcNumberRead != NULL)                    /* ensure pointer is valid */
      *lpcNumberRead = 0;                         /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < cchRead;
       ulCounter++,
       lpwReadBuffer++)
  {
    /* @@@PH Ascii -> Unicode translation */
                                                /* write character into cell */
    *lpwReadBuffer =
      *(pConsoleBuffer->ppszLine[coordReadCoord.Y] +
                                (coordReadCoord.X * 2));

    coordReadCoord.X++;                               /* move write position */
    if (coordReadCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      coordReadCoord.X = 0;                             /* skip to next line */
      coordReadCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (coordReadCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcNumberRead != NULL)                /* ensure pointer is valid */
          *lpcNumberRead = ulCounter;

        return (TRUE);
      }
    }
  }

  if (lpcNumberRead != NULL)                         /* ensure pointer is valid */
    *lpcNumberRead = cchRead;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ScrollConsoleScreenBufferA
 * Purpose   : move a block of data within the screen buffer
 * Parameters: PHMHANDLEDATA pHMHandleData   - handle specific data
 *             PSMALL_RECT   psrctSourceRect - source rectangle
 *             PSMALL_RECT   psrctClipRect   - clipping rectangle
 *             COORD         coordDestOrigin - destination coordinate
 *             PCHAR_INFO    pchiFill        - fill character
 * Variables :
 * Result    :
 * Remark    : Routine is subject to optimizations.
 *             @@@PH rewrite -> faster, better handling of overlapped buffers
 *                   copy srctSource to buffer, fill it with fill character
 *                   copy buffer to srctDest ?
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ScrollConsoleScreenBufferA(PHMHANDLEDATA pHMHandleData,
                                                             PSMALL_RECT   psrctSourceRect,
                                                             PSMALL_RECT   psrctClipRect,
                                                             COORD         coordDestOrigin,
                                                             PCHAR_INFO    pchiFill)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  SMALL_RECT     srctView;            /* the actual rectangle of "happening" */
  SMALL_RECT     srctSource;       /* the clipped source and dest rectangles */
  SMALL_RECT     srctDest;
  int            iX, iY;                             /* scan loop counters */
  PUSHORT        pusTarget, pusSource;      /* pointer to source, dest cells */
  WORD           wAttr;                      /* fill character and attribute */
  int            iBlitDirection;             /* to handle overlapped buffers */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ScrollConsoleScreenBufferA(%08x,%08x,%08x,x=%u y=%u,%08x).\n",
           pHMHandleData,
           psrctSourceRect,
           psrctClipRect,
           coordDestOrigin.X,
           coordDestOrigin.Y,
           pchiFill);
#endif

                                   /* calculate effective clipping rectangle */
  if (psrctClipRect != NULL)          /* if clipping rectangle was specified */
  {
    memcpy(&srctView,
           psrctClipRect,
           sizeof (SMALL_RECT) );

                                          /* check boundary with buffer size */
    srctView.Left   = max(0, srctView.Left);
    srctView.Top    = max(0, srctView.Top );
    srctView.Right  = min(srctView.Right,  pConsoleBuffer->coordBufferSize.X);
    srctView.Bottom = min(srctView.Bottom, pConsoleBuffer->coordBufferSize.Y);
  }
  else
  {
    srctView.Left   = 0;   /* buffer size is the maximum clipping rectangle */
    srctView.Top    = 0;
    srctView.Right  = pConsoleBuffer->coordBufferSize.X;
    srctView.Bottom = pConsoleBuffer->coordBufferSize.Y;
  }

  memcpy(&srctSource,                               /* copy source rectangle */
         psrctSourceRect,
         sizeof (srctSource) );
                                   /* check boundary with clipping rectangle */
  srctSource.Left   = max(srctSource.Left,  srctView.Left  );
  srctSource.Top    = max(srctSource.Top,   srctView.Top   );
  srctSource.Right  = min(srctSource.Right, srctView.Right );
  srctSource.Bottom = min(srctSource.Bottom,srctView.Bottom);

  srctDest.Left  = max(srctView.Left,   coordDestOrigin.X);
  srctDest.Top   = max(srctView.Top,    coordDestOrigin.Y);
  srctDest.Right = min(srctView.Right,  srctDest.Left + srctSource.Right  - srctSource.Left);
  srctDest.Bottom= min(srctView.Bottom, srctDest.Top  + srctSource.Bottom - srctSource.Top);

  /****************************
   * first copy the rectangle *
   ****************************/

  if (srctDest.Left >  srctSource.Left) iBlitDirection  = 0;
  else                                  iBlitDirection  = 1;
  if (srctDest.Top  >  srctSource.Top)  iBlitDirection += 2;

                               /* this leaves us with three different cases: */
                               /*                                            */
                               /* 0 - dest is to upper left of the source    */
                               /* 1 - dest is to upper right of the source   */
                               /* 2 - dest is to lower left of the source    */
                               /* 3 - dest is to lower right of the source   */

  switch (iBlitDirection)
  {
    /**************
     * upper left *
     **************/
    case 0:
      for (iY = 0;
           iY <= srctDest.Bottom - srctDest.Top;
           iY++)
      {
                         /* calculate pointer to start of target screen line */
        pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctDest.Top] +
                               (srctDest.Left << 1) );

                         /* calculate pointer to start of source screen line */
        pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctSource.Top] +
                               (srctSource.Left << 1) );

        for (iX = srctDest.Left;
             iX <= srctDest.Right;
             iX++,
             pusTarget++,
             pusSource++)
          *pusTarget = *pusSource;                         /* copy character */
      }
      break;

    /***************
     * upper right *
     ***************/
    case 1:
      for (iY = 0;
           iY <= srctDest.Bottom - srctDest.Top;
           iY++)
      {
                           /* calculate pointer to end of target screen line */
        pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctDest.Top] +
                               ( srctDest.Right << 1) );

                           /* calculate pointer to end of source screen line */
        pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctSource.Top] +
                               ( srctSource.Right << 1) );

        for (iX = srctDest.Right;
             iX >= srctDest.Left;
             iX--,
             pusTarget--,
             pusSource--)
          *pusTarget = *pusSource;                         /* copy character */
      }
      break;

    /***************
     * lower left  *
     ***************/
    case 2:
      for (iY = srctDest.Bottom - srctDest.Top;
           iY >= 0;
           iY--)
      {
                         /* calculate pointer to start of target screen line */
        pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctDest.Top] +
                               (srctDest.Left << 1) );

                         /* calculate pointer to start of source screen line */
        pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctSource.Top] +
                               (srctSource.Left << 1) );

        for (iX = srctDest.Left;
             iX <= srctDest.Right;
             iX++,
             pusTarget++,
             pusSource++)
          *pusTarget = *pusSource;                         /* copy character */
      }
      break;

    /****************
     * lower right  *
     ****************/
    case 3:
      for (iY = srctDest.Bottom - srctDest.Top;
           iY >= 0;
           iY--)
      {
                           /* calculate pointer to end of target screen line */
        pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctDest.Top] +
                               ( srctDest.Right << 1) );

                           /* calculate pointer to end of source screen line */
        pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[iY + srctSource.Top] +
                               (srctSource.Right << 1) );

        for (iX = srctDest.Right;
             iX >= srctDest.Left;
             iX--,
             pusTarget--,
             pusSource--)
          *pusTarget = *pusSource;                         /* copy character */
      }
      break;
  }


              /* this is the character and attribute for the uncovered cells */
  wAttr = (pchiFill->Char.AsciiChar) + (pchiFill->Attributes << 8);

  for (iY = srctSource.Top;            /* now fill uncovered area with pchi */
       iY < srctSource.Bottom;
       iY++)
  {
    pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[iY] + srctSource.Left);

    for (iX = srctSource.Left;
         iX < srctSource.Right;
         iX++,
         pusTarget++)
         /* check if covered by srctDest or not */
      if ( (iY >= srctDest.Top)    &&
           (iY <= srctDest.Bottom) &&
           (iX >= srctDest.Left)   &&
           (iX <= srctDest.Right)
         )
        ;                              /* should be faster for the optimizer */
      else
        *pusTarget = wAttr;            /* write fill character and attribute */
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::ScrollConsoleScreenBufferW
 * Purpose   : move a block of data within the screen buffer


 * Parameters: PHMHANDLEDATA pHMHandleData   - handle specific data
 *             PSMALL_RECT   psrctSourceRect - source rectangle
 *             PSMALL_RECT   psrctClipRect   - clipping rectangle
 *             COORD         coordDestOrigin - destination coordinate
 *             PCHAR_INFO    pchiFill        - fill character
 * Variables :
 * Result    :
 * Remark    : Routine is subject to optimizations.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::ScrollConsoleScreenBufferW(PHMHANDLEDATA pHMHandleData,
                                                             PSMALL_RECT   psrctSourceRect,
                                                             PSMALL_RECT   psrctClipRect,
                                                             COORD         coordDestOrigin,
                                                             PCHAR_INFO    pchiFill)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  SMALL_RECT     srctView;            /* the actual rectangle of "happening" */
  SMALL_RECT     srctSource;       /* the clipped source and dest rectangles */
  SMALL_RECT     srctDest;
  ULONG          ulX, ulY;                             /* scan loop counters */
  PUSHORT        pusTarget, pusSource;      /* pointer to source, dest cells */
  WORD           wAttr;                      /* fill character and attribute */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::ScrollConsoleScreenBufferW(%08x,%08x,%08x,x=%u y=%u,%08x).\n",
           pHMHandleData,
           psrctSourceRect,
           psrctClipRect,
           coordDestOrigin.X,
           coordDestOrigin.Y,
           pchiFill);
#endif

                                   /* calculate effective clipping rectangle */
  if (psrctClipRect != NULL)          /* if clipping rectangle was specified */
  {
    memcpy(&srctView,
           psrctClipRect,
           sizeof (SMALL_RECT) );

                                          /* check boundary with buffer size */
    srctView.Left   = max(0, srctView.Left);
    srctView.Top    = max(0, srctView.Top );
    srctView.Right  = min(srctView.Right,  pConsoleBuffer->coordBufferSize.X);
    srctView.Bottom = min(srctView.Bottom, pConsoleBuffer->coordBufferSize.Y);
  }
  else
  {
    srctView.Left   = 0;   /* buffer size is the maximum clipping rectangle */
    srctView.Top    = 0;
    srctView.Right  = pConsoleBuffer->coordBufferSize.X;
    srctView.Bottom = pConsoleBuffer->coordBufferSize.Y;
  }

  memcpy(&srctSource,                               /* copy source rectangle */
         psrctSourceRect,
         sizeof (srctSource) );
                                   /* check boundary with clipping rectangle */
  srctSource.Left   = max(srctSource.Left,  srctView.Left  );
  srctSource.Top    = max(srctSource.Top,   srctView.Top   );
  srctSource.Right  = min(srctSource.Right, srctView.Right );
  srctSource.Bottom = min(srctSource.Bottom,srctView.Bottom);

  srctDest.Left  = max(srctView.Left,   coordDestOrigin.X);
  srctDest.Top   = max(srctView.Top,    coordDestOrigin.Y);
  srctDest.Right = min(srctView.Right,  srctDest.Left + srctSource.Right  - srctSource.Left);
  srctDest.Bottom= min(srctView.Bottom, srctDest.Top  + srctSource.Bottom - srctSource.Top);

  /* first copy the rectangle */
  for (ulY = 0;
       ulY < srctDest.Bottom - srctDest.Top;
       ulY++)
  {
                         /* calculate pointer to start of target screen line */
    pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[ulY + srctDest.Top] +
                           srctDest.Left);

                         /* calculate pointer to start of source screen line */
    pusSource = (PUSHORT) (pConsoleBuffer->ppszLine[ulY + srctSource.Top] +
                           srctSource.Left);

    for (ulX = srctDest.Left;
         ulX < srctDest.Right;
         ulX++,
         pusTarget++,
         pusSource++)
      *pusTarget = *pusSource;                             /* copy character */
  }


              /* this is the character and attribute for the uncovered cells */
  /* @@@PH Unicode->Ascii translation */
  wAttr = (pchiFill->Char.UnicodeChar) + (pchiFill->Attributes << 8);

  for (ulY = srctSource.Top;            /* now fill uncovered area with pchi */
       ulY < srctSource.Bottom;
       ulY++)
  {
    pusTarget = (PUSHORT) (pConsoleBuffer->ppszLine[ulY] + srctSource.Left);

    for (ulX = srctSource.Left;
         ulX < srctSource.Right;
         ulX++,
         pusTarget++)
      *pusTarget = wAttr;              /* write fill character and attribute */
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::SetConsoleCursorInfo
 * Purpose   : sets the current console's cursor information
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCONSOLE_CURSOR_INFO pCCI
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleCursorInfo(PHMHANDLEDATA        pHMHandleData,
                                                       PCONSOLE_CURSOR_INFO pCCI)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleCursorInfo(%08x,%08x).\n",
           pHMHandleData,
           pCCI);
#endif

                                                       /* validate structure */
  if ( (pCCI->dwSize < 1) ||
       (pCCI->dwSize > 100) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);        /* set extended error info */
    return (FALSE);                                            /* API failed */
  }

              /* if we're the active buffer, remove cursor from screen first */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleCursorShow(pConsoleBuffer,
                      CONSOLECURSOR_HIDE);

  memcpy(&pConsoleBuffer->CursorInfo,    /* copy the whole information block */
         pCCI,
         sizeof (pConsoleBuffer->CursorInfo) );

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::SetConsoleCursorPosition
 * Purpose   : sets the current console's cursor position
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             COORD         coordCursorPosition
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleCursorPosition(PHMHANDLEDATA pHMHandleData,
                                                           COORD         coordCursorPosition)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleCursorPosition(%08x,x=%u.y=%u).\n",
           pHMHandleData,
           coordCursorPosition.X,
           coordCursorPosition.Y);
#endif

                                  /* @@@PH remove cursor from screen first ! */
  pConsoleBuffer->coordCursorPosition = coordCursorPosition;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::SetConsoleMode
 * Purpose   : sets the current console mode
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             DWORD         dwMode        - console mode
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleMode(PHMHANDLEDATA pHMHandleData,
                                                 DWORD         dwMode)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleMode(%08x,%08x).\n",
           pHMHandleData,
           dwMode);
#endif

  pConsoleBuffer->dwConsoleMode = dwMode;        /* set current console mode */

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::SetConsoleScreenBufferSize
 * Purpose   : allocate or re-allocate the screenbuffer and transform the
 *             old buffer as required
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             COORD         coordSize     - the new buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleScreenBufferSize (PHMHANDLEDATA pHMHandleData,
                                                              COORD         coordSize)
{
  ULONG ulSize;                         /* calculated size of the new buffer */
  PSZ  *ppszNew;                                 /* pointer to the new array */
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulLine;                                        /* line index counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleScreenBufferSize(%u,%u).\n",
           coordSize.X,
           coordSize.Y);
#endif


                                 /* re-allocate the whole line-pointer array */
  ulSize = coordSize.Y * (coordSize.X * 2 + sizeof (PSZ) );
  if (ulSize == 0)                          /* set new buffer size to zero ? */
  {
    if (pConsoleBuffer->ppszLine != NULL)        /* if old buffer is present */
      free (pConsoleBuffer->ppszLine);                    /* free old buffer */

    pConsoleBuffer->ppszLine          = NULL;
    pConsoleBuffer->coordBufferSize.X = 0;
    pConsoleBuffer->coordBufferSize.Y = 0;
    pConsoleBuffer->coordWindowSize.X = 0;
    pConsoleBuffer->coordWindowSize.Y = 0;
    pConsoleBuffer->coordWindowPosition.X = 0;
    pConsoleBuffer->coordWindowPosition.Y = 0;

    return (TRUE);                                                     /* OK */
  }


  ppszNew = (PSZ *) malloc(ulSize);                        /* allocate array */
  if (ppszNew == NULL)                            /* check proper allocation */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);          /* set error information */
    return (FALSE);                                 /* raise error condition */
  }


  for (ulLine = 0;                               /* setup line pointer array */
       ulLine < coordSize.Y;
       ulLine++)
  {
                                                   /* calculate line pointer */
    ulSize = (ULONG)ppszNew + (coordSize.Y * sizeof(PSZ) )
                            + (coordSize.X * ulLine * 2);
    ppszNew[ulLine] = (PSZ)ulSize;                /* OK, write index pointer */
  }

  ulSize     = ( ((ULONG)(pConsoleBuffer->ucDefaultAttribute) << 8) +
                 ((ULONG)' ') +
                 ((ULONG)(pConsoleBuffer->ucDefaultAttribute) << 24) +
                 ((ULONG)' ' << 16) );

                                                    /* scroll the line index */
  for (ulLine = 0;
       ulLine < coordSize.Y;
       ulLine++)
    ConsoleBufferFillLine(ulSize,
                          (PUSHORT)(ppszNew[ulLine]),
                          coordSize.X);



                                                   /* copy lines as required */
  if (pConsoleBuffer->ppszLine != NULL)         /* previous buffer present ? */
  {
    ULONG x, y;

                                          /* copy old characters as required */
    x = min(pConsoleBuffer->coordBufferSize.X, coordSize.X);
    y = min(pConsoleBuffer->coordBufferSize.Y, coordSize.Y);

    for (ulLine = 0;                                    /* copy line by line */
         ulLine < y;
         ulLine++)
      memcpy(ppszNew[ulLine],
             pConsoleBuffer->ppszLine[ulLine],
             x * 2);

    free (pConsoleBuffer->ppszLine);    /* free previous screen buffer array */
  }


  pConsoleBuffer->ppszLine          = ppszNew;     /* poke in the new values */
  pConsoleBuffer->coordBufferSize.X = coordSize.X;
  pConsoleBuffer->coordBufferSize.Y = coordSize.Y;
  pConsoleBuffer->coordCursorPosition.X = 0;
  pConsoleBuffer->coordCursorPosition.Y = 0;

  /* @@@PH to be changed ! */
  pConsoleBuffer->coordWindowSize.X = coordSize.X;                /* default */
  pConsoleBuffer->coordWindowSize.Y = coordSize.Y;
  pConsoleBuffer->coordWindowPosition.X = 0;
  pConsoleBuffer->coordWindowPosition.Y = 0;

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return TRUE;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleTextAttribute(PHMHANDLEDATA pHMHandleData,
                                                          WORD          wAttr)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleTextAttribute(%u).\n",
           wAttr);
#endif

  pConsoleBuffer->ucDefaultAttribute = (UCHAR)wAttr;
  return (TRUE);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::SetConsoleActiveScreenBuffer(PHMHANDLEDATA pHMHandleData)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleActiveScreenBuffer().\n");
#endif

                              /* set new buffer handle to the global console */
  ConsoleGlobals.hConsoleBuffer  = pHMHandleData->hHandle;
  ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);
}


/*****************************************************************************
 * Name      : BOOL HMDeviceConsoleBufferClass::SetConsoleWindowInfo
 * Purpose   : set a new size to the console window
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             BOOL          fAbsolute
 *             PSMALL_RECT   psrctWindowRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

BOOL HMDeviceConsoleBufferClass::SetConsoleWindowInfo(PHMHANDLEDATA pHMHandleData,
                                                      BOOL          fAbsolute,
                                                      PSMALL_RECT   psrctWindowRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::SetConsoleWindowInfo(%08x,%u,%08x).\n",
           pHMHandleData,
           fAbsolute,
           psrctWindowRect);
#endif

  if (fAbsolute == TRUE)                  /* absolute coordinates provided ? */
  {
    if ( (psrctWindowRect->Left   < 0) ||          /* check parameters first */
         (psrctWindowRect->Top    < 0) ||
         (psrctWindowRect->Right  < 0) ||
         (psrctWindowRect->Bottom < 0) ||
         (psrctWindowRect->Right  <= psrctWindowRect->Left) ||
         (psrctWindowRect->Bottom <= psrctWindowRect->Top)
       )
    {
      SetLastError(ERROR_INVALID_PARAMETER);        /* set error information */
      return (FALSE);                                               /* error */
    }

                                 /* check we don't go beyond screen buffer ! */
    if ( ((psrctWindowRect->Right  - psrctWindowRect->Left) > pConsoleBuffer->coordBufferSize.X) ||
         ((psrctWindowRect->Bottom - psrctWindowRect->Top ) > pConsoleBuffer->coordBufferSize.Y) ||
         (psrctWindowRect->Left >= pConsoleBuffer->coordBufferSize.X) ||
         (psrctWindowRect->Top  >= pConsoleBuffer->coordBufferSize.Y)
       )
    {
      SetLastError(ERROR_INVALID_PARAMETER);        /* set error information */
      return (FALSE);                                               /* error */
    }

    pConsoleBuffer->coordWindowSize.X     = psrctWindowRect->Right -
                                            psrctWindowRect->Left;
    pConsoleBuffer->coordWindowSize.Y     = psrctWindowRect->Bottom -
                                            psrctWindowRect->Top;

    pConsoleBuffer->coordWindowPosition.X = psrctWindowRect->Left;
    pConsoleBuffer->coordWindowPosition.Y = psrctWindowRect->Top;
  }
  else
  {
    int iSizeX;                                      /* relative coordinates */
    int iSizeY;
    int iPosX;
    int iPosY;

    iSizeX = pConsoleBuffer->coordWindowSize.X + psrctWindowRect->Left + psrctWindowRect->Right;
    iSizeY = pConsoleBuffer->coordWindowSize.Y + psrctWindowRect->Top  + psrctWindowRect->Bottom;
    iPosX  = pConsoleBuffer->coordWindowPosition.X  + psrctWindowRect->Left;
    iPosY  = pConsoleBuffer->coordWindowPosition.Y  + psrctWindowRect->Top;

                                 /* check we don't go beyond screen buffer ! */
    if ( (iSizeX > pConsoleBuffer->coordBufferSize.X) ||
         (iSizeY > pConsoleBuffer->coordBufferSize.Y) ||
         (iPosX >= pConsoleBuffer->coordBufferSize.X) ||
         (iPosY >= pConsoleBuffer->coordBufferSize.Y) ||
         (iSizeX < 0) ||
         (iSizeY < 0) ||
         (iPosX  < 0) ||
         (iPosY  < 0)
       )
    {
      SetLastError(ERROR_INVALID_PARAMETER);        /* set error information */
      return (FALSE);                                               /* error */
    }

                                       /* Values are verified for being OK ! */
    pConsoleBuffer->coordWindowPosition.X  = iPosX;
    pConsoleBuffer->coordWindowPosition.Y  = iPosY;
    pConsoleBuffer->coordWindowSize.X = iSizeX;
    pConsoleBuffer->coordWindowSize.Y = iSizeY;
  }

  /* update window */
  /* @@@PH

   ConsoleWindowResize(COORD coordWindowSize,
                       COORD coordWindowPos,

   */

                                          /* update window contents (scroll) */
                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);                                                       /* OK */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleA
 * Purpose   : write a string to the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cWriteCells
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleA(PHMHANDLEDATA pHMHandleData,
                                                CONST VOID*   lpvBuffer,
                                                DWORD         cchToWrite,
                                                LPDWORD       lpcchWritten,
                                                LPVOID        lpvReserved)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleA(%08x,%08x,%u,%08x,%08x).\n",
           pHMHandleData,
           lpvBuffer,
           cchToWrite,
           lpcchWritten,
           lpvReserved);
#endif

  /* simply forward the request to that routine */
  return (HMDeviceConsoleBufferClass::WriteFile(pHMHandleData,
                                                lpvBuffer,
                                                cchToWrite,
                                                lpcchWritten,
                                                NULL));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleW
 * Purpose   : write a string to the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cWriteCells
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleW(PHMHANDLEDATA pHMHandleData,
                                                CONST VOID*   lpvBuffer,
                                                DWORD         cchToWrite,
                                                LPDWORD       lpcchWritten,
                                                LPVOID        lpvReserved)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleW(%08x,%08x,%u,%08x,%08x).\n",
           pHMHandleData,
           lpvBuffer,
           cchToWrite,
           lpcchWritten,
           lpvReserved);
#endif

  /* @@@PH AScii -> unicode translation */

  /* simply forward the request to that routine */
  return (HMDeviceConsoleBufferClass::WriteFile(pHMHandleData,
                                                lpvBuffer,
                                                cchToWrite,
                                                lpcchWritten,
                                                NULL));
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputA
 * Purpose   : write character and color attribute data to screen rectangle
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCHAR_INFO    pchiSrcBuffer
 *             COORD         coordSrcBufferSize
 *             COORD         coordSrcBufferCoord
 *             PSMALL_RECT   psrctDestRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputA(PHMHANDLEDATA pHMHandleData,
                                                      PCHAR_INFO    pchiSrcBuffer,
                                                      COORD         coordSrcBufferSize,
                                                      COORD         coordSrcBufferCoord,
                                                      PSMALL_RECT   psrctDestRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulX,      ulY;                      /* current coordinate to be read */
  ULONG ulCX,     ulCY;                   /* width and height of target area */
  ULONG ulWriteX, ulWriteY;                    /* position data is read from */
  WORD  wCell;                                        /* currently read data */

  PCHAR_INFO pchi;
  PSZ        pszTarget;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputA(%08x,%08x,x=%u y=%u,x=%u y=%u, %08x).\n",
           pHMHandleData,
           pchiSrcBuffer,
           coordSrcBufferSize.X,
           coordSrcBufferSize.Y,
           coordSrcBufferCoord.X,
           coordSrcBufferCoord.Y,
           psrctDestRect);
#endif


  /* verify psrctDestRect first */
  psrctDestRect->Left  = max(psrctDestRect->Left,  0);
  psrctDestRect->Top   = max(psrctDestRect->Top,   0);
  psrctDestRect->Right = min(psrctDestRect->Right, pConsoleBuffer->coordBufferSize.X - 1);
  psrctDestRect->Bottom= min(psrctDestRect->Bottom,pConsoleBuffer->coordBufferSize.Y - 1);

                                                     /* verify target buffer */
  if ( (coordSrcBufferSize.X < coordSrcBufferCoord.X) ||
       (coordSrcBufferSize.Y < coordSrcBufferCoord.Y) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);        /* set detailed error info */
    return (FALSE);                                            /* API failed */
  }

  ulCX = coordSrcBufferSize.X - coordSrcBufferCoord.X;
  ulCY = coordSrcBufferSize.Y - coordSrcBufferCoord.Y;

  ulCX = min(ulCX, (psrctDestRect->Right  - psrctDestRect->Left));
  ulCY = min(ulCY, (psrctDestRect->Bottom - psrctDestRect->Top));

                                  /* final calculation of the copy rectangle */
  psrctDestRect->Right  = psrctDestRect->Left + ulCX;
  psrctDestRect->Bottom = psrctDestRect->Top  + ulCY;


  for (ulY = 0,
       ulWriteY = psrctDestRect->Top;

       ulY <= ulCY;

       ulY++,
       ulWriteY++)
  {
    pchi = pchiSrcBuffer + sizeof(CHAR_INFO) * coordSrcBufferCoord.X
                          + sizeof(CHAR_INFO) * (coordSrcBufferCoord.Y + ulY)
                            * coordSrcBufferSize.X;

                                /* calculate pointer to start of screen line */
    pszTarget = pConsoleBuffer->ppszLine[ulWriteY] + psrctDestRect->Left;

    for (ulX = 0,
         ulWriteX = psrctDestRect->Left;

         ulX <= ulCX;

         ulX++,
         ulWriteX++,
         pchi++)
    {
                                            /* write character and attribute */
      *pszTarget++ = (UCHAR)pchi->Char.AsciiChar;
      *pszTarget++ = (UCHAR)pchi->Attributes;
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);                                            /* OK, that's it */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputW
 * Purpose   : write character and color attribute data to screen rectangle
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             PCHAR_INFO    pchiSrcBuffer
 *             COORD         coordSrcBufferSize
 *             COORD         coordSrcBufferCoord
 *             PSMALL_RECT   psrctDestRect
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputW(PHMHANDLEDATA pHMHandleData,
                                                      PCHAR_INFO    pchiSrcBuffer,
                                                      COORD         coordSrcBufferSize,
                                                      COORD         coordSrcBufferCoord,
                                                      PSMALL_RECT   psrctDestRect)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG ulX,      ulY;                      /* current coordinate to be read */
  ULONG ulCX,     ulCY;                   /* width and height of target area */
  ULONG ulWriteX, ulWriteY;                    /* position data is read from */
  WORD  wCell;                                        /* currently read data */

  PCHAR_INFO pchi;
  PSZ        pszTarget;

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputW(%08x,%08x,x=%u y=%u,x=%u y=%u, %08x).\n",
           pHMHandleData,
           pchiSrcBuffer,
           coordSrcBufferSize.X,
           coordSrcBufferSize.Y,
           coordSrcBufferCoord.X,
           coordSrcBufferCoord.Y,
           psrctDestRect);
#endif


  /* verify psrctDestRect first */
  psrctDestRect->Left  = max(psrctDestRect->Left,  0);
  psrctDestRect->Top   = max(psrctDestRect->Top,   0);
  psrctDestRect->Right = min(psrctDestRect->Right, pConsoleBuffer->coordBufferSize.X - 1);
  psrctDestRect->Bottom= min(psrctDestRect->Bottom,pConsoleBuffer->coordBufferSize.Y - 1);

                                                     /* verify target buffer */
  if ( (coordSrcBufferSize.X < coordSrcBufferCoord.X) ||
       (coordSrcBufferSize.Y < coordSrcBufferCoord.Y) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);        /* set detailed error info */
    return (FALSE);                                            /* API failed */
  }

  ulCX = coordSrcBufferSize.X - coordSrcBufferCoord.X;
  ulCY = coordSrcBufferSize.Y - coordSrcBufferCoord.Y;

  ulCX = min(ulCX, (psrctDestRect->Right  - psrctDestRect->Left));
  ulCY = min(ulCY, (psrctDestRect->Bottom - psrctDestRect->Top));

                                  /* final calculation of the copy rectangle */
  psrctDestRect->Right  = psrctDestRect->Left + ulCX;
  psrctDestRect->Bottom = psrctDestRect->Top  + ulCY;


  for (ulY = 0,
       ulWriteY = psrctDestRect->Top;

       ulY <= ulCY;

       ulY++,
       ulWriteY++)
  {
    pchi = pchiSrcBuffer + sizeof(CHAR_INFO) * coordSrcBufferCoord.X
                          + sizeof(CHAR_INFO) * (coordSrcBufferCoord.Y + ulY)
                            * coordSrcBufferSize.X;

                                /* calculate pointer to start of screen line */
    pszTarget = pConsoleBuffer->ppszLine[ulWriteY] + psrctDestRect->Left;

    for (ulX = 0,
         ulWriteX = psrctDestRect->Left;

         ulX <= ulCX;

         ulX++,
         ulWriteX++,
         pchi++)
    {
                                            /* write character and attribute */
      *pszTarget++ = (UCHAR)pchi->Char.UnicodeChar;  /* @@@PH unicode->ascii */
      *pszTarget++ = (UCHAR)pchi->Attributes;
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  return (TRUE);                                            /* OK, that's it */
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputAttribute
 * Purpose   : write an array with specified attributes to the console
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWORD        lpwAttribute
 *             DWORD         cWriteCells
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputAttribute(PHMHANDLEDATA pHMHandleData,
                                                              LPWORD        lpwAttribute,
                                                              DWORD         cWriteCells,
                                                              COORD         dwWriteCoord,
                                                              LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputAttribute(%08x,pattr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpwAttribute,


           cWriteCells,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpcWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }

                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the attribute lines */
  for (ulCounter = 0;
       ulCounter < cWriteCells;
       ulCounter++,
       lpwAttribute++)
  {
                                                /* write attribute into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2 + 1)
     ) = (UCHAR)*lpwAttribute;           /* write attribute and skip to next */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcWritten != NULL)                   /* ensure pointer is valid */
          *lpcWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
          ConsoleGlobals.fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpcWritten != NULL)                         /* ensure pointer is valid */
    *lpcWritten = cWriteCells;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputCharacterA
 * Purpose   : fills the console buffer with a specified attribute
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPTSTR        lpWriteBuffer
 *             DWORD         cchWrite
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputCharacterA(PHMHANDLEDATA pHMHandleData,
                                                               LPTSTR        lpWriteBuffer,
                                                               DWORD         cchWrite,
                                                               COORD         dwWriteCoord,
                                                               LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputCharacterA(%08x,pstr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpWriteBuffer,
           cchWrite,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpcWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }

                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the character lines */
  for (ulCounter = 0;
       ulCounter < cchWrite;
       ulCounter++,
       lpWriteBuffer++)
  {
                                                /* write character into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2)
     ) = (UCHAR)*lpWriteBuffer;          /* write character and skip to next */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcWritten != NULL)                   /* ensure pointer is valid */
          *lpcWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
          ConsoleGlobals.fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpcWritten != NULL)                         /* ensure pointer is valid */
    *lpcWritten = cchWrite;

  return (TRUE);
}


/*****************************************************************************
 * Name      : DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputCharacterW
 * Purpose   : fills the console buffer with a specified attribute
 * Parameters: PHMHANDLEDATA pHMHandleData - handle specific data
 *             LPWSTR        lpWriteBuffer
 *             DWORD         cchWrite
 *             COORD         dwWriteCoord
 *             LPDWORD       lpcWritten
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Wed, 1998/02/16 11:46]
 *****************************************************************************/

DWORD HMDeviceConsoleBufferClass::WriteConsoleOutputCharacterW(PHMHANDLEDATA pHMHandleData,
                                                               LPWSTR        lpWriteBuffer,
                                                               DWORD         cchWrite,
                                                               COORD         dwWriteCoord,
                                                               LPDWORD       lpcWritten)
{
  PCONSOLEBUFFER pConsoleBuffer = (PCONSOLEBUFFER)pHMHandleData->lpHandlerData;
  ULONG          ulCounter;                     /* current character counter */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: CONBUFFER$::WriteConsoleOutputCharacterW(%08x,pstr=%08x,%u,x=%u y=%u,res=%08x).\n",
           pHMHandleData,
           lpWriteBuffer,
           cchWrite,
           dwWriteCoord.X,
           dwWriteCoord.Y,
           lpcWritten);
#endif

  if ( (dwWriteCoord.X < 0) ||
       (dwWriteCoord.Y < 0) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }

                                    /* check if dwWriteCoord is within specs */
  if ( (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X) ||
       (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y) )
  {
    if (lpcWritten != NULL)                       /* ensure pointer is valid */
      *lpcWritten = 0;                            /* complete error handling */

    SetLastError(ERROR_INVALID_PARAMETER);
    return (FALSE);
  }


                                        /* OK, now write the character lines */
  for (ulCounter = 0;
       ulCounter < cchWrite;
       ulCounter++,
       lpWriteBuffer++)
  {
                                                /* write character into cell */
    *(pConsoleBuffer->ppszLine[dwWriteCoord.Y] +
                              (dwWriteCoord.X * 2)
     ) = (UCHAR)*lpWriteBuffer;          /* write character and skip to next */
                                       /* @@@PH unicode to ascii translation */

    dwWriteCoord.X++;                                 /* move write position */
    if (dwWriteCoord.X >= pConsoleBuffer->coordBufferSize.X)
    {
      dwWriteCoord.X = 0;                               /* skip to next line */
      dwWriteCoord.Y++;

                         /* oops, we're at the end of the buffer. Abort now. */
      if (dwWriteCoord.Y >= pConsoleBuffer->coordBufferSize.Y)
      {
        if (lpcWritten != NULL)                   /* ensure pointer is valid */
          *lpcWritten = ulCounter;

                                          /* update screen if active console */
        if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
          ConsoleGlobals.fUpdateRequired = TRUE;/* update with next WM_TIMER */

        return (TRUE);
      }
    }
  }

                                          /* update screen if active console */
  if (pHMHandleData->hHandle == ConsoleGlobals.hConsoleBuffer)
    ConsoleGlobals.fUpdateRequired = TRUE;      /* update with next WM_TIMER */

  if (lpcWritten != NULL)                         /* ensure pointer is valid */
    *lpcWritten = cchWrite;

  return (TRUE);
}

