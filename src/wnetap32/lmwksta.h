/* $Id */
/*
 * Wrappers for NT/LAN Manager specific data structures
 *
 * Copyright 2000 Patrick Haller (patrick.haller@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
 
#ifndef __LMWKSTA_H__
#define __LMWKSTA_H__


/****************************************************************************
 * OS/2 LAN Server structures                                               *
 ****************************************************************************/
#pragma pack(1)

struct wksta_info_0 {
    unsigned short wki0_reserved_1;
    unsigned long  wki0_reserved_2;
    unsigned char *wki0_root;
    unsigned char *wki0_computername;
    unsigned char *wki0_username;
    unsigned char *wki0_langroup;
    unsigned char  wki0_ver_major;
    unsigned char  wki0_ver_minor;
    unsigned long  wki0_reserved_3;
    unsigned short wki0_charwait;
    unsigned long  wki0_chartime;
    unsigned short wki0_charcount;
    unsigned short wki0_reserved_4;
    unsigned short wki0_reserved_5;
    unsigned short wki0_keepconn;
    unsigned short wki0_keepsearch;
    unsigned short wki0_maxthreads;
    unsigned short wki0_maxcmds;
    unsigned short wki0_reserved_6;
    unsigned short wki0_numworkbuf;
    unsigned short wki0_sizworkbuf;
    unsigned short wki0_maxwrkcache;
    unsigned short wki0_sesstimeout;
    unsigned short wki0_sizerror;
    unsigned short wki0_numalerts;
    unsigned short wki0_numservices;
    unsigned short wki0_errlogsz;
    unsigned short wki0_printbuftime;
    unsigned short wki0_numcharbuf;
    unsigned short wki0_sizcharbuf;
    unsigned char *wki0_logon_server;
    unsigned char *wki0_wrkheuristics;
    unsigned short wki0_mailslots;
};  /* wksta_info_0 */

struct wksta_info_1 {
    unsigned short wki1_reserved_1;
    unsigned long  wki1_reserved_2;
    unsigned char *wki1_root;
    unsigned char *wki1_computername;
    unsigned char *wki1_username;
    unsigned char *wki1_langroup;
    unsigned char  wki1_ver_major;
    unsigned char  wki1_ver_minor;
    unsigned long  wki1_reserved_3;
    unsigned short wki1_charwait;
    unsigned long  wki1_chartime;
    unsigned short wki1_charcount;
    unsigned short wki1_reserved_4;
    unsigned short wki1_reserved_5;
    unsigned short wki1_keepconn;
    unsigned short wki1_keepsearch;
    unsigned short wki1_maxthreads;
    unsigned short wki1_maxcmds;
    unsigned short wki1_reserved_6;
    unsigned short wki1_numworkbuf;
    unsigned short wki1_sizworkbuf;
    unsigned short wki1_maxwrkcache;
    unsigned short wki1_sesstimeout;
    unsigned short wki1_sizerror;
    unsigned short wki1_numalerts;
    unsigned short wki1_numservices;
    unsigned short wki1_errlogsz;
    unsigned short wki1_printbuftime;
    unsigned short wki1_numcharbuf;
    unsigned short wki1_sizcharbuf;
    unsigned char *wki1_logon_server;
    unsigned char *wki1_wrkheuristics;
    unsigned short wki1_mailslots;
    unsigned char *wki1_logon_domain;
    unsigned char *wki1_oth_domains;
    unsigned short wki1_numdgrambuf;
};  /* wksta_info_1 */

struct wksta_info_10 {
    unsigned char *wki10_computername;
    unsigned char *wki10_username;
    unsigned char *wki10_langroup;
    unsigned char  wki10_ver_major;
    unsigned char  wki10_ver_minor;
    unsigned char *wki10_logon_domain;
    unsigned char *wki10_oth_domains;
};  /* wksta_info_10 */

#pragma pack()

/****************************************************************************
 * NT/LAN MANAGER structures                                                *
 ****************************************************************************/
// NetWkstaGetInfo System Information / guest access
typedef struct _WKSTA_INFO_100
{
  DWORD  wki100_platform_id;
  LPWSTR wki100_computername;
  LPWSTR wki100_langroup;
  DWORD  wki100_ver_major;
  DWORD  wki100_ver_minor;
} WKSTA_INFO_100, *PWKSTA_INFO_100, *LPWKSTA_INFO_100;


// NetWkstaGetInfo System Information / user access
typedef struct _WKSTA_INFO_101
{
  DWORD  wki101_platform_id;
  LPWSTR wki101_computername;
  LPWSTR wki101_langroup;
  DWORD  wki101_ver_major;
  DWORD  wki101_ver_minor;
  LPWSTR wki101_lanroot;
} WKSTA_INFO_101, *PWKSTA_INFO_101, *LPWKSTA_INFO_101;


