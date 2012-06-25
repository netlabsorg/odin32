/* $Id: win32wfake.cpp,v 1.5 2003-04-24 17:20:42 sandervl Exp $ */
/*
 * Win32 Fake Window Class for OS/2
 *
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@innotek.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <dbglog.h>
#include <win/winproc.h>
#include "win32wbase.h"
#include "win32wfake.h"
#include "oslibwin.h"
#include "win32wndhandle.h"
#include "pmwindow.h"

#define DBG_LOCALLOG	DBG_win32wfake
#include "dbglocal.h"

/** The Odin HWND Property - must be unique for this (custom)build. */
char Win32FakeWindow::szPropOdinHandle[64] = "OdinFakeHWND";
/** Flag which tells if we've inited or not. */
BOOL Win32FakeWindow::fInited = FALSE;

/**
 * Creates a fake Odin window for an existing OS/2 window.
 *
 * @returns odin handle to the fake window.
 * @returns NULL on failure.
 * @param   hwndOS2     Window handle.
 * @param   classAtom   The atom of the Odin window class.
 * @author  Sander van Leeuwen <sandervl@innotek.de>
 * @author  knut st. osmundsen <bird@anduin.net>
 */
HWND WIN32API CreateFakeWindowEx(HWND  hwndOS2, ATOM  classAtom)
{
    Win32FakeWindow *window;

    /*
     * Check if already faked - fail.
     */
    window = Win32FakeWindow::GetWindowFromOS2Handle(hwndOS2);
    if (window)
    {
        dprintf(("CreateFakeWindowEx: already faked!!! (hwndOS2=%x hwndOdin=%x)",
                 hwndOS2, window->getWindowHandle()));
        return window->getWindowHandle();
    }

    /*
     * Now proceed.
     */
    window = new Win32FakeWindow(hwndOS2, classAtom);
    if (window == NULL)
    {
        dprintf(("Win32FakeWindow creation failed for %x!!!", hwndOS2));
        return 0;
    }
    HWND hwnd = window->getWindowHandle();

    // set myself as last active popup / window
    window->setLastActive( hwnd );

    RELEASE_WNDOBJ(window);
    dprintf(("CreateFakeWindowEx: created %x for %x", hwnd, hwndOS2));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyFakeWindow(HWND hwnd)
{
    Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if (!window) {
        dprintf(("DestroyFakeWindow, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    delete window;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32FakeWindow::Win32FakeWindow(HWND hwndOS2, ATOM classAtom)
                     : Win32BaseWindow()
{
    if (!fInited)  init();

    OS2Hwnd = OS2HwndFrame = hwndOS2;

    /* Find the window class */
    windowClass = Win32WndClass::FindClass(NULL, (LPSTR)classAtom);
    if (!windowClass)
    {
        char buffer[32];
        GlobalGetAtomNameA( classAtom, buffer, sizeof(buffer) );
        dprintf(("Bad class '%s'", buffer ));
        DebugInt3();
    }

    //Allocate window words
    nrUserWindowBytes = windowClass->getExtraWndBytes();
    if(nrUserWindowBytes) {
        userWindowBytes = (char *)_smalloc(nrUserWindowBytes);
        memset(userWindowBytes, 0, nrUserWindowBytes);
    }

    WINPROC_SetProc((HWINDOWPROC *)&win32wndproc, windowClass->getWindowProc((isUnicode) ? WNDPROC_UNICODE : WNDPROC_ASCII), WINPROC_GetProcType((HWINDOWPROC)windowClass->getWindowProc((isUnicode) ? WNDPROC_UNICODE : WNDPROC_ASCII)), WIN_PROC_WINDOW);
    hInstance  = NULL;
    dwStyle    = WS_VISIBLE;
    dwOldStyle = dwStyle;
    dwExStyle  = 0;

    //We pretend this window has no parent and its position is in screen coordinates
    OSLibWinQueryWindowClientRect(OS2Hwnd, &rectClient);
    OSLibQueryWindowRectAbsolute (OS2Hwnd, &rectWindow);

    //Store the window handle as a property of the OS/2 window.
    if (!OSLibSetProperty(OS2Hwnd, szPropOdinHandle, (void*)getWindowHandle(), 0))
    {
        dprintf(("%s: f**k! WinSetProperty(%x,%s,%x,0) failed!!!", __FUNCTION__,
                 hwndOS2, szPropOdinHandle, getWindowHandle()));
    }

    //See if parent window is either odin or fake window.
    Win32BaseWindow * pParent = NULL;
    HWND hwndParentOS2 = OSLibWinQueryWindow(OS2Hwnd, QWOS_PARENT);
    if (hwndParentOS2 != OSLIB_HWND_DESKTOP)
    {
        pParent = Win32FakeWindow::GetWindowFromOS2Handle(hwndParentOS2);
        if (!pParent)
            pParent = Win32BaseWindow::GetWindowFromOS2Handle(hwndParentOS2);
        if (pParent)
        {
            setParent(pParent);
            dprintf(("%s: hwndParentOS2=%x %s window, set as parent.", __FUNCTION__,
                     hwndParentOS2, pParent->isFakeWindow() ? "fake" : "odin"));
            dwStyle |= WS_CHILD;
        }
        else
        {
            dprintf(("%s: hwndParentOS2=%x but not a fake nor odin window.", __FUNCTION__, hwndParentOS2));
        }
    }

    //See if owner widnow is either odin or fake window.
    HWND hwndOwnerOS2 = OSLibWinQueryWindow(OS2Hwnd, QWOS_OWNER);
    if (hwndOwnerOS2 != OSLIB_HWND_DESKTOP)
    {
        Win32BaseWindow * pOwner;
        if (hwndOwnerOS2 == hwndParentOS2)
            pOwner = pParent;
        else
        {
            pOwner = Win32FakeWindow::GetWindowFromOS2Handle(hwndOwnerOS2);
            if (!pOwner)
                pOwner = Win32BaseWindow::GetWindowFromOS2Handle(hwndOwnerOS2);
        }
        if (pOwner)
        {
            setOwner(pOwner);
            dprintf(("%s: hwndOwnerOS2=%x %s window, set as owner.", __FUNCTION__,
                     hwndOwnerOS2, pOwner->isFakeWindow() ? "fake" : "odin"));
            dwStyle |= WS_CHILD;
        }
        else
        {
            dprintf(("%s: hwndOwnerOS2=%x but not a fake nor odin window.", __FUNCTION__, hwndOwnerOS2));
        }
    }


    setOldPMWindowProc(PMWinSubclassFakeWindow(OS2Hwnd));
}
//******************************************************************************
//******************************************************************************
Win32FakeWindow::~Win32FakeWindow()
{
    OSLibRemoveProperty(OS2Hwnd, szPropOdinHandle);
}
//******************************************************************************
//******************************************************************************
PRECT Win32FakeWindow::getWindowRect()
{
    //the coordinates can change without us being notified, so recheck every time

    Win32BaseWindow * pParent = getParent();
    if (pParent)
    {
        SWP     swp;
        if (OSLibWinQueryWindowPos(OS2Hwnd, &swp))
        {
            ULONG y = pParent->getWindowHeight();
            rectWindow.left   = swp.x;
            rectWindow.right  = swp.x + swp.cx;
            rectWindow.top    = y - swp.y - swp.cy;
            rectWindow.bottom = y - swp.y;
        }
    }
    else
        OSLibQueryWindowRectAbsolute(OS2Hwnd, &rectWindow);

    return &rectWindow;
}
//******************************************************************************
//******************************************************************************
BOOL Win32FakeWindow::SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx,
                                   int cy, UINT fuFlags, BOOL fShowWindow)
{
    // we don't trust input either.
    dprintf(("Win32FakeWindow::SetWindowPos; don't change window position, just update internal position"));

    OSLibWinQueryWindowClientRect(OS2Hwnd, &rectClient);
    getWindowRect();
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32FakeWindow::isFakeWindow()
{
    return TRUE;
}
//******************************************************************************
//******************************************************************************

/**
 * Locates window in linked list and increases reference count if found.
 * The window object msut be unreferenced after usage.
 *
 * @returns Pointer to fake window.
 * @returns NULL if not found.
 * @param   hwndOS2     OS/2 window handle of the fake window.
 */
Win32FakeWindow *Win32FakeWindow::GetWindowFromOS2Handle(HWND hwndOS2)
{
    if (!fInited)    init();

    HWND hwnd = (HWND)OSLibQueryProperty(hwndOS2, szPropOdinHandle);
    Win32BaseWindow *pWindow = GetWindowFromHandle(hwnd);
    if (pWindow)
    {
        if (pWindow->isFakeWindow())
            return (Win32FakeWindow*)pWindow;
        DebugInt3();
    }
    dprintf2(("Win32FakeWindow::GetWindowFromOS2Handle(%x) -> null", hwndOS2));
    return NULL;
}


/**
 * Init the global(s).
 * It will make the name property unique among the odin instances.
 *
 * This will chagne szPropOdinHandle and fInited.
 */
void     Win32FakeWindow::init()
{
    if (fInited)
        return;

    /*
     * Get the name of this dll.
     */
    extern int _System DosQueryModFromEIP(HMODULE *phMod, ULONG *pObjNum, ULONG BuffLen, PCHAR pBuff, ULONG *pOffset, ULONG Address);
    char        szModName[260];
    HMODULE     hmod;
    ULONG       iObj, offObj;
    USHORT      sel = GetFS();

    int rc = DosQueryModFromEIP(&hmod, &iObj, sizeof(szModName), &szModName[0], &offObj, (ULONG)Win32FakeWindow::init);
    SetFS(sel);

    if (rc)
    {
        dprintf(("Win32FakeWindow::init: DosQueryModFromEIP failed!!!"));
        DebugInt3();
        return;
    }
    char *psz = strrchr(szModName, '\\');
    if (!psz)
        psz = szModName;
    if (strchr(szModName, '.'))
        *strchr(szModName, '.') = '\0';

    fInited = 1;
    strcat(szPropOdinHandle, psz);
    dprintf(("Win32FakeWindow::init: szPropOdinHandle='%s'", szPropOdinHandle));
}

