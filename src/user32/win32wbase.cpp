/* $Id: win32wbase.cpp,v 1.161 2000-02-14 17:30:11 cbratschi Exp $ */
/*
 * Win32 Window Base Class for OS/2
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 * Copyright 1999-2000 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Parts based on Wine Windows code (windows\win.c)
 *
 * Copyright 1993, 1994, 1996 Alexandre Julliard
 *           1995 Alex Korobka
 *
 * TODO: Not thread/process safe
 *
 * NOTE: Client rectangle always relative to frame window; window rectangle in screen coordinates
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
#include <win32wbase.h>
#include <winres.h>
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
#include "pmframe.h"
#include "win32wdesktop.h"
#include "pmwindow.h"
#include "controls.h"
#include <wprocess.h>
#include "winmouse.h"
#include <win\hook.h>
#include <menu.h>
#define INCL_TIMERWIN32
#include "timer.h"

#define SC_ABOUTWINE            (SC_SCREENSAVE+1)
#define SC_PUTMARK              (SC_SCREENSAVE+2)

/* bits in the dwKeyData */
#define KEYDATA_ALT         0x2000
#define KEYDATA_PREVSTATE   0x4000

void PrintWindowStyle(DWORD dwStyle, DWORD dwExStyle);

static fDestroyAll = FALSE;
//For quick lookup of current process id
static ULONG currentProcessId = -1;

//******************************************************************************
//******************************************************************************
Win32BaseWindow::Win32BaseWindow(DWORD objType) : GenericObject(&windows, objType)
{
  Init();
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow::Win32BaseWindow(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
                        : GenericObject(&windows, OBJTYPE_WINDOW), ChildWindow()
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
  fNoSizeMsg       = FALSE;
  fIsDestroyed     = FALSE;
  fDestroyWindowCalled = FALSE;
  fCreated         = FALSE;
  fTaskList        = FALSE;
  fParentDC        = FALSE;

  windowNameA      = NULL;
  windowNameW      = NULL;
  wndNameLength    = 0;

  userWindowLong   = NULL;;
  nrUserWindowLong = 0;

  magic            = WIN32PM_MAGIC;
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
  win32wndproc     = 0;
  hInstance        = 0;
  dwIDMenu         = 0; //0xFFFFFFFF; //default -1
  userData         = 0;
  contextHelpId    = 0;
  hotkey           = 0;

  pOldFrameProc = NULL;

  hwndLinkAfter    = HWND_BOTTOM;
  flags            = 0;
  isIcon           = FALSE;
  lastHitTestVal   = HTOS_NORMAL;
  owner            = NULL;
  windowClass      = 0;

  iconResource       = NULL;

  EraseBkgndFlag     = TRUE;
  PSEraseFlag        = FALSE;
  SuppressEraseFlag  = FALSE;

  horzScrollInfo     = NULL;
  vertScrollInfo     = NULL;

  ownDC              = 0;
  hWindowRegion      = 0;
  hClipRegion        = 0;

  if(currentProcessId == -1)
  {
        currentProcessId = GetCurrentProcessId();
  }
  dwThreadId         = GetCurrentThreadId();
  dwProcessId        = currentProcessId;
}
//******************************************************************************
//todo get rid of resources (menu, icon etc)
//******************************************************************************
Win32BaseWindow::~Win32BaseWindow()
{
    OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, 0);
    OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, 0);

    if(!fDestroyAll && getParent() && getParent()->getFirstChild() == this && getNextChild() == NULL)
    {
        //if we're the last child that's being destroyed and our
        //parent window was also destroyed, then we delete the parent object
        if(getParent()->IsWindowDestroyed())
        {
            dprintf(("Last Child (%x) destroyed, get rid of our parent window (%x)", getWindowHandle(), getParent()->getWindowHandle()));
            delete getParent();
            setParent(NULL);  //or else we'll crash in the dtor of the ChildWindow class
        }
    }
    else
    if(fDestroyAll) {
        dprintf(("Destroying window %x %s", getWindowHandle(), windowNameA));
        setParent(NULL);  //or else we'll crash in the dtor of the ChildWindow class
    }

    if(isOwnDC())
        releaseOwnDC(ownDC);

    if(Win32Hwnd)
        HwFreeWindowHandle(Win32Hwnd);

    if(userWindowLong)
        free(userWindowLong);

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
                    dprintf(("Bad parent %04x\n", cs->hwndParent ));
                    SetLastError(ERROR_INVALID_PARAMETER);
                    return FALSE;
            }
    }
    else
    if ((cs->style & WS_CHILD) && !(cs->style & WS_POPUP)) {
            dprintf(("No parent for child window\n" ));
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;  /* WS_CHILD needs a parent, but WS_POPUP doesn't */
    }

  /* Find the window class */
  windowClass = Win32WndClass::FindClass(cs->hInstance, (LPSTR)classAtom);
  if (!windowClass)
  {
        GlobalGetAtomNameA( classAtom, buffer, sizeof(buffer) );
        dprintf(("Bad class '%s'\n", buffer ));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
  }
