/* $Id: win32wnd.cpp,v 1.23 1999-07-26 09:01:34 sandervl Exp $ */
/*
 * Win32 Window Code for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Parts based on Wine Windows code (windows\win.c)
 *
 * Copyright 1993, 1994 Alexandre Julliard
 *
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
#include <handlemanager.h>
#include <win32wnd.h>
#include <spy.h>
#include "wndmsg.h"
#include "hooks.h"
#include <oslibwin.h>
#include <oslibutil.h>
#include <oslibgdi.h>
#include <oslibres.h>
#include <winres.h>
#include "syscolor.h"

#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_BORDER)))

#define HAS_THICKFRAME(style) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))

#define HAS_BORDER(style, exStyle) \
    ((style & WS_BORDER) || HAS_THICKFRAME(style) || HAS_DLGFRAME(style,exStyle))

//******************************************************************************
//******************************************************************************
Win32Window::Win32Window(DWORD objType) : GenericObject(&windows, objType)
{
  Init();
}
//******************************************************************************
//******************************************************************************
Win32Window::Win32Window(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
                        : GenericObject(&windows, OBJTYPE_WINDOW), ChildWindow()
{
  Init();
  this->isUnicode = isUnicode;
  CreateWindowExA(lpCreateStructA, classAtom);
}
//******************************************************************************
//******************************************************************************
void Win32Window::Init()
{
  isUnicode        = FALSE;
  fCreated         = FALSE;

  memset(windowNameA, 0, MAX_WINDOW_NAMELENGTH);
  memset(windowNameW, 0, MAX_WINDOW_NAMELENGTH*sizeof(WCHAR));
  wndNameLength    = 0;

  userWindowLong   = NULL;;
  nrUserWindowLong = 0;

  magic            = WIN32PM_MAGIC;
  OS2Hwnd          = 0;
  OS2HwndFrame     = 0;
  OS2HwndMenu      = 0;
  Win32Hwnd        = 0;

  if(HMHandleAllocate(&Win32Hwnd, (ULONG)this) != 0)
  {
        dprintf(("Win32Window::Init HMHandleAllocate failed!!"));
        DebugInt3();
  }
  Win32Hwnd       &= 0xFFFF;
  Win32Hwnd       |= 0x68000000;

  posx = posy      = 0;
  width = height   = 0;

  dwExStyle        = 0;
  dwStyle          = 0;
  win32wndproc     = 0;
  hInstance        = 0;
  windowId         = 0xFFFFFFFF;        //default = -1
  userData         = 0;

  hwndLinkAfter    = HWND_BOTTOM;
  flags            = 0;
  isIcon           = FALSE;
  lastHitTestVal   = 0;
  owner            = NULL;
  windowClass      = 0;

  acceltableResource = NULL;
  menuResource       = NULL;
  iconResource       = NULL;
}
//******************************************************************************
//todo get rid of resources (menu, accel, icon etc)
//******************************************************************************
Win32Window::~Win32Window()
{
  OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, 0);
  OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, 0);

  if(Win32Hwnd)
        HMHandleFree(Win32Hwnd & 0xFFFF);
  if(userWindowLong)
        free(userWindowLong);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::isChild()
{
    return (dwStyle & WS_CHILD) != 0;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::CreateWindowExA(CREATESTRUCTA *cs, ATOM classAtom)
{
 char  buffer[256];
 INT   sw = SW_SHOW;
 POINT maxSize, maxPos, minTrack, maxTrack;

    SetLastError(0);

    /* Find the parent window */
    if (cs->hwndParent)
    {
            Win32Window *window = GetWindowFromHandle(cs->hwndParent);
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
        userWindowLong = (ULONG *)malloc(nrUserWindowLong);
        memset(userWindowLong, 0, nrUserWindowLong);
  }

  if ((cs->style & WS_CHILD) && cs->hwndParent)
  {
        SetParent(cs->hwndParent);
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
                dprintf(("HMHandleTranslateToOS2 couldn't find owner window %x!!!", cs->hwndParent));
                return FALSE;
            }
        }
  }

  setWindowProc(windowClass->getWindowProc());
  hInstance = cs->hInstance;
  dwStyle   = cs->style & ~WS_VISIBLE;
  dwExStyle = cs->dwExStyle;

  hwndLinkAfter = ((cs->style & (WS_CHILD|WS_MAXIMIZE)) == WS_CHILD)
                  ? HWND_BOTTOM : HWND_TOP;

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

  OSLibWinConvertStyle(cs->style, cs->dwExStyle, &dwOSWinStyle, &dwOSFrameStyle);

  //TODO: Test