// NetWkstaGetInfo System Information / admin access
typedef struct _WKSTA_INFO_102
{
  DWORD  wki102_platform_id;
  LPWSTR wki102_computername;
  LPWSTR wki102_langroup;
  DWORD  wki102_ver_major;
  DWORD  wki102_ver_minor;
  LPWSTR wki102_lanroot;
  DWORD  wki102_logged_on_users;
} WKSTA_INFO_102, *PWKSTA_INFO_102, *LPWKSTA_INFO_102;

//
// Down-level NetWkstaGetInfo and NetWkstaSetInfo.
//
// DOS specific workstation information -
//    admin or domain operator access
//
typedef struct _WKSTA_INFO_302{
    DWORD   wki302_char_wait;
    DWORD   wki302_collection_time;
    DWORD   wki302_maximum_collection_count;
    DWORD   wki302_keep_conn;
    DWORD   wki302_keep_search;
    DWORD   wki302_max_cmds;
    DWORD   wki302_num_work_buf;
    DWORD   wki302_siz_work_buf;
    DWORD   wki302_max_wrk_cache;
    DWORD   wki302_sess_timeout;
    DWORD   wki302_siz_error;
    DWORD   wki302_num_alerts;
    DWORD   wki302_num_services;
    DWORD   wki302_errlog_sz;
    DWORD   wki302_print_buf_time;
    DWORD   wki302_num_char_buf;
    DWORD   wki302_siz_char_buf;
    LPTSTR  wki302_wrk_heuristics;
    DWORD   wki302_mailslots;
    DWORD   wki302_num_dgram_buf;
}WKSTA_INFO_302, *PWKSTA_INFO_302, *LPWKSTA_INFO_302;

//
// Down-level NetWkstaGetInfo and NetWkstaSetInfo
//
// OS/2 specific workstation information -
//    admin or domain operator access
//
typedef struct _WKSTA_INFO_402{
    DWORD   wki402_char_wait;
    DWORD   wki402_collection_time;
    DWORD   wki402_maximum_collection_count;
    DWORD   wki402_keep_conn;
    DWORD   wki402_keep_search;
    DWORD   wki402_max_cmds;
    DWORD   wki402_num_work_buf;
    DWORD   wki402_siz_work_buf;
    DWORD   wki402_max_wrk_cache;
    DWORD   wki402_sess_timeout;
    DWORD   wki402_siz_error;
    DWORD   wki402_num_alerts;
    DWORD   wki402_num_services;
    DWORD   wki402_errlog_sz;
    DWORD   wki402_print_buf_time;
    DWORD   wki402_num_char_buf;
    DWORD   wki402_siz_char_buf;
    LPTSTR  wki402_wrk_heuristics;
    DWORD   wki402_mailslots;
    DWORD   wki402_num_dgram_buf;
    DWORD   wki402_max_threads;
}WKSTA_INFO_402, *PWKSTA_INFO_402, *LPWKSTA_INFO_402;

//
// Same-level NetWkstaGetInfo and NetWkstaSetInfo.
//
// NT specific workstation information -
//    admin or domain operator access
//
typedef struct _WKSTA_INFO_502{
    DWORD   wki502_char_wait;
    DWORD   wki502_collection_time;
    DWORD   wki502_maximum_collection_count;
    DWORD   wki502_keep_conn;
    DWORD   wki502_max_cmds;
    DWORD   wki502_sess_timeout;
    DWORD   wki502_siz_char_buf;
    DWORD   wki502_max_threads;

    DWORD   wki502_lock_quota;
    DWORD   wki502_lock_increment;
    DWORD   wki502_lock_maximum;
    DWORD   wki502_pipe_increment;
    DWORD   wki502_pipe_maximum;
    DWORD   wki502_cache_file_timeout;
    DWORD   wki502_dormant_file_limit;
    DWORD   wki502_read_ahead_throughput;

    DWORD   wki502_num_mailslot_buffers;
    DWORD   wki502_num_srv_announce_buffers;
    DWORD   wki502_max_illegal_datagram_events;
    DWORD   wki502_illegal_datagram_event_reset_frequency;
    BOOL    wki502_log_election_packets;

    BOOL    wki502_use_opportunistic_locking;
    BOOL    wki502_use_unlock_behind;
    BOOL    wki502_use_close_behind;
    BOOL    wki502_buf_named_pipes;
    BOOL    wki502_use_lock_read_unlock;
    BOOL    wki502_utilize_nt_caching;
    BOOL    wki502_use_raw_read;
    BOOL    wki502_use_raw_write;
    BOOL    wki502_use_write_raw_data;
    BOOL    wki502_use_encryption;
    BOOL    wki502_buf_files_deny_write;
    BOOL    wki502_buf_read_only_files;
    BOOL    wki502_force_core_create_mode;
    BOOL    wki502_use_512_byte_max_transfer;
}WKSTA_INFO_502, *PWKSTA_INFO_502, *LPWKSTA_INFO_502;


#endif
