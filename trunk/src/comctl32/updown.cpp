/* $Id: updown.cpp,v 1.3 2001-03-31 13:25:27 sandervl Exp $ */
/*
 * Updown control
 *
 * Copyright 1997 Dimitrie O. Paun
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 *
 * TODO:
 *   - I am not sure about the default values for the Min, Max, Pos
 *     (in the UPDOWN_INFO the fields: MinVal, MaxVal, CurVal)
 *   - I think I do not handle correctly the WS_BORDER style.
 *     (Should be fixed. <ekohl@abo.rhein-zeitung.de>)
 *
 * Testing:
 *   Not much. The following  have not been tested at all:
 *     - horizontal arrows
 *     - listbox as buddy window
 *     - acceleration
 *     - UDS_ALIGNLEFT, ~UDS_WRAP
 *       (tested - they work)
 *     - integers with thousand separators.
 *       (fixed bugs. <noel@macadamian.com>)
 *
 *   Even though the above list seems rather large, the control seems to
 *   behave very well so I am confident it does work in most (all) of the
 *   untested cases.
 * Problems:
 *   I do not like the arrows yet, I'll work more on them later on.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "winbase.h"
#include "winuser.h"
#include "commctrl.h"
#include "winnls.h"
#include "ccbase.h"
#include "updown.h"
#include "comctl32.h"

/* Control configuration constants */

#define INITIAL_DELAY    500 /* initial timer until auto-increment kicks in */
#define REPEAT_DELAY     50  /* delay between auto-increments */

#define DEFAULT_WIDTH    14  /* default width of the ctrl */
#define DEFAULT_XSEP      0  /* default separation between buddy and crtl */
#define DEFAULT_ADDTOP    0  /* amount to extend above the buddy window */
#define DEFAULT_ADDBOT    0  /* amount to extend below the buddy window */
#define DEFAULT_BUDDYBORDER  2  /* Width/height of the buddy border */


/* Work constants */

#define FLAG_INCR        0x01
#define FLAG_DECR        0x02
#define FLAG_MOUSEIN     0x04
#define FLAG_CLICKED     (FLAG_INCR | FLAG_DECR)

#define TIMERID1         1
#define TIMERID2         2
#define BUDDY_UPDOWN_HWND        "buddyUpDownHWND"
#define BUDDY_SUPERCLASS_WNDPROC "buddySupperClassWndProc"

static int accelIndex = -1;

//#define UNKNOWN_PARAM(msg, wParam, lParam) WARN(updown, \
//        "UpDown Ctrl: Unknown parameter(s) for message " #msg     \
//      "(%04x): wp=%04x lp=%08lx\n", msg, wParam, lParam);
#define UNKNOWN_PARAM(msg, wParam, lParam)

#define UPDOWN_GetInfoPtr(hwnd) ((UPDOWN_INFO*)getInfoPtr(hwnd))

static LRESULT CALLBACK
UPDOWN_Buddy_SubclassProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/***********************************************************************
 *           UPDOWN_InBounds
 * Tests if a given value 'val' is between the Min&Max limits
 */
static BOOL UPDOWN_InBounds(HWND hwnd, int val)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr (hwnd);

  if(infoPtr->MaxVal > infoPtr->MinVal)
    return (infoPtr->MinVal <= val) && (val <= infoPtr->MaxVal);
  else
    return (infoPtr->MaxVal <= val) && (val <= infoPtr->MinVal);
}

/***********************************************************************
 *           UPDOWN_OffsetVal
 * Tests if we can change the current value by delta. If so, it changes
 * it and returns TRUE. Else, it leaves it unchanged and returns FALSE.
 */
static BOOL UPDOWN_OffsetVal(HWND hwnd, int delta)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr (hwnd);

  /* check if we can do the modification first */
  if(!UPDOWN_InBounds (hwnd, infoPtr->CurVal+delta)){
    if (GetWindowLongA (hwnd, GWL_STYLE) & UDS_WRAP)
    {
      delta += (delta < 0 ? -1 : 1) *
        (infoPtr->MaxVal < infoPtr->MinVal ? -1 : 1) *
        (infoPtr->MinVal - infoPtr->MaxVal) +
        (delta < 0 ? 1 : -1);
    }
    else
      return FALSE;
  }

  infoPtr->CurVal += delta;
  return TRUE;
}

/***********************************************************************
 * UPDOWN_HasBuddyBorder [Internal]
 *
 * When we have a buddy set and that we are aligned on our buddy, we
 * want to draw a sunken edge to make like we are part of that control.
 */
static BOOL UPDOWN_HasBuddyBorder(HWND hwnd)
{
  UPDOWN_INFO* infoPtr = UPDOWN_GetInfoPtr (hwnd);
  DWORD        dwStyle = GetWindowLongA (hwnd, GWL_STYLE);

  return  ( ((dwStyle & (UDS_ALIGNLEFT | UDS_ALIGNRIGHT)) != 0) &&
            (SendMessageA(hwnd, UDM_GETBUDDY, 0, 0) != 0) &&
            (lstrcmpiA(infoPtr->szBuddyClass, "EDIT") == 0 ) );
}