#if 1
  if(cs->style & WS_CHILD) {
        dwOSFrameStyle = 0;
  }
#endif

  OS2Hwnd = OSLibWinCreateWindow((getParent()) ? getParent()->getOS2FrameWindowHandle() : OSLIB_HWND_DESKTOP,
                                 dwOSWinStyle, dwOSFrameStyle, (char *)cs->lpszName,
                                 (owner) ? owner->getOS2FrameWindowHandle() : OSLIB_HWND_DESKTOP,
                                 (hwndLinkAfter == HWND_BOTTOM) ? TRUE : FALSE,
                                 &OS2HwndFrame);

  if(OS2Hwnd == 0) {
        dprintf(("Window creation failed!!"));
        return FALSE;
  }

  if(OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, (ULONG)this) == FALSE) {
        dprintf(("WM_CREATE: WinSetWindowULong %X failed!!", OS2Hwnd));
        return FALSE;
  }
  if(OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, WIN32PM_MAGIC) == FALSE) {
        dprintf(("WM_CREATE: WinSetWindowULong2 %X failed!!", OS2Hwnd));
        return FALSE;
  }
#if 0
  if(OS2Hwnd != OS2HwndFrame) {
    if(OSLibWinSetWindowULong(OS2HwndFrame, OFFSET_WIN32WNDPTR, (ULONG)this) == FALSE) {
            dprintf(("WM_CREATE: WinSetWindowULong %X failed!!", OS2HwndFrame));
            return FALSE;
    }
    if(OSLibWinSetWindowULong(OS2HwndFrame, OFFSET_WIN32PM_MAGIC, WIN32PM_MAGIC) == FALSE) {
            dprintf(("WM_CREATE: WinSetWindowULong2 %X failed!!", OS2HwndFrame));
            return FALSE;
    }
  }
