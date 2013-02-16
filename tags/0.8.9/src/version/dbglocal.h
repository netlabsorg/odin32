/* $Id: dbglocal.h,v 1.1 2003-10-24 15:08:00 sandervl Exp $ */

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

#ifdef __cplusplus
extern "C" {
#endif

void ParseLogStatusVERSION();
BOOL CheckDebugStatus(int index);

#ifdef __cplusplus
}
#endif

#define DBG_ENVNAME        "dbg_version"
#define DBG_ENVNAME_LVL2   "dbg_version_lvl2"

#define DBG_info                 0
#define DBG_install              1
#define DBG_resource             2
#define DBG_MAXFILES             3

extern USHORT DbgEnabledVERSION[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif


#define dprintf(a)      if(CheckDebugStatus(DBG_LOCALLOG)) WriteLog a

#else

#define ParseLogStatusVERSION()

#endif //DEBUG

#endif