/***********************************************************************
 *           UPDOWN_GetArrowRect
 * wndPtr   - pointer to the up-down wnd
 * rect     - will hold the rectangle
 * incr     - TRUE  get the "increment" rect (up or right)
 *            FALSE get the "decrement" rect (down or left)
 *
 */
static void UPDOWN_GetArrowRect (HWND hwnd, RECT *rect, BOOL incr)
{
  DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
  int   len; /* will hold the width or height */

  GetClientRect (hwnd, rect);

  /*
   * Make sure we calculate the rectangle to fit even if we draw the
   * border.
   */
  if (UPDOWN_HasBuddyBorder(hwnd))
  {
    if (dwStyle & UDS_ALIGNLEFT)
      rect->left+=DEFAULT_BUDDYBORDER;
    else
      rect->right-=DEFAULT_BUDDYBORDER;

    InflateRect(rect, 0, -DEFAULT_BUDDYBORDER);
  }

  /*
   * We're calculating the midpoint to figure-out where the
   * separation between the buttons will lay. We make sure that we
   * round the uneven numbers by adding 1.
   */
  if (dwStyle & UDS_HORZ) {
    len = rect->right - rect->left + 1; /* compute the width */
    if (incr)
      rect->left = rect->left + len/2;
    else
      rect->right =  rect->left + len/2;
  }
  else {
    len = rect->bottom - rect->top + 1; /* compute the height */
    if (incr)
      rect->bottom =  rect->top + len/2;
    else
      rect->top =  rect->top + len/2;
  }
}

/***********************************************************************
 *           UPDOWN_GetArrowFromPoint
 * Returns the rectagle (for the up or down arrow) that contains pt.
 * If it returns the up rect, it returns TRUE.
 * If it returns the down rect, it returns FALSE.
 */
static BOOL
UPDOWN_GetArrowFromPoint (HWND hwnd, RECT *rect, POINT pt)
{
    UPDOWN_GetArrowRect (hwnd, rect, TRUE);
  if(PtInRect(rect, pt))
    return TRUE;

    UPDOWN_GetArrowRect (hwnd, rect, FALSE);
  return FALSE;
}


/***********************************************************************
 *           UPDOWN_GetThousandSep
 * Returns the thousand sep. If an error occurs, it returns ','.
 */
static char UPDOWN_GetThousandSep(VOID)
{
  char sep[2];

  if(GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND,
                      sep, sizeof(sep)) != 1)
    return ',';

  return sep[0];
}

/***********************************************************************
 *           UPDOWN_GetBuddyInt
 * Tries to read the pos from the buddy window and if it succeeds,
 * it stores it in the control's CurVal
 * returns:
 *   TRUE  - if it read the integer from the buddy successfully
 *   FALSE - if an error occured
 */
static BOOL UPDOWN_GetBuddyInt (HWND hwnd)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr (hwnd);
  char txt[20], sep, *src, *dst;
  int newVal;

  if (!IsWindow(infoPtr->Buddy)) return FALSE;

  /*if the buddy is a list window, we must set curr index */
  if (!lstrcmpiA(infoPtr->szBuddyClass,"ListBox"))
  {
    newVal = SendMessageA(infoPtr->Buddy,LB_GETCARETINDEX,0,0);
    if(newVal < 0) return FALSE;
  } else
  {
    /* we have a regular window, so will get the text */
    if (!GetWindowTextA(infoPtr->Buddy,txt,sizeof(txt))) return FALSE;

    sep = UPDOWN_GetThousandSep();

    /* now get rid of the separators */
    for(src = dst = txt;*src;src++)
      if (*src != sep)
        *dst++ = *src;
    *dst = 0;

    /* try to convert the number and validate it */
    newVal = strtol(txt,&src,infoPtr->Base);
    if (*src || !UPDOWN_InBounds(hwnd,newVal)) return FALSE;

//    TRACE(updown, "new value(%d) read from buddy (old=%d)\n",
//               newVal, infoPtr->CurVal);
  }

  infoPtr->CurVal = newVal;

  return TRUE;
}


/***********************************************************************
 *           UPDOWN_SetBuddyInt
 * Tries to set the pos to the buddy window based on current pos
 * returns:
 *   TRUE  - if it set the caption of the  buddy successfully
 *   FALSE - if an error occured
 */
static BOOL UPDOWN_SetBuddyInt (HWND hwnd)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  char txt1[20],sep;
  int len;

  if (!IsWindow(infoPtr->Buddy)) return FALSE;

