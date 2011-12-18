/* $Id: dbglocal.h,v 1.5 2001/07/20 15:41:04 sandervl Exp $ */

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
void ParseLogStatusCRYPT32();

#define DBG_ENVNAME        "dbg_crypt32"
#define DBG_ENVNAME_LVL2   "dbg_crypt32_lvl2"

#define DBG_initterm       0
#define DBG_MAXFILES       1

extern USHORT DbgEnabledCRYPT32[DBG_MAXFILES];
extern USHORT DbgEnabledLvl2CRYPT32[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif

#define dprintf(a)      if(DbgEnabledCRYPT32[DBG_LOCALLOG] == 1) WriteLog a

#ifdef dprintf2
#undef dprintf2
#endif

#define dprintf2(a)     if(DbgEnabledLvl2CRYPT2[DBG_LOCALLOG] == 1) WriteLog a

#else

#define ParseLogStatusCRYPT32()

#endif //DEBUG

#endif
