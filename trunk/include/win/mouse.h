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

typedef BOOL (* CALLBACK LPMOUSE_EVENT_PROC)(DWORD,DWORD,DWORD,DWORD,DWORD);

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

#define MSH_MOUSEWHEEL TEXT("MSWHEEL_ROLLMSG")

#define WHEEL_DELTA      120

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST+1)
#endif

#define MOUSEZ_CLASSNAME  TEXT("MouseZ")
#define MOUSEZ_TITLE      TEXT("Magellan MSWHEEL")

#define MSH_WHEELMODULE_CLASS (MOUSEZ_CLASSNAME)
#define MSH_WHEELMODULE_TITLE (MOUSEZ_TITLE)

#define MSH_WHEELSUPPORT TEXT("MSH_WHEELSUPPORT_MSG")

#define MSH_SCROLL_LINES TEXT("MSH_SCROLL_LINES_MSG")

#ifndef  WHEEL_PAGESCROLL
#define WHEEL_PAGESCROLL  (UINT_MAX)
#endif

#ifndef SPI_SETWHEELSCROLLLINES
#define SPI_SETWHEELSCROLLLINES   105
#endif


/* 	MouseWheel support
***********************************/

#endif /* __WINE_MOUSE_H */

