/* $Id: pmwindow.cpp,v 1.230 2004-05-24 09:01:59 sandervl Exp $ */
/*
 * Win32 Window Managment Code for OS/2
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_WIN
#define INCL_GPI
#define INCL_DEV                /* Device Function definitions  */
#define INCL_GPICONTROL         /* GPI control Functions        */
#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_WINTRACKRECT
#define INCL_BASE

#include <os2wrap.h>
#include <odinwrap.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <winuser32.h>
#include <wprocess.h>
#include <dbglog.h>
#include <win32wbase.h>
#include <win32wfake.h>
#include <win32dlg.h>
#include "win32wdesktop.h"
#include "pmwindow.h"
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibmsg.h"
#define INCLUDED_BY_DC
#include "dc.h"
#include <thread.h>
#include <wprocess.h>
#include <objhandle.h>
#include "caret.h"
#include "timer.h"
#include <codepage.h>
#include "syscolor.h"
#include "options.h"
#include "menu.h"
#include <pmkbdhk.h>
#include <pmscan.h>
#include <winscan.h>
#include <oslibdnd.h>
#include <custombuild.h>
#include <win\dbt.h>
#include "dragdrop.h"
#include "menu.h"
#include "user32api.h"
#include <kbdhook.h>
#include <heapstring.h>

#include <os2im.h>
#include <im32.h>

#define DBG_LOCALLOG    DBG_pmwindow
#include "dbglocal.h"


// Notification that focus change has completed (UNDOCUMENTED)
#define WM_FOCUSCHANGED            0x000e

//define this to use the new code for WM_CALCVALIDRECT handling
//#define USE_CALCVALIDRECT

HMQ     hmq = 0;                             /* Message queue handle         */
HAB     hab = 0;
RECTL   desktopRectl = {0};
ULONG   ScreenWidth  = 0;
ULONG   ScreenHeight = 0;
ULONG   ScreenBitsPerPel = 0;
BOOL    fOS2Look = FALSE;
BOOL    fForceMonoCursor = FALSE;
BOOL    fDragDropActive = FALSE;
BOOL    fDragDropDisabled = FALSE;

const char WIN32_CDCLASS[]       = ODIN_WIN32_CDCLASS;
      char WIN32_STDCLASS[255]   = ODIN_WIN32_STDCLASS;

#define PMMENU_MINBUTTON           0
#define PMMENU_MAXBUTTON           1
#define PMMENU_RESTOREBUTTON       2
#define PMMENU_CLOSEBUTTON         3
#define PMMENU_MINBUTTONDOWN       4
#define PMMENU_MAXBUTTONDOWN       5
#define PMMENU_RESTOREBUTTONDOWN   6
#define PMMENU_CLOSEBUTTONDOWN     7

HBITMAP hbmFrameMenu[8] = {0};

//Win32 bitmap handles of the OS/2 min, max and restore buttons
HBITMAP hBmpMinButton     = 0;
HBITMAP hBmpMaxButton     = 0;
HBITMAP hBmpRestoreButton = 0;
HBITMAP hBmpCloseButton   = 0;
HBITMAP hBmpMinButtonDown     = 0;
HBITMAP hBmpMaxButtonDown     = 0;
HBITMAP hBmpRestoreButtonDown = 0;
HBITMAP hBmpCloseButtonDown   = 0;

       PFNWP pfnFrameWndProc = NULL;
static HWND  hwndFocusChange = 0;
       HWND  hwndCD = 0;

// this holds the font height that the display driver returns using DevQueryCaps
// 13 would be small fonts, 16 medium fonts and 20 large fonts
LONG CapsCharHeight = 0;

// Note:
// For a "lonekey"-press of AltGr, we only receive WM_KEYUP
// messages. If the key is pressed longer and starts to repeat,
// WM_KEYDOWN messages come in properly.
static BOOL fKeyAltGrDown = FALSE;
static BOOL fEnableCDPolling = FALSE;

static char *PMDragExtractFiles(PDRAGINFO pDragInfo, ULONG *pcItems, ULONG *pulBytes);
static BOOL  PMDragValidate(PDRAGINFO pDragInfo);
static void  QueryPMMenuBitmaps();

MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY Win32CDWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY Win32FrameWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void FrameReplaceMenuItem(HWND hwndMenu, ULONG nIndex, ULONG idOld, ULONG   idNew,
                          HBITMAP hbmNew);
void FrameSetFocus(HWND hwnd);

VOID APIENTRY DspInitSystemDriverName(PSZ pszDriverName, ULONG lenDriverName);

#ifdef DEBUG_LOGGING
static char *DbgGetStringSWPFlags(ULONG flags);
static char *DbgPrintQFCFlags(ULONG flags);
#endif

extern "C" ULONG OSLibImSetMsgQueueProperty( ULONG, ULONG );

//******************************************************************************
// Initialize PM; create hab, message queue and register special Win32 window classes
//
// This is called from the initterm, so we call it only once for each process.
// We make sure PM is up and running for our purposes and init the existing
// thread 0.
//******************************************************************************
BOOL InitPM()
{
    hab = WinInitialize(0);
    dprintf(("Winitialize returned %x", hab));
    hmq = WinCreateMsgQueue(hab, 0);

    if(!hab || !hmq)
    {
        UINT error;
        //CB: only fail on real error
        error = WinGetLastError(hab) & 0xFFFF; //error code
        if (!hab || (error != PMERR_MSG_QUEUE_ALREADY_EXISTS))
        {
            dprintf(("WinInitialize or WinCreateMsgQueue failed %x %x", hab, hmq));
            dprintf((" Error = %x",error));
            if(error == PMERR_NOT_IN_A_PM_SESSION) return TRUE;

            return(FALSE);
        }
        else
        {
            if(!hab) {
                hab = WinQueryAnchorBlock(HWND_DESKTOP);
                dprintf(("WinQueryAnchorBlock returned %x", hab));
            }
            if(!hmq) {
                PTIB ptib;
                PPIB ppib;

                DosGetInfoBlocks(&ptib, &ppib);

                hmq = WinQueueFromID(hab, ppib->pib_ulpid, ptib->tib_ptib2->tib2_ultid);
            }
        }
    }

    // store our HAB and HMQ in the TEB - we need it quite often
    // and they don't map 1:1 to Windows entities
    SetThreadHAB(hab);
    dprintf(("InitPM: hmq = %x", hmq));
    SetThreadMessageQueue(hmq);

    //initialize keyboard hook for first thread
    hookInit(hab);

    BOOL rc = WinSetCp(hmq, GetDisplayCodepage());
    dprintf(("InitPM: WinSetCP was %sOK", rc ? "" : "not "));

    /* IM instace is created per message queue, that is, thread */
    if( IsDBCSEnv())
        OSLibImSetMsgQueueProperty( hmq, MQP_INSTANCE_PERMQ );

    //CD polling window class
    if(!WinRegisterClass(                    /* Register window class        */
        hab,                                 /* Anchor block handle          */
        (PSZ)WIN32_CDCLASS,                  /* Window class name            */
        (PFNWP)Win32CDWindowProc,            /* Address of window procedure  */
        0,
        0))
    {
            dprintf(("WinRegisterClass Win32BaseWindow failed"));
            return(FALSE);
    }

    //Standard Odin window class
    if(!WinRegisterClass(                 /* Register window class        */
        hab,                               /* Anchor block handle          */
        (PSZ)WIN32_STDCLASS,               /* Window class name            */
        (PFNWP)Win32WindowProc,            /* Address of window procedure  */
        0,
        NROF_WIN32WNDBYTES))
    {
            dprintf(("WinRegisterClass Win32BaseWindow failed"));
            return(FALSE);
    }

    //We no longer register our own frame window class as there is code in PM
    //that makes assumptions about frame window class names.
    //Instead we subclass the frame window right after creating it.
    CLASSINFO FrameClassInfo;
    if(!WinQueryClassInfo (hab, WC_FRAME, &FrameClassInfo)) {
        dprintf (("WinQueryClassInfo WC_FRAME failed"));
        return (FALSE);
    }
    pfnFrameWndProc = FrameClassInfo.pfnWindowProc;

    dprintf(("WC_FRAME style %x", FrameClassInfo.flClassStyle));

    // get the screen dimensions and store them
    WinQueryWindowRect(HWND_DESKTOP, &desktopRectl);
    ScreenWidth  = desktopRectl.xRight;
    ScreenHeight = desktopRectl.yTop;

    HDC   hdc;              /* Device-context handle                */
    /* context data structure */
    DEVOPENSTRUC dop = {NULL, "DISPLAY", NULL, NULL, NULL, NULL,
                        NULL, NULL, NULL};

    /* create memory device context - it's temporary to query some information */
    hdc = DevOpenDC(hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE);

    // check if we have the OS/2 Look and Feel enabled
    fOS2Look = PROFILE_GetOdinIniBool(ODINSYSTEM_SECTION, "OS2Look", TRUE);
    if(fOS2Look)
    {
        SYSCOLOR_Init(FALSE); //use OS/2 colors
        QueryPMMenuBitmaps();
    }

    // find out which colordepth we're running
    DevQueryCaps(hdc, CAPS_COLOR_BITCOUNT, 1, (PLONG)&ScreenBitsPerPel);

    // query the font height to find out whether we have small or large fonts
    DevQueryCaps(hdc, CAPS_GRAPHICS_CHAR_HEIGHT, 1, (PLONG)&CapsCharHeight);
    dprintf(("CAPS_GRAPHICS_CHAR_HEIGHT = %d", CapsCharHeight));
    if(CapsCharHeight > 16) {
       CapsCharHeight = 16;
    }

#ifdef DEBUG
    ULONG temp;
    DevQueryCaps(hdc, CAPS_GRAPHICS_CHAR_WIDTH, 1, (PLONG)&temp);
    dprintf(("CAPS_GRAPHICS_CHAR_WIDTH = %d", temp));
    DevQueryCaps(hdc, CAPS_CHAR_HEIGHT, 1, (PLONG)&temp);
    dprintf(("CAPS_CHAR_HEIGTH = %d", temp));
    DevQueryCaps(hdc, CAPS_CHAR_WIDTH, 1, (PLONG)&temp);
    dprintf(("CAPS_CHAR_WIDTH = %d", temp));
    DevQueryCaps(hdc, CAPS_SMALL_CHAR_HEIGHT, 1, (PLONG)&temp);
    dprintf(("CAPS_SMALL_CHAR_HEIGTH = %d", temp));
    DevQueryCaps(hdc, CAPS_SMALL_CHAR_WIDTH, 1, (PLONG)&temp);
    dprintf(("CAPS_SMALL_CHAR_WIDTH = %d", temp));
    DevQueryCaps(hdc, CAPS_VERTICAL_FONT_RES, 1,(PLONG)&temp);
    dprintf(("CAPS_VERTICAL_FONT_RES = %d", temp));
    DevQueryCaps(hdc, CAPS_HORIZONTAL_FONT_RES, 1,(PLONG)&temp);
    dprintf(("CAPS_HORIZONTAL_FONT_RES = %d", temp));
#endif

    DevCloseDC(hdc);

    dprintf(("InitPM: Desktop (%d,%d) bpp %d font size %d", ScreenWidth, ScreenHeight, ScreenBitsPerPel, CapsCharHeight));
    return TRUE;
} /* End of main */
//******************************************************************************
HBITMAP OPEN32API _O32_CreateBitmapFromPMHandle(HBITMAP hPMBitmap);

