/* $Id: win32wbase.cpp,v 1.38 1999-10-12 18:14:56 sandervl Exp $ */
/*
 * Win32 Window Base Class for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 * Parts based on Wine Windows code (windows\win.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
 * TODO: Not thread/process safe
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
#include <spy.h>
#include "wndmsg.h"
#include "hooks.h"
#include "oslibwin.h"
#include "oslibutil.h"
#include "oslibgdi.h"
#include "oslibres.h"
#include "oslibmenu.h"
#include "oslibdos.h"
#include "syscolor.h"
#include "win32wndhandle.h"
#include "heapshared.h"
#include "dc.h"
#include "pmframe.h"
#include "win32wdesktop.h"

#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_BORDER)))

#define HAS_THICKFRAME(style) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))

#define HAS_3DFRAME(exStyle) \
    ((exStyle & WS_EX_CLIENTEDGE) || (exStyle & WS_EX_STATICEDGE) || (exStyle & WS_EX_WINDOWEDGE))

#define HAS_BORDER(style, exStyle) \
    ((style & WS_BORDER) || HAS_THICKFRAME(style) || HAS_DLGFRAME(style,exStyle))

#define IS_OVERLAPPED(style) \
    !(style & (WS_CHILD | WS_POPUP))

/* bits in the dwKeyData */
#define KEYDATA_ALT         0x2000
#define KEYDATA_PREVSTATE   0x4000

void PrintWindowStyle(DWORD dwStyle, DWORD dwExStyle);

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
  fCreated         = FALSE;
  fFirstShow       = TRUE;
  fIsDialog        = FALSE;
  fInternalMsg     = FALSE;

  windowNameA      = NULL;
  windowNameW      = NULL;
  wndNameLength    = 0;

  userWindowLong   = NULL;;
  nrUserWindowLong = 0;

  magic            = WIN32PM_MAGIC;
  OS2Hwnd          = 0;
  OS2HwndFrame     = 0;
  OS2HwndMenu      = 0;
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
  windowId         = 0xFFFFFFFF;        //default = -1
  userData         = 0;

  pOldFrameProc = NULL;
  borderWidth   = 0;
  borderHeight  = 0;

  hwndLinkAfter    = HWND_BOTTOM;
  flags            = 0;
  isIcon           = FALSE;
  lastHitTestVal   = 0;
  owner            = NULL;
  windowClass      = 0;

  acceltableResource = NULL;
  iconResource       = NULL;

  EraseBkgndFlag     = TRUE;
  PSEraseFlag        = FALSE;
  SupressEraseFlag   = FALSE;

  horzScrollInfo     = NULL;
  vertScrollInfo     = NULL;
  hwndHorzScroll     = 0;
  hwndVertScroll     = 0;

  ownDC              = 0;
}
//******************************************************************************
//todo get rid of resources (menu, accel, icon etc)
//******************************************************************************
Win32BaseWindow::~Win32BaseWindow()
{
  OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, 0);
  OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, 0);

  if (isOwnDC())
        releaseOwnDC (ownDC);

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
  //TODO: Destroy windows if they're not associated with our window anymore (showwindow false)?
//  hwndHorzScroll
//  hwndVertScroll

}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::isChild()
{
    return (dwStyle & WS_CHILD) != 0;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::CreateWindowExA(CREATESTRUCTA *cs, ATOM classAtom)
{
 char  buffer[256];
 INT   sw = SW_SHOW;
 POINT maxSize, maxPos, minTrack, maxTrack;

#ifdef DEBUG
    PrintWindowStyle(cs->style, cs->dwExStyle);
#endif

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
  if (cs->x == CW_USEDEFAULT || cs->x == CW_USEDEFAULT16)
  {
//        PDB *pdb = PROCESS_Current();

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
        if (cs->y != CW_USEDEFAULT && cs->y != CW_USEDEFAULT16) sw = cs->y;

        /* We have saved cs->y, now we can trash it */
#if 0
        if (   !(cs->style & (WS_CHILD | WS_POPUP))
            &&  (pdb->env_db->startup_info->dwFlags & STARTF_USEPOSITION) )
        {
            cs->x = pdb->env_db->startup_info->dwX;
            cs->y = pdb->env_db->startup_info->dwY;
        }
#endif
            cs->x = 0;
            cs->y = 0;
//        }
  }
  if (cs->cx == CW_USEDEFAULT || cs->cx == CW_USEDEFAULT16)
  {
#if 0
        PDB *pdb = PROCESS_Current();
        if (   !(cs->style & (WS_CHILD | WS_POPUP))
            &&  (pdb->env_db->startup_info->dwFlags & STARTF_USESIZE) )
        {
            cs->cx = pdb->env_db->startup_info->dwXSize;
            cs->cy = pdb->env_db->startup_info->dwYSize;
        }
        else
        {
#endif
            cs->cx = 600; /* FIXME */
            cs->cy = 400;
//        }
  }

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
  }
  else
  {
        if (!cs->hwndParent) {
            owner = NULL;
        }
        else
        {
            owner = GetWindowFromHandle(cs->hwndParent);
            if(owner == NULL)
            {
                dprintf(("HwGetWindowHandleData couldn't find owner window %x!!!", cs->hwndParent));
                SetLastError(ERROR_INVALID_WINDOW_HANDLE);
                return FALSE;
            }
        }
  }

  setWindowProc(windowClass->getWindowProc());
  hInstance = cs->hInstance;
  dwStyle   = cs->style & ~WS_VISIBLE;
  dwExStyle = cs->dwExStyle;

#if 1
  //SvL: Messes up Z-order of dialog controls
  hwndLinkAfter = HWND_TOP;
#else
  hwndLinkAfter = ((cs->style & (WS_CHILD|WS_MAXIMIZE)) == WS_CHILD)
                  ? HWND_BOTTOM : HWND_TOP;
#endif

#if 0
//TODO
    /* Call the WH_CBT hook */

    if (HOOK_IsHooked( WH_CBT ))
    {
        CBT_CREATEWNDA cbtc;
        LRESULT ret;

        cbtc.lpcs = cs;
        cbtc.hwndInsertAfter = hwndLinkAfter;
        ret = unicode ? HOOK_CallHooksW(WH_CBT, HCBT_CREATEWND, Win32Hwnd, (LPARAM)&cbtc)
                      : HOOK_CallHooksA(WH_CBT, HCBT_CREATEWND, Win32Hwnd, (LPARAM)&cbtc);
        if (ret)
    {
        TRACE_(win)("CBT-hook returned 0\n");
        wndPtr->pDriver->pFinalize(wndPtr);
            retvalue =  0;
            goto end;
    }
    }
#endif

  /* Increment class window counter */
  windowClass->IncreaseWindowCount();

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

  //TODO?
#if 0
  /* Get class or window DC if needed */
  if (classPtr->style & CS_OWNDC) dce = DCE_AllocDCE(hwnd,DCE_WINDOW_DC);
  else if (classPtr->style & CS_CLASSDC) wndPtr->dce = classPtr->dce;
  else wndPtr->dce = NULL;
#endif

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

  /* Send the WM_GETMINMAXINFO message and fix the size if needed */
  if ((cs->style & WS_THICKFRAME) || !(cs->style & (WS_POPUP | WS_CHILD)))
  {
        GetMinMaxInfo(&maxSize, &maxPos, &minTrack, &maxTrack);
        if (maxSize.x < cs->cx) cs->cx = maxSize.x;
        if (maxSize.y < cs->cy) cs->cy = maxSize.y;
        if (cs->cx < minTrack.x ) cs->cx = minTrack.x;
        if (cs->cy < minTrack.y ) cs->cy = minTrack.y;
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

  rectWindow.left   = cs->x;
  rectWindow.top    = cs->y;
  rectWindow.right  = cs->x + cs->cx;
  rectWindow.bottom = cs->y + cs->cy;
  rectClient        = rectWindow;

  DWORD dwOSWinStyle, dwOSFrameStyle;

  OSLibWinConvertStyle(cs->style, &cs->dwExStyle, &dwOSWinStyle, &dwOSFrameStyle, &borderWidth, &borderHeight);
  dwExStyle = cs->dwExStyle;

//CB: dwOSFrameStyle handled by OSLibWinConvertStyle
//    OSLibWinCreateWindow: perhaps problems
//    shouldn't we always use a frame? -> no problems with scrollbars

  if(HIWORD(cs->lpszName))
  {
        if(isUnicode)
                SetWindowTextW((LPWSTR)cs->lpszName);
        else    SetWindowTextA((LPSTR)cs->lpszName);
  }

  OS2Hwnd = OSLibWinCreateWindow((getParent()) ? getParent()->getOS2WindowHandle() : OSLIB_HWND_DESKTOP,
                                 dwOSWinStyle, dwOSFrameStyle, (char *)windowNameA,
                                 (owner) ? owner->getOS2WindowHandle() : OSLIB_HWND_DESKTOP,
                                 (hwndLinkAfter == HWND_BOTTOM) ? TRUE : FALSE,
                                 &OS2HwndFrame);

  if(OS2Hwnd == 0) {
        dprintf(("Window creation failed!!"));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
  }

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
  //SvL: Need to store the shared memory base, or else other apps can map it into their memory space
  if(OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_SHAREDMEM, HeapGetSharedMemBase()) == FALSE) {
        dprintf(("WM_CREATE: WinSetWindowULong2 %X failed!!", OS2Hwnd));
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
        return FALSE;
  }
#if 0
  if(OS2Hwnd != OS2HwndFrame) {
    if(OSLibWinSetWindowULong(OS2HwndFrame, OFFSET_WIN32WNDPTR, (ULONG)this) == FALSE) {
            dprintf(("WM_CREATE: WinSetWindowULong %X failed!!", OS2HwndFrame));
            SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
            return FALSE;
    }
    if(OSLibWinSetWindowULong(OS2HwndFrame, OFFSET_WIN32PM_MAGIC, WIN32PM_MAGIC) == FALSE) {
            dprintf(("WM_CREATE: WinSetWindowULong2 %X failed!!", OS2HwndFrame));
            SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
            return FALSE;
    }
    //SvL: Need to store the shared memory base, or else other apps can map it into their memory space
    if(OSLibWinSetWindowULong(OS2HwndFrame, OFFSET_WIN32PM_SHAREDMEM, HeapGetSharedMemBase()) == FALSE) {
            dprintf(("WM_CREATE: WinSetWindowULong2 %X failed!!", OS2HwndFrame));
            SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
            return FALSE;
    }
  }
#endif

  if (cs->style & WS_HSCROLL)
  {
        hwndHorzScroll = OSLibWinQueryScrollBarHandle(OS2HwndFrame, OSLIB_HSCROLL);
        OSLibWinShowScrollBar(OS2HwndFrame, hwndHorzScroll, OSLIB_HSCROLL, FALSE, TRUE);
  }

  if (cs->style & WS_VSCROLL) {
        hwndVertScroll = OSLibWinQueryScrollBarHandle(OS2HwndFrame, OSLIB_VSCROLL);
        OSLibWinShowScrollBar(OS2HwndFrame, hwndVertScroll, OSLIB_VSCROLL, FALSE, TRUE);
  }

  fakeWinBase.hwndThis     = OS2Hwnd;
  fakeWinBase.pWindowClass = windowClass;
//  SetFakeOpen32();

  /* Set the window menu */
  if ((dwStyle & (WS_CAPTION | WS_CHILD)) == WS_CAPTION )
  {
        if (cs->hMenu) SetMenu(cs->hMenu);
        else
        {
                if (windowClass->getMenuNameA()) {
                        cs->hMenu = LoadMenuA(cs->hInstance, windowClass->getMenuNameA());
                        if (cs->hMenu) SetMenu(cs->hMenu );
                }
        }
  }
  else
  {
        setWindowId((DWORD)cs->hMenu);
  }

  //Set icon from class
  if(windowClass->getIcon())
        SetIcon(windowClass->getIcon());

  if(getParent()) {
        SetWindowPos(getParent()->getWindowHandle(), rectClient.left, rectClient.top,
                     rectClient.right-rectClient.left,
                     rectClient.bottom-rectClient.top,
                     SWP_NOACTIVATE | SWP_NOZORDER );
  }
  else {
        SetWindowPos(HWND_TOP, rectClient.left, rectClient.top,
                     rectClient.right-rectClient.left,
                     rectClient.bottom-rectClient.top,
                     SWP_NOACTIVATE);
  }

  //Subclass frame
  if (isFrameWindow() && HAS_3DFRAME(dwExStyle))
  {
    pOldFrameProc = FrameSubclassFrameWindow(this);
    if (isChild()) FrameSetBorderSize(this,TRUE);
  }

  //Get the client window rectangle
  GetClientRect(Win32Hwnd, &rectClient);

  /* Send the WM_CREATE message
   * Perhaps we shouldn't allow width/height changes as well.
   * See p327 in "Internals".
   */
  maxPos.x = rectWindow.left; maxPos.y = rectWindow.top;

  fCreated = TRUE; //Allow WM_SIZE messages now
  if(SendMessageA(WM_NCCREATE, 0, (LPARAM)cs) )
  {
        //doesn't work right, messes up client rectangle
#if 0
        SendNCCalcSize(FALSE, &rectWindow, NULL, NULL, 0, &rectClient );
#endif
        OffsetRect(&rectWindow, maxPos.x - rectWindow.left, maxPos.y - rectWindow.top);
        dprintf(("Sending WM_CREATE"));
        if( (SendMessageA(WM_CREATE, 0, (LPARAM)cs )) != -1 )
        {
            if(!(flags & WIN_NEED_SIZE)) {
                SendMessageA(WM_SIZE, SIZE_RESTORED,
                                MAKELONG(rectClient.right-rectClient.left,
                                         rectClient.bottom-rectClient.top));
                SendMessageA(WM_MOVE, 0, MAKELONG( rectClient.left, rectClient.top ) );
            }
            if (cs->style & WS_VISIBLE) ShowWindow( sw );

#if 0
            /* Call WH_SHELL hook */

            if (!(dwStyle & WS_CHILD) && !owner)
                HOOK_CallHooks16( WH_SHELL, HSHELL_WINDOWCREATED, hwnd, 0 );
#endif
            SetLastError(0);
            return TRUE;
        }
  }
  dprintf(("Window creation FAILED (NCCREATE cancelled creation)"));
  fCreated = FALSE;
  OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, 0);
  OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, 0);
  DestroyWindow();
  SetLastError(ERROR_OUTOFMEMORY); //TODO: Better error
  return FALSE;
}
#if 0
/***********************************************************************
 *           WINPOS_MinMaximize
 *
 * Fill in lpRect and return additional flags to be used with SetWindowPos().
 * This function assumes that 'cmd' is different from the current window
 * state.
 */
