/* $Id: win32wbase.cpp,v 1.291 2001-10-17 14:30:09 sandervl Exp $ */
/*
 * Win32 Window Base Class for OS/2
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 * Copyright 1999-2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Parts based on Wine Windows code (windows\win.c)
 *   Corel version: corel20000212
 *
 * Copyright 1993, 1994, 1996 Alexandre Julliard
 *           1995 Alex Korobka
 *
 * TODO: Not thread/process safe
 *
 * NOTE: To access a window object, you must call GetWindowFromOS2Handle or
 *       GetWindowFromHandle. Both these methods increase the reference count
 *       of the object. When you're done with the object, you MUST call
 *       the release method!
 *       This mechanism prevents premature destruction of objects when there
 *       are still clients using it.
 *
 * NOTE: Client rectangle always relative to frame window
 *       Window rectangle in parent coordinates (relative to parent's client window)
 *       (screen coord. if no parent)
 *
 * NOTE: Status of window:
 *       Before a window has processed WM_NCCREATE:
 *       - GetTopWindow can't return that window handle
 *       - GetWindow(parent, GW_CHILD) can't return that window handle
 *       - IsChild works
 *       TODO: Does this affect more functions?? (other GetWindow ops)
 *       (verified in NT4, SP6)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <win.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <heapstring.h>
#include <winuser32.h>
#include "win32wbase.h"
#include "wndmsg.h"
#include "oslibwin.h"
#include "oslibmsg.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"
#include "dc.h"
#include "win32wdesktop.h"
#include "pmwindow.h"
#include "controls.h"
#include <wprocess.h>
#include <win\hook.h>
#include <menu.h>
#define INCL_TIMERWIN32
#include "timer.h"

#define DBG_LOCALLOG    DBG_win32wbase
#include "dbglocal.h"

/* bits in the dwKeyData */
#define KEYDATA_ALT         0x2000
#define KEYDATA_PREVSTATE   0x4000

void PrintWindowStyle(DWORD dwStyle, DWORD dwExStyle);

static fDestroyAll = FALSE;
//For quick lookup of current process id
static ULONG currentProcessId = -1;
static int iF10Key = 0;
static int iMenuSysKey = 0;