#endif

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
  else  windowId = (UINT)cs->hMenu;

  //Set icon from class
  if(windowClass->getIcon())
        SetIcon(windowClass->getIcon());

  if(getParent()) {
        SetWindowPos(getParent()->getWindowHandle(), rectClient.left, rectClient.top,
                     rectClient.right-rectClient.left,
                     rectClient.bottom-rectClient.top,
                     SWP_NOACTIVATE);
  }
  else {
        SetWindowPos(HWND_TOP, rectClient.left, rectClient.top,
                     rectClient.right-rectClient.left,
                     rectClient.bottom-rectClient.top,
                     SWP_NOACTIVATE);
  }
  //Get the client window rectangle
  GetClientRect(Win32Hwnd, &rectClient);

  /* Send the WM_CREATE message
   * Perhaps we shouldn't allow width/height changes as well.
   * See p327 in "Internals".
   */
  maxPos.x = rectWindow.left; maxPos.y = rectWindow.top;

  if(SendInternalMessage(WM_NCCREATE, 0, (LPARAM)cs) )
  {
  //doesn't work right, messes up client rectangle
#if 0
        SendNCCalcSize(FALSE, &rectWindow, NULL, NULL, 0, &rectClient );
#endif
        OffsetRect(&rectWindow, maxPos.x - rectWindow.left, maxPos.y - rectWindow.top);
        dprintf(("Sending WM_CREATE"));
        if( (SendInternalMessage(WM_CREATE, 0, (LPARAM)cs )) != -1 )
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
            fCreated = TRUE;
            SetLastError(0);
            return TRUE;
        }
  }
  OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32WNDPTR, 0);
  OSLibWinSetWindowULong(OS2Hwnd, OFFSET_WIN32PM_MAGIC, 0);
  DestroyWindow();
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
UINT Win32Window::MinMaximize(UINT16 cmd, LPRECT16 lpRect )
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
        if( !SendInternalMessageA(WM_QUERYOPEN, 0, 0L ) )
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
                CONV_POINT16TO32( &lpPos->ptMaxPos, &pt );
                WINPOS_GetMinMaxInfo( wndPtr, &size, &pt, NULL, NULL );
                CONV_POINT32TO16( &pt, &lpPos->ptMaxPos );

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
void Win32Window::GetMinMaxInfo(POINT *maxSize, POINT *maxPos,
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

    SendInternalMessageA(WM_GETMINMAXINFO, 0, (LPARAM)&MinMax );

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
LONG Win32Window::SendNCCalcSize(BOOL calcValidRect, RECT *newWindowRect, RECT *oldWindowRect,
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
   result = SendInternalMessageA(WM_NCCALCSIZE, calcValidRect,
                                 (LPARAM)&params );
   *newClientRect = params.rgrc[0];
   return result;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgCreate(HWND hwndOS2, ULONG initParam)
{
  OS2Hwnd = hwndOS2;
  return SendInternalMessageA(WM_CREATE, 0, initParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgQuit()
{
  return SendInternalMessageA(WM_QUIT, 0, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgClose()
{
  if(SendInternalMessageA(WM_CLOSE, 0, 0) == 0) {
        return 0; //app handles this message
  }
  delete this;
  return 1;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgDestroy()
{
 ULONG rc;

  rc = SendInternalMessageA(WM_DESTROY, 0, 0);
  delete this;
  return rc;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgEnable(BOOL fEnable)
{
    return SendInternalMessageA(WM_ENABLE, fEnable, 0);
}
//******************************************************************************
//TODO: SW_PARENTCLOSING/OPENING flag (lParam)
//******************************************************************************
ULONG Win32Window::MsgShow(BOOL fShow)
{
    return SendInternalMessageA(WM_SHOWWINDOW, fShow, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgMove(ULONG x, ULONG y)
{
    dprintf(("MsgMove to (%d,%d)", x, y));
    if(fCreated == FALSE) {
        return 1;
    }

    return SendInternalMessageA(WM_MOVE, 0, MAKELONG((USHORT)x, (USHORT)y));
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgCommand(ULONG cmd, ULONG Id, HWND hwnd)
{
  switch(cmd) {
    case CMD_MENU:
        return SendInternalMessageA(WM_COMMAND, MAKELONG(Id, 0), 0);
    case CMD_CONTROL:
        return 0; //todo
    case CMD_ACCELERATOR:
        dprintf(("accelerator command"));
        return 0; //todo
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgHitTest(ULONG x, ULONG y)
{
  lastHitTestVal = SendInternalMessageA(WM_NCHITTEST, 0, MAKELONG((USHORT)x, (USHORT)y));
  return 1; //TODO: May need to change this
}
//******************************************************************************
//TODO: Send WM_NCCALCSIZE message here and correct size if necessary
//******************************************************************************
ULONG Win32Window::MsgSize(ULONG width, ULONG height, BOOL fMinimize, BOOL fMaximize)
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
ULONG Win32Window::MsgActivate(BOOL fActivate, HWND hwnd)
{
    return SendInternalMessageA(WM_ACTIVATE, (fActivate) ? WA_ACTIVE : WA_INACTIVE, hwnd);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgSysCommand(ULONG win32sc, ULONG x, ULONG y)
{
    return SendInternalMessageA(WM_SYSCOMMAND, win32sc, MAKELONG((USHORT)x, (USHORT)y));
}
//******************************************************************************
//TODO: virtual key & (possibly) scancode translation, extended keyboard bit & Unicode
//******************************************************************************
ULONG Win32Window::MsgChar(ULONG cmd, ULONG repeatcnt, ULONG scancode, ULONG vkey, ULONG keyflags)
{
 ULONG lParam = 0;

    lParam  = repeatcnt;
    lParam |= (scancode << 16);
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
ULONG Win32Window::MsgSetFocus(HWND hwnd)
{
    return SendInternalMessageA(WM_SETFOCUS, hwnd, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgKillFocus(HWND hwnd)
{
    return SendInternalMessageA(WM_KILLFOCUS, hwnd, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgButton(ULONG msg, ULONG x, ULONG y)
{
 ULONG win32msg;
 ULONG win32ncmsg;

    dprintf(("MsgButton to (%d,%d)", x, y));
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
                win32msg = WM_LBUTTONDBLCLK;
                win32ncmsg = WM_NCLBUTTONDBLCLK;
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
                win32msg = WM_RBUTTONDBLCLK;
                win32ncmsg = WM_NCRBUTTONDBLCLK;
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
                win32msg = WM_MBUTTONDBLCLK;
                win32ncmsg = WM_NCMBUTTONDBLCLK;
                break;
        default:
                dprintf(("Win32Window::Button: invalid msg!!!!"));
                return 1;
    }
    SendInternalMessageA(win32ncmsg, lastHitTestVal, MAKELONG(x, y)); //TODO:
    return SendInternalMessageA(win32msg, 0, MAKELONG(x, y));
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgMouseMove(ULONG keystate, ULONG x, ULONG y)
{
 ULONG winstate = 0;

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

    return SendInternalMessageA(WM_MOUSEMOVE, keystate, MAKELONG(x, y));
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgPaint(ULONG tmp1, ULONG tmp2)
{
    return SendInternalMessageA(WM_PAINT, 0, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgEraseBackGround(ULONG hps)
{
    if(isIcon) {
            return SendInternalMessageA(WM_ICONERASEBKGND, hps, 0);
    }
    else    return SendInternalMessageA(WM_ERASEBKGND, hps, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgSetText(LPSTR lpsz, LONG cch)
{
    if(isUnicode) {
            return SendInternalMessageW(WM_SETTEXT, 0, (LPARAM)lpsz);
    }
    else    return SendInternalMessageA(WM_SETTEXT, 0, (LPARAM)lpsz);
}
//******************************************************************************
//TODO: in- or excluding terminating 0?
//******************************************************************************
ULONG Win32Window::MsgGetTextLength()
{
    return SendInternalMessageA(WM_GETTEXTLENGTH, 0, 0);
}
//******************************************************************************
//******************************************************************************
char *Win32Window::MsgGetText()
{
    if(isUnicode) {
        SendInternalMessageW(WM_GETTEXT, MAX_WINDOW_NAMELENGTH, (LPARAM)windowNameW);
    }
    else {
        SendInternalMessageA(WM_GETTEXT, MAX_WINDOW_NAMELENGTH, (LPARAM)windowNameA);
    }
    return windowNameA;
}
//******************************************************************************
//******************************************************************************
LRESULT Win32Window::DefWindowProcA(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_GETTEXTLENGTH:
        return wndNameLength;

    case WM_GETTEXT:   //TODO: SS_ICON controls
        strncpy((LPSTR)lParam, windowNameA, wParam);
        return min(wndNameLength, wParam);

    case WM_SETTEXT:
        return 0;

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
         SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
         SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
         return GetSysColorBrush(COLOR_BTNFACE);

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
        return 0;

    case WM_ERASEBKGND:
    case WM_ICONERASEBKGND:
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

    case WM_NCHITTEST: //TODO:
        return 0;

    default:
        return 1;
    }
}
//******************************************************************************
//******************************************************************************
LRESULT Win32Window::DefWindowProcW(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_GETTEXTLENGTH:
        return wndNameLength;

    case WM_GETTEXT:   //TODO: SS_ICON controls
        lstrcpynW((LPWSTR)lParam, windowNameW, wParam);
        return min(wndNameLength, wParam);

    default:
        return DefWindowProcA(Msg, wParam, lParam);
    }
}
//******************************************************************************
//******************************************************************************
LRESULT Win32Window::SendMessageA(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
  if(PostSpyMessage(getWindowHandle(), Msg, wParam, lParam) == FALSE)
        dprintf(("SendMessageA %s for %x %x %x", GetMsgText(Msg), getWindowHandle(), wParam, lParam));

  if(HkCBT::OS2HkCBTProc(getWindowHandle(), Msg, wParam, lParam) == TRUE) {//hook swallowed msg
        return(0);
  }
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(win32wndproc(getWindowHandle(), WM_NCCREATE, 0, lParam) == 0) {
                        dprintf(("WM_NCCREATE returned FALSE\n"));
                        return(-1); //don't create window
                }
                if(win32wndproc(getWindowHandle(), WM_CREATE, 0, lParam) == -1) {
                        dprintf(("WM_CREATE returned -1\n"));
                        return(-1); //don't create window
                }
                NotifyParent(Msg, wParam, lParam);

                return(0);
        }
        case WM_SETTEXT: //TODO: Nothing happens if passed to DefWindowProc
                return win32wndproc(getWindowHandle(), WM_SETTEXT, wParam, lParam);

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
                NotifyParent(Msg, wParam, lParam);
                return win32wndproc(getWindowHandle(), Msg, wParam, lParam);

        case WM_DESTROY:
                win32wndproc(getWindowHandle(), WM_NCDESTROY, 0, 0);
                NotifyParent(Msg, wParam, lParam);
                return win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
        default:
                return win32wndproc(getWindowHandle(), Msg, wParam, lParam);
  }
}
//******************************************************************************
//******************************************************************************
LRESULT Win32Window::SendMessageW(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
  if(PostSpyMessage(getWindowHandle(), Msg, wParam, lParam) == FALSE)
        dprintf(("SendMessageA %s for %x %x %x", GetMsgText(Msg), getWindowHandle(), wParam, lParam));

  if(HkCBT::OS2HkCBTProc(getWindowHandle(), Msg, wParam, lParam) == TRUE) {//hook swallowed msg
        return(0);
  }
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(win32wndproc(getWindowHandle(), WM_NCCREATE, 0, lParam) == 0) {
                        dprintf(("WM_NCCREATE returned FALSE\n"));
                        return(0); //don't create window
                }
                if(win32wndproc(getWindowHandle(), WM_CREATE, 0, lParam) == 0) {
                        dprintf(("WM_CREATE returned FALSE\n"));
                        return(0); //don't create window
                }
                NotifyParent(Msg, wParam, lParam);

                return(1);
        }
        case WM_SETTEXT: //TODO: Nothing happens if passed to DefWindowProc
                return win32wndproc(getWindowHandle(), WM_SETTEXT, wParam, lParam);

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
                NotifyParent(Msg, wParam, lParam);
                return win32wndproc(getWindowHandle(), Msg, wParam, lParam);

        case WM_DESTROY:
                win32wndproc(getWindowHandle(), WM_NCDESTROY, 0, 0);
                NotifyParent(Msg, wParam, lParam);
                return win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);

        default:
                return win32wndproc(getWindowHandle(), Msg, wParam, lParam);
  }
}
//******************************************************************************
//Called as a result of an OS/2 message
//******************************************************************************
LRESULT Win32Window::SendInternalMessageA(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
  if(PostSpyMessage(getWindowHandle(), Msg, wParam, lParam) == FALSE)
        dprintf(("SendInternalMessageA %s for %x %x %x", GetMsgText(Msg), getWindowHandle(), wParam, lParam));

  if(HkCBT::OS2HkCBTProc(getWindowHandle(), Msg, wParam, lParam) == TRUE) {//hook swallowed msg
        return(0);
  }
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(win32wndproc(getWindowHandle(), WM_NCCREATE, 0, lParam) == 0) {
                        dprintf(("WM_NCCREATE returned FALSE\n"));
                        return(0); //don't create window
                }
                if(win32wndproc(getWindowHandle(), WM_CREATE, 0, lParam) == 0) {
                        dprintf(("WM_CREATE returned FALSE\n"));
                        return(0); //don't create window
                }
                NotifyParent(Msg, wParam, lParam);

                return(1);
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
                NotifyParent(Msg, wParam, lParam);
                return win32wndproc(getWindowHandle(), Msg, wParam, lParam);

        case WM_DESTROY:
                win32wndproc(getWindowHandle(), WM_NCDESTROY, 0, 0);
                NotifyParent(Msg, wParam, lParam);
                return win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
        default:
                return win32wndproc(getWindowHandle(), Msg, wParam, lParam);
  }
}
//******************************************************************************
//Called as a result of an OS/2 message
//todo, unicode msgs (WM_SETTEXT etc)
//******************************************************************************
LRESULT Win32Window::SendInternalMessageW(ULONG Msg, WPARAM wParam, LPARAM lParam)
{
  if(PostSpyMessage(getWindowHandle(), Msg, wParam, lParam) == FALSE)
        dprintf(("SendInternalMessageW %s for %x %x %x", GetMsgText(Msg), getWindowHandle(), wParam, lParam));

  if(HkCBT::OS2HkCBTProc(getWindowHandle(), Msg, wParam, lParam) == TRUE) {//hook swallowed msg
        return(0);
  }
  switch(Msg)
  {
        case WM_CREATE:
        {
                if(win32wndproc(getWindowHandle(), WM_NCCREATE, 0, lParam) == 0) {
                        dprintf(("WM_NCCREATE returned FALSE\n"));
                        return(0); //don't create window
                }
                if(win32wndproc(getWindowHandle(), WM_CREATE, 0, lParam) == 0) {
                        dprintf(("WM_CREATE returned FALSE\n"));
                        return(0); //don't create window
                }
                NotifyParent(Msg, wParam, lParam);

                return(1);
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
                NotifyParent(Msg, wParam, lParam);
                return win32wndproc(getWindowHandle(), Msg, wParam, lParam);

        case WM_DESTROY:
                win32wndproc(getWindowHandle(), WM_NCDESTROY, 0, 0);
                NotifyParent(Msg, wParam, lParam);
                return win32wndproc(getWindowHandle(), WM_DESTROY, 0, 0);
        default:
                return win32wndproc(getWindowHandle(), Msg, wParam, lParam);
  }
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::PostMessageA(ULONG msg, WPARAM wParam, LPARAM lParam)
{
 POSTMSG_PACKET *postmsg;

  postmsg = (POSTMSG_PACKET *)malloc(sizeof(POSTMSG_PACKET));
  if(postmsg == NULL) {
    dprintf(("Win32Window::PostMessageA: malloc returned NULL!!"));
    return 0;
  }
  postmsg->Msg    = msg;
  postmsg->wParam = wParam;
  postmsg->lParam = lParam;
  return OSLibPostMessage(OS2Hwnd, WM_WIN32_POSTMESSAGEA, (ULONG)postmsg, 0);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::PostMessageW(ULONG msg, WPARAM wParam, LPARAM lParam)
{
 POSTMSG_PACKET *postmsg;

  postmsg = (POSTMSG_PACKET *)malloc(sizeof(POSTMSG_PACKET));
  if(postmsg == NULL) {
    dprintf(("Win32Window::PostMessageW: malloc returned NULL!!"));
    return 0;
  }
  postmsg->Msg    = msg;
  postmsg->wParam = wParam;
  postmsg->lParam = lParam;
  return OSLibPostMessage(OS2Hwnd, WM_WIN32_POSTMESSAGEW, (ULONG)postmsg, 0);
}
//******************************************************************************
//TODO: do we need to inform the parent of the parent (etc) of the child window?
//******************************************************************************
void Win32Window::NotifyParent(UINT Msg, WPARAM wParam, LPARAM lParam)
{
 Win32Window *window = this;
 Win32Window *parentwindow;

   while(window)
   {
        if(window->getStyle() & WS_CHILD && !(window->getExStyle() & WS_EX_NOPARENTNOTIFY) )
        {
                /* Notify the parent window only */
                parentwindow = window->getParent();
                if(parentwindow) {
                        if(Msg == WM_CREATE || Msg == WM_DESTROY) {
                                parentwindow->SendInternalMessageA(WM_PARENTNOTIFY, MAKEWPARAM(Msg, window->getWindowId()), (LPARAM)window->getWindowHandle());
                        }
                        else    parentwindow->SendInternalMessageA(WM_PARENTNOTIFY, MAKEWPARAM(Msg, window->getWindowId()), lParam );
                }
        }
        else    break;

        window = parentwindow;
   }
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::SetMenu(HMENU hMenu)
{
 PVOID          menutemplate;
 Win32Resource *winres = (Win32Resource *)hMenu;

    dprintf(("SetMenu %x", hMenu));
    if(HIWORD(winres) == 0) {
        dprintf(("Win32Window:: Win32Resource *winres == 0"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    menutemplate = winres->lockOS2Resource();
    if(menutemplate == NULL)
    {
        dprintf(("Win32Window::SetMenu menutemplate == 0"));
        return FALSE;
    }
    OS2HwndMenu = OSLibWinCreateMenu(OS2HwndFrame, menutemplate);
    if(OS2HwndMenu == 0) {
        dprintf(("Win32Window::SetMenu OS2HwndMenu == 0"));
        return FALSE;
    }
    menuResource = winres;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::SetAccelTable(HACCEL hAccel)
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
BOOL Win32Window::SetIcon(HICON hIcon)
{
 Win32Resource *winres = (Win32Resource *)hIcon;
 HANDLE         iconhandle;

    if(HIWORD(hIcon) == 0) {
        dprintf(("SetIcon: hIcon %x invalid", hIcon));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    dprintf(("Win32Window::SetIcon %x", hIcon));
    iconResource = winres;
    iconhandle = OSLibWinSetIcon(OS2HwndFrame, winres->getOS2Handle(), winres->lockOS2Resource());
    winres->setOS2Handle(iconhandle);
    return(iconhandle != 0);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::ShowWindow(ULONG nCmdShow)
{
 ULONG showstate = 0;

  dprintf(("ShowWindow %x", nCmdShow));
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
  return OSLibWinShowWindow(OS2HwndFrame, showstate);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::SetWindowPos(HWND hwndInsertAfter, int x, int y, int cx, int cy, UINT fuFlags)
{
 Win32Window *window;
 ULONG        setstate = 0;

    switch(hwndInsertAfter) {
    case HWND_BOTTOM:
        hwndInsertAfter = HWNDOS_BOTTOM;
        break;
    case HWND_TOPMOST: //TODO:
    case HWND_NOTOPMOST: //TODO:
    case HWND_TOP:
        hwndInsertAfter = HWNDOS_TOP;
        break;
    default:
        window = GetWindowFromHandle(hwndInsertAfter);
        if(window) {
            hwndInsertAfter = window->getOS2WindowHandle();
            PRECT clientRect = window->getClientRect();

#if 0
            if(x+cx > clientRect->right - clientRect->left) {
                cx = (clientRect->right - clientRect->left) - x;
            }
            if(y+cy > clientRect->bottom - clientRect->top) {
                cy = (clientRect->bottom - clientRect->top) - y;
            }
#endif
            //TODO: Not quite right (Solitaire child window placement slightly wrong)
            if (HAS_DLGFRAME(window->getStyle(), window->getExStyle() ))
            {
                x += GetSystemMetrics(SM_CXDLGFRAME);
                y -= GetSystemMetrics(SM_CYDLGFRAME);
            }
            else
            {
                if (HAS_THICKFRAME(window->getStyle()))
                {
                    x += GetSystemMetrics(SM_CXFRAME);
                    y -= GetSystemMetrics(SM_CYFRAME);
                }
                if (window->getStyle() & WS_BORDER)
                {
                    x += GetSystemMetrics(SM_CXBORDER);
                    y -= GetSystemMetrics(SM_CYBORDER);
                }
            }
        }
        else {
            dprintf(("Win32Window::SetWindowPos, unknown hwndInsertAfter %x", hwndInsertAfter));
            hwndInsertAfter = 0;
        }

        break;

    }
    setstate = SWPOS_MOVE | SWPOS_SIZE | SWPOS_ACTIVATE | SWPOS_ZORDER;
    if(fuFlags & SWP_DRAWFRAME)
        setstate |= 0; //TODO
    if(fuFlags & SWP_FRAMECHANGED)
        setstate |= 0; //TODO
    if(fuFlags & SWP_HIDEWINDOW)
        setstate &= ~SWPOS_ZORDER;
    if(fuFlags & SWP_NOACTIVATE)
        setstate &= ~SWPOS_ACTIVATE;
    if(fuFlags & SWP_NOCOPYBITS)
        setstate |= 0;      //TODO
    if(fuFlags & SWP_NOMOVE)
        setstate &= ~SWPOS_MOVE;
    if(fuFlags & SWP_NOSIZE)
        setstate &= ~SWPOS_SIZE;
    if(fuFlags & SWP_NOREDRAW)
        setstate |= SWPOS_NOREDRAW;
    if(fuFlags & SWP_NOZORDER)
        setstate &= ~SWPOS_ZORDER;
    if(fuFlags & SWP_SHOWWINDOW)
        setstate |= SWPOS_SHOW;

    //TODO send NCCREATE if size changed or SWP_FRAMECHANGED flag specified.
    return OSLibWinSetWindowPos(OS2HwndFrame, hwndInsertAfter, x, y, cx, cy, setstate);
}
//******************************************************************************
//Also destroys all the child windows (destroy parent, destroy children)
//******************************************************************************
BOOL Win32Window::DestroyWindow()
{
  return OSLibWinDestroyWindow(OS2HwndFrame);
}
//******************************************************************************
//******************************************************************************
HWND Win32Window::GetParent()
{
  if(getParent()) {
    return getParent()->getWindowHandle();
  }
  else  return 0;
}
//******************************************************************************
//******************************************************************************
HWND Win32Window::SetParent(HWND hwndNewParent)
{
 HWND oldhwnd;
 Win32Window *newparent;

   if(getParent()) {
        oldhwnd = getParent()->getWindowHandle();
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
        OSLibWinSetParent(getOS2WindowHandle(), getParent()->getOS2WindowHandle());
        return oldhwnd;
   }
   SetLastError(ERROR_INVALID_PARAMETER);
   return 0;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::IsChild(HWND hwndParent)
{
  if(getParent()) {
    return getParent()->getWindowHandle() == hwndParent;
  }
  else  return 0;
}
//******************************************************************************
//******************************************************************************
HWND Win32Window::GetTopWindow()
{
  return GetWindow(GW_CHILD);
}
//******************************************************************************
//Don't call WinUpdateWindow as that one also updates the child windows
//Also need to send WM_PAINT directly to the window procedure, which doesn't
//always happen with WinUpdateWindow (could be posted if thread doesn't own window)
//******************************************************************************
BOOL Win32Window::UpdateWindow()
{
 RECT rect;

    if(OSLibWinQueryUpdateRect(OS2Hwnd, &rect))
    {//update region not empty
        SendInternalMessageA((isIcon) ? WM_PAINTICON : WM_PAINT, 0, 0);
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::IsIconic()
{
    return OSLibWinIsIconic(OS2Hwnd);
}
//******************************************************************************
//TODO: not complete nor correct (distinction between top-level, top-most & child windows)
//******************************************************************************
HWND Win32Window::GetWindow(UINT uCmd)
{
 Win32Window  *win32wnd;
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
        win32wnd = (Win32Window *)OSLibWinGetWindowULong(hwndRelated, OFFSET_WIN32WNDPTR);
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
HWND Win32Window::SetActiveWindow()
{
  return OSLibWinSetActiveWindow(OS2Hwnd);
}
//******************************************************************************
//WM_ENABLE is sent to hwnd, but not to it's children (as it should be)
//******************************************************************************
BOOL Win32Window::EnableWindow(BOOL fEnable)
{
  return OSLibWinEnableWindow(OS2Hwnd, fEnable);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::CloseWindow()
{
  return OSLibWinMinimizeWindow(OS2Hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::BringWindowToTop()
{
  return SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
}
//******************************************************************************
//******************************************************************************
HWND Win32Window::GetActiveWindow()
{
 HWND          hwndActive;
 Win32Window  *win32wnd;
 ULONG         magic;

  hwndActive = OSLibWinQueryActiveWindow();

  win32wnd = (Win32Window *)OSLibWinGetWindowULong(hwndActive, OFFSET_WIN32WNDPTR);
  magic    = OSLibWinGetWindowULong(hwndActive, OFFSET_WIN32PM_MAGIC);
  if(CheckMagicDword(magic) && win32wnd)
  {
    return win32wnd->getWindowHandle();
  }
  return hwndActive;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::IsWindow()
{
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::IsWindowEnabled()
{
    return OSLibWinIsWindowEnabled(OS2Hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::IsWindowVisible()
{
    return OSLibWinIsWindowVisible(OS2Hwnd);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::GetWindowRect(PRECT pRect)
{
    return OSLibWinQueryWindowRect(OS2Hwnd, pRect, RELATIVE_TO_SCREEN);
}
//******************************************************************************
//******************************************************************************
int Win32Window::GetWindowTextLengthA()
{
    return OSLibWinQueryWindowTextLength(OS2Hwnd);
}
//******************************************************************************
//******************************************************************************
int Win32Window::GetWindowTextA(LPSTR lpsz, int cch)
{
    return OSLibWinQueryWindowText(OS2Hwnd, cch, lpsz);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::SetWindowTextA(LPCSTR lpsz)
{
  return OSLibWinSetWindowText(OS2Hwnd, (LPSTR)lpsz);
}
//******************************************************************************
//******************************************************************************
LONG Win32Window::SetWindowLongA(int index, ULONG value)
{
 LONG oldval;

   switch(index) {
        case GWL_EXSTYLE:
                oldval = dwExStyle;
                dwExStyle = value;
                return oldval;
        case GWL_STYLE:
                oldval = dwStyle;
                dwStyle = value;
                return oldval;
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
ULONG Win32Window::GetWindowLongA(int index)
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
WORD Win32Window::SetWindowWord(int index, WORD value)
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
WORD Win32Window::GetWindowWord(int index)
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
Win32Window *Win32Window::GetWindowFromHandle(HWND hwnd)
{
 Win32Window *window;

   if(HIWORD(hwnd) != 0x6800) {
    return NULL;
   }

   if(HMHandleTranslateToOS2(LOWORD(hwnd), (PULONG)&window) == NO_ERROR) {
    return window;
   }
   else return NULL;
}
//******************************************************************************
//******************************************************************************
Win32Window *Win32Window::GetWindowFromOS2Handle(HWND hwnd)
{
 Win32Window *win32wnd;
 DWORD        magic;

  win32wnd = (Win32Window *)OSLibWinGetWindowULong(hwnd, OFFSET_WIN32WNDPTR);
  magic    = OSLibWinGetWindowULong(hwnd, OFFSET_WIN32PM_MAGIC);

  if(win32wnd && CheckMagicDword(magic)) {
    return win32wnd;
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
GenericObject *Win32Window::windows  = NULL;