inline HBITMAP O32_CreateBitmapFromPMHandle(HBITMAP hPMBitmap)
{
 HBITMAP yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_CreateBitmapFromPMHandle(hPMBitmap);
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
static void QueryPMMenuBitmaps()
{
    CHAR szDisplay[30];
    HMODULE hModDisplay;

    if(hbmFrameMenu[0] == 0)
    {
        CHAR szDisplay[30];
        HMODULE hModDisplay;
        HDC   hdc;              /* Device-context handle                */
        DEVOPENSTRUC dop = {NULL, "DISPLAY", NULL, NULL, NULL, NULL,
                            NULL, NULL, NULL};

        /* create memory device context */
        hdc = DevOpenDC(hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE);

        DspInitSystemDriverName(szDisplay, sizeof(szDisplay));
        DosQueryModuleHandle(szDisplay, &hModDisplay);

        hbmFrameMenu[PMMENU_MINBUTTON] = GpiLoadBitmap(hdc, hModDisplay, SBMP_MINBUTTON, 0, 0);
        hbmFrameMenu[PMMENU_MINBUTTONDOWN] = GpiLoadBitmap(hdc, hModDisplay, SBMP_MINBUTTONDEP, 0, 0);
        hbmFrameMenu[PMMENU_MAXBUTTON] = GpiLoadBitmap(hdc, hModDisplay, SBMP_MAXBUTTON, 0, 0);
        hbmFrameMenu[PMMENU_MAXBUTTONDOWN] = GpiLoadBitmap(hdc, hModDisplay, SBMP_MAXBUTTONDEP, 0, 0);
        hbmFrameMenu[PMMENU_RESTOREBUTTON] = GpiLoadBitmap(hdc, hModDisplay, SBMP_RESTOREBUTTON, 0, 0);
        hbmFrameMenu[PMMENU_RESTOREBUTTONDOWN] = GpiLoadBitmap(hdc, hModDisplay, SBMP_RESTOREBUTTONDEP, 0, 0);
        hbmFrameMenu[PMMENU_CLOSEBUTTON] = GpiLoadBitmap(hdc, hModDisplay, SBMP_CLOSE, 0, 0);
        hbmFrameMenu[PMMENU_CLOSEBUTTONDOWN] = GpiLoadBitmap(hdc, hModDisplay, SBMP_CLOSEDEP, 0, 0);

        //Create win32 bitmap handles of the OS/2 min, max and restore buttons
        hBmpMinButton     = O32_CreateBitmapFromPMHandle(hbmFrameMenu[PMMENU_MINBUTTON]);
        ObjSetHandleFlag(hBmpMinButton, OBJHANDLE_FLAG_NODELETE, 1);
        hBmpMinButtonDown = O32_CreateBitmapFromPMHandle(hbmFrameMenu[PMMENU_MINBUTTONDOWN]);
        ObjSetHandleFlag(hBmpMinButtonDown, OBJHANDLE_FLAG_NODELETE, 1);
        hBmpMaxButton     = O32_CreateBitmapFromPMHandle(hbmFrameMenu[PMMENU_MAXBUTTON]);
        ObjSetHandleFlag(hBmpMaxButton, OBJHANDLE_FLAG_NODELETE, 1);
        hBmpMaxButtonDown = O32_CreateBitmapFromPMHandle(hbmFrameMenu[PMMENU_MAXBUTTONDOWN]);
        ObjSetHandleFlag(hBmpMaxButtonDown, OBJHANDLE_FLAG_NODELETE, 1);
        hBmpRestoreButton = O32_CreateBitmapFromPMHandle(hbmFrameMenu[PMMENU_RESTOREBUTTON]);
        ObjSetHandleFlag(hBmpRestoreButton, OBJHANDLE_FLAG_NODELETE, 1);
        hBmpRestoreButtonDown = O32_CreateBitmapFromPMHandle(hbmFrameMenu[PMMENU_RESTOREBUTTONDOWN]);
        ObjSetHandleFlag(hBmpRestoreButtonDown, OBJHANDLE_FLAG_NODELETE, 1);
        hBmpCloseButton   = O32_CreateBitmapFromPMHandle(hbmFrameMenu[PMMENU_CLOSEBUTTON]);
        ObjSetHandleFlag(hBmpCloseButton, OBJHANDLE_FLAG_NODELETE, 1);
        hBmpCloseButtonDown   = O32_CreateBitmapFromPMHandle(hbmFrameMenu[PMMENU_CLOSEBUTTONDOWN]);
        ObjSetHandleFlag(hBmpCloseButtonDown, OBJHANDLE_FLAG_NODELETE, 1);
        DevCloseDC(hdc);
    }
}
//******************************************************************************
//******************************************************************************
void WIN32API SetWindowAppearance(int fLooks)
{
    if(fLooks == OS2_APPEARANCE || fLooks == OS2_APPEARANCE_SYSMENU)
    {
        SYSCOLOR_Init(FALSE); //use OS/2 colors
        QueryPMMenuBitmaps();
    }
    fOS2Look = fLooks;
    MENU_Init();
}
//******************************************************************************
//******************************************************************************
void WIN32API CustForceMonoCursor()
{
    fForceMonoCursor = TRUE;
}
//******************************************************************************
//******************************************************************************
void WIN32API DisableDragDrop(BOOL fDisabled)
{
    fDragDropDisabled = fDisabled;
}
//******************************************************************************
// Turn on CD Polling (window with 2 second timer to check CD disk presence)
//
// NOTE: This can cause PM hangs when executing a program for a very long time
//       (block in IOCtl)
//******************************************************************************
void WIN32API CustEnableCDPolling()
{
    fEnableCDPolling = TRUE;
}
//******************************************************************************
//CD notification window class
//******************************************************************************
MRESULT EXPENTRY Win32CDWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
#pragma pack(1)
 typedef struct
 {
   BYTE  ucCommandInfo;
   WORD  usDriveUnit;
 } ParameterBlock;
#pragma pack()

 MRESULT      rc              = 0;
 static ULONG drives[26]      = {0};
 static int   drivestatus[26] = {0};

    switch( msg )
    {
    //OS/2 msgs
    case WM_CREATE:
    {
        char drive[4];

        //skip floppy drives
        drive[0] = 'C';
        drive[1] = ':';
        drive[2] = '\0';

        for(int i=2;i<26;i++) {
           drives[i] = GetDriveTypeA(drive);
           if(drives[i] == DRIVE_CDROM_W)
           {
                DWORD parsize = sizeof(ParameterBlock);
                DWORD datasize = 2;
                WORD status = 0;
                DWORD rc;
                ParameterBlock parm;

                parm.ucCommandInfo = 0;
                parm.usDriveUnit   = i;
                rc = DosDevIOCtl(-1, IOCTL_DISK, DSK_GETLOCKSTATUS, &parm, sizeof(parm), &parsize,
                                      &status, sizeof(status), &datasize);
                if(rc != NO_ERROR) {
                    dprintf(("DosDevIOCtl failed with rc %d", rc));
                    drives[i] = 0;
                    continue;
                }
                //if no disk present, return FALSE
                if(status & 4) {
                    drivestatus[i] = status & 4;
                }
           }
           drive[0]++;
        }
        WinStartTimer(hab, hwnd, TIMERID_DRIVEPOLL, 32*60);
////        WinStartTimer(hab, hwnd, TIMERID_DRIVEPOLL, 32*3);
////        WinStartTimer(hab, hwnd, TIMERID_DRIVEPOLL, 5000);
        rc = (MRESULT)FALSE;
        break;
    }
    case WM_TIMER:
    {
        for(int i=0;i<26;i++)
        {
            //for now only cdrom/dvd drives
            if(drives[i] == DRIVE_CDROM_W)
            {
                DWORD parsize = sizeof(ParameterBlock);
                DWORD datasize = 2;
                WORD status = 0;
                DWORD rc;
                ParameterBlock parm;

                parm.ucCommandInfo = 0;
                parm.usDriveUnit   = i;
                rc = DosDevIOCtl(-1, IOCTL_DISK, DSK_GETLOCKSTATUS, &parm, sizeof(parm), &parsize,
                                      &status, sizeof(status), &datasize);
                if(rc != NO_ERROR) {
                    dprintf(("DosDevIOCtl failed with rc %d", rc));
                    return FALSE;
                }
                //Send WM_DEVICECHANGE message when CD status changes
                if((status & 4) != drivestatus[i])
                {
                    PID pidThis, pidTemp;
                    HENUM henum;
                    HWND  hwndEnum;
                    DEV_BROADCAST_VOLUME volchange;

                    dprintf(("Disk status 0x%x", status));

                    volchange.dbcv_size       = sizeof(volchange);
                    volchange.dbcv_devicetype = DBT_DEVTYP_VOLUME;
                    volchange.dbcv_reserved   = 0;
                    volchange.dbcv_unitmask   = (1 << i);
                    volchange.dbcv_flags      = DBTF_MEDIA;

                    WinQueryWindowProcess(hwnd, &pidThis, NULL);

                    //Iterate over all child windows of the desktop
                    henum = WinBeginEnumWindows(HWND_DESKTOP);

                    SetWin32TIB();
                    while(hwndEnum = WinGetNextWindow(henum))
                    {
                        WinQueryWindowProcess(hwndEnum, &pidTemp, NULL);
                        if(pidTemp == pidThis)
                        {
                            HWND hwndWin32 = OS2ToWin32Handle(hwndEnum);
                            if(hwndWin32) {
                                SendMessageA(hwndWin32,
                                             WM_DEVICECHANGE_W,
                                             (status & 4) ? DBT_DEVICEARRIVAL : DBT_DEVICEREMOVECOMPLETE,
                                             (LPARAM)&volchange);
                            }
                        }
                    }
                    RestoreOS2TIB();
                    WinEndEnumWindows(henum);

                    drivestatus[i] = (status & 4);
                }
            }
        }
        break;
    }

    case WM_DESTROY:
        dprintf(("WM_DESTROY for CD notification window"));
        WinStopTimer(hab, hwnd, TIMERID_DRIVEPOLL);
        break;

    default:
        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
    return (MRESULT)rc;
}
//******************************************************************************
// Win32 window message handler
// The PM window procedure for our client window class (non frame)
//******************************************************************************
MRESULT EXPENTRY Win32WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 Win32BaseWindow *win32wnd;
 TEB             *teb;
 MSG              winMsg, *pWinMsg;
 MRESULT          rc = 0;
 POSTMSG_PACKET  *postmsg;
 OSLIBPOINT       point, ClientPoint;
 EXCEPTIONREGISTRATIONRECORD exceptRegRec = {0,0};

    ODIN_SetExceptionHandler(&exceptRegRec);
    // restore our FS selector
    SetWin32TIB();

#ifdef DEBUG
    dbg_ThreadPushCall("Win32WindowProc");
#endif

    // BEGIN NOTE-------------->>>>>> If this is changed, also change Win32FrameWindowProc!! <<<<<<<<<<<-------------------- BEGIN
    teb = GetThreadTEB();
    win32wnd = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);

////    dprintf(("window %x msg %x", (win32wnd) ? win32wnd->getWindowHandle() : 0, msg));

    // do some sanity checking here:
    // - we need to have a TEB handle
    // - unless this is WM_CREATE (the very first message), there has to be
    //   a USER32 window object for this window handle
    // - thread must not be suspended in WaitMessage
    if(!teb || (msg != WM_CREATE && win32wnd == NULL) || teb->o.odin.fWaitMessageSuspend) {
        if(teb && teb->o.odin.fWaitMessageSuspend)
             dprintf(("OS2: fWaitMessageSuspend window %x msg %x -> run default frame proc", hwnd, msg));
        else dprintf(("OS2: Invalid win32wnd pointer for window %x msg %x", hwnd, msg));
        goto RunDefWndProc;
    }
