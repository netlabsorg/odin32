/* $Id*/
/*
 * Wrappers for NT/LAN Manager specific data structures
 *
 * Copyright 2000 Patrick Haller (patrick.haller@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
 
#ifndef __WNETAP32_H__
#define __WNETAP32_H__


/****************************************************************************
 * OS/2 LAN Server structures                                               *
 ****************************************************************************/
#pragma pack(1)

#define NCBNAMSZ_w        16
#define MAX_LANA_w       254

typedef struct _NCB_w {
    UCHAR   ncb_command;
    UCHAR   ncb_retcode;
    UCHAR   ncb_lsn;
    UCHAR   ncb_num;
    PUCHAR  ncb_buffer;
    WORD    ncb_length;
    UCHAR   ncb_callname[NCBNAMSZ_w];
    UCHAR   ncb_name[NCBNAMSZ_w];
    UCHAR   ncb_rto;
    UCHAR   ncb_sto;
    void (* CALLBACK ncb_post)( struct _NCB_w * );
    UCHAR   ncb_lana_num;
    UCHAR   ncb_cmd_cplt;
    UCHAR   ncb_reserve[10];
    HANDLE  ncb_event;
} NCB_w, *PNCB_w;


typedef struct _LANA_ENUM_w
{
  UCHAR length;
  UCHAR lana[ MAX_LANA_w ];
} LANA_ENUM_w, *PLANA_ENUM_w;


/* NCB command codes:
 * "!" marked values have no direct OS/2 correspondance, all
 * other values and return codes are identical as defined
 * for NetBIOS 3.0
 */

#define NCBCALL_w         0x10            /* NCB CALL                           */
#define NCBLISTEN_w       0x11            /* NCB LISTEN                         */
#define NCBHANGUP_w       0x12            /* NCB HANG UP                        */
#define NCBSEND_w         0x14            /* NCB SEND                           */
#define NCBRECV_w         0x15            /* NCB RECEIVE                        */
#define NCBRECVANY_w      0x16            /* NCB RECEIVE ANY                    */
#define NCBCHAINSEND_w    0x17            /* NCB CHAIN SEND                     */
#define NCBDGSEND_w       0x20            /* NCB SEND DATAGRAM                  */
#define NCBDGRECV_w       0x21            /* NCB RECEIVE DATAGRAM               */
#define NCBDGSENDBC_w     0x22            /* NCB SEND BROADCAST DATAGRAM        */
#define NCBDGRECVBC_w     0x23            /* NCB RECEIVE BROADCAST DATAGRAM     */
#define NCBADDNAME_w      0x30            /* NCB ADD NAME                       */
#define NCBDELNAME_w      0x31            /* NCB DELETE NAME                    */
#define NCBRESET_w        0x32            /* NCB RESET                          */
#define NCBASTAT_w        0x33            /* NCB ADAPTER STATUS                 */
#define NCBSSTAT_w        0x34            /* NCB SESSION STATUS                 */
#define NCBCANCEL_w       0x35            /* NCB CANCEL                         */
#define NCBADDGRNAME_w    0x36            /* NCB ADD GROUP NAME                 */
#define NCBENUM_w         0x37            /* NCB ENUMERATE LANA NUMBERS        !*/
#define NCBUNLINK_w       0x70            /* NCB UNLINK                         */
#define NCBSENDNA_w       0x71            /* NCB SEND NO ACK                    */
#define NCBCHAINSENDNA_w  0x72            /* NCB CHAIN SEND NO ACK              */
#define NCBLANSTALERT_w   0x73            /* NCB LAN STATUS ALERT              !*/
#define NCBACTION_w       0x77            /* NCB ACTION                        !*/
#define NCBFINDNAME_w     0x78            /* NCB FIND NAME                     !*/
#define NCBTRACE_w        0x79            /* NCB TRACE                         !*/

#define ASYNCH_w          0x80            /* high bit set == asynchronous       */


/* NCB return codes
 * // commented values do exist in OS/2 but not on Win32
 * ! marked values do exist in Win32 but not on OS/2
 */
#define NRC_GOODRET_w     0x00
#define NRC_BUFLEN_w      0x01
// NRC_BFULL
#define NRC_ILLCMD_w      0x03
#define NRC_CMDTMO_w      0x05
#define NRC_INCOMP_w      0x06
#define NRC_BADDR_w       0x07
#define NRC_SNUMOUT_w     0x08
#define NRC_NORES_w       0x09
#define NRC_SCLOSED_w     0x0a
#define NRC_CMDCAN_w      0x0b
// NRC_DMAFAIL
#define NRC_DUPNAME_w     0x0d
#define NRC_NAMTFUL_w     0x0e
#define NRC_ACTSES_w      0x0f
// NRC_INVALID
#define NRC_LOCTFUL_w     0x11
#define NRC_REMTFUL_w     0x12
#define NRC_ILLNN_w       0x13
#define NRC_NOCALL_w      0x14
#define NRC_NOWILD_w      0x15
#define NRC_INUSE_w       0x16
#define NRC_NAMERR_w      0x17
#define NRC_SABORT_w      0x18
#define NRC_NAMCONF_w     0x19
#define NRC_IFBUSY_w      0x21
#define NRC_TOOMANY_w     0x22
#define NRC_BRIDGE_w      0x23
#define NRC_CANOCCR_w     0x24
// NRC_RESNAME
#define NRC_CANCEL_w      0x26
#define NRC_DUPENV_w      0x30 /* ! */
// NRC_MULT
#define NRC_ENVNOTDEF_w   0x34 /* ! */
#define NRC_OSRESNOTAV_w  0x35 /* ! */
#define NRC_MAXAPPS_w     0x36
#define NRC_NOSAPS_w      0x37 /* ! */
#define NRC_NORESOURCES_w 0x38
#define NRC_INVADDRESS_w  0x39 /* ! */
#define NRC_INVDDID_w     0x3B /* ! */
#define NRC_LOCKFAIL_w    0x3C /* ! */
#define NRC_OPENERR_w     0x3f /* ! */
#define NRC_SYSTEM_w      0x40
// NRC_ROM
// NRC_RAM
// NRC_DLF
// NRC_ALF
// NRC_IFAIL
// NRC_NET_STAT
// NRC_ADPTMALFN


#define NRC_PENDING_w     0xff


UCHAR OSLibNetBIOS(PNCB_w pncb);
UCHAR OSLibNetBIOSEnum(PNCB_w pncb);


#endif
