/* $Id: win32wnd.cpp,v 1.1 1999-07-14 08:35:37 sandervl Exp $ */
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

#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_BORDER)))

#define HAS_THICKFRAME(style) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))

//******************************************************************************
//******************************************************************************
Win32Window::Win32Window(DWORD objType) : GenericObject(&windows, objType)
{
  Init();
}
//******************************************************************************
//******************************************************************************
Win32Window::Win32Window(CREATESTRUCTA *lpCreateStructA, ATOM classAtom, BOOL isUnicode)
			: GenericObject(&windows, OBJTYPE_WINDOW)
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

  *windowName      = NULL;
  wndNameLength    = 0;

  *windowText      = NULL;;
  wndTextLength    = 0;
	
  *userWindowLong  = NULL;;
  nrUserWindowLong = 0;

  magic            = WIN32PM_MAGIC;
  OS2Hwnd          = 0;
  Win32Hwnd        = 0;
  if(HMHandleAllocate(&Win32Hwnd, (ULONG)this) != 0) 
  {
	dprintf(("Win32Window::Init HMHandleAllocate failed!!"));
	DebugInt3();
  }
  posx = posy      = 0;
  width = height   = 0;

  dwExStyle        = 0;
  dwStyle	   = 0;
  win32wndproc     = 0;
  hInstance        = 0;
  parent           = 0;
  windowId         = 0xFFFFFFFF;	//default = -1
  userData         = 0;

  hwndLinkAfter    = HWND_BOTTOM;
  flags            = 0;
  owner            = NULL;
  windowClass      = 0;
}
//******************************************************************************
//******************************************************************************
Win32Window::~Win32Window()
{
  if(Win32Hwnd)
	HMHandleFree(Win32Hwnd);
  if(windowName)
	free(windowName);
  if(windowText)
	free(windowText);
  if(userWindowLong)
	free(userWindowLong);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Window::CreateWindowExA(CREATESTRUCTA *cs, ATOM classAtom)
{
 char  buffer[256];
 DWORD tmp;
 INT   sw = SW_SHOW; 
 POINT maxSize, maxPos, minTrack, maxTrack;

  SetLastError(0);

  /* Find the parent window */
  if (cs->hwndParent)
  {
	/* Make sure parent is valid */
        if (!IsWindow( cs->hwndParent ))
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

  //Allocate window words
  nrUserWindowLong = windowClass->getExtraWndWords();
  if(nrUserWindowLong) {
	userWindowLong = (ULONG *)malloc(nrUserWindowLong);
	memset(userWindowLong, 0, nrUserWindowLong);
  }

  if ((cs->style & WS_CHILD) && cs->hwndParent)
  {
	if(HMHandleTranslateToOS2(cs->hwndParent, &tmp) != NO_ERROR)
    	{
		dprintf(("HMHandleTranslateToOS2 couldn't find parent window %x!!!", cs->hwndParent));
		return FALSE;
    	}
    	parent = (Win32Window *)tmp;
  }
  else
  {
        parent = NULL;	//desktop
        if (!cs->hwndParent) {
            	owner = NULL;
	}
        else
        {
		if(HMHandleTranslateToOS2(cs->hwndParent, &tmp) != NO_ERROR)
    		{
			dprintf(("HMHandleTranslateToOS2 couldn't find owner window %x!!!", cs->hwndParent));
			return FALSE;
    		}
            	owner = (Win32Window *)tmp;
	}
  }

  setWindowProc(windowClass->getWindowProc());
  hInstance = cs->hInstance;
  dwStyle   = cs->style & ~WS_VISIBLE;
  dwExStyle = cs->dwExStyle;

  hwndLinkAfter = ((cs->style & (WS_CHILD|WS_MAXIMIZE)) == WS_CHILD)
                  ? HWND_BOTTOM : HWND_TOP;

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

  /* Set the window menu */
  if ((dwStyle & (WS_CAPTION | WS_CHILD)) == WS_CAPTION )
  {
        if (cs->hMenu) SetMenu(getWindowHandle(), cs->hMenu);
        else
        {
            	if (windowClass->getMenuNameA()) {
	               	cs->hMenu = LoadMenuA(cs->hInstance, windowClass->getMenuNameA());
        	        if (cs->hMenu) SetMenu( getWindowHandle(), cs->hMenu );
		}
        }
  }
  else 	windowId = (UINT)cs->hMenu;

  DWORD dwOSWinStyle, dwOSFrameStyle;

  OSLibWinConvertStyle(cs->style, &dwOSWinStyle, &dwOSFrameStyle);

  OS2Hwnd = OSLibWinCreateWindow((parent) ? parent->getOS2WindowHandle() : 0,
                                 dwOSWinStyle, dwOSFrameStyle, (char *)cs->lpszName,
                                 cs->x, cs->y, cs->cx, cs->cy, 
		   	         (owner) ? owner->getOS2WindowHandle() : 0,
                                 (hwndLinkAfter == HWND_BOTTOM) ? TRUE : FALSE);

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

}
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
    lpPos = (LPINTERNALPOS)GetPropA( wndPtr->hwndSelf, atomInternalPos );
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

    if (maxSize) *maxSize = MinMax.ptMaxSize;
    if (maxPos) *maxPos = MinMax.ptMaxPosition;
    if (minTrack) *minTrack = MinMax.ptMinTrackSize;
    if (maxTrack) *maxTrack = MinMax.ptMaxTrackSize;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgCreate(HWND hwndOS2, ULONG initParam)
{
  OS2Hwnd = hwndOS2;
  return SendMessageA(WM_CREATE, 0, initParam);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgQuit()
{
  return SendMessageA(WM_QUIT, 0, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgClose()
{
  return SendMessageA(WM_CLOSE, 0, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgDestroy()
{
  return SendMessageA(WM_DESTROY, 0, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgEnable(BOOL fEnable)
{
  return SendMessageA(WM_ENABLE, fEnable, 0);
}
//******************************************************************************
//TODO: SW_PARENTCLOSING/OPENING flag (lParam)
//******************************************************************************
ULONG Win32Window::MsgShow(BOOL fShow)
{
  return SendMessageA(WM_SHOWWINDOW, fShow, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgMove(ULONG xScreen, ULONG yScreen, ULONG xParent, ULONG yParent)
{
  return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgSize(ULONG width, ULONG height, BOOL fMinimize, BOOL fMaximize)
{
 WORD fwSizeType = 0;
  
  if(fMinimize) {
	fwSizeType = SIZE_MINIMIZED;
  }
  else
  if(fMaximize) {
	fwSizeType = SIZE_MAXIMIZED;
  }
  else	fwSizeType = SIZE_RESTORED;

  return SendMessageA(WM_SIZE, fwSizeType, MAKELONG((USHORT)width, (USHORT)height));
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgActivate(BOOL fActivate, HWND hwnd)
{
  return SendMessageA(WM_ACTIVATE, (fActivate) ? WA_ACTIVE : WA_INACTIVE, hwnd);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgSetFocus(HWND hwnd)
{
  return SendMessageA(WM_SETFOCUS, hwnd, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgKillFocus(HWND hwnd)
{
  return SendMessageA(WM_KILLFOCUS, hwnd, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgButton(ULONG msg, ULONG x, ULONG y)
{
 ULONG win32msg;

  switch(msg) {
	case BUTTON_LEFTDOWN:
		win32msg = WM_LBUTTONDOWN;
		break;
	case BUTTON_LEFTUP:
		win32msg = WM_LBUTTONUP;
		break;
	case BUTTON_LEFTDBLCLICK:
		win32msg = WM_LBUTTONDBLCLK;
		break;
	case BUTTON_RIGHTUP:
		win32msg = WM_RBUTTONUP;
		break;
	case BUTTON_RIGHTDOWN:
		win32msg = WM_RBUTTONDOWN;
		break;
	case BUTTON_RIGHTDBLCLICK:
		win32msg = WM_RBUTTONDBLCLK;
		break;
	default:
		dprintf(("Win32Window::Button: invalid msg!!!!"));
		return 1;
  }
  return SendMessageA(win32msg, 0, MAKELONG(x, OS2TOWIN32POINT(height, y)));
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgPaint(ULONG tmp1, ULONG tmp2)
{
  return SendMessageA(WM_PAINT, 0, 0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Window::MsgEraseBackGround(ULONG hps)
{
  return SendMessageA(WM_ERASEBKGND, hps, 0);
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
				parentwindow->SendMessageA(WM_PARENTNOTIFY, MAKEWPARAM(Msg, window->getWindowId()), (LPARAM)window->getWindowHandle());
			}
			else	parentwindow->SendMessageA(WM_PARENTNOTIFY, MAKEWPARAM(Msg, window->getWindowId()), lParam );
		}
	}
	else	break;

	window = parentwindow;
   }
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
	{
 	 ULONG tmp;

		if(getParent()) {
			oldval = getParent()->getWindowHandle();
		}
		else	oldval = 0;

		if(value == 0) {//desktop window = parent
			setParent(NULL);
			OSLibWinSetParent(getOS2WindowHandle(), OSLIB_HWND_DESKTOP);
			return oldval;
		}
		if(HMHandleTranslateToOS2(value, &tmp) == NO_ERROR)
		{
			setParent((Win32Window *)tmp);
			OSLibWinSetParent(getOS2WindowHandle(), getParent()->getOS2WindowHandle());
			return oldval;
		}
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
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
		else	return 0;
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
GenericObject *Win32Window::windows  = NULL;
