/* $Id: progress.c,v 1.12 2000-01-26 18:04:29 cbratschi Exp $ */
/*
 * Progress control
 *
 * Copyright 1997 Dimitrie O. Paun
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 *
 * Status: complete
 * Version: 5.00
 */

#include "winbase.h"
#include "commctrl.h"
#include "progress.h"
#include "comctl32.h"


/* Control configuration constants */

#define LED_GAP      2
#define BORDER_WIDTH 1

/* Work constants */

//#define UNKNOWN_PARAM(msg, wParam, lParam) WARN(progress, \
//        "Unknown parameter(s) for message " #msg     \
//      "(%04x): wp=%04x lp=%08lx\n", msg, wParam, lParam);
//#define UNKNOWN_PARAM(msg, wParam, lParam)

#define PROGRESS_GetInfoPtr(hwnd) ((PROGRESS_INFO *)GetWindowLongA(hwnd,0))


/***********************************************************************
 * PROGRESS_Draw
 * Draws the progress bar.
 */
static void
PROGRESS_Draw (HWND hwnd, HDC hdc, INT lastVal, BOOL inUpdate)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  HBRUSH hbrBar,hbrBk,hbrLight,hbrShadow,hbrOld;
  int rightBar, rightMost, ledWidth;
  int lastBar;
  RECT rect;
  DWORD dwStyle;

//  TRACE(progress, "refresh pos=%d min=%d, max=%d\n",
//             infoPtr->CurVal, infoPtr->MinVal, infoPtr->MaxVal);

  if (infoPtr->MinVal == infoPtr->MaxVal) return; //Prevent division through 0

  /* get the required bar brush */
  if (infoPtr->ColorBar == CLR_DEFAULT) hbrBar = GetSysColorBrush(COLOR_HIGHLIGHT);
  else hbrBar = CreateSolidBrush (infoPtr->ColorBar);

  /* get the required background brush */
  if (infoPtr->ColorBk == CLR_DEFAULT) hbrBk = GetSysColorBrush(COLOR_3DFACE);
  else hbrBk = CreateSolidBrush(infoPtr->ColorBk);

  /* get client rectangle */
  GetClientRect (hwnd, &rect);

  /* draw the background */
  if (!inUpdate) FillRect(hdc, &rect, hbrBk);

  rect.left += BORDER_WIDTH;
  rect.right -= BORDER_WIDTH;
  rect.top += BORDER_WIDTH;
  rect.bottom -= BORDER_WIDTH;

  /* get the window style */
  dwStyle = GetWindowLongA(hwnd, GWL_STYLE);

  /* compute extent of progress bar */
  if (dwStyle & PBS_VERTICAL)
  {
    rightBar = rect.bottom-MulDiv(infoPtr->CurVal-infoPtr->MinVal,rect.bottom-rect.top,infoPtr->MaxVal-infoPtr->MinVal);
    if (inUpdate) lastBar = rect.bottom-MulDiv(lastVal-infoPtr->MinVal,rect.bottom-rect.top,infoPtr->MaxVal-infoPtr->MinVal);
    ledWidth = MulDiv ((rect.right - rect.left), 2, 3);
    rightMost = rect.top;
  }
  else
  {
    rightBar = rect.left+MulDiv(infoPtr->CurVal-infoPtr->MinVal,rect.right-rect.left,infoPtr->MaxVal-infoPtr->MinVal);
    if (inUpdate) lastBar = rect.left+MulDiv(lastVal-infoPtr->MinVal,rect.right-rect.left,infoPtr->MaxVal-infoPtr->MinVal);
    ledWidth = MulDiv ((rect.bottom - rect.top), 2, 3);
    rightMost = rect.right;
  }

  /* now draw the bar */
  if (dwStyle & PBS_SMOOTH)
  {
    if (dwStyle & PBS_VERTICAL)
    {
      if (inUpdate)
      {
        if (infoPtr->CurVal > lastVal)
        {
          rect.top = rightBar;
          rect.bottom = lastBar;
          FillRect(hdc,&rect,hbrBar);
        } else if (infoPtr->CurVal < lastVal)
        {
          rect.top = lastBar;
          rect.bottom = rightBar;
          FillRect(hdc,&rect,hbrBk);
        }
      } else
      {
        rect.top = rightBar;
        FillRect(hdc,&rect,hbrBar);
      }
    } else //Horizontal
    {
      if (inUpdate)
      {
        if (infoPtr->CurVal > lastVal)
        {
          rect.left = lastBar;
          rect.right = rightBar;
          FillRect(hdc,&rect,hbrBar);
        } else if (infoPtr->CurVal < lastVal)
        {
          rect.left = rightBar;
          rect.right = lastBar;
          FillRect(hdc,&rect,hbrBk);
        }
      } else
      {
        rect.right = rightBar;
        FillRect(hdc,&rect,hbrBar);
      }
    }
  } else
  {
    if (dwStyle & PBS_VERTICAL)
    {
      if (inUpdate)
      {
        if (infoPtr->CurVal > lastVal)
        {
          rect.bottom -= ((int)(rect.bottom-lastBar)/(ledWidth+LED_GAP))*(ledWidth+LED_GAP);
          while(rect.bottom > rightBar)
          {
            rect.top = rect.bottom-ledWidth;
            if (rect.top < rightMost) rect.top = rightMost;
            FillRect(hdc,&rect,hbrBar);
            rect.bottom = rect.top-LED_GAP;
          }
        } else if (infoPtr->CurVal < lastVal)
        {
          rect.top = rect.bottom-((int)(rect.bottom-lastBar)/(ledWidth+LED_GAP))*(ledWidth+LED_GAP)-ledWidth;
          if (rect.top < rightMost) rect.top = rightMost;
          rect.bottom -= ((int)(rect.bottom-rightBar)/(ledWidth+LED_GAP))*(ledWidth+LED_GAP);
          FillRect(hdc,&rect,hbrBk);
        }
      } else
      {
        while(rect.bottom > rightBar)
        {
          rect.top = rect.bottom-ledWidth;
          if (rect.top < rightMost) rect.top = rightMost;
          FillRect(hdc,&rect,hbrBar);
          rect.bottom = rect.top-LED_GAP;
        }
      }
    } else //Horizontal
    {
      if (inUpdate)
      {
        if (infoPtr->CurVal > lastVal)
        {
          rect.left += ((int)(lastBar-rect.left)/(ledWidth+LED_GAP))*(ledWidth+LED_GAP);
          while(rect.left < rightBar)
          {
            rect.right = rect.left+ledWidth;
            if (rect.right > rightMost) rect.right = rightMost;
            FillRect(hdc,&rect,hbrBar);
            rect.left  = rect.right+LED_GAP;
          }
        } else if (infoPtr->CurVal < lastVal)
        {
          rect.right = rect.left+((int)(lastBar-rect.left)/(ledWidth+LED_GAP))*(ledWidth+LED_GAP)+ledWidth;
          if (rect.right > rightMost) rect.right = rightMost;
          rect.left += ((int)(rightBar-rect.left)/(ledWidth+LED_GAP))*(ledWidth+LED_GAP);
          FillRect(hdc,&rect,hbrBk);
        }
      } else
      {
        while(rect.left < rightBar)
        {
          rect.right = rect.left+ledWidth;
          if (rect.right > rightMost) rect.right = rightMost;
          FillRect(hdc,&rect,hbrBar);
          rect.left  = rect.right+LED_GAP;
        }
      }
  }
  }

  /* delete bar brush */
  if (infoPtr->ColorBar != CLR_DEFAULT) DeleteObject (hbrBar);

  /* delete background brush */
  if (infoPtr->ColorBk != CLR_DEFAULT) DeleteObject (hbrBk);
}

