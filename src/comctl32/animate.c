/* $Id: animate.c,v 1.7 2000-02-22 17:11:38 cbratschi Exp $ */
/*
 * Animation control
 *
 * Copyright 1998, 1999 Eric Kohl
 * Copyright 1999 Achim Hasenmueller
 * Copyright 1999 Christoph Bratschi
 *
 * Status: can't play videos through MCI -> WINMM stubs
 * Version: 5.00
 */


#include "winbase.h"
#include "commctrl.h"
#include "comctl32.h"
#include "animate.h"


#define ANIMATE_GetInfoPtr(hwnd) ((ANIMATE_INFO*)GetWindowLongA(hwnd,0))
#define TIMER_ID 1

static BOOL
ANIMATE_LoadRes(ANIMATE_INFO *infoPtr,HINSTANCE hInst,LPSTR lpName,BOOL unicode)
{
    HRSRC hrsrc;
    HGLOBAL handle;
    LPVOID lpAvi;

    if (unicode)
    {
      WCHAR buf[4];

      lstrcpyAtoW(buf,"AVI");
      hrsrc = FindResourceW(hInst,(LPWSTR)lpName,buf);
    } else hrsrc = FindResourceA(hInst,lpName,"AVI");
    if (!hrsrc) return FALSE;

    handle = LoadResource (hInst, hrsrc);
    if (!handle) return FALSE;

    lpAvi = LockResource (handle);
    if (!lpAvi) return FALSE;

    infoPtr->lpAvi = lpAvi;
    infoPtr->hRes = handle;
    infoPtr->cSize = SizeofResource(hInst,hrsrc);

    return TRUE;
}

static BOOL
ANIMATE_LoadFile(ANIMATE_INFO *infoPtr,LPSTR lpName,BOOL unicode)
{
    HANDLE handle;
    HFILE hFile;
    LPVOID lpAvi;

    if (unicode)
    {
      hFile = CreateFileW((LPWSTR)lpName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
      if (!hFile) return FALSE;

      handle = CreateFileMappingW(hFile,NULL,PAGE_READONLY | SEC_COMMIT,0,0,NULL);
    } else
    {
      hFile = CreateFileA (lpName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
      if (!hFile) return FALSE;

      handle = CreateFileMappingA(hFile,NULL,PAGE_READONLY | SEC_COMMIT,0,0,NULL);
    }
    if (!handle)
    {
      CloseHandle (hFile);
      return FALSE;
    }

    lpAvi = MapViewOfFile (handle, FILE_MAP_READ, 0, 0, 0);
    if (!lpAvi)
    {
      CloseHandle (hFile);
      return FALSE;
    }

    infoPtr->lpAvi = lpAvi;
    infoPtr->hFile = hFile;
    infoPtr->cSize = GetFileSize(hFile,NULL);

    return TRUE;
}

static VOID
ANIMATE_GetAviInfo (ANIMATE_INFO *infoPtr)
{


}

static VOID
ANIMATE_FreeResource(ANIMATE_INFO *infoPtr)
{
  if (infoPtr->hFile)
  {
    UnmapViewOfFile (infoPtr->lpAvi);
    CloseHandle (infoPtr->hFile);
    infoPtr->lpAvi = NULL;
    infoPtr->hFile = NULL;
  } else if (infoPtr->hRes)
  {
    GlobalFree (infoPtr->hRes);
    infoPtr->lpAvi = NULL;
    infoPtr->hRes = NULL;
  }
  infoPtr->cSize = 0;
}

static BOOL
ANIMATE_IsLoaded(ANIMATE_INFO *infoPtr)
{
  return (infoPtr->lpAvi != NULL);
}

static BOOL
ANIMATE_IsPlaying(ANIMATE_INFO *infoPtr)
{
  return infoPtr->bPlaying;
}

/* Message handlers */

static LRESULT
ANIMATE_Close(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hwnd);

  ANIMATE_FreeResource(infoPtr);

  return DefWindowProcA(hwnd,WM_CLOSE,wParam,lParam);
}

static LRESULT
ANIMATE_Destroy (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hwnd);

    /* free avi data */
    ANIMATE_FreeResource(infoPtr);

    /* free animate info data */
    COMCTL32_Free(infoPtr);

    return DefWindowProcA(hwnd,WM_DESTROY,wParam,lParam);
}