UINT Win32BaseWindow::MinMaximize(UINT cmd, LPRECT lpRect )
{
    UINT swpFlags = 0;
    POINT pt, size;
    LPINTERNALPOS lpPos;

    size.x = rectWindow.left; size.y = rectWindow.top;
    lpPos = WINPOS_InitInternalPos( wndPtr, size, &rectWindow );

    if (lpPos && !HOOK_CallHooks16(WH_CBT, HCBT_MINMAX, hwndSelf, cmd))
    {
    if( dwStyle & WS_MINIMIZE )
    {
        if( !SendMessageA(WM_QUERYOPEN, 0, 0L ) )
        return (SWP_NOSIZE | SWP_NOMOVE);
        swpFlags |= SWP_NOCOPYBITS;
    }
    switch( cmd )
    {
        case SW_MINIMIZE:
         if( dwStyle & WS_MAXIMIZE)
         {
             flags |= WIN_RESTORE_MAX;
             dwStyle &= ~WS_MAXIMIZE;
                 }
                 else
             flags &= ~WIN_RESTORE_MAX;
             dwStyle |= WS_MINIMIZE;

#if 0
         if( flags & WIN_NATIVE )
             if( pDriver->pSetHostAttr( wndPtr, HAK_ICONICSTATE, TRUE ) )
             swpFlags |= MINMAX_NOSWP;
#endif

         lpPos->ptIconPos = WINPOS_FindIconPos( wndPtr, lpPos->ptIconPos );

         SetRect(lpRect, lpPos->ptIconPos.x, lpPos->ptIconPos.y,
             GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON) );
         swpFlags |= SWP_NOCOPYBITS;
         break;

        case SW_MAXIMIZE:
                WINPOS_GetMinMaxInfo( wndPtr, &size, &pt, NULL, NULL );

         if( dwStyle & WS_MINIMIZE )
         {
             if( flags & WIN_NATIVE )
             if( pDriver->pSetHostAttr( wndPtr, HAK_ICONICSTATE, FALSE ) )
                 swpFlags |= MINMAX_NOSWP;

             WINPOS_ShowIconTitle( wndPtr, FALSE );
             dwStyle &= ~WS_MINIMIZE;
         }
                 dwStyle |= WS_MAXIMIZE;

         SetRect16( lpRect, lpPos->ptMaxPos.x, lpPos->ptMaxPos.y,
                    size.x, size.y );
         break;

        case SW_RESTORE:
         if( dwStyle & WS_MINIMIZE )
         {
             if( flags & WIN_NATIVE )
             if( pDriver->pSetHostAttr( wndPtr, HAK_ICONICSTATE, FALSE ) )
                 swpFlags |= MINMAX_NOSWP;

             dwStyle &= ~WS_MINIMIZE;
             WINPOS_ShowIconTitle( wndPtr, FALSE );

             if( flags & WIN_RESTORE_MAX)
             {
             /* Restore to maximized position */
                         CONV_POINT16TO32( &lpPos->ptMaxPos, &pt );
                         WINPOS_GetMinMaxInfo( wndPtr, &size, &pt, NULL, NULL);
                         CONV_POINT32TO16( &pt, &lpPos->ptMaxPos );
             dwStyle |= WS_MAXIMIZE;
             SetRect16( lpRect, lpPos->ptMaxPos.x, lpPos->ptMaxPos.y, size.x, size.y );
             break;
             }
         }
         else
             if( !(dwStyle & WS_MAXIMIZE) ) return (UINT16)(-1);
             else dwStyle &= ~WS_MAXIMIZE;

         /* Restore to normal position */

        *lpRect = lpPos->rectNormal;
         lpRect->right -= lpRect->left;
         lpRect->bottom -= lpRect->top;

         break;
    }
    } else swpFlags |= SWP_NOSIZE | SWP_NOMOVE;
    return swpFlags;
}
#endif
/*******************************************************************
 *           GetMinMaxInfo
 *
 * Get the minimized and maximized information for a window.
 */
