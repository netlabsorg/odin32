/* $Id: console.cpp,v 1.28 2001-10-01 01:47:01 bird Exp $ */

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

//#undef DEBUG_LOCAL
//#undef DEBUG_LOCAL2


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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <process.h>
#include <stdlib.h>
#include <string.h>

#define  INCL_WIN
#define  INCL_DOSMEMMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#define  INCL_DOSPROCESS
#define  INCL_DOSMODULEMGR
#define  INCL_VIO
#define  INCL_KBD
#define  INCL_AVIO
#include <os2wrap.h>         //Odin32 OS/2 api wrappers

#include <win32type.h>
#include <win32api.h>
#include <misc.h>

#include "conwin.h"          // Windows Header for console only
#include "HandleManager.h"
#include "HMDevice.h"

#include "console.h"
#include "console2.h"
#include "conin.h"
#include "conout.h"
#include "conbuffer.h"
#include "conbuffervio.h"

#include "conprop.h"
#include "unicode.h"
#include "heapstring.h"

#define DBG_LOCALLOG    DBG_console
#include "dbglocal.h"

#include <os2sel.h>


/*******************************************************************************
*   Process Global Structures                                                  *
*******************************************************************************/
static ICONSOLEGLOBALS ConsoleGlobals;
static ICONSOLEINPUT   ConsoleInput;
BOOL                   flVioConsole = FALSE;

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void iConsoleInputQueueLock();
static void iConsoleInputQueueUnlock();


