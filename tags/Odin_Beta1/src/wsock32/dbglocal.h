/* $Id: dbglocal.h,v 1.5 2001-07-20 15:41:04 sandervl Exp $ */

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
void ParseLogStatusWSOCK32();

#define DBG_ENVNAME        "dbg_wsock32"
#define DBG_ENVNAME_LVL2   "dbg_wsock32_lvl2"

#define DBG_initterm       0
#define DBG_wsastruct      1
#define DBG_wsock32        2
#define DBG_unknown        3
#define DBG_async          4
#define DBG_asyncthread    5
#define DBG_wsa            6
#define DBG_MAXFILES       7

extern USHORT DbgEnabledWSOCK32[DBG_MAXFILES];
extern USHORT DbgEnabledLvl2WSOCK32[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif

#define dprintf(a)      if(DbgEnabledWSOCK32[DBG_LOCALLOG] == 1) WriteLog a

#ifdef dprintf2
#undef dprintf2
#endif

#define dprintf2(a)     if(DbgEnabledLvl2WSOCK32[DBG_LOCALLOG] == 1) WriteLog a

#else

#define ParseLogStatusWSOCK32()

#endif //DEBUG

#endif