static LRESULT
ANIMATE_NCCreate (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ANIMATE_INFO *infoPtr;

    /* allocate memory for info structure */
    infoPtr = (ANIMATE_INFO *)COMCTL32_Alloc (sizeof(ANIMATE_INFO));

    /* store pointer to info structure */
    SetWindowLongA (hwnd,0,(DWORD)infoPtr);

    /* set default settings */
    infoPtr->lpAvi    = NULL;
    infoPtr->hFile    = 0;
    infoPtr->hRes     = 0;
    infoPtr->cSize    = 0;
    infoPtr->cRepeat  = 0;
    infoPtr->wFrom    = 0;
    infoPtr->wTo      = 0;
    infoPtr->wPos     = 0;
    infoPtr->bPlaying = FALSE;
    infoPtr->cFrames  = 0;
    infoPtr->pos.x    = 0;
    infoPtr->pos.y    = 0;
    infoPtr->size.x   = 0;
    infoPtr->size.y   = 0;
    infoPtr->bThread  = 0;

    return DefWindowProcA(hwnd,WM_NCCREATE,wParam,lParam);
}

static LRESULT
ANIMATE_EraseBackground (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HBRUSH hBrush;
    RECT rect;
    HDC hdc = (HDC)wParam;

    if (GetWindowLongA(hwnd,GWL_STYLE) & ACS_TRANSPARENT)
    {
      hBrush = SendMessageA(hwnd,WM_CTLCOLORSTATIC,hdc,hwnd);
    } else hBrush = GetSysColorBrush(COLOR_WINDOW);

    GetClientRect(hwnd,&rect);
    FillRect((HDC)wParam,&rect,hBrush);

    return TRUE;
}

static LRESULT
ANIMATE_NCHitTest(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  return HTTRANSPARENT;
}

static VOID
ANIMATE_DrawFrame(ANIMATE_INFO *infoPtr)
{
  //CB: todo
  //MCI_PUT
}

static LRESULT
ANIMATE_Paint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hwnd);

  if (ANIMATE_IsLoaded(infoPtr)) ANIMATE_DrawFrame(infoPtr);

  return 0;
}

static LRESULT
ANIMATE_Size(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hwnd);
  DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

  if (ANIMATE_IsPlaying(infoPtr) && dwStyle & ACS_CENTER)
  {
    infoPtr->pos.x = (LOWORD(lParam)-infoPtr->size.x)/2;
    infoPtr->pos.y = (HIWORD(lParam)-infoPtr->size.y)/2;
  }

  return DefWindowProcA(hwnd,WM_SIZE,wParam,lParam);
}

static LRESULT
ANIMATE_StyleChanged(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  //CB: todo

  return 0;
}

static LRESULT
ANIMATE_Timer(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hwnd);
  INT newPos = infoPtr->wPos+1;

  if (wParam != TIMER_ID) return 0;

  if (newPos == infoPtr->wTo || (infoPtr->wTo == -1 && newPos == infoPtr->cFrames-1))
  {
    if (infoPtr->cRepeat != -1)
    {
      infoPtr->cRepeat--;
      if (infoPtr->cRepeat == 0) Animate_Stop(hwnd);
    }
  } else
  {
    infoPtr->wPos++;
    ANIMATE_DrawFrame(infoPtr);
  }

  return 0;
}

/* Control messages */

static LRESULT
ANIMATE_Open(HWND hwnd,WPARAM wParam,LPARAM lParam,BOOL unicode)
{
    ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hwnd);
    HINSTANCE hInstance = (HINSTANCE)wParam;

    ANIMATE_FreeResource(infoPtr);

MessageBoxA(hwnd,"open",NULL,MB_OK);
    if (!lParam) return TRUE;

    if (HIWORD(lParam))
    {
      if (ANIMATE_LoadRes(infoPtr,hInstance,(LPSTR)lParam,unicode));
      else if (ANIMATE_LoadFile(infoPtr,(LPSTR)lParam,unicode));
      else return FALSE;
    } else
    {
      if (!ANIMATE_LoadRes(infoPtr,hInstance,unicode ? (LPTSTR)MAKEINTRESOURCEW((INT)lParam):MAKEINTRESOURCEA((INT)lParam),unicode)) return FALSE;
    }

    ANIMATE_GetAviInfo(infoPtr);

    if (GetWindowLongA(hwnd,GWL_STYLE) & ACS_AUTOPLAY) Animate_Play(hwnd,0,-1,1);

    return TRUE;
}

