/* $Id: winmouse.cpp,v 1.17 2001-09-27 10:02:38 phaller Exp $ */
/*
 * Mouse handler for DINPUT
 *
 * Copyright 1999-2001 Sander van Leeuwen
 *
 * TODO: SwapMouseButton: 
 *       We shouldn't let win32 apps change this for the whole system
 *       better to change mouse button message translation instead
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <misc.h>
#include "win32wbase.h"
#include <winuser32.h>
#include <winuser.h>
#include <win\mouse.h>
#include "winmouse.h"
#include "oslibmsg.h"
#include "pmwindow.h"
#include "oslibwin.h"

#define DBG_LOCALLOG	DBG_winmouse
#include "dbglocal.h"


ODINDEBUGCHANNEL(USER32-WINMOUSE)


/****************************************************************************
 * local variables
 ****************************************************************************/

LPMOUSE_EVENT_PROC mouseHandler = NULL;
WNDPROC            keyboardHandler = NULL;


//******************************************************************************
//******************************************************************************
ODINPROCEDURE1(MOUSE_Enable,
               LPMOUSE_EVENT_PROC, lpMouseEventProc)
{
  if(lpMouseEventProc == (LPMOUSE_EVENT_PROC)-1)
    mouseHandler = NULL;
  else	
    mouseHandler = lpMouseEventProc;
}
//******************************************************************************
//******************************************************************************
ODINPROCEDURE1(KEYBOARD_Enable,
               WNDPROC, handler)
{
  keyboardHandler = handler;
}
//******************************************************************************
//******************************************************************************
BOOL DInputKeyBoardHandler(MSG *msg)
{
  if(!ISKDB_CAPTURED())
  	return FALSE;

  return keyboardHandler(msg->hwnd, msg->message, msg->wParam, msg->lParam);
}
//******************************************************************************
//******************************************************************************
BOOL DInputMouseHandler(HWND hwnd, ULONG msg, ULONG x, ULONG y)
{
 WINE_MOUSEEVENT mouseEvent;
 DWORD dwFlags = MOUSEEVENTF_ABSOLUTE;

  if(!ISMOUSE_CAPTURED())
	return FALSE;

  mouseEvent.magic = WINE_MOUSEEVENT_MAGIC;
  mouseEvent.hWnd  = hwnd;
  mouseEvent.time  = OSLibWinQueryMsgTime();
  mouseEvent.keyState = 0; //not used in dinput right now
  switch(msg) 
  {
  case WM_NCLBUTTONDOWN:
  case WM_LBUTTONDOWN:
	dwFlags |= MOUSEEVENTF_LEFTDOWN;
        break;
  case WM_NCLBUTTONUP:
  case WM_LBUTTONUP:
	dwFlags |= MOUSEEVENTF_LEFTUP;
        break;
  case WM_NCRBUTTONUP:
  case WM_RBUTTONUP:
	dwFlags |= MOUSEEVENTF_RIGHTUP;
        break;
  case WM_NCRBUTTONDOWN:
  case WM_RBUTTONDOWN:
	dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        break;
  case WM_NCMBUTTONUP:
  case WM_MBUTTONUP:
	dwFlags |= MOUSEEVENTF_MIDDLEUP;
        break;
  case WM_NCMBUTTONDOWN:
  case WM_MBUTTONDOWN:
	dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
        break;
  case WM_MOUSEMOVE:
  case WM_NCMOUSEMOVE:
	dwFlags |= MOUSEEVENTF_MOVE;
 	break;
  default:
	//TODO: handle double clicks???
	return FALSE;
  }

  x = (((long)x << 16) + ScreenWidth-1)  / ScreenWidth;
  y = (((long)y << 16) + ScreenHeight-1) / ScreenHeight;

  return mouseHandler(dwFlags, x, y, 0, (DWORD)&mouseEvent);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(HWND, GetCapture)
{
  return OS2ToWin32Handle(OSLibWinQueryCapture());
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HWND, SetCapture,
              HWND, hwnd)
{
  HWND hwndPrev = GetCapture();
  BOOL rc;

  if(hwnd == 0) 
  {
    ReleaseCapture();
    return hwndPrev;
  }
  
  if(hwnd == hwndPrev) 
  {
    dprintf(("USER32: SetCapture %x; already set to that window; ignore", hwnd));
    return hwndPrev;
  }
  
  if(hwndPrev != NULL) 
  {
    //SvL: WinSetCapture returns an error if mouse is already captured
    OSLibWinSetCapture(0);
  }
  
  rc = OSLibWinSetCapture(Win32ToOS2Handle(hwnd));
  dprintf(("USER32: SetCapture %x (prev %x) returned %d", hwnd, hwndPrev, rc));
  if(hwndPrev) 
  {
    SendMessageA(hwndPrev, WM_CAPTURECHANGED, 0L, hwnd);
  }
  return hwndPrev;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(BOOL, ReleaseCapture)
{
  HWND hwndPrev;
  BOOL ret;

  hwndPrev = GetCapture();
  ret = OSLibWinSetCapture(0);
  if(hwndPrev) 
  {
    SendMessageA(hwndPrev, WM_CAPTURECHANGED, 0L, 0L);
  }
  return ret;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION0(UINT, GetDoubleClickTime)
{
  UINT result = OSLibWinQuerySysValue(SVOS_DBLCLKTIME);
  if(result == 0)
    SetLastError(ERROR_INVALID_PARAMETER); //TODO: ????

  return result;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL, SetDoubleClickTime,
              UINT, uInterval)
{
  BOOL ret = TRUE;

  ret = OSLibWinSetSysValue(SVOS_DBLCLKTIME, uInterval);
  if(ret == FALSE )
  {
    SetLastError(ERROR_INVALID_PARAMETER); //TODO: ????
  }
  return (ret);
}
//******************************************************************************
//TODO: we shouldn't let win32 apps change this for the whole system
//      better to change mouse button message translation instead
BOOL OPEN32API __SwapMouseButton(BOOL swapFlag);

inline BOOL _SwapMouseButton(BOOL swapFlag)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = __SwapMouseButton(swapFlag);
    SetFS(sel);

    return yyrc;
}

//******************************************************************************
ODINFUNCTION1(BOOL, SwapMouseButton,
              BOOL, fSwap)
{
  return _SwapMouseButton(fSwap);
}

/*****************************************************************************
 * Name      : VOID WIN32API mouse_event
 * Purpose   : The mouse_event function synthesizes mouse motion and button clicks.
 * Parameters: DWORD dwFlags     flags specifying various motion/click variants
 *             DWORD dx          horizontal mouse position or position change
 *             DWORD dy          vertical mouse position or position change
 *             DWORD cButtons    unused, reserved for future use, set to zero
 *             DWORD dwExtraInfo 32 bits of application-defined information
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINPROCEDURE5(mouse_event,
               DWORD, dwFlags, 
               DWORD, dx,
               DWORD, dy,
               DWORD, cButtons,
               DWORD, dwExtraInfo)
{
  dprintf(("not implemented"));
}


ODINFUNCTION1(DWORD, TrackMouseEvent,
              DWORD, param1)
{
  dprintf(("not implemented"));
  return 0;
}


/*****************************************************************************
 * Name      : UINT SendInput
 * Purpose   : The SendInput function synthesizes keystrokes, mouse motions, 
 *             and button clicks
 * Parameters: UINT    nInputs // count if input events
 *             LPINPUT pInputs // array of input structures
 *             int     chSize  // size of structure
 * Variables :
 * Result    : number of events successfully inserted,
 *             0 if the input was already blocked by another thread
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

ODINFUNCTION3(UINT,    SendInput,
              UINT,    nInputs,
              LPINPUT, pInputs,
              int,     chSize)
{
  dprintf(("not correctly implemented"));
  
  // The simulated input is sent to the
  // foreground thread's message queue.
  // (WM_KEYUP, WM_KEYDOWN)
  // After GetMessage or PeekMessage,
  // TranslateMessage posts an appropriate
  // WM_CHAR message.
  HWND hwnd = GetForegroundWindow();
  
  LPINPUT piBase = pInputs;
  for (int i = 0;
       i < nInputs;
       i++,
       piBase++)
  {
    switch(piBase->type)
    {
      case INPUT_MOUSE:
      {
        PMOUSEINPUT p = (PMOUSEINPUT)&piBase->mi;
        MSG msg;
        
        // simulate mouse input message
        // @@@PH
      }  
      break;
      
      // compose a keyboard input message
      case INPUT_KEYBOARD:
      {
        PKEYBDINPUT p = (PKEYBDINPUT)&piBase->ki;
        MSG msg;
        BOOL fUnicode = (p->dwFlags & KEYEVENTF_UNICODE) == KEYEVENTF_UNICODE;
        
        // build keyboard message
        msg.message = (p->dwFlags & KEYEVENTF_KEYUP) ? WM_KEYUP : WM_KEYDOWN;
        
        if (p->dwFlags & KEYEVENTF_SCANCODE)
        {
          // keystroke is identified by the scancode
          if (fUnicode)
            msg.wParam = MapVirtualKeyW(p->wScan, 1);
          else
            msg.wParam = MapVirtualKeyA(p->wScan, 1);
        }
        else
          msg.wParam = p->wVk;
        
        msg.lParam = 0x0000001 |                    // repeat count
                      ( (p->wScan & 0xff) << 16);   // scan code
        
        if (p->dwFlags & KEYEVENTF_EXTENDEDKEY)
          msg.lParam |= (1 << 24);
        
        // set additional message flags
        if (msg.message == WM_KEYDOWN)
        {
          // @@@PH
          // bit 30 - previous key state cannot be set, how to determine?
        }
        else
        {
          // WM_KEYUP -> previous key is always 1, transistion state
          // always 1
          msg.lParam |= (1 << 30);
          msg.lParam |= (1 << 31);
        }
        
        msg.time   = p->time;
        
        // @@@PH
        // unknown: do we have to post or to send the message?
        
        if (fUnicode)
          SendMessageW(hwnd, msg.message, msg.wParam, msg.lParam);
        else
          SendMessageA(hwnd, msg.message, msg.wParam, msg.lParam);
        
        SetMessageExtraInfo( (LPARAM)p->dwExtraInfo );
      }  
      break;
      
      case INPUT_HARDWARE:
      {
        PHARDWAREINPUT p = (PHARDWAREINPUT)&piBase->hi;
        
        // @@@PH
        // not supported for the time being
      }  
      break;
      
      default:
        dprintf(("unsupported input packet type %d",
                piBase->type));
        break;
    }
  }
  
  return 0;
}

/*****************************************************************************
 * Name      : BOOL WIN32API DragDetect
 * Purpose   : The DragDetect function captures the mouse and tracks its movement
 * Parameters: HWND  hwnd
 *             POINT pt
 * Variables :
 * Result    : If the user moved the mouse outside of the drag rectangle while
 *               holding the left button down, the return value is TRUE.
 *             If the user did not move the mouse outside of the drag rectangle
 *               while holding the left button down, the return value is FALSE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
ODINFUNCTION2(BOOL,  DragDetect,
              HWND,  hwnd,
              POINT, pt)
{
  dprintf(("not implemented"));

  return (FALSE);
}
//******************************************************************************
//******************************************************************************
