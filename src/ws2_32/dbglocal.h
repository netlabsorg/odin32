/* $Id: dbglocal.h,v 1.1 2001-05-07 08:23:49 phaller Exp $ */

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

#define DBG_ENVNAME        "dbg_ws2help"

#define DBG_initterm       0
#define DBG_wsastruct      1
#define DBG_wsock32        2
#define DBG_unknown        3
#define DBG_async          4
#define DBG_asyncthread    5
#define DBG_wsa            6
#define DBG_MAXFILES       7

extern USHORT DbgEnabled[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif

#define dprintf(a)      if(DbgEnabled[DBG_LOCALLOG] == 1) WriteLog a

#else

#define ParseLogStatus()

#endif //DEBUG

#endif
