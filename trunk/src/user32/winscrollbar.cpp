/* $Id: winscrollbar.cpp,v 1.1 1999-09-15 23:19:03 sandervl Exp $ */
/*
 * Win32 scrollbar functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>

//******************************************************************************
//******************************************************************************
BOOL WIN32API EnableScrollBar( HWND arg1, INT arg2, UINT arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  EnableScrollBar\n");
#endif
    //CB: implement in window class
    return O32_EnableScrollBar(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetScrollPos( HWND arg1, int arg2, int arg3, BOOL  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  SetScrollPos\n");
#endif
    return O32_SetScrollPos(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetScrollRange( HWND arg1, int arg2, int arg3, int arg4, BOOL  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SetScrollRange\n");
#endif
    return O32_SetScrollRange(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ShowScrollBar( HWND arg1, int arg2, BOOL  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  ShowScrollBar\n");
#endif
    return O32_ShowScrollBar(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetScrollPos(HWND hwnd, int fnBar)
{
 int pos;

    pos = O32_GetScrollPos(hwnd, fnBar);
#ifdef DEBUG
    WriteLog("USER32:  GetScrollPos of %X type %d returned %d\n", hwnd, fnBar, pos);
#endif
    return(pos);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetScrollRange( HWND arg1, int arg2, int * arg3, int *  arg4)
{
#ifdef DEBUG
    WriteLog("USER32:  GetScrollRange\n");
#endif
    return O32_GetScrollRange(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ScrollWindow( HWND arg1, int arg2, int arg3, const RECT * arg4, const RECT *  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  ScrollWindow\n");
#endif
    return O32_ScrollWindow(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
INT WIN32API ScrollWindowEx( HWND arg1, int arg2, int arg3, const RECT * arg4, const RECT * arg5, HRGN arg6, PRECT arg7, UINT  arg8)
{
#ifdef DEBUG
    WriteLog("USER32:  ScrollWindowEx\n");
#endif
    return O32_ScrollWindowEx(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi)
{
#ifdef DEBUG
  WriteLog("USER32:  GetScrollInfo\n");
#endif
  if(lpsi == NULL)
    return(FALSE);

  if(lpsi->fMask & SIF_POS)
    lpsi->nPos = GetScrollPos(hwnd, fnBar);
  if(lpsi->fMask & SIF_RANGE)
    GetScrollRange(hwnd, fnBar, &lpsi->nMin, &lpsi->nMax);
  if(lpsi->fMask & SIF_PAGE) {
#ifdef DEBUG
    WriteLog("USER32:  GetScrollInfo, page info not implemented\n");
#endif
    lpsi->nPage     = 25;
  }
  return(TRUE);
}
//******************************************************************************
//TODO: Not complete
//******************************************************************************
INT WIN32API SetScrollInfo(HWND hwnd, INT fnBar, const SCROLLINFO *lpsi, BOOL fRedraw)
{
 int smin, smax;

#ifdef DEBUG
  WriteLog("USER32:  SetScrollInfo\n");
#endif
  if(lpsi == NULL)
    return(FALSE);

  if(lpsi->fMask & SIF_POS)
        SetScrollPos(hwnd, fnBar, lpsi->nPos, fRedraw);
  if(lpsi->fMask & SIF_RANGE)
        SetScrollRange(hwnd, fnBar, lpsi->nMin, lpsi->nMax, fRedraw);
  if(lpsi->fMask & SIF_PAGE) {
#ifdef DEBUG
        WriteLog("USER32:  GetScrollInfo, page info not implemented\n");
#endif
  }
  if(lpsi->fMask & SIF_DISABLENOSCROLL) {
#ifdef DEBUG
        WriteLog("USER32:  GetScrollInfo, disable scrollbar not yet implemented\n");
#endif
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
