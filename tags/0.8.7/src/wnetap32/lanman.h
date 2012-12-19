/* $Id: lanman.h,v 1.2 2001-09-06 22:23:39 phaller Exp $ */
/*
 * Wrappers for NT/LAN Manager specific data structures
 *
 * Copyright 2000 Patrick Haller (patrick.haller@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
 
#ifndef __LANMAN_H__
#define __LANMAN_H__


/****************************************************************************
 * OS/2 LAN Server structures                                               *
 ****************************************************************************/
#pragma pack(1)


#define CNLEN           15                  /* Computer name length     */
#define UNCLEN          (CNLEN+2)           /* UNC computer name length */
#define NNLEN           12                  /* 8.3 Net name length      */
#define RMLEN           (UNCLEN+1+NNLEN)    /* Maximum remote name length */


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


struct statistics_info_0 {
    unsigned long   st0_start;        /* time statistics collection started   */
    unsigned long   st0_wknumNCBs;    /* # workstation NCBs issued            */
    unsigned long   st0_wkfiNCBs;     /* # workstation NCBs failed issue      */
    unsigned long   st0_wkfcNCBs;     /* # workstation NCBs failed completion */
    unsigned long   st0_wksesstart;   /* # workstation sessions started       */
    unsigned long   st0_wksessfail;   /* # workstation session failures       */
    unsigned long   st0_wkuses;       /* # workstation uses                   */
    unsigned long   st0_wkusefail;    /* # workstation use failures           */
    unsigned long   st0_wkautorec;    /* # workstation auto-reconnects        */
    unsigned long   st0_rdrnumNCBs;   /* # redir NCBs issued                  */
    unsigned long   st0_srvnumNCBs;   /* # NCBs issued for the server         */
    unsigned long   st0_usrnumNCBs;   /* # user NCBs issued                   */
    unsigned long   st0_reserved4;    /* reserved for future use              */
    unsigned long   st0_reserved5;    /* reserved for future use              */
    unsigned long   st0_reserved6;    /* reserved for future use              */
    unsigned long   st0_reserved7;    /* reserved for future use              */
    unsigned long   st0_reserved8;    /* reserved for future use              */
    unsigned long   st0_svfopens;     /* # of server file opens               */
    unsigned long   st0_svdevopens;   /* # of server device opens             */
    unsigned long   st0_svjobsqueued; /* # of server print jobs spooled       */
    unsigned long   st0_svsopens;     /* # of server session starts           */
    unsigned long   st0_svstimedout;  /* # of server session auto-disconnects */
    unsigned long   st0_svserrorout;  /* # of server sessions errored out     */
    unsigned long   st0_svpwerrors;   /* # of server password violations      */
    unsigned long   st0_svpermerrors; /* # of server access permission errors */
    unsigned long   st0_svsyserrors;  /* # of server system errors            */
    unsigned long   st0_svbytessent;  /* # of server bytes sent to net        */
    unsigned long   st0_svbytesrcvd;  /* # of server bytes received from net  */
    unsigned long   st0_svavresponse; /* average server response time in msec */
}; /* statistics_info_0 */



struct stat_workstation_0  {
        unsigned long  stw0_start;
        unsigned long  stw0_numNCB_r;
        unsigned long  stw0_numNCB_s;
        unsigned long  stw0_numNCB_a;
        unsigned long  stw0_fiNCB_r;
        unsigned long  stw0_fiNCB_s;
        unsigned long  stw0_fiNCB_a;
        unsigned long  stw0_fcNCB_r;
        unsigned long  stw0_fcNCB_s;
        unsigned long  stw0_fcNCB_a;
        unsigned long  stw0_sesstart;
        unsigned long  stw0_sessfailcon;
        unsigned long  stw0_sessbroke;
        unsigned long  stw0_uses;
        unsigned long  stw0_usefail;
        unsigned long  stw0_autorec;
        unsigned long  stw0_bytessent_r_lo;
        unsigned long  stw0_bytessent_r_hi;
        unsigned long  stw0_bytesrcvd_r_lo;
        unsigned long  stw0_bytesrcvd_r_hi;
        unsigned long  stw0_bytessent_s_lo;
        unsigned long  stw0_bytessent_s_hi;
        unsigned long  stw0_bytesrcvd_s_lo;
        unsigned long  stw0_bytesrcvd_s_hi;
        unsigned long  stw0_bytessent_a_lo;
        unsigned long  stw0_bytessent_a_hi;
        unsigned long  stw0_bytesrcvd_a_lo;
        unsigned long  stw0_bytesrcvd_a_hi;
        unsigned long  stw0_reqbufneed;
        unsigned long  stw0_bigbufneed;
}; /* stat_workstation_0 */


struct stat_server_0  {
        unsigned long  sts0_start;
        unsigned long  sts0_fopens;
        unsigned long  sts0_devopens;
        unsigned long  sts0_jobsqueued;
        unsigned long  sts0_sopens;
        unsigned long  sts0_stimedout;
        unsigned long  sts0_serrorout;
        unsigned long  sts0_pwerrors;
        unsigned long  sts0_permerrors;
        unsigned long  sts0_syserrors;
        unsigned long  sts0_bytessent_low;
        unsigned long  sts0_bytessent_high;
        unsigned long  sts0_bytesrcvd_low;
        unsigned long  sts0_bytesrcvd_high;
        unsigned long  sts0_avresponse;
        unsigned long  sts0_reqbufneed;
        unsigned long  sts0_bigbufneed;
}; /* stat_server_0 */