//  TRACE(updown, "set new value(%d) to buddy.\n",
//             infoPtr->CurVal);

  /*if the buddy is a list window, we must set curr index */
  if(!lstrcmpiA(infoPtr->szBuddyClass, "ListBox"))
  {
    SendMessageA(infoPtr->Buddy,LB_SETCURSEL,infoPtr->CurVal,0);
  } else
  { /* Regular window, so set caption to the number */
    len = sprintf(txt1,(infoPtr->Base == 16) ? "%X" : "%d",infoPtr->CurVal);

    sep = UPDOWN_GetThousandSep();

    /* Do thousands seperation if necessary */
    if (!(GetWindowLongA (hwnd, GWL_STYLE) & UDS_NOTHOUSANDS) && (len > 3)) {
      char txt2[20], *src = txt1, *dst = txt2;
      if(len%3 > 0){
        lstrcpynA (dst, src, len%3 + 1);      /* need to include the null */
        dst += len%3;
        src += len%3;
      }
      for(len=0; *src; len++){
        if(len%3==0)
          *dst++ = sep;
        *dst++ = *src++;
      }
      *dst = 0;           /* null terminate it */
      strcpy(txt1, txt2); /* move it to the proper place */
    }
    SetWindowTextA(infoPtr->Buddy, txt1);
  }

  return TRUE;
}

/***********************************************************************
 * UPDOWN_DrawBuddyBorder [Internal]
 *
 * When we have a buddy set and that we are aligned on our buddy, we
 * want to draw a sunken edge to make like we are part of that control.
 */
static void UPDOWN_DrawBuddyBorder (HWND hwnd, HDC hdc)
{
  DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
  RECT  clientRect;

  GetClientRect(hwnd, &clientRect);

  if (dwStyle & UDS_ALIGNLEFT)
    DrawEdge(hdc, &clientRect, EDGE_SUNKEN, BF_BOTTOM | BF_LEFT | BF_TOP);
  else
    DrawEdge(hdc, &clientRect, EDGE_SUNKEN, BF_BOTTOM | BF_RIGHT | BF_TOP);
}

/***********************************************************************
 * UPDOWN_Draw [Internal]
 *
 * Draw the arrows. The background need not be erased.
 */
static void UPDOWN_Draw(HWND hwnd,HDC hdc)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd, GWL_STYLE);
  BOOL prssed;
  RECT rect;

  /*
   * Draw the common border between ourselves and our buddy.
   */
  if (UPDOWN_HasBuddyBorder(hwnd))
    UPDOWN_DrawBuddyBorder(hwnd, hdc);

  /* Draw the incr button */
  UPDOWN_GetArrowRect (hwnd, &rect, TRUE);
  prssed = (infoPtr->Flags & FLAG_INCR) && (infoPtr->Flags & FLAG_MOUSEIN);
  DrawFrameControl(hdc, &rect, DFC_SCROLL,
        (dwStyle & UDS_HORZ ? DFCS_SCROLLLEFT : DFCS_SCROLLUP) |
        (prssed ? DFCS_PUSHED : 0) |
        (dwStyle&WS_DISABLED ? DFCS_INACTIVE : 0) );

  /* Draw the space between the buttons */
  rect.top = rect.bottom;
  rect.bottom++;
  DrawEdge(hdc, &rect, 0, BF_MIDDLE);

  /* Draw the decr button */
  UPDOWN_GetArrowRect(hwnd, &rect, FALSE);
  prssed = (infoPtr->Flags & FLAG_DECR) && (infoPtr->Flags & FLAG_MOUSEIN);
  DrawFrameControl(hdc, &rect, DFC_SCROLL,
        (dwStyle & UDS_HORZ ? DFCS_SCROLLRIGHT : DFCS_SCROLLDOWN) |
        (prssed ? DFCS_PUSHED : 0) |
        (dwStyle & WS_DISABLED ? DFCS_INACTIVE : 0) );
}

/***********************************************************************
 * UPDOWN_Refresh [Internal]
 *
 * Synchronous drawing (must NOT be used in WM_PAINT).
 * Calls UPDOWN_Draw.
 */
static void UPDOWN_Refresh (HWND hwnd)
{
    HDC hdc;

    hdc = GetDC (hwnd);
    UPDOWN_Draw (hwnd, hdc);
    ReleaseDC (hwnd, hdc);
}


/***********************************************************************
 * UPDOWN_Paint [Internal]
 *
 * Asynchronous drawing (must ONLY be used in WM_PAINT).
 * Calls UPDOWN_Draw.
 */
static void UPDOWN_Paint (HWND hwnd, HDC passedDC)
{
    PAINTSTRUCT ps;
    HDC         hdc = passedDC;

    if (passedDC == 0)
      hdc = BeginPaint (hwnd, &ps);

    UPDOWN_Draw (hwnd, hdc);

    if (passedDC == 0)
      EndPaint (hwnd, &ps);
}

/***********************************************************************
 *           UPDOWN_SetBuddyHandle
 * CB: UPDOWN_SetBuddy == message handler
 * Tests if 'hwndBud' is a valid window handle. If not, returns FALSE.
 * Else, sets it as a new Buddy.
 * Then, it should subclass the buddy
 * If window has the UDS_ARROWKEYS, it subcalsses the buddy window to
 * process the UP/DOWN arrow keys.
 * If window has the UDS_ALIGNLEFT or UDS_ALIGNRIGHT style
 * the size/pos of the buddy and the control are adjusted accordingly.
 */