/***********************************************************************
 * PROGRESS_Update (prototype, todo)
 * Updates only the changed pixels -> faster, no flickering
 */
static void PROGRESS_Update(HWND hwnd,INT lastVal)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  HDC hdc;

  if (lastVal != infoPtr->CurVal) //Only update if really necessary
  {
   hdc = GetDC(hwnd);
   PROGRESS_Draw(hwnd,hdc,lastVal,TRUE);
   ReleaseDC(hwnd,hdc);
  }
}

/***********************************************************************
 * PROGRESS_Refresh
 * Draw the progress bar. The background need not be erased.
 */
static void
PROGRESS_Refresh (HWND hwnd)
{
    HDC hdc;

    hdc = GetDC (hwnd);
    PROGRESS_Draw (hwnd, hdc, 0, FALSE);
    ReleaseDC (hwnd, hdc);
}

/***********************************************************************
 * PROGRESS_Paint
 * Draw the progress bar. The background need not be erased.
 * If dc!=0, it draws on it
 */
static void
PROGRESS_Paint (HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint (hwnd, &ps);
    PROGRESS_Draw (hwnd, hdc, 0, FALSE);
    EndPaint (hwnd, &ps);
}


/***********************************************************************
 *           PROGRESS_CoercePos
 * Makes sure the current position (CUrVal) is within bounds.
 */
static void PROGRESS_CoercePos(HWND hwnd)
{
    PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);

  if(infoPtr->CurVal < infoPtr->MinVal)
    infoPtr->CurVal = infoPtr->MinVal;
  if(infoPtr->CurVal > infoPtr->MaxVal)
    infoPtr->CurVal = infoPtr->MaxVal;
}