////    if(teb->o.odin.fIgnoreMsgs) {
////        goto RunDefWndProc;
////    }

    // check if the message state counter in the TEB is odd
    // This means the message has been sent directly from PM to our message
    // handler (so it is the first time we know about this PM message).
    // If this is the case, we have to translate it here to a Win32
    // message first. The other case is that the message is the result of a
    // WinDispatchMsg call and therefore has already been translated.
    if((teb->o.odin.msgstate & 1) == 0)
    {
        // message that was sent directly to our window proc handler; translate it here
        QMSG qmsg;

        qmsg.msg  = msg;
        qmsg.hwnd = hwnd;
        qmsg.mp1  = mp1;
        qmsg.mp2  = mp2;
        qmsg.time = WinQueryMsgTime(teb->o.odin.hab);
        WinQueryMsgPos(teb->o.odin.hab, &qmsg.ptl);
        qmsg.reserved = 0;

        /* sometimes MSG_REMOVE here caused to double free of the message */
        if(OS2ToWinMsgTranslate((PVOID)teb, &qmsg, &winMsg, FALSE, /*MSG_REMOVE*/0) == FALSE)
        {//message was not translated
            memset(&winMsg, 0, sizeof(MSG));
        }
        pWinMsg = &winMsg;
    }
    else {
        // message has already been translated before (GetMessage/PeekMessage).
        // Use the translated information. Flip the translation flag.
        pWinMsg = &teb->o.odin.msg;
        teb->o.odin.msgstate++;
    }
    // END NOTE-------------->>>>>> If this is changed, also change Win32FrameWindowProc!! <<<<<<<<<<<-------------------- END

    if(msg >= WIN32APP_POSTMSG) {
        //probably win32 app user message
        dprintf2(("Posted message %x->%x", msg, msg-WIN32APP_POSTMSG));
        if((ULONG)mp1 == WIN32MSG_MAGICA) {
            rc = (MRESULT)win32wnd->DispatchMsgA(pWinMsg);
        }
        else
        if((ULONG)mp1 == WIN32MSG_MAGICW) {
            rc = (MRESULT)win32wnd->DispatchMsgW(pWinMsg);
        }
        else {//broadcasted message
            rc = (MRESULT)win32wnd->DispatchMsgA(pWinMsg);
        }
        RELEASE_WNDOBJ(win32wnd);
        RestoreOS2TIB();
        ODIN_UnsetExceptionHandler(&exceptRegRec);

#ifdef DEBUG
        dbg_ThreadPopCall();
#endif
        return rc;
    }

    switch( msg )
    {
    //OS/2 msgs
    case WM_CREATE:
    {
        if(teb->o.odin.newWindow == 0)
            goto createfail;

        //Processing is done in after WinCreateWindow returns
        dprintf(("OS2: WM_CREATE %x", hwnd));
        win32wnd = (Win32BaseWindow *)teb->o.odin.newWindow;
        win32wnd->addRef();
        teb->o.odin.newWindow = 0;
        if(win32wnd->MsgCreate(hwnd) == FALSE)
        {
            rc = (MRESULT)TRUE; //discontinue window creation
            break;
        }

        //Create CD notification window
        if(hwndCD == 0 && fEnableCDPolling) {
            hwndCD = WinCreateWindow(HWND_DESKTOP, WIN32_CDCLASS,
                                     NULL, 0, 0, 0, 0, 0,
                                     HWND_DESKTOP, HWND_TOP, 0, NULL, NULL);
        }

    createfail:
        rc = (MRESULT)FALSE;
        break;
    }

    case WM_QUIT:
        dprintf(("OS2: WM_QUIT %x", hwnd));
        win32wnd->MsgQuit();
        break;

    case WM_CLOSE:
        dprintf(("OS2: WM_CLOSE %x", hwnd));
        win32wnd->MsgClose();
        break;

    case WM_DESTROY:
        dprintf(("OS2: WM_DESTROY %x", hwnd));
        win32wnd->MsgDestroy();
        WinSetVisibleRegionNotify(hwnd, FALSE);
        goto RunDefWndProc;

    case WM_ENABLE:
        dprintf(("OS2: WM_ENABLE %x", hwnd));
        break;

    case WM_SHOW:
    {
        dprintf(("OS2: WM_SHOW %x %d", hwnd, mp1));
        win32wnd->MsgShow((ULONG)mp1);

        //if a child window is hidden, then the update region of the
        //parent changes and a WM_ERASEBKGND is required during the next
        //BeginPaint call.
        if((ULONG)mp1 == FALSE)
        {
            Win32BaseWindow *parent = win32wnd->getParent();
            if(parent) {
                dprintf(("PM Update region changed for parent %x", win32wnd->getWindowHandle()));
                parent->SetPMUpdateRegionChanged(TRUE);
            }
        }
        break;
    }

    case WM_ACTIVATE:
    {
        ULONG flags = WinQueryWindowULong(hwnd, OFFSET_WIN32FLAGS);

        dprintf(("OS2: WM_ACTIVATE %x %x %x", hwnd, mp1, mp2));
        WinSetWindowULong(hwnd, OFFSET_WIN32FLAGS, SHORT1FROMMP(mp1) ? (flags | WINDOWFLAG_ACTIVE):(flags & ~WINDOWFLAG_ACTIVE));
        if(win32wnd->IsWindowCreated())
        {
            win32wnd->MsgActivate((LOWORD(pWinMsg->wParam) == WA_ACTIVE_W) ? 1 : 0, HIWORD(pWinMsg->wParam), pWinMsg->lParam, (HWND)mp2);
        }
        break;
    }

    case WM_SIZE:
    {
        dprintf(("OS2: WM_SIZE (%d,%d) (%d,%d)", SHORT1FROMMP(mp2), SHORT2FROMMP(mp2), SHORT1FROMMP(mp1), SHORT2FROMMP(mp1)));
        win32wnd->SetPMUpdateRegionChanged(TRUE);

        goto RunDefWndProc;
    }


    case WM_VRNENABLED:
    {
        dprintf(("OS2: WM_VRNENABLED %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        //Always call handler; even if mp1 is 0. If we don't do this, the
        //DivX 4 player will never be allowed to draw after putting another window
        //on top of it.

        win32wnd->callVisibleRgnNotifyProc(TRUE);

        //Workaround for PM/GPI bug when moving/sizing a window with open DCs
        //
        //Windows applictions often get a DC and keep it open for the duration
        //of the application. When the DC's window is moved (full window dragging on)
        //PM/GPI doesn't seem to update the DC properly/in time.
        //This can result is visible distortions on the screen.
        //Debugging showed that querying the visible region of a DC will cure
        //this problem (GPI probably recalculates the visible region). (#334)

        int  nrdcs = 0;
        HDC  hdcWindow[MAX_OPENDCS];

        if(win32wnd->queryOpenDCs(hdcWindow, MAX_OPENDCS, &nrdcs))
        {
            RECTL rcl = {0,0,1,1};
            HRGN hrgnRect;

            for(int i=0;i<nrdcs;i++) {
                dprintf(("Recalc visible region of DC %x for window %x", hdcWindow[i], win32wnd->getWindowHandle()));

                hrgnRect = GreCreateRectRegion(hdcWindow[i], &rcl, 1);
                GreCopyClipRegion(hdcWindow[i], hrgnRect, 0, COPYCRGN_VISRGN);
                GreDestroyRegion(hdcWindow[i], hrgnRect);
            }
        }
        //Workaround END

        if(!win32wnd->isComingToTop() && ((win32wnd->getExStyle() & WS_EX_TOPMOST_W) == WS_EX_TOPMOST_W))
        {
            HWND hwndrelated;
            Win32BaseWindow *topwindow;

            win32wnd->setComingToTop(TRUE);

            hwndrelated = WinQueryWindow(hwnd, QW_PREV);
            dprintf(("WM_VRNENABLED hwndrelated = %x (hwnd=%x)", hwndrelated, hwnd));
            topwindow = Win32BaseWindow::GetWindowFromOS2Handle(hwndrelated);
            if(topwindow == NULL || ((win32wnd->getExStyle() & WS_EX_TOPMOST_W) == 0)) {
                //put window at the top of z order
                WinSetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );
            }
            if(topwindow) RELEASE_WNDOBJ(topwindow);

            win32wnd->setComingToTop(FALSE);
            break;
        }
        goto RunDefWndProc;
    }

    case WM_VRNDISABLED:
    {
        dprintf(("OS2: WM_VRNDISABLED %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        //visible region is about to change or WinLockWindowUpdate called
        //suspend window drawing

        win32wnd->callVisibleRgnNotifyProc(FALSE);
        goto RunDefWndProc;
    }

    case WIN32APP_DDRAWFULLSCREEN:
        //Changing the size of the win32 window in SetCooperativeLevel can
        //fail if this happens during WM_ADJUSTWINDOWPOS
        //NOTE: This is not a good solution, but a proper fix is more difficult
        //      with the current window mess
        dprintf(("WIN32APP_DDRAWFULLSCREEN %x (%d,%d)", win32wnd->getWindowHandle(), mp1, mp2));
        SetWindowPos(win32wnd->getWindowHandle(), HWND_TOP_W, 0, 0, (DWORD)mp1, (DWORD)mp2, 0);
        ShowWindow(win32wnd->getWindowHandle(), SW_SHOW_W);
        break;

    case WIN32APP_CHNGEFRAMECTRLS:
    {
        dprintf(("OS2: WIN32APP_CHANGEFRAMECTRLS"));
        OSLibSetWindowStyle(win32wnd->getOS2FrameWindowHandle(), win32wnd->getOS2WindowHandle(), (ULONG)mp1, win32wnd->getExStyle(), (ULONG)mp2);
        break;
    }

#ifdef DEBUG
    case WM_SETFOCUS:
    {
        HWND hwndFocus = (HWND)mp1;
        dprintf(("OS2: WM_SETFOCUS %x %x (%x) %d cur focus %x", win32wnd->getWindowHandle(), mp1, OS2ToWin32Handle(hwndFocus), mp2, WinQueryFocus(HWND_DESKTOP)));
        if(WinQueryFocus(HWND_DESKTOP) == win32wnd->getOS2FrameWindowHandle()) {
            dprintf(("WARNING: Focus set to frame window"));
        }
        break;
    }
#endif

    //Handle all focus processed during WM_FOCUSCHANGED; PM doesn't like focus
    //changes during focus processing (WM_SETFOCUS). This message is sent
    //after all focus work has been completed.
    case WM_FOCUSCHANGED:
    {
      HWND hwndFocus      = (HWND)mp1;
      HWND hwndFocusWin32 = OS2ToWin32Handle(hwndFocus);

        dprintf(("OS2: WM_FOCUSCHANGED %x %x (%x) %d cur focus %x", win32wnd->getWindowHandle(), mp1, OS2ToWin32Handle(hwndFocus), mp2, WinQueryFocus(HWND_DESKTOP)));

        //PM doesn't allow SetFocus calls during WM_SETFOCUS message processing;
        //must delay this function call

        if(WinQueryWindowULong(hwndFocus, OFFSET_WIN32PM_MAGIC) != WIN32PM_MAGIC)
        {
            //another (non-win32) application's window
            //set to NULL (allowed according to win32 SDK) to avoid problems
            hwndFocus      = 0;
            hwndFocusWin32 = 0;
        }
        if((ULONG)mp2 == TRUE) {
            recreateCaret(hwndFocusWin32);
            win32wnd->MsgSetFocus(hwndFocusWin32);
        }
        else {
            //If SetFocus(0) was called, then the window has already received
            //a WM_KILLFOCUS; don't send another one
            if(!fIgnoreKeystrokes) {
                 win32wnd->MsgKillFocus(OS2ToWin32Handle(hwndFocus));
            }
            else dprintf(("Window has already received a WM_KILLFOCUS (SetFocus(0)); ignore"));
        }
        break;
    }

    //**************************************************************************
    //Mouse messages (OS/2 Window coordinates -> Win32 coordinates relative to screen
    //**************************************************************************

    case WM_BUTTON1DOWN:
    case WM_BUTTON1UP:
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON2DOWN:
    case WM_BUTTON2UP:
    case WM_BUTTON2DBLCLK:
    case WM_BUTTON3DOWN:
    case WM_BUTTON3UP:
    case WM_BUTTON3DBLCLK:
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->MsgButton(pWinMsg);

        rc = (MRESULT)TRUE;
        break;

    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:
    case WM_BUTTON2CLICK:
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:
    case WM_BUTTON1CLICK:
    case WM_BUTTON3MOTIONSTART:
    case WM_BUTTON3MOTIONEND:
    case WM_BUTTON3CLICK:
        rc = (MRESULT)TRUE;
        break;

    case WM_MOUSEMOVE:
    {
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->MsgMouseMove(pWinMsg);
        break;
    }

    case WM_CONTROL:
        goto RunDefWndProc;

    case WM_COMMAND:
        dprintf(("OS2: WM_COMMAND %x %x %x", hwnd, mp1, mp2));
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_SYSCOMMAND:
        dprintf(("OS2: WM_SYSCOMMAND %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_RENDERFMT:
    case WM_RENDERALLFMTS:
    case WM_DESTROYCLIPBOARD:
    case WM_DRAWCLIPBOARD:
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_CHAR_SPECIAL:
      /* NO BREAK! FALLTHRU CASE! */

    case WM_CHAR:
        dprintf(("OS2: WM_CHAR %x %x %x, %x %x focus wnd %x", win32wnd->getWindowHandle(), mp1, mp2, pWinMsg->wParam, pWinMsg->lParam, WinQueryFocus(HWND_DESKTOP)));
        win32wnd->MsgChar(pWinMsg);
        break;

    case WM_TIMER:
        dprintf(("OS2: WM_TIMER %x %x time %x", win32wnd->getWindowHandle(), pWinMsg->wParam, GetTickCount()));
        win32wnd->DispatchMsgA(pWinMsg);
        goto RunDefWndProc;

    case WM_SETWINDOWPARAMS:
    {
        WNDPARAMS *wndParams = (WNDPARAMS *)mp1;

        dprintf(("OS2: WM_SETWINDOWPARAMS %x", hwnd));
        if(wndParams->fsStatus & WPM_TEXT) {
            win32wnd->MsgSetText(wndParams->pszText, wndParams->cchText);
        }
        goto RunDefWndProc;
    }

    case WM_QUERYWINDOWPARAMS:
    {
     PWNDPARAMS wndpars = (PWNDPARAMS)mp1;
     ULONG textlen;
     PSZ   wintext;

        if(wndpars->fsStatus & (WPM_CCHTEXT | WPM_TEXT))
        {
            if(wndpars->fsStatus & WPM_TEXT)
                win32wnd->MsgGetText(wndpars->pszText, wndpars->cchText);
            if(wndpars->fsStatus & WPM_CCHTEXT)
                wndpars->cchText = win32wnd->MsgGetTextLength();

            wndpars->fsStatus = 0;
            wndpars->cbCtlData = 0;
            wndpars->cbPresParams = 0;
            rc = (MRESULT)TRUE;
            break;
        }
        goto RunDefWndProc;
    }

    case WM_PAINT:
    {
      RECTL rectl;
      BOOL  rc;

        win32wnd->checkForDirtyUpdateRegion();

        rc = WinQueryUpdateRect(hwnd, &rectl);
        dprintf(("OS2: WM_PAINT %x (%d,%d) (%d,%d) rc=%d", win32wnd->getWindowHandle(), rectl.xLeft, rectl.yBottom, rectl.xRight, rectl.yTop, rc));

        if(rc && win32wnd->IsWindowCreated() && (rectl.xLeft != rectl.xRight &&
           rectl.yBottom != rectl.yTop) && !IsIconic(win32wnd->GetTopParent()))
        {
                win32wnd->DispatchMsgA(pWinMsg);
                if(WinQueryUpdateRect(hwnd, NULL) == TRUE)
                {//the application didn't validate the update region; Windows
                 //will only send a WM_PAINT once until another part of the
                 //window is invalidated. Unfortunately PM keeps on sending
                 //WM_PAINT messages until we validate the update region.

                    win32wnd->saveAndValidateUpdateRegion();
                }
        }
        else    goto RunDefWndProc;
        break;
    }

    case WM_ERASEBACKGROUND:
    {
        dprintf(("OS2: WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)FALSE;
        break;
    }

    case WM_CALCVALIDRECTS:
        dprintf(("OS2: WM_CALCVALIDRECTS %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)(CVR_ALIGNLEFT | CVR_ALIGNTOP);
        break;

    case WM_REALIZEPALETTE:
    {
        dprintf(("OS2: WM_REALIZEPALETTE %x", win32wnd->getWindowHandle()));
        win32wnd->DispatchMsgA(pWinMsg);
        break;
    }

    case WM_HSCROLL:
    case WM_VSCROLL:
        dprintf(("OS2: %s %x %x %x", (msg == WM_HSCROLL) ? "WM_HSCROLL" : "WM_VSCROLL", win32wnd->getWindowHandle(), mp1, mp2));
        win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_IMEREQUEST:
    case WM_IMECONTROL:
    case WM_IMENOTIFY:
        if( pWinMsg->message )
        {
            win32wnd->DispatchMsgA( pWinMsg );

            if(( pWinMsg->message = WM_IME_NOTIFY_W ) &&
               ( pWinMsg->wParam == IMN_SETOPENSTATUS_W ))
            {
                MSG m;

                m.message = WM_IME_NOTIFY_W;
                m.wParam = IMN_SETCONVERSIONMODE_W;
                m.lParam = 0;

                win32wnd->DispatchMsgA( &m );
            }
        }
        else
            goto RunDefWndProc;
        break;

    case DM_DRAGOVER:
    {
        PDRAGINFO pDragInfo = (PDRAGINFO)mp1;
        PDRAGITEM pDragItem;
        USHORT    sxDrop = SHORT1FROMMP(mp2);
        USHORT    syDrop = SHORT2FROMMP(mp2);

        dprintf(("OS2: DM_DRAGOVER %x (%d,%d)", win32wnd->getWindowHandle(), sxDrop, syDrop));

        if(fDragDropDisabled) {
            rc = (MRFROM2SHORT (DOR_NEVERDROP, 0));
            break;
        }

        //does this window accept dropped files?
        if(!DragDropAccept(win32wnd->getWindowHandle())) {
            rc = (MRFROM2SHORT (DOR_NEVERDROP, 0));
            break;
        }

        if(PMDragValidate(pDragInfo) == FALSE) {
            rc = (MRFROM2SHORT (DOR_NEVERDROP, 0));
            break;
        }
        if(win32wnd->isDragDropActive() == FALSE) {
            ULONG ulBytes, cItems;
            char *pszFiles;

            pszFiles = PMDragExtractFiles(pDragInfo, &cItems, &ulBytes);
            if(pszFiles) {
                POINT point = {sxDrop, syDrop};
                if(DragDropDragEnter(win32wnd->getWindowHandle(), point, cItems, pszFiles, ulBytes, DROPEFFECT_COPY_W) == FALSE) {
                    rc = (MRFROM2SHORT (DOR_NEVERDROP, 0));
                }
                else {
                    fDragDropActive = TRUE;
                    rc = (MRFROM2SHORT(DOR_DROP, DO_MOVE));
                    win32wnd->setDragDropActive(TRUE);
                }
                free(pszFiles);
            }
            else {
                rc = (MRFROM2SHORT (DOR_NEVERDROP, 0));
            }
        }
        else {
            if(DragDropDragOver(win32wnd->getWindowHandle(), DROPEFFECT_COPY_W) == FALSE) {
                    rc = (MRFROM2SHORT (DOR_NEVERDROP, 0));
            }
            else    rc = (MRFROM2SHORT(DOR_DROP, DO_MOVE));
        }
        break;
    }

    case DM_DRAGLEAVE:
    {
        dprintf(("OS2: DM_DRAGLEAVE %x", win32wnd->getWindowHandle()));

        if(fDragDropDisabled) {
            break;
        }

        fDragDropActive = FALSE;

        //does this window accept dropped files?
        if(!DragDropAccept(win32wnd->getWindowHandle())) {
            break;
        }

        DragDropDragLeave(win32wnd->getWindowHandle());
        win32wnd->setDragDropActive(FALSE);
        break;
    }

    case DM_DROP:
    {
        PDRAGINFO pDragInfo = (PDRAGINFO)mp1;
        PDRAGITEM pDragItem;
        USHORT    sxDrop = SHORT1FROMMP(mp2);
        USHORT    syDrop = SHORT2FROMMP(mp2);
        USHORT    usIndicator, usOp;

        dprintf(("OS2: DM_DROP %x (%d,%d)", win32wnd->getWindowHandle(), sxDrop, syDrop));

        fDragDropActive = FALSE;
        rc = (MRFROM2SHORT (DOR_NODROP, 0));

        if(fDragDropDisabled) {
            rc = (MRFROM2SHORT (DOR_NODROP, 0));
            break;
        }

        //does this window accept dropped files?
        if(!DragDropAccept(win32wnd->getWindowHandle())) {
            break;
        }

        ULONG ulBytes, cItems;
        char *pszFiles;

        pszFiles = PMDragExtractFiles(pDragInfo, &cItems, &ulBytes);
        if(pszFiles) {
            POINT point = {sxDrop, syDrop};
            if(DragDropFiles(win32wnd->getWindowHandle(), point, cItems, pszFiles, ulBytes) == FALSE) {
                rc = (MRFROM2SHORT (DOR_NEVERDROP, 0));
            }
            else {
                rc = (MRFROM2SHORT(DOR_DROP, DO_MOVE));
                win32wnd->setDragDropActive(FALSE);
            }
            free(pszFiles);
        }
        else {
            rc = (MRFROM2SHORT (DOR_NEVERDROP, 0));
        }
        break;
    }

    case DM_RENDER:
    {
        PDRAGTRANSFER pDragTransfer = (PDRAGTRANSFER)mp1;

        dprintf(("OS2: DM_RENDER %x", pDragTransfer));

        rc = (MRESULT)OSLibRenderFormat(pDragTransfer);
        break;
    }

    case DM_RENDERPREPARE:
    {
        PDRAGTRANSFER pDragTransfer = (PDRAGTRANSFER)mp1;

        dprintf(("OS2: DM_RENDERPREPARE %x", pDragTransfer));
        break;
    }

    case DM_ENDCONVERSATION:
    {
        dprintf(("OS2: DM_ENDCONVERSATION"));
        rc = (MRESULT)OSLibEndConversation();
        break;
    }

    case DM_RENDERFILE:
    {
        dprintf(("OS2: DM_ENDCONVERSATION"));
        rc = FALSE;
        break;
    }

    case WM_DDE_INITIATE:
    case WM_DDE_INITIATEACK:
    case WM_DDE_REQUEST:
    case WM_DDE_ACK:
    case WM_DDE_DATA:
    case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE:
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
    case WM_DDE_TERMINATE:
        dprintf(("OS2: WM_DDE %x %x", msg, win32wnd->getWindowHandle()));
        goto RunDefWndProc;

    case WM_INITMENU:
    case WM_MENUSELECT:
    case WM_MENUEND:
    case WM_NEXTMENU:
    case WM_SYSCOLORCHANGE:
    case WM_SYSVALUECHANGED:
    case WM_SETSELECTION:
    case WM_PPAINT:
    case WM_PSETFOCUS:
    case WM_PSYSCOLORCHANGE:
    case WM_PSIZE:
    case WM_PACTIVATE:
    case WM_PCONTROL:
    case WM_HELP:
    case WM_APPTERMINATENOTIFY:
    case WM_PRESPARAMCHANGED:
    case WM_DRAWITEM:
    case WM_MEASUREITEM:
    case WM_CONTROLPOINTER:
    case WM_QUERYDLGCODE:
    case WM_SUBSTITUTESTRING:
    case WM_MATCHMNEMONIC:
    case WM_SAVEAPPLICATION:
    case WM_SEMANTICEVENT:
    default:
        dprintf2(("OS2: RunDefWndProc hwnd %x msg %x mp1 %x mp2 %x", hwnd, msg, mp1, mp2));
        goto RunDefWndProc;
    }
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    ODIN_UnsetExceptionHandler(&exceptRegRec);

#ifdef DEBUG
    dbg_ThreadPopCall();
#endif
    return (MRESULT)rc;

RunDefWndProc:
//  dprintf(("OS2: RunDefWndProc msg %x for %x", msg, hwnd));
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);

    RestoreOS2TIB();
    ODIN_UnsetExceptionHandler(&exceptRegRec);

#ifdef DEBUG
    dbg_ThreadPopCall();
#endif
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
} /* End of Win32WindowProc */
//******************************************************************************
//******************************************************************************
MRESULT EXPENTRY Win32FrameWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 POSTMSG_PACKET  *postmsg;
 OSLIBPOINT       point, ClientPoint;
 Win32BaseWindow *win32wnd;
 TEB             *teb;
 MRESULT          rc = 0;
 MSG              winMsg, *pWinMsg;
 EXCEPTIONREGISTRATIONRECORD exceptRegRec = {0,0};

#ifdef DEBUG
    dbg_ThreadPushCall("Win32FrameWindowProc");
#endif

    ODIN_SetExceptionHandler(&exceptRegRec);
    //Restore our FS selector
    SetWin32TIB();

    // BEGIN NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- BEGIN
    teb = GetThreadTEB();
    win32wnd = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwnd);

    // do some sanity checking here:
    // - we need to have a TEB handle
    // - unless this is WM_CREATE (the very first message), there has to be
    //   a USER32 window object for this window handle
    // - thread must not be suspended in WaitMessage
    if(!teb || (msg != WM_CREATE && win32wnd == NULL) || teb->o.odin.fWaitMessageSuspend) {
        if(teb && teb->o.odin.fWaitMessageSuspend)
             dprintf(("PMFRAME: fWaitMessageSuspend window %x msg %x -> run default frame proc", hwnd, msg));
        else dprintf(("PMFRAME: Invalid win32wnd pointer for window %x msg %x", hwnd, msg));
        goto RunDefFrameWndProc;
    }
////    if(teb->o.odin.fIgnoreMsgs) {
////        goto RunDefWndProc;
////    }

    // check if the message state counter in the TEB is odd
    // This means the message has been sent directly from PM to our message
    // handler (so it is the first time we know about this PM message).
    // If this is the case, we have to translate it here to a Win32
    // message first. The other case is that the message is the result of a
    // WinDispatchMsg call and therefore has already been translated.
    if((teb->o.odin.msgstate & 1) == 0)
    {//message that was sent directly to our window proc handler; translate it here
        QMSG qmsg;

        qmsg.msg  = msg;
        qmsg.hwnd = hwnd;
        qmsg.mp1  = mp1;
        qmsg.mp2  = mp2;
        qmsg.time = WinQueryMsgTime(teb->o.odin.hab);
        WinQueryMsgPos(teb->o.odin.hab, &qmsg.ptl);
        qmsg.reserved = 0;

        if(OS2ToWinMsgTranslate((PVOID)teb, &qmsg, &winMsg, FALSE, MSG_REMOVE) == FALSE)
        {//message was not translated
            memset(&winMsg, 0, sizeof(MSG));
        }
        pWinMsg = &winMsg;
    }
    else {
        // message has already been translated before (GetMessage/PeekMessage).
        // Use the translated information. Flip the translation flag.
        pWinMsg = &teb->o.odin.msg;
        teb->o.odin.msgstate++;
    }
    // END NOTE-------------->>>>>> If this is changed, also change Win32WindowProc!! <<<<<<<<<<<-------------------- END

    switch( msg )
    {
    case WM_CREATE:
    {
        //WM_CREATE handled during client window creation
        dprintf(("PMFRAME: WM_CREATE %x", hwnd));
        goto RunDefFrameWndProc;
    }

//hack alert; PM crashes if child calls DestroyWindow for parent/owner in WM_DESTROY
//            handler; must postpone it, so do it here
    case WIN32APP_POSTPONEDESTROY:
        OSLibWinDestroyWindow(hwnd);
        break;
//hack end

#ifdef DEBUG
    case WM_CLOSE:
    {
        dprintf(("PMFRAME: WM_CLOSE %x", hwnd));
        goto RunDefFrameWndProc;
    }
#endif

    case WM_PAINT:
    {
        RECTL rectl;
        HRGN  hrgn;

        hrgn = CreateRectRgn(0, 0, 0, 0);
        GetUpdateRgnFrame(win32wnd->getWindowHandle(), hrgn);

        HPS hps = WinBeginPaint(hwnd, NULL, &rectl);
        dprintf(("PMFRAME: WM_PAINT %x (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), rectl.xLeft, rectl.yBottom, rectl.xRight, rectl.yTop));

        if(win32wnd->IsWindowCreated() && (rectl.xLeft != rectl.xRight &&
           rectl.yBottom != rectl.yTop))
        {
            PRECT pClient = win32wnd->getClientRectPtr();
            PRECT pWindow = win32wnd->getWindowRect();

            if(!(pClient->left == 0 && pClient->top == 0 &&
               win32wnd->getClientHeight() == win32wnd->getWindowHeight() &&
               win32wnd->getClientWidth()  == win32wnd->getWindowWidth()))
            {
                RECT rectUpdate;

                mapOS2ToWin32Rect(win32wnd->getWindowHeight(), (PRECTLOS2)&rectl, &rectUpdate);
                win32wnd->MsgNCPaint(&rectUpdate, hrgn);
            }
        }
        WinEndPaint(hps);

        DeleteObject(hrgn);
        break;
    }

    case WM_ERASEBACKGROUND:
    {
        dprintf(("PMFRAME:WM_ERASEBACKGROUND %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)FALSE;
        break;
    }

    //**************************************************************************
    //Mouse messages (OS/2 Window coordinates -> Win32 coordinates relative to screen
    //**************************************************************************

    case WM_BUTTON1DOWN:
    case WM_BUTTON1UP:
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON2DOWN:
    case WM_BUTTON2UP:
    case WM_BUTTON2DBLCLK:
    case WM_BUTTON3DOWN:
    case WM_BUTTON3UP:
    case WM_BUTTON3DBLCLK:
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->MsgButton(pWinMsg);

        rc = (MRESULT)TRUE;
        break;

    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:
    case WM_BUTTON2CLICK:
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:
    case WM_BUTTON1CLICK:
    case WM_BUTTON3MOTIONSTART:
    case WM_BUTTON3MOTIONEND:
    case WM_BUTTON3CLICK:
        rc = (MRESULT)TRUE;
        break;

    case WM_MOUSEMOVE:
    {
        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->MsgMouseMove(pWinMsg);
        break;
    }

    case WM_CHAR_SPECIAL_CONSOLE_BREAK:
    {
        //ignore this message. don't forward it to the default PM frame window handler
        //as that one sends it to the client. as a result we end up translating
        //it twice
        break;
    }
	
	case WM_CHAR:
		{
			dprintf(("PMFRAME:WM_CHAR"));
			break;
		}

    case WM_ADJUSTWINDOWPOS:
    {
      PSWP     pswp = (PSWP)mp1;
      SWP      swpOld;
      WINDOWPOS wp,wpOld;
      ULONG     ulFlags;
      ULONG     ret = 0;
      HWND      hParent = NULLHANDLE, hwndAfter;

        dprintf(("PMFRAME:WM_ADJUSTWINDOWPOS %x %x %x (%s) (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, DbgGetStringSWPFlags(pswp->fl), pswp->x, pswp->y, pswp->cx, pswp->cy));

        ulFlags = pswp->fl;

        if(win32wnd->IsParentChanging()) {
            rc = 0;
            break;
        }

        //@@PF all commands from minimized window viewer or from Ctrl-Esc
        //are 'pure' and should be handled only by DeFrameProc - this is weird
        //but without them we will not have results. Pure = plain flag of restore/minimize/maximize
        if((pswp->fl == SWP_MINIMIZE) || (pswp->fl & SWP_RESTORE)) {
          // note the purity of SWP no other SWP_FLAGS allowed
           goto RunDefFrameWndProc;
        }

        if(pswp->fl & SWP_NOADJUST) {
            //ignore weird messages (TODO: why are they sent?)
            dprintf(("WARNING: WM_ADJUSTWINDOWPOS with SWP_NOADJUST flag!!"));
            break;

        }

        //PF Pure flags should not cause any subsequent messages to win32 windows
        //we should route them to DefFrameWndProc and check highlight.

        if ((pswp->fl == SWP_FOCUSACTIVATE) || (pswp->fl == SWP_FOCUSDEACTIVATE))
        {
           if (fOS2Look)
           {
             if(pswp->fl == SWP_FOCUSACTIVATE)
             {
                 dprintf2(("TBM_QUERYHILITE returned %d", WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_QUERYHILITE, 0, 0)));
                 WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_SETHILITE, (MPARAM)1, 0);
             }
             else
             {
                 dprintf2(("TBM_QUERYHILITE returned %d", WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_QUERYHILITE, 0, 0)));
                 WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_SETHILITE, 0, 0);
             }
           }
           goto RunDefFrameWndProc;
        }

        //CB: show dialog in front of owner
        if (win32wnd->IsModalDialogOwner())
        {
            dprintf(("win32wnd->IsModalDialogOwner %x", win32wnd->getWindowHandle()));
            pswp->fl |= SWP_ZORDER;
            pswp->hwndInsertBehind = win32wnd->getOS2HwndModalDialog();
            if (pswp->fl & SWP_ACTIVATE)
            {
                pswp->fl &= ~SWP_ACTIVATE;
                WinSetWindowPos(win32wnd->getOS2HwndModalDialog(),0,0,0,0,0,SWP_ACTIVATE);
            }
        }

        if(!win32wnd->CanReceiveSizeMsgs())
            break;

        WinQueryWindowPos(hwnd, &swpOld);
        if(pswp->fl & (SWP_MOVE | SWP_SIZE)) {
            if (win32wnd->isChild()) {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto RunDefFrameWndProc;
            }
        }
        hwndAfter = pswp->hwndInsertBehind;
        if(win32wnd->getParent()) {
             OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getClientHeight(), hwnd);
        }
        else OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
           Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
           dprintf2(("SWP_ZORDER: %x %x", pswp->hwndInsertBehind, (wndAfter) ? wndAfter->getWindowHandle() : 0));
           if(wndAfter) {
                wp.hwndInsertAfter = wndAfter->getWindowHandle();
                RELEASE_WNDOBJ(wndAfter);
           }
           else wp.hwndInsertAfter = HWND_TOP_W;
        }

        wpOld = wp;
        win32wnd->MsgPosChanging((LPARAM)&wp);

        if(win32wnd->getOldStyle() != win32wnd->getStyle())
        {
             OSLibSetWindowStyle(win32wnd->getOS2FrameWindowHandle(), win32wnd->getOS2WindowHandle(), win32wnd->getStyle(), win32wnd->getExStyle(), win32wnd->getStyle());
             if(fOS2Look) {
                 DWORD dwOldStyle = win32wnd->getOldStyle();
                 DWORD dwStyle    = win32wnd->getStyle();

                 win32wnd->setOldStyle(dwStyle);
                 if((dwOldStyle & WS_MINIMIZE_W) && !(dwStyle & WS_MINIMIZE_W)) {
                     //SC_RESTORE -> SC_MINIMIZE
                     dprintf(("%x -> SC_RESTORE -> SC_MINIMIZE", win32wnd->getWindowHandle()));
                     FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), 0, SC_RESTORE, SC_MINIMIZE, hbmFrameMenu[PMMENU_MINBUTTON]);
                     if(dwStyle & WS_MAXIMIZE_W) {
                         //SC_MAXIMIZE -> SC_RESTORE
                         dprintf(("%x -> SC_MAXIMIZE -> SC_RESTORE (1)", win32wnd->getWindowHandle()));
                         FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), MIT_END, SC_MAXIMIZE, SC_RESTORE, hbmFrameMenu[PMMENU_RESTOREBUTTON]);
                     }
                 }
                 else
                 if((dwOldStyle & WS_MAXIMIZE_W) && !(dwStyle & WS_MAXIMIZE_W)) {
                     //SC_RESTORE -> SC_MAXIMIZE
                     dprintf(("%x -> SC_RESTORE -> SC_MAXIMIZE", win32wnd->getWindowHandle()));
                     FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), MIT_END, SC_RESTORE, SC_MAXIMIZE, hbmFrameMenu[PMMENU_MAXBUTTON]);
                 }
                 else
                 if(!(dwOldStyle & WS_MINIMIZE_W) && (dwStyle & WS_MINIMIZE_W)) {
                     //SC_MINIMIZE -> SC_RESTORE
                     dprintf(("%x -> SC_MINIMIZE -> SC_RESTORE", win32wnd->getWindowHandle()));
                     FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), 0, SC_MINIMIZE, SC_RESTORE, hbmFrameMenu[PMMENU_RESTOREBUTTON]);
                 }
                 else
                 if(!(dwOldStyle & WS_MAXIMIZE_W) && (dwStyle & WS_MAXIMIZE_W)) {
                     //SC_MAXIMIZE -> SC_RESTORE
                     dprintf(("%x -> SC_MAXIMIZE -> SC_RESTORE (2)", win32wnd->getWindowHandle()));
                     FrameReplaceMenuItem(WinWindowFromID(hwnd, FID_MINMAX), MIT_END, SC_MAXIMIZE, SC_RESTORE, hbmFrameMenu[PMMENU_RESTOREBUTTON]);
                 }
             }
        }

        if ((wp.hwndInsertAfter != wpOld.hwndInsertAfter) ||
            (wp.x != wpOld.x) || (wp.y != wpOld.y) || (wp.cx != wpOld.cx) || (wp.cy != wpOld.cy) || (wp.flags != wpOld.flags))
        {
            ULONG flags = pswp->fl;      //make a backup copy; OSLibMapWINDOWPOStoSWP will modify it

            dprintf(("PMFRAME:WM_ADJUSTWINDOWPOS, app changed windowpos struct"));
            dprintf(("%x (%s) (%d,%d), (%d,%d)", pswp->fl, DbgGetStringSWPFlags(pswp->fl), pswp->x, pswp->y, pswp->cx, pswp->cy));

            if(win32wnd->getParent()) {
                  OSLibMapWINDOWPOStoSWP(&wp, pswp, &swpOld, win32wnd->getParent()->getClientHeight(),
                                         hwnd);
            }
            else  OSLibMapWINDOWPOStoSWP(&wp, pswp, &swpOld, OSLibQueryScreenHeight(), hwnd);

            dprintf(("%x (%d,%d), (%d,%d)", pswp->fl, pswp->x, pswp->y, pswp->cx, pswp->cy));

            //OSLibMapWINDOWPOStoSWP can add flags, but we must not let it remove flags!
            if(pswp->fl & SWP_SIZE)
                flags |= SWP_SIZE;

            if(pswp->fl & SWP_MOVE)
                flags |= SWP_MOVE;

            pswp->fl = flags;   //restore flags

            pswp->fl |= SWP_NOADJUST;
            pswp->hwndInsertBehind = hwndAfter;
            pswp->hwnd = hwnd;

            ret = 0xf;
        }