//******************************************************************************
//******************************************************************************
Win32BaseWindow::Win32BaseWindow() 
                     : GenericObject(&windows, &critsect), ChildWindow(&critsect)
{
  Init();
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow::Win32BaseWindow(HWND hwndOS2, ULONG reserved)
                     : GenericObject(&windows, &critsect), ChildWindow(&critsect)
{
  Init();
  OS2Hwnd = hwndOS2;
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow::Win32BaseWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
                     : GenericObject(&windows, &critsect), ChildWindow(&critsect)
{
  Init();
  this->isUnicode = isUnicode;
  CreateWindowExA(lpCreateStructA, classAtom);
}
//******************************************************************************
//******************************************************************************
void Win32BaseWindow::Init()
{
  isUnicode        = FALSE;
  fFirstShow       = TRUE;
  fIsDialog        = FALSE;
  fIsModalDialogOwner = FALSE;
  OS2HwndModalDialog  = 0;
  fInternalMsg     = FALSE;
  fParentChange    = FALSE;
  fDestroyWindowCalled = FALSE;
  fTaskList        = FALSE;
  fParentDC        = FALSE;
  fComingToTop     = FALSE;
  fMinMaxChange    = FALSE;
  fVisibleRegionChanged = FALSE;
  fEraseBkgndFlag  = TRUE;

  state            = STATE_INIT;
  windowNameA      = NULL;
  windowNameW      = NULL;

  userWindowBytes  = NULL;;
  nrUserWindowBytes= 0;

  OS2Hwnd          = 0;
  OS2HwndFrame     = 0;
  hSysMenu         = 0;
  Win32Hwnd        = 0;

  if(HwAllocateWindowHandle(&Win32Hwnd, (ULONG)this) == FALSE)
  {
        dprintf(("Win32BaseWindow::Init HwAllocateWindowHandle failed!!"));
        DebugInt3();
  }

  posx = posy      = 0;
  width = height   = 0;

  dwExStyle        = 0;
  dwStyle          = 0;
  dwOldStyle       = 0;
  win32wndproc     = 0;
  hInstance        = 0;
  dwIDMenu         = 0; //0xFFFFFFFF; //default -1
  userData         = 0;
  contextHelpId    = 0;
  hotkey           = 0;


  hwndLinkAfter    = HWND_BOTTOM;
  flags            = 0;
  lastHitTestVal   = HTOS_NORMAL;
  owner            = NULL;
  windowClass      = 0;

  hIcon            = 0;
  hIconSm          = 0;

  horzScrollInfo   = NULL;
  vertScrollInfo   = NULL;

  propertyList     = NULL;

  cbExtra          = 0;
  pExtra           = NULL;

  ownDC              = 0;
  hWindowRegion      = 0;
  hClipRegion        = 0;

  hTaskList          = 0;

  if(currentProcessId == -1)
  {
        currentProcessId = GetCurrentProcessId();
  }
  dwThreadId         = GetCurrentThreadId();
  dwProcessId        = currentProcessId;

  memset(&windowpos, 0, sizeof(windowpos));
  //min and max position are initially -1 (verified in NT4, SP6)
  windowpos.ptMinPosition.x = -1;
  windowpos.ptMinPosition.y = -1;
  windowpos.ptMaxPosition.x = -1;
  windowpos.ptMaxPosition.y = -1;

  lpVisRgnNotifyProc  = NULL;
  dwVisRgnNotifyParam = NULL;
}
//******************************************************************************
//todo get rid of resources (menu, icon etc)
//******************************************************************************
Win32BaseWindow::~Win32BaseWindow()
{
    if(getRefCount() < 0) {
        DebugInt3();
    }

    if(hTaskList) {
        OSLibWinRemoveFromTasklist(hTaskList);
    }

    OSLibWinSetVisibleRegionNotify(OS2Hwnd, FALSE);
    OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, 0);
    OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, 0);

    if(fDestroyAll) {
        dprintf(("Destroying window %x %s", getWindowHandle(), windowNameA));
        setParent(NULL);  //or else we'll crash in the dtor of the ChildWindow class
    }
    else
    if(getParent() && getParent()->getFirstChild() == this && getNextChild() == NULL)
    {
        //if we're the last child that's being destroyed and our
        //parent window was also destroyed, then we 
        if(getParent()->IsWindowDestroyed())
        {
            setParent(NULL);  //or else we'll crash in the dtor of the ChildWindow class
        }
    }
    /* Decrement class window counter */
    if(windowClass) {
        RELEASE_CLASSOBJ(windowClass);
    }

    if(isOwnDC())
        releaseOwnDC(ownDC);

    if(Win32Hwnd)
        HwFreeWindowHandle(Win32Hwnd);

    if(userWindowBytes)
        free(userWindowBytes);

    if(windowNameA) {
        free(windowNameA);
        windowNameA = NULL;
    }
    if(windowNameW) {
        free(windowNameW);
        windowNameW = NULL;
    }
    if(vertScrollInfo) {
        free(vertScrollInfo);
        vertScrollInfo = NULL;
    }
    if(horzScrollInfo) {
        free(horzScrollInfo);
        horzScrollInfo = NULL;
    }
    if(propertyList) {
        removeWindowProps();
    }
    Win32BaseWindow *wndparent = (Win32BaseWindow *)ChildWindow::getParentOfChild();
    if(wndparent && !fDestroyAll) {
        RELEASE_WNDOBJ(wndparent);
    }
    if(owner && !fDestroyAll) {
        RELEASE_WNDOBJ(owner);
    }
    if(windowClass) {
        RELEASE_CLASSOBJ(windowClass);
    }
}
//******************************************************************************
//******************************************************************************
void Win32BaseWindow::DestroyAll()
{
    fDestroyAll = TRUE;
    GenericObject::DestroyAll(windows);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::isChild()
{
    return ((dwStyle & WS_CHILD) != 0);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsWindowUnicode()
{
    dprintf2(("IsWindowUnicode %x %d", getWindowHandle(), WINPROC_GetProcType(getWindowProc()) == WIN_PROC_32W));
    return (WINPROC_GetProcType(getWindowProc()) == WIN_PROC_32W);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::CreateWindowExA(CREATESTRUCTA *cs, ATOM classAtom)
{
 char  buffer[256];

#ifdef DEBUG
    PrintWindowStyle(cs->style, cs->dwExStyle);
#endif

    //If window has no owner/parent window, then it will be added to the tasklist
    //(depending on visibility state)
    if (!cs->hwndParent) fTaskList = TRUE;

    sw = SW_SHOW;
    SetLastError(0);

    /* Find the parent window */
    if (cs->hwndParent)
    {
            Win32BaseWindow *window = GetWindowFromHandle(cs->hwndParent);
            if(!window) {
                    dprintf(("Bad parent %04x\n", cs->hwndParent ));
                    SetLastError(ERROR_INVALID_PARAMETER);
                    return FALSE;
            }
            /* Make sure parent is valid */
            if (!window->IsWindow() )
            {
                    RELEASE_WNDOBJ(window);
                    dprintf(("Bad parent %04x\n", cs->hwndParent ));
                    SetLastError(ERROR_INVALID_PARAMETER);
                    return FALSE;
            }
            RELEASE_WNDOBJ(window);
            /* Windows does this for overlapped windows
             * (I don't know about other styles.) */
            if (cs->hwndParent == GetDesktopWindow() && (!(cs->style & WS_CHILD) || (cs->style & WS_POPUP)))
            {
                    cs->hwndParent = 0;
            }
    }
    else
    if ((cs->style & WS_CHILD) && !(cs->style & WS_POPUP)) {
            dprintf(("No parent for child window" ));
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;  /* WS_CHILD needs a parent, but WS_POPUP doesn't */
    }

    /* Find the window class */
    windowClass = Win32WndClass::FindClass(cs->hInstance, (LPSTR)classAtom);
    if (!windowClass)
    {
        GlobalGetAtomNameA( classAtom, buffer, sizeof(buffer) );
        dprintf(("Bad class '%s'", buffer ));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

#ifdef DEBUG
    if(HIWORD(cs->lpszClass))
    {
         if(isUnicode) dprintf(("Window class %ls", cs->lpszClass));
         else          dprintf(("Window class %s", cs->lpszClass));
    }
    else dprintf(("Window class %x", cs->lpszClass));
#endif

    /* Fix the lpszClass field: from existing programs, it seems ok to call a CreateWindowXXX
     * with an atom as the class name, put some programs expect to have a *REAL* string in
     * lpszClass when the CREATESTRUCT is sent with WM_CREATE
     */
    if (!HIWORD(cs->lpszClass) ) {
        if (isUnicode) {
                GlobalGetAtomNameW( classAtom, (LPWSTR)buffer, sizeof(buffer) );
        }
        else {
                GlobalGetAtomNameA( classAtom, buffer, sizeof(buffer) );
        }
        cs->lpszClass = buffer;
    }

    /* Fix the coordinates */
    fXDefault = FALSE;
    fCXDefault = FALSE;
    if ((cs->x == CW_USEDEFAULT) || (cs->x == CW_USEDEFAULT16))
    {
       /* Never believe Microsoft's documentation... CreateWindowEx doc says
        * that if an overlapped window is created with WS_VISIBLE style bit
        * set and the x parameter is set to CW_USEDEFAULT, the system ignores
        * the y parameter. However, disassembling NT implementation (WIN32K.SYS)
        * reveals that
        *
        * 1) not only if checks for CW_USEDEFAULT but also for CW_USEDEFAULT16
        * 2) it does not ignore the y parameter as the docs claim; instead, it
        *    uses it as second parameter to ShowWindow() unless y is either
        *    CW_USEDEFAULT or CW_USEDEFAULT16.
        *
        * The fact that we didn't do 2) caused bogus windows pop up when wine
        * was running apps that were using this obscure feature. Example -
        * calc.exe that comes with Win98 (only Win98, it's different from
        * the one that comes with Win95 and NT)
        */
        if ((cs->y != CW_USEDEFAULT) && (cs->y != CW_USEDEFAULT16)) sw = cs->y;

        /* We have saved cs->y, now we can trash it */
        cs->x = 0;
        cs->y = 0;
        fXDefault = TRUE;
    }
    if ((cs->cx == CW_USEDEFAULT) || (cs->cx == CW_USEDEFAULT16))
    {
        cs->cx = 600; /* FIXME */
        cs->cy = 400;
        fCXDefault = TRUE;
    }
    if (cs->style & (WS_POPUP | WS_CHILD))
    {
        fXDefault = FALSE;
        if (fCXDefault)
        {
            fCXDefault = FALSE;
            cs->cx = cs->cy = 0;
        }
    }
    if (fXDefault && !fCXDefault) fXDefault = FALSE; //CB: only x positioning doesn't work (calc.exe,cdrlabel.exe)

    //Allocate window words
    nrUserWindowBytes = windowClass->getExtraWndBytes();
    if(nrUserWindowBytes) {
        userWindowBytes = (char *)_smalloc(nrUserWindowBytes);
        memset(userWindowBytes, 0, nrUserWindowBytes);
    }

    if ((cs->style & WS_CHILD) && cs->hwndParent)
    {
        SetParent(cs->hwndParent);
//        owner = GetWindowFromHandle(cs->hwndParent);
        owner = 0;
/*        if(owner == NULL)
        {
            dprintf(("HwGetWindowHandleData couldn't find owner window %x!!!", cs->hwndParent));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return FALSE;
        }*/
        //SvL: Shell positioning shouldn't be done for child windows! (breaks Notes)
        fXDefault = fCXDefault = FALSE;
    }
    else
    {
        SetParent(0);
        if (!cs->hwndParent || (cs->hwndParent == windowDesktop->getWindowHandle())) {
            owner = NULL;
        }
        else
        {
            Win32BaseWindow *wndparent = GetWindowFromHandle(cs->hwndParent); 
            if(wndparent) {
                 owner = GetWindowFromHandle(wndparent->GetTopParent());
                 RELEASE_WNDOBJ(wndparent);
            }
            else owner = NULL;        

            if(owner == NULL)
            {
                dprintf(("HwGetWindowHandleData couldn't find owner window %x!!!", cs->hwndParent));
                SetLastError(ERROR_INVALID_WINDOW_HANDLE);
                return FALSE;
            }
        }
    }

    WINPROC_SetProc((HWINDOWPROC *)&win32wndproc, windowClass->getWindowProc(), WINPROC_GetProcType(windowClass->getWindowProc()), WIN_PROC_WINDOW);
    hInstance  = cs->hInstance;
    dwStyle    = cs->style & ~WS_VISIBLE;
    dwOldStyle = dwStyle;
    dwExStyle  = cs->dwExStyle;

    hwndLinkAfter = ((cs->style & (WS_CHILD|WS_MAXIMIZE)) == WS_CHILD) ? HWND_BOTTOM : HWND_TOP;

    /* Correct the window style */
    if (!(cs->style & WS_CHILD))
    {
        dwStyle |= WS_CLIPSIBLINGS;
        if (!(cs->style & WS_POPUP))
        {
            dwStyle |= WS_CAPTION;
            flags |= WIN_NEED_SIZE;
        }
    }
    if (cs->dwExStyle & WS_EX_DLGMODALFRAME) dwStyle &= ~WS_THICKFRAME;

    //copy pointer of CREATESTRUCT for usage in MsgCreate method
    tmpcs = cs;

    //Store our window object pointer in thread local memory, so PMWINDOW.CPP can retrieve it
    TEB *teb = GetThreadTEB();
    if(teb == NULL) {
        dprintf(("Window creation failed - teb == NULL")); //this is VERY bad
        ExitProcess(666);
        return FALSE;
    }

    teb->o.odin.newWindow = (ULONG)this;

    DWORD dwOSWinStyle, dwOSFrameStyle;

    OSLibWinConvertStyle(dwStyle,dwExStyle,&dwOSWinStyle, &dwOSFrameStyle);

    OS2Hwnd = OSLibWinCreateWindow((getParent()) ? getParent()->getOS2WindowHandle() : OSLIB_HWND_DESKTOP,
                                   dwOSWinStyle, dwOSFrameStyle, (char *)windowNameA,
                                   (owner) ? owner->getOS2WindowHandle() : ((getParent()) ? getParent()->getOS2WindowHandle() : OSLIB_HWND_DESKTOP),
                                   (hwndLinkAfter == HWND_BOTTOM) ? TRUE : FALSE,
                                   0, fTaskList,fXDefault | fCXDefault,windowClass->getStyle(), &OS2HwndFrame);
    if(OS2Hwnd == 0) {
        dprintf(("Window creation failed!! OS LastError %0x", OSLibWinGetLastError()));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
    }
    OSLibWinSetVisibleRegionNotify(OS2Hwnd, TRUE);
    state = STATE_CREATED;
    SetLastError(0);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::MsgCreate(HWND hwndOS2)
{
 CREATESTRUCTA *cs = tmpcs;  //pointer to CREATESTRUCT used in CreateWindowExA method
 POINT          maxSize, maxPos, minTrack, maxTrack;
 HWND           hwnd = getWindowHandle();
 LRESULT (* CALLBACK localSend32)(HWND, UINT, WPARAM, LPARAM);

    OS2Hwnd      = hwndOS2;

    if(OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, getWindowHandle()) == FALSE) {
        dprintf(("WM_CREATE: WinSetWindowULong %X failed!!", OS2Hwnd));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
    }
    if(OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, WIN32PM_MAGIC) == FALSE) {
        dprintf(("WM_CREATE: WinSetWindowULong2 %X failed!!", OS2Hwnd));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
    }
    if(OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32FLAGS, 0) == FALSE) {
        dprintf(("WM_CREATE: WinSetWindowULong2 %X failed!!", OS2Hwnd));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
    }

    if (HOOK_IsHooked( WH_CBT ))
    {
        CBT_CREATEWNDA cbtc;
        LRESULT ret;

        cbtc.lpcs = cs;
        cbtc.hwndInsertAfter = hwndLinkAfter;
        ret = (isUnicode) ? HOOK_CallHooksW(WH_CBT, HCBT_CREATEWND, getWindowHandle(), (LPARAM)&cbtc)
                          : HOOK_CallHooksA(WH_CBT, HCBT_CREATEWND, getWindowHandle(), (LPARAM)&cbtc);
        if(ret)
        {
            dprintf(("CBT-hook returned 0!!"));
            SetLastError(ERROR_CAN_NOT_COMPLETE); //todo: wrong error
            return FALSE;
        }
        //todo: if hook changes parent, we need to do so too!!!!!!!!!!
    }

    if (cs->style & WS_HSCROLL)
    {
        horzScrollInfo = (SCROLLBAR_INFO*)malloc(sizeof(SCROLLBAR_INFO));
        horzScrollInfo->MinVal = horzScrollInfo->CurVal = horzScrollInfo->Page = 0;
        horzScrollInfo->MaxVal = 100;
        horzScrollInfo->flags  = ESB_ENABLE_BOTH;
    }

    if (cs->style & WS_VSCROLL)
    {
        vertScrollInfo = (SCROLLBAR_INFO*)malloc(sizeof(SCROLLBAR_INFO));
        vertScrollInfo->MinVal = vertScrollInfo->CurVal = vertScrollInfo->Page = 0;
        vertScrollInfo->MaxVal = 100;
        vertScrollInfo->flags  = ESB_ENABLE_BOTH;
    }

    if(HIWORD(cs->lpszName))
    {
        if (!isUnicode)
        {
            int wndNameLength = strlen(cs->lpszName);
            windowNameA = (LPSTR)_smalloc(wndNameLength+1);
            strcpy(windowNameA,cs->lpszName);
            windowNameW = (LPWSTR)_smalloc((wndNameLength+1)*sizeof(WCHAR));
            lstrcpyAtoW(windowNameW,windowNameA);
            windowNameA[wndNameLength] = 0;
            windowNameW[wndNameLength] = 0;
        }
        else
        {
            // Wide
            int wndNameLength = lstrlenW((LPWSTR)cs->lpszName);
            windowNameW = (LPWSTR)_smalloc((wndNameLength+1)*sizeof(WCHAR));
            lstrcpyW(windowNameW,(LPWSTR)cs->lpszName);
            windowNameW[lstrlenW((LPWSTR)cs->lpszName)] = 0; // need ?
            // Ascii
            LPSTR tmp = HEAP_strdupWtoA(GetProcessHeap(), 0, (LPWSTR)cs->lpszName);
            if(tmp) {
                long tmpLength = strlen( tmp );
                windowNameA = (LPSTR)_smalloc(tmpLength+1);
                strcpy(windowNameA,tmp);
                windowNameA[tmpLength] = 0; // need ?
                HEAP_free(tmp);
            } else {
                windowNameA = (LPSTR)_smalloc(1);
                windowNameA[0] = 0;
            }
        }
        if(fOS2Look) {
            OSLibWinSetTitleBarText(OS2HwndFrame, windowNameA);
        }
    }

//SvL: This completely messes up MS Word 97 (no button bar, no menu)
#if 0
  //adjust CW_USEDEFAULT position
  if (fXDefault | fCXDefault)
  {
    RECT rect;

    //SvL: Returns invalid rectangle (not the expected shell default size)
    OSLibWinQueryWindowRect(OS2Hwnd,&rect,RELATIVE_TO_SCREEN);
    if (getParent()) mapWin32Rect(OSLIB_HWND_DESKTOP,getParent()->getOS2WindowHandle(),&rect);
    if (fXDefault)
    {
      cs->x = rect.left;
      cs->y = rect.top;
      if (!fCXDefault)
      {
        //CB: todo: adjust pos to screen rect
      }
    }
    if (fCXDefault)
    {
      cs->cx = rect.right-rect.left;
      cs->cy = rect.bottom-rect.top;
    }
  }
#endif

    fakeWinBase.hwndThis     = OS2Hwnd;
    fakeWinBase.pWindowClass = windowClass;

    //Set icon from window or class
    if (hIcon)
        OSLibWinSetIcon(OS2HwndFrame,hIcon);
    else
    if (windowClass->getIcon())
        OSLibWinSetIcon(OS2HwndFrame,windowClass->getIcon());

    /* Get class or window DC if needed */
    if(windowClass->getStyle() & CS_OWNDC) {
        dprintf(("Class with CS_OWNDC style"));
        ownDC = GetDCEx(getWindowHandle(), NULL, DCX_USESTYLE);
    }
    else
    if (windowClass->getStyle() & CS_PARENTDC)  {
        fParentDC = TRUE;
        ownDC = 0;
    }
    else
    if (windowClass->getStyle() & CS_CLASSDC)  {
        dprintf(("WARNING: Class with CS_CLASSDC style!"));
        //not a good solution, but it's a bit difficult to share a single
        //DC among different windows... DevOpenDC apparently can't be used
        //for window DCs and WinOpenWindowDC must be associated with a window
        ownDC = GetDCEx(getWindowHandle(), NULL, DCX_USESTYLE);
    }
    /* Set the window menu */
    if ((dwStyle & (WS_CAPTION | WS_CHILD)) == WS_CAPTION )
    {
        if (cs->hMenu) {
            ::SetMenu(getWindowHandle(), cs->hMenu);
        }
        else {
                if (windowClass->getMenuNameA()) {
                        cs->hMenu = LoadMenuA(windowClass->getInstance(),windowClass->getMenuNameA());
#if 0 //CB: hack for treeview test cases bug
if (!cs->hMenu) cs->hMenu = LoadMenuA(windowClass->getInstance(),"MYAPP");
#endif
                        if (cs->hMenu) ::SetMenu(getWindowHandle(), cs->hMenu );
                }
        }
    }
    else
    {
        setWindowId((DWORD)cs->hMenu);
    }
    hSysMenu = (dwStyle & WS_SYSMENU) ? MENU_GetSysMenu(Win32Hwnd,0):0;

    /* Send the WM_GETMINMAXINFO message and fix the size if needed */
    if ((cs->style & WS_THICKFRAME) || !(cs->style & (WS_POPUP | WS_CHILD)))
    {
        GetMinMaxInfo(&maxSize, &maxPos, &minTrack, &maxTrack);
        if (maxSize.x < cs->cx) cs->cx = maxSize.x;
        if (maxSize.y < cs->cy) cs->cy = maxSize.y;
        if (cs->cx < minTrack.x) cs->cx = minTrack.x;
        if (cs->cy < minTrack.y) cs->cy = minTrack.y;
    }

    if(cs->style & WS_CHILD)
    {
        if(cs->cx < 0) cs->cx = 0;
        if(cs->cy < 0) cs->cy = 0;
    }
    else
    {
        if (cs->cx <= 0) cs->cx = 1;
        if (cs->cy <= 0) cs->cy = 1;
    }

    //set client & window rectangles from CreateWindowEx CREATESTRUCT
    rectWindow.left = cs->x;
    rectWindow.right = cs->x+cs->cx;
    rectWindow.top = cs->y;
    rectWindow.bottom = cs->y+cs->cy;
    rectClient = rectWindow;
    OffsetRect(&rectClient, -rectClient.left, -rectClient.top);

    /* Send the WM_CREATE message
     * Perhaps we shouldn't allow width/height changes as well.
     * See p327 in "Internals".
     */
    maxPos.x = rectWindow.left; maxPos.y = rectWindow.top;

    if(fTaskList) {
        hTaskList = OSLibWinAddToTaskList(OS2HwndFrame, windowNameA, (cs->style & WS_VISIBLE) ? 1 : 0);
    }

    localSend32 = (isUnicode) ? ::SendMessageW : ::SendMessageA;

    state = STATE_PRE_WMNCCREATE;
    if(localSend32(getWindowHandle(), WM_NCCREATE,0,(LPARAM)cs))
    {
        RECT tmpRect;

        //CB: recheck flags
        if (cs->style & (WS_POPUP | WS_CHILD))
        {
            fXDefault = FALSE;
            if (fCXDefault)
            {
                fCXDefault = FALSE;
                cs->cx = cs->cy = 0;
                rectWindow.right  = rectWindow.left;
                rectWindow.bottom = rectWindow.top;
            }
        }
        tmpRect = rectWindow;
        state   = STATE_POST_WMNCCREATE;

        //set the window size and update the client
        SetWindowPos(hwndLinkAfter, tmpRect.left, tmpRect.top, tmpRect.right-tmpRect.left, tmpRect.bottom-tmpRect.top,SWP_NOACTIVATE | SWP_NOREDRAW | SWP_FRAMECHANGED);

        state = STATE_PRE_WMCREATE;
        if (cs->style & WS_VISIBLE) dwStyle |= WS_VISIBLE; //program could change position in WM_CREATE
        if( (localSend32(getWindowHandle(), WM_CREATE, 0, (LPARAM)cs )) != -1 )
        {
            state = STATE_POST_WMCREATE;

            if(!(flags & WIN_NEED_SIZE))
            {
                SendInternalMessageA(WM_SIZE, SIZE_RESTORED,
                                MAKELONG(rectClient.right-rectClient.left,
                                         rectClient.bottom-rectClient.top));

                if(!::IsWindow(hwnd))
                {
                    dprintf(("Createwindow: WM_SIZE destroyed window"));
                    goto end;
                }
                SendInternalMessageA(WM_MOVE,0,MAKELONG(rectClient.left,rectClient.top));
                if(!::IsWindow(hwnd))
                {
                    dprintf(("Createwindow: WM_MOVE destroyed window"));
                    goto end;
                }
            }
            if (getStyle() & (WS_MINIMIZE | WS_MAXIMIZE))
            {
                RECT newPos;
                UINT swFlag = (getStyle() & WS_MINIMIZE) ? SW_MINIMIZE : SW_MAXIMIZE;
                setStyle(getStyle() & ~(WS_MAXIMIZE | WS_MINIMIZE));
                MinMaximize(swFlag, &newPos);
                swFlag = ((getStyle() & WS_CHILD) || GetActiveWindow()) ? SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED
                                                                        : SWP_NOZORDER | SWP_FRAMECHANGED;
                SetWindowPos(0, newPos.left, newPos.top,  newPos.right, newPos.bottom, swFlag);
                if(!::IsWindow(hwnd))
                {
                    dprintf(("Createwindow: min/max destroyed window"));
                    goto end;
                }
            }

            if( (getStyle() & WS_CHILD) && !(getExStyle() & WS_EX_NOPARENTNOTIFY) )
            {
                /* Notify the parent window only */
                if(getParent() && getParent()->IsWindowDestroyed() == FALSE)
                {
                    getParent()->SendInternalMessageA(WM_PARENTNOTIFY, MAKEWPARAM(WM_CREATE, getWindowId()), (LPARAM)getWindowHandle());
                }
                if(!::IsWindow(hwnd))
                {
                    dprintf(("Createwindow: WM_PARENTNOTIFY destroyed window"));
                    goto end;
                }
            }

            if(cs->style & WS_VISIBLE) {
                dwStyle &= ~WS_VISIBLE;
                ShowWindow(sw);
            }

            /* Call WH_SHELL hook */
            if (!(getStyle() & WS_CHILD) && !owner)
                HOOK_CallHooksA(WH_SHELL, HSHELL_WINDOWCREATED, getWindowHandle(), 0);

            SetLastError(0);
            return TRUE;
        }
    }
    dprintf(("Window creation FAILED (NCCREATE cancelled creation)"));
    SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
end:
    return FALSE;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgQuit()
{
  return SendInternalMessageA(WM_QUIT, 0, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgClose()
{
  return SendInternalMessageA(WM_CLOSE,0,0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgDestroy()
{
 ULONG rc;
 Win32BaseWindow *child;
 HWND hwnd = getWindowHandle();

    state = STATE_DESTROYED;

    if(fDestroyWindowCalled == FALSE)
    {//this window was destroyed because DestroyWindow was called for it's parent
     //so: send a WM_PARENTNOTIFY now as that hasn't happened yet
        if((getStyle() & WS_CHILD) && !(getExStyle() & WS_EX_NOPARENTNOTIFY))
        {
            if(getParent() && getParent()->IsWindowDestroyed() == FALSE)
            {
                    /* Notify the parent window only */
                    getParent()->SendMessageA(WM_PARENTNOTIFY, MAKEWPARAM(WM_DESTROY, getWindowId()), (LPARAM)getWindowHandle());
            }
////            else    DebugInt3();
        }
    }
    SendInternalMessageA(WM_DESTROY, 0, 0);
    if(::IsWindow(hwnd) == FALSE) {
        //object already destroyed, so return immediately
        return 1;
    }
    SendInternalMessageA(WM_NCDESTROY, 0, 0);

    TIMER_KillTimerFromWindow(getWindowHandle());

    if(getRefCount() == 0 && getFirstChild() == NULL && state == STATE_CREATED) {
        delete this;
    }
    else {
        //make sure no message can ever arrive for this window again (PM or from other win32 windows)
        dprintf(("Mark window %x (%x) as deleted", getWindowHandle(), this));
        markDeleted();
        OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, 0);
        OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, 0);
        if(Win32Hwnd) {
            HwFreeWindowHandle(Win32Hwnd);
            Win32Hwnd = 0;
        }
    }
    return 1;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgEnable(BOOL fEnable)
{
    if(fEnable) {
         dwStyle &= ~WS_DISABLED;
    }
    else dwStyle |= WS_DISABLED;

    return SendInternalMessageA(WM_ENABLE, fEnable, 0);
}
//******************************************************************************
//TODO: SW_PARENTCLOSING/OPENING flag (lParam)
//******************************************************************************
ULONG Win32BaseWindow::MsgShow(BOOL fShow)
{
    if(!CanReceiveSizeMsgs() || fDestroyWindowCalled) {
        return 1;
    }

    if(fShow) {
         setStyle(getStyle() | WS_VISIBLE);
         if(getStyle() & WS_MINIMIZE) {
            return ShowWindow(SW_RESTORE);
         }
    }
    else setStyle(getStyle() & ~WS_VISIBLE);

    //already sent from ShowWindow
////    return SendInternalMessageA(WM_SHOWWINDOW, fShow, 0);
    return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgPosChanging(LPARAM lp)
{
    //SvL: Notes crashes when switching views (calls DestroyWindow -> PM sends
    //     a WM_WINDOWPOSCHANGED msg -> crash)
    if(!CanReceiveSizeMsgs() || fDestroyWindowCalled)
        return 0;

    return SendInternalMessageA(WM_WINDOWPOSCHANGING, 0, lp);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgPosChanged(LPARAM lp)
{
    //SvL: Notes crashes when switching views (calls DestroyWindow -> PM sends
    //     a WM_WINDOWPOSCHANGED msg -> crash)
    if(!CanReceiveSizeMsgs() || fDestroyWindowCalled)
        return 1;

    return SendInternalMessageA(WM_WINDOWPOSCHANGED, 0, lp);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgScroll(ULONG msg, ULONG scrollCode, ULONG scrollPos)
{
  //According to the SDK docs, the scrollbar handle (lParam) is 0 when the standard
  //window scrollbars send these messages
  return SendInternalMessageA(msg, MAKELONG(scrollCode, scrollPos), 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgActivate(BOOL fActivate, BOOL fMinimized, HWND hwnd, HWND hwndOS2Win)
{
 ULONG rc, procidhwnd = -1, threadidhwnd = 0;

    //SvL: Don't send WM_(NC)ACTIVATE messages when the window is being destroyed
    if(fDestroyWindowCalled) {
        return 0;
    }

    //According to SDK docs, if app returns FALSE & window is being deactivated,
    //default processing is cancelled
    //TODO: According to Wine we should proceed anyway if window is sysmodal
    if(SendInternalMessageA(WM_NCACTIVATE, fActivate, 0) == FALSE && !fActivate)
    {
        dprintf(("WARNING: WM_NCACTIVATE return code = FALSE -> cancel processing"));
        return 0;
    }
    /* child windows get a WM_CHILDACTIVATE message */
    if((getStyle() & (WS_CHILD | WS_POPUP)) == WS_CHILD )
    {
        if(fActivate) {//WM_CHILDACTIVE is for activation only
            SendInternalMessageA(WM_CHILDACTIVATE, 0, 0L);
        }
        return 0;
    }

    rc = SendInternalMessageA(WM_ACTIVATE, MAKELONG((fActivate) ? WA_ACTIVE : WA_INACTIVE, fMinimized), hwnd);

    if(hwndOS2Win) {
        threadidhwnd = O32_GetWindowThreadProcessId(hwndOS2Win, &procidhwnd);
    }

    if(fActivate) {
         SendInternalMessageA(WM_ACTIVATEAPP, 1, dwThreadId);    //activate; specify window thread id
    }
    else SendInternalMessageA(WM_ACTIVATEAPP, 0, threadidhwnd);  //deactivate; specify thread id of other process
    return rc;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgChildActivate(BOOL fActivate)
{
    //SvL: Don't send WM_(NC)ACTIVATE messages when the window is being destroyed
    if(fDestroyWindowCalled) {
        return 0;
    }

    //According to SDK docs, if app returns FALSE & window is being deactivated,
    //default processing is cancelled
    //TODO: According to Wine we should proceed anyway if window is sysmodal
    if(SendInternalMessageA(WM_NCACTIVATE, fActivate, 0) == FALSE && !fActivate)
    {
        dprintf(("WARNING: WM_NCACTIVATE return code = FALSE -> cancel processing"));
        return 0;
    }
    /* child windows get a WM_CHILDACTIVATE message */
    if((getStyle() & (WS_CHILD | WS_POPUP)) == WS_CHILD )
    {
        if(fActivate) {//WM_CHILDACTIVE is for activation only
            SendInternalMessageA(WM_CHILDACTIVATE, 0, 0L);
        }
        return 0;
    }
    DebugInt3();
    return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::DispatchMsgA(MSG *msg)
{
    return SendInternalMessageA(msg->message, msg->wParam, msg->lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::DispatchMsgW(MSG *msg)
{
    return SendInternalMessageW(msg->message, msg->wParam, msg->lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgSetFocus(HWND hwnd)
{
    //SvL: Don't send WM_(NC)ACTIVATE messages when the window is being destroyed
    if(fDestroyWindowCalled) {
        return 0;
    }

    return  SendInternalMessageA(WM_SETFOCUS, hwnd, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgKillFocus(HWND hwnd)
{
    //SvL: Don't send WM_(NC)ACTIVATE messages when the window is being destroyed
    if(fDestroyWindowCalled) {
        return 0;
    }
    return  SendInternalMessageA(WM_KILLFOCUS, hwnd, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgButton(MSG *msg)
{
 BOOL  fClick = FALSE;

//    dprintf(("MsgButton at (%d,%d)", msg->pt.x, msg->pt.y));
    switch(msg->message) {
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
                if (!(windowClass && windowClass->getClassLongA(GCL_STYLE) & CS_DBLCLKS))
                {
                    msg->message = msg->message - (WM_LBUTTONDBLCLK - WM_LBUTTONDOWN); //dblclick -> down
                    MsgButton(msg);
                    msg->message++; //button-up
                    return MsgButton(msg);
                }
                break;
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCMBUTTONDBLCLK:
                //Docs say CS_DBLCLKS style doesn't matter for non-client double clicks
                fClick = TRUE;
                break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_NCLBUTTONDOWN:
        case WM_NCRBUTTONDOWN:
        case WM_NCMBUTTONDOWN:
                fClick = TRUE;
                break;
    }

    if(fClick)
    {
      HWND hwndTop;

        /* Activate the window if needed */
        hwndTop = GetTopParent();

        HWND hwndActive = GetActiveWindow();
        if (hwndTop && (getWindowHandle() != hwndActive))
        {
                LONG ret = SendInternalMessageA(WM_MOUSEACTIVATE, hwndTop,
                                                MAKELONG( lastHitTestVal, msg->message) );

                dprintf2(("WM_MOUSEACTIVATE returned %d", ret));
#if 0
                if ((ret == MA_ACTIVATEANDEAT) || (ret == MA_NOACTIVATEANDEAT))
                         eatMsg = TRUE;
#endif
                //SvL: 0 is not documented, but experiments in NT4 show that
                //     the window will get activated when it returns this.
                //     (FreeCell is an example)
                if(((ret == MA_ACTIVATE) || (ret == MA_ACTIVATEANDEAT) || (ret == 0))
                   && (hwndTop != GetForegroundWindow()) )
                {
                    Win32BaseWindow *win32top = Win32BaseWindow::GetWindowFromHandle(hwndTop);

                    //SvL: Calling OSLibSetActiveWindow(hwndTop); causes focus problems
                    if (win32top) {
                        OSLibWinSetFocus(win32top->getOS2FrameWindowHandle());
                        RELEASE_WNDOBJ(win32top);
                    }
                }
        }
    }

    SendInternalMessageA(WM_SETCURSOR, getWindowHandle(), MAKELONG(lastHitTestVal, msg->message));

    return  SendInternalMessageA(msg->message, msg->wParam, msg->lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgPaint(ULONG tmp, ULONG select)
{
    if (select && IsWindowIconic())
        return SendInternalMessageA(WM_PAINTICON, 1, 0);
    else
        return SendInternalMessageA(WM_PAINT, 0, 0);
}
//******************************************************************************
//TODO: Is the clipper region of the window DC equal to the invalidated rectangle?
//      (or are we simply erasing too much here)
//******************************************************************************
ULONG Win32BaseWindow::MsgEraseBackGround(HDC hdc)
{
    ULONG rc;
    HDC   hdcErase = hdc;

    if (hdcErase == 0)
        hdcErase = GetDC(getWindowHandle());

    if(IsWindowIconic())
        rc = SendInternalMessageA(WM_ICONERASEBKGND, hdcErase, 0);
    else
        rc = SendInternalMessageA(WM_ERASEBKGND, hdcErase, 0);
    if (hdc == 0)
        ReleaseDC(getWindowHandle(), hdcErase);
    return (rc);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgMouseMove(MSG *msg)
{
    //TODO: hiword should be 0 if window enters menu mode (SDK docs)
    SendInternalMessageA(WM_SETCURSOR, Win32Hwnd, MAKELONG(lastHitTestVal, msg->message));

    //translated message == WM_(NC)MOUSEMOVE
    return SendInternalMessageA(msg->message, msg->wParam, msg->lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgChar(MSG *msg)
{
    return DispatchMsgA(msg);
}
//******************************************************************************
//TODO: Should use update region, not rectangle
//******************************************************************************
ULONG Win32BaseWindow::MsgNCPaint(PRECT pUpdateRect)
{
    HRGN hrgn;
    ULONG rc;
    RECT client = rectClient;

    if ((pUpdateRect->left >= client.left) && (pUpdateRect->left < client.right) &&
       (pUpdateRect->right >= client.left) && (pUpdateRect->right < client.right) &&
       (pUpdateRect->top  >= client.top) && (pUpdateRect->top < client.bottom) &&
       (pUpdateRect->bottom >= client.top) && (pUpdateRect->bottom < client.bottom))
    {
        return 0;
    }

    dprintf(("MsgNCPaint (%d,%d)(%d,%d)", pUpdateRect->left, pUpdateRect->top, pUpdateRect->right, pUpdateRect->bottom));
    hrgn = CreateRectRgnIndirect(pUpdateRect);

    rc = SendInternalMessageA(WM_NCPAINT, hrgn, 0);

    DeleteObject(hrgn);

    return rc;
}
//******************************************************************************
//Called when either the frame's size or position has changed (lpWndPos != NULL)
//or when the frame layout has changed (i.e. scrollbars added/removed) (lpWndPos == NULL)
//******************************************************************************
ULONG Win32BaseWindow::MsgFormatFrame(WINDOWPOS *lpWndPos)
{
  RECT oldWindowRect = rectWindow, client = rectClient, newWindowRect;
  RECT newClientRect;
  WINDOWPOS wndPos;
  ULONG rc;

    if(lpWndPos)
    {
        //set new window rectangle
        setWindowRect(lpWndPos->x, lpWndPos->y, lpWndPos->x+lpWndPos->cx,
                      lpWndPos->y+lpWndPos->cy);
        newWindowRect = rectWindow;
    }
    else {
        wndPos.hwnd  = getWindowHandle();
        wndPos.hwndInsertAfter = 0;
        newWindowRect= rectWindow;
        wndPos.x     = newWindowRect.left;
        wndPos.y     = newWindowRect.top;
        wndPos.cx    = newWindowRect.right - newWindowRect.left;
        wndPos.cy    = newWindowRect.bottom - newWindowRect.top;
        wndPos.flags = SWP_FRAMECHANGED;
        lpWndPos     = &wndPos;
    }

    newClientRect = rectClient;
    rc = SendNCCalcSize(TRUE, &newWindowRect,  &oldWindowRect, &client, lpWndPos, &newClientRect);
    rectClient = newClientRect; //must update rectClient here

    dprintf(("MsgFormatFrame: old client rect (%d,%d)(%d,%d), new client (%d,%d)(%d,%d)", client.left, client.top, client.right, client.bottom, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom));
    dprintf(("MsgFormatFrame: old window rect (%d,%d)(%d,%d), new window (%d,%d)(%d,%d)", oldWindowRect.left, oldWindowRect.top, oldWindowRect.right, oldWindowRect.bottom, rectWindow.left, rectWindow.top, rectWindow.right, rectWindow.bottom));

    if(!CanReceiveSizeMsgs() || !EqualRect(&client, &rectClient)) {
        OSLibWinSetClientPos(getOS2WindowHandle(), rectClient.left, rectClient.top, getClientWidth(), getClientHeight(), getWindowHeight());
    }

#if 1
//this doesn't always work
//  if(CanReceiveSizeMsgs() && (client.left != rectClient.left || client.top != rectClient.top))
  if(CanReceiveSizeMsgs() && ((oldWindowRect.right - oldWindowRect.left < rectClient.left
                  || oldWindowRect.bottom - oldWindowRect.top < rectClient.top) ||
     (EqualRect(&oldWindowRect, &rectWindow) && (client.left != rectClient.left || client.top != rectClient.top))))
  {
   Win32BaseWindow *child = (Win32BaseWindow *)getFirstChild();

    //client rectangle has moved -> inform children
    dprintf(("MsgFormatFrame -> client rectangle has changed, move children"));
    while(child) {
        ::SetWindowPos(child->getWindowHandle(),
                               HWND_TOP, child->getWindowRect()->left,
                               child->getWindowRect()->top, 0, 0,
                               SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
        child = (Win32BaseWindow *)child->getNextChild();
    }
  }
#endif
  if(fOS2Look && ((dwStyle & WS_CAPTION) == WS_CAPTION))
  {
      RECT rect = {0};
      int  height  = getWindowHeight();
      RECTLOS2 rectOS2;

      AdjustRectOuter(&rect, FALSE);

      rect.left   = -rect.left;
      rect.top    = rect.bottom - rect.top;
      rect.right  = rectWindow.right - rectWindow.left - rect.right;
 
      rectOS2.xLeft   = rect.left;
      rectOS2.xRight  = rect.right;
      rectOS2.yBottom = height - rect.top;
      rectOS2.yTop    = height - rect.bottom;
      OSLibWinPositionFrameControls(getOS2FrameWindowHandle(), &rectOS2, dwStyle, dwExStyle, IconForWindow(ICON_SMALL));
  }
  return rc;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgSetText(LPSTR lpsz, LONG cch)
{
    return SendInternalMessageA(WM_SETTEXT, 0, (LPARAM)lpsz);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgGetTextLength()
{
    return SendInternalMessageA(WM_GETTEXTLENGTH, 0, 0);
}
//******************************************************************************
//******************************************************************************
void Win32BaseWindow::MsgGetText(char *wndtext, ULONG textlength)
{
    SendInternalMessageA(WM_GETTEXT, textlength, (LPARAM)wndtext);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::isMDIClient()
{
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::isMDIChild()
{
    return FALSE;
}
//******************************************************************************
//TODO: Not complete
//******************************************************************************
BOOL Win32BaseWindow::isFrameWindow()
{
    if(getParent() == NULL)
        return TRUE;

    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::isDesktopWindow()
{
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsWindowIconic()
{
    return ((getStyle() & WS_MINIMIZE) && windowClass->getIcon());
}
//******************************************************************************
//******************************************************************************
SCROLLBAR_INFO *Win32BaseWindow::getScrollInfo(int nBar)
{
  switch(nBar)
  {
    case SB_HORZ:
      if (!horzScrollInfo)
      {
        horzScrollInfo = (SCROLLBAR_INFO*)malloc(sizeof(SCROLLBAR_INFO));
        if (!horzScrollInfo) break;
        horzScrollInfo->MinVal = horzScrollInfo->CurVal = horzScrollInfo->Page = 0;
        horzScrollInfo->MaxVal = 100;
        horzScrollInfo->flags  = ESB_ENABLE_BOTH;
      }
      return horzScrollInfo;

    case SB_VERT:
      if (!vertScrollInfo)
      {
        vertScrollInfo = (SCROLLBAR_INFO*)malloc(sizeof(SCROLLBAR_INFO));
        if (!vertScrollInfo) break;
        vertScrollInfo->MinVal = vertScrollInfo->CurVal = vertScrollInfo->Page = 0;
        vertScrollInfo->MaxVal = 100;
        vertScrollInfo->flags  = ESB_ENABLE_BOTH;
      }
      return vertScrollInfo;
  }

  return NULL;
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::DefWndControlColor(UINT ctlType, HDC hdc)
{
    //SvL: Set background color to default button color (not window (white))
    if(ctlType == CTLCOLOR_BTN)
    {
        SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        return GetSysColorBrush(COLOR_BTNFACE);
    }
    //SvL: Set background color to default dialog color if window is dialog
    if((ctlType == CTLCOLOR_DLG || ctlType == CTLCOLOR_STATIC) && IsDialog()) {
        SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        return GetSysColorBrush(COLOR_BTNFACE);
    }
    if( ctlType == CTLCOLOR_SCROLLBAR)
    {
        HBRUSH hb = GetSysColorBrush(COLOR_SCROLLBAR);
        COLORREF bk = GetSysColor(COLOR_3DHILIGHT);
        SetTextColor( hdc, GetSysColor(COLOR_3DFACE));
        SetBkColor( hdc, bk);

         /* if COLOR_WINDOW happens to be the same as COLOR_3DHILIGHT
          * we better use 0x55aa bitmap brush to make scrollbar's background
          * look different from the window background.
          */
        if (bk == GetSysColor(COLOR_WINDOW)) {
             return GetPattern55AABrush();
        }

        UnrealizeObject( hb );
        return (LRESULT)hb;
    }

    SetTextColor( hdc, GetSysColor(COLOR_WINDOWTEXT));

    if ((ctlType == CTLCOLOR_EDIT) || (ctlType == CTLCOLOR_LISTBOX))
    {
        SetBkColor( hdc, GetSysColor(COLOR_WINDOW) );
    }
    else
    {
        SetBkColor( hdc, GetSysColor(COLOR_3DFACE) );
        return (LRESULT)GetSysColorBrush(COLOR_3DFACE);
    }
    return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::DefWndPrint(HDC hdc,ULONG uFlags)
{
  /*
   * Visibility flag.
   */
  if ( (uFlags & PRF_CHECKVISIBLE) &&
       !IsWindowVisible(getWindowHandle()) )
      return 0;

  /*
   * Unimplemented flags.
   */
  if ( (uFlags & PRF_CHILDREN) ||
       (uFlags & PRF_OWNED)    ||
       (uFlags & PRF_NONCLIENT) )
  {
    dprintf(("WM_PRINT message with unsupported flags\n"));
  }

  /*
   * Background
   */
  if ( uFlags & PRF_ERASEBKGND)
    SendInternalMessageA(WM_ERASEBKGND, (WPARAM)hdc, 0);

  /*
   * Client area
   */
  if ( uFlags & PRF_CLIENT)
    SendInternalMessageA(WM_PRINTCLIENT, (WPARAM)hdc, PRF_CLIENT);


  return 0;
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::DefWindowProcA(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_CLOSE:
        dprintf(("DefWindowProcA: WM_CLOSE %x", getWindowHandle()));
        DestroyWindow();
        return 0;

    case WM_GETTEXTLENGTH:
        if(windowNameA) {
          return strlen(windowNameA);
        }
        else {
          return 0;
        }

    case WM_GETTEXT:
        if (!lParam || !wParam) return 0;
        if (!windowNameA) ((LPSTR)lParam)[0] = 0;
        else lstrcpynA((LPSTR)lParam, windowNameA, wParam);
        return min((windowNameA ? strlen(windowNameA) : 0), wParam);

    case WM_SETTEXT:
    {
        LPCSTR lpsz = (LPCSTR)lParam;

        if(windowNameA) free(windowNameA);
        if(windowNameW) free(windowNameW);
        if (lParam)
        {
            int wndNameLength = strlen(lpsz);
            windowNameA = (LPSTR)_smalloc(wndNameLength+1);
            strcpy(windowNameA, lpsz);
            windowNameW = (LPWSTR)_smalloc((wndNameLength+1)*sizeof(WCHAR));
            lstrcpyAtoW(windowNameW, windowNameA);
        }
        else
        {
            windowNameA = NULL;
            windowNameW = NULL;
        }
        dprintf(("WM_SETTEXT of %x to %s\n", Win32Hwnd, lParam));
        if ((dwStyle & WS_CAPTION) == WS_CAPTION)
        {
            HandleNCPaint((HRGN)1);
            if(hTaskList) {
                OSLibWinChangeTaskList(hTaskList, OS2HwndFrame, getWindowNameA(), (getStyle() & WS_VISIBLE) ? 1 : 0);
            }
            if(fOS2Look) {
                OSLibWinSetTitleBarText(OS2HwndFrame, getWindowNameA());
            }
        }

        return TRUE;
    }

    case WM_SETREDRAW:
    {
        if (wParam)
        {
            setStyle(getStyle() | WS_VISIBLE);
            dprintf(("Enable window update for %x", getWindowHandle()));
            OSLibWinEnableWindowUpdate(OS2HwndFrame, OS2Hwnd, TRUE);
        }
        else
        {
            if (getStyle() & WS_VISIBLE)
            {
                setStyle(getStyle() & ~WS_VISIBLE);
                dprintf(("Disable window update for %x", getWindowHandle()));
                OSLibWinEnableWindowUpdate(OS2HwndFrame, OS2Hwnd, FALSE);
            }
        }
        return 0;
    }

    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORSCROLLBAR:
        return DefWndControlColor(Msg - WM_CTLCOLORMSGBOX, (HDC)wParam);

    case WM_CTLCOLOR:
        return DefWndControlColor(HIWORD(lParam), (HDC)wParam);

    case WM_VKEYTOITEM:
    case WM_CHARTOITEM:
             return -1;

    case WM_PARENTNOTIFY:
        return 0;

    case WM_MOUSEACTIVATE:
    {
        dprintf(("DefWndProc: WM_MOUSEACTIVATE for %x Msg %s", Win32Hwnd, GetMsgText(HIWORD(lParam))));
        if(getStyle() & WS_CHILD && !(getExStyle() & WS_EX_NOPARENTNOTIFY) )
        {
            if(getParent()) {
                LRESULT rc = getParent()->SendInternalMessageA(WM_MOUSEACTIVATE, wParam, lParam );
                if(rc)  return rc;
            }
        }
        return (LOWORD(lParam) == HTCAPTION) ? MA_NOACTIVATE : MA_ACTIVATE;
    }

    case WM_ACTIVATE:
        /* The default action in Windows is to set the keyboard focus to
         * the window, if it's being activated and not minimized */
        if (LOWORD(wParam) != WA_INACTIVE) {
            if(!(getStyle() & WS_MINIMIZE))
                SetFocus(getWindowHandle());
        }
        return 0;

    case WM_SETCURSOR:
    {
        dprintf(("DefWndProc: WM_SETCURSOR for %x Msg %s", Win32Hwnd, GetMsgText(HIWORD(lParam))));
        if((getStyle() & WS_CHILD))
        {
            if(getParent()) {
                LRESULT rc = getParent()->SendInternalMessageA(WM_SETCURSOR, wParam, lParam);
                if(rc)  return rc;
            }
        }
        if (wParam == getWindowHandle())
        {
          HCURSOR hCursor;

          switch(LOWORD(lParam))
          {
            case HTCLIENT:
              hCursor = windowClass ? windowClass->getCursor():LoadCursorA(0,IDC_ARROWA);
              break;

            case HTLEFT:
            case HTRIGHT:
              hCursor = LoadCursorA(0,IDC_SIZEWEA);
              break;

            case HTTOP:
            case HTBOTTOM:
              hCursor = LoadCursorA(0,IDC_SIZENSA);
              break;

            case HTTOPLEFT:
            case HTBOTTOMRIGHT:
              hCursor = LoadCursorA(0,IDC_SIZENWSEA);
              break;

            case HTTOPRIGHT:
            case HTBOTTOMLEFT:
              hCursor = LoadCursorA(0,IDC_SIZENESWA);
              break;

            default:
              hCursor = LoadCursorA(0,IDC_ARROWA);
              break;
          }

          if (hCursor)
          {
            SetCursor(hCursor);
            return 1;
          }
          else return 0;
        }
        else return 0;
    }

    case WM_MOUSEMOVE:
        return 0;

    case WM_WINDOWPOSCHANGED:
    {
        PWINDOWPOS wpos = (PWINDOWPOS)lParam;
        WPARAM     wp   = SIZE_RESTORED;

        if (!(wpos->flags & SWP_NOMOVE) && !(wpos->flags & SWP_NOCLIENTMOVE))
        {
            SendInternalMessageA(WM_MOVE, 0, MAKELONG(rectClient.left,rectClient.top));
        }
        if (!(wpos->flags & SWP_NOSIZE) && !(wpos->flags & SWP_NOCLIENTSIZE))
        {
            if (dwStyle & WS_MAXIMIZE) wp = SIZE_MAXIMIZED;
            else
            if (dwStyle & WS_MINIMIZE) wp = SIZE_MINIMIZED;

            SendInternalMessageA(WM_SIZE, wp, MAKELONG(rectClient.right  - rectClient.left,
                                                       rectClient.bottom - rectClient.top));
        }
        return 0;
    }
    case WM_WINDOWPOSCHANGING:
        return HandleWindowPosChanging((WINDOWPOS *)lParam);

    case WM_ERASEBKGND:
    case WM_ICONERASEBKGND:
    {
      RECT rect;
      int rc;

        if (!windowClass || !windowClass->getBackgroundBrush()) return 0;

        rc = GetClipBox( (HDC)wParam, &rect );
        if ((rc == SIMPLEREGION) || (rc == COMPLEXREGION))
        {
            HBRUSH hBrush = windowClass->getBackgroundBrush();

            if (hBrush <= (HBRUSH)(SYSCOLOR_GetLastColor()+1))
                hBrush = GetSysColorBrush(hBrush-1);

            FillRect( (HDC)wParam, &rect, hBrush);
        }
        return 1;
    }

    case WM_PRINT:
        return DefWndPrint(wParam,lParam);

    case WM_PAINTICON:
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(getWindowHandle(), &ps );
        if( hdc )
        {
            if( (getStyle() & WS_MINIMIZE) && (getWindowClass()->getIcon() || hIcon))
            {
                int x = (rectWindow.right - rectWindow.left - GetSystemMetrics(SM_CXICON))/2;
                int y = (rectWindow.bottom - rectWindow.top - GetSystemMetrics(SM_CYICON))/2;
                dprintf(("Painting class icon: vis rect=(%i,%i - %i,%i)\n", ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom ));
                DrawIcon(hdc, x, y, hIcon ? hIcon:getWindowClass()->getIcon() );
            }
            EndPaint(getWindowHandle(), &ps );
        }
        return 0;
    }

    case WM_GETDLGCODE:
        return 0;

    case WM_NCPAINT:
         return HandleNCPaint((HRGN)wParam);

    case WM_NCACTIVATE:
        return HandleNCActivate(wParam);

    case WM_NCCREATE:
        return(TRUE);

    case WM_NCDESTROY:
        return 0;

    case WM_NCCALCSIZE:
        return HandleNCCalcSize((BOOL)wParam,(RECT*)lParam);

    case WM_NCLBUTTONDOWN:
        return HandleNCLButtonDown(wParam,lParam);

    case WM_LBUTTONDBLCLK:
    case WM_NCLBUTTONDBLCLK:
        return HandleNCLButtonDblClk(wParam,lParam);

    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONDBLCLK:
        if (lastHitTestVal == HTERROR) MessageBeep(MB_ICONEXCLAMATION);
        return 0;

    case WM_NCRBUTTONUP:
        return HandleNCRButtonUp(wParam,lParam);

    case WM_NCMBUTTONUP:
        return 0;

    case WM_NCHITTEST:
    {
      POINT point;
      LRESULT retvalue;

      point.x = (SHORT)LOWORD(lParam);
      point.y = (SHORT)HIWORD(lParam);

      retvalue = HandleNCHitTest(point);
#if 0 //CB: let the Corel people fix the bugs first
      if(retvalue == HTMENU)
        MENU_TrackMouseMenuBar_MouseMove(Win32Hwnd,point,TRUE);
      else
        MENU_TrackMouseMenuBar_MouseMove(Win32Hwnd,point,FALSE);
#endif
      return retvalue;
    }

    case WM_SYSCOMMAND:
    {
     POINT point;

        point.x = LOWORD(lParam);
        point.y = HIWORD(lParam);
        return HandleSysCommand(wParam,&point);
    }

    case WM_SYSKEYDOWN:
    {
	if( HIWORD(lParam) & KEYDATA_ALT )
	{
	    /* if( HIWORD(lParam) & ~KEYDATA_PREVSTATE ) */
	      if( wParam == VK_MENU && !iMenuSysKey )
		iMenuSysKey = 1;
	      else
		iMenuSysKey = 0;
	    
	    iF10Key = 0;

	    if( wParam == VK_F4 )	/* try to close the window */
	    {
                HWND top = GetTopWindow();
                if (!(GetClassLongW( top, GCL_STYLE ) & CS_NOCLOSE))
                    PostMessageW( top, WM_SYSCOMMAND, SC_CLOSE, 0 );
	    }
	} 
	else if( wParam == VK_F10 )
	        iF10Key = 1;
	     else
	        if( wParam == VK_ESCAPE && (GetKeyState(VK_SHIFT) & 0x8000))
                    SendMessageW(WM_SYSCOMMAND, SC_KEYMENU, VK_SPACE );

        Win32BaseWindow *siblingWindow;
        HWND sibling;
        char nameBuffer [40], mnemonic;
        int nameLength;

        GetWindowTextA (nameBuffer, 40);

        // search all sibling to see it this key is their mnemonic
        sibling = GetWindow (GW_HWNDFIRST);
        while (sibling != 0) {
            siblingWindow = GetWindowFromHandle (sibling);
            nameLength = siblingWindow->GetWindowTextA (nameBuffer, 40);

            // find the siblings mnemonic
            mnemonic = '\0';
            for (int i=0 ; i<nameLength ; i++) {
                if (IsDBCSLeadByte(nameBuffer[i])) {
                    // Skip DBCS
                    continue;
                }
                if (nameBuffer [i] == '&') {
                    mnemonic = nameBuffer [i+1];
                    if ((mnemonic >= 'a') && (mnemonic <= 'z'))
                        mnemonic -= 32; // make it uppercase
                    break;  // stop searching
                }
            }

            // key matches siblings mnemonic, send mouseclick
            if (mnemonic == (char) wParam) {
                siblingWindow->SendInternalMessageA (BM_CLICK, 0, 0);
            }
            sibling = siblingWindow->GetNextWindow (GW_HWNDNEXT);
            RELEASE_WNDOBJ(siblingWindow);
        }

        return 0;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
	/* Press and release F10 or ALT */
	if (((wParam == VK_MENU) && iMenuSysKey) ||
            ((wParam == VK_F10) && iF10Key))
              ::SendMessageW( GetTopWindow(), WM_SYSCOMMAND, SC_KEYMENU, 0L );
	iMenuSysKey = iF10Key = 0;
        break;

    case WM_SYSCHAR:
    {
        iMenuSysKey = 0;
        if (wParam == VK_RETURN && (getStyle() & WS_MINIMIZE))
        {
                PostMessageA(getWindowHandle(), WM_SYSCOMMAND,
                         (WPARAM)SC_RESTORE, 0L );
                break;
        }
        if((HIWORD(lParam) & KEYDATA_ALT) && wParam)
        {
                if (wParam == VK_TAB || wParam == VK_ESCAPE || wParam == VK_F4)
                        break;
                if (wParam == VK_SPACE && (getStyle() & WS_CHILD)) {
                        ::SendMessageW(GetParent(), Msg, wParam, lParam );
                }
                else    SendMessageA(WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)(DWORD)wParam );
        }
#if 0
        else /* check for Ctrl-Esc */
                if (wParam != VK_ESCAPE) MessageBeep(0);
                        break;
#endif
    }

    case WM_SETHOTKEY:
        hotkey = wParam;
        return 1; //CB: always successful

    case WM_GETHOTKEY:
        return hotkey;

    case WM_CONTEXTMENU:
        if ((dwStyle & WS_CHILD) && getParent())
          getParent()->SendInternalMessageA(WM_CONTEXTMENU,wParam,lParam);
        return 0;

    case WM_SHOWWINDOW:
        if (!lParam) return 0; /* sent from ShowWindow */
        if (!(dwStyle & WS_POPUP) || !owner) return 0;
        if ((dwStyle & WS_VISIBLE) && wParam) return 0;
        else if (!(dwStyle & WS_VISIBLE) && !wParam) return 0;
        ShowWindow(wParam ? SW_SHOW:SW_HIDE);
        return 0;

    case WM_CANCELMODE:
        if (getParent() == windowDesktop) EndMenu();
        if (GetCapture() == Win32Hwnd) ReleaseCapture();
        return 0;

    case WM_DROPOBJECT:
        return DRAG_FILE;

    case WM_QUERYDROPOBJECT:
        return (dwExStyle & WS_EX_ACCEPTFILES) ? 1:0;

    case WM_QUERYDRAGICON:
        {
            HICON hDragIcon = windowClass->getCursor();
            UINT len;

            if(hDragIcon) return (LRESULT)hDragIcon;
            for(len = 1; len < 64; len++)
            {
              hDragIcon = LoadIconA(hInstance,MAKEINTRESOURCEA(len));
              if(hDragIcon)
                return (LRESULT)hDragIcon;
            }
            return (LRESULT)LoadIconA(0,IDI_APPLICATIONA);
        }

    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
        return 1;

    case WM_NOTIFYFORMAT:
        return IsWindowUnicode() ? NFR_UNICODE:NFR_ANSI;

    case WM_SETICON:
    case WM_GETICON:
        {
          LRESULT result = 0;

          /* Set the appropriate icon members in the window structure. */
          if (wParam == ICON_SMALL)
          {
            result = hIconSm;
            if (Msg == WM_SETICON)
              hIconSm = (HICON)lParam;
          }
          else
          {
            result = hIcon;
            if (Msg == WM_SETICON)
            {
              hIcon = (HICON)lParam;
              if ((dwStyle & WS_CAPTION) == WS_CAPTION)
                OSLibWinSetIcon(OS2HwndFrame,hIcon);
            }
          }
          if ((Msg == WM_SETICON) && ((dwStyle & WS_CAPTION) == WS_CAPTION))
            HandleNCPaint((HRGN)1);

          return result;
        }

    case WM_HELP:
        if (getParent()) getParent()->SendInternalMessageA(Msg,wParam,lParam);
        break;

    case WM_NOTIFY:
        return 0; //comctl32 controls expect this

    default:
        return 0;
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::DefWindowProcW(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_GETTEXTLENGTH:
        if(windowNameW) {
             return lstrlenW(windowNameW);
        }
        else return 0;

    case WM_GETTEXT:
        if (!lParam || !wParam) return 0;
        if (!windowNameW) ((LPWSTR)lParam)[0] = 0;
        else lstrcpynW((LPWSTR)lParam,windowNameW,wParam);
        return min((windowNameW ? lstrlenW(windowNameW) : 0),wParam);

    case WM_SETTEXT:
    {
        LPWSTR lpsz = (LPWSTR)lParam;

        if(windowNameA) free(windowNameA);
        if(windowNameW) free(windowNameW);
        if (lParam)
        {
            // Wide
            int wndNameLength = lstrlenW(lpsz);
            windowNameW = (LPWSTR)_smalloc((wndNameLength+1)*sizeof(WCHAR));
            lstrcpyW(windowNameW,lpsz);
            // Ascii
            LPSTR tmp = HEAP_strdupWtoA(GetProcessHeap(), 0, lpsz);
            if(tmp) {
                long tmpLength = strlen( tmp );
                windowNameA = (LPSTR)_smalloc(tmpLength+1);
                strcpy(windowNameA,tmp);
                windowNameA[tmpLength] = 0; // need ?
                HEAP_free(tmp);
            }
            else {
                windowNameA = (LPSTR)_smalloc(1);
                windowNameA[0] = 0;
            }
        }
        else
        {
            windowNameA = NULL;
            windowNameW = NULL;
        }
        dprintf(("WM_SETTEXT of %x\n",Win32Hwnd));
        if ((dwStyle & WS_CAPTION) == WS_CAPTION)
        {
            HandleNCPaint((HRGN)1);
            if(hTaskList) {
                OSLibWinChangeTaskList(hTaskList, OS2HwndFrame, getWindowNameA(), (getStyle() & WS_VISIBLE) ? 1 : 0);
            }
            if(fOS2Look) {
                OSLibWinSetTitleBarText(OS2HwndFrame, getWindowNameA());
            }
        }

        return TRUE;
    }

    default:
        return DefWindowProcA(Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::SendMessageA(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
    //if the destination window is created by this process & thread, call window proc directly
    if(dwProcessId == currentProcessId && dwThreadId == GetCurrentThreadId()) {
        return SendInternalMessageA(Msg, wParam, lParam);
    }
    //otherwise use WinSendMsg to send it to the right process/thread
    dprintf(("SendMessages (inter-process) %x %x %x %x", getWindowHandle(), Msg, wParam, lParam));
    return OSLibSendMessage(getOS2WindowHandle(), Msg, wParam, lParam, FALSE);
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::SendMessageW(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
    //if the destination window is created by this process & thread, call window proc directly
    if(dwProcessId == currentProcessId && dwThreadId == GetCurrentThreadId()) {
        return SendInternalMessageW(Msg, wParam, lParam);
    }
    //otherwise use WinSendMsg to send it to the right process/thread
    return OSLibSendMessage(getOS2WindowHandle(), Msg, wParam, lParam, TRUE);
}
//******************************************************************************
//Called as a result of an OS/2 message or called from a class method
//******************************************************************************
LRESULT Win32BaseWindow::SendInternalMessageA(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
 LRESULT rc;
 HWND    hwnd = getWindowHandle();
 BOOL    fInternalMsgBackup = fInternalMsg;

  //if the destination window was created by this process & thread, call window proc directly
  if(dwProcessId != currentProcessId || dwThreadId != GetCurrentThreadId()) {
        dprintf(("SendMessages (inter-process) %x %x %x %x", getWindowHandle(), Msg, wParam, lParam));
        return OSLibSendMessage(getOS2WindowHandle(), Msg, wParam, lParam, FALSE);
  }

  DebugPrintMessage(getWindowHandle(), Msg, wParam, lParam, FALSE, TRUE);

  CallWindowHookProc(WH_CALLWNDPROC, Msg, wParam, lParam, FALSE);

  fInternalMsg = TRUE;
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(CallWindowProcA(win32wndproc, getWindowHandle(), WM_CREATE, 0, lParam) == -1) {
                        dprintf(("WM_CREATE returned -1\n"));
                        rc = -1; //don't create window
                        break;
                }
                rc = 0;
                break;
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        {
                if (getParent())
                {
                    POINTS pt = MAKEPOINTS(lParam);
                    POINT point;

                    point.x = pt.x;
                    point.y = pt.y;
                    MapWindowPoints(getWindowHandle(), getParent()->getWindowHandle(), &point, 1);
                    NotifyParent(Msg,wParam,MAKELPARAM(point.x,point.y));
                }
                rc = win32wndproc(getWindowHandle(), Msg, wParam, lParam);
                break;
        }
        case WM_NCHITTEST:
                rc = lastHitTestVal = win32wndproc(getWindowHandle(), WM_NCHITTEST, wParam, lParam);
                break;

        case WM_DESTROY:
                rc = win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
                break;

        default:
                rc = CallWindowProcA(win32wndproc, getWindowHandle(), Msg, wParam, lParam);
                break;
  }
  if(!::IsWindow(hwnd)) {
        //window might have been destroyed by now. (this pointer invalid)
        //we must return immediately
        //(MS Visual C++ install heap corruption)
        //TODO: could happen in several places here!!!!
        return rc;
  }
  fInternalMsg = fInternalMsgBackup;
  dprintf2(("SendMessageA %x %x %x %x returned %d", getWindowHandle(), Msg, wParam, lParam, rc));
  return rc;
}
//******************************************************************************
//Called as a result of an OS/2 message or called from a class method
//******************************************************************************
LRESULT Win32BaseWindow::SendInternalMessageW(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
 LRESULT rc;
 HWND    hwnd = getWindowHandle();
 BOOL    fInternalMsgBackup = fInternalMsg;

  //if the destination window was created by this process & thread, call window proc directly
  if(dwProcessId != currentProcessId || dwThreadId != GetCurrentThreadId()) {
        dprintf(("SendMessages (inter-process) %x %x %x %x", getWindowHandle(), Msg, wParam, lParam));
        return OSLibSendMessage(getOS2WindowHandle(), Msg, wParam, lParam, FALSE);
  }

  DebugPrintMessage(getWindowHandle(), Msg, wParam, lParam, TRUE, TRUE);

  CallWindowHookProc(WH_CALLWNDPROC, Msg, wParam, lParam, TRUE);

  fInternalMsg = TRUE;
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(CallWindowProcW(win32wndproc, getWindowHandle(), WM_CREATE, 0, lParam) == -1) {
                        dprintf(("WM_CREATE returned -1\n"));
                        rc = -1; //don't create window
                        break;
                }
                rc = 0;
                break;
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
                NotifyParent(Msg, wParam, lParam);
                rc = win32wndproc(getWindowHandle(), Msg, wParam, lParam);
                break;

        case WM_NCHITTEST:
                rc = lastHitTestVal = win32wndproc(getWindowHandle(), WM_NCHITTEST, wParam, lParam);
                break;

        case WM_DESTROY:
                rc = win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
                break;
        default:
                rc = CallWindowProcW(win32wndproc, getWindowHandle(), Msg, wParam, lParam);
                break;
  }
  if(!::IsWindow(hwnd)) {
        //window might have been destroyed by now. (this pointer invalid)
        //we must return immediately
        //(MS Visual C++ install heap corruption)
        //TODO: could happen in several places here!!!!
        return rc;
  }
  fInternalMsg = fInternalMsgBackup;
  dprintf2(("SendMessageW %x %x %x %x returned %d", getWindowHandle(), Msg, wParam, lParam, rc));
  return rc;
}
//******************************************************************************
//******************************************************************************
void Win32BaseWindow::CallWindowHookProc(ULONG hooktype, ULONG Msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode)
{
 CWPSTRUCT cwp;

    cwp.lParam  = lParam;
    cwp.wParam  = wParam;
    cwp.message = Msg;
    cwp.hwnd    = getWindowHandle();

    switch(hooktype) {
    case WH_CALLWNDPROC:
        if(fUnicode) {
             HOOK_CallHooksW(WH_CALLWNDPROC, HC_ACTION, 1, (LPARAM)&cwp);
        }
        else HOOK_CallHooksA(WH_CALLWNDPROC, HC_ACTION, 1, (LPARAM)&cwp);
        break;
    }
}
//******************************************************************************
//TODO: Do this more efficiently
//******************************************************************************
LRESULT Win32BaseWindow::BroadcastMessageA(int type, UINT msg, WPARAM wParam, LPARAM lParam)
{
 Win32BaseWindow *window;
 HWND hwnd = WNDHANDLE_MAGIC_HIGHWORD;

    dprintf(("BroadCastMessageA %x %x %x %s", msg, wParam, lParam, (type == BROADCAST_SEND) ? "Send" : "Post"));

    for(int i=0;i<MAX_WINDOW_HANDLES;i++) {
        window = GetWindowFromHandle(hwnd++);
        if(window) {
            if ((window->getStyle() & WS_POPUP) || ((window->getStyle() & WS_CAPTION) == WS_CAPTION))
            {
                if(type == BROADCAST_SEND) {
                        ::SendMessageA(window->getWindowHandle(), msg, wParam, lParam);
                }
                else    PostMessageA(window->getWindowHandle(), msg, wParam, lParam);
            }
            RELEASE_WNDOBJ(window);
        }
    }
    return 0;
}
//******************************************************************************
//TODO: Do this more efficiently
//******************************************************************************
LRESULT Win32BaseWindow::BroadcastMessageW(int type, UINT msg, WPARAM wParam, LPARAM lParam)
{
 Win32BaseWindow *window;
 HWND hwnd = WNDHANDLE_MAGIC_HIGHWORD;

    dprintf(("BroadCastMessageW %x %x %x %s", msg, wParam, lParam, (type == BROADCAST_SEND) ? "Send" : "Post"));

    for(int i=0;i<MAX_WINDOW_HANDLES;i++) {
        window = GetWindowFromHandle(hwnd++);
        if(window) {
            if ((window->getStyle() & WS_POPUP) || ((window->getStyle() & WS_CAPTION) == WS_CAPTION))
            {
                if(type == BROADCAST_SEND) {
                        ::SendMessageW(window->getWindowHandle(), msg, wParam, lParam);
                }
                else    PostMessageW(window->getWindowHandle(), msg, wParam, lParam);
            }
            RELEASE_WNDOBJ(window);
        }
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
void Win32BaseWindow::NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam)
{
 Win32BaseWindow *window = this;
 Win32BaseWindow *parentwindow;

   while(window)
   {
        if(window->getStyle() & WS_CHILD && !(window->getExStyle() & WS_EX_NOPARENTNOTIFY) )
        {
                /* Notify the parent window only */
                parentwindow = window->getParent();
                if(parentwindow) {
                        parentwindow->SendInternalMessageA(WM_PARENTNOTIFY, MAKEWPARAM(Msg, getWindowId()), lParam );
                }
        }
        else    break;

        window = parentwindow;
   }
}
//******************************************************************************
// Returns the big or small icon for the window, falling back to the
// class as windows does.
//******************************************************************************
HICON Win32BaseWindow::IconForWindow(WPARAM fType)
{
  HICON hWndIcon;

  if (fType == ICON_BIG)
  {
    if (hIcon)
        hWndIcon = hIcon;
    else
    if (windowClass && windowClass->getIcon())
        hWndIcon = windowClass->getIcon();
    else
    if (!(dwStyle & DS_MODALFRAME))
         hWndIcon = LoadImageA(0,MAKEINTRESOURCEA(OIC_ODINICON),IMAGE_ICON,0,0,LR_DEFAULTCOLOR);
    else hWndIcon = 0;
  }
  else
  {
    if (hIconSm)
        hWndIcon = hIconSm;
    else
    if (hIcon)
        hWndIcon = hIcon;
    else
    if (windowClass && windowClass->getIconSm())
        hWndIcon = windowClass->getIconSm();
    else
    if (windowClass && windowClass->getIcon())
        hWndIcon = windowClass->getIcon();
    else
    if (!(dwStyle & DS_MODALFRAME))
         hWndIcon = LoadImageA(0,MAKEINTRESOURCEA(OIC_ODINICON),IMAGE_ICON,0,0,LR_DEFAULTCOLOR);
    else hWndIcon = 0;
  }

  return hWndIcon;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::ShowWindow(ULONG nCmdShow)
{
 ULONG swp = 0;
 HWND  hWinAfter;
 BOOL  rc,wasVisible,showFlag;
 RECT  newPos = {0, 0, 0, 0};

    dprintf(("ShowWindow %x %x", getWindowHandle(), nCmdShow));
    wasVisible = (getStyle() & WS_VISIBLE) != 0;

    dwOldStyle = getStyle();

    switch(nCmdShow)
    {
    case SW_HIDE:
        if (!wasVisible) goto END;

        swp |= SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER;
        break;

    case SW_SHOWMINNOACTIVE:
        swp |= SWP_NOACTIVATE | SWP_NOZORDER;
        /* fall through */
    case SW_SHOWMINIMIZED:
        swp |= SWP_SHOWWINDOW;
        /* fall through */
    case SW_MINIMIZE:
        swp |= SWP_FRAMECHANGED;
        if( !(getStyle() & WS_MINIMIZE) ) {
             swp |= MinMaximize(SW_MINIMIZE, &newPos );
             fMinMaxChange = TRUE; //-> invalidate entire window in WM_CALCINVALIDRECT
        }
        else swp |= SWP_NOSIZE | SWP_NOMOVE;
        break;

    case SW_SHOWMAXIMIZED: /* same as SW_MAXIMIZE */
        swp |= SWP_SHOWWINDOW | SWP_FRAMECHANGED;
        if( !(getStyle() & WS_MAXIMIZE) ) {
             swp |= MinMaximize(SW_MAXIMIZE, &newPos );
             fMinMaxChange = TRUE; //-> invalidate entire window in WM_CALCINVALIDRECT
        }
        else swp |= SWP_NOSIZE | SWP_NOMOVE;
        break;

    case SW_SHOWNA:
        swp |= SWP_NOACTIVATE | SWP_NOZORDER;
        /* fall through */
    case SW_SHOW:
        swp |= SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE;

        /*
         * ShowWindow has a little peculiar behavior that if the
         * window is already the topmost window, it will not
         * activate it.
         */
        if (::GetTopWindow((HWND)0)==getWindowHandle() && (wasVisible || GetActiveWindow() == getWindowHandle()))
            swp |= SWP_NOACTIVATE;

        break;

    case SW_SHOWNOACTIVATE:
        swp |= SWP_NOZORDER;
        if (GetActiveWindow())
            swp |= SWP_NOACTIVATE;
        /* fall through */
    case SW_SHOWNORMAL:  /* same as SW_NORMAL: */
    case SW_SHOWDEFAULT: /* FIXME: should have its own handler */
    case SW_RESTORE:
         swp |= SWP_SHOWWINDOW | SWP_FRAMECHANGED;

         if( getStyle() & (WS_MINIMIZE | WS_MAXIMIZE) ) {
              swp |= MinMaximize(SW_RESTORE, &newPos );
              fMinMaxChange = TRUE; //-> invalidate entire window in WM_CALCINVALIDRECT
         }
         else swp |= SWP_NOSIZE | SWP_NOMOVE;
         break;
    }

    showFlag = (nCmdShow != SW_HIDE);
    if (showFlag != wasVisible)
    {
        SendInternalMessageA(WM_SHOWWINDOW, showFlag, 0 );
        if (!::IsWindow( getWindowHandle() )) goto END;
    }

    /* We can't activate a child window */
    if((getStyle() & WS_CHILD) && !(getExStyle() & WS_EX_MDICHILD))
        swp |= SWP_NOACTIVATE | SWP_NOZORDER;

    SetWindowPos(HWND_TOP, newPos.left, newPos.top, newPos.right, newPos.bottom, LOWORD(swp));

    if(!(swp & SWP_NOACTIVATE)) {
        OSLibWinSetActiveWindow(OS2HwndFrame);
    }

    if (flags & WIN_NEED_SIZE)
    {
        /* should happen only in CreateWindowEx() */
        int wParam = SIZE_RESTORED;

        flags &= ~WIN_NEED_SIZE;
        if (dwStyle & WS_MAXIMIZE)
            wParam = SIZE_MAXIMIZED;
        else
        if (dwStyle & WS_MINIMIZE)
            wParam = SIZE_MINIMIZED;

        SendInternalMessageA(WM_SIZE, wParam,
                     MAKELONG(rectClient.right-rectClient.left,
                              rectClient.bottom-rectClient.top));
        SendInternalMessageA(WM_MOVE,0,MAKELONG(rectClient.left,rectClient.top));
    }
//testestest
    //temporary workaround for file dialogs with template dialog child
    //they don't redraw when switching directories
    //For some reason the new child's (syslistview32) update rectangle stays
    //empty after its parent is made visible with ShowWindow
    //TODO: find real cause
    if(!wasVisible) {
        InvalidateRect(getWindowHandle(), NULL, TRUE);
    }
//testestest
END:
    fMinMaxChange = FALSE;
    return wasVisible;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags)
{
   BOOL rc = FALSE;
   Win32BaseWindow *window;
   HWND hParent = 0;
   RECT oldClientRect = rectClient;

    if (fuFlags &
       ~(SWP_NOSIZE     | SWP_NOMOVE     | SWP_NOZORDER     |
         SWP_NOREDRAW   | SWP_NOACTIVATE | SWP_FRAMECHANGED |
         SWP_SHOWWINDOW | SWP_HIDEWINDOW | SWP_NOCOPYBITS   |
         SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_DEFERERASE |
         SWP_NOCLIENTSIZE | SWP_NOCLIENTMOVE))
    {
        dprintf(("ERROR: SetWindowPos; UNKNOWN flag"));
        return FALSE;
    }

    if( fuFlags & (SWP_DEFERERASE | SWP_NOCLIENTSIZE | SWP_NOCLIENTMOVE)) {
        dprintf(("WARNING: SetWindowPos; unsupported flag"));
    }

    if(IsWindowDestroyed()) {
        //changing the position of a window that's being destroyed can cause crashes in PMMERGE
        dprintf(("SetWindowPos; window already destroyed"));
        return TRUE;
    }
#if 0
    /* Fix redundant flags */
    if(getStyle() & WS_VISIBLE) {
        fuFlags &= ~SWP_SHOWWINDOW;
    }
    else
    {
        if (!(fuFlags & SWP_SHOWWINDOW))
              fuFlags |= SWP_NOREDRAW;
        fuFlags &= ~SWP_HIDEWINDOW;
    }

    if(cx < 0) cx = 0;
    if(cy < 0) cy = 0;

    if((rectWindow.right - rectWindow.left == cx) && (rectWindow.bottom - rectWindow.top == cy)) {
        fuFlags |= SWP_NOSIZE;    /* Already the right size */
    }

    if((rectWindow.left == x) && (rectWindow.top == y)) {
        fuFlags |= SWP_NOMOVE;    /* Already the right position */
    }

    if(getWindowHandle() == GetActiveWindow()) {
        fuFlags |= SWP_NOACTIVATE;   /* Already active */
    }
    else
    if((getStyle() & (WS_POPUP | WS_CHILD)) != WS_CHILD )
    {
        if(!(fuFlags & SWP_NOACTIVATE)) /* Bring to the top when activating */
        {
            fuFlags &= ~SWP_NOZORDER;
            hwndInsertAfter = HWND_TOP;
        }
    }
#endif

    //Note: Solitaire crashes when receiving WM_SIZE messages before WM_CREATE
    if(state < STATE_POST_WMNCCREATE)
    {//don't change size; modify internal structures only
        //TODO: not 100% correct yet (activate)
        dprintf2(("state < STATE_POST_WMNCCREATE"));
        if(!(fuFlags & SWP_NOZORDER)) {
            hwndLinkAfter = hwndInsertAfter;
        }
        if(!(fuFlags & SWP_NOMOVE)) {
            rectWindow.bottom = (rectWindow.bottom - rectWindow.top) + y;
            rectWindow.top    = y;
            rectWindow.right  = (rectWindow.right - rectWindow.left) + x;
            rectWindow.left   = x;
        }
        if(!(fuFlags & SWP_NOSIZE)) {
            rectWindow.bottom = rectWindow.top + cy;
            rectWindow.right  = rectWindow.left + cx;
        }
        return TRUE;
    }

    WINDOWPOS wpos;
    SWP swp, swpOld;
    wpos.flags            = fuFlags;
    wpos.cy               = cy;
    wpos.cx               = cx;
    wpos.x                = x;
    wpos.y                = y;
    wpos.hwndInsertAfter  = hwndInsertAfter;
    wpos.hwnd             = getWindowHandle();

    if(~fuFlags & (SWP_NOMOVE | SWP_NOSIZE))
    {
        if (isChild())
        {
            if(!getParent()) {
                dprintf(("WARNING: Win32BaseWindow::SetWindowPos window %x is child but has no parent!!", getWindowHandle()));
            }
        }
        OSLibWinQueryWindowPos(OS2HwndFrame, &swpOld);
    }

    if(getParent()) {
          OSLibMapWINDOWPOStoSWP(&wpos, &swp, &swpOld, getParent()->getClientHeight(),
                                 OS2HwndFrame);
    }
    else  OSLibMapWINDOWPOStoSWP(&wpos, &swp, &swpOld, OSLibQueryScreenHeight(), OS2HwndFrame);

    if (swp.fl == 0) {
        dprintf2(("swp.fl == 0"));
        if(fuFlags & SWP_FRAMECHANGED)
        {
            NotifyFrameChanged(&wpos, &oldClientRect);
        }
        return TRUE;
    }

//   if ((swp.fl & SWPOS_ZORDER) && (swp.hwndInsertBehind > HWNDOS_BOTTOM))
    if ((swp.hwndInsertBehind > HWNDOS_BOTTOM))
    {
        Win32BaseWindow *wndBehind = Win32BaseWindow::GetWindowFromHandle(swp.hwndInsertBehind);
        if(wndBehind) {
            swp.hwndInsertBehind   = wndBehind->getOS2FrameWindowHandle();
            RELEASE_WNDOBJ(wndBehind);
        }
        else {
            dprintf(("ERROR: SetWindowPos: hwndInsertBehind %x invalid!",swp.hwndInsertBehind));
            swp.hwndInsertBehind = 0;
        }
    }
    swp.hwnd = OS2HwndFrame;

    if(fuFlags & SWP_SHOWWINDOW && !IsWindowVisible(getWindowHandle())) {
        setStyle(getStyle() | WS_VISIBLE);
        if(hTaskList) {
            dprintf(("Adding window %x to tasklist", getWindowHandle()));
            OSLibWinChangeTaskList(hTaskList, OS2HwndFrame, getWindowNameA(), 1);
        }
    }
    else
    if((fuFlags & SWP_HIDEWINDOW) && IsWindowVisible(getWindowHandle())) {
        setStyle(getStyle() & ~WS_VISIBLE);
        if(hTaskList && !(getStyle() & WS_MINIMIZE)) {
            dprintf(("Removing window %x from tasklist", getWindowHandle()));
            OSLibWinChangeTaskList(hTaskList, OS2HwndFrame, getWindowNameA(), 0);
        }
    }
    dprintf (("WinSetWindowPos %x %x (%d,%d)(%d,%d) %x", swp.hwnd, swp.hwndInsertBehind, swp.x, swp.y, swp.cx, swp.cy, swp.fl));
    rc = OSLibWinSetMultWindowPos(&swp, 1);

    if(rc == FALSE)
    {
        dprintf(("OSLibWinSetMultWindowPos failed! Error %x",OSLibWinGetLastError()));
        return 0;
    }

    if((fuFlags & SWP_FRAMECHANGED) && (fuFlags & (SWP_NOMOVE | SWP_NOSIZE) == (SWP_NOMOVE | SWP_NOSIZE)))
    {
        NotifyFrameChanged(&wpos, &oldClientRect);
    }
    return (rc);
}
//******************************************************************************
//Called by ScrollWindowEx (dc.cpp) to notify child window that it has moved
//******************************************************************************
BOOL Win32BaseWindow::ScrollWindow(int dx, int dy)
{
    rectWindow.left   += dx;
    rectWindow.right  += dx;
    rectWindow.top    += dy;
    rectWindow.bottom += dy;
    SendInternalMessageA(WM_MOVE, 0, MAKELONG(rectClient.left, rectClient.top));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
void Win32BaseWindow::NotifyFrameChanged(WINDOWPOS *wpos, RECT *oldClientRect)
{
 HRGN hrgn, hrgnClient;
 RECT rect;

    MsgFormatFrame(NULL);

    if(RECT_WIDTH(rectClient) != RECT_WIDTH(*oldClientRect) ||
       RECT_HEIGHT(rectClient) != RECT_HEIGHT(*oldClientRect))
    {
         wpos->flags &= ~(SWP_NOSIZE|SWP_NOCLIENTSIZE);
         wpos->cx     = RECT_WIDTH(rectWindow);
         wpos->cy     = RECT_HEIGHT(rectWindow);
    }

    if(rectClient.left != oldClientRect->left ||
       rectClient.top != oldClientRect->top)
    {
         wpos->flags &= ~(SWP_NOMOVE|SWP_NOCLIENTMOVE);
         wpos->x      = rectWindow.left;
         wpos->y      = rectWindow.top;
    }

    WINDOWPOS wpOld = *wpos;
    SendInternalMessageA(WM_WINDOWPOSCHANGING, 0, (LPARAM)wpos);

    if ((wpos->hwndInsertAfter != wpOld.hwndInsertAfter) ||
        (wpos->x != wpOld.x) || (wpos->y != wpOld.y) || (wpos->cx != wpOld.cx) || (wpos->cy != wpOld.cy) || (wpos->flags != wpOld.flags))
    {
         dprintf(("WARNING, NotifyFrameChanged: TODO -> adjust flags!!!!"));
         SetWindowPos(wpos->hwndInsertAfter, wpos->x, wpos->y, wpos->cx, wpos->cy, wpos->flags | SWP_NOSENDCHANGING);
    }
    else SendInternalMessageA(WM_WINDOWPOSCHANGED, 0, (LPARAM)wpos);

    //Calculate invalid areas
    rect = rectWindow;
    OffsetRect(&rect, -rectWindow.left, -rectWindow.top);
    hrgn = CreateRectRgnIndirect(&rect);
    if (!hrgn) {
         dprintf(("ERROR: NotifyFrameChanged, CreateRectRgnIndirect failed!!"));
         return;
    }
    rect = rectClient;
    hrgnClient = CreateRectRgnIndirect(&rect);
    if (!hrgn) {
         dprintf(("ERROR: NotifyFrameChanged, CreateRectRgnIndirect failed!!"));
         return;
    }
    CombineRgn(hrgn, hrgn, hrgnClient, RGN_DIFF);
    DeleteObject(hrgnClient);

    if(!EqualRect(oldClientRect, &rectClient)) {
         UnionRect(oldClientRect, oldClientRect, &rectClient);
         hrgnClient = CreateRectRgnIndirect(oldClientRect);
         if (!hrgn) {
              dprintf(("ERROR: NotifyFrameChanged, CreateRectRgnIndirect failed!!"));
              return;
         }
         CombineRgn(hrgn, hrgn, hrgnClient, RGN_OR);
         DeleteObject(hrgnClient);
    }
    RedrawWindow(getWindowHandle(), NULL, hrgn, RDW_ALLCHILDREN |
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME);
    DeleteObject(hrgn);
}
//******************************************************************************
//TODO: Check how this api really works in NT
//******************************************************************************
BOOL Win32BaseWindow::SetWindowPlacement(WINDOWPLACEMENT *wndpl)
{
   dprintf(("SetWindowPlacement %x min  (%d,%d)", getWindowHandle(), wndpl->ptMinPosition.x, wndpl->ptMinPosition.y));
   dprintf(("SetWindowPlacement %x max  (%d,%d)", getWindowHandle(), wndpl->ptMaxPosition.x, wndpl->ptMaxPosition.y));
   dprintf(("SetWindowPlacement %x norm (%d,%d)(%d,%d)", getWindowHandle(), wndpl->rcNormalPosition.left, wndpl->rcNormalPosition.top, wndpl->rcNormalPosition.right, wndpl->rcNormalPosition.bottom));
   windowpos.ptMinPosition    = wndpl->ptMinPosition;
   windowpos.ptMaxPosition    = wndpl->ptMaxPosition;
   windowpos.rcNormalPosition = wndpl->rcNormalPosition;

   if(getStyle() & WS_MINIMIZE )
   {
        //TODO: Why can't this be (0,0)?
        if(wndpl->flags & WPF_SETMINPOSITION && !(!windowpos.ptMinPosition.x && !windowpos.ptMinPosition.y)) {
            SetWindowPos(0, windowpos.ptMinPosition.x, windowpos.ptMinPosition.y,
                         0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
   }
   else
   if(getStyle() & WS_MAXIMIZE )
   {
        //TODO: Why can't this be (0,0)?
        if(windowpos.ptMaxPosition.x != 0 || windowpos.ptMaxPosition.y != 0 )
            SetWindowPos(0, windowpos.ptMaxPosition.x, windowpos.ptMaxPosition.y,
                         0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
   }
   else {
        SetWindowPos(0, windowpos.rcNormalPosition.left, windowpos.rcNormalPosition.top,
             windowpos.rcNormalPosition.right - windowpos.rcNormalPosition.left,
             windowpos.rcNormalPosition.bottom - windowpos.rcNormalPosition.top,
             SWP_NOZORDER | SWP_NOACTIVATE );
   }
   ShowWindow(wndpl->showCmd);
   if( ::IsWindow(getWindowHandle()) && getStyle() & WS_MINIMIZE )
   {
        /* SDK: ...valid only the next time... */
        if(wndpl->flags & WPF_RESTORETOMAXIMIZED)
        setFlags(getFlags() | WIN_RESTORE_MAX);
   }
   return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::GetWindowPlacement(LPWINDOWPLACEMENT wndpl)
{
   wndpl->length = sizeof(*wndpl);
   if(getStyle() & WS_MINIMIZE )
        wndpl->showCmd = SW_SHOWMINIMIZED;
   else wndpl->showCmd = (getStyle() & WS_MAXIMIZE) ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;

   //TODO: Verify if this is correct -> SDK docs claim this flag must always be set to 0
   if(getFlags() & WIN_RESTORE_MAX )
        wndpl->flags = WPF_RESTORETOMAXIMIZED;
   else wndpl->flags = 0;

   wndpl->ptMinPosition    = windowpos.ptMinPosition;
   wndpl->ptMaxPosition    = windowpos.ptMaxPosition;
   //Must be in parent coordinates (or screen if no parent); verified in NT4, SP6
   wndpl->rcNormalPosition = rectWindow;

   return TRUE;
}
//******************************************************************************
//Also destroys all the child windows (destroy children first, parent last)
//******************************************************************************
BOOL Win32BaseWindow::DestroyWindow()
{
  HWND hwnd = getWindowHandle();

    dprintf(("DestroyWindow %x", hwnd));

    /* Call hooks */
    if(HOOK_CallHooksA( WH_CBT, HCBT_DESTROYWND, getWindowHandle(), 0L))
    {
        return FALSE;
    }
  
    if(!(getStyle() & WS_CHILD) && getOwner() == NULL)
    {
        HOOK_CallHooksA(WH_SHELL, HSHELL_WINDOWDESTROYED, getWindowHandle(), 0L);
        /* FIXME: clean up palette - see "Internals" p.352 */
    }

    if((getStyle() & WS_CHILD) && !(getExStyle() & WS_EX_NOPARENTNOTIFY))
    {
        if(getParent() && getParent()->IsWindowDestroyed() == FALSE)
        {
             /* Notify the parent window only */
             getParent()->SendMessageA(WM_PARENTNOTIFY, MAKEWPARAM(WM_DESTROY, getWindowId()), (LPARAM)getWindowHandle());
             if(!::IsWindow(hwnd) )
             {
                return TRUE;
             }
        }
////        else DebugInt3();
    }
    /* Hide the window */
    if(IsWindowVisible(getWindowHandle()))
    {
        SetWindowPos(0, 0, 0, 0, 0, SWP_HIDEWINDOW |
                     SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE);
        if(!::IsWindow(hwnd))
        {
                return TRUE;
        }
    }
    dprintf(("DestroyWindow %x -> HIDDEN", hwnd));
  
    // check the handle for the last active popup window
    Win32BaseWindow* owner = getOwner();
    if (NULL != owner)
    {
      if (owner->getLastActive() == hwnd)
        owner->setLastActive( owner->getWindowHandle() );
    }
  
    fDestroyWindowCalled = TRUE;
    return OSLibWinDestroyWindow(OS2HwndFrame);
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::getParent()
{
  Win32BaseWindow *wndparent = (Win32BaseWindow *)ChildWindow::getParentOfChild();
  return ((ULONG)wndparent == (ULONG)windowDesktop) ? NULL : wndparent;
}
//******************************************************************************
//Note: does not set last error if no parent (verified in NT4, SP6)
//******************************************************************************
HWND Win32BaseWindow::GetParent()
{
  if((!(getStyle() & (WS_POPUP|WS_CHILD)))) {
    return 0;
  }

  Win32BaseWindow *wndparent = (Win32BaseWindow *)ChildWindow::getParentOfChild();

  if(getStyle() & WS_CHILD) {
    if(wndparent) {
        return wndparent->getWindowHandle();
    }
    dprintf(("WARNING: GetParent: WS_CHILD but no parent!!"));
    DebugInt3();
    return 0;
  }
  else  return (getOwner()) ? getOwner()->getWindowHandle() : 0;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::SetParent(HWND hwndNewParent)
{
 HWND oldhwnd;
 Win32BaseWindow *newparent;
 Win32BaseWindow *oldparent = (Win32BaseWindow *)ChildWindow::getParentOfChild();
 BOOL fShow = FALSE;

   if(oldparent) {
        oldhwnd = oldparent->getWindowHandle();
        oldparent->removeChild(this);
   }
   else oldhwnd = 0;

   /* Windows hides the window first, then shows it again
    * including the WM_SHOWWINDOW messages and all */
   if(IsWindowCreated() && (getStyle() & WS_VISIBLE)) {
        ShowWindow(SW_HIDE);
        fShow = TRUE;
   }
   if(oldparent) {
        //release parent here (increased refcount during creation)
        RELEASE_WNDOBJ(oldparent);
   }
   newparent = GetWindowFromHandle(hwndNewParent);
   if(newparent && !newparent->isDesktopWindow())
   {
        setParent(newparent);
        getParent()->addChild(this);
        fParentChange = TRUE;

        OSLibWinSetParent(getOS2FrameWindowHandle(), getParent()->getOS2WindowHandle());
        if(!(getStyle() & WS_CHILD))
        {
            //TODO: Send WM_STYLECHANGED msg?
            setStyle(getStyle() | WS_CHILD);
            if(getWindowId())
            {
                DestroyMenu( (HMENU) getWindowId() );
                setWindowId(0);
            }
        }
        //SvL: Even though the win32 coordinates might not change, the PM
        //     coordinates can. We must make sure the control stays at the
        //     same position (y) relative to the (new) parent.
        SetWindowPos(HWND_TOPMOST, rectWindow.left, rectWindow.top, 0, 0,
                     SWP_NOACTIVATE|SWP_NOSIZE);
        fParentChange = FALSE;
   }
   else {
        if(newparent) RELEASE_WNDOBJ(newparent);

        setParent(windowDesktop);
        windowDesktop->addRef();
        windowDesktop->addChild(this);
        OSLibWinSetParent(getOS2FrameWindowHandle(), OSLIB_HWND_DESKTOP);

        //TODO: Send WM_STYLECHANGED msg?
        setStyle(getStyle() & ~WS_CHILD);
        setWindowId(0);
   }
   /* SetParent additionally needs to make hwndChild the topmost window
      in the x-order and send the expected WM_WINDOWPOSCHANGING and
      WM_WINDOWPOSCHANGED notification messages.
   */
   if(state >= STATE_PRE_WMNCCREATE) {
        SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|(fShow? SWP_SHOWWINDOW : 0));

   /* FIXME: a WM_MOVE is also generated (in the DefWindowProc handler
    * for WM_WINDOWPOSCHANGED) in Windows, should probably remove SWP_NOMOVE */
   }
   return oldhwnd;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsChild(HWND hwndParent)
{
  // PH: Optimizer won't unroll calls to getParent() even
  // in release build.
  Win32BaseWindow *_parent = getParent();
    
  if(_parent) 
  {
    if(_parent->getWindowHandle() == hwndParent)
      return TRUE;

    return _parent->IsChild(hwndParent);
  }
  else 
    return 0;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::GetTopWindow()
{
 HWND             hwndTop;
 Win32BaseWindow *topwindow;

    hwndTop = OSLibWinQueryWindow(getOS2WindowHandle(), QWOS_TOP);
    if(!isDesktopWindow())
    {
        topwindow = GetWindowFromOS2FrameHandle(hwndTop);
        //Note: GetTopWindow can't return a window that hasn't processed
        //      WM_NCCREATE yet (verified in NT4, SP6)
        if(topwindow) {
            if(topwindow->state >= STATE_POST_WMNCCREATE) {
                 hwndTop = topwindow->getWindowHandle();
            }
            else hwndTop = topwindow->GetWindow(GW_HWNDNEXT);
            RELEASE_WNDOBJ(topwindow);
            return hwndTop;
        }
        if(topwindow) RELEASE_WNDOBJ(topwindow);
        return 0;
    }
    while(hwndTop) {
        topwindow = GetWindowFromOS2FrameHandle(hwndTop);
        //Note: GetTopWindow can't return a window that hasn't processed
        //      WM_NCCREATE yet (verified in NT4, SP6)
        if(topwindow) {
            if(topwindow->state >= STATE_POST_WMNCCREATE) {
                 hwndTop = topwindow->getWindowHandle();
            }
            else hwndTop = topwindow->GetWindow(GW_HWNDNEXT);
            RELEASE_WNDOBJ(topwindow);
            return hwndTop;
        }
        if(topwindow) RELEASE_WNDOBJ(topwindow);
        hwndTop = OSLibWinQueryWindow(hwndTop, QWOS_NEXT);
    }

    return 0;
}
//******************************************************************************
// Get the top-level parent for a child window.
//******************************************************************************
HWND Win32BaseWindow::GetTopParent()
{
 Win32BaseWindow *window = this;
 HWND             hwndTopParent = 0;

    lock();
    while(window && (window->getStyle() & WS_CHILD))
    {
        window = window->getParent();
    }
    if(window) {
        hwndTopParent = window->getWindowHandle();
    }
    unlock();
    return hwndTopParent;
}
//******************************************************************************
//TODO: Should not enumerate children that are created during the enumeration!
//******************************************************************************
BOOL Win32BaseWindow::EnumChildWindows(WNDENUMPROC lpfn, LPARAM lParam)
{
 BOOL rc = TRUE;
 HWND hwnd;
 Win32BaseWindow *prevchild = 0, *child = 0;

    dprintf(("EnumChildWindows of %x parameter %x %x (%x)", getWindowHandle(), lpfn, lParam, getFirstChild()));
    lock();
    for (child = (Win32BaseWindow *)getFirstChild(); child; child = (Win32BaseWindow *)child->getNextChild())
    {
        dprintf(("EnumChildWindows: enumerating child %x (owner %x; parent %x)", child->getWindowHandle(), (child->getOwner()) ? child->getOwner()->getWindowHandle() : 0, getWindowHandle()));
        hwnd = child->getWindowHandle();
        if(child->IsWindowDestroyed() || child->getOwner()) {
                continue; //shouldn't have an owner (Wine)
        }
        child->addRef();
        unlock();
        if(lpfn(hwnd, lParam) == FALSE)
        {
                child->release();
                return FALSE;
        }
        child->release();
        lock();
        //check if the window still exists
        if(!::IsWindow(hwnd))
        {
            child = prevchild;
            continue;
        }
        if(child->getFirstChild() != NULL)
        {
            dprintf(("EnumChildWindows: Enumerate children of %x", child->getWindowHandle()));
            child->addRef();
            unlock();
            if(child->EnumChildWindows(lpfn, lParam) == FALSE)
            {
                child->release();
                return FALSE;
            }
            child->release();
            lock();
        }
        prevchild = child;
    }
    unlock();
    return rc;
}
//******************************************************************************
//Enumerate first-level children only and check thread id
//******************************************************************************
BOOL Win32BaseWindow::EnumThreadWindows(DWORD dwThreadId, WNDENUMPROC lpfn, LPARAM lParam)
{
 Win32BaseWindow *child = 0;
 ULONG  tid, pid;
 BOOL   rc;
 HWND   hwnd;

    dprintf(("EnumThreadWindows %x %x %x", dwThreadId, lpfn, lParam));

    for (child = (Win32BaseWindow *)getFirstChild(); child; child = (Win32BaseWindow *)child->getNextChild())
    {
        OSLibWinQueryWindowProcess(child->getOS2WindowHandle(), &pid, &tid);

        if(dwThreadId == tid) {
            dprintf2(("EnumThreadWindows: Found Window %x", child->getWindowHandle()));
            if((rc = lpfn(child->getWindowHandle(), lParam)) == FALSE) {
                break;
            }
        }
    }
    return TRUE;
}
//******************************************************************************
//Enumerate first-level children only
//******************************************************************************
BOOL Win32BaseWindow::EnumWindows(WNDENUMPROC lpfn, LPARAM lParam)
{
 Win32BaseWindow *child = 0;
 BOOL   rc;
 HWND   hwnd;

    dprintf(("EnumWindows %x %x", lpfn, lParam));

    for (child = (Win32BaseWindow *)getFirstChild(); child; child = (Win32BaseWindow *)child->getNextChild())
    {
        hwnd = child->getWindowHandle();

        dprintf2(("EnumWindows: Found Window %x", child->getWindowHandle()));
        if((rc = lpfn(child->getWindowHandle(), lParam)) == FALSE) {
            break;
        }
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::FindWindowById(int id)
{
    HWND hwnd;

    lock();
    for (Win32BaseWindow *child = (Win32BaseWindow *)getFirstChild(); child; child = (Win32BaseWindow *)child->getNextChild())
    {
        if (child->getWindowId() == id)
        {
            hwnd = child->getWindowHandle();
            unlock();
            return hwnd;
        }
    }
    unlock();
    return 0;
}
//******************************************************************************
//TODO:
//We assume (for now) that if hwndParent or hwndChildAfter are real window handles, that
//the current process owns them.
//******************************************************************************
HWND Win32BaseWindow::FindWindowEx(HWND hwndParent, HWND hwndChildAfter, ATOM atom, LPSTR lpszWindow)
{
 Win32BaseWindow *parent = GetWindowFromHandle(hwndParent);
 Win32BaseWindow *child  = GetWindowFromHandle(hwndChildAfter);
 Win32BaseWindow *firstchild = child;

    dprintf(("FindWindowEx %x %x %x %s", hwndParent, hwndChildAfter, atom, lpszWindow));
    if((hwndParent != 0 && !parent) ||
       (hwndChildAfter != 0 && !child) ||
       (hwndParent == 0 && hwndChildAfter != 0))
    {
        if(parent)      RELEASE_WNDOBJ(parent);
        if(firstchild)  RELEASE_WNDOBJ(firstchild);
        dprintf(("Win32BaseWindow::FindWindowEx: parent or child not found %x %x", hwndParent, hwndChildAfter));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    SetLastError(0);
    if(hwndParent != 0)
    {//if the current process owns the window, just do a quick search
        lock(&critsect);
        child = (Win32BaseWindow *)parent->getFirstChild();
        if(hwndChildAfter != 0)
        {
            while(child)
            {
                if(child->getWindowHandle() == hwndChildAfter)
                {
                    child = (Win32BaseWindow *)child->getNextChild();
                    break;
                }
                child = (Win32BaseWindow *)child->getNextChild();
            }
        }
        while(child)
        {
            //According to Wine, the class doesn't need to be specified
            if((!atom || child->getWindowClass()->getAtom() == atom) &&
               (!lpszWindow || child->hasWindowName(lpszWindow)))
            {
                dprintf(("FindWindowEx: Found window %x", child->getWindowHandle()));
                HWND hwndChild = child->getWindowHandle();
                unlock(&critsect);
                if(parent)      RELEASE_WNDOBJ(parent);
                if(firstchild)  RELEASE_WNDOBJ(firstchild);
                dprintf(("FindWindowEx: Found window %x", child->getWindowHandle()));
                return hwndChild;
            }
            child = (Win32BaseWindow *)child->getNextChild();
        }
        unlock(&critsect);
        if(parent)      RELEASE_WNDOBJ(parent);
        if(firstchild)  RELEASE_WNDOBJ(firstchild);
    }
    else {
        Win32BaseWindow *wnd;
        HWND henum, hwnd;

        henum = OSLibWinBeginEnumWindows(OSLIB_HWND_DESKTOP);
        hwnd = OSLibWinGetNextWindow(henum);

        while(hwnd)
        {
            wnd = GetWindowFromOS2FrameHandle(hwnd);
            if(wnd == NULL) {
                hwnd = OSLibWinQueryClientWindow(hwnd);
                if(hwnd)  wnd = GetWindowFromOS2Handle(hwnd);
            }

            if(wnd) {
                //According to Wine, the class doesn't need to be specified
                if((!atom || wnd->getWindowClass()->getAtom() == atom) &&
                   (!lpszWindow || wnd->hasWindowName(lpszWindow)))
                {
                    OSLibWinEndEnumWindows(henum);
                    dprintf(("FindWindowEx: Found window %x", wnd->getWindowHandle()));
                    HWND hwndret = wnd->getWindowHandle();
                    RELEASE_WNDOBJ(wnd);
                    return hwndret;
                }
                RELEASE_WNDOBJ(wnd);
            }
            hwnd = OSLibWinGetNextWindow(henum);
        }
        OSLibWinEndEnumWindows(henum);
        if(parent)      RELEASE_WNDOBJ(parent);
        if(firstchild)  RELEASE_WNDOBJ(firstchild);
    }
    SetLastError(ERROR_CANNOT_FIND_WND_CLASS); //TODO: not always correct
    return 0;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::GetWindow(UINT uCmd)
{
 HWND hwndRelated = 0;
 Win32BaseWindow *window;

    switch(uCmd)
    {
    case GW_HWNDFIRST:
        window = (Win32BaseWindow *)getParent();
        if(window)
        {
            hwndRelated = OSLibWinQueryWindow(window->getOS2WindowHandle(), QWOS_TOP);
            window = GetWindowFromOS2FrameHandle(hwndRelated);
            if(window) {
                 hwndRelated = window->getWindowHandle();
                 RELEASE_WNDOBJ(window);
            }
            else hwndRelated = 0;
        }
        else {
            dprintf(("WARNING: GW_HWNDFIRST not correctly implemented for toplevel/most windows!"));
            hwndRelated = 0; //TODO: not correct; should get first child in z-order of desktop
        }
        break;

    case GW_HWNDLAST:
        window = (Win32BaseWindow *)getParent();
        if(window) {
            hwndRelated = OSLibWinQueryWindow(window->getOS2WindowHandle(), QWOS_BOTTOM);
            dprintf(("os2 handle %x", hwndRelated));
            window = GetWindowFromOS2FrameHandle(hwndRelated);
            if(window) {
                 hwndRelated = window->getWindowHandle();
                 RELEASE_WNDOBJ(window);
            }
            else hwndRelated = 0;
        }
        else {
            dprintf(("WARNING: GW_HWNDLAST not correctly implemented for toplevel/most windows!"));
            hwndRelated = 0; //TODO: not correct; should get first child in z-order of desktop
        }
        break;

    case GW_HWNDNEXT:
        if(getParent()) {
            hwndRelated = OSLibWinQueryWindow(getOS2FrameWindowHandle(), QWOS_NEXT);
            window = GetWindowFromOS2FrameHandle(hwndRelated);
            if(window) {
                 hwndRelated = window->getWindowHandle();
                 RELEASE_WNDOBJ(window);
            }
            else hwndRelated = 0;
        }
        else {
            dprintf(("WARNING: GW_HWNDNEXT not correctly implemented for toplevel/most windows!"));
            hwndRelated = 0; //TODO: not correct; should get first child in z-order of desktop
        }
        break;

    case GW_HWNDPREV:
        if(getParent()) {
            hwndRelated = OSLibWinQueryWindow(getOS2FrameWindowHandle(), QWOS_PREV);
            window = GetWindowFromOS2FrameHandle(hwndRelated);
            if(window) {
                 hwndRelated = window->getWindowHandle();
                 RELEASE_WNDOBJ(window);
            }
            else hwndRelated = 0;
        }
        else {
            dprintf(("WARNING: GW_HWNDPREV not correctly implemented for toplevel/most windows!"));
            hwndRelated = 0; //TODO: not correct; should get first child in z-order of desktop
        }
        break;

    case GW_OWNER:
      {
        Win32BaseWindow *owner = getOwner();
        if(owner) {
            hwndRelated = owner->getWindowHandle();
        }
        break;
      }

    case GW_CHILD:
        hwndRelated = OSLibWinQueryWindow(getOS2WindowHandle(), QWOS_TOP);
        window = GetWindowFromOS2FrameHandle(hwndRelated);

        //Before a window has processed WM_NCCREATE:
        //- GetWindow(parent, GW_CHILD) can't return that window handle
        //(verified in NT4, SP6)
        if(window) {
             if(window->state >= STATE_POST_WMNCCREATE) {
                 hwndRelated = window->getWindowHandle();
                 RELEASE_WNDOBJ(window); 
             }
             else {
                 hwndRelated = window->GetWindow(GW_HWNDNEXT);
                 RELEASE_WNDOBJ(window); 
             }
        }
        else hwndRelated = 0;

        break;

    //for internal use only
    case GW_HWNDNEXTCHILD:
        lock();
        window = (Win32BaseWindow *)getNextChild();
        if(window) {
             hwndRelated = window->getWindowHandle();
        }
        else hwndRelated = 0;
        unlock();
        break;

    case GW_HWNDPREVCHILD:
        DebugInt3();
        break;

    case GW_HWNDFIRSTCHILD:
        lock();
        window = (Win32BaseWindow *)getFirstChild();
        if(window) {
             hwndRelated = window->getWindowHandle();
        }
        else hwndRelated = 0;
        unlock();
        break;

    case GW_HWNDLASTCHILD:
        lock();
        window = (Win32BaseWindow *)getFirstChild();
        if(window) {
             while (window->getNextChild())
             {
                window = (Win32BaseWindow *)window->getNextChild();
             }
             hwndRelated = window->getWindowHandle();
        }
        else hwndRelated = 0;
        unlock();
        break;
    }
end:
    dprintf(("GetWindow %x %d returned %x", getWindowHandle(), uCmd, hwndRelated));
    return hwndRelated;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::SetActiveWindow()
{
 HWND hwndActive;

    dprintf(("SetActiveWindow %x", getWindowHandle()));
    if(getStyle() & WS_CHILD) {
//    if(getStyle() & (WS_DISABLED | WS_CHILD)) {
        dprintf(("WARNING: Window is a child or disabled"));
        return 0;
    }

    if(GetActiveWindow() == getWindowHandle()) {
        dprintf(("Window already active"));
        return getWindowHandle();
    }
    if (HOOK_IsHooked( WH_CBT ))
    {
        CBTACTIVATESTRUCT cbta;
        LRESULT ret;

        cbta.fMouse = FALSE;
        cbta.hWndActive = GetActiveWindow();
        ret = HOOK_CallHooksA(WH_CBT, HCBT_ACTIVATE, getWindowHandle(), (LPARAM)&cbta);
        if(ret)
        {
            dprintf(("SetActiveWindow %x, CBT hook cancelled operation", getWindowHandle()));
            return cbta.hWndActive;
        }
    }
    SetWindowPos(HWND_TOP, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );

//    if(OSLibWinSetActiveWindow(OS2Hwnd) == FALSE) {
//        dprintf(("OSLibWinSetActiveWindow %x returned FALSE!", OS2Hwnd));
//    }
    hwndActive = GetActiveWindow();
    return (hwndActive) ? hwndActive : windowDesktop->getWindowHandle(); //pretend the desktop was active
}
//******************************************************************************
//Used to change active status of an mdi window
//******************************************************************************
BOOL Win32BaseWindow::DeactivateChildWindow()
{
    /* child windows get a WM_CHILDACTIVATE message */
    if((getStyle() & (WS_CHILD | WS_POPUP)) == WS_CHILD )
    {
        ULONG flags = OSLibWinGetWindowULong(getOS2WindowHandle(), OFFSET_WIN32FLAGS);
        OSLibWinSetWindowULong(getOS2WindowHandle(), OFFSET_WIN32FLAGS, (flags & ~WINDOWFLAG_ACTIVE));
        return TRUE;
    }
    DebugInt3();    //should not be called for non-child window
    return FALSE;
}
//******************************************************************************
//WM_ENABLE is sent to hwnd, but not to it's children (as it should be)
//******************************************************************************
BOOL Win32BaseWindow::EnableWindow(BOOL fEnable)
{
 BOOL rc;

    dprintf(("Win32BaseWindow::EnableWindow %x %d", getWindowHandle(), fEnable));
    //return true if previous state was disabled, else false (sdk docs)
    rc = (getStyle() & WS_DISABLED) != 0;
    if(rc && !fEnable) {
        SendMessageA(WM_CANCELMODE, 0, 0);
    }
    OSLibWinEnableWindow(OS2HwndFrame, fEnable);
    if(fEnable == FALSE) {
        //SvL: No need to clear focus as PM already does this
        if(getWindowHandle() == GetCapture()) {
                ReleaseCapture();  /* A disabled window can't capture the mouse */
                dprintf(("Released capture for window %x that is being disabled", getWindowHandle()));
        }
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::CloseWindow()
{
  return OSLibWinMinimizeWindow(OS2Hwnd);
}
//******************************************************************************
//TODO: Not be 100% correct; should return active window of current thread
//      or NULL when there is none -> WinQueryActiveWindow just returns
//      the current active window
//******************************************************************************
HWND Win32BaseWindow::GetActiveWindow()
{
 HWND          hwndActive;

  hwndActive = OSLibWinQueryActiveWindow();

  return OS2ToWin32Handle(hwndActive);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::hasWindowName(LPSTR wndname, BOOL fUnicode)
{
    INT len = GetWindowTextLength(fUnicode);
    BOOL res;

    if (wndname == NULL)
        return (len == 0);

    len++;
    if (fUnicode)
    {
        WCHAR *text = (WCHAR*)malloc(len*sizeof(WCHAR));

        GetWindowTextW(text,len);
        res = (lstrcmpW(text,(LPWSTR)wndname) == 0);
        free(text);
    }
    else
    {
        CHAR *text = (CHAR*)malloc(len*sizeof(CHAR));

        GetWindowTextA(text,len);
        res = (strcmp(text,wndname) == 0);
        free(text);
    }

    return res;
}
//******************************************************************************
//******************************************************************************
CHAR *Win32BaseWindow::getWindowNamePtrA()
{
    INT len = GetWindowTextLength(FALSE);
    CHAR *text;

    if (len == 0) return NULL;
    len++;
    text = (CHAR*)malloc(len*sizeof(CHAR));
    GetWindowTextA(text,len);

    return text;
}
//******************************************************************************
//******************************************************************************
WCHAR *Win32BaseWindow::getWindowNamePtrW()
{
    INT len = GetWindowTextLength(TRUE);
    WCHAR *text;

    if (len == 0) return NULL;
    len++;
    text = (WCHAR*)malloc(len*sizeof(WCHAR));
    GetWindowTextW(text,len);

    return text;
}
//******************************************************************************
//******************************************************************************
VOID Win32BaseWindow::freeWindowNamePtr(PVOID namePtr)
{
    if (namePtr) free(namePtr);
}
//******************************************************************************
//When using this API for a window that was created by a different process, NT
//does NOT send WM_GETTEXTLENGTH.
//******************************************************************************
int Win32BaseWindow::GetWindowTextLength(BOOL fUnicode)
{
    //if the destination window is created by this process, send message
    if(dwProcessId == currentProcessId) {
        if(fUnicode) {
             return SendInternalMessageW(WM_GETTEXTLENGTH,0,0);
        }
        else return SendInternalMessageA(WM_GETTEXTLENGTH,0,0);
    }
    //else get data directory from window structure
    //TODO: must lock window structure.... (TODO)
    if(fUnicode) {
        if(windowNameW) {
             return strlenW(windowNameW);
        }
        else return 0;
    }
    else {
        if(windowNameA) {
             return strlen(windowNameA);
        }
        else return 0;
    }
}
//******************************************************************************
//When using this API for a window that was created by a different process, NT
//does NOT send WM_GETTEXT.
//******************************************************************************
int Win32BaseWindow::GetWindowTextA(LPSTR lpsz, int cch)
{
    //if the destination window is created by this process, send message
    if(dwProcessId == currentProcessId) {
        return SendInternalMessageA(WM_GETTEXT,(WPARAM)cch,(LPARAM)lpsz);
    }
    //else get data directory from window structure
    if (!lpsz || !cch) return 0;
    if (!windowNameA) lpsz[0] = 0;
    else lstrcpynA(lpsz, windowNameA, cch);
    return min((windowNameA ? strlen(windowNameA) : 0), cch);
}
//******************************************************************************
//When using this API for a window that was created by a different process, NT
//does NOT send WM_GETTEXT.
//******************************************************************************
int Win32BaseWindow::GetWindowTextW(LPWSTR lpsz, int cch)
{
    //if the destination window is created by this process, send message
    if(dwProcessId == currentProcessId) {
        return SendInternalMessageW(WM_GETTEXT,(WPARAM)cch,(LPARAM)lpsz);
    }
    //else get data directory from window structure
    if (!lpsz || !cch) return 0;
    if (!windowNameW) lpsz[0] = 0;
    else lstrcpynW(lpsz, windowNameW, cch);
    return min((windowNameW ? strlenW(windowNameW) : 0), cch);
}
//******************************************************************************
//TODO: How does this work when the target window belongs to a different process???
//******************************************************************************
BOOL Win32BaseWindow::SetWindowTextA(LPSTR lpsz)
{
    return SendInternalMessageA(WM_SETTEXT,0,(LPARAM)lpsz);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::SetWindowTextW(LPWSTR lpsz)
{
    return SendInternalMessageW(WM_SETTEXT,0,(LPARAM)lpsz);
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::SetWindowLongA(int index, ULONG value, BOOL fUnicode)
{
 LONG oldval;

    switch(index) {
        case GWL_EXSTYLE:
        {
           STYLESTRUCT ss;

                if(dwExStyle == value) {
                    oldval = value;
                    break;
                }
                ss.styleOld = dwExStyle;
                ss.styleNew = value;
                dprintf(("SetWindowLong GWL_EXSTYLE %x old %x new style %x", getWindowHandle(), dwExStyle, value));
                SendInternalMessageA(WM_STYLECHANGING,GWL_EXSTYLE,(LPARAM)&ss);
                setExStyle(ss.styleNew);
                SendInternalMessageA(WM_STYLECHANGED,GWL_EXSTYLE,(LPARAM)&ss);
                oldval = ss.styleOld;
                break;
        }
        case GWL_STYLE:
        {
           STYLESTRUCT ss;

                //SvL: TODO: Can you change minimize or maximize status here too?

                if(dwStyle == value) {
                    oldval = value;
                    break;
                }
                dprintf(("SetWindowLong GWL_STYLE %x old %x new style %x (%x)", getWindowHandle(), dwStyle, value));
#ifdef DEBUG
                if((value & WS_CHILD) != (dwStyle & WS_CHILD)) {
                    DebugInt3(); //is this allowed?
                }
#endif
                value &= ~(WS_CHILD);
                ss.styleOld = getStyle();
                ss.styleNew = value | (ss.styleOld & WS_CHILD);
                SendInternalMessageA(WM_STYLECHANGING,GWL_STYLE,(LPARAM)&ss);
                setStyle(ss.styleNew);
                SendInternalMessageA(WM_STYLECHANGED,GWL_STYLE,(LPARAM)&ss);
                OSLibSetWindowStyle(getOS2FrameWindowHandle(), getOS2WindowHandle(), getStyle(), getExStyle());

                //TODO: Might not be correct to use ShowWindow here
                if((ss.styleOld & WS_VISIBLE) != (ss.styleNew & WS_VISIBLE)) {
                    if(ss.styleNew & WS_VISIBLE)
                         ShowWindow(SW_SHOWNOACTIVATE);
                    else ShowWindow(SW_HIDE);
                }
#ifdef DEBUG
                PrintWindowStyle(ss.styleNew, 0);
#endif
                oldval = ss.styleOld;
                break;
        }
        case GWL_WNDPROC:
        {
                //Note: Type of SetWindowLong determines new window proc type
                //      UNLESS the new window proc has already been registered
                //      (use the old type in that case)
                //      (VERIFIED in NT 4, SP6)
                WINDOWPROCTYPE type = WINPROC_GetProcType((HWINDOWPROC)value);
                if(type == WIN_PROC_INVALID) {
                    type = (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A;
                }
                oldval = (LONG)WINPROC_GetProc(win32wndproc, (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A);
                dprintf(("SetWindowLong GWL_WNDPROC %x old %x new wndproc %x", getWindowHandle(), oldval, value));
                WINPROC_SetProc((HWINDOWPROC *)&win32wndproc, (WNDPROC)value, type, WIN_PROC_WINDOW);
                break;
        }
        case GWL_HINSTANCE:
                oldval = hInstance;
                hInstance = value;
                break;

        case GWL_HWNDPARENT:
                oldval = SetParent((HWND)value);
                break;

        case GWL_ID:
                oldval = getWindowId();
                setWindowId(value);
                break;

        case GWL_USERDATA:
                oldval = userData;
                userData = value;
                break;

        default:
                if(index >= 0 && index + sizeof(ULONG) <= nrUserWindowBytes)
                {
                    oldval = *(ULONG *)(userWindowBytes + index);
                    *(ULONG *)(userWindowBytes + index) = value;
                    break;
                }
                dprintf(("WARNING: SetWindowLong%c %x %d %x returned %x INVALID index!", (fUnicode) ? 'W' : 'A', getWindowHandle(), index, value));
                SetLastError(ERROR_INVALID_INDEX);  //verified in NT4, SP6
                return 0;
    }
    //Note: NT4, SP6 does not set the last error to 0
    SetLastError(ERROR_SUCCESS);
    dprintf2(("SetWindowLong%c %x %d %x returned %x", (fUnicode) ? 'W' : 'A', getWindowHandle(), index, value, oldval));
    return oldval;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::GetWindowLongA(int index, BOOL fUnicode)
{
 ULONG value;

    switch(index) {
    case GWL_EXSTYLE:
        value = dwExStyle;
        break;
    case GWL_STYLE:
        value = dwStyle;
        break;
    case GWL_WNDPROC:
        value = (LONG)WINPROC_GetProc(win32wndproc, (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A);
        break;
    case GWL_HINSTANCE:
        value = hInstance;
        break;
    case GWL_HWNDPARENT:
        value = GetParent();
        break;
    case GWL_ID:
        value = getWindowId();
        break;
    case GWL_USERDATA:
        value = userData;
        break;
    default:
        if(index >= 0 && index + sizeof(ULONG) <= nrUserWindowBytes)
        {
            value = *(ULONG *)(userWindowBytes + index);
            break;
        }
        dprintf(("WARNING: GetWindowLong%c %x %d %x returned %x INVALID index!", (fUnicode) ? 'W' : 'A', getWindowHandle(), index, value));
        SetLastError(ERROR_INVALID_INDEX);  //verified in NT4, SP6
        return 0;
    }
    dprintf2(("GetWindowLong%c %x %d %x", (fUnicode) ? 'W' : 'A', getWindowHandle(), index, value));
    //Note: NT4, SP6 does not set the last error to 0
    SetLastError(ERROR_SUCCESS);
    return value;
}
//******************************************************************************
//******************************************************************************
WORD Win32BaseWindow::SetWindowWord(int index, WORD value)
{
 WORD oldval;

    if(index >= 0 && index + sizeof(WORD) <= nrUserWindowBytes)
    {
        oldval = *(WORD *)(userWindowBytes + index);
        *(WORD *)(userWindowBytes + index) = value;
        //Note: NT4, SP6 does not set the last error to 0
        dprintf2(("SetWindowWord %x %d %x returned %x", getWindowHandle(), index, value, oldval));
        SetLastError(ERROR_SUCCESS);
        return oldval;
    }
    switch(index)
    {
    case GWW_HINSTANCE:
        oldval = hInstance;
        hInstance = value;
        break;

    case GWW_HWNDPARENT:
        oldval = SetParent((HWND)(WNDHANDLE_MAGIC_HIGHWORD | value));
        break;

    case GWW_ID:
        oldval = getWindowId();
        setWindowId(value);
        break;

    default:
        dprintf(("WARNING: SetWindowWord %x %d %x returned %x INVALID index!", getWindowHandle(), index, value));
        SetLastError(ERROR_INVALID_INDEX);  //verified in NT4, SP6
        return 0;
    }
    //Note: NT4, SP6 does not set the last error to 0
    SetLastError(ERROR_SUCCESS);
    dprintf2(("SetWindowWord %x %d %x returned %x", getWindowHandle(), index, value, oldval));
    return oldval;
}
//******************************************************************************
//******************************************************************************
WORD Win32BaseWindow::GetWindowWord(int index)
{
    if(index >= 0 && index + sizeof(WORD) <= nrUserWindowBytes)
    {
        //Note: NT4, SP6 does not set the last error to 0
        SetLastError(ERROR_SUCCESS);
        dprintf2(("GetWindowWord %x %d %x", getWindowHandle(), index, *(WORD *)(userWindowBytes + index)));
        return *(WORD *)(userWindowBytes + index);
    }
    switch(index)
    {
    case GWW_ID:         
    	if(HIWORD(getWindowId()))
            dprintf(("WARNING: GWW_ID: discards high bits of 0x%08x!\n", getWindowId()));
        return (WORD)getWindowId();

    case GWW_HWNDPARENT: 
        dprintf(("WARNING: GWW_HWNDPARENT: discards high bits of 0x%08x!\n", GetParent()));
    	return (WORD) GetParent();

    case GWW_HINSTANCE:  
    	if (HIWORD(hInstance))
            dprintf(("WARNING: GWW_HINSTANCE: discards high bits of 0x%08x!\n", hInstance));
        return (WORD)hInstance;
    }

    dprintf(("WARNING: GetWindowWord %x %d returned %x INVALID index!", getWindowHandle(), index));
    SetLastError(ERROR_INVALID_INDEX);  //verified in NT4, SP6
    return 0;
}
//******************************************************************************
//******************************************************************************
void Win32BaseWindow::setWindowId(DWORD id)
{
    dwIDMenu = id;
}
//******************************************************************************
//Locates window in linked list and increases reference count (if found)
//Window object must be unreferenced after usage
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetWindowFromHandle(HWND hwnd)
{
 Win32BaseWindow *window;

////TODO: temporary workaround for crashes in Opera (pmwinx; releasesemaphore)
////      while browsing
////      Not thread safe now!
////    lock(&critsect);
    if(HwGetWindowHandleData(hwnd, (DWORD *)&window) == TRUE) {
         if(window) {
////             dprintf(("addRef %x; refcount %d", hwnd, window->getRefCount()+1));
             window->addRef();
         }
////         unlock(&critsect);
         return window;
    }
////    unlock(&critsect);
//    dprintf2(("Win32BaseWindow::GetWindowFromHandle: not a win32 window %x", hwnd));
    return NULL;
}
//******************************************************************************
//Locates window in linked list and increases reference count (if found)
//Window object must be unreferenced after usage
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetWindowFromOS2Handle(HWND hwndOS2)
{
 DWORD            magic;
 HWND             hwnd; 

    if(hwndOS2 == OSLIB_HWND_DESKTOP)
    {
        windowDesktop->addRef();
        return windowDesktop;
    }

    hwnd  = (HWND)OSLibWinGetWindowULong(hwndOS2, OFFSET_WIN32WNDPTR);
    magic = OSLibWinGetWindowULong(hwndOS2, OFFSET_WIN32PM_MAGIC);

    if(hwnd && CheckMagicDword(magic)) {
        return GetWindowFromHandle(hwnd);
    }
//  dprintf2(("Win32BaseWindow::GetWindowFromOS2Handle: not an Odin os2 window %x", hwndOS2));
    return 0;
}
//******************************************************************************
//Locates window in linked list and increases reference count (if found)
//Window object must be unreferenced after usage
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetWindowFromOS2FrameHandle(HWND hwnd)
{
    return GetWindowFromOS2Handle(OSLibWinWindowFromID(hwnd,OSLIB_FID_CLIENT));
}
//******************************************************************************
//******************************************************************************
HWND WIN32API Win32ToOS2Handle(HWND hwnd)
{
    HWND hwndOS2;

    Win32BaseWindow *window = Win32BaseWindow::GetWindowFromHandle(hwnd);

    if(window) {
            hwndOS2 = window->getOS2WindowHandle();
            RELEASE_WNDOBJ(window);
            return hwndOS2;
    }
//    dprintf2(("Win32BaseWindow::Win32ToOS2Handle: not a win32 window %x", hwnd));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API Win32ToOS2FrameHandle(HWND hwnd)
{
    HWND hwndOS2;

    Win32BaseWindow *window = Win32BaseWindow::GetWindowFromHandle(hwnd);

    if(window) {
            hwndOS2 = window->getOS2FrameWindowHandle();
            RELEASE_WNDOBJ(window);
            return hwndOS2;
    }
//    dprintf2(("Win32BaseWindow::Win32ToOS2Handle: not a win32 window %x", hwnd));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND WIN32API OS2ToWin32Handle(HWND hwnd)
{
    Win32BaseWindow *window = Win32BaseWindow::GetWindowFromOS2Handle(hwnd);
    HWND hwndWin32;

    if(window) {
            hwndWin32 = window->getWindowHandle();
            RELEASE_WNDOBJ(window);
            return hwndWin32;
    }
    window = Win32BaseWindow::GetWindowFromOS2FrameHandle(hwnd);
    if(window) {
            hwndWin32 = window->getWindowHandle();
            RELEASE_WNDOBJ(window);
            return hwndWin32;
    }

//    dprintf2(("Win32BaseWindow::OS2ToWin32Handle: not a win32 window %x", hwnd));
    return 0;
//    else    return hwnd;    //OS/2 window handle
}
#ifdef DEBUG
LONG  Win32BaseWindow::addRef()
{
    dprintf2(("addRef %x %d", getWindowHandle(), getRefCount()+1));
    return GenericObject::addRef();
}
//******************************************************************************
//******************************************************************************
LONG  Win32BaseWindow::release(char *function, int line)
{
    dprintf2(("release %s %d %x %d", function, line, getWindowHandle(), getRefCount()-1));
    return GenericObject::release();
}
#endif
//******************************************************************************
//******************************************************************************
GenericObject   *Win32BaseWindow::windows  = NULL;
CRITICAL_SECTION Win32BaseWindow::critsect = {0};

//******************************************************************************
//******************************************************************************
#ifdef DEBUG
void PrintWindowStyle(DWORD dwStyle, DWORD dwExStyle)
{
 char style[256] = "";
 char exstyle[256] = "";

    /* Window styles */
    if(dwStyle & WS_CHILD)
        strcat(style, "WS_CHILD ");
    if(dwStyle & WS_POPUP)
        strcat(style, "WS_POPUP ");
    if(dwStyle & WS_VISIBLE)
        strcat(style, "WS_VISIBLE ");
    if(dwStyle & WS_DISABLED)
        strcat(style, "WS_DISABLED ");
    if(dwStyle & WS_CLIPSIBLINGS)
        strcat(style, "WS_CLIPSIBLINGS ");
    if(dwStyle & WS_CLIPCHILDREN)
        strcat(style, "WS_CLIPCHILDREN ");
    if(dwStyle & WS_MAXIMIZE)
        strcat(style, "WS_MAXIMIZE ");
    if(dwStyle & WS_MINIMIZE)
        strcat(style, "WS_MINIMIZE ");
    if(dwStyle & WS_GROUP)
        strcat(style, "WS_GROUP ");
    if(dwStyle & WS_TABSTOP)
        strcat(style, "WS_TABSTOP ");

    if((dwStyle & WS_CAPTION) == WS_CAPTION)
        strcat(style, "WS_CAPTION ");
    if(dwStyle & WS_DLGFRAME)
        strcat(style, "WS_DLGFRAME ");
    if(dwStyle & WS_BORDER)
        strcat(style, "WS_BORDER ");

    if(dwStyle & WS_VSCROLL)
        strcat(style, "WS_VSCROLL ");
    if(dwStyle & WS_HSCROLL)
        strcat(style, "WS_HSCROLL ");
    if(dwStyle & WS_SYSMENU)
        strcat(style, "WS_SYSMENU ");
    if(dwStyle & WS_THICKFRAME)
        strcat(style, "WS_THICKFRAME ");
    if(dwStyle & WS_MINIMIZEBOX)
        strcat(style, "WS_MINIMIZEBOX ");
    if(dwStyle & WS_MAXIMIZEBOX)
        strcat(style, "WS_MAXIMIZEBOX ");

    if(dwExStyle & WS_EX_DLGMODALFRAME)
        strcat(exstyle, "WS_EX_DLGMODALFRAME ");
    if(dwExStyle & WS_EX_ACCEPTFILES)
        strcat(exstyle, "WS_EX_ACCEPTFILES ");
    if(dwExStyle & WS_EX_NOPARENTNOTIFY)
        strcat(exstyle, "WS_EX_NOPARENTNOTIFY ");
    if(dwExStyle & WS_EX_TOPMOST)
        strcat(exstyle, "WS_EX_TOPMOST ");
    if(dwExStyle & WS_EX_TRANSPARENT)
        strcat(exstyle, "WS_EX_TRANSPARENT ");

    if(dwExStyle & WS_EX_MDICHILD)
        strcat(exstyle, "WS_EX_MDICHILD ");
    if(dwExStyle & WS_EX_TOOLWINDOW)
        strcat(exstyle, "WS_EX_TOOLWINDOW ");
    if(dwExStyle & WS_EX_WINDOWEDGE)
        strcat(exstyle, "WS_EX_WINDOWEDGE ");
    if(dwExStyle & WS_EX_CLIENTEDGE)
        strcat(exstyle, "WS_EX_CLIENTEDGE ");
    if(dwExStyle & WS_EX_CONTEXTHELP)
        strcat(exstyle, "WS_EX_CONTEXTHELP ");
    if(dwExStyle & WS_EX_RIGHT)
        strcat(exstyle, "WS_EX_RIGHT ");
    if(dwExStyle & WS_EX_LEFT)
        strcat(exstyle, "WS_EX_LEFT ");
    if(dwExStyle & WS_EX_RTLREADING)
        strcat(exstyle, "WS_EX_RTLREADING ");
    if(dwExStyle & WS_EX_LTRREADING)
        strcat(exstyle, "WS_EX_LTRREADING ");
    if(dwExStyle & WS_EX_LEFTSCROLLBAR)
        strcat(exstyle, "WS_EX_LEFTSCROLLBAR ");
    if(dwExStyle & WS_EX_RIGHTSCROLLBAR)
        strcat(exstyle, "WS_EX_RIGHTSCROLLBAR ");
    if(dwExStyle & WS_EX_CONTROLPARENT)
        strcat(exstyle, "WS_EX_CONTROLPARENT ");
    if(dwExStyle & WS_EX_STATICEDGE)
        strcat(exstyle, "WS_EX_STATICEDGE ");
    if(dwExStyle & WS_EX_APPWINDOW)
        strcat(exstyle, "WS_EX_APPWINDOW ");

    dprintf(("Window style:   %x %s", dwStyle, style));
    dprintf(("Window exStyle: %x %s", dwExStyle, exstyle));
}
#endif
//******************************************************************************
//******************************************************************************
