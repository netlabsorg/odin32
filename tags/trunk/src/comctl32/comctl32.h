/* $Id: comctl32.h,v 1.13 2000-04-16 18:26:57 cbratschi Exp $ */
/*
 * Win32 common controls implementation
 *
 * Copyright (C) 1999 Achim Hasenmueller
 *
 * Based on the work of the WINE group (www.winehq.com)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/* Status: WINE 991212 */

#ifndef _H_COMCTL32
#define _H_COMCTL32

#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include <win/commctrl.h>
#include "unicode.h"
#include "wineuni.h"

#define _OS2WIN_H
#define NO_ULONG
#include <misc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* registers window classes inside this DLL */
void CDECL RegisterCOMCTL32WindowClasses(unsigned long hinstDLL);
void CDECL UnregisterCOMCTL32WindowClasses(void);
#ifdef __cplusplus
}
#endif


// string functions from kernel32
LPWSTR WIN32API HEAP_strdupAtoW( HANDLE heap, DWORD flags, LPCSTR str );
LPSTR  WIN32API HEAP_strdupWtoA( HANDLE heap, DWORD flags, LPCWSTR str );


/**************************************************************************
 *  UNDOCUMENTED functions
 */

/* private heap memory functions */

LPVOID WINAPI COMCTL32_Alloc (DWORD);
LPVOID WINAPI COMCTL32_ReAlloc (LPVOID, DWORD);
BOOL WINAPI COMCTL32_Free (LPVOID);
DWORD  WINAPI COMCTL32_GetSize (LPVOID);


extern HMODULE COMCTL32_hModule;

/* Property sheet / Wizard */
#define IDD_PROPSHEET 1006
#define IDD_WIZARD    1020

#define IDC_TABCONTROL   12320
#define IDC_APPLY_BUTTON 12321
#define IDC_BACK_BUTTON  12323
#define IDC_NEXT_BUTTON  12324
#define IDC_FINISH_BUTTON 12325
#define IDC_SUNKEN_LINE   12326

#define IDS_CLOSE 4160

/* Toolbar customization dialog */
#define IDD_TBCUSTOMIZE     200

#define IDC_AVAILBTN_LBOX   201
#define IDC_RESET_BTN       202
#define IDC_TOOLBARBTN_LBOX 203
#define IDC_REMOVE_BTN      204
#define IDC_HELP_BTN        205
#define IDC_MOVEUP_BTN      206
#define IDC_MOVEDN_BTN      207

/* Toolbar imagelist bitmaps */
#define IDB_STD_SMALL       120
#define IDB_STD_LARGE       121
#define IDB_VIEW_SMALL      124
#define IDB_VIEW_LARGE      125
#define IDB_HIST_SMALL      130
#define IDB_HIST_LARGE      131

//direction bitmaps
#define IDB_DIRECTION_ALL   132
#define IDB_DIRECTION_NS    133
#define IDB_DIRECTION WE    134

//Header filter bitmap
#define IDB_HEADER_FILTER   140

//cursors
#define IDC_COMCTL32_ERROR       20480
#define IDC_COMCTL32_INFORMATION 20481
#define IDC_COMCTL32_EXCLAMATION 20482

#define IDC_COMCTL32_DRAGRECT      102
#define IDC_COMCTL32_ARROW1        104
#define IDC_COMCTL32_ARROW2        105
#define IDC_COMCTL32_DRAGHLINE     106
#define IDC_COMCTL32_SPLITHLINE    107
#define IDC_COMCTL32_HAND          108
#define IDC_COMCTL32_DIRECTION_NS  109
#define IDC_COMCTL32_DIRECTION_WE  110
#define IDC_COMCTL32_DIRECTION_ALL 111
#define IDC_COMCTL32_DIRECTION_N   112
#define IDC_COMCTL32_DIRECTION_S   113
#define IDC_COMCTL32_DIRECTION_E   114
#define IDC_COMCTL32_DIRECTION_W   115
#define IDC_COMCTL32_DIRECTION_NE  116
#define IDC_COMCTL32_DIRECTION_NW  117
#define IDC_COMCTL32_DIRECTION_SE  118
#define IDC_COMCTL32_DIRECTION_SW  119

#define IDC_COMCTL32_SPLITVLINE    135
#define IDC_COMCTL32_ENTER         150

/* Month calendar month menu popup */
#define IDD_MCMONTHMENU     300

#define IDM_JAN                         301
#define IDM_FEB                         302
#define IDM_MAR                         303
#define IDM_APR                         304
#define IDM_MAY                         305
#define IDM_JUN                         306
#define IDM_JUL                         307
#define IDM_AUG                         308
#define IDM_SEP                         309
#define IDM_OCT                         310
#define IDM_NOV                         311
#define IDM_DEC                         312

/* treeview checkboxes */
#define IDT_CHECK 401

#endif /* _H_COMCTL32 */