adjustend:
        //The next part needs to be done for top-level windows only
        if(!((win32wnd->getStyle() & (WS_POPUP_W|WS_CHILD_W)) == WS_CHILD_W))
        {
            //Setting these flags is necessary to avoid activation/focus problems
            if(ulFlags & SWP_DEACTIVATE) {
                ret |= AWP_DEACTIVATE;
            }
            if(ulFlags & SWP_ACTIVATE)
            {
                if(ulFlags & SWP_ZORDER) {
                    dprintf(("Set FF_NOACTIVATESWP"));
                    ULONG ulFrameFlags = WinQueryWindowUShort(hwnd, QWS_FLAGS);
                    WinSetWindowUShort(hwnd, QWS_FLAGS, ulFrameFlags | FF_NOACTIVATESWP);
                }

                if(!(ulFlags & SWP_SHOW))
                {
                    ret |= AWP_ACTIVATE;
                }
                else
                {
                    FrameSetFocus(hwnd);
                }
            }
        }
        else {
            if(ulFlags & (SWP_ACTIVATE|SWP_FOCUSACTIVATE))
            {
                win32wnd->MsgChildActivate(TRUE);
                if(fOS2Look) {
                    dprintf(("TBM_QUERYHILITE returned %d", WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_QUERYHILITE, 0, 0)));
                    WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_SETHILITE, (MPARAM)1, 0);
                }
            }
            else
            if(ulFlags & (SWP_DEACTIVATE|SWP_FOCUSDEACTIVATE))
            {
                win32wnd->MsgChildActivate(FALSE);
                if(fOS2Look) {
                    dprintf(("TBM_QUERYHILITE returned %d", WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_QUERYHILITE, 0, 0)));
                    WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_SETHILITE, 0, 0);
                }
            }
        }