#ifdef DEBUG
  if(HIWORD(cs->lpszClass))
  {
        char *astring;

        if(isUnicode) astring = UnicodeToAsciiString((LPWSTR)cs->lpszClass);
        else          astring = (char *)cs->lpszClass;

        dprintf(("Window class %s", astring));
        if(isUnicode) FreeAsciiString(astring);
  }
  else  dprintf(("Window class %x", cs->lpszClass));
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

  if (cs->x < 0) cs->x = 0;
  if (cs->y < 0) cs->y = 0;

  //Allocate window words
  nrUserWindowLong = windowClass->getExtraWndWords();
  if(nrUserWindowLong) {
        userWindowLong = (ULONG *)_smalloc(nrUserWindowLong);
        memset(userWindowLong, 0, nrUserWindowLong);
  }

  if ((cs->style & WS_CHILD) && cs->hwndParent)
  {
        SetParent(cs->hwndParent);
        owner = GetWindowFromHandle(cs->hwndParent);
        if(owner == NULL)
        {
            dprintf(("HwGetWindowHandleData couldn't find owner window %x!!!", cs->hwndParent));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return FALSE;
        }
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
            owner = GetWindowFromHandle(cs->hwndParent)->GetTopParent();
            if(owner == NULL)
            {
                dprintf(("HwGetWindowHandleData couldn't find owner window %x!!!", cs->hwndParent));
                SetLastError(ERROR_INVALID_WINDOW_HANDLE);
                return FALSE;
            }
        }
  }

  WINPROC_SetProc((HWINDOWPROC *)&win32wndproc, windowClass->getWindowProc(), WINPROC_GetProcType(windowClass->getWindowProc()), WIN_PROC_WINDOW);
  hInstance = cs->hInstance;
  dwStyle   = cs->style & ~WS_VISIBLE;
  dwExStyle = cs->dwExStyle;

  hwndLinkAfter = HWND_TOP;
  if(CONTROLS_IsControl(this, BUTTON_CONTROL) && ((dwStyle & 0x0f) == BS_GROUPBOX))
  {
        hwndLinkAfter = HWND_BOTTOM;
        dwStyle |= WS_CLIPSIBLINGS;
  }
  else
  if(CONTROLS_IsControl(this, STATIC_CONTROL) && !(dwStyle & WS_GROUP)) {
        dwStyle |= WS_CLIPSIBLINGS;
  }

  /* Increment class window counter */
  windowClass->IncreaseWindowCount();

  if (HOOK_IsHooked( WH_CBT ))
  {
        CBT_CREATEWNDA cbtc;
        LRESULT ret;

        cbtc.lpcs = cs;
        cbtc.hwndInsertAfter = hwndLinkAfter;
        ret = HOOK_CallHooksA(WH_CBT, HCBT_CREATEWND, getWindowHandle(), (LPARAM)&cbtc);
        if(ret)
        {
            dprintf(("CBT-hook returned 0!!"));
            SetLastError(ERROR_CAN_NOT_COMPLETE); //todo: wrong error
            return FALSE;
        }
  }

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
        wndNameLength = strlen(cs->lpszName);
        windowNameA = (LPSTR)_smalloc(wndNameLength+1);
        strcpy(windowNameA,cs->lpszName);
        windowNameW = (LPWSTR)_smalloc((wndNameLength+1)*sizeof(WCHAR));
        lstrcpyAtoW(windowNameW,windowNameA);
        windowNameA[wndNameLength] = 0;
        windowNameW[wndNameLength] = 0;
    }
    else
    {
        wndNameLength = lstrlenW((LPWSTR)cs->lpszName);
        windowNameA = (LPSTR)_smalloc(wndNameLength+1);
        lstrcpyWtoA(windowNameA,(LPWSTR)cs->lpszName);
        windowNameW = (LPWSTR)_smalloc((wndNameLength+1)*sizeof(WCHAR));
        lstrcpyW(windowNameW,(LPWSTR)cs->lpszName);
        windowNameA[wndNameLength] = 0;
        windowNameW[wndNameLength] = 0;
    }
  }

  //copy pointer of CREATESTRUCT for usage in MsgCreate method
  tmpcs = cs;

  //Store our window object pointer in thread local memory, so PMWINDOW.CPP can retrieve it
  THDB *thdb = GetThreadTHDB();

  if(thdb == NULL) {
        dprintf(("Window creation failed - thdb == NULL")); //this is VERY bad
        ExitProcess(666);
        return FALSE;
  }

  thdb->newWindow = (ULONG)this;

  DWORD dwOSWinStyle;

  OSLibWinConvertStyle(dwStyle,dwExStyle,&dwOSWinStyle);
  if (((dwStyle & (WS_CAPTION | WS_SYSMENU | 0xC0000000)) == (WS_CAPTION | WS_SYSMENU))) fTaskList = TRUE;

  OS2Hwnd = OSLibWinCreateWindow((getParent()) ? getParent()->getOS2WindowHandle() : OSLIB_HWND_DESKTOP,
                                 dwOSWinStyle,(char *)windowNameA,
                                 (owner) ? owner->getOS2WindowHandle() : OSLIB_HWND_DESKTOP,
                                 (hwndLinkAfter == HWND_BOTTOM) ? TRUE : FALSE,
                                 &OS2HwndFrame, 0, fTaskList,fXDefault | fCXDefault,windowClass->getStyle() & CS_SAVEBITS);
  if(OS2Hwnd == 0) {
        dprintf(("Window creation failed!!"));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
  }

  SetLastError(0);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::MsgCreate(HWND hwndFrame, HWND hwndClient)
{
 CREATESTRUCTA  *cs = tmpcs;  //pointer to CREATESTRUCT used in CreateWindowExA method
 POINT maxSize, maxPos, minTrack, maxTrack;

  OS2Hwnd      = hwndClient;
  OS2HwndFrame = hwndFrame;

  fNoSizeMsg = TRUE;

  if(OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, (ULONG)this) == FALSE) {
        dprintf(("WM_CREATE: WinSetWindowULong %X failed!!", OS2Hwnd));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
  }
  if(OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, WIN32PM_MAGIC) == FALSE) {
        dprintf(("WM_CREATE: WinSetWindowULong2 %X failed!!", OS2Hwnd));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
  }

  //adjust CW_USEDEFAULT position
  if (fXDefault | fCXDefault)
  {
    RECT rect;

    OSLibWinQueryWindowRect(OS2HwndFrame,&rect,RELATIVE_TO_SCREEN);
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

  OSLibWinSetOwner(OS2Hwnd, OS2HwndFrame);

  fakeWinBase.hwndThis     = OS2Hwnd;
  fakeWinBase.pWindowClass = windowClass;

  //Set icon from class
  if(windowClass->getIcon())
        SetIcon(windowClass->getIcon());
  /* Get class or window DC if needed */
  if(windowClass->getStyle() & CS_OWNDC) {
        dprintf(("Class with CS_OWNDC style"));
        ownDC = GetDC(getWindowHandle()); //TODO: or GetWindowDC???
  }
  else
  if (windowClass->getStyle() & CS_PARENTDC)  {
        dprintf(("WARNING: Class with CS_PARENTDC style!"));
        fParentDC = TRUE;
        ownDC = 0;
  }
  else
  if (windowClass->getStyle() & CS_CLASSDC)  {
        dprintf(("WARNING: Class with CS_CLASSDC style!"));
        ownDC = 0;
  }
  /* Set the window menu */
  if ((dwStyle & (WS_CAPTION | WS_CHILD)) == WS_CAPTION )
  {
        if (cs->hMenu) {
            ::SetMenu(getWindowHandle(), cs->hMenu);
        }
        else {
                if (windowClass->getMenuNameA()) {
                        cs->hMenu = LoadMenuA(cs->hInstance, windowClass->getMenuNameA());
                        if (cs->hMenu) ::SetMenu(getWindowHandle(), cs->hMenu );
                }
        }
  }
  else
  {
        setWindowId((DWORD)cs->hMenu);
  }
  hSysMenu = (dwStyle & WS_SYSMENU) ? MENU_GetSysMenu(Win32Hwnd,0):0;

  // Subclass frame
  pOldFrameProc = FrameSubclassFrameWindow(this);

  //preset rects
  rectWindow.left = cs->x;
  rectWindow.right = cs->x+cs->cx;
  rectWindow.top = cs->y;
  rectWindow.bottom = cs->y+cs->cy;
  rectClient = rectWindow; //dummy client rect
  OffsetRect(&rectClient, -rectClient.left, -rectClient.top);

  if (getParent()) mapWin32Rect(getParent()->getOS2WindowHandle(),OSLIB_HWND_DESKTOP,&rectWindow);
  /* Send the WM_CREATE message
   * Perhaps we shouldn't allow width/height changes as well.
   * See p327 in "Internals".
   */
  maxPos.x = rectWindow.left; maxPos.y = rectWindow.top;

  //Note: Solitaire crashes when receiving WM_SIZE messages before WM_CREATE
  //fNoSizeMsg = FALSE;
  fCreated = TRUE;

  if (SendInternalMessageA(WM_NCCREATE,0,(LPARAM)cs))
  {
        RECT tmpRect;

        //update rect
        rectWindow.left = cs->x;
        rectWindow.right = cs->x+cs->cx;
        rectWindow.top = cs->y;
        rectWindow.bottom = cs->y+cs->cy;
        tmpRect = rectWindow;
        if (getParent()) mapWin32Rect(getParent()->getOS2WindowHandle(),OSLIB_HWND_DESKTOP,&rectWindow);
        OffsetRect(&rectWindow, maxPos.x - rectWindow.left, maxPos.y - rectWindow.top);

        rectClient = rectWindow;
        OffsetRect(&rectClient, -rectClient.left, -rectClient.top);

        //set the window size and update the client
        SetWindowPos(hwndLinkAfter, tmpRect.left, tmpRect.top, tmpRect.right-tmpRect.left, tmpRect.bottom-tmpRect.top,SWP_NOACTIVATE | SWP_NOREDRAW | SWP_FRAMECHANGED);
        fNoSizeMsg = FALSE;
        if (cs->style & WS_VISIBLE) dwStyle |= WS_VISIBLE; //program could change position in WM_CREATE
        if( (SendInternalMessageA(WM_CREATE, 0, (LPARAM)cs )) != -1 )
        {
            if(!(flags & WIN_NEED_SIZE))
            {
                SendInternalMessageA(WM_SIZE, SIZE_RESTORED,
                                MAKELONG(rectClient.right-rectClient.left,
                                         rectClient.bottom-rectClient.top));

                SendInternalMessageA(WM_MOVE,0,MAKELONG(rectClient.left,rectClient.top));
            }

            if( (getStyle() & WS_CHILD) && !(getExStyle() & WS_EX_NOPARENTNOTIFY) )
            {
                /* Notify the parent window only */
                SendInternalMessageA(WM_PARENTNOTIFY, MAKEWPARAM(WM_CREATE, getWindowId()), (LPARAM)getWindowHandle());
                if(!::IsWindow(getWindowHandle()))
                {
                    dprintf(("Createwindow: WM_PARENTNOTIFY destroyed window"));
                    goto end;
                }
            }

            //CB: timew32.exe, blockcad.exe -> children not shown!!!
            if (cs->style & WS_VISIBLE) ShowWindow(sw);

            /* Call WH_SHELL hook */
            if (!(getStyle() & WS_CHILD) && !owner)
                HOOK_CallHooksA(WH_SHELL, HSHELL_WINDOWCREATED, getWindowHandle(), 0 );

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

    fIsDestroyed = TRUE;

    if(fDestroyWindowCalled == FALSE)
    {//this window was destroyed because DestroyWindow was called for it's parent
     //so: send a WM_PARENTNOTIFY now as that hasn't happened yet
        if((getStyle() & WS_CHILD) && !(getExStyle() & WS_EX_NOPARENTNOTIFY))
        {
            if(getParent())
            {
                    /* Notify the parent window only */
                    getParent()->SendMessageA(WM_PARENTNOTIFY, MAKEWPARAM(WM_DESTROY, getWindowId()), (LPARAM)getWindowHandle());
            }
            else    DebugInt3();
        }
    }
    SendInternalMessageA(WM_DESTROY, 0, 0);
    if(::IsWindow(hwnd) == FALSE) {
        //object already destroyed, so return immediately
        return 1;
    }
    SendInternalMessageA(WM_NCDESTROY, 0, 0);

    TIMER_KillTimerFromWindow(OS2Hwnd);

    if(getFirstChild() == NULL) {
        delete this;
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
    if(fNoSizeMsg) {
        return 1;
    }

    if(fShow) {
            setStyle(getStyle() | WS_VISIBLE);
    }
    else    setStyle(getStyle() & ~WS_VISIBLE);

    return SendInternalMessageA(WM_SHOWWINDOW, fShow, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgPosChanging(LPARAM lp)
{
    //SvL: Notes crashes when switching views (calls DestroyWindow -> PM sends
    //     a WM_WINDOWPOSCHANGED msg -> crash)
    if(fNoSizeMsg || fDestroyWindowCalled)
        return 0;

    return SendInternalMessageA(WM_WINDOWPOSCHANGING, 0, lp);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgPosChanged(LPARAM lp)
{
    //SvL: Notes crashes when switching views (calls DestroyWindow -> PM sends
    //     a WM_WINDOWPOSCHANGED msg -> crash)
    if(fNoSizeMsg || fDestroyWindowCalled)
        return 1;

    return SendInternalMessageA(WM_WINDOWPOSCHANGED, 0, lp);
}
//******************************************************************************
//******************************************************************************
#if 0
ULONG Win32BaseWindow::MsgMinMax()
{

}
#endif
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
ULONG Win32BaseWindow::MsgHitTest(ULONG x, ULONG y)
{
  lastHitTestVal = SendInternalMessageA(WM_NCHITTEST, 0, MAKELONG((USHORT)x, (USHORT)y));
  dprintf2(("MsgHitTest (%d,%d) (%d,%d) (%d,%d) returned %x", x, y, rectWindow.left, rectWindow.right, rectWindow.top, rectWindow.bottom, lastHitTestVal));
  return lastHitTestVal;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgActivate(BOOL fActivate, BOOL fMinimized, HWND hwnd, HWND hwndOS2Win)
{
 ULONG rc, procidhwnd = -1, threadidhwnd = 0;


    //According to SDK docs, if app returns FALSE & window is being deactivated,
    //default processing is cancelled
    //TODO: According to Wine we should proceed anyway if window is sysmodal
#if 0
    if(SendInternalMessageA(WM_NCACTIVATE, fActivate, 0) == FALSE && !fActivate)
    {
        return 0;
    }
#endif
    /* child windows get WM_CHILDACTIVATE message */
    if((getStyle() & (WS_CHILD | WS_POPUP)) == WS_CHILD )
    {
        SendInternalMessageA(WM_CHILDACTIVATE, 0, 0L);
        return 0;
    }

    rc = SendInternalMessageA(WM_ACTIVATE, MAKELONG((fActivate) ? WA_ACTIVE : WA_INACTIVE, fMinimized), hwnd);

    if(hwndOS2Win) {
            threadidhwnd = O32_GetWindowThreadProcessId(hwndOS2Win, &procidhwnd);
    }

    if(fActivate) {
            SendInternalMessageA(WM_ACTIVATEAPP, 1, dwThreadId);    //activate; specify window thread id
    }
    else    SendInternalMessageA(WM_ACTIVATEAPP, 0, threadidhwnd);  //deactivate; specify thread id of other process
    return rc;
}
//******************************************************************************
//TODO: Is this correct and complete?
//Add print screen, break & numlock
//******************************************************************************
void Win32BaseWindow::setExtendedKey(ULONG virtualkey, ULONG *lParam)
{
    switch(virtualkey) {
    case VK_DOWN:
    case VK_UP:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_END:
    case VK_DIVIDE:
    case VK_DELETE:
    case VK_EXECUTE: //Numeric enter key?
    case VK_HOME:
    case VK_INSERT:
    case VK_RCONTROL:
    case VK_RMENU: //is this the right alt???
        *lParam = *lParam | (1<<24);
    }
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
    return  SendInternalMessageA(WM_SETFOCUS, hwnd, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgKillFocus(HWND hwnd)
{
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
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCMBUTTONDBLCLK:
                if (!(windowClass && windowClass->getClassLongA(GCL_STYLE) & CS_DBLCLKS) && (msg->message != WM_NCLBUTTONDBLCLK))
                {
                    msg->message = msg->message - (WM_LBUTTONDBLCLK - WM_LBUTTONDOWN); //dblclick -> down
                    MsgButton(msg);
                    msg->message++; //button-up
                    return MsgButton(msg);
                }
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

    if(ISMOUSE_CAPTURED())
    {
        if(DInputMouseHandler(getWindowHandle(), MOUSEMSG_BUTTON, msg->pt.x, msg->pt.y))
            return 0;
    }

    if(fClick)
    {
     HWND hwndTop;

        /* Activate the window if needed */
        hwndTop = (GetTopParent()) ? GetTopParent()->getWindowHandle() : 0;

        HWND hwndActive = GetActiveWindow();
        if (hwndTop && (getWindowHandle() != hwndActive))
        {
                LONG ret = SendInternalMessageA(WM_MOUSEACTIVATE, hwndTop,
                                                MAKELONG( lastHitTestVal, msg->message) );

#if 0
                if ((ret == MA_ACTIVATEANDEAT) || (ret == MA_NOACTIVATEANDEAT))
                         eatMsg = TRUE;
#endif
                if(((ret == MA_ACTIVATE) || (ret == MA_ACTIVATEANDEAT))
                   && (hwndTop != GetForegroundWindow()) )
                {
                      ::SetActiveWindow(hwndTop);
                }
        }
    }

    SendInternalMessageA(WM_SETCURSOR, getWindowHandle(), MAKELONG(lastHitTestVal, msg->message));

    return  SendInternalMessageA(msg->message, msg->wParam, msg->lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgPaint(ULONG tmp1, BOOL select)
{
    if (select && isIcon)
        return SendInternalMessageA(WM_PAINTICON, 0, 0);
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
        hdcErase = O32_GetDC(OS2Hwnd);

    if(isIcon)
        rc = SendInternalMessageA(WM_ICONERASEBKGND, hdcErase, 0);
    else
        rc = SendInternalMessageA(WM_ERASEBKGND, hdcErase, 0);
    if (hdc == 0)
        O32_ReleaseDC(OS2Hwnd, hdcErase);
    return (rc);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgMouseMove(MSG *msg)
{
    if(ISMOUSE_CAPTURED()) {
        if(DInputMouseHandler(getWindowHandle(), MOUSEMSG_MOVE, msg->pt.x, msg->pt.y))
            return 0;
    }

    //TODO: hiword should be 0 if window enters menu mode (SDK docs)
    SendInternalMessageA(WM_SETCURSOR, Win32Hwnd, MAKELONG(lastHitTestVal, msg->message));

    //translated message == WM_(NC)MOUSEMOVE
    return SendInternalMessageA(msg->message, msg->wParam, msg->lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgChar(MSG *msg)
{
    if(ISKDB_CAPTURED())
    {
        DInputKeyBoardHandler(msg);
    }
    return DispatchMsgA(msg);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgNCPaint()
{
  RECT rect;

  if (GetOS2UpdateRect(OS2HwndFrame,&rect))
  {
    HRGN hrgn;
    ULONG rc;
    RECT client = rectClient;

////    mapWin32Rect(getParent() ? getParent()->getOS2WindowHandle():OSLIB_HWND_DESKTOP,OS2HwndFrame,&client);
    if ((rect.left >= client.left) && (rect.left < client.right) &&
        (rect.right >= client.left) && (rect.right < client.right) &&
        (rect.top  >= client.top) && (rect.top < client.bottom) &&
        (rect.bottom >= client.top) && (rect.bottom < client.bottom))
      return 0;

    dprintf(("MsgNCPaint (%d,%d)(%d,%d)", rect.left, rect.top, rect.right, rect.bottom));
    hrgn = CreateRectRgnIndirect(&rect);
    if (!hrgn) return 0;
    rc = SendInternalMessageA(WM_NCPAINT,hrgn,0);
    DeleteObject(hrgn);

    return rc;
  }
  else return 0;
}
//******************************************************************************
//Called when either the frame's size or position has changed (lpWndPos != NULL)
//or when the frame layout has changed (i.e. scrollbars added/removed) (lpWndPos == NULL)
//******************************************************************************
ULONG Win32BaseWindow::MsgFormatFrame(WINDOWPOS *lpWndPos)
{
  RECT oldWindowRect = rectWindow, client = rectClient, newWindowRect;
  WINDOWPOS wndPos;
  ULONG rc;

  if(lpWndPos)
  {
    POINT point;

    //set new window rectangle
    point.x = lpWndPos->x;
    point.y = lpWndPos->y;
    if (getParent()) ClientToScreen(getParent()->getWindowHandle(),&point);

    setWindowRect(point.x,point.y,point.x+lpWndPos->cx,point.y+lpWndPos->cy);
    newWindowRect = rectWindow;
  }
  else {
    wndPos.hwnd  = getWindowHandle();
    wndPos.hwndInsertAfter = 0;
    newWindowRect= rectWindow;
    if (getParent()) {
        mapWin32Rect(OSLIB_HWND_DESKTOP,getParent()->getOS2WindowHandle(),&newWindowRect);
    }
    wndPos.x     = newWindowRect.left;
    wndPos.y     = newWindowRect.top;
    wndPos.cx    = newWindowRect.right - newWindowRect.left;
    wndPos.cy    = newWindowRect.bottom - newWindowRect.top;
    wndPos.flags = SWP_FRAMECHANGED;
    lpWndPos     = &wndPos;

    if (getParent()) {
        newWindowRect= rectWindow; //reset; was modified
    }
  }

  rc = SendNCCalcSize(TRUE, &newWindowRect,  &oldWindowRect, &client, lpWndPos, &rectClient);

  dprintf(("MsgFormatFrame: old client rect (%d,%d)(%d,%d), new client (%d,%d)(%d,%d)", client.left, client.top, client.right, client.bottom, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom));
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
//    if(isMDIChild() || IsDialog() || (getParent() == NULL || getParent() == windowDesktop) && ((dwStyle & WS_CAPTION) == WS_CAPTION))
    if((dwStyle & WS_CAPTION) == WS_CAPTION || dwStyle & (WS_VSCROLL|WS_HSCROLL))
        return TRUE;

    return FALSE;
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

//TODO?
#if 0
         /* if COLOR_WINDOW happens to be the same as COLOR_3DHILIGHT
          * we better use 0x55aa bitmap brush to make scrollbar's background
          * look different from the window background.
          */
        if (bk == GetSysColor(COLOR_WINDOW)) {
             return CACHE_GetPattern55AABrush();
        }
#endif
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
       !IsWindowVisible() )
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
        return wndNameLength;

    case WM_GETTEXT:
        if (!lParam || !wParam) return 0;
        if (!windowNameA) ((LPSTR)lParam)[0] = 0;
        else lstrcpynA((LPSTR)lParam, windowNameA, wParam);
        return min(wndNameLength, wParam);

    case WM_SETTEXT:
    {
        LPCSTR lpsz = (LPCSTR)lParam;

        if(windowNameA) free(windowNameA);
        if(windowNameW) free(windowNameW);
        if (lParam)
        {
          wndNameLength = strlen(lpsz);
          windowNameA = (LPSTR)_smalloc(wndNameLength+1);
          strcpy(windowNameA, lpsz);
          windowNameW = (LPWSTR)_smalloc((wndNameLength+1)*sizeof(WCHAR));
          lstrcpyAtoW(windowNameW, windowNameA);
        }
        else
        {
          windowNameA = NULL;
          windowNameW = NULL;
          wndNameLength = 0;
        }
        dprintf(("WM_SETTEXT of %x to %s\n", Win32Hwnd, lParam));
        if ((dwStyle & WS_CAPTION) == WS_CAPTION)
        {
          HandleNCPaint((HRGN)1);
          OSLibWinSetWindowText(OS2HwndFrame,(LPSTR)windowNameA);
        }

        return TRUE;
    }

    case WM_SETREDRAW:
    {
      if (wParam)
      {
        setStyle(getStyle() | WS_VISIBLE);
        OSLibWinEnableWindowUpdate(OS2HwndFrame,TRUE);
      }
      else
      {
        if (getStyle() & WS_VISIBLE)
        {
          setStyle(getStyle() & ~WS_VISIBLE);
          OSLibWinEnableWindowUpdate(OS2HwndFrame,FALSE);
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
      return 0;

    case WM_SETCURSOR:
    {
        dprintf(("DefWndProc: WM_SETCURSOR for %x Msg %s", Win32Hwnd, GetMsgText(HIWORD(lParam))));
        if((getStyle() & WS_CHILD) && !(getExStyle() & WS_EX_NOPARENTNOTIFY))
        {
            if(getParent()) {
                LRESULT rc = getParent()->SendInternalMessageA(WM_SETCURSOR, wParam, lParam);
                if(rc)  return rc;
            }
        }
        if (wParam == Win32Hwnd)
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
          if( (getStyle() & WS_MINIMIZE) && getWindowClass()->getIcon())
          {
            int x = (rectWindow.right - rectWindow.left - GetSystemMetrics(SM_CXICON))/2;
            int y = (rectWindow.bottom - rectWindow.top - GetSystemMetrics(SM_CYICON))/2;
            dprintf(("Painting class icon: vis rect=(%i,%i - %i,%i)\n", ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom ));
            DrawIcon(hdc, x, y, getWindowClass()->getIcon() );
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

      point.x = (SHORT)LOWORD(lParam);
      point.y = (SHORT)HIWORD(lParam);

      return HandleNCHitTest(point);
    }

    case WM_SYSCOMMAND:
    {
     POINT point;

        point.x = LOWORD(lParam);
        point.y = HIWORD(lParam);
        return HandleSysCommand(wParam,&point);
    }

    case WM_SYSKEYDOWN:
        if(wParam == VK_F4) /* try to close the window */
        {
            Win32BaseWindow *window = GetTopParent();
            if(window && !(window->getClass()->getStyle() & CS_NOCLOSE))
                PostMessageA(getWindowHandle(), WM_SYSCOMMAND, SC_CLOSE, 0);
        }

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
        }

        return 0;

    case WM_SYSCHAR:
    {
            int iMenuSysKey = 0;
            if (wParam == VK_RETURN && (getStyle() & WS_MINIMIZE))
        {
                PostMessageA(getWindowHandle(), WM_SYSCOMMAND,
                         (WPARAM)SC_RESTORE, 0L );
                break;
        }
            if ((HIWORD(lParam) & KEYDATA_ALT) && wParam)
        {
                if (wParam == VK_TAB || wParam == VK_ESCAPE) break;
                if (wParam == VK_SPACE && (getStyle() & WS_CHILD))
                getParent()->SendMessageA(Msg, wParam, lParam );
                else
                SendMessageA(WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)(DWORD)wParam );
        }
            else /* check for Ctrl-Esc */
            if (wParam != VK_ESCAPE) MessageBeep(0);
            break;
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
        ShowWindow(wParam ? SW_SHOWNOACTIVATE : SW_HIDE);
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
            HICON hIcon = windowClass->getCursor();
            UINT len;

            if(hIcon) return (LRESULT)hIcon;
            for(len = 1; len < 64; len++)
            {
              hIcon = LoadIconA(hInstance,MAKEINTRESOURCEA(len));
              if(hIcon)
                return (LRESULT)hIcon;
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
            if (!windowClass) return result;
            int index = GCL_HICON;

            if (wParam == ICON_SMALL)
                index = GCL_HICONSM;

            result = windowClass->getClassLongA(index);

            if (Msg == WM_SETICON)
                windowClass->setClassLongA(index, lParam);

            return result;
    }

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
        return wndNameLength;

    case WM_GETTEXT:
        if (!lParam || !wParam) return 0;
        if (!windowNameW) ((LPWSTR)lParam)[0] = 0;
        else lstrcpynW((LPWSTR)lParam,windowNameW,wParam);
        return min(wndNameLength,wParam);

    case WM_SETTEXT:
    {
        LPWSTR lpsz = (LPWSTR)lParam;

        if(windowNameA) free(windowNameA);
        if(windowNameW) free(windowNameW);
        if (lParam)
        {
          wndNameLength = lstrlenW(lpsz);
          windowNameA = (LPSTR)_smalloc(wndNameLength+1);
          lstrcpyWtoA(windowNameA,lpsz);
          windowNameW = (LPWSTR)_smalloc((wndNameLength+1)*sizeof(WCHAR));
          lstrcpyW(windowNameW,lpsz);
        }
        else
        {
          windowNameA = NULL;
          windowNameW = NULL;
          wndNameLength = 0;
        }
        dprintf(("WM_SETTEXT of %x\n",Win32Hwnd));
        if ((dwStyle & WS_CAPTION) == WS_CAPTION)
        {
          HandleNCPaint((HRGN)1);
          OSLibWinSetWindowText(OS2HwndFrame,(LPSTR)windowNameA);
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
 BOOL    fInternalMsgBackup = fInternalMsg;

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
                  mapWin32Point(OS2Hwnd,getParent()->getOS2WindowHandle(),(OSLIBPOINT*)&point);
                  NotifyParent(Msg,wParam,MAKELPARAM(point.x,point.y));
                }
                rc = win32wndproc(getWindowHandle(), Msg, wParam, lParam);
                break;
        }

        case WM_DESTROY:
                rc = win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
                break;

        default:
                rc = CallWindowProcA(win32wndproc, getWindowHandle(), Msg, wParam, lParam);
                break;
  }
  fInternalMsg = fInternalMsgBackup;
  return rc;
}
//******************************************************************************
//Called as a result of an OS/2 message or called from a class method
//******************************************************************************
LRESULT Win32BaseWindow::SendInternalMessageW(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
 LRESULT rc;
 BOOL    fInternalMsgBackup = fInternalMsg;

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

        case WM_DESTROY:
                rc = win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
                break;
        default:
                rc = CallWindowProcW(win32wndproc, getWindowHandle(), Msg, wParam, lParam);
                break;
  }
  fInternalMsg = fInternalMsgBackup;
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

    dprintf(("BroadCastMessageA %x %x %x", msg, wParam, lParam, GetFS()));

    for(int i=0;i<MAX_WINDOW_HANDLES;i++) {
        window = GetWindowFromHandle(hwnd++);
        if(window) {
            if ((window->getStyle() & WS_POPUP) || ((window->getStyle() & WS_CAPTION) == WS_CAPTION))
            {

                if(type == BROADCAST_SEND) {
                        window->SendInternalMessageA(msg, wParam, lParam);
                }
                else    PostMessageA(window->getWindowHandle(), msg, wParam, lParam);
            }
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

    dprintf(("BroadCastMessageW %x %x %x", msg, wParam, lParam));

    for(int i=0;i<MAX_WINDOW_HANDLES;i++) {
        window = GetWindowFromHandle(hwnd++);
        if(window) {
            if ((window->getStyle() & WS_POPUP) || ((window->getStyle() & WS_CAPTION) == WS_CAPTION))
            {

                if(type == BROADCAST_SEND) {
                        window->SendInternalMessageW(msg, wParam, lParam);
                }
                else    PostMessageW(window->getWindowHandle(), msg, wParam, lParam);
            }
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
//******************************************************************************
BOOL Win32BaseWindow::SetIcon(HICON hIcon)
{
    dprintf(("Win32BaseWindow::SetIcon %x", hIcon));
    if(OSLibWinSetIcon(OS2HwndFrame, hIcon) == TRUE) {
//TODO: Wine does't send these. Correct?
//        SendInternalMessageA(WM_SETICON, ICON_BIG, hIcon);
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::ShowWindow(ULONG nCmdShow)
{
 ULONG showstate = 0;
 HWND hWinAfter;

    dprintf(("ShowWindow %x %x", getWindowHandle(), nCmdShow));

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
    switch(nCmdShow)
    {
    case SW_SHOW:
    case SW_SHOWDEFAULT: //todo
        showstate = SWPOS_SHOW | SWPOS_ACTIVATE;
        break;
    case SW_HIDE:
        showstate = SWPOS_HIDE;
        break;
    case SW_RESTORE:
        showstate = SWPOS_RESTORE | SWPOS_SHOW | SWPOS_ACTIVATE;
        break;
    case SW_MINIMIZE:
        showstate = SWPOS_MINIMIZE;
        break;
    case SW_SHOWMAXIMIZED:
        showstate = SWPOS_MAXIMIZE | SWPOS_SHOW | SWPOS_ACTIVATE;
        break;
    case SW_SHOWMINIMIZED:
        showstate = SWPOS_MINIMIZE | SWPOS_SHOW | SWPOS_ACTIVATE;
        break;
    case SW_SHOWMINNOACTIVE:
        showstate = SWPOS_MINIMIZE | SWPOS_SHOW;
        break;
    case SW_SHOWNA:
        showstate = SWPOS_SHOW;
        break;
    case SW_SHOWNOACTIVATE:
        showstate = SWPOS_SHOW;
        break;
    case SW_SHOWNORMAL:
        showstate = SWPOS_RESTORE | SWPOS_ACTIVATE | SWPOS_SHOW;
        break;
    }

    /* We can't activate a child window (WINE) */
    if(getStyle() & WS_CHILD)
        showstate &= ~SWPOS_ACTIVATE;

    if(showstate & SWPOS_SHOW) {
            setStyle(getStyle() | WS_VISIBLE);
    }
    else    setStyle(getStyle() & ~WS_VISIBLE);

    BOOL rc = OSLibWinShowWindow(OS2HwndFrame, showstate);

    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags)
{
   BOOL rc = FALSE;
   Win32BaseWindow *window;
   HWND hParent = 0;
   BOOL fShow = FALSE, fHide = FALSE;

    if (fuFlags &
       ~(SWP_NOSIZE     | SWP_NOMOVE     | SWP_NOZORDER     |
         SWP_NOREDRAW   | SWP_NOACTIVATE | SWP_FRAMECHANGED |
         SWP_SHOWWINDOW | SWP_HIDEWINDOW | SWP_NOCOPYBITS   |
         SWP_NOOWNERZORDER))
    {
        return FALSE;
    }

    if(IsWindowDestroyed()) {
        //changing the position of a window that's being destroyed can cause crashes in PMMERGE
        dprintf(("SetWindowPos; window already destroyed"));
        return TRUE;
    }
    WINDOWPOS wpos;
    SWP swp, swpOld;

#if 0 //CB: test: MSIE 2.0 displays the tool-/addressbar this way -> to check
      //CB: -> comctl32: toolbar (WM_SIZE), same bug: statusbar height
  //CB: cx or cy are 0

if (cx == 0)
{
  dprintf(("CB: cx is 0! %d",fuFlags));
  if (fuFlags & SWP_NOSIZE) dprintf(("CB: nosize"));
  cx = 50;
}
if (cy == 0)
{
  dprintf(("CB: cy is 0! %d",fuFlags));
  if (fuFlags & SWP_NOSIZE) dprintf(("CB: nosize"));
  cy = 50;
}

#endif

    if(fuFlags & SWP_SHOWWINDOW) {
        fShow = TRUE;
        fuFlags &= ~SWP_SHOWWINDOW;
    }
    else
    if(fuFlags & SWP_HIDEWINDOW) {
        fHide = TRUE;
        fuFlags &= ~SWP_HIDEWINDOW;
    }
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
            Win32BaseWindow *windowParent = getParent();
            if(windowParent) {
                hParent = getParent()->getOS2WindowHandle();
        }
        else    dprintf(("WARNING: Win32BaseWindow::SetWindowPos window %x is child but has no parent!!", getWindowHandle()));
       }
       OSLibWinQueryWindowPos(OS2HwndFrame, &swpOld);
    }

    OSLibMapWINDOWPOStoSWP(&wpos, &swp, &swpOld, hParent, OS2HwndFrame);
    if (swp.fl == 0) {
        if (fuFlags & SWP_FRAMECHANGED)
        {
            FrameUpdateClient(this);
        }
        return TRUE;
    }

//   if ((swp.fl & SWPOS_ZORDER) && (swp.hwndInsertBehind > HWNDOS_BOTTOM))
    if ((swp.hwndInsertBehind > HWNDOS_BOTTOM))
    {
        Win32BaseWindow *wndBehind = Win32BaseWindow::GetWindowFromHandle(swp.hwndInsertBehind);
        if(wndBehind) {
            swp.hwndInsertBehind   = wndBehind->getOS2FrameWindowHandle();
        }
        else {
            dprintf(("ERROR: SetWindowPos: hwndInsertBehind %x invalid!",swp.hwndInsertBehind));
            swp.hwndInsertBehind = 0;
        }
    }
    swp.hwnd = OS2HwndFrame;

    dprintf (("WinSetWindowPos %x %x (%d,%d)(%d,%d) %x", swp.hwnd, swp.hwndInsertBehind, swp.x, swp.y, swp.cx, swp.cy, swp.fl));

    //SvL: For some reason WinSetMultWindowPos doesn't work for showing windows when they are hidden..
    if(fHide) {
        ShowWindow(SW_HIDE);
    }
    rc = OSLibWinSetMultWindowPos(&swp, 1);
    if(fShow) {
        ShowWindow(SW_SHOWNA);
    }

    if (rc == FALSE)
    {
        dprintf(("OSLibWinSetMultWindowPos failed! Error %x",OSLibWinGetLastError()));
    }

    if(fuFlags & SWP_FRAMECHANGED && (fuFlags & (SWP_NOMOVE | SWP_NOSIZE) == (SWP_NOMOVE | SWP_NOSIZE)))
    {
        FrameUpdateClient(this);
    }

    return (rc);
}
//******************************************************************************
//TODO: WPF_RESTOREMAXIMIZED
//******************************************************************************
BOOL Win32BaseWindow::SetWindowPlacement(WINDOWPLACEMENT *winpos)
{
   if(isFrameWindow())
   {
      // Set the minimized position
      if (winpos->flags & WPF_SETMINPOSITION)
      {
         OSLibSetWindowMinPos(OS2HwndFrame, winpos->ptMinPosition.x, winpos->ptMinPosition.y);
      }

      //TODO: Max position

      // Set the new restore position.
      OSLibSetWindowRestoreRect(OS2HwndFrame, &winpos->rcNormalPosition);
   }

   return ShowWindow(winpos->showCmd);
}
//******************************************************************************
//Also destroys all the child windows (destroy children first, parent last)
//******************************************************************************
BOOL Win32BaseWindow::DestroyWindow()
{
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
        if(getParent())
        {
             /* Notify the parent window only */
             getParent()->SendMessageA(WM_PARENTNOTIFY, MAKEWPARAM(WM_DESTROY, getWindowId()), (LPARAM)getWindowHandle());
             if( !::IsWindow(getWindowHandle()) )
             {
                return TRUE;
             }
        }
        else DebugInt3();
    }
    fDestroyWindowCalled = TRUE;
    return OSLibWinDestroyWindow(OS2HwndFrame);
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::getParent()
{
  Win32BaseWindow *wndparent = (Win32BaseWindow *)ChildWindow::GetParent();
  return ((ULONG)wndparent == (ULONG)windowDesktop) ? NULL : wndparent;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::GetParent()
{
 Win32BaseWindow *wndparent;

    if ((!(getStyle() & (WS_POPUP|WS_CHILD))))
    {
        return 0;
    }
    wndparent = ((getStyle() & WS_CHILD) ? getParent() : getOwner());

    return (wndparent) ? wndparent->getWindowHandle() : 0;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::SetParent(HWND hwndNewParent)
{
 HWND oldhwnd;
 Win32BaseWindow *newparent;

   if(getParent()) {
        oldhwnd = getParent()->getWindowHandle();
        getParent()->RemoveChild(this);
   }
   else oldhwnd = 0;

   newparent = GetWindowFromHandle(hwndNewParent);
   if(newparent)
   {
        setParent(newparent);
        getParent()->AddChild(this);
        OSLibWinSetParent(getOS2FrameWindowHandle(), getParent()->getOS2WindowHandle());
        return oldhwnd;
   }
   else {
        setParent(windowDesktop);
        windowDesktop->AddChild(this);
        OSLibWinSetParent(getOS2FrameWindowHandle(), OSLIB_HWND_DESKTOP);
        return oldhwnd;
   }
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsChild(HWND hwndParent)
{
    if(getParent()) {
          return getParent()->getWindowHandle() == hwndParent;
    }
    else  return 0;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::GetTopWindow()
{
    return GetWindow(GW_CHILD);
}
//******************************************************************************
// Get the top-level parent for a child window.
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetTopParent()
{
 Win32BaseWindow *window = this;

    while(window && (window->getStyle() & WS_CHILD))
    {
        window = window->getParent();
    }
    return window;
}
//******************************************************************************
//Don't call WinUpdateWindow as that one also updates the child windows
//Also need to send WM_PAINT directly to the window procedure, which doesn't
//always happen with WinUpdateWindow (could be posted if thread doesn't own window)
//******************************************************************************
BOOL Win32BaseWindow::UpdateWindow()
{
 RECT rect;

    if(OSLibWinQueryUpdateRect(OS2Hwnd, &rect))
    {//update region not empty
        HDC hdc;

        hdc = O32_GetDC(OS2Hwnd);
        if (isIcon)
        {
            SendInternalMessageA(WM_ICONERASEBKGND, (WPARAM)hdc, 0);
            SendInternalMessageA(WM_PAINTICON, 0, 0);
        }
        else
        {
            SendInternalMessageA(WM_ERASEBKGND, (WPARAM)hdc, 0);
            SendInternalMessageA(WM_PAINT, 0, 0);
        }
        O32_ReleaseDC(OS2Hwnd, hdc);
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsIconic()
{
    return OSLibWinIsIconic(OS2Hwnd);
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
    for (child = (Win32BaseWindow *)getFirstChild(); child; child = (Win32BaseWindow *)child->getNextChild())
    {
        dprintf(("EnumChildWindows: enumerating child %x", child->getWindowHandle()));
        hwnd = child->getWindowHandle();
        if(child->getOwner()) {
                continue; //shouldn't have an owner (Wine)
        }
        if(lpfn(hwnd, lParam) == FALSE)
        {
                rc = FALSE;
                break;
        }
        //check if the window still exists
        if(!::IsWindow(hwnd))
        {
            child = prevchild;
            continue;
        }
        if(child->getFirstChild() != NULL)
        {
            dprintf(("EnumChildWindows: Enumerate children of %x", child->getWindowHandle()));
            if(child->EnumChildWindows(lpfn, lParam) == FALSE)
            {
                rc = FALSE;
                break;
            }
        }
        prevchild = child;
    }
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
Win32BaseWindow *Win32BaseWindow::FindWindowById(int id)
{
    for (Win32BaseWindow *child = (Win32BaseWindow *)getFirstChild(); child; child = (Win32BaseWindow *)child->getNextChild())
    {
        if (child->getWindowId() == id)
        {
            return child;
        }
    }
    return 0;
}
//******************************************************************************
//TODO:
//We assume (for now) that if hwndParent or hwndChildAfter are real window handles, that
//the current process owns them.
//******************************************************************************
HWND Win32BaseWindow::FindWindowEx(HWND hwndParent, HWND hwndChildAfter, LPSTR lpszClass, LPSTR lpszWindow,
                                   BOOL fUnicode)
{
 Win32BaseWindow *parent = GetWindowFromHandle(hwndParent);
 Win32BaseWindow *child  = GetWindowFromHandle(hwndChildAfter);

    if((hwndParent != OSLIB_HWND_DESKTOP && !parent) ||
       (hwndChildAfter != 0 && !child) ||
       (hwndParent == OSLIB_HWND_DESKTOP && hwndChildAfter != 0))
    {
        dprintf(("Win32BaseWindow::FindWindowEx: parent or child not found %x %x", hwndParent, hwndChildAfter));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    if(hwndParent != OSLIB_HWND_DESKTOP)
    {//if the current process owns the window, just do a quick search
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
            if(child->getWindowClass()->hasClassName(lpszClass, fUnicode) &&
               (!lpszWindow || child->hasWindowName(lpszWindow, fUnicode)))
            {
                dprintf(("FindWindowEx: Found window %x", child->getWindowHandle()));
                return child->getWindowHandle();
            }
            child = (Win32BaseWindow *)child->getNextChild();
        }
    }
    else {
        Win32BaseWindow *wnd;
        HWND henum, hwnd;

        henum = OSLibWinBeginEnumWindows(OSLIB_HWND_DESKTOP);
        hwnd = OSLibWinGetNextWindow(henum);

        while(hwnd)
        {
            wnd = GetWindowFromOS2Handle(hwnd);
            if(wnd == NULL) {
                hwnd = OSLibWinQueryClientWindow(hwnd);
                if(hwnd)  wnd = GetWindowFromOS2Handle(hwnd);
                if(!hwnd) wnd = GetWindowFromOS2FrameHandle(hwnd);
            }

            if(wnd) {
                if(wnd->getWindowClass()->hasClassName(lpszClass, fUnicode) &&
                   (!lpszWindow || wnd->hasWindowName(lpszWindow, fUnicode)))
                {
                    OSLibWinEndEnumWindows(henum);
                    dprintf(("FindWindowEx: Found window %x", wnd->getWindowHandle()));
                    return wnd->getWindowHandle();
                }
            }
            hwnd = OSLibWinGetNextWindow(henum);
        }
        OSLibWinEndEnumWindows(henum);
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
        if(getParent()) {
            window = (Win32BaseWindow *)getParent()->getFirstChild();
            hwndRelated = window->getWindowHandle();
        }
        break;

    case GW_HWNDLAST:
        if(!getParent())
        {
            goto end;
        }

        window = this;
        while(window->getNextChild())
        {
            window = (Win32BaseWindow *)window->getNextChild();
        }
        hwndRelated = window->getWindowHandle();
        break;

    case GW_HWNDNEXT:
        window = (Win32BaseWindow *)getNextChild();
        if(window) {
            hwndRelated = window->getWindowHandle();
        }
        break;

    case GW_HWNDPREV:
        if(!getParent())
        {
            goto end;
        }
        window = (Win32BaseWindow *)(getParent()->getFirstChild()); /* First sibling */
        if(window == this)
        {
            hwndRelated = 0;  /* First in list */
            goto end;
        }
        while(window->getNextChild())
        {
            if (window->getNextChild() == this)
            {
                hwndRelated = window->getWindowHandle();
                goto end;
            }
            window = (Win32BaseWindow *)window->getNextChild();
        }
        break;

    case GW_OWNER:
        if(getOwner()) {
            hwndRelated = getOwner()->getWindowHandle();
        }
        break;

    case GW_CHILD:
        if(getFirstChild()) {
            hwndRelated = ((Win32BaseWindow *)getFirstChild())->getWindowHandle();
        }
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
 Win32BaseWindow  *win32wnd;
 ULONG         magic;

    dprintf(("SetActiveWindow %x", getWindowHandle()));
    hwndActive = OSLibWinSetActiveWindow(OS2HwndFrame);
    win32wnd = (Win32BaseWindow *)OSLibWinGetWindowULong(hwndActive, OFFSET_WIN32WNDPTR);
    magic    = OSLibWinGetWindowULong(hwndActive, OFFSET_WIN32PM_MAGIC);
    if(CheckMagicDword(magic) && win32wnd)
    {
        return win32wnd->getWindowHandle();
    }
    return windowDesktop->getWindowHandle(); //pretend the desktop was active
}
//******************************************************************************
//WM_ENABLE is sent to hwnd, but not to it's children (as it should be)
//******************************************************************************
BOOL Win32BaseWindow::EnableWindow(BOOL fEnable)
{
  return OSLibWinEnableWindow(OS2HwndFrame, fEnable);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::CloseWindow()
{
  return OSLibWinMinimizeWindow(OS2HwndFrame);
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::GetActiveWindow()
{
 HWND          hwndActive;
 Win32BaseWindow  *win32wnd;
 ULONG         magic;

  hwndActive = OSLibWinQueryActiveWindow();

  return OS2ToWin32Handle(hwndActive);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsWindowEnabled()
{
    return OSLibWinIsWindowEnabled(OS2HwndFrame);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsWindowVisible()
{
#if 1
    return (dwStyle & WS_VISIBLE) == WS_VISIBLE;
#else
    return OSLibWinIsWindowVisible(OS2HwndFrame);
#endif
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::hasWindowName(LPSTR wndname, BOOL fUnicode)
{
    INT len = GetWindowTextLength();
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
    } else
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
    INT len = GetWindowTextLength();
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
    INT len = GetWindowTextLength();
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
//******************************************************************************
int Win32BaseWindow::GetWindowTextLength()
{
    return SendInternalMessageA(WM_GETTEXTLENGTH,0,0);
}
//******************************************************************************
//******************************************************************************
int Win32BaseWindow::GetWindowTextA(LPSTR lpsz, int cch)
{
    return SendInternalMessageA(WM_GETTEXT,(WPARAM)cch,(LPARAM)lpsz);
}
//******************************************************************************
//******************************************************************************
int Win32BaseWindow::GetWindowTextW(LPWSTR lpsz, int cch)
{
    return SendInternalMessageW(WM_GETTEXT,(WPARAM)cch,(LPARAM)lpsz);
}
//******************************************************************************
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

    dprintf2(("SetWindowLong%c %x %d %x", (fUnicode) ? 'W' : 'A', getWindowHandle(), index, value));
    switch(index) {
        case GWL_EXSTYLE:
        {
           STYLESTRUCT ss;

                if(dwExStyle == value)
                    return value;

                ss.styleOld = dwExStyle;
                ss.styleNew = value;
                dprintf(("SetWindowLong GWL_EXSTYLE %x old %x new style %x", getWindowHandle(), dwExStyle, value));
                SendInternalMessageA(WM_STYLECHANGING,GWL_EXSTYLE,(LPARAM)&ss);
                setExStyle(ss.styleNew);
                SendInternalMessageA(WM_STYLECHANGED,GWL_EXSTYLE,(LPARAM)&ss);
                return ss.styleOld;
        }
        case GWL_STYLE:
        {
           STYLESTRUCT ss;

                if(dwStyle == value)
                    return value;

                value &= ~(WS_VISIBLE | WS_CHILD);      /* Some bits can't be changed this way (WINE) */
                ss.styleOld = getStyle();
                ss.styleNew = value | (ss.styleOld & (WS_VISIBLE | WS_CHILD));
                dprintf(("SetWindowLong GWL_STYLE %x old %x new style %x", getWindowHandle(), ss.styleOld, ss.styleNew));
                SendInternalMessageA(WM_STYLECHANGING,GWL_STYLE,(LPARAM)&ss);
                setStyle(ss.styleNew);
                SendInternalMessageA(WM_STYLECHANGED,GWL_STYLE,(LPARAM)&ss);
////                OSLibSetWindowStyle(getOS2FrameWindowHandle(), getStyle(), getExStyle(),
////                                    windowClass->getStyle() & CS_SAVEBITS);
#ifdef DEBUG
                PrintWindowStyle(ss.styleNew, 0);
#endif
                return ss.styleOld;
        }
        case GWL_WNDPROC:
                oldval = (LONG)WINPROC_GetProc(win32wndproc, (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A);
                WINPROC_SetProc((HWINDOWPROC *)&win32wndproc, (WNDPROC)value, WINPROC_GetProcType(win32wndproc), WIN_PROC_WINDOW);
                return oldval;

        case GWL_HINSTANCE:
                oldval = hInstance;
                hInstance = value;
                return oldval;

        case GWL_HWNDPARENT:
                return SetParent((HWND)value);

        case GWL_ID:
                oldval = getWindowId();
                setWindowId(value);
                return oldval;

        case GWL_USERDATA:
                oldval = userData;
                userData = value;
                return oldval;

        default:
                if(index >= 0 && index/4 < nrUserWindowLong)
                {
                        oldval = userWindowLong[index/4];
                        userWindowLong[index/4] = value;
                        return oldval;
                }
                SetLastError(ERROR_INVALID_PARAMETER);
                return 0;
    }
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
        if(getParent()) {
            value = getParent()->getWindowHandle();
        }
        else value = 0;
        break;
    case GWL_ID:
        value = getWindowId();
        break;
    case GWL_USERDATA:
        value = userData;
        break;
    default:
        if(index >= 0 && index/4 < nrUserWindowLong)
        {
            value = userWindowLong[index/4];
            break;
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    dprintf2(("GetWindowLongA %x %d %x", getWindowHandle(), index, value));
    return value;
}
//******************************************************************************
//******************************************************************************
WORD Win32BaseWindow::SetWindowWord(int index, WORD value)
{
 WORD oldval;

   if(index >= 0 && index/4 < nrUserWindowLong)
   {
        oldval = ((WORD *)userWindowLong)[index/2];
        ((WORD *)userWindowLong)[index/2] = value;
        return oldval;
   }
   SetLastError(ERROR_INVALID_PARAMETER);
   return 0;
}
//******************************************************************************
//******************************************************************************
WORD Win32BaseWindow::GetWindowWord(int index)
{
    if(index >= 0 && index/4 < nrUserWindowLong)
    {
        return ((WORD *)userWindowLong)[index/2];
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
}
//******************************************************************************
//******************************************************************************
void Win32BaseWindow::setWindowId(DWORD id)
{
    dwIDMenu = id;
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetWindowFromHandle(HWND hwnd)
{
 Win32BaseWindow *window;

    if(HwGetWindowHandleData(hwnd, (DWORD *)&window) == TRUE) {
         return window;
    }
//    dprintf2(("Win32BaseWindow::GetWindowFromHandle: not a win32 window %x", hwnd));
    return NULL;
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetWindowFromOS2Handle(HWND hwnd)
{
 Win32BaseWindow *win32wnd;
 DWORD        magic;

  if(hwnd == OSLIB_HWND_DESKTOP)
  {
    return windowDesktop;
  }

  win32wnd = (Win32BaseWindow *)OSLibWinGetWindowULong(hwnd, OFFSET_WIN32WNDPTR);
  magic    = OSLibWinGetWindowULong(hwnd, OFFSET_WIN32PM_MAGIC);

  if(win32wnd && CheckMagicDword(magic)) {
        return win32wnd;
  }
//  dprintf2(("Win32BaseWindow::GetWindowFromOS2Handle: not an Odin os2 window %x", hwnd));
  return 0;
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetWindowFromOS2FrameHandle(HWND hwnd)
{
    return GetWindowFromOS2Handle(OSLibWinWindowFromID(hwnd,OSLIB_FID_CLIENT));
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::Win32ToOS2Handle(HWND hwnd)
{
    Win32BaseWindow *window = GetWindowFromHandle(hwnd);

    if(window) {
            return window->getOS2WindowHandle();
    }
//    dprintf2(("Win32BaseWindow::Win32ToOS2Handle: not a win32 window %x", hwnd));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::Win32ToOS2FrameHandle(HWND hwnd)
{
    Win32BaseWindow *window = GetWindowFromHandle(hwnd);

    if(window) {
            return window->getOS2FrameWindowHandle();
    }
//    dprintf2(("Win32BaseWindow::Win32ToOS2FrameHandle: not a win32 window %x", hwnd));
    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::OS2ToWin32Handle(HWND hwnd)
{
    Win32BaseWindow *window = GetWindowFromOS2Handle(hwnd);

    if(window) {
            return window->getWindowHandle();
    }
    window = GetWindowFromOS2FrameHandle(hwnd);
    if(window) {
            return window->getWindowHandle();
    }
//    dprintf2(("Win32BaseWindow::OS2ToWin32Handle: not a win32 window %x", hwnd));
    return 0;
//    else    return hwnd;    //OS/2 window handle
}
//******************************************************************************
// GetNextDlgTabItem32   (USER32.276)
//******************************************************************************
HWND Win32BaseWindow::getNextDlgTabItem(HWND hwndCtrl, BOOL fPrevious)
{
 Win32BaseWindow *child, *nextchild, *lastchild;
 HWND retvalue;

    if (hwndCtrl)
    {
        child = GetWindowFromHandle(hwndCtrl);
        if (!child)
        {
            retvalue = 0;
            goto END;
        }
        /* Make sure hwndCtrl is a top-level child */
        while ((child->getStyle() & WS_CHILD) && (child->getParent() != this))
        {
            child = child->getParent();
            if(child == NULL) break;
        }

        if (!child || (child->getParent() != this))
        {
            retvalue = 0;
            goto END;
        }
    }
    else
    {
        /* No ctrl specified -> start from the beginning */
        child = (Win32BaseWindow *)getFirstChild();
        if (!child)
        {
            retvalue = 0;
            goto END;
        }

        if (!fPrevious)
        {
            while (child->getNextChild())
            {
                child = (Win32BaseWindow *)child->getNextChild();
            }
        }
    }

    lastchild = child;
    nextchild = (Win32BaseWindow *)child->getNextChild();
    while (TRUE)
    {
        if (!nextchild) nextchild = (Win32BaseWindow *)getFirstChild();

        if (child == nextchild) break;

        if ((nextchild->getStyle() & WS_TABSTOP) && (nextchild->getStyle() & WS_VISIBLE) &&
            !(nextchild->getStyle() & WS_DISABLED))
        {
            lastchild = nextchild;
            if (!fPrevious) break;
        }
        nextchild = (Win32BaseWindow *)nextchild->getNextChild();
    }
    retvalue = lastchild->getWindowHandle();

END:
    return retvalue;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::getNextDlgGroupItem(HWND hwndCtrl, BOOL fPrevious)
{
 Win32BaseWindow *child, *nextchild, *lastchild;
 HWND retvalue;

    if (hwndCtrl)
    {
        child = GetWindowFromHandle(hwndCtrl);
        if (!child)
        {
            retvalue = 0;
            goto END;
        }
        /* Make sure hwndCtrl is a top-level child */
        while ((child->getStyle() & WS_CHILD) && (child->getParent() != this))
        {
            child = child->getParent();
            if(child == NULL) break;
        }

        if (!child || (child->getParent() != this))
        {
            retvalue = 0;
            goto END;
        }
    }
    else
    {
        /* No ctrl specified -> start from the beginning */
        child = (Win32BaseWindow *)getFirstChild();
        if (!child)
        {
            retvalue = 0;
            goto END;
        }

        if (fPrevious)
        {
            while (child->getNextChild())
            {
                child = (Win32BaseWindow *)child->getNextChild();
            }
        }
    }

    lastchild = child;
    nextchild = (Win32BaseWindow *)child->getNextChild();
    while (TRUE)
    {
        if (!nextchild || (nextchild->getStyle() & WS_GROUP))
        {
            /* Wrap-around to the beginning of the group */
            Win32BaseWindow *pWndTemp;

            nextchild = (Win32BaseWindow *)getFirstChild();

            for(pWndTemp = nextchild;pWndTemp;pWndTemp = (Win32BaseWindow *)pWndTemp->getNextChild())
            {
                if (pWndTemp->getStyle() & WS_GROUP)
                    nextchild = pWndTemp;

                if (pWndTemp == child)
                    break;
            }

        }
        if (nextchild == child)
            break;

        if ((nextchild->getStyle() & WS_VISIBLE) && !(nextchild->getStyle() & WS_DISABLED))
        {
            lastchild = nextchild;

            if (!fPrevious)
                break;
        }

        nextchild = (Win32BaseWindow *)nextchild->getNextChild();
    }
    retvalue = lastchild->getWindowHandle();
END:
    return retvalue;
}
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
    dprintf(("Window exStyle: %x %s (FS = %x)", dwExStyle, exstyle, GetFS()));
}
#endif
//******************************************************************************
//******************************************************************************

GenericObject *Win32BaseWindow::windows  = NULL;
