/* $Id: oslibgdi.cpp,v 1.1 2001-05-11 08:37:24 sandervl Exp $ */
/*
 * Window GDI wrapper functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_PM
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <winconst.h>
#include <oslibgdi.h>
#include <oslibwin.h>
#include "win32wbase.h"

#define DBG_LOCALLOG	DBG_oslibgdi
#include "dbglocal.h"

/*
First letter is lower case to avoid conflicts with Win32 API names
All transformations are for screen or client windows, for frame windows use OS/2 API's

Single y mapping:
 mapScreenY()
 mapY()			//only reverses y
 mapOS2ToWin32Y()	//reverse y + subtract parent client offset
 mapOS2ToWin32X()	//subtract parent client offset

 mapWin32ToOS2Y()	//reverse y + add parent client offset
 mapWin32ToOS2Y()	//add parent client offset

Single point mapping:
 mapScreenPoint()
 mapOS2ToWin32Point()
 mapWin32ToOS2Point()
 mapWin32Point()

Single rect mapping:
 mapOS2ToWin32ScreenRect()
 mapWin32ToOS2ScreenRect()
 mapOS2ToWin32Rect()
 mapWin32ToOS2Rect()
 mapWin32Rect()

Rect transformation:
 copyOS2ToWin32Rect()
 copyWin32ToOS2Rect()
*/

//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
INT mapScreenY(INT screenPosY)
{
  return ScreenHeight-1-screenPosY;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
INT mapScreenY(INT screenH,INT screenPosY)
{
  return screenH-1-screenPosY;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
BOOL mapScreenPoint(OSLIBPOINT *screenPt)
{
  if(!screenPt) return FALSE;
  screenPt->y = ScreenHeight-1-screenPt->y;

  return TRUE;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
BOOL mapScreenPoint(INT screenH,OSLIBPOINT *screenPt)
{
  if (!screenPt) return FALSE;
  screenPt->y = screenH-1-screenPt->y;

  return TRUE;
}
//******************************************************************************
// To translation between OS/2 <-> Win32
//******************************************************************************
BOOL mapOS2ToWin32Rect(int height, PRECTLOS2 rectOS2, PRECT rectWin32)
{
  if(!rectOS2 || !rectWin32) {
	DebugInt3();
  	return FALSE;
  }
  rectWin32->bottom = height-rectOS2->yBottom;
  rectWin32->top    = height-rectOS2->yTop;
  rectWin32->left   = rectOS2->xLeft;
  rectWin32->right  = rectOS2->xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL mapWin32ToOS2Rect(int height, PRECT rectWin32, PRECTLOS2 rectOS2)
{
  if(!rectOS2 || !rectWin32) {
	DebugInt3();
  	return FALSE;
  }
  rectOS2->yBottom = height-rectWin32->bottom;
  rectOS2->yTop    = height-rectWin32->top;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;

  return TRUE;
}
//******************************************************************************
//Win32 rectangle in client coordinates (relative to upper left corner of client window)
//Convert to frame coordinates (relative to lower left corner of window)
//******************************************************************************
BOOL mapWin32ToOS2RectClientToFrame(Win32BaseWindow *window, PRECT rectWin32,PRECTLOS2 rectOS2)
{
 int height;
 int xclientorg;
 int yclientorg;

  if(!window || !rectOS2 || !rectWin32) {
	DebugInt3();
  	return FALSE;
  }
  height     = window->getWindowHeight();
  xclientorg = window->getClientRectPtr()->left;
  yclientorg = window->getClientRectPtr()->top;

  rectOS2->yBottom = height - (rectWin32->bottom + yclientorg);
  rectOS2->yTop    = height - (rectWin32->top + yclientorg);
  rectOS2->xLeft   = rectWin32->left  + xclientorg;
  rectOS2->xRight  = rectWin32->right + xclientorg;

  return TRUE;
}
//******************************************************************************
//OS/2 rectangle in frame coordinates (relative to lower left corner of window)
//Convert to client coordinates (relative to upper left corner of client window)
//Note: win32 rectangle can be bigger than client area!
//******************************************************************************
BOOL mapOS2ToWin32RectFrameToClient(Win32BaseWindow *window, PRECTLOS2 rectOS2, 
                                    PRECT rectWin32)
{
 int height;
 int xclientorg;
 int yclientorg;

  if(!window || !rectOS2 || !rectWin32) {
	DebugInt3();
  	return FALSE;
  }
  height     = window->getWindowHeight();
  xclientorg = window->getClientRectPtr()->left;
  yclientorg = window->getClientRectPtr()->top;

  rectWin32->bottom = height - (rectOS2->yBottom + yclientorg);
  rectWin32->top    = height - (rectOS2->yTop + yclientorg);
  rectWin32->left   = rectOS2->xLeft  - xclientorg;
  rectWin32->right  = rectOS2->xRight - xclientorg;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL copyOS2ToWin32Rect(PRECTLOS2 rectOS2,PRECT rectWin32)
{
  rectWin32->bottom = rectOS2->yBottom;
  rectWin32->top    = rectOS2->yTop;
  rectWin32->left   = rectOS2->xLeft;
  rectWin32->right  = rectOS2->xRight;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL copyWin32ToOS2WindowRect(PRECT rectWin32,PRECTLOS2 rectOS2)
{
  rectOS2->yBottom = rectWin32->bottom;
  rectOS2->yTop    = rectWin32->top;
  rectOS2->xLeft   = rectWin32->left;
  rectOS2->xRight  = rectWin32->right;

  return TRUE;
}
//******************************************************************************
//******************************************************************************
