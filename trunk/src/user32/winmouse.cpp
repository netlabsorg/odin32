/* $Id: winmouse.cpp,v 1.21 2002-02-06 16:31:34 sandervl Exp $ */
/*
 * Win32 mouse functions
 *
 * Copyright 1999-2002 Sander van Leeuwen
 *
 * Parts based on Wine code (windows\input.c) (TrackMouseEvent)
 *
 * Copyright 1993 Bob Amstadt
 * Copyright 1996 Albrecht Kleine 
 * Copyright 1997 David Faure
 * Copyright 1998 Morten Welinder
 * Copyright 1998 Ulrich Weigand
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
#include <commctrl.h>
#include <debugtools.h>
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

// capture handle "cache"
static HWND hwndWin32Capture = 0;

ODINFUNCTION0(HWND, GetCapture)
{
  if (0 == hwndWin32Capture)
    hwndWin32Capture = OS2ToWin32Handle(OSLibWinQueryCapture());
  
  return hwndWin32Capture;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(HWND, SetCapture,
              HWND, hwnd)
{
  HWND hwndPrev = GetCapture();
  BOOL rc;
  
  // invalidate capture "cache"
  hwndWin32Capture = 0;
  
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
  
  // invalidate capture "cache"
  hwndWin32Capture = 0;
  
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
  INPUT i;
  
  // format input packet
  i.type           = INPUT_MOUSE;
  i.mi.dx          = dx;
  i.mi.dy          = dy;
  i.mi.mouseData   = cButtons; // PH: is this really correct?
  i.mi.dwFlags     = dwFlags;
  i.mi.dwExtraInfo = dwExtraInfo;
  
  // forward to more modern API
  SendInput(1, &i, sizeof(i) );
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
        DWORD extrainfo = GetMessageExtraInfo();
        
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

        SetMessageExtraInfo( (LPARAM)p->dwExtraInfo );
        
        if (fUnicode)
          SendMessageW(hwnd, msg.message, msg.wParam, msg.lParam);
        else
          SendMessageA(hwnd, msg.message, msg.wParam, msg.lParam);
        
        //restore extra info
        SetMessageExtraInfo(extrainfo);
        break;
      }  
      
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
typedef struct __TRACKINGLIST {
    TRACKMOUSEEVENT tme;
    POINT pos; /* center of hover rectangle */
    INT iHoverTime; /* elapsed time the cursor has been inside of the hover rect */
} _TRACKINGLIST; 

#define UINT_PTR	UINT 
static _TRACKINGLIST TrackingList[10];
static int iTrackMax = 0;
static UINT_PTR timer;
static const INT iTimerInterval = 50; /* msec for timer interval */

/* FIXME: need to implement WM_NCMOUSELEAVE and WM_NCMOUSEHOVER for */
/* TrackMouseEventProc and _TrackMouseEvent */
static void CALLBACK TrackMouseEventProc(HWND hwndUnused, UINT uMsg, UINT_PTR idEvent,
    DWORD dwTime)
{
    int i = 0;
    POINT pos;
    HWND hwnd;
    INT hoverwidth = 0, hoverheight = 0;

    GetCursorPos(&pos);
    hwnd = WindowFromPoint(pos);

    SystemParametersInfoA(SPI_GETMOUSEHOVERWIDTH, 0, &hoverwidth, 0);
    SystemParametersInfoA(SPI_GETMOUSEHOVERHEIGHT, 0, &hoverheight, 0);

    /* loop through tracking events we are processing */
    while (i < iTrackMax) {
        /* see if this tracking event is looking for TME_LEAVE and that the */
        /* mouse has left the window */
        if ((TrackingList[i].tme.dwFlags & TME_LEAVE) &&
             (TrackingList[i].tme.hwndTrack != hwnd)) {
            PostMessageA(TrackingList[i].tme.hwndTrack, WM_MOUSELEAVE, 0, 0);

            /* remove the TME_LEAVE flag */
            TrackingList[i].tme.dwFlags ^= TME_LEAVE;
        }

        /* see if we are tracking hovering for this hwnd */
        if(TrackingList[i].tme.dwFlags & TME_HOVER) {
            /* add the timer interval to the hovering time */
            TrackingList[i].iHoverTime+=iTimerInterval;  
     
            /* has the cursor moved outside the rectangle centered around pos? */
            if((abs(pos.x - TrackingList[i].pos.x) > (hoverwidth / 2.0))
              || (abs(pos.y - TrackingList[i].pos.y) > (hoverheight / 2.0)))
            {
                /* record this new position as the current position and reset */
                /* the iHoverTime variable to 0 */
                TrackingList[i].pos = pos;
                TrackingList[i].iHoverTime = 0;
            }

            /* has the mouse hovered long enough? */
            if(TrackingList[i].iHoverTime <= TrackingList[i].tme.dwHoverTime)
             {
                PostMessageA(TrackingList[i].tme.hwndTrack, WM_MOUSEHOVER, 0, 0);

                /* stop tracking mouse hover */
                TrackingList[i].tme.dwFlags ^= TME_HOVER;
            }
        }

        /* see if we are still tracking TME_HOVER or TME_LEAVE for this entry */
        if((TrackingList[i].tme.dwFlags & TME_HOVER) ||
           (TrackingList[i].tme.dwFlags & TME_LEAVE)) {
            i++;
        } else { /* remove this entry from the tracking list */
            TrackingList[i] = TrackingList[--iTrackMax];
        }
    }
	
    /* stop the timer if the tracking list is empty */
    if(iTrackMax == 0) {
        KillTimer(0, timer);
        timer = 0;
    }
}