static BOOL UPDOWN_SetBuddyHandle (HWND hwnd, HWND hwndBud)
{
  UPDOWN_INFO* infoPtr = UPDOWN_GetInfoPtr (hwnd);
  DWORD        dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
  RECT         budRect;  /* new coord for the buddy */
  int          x,width;  /* new x position and width for the up-down */

  /* Is it a valid bud? */
  if(!IsWindow(hwndBud))
    return FALSE;

  /* there is already a body assigned */
  if ( infoPtr->Buddy )
    RemovePropA(infoPtr->Buddy, BUDDY_UPDOWN_HWND);

  /* Store buddy window handle */
  infoPtr->Buddy = hwndBud;

  /* keep upDown ctrl hwnd in a buddy property */
  SetPropA( hwndBud, BUDDY_UPDOWN_HWND, hwnd);

  /* Store buddy window clas name */
  memset(infoPtr->szBuddyClass, 0, UPDOWN_BUDDYCLASSNAMELEN);
  GetClassNameA (hwndBud, infoPtr->szBuddyClass, UPDOWN_BUDDYCLASSNAMELEN-1);

  if(dwStyle & UDS_ARROWKEYS){
    /* Note that I don't clear the BUDDY_SUPERCLASS_WNDPROC property
       when we reset the upDown ctrl buddy to another buddy because it is not
       good to break the window proc chain. */

    /* keep buddy supperclass wndproc in prop instead of in ptr struct
       to prevent accessing freed memory */
    SetPropA(
      hwndBud,
      BUDDY_SUPERCLASS_WNDPROC,
      (LONG)GetWindowLongA(hwndBud, GWL_WNDPROC) );

    /* Assign the buddy wndproc to local wndproc in order to override
       keyboard's up and down arrow */
    SetWindowLongA(
      hwndBud,
      GWL_WNDPROC,
      (LONG)UPDOWN_Buddy_SubclassProc);
  }

  /* do we need to do any adjustments? */
  if(!(dwStyle & (UDS_ALIGNLEFT | UDS_ALIGNRIGHT)))
    return TRUE;

  /* Get the rect of the buddy relative to its parent */
  GetWindowRect(infoPtr->Buddy, &budRect);
  MapWindowPoints(HWND_DESKTOP, GetParent(infoPtr->Buddy),
                  (POINT *)(&budRect.left), 2);

  /* now do the positioning */
  if(dwStyle & UDS_ALIGNRIGHT){
    budRect.right -= DEFAULT_WIDTH+DEFAULT_XSEP;
    x  = budRect.right+DEFAULT_XSEP;
  }
  else{ /* UDS_ALIGNLEFT */
    x  = budRect.left;
    budRect.left += DEFAULT_WIDTH+DEFAULT_XSEP;
  }

  /* first adjust the buddy to accomodate the up/down */
  SetWindowPos(infoPtr->Buddy, 0, budRect.left, budRect.top,
               budRect.right  - budRect.left, budRect.bottom - budRect.top,
               SWP_NOACTIVATE|SWP_NOZORDER);

  /* now position the up/down */
  /* Since the UDS_ALIGN* flags were used, */
  /* we will pick the position and size of the window. */
  width = DEFAULT_WIDTH;

  /*
   * If the updown has a buddy border, it has to overlap with the buddy
   * to look as if it is integrated with the buddy control.
   * We nudge the control or change it size to overlap.
   */
  if (UPDOWN_HasBuddyBorder(hwnd))
  {
    if(dwStyle & UDS_ALIGNRIGHT)
      x-=DEFAULT_BUDDYBORDER;
    else
      width+=DEFAULT_BUDDYBORDER;
  }

  SetWindowPos (hwnd, infoPtr->Buddy,
                x, budRect.top-DEFAULT_ADDTOP,
                width, (budRect.bottom-budRect.top)+DEFAULT_ADDTOP+DEFAULT_ADDBOT,
                SWP_NOACTIVATE);

  return TRUE;
}

/***********************************************************************
 *           UPDOWN_DoAction
 *
 * This function increments/decrements the CurVal by the
 * 'delta' amount according to the 'incr' flag
 * It notifies the parent as required.
 * It handles wraping and non-wraping correctly.
 * It is assumed that delta>0
 */
static void UPDOWN_DoAction (HWND hwnd, int delta, BOOL incr)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  int old_val = infoPtr->CurVal;
  NM_UPDOWN ni;

//  TRACE(updown, "%s by %d\n", incr ? "inc" : "dec", delta);

  /* check if we can do the modification first */
  delta *= (incr ? 1 : -1) * (infoPtr->MaxVal < infoPtr->MinVal ? -1 : 1);
  if(!UPDOWN_OffsetVal(hwnd,delta)) return;

  /* so, if we can do the change, recompute delta and restore old value */
  delta = infoPtr->CurVal-old_val;
  infoPtr->CurVal = old_val;

  /* We must notify parent now to obtain permission */
  ni.iPos = infoPtr->CurVal;
  ni.iDelta = delta;
  ni.hdr.hwndFrom = hwnd;
  ni.hdr.idFrom   = GetWindowLongA(hwnd,GWL_ID);
  ni.hdr.code = UDN_DELTAPOS;
  if (SendMessageA(GetParent(hwnd),WM_NOTIFY,
                   (WPARAM)ni.hdr.idFrom,(LPARAM)&ni))
    return; /* we are not allowed to change */

  /* Now adjust value with (maybe new) delta */
  if (!UPDOWN_OffsetVal (hwnd,ni.iDelta)) return;

  /* Now take care about our buddy */
  if(!IsWindow(infoPtr->Buddy))
    return; /* Nothing else to do */


  if (dwStyle & UDS_SETBUDDYINT) UPDOWN_SetBuddyInt(hwnd);

  /* Also, notify it */
  /* FIXME: do we need to send the notification only if
            we do not have the UDS_SETBUDDYINT style set? */

  SendMessageA (GetParent (hwnd),
                dwStyle & UDS_HORZ ? WM_HSCROLL : WM_VSCROLL,
                 MAKELONG(incr ? SB_LINEUP : SB_LINEDOWN, infoPtr->CurVal),
                hwnd);
}

