/* $Id: dbglocal.h,v 1.13 2001-01-23 18:31:26 sandervl Exp $ */

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

#define DBG_ENVNAME        "dbg_kernel32"
#define DBG_ENVNAME_LVL2   "dbg_kernel32_lvl2"

#define DBG_kernel32       0
#define DBG_kobjects       1
#define DBG_console        2
#define DBG_conin          3
#define DBG_conbuffer      4
#define DBG_conout         5
#define DBG_unicode        6
#define DBG_network        7
#define DBG_hmdevio        8
#define DBG_profile        9
#define DBG_thread         10
#define DBG_virtual        11
#define DBG_thunk          12
#define DBG_obsolete       13
#define DBG_comm           14
#define DBG_message        15
#define DBG_resource       16
#define DBG_exceptions     17
#define DBG_heapshared     18
#define DBG_cpuhlp         19
#define DBG_heapcode       20
#define DBG_lfile          21
#define DBG_npipe          22
#define DBG_oslibdos       23
#define DBG_oslibmisc      24
#define DBG_misc           25
#define DBG_exceptutil     26
#define DBG_lang           27
#define DBG_iccio          28
#define DBG_map            29
#define DBG_win32util      30
#define DBG_heap           31
#define DBG_heapstring     32
#define DBG_os2heap        33
#define DBG_vmutex         34
#define DBG_initterm       35
#define DBG_handlemanager  36
#define DBG_environ        37
#define DBG_initsystem     38
#define DBG_hmdevice       39
#define DBG_hmopen32       40
#define DBG_hmobjects      41
#define DBG_hmevent        42
#define DBG_hmmutex        43
#define DBG_hmcomm         44
#define DBG_hmsemaphore    45
#define DBG_wprocess       46
#define DBG_conprop        47
#define DBG_conprop2       48
#define DBG_winimagelx     49
#define DBG_winimagebase   50
#define DBG_windllbase     51
#define DBG_winexebase     52
#define DBG_time           53
#define DBG_mmap           54
#define DBG_winimagepe2lx  55
#define DBG_winimagepeldr  56
#define DBG_windllpe2lx    57
#define DBG_windlllx       58
#define DBG_windllpeldr    59
#define DBG_winexepe2lx    60
#define DBG_winexelx       61
#define DBG_winexepeldr    62
#define DBG_winres         63
#define DBG_critsection    64
#define DBG_pefile         65
#define DBG_winimgres      66
#define DBG_wintls         67
#define DBG_async          68
#define DBG_fileio         69
#define DBG_hmtoken        70
#define DBG_kernelrsrc     71
#define DBG_atom           72
#define DBG_disk           73
#define DBG_directory      74
#define DBG_cvtbitmap      75
#define DBG_hmmmap         76
#define DBG_cvtaccel       77
#define DBG_cvticon        78
#define DBG_cvticongrp     79
#define DBG_oslibexcept    80
#define DBG_cpu            81
#define DBG_process        82
#define DBG_cvtcursor      83
#define DBG_cvtcursorgrp   84
#define DBG_stubs          85
#define DBG_interlock      86
#define DBG_toolhelp       87
#define DBG_codepage       88
#define DBG_debug          89
#define DBG_oslibdebug     90
#define DBG_registry       91
#define DBG_queue          92
#define DBG_hmthread       93
#define DBG_hmprocess      94
#define DBG_VSemaphore     95
#define DBG_exceptstackdump 96
#define DBG_hmfile         97
#define DBG_hmnpipe        98
#define DBG_hmdisk         99
#define DBG_version        100
#define DBG_hmstd          101
#define DBG_MAXFILES       102

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
