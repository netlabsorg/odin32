/* $Id: dbglocal.h,v 1.1 2002-06-02 10:09:28 sandervl Exp $ */

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
void ParseLogStatusUSER32();

#define DBG_ENVNAME        "dbg_user32"
#define DBG_ENVNAME_LVL2   "dbg_user32_lvl2"

#define DBG_controls             0
#define DBG_button               1
#define DBG_static               2
#define DBG_scroll               3
#define DBG_listbox              4
#define DBG_menu                 5
#define DBG_combo                6
#define DBG_edit                 7
#define DBG_winswitch            8
#define DBG_icontitle            9
#define DBG_initterm             10
#define DBG_trace                11
#define DBG_dialog               12
#define DBG_MAXFILES             13

extern USHORT DbgEnabledOdinCtrl[DBG_MAXFILES];
extern USHORT DbgEnabledLvl2OdinCtrl[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif

#define dprintf(a)      if(DbgEnabledOdinCtrl[DBG_LOCALLOG] == 1) WriteLog a

#ifdef dprintf2
#undef dprintf2
#endif

#define dprintf2(a)     if(DbgEnabledLvl2OdinCtrl[DBG_LOCALLOG] == 1) WriteLog a

#else

#define ParseLogStatusOdinCtrl()

#endif //DEBUG

#endif