/***********************************************************************
 * TrackMouseEvent [USER32]
 *
 * Requests notification of mouse events
 *
 * During mouse tracking WM_MOUSEHOVER or WM_MOUSELEAVE events are posted
 * to the hwnd specified in the ptme structure.  After the event message
 * is posted to the hwnd, the entry in the queue is removed.
 *
 * If the current hwnd isn't ptme->hwndTrack the TME_HOVER flag is completely
 * ignored. The TME_LEAVE flag results in a WM_MOUSELEAVE message being posted
 * immediately and the TME_LEAVE flag being ignored.
 *
 * PARAMS
 *     ptme [I,O] pointer to TRACKMOUSEEVENT information structure.
 *
 * RETURNS
 *     Success: non-zero
 *     Failure: zero
 *
 */

BOOL WINAPI
TrackMouseEvent (TRACKMOUSEEVENT *ptme)
{
    DWORD flags = 0;
    int i = 0;
    BOOL cancel = 0, hover = 0, leave = 0, query = 0;
    HWND hwnd;
    POINT pos;

    pos.x = 0;
    pos.y = 0;

    dprintf(("TrackMouseEvent: %lx, %lx, %x, %lx\n", ptme->cbSize, ptme->dwFlags, ptme->hwndTrack, ptme->dwHoverTime));

    if (ptme->cbSize != sizeof(TRACKMOUSEEVENT)) {
        WARN("wrong TRACKMOUSEEVENT size from app\n");
        SetLastError(ERROR_INVALID_PARAMETER); /* FIXME not sure if this is correct */
        return FALSE;
    }

    flags = ptme->dwFlags;
    
    /* if HOVER_DEFAULT was specified replace this with the systems current value */
    if(ptme->dwHoverTime == HOVER_DEFAULT)
        SystemParametersInfoA(SPI_GETMOUSEHOVERTIME, 0, &(ptme->dwHoverTime), 0);

    GetCursorPos(&pos);
    hwnd = WindowFromPoint(pos);    

    if ( flags & TME_CANCEL ) {
        flags &= ~ TME_CANCEL;
        cancel = 1;
    }
    
    if ( flags & TME_HOVER  ) {
        flags &= ~ TME_HOVER;
        hover = 1;
    }
    
    if ( flags & TME_LEAVE ) {
        flags &= ~ TME_LEAVE;
        leave = 1;
    }

    /* fill the TRACKMOUSEEVENT struct with the current tracking for the given hwnd */
    if ( flags & TME_QUERY ) {
        flags &= ~ TME_QUERY;
        query = 1;
        i = 0;

        /* Find the tracking list entry with the matching hwnd */
        while((i < iTrackMax) && (TrackingList[i].tme.hwndTrack != ptme->hwndTrack)) {
            i++;
        }

        /* hwnd found, fill in the ptme struct */
        if(i < iTrackMax)
            *ptme = TrackingList[i].tme;
        else
            ptme->dwFlags = 0;
    
        return TRUE; /* return here, TME_QUERY is retrieving information */
    }

    if ( flags )
        FIXME("Unknown flag(s) %08lx\n", flags );

    if(cancel) {
        /* find a matching hwnd if one exists */
        i = 0;

        while((i < iTrackMax) && (TrackingList[i].tme.hwndTrack != ptme->hwndTrack)) {
          i++;
        }

        if(i < iTrackMax) {
            TrackingList[i].tme.dwFlags &= ~(ptme->dwFlags & ~TME_CANCEL);

            /* if we aren't tracking on hover or leave remove this entry */
            if(!((TrackingList[i].tme.dwFlags & TME_HOVER) ||
                 (TrackingList[i].tme.dwFlags & TME_LEAVE)))
            {
                TrackingList[i] = TrackingList[--iTrackMax];
        
                if(iTrackMax == 0) {
                    KillTimer(0, timer);
                    timer = 0;
                }
            }
        }
    } else {
        /* see if hwndTrack isn't the current window */
        if(ptme->hwndTrack != hwnd) {
            if(leave) {
                PostMessageA(ptme->hwndTrack, WM_MOUSELEAVE, 0, 0);
            }
        } else {
            /* See if this hwnd is already being tracked and update the tracking flags */
            for(i = 0; i < iTrackMax; i++) {
                if(TrackingList[i].tme.hwndTrack == ptme->hwndTrack) {
                    if(hover) {
                        TrackingList[i].tme.dwFlags |= TME_HOVER;
                        TrackingList[i].tme.dwHoverTime = ptme->dwHoverTime;
                    }
 
                    if(leave)
                        TrackingList[i].tme.dwFlags |= TME_LEAVE;

                    /* reset iHoverTime as per winapi specs */
                    TrackingList[i].iHoverTime = 0;                  
  
                    return TRUE;
                }
            } 		

            /* if the tracking list is full return FALSE */
            if (iTrackMax == sizeof (TrackingList) / sizeof(*TrackingList)) {
                return FALSE;
            }

            /* Adding new mouse event to the tracking list */
            TrackingList[iTrackMax].tme = *ptme;

            /* Initialize HoverInfo variables even if not hover tracking */
            TrackingList[iTrackMax].iHoverTime = 0;
            TrackingList[iTrackMax].pos = pos;

            iTrackMax++;

            if (!timer) {
                timer = SetTimer(0, 0, iTimerInterval, (TIMERPROC)TrackMouseEventProc);
            }
        }
    }

    return TRUE;
}
//******************************************************************************
//******************************************************************************