/*****************************************************************************
 * Name      : iConsoleInputQueueLock
 * Purpose   : lock the input queue to ensure correct event sequence
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

static void iConsoleInputQueueLock()
{
  APIRET rc;

  rc = DosRequestMutexSem(ConsoleInput.hmtxInputQueue,
                          SEM_INDEFINITE_WAIT);
  if (rc != NO_ERROR)
     dprintf(("KERNEL32: Console:iConsoleInputQueueLock error %08xh\n",
              rc));
}


/*****************************************************************************
 * Name      : iConsoleInputQueueUnlock
 * Purpose   : unlock the input queue
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

static void iConsoleInputQueueUnlock()
{
  APIRET rc;

  rc = DosReleaseMutexSem(ConsoleInput.hmtxInputQueue);
  if (rc != NO_ERROR)
     dprintf(("KERNEL32: Console:iConsoleInputQueueUnlock error %08xh\n",
              rc));
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

APIRET iConsoleInit(BOOL fVioConsole)              /* creation of the console subsystem */
{
  APIRET rc;                                       /* API return code */
  ULONG  ulPostCount;                              /* semaphore post counter */


  flVioConsole = fVioConsole;

  if (ConsoleGlobals.hevConsole != NULLHANDLE) /* we're already initialized ?*/
    return (NO_ERROR);                             /* then abort immediately */

  if(flVioConsole == TRUE)
  {
    /***************************************************************************
    * Create pseudo-devices and initialize ConsoleGlobals                     *
    ***************************************************************************/

    rc = iConsoleDevicesRegister();                /* ensure devices are there */
    if (rc != NO_ERROR)                                    /* check for errors */
    {
        return (rc);                                    /* raise error condition */
    }
    rc = DosCreateMutexSem(NULL,
                             &ConsoleInput.hmtxInputQueue,
                             0L,
                             FALSE);
    if (rc != NO_ERROR)                                       /* other error ? */
    {
        return (rc);                                    /* raise error condition */
    }

    return NO_ERROR;
  }
  else {
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


    rc = DosCreateMutexSem(NULL,
                             &ConsoleInput.hmtxInputQueue,
                             0L,
                             FALSE);
    if (rc != NO_ERROR)                                       /* other error ? */
    {
        DosCloseEventSem(ConsoleGlobals.hevConsole);    /* close other semaphore */
        DosCloseEventSem(ConsoleInput.hevInputQueue);   /* close other semaphore */
        return (rc);                                    /* raise error condition */
    }

    /***************************************************************************
    * Create pseudo-devices and initialize ConsoleGlobals                     *
    ***************************************************************************/

    rc = iConsoleDevicesRegister();                /* ensure devices are there */
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
    ConsoleGlobals.tidConsole = _beginthread(iConsoleMsgThread,
                                               NULL,
                                               12288,
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

APIRET iConsoleDevicesRegister(void)
{
  DWORD  dwType;                                       /* device handle type */
  HANDLE hStandardIn;                              /* stdin handle to CONIN$ */
  HANDLE hStandardOut;                           /* stdout handle to CONOUT$ */
  HANDLE hStandardError;                         /* stderr handle to CONOUT$ */

  HMDeviceConsoleInClass     *pHMDeviceConsoleIn;
  HMDeviceConsoleOutClass    *pHMDeviceConsoleOut;
  HMDeviceConsoleBufferClass *pHMDeviceConsoleBuffer;

  DWORD rc;

  static BOOL fDevicesInitialized;     /* have we been initialized already ? */

  if (fDevicesInitialized == TRUE)                 /* OK, we're already done */
    return (NO_ERROR);
  else
    fDevicesInitialized = TRUE;

  dprintf(("KERNEL32:ConsoleDevicesRegister\n"));


  /*************************************
   * Initialize Console Window Options *
   *************************************/

  // load defaults
  ConsolePropertyDefault(&ConsoleGlobals.Options);

  ConsoleGlobals.ulTimerFrequency = 10;  /* cursor + blitter timer frequency */

  ConsoleGlobals.coordWindowSize.X               = ConsoleGlobals.Options.coordDefaultSize.X;
  ConsoleGlobals.coordWindowSize.Y               = ConsoleGlobals.Options.coordDefaultSize.Y;
  ConsoleGlobals.coordWindowPos.X                = 0;
  ConsoleGlobals.coordWindowPos.Y                = 0;
  if(flVioConsole == TRUE) {
        VIOMODEINFO videoinfo;

        videoinfo.cb = sizeof(VIOMODEINFO);
        rc = VioGetMode(&videoinfo, 0);
        if(rc == 0) {
            dprintf(("video mode (%d,%d)", videoinfo.col, videoinfo.row));
            ConsoleGlobals.coordWindowSize.X = videoinfo.col;
            ConsoleGlobals.coordWindowSize.Y = videoinfo.row;
        }
  }


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
  ConsoleGlobals.pszWindowTitle = strdup(GetCommandLineA());

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


  if (flVioConsole == FALSE)
  {
    // defaults are effective, try to read and apply stored properties
    if (ConsolePropertyLoad(&ConsoleGlobals.Options) == NO_ERROR)
        ConsolePropertyApply(&ConsoleGlobals.Options);
  }


  /***************************************************************************
   * Standard handles     Initialization phase                               *
   ***************************************************************************/

  /* create devices and register devices with handlemanager */
  pHMDeviceConsoleIn  = new HMDeviceConsoleInClass("CONIN$",
                                                   &ConsoleInput,
                                                   &ConsoleGlobals);

  rc = HMDeviceRegister ("CONIN$",
                         pHMDeviceConsoleIn);
  if (rc != NO_ERROR)                                  /* check for errors */
      dprintf(("KERNEL32:ConsoleDevicesRegister: registering CONIN$ failed with %u.\n",
               rc));


  pHMDeviceConsoleOut = new HMDeviceConsoleOutClass("CONOUT$",
                                                    &ConsoleInput,
                                                    &ConsoleGlobals);
  rc = HMDeviceRegister ("CONOUT$",
                         pHMDeviceConsoleOut);
  if (rc != NO_ERROR)                                  /* check for errors */
      dprintf(("KERNEL32:ConsoleDevicesRegister: registering CONOUT$ failed with %u.\n",
               rc));

  if(flVioConsole == TRUE)
  {
        pHMDeviceConsoleBuffer = (HMDeviceConsoleBufferClass *)new HMDeviceConsoleVioBufferClass("CONBUFFER$",
                                                                                                 &ConsoleInput,
                                                                                                 &ConsoleGlobals);
  }
  else {
        pHMDeviceConsoleBuffer = new HMDeviceConsoleBufferClass("CONBUFFER$",
                                                                &ConsoleInput,
                                                                &ConsoleGlobals);
  }
  rc = HMDeviceRegister ("CONBUFFER$",
                         pHMDeviceConsoleBuffer);
  if (rc != NO_ERROR)                                  /* check for errors */
      dprintf(("KERNEL32:ConsoleDevicesRegister: registering CONBUFFER$ failed with %u.\n",
               rc));


  if(flVioConsole == FALSE)
  {
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
  }

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

ULONG iConsoleTerminate(VOID)
{
  APIRET rc = NO_ERROR;

  if(flVioConsole == FALSE)
  {
    DosCloseEventSem(ConsoleGlobals.hevConsole);      /* close other semaphore */
    DosCloseEventSem(ConsoleInput.hevInputQueue);     /* close other semaphore */


    WinPostMsg (ConsoleGlobals.hwndFrame,         /* force thread to terminate */
                WM_CLOSE,
                (MPARAM)NULL,
                (MPARAM)NULL);

    rc = DosWaitThread(&ConsoleGlobals.tidConsole,/* wait until thd terminates */
                       DCWW_WAIT);

  }
  DosCloseMutexSem(ConsoleInput.hmtxInputQueue);          /* close semaphore */

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

void iConsoleWaitClose(void)
{
  CHAR szBuffer[128];                                /* buffer for the title */
  BOOL fResult;                /* result from subsequent calls to Win32 APIs */

                                /* check if there is a console window at all */
  if (iConsoleIsActive() == FALSE)
    return;                                                          /* nope */

  if(flVioConsole == TRUE)  //no need to wait for VIO session
    return;

  strcpy (szBuffer,               /* indicate console process has terminated */
          "Completed: ");

  fResult = GetConsoleTitleA(szBuffer + 11,/* 11 is length of Completed:_ */
                                sizeof(szBuffer) - 11);


                                /* Set new title: Win32 Console - Terminated */
  fResult = SetConsoleTitleA(szBuffer);

  DosCloseEventSem(ConsoleGlobals.hevConsole);      /* close other semaphore */
  DosCloseEventSem(ConsoleInput.hevInputQueue);     /* close other semaphore */
  DosCloseMutexSem(ConsoleInput.hmtxInputQueue);          /* close semaphore */


                                          /* terminate console immediately ? */
  if (ConsoleGlobals.Options.fTerminateAutomatically == FALSE) {
    if(getenv("ODIN_AUTOEXITCONSOLE") == NULL) {
        DosWaitThread(&ConsoleGlobals.tidConsole,   /* wait until thd terminates */
                      DCWW_WAIT);
    }
  }
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

BOOL iConsoleIsActive(void)
{
  if(flVioConsole == TRUE) //actually, this isn't needed in VIO mode
     return TRUE;

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

VOID iConsoleMsgThread(PVOID pParameters)
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
                           iConsoleWindowProc,
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
  DosCloseMutexSem(ConsoleInput.hmtxInputQueue);          /* close semaphore */

  ConsoleGlobals.hevConsole   = NULLHANDLE;         /* for ConsoleIsActive() */
  ConsoleInput.hevInputQueue  = NULLHANDLE;
  ConsoleInput.hmtxInputQueue = NULLHANDLE;

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

MRESULT EXPENTRY iConsoleWindowProc(HWND   hwnd,
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
                                                            iConsoleFrameWindowProc);

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

      iConsoleFontQuery();                       /* query current cell sizes */

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
                       ConsoleGlobals.Options.coordDefaultSize.X *
                         ConsoleGlobals.sCellCX +
                         2 * WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER),
                       ConsoleGlobals.Options.coordDefaultSize.Y *
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
          switch (ulResult)
          {
            case ID_BTN_SAVE:
              ConsolePropertySave(&ConsoleGlobals.Options);
              ConsolePropertyApply(&ConsoleGlobals.Options);
              break;

            case ID_BTN_APPLY:
              ConsolePropertyApply(&ConsoleGlobals.Options);
              break;

            default:           break;
          }

          return (MPARAM) FALSE;
        }
      }
    }
    break;


    /*************************************************************************
     * WM_CHAR keyboard char processing                                      *
     *************************************************************************/

    case WM_CHAR:
      iConsoleInputEventPushKey(mp1,                /* push event into queue */
                                mp2);
      break;                                  /* enable further processing ! */


    /*************************************************************************
     * WM_SETFOCUS focus changing processing                                 *
     *************************************************************************/

    case WM_SETFOCUS:
      iConsoleInputEventPushFocus((BOOL)mp2);       /* push event into queue */
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
      iConsoleInputEventPushMouse(msg,
                                  mp1,              /* push event into queue */
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

MRESULT EXPENTRY iConsoleFrameWindowProc(HWND   hwnd,
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

void iConsoleBufferMap(PCONSOLEBUFFER pConsoleBuffer)
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

void iConsoleBufferFillLine(ULONG   ulPattern,
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

void iConsoleBufferScrollUp(PCONSOLEBUFFER pConsoleBuffer,
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
    iConsoleBufferFillLine(ulPosition,
                           (PUSHORT)(pConsoleBuffer->ppszLine[ulLine]),
                           pConsoleBuffer->coordBufferSize.X);

    /* this code ensures frequent screen updating, even if the timer prooves */
                                                            /* to be to slow */
  ulUpdateCounter++;
  if (ulUpdateCounter > ConsoleGlobals.Options.ulUpdateLimit)
  {
    ulUpdateCounter = 0;                                /* reset the counter */
    iConsoleBufferMap(pConsoleBuffer);
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

APIRET iConsoleInputEventPush(PINPUT_RECORD pInputRecord)
{
  PINPUT_RECORD pirFree;                           /* pointer to free record */
  APIRET        rc;                                        /* API-returncode */

#ifdef DEBUG_LOCAL2
  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPush(%08x).\n",
           pInputRecord));
#endif

  iConsoleInputQueueLock();
                                                           /* get free event */
  pirFree = &ConsoleInput.arrInputRecord[ConsoleInput.ulIndexFree];
  if (pirFree->EventType != 0x0000)
  {
    iConsoleInputQueueUnlock();
    return (ERROR_QUE_NO_MEMORY);                         /* queue is full ! */
  }
                                                       /* put event in queue */

  ConsoleInput.ulIndexFree++;                        /* update index counter */
  if (ConsoleInput.ulIndexFree >= CONSOLE_INPUTQUEUESIZE)
    ConsoleInput.ulIndexFree = 0;

  ConsoleInput.ulEvents++;                   /* increate queue event counter */

  iConsoleInputQueueUnlock();

  memcpy(pirFree,                                               /* copy data */
         pInputRecord,
         sizeof (INPUT_RECORD) );

  if(flVioConsole == FALSE)
  {     /* unblock reading threads */
        rc = DosPostEventSem(ConsoleInput.hevInputQueue);
  }
  else  rc = 0;
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

APIRET iConsoleInputEventPop(PINPUT_RECORD pInputRecord)
{
  PINPUT_RECORD pirEvent;                         /* pointer to event record */
  APIRET        rc;                                        /* API-returncode */

#ifdef DEBUG_LOCAL2
  dprintf(("KERNEL32/CONSOLE:ConsoleInputEventPop(%08x).\n",
           pInputRecord));
#endif

  if (ConsoleInput.ulEvents == 0)                         /* empty console ? */
    return (ERROR_QUE_EMPTY);                            /* queue is empty ! */

  iConsoleInputQueueLock();
                                                          /* get first event */
  pirEvent = &ConsoleInput.arrInputRecord[ConsoleInput.ulIndexEvent];
  if (pirEvent->EventType == 0x0000)
  {
    iConsoleInputQueueUnlock();
    return (ERROR_QUE_EMPTY);                            /* queue is empty ! */
  }

  if (ConsoleInput.ulEvents >= 0)       /* decrease number of console events */
    ConsoleInput.ulEvents--;

  ConsoleInput.ulIndexEvent++;                       /* update index counter */
  if (ConsoleInput.ulIndexEvent >= CONSOLE_INPUTQUEUESIZE)
    ConsoleInput.ulIndexEvent = 0;

                                                       /* put event in queue */
  memcpy(pInputRecord,                                          /* copy data */
         pirEvent,
         sizeof (INPUT_RECORD) );

  pirEvent->EventType = 0x0000;                 /* mark event as read = free */

  iConsoleInputQueueUnlock();

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

char tabVirtualKeyCodes[TABVIRTUALKEYCODES] =
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


APIRET iConsoleInputEventPushKey(MPARAM mp1,
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

  rc = iConsoleInputEventPush(&InputRecord);          /* add it to the queue */
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

APIRET iConsoleInputEventPushMouse(ULONG  ulMessage,
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

  rc = iConsoleInputEventPush(&InputRecord);          /* add it to the queue */
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

APIRET iConsoleInputEventPushWindow(COORD coordWindowSize)
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

  rc = iConsoleInputEventPush(&InputRecord);          /* add it to the queue */
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

APIRET iConsoleInputEventPushMenu(DWORD dwCommandId)
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

  rc = iConsoleInputEventPush(&InputRecord);          /* add it to the queue */
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

APIRET iConsoleInputEventPushFocus(BOOL bSetFocus)
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

  rc = iConsoleInputEventPush(&InputRecord);          /* add it to the queue */
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

ULONG iConsoleInputQueryEvents(PICONSOLEINPUT pConsoleInput, int fWait)
{
 ULONG ulPostCounter;                   /* semaphore post counter - ignored */
 APIRET rc;                                               /* API returncode */

  if(flVioConsole)
  {
    KBDKEYINFO keyinfo;

        rc = KbdCharIn(&keyinfo, IO_NOWAIT, 0); //grab key if present; don't wait
        if((rc || !(keyinfo.fbStatus & 0x40)) && fWait == QUERY_EVENT_WAIT && ConsoleInput.ulEvents == 0) {
                rc = KbdCharIn(&keyinfo, IO_WAIT, 0);
        }
        while(rc == 0 && (keyinfo.fbStatus & 0x40))
        {
                INPUT_RECORD InputRecord = {0};

                InputRecord.EventType = KEY_EVENT;
                InputRecord.Event.KeyEvent.wRepeatCount = 1;
                InputRecord.Event.KeyEvent.bKeyDown     = 1;
                InputRecord.Event.KeyEvent.dwControlKeyState = 0;

                if(keyinfo.fbStatus & 2)
                        InputRecord.Event.KeyEvent.dwControlKeyState |= ENHANCED_KEY;

                if(keyinfo.fsState & (KBDSTF_RIGHTSHIFT|KBDSTF_LEFTSHIFT))
                        InputRecord.Event.KeyEvent.dwControlKeyState |= SHIFT_PRESSED;

                if(keyinfo.fsState & KBDSTF_LEFTALT)
                        InputRecord.Event.KeyEvent.dwControlKeyState |= LEFT_ALT_PRESSED;

                if(keyinfo.fsState & KBDSTF_RIGHTALT)
                        InputRecord.Event.KeyEvent.dwControlKeyState |= RIGHT_ALT_PRESSED;

                if(keyinfo.fsState & KBDSTF_LEFTCONTROL)
                    InputRecord.Event.KeyEvent.dwControlKeyState |= LEFT_CTRL_PRESSED;

                if(keyinfo.fsState & KBDSTF_RIGHTCONTROL)
                    InputRecord.Event.KeyEvent.dwControlKeyState |= RIGHT_CTRL_PRESSED;

                if(keyinfo.fsState & KBDSTF_CAPSLOCK_ON)
                    InputRecord.Event.KeyEvent.dwControlKeyState |= CAPSLOCK_ON;

                if(keyinfo.fsState & KBDSTF_SCROLLLOCK_ON)
                    InputRecord.Event.KeyEvent.dwControlKeyState |= SCROLLLOCK_ON;

                if(keyinfo.fsState & KBDSTF_NUMLOCK_ON)
                    InputRecord.Event.KeyEvent.dwControlKeyState |= NUMLOCK_ON;

                InputRecord.Event.KeyEvent.wVirtualKeyCode  = 0;
                InputRecord.Event.KeyEvent.wVirtualScanCode = keyinfo.chScan;
                InputRecord.Event.KeyEvent.uChar.AsciiChar  = keyinfo.chChar;

                rc = iConsoleInputEventPush(&InputRecord);          /* add it to the queue */
                if(rc) {
                    dprintf(("WARNING: lost key!!"));
                    break;
                }

                rc = KbdCharIn(&keyinfo, IO_NOWAIT, 0); //grab key if present; don't wait
        }
  }
  else
  if(fWait == QUERY_EVENT_WAIT && ConsoleInput.ulEvents == 0)
  {
        rc = DosWaitEventSem(pConsoleInput->hevInputQueue,      /* wait for input */
                             SEM_INDEFINITE_WAIT);
        DosResetEventSem(pConsoleInput->hevInputQueue,         /* reset semaphore */
                         &ulPostCounter);              /* post counter - ignored */
  }
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

void iConsoleCursorShow (PCONSOLEBUFFER pConsoleBuffer,
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

  //@@@PH top calculation is wrong!
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

APIRET iConsoleFontQuery (void)
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

void iConsoleAdjustWindow (PCONSOLEBUFFER pConsoleBuffer)
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

  rc = iConsoleInit(flVioConsole);               /* initialize subsystem if required */
  if (rc != NO_ERROR)                            /* check for errors */
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
                         (LPSECURITY_ATTRIBUTES)lpSecurityAttributes,
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

  rc = iConsoleTerminate();               /* terminate subsystem if required */
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
  lpConsoleTitle[nSize-1] = 0;

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

DWORD WIN32API GetConsoleTitleW(LPWSTR lpConsoleTitle,
                                DWORD  nSize)
{
  ULONG ulLength;                                          /* length of text */

#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: GetConsoleTitleW(%08x,%08x)",
           lpConsoleTitle,
           nSize);
#endif

  if (ConsoleGlobals.pszWindowTitle == NULL)    /* is there a window title ? */
    return 0;                                           /* abort immediately */

  ulLength = strlen(ConsoleGlobals.pszWindowTitle);        /* length of text */

  lstrcpynAtoW(lpConsoleTitle,
               ConsoleGlobals.pszWindowTitle,
               nSize);

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

BOOL WIN32API SetConsoleTitleW(LPWSTR lpszTitle)
{
#ifdef DEBUG_LOCAL2
  WriteLog("KERNEL32/CONSOLE: OS2SetConsoleTitleW(%s) not implemented.\n",
           lpszTitle);
#endif

  if (lpszTitle == NULL)                                 /* check parameters */
    return FALSE;

  if (ConsoleGlobals.pszWindowTitle != NULL)           /* previously set name */
    free (ConsoleGlobals.pszWindowTitle);                     /* then free it */

  /* create an ascii copy of the lpszTitle */
  int iLength = lstrlenW(lpszTitle);

  ConsoleGlobals.pszWindowTitle = (PSZ)malloc(iLength+1);
  ConsoleGlobals.pszWindowTitle[iLength] = 0;
  lstrcpynWtoA(ConsoleGlobals.pszWindowTitle,
               lpszTitle,
               iLength+1); //must add one (lstrcpynWtoA terminates string)

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