static LRESULT PROGRESS_NCCreate(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwExStyle;

  dwExStyle = GetWindowLongA(hwnd,GWL_EXSTYLE);
  SetWindowLongA(hwnd,GWL_EXSTYLE,dwExStyle | WS_EX_STATICEDGE);

  return TRUE;
}

static LRESULT PROGRESS_Create(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr;

  /* allocate memory for info struct */
  infoPtr = (PROGRESS_INFO *)COMCTL32_Alloc(sizeof(PROGRESS_INFO));
  SetWindowLongA(hwnd,0,(DWORD)infoPtr);

  /* initialize the info struct */
  infoPtr->MinVal = 0;
  infoPtr->MaxVal = 100;
  infoPtr->CurVal = 0;
  infoPtr->Step = 10;
  infoPtr->ColorBar = CLR_DEFAULT;
  infoPtr->ColorBk = CLR_DEFAULT;
  infoPtr->hFont = (HANDLE)NULL;
//      TRACE(progress, "Progress Ctrl creation, hwnd=%04x\n", hwnd);

  return 0;
}

static LRESULT PROGRESS_Destroy(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);

  //      TRACE (progress, "Progress Ctrl destruction, hwnd=%04x\n", hwnd);
  COMCTL32_Free (infoPtr);

  return 0;
}

static LRESULT PROGRESS_GetFont(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);

  return (LRESULT)infoPtr->hFont;
}

/***********************************************************************
 *           PROGRESS_SetFont
 * Set new Font for progress bar
 */
static HFONT PROGRESS_SetFont (HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  HFONT hOldFont = infoPtr->hFont;

  infoPtr->hFont = (HFONT)wParam;
  if (LOWORD(lParam)) PROGRESS_Refresh (hwnd);

  return hOldFont;
}

static LRESULT PROGRESS_DeltaPos(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  INT temp;

  //if(lParam) UNKNOWN_PARAM(PBM_DELTAPOS, wParam, lParam);
  temp = infoPtr->CurVal;
  if (wParam != 0)
  {
    infoPtr->CurVal += (INT)wParam;
    PROGRESS_CoercePos(hwnd);
    PROGRESS_Update(hwnd,temp);
  }

  return temp;
}

static LRESULT PROGRESS_SetPos(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  INT temp;

  //if (lParam) UNKNOWN_PARAM(PBM_SETPOS, wParam, lParam);
  temp = infoPtr->CurVal;
  if (temp != wParam)
  {
    infoPtr->CurVal = (UINT16)wParam; //CB: 0..65535 allowed
    PROGRESS_CoercePos(hwnd);
    PROGRESS_Update(hwnd,temp);
  }

  return temp;
}

static LRESULT PROGRESS_SetRange(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  INT temp;

  //if (wParam) UNKNOWN_PARAM(PBM_SETRANGE, wParam, lParam);
  temp = MAKELONG(infoPtr->MinVal,infoPtr->MaxVal);
  if (temp != lParam)
  {
    infoPtr->MinVal = LOWORD(lParam);
    infoPtr->MaxVal = HIWORD(lParam);
    if (infoPtr->MaxVal <= infoPtr->MinVal) infoPtr->MaxVal = infoPtr->MinVal+1;
    PROGRESS_CoercePos(hwnd);
    PROGRESS_Refresh(hwnd);
  }

  return temp;
}

static LRESULT PROGRESS_SetStep(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  INT temp;

  //if (lParam) UNKNOWN_PARAM(PBM_SETSTEP, wParam, lParam);
  temp = infoPtr->Step;
  infoPtr->Step = (INT)wParam; //CB: negative steps allowed

  return temp;
}

static LRESULT PROGRESS_StepIt(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  INT temp;

  //if (wParam || lParam) UNKNOWN_PARAM(PBM_STEPIT, wParam, lParam);
  temp = infoPtr->CurVal;
  infoPtr->CurVal += infoPtr->Step;
  if(infoPtr->CurVal > infoPtr->MaxVal) infoPtr->CurVal = infoPtr->MinVal;
  if(temp != infoPtr->CurVal) PROGRESS_Update (hwnd,temp);

  return temp;
}

static LRESULT PROGRESS_SetRange32(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  INT temp;

  temp = MAKELONG(infoPtr->MinVal,infoPtr->MaxVal);
  if((infoPtr->MinVal != (INT)wParam) || (infoPtr->MaxVal != (INT)lParam))
  {
    infoPtr->MinVal = (INT)wParam;
    infoPtr->MaxVal = (INT)lParam;
    if(infoPtr->MaxVal <= infoPtr->MinVal) infoPtr->MaxVal = infoPtr->MinVal+1;
    PROGRESS_CoercePos(hwnd);
    PROGRESS_Refresh(hwnd);
  }

  return temp;
}