/***********************************************************************
 *           UPDOWN_IsEnabled
 *
 * Returns TRUE if it is enabled as well as its buddy (if any)
 *         FALSE otherwise
 */
static BOOL UPDOWN_IsEnabled (HWND hwnd)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr (hwnd);

  if(GetWindowLongA (hwnd, GWL_STYLE) & WS_DISABLED)
    return FALSE;
  return IsWindowEnabled(infoPtr->Buddy);
}

/***********************************************************************
 *           UPDOWN_CancelMode
 *
 * Deletes any timers, releases the mouse and does  redraw if necessary.
 * If the control is not in "capture" mode, it does nothing.
 * If the control was not in cancel mode, it returns FALSE.
 * If the control was in cancel mode, it returns TRUE.
 */
static BOOL UPDOWN_CancelMode (HWND hwnd)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr (hwnd);

  /* if not in 'capture' mode, do nothing */
  if(!(infoPtr->Flags & FLAG_CLICKED))
    return FALSE;

  KillTimer (hwnd, TIMERID1); /* kill all possible timers */
  KillTimer (hwnd, TIMERID2);

  if (GetCapture() == hwnd)    /* let the mouse go         */
    ReleaseCapture();          /* if we still have it      */

  infoPtr->Flags = 0;          /* get rid of any flags     */
  UPDOWN_Refresh (hwnd);       /* redraw the control just in case */

  return TRUE;
}

/***********************************************************************
 *           UPDOWN_HandleMouseEvent
 *
 * Handle a mouse event for the updown.
 * 'pt' is the location of the mouse event in client or
 * windows coordinates.
 */
static void UPDOWN_HandleMouseEvent (HWND hwnd, UINT msg, POINT pt)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr (hwnd);
  DWORD dwStyle = GetWindowLongA (hwnd, GWL_STYLE);
  RECT rect;
  int temp;

  switch(msg)
    {
    case WM_LBUTTONDOWN:  /* Initialise mouse tracking */
      /* If we are already in the 'clicked' mode, then nothing to do */
      if (infoPtr->Flags & FLAG_CLICKED) return;

      /* If the buddy is an edit, will set focus to it */
      if (!lstrcmpiA(infoPtr->szBuddyClass,"Edit")) SetFocus(infoPtr->Buddy);

      /* Now see which one is the 'active' arrow */
      temp = UPDOWN_GetArrowFromPoint(hwnd,&rect,pt);

      /* Update the CurVal if necessary */
      if (dwStyle & UDS_SETBUDDYINT) UPDOWN_GetBuddyInt(hwnd);

      /* Before we proceed, see if we can spin... */
      if (!(dwStyle & UDS_WRAP))
        if ((temp && infoPtr->CurVal == infoPtr->MaxVal) ||
           (!temp && infoPtr->CurVal == infoPtr->MinVal))
          return;

      /* Set up the correct flags */
      infoPtr->Flags  = 0;
      infoPtr->Flags |= temp ? FLAG_INCR : FLAG_DECR;
      infoPtr->Flags |= FLAG_MOUSEIN;

      /* repaint the control */
      UPDOWN_Refresh (hwnd);

      /* process the click */
      UPDOWN_DoAction (hwnd,1,infoPtr->Flags & FLAG_INCR);

      /* now capture all mouse messages */
      SetCapture (hwnd);

      /* and startup the first timer */
      SetTimer(hwnd,TIMERID1,INITIAL_DELAY,0);
      break;

    case WM_MOUSEMOVE:
      /* If we are not in the 'clicked' mode, then nothing to do */
      if (!(infoPtr->Flags & FLAG_CLICKED)) return;

      /* save the flags to see if any got modified */
      temp = infoPtr->Flags;

      /* Now get the 'active' arrow rectangle */
      if (infoPtr->Flags & FLAG_INCR)
        UPDOWN_GetArrowRect (hwnd, &rect, TRUE);
      else
        UPDOWN_GetArrowRect (hwnd, &rect, FALSE);

      /* Update the flags if we are in/out */
      if(PtInRect(&rect, pt))
        infoPtr->Flags |=  FLAG_MOUSEIN;
      else{
        infoPtr->Flags &= ~FLAG_MOUSEIN;
        if(accelIndex != -1) /* if we have accel info */
          accelIndex = 0;    /* reset it              */
      }
      /* If state changed, redraw the control */
      if(temp != infoPtr->Flags)
        UPDOWN_Refresh (hwnd);
      break;

      default:
//      ERR(updown, "Impossible case!\n");
        break;
    }

}