#ifdef DEBUG
        dprintf(("WM_ADJUSTWINDOWPOS ret %x flags %x", ret, WinQueryWindowUShort(hwnd, QWS_FLAGS)));
        if(ret == 0x0f) {
            dprintf(("PMFRAME:WM_ADJUSTWINDOWPOS, app changed windowpos struct"));
            dprintf(("%x (%s) (%d,%d), (%d,%d)", pswp->fl, DbgGetStringSWPFlags(pswp->fl), pswp->x, pswp->y, pswp->cx, pswp->cy));
        }
#endif
        rc = (MRESULT)ret;
        break;
    }

    case WM_WINDOWPOSCHANGED:
    {
      PSWP      pswp    = (PSWP)mp1,pswpOld = pswp+1;
      SWP       swpOld  = *(pswp + 1);
      WINDOWPOS wp;
      ULONG     flAfp   = (ULONG)mp2;
      HWND      hParent = NULLHANDLE;
      RECTL     rect;

        dprintf(("PMFRAME:WM_WINDOWPOSCHANGED (%x) %x %x (%s) (%d,%d) (%d,%d) z %x", mp2, win32wnd->getWindowHandle(), pswp->fl, DbgGetStringSWPFlags(pswp->fl), pswp->x, pswp->y, pswp->cx, pswp->cy, Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind)));
        if(win32wnd->IsParentChanging()) {
            goto PosChangedEnd;
        }

        // PF: MDI window is not a common OS/2 frame window so we just skipped all
        // PM default processing for it that basicly moved this window to 0,0 point
        // and changed frame controls. Now do our own processing.

        if ((pswp->fl & SWP_MAXIMIZE) && (win32wnd->getExStyle() & WS_EX_MDICHILD_W))
        {
            SendMessageA(win32wnd->getWindowHandle(), WM_SYSCOMMAND_W, SC_MAXIMIZE_W, 0);
            goto PosChangedEnd;
        }

        //SvL: When a window is made visible, then we don't receive a
        //     WM_VRNENABLED message (for some weird reason)
        if(pswp->fl & SWP_SHOW) {
            win32wnd->callVisibleRgnNotifyProc(TRUE);
        }
        else
        if(pswp->fl & SWP_HIDE) {
            win32wnd->callVisibleRgnNotifyProc(FALSE);
        }

        if(pswp->fl & (SWP_MOVE | SWP_SIZE))
        {
            if(win32wnd->isChild())
            {
                if(win32wnd->getParent()) {
                        hParent = win32wnd->getParent()->getOS2WindowHandle();
                }
                else    goto PosChangedEnd; //parent has just been destroyed
            }
        }


        if(win32wnd->getParent()) {
             OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getClientHeight(),
                                    hwnd);
        }
        else OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
            Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
            dprintf2(("SWP_ZORDER: %x %x", pswp->hwndInsertBehind, (wndAfter) ? wndAfter->getWindowHandle() : 0));
            if(wndAfter) {
                 wp.hwndInsertAfter = wndAfter->getWindowHandle();
                 RELEASE_WNDOBJ(wndAfter);
            }
            else wp.hwndInsertAfter = HWND_TOP_W;
        }

        if ((pswp->fl & (SWP_SIZE | SWP_MOVE | SWP_ZORDER)) == 0)
        {
            if(pswp->fl & SWP_RESTORE && win32wnd->getStyle() & WS_MINIMIZE_W) {
                dprintf(("Restoring minimized window %x", win32wnd->getWindowHandle()));
                win32wnd->ShowWindow(SW_RESTORE_W);
            }
            if(pswp->fl & SWP_SHOW) {
                WinShowWindow(win32wnd->getOS2WindowHandle(), 1);
            }
            else
            if(pswp->fl & SWP_HIDE) {
                WinShowWindow(win32wnd->getOS2WindowHandle(), 0);
            }
            if(pswp->fl & (SWP_SHOW|SWP_HIDE))
            {//TODO: necessary for more options? (activate?)
                if(win32wnd->CanReceiveSizeMsgs())
                    win32wnd->MsgPosChanged((LPARAM)&wp);
            }

            //MUST call the old frame window proc!
            goto RunDefFrameWndProc;
        }

        if(pswp->fl & SWP_SHOW) {
            WinShowWindow(win32wnd->getOS2WindowHandle(), 1);
        }
        else
        if(pswp->fl & SWP_HIDE) {
            WinShowWindow(win32wnd->getOS2WindowHandle(), 0);
        }

        if(flAfp & AWP_ACTIVATE)
        {
            FrameSetFocus(hwnd);
        }

