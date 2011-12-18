/* $Id: ctrlconf.h,v 1.3 2003-05-27 09:46:30 sandervl Exp $ */
/*
 * Win32 common controls
 *
 */

#ifndef __CTRLCONF_H
#define __CTRLCONF_H

#include "wndmsg.h"
#include "win32wmisc.h"

#define SPY_GetMsgName(msg, hwnd)	GetMsgText(msg)

#define MapSL(a)			a

#define SendMessage16			SendMessageA

#define WIN_GetFullHandle(hwnd)		hwnd

#define  WIN_SuspendWndsLock()		0
#define  WIN_RestoreWndsLock(ipreviousLock)

//Set the style of a window without sending WM_STYLECHANGING/ED messages
void WIN_SetStyle(HWND hwnd, DWORD dwStyle);

//WIN31 look only -> we don't use it
#define NC_DrawSysButton(hwnd, hdc, state)


#define LOCAL_Alloc(ds, flags, size)		0
#define LOCAL_ReAlloc(ds, handle, size, flags )	0
#define LOCAL_Lock(ds, handle )			NULL
#define LOCAL_Unlock(ds, handle )		1
#define LOCAL_Size(ds, handle )			0

/* Wine look */

typedef enum
{
    WIN31_LOOK,
    WIN95_LOOK,
    WIN98_LOOK
} WINE_LOOK;

#define TWEAK_WineLook WIN98_LOOK

typedef LPDRAGINFO LPDRAGINFO16;

/* Built-in class descriptor */
struct builtin_class_descr
{
    LPCSTR  name;    /* class name */
    UINT    style;   /* class style */
    WNDPROC procA;   /* ASCII window procedure */
    WNDPROC procW;   /* Unicode window procedure */
    INT     extra;   /* window extra bytes */
    LPCSTR  cursor;  /* cursor name */
    HBRUSH  brush;   /* brush or system color */
};

#ifdef __cplusplus
extern "C" {
#endif

extern const struct builtin_class_descr LISTBOX_builtin_class;
extern const struct builtin_class_descr COMBOLBOX_builtin_class;
extern const struct builtin_class_descr COMBO_builtin_class;
extern const struct builtin_class_descr EDIT_builtin_class;
extern const struct builtin_class_descr MDICLIENT_builtin_class;
extern const struct builtin_class_descr SCROLL_builtin_class;
extern const struct builtin_class_descr MENU_builtin_class;

#ifdef __cplusplus
}
#endif

#endif // __CTRLCONF_H