//Message handling

static LRESULT UPDOWN_NCCreate(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  /* get rid of border, if any */
  SetWindowLongA(hwnd,GWL_STYLE,dwStyle & ~WS_BORDER);
  return TRUE;
}

static LRESULT UPDOWN_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr;
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  infoPtr = (UPDOWN_INFO*)initControl(hwnd,sizeof(UPDOWN_INFO));

  /* initialize the info struct */
  infoPtr->AccelCount = 0;
  infoPtr->AccelVect = 0;
  infoPtr->CurVal = 0;
  infoPtr->MinVal = 0;
  infoPtr->MaxVal = 100; /*FIXME*/
  infoPtr->Base  = 10; /* Default to base 10  */
  infoPtr->Buddy = 0;  /* No buddy window yet */
  infoPtr->Flags = 0;  /* And no flags        */

  /* Do we pick the buddy win ourselves? */
  if (dwStyle & UDS_AUTOBUDDY) UPDOWN_SetBuddyHandle(hwnd,GetWindow(hwnd,GW_HWNDPREV));

//      TRACE(updown, "UpDown Ctrl creation, hwnd=%04x\n", hwnd);

  return 0;
}

static LRESULT UPDOWN_Destroy(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  if(infoPtr->AccelVect) COMCTL32_Free(infoPtr->AccelVect);

  if ( IsWindow(infoPtr->Buddy) ) /* Cleanup */
        RemovePropA(infoPtr->Buddy, BUDDY_UPDOWN_HWND);

  doneControl(hwnd);

  return 0;
}

static LRESULT UPDOWN_Enable(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if (dwStyle & WS_DISABLED) UPDOWN_CancelMode(hwnd);

  UPDOWN_Refresh(hwnd);

  return 0;
}

static LRESULT UPDOWN_Timer(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  int temp;

  /* if initial timer, kill it and start the repeat timer */
  if(wParam == TIMERID1)
  {
    KillTimer(hwnd, TIMERID1);
    /* if no accel info given, used default timer */
    if(infoPtr->AccelCount==0 || infoPtr->AccelVect==0)
    {
      accelIndex = -1;
      temp = REPEAT_DELAY;
    } else
    {
      accelIndex = 0; /* otherwise, use it */
      temp = infoPtr->AccelVect[accelIndex].nSec * 1000 + 1;
    }
    SetTimer(hwnd, TIMERID2, temp, 0);
   }

   /* now, if the mouse is above us, do the thing...*/
   if(infoPtr->Flags & FLAG_MOUSEIN)
   {
     temp = accelIndex==-1 ? 1 : infoPtr->AccelVect[accelIndex].nInc;
     UPDOWN_DoAction(hwnd, temp, infoPtr->Flags & FLAG_INCR);

     if(accelIndex!=-1 && accelIndex < infoPtr->AccelCount-1)
     {
       KillTimer(hwnd, TIMERID2);
       accelIndex++; /* move to the next accel info */
       temp = infoPtr->AccelVect[accelIndex].nSec * 1000 + 1;
       /* make sure we have at least 1ms intervals */
       SetTimer(hwnd, TIMERID2, temp, 0);
     }
   }

   return 0;
}

static LRESULT UPDOWN_LButtonUp(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  if (!UPDOWN_CancelMode(hwnd)) return 0;
   /*If we released the mouse and our buddy is an edit */
   /* we must select all text in it.                   */
   if (!lstrcmpiA(infoPtr->szBuddyClass,"Edit"))
       SendMessageA(infoPtr->Buddy,EM_SETSEL,0,MAKELONG(0,-1));

   return 0;
}

static LRESULT UPDOWN_KeyDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if((dwStyle & UDS_ARROWKEYS) && UPDOWN_IsEnabled(hwnd))
  {
    switch(wParam)
    {
      case VK_UP:
      case VK_DOWN:
        UPDOWN_GetBuddyInt (hwnd);
        UPDOWN_DoAction (hwnd, 1, wParam==VK_UP);
        break;
    }
  }

  return 0;
}

static LRESULT UPDOWN_GetAccel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  INT temp;

  if (wParam == 0 && lParam == 0)    /*if both zero, */
     return infoPtr->AccelCount;  /*just return the accel count*/
  if (wParam || lParam)
  {
    UNKNOWN_PARAM(UDM_GETACCEL,wParam,lParam);
    return 0;
  }
  temp = MIN(infoPtr->AccelCount,wParam);
  memcpy((void *)lParam,infoPtr->AccelVect,temp*sizeof(UDACCEL));

  return temp;
}

static LRESULT UPDOWN_SetAccel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  //      TRACE(updown, "UpDown Ctrl new accel info, hwnd=%04x\n", hwnd);
  if(infoPtr->AccelVect)
  {
    COMCTL32_Free(infoPtr->AccelVect);
    infoPtr->AccelCount = 0;
    infoPtr->AccelVect  = 0;
  }
  if(wParam == 0) return TRUE;
  infoPtr->AccelVect = (UDACCEL*)COMCTL32_Alloc(wParam*sizeof(UDACCEL));
  if(infoPtr->AccelVect == 0) return FALSE;
  memcpy(infoPtr->AccelVect,(void*)lParam,wParam*sizeof(UDACCEL));

  return TRUE;
}

