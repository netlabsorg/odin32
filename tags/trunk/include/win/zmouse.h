/* $Id: zmouse.h,v 1.1 2001-05-03 18:06:46 sandervl Exp $ */

/*
 * MOUSE driver interface
 *
 * Copyright 1998 Ulrich Weigand
 */

#ifndef __WINE_MOUSE_H
#define __WINE_MOUSE_H

#include "windef.h"

struct tagCURSORICONINFO;

#include "pshpack1.h"
typedef struct _MOUSEINFO
{
    BYTE msExist;
    BYTE msRelative;
    WORD msNumButtons;
    WORD msRate;
    WORD msXThreshold;
    WORD msYThreshold;
    WORD msXRes;
    WORD msYRes;
    WORD msMouseCommPort;
} MOUSEINFO, *LPMOUSEINFO;
#include "poppack.h"

typedef BOOL (CALLBACK *LPMOUSE_EVENT_PROC)(DWORD,DWORD,DWORD,DWORD,DWORD);

WORD WINAPI MOUSE_Inquire(LPMOUSEINFO lpMouseInfo);
VOID WINAPI MOUSE_Enable(LPMOUSE_EVENT_PROC lpMouseEventProc);
VOID WINAPI MOUSE_Disable(VOID);

/* Wine internals */

typedef struct tagMOUSE_DRIVER {
  VOID (*pSetCursor)(struct tagCURSORICONINFO *);
  VOID (*pMoveCursor)(WORD, WORD);
  BOOL (*pEnableWarpPointer)(BOOL);
} MOUSE_DRIVER;

extern MOUSE_DRIVER *MOUSE_Driver;

#define WINE_MOUSEEVENT_MAGIC  ( ('M'<<24)|('A'<<16)|('U'<<8)|'S' )
typedef struct _WINE_MOUSEEVENT
{
    DWORD magic;
    DWORD keyState;
    DWORD time;
    HWND hWnd;

} WINE_MOUSEEVENT;

void MOUSE_SendEvent( DWORD mouseStatus, DWORD posX, DWORD posY,
                      DWORD keyState, DWORD time, HWND hWnd );

/***********************************
 * 	MouseWheel support (defines)
 */

#define MSH_MOUSEWHEEL "MSWHEEL_ROLLMSG"

#define WHEEL_DELTA      120

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST+1)
#endif

#define MOUSEZ_CLASSNAME  "MouseZ"          
#define MOUSEZ_TITLE      "Magellan MSWHEEL"

#define MSH_WHEELMODULE_CLASS (MOUSEZ_CLASSNAME)
#define MSH_WHEELMODULE_TITLE (MOUSEZ_TITLE)

#define MSH_WHEELSUPPORT "MSH_WHEELSUPPORT_MSG"

#define MSH_SCROLL_LINES "MSH_SCROLL_LINES_MSG"

#ifndef  WHEEL_PAGESCROLL  
#define WHEEL_PAGESCROLL  (UINT_MAX)
#endif 

#ifndef SPI_SETWHEELSCROLLLINES
#define SPI_SETWHEELSCROLLLINES   105
#endif

/*********************************************************************
* INLINE FUNCTION: HwndMsWheel
* Purpose : Get a reference to MSWheel Window, the registered messages,  
*           wheel support active setting, and number of scrollLines
* Params  : PUINT puiMsh_MsgMouseWheel - address of UINT to contain returned registered wheel message
*           PUINT puiMsh_Msg3DSupport - address of UINT to contain wheel support registered message
*           PUINT puiMsh_MsgScrollLines - address of UINT to contain Scroll lines registered message
*           PBOOL pf3DSupport - address of BOOL to contain returned flag for wheel support active
*           PINT  piScrollLines - address of int to contain returned scroll lines
* Returns : HWND handle to the MsWheel window
* Note    : The return value for pf3DSupport and piScrollLines is dependant
*           on the POINT32 module.  If POINT32 module is not running then
*           the values returned for these parameters will be
*           FALSE and 3, respectively. 
*********************************************************************/
inline HWND HwndMSWheel( 
      PUINT puiMsh_MsgMouseWheel,
      PUINT puiMsh_Msg3DSupport,
      PUINT puiMsh_MsgScrollLines,
      PBOOL pf3DSupport,
      PINT  piScrollLines
)
{
   HWND hdlMsWheel;

   hdlMsWheel = FindWindow(MSH_WHEELMODULE_CLASS, MSH_WHEELMODULE_TITLE);

   *puiMsh_MsgMouseWheel = RegisterWindowMessage(MSH_MOUSEWHEEL);
   *puiMsh_Msg3DSupport = RegisterWindowMessage(MSH_WHEELSUPPORT);
   *puiMsh_MsgScrollLines = RegisterWindowMessage(MSH_SCROLL_LINES);

   if (*puiMsh_Msg3DSupport)
      *pf3DSupport = (BOOL)SendMessage(hdlMsWheel, *puiMsh_Msg3DSupport, 0, 0);
   else
      *pf3DSupport = FALSE;  // default to FALSE

   if (*puiMsh_MsgScrollLines)
      *piScrollLines = (int)SendMessage(hdlMsWheel, *puiMsh_MsgScrollLines, 0, 0);
   else
      *piScrollLines = 3;  // default

   return(hdlMsWheel);
}

/* 	MouseWheel support
***********************************/

#endif /* __WINE_MOUSE_H */