struct server_info_1 {
  unsigned char         sv1_name[CNLEN + 1];
  unsigned char         sv1_version_major;
  unsigned char         sv1_version_minor;
  unsigned long         sv1_type;
  unsigned char *       sv1_comment;
}; /* server_info_1 */


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


typedef struct _STAT_WORKSTATION_LM_0 {
     DWORD          stw0_start;
     DWORD          stw0_numNCB_r;
     DWORD          stw0_numNCB_s;
     DWORD          stw0_numNCB_a;
     DWORD          stw0_fiNCB_r;
     DWORD          stw0_fiNCB_s;
     DWORD          stw0_fiNCB_a;
     DWORD          stw0_fcNCB_r;
     DWORD          stw0_fcNCB_s;
     DWORD          stw0_fcNCB_a;
     DWORD          stw0_sesstart;
     DWORD          stw0_sessfailcon;
     DWORD          stw0_sessbroke;
     DWORD          stw0_uses;
     DWORD          stw0_usefail;
     DWORD          stw0_autorec;
     DWORD          stw0_bytessent_r_lo;
     DWORD          stw0_bytessent_r_hi;
     DWORD          stw0_bytesrcvd_r_lo;
     DWORD          stw0_bytesrcvd_r_hi;
     DWORD          stw0_bytessent_s_lo;
     DWORD          stw0_bytessent_s_hi;
     DWORD          stw0_bytesrcvd_s_lo;
     DWORD          stw0_bytesrcvd_s_hi;
     DWORD          stw0_bytessent_a_lo;
     DWORD          stw0_bytessent_a_hi;
     DWORD          stw0_bytesrcvd_a_lo;
     DWORD          stw0_bytesrcvd_a_hi;
     DWORD          stw0_reqbufneed;
     DWORD          stw0_bigbufneed;
} STAT_WORKSTATION_LM_0, *PSTAT_WORKSTATION_LM_0, *LPSTAT_WORKSTATION_LM_0;


typedef struct _STAT_WORKSTATION_NT_0 {
    LARGE_INTEGER   StatisticsStartTime;

    LARGE_INTEGER   BytesReceived;
    LARGE_INTEGER   SmbsReceived;
    LARGE_INTEGER   PagingReadBytesRequested;
    LARGE_INTEGER   NonPagingReadBytesRequested;
    LARGE_INTEGER   CacheReadBytesRequested;
    LARGE_INTEGER   NetworkReadBytesRequested;

    LARGE_INTEGER   BytesTransmitted;
    LARGE_INTEGER   SmbsTransmitted;
    LARGE_INTEGER   PagingWriteBytesRequested;
    LARGE_INTEGER   NonPagingWriteBytesRequested;
    LARGE_INTEGER   CacheWriteBytesRequested;
    LARGE_INTEGER   NetworkWriteBytesRequested;

    DWORD           InitiallyFailedOperations;
    DWORD           FailedCompletionOperations;

    DWORD           ReadOperations;
    DWORD           RandomReadOperations;
    DWORD           ReadSmbs;
    DWORD           LargeReadSmbs;
    DWORD           SmallReadSmbs;

    DWORD           WriteOperations;
    DWORD           RandomWriteOperations;
    DWORD           WriteSmbs;
    DWORD           LargeWriteSmbs;
    DWORD           SmallWriteSmbs;

    DWORD           RawReadsDenied;
    DWORD           RawWritesDenied;

    DWORD           NetworkErrors;

    //  Connection/Session counts
    DWORD           Sessions;
    DWORD           FailedSessions;
    DWORD           Reconnects;
    DWORD           CoreConnects;
    DWORD           Lanman20Connects;
    DWORD           Lanman21Connects;
    DWORD           LanmanNtConnects;
    DWORD           ServerDisconnects;
    DWORD           HungSessions;
    DWORD           UseCount;
    DWORD           FailedUseCount;

    //
    //  Queue Lengths (updates protected by RdrMpxTableSpinLock NOT
    //  RdrStatisticsSpinlock)
    //

    DWORD           CurrentCommands;

} STAT_WORKSTATION_NT_0, *PSTAT_WORKSTATION_NT_0, *LPSTAT_WORKSTATION_NT_0;


typedef struct _STAT_SERVER_0 {
     DWORD          sts0_start;
     DWORD          sts0_fopens;
     DWORD          sts0_devopens;
     DWORD          sts0_jobsqueued;
     DWORD          sts0_sopens;
     DWORD          sts0_stimedout;
     DWORD          sts0_serrorout;
     DWORD          sts0_pwerrors;
     DWORD          sts0_permerrors;
     DWORD          sts0_syserrors;
     DWORD          sts0_bytessent_low;
     DWORD          sts0_bytessent_high;
     DWORD          sts0_bytesrcvd_low;
     DWORD          sts0_bytesrcvd_high;
     DWORD          sts0_avresponse;
     DWORD          sts0_reqbufneed;
     DWORD          sts0_bigbufneed;
} STAT_SERVER_0, *PSTAT_SERVER_0, *LPSTAT_SERVER_0;


typedef struct _SERVER_INFO_100 {
     DWORD          sv100_platform_id;
     LPWSTR         sv100_name;
} SERVER_INFO_100, *PSERVER_INFO_100, *LPSERVER_INFO_100;


typedef struct _SERVER_INFO_101 {
     DWORD          sv101_platform_id;
     LPWSTR         sv101_name;
     DWORD          sv101_version_major;
     DWORD          sv101_version_minor;
     DWORD          sv101_type;
     LPWSTR         sv101_comment;
} SERVER_INFO_101, *PSERVER_INFO_101, *LPSERVER_INFO_101;


#endif