static LRESULT
ANIMATE_Play (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hwnd);
    INT wFrom   = (INT)LOWORD(lParam);
    INT wTo     = (INT)HIWORD(lParam);
    DWORD dwStyle = GetWindowLongA(hwnd,GWL_STYLE);

    if (!ANIMATE_IsLoaded(infoPtr)) return FALSE;

    Animate_Stop(hwnd);

    if (wFrom < 0) wFrom = 0;
    if (wFrom > wTo && wTo != -1) return FALSE;
    if (wFrom >= infoPtr->cFrames) wFrom = infoPtr->cFrames-1;
    if (wTo >= infoPtr->cFrames) wTo = infoPtr->cFrames-1;

    infoPtr->cRepeat = (INT)wParam;
    infoPtr->wFrom   = wFrom;
    infoPtr->wTo     = wTo;
    infoPtr->wPos    = infoPtr->wFrom;

    if (dwStyle & ACS_CENTER)
    {
      RECT rect;

      GetClientRect(hwnd,&rect);
      infoPtr->pos.x = (rect.right-infoPtr->size.x)/2;
      infoPtr->pos.y = (rect.bottom-infoPtr->size.y)/2;
    } else
    {
      infoPtr->pos.x = 0;
      infoPtr->pos.y = 0;
    }

    InvalidateRect(hwnd,NULL,FALSE);

    if (wFrom == wTo)
    {
      SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),ACN_START),hwnd);
      SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),ACN_STOP),hwnd);

      return TRUE;
    }

    infoPtr->bPlaying = TRUE;

    if (dwStyle & ACS_TIMER)
    {
      infoPtr->bThread = FALSE;
      SetTimer(hwnd,TIMER_ID,infoPtr->msFrame,NULL);
    } else
    {
      infoPtr->bThread = TRUE;
      //CB:todo
    }

    SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),ACN_START),hwnd);

    return TRUE;
}

static LRESULT
ANIMATE_Stop (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  ANIMATE_INFO *infoPtr = ANIMATE_GetInfoPtr(hwnd);

  if (!ANIMATE_IsLoaded(infoPtr)) return FALSE;
  if (!ANIMATE_IsPlaying(infoPtr)) return TRUE;

  if (infoPtr->bThread)
  {
    //CB:todo
  } else
  {
    KillTimer(hwnd,TIMER_ID);
  }

  infoPtr->bPlaying = FALSE;
  SendMessageA(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID),ACN_STOP),hwnd);

  return TRUE;
}

static LRESULT WINAPI
ANIMATE_WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case ACM_OPENA:
          return ANIMATE_Open(hwnd,wParam,lParam,FALSE);

        case ACM_OPENW:
          return ANIMATE_Open(hwnd,wParam,lParam,TRUE);

        case ACM_PLAY:
          return ANIMATE_Play (hwnd, wParam, lParam);

        case ACM_STOP:
          return ANIMATE_Stop (hwnd, wParam, lParam);

        case WM_CLOSE:
          return ANIMATE_Close(hwnd,wParam,lParam);

        case WM_NCCREATE:
          return ANIMATE_NCCreate(hwnd,wParam,lParam);

        case WM_DESTROY:
          return ANIMATE_Destroy (hwnd, wParam, lParam);

        case WM_ERASEBKGND:
          return ANIMATE_EraseBackground (hwnd, wParam, lParam);

        case WM_NCHITTEST:
          return ANIMATE_NCHitTest(hwnd,wParam,lParam);

        case WM_PAINT:
          return ANIMATE_Paint(hwnd,wParam,lParam);

        case WM_SIZE:
          return ANIMATE_Size(hwnd,wParam,lParam);

      case WM_STYLECHANGED:
          return ANIMATE_StyleChanged(hwnd,wParam,lParam);

      case WM_TIMER:
          return ANIMATE_Timer(hwnd,wParam,lParam);

        default:
//          if (uMsg >= WM_USER)
//              ERR (animate, "unknown msg %04x wp=%08x lp=%08lx\n",
//                   uMsg, wParam, lParam);
            return DefWindowProcA (hwnd, uMsg, wParam, lParam);
    }

    return 0;
}


VOID
ANIMATE_Register (VOID)
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS;
    wndClass.lpfnWndProc   = (WNDPROC)ANIMATE_WindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(ANIMATE_INFO *);
    wndClass.hCursor       = LoadCursorA (0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wndClass.lpszClassName = ANIMATE_CLASSA;

    RegisterClassA (&wndClass);
}


VOID
ANIMATE_Unregister (VOID)
{
    UnregisterClassA (ANIMATE_CLASSA, (HINSTANCE)NULL);
}

