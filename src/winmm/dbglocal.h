/* $Id: dbglocal.h,v 1.6 2001-07-20 15:35:00 sandervl Exp $ */

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
void ParseLogStatusWINMM();

#define DBG_ENVNAME        "dbg_winmm"
#define DBG_ENVNAME_LVL2   "dbg_winmm_lvl2"

#define DBG_os2timer        0
#define DBG_waveout         1
#define DBG_waveoutdart     2
#define DBG_time            3
#define DBG_wavein          4
#define DBG_auxiliary       5
#define DBG_auxos2          6
#define DBG_mixer           7
#define DBG_midi            8
#define DBG_irtmidi         9
#define DBG_midistrm        10
#define DBG_initterm        11
#define DBG_mci             12
#define DBG_joy             13
#define DBG_mmio            14
#define DBG_driver          15
#define DBG_playsound       16
#define DBG_joyos2          17
#define DBG_resource        18
#define DBG_waveindart      19
#define DBG_waveoutdaud     20
#define DBG_waveoutbase     21
#define DBG_waveinoutbase   22
#define DBG_MAXFILES        23

extern USHORT DbgEnabledWINMM[DBG_MAXFILES];
extern USHORT DbgEnabledLvl2WINMM[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif

#define dprintf(a)      if(DbgEnabledWINMM[DBG_LOCALLOG] == 1) WriteLog a

#ifdef dprintf2
#undef dprintf2
#endif

#define dprintf2(a)     if(DbgEnabledLvl2WINMM[DBG_LOCALLOG] == 1) WriteLog a

#else

#define ParseLogStatusWINMM()

#endif //DEBUG

#endif