void Win32BaseWindow::GetMinMaxInfo(POINT *maxSize, POINT *maxPos,
                                POINT *minTrack, POINT *maxTrack )
{
    MINMAXINFO MinMax;
    INT xinc, yinc;

    /* Compute default values */

    MinMax.ptMaxSize.x = GetSystemMetrics(SM_CXSCREEN);
    MinMax.ptMaxSize.y = GetSystemMetrics(SM_CYSCREEN);
    MinMax.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
    MinMax.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
    MinMax.ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
    MinMax.ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);

    if (flags & WIN_MANAGED) xinc = yinc = 0;
    else if (HAS_DLGFRAME( dwStyle, dwExStyle ))
    {
        xinc = GetSystemMetrics(SM_CXDLGFRAME);
        yinc = GetSystemMetrics(SM_CYDLGFRAME);
    }
    else
    {
        xinc = yinc = 0;
        if (HAS_THICKFRAME(dwStyle))
        {
            xinc += GetSystemMetrics(SM_CXFRAME);
            yinc += GetSystemMetrics(SM_CYFRAME);
        }
        if (dwStyle & WS_BORDER)
        {
            xinc += GetSystemMetrics(SM_CXBORDER);
            yinc += GetSystemMetrics(SM_CYBORDER);
        }
    }
    MinMax.ptMaxSize.x += 2 * xinc;
    MinMax.ptMaxSize.y += 2 * yinc;

#if 0
    lpPos = (LPINTERNALPOS)GetPropA( hwndSelf, atomInternalPos );
    if( lpPos && !EMPTYPOINT(lpPos->ptMaxPos) )
        CONV_POINT16TO32( &lpPos->ptMaxPos, &MinMax.ptMaxPosition );
    else
    {
#endif
        MinMax.ptMaxPosition.x = -xinc;
        MinMax.ptMaxPosition.y = -yinc;
//    }

    SendMessageA(WM_GETMINMAXINFO, 0, (LPARAM)&MinMax );

      /* Some sanity checks */

    dprintf(("GetMinMaxInfo: %ld %ld / %ld %ld / %ld %ld / %ld %ld\n",
                      MinMax.ptMaxSize.x, MinMax.ptMaxSize.y,
                      MinMax.ptMaxPosition.x, MinMax.ptMaxPosition.y,
                      MinMax.ptMaxTrackSize.x, MinMax.ptMaxTrackSize.y,
                      MinMax.ptMinTrackSize.x, MinMax.ptMinTrackSize.y));
    MinMax.ptMaxTrackSize.x = MAX( MinMax.ptMaxTrackSize.x,
                                   MinMax.ptMinTrackSize.x );
    MinMax.ptMaxTrackSize.y = MAX( MinMax.ptMaxTrackSize.y,
                                   MinMax.ptMinTrackSize.y );

    if (maxSize)    *maxSize  = MinMax.ptMaxSize;
    if (maxPos)     *maxPos   = MinMax.ptMaxPosition;
    if (minTrack)   *minTrack = MinMax.ptMinTrackSize;
    if (maxTrack)   *maxTrack = MinMax.ptMaxTrackSize;
}
/***********************************************************************
 *           WINPOS_SendNCCalcSize
 *
 * Send a WM_NCCALCSIZE message to a window.
 * All parameters are read-only except newClientRect.
 * oldWindowRect, oldClientRect and winpos must be non-NULL only
 * when calcValidRect is TRUE.
 */