#ifndef USE_CALCVALIDRECT
        if((pswp->fl & (SWP_MOVE | SWP_SIZE)))
        {
            //CB: todo: use result for WM_CALCVALIDRECTS
            //Get old client rectangle (for invalidation of frame window parts later on)
            //Use new window height to calculate the client area
            mapWin32ToOS2Rect(pswp->cy, win32wnd->getClientRectPtr(), (PRECTLOS2)&rect);

            //Note: Also updates the new window rectangle
            win32wnd->MsgFormatFrame(&wp);

            if(win32wnd->isOwnDC()) {
                setPageXForm(win32wnd, (pDCData)GpiQueryDCData(win32wnd->getOwnDC()));
            }

            if(win32wnd->CanReceiveSizeMsgs())
                win32wnd->MsgPosChanged((LPARAM)&wp);

            if((pswp->fl & SWP_SIZE) && ((pswp->cx != pswpOld->cx) || (pswp->cy != pswpOld->cy)))
            {
                //redraw the frame (to prevent unnecessary client updates)
                BOOL redrawAll = FALSE;

                dprintf2(("WM_WINDOWPOSCHANGED: redraw frame"));
                if (win32wnd->getWindowClass())
                {
                    DWORD dwStyle = win32wnd->getWindowClass()->getClassLongA(GCL_STYLE_W);

                    if ((dwStyle & CS_HREDRAW_W) && (pswp->cx != pswpOld->cx))
                        redrawAll = TRUE;
                    else
                    if ((dwStyle & CS_VREDRAW_W) && (pswp->cy != pswpOld->cy))
                        redrawAll = TRUE;
                }
                else redrawAll = TRUE;

                if(win32wnd->IsMixMaxStateChanging()) {
                    dprintf(("WM_CALCVALIDRECT: window changed min/max/restore state, invalidate entire window"));
                    redrawAll = TRUE;
                }

                if (redrawAll)
                {
                    //CB: redraw all children for now
                    //    -> problems with update region if we don't do it
                    //       todo: rewrite whole handling
                    dprintf(("PMFRAME: WM_WINDOWPOSCHANGED invalidate all"));
                    WinInvalidateRect(hwnd,NULL,TRUE);
                }
                else
                {
                    HPS hps = WinGetPS(hwnd);
                    RECTL frame,client,arcl[4];

                    WinQueryWindowRect(hwnd,&frame);

                    //top
                    arcl[0].xLeft = 0;
                    arcl[0].xRight = frame.xRight;
                    arcl[0].yBottom = rect.yTop;
                    arcl[0].yTop = frame.yTop;
                    //right
                    arcl[1].xLeft = rect.xRight;
                    arcl[1].xRight = frame.xRight;
                    arcl[1].yBottom = 0;
                    arcl[1].yTop = frame.yTop;
                    //left
                    arcl[2].xLeft = 0;
                    arcl[2].xRight = rect.xLeft;
                    arcl[2].yBottom = 0;
                    arcl[2].yTop = frame.yTop;
                    //bottom
                    arcl[3].xLeft = 0;
                    arcl[3].xRight = frame.xRight;
                    arcl[3].yBottom = 0;
                    arcl[3].yTop = rect.yBottom;

                    HRGN hrgn = GpiCreateRegion(hps,4,(PRECTL)&arcl);

                    WinInvalidateRegion(hwnd,hrgn,FALSE);
                    GpiDestroyRegion(hps,hrgn);
                    WinReleasePS(hps);
                }
            }
        }
        else
        {
#endif //USE_CALCVALIDRECT
            if(win32wnd->CanReceiveSizeMsgs())
                win32wnd->MsgPosChanged((LPARAM)&wp);
#ifndef USE_CALCVALIDRECT
        }
#endif
        //PF This is the final step of PM restoration - should end up
        //in default handler.
        if (win32wnd->getOldStyle() & WS_MINIMIZE_W && pswp->fl & SWP_RESTORE)
              goto RunDefFrameWndProc;

        //PF This is the final step of PM minimization - shoukd end up
        //in default handler
        if (win32wnd->getStyle() & WS_MINIMIZE_W && pswp->fl & SWP_MINIMIZE)
              goto RunDefFrameWndProc;

PosChangedEnd:
        rc = (MRESULT)FALSE;
        break;
    }

    case WM_CALCVALIDRECTS:
#ifdef USE_CALCVALIDRECT
    {
        PRECTL    oldRect = (PRECTL)mp1, newRect = oldRect+1;
        PSWP      pswp = (PSWP)mp2;
        SWP       swpOld;
        WINDOWPOS wp;
        RECTL     newClientRect, oldClientRect;
        ULONG     nccalcret;
//        UINT      res = CVR_ALIGNLEFT | CVR_ALIGNTOP;
        UINT      res = 0;

        dprintf(("PMWINDOW: WM_CALCVALIDRECTS %x", win32wnd->getWindowHandle()));

        //Get old position info
        WinQueryWindowPos(hwnd, &swpOld);

        if(win32wnd->getParent()) {
             OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getClientHeight(),
                                    win32wnd->getParent()->getClientRectPtr()->left,
                                    win32wnd->getParent()->getClientRectPtr()->top,
                                    hwnd);
        }
        else OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), 0, 0, hwnd);

        wp.hwnd = win32wnd->getWindowHandle();
        if ((pswp->fl & SWP_ZORDER) && (pswp->hwndInsertBehind > HWND_BOTTOM))
        {
            Win32BaseWindow *wndAfter = Win32BaseWindow::GetWindowFromOS2Handle(pswp->hwndInsertBehind);
            if(wndAfter) {
                 wp.hwndInsertAfter = wndAfter->getWindowHandle();
                 RELEASE_WNDOBJ(wndAfter);
            }
            else wp.hwndInsertAfter = HWND_TOP_W;
        }

        //Get old client rectangle
        mapWin32ToOS2Rect(oldRect->yTop - oldRect->yBottom, win32wnd->getClientRectPtr(), (PRECTLOS2)&oldClientRect);

        //Note: Also updates the new window rectangle
        nccalcret = win32wnd->MsgFormatFrame(&wp);

        //Get new client rectangle
        mapWin32ToOS2Rect(pswp->cy, win32wnd->getClientRectPtr(), (PRECTLOS2)&newClientRect);

        if(nccalcret == 0) {
            res = CVR_ALIGNTOP | CVR_ALIGNLEFT;
        }
        else {
            if(nccalcret & WVR_ALIGNTOP_W) {
                res |= CVR_ALIGNTOP;
            }
            else
            if(nccalcret & WVR_ALIGNBOTTOM_W) {
                res |= CVR_ALIGNBOTTOM;
            }

            if(nccalcret & WVR_ALIGNLEFT_W) {
                res |= CVR_ALIGNLEFT;
            }
            else
            if(nccalcret & WVR_ALIGNRIGHT_W) {
                res |= CVR_ALIGNRIGHT;
            }

            if(nccalcret & WVR_REDRAW_W) {//WVR_REDRAW_W = (WVR_HREDRAW | WVR_VREDRAW)
                res |= CVR_REDRAW;
            }
            else
            if(nccalcret & WVR_VALIDRECTS_W) {
                //TODO:
                //res = 0;
            }
        }
        if(win32wnd->IsMixMaxStateChanging()) {
            dprintf(("WM_CALCVALIDRECT: window changed min/max/restore state, invalidate entire window"));
            res |= CVR_REDRAW;
        }
        if(res == (CVR_ALIGNTOP|CVR_ALIGNLEFT)) {
            oldRect->xRight  -= oldClientRect.xLeft;
            oldRect->yBottom += oldClientRect.yBottom;
            newRect->xRight  -= newClientRect.xLeft;
            newRect->yBottom += newClientRect.yBottom;
        }
        rc = res;
        break;
    }
