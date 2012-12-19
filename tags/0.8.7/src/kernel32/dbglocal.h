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
void ParseLogStatusKERNEL32();

#define DBG_ENVNAME        "dbg_kernel32"
#define DBG_ENVNAME_LVL2   "dbg_kernel32_lvl2"

#define DBG_kernel32       0
#define DBG_kobjects       1
#define DBG_console        2
#define DBG_conin          3
#define DBG_conbuffer      4
#define DBG_conout         5
#define DBG_network        6
#define DBG_hmdevio        7
#define DBG_profile        8
#define DBG_thread         9
#define DBG_virtual        10
#define DBG_thunk          11
#define DBG_obsolete       12
#define DBG_comm           13
#define DBG_message        14
#define DBG_resource       15
#define DBG_exceptions     16
#define DBG_heapshared     17
#define DBG_cpuhlp         18
#define DBG_heapcode       19
#define DBG_lfile          20
#define DBG_npipe          21
#define DBG_oslibdos       22
#define DBG_oslibmisc      23
#define DBG_misc           24
#define DBG_exceptutil     25
#define DBG_lang           26
#define DBG_iccio          27
#define DBG_map            28
#define DBG_win32util      29
#define DBG_heap           30
#define DBG_heapstring     31
#define DBG_os2heap        32
#define DBG_vmutex         33
#define DBG_initterm       34
#define DBG_handlemanager  35
#define DBG_environ        36
#define DBG_initsystem     37
#define DBG_hmdevice       38
#define DBG_hmopen32       39
#define DBG_hmobjects      40
#define DBG_hmevent        41
#define DBG_hmmutex        42
#define DBG_hmcomm         43
#define DBG_hmsemaphore    44
#define DBG_wprocess       45
#define DBG_conprop        46
#define DBG_conprop2       47
#define DBG_winimagelx     48
#define DBG_winimagebase   49
#define DBG_windllbase     50
#define DBG_winexebase     51
#define DBG_time           52
#define DBG_mmap           53
#define DBG_winimagepe2lx  54
#define DBG_winimagepeldr  55
#define DBG_windllpe2lx    56
#define DBG_windlllx       57
#define DBG_windllpeldr    58
#define DBG_winexepe2lx    59
#define DBG_winexelx       60
#define DBG_winexepeldr    61
#define DBG_winres         62
#define DBG_critsection    63
#define DBG_pefile         64
#define DBG_winimgres      65
#define DBG_wintls         66
#define DBG_async          67
#define DBG_fileio         68
#define DBG_hmtoken        69
#define DBG_kernelrsrc     70
#define DBG_atom           71
#define DBG_disk           72
#define DBG_directory      73
#define DBG_cvtbitmap      74
#define DBG_hmmmap         75
#define DBG_cvtaccel       76
#define DBG_cvticon        77
#define DBG_cvticongrp     78
#define DBG_oslibexcept    79
#define DBG_cpu            80
#define DBG_process        81
#define DBG_cvtcursor      82
#define DBG_cvtcursorgrp   83
#define DBG_stubs          84
#define DBG_interlock      85
#define DBG_toolhelp       86
#define DBG_codepage       87
#define DBG_debug          88
#define DBG_oslibdebug     89
#define DBG_registry       90
#define DBG_queue          91
#define DBG_hmthread       92
#define DBG_hmprocess      93
#define DBG_VSemaphore     94
#define DBG_exceptstackdump 95
#define DBG_hmfile         96
#define DBG_hmnpipe        97
#define DBG_hmdisk         98
#define DBG_version        99
#define DBG_hmstd          100
#define DBG_module         101
#define DBG_mailslot       102
#define DBG_hmmailslot     103
#define DBG_hmparport      104
#define DBG_hmnul          105
#define DBG_overlappedio   106
#define DBG_trace          107
#define DBG_event          108
#define DBG_mutex          109
#define DBG_semaphore      110
#define DBG_nls            111
#define DBG_memory         112
#define DBG_system         113
#define DBG_string         114
#define DBG_char           115
#define DBG_osliblvm       116
#define DBG_oslibtime      117
#define DBG_conodin32      118
#define DBG_oslibmem       119
#define DBG_mmapview       120
#define DBG_mmapdup        121
#define DBG_oslibthread    122
#define DBG_mmapnotify     123
#define DBG__ras           124
#define DBG_MAXFILES       125

extern USHORT DbgEnabledKERNEL32[DBG_MAXFILES];
extern USHORT DbgEnabledLvl2KERNEL32[DBG_MAXFILES];

#ifdef dprintf
#undef dprintf
#endif

#define dprintf(a)      if(DbgEnabledKERNEL32[DBG_LOCALLOG] == 1) WriteLog a

#ifdef dprintf2
#undef dprintf2
#endif

#define dprintf2(a)     if(DbgEnabledLvl2KERNEL32[DBG_LOCALLOG] == 1) WriteLog a

#else

#define ParseLogStatusKERNEL32()

#endif //DEBUG

#endif
