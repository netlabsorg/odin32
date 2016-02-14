/* $Id: dbglocal.h,v 1.9 2016-02-09 10:00:00 rousseau Exp $ */

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
void ParseLogStatusGDIPLUS();

#define DBG_ENVNAME        "dbg_gdiplus"
#define DBG_ENVNAME_LVL2   "dbg_gdiplus_lvl2"

#define DBG_gdiplus         0
//~ #define DBG_opengl          1
//~ #define DBG_callback        2
//~ #define DBG_dibsect         3
#define DBG_initterm        1
//~ #define DBG_resource        5
//~ #define DBG_oslibgdi        6
//~ #define DBG_font            7
//~ #define DBG_text            8
//~ #define DBG_palette         9
//~ #define DBG_line            10
//~ #define DBG_oslibgpi        11
//~ #define DBG_region          12
//~ #define DBG_metafile        13
//~ #define DBG_dibitmap        14
//~ #define DBG_blit            15
//~ #define DBG_rgbcvt          16
//~ #define DBG_objhandle       17
//~ #define DBG_transform       18
//~ #define DBG_printer         19
//~ #define DBG_icm             20
//~ #define DBG_trace           21
//~ #define DBG_fontres         22
//~ #define DBG_devcontext      23
#define DBG_MAXFILES        2

extern USHORT DbgEnabledGDIPLUS[DBG_MAXFILES];
extern USHORT DbgEnabledLvl2GDIPLUS[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif

#define dprintf(a)      if(DbgEnabledGDIPLUS[DBG_LOCALLOG] == 1) WriteLog a

#ifdef dprintf2
#undef dprintf2
#endif

#define dprintf2(a)     if(DbgEnabledLvl2GDIPLUS[DBG_LOCALLOG] == 1) WriteLog a

#else

#define ParseLogStatusGDIPLUS()

#endif //DEBUG

#endif