static LRESULT UPDOWN_GetBase(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  if (wParam || lParam) UNKNOWN_PARAM(UDM_GETBASE,wParam,lParam);

  return infoPtr->Base;
}

static LRESULT UPDOWN_SetBase(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  INT temp;

  //      TRACE(updown, "UpDown Ctrl new base(%d), hwnd=%04x\n",
  //                   wParam, hwnd);
  if (!(wParam==10 || wParam==16) || lParam) UNKNOWN_PARAM(UDM_SETBASE,wParam,lParam);
  if (wParam==10 || wParam==16)
  {
    UPDOWN_GetBuddyInt(hwnd);

    temp = infoPtr->Base;
    infoPtr->Base = wParam;

    UPDOWN_SetBuddyInt(hwnd);

    return temp;       /* return the prev base */
  }

  return 0;
}

static LRESULT UPDOWN_GetBuddy(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  if (wParam || lParam) UNKNOWN_PARAM(UDM_GETBUDDY,wParam,lParam);

  return infoPtr->Buddy;
}


static LRESULT UPDOWN_SetBuddy(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  INT temp;

  if (lParam) UNKNOWN_PARAM(UDM_SETBUDDY,wParam,lParam);
  temp = infoPtr->Buddy;
  UPDOWN_SetBuddyHandle(hwnd,wParam);
//      TRACE(updown, "UpDown Ctrl new buddy(%04x), hwnd=%04x\n",
//                   infoPtr->Buddy, hwnd);

  return temp;
}

static LRESULT UPDOWN_GetPos(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  INT temp;

  if (wParam || lParam) UNKNOWN_PARAM(UDM_GETPOS, wParam, lParam);
  temp = UPDOWN_GetBuddyInt(hwnd);

  return MAKELONG(infoPtr->CurVal,temp ? 0:1);
}

static LRESULT UPDOWN_SetPos(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);
  INT temp;

  if (wParam || HIWORD(lParam)) UNKNOWN_PARAM(UDM_GETPOS,wParam,lParam);
  temp = SLOWORD(lParam);
//      TRACE(updown, "UpDown Ctrl new value(%d), hwnd=%04x\n",
//                   temp, hwnd);
  if (!UPDOWN_InBounds(hwnd, temp))
  {
    if (temp < infoPtr->MinVal) temp = infoPtr->MinVal;
    if (temp > infoPtr->MaxVal) temp = infoPtr->MaxVal;
  }
  wParam = infoPtr->CurVal; /* save prev value   */
  infoPtr->CurVal = temp;   /* set the new value */
  if (dwStyle & UDS_SETBUDDYINT) UPDOWN_SetBuddyInt (hwnd);

  return wParam;            /* return prev value */
}

static LRESULT UPDOWN_GetRange(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  if (wParam || lParam) UNKNOWN_PARAM(UDM_GETRANGE, wParam, lParam);

  return MAKELONG(infoPtr->MaxVal,infoPtr->MinVal);
}

static LRESULT UPDOWN_SetRange(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  if (wParam) UNKNOWN_PARAM(UDM_SETRANGE,wParam,lParam); /* we must have:     */
  infoPtr->MaxVal = SLOWORD(lParam); /* UD_MINVAL <= Max <= UD_MAXVAL */
  infoPtr->MinVal = SHIWORD(lParam); /* UD_MINVAL <= Min <= UD_MAXVAL */
                                     /* |Max-Min| <= UD_MAXVAL        */
//      TRACE(updown, "UpDown Ctrl new range(%d to %d), hwnd=%04x\n",
//                   infoPtr->MinVal, infoPtr->MaxVal, hwnd);

  return 0;
}

static LRESULT UPDOWN_GetRange32(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  if (wParam) *(LPINT)wParam = infoPtr->MinVal;
  if (lParam) *(LPINT)lParam = infoPtr->MaxVal;

  return 0;
}

static LRESULT UPDOWN_SetRange32(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(hwnd);

  infoPtr->MinVal = (INT)wParam;
  infoPtr->MaxVal = (INT)lParam;
  if (infoPtr->MaxVal <= infoPtr->MinVal) infoPtr->MaxVal = infoPtr->MinVal+1;
//      TRACE(updown, "UpDown Ctrl new range(%d to %d), hwnd=%04x\n",
//                   infoPtr->MinVal, infoPtr->MaxVal, hwnd);

  return 0;
}

/***********************************************************************
 *           UpDownWndProc
 */
