/* $Id: dbglocal.h,v 1.3 2000-04-29 18:28:35 sandervl Exp $ */

/*
 * debug logging functions for OS/2
 *
 * Overrides main dprintf macros
 *
 * Copyright 2000 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DBGLOCAL_H__
#define __DBGLOCAL_H__

#ifdef DEBUG
//Parses environment variable for selective enabling/disabling of logging
void ParseLogStatus();

#define DBG_ENVNAME        "dbg_user32"
#define DBG_ENVNAME_LVL2   "dbg_user32_lvl2"

#define DBG_user32               0
#define DBG_loadres              1
#define DBG_dde                  2
#define DBG_win32wndhandle       3
#define DBG_wsprintf             4
#define DBG_winmouse             5
#define DBG_icon                 6
#define DBG_hook                 7
#define DBG_winkeyboard          8
#define DBG_defwndproc           9
#define DBG_syscolor             10
#define DBG_char                 11
#define DBG_initterm             12
#define DBG_uitools              13
#define DBG_unknown              14
#define DBG_spy                  15
#define DBG_wndmsg               16
#define DBG_display              17
#define DBG_pmwindow             18
#define DBG_pmframe              19
#define DBG_win32class           20
#define DBG_win32wnd             21
#define DBG_win32dlg             22
#define DBG_msgbox               23
#define DBG_window               24
#define DBG_windowmsg            25
#define DBG_windowclass          26
#define DBG_win32wbase           27
#define DBG_windowword           28
#define DBG_gen_object           29
#define DBG_oslibwin             30
#define DBG_win32wndchild        31
#define DBG_controls             32
#define DBG_button               33
#define DBG_static               34
#define DBG_scroll               35
#define DBG_listbox              36
#define DBG_menu                 37
#define DBG_combo                38
#define DBG_edit                 39
#define DBG_winswitch            40
#define DBG_icontitle            41
#define DBG_clipboard            42
#define DBG_winicon              43
#define DBG_win32wmdiclient      44
#define DBG_oslibutil            45
#define DBG_oslibmsg             46
#define DBG_dib                  47
#define DBG_oslibdos             48
#define DBG_win32wmdichild       49
#define DBG_winprop              50
#define DBG_wingdi               51
#define DBG_oslibgdi             52
#define DBG_winaccel             53
#define DBG_win32wbasepos        54
#define DBG_oslibres             55
#define DBG_dc                   56
#define DBG_timer                57
#define DBG_caret                58
#define DBG_resource             59
#define DBG_winproc              60
#define DBG_text                 61
#define DBG_oslibmsgtranslate    62
#define DBG_windlgmsg            63
#define DBG_windlg               64
#define DBG_win32wdesktop        65
#define DBG_win32wbasenonclient  66
#define DBG_MAXFILES             67

extern USHORT DbgEnabled[DBG_MAXFILES];
extern USHORT DbgEnabledLvl2[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif

#define dprintf(a)      if(DbgEnabled[DBG_LOCALLOG] == 1) WriteLog a

#ifdef dprintf2
#undef dprintf2
#endif

#define dprintf2(a)     if(DbgEnabledLvl2[DBG_LOCALLOG] == 1) WriteLog a

#else

#define ParseLogStatus()

#endif //DEBUG

#endif