#else
        dprintf(("PMWINDOW: WM_CALCVALIDRECTS %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)(CVR_ALIGNLEFT | CVR_ALIGNTOP);
        break;
#endif

    case WM_CALCFRAMERECT:
        dprintf(("PMFRAME:WM_CALCFRAMERECT %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)TRUE;
        break;

    case WM_QUERYCTLTYPE:
        // This is a frame window
        dprintf(("PMFRAME:WM_QUERYCTLTYPE %x", win32wnd->getWindowHandle()));
        rc = (MRESULT)CCT_FRAME;
        break;

#ifdef DEBUG
    case WM_QUERYFOCUSCHAIN:
        dprintf2(("PMFRAME:WM_QUERYFOCUSCHAIN %x fsCmd %x (%s) parent %x", win32wnd->getWindowHandle(), SHORT1FROMMP(mp1), DbgPrintQFCFlags(SHORT1FROMMP(mp1)), (mp2) ? OS2ToWin32Handle((DWORD)mp2) : 0));

        RestoreOS2TIB();
        rc = pfnFrameWndProc(hwnd, msg, mp1, mp2);
        SetWin32TIB();
        dprintf2(("PMFRAME:WM_QUERYFOCUSCHAIN %x fsCmd %x parent %x returned %x (%x)", win32wnd->getWindowHandle(), SHORT1FROMMP(mp1), (mp2) ? OS2ToWin32Handle((DWORD)mp2) : 0, (rc) ? OS2ToWin32Handle((DWORD)rc) : 0, rc));
        break;
//        goto RunDefFrameWndProc;
#endif

    case WM_FOCUSCHANGE:
    {
        HWND   hwndFocus = (HWND)mp1;
        HWND   hwndLoseFocus, hwndGainFocus;
        USHORT usSetFocus = SHORT1FROMMP(mp2);
        USHORT fsFocusChange = SHORT2FROMMP(mp2);

        //Save window that gains focus so we can determine which
        //process we lose activation to
        hwndFocusChange = (HWND)mp1;

        dprintf(("PMFRAME:WM_FOCUSCHANGE %x %x (%x) %x %x", win32wnd->getWindowHandle(), OS2ToWin32Handle(hwndFocus), hwndFocus, usSetFocus, fsFocusChange));
        goto RunDefFrameWndProc;
    }

#ifdef DEBUG
    case WM_SETFOCUS:
    {
        dprintf(("PMFRAME: WM_SETFOCUS %x %x %d -> %x", win32wnd->getWindowHandle(), hwnd, mp2, mp1));
        goto RunDefFrameWndProc;
    }
#endif

    case WM_ACTIVATE:
    {
        HWND hwndTitle;
        USHORT flags = WinQueryWindowUShort(hwnd,QWS_FLAGS);

        dprintf(("PMFRAME: WM_ACTIVATE %x %x %x", win32wnd->getWindowHandle(), mp1, OS2ToWin32Handle((DWORD)mp2)));
        if (win32wnd->IsWindowCreated())
        {
            WinSetWindowUShort(hwnd,QWS_FLAGS,mp1 ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));
            if(fOS2Look) {
                dprintf(("TBM_QUERYHILITE returned %d", WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_QUERYHILITE, 0, 0)));
                WinSendDlgItemMsg(hwnd, FID_TITLEBAR, TBM_SETHILITE, mp1, 0);
            }
            if(SHORT1FROMMP(mp1) == 0) {
                //deactivate
                WinSendDlgItemMsg(hwnd, FID_CLIENT, WM_ACTIVATE, mp1, mp2);
            }
            PID pidThis, pidPartner, pidTemp;
            TID tidPartner;
            HENUM henum;
            HWND  hwndEnum;

            WinQueryWindowProcess(hwnd, &pidThis, NULL);
            WinQueryWindowProcess(hwndFocusChange, &pidPartner, &tidPartner);

            if(pidThis != pidPartner) {
                //Gain or lose activation to window in other process
                //must send WM_ACTIVATEAPP to top-level windows

                //Iterate over all child windows of the desktop
                henum = WinBeginEnumWindows(HWND_DESKTOP);

                while(hwndEnum = WinGetNextWindow(henum))
                {
                    WinQueryWindowProcess(hwndEnum, &pidTemp, NULL);
                    if(pidTemp == pidThis)
                    {
                        SendMessageA(OS2ToWin32Handle(hwndEnum), WM_ACTIVATEAPP_W, (WPARAM)SHORT1FROMMP(mp1), (LPARAM)tidPartner);
                    }
                }
                WinEndEnumWindows(henum);
            }
            if(SHORT1FROMMP(mp1)) {
                //activate
                WinSendDlgItemMsg(hwnd, FID_CLIENT, WM_ACTIVATE, mp1, mp2);
            }

            //CB: show owner behind the dialog
            if (win32wnd->IsModalDialog())
            {
                if(win32wnd->getOwner()) {
                    Win32BaseWindow *topOwner = Win32BaseWindow::GetWindowFromHandle(win32wnd->getOwner()->GetTopParent());

                    if (topOwner) {
                        WinSetWindowPos(topOwner->getOS2FrameWindowHandle(),hwnd,0,0,0,0,SWP_ZORDER);
                        RELEASE_WNDOBJ(topOwner);
                    }
                }
            }
        }
        else
        {
            WinSetWindowUShort(hwnd,QWS_FLAGS,mp1 ? (flags | FF_ACTIVE):(flags & ~FF_ACTIVE));
        }
        rc = 0;
        break;
    }

    case WM_ENABLE:
        dprintf(("PMFRAME: WM_ENABLE %x", hwnd));
        win32wnd->MsgEnable(SHORT1FROMMP(mp1));
        break;

    case WM_SHOW:
        dprintf(("PMFRAME: WM_SHOW %x %d", hwnd, mp1));
        //show client window
        WinShowWindow(win32wnd->getOS2WindowHandle(), (BOOL)mp1);
        break;

    case WM_QUERYTRACKINFO:
    {
        PTRACKINFO trackInfo = (PTRACKINFO)mp2;

        dprintf(("PMFRAME:WM_QUERYTRACKINFO %x", win32wnd->getWindowHandle()));
        trackInfo->cxBorder = 4;
        trackInfo->cyBorder = 4;
        win32wnd->AdjustTrackInfo((PPOINT)&trackInfo->ptlMinTrackSize,(PPOINT)&trackInfo->ptlMaxTrackSize);
        rc = (MRESULT)TRUE;
        break;
    }

    case WM_QUERYBORDERSIZE:
    {
        PWPOINT size = (PWPOINT)mp1;

        dprintf(("PMFRAME:WM_QUERYBORDERSIZE %x", win32wnd->getWindowHandle()));

        size->x = 0;
        size->y = 0;
        rc = (MRESULT)TRUE;
        break;
    }

#ifdef DEBUG
    case WM_QUERYFRAMEINFO:
        dprintf(("PMFRAME:WM_QUERYFRAMEINFO %x", win32wnd->getWindowHandle()));
        goto RunDefFrameWndProc;
#endif

    case WM_FORMATFRAME:
        dprintf(("PMFRAME:WM_FORMATFRAME %x", win32wnd->getWindowHandle()));
        break;

    case WM_ADJUSTFRAMEPOS:
    {
        PSWP pswp   = (PSWP)mp1;

        dprintf(("PMFRAME:WM_ADJUSTFRAMEPOS %x %x %x (%s) (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, DbgGetStringSWPFlags(pswp->fl), pswp->x, pswp->y, pswp->cx, pswp->cy));
        //hack alert: the PM frame control changes the z-order of a child window
        //            if it receives focus after a window has been destroyed
        //            We can't let this happen as this messes up assumptions
        //            elsewhere (e.g. GetNextDlgGroupItem)
        //            By returning 0 here, we prevent the default frame handler
        //            from messing things up. (one example is a group of radio buttons)
        //NOTE: We really need to get rid of frame & client windows for each
        //      win32 window
        if(pswp->fl == SWP_FOCUSACTIVATE && win32wnd->isChild()) {
            rc = 0;
            break;
        }
        //hack alert: as we return zero as border size (check handler) we need
        //to do adjustments here as well or PM will calculate it for us and
        //result will be illegal. Btw we do not honour PM border size settings
        //and never will. I was unable to figure out why only X coordinate
        //is being broken but not Y as well.

        if ((pswp->fl & SWP_MAXIMIZE) == SWP_MAXIMIZE)
        {
            RECT rect;
            rect.left = rect.top = rect.right = rect.bottom = 0;
            win32wnd->AdjustMaximizedRect(&rect);

            pswp->x += rect.left;
        }
        goto RunDefFrameWndProc;
    }

#ifdef DEBUG
    case WM_OWNERPOSCHANGE:
    {
        PSWP pswp   = (PSWP)mp1;

        dprintf(("PMFRAME:WM_OWNERPOSCHANGE %x %x %x (%s) (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, DbgGetStringSWPFlags(pswp->fl), pswp->x, pswp->y, pswp->cx, pswp->cy));
        RestoreOS2TIB();
        rc = pfnFrameWndProc(hwnd, msg, mp1, mp2);
        SetWin32TIB();
        dprintf(("PMFRAME: DEF WM_OWNERPOSCHANGE %x %x %x (%s) (%d,%d) (%d,%d)", win32wnd->getWindowHandle(), pswp->hwnd, pswp->fl, DbgGetStringSWPFlags(pswp->fl), pswp->x, pswp->y, pswp->cx, pswp->cy));
        break;
    }
#endif

    case WM_MINMAXFRAME:
    {
        PSWP swp = (PSWP)mp1;

        if (!win32wnd->IsWindowCreated()) goto RunDefWndProc;

        dprintf(("PMFRAME:WM_MINMAXFRAME %x",hwnd));
        if ((swp->fl & SWP_MAXIMIZE) == SWP_MAXIMIZE)
        {
            // MDI frame windows are not common PM windows so we need to
            // drop-out WHOLE chain of WM_X commands with SWP_MAXIMIZE flag
            // finally last WM_WINDOWPOSCHANGED will take care of maximization

            if (win32wnd->getExStyle() & WS_EX_MDICHILD_W) {
               rc = 0;
               break;
            }

            RECT rect;
            rect.left = rect.top = rect.right = rect.bottom = 0;
            win32wnd->AdjustMaximizedRect(&rect);

            swp->x += rect.left;
            swp->cx += rect.right-rect.left;
            swp->y -= rect.bottom;
            swp->cy += rect.bottom-rect.top;

            win32wnd->ShowWindow(SW_RESTORE_W);
        }
        else
        if ((swp->fl & SWP_MINIMIZE) == SWP_MINIMIZE)
        {
            win32wnd->setStyle((win32wnd->getStyle() & ~WS_MAXIMIZE_W) | WS_MINIMIZE_W);
        }
        else
        if ((swp->fl & SWP_RESTORE) == SWP_RESTORE)
        {
            win32wnd->setStyle(win32wnd->getStyle() & ~(WS_MINIMIZE_W | WS_MAXIMIZE_W));
        }
        goto RunDefWndProc;
    }

#ifdef DEBUG
    case WM_UPDATEFRAME:
        dprintf(("PMFRAME:WM_UPDATEFRAME %x", win32wnd->getWindowHandle()));
        goto RunDefFrameWndProc;
#endif

    case WM_TRACKFRAME:
        dprintf(("PMFRAME: WM_TRACKFRAME %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        if(fOS2Look) {//sent by titlebar control
            Frame_SysCommandSizeMove(win32wnd, SC_MOVE_W+HTCAPTION_W);
        }
        rc = 0;
        break;

    case WM_SYSCOMMAND:
        dprintf(("PMFRAME: WM_SYSCOMMAND %x %x %x", win32wnd->getWindowHandle(), mp1, mp2));
        if (fOS2Look == OS2_APPEARANCE_SYSMENU && mp1 == (MPARAM)OSSC_SYSMENU)
            goto RunDefFrameWndProc;

        if(win32wnd->getWindowHandle() != pWinMsg->hwnd) {
            RELEASE_WNDOBJ(win32wnd);
            win32wnd = Win32BaseWindow::GetWindowFromHandle(pWinMsg->hwnd);
        }
        if(win32wnd)
            win32wnd->DispatchMsgA(pWinMsg);
        break;

    case WM_IMEREQUEST:
    case WM_IMECONTROL:
    case WM_IMENOTIFY:
        if( pWinMsg->message )
        {
            dprintf(("Frame window received IME message"));
            win32wnd->DispatchMsgA( pWinMsg );

            if(( pWinMsg->message = WM_IME_NOTIFY_W ) &&
               ( pWinMsg->wParam == IMN_SETOPENSTATUS_W ))
            {
                MSG m;

                m.message = WM_IME_NOTIFY_W;
                m.wParam = IMN_SETCONVERSIONMODE_W;
                m.lParam = 0;

                win32wnd->DispatchMsgA( &m );
            }
        }
        else
            goto RunDefWndProc;
        break;

#ifdef DEBUG
    case WM_DDE_INITIATE:
    case WM_DDE_INITIATEACK:
    case WM_DDE_REQUEST:
    case WM_DDE_ACK:
    case WM_DDE_DATA:
    case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE:
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
    case WM_DDE_TERMINATE:
         dprintf(("PMFRAME: WM_DDE %x %x", msg, win32wnd->getWindowHandle()));
         break;
#endif

    default:
        goto RunDefFrameWndProc;
    }
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    ODIN_UnsetExceptionHandler(&exceptRegRec);

#ifdef DEBUG
    dbg_ThreadPopCall();
#endif
    return (MRESULT)rc;

RunDefFrameWndProc:
    dprintf2(("RunDefFrameWndProc"));
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    ODIN_UnsetExceptionHandler(&exceptRegRec);

#ifdef DEBUG
    dbg_ThreadPopCall();
#endif
    return pfnFrameWndProc(hwnd, msg, mp1, mp2);

RunDefWndProc:
    dprintf2(("RunDefWndProc"));
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    ODIN_UnsetExceptionHandler(&exceptRegRec);

    //calling WinDefWindowProc here breaks Opera hotlist window (WM_ADJUSTWINDOWPOS)
//    return pfnFrameWndProc(hwnd, msg, mp1, mp2);

#ifdef DEBUG
    dbg_ThreadPopCall();
#endif
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
//******************************************************************************
//******************************************************************************
MRESULT EXPENTRY Win32FakeWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PFNWP            pfnOldWindowProc;
    Win32BaseWindow *win32wnd, *win32wndchild;
    TEB             *teb;
    MRESULT          rc = 0;
    EXCEPTIONREGISTRATIONRECORD exceptRegRec = {0,0};

    //Restore our FS selector
    SetWin32TIB();

    win32wnd = Win32FakeWindow::GetWindowFromOS2Handle(hwnd);
    if(win32wnd == NULL) {
        DebugInt3();
        goto RunDefWndProc;
    }

    pfnOldWindowProc = (PFNWP)win32wnd->getOldPMWindowProc();
    if(pfnOldWindowProc == NULL) {
        DebugInt3();
        goto RunDefWndProc;
    }

    RestoreOS2TIB();
    rc = pfnOldWindowProc(hwnd, msg, mp1, mp2);

    ODIN_SetExceptionHandler(&exceptRegRec);
    SetWin32TIB();
    switch(msg) {
    case WM_WINDOWPOSCHANGED:
    {
        PSWP      pswp    = (PSWP)mp1,pswpOld = pswp+1;
        SWP       swpOld  = *(pswp + 1);
        WINDOWPOS wp;

        if(win32wnd->getParent()) {
             OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, win32wnd->getParent()->getClientHeight(),
                                    hwnd);
        }
        else OSLibMapSWPtoWINDOWPOS(pswp, &wp, &swpOld, OSLibQueryScreenHeight(), hwnd);

        win32wnd->SetWindowPos(wp.hwndInsertAfter, wp.x, wp.y, wp.cx, wp.cy, wp.flags);
        break;
    }

    }
    if(win32wnd) RELEASE_WNDOBJ(win32wnd);
    RestoreOS2TIB();
    ODIN_UnsetExceptionHandler(&exceptRegRec);
    return rc;

RunDefWndProc:
    RestoreOS2TIB();
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}
//******************************************************************************
// PMWinSubclassFakeWindow
//
// Subclass a fake window (converted PM window)
//
// Parameters
//
//     HWND hwndOS2     - PM handle of fake window
//
// Returns
//     NULL         - Failure
//     else                     - Old PM window procedure
//
//******************************************************************************
PVOID PMWinSubclassFakeWindow(HWND hwndOS2)
{
    return WinSubclassWindow(hwndOS2, Win32FakeWindowProc);
}
//******************************************************************************
//******************************************************************************
void FrameSetFocus(HWND hwnd)
{
    HWND hwndFocusSave = WinQueryWindowULong(hwnd, QWL_HWNDFOCUSSAVE);
    if(!WinIsWindow(hab, hwndFocusSave)) {
        hwndFocusSave = WinWindowFromID(hwnd, FID_CLIENT);
        WinSetWindowULong(hwnd, QWL_HWNDFOCUSSAVE, hwndFocusSave);
    }
    dprintf(("FrameSetFocus: hwndFocusSave %x %x", OS2ToWin32Handle(hwndFocusSave), hwndFocusSave));
    dprintf(("FrameSetFocus: cur focus=%x (%x) cur active=%x (%x)\n",
             WinQueryFocus(HWND_DESKTOP),        OS2ToWin32Handle(WinQueryFocus(HWND_DESKTOP)),
             WinQueryActiveWindow(HWND_DESKTOP), OS2ToWin32Handle(WinQueryActiveWindow(HWND_DESKTOP))));
    WinSetFocus(HWND_DESKTOP, hwndFocusSave);

    ULONG ulFrameFlags  = WinQueryWindowUShort(hwnd, QWS_FLAGS);
    ulFrameFlags &= ~FF_NOACTIVATESWP;
    WinSetWindowUShort(hwnd, QWS_FLAGS, ulFrameFlags);
}
//******************************************************************************
//******************************************************************************
void FrameReplaceMenuItem(HWND hwndMenu, ULONG nIndex, ULONG idOld, ULONG   idNew,
                          HBITMAP hbmNew)
{
    MENUITEM mi;

    if (!hwndMenu)
        return;

    WinEnableWindowUpdate(hwndMenu, FALSE);

    if (WinSendMsg(hwndMenu, MM_QUERYITEM, MPFROM2SHORT(idOld, TRUE), MPFROMP(&mi)))
    {
        WinSendMsg(hwndMenu, MM_REMOVEITEM, (MPARAM)idOld, 0);
        mi.afStyle     = MIS_BITMAP | MIS_SYSCOMMAND;
        mi.afAttribute = 0;
        mi.hwndSubMenu = 0;
        mi.id    = idNew;
        mi.hItem = (ULONG)hbmNew;
        WinSendMsg(hwndMenu, MM_INSERTITEM, (MPARAM)&mi, 0);
    }
    else
     dprintf(("WARNING: FrameReplaceMenuItem control %x not found",idOld));

    WinEnableWindowUpdate(hwndMenu, TRUE);

    WinInvalidateRect(hwndMenu, NULL, TRUE);
}
//******************************************************************************
//******************************************************************************
void RecalcVisibleRegion(Win32BaseWindow *win32wnd)
{
        //Workaround for PM/GPI bug when moving/sizing a window with open DCs
        //
        //Windows applictions often get a DC and keep it open for the duration
        //of the application. When the DC's window is moved (full window dragging on)
        //PM/GPI doesn't seem to update the DC properly/in time.
        //This can result is visible distortions on the screen.

        //We need to reset our DC (transformation & y-inversion) (#945)
        //Debugging showed that querying the visible region of a DC will cure
        //this problem (GPI probably recalculates the visible region). (#334)

        int  nrdcs = 0;
        HDC  hdcWindow[MAX_OPENDCS];

        if(win32wnd->queryOpenDCs(hdcWindow, MAX_OPENDCS, &nrdcs))
        {
            for(int i=0;i<nrdcs;i++)
            {
                dprintf(("Recalc visible region of DC %x for window %x", hdcWindow[i], win32wnd->getWindowHandle()));

                pDCData pHps = (pDCData)GpiQueryDCData (hdcWindow[i]);

                if(pHps) setPageXForm (win32wnd, pHps);
                else     DebugInt3();
            }
        }
        //Workaround END
}
//******************************************************************************
//******************************************************************************
static char *PMDragExtractFiles(PDRAGINFO pDragInfo, ULONG *pcItems, ULONG *pulBytes)
{
    PDRAGITEM pDragItem;
    int       i, cItems;
    BOOL      ret;
    char      szFileName[CCHMAXPATH];
    char      szContainerName[CCHMAXPATH];
    ULONG     ulBytes;
    char     *pszCurFile = NULL;

    /* Get access to the DRAGINFO data structure */
    if(!DrgAccessDraginfo(pDragInfo)) {
        return NULL;
    }

    cItems = DrgQueryDragitemCount(pDragInfo);

    //compute memory required to hold all filenames
    int bufsize = 0;
    for (i = 0; i < cItems; i++) {
        pDragItem = DrgQueryDragitemPtr(pDragInfo, i);

        bufsize += DrgQueryStrNameLen(pDragItem->hstrContainerName) + DrgQueryStrNameLen(pDragItem->hstrSourceName);
        bufsize++;  //0 terminator
        bufsize++;  //+ potential missing backslash
    }
    bufsize++;  //extra 0 terminator
    char *pszFiles = (char *)malloc(bufsize);
    if(pszFiles == NULL) {
        dprintf(("Out of memory!!"));
        DebugInt3();
        goto failure;
    }
    memset(pszFiles, 0, bufsize);

    pszCurFile = pszFiles;

    //copy all filenames
    for (i = 0; i < cItems; i++) {
        char *pszTemp = pszCurFile;

        pDragItem = DrgQueryDragitemPtr(pDragInfo, i);

        ulBytes = DrgQueryStrNameLen(pDragItem->hstrContainerName);
        ulBytes = DrgQueryStrName(pDragItem->hstrContainerName,
                                  ulBytes, pszCurFile);
        if(pszCurFile[ulBytes-1] != '\\') {
            pszCurFile[ulBytes] = '\\';
            pszCurFile++;
        }
        pszCurFile += ulBytes;

        ulBytes = DrgQueryStrNameLen(pDragItem->hstrSourceName);
        ulBytes = DrgQueryStrName(pDragItem->hstrSourceName,
                                  ulBytes+1, pszCurFile);
        pszCurFile += ulBytes + 1;  //+ terminator

        dprintf(("dropped file %s", pszTemp));
    }

    /* Release the draginfo data structure */
    DrgFreeDraginfo(pDragInfo);

    *pulBytes = bufsize;
    *pcItems  = cItems;

    return pszFiles;

failure:
    /* Release the draginfo data structure */
    DrgFreeDraginfo(pDragInfo);
    if(pszFiles) {
        free(pszFiles);
    }
    return NULL;
}
//******************************************************************************
//******************************************************************************
static BOOL PMDragValidate(PDRAGINFO pDragInfo)
{
    PDRAGITEM pDragItem;
    ULONG     ulBytes;
    int       i, cItems;
    BOOL      ret;
    char      szFileName[CCHMAXPATH];
    char      szContainerName[CCHMAXPATH];
    USHORT    usOp = DO_MOVE;

    /* Get access to the DRAGINFO data structure */
    if(!DrgAccessDraginfo(pDragInfo)) {
        return FALSE;
    }

    /* Can we accept this drop? */
    switch (pDragInfo->usOperation) {
    /* Return DOR_NODROPOP if current operation */
    /* is link or unknown                       */
    case DO_LINK:
    case DO_COPY:
    case DO_UNKNOWN:
        goto failure;

    /* Our default operation is Move */
    case DO_MOVE:
    case DO_DEFAULT:
        pDragItem = DrgQueryDragitemPtr(pDragInfo, 0);
        ulBytes   = DrgQueryStrName(pDragItem->hstrContainerName,
                                    sizeof(szContainerName),
                                    szContainerName);
        ulBytes   = DrgQueryStrName(pDragItem->hstrSourceName,
                                    sizeof(szFileName),
                                    szFileName);
        if (!ulBytes) {
            goto failure;
        }

        dprintf(("dropped file %s%s", szContainerName, szFileName));
        break;
    }

    cItems = DrgQueryDragitemCount(pDragInfo);

    /* Now, we need to look at each item in turn */
    for (i = 0; i < cItems; i++) {
        pDragItem = DrgQueryDragitemPtr(pDragInfo, i);

        /* Make sure we can move for a Move request */
        if (!((pDragItem->fsSupportedOps & DO_MOVEABLE)   &&
           (usOp == (USHORT)DO_MOVE)))
        {
            dprintf(("item %d not accepted", i));
            goto failure;
        }
    }
    /* Release the draginfo data structure */
    DrgFreeDraginfo(pDragInfo);
    return TRUE;

failure:
    DrgFreeDraginfo(pDragInfo);
    return FALSE;
}

//******************************************************************************
// Override std class names we use in Odin (necessary for keyboard hook)
//******************************************************************************
void WIN32API SetCustomStdClassName(LPSTR pszStdClassName)
{
   strcpy(WIN32_STDCLASS, pszStdClassName);
}
//******************************************************************************
//******************************************************************************
char *WIN32API QueryCustomStdClassName()
{
   return WIN32_STDCLASS;
}
//******************************************************************************
//******************************************************************************

#ifdef DEBUG_LOGGING
static char *DbgGetStringSWPFlags(ULONG flags)
{
    static char szSWPFlags[512];

    szSWPFlags[0] = 0;

    if(flags & SWP_SIZE) {
        strcat(szSWPFlags, "SWP_SIZE ");
    }
    if(flags & SWP_MOVE) {
        strcat(szSWPFlags, "SWP_MOVE ");
    }
    if(flags & SWP_ZORDER) {
        strcat(szSWPFlags, "SWP_ZORDER ");
    }
    if(flags & SWP_SHOW) {
        strcat(szSWPFlags, "SWP_SHOW ");
    }
    if(flags & SWP_HIDE) {
        strcat(szSWPFlags, "SWP_HIDE ");
    }
    if(flags & SWP_NOREDRAW) {
        strcat(szSWPFlags, "SWP_NOREDRAW ");
    }
    if(flags & SWP_NOADJUST) {
        strcat(szSWPFlags, "SWP_NOADJUST ");
    }
    if(flags & SWP_ACTIVATE) {
        strcat(szSWPFlags, "SWP_ACTIVATE ");
    }
    if(flags & SWP_DEACTIVATE) {
        strcat(szSWPFlags, "SWP_DEACTIVATE ");
    }
    if(flags & SWP_EXTSTATECHANGE) {
        strcat(szSWPFlags, "SWP_EXTSTATECHANGE ");
    }
    if(flags & SWP_MINIMIZE) {
        strcat(szSWPFlags, "SWP_MINIMIZE ");
    }
    if(flags & SWP_MAXIMIZE) {
        strcat(szSWPFlags, "SWP_MAXIMIZE ");
    }
    if(flags & SWP_RESTORE) {
        strcat(szSWPFlags, "SWP_RESTORE ");
    }
    if(flags & SWP_FOCUSACTIVATE) {
        strcat(szSWPFlags, "SWP_FOCUSACTIVATE ");
    }
    if(flags & SWP_FOCUSDEACTIVATE) {
        strcat(szSWPFlags, "SWP_FOCUSDEACTIVATE ");
    }
    if(flags & SWP_NOAUTOCLOSE) {
        strcat(szSWPFlags, "SWP_NOAUTOCLOSE ");
    }
    return szSWPFlags;
}
static char *DbgPrintQFCFlags(ULONG flags)
{
    static char szQFCFlags[64];

    szQFCFlags[0] = 0;

    if(flags & QFC_NEXTINCHAIN) {
        strcat(szQFCFlags, "QFC_NEXTINCHAIN");
    }
    else
    if(flags & QFC_ACTIVE) {
        strcat(szQFCFlags, "QFC_ACTIVE");
    }
    else
    if(flags & QFC_FRAME) {
        strcat(szQFCFlags, "QFC_FRAME");
    }
    else
    if(flags & QFC_SELECTACTIVE) {
        strcat(szQFCFlags, "QFC_SELECTACTIVE");
    }
    else
    if(flags & QFC_PARTOFCHAIN) {
        strcat(szQFCFlags, "QFC_PARTOFCHAIN");
    }

    return szQFCFlags;
}
#endif