static LRESULT PROGRESS_GetRange(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);

  if (lParam)
  {
    ((PPBRANGE)lParam)->iLow = infoPtr->MinVal;
    ((PPBRANGE)lParam)->iHigh = infoPtr->MaxVal;
  }

  return (wParam) ? infoPtr->MinVal : infoPtr->MaxVal;
}

static LRESULT PROGRESS_GetPos(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);

  //if (wParam || lParam) UNKNOWN_PARAM(PBM_STEPIT, wParam, lParam);

  return (infoPtr->CurVal);
}

static LRESULT PROGRESS_SetBarColor(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  COLORREF oldColorBar = infoPtr->ColorBar;

  //if (wParam) UNKNOWN_PARAM(PBM_SETBARCOLOR, wParam, lParam);
  infoPtr->ColorBar = (COLORREF)lParam;
  if (infoPtr->ColorBar != oldColorBar) PROGRESS_Refresh(hwnd);

  return 0;
}

static LRESULT PROGRESS_SetBkColor(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  PROGRESS_INFO *infoPtr = PROGRESS_GetInfoPtr(hwnd);
  COLORREF oldColorBk = infoPtr->ColorBk;

  //if (wParam) UNKNOWN_PARAM(PBM_SETBKCOLOR, wParam, lParam);
  infoPtr->ColorBk = (COLORREF)lParam;
  if (infoPtr->ColorBk != oldColorBk) PROGRESS_Refresh (hwnd);

  return 0;
}

/***********************************************************************
 *           ProgressWindowProc
 */
static LRESULT WINAPI ProgressWindowProc(HWND hwnd, UINT message,
                                  WPARAM wParam, LPARAM lParam)
{
  switch(message)
    {
    case WM_NCCREATE:
      return PROGRESS_NCCreate(hwnd,wParam,lParam);

    case WM_CREATE:
      return PROGRESS_Create(hwnd,wParam,lParam);

    case WM_DESTROY:
      return PROGRESS_Destroy(hwnd,wParam,lParam);

    case WM_ERASEBKGND:
      /* pretend to erase it here, but we will do it in the paint
         function to avoid flicker */
      return 1;

    case WM_GETFONT:
      return PROGRESS_GetFont(hwnd,wParam,lParam);

    case WM_SETFONT:
      return PROGRESS_SetFont (hwnd,wParam,lParam);

    case WM_PAINT:
      PROGRESS_Paint(hwnd);
      break;

    case PBM_DELTAPOS:
      return PROGRESS_DeltaPos(hwnd,wParam,lParam);

    case PBM_SETPOS:
      return PROGRESS_SetPos(hwnd,wParam,lParam);

    case PBM_SETRANGE:
      return PROGRESS_SetRange(hwnd,wParam,lParam);

    case PBM_SETSTEP:
      return PROGRESS_SetStep(hwnd,wParam,lParam);

    case PBM_STEPIT:
      return PROGRESS_StepIt(hwnd,wParam,lParam);

    case PBM_SETRANGE32:
      return PROGRESS_SetRange32(hwnd,wParam,lParam);

    case PBM_GETRANGE:
      return PROGRESS_GetRange(hwnd,wParam,lParam);

    case PBM_GETPOS:
      return PROGRESS_GetPos(hwnd,wParam,lParam);

    case PBM_SETBARCOLOR:
      return PROGRESS_SetBarColor(hwnd,wParam,lParam);

    case PBM_SETBKCOLOR:
      return PROGRESS_SetBkColor(hwnd,wParam,lParam);

    default:
//      if (message >= WM_USER)
//      ERR(progress, "unknown msg %04x wp=%04x lp=%08lx\n",
//                  message, wParam, lParam );
      return DefWindowProcA( hwnd, message, wParam, lParam );
    }

    return 0;
}


/***********************************************************************
 * PROGRESS_Register [Internal]
 *
 * Registers the progress bar window class.
 */

VOID
PROGRESS_Register (VOID)
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(PROGRESS_CLASSA)) return;

    ZeroMemory (&wndClass, sizeof( WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_VREDRAW | CS_HREDRAW;
    wndClass.lpfnWndProc   = (WNDPROC)ProgressWindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof (PROGRESS_INFO *);
    wndClass.hCursor       = LoadCursorA(0, IDC_ARROWA);
    wndClass.lpszClassName = PROGRESS_CLASSA;

    RegisterClassA(&wndClass);
}


/***********************************************************************
 * PROGRESS_Unregister [Internal]
 *
 * Unregisters the progress bar window class.
 */

VOID
PROGRESS_Unregister (VOID)
{
    if (GlobalFindAtomA(PROGRESS_CLASSA))
        UnregisterClassA(PROGRESS_CLASSA, (HINSTANCE)NULL);
}