LONG Win32BaseWindow::SendNCCalcSize(BOOL calcValidRect, RECT *newWindowRect, RECT *oldWindowRect,
                                 RECT *oldClientRect, WINDOWPOS *winpos,
                                 RECT *newClientRect )
{
   NCCALCSIZE_PARAMS params;
   WINDOWPOS winposCopy;
   LONG result;

   params.rgrc[0] = *newWindowRect;
   if (calcValidRect)
   {
        winposCopy = *winpos;
        params.rgrc[1] = *oldWindowRect;
        params.rgrc[2] = *oldClientRect;
        params.lppos = &winposCopy;
   }
   result = SendMessageA(WM_NCCALCSIZE, calcValidRect,
                         (LPARAM)&params );
   *newClientRect = params.rgrc[0];
   return result;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgCreate(HWND hwndOS2, ULONG initParam)
{
  OS2Hwnd = hwndOS2;
  return SendInternalMessageA(WM_CREATE, 0, initParam);
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
  if(SendInternalMessageA(WM_CLOSE, 0, 0) == 0) {
        return 0; //app handles this message
  }
  delete this;
  return 1;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgDestroy()
{
 ULONG rc;

  rc = SendInternalMessageA(WM_DESTROY, 0, 0);
  delete this;
  return rc;
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
    return SendInternalMessageA(WM_SHOWWINDOW, fShow, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgPosChanging(LPARAM lp)
{
    dprintf(("MsgPosChanging"));
#if 1
    if(fCreated == FALSE) {
        return 1;
    }
#endif
    return SendInternalMessageA(WM_WINDOWPOSCHANGING, 0, lp);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgPosChanged(LPARAM lp)
{
    dprintf(("MsgPosChanged"));
#if 1
    if(fCreated == FALSE) {
        return 1;
    }
#endif
    return SendInternalMessageA(WM_WINDOWPOSCHANGED, 0, lp);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgMove(ULONG x, ULONG y)
{
    dprintf(("MsgMove to (%d,%d)", x, y));
    if(fCreated == FALSE) {
        return 1;
    }

    return SendInternalMessageA(WM_MOVE, 0, MAKELONG((USHORT)x, (USHORT)y));
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgTimer(ULONG TimerID)
{
  // TODO: call TIMERPROC if not NULL
  return SendInternalMessageA(WM_TIMER, TimerID, 0);
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
ULONG Win32BaseWindow::MsgCommand(ULONG cmd, ULONG Id, HWND hwnd)
{
  switch(cmd) {
    case CMD_MENU:
        return SendInternalMessageA(WM_COMMAND, MAKELONG(Id, 0), 0);
    case CMD_CONTROL:
        return 0; //todo
    case CMD_ACCELERATOR:
        // this fit not really windows behavior.
        // maybe TranslateAccelerator() is better
        dprintf(("accelerator command"));
        return SendInternalMessageA(WM_COMMAND, MAKELONG(Id, 0), 0);
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgHitTest(ULONG x, ULONG y)
{
  lastHitTestVal = SendInternalMessageA(WM_NCHITTEST, 0, MAKELONG((USHORT)x, (USHORT)y));
  return 1; //TODO: May need to change this
}
//******************************************************************************
//TODO: Send WM_NCCALCSIZE message here and correct size if necessary
//******************************************************************************
ULONG Win32BaseWindow::MsgSize(ULONG width, ULONG height, BOOL fMinimize, BOOL fMaximize)
{
 WORD fwSizeType = 0;

    if(fCreated == FALSE) {//Solitaire crashes if it receives a WM_SIZE during CreateWindowEx (normal or our fault?)
        return 1;
    }

    if(fMinimize) {
            fwSizeType = SIZE_MINIMIZED;
    }
    else
    if(fMaximize) {
            fwSizeType = SIZE_MAXIMIZED;
    }
    else    fwSizeType = SIZE_RESTORED;

    return SendInternalMessageA(WM_SIZE, fwSizeType, MAKELONG((USHORT)width, (USHORT)height));
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgActivate(BOOL fActivate, BOOL fMinimized, HWND hwnd)
{
 ULONG rc, curprocid, procidhwnd = -1, threadidhwnd = 0;

    //According to SDK docs, if app returns FALSE & window is being deactivated,
    //default processing is cancelled
    //TODO: According to Wine we should proceed anyway if window is sysmodal
    if(SendInternalMessageA(WM_NCACTIVATE, fActivate, 0) == FALSE && !fActivate)
    {
        return 0;
    }
    rc = SendInternalMessageA(WM_ACTIVATE, MAKELONG((fActivate) ? WA_ACTIVE : WA_INACTIVE, fMinimized), hwnd);

    curprocid  = GetCurrentProcessId();
    if(hwnd) {
            threadidhwnd = GetWindowThreadProcessId(hwnd, &procidhwnd);
    }

    if(curprocid != procidhwnd && fActivate) {
        SendInternalMessageA(WM_ACTIVATEAPP, 1, threadidhwnd);
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgSysCommand(ULONG win32sc, ULONG x, ULONG y)
{
    return SendInternalMessageA(WM_SYSCOMMAND, win32sc, MAKELONG((USHORT)x, (USHORT)y));
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
//TODO: virtual key & (possibly) scancode translation, extended keyboard bit & Unicode
//******************************************************************************
ULONG Win32BaseWindow::MsgChar(ULONG cmd, ULONG repeatcnt, ULONG scancode, ULONG vkey, ULONG keyflags)
{
 ULONG lParam = 0;

    lParam  = repeatcnt;
    lParam |= (scancode << 16);
    setExtendedKey(vkey, &lParam);

    if(keyflags & KEY_ALTDOWN)
        lParam |= (1<<29);
    if(keyflags & KEY_PREVDOWN)
        lParam |= (1<<30);
    if(keyflags & KEY_UP)
        lParam |= (1<<31);
    if(keyflags & KEY_DEADKEY) {
        dprintf(("WM_DEADCHAR: %x %x %08x", OS2Hwnd, cmd, lParam));
        return SendInternalMessageA(WM_DEADCHAR, cmd, lParam);
    }
    else {
        dprintf(("WM_CHAR: %x %x %08x", OS2Hwnd, cmd, lParam));
        return SendInternalMessageA(WM_CHAR, cmd, lParam);
    }
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgKeyUp (ULONG repeatCount, ULONG scancode, ULONG virtualKey)
{
  ULONG lParam=0;

    lParam = repeatCount & 0x0FFFF;                 // bit 0-15, repeatcount
    lParam |= (scancode & 0x0FF) << 16;             // bit 16-23, scancode
                                                    // bit 24, 1=extended key
                                                    // bit 25-28, reserved
    lParam |= 0 << 29;                              // bit 29, key is released, always 0 for WM_KEYUP ?? <- conflict according to the MS docs
    lParam |= 1 << 30;                              // bit 30, previous state, always 1 for a WM_KEYUP message
    lParam |= 1 << 31;                              // bit 31, transition state, always 1 for WM_KEYUP

    dprintf(("WM_KEYUP: vkey:(%x) param:(%x)", virtualKey, lParam));

    setExtendedKey(virtualKey, &lParam);
    return SendInternalMessageA (WM_KEYUP, virtualKey, lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgKeyDown (ULONG repeatCount, ULONG scancode, ULONG virtualKey, BOOL keyWasPressed)
{
  ULONG lParam=0;

    lParam = repeatCount & 0x0FFFF;                 // bit 0-15, repeatcount
    lParam |= (scancode & 0x0FF) << 16;             // bit 16-23, scancode
                                                    // bit 24, 1=extended key
                                                    // bit 25-28, reserved
                                                    // bit 29, key is pressed, always 0 for WM_KEYDOWN ?? <- conflict according to the MS docs
    if (keyWasPressed)
        lParam |= 1 << 30;                          // bit 30, previous state, 1 means key was pressed
                                                    // bit 31, transition state, always 0 for WM_KEYDOWN

    setExtendedKey(virtualKey, &lParam);

    dprintf(("WM_KEYDOWN: vkey:(%x) param:(%x)", virtualKey, lParam));

    return SendInternalMessageA (WM_KEYDOWN, virtualKey, lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgSysKeyUp (ULONG repeatCount, ULONG scancode, ULONG virtualKey)
{
  ULONG lParam=0;

    lParam = repeatCount & 0x0FFFF;                 // bit 0-15,repeatcount
    lParam |= (scancode & 0x0FF) << 16;             // bit 16-23, scancode
                                                    // bit 24, 1=extended key
                                                    // bit 25-28, reserved
    lParam |= 0 << 29;                              // bit 29, key is released, always 1 for WM_SYSKEYUP ?? <- conflict according to the MS docs
    lParam |= 1 << 30;                              // bit 30, previous state, always 1 for a WM_KEYUP message
    lParam |= 1 << 31;                              // bit 31, transition state, always 1 for WM_KEYUP

    setExtendedKey(virtualKey, &lParam);
    dprintf(("WM_SYSKEYUP: vkey:(%x) param:(%x)", virtualKey, lParam));

    return SendInternalMessageA (WM_SYSKEYUP, virtualKey, lParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgSysKeyDown (ULONG repeatCount, ULONG scancode, ULONG virtualKey, BOOL keyWasPressed)
{
 ULONG lParam=0;

    lParam = repeatCount & 0x0FFFF;                 // bit 0-15, repeatcount
    lParam |= (scancode & 0x0FF) << 16;             // bit 16-23, scancode
                                                    // bit 24, 1=extended key
                                                    // bit 25-28, reserved
                                            // bit 29, key is released, always 1 for WM_SYSKEYUP ?? <- conflict according to the MS docs
    if (keyWasPressed)
        lParam |= 1 << 30;                          // bit 30, previous state, 1 means key was pressed
                                                    // bit 31, transition state, always 0 for WM_KEYDOWN

    setExtendedKey(virtualKey, &lParam);
    dprintf(("WM_SYSKEYDOWN: vkey:(%x) param:(%x)", virtualKey, lParam));

    return SendInternalMessageA (WM_SYSKEYDOWN, virtualKey, lParam);
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
//******************************************************************************
ULONG Win32BaseWindow::MsgButton(ULONG msg, ULONG ncx, ULONG ncy, ULONG clx, ULONG cly)
{
 ULONG win32msg;
 ULONG win32ncmsg;

    dprintf(("MsgButton to (%d,%d)", ncx, ncy));
    switch(msg) {
        case BUTTON_LEFTDOWN:
                win32msg = WM_LBUTTONDOWN;
                win32ncmsg = WM_NCLBUTTONDOWN;
                break;
        case BUTTON_LEFTUP:
                win32msg = WM_LBUTTONUP;
                win32ncmsg = WM_NCLBUTTONUP;
                break;
        case BUTTON_LEFTDBLCLICK:
                if (windowClass->getClassLongA(GCL_STYLE) & CS_DBLCLKS)
                {
                  win32msg = WM_LBUTTONDBLCLK;
                  win32ncmsg = WM_NCLBUTTONDBLCLK;
                } else
                {
                  MsgButton(BUTTON_LEFTDOWN,ncx,ncy,clx,cly);
                  return MsgButton(BUTTON_LEFTUP,ncx,ncy,clx,cly);
                }
                break;
        case BUTTON_RIGHTUP:
                win32msg = WM_RBUTTONUP;
                win32ncmsg = WM_NCRBUTTONUP;
                break;
        case BUTTON_RIGHTDOWN:
                win32msg = WM_RBUTTONDOWN;
                win32ncmsg = WM_NCRBUTTONDOWN;
                break;
        case BUTTON_RIGHTDBLCLICK:
                if (windowClass->getClassLongA(GCL_STYLE) & CS_DBLCLKS)
                {
                  win32msg = WM_RBUTTONDBLCLK;
                  win32ncmsg = WM_NCRBUTTONDBLCLK;
                } else
                {
                  MsgButton(BUTTON_RIGHTDOWN,ncx,ncy,clx,cly);
                  return MsgButton(BUTTON_RIGHTUP,ncx,ncy,clx,cly);
                }
                break;
        case BUTTON_MIDDLEUP:
                win32msg = WM_MBUTTONUP;
                win32ncmsg = WM_NCMBUTTONUP;
                break;
        case BUTTON_MIDDLEDOWN:
                win32msg = WM_MBUTTONDOWN;
                win32ncmsg = WM_NCMBUTTONDOWN;
                break;
        case BUTTON_MIDDLEDBLCLICK:
                if (windowClass->getClassLongA(GCL_STYLE) & CS_DBLCLKS)
                {
                  win32msg = WM_MBUTTONDBLCLK;
                  win32ncmsg = WM_NCMBUTTONDBLCLK;
                } else
                {
                  MsgButton(BUTTON_MIDDLEDOWN,ncx,ncy,clx,cly);
                  return MsgButton(BUTTON_MIDDLEUP,ncx,ncy,clx,cly);
                }
                break;
        default:
                dprintf(("Win32BaseWindow::Button: invalid msg!!!!"));
                return 1;
    }

    SendInternalMessageA(WM_SETCURSOR, Win32Hwnd, MAKELONG(lastHitTestVal, win32ncmsg));

    //WM_NC*BUTTON* is posted when the cursor is in a non-client area of the window
    if(lastHitTestVal != HTCLIENT) {
            return SendInternalMessageA(win32ncmsg, lastHitTestVal, MAKELONG(ncx, ncy)); //TODO:
    }
    return  SendInternalMessageA(win32msg, 0, MAKELONG(clx, cly));
}
//******************************************************************************
//******************************************************************************
ULONG Win32BaseWindow::MsgMouseMove(ULONG keystate, ULONG x, ULONG y)
{
 ULONG winstate = 0;
 ULONG setcursormsg = WM_MOUSEMOVE;

    if(keystate & WMMOVE_LBUTTON)
        winstate |= MK_LBUTTON;
    if(keystate & WMMOVE_RBUTTON)
        winstate |= MK_RBUTTON;
    if(keystate & WMMOVE_MBUTTON)
        winstate |= MK_MBUTTON;
    if(keystate & WMMOVE_SHIFT)
        winstate |= MK_SHIFT;
    if(keystate & WMMOVE_CTRL)
        winstate |= MK_CONTROL;

    if(lastHitTestVal != HTCLIENT) {
        setcursormsg = WM_NCMOUSEMOVE;
    }
    //TODO: hiword should be 0 if window enters menu mode (SDK docs)
    SendInternalMessageA(WM_SETCURSOR, Win32Hwnd, MAKELONG(lastHitTestVal, setcursormsg));

    //WM_NCMOUSEMOVE is posted when the cursor moves into a non-client area of the window
    if(lastHitTestVal != HTCLIENT) {
            SendInternalMessageA(WM_NCMOUSEMOVE, lastHitTestVal, MAKELONG(x, y));
    }
    return  SendInternalMessageA(WM_MOUSEMOVE, keystate, MAKELONG(x, y));
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
ULONG Win32BaseWindow::MsgSetText(LPSTR lpsz, LONG cch)
{
    if(isUnicode) {
            return SendInternalMessageW(WM_SETTEXT, 0, (LPARAM)lpsz);
    }
    else    return SendInternalMessageA(WM_SETTEXT, 0, (LPARAM)lpsz);
}
//******************************************************************************
//TODO: in- or excluding terminating 0?
//******************************************************************************
ULONG Win32BaseWindow::MsgGetTextLength()
{
    return SendInternalMessageA(WM_GETTEXTLENGTH, 0, 0);
}
//******************************************************************************
//******************************************************************************
char *Win32BaseWindow::MsgGetText()
{
    if(isUnicode) {
        SendInternalMessageW(WM_GETTEXT, wndNameLength, (LPARAM)windowNameW);
    }
    else {
        SendInternalMessageA(WM_GETTEXT, wndNameLength, (LPARAM)windowNameA);
    }
    return windowNameA;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::isMDIClient()
{
    return FALSE;
}
//******************************************************************************
//TODO: Not complete (flags)
//******************************************************************************
SCROLLBAR_INFO *Win32BaseWindow::getScrollInfo(int nBar)
{
    switch(nBar) {
    case SB_HORZ:
        if(horzScrollInfo) {
            horzScrollInfo->CurVal = OSLibWinGetScrollPos(OS2HwndFrame, hwndHorzScroll);
            return horzScrollInfo;
        }
        break;
    case SB_VERT:
        if(vertScrollInfo) {
            vertScrollInfo->CurVal = OSLibWinGetScrollPos(OS2HwndFrame, hwndVertScroll);
            return vertScrollInfo;
        }
        break;
    }
    return NULL;
}
//******************************************************************************
//TODO: Not complete
//******************************************************************************
LONG Win32BaseWindow::setScrollInfo(int nBar, SCROLLINFO *info, int fRedraw)
{
  SCROLLBAR_INFO *infoPtr;
  HWND            hwndScroll;
  ULONG           scrollType;
  int             new_flags;

    switch(nBar) {
    case SB_HORZ:
        if(!horzScrollInfo) {
            return 0;
        }
        infoPtr = horzScrollInfo;
        hwndScroll = hwndHorzScroll;
        scrollType = OSLIB_HSCROLL;
        break;
    case SB_VERT:
        if(!vertScrollInfo) {
            return 0;
        }
        infoPtr = vertScrollInfo;
        hwndScroll = hwndVertScroll;
        scrollType = OSLIB_VSCROLL;
        break;
    default:
        return 0;
    }

    if (info->fMask & ~(SIF_ALL | SIF_DISABLENOSCROLL)) return 0;
    if ((info->cbSize != sizeof(*info)) &&
        (info->cbSize != sizeof(*info)-sizeof(info->nTrackPos))) return 0;

    /* Set the page size */
    if (info->fMask & SIF_PAGE)
    {
        if( infoPtr->Page != info->nPage )
        {
            infoPtr->Page = info->nPage;
            OSLibWinSetScrollPageSize(OS2HwndFrame, hwndScroll, info->nPage, infoPtr->MaxVal, fRedraw);
        }
    }

    /* Set the scroll pos */
    if (info->fMask & SIF_POS)
    {
        if( infoPtr->CurVal != info->nPos )
        {
            infoPtr->CurVal = info->nPos;
            OSLibWinSetScrollPos(OS2HwndFrame, hwndScroll, info->nPos, fRedraw);
        }
    }

    /* Set the scroll range */
    if (info->fMask & SIF_RANGE)
    {
        /* Invalid range -> range is set to (0,0) */
        if ((info->nMin > info->nMax) ||
            ((UINT)(info->nMax - info->nMin) >= 0x80000000))
        {
            infoPtr->MinVal = 0;
            infoPtr->MaxVal = 0;
        }
        else
        {
            if( infoPtr->MinVal != info->nMin ||
                infoPtr->MaxVal != info->nMax )
            {
                infoPtr->MinVal = info->nMin;
                infoPtr->MaxVal = info->nMax;

                OSLibWinSetScrollRange(OS2HwndFrame, hwndScroll, info->nMin, info->nMax, fRedraw);
            }
        }
    }

    /* Make sure the page size is valid */
    if (infoPtr->Page < 0) infoPtr->Page = 0;
    else if (infoPtr->Page > infoPtr->MaxVal - infoPtr->MinVal + 1 )
        infoPtr->Page = infoPtr->MaxVal - infoPtr->MinVal + 1;

    /* Make sure the pos is inside the range */
    if (infoPtr->CurVal < infoPtr->MinVal)
        infoPtr->CurVal = infoPtr->MinVal;
    else if (infoPtr->CurVal > infoPtr->MaxVal - MAX( infoPtr->Page-1, 0 ))
        infoPtr->CurVal = infoPtr->MaxVal - MAX( infoPtr->Page-1, 0 );

    /* Check if the scrollbar should be hidden or disabled */
    if (info->fMask & (SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL))
    {
        new_flags = infoPtr->flags;
        if (infoPtr->MinVal >= infoPtr->MaxVal - MAX( infoPtr->Page-1, 0 ))
        {
            /* Hide or disable scroll-bar */
            if (info->fMask & SIF_DISABLENOSCROLL)
            {
                new_flags = ESB_DISABLE_BOTH;
//               *action |= SA_SSI_REFRESH;
            }
            else if (nBar != SB_CTL)
            {
//                *action = SA_SSI_HIDE;
                goto done;
            }
        }
        else  /* Show and enable scroll-bar */
        {
            new_flags = 0;
//            if (nBar != SB_CTL)
//                *action |= SA_SSI_SHOW;
        }

        if (infoPtr->flags != new_flags) /* check arrow flags */
        {
            infoPtr->flags = new_flags;
        }
    }

done:
    /* Return current position */

    return infoPtr->CurVal;
}
/***********************************************************************
 *           NC_HandleSysCommand
 *
 * Handle a WM_SYSCOMMAND message. Called from DefWindowProc().
 *
 * TODO: Not done (see #if 0)
 */
LONG Win32BaseWindow::HandleSysCommand(WPARAM wParam, POINT *pt32)
{
    UINT uCommand = wParam & 0xFFF0;

    if (getStyle() & WS_CHILD && uCommand != SC_KEYMENU )
        ScreenToClient(getParent()->getWindowHandle(), pt32 );

    switch (uCommand)
    {
#if 0
    case SC_SIZE:
    case SC_MOVE:
        NC_DoSizeMove( hwnd, wParam );
        break;
#endif

    case SC_MINIMIZE:
        ShowWindow(SW_MINIMIZE);
        break;

    case SC_MAXIMIZE:
        ShowWindow(SW_MAXIMIZE);
        break;

    case SC_RESTORE:
        ShowWindow(SW_RESTORE);
        break;

    case SC_CLOSE:
        return SendMessageA(WM_CLOSE, 0, 0);

#if 0
    case SC_VSCROLL:
    case SC_HSCROLL:
        NC_TrackScrollBar( hwnd, wParam, pt32 );
        break;

    case SC_MOUSEMENU:
        MENU_TrackMouseMenuBar( wndPtr, wParam & 0x000F, pt32 );
        break;

    case SC_KEYMENU:
        MENU_TrackKbdMenuBar( wndPtr , wParam , pt.x );
        break;

    case SC_TASKLIST:
        WinExec( "taskman.exe", SW_SHOWNORMAL );
        break;

    case SC_SCREENSAVE:
        if (wParam == SC_ABOUTWINE)
            ShellAboutA(hwnd, "Odin", WINE_RELEASE_INFO, 0);
        else
        if (wParam == SC_PUTMARK)
            dprintf(("Mark requested by user\n"));
        break;

    case SC_HOTKEY:
    case SC_ARRANGE:
    case SC_NEXTWINDOW:
    case SC_PREVWINDOW:
        break;
#endif
    }
    return 0;
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
LRESULT Win32BaseWindow::DefWindowProcA(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_CLOSE:
        DestroyWindow();
        return 0;

    case WM_GETTEXTLENGTH:
        return wndNameLength;

    case WM_GETTEXT:   //TODO: SS_ICON controls
        strncpy((LPSTR)lParam, windowNameA, wParam);
        return min(wndNameLength, wParam);

    case WM_SETTEXT:
        if(!fInternalMsg) {
                return SetWindowTextA((LPSTR)lParam);
        }
        else    return 0;

    case WM_SETREDRAW:
        if(wParam)
                SetWindowLongA (GWL_STYLE, GetWindowLongA (GWL_STYLE) | WS_VISIBLE);
        else    SetWindowLongA (GWL_STYLE, GetWindowLongA (GWL_STYLE) & ~WS_VISIBLE);

        return 0; //TODO

    case WM_NCCREATE:
        return(TRUE);

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
        DWORD dwStyle = GetWindowLongA(GWL_STYLE);
        DWORD dwExStyle = GetWindowLongA(GWL_EXSTYLE);
        dprintf(("DefWndProc: WM_MOUSEACTIVATE for %x Msg %s", Win32Hwnd, GetMsgText(HIWORD(lParam))));
        if(dwStyle & WS_CHILD && !(dwExStyle & WS_EX_NOPARENTNOTIFY) )
        {
            if(getParent()) {
                LRESULT rc = getParent()->SendMessageA(WM_MOUSEACTIVATE, wParam, lParam );
                if(rc)  return rc;
            }
        }
        return (LOWORD(lParam) == HTCAPTION) ? MA_NOACTIVATE : MA_ACTIVATE;
    }
    case WM_SETCURSOR:
    {
        DWORD dwStyle = GetWindowLongA(GWL_STYLE);
        DWORD dwExStyle = GetWindowLongA(GWL_EXSTYLE);
        dprintf(("DefWndProc: WM_SETCURSOR for %x Msg %s", Win32Hwnd, GetMsgText(HIWORD(lParam))));
        if(dwStyle & WS_CHILD && !(dwExStyle & WS_EX_NOPARENTNOTIFY) )
        {
            if(getParent()) {
                LRESULT rc = getParent()->SendMessageA(WM_SETCURSOR, wParam, lParam);
                if(rc)  return rc;
            }
        }
        return 1;
    }
    case WM_MOUSEMOVE:
        return 1; //Let OS/2 change the mouse cursor back to the default

    case WM_WINDOWPOSCHANGED:
    {

/* undocumented SWP flags - from SDK 3.1 */
#define SWP_NOCLIENTSIZE        0x0800
#define SWP_NOCLIENTMOVE        0x1000

        PWINDOWPOS wpos = (PWINDOWPOS)lParam;
        WPARAM     wp   = SIZE_RESTORED;

        if (!(wpos->flags & SWP_NOMOVE) && !(wpos->flags & SWP_NOCLIENTMOVE))
            SendMessageA(WM_MOVE, 0, MAKELONG(rectClient.left, rectClient.top));

        if (!(wpos->flags & SWP_NOSIZE) && !(wpos->flags & SWP_NOCLIENTSIZE))
        {
            if (dwStyle & WS_MAXIMIZE) wp = SIZE_MAXIMIZED;
            else if (dwStyle & WS_MINIMIZE) wp = SIZE_MINIMIZED;

            SendMessageA(WM_SIZE, wp, MAKELONG(rectClient.right  - rectClient.left,
                                               rectClient.bottom - rectClient.top));
        }
        return 0;
    }
    case WM_ERASEBKGND:
    case WM_ICONERASEBKGND:
    {
      RECT rect;
      int rc;

        if (!windowClass->getBackgroundBrush()) return 0;

        rc = GetClipBox( (HDC)wParam, &rect );
        if ((rc == SIMPLEREGION) || (rc == COMPLEXREGION))
            FillRect( (HDC)wParam, &rect, windowClass->getBackgroundBrush());

        return 1;
    }
    case WM_GETDLGCODE:
        return 0;

    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONUP:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONUP:
    case WM_NCMBUTTONDBLCLK:
        return 0;           //TODO: Send WM_SYSCOMMAND if required

    case WM_NCHITTEST: //TODO: Calculate position of
        return HTCLIENT;

    case WM_SYSCOMMAND:
    {
     POINT point;

        point.x = LOWORD(lParam);
        point.y = HIWORD(lParam);
        return HandleSysCommand(wParam, &point);
    }

    case WM_SYSKEYDOWN:
        if(HIWORD(lParam) & KEYDATA_ALT)
        {
            if(wParam == VK_F4) /* try to close the window */
            {
                Win32BaseWindow *window = GetTopParent();
                if(window && !(window->getClass()->getStyle() & CS_NOCLOSE) )
                    window->PostMessageA(WM_SYSCOMMAND, SC_CLOSE, 0);
            }
        }
        return 0;

    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
        return 1;

    case WM_NOTIFYFORMAT:
        if (IsUnicode()) return NFR_UNICODE;
        else return NFR_ANSI;

    case WM_SETICON:
    case WM_GETICON:
    {
            LRESULT result = 0;
            int index = GCL_HICON;

            if (wParam == ICON_SMALL)
                index = GCL_HICONSM;

            result = windowClass->getClassLongA(index);

            if (Msg == WM_SETICON)
                windowClass->setClassLongA(index, lParam);

            return result;
    }

    default:
        return 1;
    }
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::DefWindowProcW(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_GETTEXTLENGTH:
        return wndNameLength;

    case WM_GETTEXT:   //TODO: SS_ICON controls
        lstrcpynW((LPWSTR)lParam, windowNameW, wParam);
        return min(wndNameLength, wParam);

    case WM_SETTEXT:
        if(!fInternalMsg) {
                return SetWindowTextW((LPWSTR)lParam);
        }
        else    return 0;

    default:
        return DefWindowProcA(Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::SendMessageA(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
 LRESULT rc;
 BOOL    fInternalMsgBackup = fInternalMsg;

  if(Msg != WM_GETDLGCODE && Msg != WM_ENTERIDLE) {//sent *very* often
        if(PostSpyMessage(getWindowHandle(), Msg, wParam, lParam) == FALSE)
            dprintf(("SendMessageA %s for %x %x %x", GetMsgText(Msg), getWindowHandle(), wParam, lParam));
  }

  if(HkCBT::OS2HkCBTProc(getWindowHandle(), Msg, wParam, lParam) == TRUE) {//hook swallowed msg
        return(0);
  }
  fInternalMsg = FALSE;
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(win32wndproc(getWindowHandle(), WM_CREATE, 0, lParam) == -1) {
                        dprintf(("WM_CREATE returned -1\n"));
                        rc = -1; //don't create window
                        break;
                }
                NotifyParent(Msg, wParam, lParam);

                rc = 0;
                break;
        }
        case WM_SETTEXT:
                rc = win32wndproc(getWindowHandle(), WM_SETTEXT, wParam, lParam);
                break;

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
                rc = win32wndproc(getWindowHandle(), Msg, wParam, lParam);
                break;
  }
  fInternalMsg = fInternalMsgBackup;
  return rc;
}
//******************************************************************************
//******************************************************************************
LRESULT Win32BaseWindow::SendMessageW(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
 LRESULT rc;
 BOOL    fInternalMsgBackup = fInternalMsg;

  if(Msg != WM_GETDLGCODE && Msg != WM_ENTERIDLE) {//sent *very* often
        if(PostSpyMessage(getWindowHandle(), Msg, wParam, lParam) == FALSE)
            dprintf(("SendMessageW %s for %x %x %x", GetMsgText(Msg), getWindowHandle(), wParam, lParam));
  }

  if(HkCBT::OS2HkCBTProc(getWindowHandle(), Msg, wParam, lParam) == TRUE) {//hook swallowed msg
        return(0);
  }
  fInternalMsg = FALSE;
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(win32wndproc(getWindowHandle(), WM_CREATE, 0, lParam) == -1) {
                        dprintf(("WM_CREATE returned -1\n"));
                        rc = -1; //don't create window
                        break;
                }
                NotifyParent(Msg, wParam, lParam);

                rc = 0;
                break;
        }
        case WM_SETTEXT:
                rc = win32wndproc(getWindowHandle(), WM_SETTEXT, wParam, lParam);
                break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
                NotifyParent(Msg, wParam, lParam);
                rc = win32wndproc(getWindowHandle(), Msg, wParam, lParam);
                break;

        case WM_DESTROY:
                win32wndproc(getWindowHandle(), WM_NCDESTROY, 0, 0);
                NotifyParent(Msg, wParam, lParam);
                rc = win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
                break;

        default:
                rc = win32wndproc(getWindowHandle(), Msg, wParam, lParam);
                break;
  }
  fInternalMsg = fInternalMsgBackup;
  return rc;
}
//******************************************************************************
//Called as a result of an OS/2 message
//******************************************************************************
LRESULT Win32BaseWindow::SendInternalMessageA(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
 LRESULT rc;
 BOOL    fInternalMsgBackup = fInternalMsg;

  if(PostSpyMessage(getWindowHandle(), Msg, wParam, lParam) == FALSE)
        dprintf(("SendInternalMessageA %s for %x %x %x", GetMsgText(Msg), getWindowHandle(), wParam, lParam));

  if(HkCBT::OS2HkCBTProc(getWindowHandle(), Msg, wParam, lParam) == TRUE) {//hook swallowed msg
        return(0);
  }
  fInternalMsg = TRUE;
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(win32wndproc(getWindowHandle(), WM_CREATE, 0, lParam) == -1) {
                        dprintf(("WM_CREATE returned -1\n"));
                        rc = -1; //don't create window
                        break;
                }
                NotifyParent(Msg, wParam, lParam);
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
                win32wndproc(getWindowHandle(), WM_NCDESTROY, 0, 0);
                NotifyParent(Msg, wParam, lParam);
                rc = win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
                break;
        default:
                rc = win32wndproc(getWindowHandle(), Msg, wParam, lParam);
                break;
  }
  fInternalMsg = fInternalMsgBackup;
  return rc;
}
//******************************************************************************
//Called as a result of an OS/2 message
//todo, unicode msgs (WM_SETTEXT etc)
//******************************************************************************
LRESULT Win32BaseWindow::SendInternalMessageW(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
 LRESULT rc;
 BOOL    fInternalMsgBackup = fInternalMsg;

  if(PostSpyMessage(getWindowHandle(), Msg, wParam, lParam) == FALSE)
        dprintf(("SendInternalMessageW %s for %x %x %x", GetMsgText(Msg), getWindowHandle(), wParam, lParam));

  if(HkCBT::OS2HkCBTProc(getWindowHandle(), Msg, wParam, lParam) == TRUE) {//hook swallowed msg
        return(0);
  }
  fInternalMsg = TRUE;
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(win32wndproc(getWindowHandle(), WM_CREATE, 0, lParam) == -1) {
                        dprintf(("WM_CREATE returned -1\n"));
                        rc = -1; //don't create window
                        break;
                }
                NotifyParent(Msg, wParam, lParam);
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
                win32wndproc(getWindowHandle(), WM_NCDESTROY, 0, 0);
                NotifyParent(Msg, wParam, lParam);
                rc = win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
                break;
        default:
                rc = win32wndproc(getWindowHandle(), Msg, wParam, lParam);
                break;
  }
  fInternalMsg = fInternalMsgBackup;
  return rc;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::PostMessageA(ULONG msg, WPARAM wParam, LPARAM lParam)
{
  return OSLibPostMessage(OS2Hwnd, WIN32APP_USERMSGBASE+msg, wParam, lParam);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::PostMessageW(ULONG msg, WPARAM wParam, LPARAM lParam)
{
  return OSLibPostMessage(OS2Hwnd, WIN32APP_USERMSGBASE+msg, wParam, lParam);
}
//******************************************************************************
//TODO: do we need to inform the parent of the parent (etc) of the child window?
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
                        if(Msg == WM_CREATE || Msg == WM_DESTROY) {
                                parentwindow->SendMessageA(WM_PARENTNOTIFY, MAKEWPARAM(Msg, window->getWindowId()), (LPARAM)window->getWindowHandle());
                        }
                        else    parentwindow->SendMessageA(WM_PARENTNOTIFY, MAKEWPARAM(Msg, window->getWindowId()), lParam );
                }
        }
        else    break;

        window = parentwindow;
   }
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::getTopParent()
{
 Win32BaseWindow *tmpWnd = this;

    while( tmpWnd && (tmpWnd->getStyle() & WS_CHILD))
    {
        tmpWnd = tmpWnd->getParent();
    }
    return tmpWnd;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::SetMenu(HMENU hMenu)
{

    dprintf(("SetMenu %x", hMenu));
    OS2HwndMenu = OSLibWinSetMenu(OS2HwndFrame, hMenu);
    if(OS2HwndMenu == 0) {
        dprintf(("Win32BaseWindow::SetMenu OS2HwndMenu == 0"));
        return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::SetAccelTable(HACCEL hAccel)
{
 Win32Resource *winres = (Win32Resource *)hAccel;
 HANDLE         accelhandle;

    if(HIWORD(hAccel) == 0) {
        dprintf(("SetAccelTable: hAccel %x invalid", hAccel));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    acceltableResource = winres;
    accelhandle = OSLibWinSetAccelTable(OS2HwndFrame, winres->getOS2Handle(), winres->lockOS2Resource());
    winres->setOS2Handle(accelhandle);
    return(accelhandle != 0);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::SetIcon(HICON hIcon)
{
    dprintf(("Win32BaseWindow::SetIcon %x", hIcon));
    if(OSLibWinSetIcon(OS2HwndFrame, hIcon) == TRUE) {
        SendMessageA(WM_SETICON, hIcon, 0);
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::ShowWindow(ULONG nCmdShow)
{
 ULONG showstate = 0;

    dprintf(("ShowWindow %x %x", getWindowHandle(), nCmdShow));
    if(fFirstShow) {
        if(isFrameWindow() && IS_OVERLAPPED(getStyle()) && !isChild()) {
                SendMessageA(WM_SIZE, SIZE_RESTORED,
                                MAKELONG(rectClient.right-rectClient.left,
                                         rectClient.bottom-rectClient.top));
                SendMessageA(WM_MOVE, 0, MAKELONG( rectClient.left, rectClient.top ) );

        }
        fFirstShow = FALSE;
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

   dprintf (("SetWindowPos %x %x (%d,%d)(%d,%d) %x", Win32Hwnd, hwndInsertAfter, x, y, cx, cy, fuFlags));

   if (fuFlags &
       ~(SWP_NOSIZE     | SWP_NOMOVE     | SWP_NOZORDER     |
         SWP_NOREDRAW   | SWP_NOACTIVATE | SWP_FRAMECHANGED |
         SWP_SHOWWINDOW | SWP_HIDEWINDOW | SWP_NOCOPYBITS   |
         SWP_NOOWNERZORDER))
   {
      return FALSE;
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
           hParent = getParent()->getOS2WindowHandle();
           OSLibWinQueryWindowPos(isFrameWindow() ? OS2HwndFrame:OS2Hwnd, &swpOld);
       }
       else
           OSLibWinQueryWindowPos(OS2HwndFrame, &swpOld);
   }

   OSLibMapWINDOWPOStoSWP(&wpos, &swp, &swpOld, hParent, OS2HwndFrame);
   if (swp.fl == 0)
      return TRUE;

   if ((swp.fl & SWPOS_ZORDER) && (swp.hwndInsertBehind > HWNDOS_BOTTOM))
   {
      Win32BaseWindow *wndBehind = Win32BaseWindow::GetWindowFromHandle(swp.hwndInsertBehind);
      swp.hwndInsertBehind   = wndBehind->getOS2WindowHandle();
   }
   if (isFrameWindow())
   {
      if (!isChild())
      {
        POINT maxSize, maxPos, minTrack, maxTrack;

        GetMinMaxInfo(&maxSize, &maxPos, &minTrack, &maxTrack);

        if (swp.cx > maxTrack.x) swp.cx = maxTrack.x;
        if (swp.cy > maxTrack.y) swp.cy = maxTrack.y;
        if (swp.cx < minTrack.x) swp.cx = minTrack.x;
        if (swp.cy < minTrack.y) swp.cy = minTrack.y;
      }

      swp.hwnd = OS2HwndFrame;
   }
   else
      swp.hwnd = OS2Hwnd;

   dprintf (("WinSetWindowPos %x %x (%d,%d)(%d,%d) %x", swp.hwnd, swp.hwndInsertBehind, swp.x, swp.y, swp.cx, swp.cy, swp.fl));

   rc = OSLibWinSetMultWindowPos(&swp, 1);

   if (rc == FALSE)
   {
        dprintf(("OSLibWinSetMultWindowPos failed!"));
   }
   else
   {
      if (fuFlags & SWP_FRAMECHANGED_W)
         OSLibSendMessage (OS2HwndFrame, 0x42 /*WM_UPDATEFRAME*/, -1, 0);
   }

   return (rc);
}
//******************************************************************************
//Also destroys all the child windows (destroy parent, destroy children)
//******************************************************************************
BOOL Win32BaseWindow::DestroyWindow()
{
  return OSLibWinDestroyWindow(OS2HwndFrame);
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::GetParent()
{
  if(getParent()) {
        return getParent()->getWindowHandle();
  }
  else  return 0;
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

   if(hwndNewParent == 0) {//desktop window = parent
        setParent(NULL);
        OSLibWinSetParent(getOS2WindowHandle(), OSLIB_HWND_DESKTOP);
        return oldhwnd;
   }
   newparent = GetWindowFromHandle(hwndNewParent);
   if(newparent)
   {
        setParent(newparent);
        getParent()->AddChild(this);
        OSLibWinSetParent(getOS2WindowHandle(), getParent()->getOS2WindowHandle());
        return oldhwnd;
   }
   SetLastError(ERROR_INVALID_PARAMETER);
   return 0;
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
            SendMessageA(WM_ICONERASEBKGND, (WPARAM)hdc, 0);
            SendMessageA(WM_PAINTICON, 0, 0);
        } else
        {
            SendMessageA(WM_ERASEBKGND, (WPARAM)hdc, 0);
            SendMessageA(WM_PAINT, 0, 0);
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
            }

            if(wnd) {
                LPVOID sharedmembase = (LPVOID)OSLibWinGetWindowULong(hwnd, OFFSET_WIN32PM_SHAREDMEM);

                if(OSLibDosGetSharedMem(sharedmembase, MAX_HEAPSIZE, OSLIB_PAG_READ) != 0) {
                    dprintf(("OSLibDosGetSharedMem returned error for %x", wnd));
                    break;
                }
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
//TODO: not complete nor correct (distinction be    tween top-level, top-most & child windows)
//******************************************************************************
HWND Win32BaseWindow::GetWindow(UINT uCmd)
{
 Win32BaseWindow  *win32wnd;
 ULONG         magic;
 ULONG         getcmd = 0;
 HWND          hwndRelated;

    dprintf(("GetWindow %x %d NOT COMPLETE", getWindowHandle(), uCmd));
    switch(uCmd)
    {
        case GW_CHILD:
            getcmd = QWOS_TOP;
            break;
        case GW_HWNDFIRST:
            if(getParent()) {
                    getcmd = QWOS_TOP; //top of child windows
            }
            else    getcmd = QWOS_TOP; //TODO
            break;
        case GW_HWNDLAST:
            if(getParent()) {
                    getcmd = QWOS_BOTTOM; //bottom of child windows
            }
            else    getcmd = QWOS_BOTTOM; //TODO
            break;
        case GW_HWNDNEXT:
            getcmd = QWOS_NEXT;
            break;
        case GW_HWNDPREV:
            getcmd = QWOS_PREV;
            break;
        case GW_OWNER:
            if(owner) {
                    return owner->getWindowHandle();
            }
            else    return 0;
    }
    hwndRelated = OSLibWinQueryWindow(OS2Hwnd, getcmd);
    if(hwndRelated)
    {
        win32wnd = (Win32BaseWindow *)OSLibWinGetWindowULong(hwndRelated, OFFSET_WIN32WNDPTR);
        magic    = OSLibWinGetWindowULong(hwndRelated, OFFSET_WIN32PM_MAGIC);
        if(CheckMagicDword(magic) && win32wnd)
        {
            return win32wnd->getWindowHandle();
        }
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::SetActiveWindow()
{
  return OSLibWinSetActiveWindow(OS2Hwnd);
}
//******************************************************************************
//WM_ENABLE is sent to hwnd, but not to it's children (as it should be)
//******************************************************************************
BOOL Win32BaseWindow::EnableWindow(BOOL fEnable)
{
  return OSLibWinEnableWindow(OS2Hwnd, fEnable);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::CloseWindow()
{
  return OSLibWinMinimizeWindow(OS2Hwnd);
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::GetActiveWindow()
{
 HWND          hwndActive;
 Win32BaseWindow  *win32wnd;
 ULONG         magic;

  hwndActive = OSLibWinQueryActiveWindow();

  win32wnd = (Win32BaseWindow *)OSLibWinGetWindowULong(hwndActive, OFFSET_WIN32WNDPTR);
  magic    = OSLibWinGetWindowULong(hwndActive, OFFSET_WIN32PM_MAGIC);
  if(CheckMagicDword(magic) && win32wnd)
  {
        return win32wnd->getWindowHandle();
  }
  return hwndActive;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsWindowEnabled()
{
    return OSLibWinIsWindowEnabled(OS2Hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::IsWindowVisible()
{
    return OSLibWinIsWindowVisible(OS2Hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::GetWindowRect(PRECT pRect)
{
    return OSLibWinQueryWindowRect(OS2Hwnd, pRect, RELATIVE_TO_SCREEN);
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::hasWindowName(LPSTR wndname, BOOL fUnicode)
{
    if(fUnicode) {
            return (lstrcmpW(windowNameW, (LPWSTR)wndname) == 0);
    }
    else    return (strcmp(windowNameA, wndname) == 0);
}
//******************************************************************************
//******************************************************************************
int Win32BaseWindow::GetWindowTextLength()
{
    return wndNameLength;
}
//******************************************************************************
//******************************************************************************
int Win32BaseWindow::GetWindowTextA(LPSTR lpsz, int cch)
{
    strncpy(lpsz, windowNameA, cch);
    return wndNameLength;
}
//******************************************************************************
//******************************************************************************
int Win32BaseWindow::GetWindowTextW(LPWSTR lpsz, int cch)
{
    lstrcpynW((LPWSTR)lpsz, windowNameW, cch);
    return wndNameLength;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::SetWindowTextA(LPSTR lpsz)
{
    if(lpsz == NULL)
        return FALSE;

    if(windowNameA) free(windowNameA);
    if(windowNameW) free(windowNameW);

    windowNameA = (LPSTR)_smalloc(strlen(lpsz)+1);
    strcpy(windowNameA, lpsz);
    windowNameW = (LPWSTR)_smalloc((strlen(lpsz)+1)*sizeof(WCHAR));
    lstrcpyAtoW(windowNameW, windowNameA);
    wndNameLength = strlen(windowNameA)+1; //including 0 terminator

    if(OS2HwndFrame)
        return OSLibWinSetWindowText(OS2HwndFrame, (LPSTR)windowNameA);

    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32BaseWindow::SetWindowTextW(LPWSTR lpsz)
{
    if(lpsz == NULL)
        return FALSE;

    if(windowNameA) free(windowNameA);
    if(windowNameW) free(windowNameW);

    windowNameW = (LPWSTR)_smalloc((lstrlenW((LPWSTR)lpsz)+1)*sizeof(WCHAR));
    lstrcpyW(windowNameW, (LPWSTR)lpsz);
    windowNameA = (LPSTR)_smalloc(lstrlenW((LPWSTR)lpsz)+1);
    lstrcpyWtoA(windowNameA, windowNameW);
    wndNameLength = strlen(windowNameA)+1; //including 0 terminator

    if(OS2HwndFrame)
        return OSLibWinSetWindowText(OS2HwndFrame, (LPSTR)windowNameA);

    return TRUE;
}
//******************************************************************************
//******************************************************************************
LONG Win32BaseWindow::SetWindowLongA(int index, ULONG value)
{
 LONG oldval;

   switch(index) {
        case GWL_EXSTYLE:
        {
           STYLESTRUCT ss;

                ss.styleOld = dwExStyle;
                ss.styleNew = value;
                SendMessageA(WM_STYLECHANGING,GWL_EXSTYLE,(LPARAM)&ss);
                setExStyle(ss.styleNew);
                SendMessageA(WM_STYLECHANGED,GWL_EXSTYLE,(LPARAM)&ss);
                return ss.styleOld;
        }
        case GWL_STYLE:
        {
           STYLESTRUCT ss;

                ss.styleOld = dwStyle;
                ss.styleNew = value;
                SendMessageA(WM_STYLECHANGING,GWL_STYLE,(LPARAM)&ss);
                setStyle(ss.styleNew);
                OSLibSetWindowStyle(OS2HwndFrame, dwStyle);
                SendMessageA(WM_STYLECHANGED,GWL_STYLE,(LPARAM)&ss);
                return ss.styleOld;
        }
        case GWL_WNDPROC:
                oldval = (LONG)getWindowProc();
                setWindowProc((WNDPROC)value);
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
ULONG Win32BaseWindow::GetWindowLongA(int index)
{
   switch(index) {
        case GWL_EXSTYLE:
                return dwExStyle;
        case GWL_STYLE:
                return dwStyle;
        case GWL_WNDPROC:
                return (ULONG)getWindowProc();
        case GWL_HINSTANCE:
                return hInstance;
        case GWL_HWNDPARENT:
                if(getParent()) {
                        return getParent()->getWindowHandle();
                }
                else    return 0;
        case GWL_ID:
                return getWindowId();
        case GWL_USERDATA:
                return userData;
        default:
                if(index >= 0 && index/4 < nrUserWindowLong)
                {
                        return userWindowLong[index/4];
                }
                SetLastError(ERROR_INVALID_PARAMETER);
                return 0;
   }
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
   windowId = id;
   OSLibSetWindowID(OS2HwndFrame, id);
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetWindowFromHandle(HWND hwnd)
{
 Win32BaseWindow *window;

   if(hwnd == NULL && windowDesktop)
    return windowDesktop;

   if(HwGetWindowHandleData(hwnd, (DWORD *)&window) == TRUE) {
        return window;
   }
   else return NULL;
}
//******************************************************************************
//******************************************************************************
Win32BaseWindow *Win32BaseWindow::GetWindowFromOS2Handle(HWND hwnd)
{
 Win32BaseWindow *win32wnd;
 DWORD        magic;

  win32wnd = (Win32BaseWindow *)OSLibWinGetWindowULong(hwnd, OFFSET_WIN32WNDPTR);
  magic    = OSLibWinGetWindowULong(hwnd, OFFSET_WIN32PM_MAGIC);

  if(win32wnd && CheckMagicDword(magic)) {
        return win32wnd;
  }
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
    else    return hwnd;
}
//******************************************************************************
//******************************************************************************
HWND Win32BaseWindow::Win32ToOS2FrameHandle(HWND hwnd)
{
    Win32BaseWindow *window = GetWindowFromHandle(hwnd);

    if(window) {
            return window->getOS2FrameWindowHandle();
    }
    else    return hwnd;
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
    else    return hwnd;    //OS/2 window handle
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
    dprintf(("Window exStyle: %x %s", dwExStyle, exstyle));
}
#endif
//******************************************************************************
//******************************************************************************

GenericObject *Win32BaseWindow::windows  = NULL;