static LRESULT WINAPI UpDownWindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                LPARAM lParam)
{
  switch(message)
    {
    case WM_NCCREATE:
      return UPDOWN_NCCreate(hwnd,wParam,lParam);

    case WM_CREATE:
      return UPDOWN_Create(hwnd,wParam,lParam);

    case WM_DESTROY:
      return UPDOWN_Destroy(hwnd,wParam,lParam);

    case WM_ENABLE:
      return UPDOWN_Enable(hwnd,wParam,lParam);

    case WM_TIMER:
      return UPDOWN_Timer(hwnd,wParam,lParam);

    case WM_CANCELMODE:
      UPDOWN_CancelMode(hwnd);
      break;

    case WM_LBUTTONUP:
      return UPDOWN_LButtonUp(hwnd,wParam,lParam);

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
      if(UPDOWN_IsEnabled(hwnd))
      {
        POINT pt;
        CONV_POINT16TO32((POINT16 *)&lParam,&pt);
        UPDOWN_HandleMouseEvent(hwnd,message,pt);
      }
      break;

    case WM_KEYDOWN:
      return UPDOWN_KeyDown(hwnd,wParam,lParam);

    case WM_PAINT:
      UPDOWN_Paint(hwnd,(HDC)wParam);
      break;

    case UDM_GETACCEL:
      return UPDOWN_GetAccel(hwnd,wParam,lParam);

    case UDM_SETACCEL:
      return UPDOWN_SetAccel(hwnd,wParam,lParam);

    case UDM_GETBASE:
      return UPDOWN_GetBase(hwnd,wParam,lParam);

    case UDM_SETBASE:
      return UPDOWN_SetBase(hwnd,wParam,lParam);

    case UDM_GETBUDDY:
      return UPDOWN_GetBuddy(hwnd,wParam,lParam);

    case UDM_SETBUDDY:
      return UPDOWN_SetBuddy(hwnd,wParam,lParam);

    case UDM_GETPOS:
      return UPDOWN_GetPos(hwnd,wParam,lParam);

    case UDM_SETPOS:
      return UPDOWN_SetPos(hwnd,wParam,lParam);

    case UDM_GETRANGE:
      return UPDOWN_GetRange(hwnd,wParam,lParam);

    case UDM_SETRANGE:
      return UPDOWN_SetRange(hwnd,wParam,lParam);

    case UDM_GETRANGE32:
      return UPDOWN_GetRange32(hwnd,wParam,lParam);

    case UDM_SETRANGE32:
      return UPDOWN_SetRange32(hwnd,wParam,lParam);

    default:
//      if (message >= WM_USER)
//      ERR (updown, "unknown msg %04x wp=%04x lp=%08lx\n",
//           message, wParam, lParam);
      return defComCtl32ProcA(hwnd,message,wParam,lParam);
    }

    return 0;
}

/***********************************************************************
 * UPDOWN_Buddy_SubclassProc used to handle messages sent to the buddy
 *                           control.
 */
LRESULT CALLBACK
UPDOWN_Buddy_SubclassProc (
  HWND   hwnd,
  UINT   uMsg,
  WPARAM wParam,
  LPARAM lParam)
{
  LONG superClassWndProc = GetPropA(hwnd, BUDDY_SUPERCLASS_WNDPROC);
  //TRACE("hwnd=%04x, wndProc=%d, uMsg=%04x, wParam=%d, lParam=%d\n",
  // hwnd, (INT)superClassWndProc, uMsg, wParam, (UINT)lParam);

  switch (uMsg)
  {
    case WM_KEYDOWN:
    {
      if ( ((int)wParam == VK_UP ) || ((int)wParam == VK_DOWN ) )
      {
        HWND upDownHwnd      = GetPropA(hwnd, BUDDY_UPDOWN_HWND);
        UPDOWN_INFO *infoPtr = UPDOWN_GetInfoPtr(upDownHwnd);

        if (!lstrcmpA (infoPtr->szBuddyClass, "ListBox"))
        {
          /* if the buddy is a list window, we must update curr index */
          INT oldVal = SendMessageA(hwnd, LB_GETCURSEL, 0, 0);
          SendMessageA(hwnd, LB_SETCURSEL, oldVal+1, 0);
        }
        else
        {
          UPDOWN_GetBuddyInt(upDownHwnd);
          UPDOWN_DoAction(upDownHwnd, 1, wParam==VK_UP);
        }

        break;
      }
      /* else Fall Through */
    }

    default:
      return CallWindowProcA( (WNDPROC)superClassWndProc, hwnd, uMsg, wParam, lParam);
  }

  return 0;
}

/***********************************************************************
 *              UPDOWN_Register [Internal]
 *
 * Registers the updown window class.
 */

VOID
UPDOWN_Register(void)
{
    WNDCLASSA wndClass;

    ZeroMemory( &wndClass, sizeof( WNDCLASSA ) );
    wndClass.style         = CS_GLOBALCLASS | CS_VREDRAW;
    wndClass.lpfnWndProc   = (WNDPROC)UpDownWindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(UPDOWN_INFO*);
    wndClass.hCursor       = LoadCursorA( 0, IDC_ARROWA );
    wndClass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wndClass.lpszClassName = UPDOWN_CLASSA;

    RegisterClassA( &wndClass );
}


/***********************************************************************
 *              UPDOWN_Unregister       [Internal]
 *
 * Unregisters the updown window class.
 */

VOID
UPDOWN_Unregister (VOID)
{
    UnregisterClassA (UPDOWN_CLASSA, (HINSTANCE)NULL);
}

