/* $Id: wnetap32.cpp,v 1.1 1999-05-24 20:20:08 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * NETAPI32 stubs
 *
 * Copyright 1998 Patrick Haller
 *
 */
/*****************************************************************************
 * Name      : WNETAP32.CPP
 * Purpose   : This module maps all Win32 functions contained in NETAPI32.DLL
 *             to their OS/2-specific counterparts as far as possible.
 *             Basis is  5.05.97  12.00  59152 MPR.DLL (NT4SP3)
 *****************************************************************************/


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include "misc.h"


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

#undef NET_API_STATUS
#define NET_API_STATUS DWORD

#define NERR_BASE 1 /* @@@PH DUMMY ! */

#define NCBNAMSZ        16
#define MAX_LANA       254

typedef struct _NCB {
    UCHAR   ncb_command;
    UCHAR   ncb_retcode;
    UCHAR   ncb_lsn;
    UCHAR   ncb_num;
    PUCHAR  ncb_buffer;
    WORD    ncb_length;
    UCHAR   ncb_callname[NCBNAMSZ];
    UCHAR   ncb_name[NCBNAMSZ];
    UCHAR   ncb_rto;
    UCHAR   ncb_sto;
    void (* CALLBACK ncb_post)( struct _NCB * );
    UCHAR   ncb_lana_num;
    UCHAR   ncb_cmd_cplt;
    UCHAR   ncb_reserve[10];
    HANDLE  ncb_event;
} NCB, *PNCB;

#define NRC_GOODRET     0x00
#define NRC_BUFLEN      0x01
#define NRC_ILLCMD      0x03
#define NRC_CMDTMO      0x05
#define NRC_INCOMP      0x06
#define NRC_BADDR       0x07
#define NRC_SNUMOUT     0x08
#define NRC_NORES       0x09
#define NRC_SCLOSED     0x0a
#define NRC_CMDCAN      0x0b
#define NRC_DUPNAME     0x0d
#define NRC_NAMTFUL     0x0e
#define NRC_ACTSES      0x0f
#define NRC_LOCTFUL     0x11
#define NRC_REMTFUL     0x12
#define NRC_ILLNN       0x13
#define NRC_NOCALL      0x14
#define NRC_NOWILD      0x15
#define NRC_INUSE       0x16
#define NRC_NAMERR      0x17
#define NRC_SABORT      0x18
#define NRC_NAMCONF     0x19
#define NRC_IFBUSY      0x21
#define NRC_TOOMANY     0x22
#define NRC_BRIDGE      0x23
#define NRC_CANOCCR     0x24
#define NRC_CANCEL      0x26
#define NRC_DUPENV      0x30
#define NRC_ENVNOTDEF   0x34
#define NRC_OSRESNOTAV  0x35
#define NRC_MAXAPPS     0x36
#define NRC_NOSAPS      0x37
#define NRC_NORESOURCES 0x38
#define NRC_INVADDRESS  0x39
#define NRC_INVDDID     0x3B
#define NRC_LOCKFAIL    0x3C
#define NRC_OPENERR     0x3f
#define NRC_SYSTEM      0x40

#define NRC_PENDING     0xff


//******************************************************************************
//******************************************************************************
UCHAR WIN32API OS2Netbios(PNCB pncb)
{
#ifdef DEBUG
  WriteLog("OS2Netbios; pretend no network available\n");
#endif
  return(NRC_OPENERR);
}
//******************************************************************************
//******************************************************************************

 /*****************************************************************************
 * Name      : NET_API_STATUS NetAlertRaise
 * Purpose   :
 * Parameters: LPWSTR AlertEventName Pointer to a Unicode string that specifies
 *                                  the interrupting message to raise
 *             LPVOID Buffer        Pointer to the data to be sent to the clients
 *                                  listening for this interrupting message
 *             DWORD BufferSize     Specifies in bytes, the size of the buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 19:40:30]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetAlertRaise(
                                          LPWSTR AlertEventName,
                                          LPVOID Buffer,
                                          DWORD BufferSize
                                          )

{

  dprintf(("NETAPI32: NetAlertRaise(%s, %08x, %d) not implemented\n"
           ,AlertEventName, Buffer, BufferSize
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetAlertRaiseEx
 * Purpose   :
 * Parameters: LPWSTR AlertEventName
 *             LPVOID VariableInfo
 *             DWORD VariableInfoSize
 *             LPWSTR ServiceName
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 19:44:43]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetAlertRaiseEx(
                                            LPWSTR AlertEventName,
                                            LPVOID VariableInfo,
                                            DWORD VariableInfoSize,
                                            LPWSTR ServiceName
                                            )

{

  dprintf(("NETAPI32: NetAlertRaiseEx(%s, %08x, %d, %s) not implemented\n"
           ,AlertEventName, VariableInfo, VariableInfoSize, ServiceName
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetApiBufferAllocate
 * Purpose   :
 * Parameters: DWORD ByteCount
 *             LPVOID *Buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 19:46:46]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetApiBufferAllocate(
                                                DWORD ByteCount,
                                                LPVOID *Buffer
                                                )

{

  dprintf(("NETAPI32: NetApiBufferAllocate(%d, %08x) not implemented\n"
           ,ByteCount, *Buffer
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetApiBufferFree
 * Purpose   :
 * Parameters: LPVOID Buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 19:47:21]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetApiBufferFree(
                                            LPVOID Buffer
                                            )

{

  dprintf(("NETAPI32: NetApiBufferFree(%08x) not implemented\n"
           ,Buffer
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetApiBufferReallocate
 * Purpose   :
 * Parameters: LPVOID OldBuffer
 *             DWORD NewByteCount
 *             LPVOID NewBuffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:25:21]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetApiBufferReallocate(
                                                  LPVOID OldBuffer,
                                                  DWORD NewByteCount,
                                                  LPVOID NewBuffer
                                                  )

{

  dprintf(("NETAPI32: NetApiBufferReallocate(%08x, %d, %08x) not implemented\n"
           ,OldBuffer, NewByteCount, NewBuffer
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetApiBufferSize
 * Purpose   :
 * Parameters: LPVOID buffer
 *             DWORD ByteCount
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:25:44]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetApiBufferSize(
                                            LPVOID buffer,
                                            DWORD ByteCount
                                            )

{

  dprintf(("NETAPI32: NetApiBufferSize(%08x, %d) not implemented\n"
           ,buffer, ByteCount
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetFileEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR basepath
 *             LPWSTR username
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resume_handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:27:44]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetFileEnum(
                                        LPWSTR servername,
                                        LPWSTR basepath,
                                        LPWSTR username,
                                        DWORD level,
                                        LPBYTE *bufptr,
                                        DWORD prefmaxlen,
                                        LPDWORD entriesread,
                                        LPDWORD totalentries,
                                        LPDWORD resume_handle
                                        )

{

  dprintf(("NETAPI32: NetFileEnum(%s, %s, %s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, basepath, username, level, *bufptr, prefmaxlen, entriesread, totalentries, resume_handle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetFileGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD fileid
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:28:38]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetFileGetInfo(
                                           LPWSTR servername,
                                           DWORD fileid,
                                           DWORD level,
                                           LPBYTE *bufptr
                                           )

{

  dprintf(("NETAPI32: NetFileGetInfo(%s, %d, %d, %08x) not implemented\n"
           ,servername, fileid, level, *bufptr
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetGetAnyDCName
 * Purpose   :
 * Parameters: LPCWSTR ServerName
 *             LPCWSTR DomainName
 *             LPBYTE *Buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:29:52]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGetAnyDCName(
                                           LPCWSTR ServerName,
                                           LPCWSTR DomainName,
                                           LPBYTE *Buffer
                                           )

{

  dprintf(("NETAPI32: NetGetAnyDCName(%08x, %08x, %08x) not implemented\n"
           ,ServerName, DomainName, *Buffer
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGetDCName
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR domainname
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:30:29]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGetDCName(
                                        LPWSTR servername,
                                        LPWSTR domainname,
                                        LPBYTE *bufptr
                                        )

{

  dprintf(("NETAPI32: NetGetDCName(%08x, %08x, %08x) not implemented\n"
           ,servername, domainname, *bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGetDisplayInformationIndex
 * Purpose   :
 * Parameters: LPWSTR ServerName  pointer to server to get information from
 *             DWORD Level  level of information to retrieve
 *             LPWSTR Prefix  pointer to prefix string
 *             LPDWORD Index  receives index of entry
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:30:53]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGetDisplayInformationIndex(
                                                         LPWSTR ServerName,
                                                         DWORD Level,
                                                         LPWSTR Prefix,
                                                         LPDWORD Index
                                                         )

{

  dprintf(("NETAPI32: NetGetDisplayInformationIndex(%08x, %d, %08x, %08x) not implemented\n"
           ,ServerName, Level, Prefix, Index
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:31:17]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupAdd(
                                       LPWSTR servername,
                                       DWORD level,
                                       LPBYTE buf,
                                       LPDWORD parm_err
                                       )

{

  dprintf(("NETAPI32: NetGroupAdd(%08x, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupAddUser
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR GroupName
 *             LPWSTR username
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:31:38]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupAddUser(
                                           LPWSTR servername,
                                           LPWSTR GroupName,
                                           LPWSTR username
                                           )

{

  dprintf(("NETAPI32: NetGroupAddUser(%08x, %08x, %08x) not implemented\n"
           ,servername, GroupName, username
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR groupname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:31:53]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupDel(
                                       LPWSTR servername,
                                       LPWSTR groupname
                                       )

{

  dprintf(("NETAPI32: NetGroupDel(%08x, %08x) not implemented\n"
           ,servername, groupname
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupDelUser
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR GroupName
 *             LPWSTR Username
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:32:05]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupDelUser(
                                           LPWSTR servername,
                                           LPWSTR GroupName,
                                           LPWSTR Username
                                           )

{

  dprintf(("NETAPI32: NetGroupDelUser(%08x, %08x, %08x) not implemented\n"
           ,servername, GroupName, Username
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resume_handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:32:38]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupEnum(
                                        LPWSTR servername,
                                        DWORD level,
                                        LPBYTE *bufptr,
                                        DWORD prefmaxlen,
                                        LPDWORD entriesread,
                                        LPDWORD totalentries,
                                        LPDWORD resume_handle
                                        )

{

  dprintf(("NETAPI32: NetGroupEnum(%08x, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resume_handle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR groupname
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:33:13]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupGetInfo(
                                           LPWSTR servername,
                                           LPWSTR groupname,
                                           DWORD level,
                                           LPBYTE *bufptr
                                           )

{

  dprintf(("NETAPI32: NetGroupGetInfo(%08x, %08x, %d, %08x) not implemented\n"
           ,servername, groupname, level, *bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupGetUsers
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR groupname
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resumeHandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:33:24]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupGetUsers(
                                            LPWSTR servername,
                                            LPWSTR groupname,
                                            DWORD level,
                                            LPBYTE *bufptr,
                                            DWORD prefmaxlen,
                                            LPDWORD entriesread,
                                            LPDWORD totalentries,
                                            LPDWORD resumeHandle
                                            )

{

  dprintf(("NETAPI32: NetGroupGetUsers(%08x, %08x, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, groupname, level, *bufptr, prefmaxlen, entriesread, totalentries, resumeHandle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR groupname
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:33:39]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupSetInfo(
                                           LPWSTR servername,
                                           LPWSTR groupname,
                                           DWORD level,
                                           LPBYTE buf,
                                           LPDWORD parm_err
                                           )

{

  dprintf(("NETAPI32: NetGroupSetInfo(%08x, %08x, %d, %08x, %08x) not implemented\n"
           ,servername, groupname, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetGroupSetUsers
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR groupname
 *             DWORD level
 *             LPBYTE buf
 *             DWORD NewMemberCount
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:34:02]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetGroupSetUsers(
                                            LPWSTR servername,
                                            LPWSTR groupname,
                                            DWORD level,
                                            LPBYTE buf,
                                            DWORD NewMemberCount
                                            )

{

  dprintf(("NETAPI32: NetGroupSetUsers(%08x, %08x, %d, %08x, %d) not implemented\n"
           ,servername, groupname, level, buf, NewMemberCount
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetHandleGetInfo
 * Purpose   :
 * Parameters: UNSIGNED SHORT hHandle
 *             SHORT sLevel
 *             CHAR FAR *pbBuffer
 *             UNSIGNED SHORT cbBuffer
 *             UNSIGNED SHORT FAR *pcbTotalAvail
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:35:03]
 *****************************************************************************/
/*  NOT in DLL but in SDK Documentation
NET_API_STATUS WIN32API OS2NetHandleGetInfo(
                                            UNSIGNED SHORT hHandle,
                                            SHORT sLevel,
                                            CHAR FAR *pbBuffer,
                                            UNSIGNED SHORT cbBuffer,
                                            UNSIGNED SHORT FAR *pcbTotalAvail
                                            )

{

  dprintf(("NETAPI32: NetHandleGetInfo(%d, %d, %08x, %d, %08x) not implemented\n"
           ,hHandle, sLevel, *pbBuffer, cbBuffer, *pcbTotalAvail
         ));

  return (NERR_BASE);
}
*/

 /*****************************************************************************
 * Name      : NET_API_STATUS NetHandleSetInfo
 * Purpose   :
 * Parameters: UNSIGNED SHORT hHandle
 *             SHORT sLevel
 *             CHAR FAR *pbBuffer
 *             UNSIGNED SHORT cbBuffer
 *             UNSIGNED SHORT FAR *sParmNum
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:39:08]
 *****************************************************************************/
/* Not in DLL but in SDK Documentation
NET_API_STATUS WIN32API OS2NetHandleSetInfo(
                                            UNSIGNED SHORT hHandle,
                                            SHORT sLevel,
                                            CHAR FAR *pbBuffer,
                                            UNSIGNED SHORT cbBuffer,
                                            UNSIGNED SHORT FAR *sParmNum
                                            )

{

  dprintf(("NETAPI32: NetHandleSetInfo(%d, %d, %08x, %d, %08x) not implemented\n"
           ,hHandle, sLevel, *pbBuffer, cbBuffer, *sParmNum
         ));

  return (NERR_BASE);
}

*/
 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:40:13]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupAdd(
                                            LPWSTR servername,
                                            DWORD level,
                                            LPBYTE buf,
                                            LPDWORD parm_err
                                            )
{

  dprintf(("NETAPI32: NetLocalGroupAdd(%08x, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupAddMembers
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR LocalGroupName
 *             DWORD level
 *             LPBYTE buf
 *             DWORD membercount
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:40:51]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupAddMembers(
                                                   LPWSTR servername,
                                                   LPWSTR LocalGroupName,
                                                   DWORD level,
                                                   LPBYTE buf,
                                                   DWORD membercount
                                                   )

{

  dprintf(("NETAPI32: NetLocalGroupAddMembers(%08x, %08x, %d, %08x, %d) not implemented\n"
           ,servername, LocalGroupName, level, buf, membercount
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR LocalGroupName
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:41:03]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupDel(
                                            LPWSTR servername,
                                            LPWSTR LocalGroupName
                                            )

{

  dprintf(("NETAPI32: NetLocalGroupDel(%08x, %08x) not implemented\n"
           ,servername, LocalGroupName
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupDelMembers
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR LocalGroupName
 *             DWORD level
 *             LPBYTE buf
 *             DWORD membercount
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:41:33]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupDelMembers(
                                                   LPWSTR servername,
                                                   LPWSTR LocalGroupName,
                                                   DWORD level,
                                                   LPBYTE buf,
                                                   DWORD membercount
                                                   )

{

  dprintf(("NETAPI32: NetLocalGroupDelMembers(%08x, %08x, %d, %08x, %d) not implemented\n"
           ,servername, LocalGroupName, level, buf, membercount
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resumehandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:41:49]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupEnum(
                                             LPWSTR servername,
                                             DWORD level,
                                             LPBYTE *bufptr,
                                             DWORD prefmaxlen,
                                             LPDWORD entriesread,
                                             LPDWORD totalentries,
                                             LPDWORD resumehandle
                                             )

{

  dprintf(("NETAPI32: NetLocalGroupEnum(%08x, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR LocalGroupName
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:42:02]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupGetInfo(
                                                LPWSTR servername,
                                                LPWSTR LocalGroupName,
                                                DWORD level,
                                                LPBYTE *bufptr
                                                )

{

  dprintf(("NETAPI32: NetLocalGroupGetInfo(%08x, %08x, %d, %08x) not implemented\n"
           ,servername, LocalGroupName, level, *bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupGetMembers
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR localgroupname
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resumehandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:42:16]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupGetMembers(
                                                   LPWSTR servername,
                                                   LPWSTR localgroupname,
                                                   DWORD level,
                                                   LPBYTE *bufptr,
                                                   DWORD prefmaxlen,
                                                   LPDWORD entriesread,
                                                   LPDWORD totalentries,
                                                   LPDWORD resumehandle
                                                   )

{

  dprintf(("NETAPI32: NetLocalGroupGetMembers(%08x, %08x, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, localgroupname, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR LocalGroupName
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:42:27]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupSetInfo(
                                                LPWSTR servername,
                                                LPWSTR LocalGroupName,
                                                DWORD level,
                                                LPBYTE buf,
                                                LPDWORD parm_err
                                                )

{

  dprintf(("NETAPI32: NetLocalGroupSetInfo(%08x, %08x, %d, %08x, %08x) not implemented\n"
           ,servername, LocalGroupName, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupSetMembers
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR LocalGroupName
 *             DWORD level
 *             LPBYTE buf
 *             DWORD totalentries
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:42:49]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetLocalGroupSetMembers(
                                                   LPWSTR servername,
                                                   LPWSTR LocalGroupName,
                                                   DWORD level,
                                                   LPBYTE buf,
                                                   DWORD totalentries
                                                   )

{

  dprintf(("NETAPI32: NetLocalGroupSetMembers(%08x, %08x, %d, %08x, %d) not implemented\n"
           ,servername, LocalGroupName, level, buf, totalentries
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetMessageBufferSend
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR msgname
 *             LPWSTR fromname
 *             LPBYTE buf
 *             DWORD buflen
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:43:01]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetMessageBufferSend(
                                                 LPWSTR servername,
                                                 LPWSTR msgname,
                                                 LPWSTR fromname,
                                                 LPBYTE buf,
                                                 DWORD buflen
                                                 )

{

  dprintf(("NETAPI32: NetMessageBufferSend(%s, %s, %s, %08x, %d) not implemented\n"
           ,servername, msgname, fromname, buf, buflen
         ));

  return (NERR_BASE);
}



 /*****************************************************************************
 * Name      : NET_API_STATUS NetMessageNameAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR msgname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:43:41]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetMessageNameAdd(
                                              LPWSTR servername,
                                              LPWSTR msgname
                                              )

{

  dprintf(("NETAPI32: NetMessageNameAdd(%s, %s) not implemented\n"
           ,servername, msgname
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetMessageNameDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR msgname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:44:12]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetMessageNameDel(
                                              LPWSTR servername,
                                              LPWSTR msgname
                                              )

{

  dprintf(("NETAPI32: NetMessageNameDel(%s, %s) not implemented\n"
           ,servername, msgname
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetMessageNameEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resume_handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:45:05]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetMessageNameEnum(
                                               LPWSTR servername,
                                               DWORD level,
                                               LPBYTE *bufptr,
                                               DWORD prefmaxlen,
                                               LPDWORD entriesread,
                                               LPDWORD totalentries,
                                               LPDWORD resume_handle
                                               )

{

  dprintf(("NETAPI32: NetMessageNameEnum(%s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resume_handle
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetMessageNameGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR msgname
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:45:40]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetMessageNameGetInfo(
                                                  LPWSTR servername,
                                                  LPWSTR msgname,
                                                  DWORD level,
                                                  LPBYTE *bufptr
                                                  )

{

  dprintf(("NETAPI32: NetMessageNameGetInfo(%s, %s, %d, %08x) not implemented\n"
           ,servername, msgname, level, *bufptr
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetQueryDisplayInformation
 * Purpose   :
 * Parameters: LPWSTR ServerName
 *             DWORD Level
 *             DWORD Index
 *             DWORD EntriesRequested
 *             DWORD PreferredMaximumLength
 *             LPDWORD ReturnedEntryCount
 *             PVOID *SortedBuffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:46:40]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetQueryDisplayInformation(
                                                      LPWSTR ServerName,
                                                      DWORD Level,
                                                      DWORD Index,
                                                      DWORD EntriesRequested,
                                                      DWORD PreferredMaximumLength,
                                                      LPDWORD ReturnedEntryCount,
                                                      PVOID *SortedBuffer
                                                      )

{

  dprintf(("NETAPI32: NetQueryDisplayInformation(%08x, %d, %d, %d, %d, %08x, %08x) not implemented\n"
           ,ServerName, Level, Index, EntriesRequested, PreferredMaximumLength, ReturnedEntryCount, *SortedBuffer
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetRemoteTOD
 * Purpose   :
 * Parameters: LPWSTR UncServerName
 *             LPBYTE *BufferPtr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:47:20]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetRemoteTOD(
                                         LPWSTR UncServerName,
                                         LPBYTE *BufferPtr
                                         )

{

  dprintf(("NETAPI32: NetRemoteTOD(%s, %08x) not implemented\n"
           ,UncServerName, *BufferPtr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:47:52]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplExportDirAdd(
                                                LPWSTR servername,
                                                DWORD level,
                                                LPBYTE buf,
                                                LPDWORD parm_err
                                                )

{

  dprintf(("NETAPI32: NetReplExportDirAdd(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:48:42]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplExportDirDel(
                                                LPWSTR servername,
                                                LPWSTR dirname
                                                )

{

  dprintf(("NETAPI32: NetReplExportDirDel(%s, %s) not implemented\n"
           ,servername, dirname
         ));

  return (NERR_BASE);
}



 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resumehandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:49:05]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplExportDirEnum(
                                                 LPWSTR servername,
                                                 DWORD level,
                                                 LPBYTE *bufptr,
                                                 DWORD prefmaxlen,
                                                 LPDWORD entriesread,
                                                 LPDWORD totalentries,
                                                 LPDWORD resumehandle
                                                 )

{

  dprintf(("NETAPI32: NetReplExportDirEnum(%s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:49:34]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplExportDirGetInfo(
                                                    LPWSTR servername,
                                                    LPWSTR dirname,
                                                    DWORD level,
                                                    LPBYTE *bufptr
                                                    )

{

  dprintf(("NETAPI32: NetReplExportDirGetInfo(%s, %s, %d, %08x) not implemented\n"
           ,servername, dirname, level, *bufptr
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirLock
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:49:58]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplExportDirLock(
                                                 LPWSTR servername,
                                                 LPWSTR dirname
                                                 )

{

  dprintf(("NETAPI32: NetReplExportDirLock(%s, %s) not implemented\n"
           ,servername, dirname
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:50:31]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplExportDirSetInfo(
                                                    LPWSTR servername,
                                                    LPWSTR dirname,
                                                    DWORD level,
                                                    LPBYTE buf,
                                                    LPDWORD parm_err
                                                    )

{

  dprintf(("NETAPI32: NetReplExportDirSetInfo(%s, %s, %d, %08x, %08x) not implemented\n"
           ,servername, dirname, level, buf, parm_err
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirUnlock
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD unlockforce
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:51:12]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplExportDirUnlock(
                                                   LPWSTR servername,
                                                   LPWSTR dirname,
                                                   DWORD unlockforce
                                                   )

{

  dprintf(("NETAPI32: NetReplExportDirUnlock(%s, %s, %d) not implemented\n"
           ,servername, dirname, unlockforce
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:51:37]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplGetInfo(
                                           LPWSTR servername,
                                           DWORD level,
                                           LPBYTE *bufptr
                                           )

{

  dprintf(("NETAPI32: NetReplGetInfo(%s, %d, %08x) not implemented\n"
           ,servername, level, *bufptr
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:52:11]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplImportDirAdd(
                                                LPWSTR servername,
                                                DWORD level,
                                                LPBYTE buf,
                                                LPDWORD parm_err
                                                )

{

  dprintf(("NETAPI32: NetReplImportDirAdd(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:52:36]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplImportDirDel(
                                                LPWSTR servername,
                                                LPWSTR dirname
                                                )

{

  dprintf(("NETAPI32: NetReplImportDirDel(%s, %s) not implemented\n"
           ,servername, dirname
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resumehandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:52:56]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplImportDirEnum(
                                                 LPWSTR servername,
                                                 DWORD level,
                                                 LPBYTE *bufptr,
                                                 DWORD prefmaxlen,
                                                 LPDWORD entriesread,
                                                 LPDWORD totalentries,
                                                 LPDWORD resumehandle
                                                 )

{

  dprintf(("NETAPI32: NetReplImportDirEnum(%s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:53:24]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplImportDirGetInfo(
                                                    LPWSTR servername,
                                                    LPWSTR dirname,
                                                    DWORD level,
                                                    LPBYTE *bufptr
                                                    )

{

  dprintf(("NETAPI32: NetReplImportDirGetInfo(%s, %s, %d, %08x) not implemented\n"
           ,servername, dirname, level, *bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirLock
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:53:45]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplImportDirLock(
                                                 LPWSTR servername,
                                                 LPWSTR dirname
                                                 )

{

  dprintf(("NETAPI32: NetReplImportDirLock(%s, %s) not implemented\n"
           ,servername, dirname
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirUnlock
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD unlockforce
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:54:16]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplImportDirUnlock(
                                                   LPWSTR servername,
                                                   LPWSTR dirname,
                                                   DWORD unlockforce
                                                   )

{

  dprintf(("NETAPI32: NetReplImportDirUnlock(%s, %s, %d) not implemented\n"
           ,servername, dirname, unlockforce
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetReplSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:54:38]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetReplSetInfo(
                                           LPWSTR servername,
                                           DWORD level,
                                           LPBYTE buf,
                                           LPDWORD parm_err
                                           )

{

  dprintf(("NETAPI32: NetReplSetInfo(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetScheduleJobAdd
 * Purpose   :
 * Parameters: LPWSTR Servername
 *             LPBYTE Buffer
 *             LPDWORD JobId
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:55:41]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetScheduleJobAdd(
                                             LPWSTR Servername,
                                             LPBYTE Buffer,
                                             LPDWORD JobId
                                             )

{

  dprintf(("NETAPI32: NetScheduleJobAdd(%08x, %08x, %08x) not implemented\n"
           ,Servername, Buffer, JobId
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetScheduleJobDel
 * Purpose   :
 * Parameters: LPWSTR Servername
 *             DWORD MinJobId
 *             DWORD MaxJobId
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:55:50]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetScheduleJobDel(
                                             LPWSTR Servername,
                                             DWORD MinJobId,
                                             DWORD MaxJobId
                                             )

{

  dprintf(("NETAPI32: NetScheduleJobDel(%08x, %d, %d) not implemented\n"
           ,Servername, MinJobId, MaxJobId
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetScheduleJobEnum
 * Purpose   :
 * Parameters: LPWSTR Servername
 *             LPBYTE *PointerToBuffer
 *             DWORD PreferredMaximumLength
 *             LPDWORD EntriesRead
 *             LPDWORD TotalEntries
 *             LPDWORD ResumeHandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:56:02]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetScheduleJobEnum(
                                              LPWSTR Servername,
                                              LPBYTE *PointerToBuffer,
                                              DWORD PreferredMaximumLength,
                                              LPDWORD EntriesRead,
                                              LPDWORD TotalEntries,
                                              LPDWORD ResumeHandle
                                              )

{

  dprintf(("NETAPI32: NetScheduleJobEnum(%08x, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,Servername, *PointerToBuffer, PreferredMaximumLength, EntriesRead, TotalEntries, ResumeHandle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetScheduleJobGetInfo
 * Purpose   :
 * Parameters: LPWSTR Servername
 *             DWORD JobId
 *             LPBYTE *PointerToBuffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:56:23]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetScheduleJobGetInfo(
                                                 LPWSTR Servername,
                                                 DWORD JobId,
                                                 LPBYTE *PointerToBuffer
                                                 )

{

  dprintf(("NETAPI32: NetScheduleJobGetInfo(%08x, %d, %08x) not implemented\n"
           ,Servername, JobId, *PointerToBuffer
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetServerDiskEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resume_handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:56:38]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetServerDiskEnum(
                                              LPWSTR servername,
                                              DWORD level,
                                              LPBYTE *bufptr,
                                              DWORD prefmaxlen,
                                              LPDWORD entriesread,
                                              LPDWORD totalentries,
                                              LPDWORD resume_handle
                                              )

{

  dprintf(("NETAPI32: NetServerDiskEnum(%s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resume_handle
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetServerEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             DWORD servertype
 *             LPWSTR domain
 *             LPDWORD resume_handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:57:09]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetServerEnum(
                                          LPWSTR servername,
                                          DWORD level,
                                          LPBYTE *bufptr,
                                          DWORD prefmaxlen,
                                          LPDWORD entriesread,
                                          LPDWORD totalentries,
                                          DWORD servertype,
                                          LPWSTR domain,
                                          LPDWORD resume_handle
                                          )

{

  dprintf(("NETAPI32: NetServerEnum(%s, %d, %08x, %d, %08x, %08x, %d, %s, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, servertype, domain, resume_handle
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetServerGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:57:43]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetServerGetInfo(
                                             LPWSTR servername,
                                             DWORD level,
                                             LPBYTE *bufptr
                                             )

{

  dprintf(("NETAPI32: NetServerGetInfo(%s, %d, %08x) not implemented\n"
           ,servername, level, *bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetServerSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD ParmError
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:58:14]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetServerSetInfo(
                                             LPWSTR servername,
                                             DWORD level,
                                             LPBYTE buf,
                                             LPDWORD ParmError
                                             )

{

  dprintf(("NETAPI32: NetServerSetInfo(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, ParmError
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetServerTransportAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:58:34]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetServerTransportAdd(
                                                  LPWSTR servername,
                                                  DWORD level,
                                                  LPBYTE bufptr
                                                  )

{

  dprintf(("NETAPI32: NetServerTransportAdd(%s, %d, %08x) not implemented\n"
           ,servername, level, bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetServerTransportDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR transportname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:59:12]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetServerTransportDel(
                                                 LPWSTR servername,
                                                 LPWSTR transportname
                                                 )

{

  dprintf(("NETAPI32: NetServerTransportDel(%08x, %08x) not implemented\n"
           ,servername, transportname
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetServerTransportEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resumehandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:59:37]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetServerTransportEnum(
                                                   LPWSTR servername,
                                                   DWORD level,
                                                   LPBYTE *bufptr,
                                                   DWORD prefmaxlen,
                                                   LPDWORD entriesread,
                                                   LPDWORD totalentries,
                                                   LPDWORD resumehandle
                                                   )

{

  dprintf(("NETAPI32: NetServerTransportEnum(%s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetSessionDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR UncClientName
 *             LPWSTR username
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:00:15]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetSessionDel(
                                          LPWSTR servername,
                                          LPWSTR UncClientName,
                                          LPWSTR username
                                          )

{

  dprintf(("NETAPI32: NetSessionDel(%s, %s, %s) not implemented\n"
           ,servername, UncClientName, username
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetSessionEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR UncClientName
 *             LPWSTR username
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resume_handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:00:46]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetSessionEnum(
                                           LPWSTR servername,
                                           LPWSTR UncClientName,
                                           LPWSTR username,
                                           DWORD level,
                                           LPBYTE *bufptr,
                                           DWORD prefmaxlen,
                                           LPDWORD entriesread,
                                           LPDWORD totalentries,
                                           LPDWORD resume_handle
                                           )

{

  dprintf(("NETAPI32: NetSessionEnum(%s, %s, %s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, UncClientName, username, level, *bufptr, prefmaxlen, entriesread, totalentries, resume_handle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetSessionGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR UncClientName
 *             LPWSTR username
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:01:17]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetSessionGetInfo(
                                              LPWSTR servername,
                                              LPWSTR UncClientName,
                                              LPWSTR username,
                                              DWORD level,
                                              LPBYTE *bufptr
                                              )

{

  dprintf(("NETAPI32: NetSessionGetInfo(%s, %s, %s, %d, %08x) not implemented\n"
           ,servername, UncClientName, username, level, *bufptr
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetShareAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:01:48]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetShareAdd(
                                        LPWSTR servername,
                                        DWORD level,
                                        LPBYTE buf,
                                        LPDWORD parm_err
                                        )

{

  dprintf(("NETAPI32: NetShareAdd(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetShareCheck
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR device
 *             LPDWORD type
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:02:16]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetShareCheck(
                                          LPWSTR servername,
                                          LPWSTR device,
                                          LPDWORD type
                                          )

{

  dprintf(("NETAPI32: NetShareCheck(%s, %s, %08x) not implemented\n"
           ,servername, device, type
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetShareDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR netname
 *             DWORD reserved
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:03:01]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetShareDel(
                                        LPWSTR servername,
                                        LPWSTR netname,
                                        DWORD reserved
                                        )

{

  dprintf(("NETAPI32: NetShareDel(%s, %s, %d) not implemented\n"
           ,servername, netname, reserved
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetShareGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR netname
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:03:39]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetShareGetInfo(
                                            LPWSTR servername,
                                            LPWSTR netname,
                                            DWORD level,
                                            LPBYTE *bufptr
                                            )

{

  dprintf(("NETAPI32: NetShareGetInfo(%s, %s, %d, %08x) not implemented\n"
           ,servername, netname, level, *bufptr
         ));

  return (NERR_BASE);
}
 /*****************************************************************************
 * Name      : NET_API_STATUS NetShareSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR netname
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:04:00]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetShareSetInfo(
                                            LPWSTR servername,
                                            LPWSTR netname,
                                            DWORD level,
                                            LPBYTE buf,
                                            LPDWORD parm_err
                                            )

{

  dprintf(("NETAPI32: NetShareSetInfo(%s, %s, %d, %08x, %08x) not implemented\n"
           ,servername, netname, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetStatisticsGet2
 * Purpose   :
 * Parameters: LPWSTR server
 *             LPWSTR service
 *             DWORD level
 *             DWORD options
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:04:44]
 *****************************************************************************/
/* Not in DLL but in SDK doc
NET_API_STATUS WIN32API OS2NetStatisticsGet2(
                                              LPWSTR server,
                                              LPWSTR service,
                                              DWORD level,
                                              DWORD options,
                                              LPBYTE *bufptr
                                              )

{

  dprintf(("NETAPI32: NetStatisticsGet2(%s, %s, %d, %d, %08x) not implemented\n"
           ,server, service, level, options, *bufptr
         ));

  return (NERR_BASE);
}
*/

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:06:02]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserAdd(
                                      LPWSTR servername,
                                      DWORD level,
                                      LPBYTE buf,
                                      LPDWORD parm_err
                                      )

{

  dprintf(("NETAPI32: NetUserAdd(%08x, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserChangePassword
 * Purpose   :
 * Parameters: LPWSTR domainname
 *             LPWSTR username
 *             LPWSTR oldpassword
 *             LPWSTR newpassword
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:06:12]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserChangePassword(
                                                 LPWSTR domainname,
                                                 LPWSTR username,
                                                 LPWSTR oldpassword,
                                                 LPWSTR newpassword
                                                 )

{

  dprintf(("NETAPI32: NetUserChangePassword(%08x, %08x, %08x, %08x) not implemented\n"
           ,domainname, username, oldpassword, newpassword
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR username
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:06:25]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserDel(
                                      LPWSTR servername,
                                      LPWSTR username
                                      )

{

  dprintf(("NETAPI32: NetUserDel(%08x, %08x) not implemented\n"
           ,servername, username
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             DWORD filter
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resume_handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:06:36]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserEnum(
                                       LPWSTR servername,
                                       DWORD level,
                                       DWORD filter,
                                       LPBYTE *bufptr,
                                       DWORD prefmaxlen,
                                       LPDWORD entriesread,
                                       LPDWORD totalentries,
                                       LPDWORD resume_handle
                                       )

{

  dprintf(("NETAPI32: NetUserEnum(%08x, %d, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, filter, *bufptr, prefmaxlen, entriesread, totalentries, resume_handle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserGetGroups
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR username
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:06:50]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserGetGroups(
                                            LPWSTR servername,
                                            LPWSTR username,
                                            DWORD level,
                                            LPBYTE *bufptr,
                                            DWORD prefmaxlen,
                                            LPDWORD entriesread,
                                            LPDWORD totalentries
                                            )

{

  dprintf(("NETAPI32: NetUserGetGroups(%08x, %08x, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,servername, username, level, *bufptr, prefmaxlen, entriesread, totalentries
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR username
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:09:52]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserGetInfo(
                                          LPWSTR servername,
                                          LPWSTR username,
                                          DWORD level,
                                          LPBYTE *bufptr
                                          )

{

  dprintf(("NETAPI32: NetUserGetInfo(%08x, %08x, %d, %08x) not implemented\n"
           ,servername, username, level, *bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserGetLocalGroups
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR username
 *             DWORD level
 *             DWORD flags
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:10:01]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserGetLocalGroups(
                                                 LPWSTR servername,
                                                 LPWSTR username,
                                                 DWORD level,
                                                 DWORD flags,
                                                 LPBYTE *bufptr,
                                                 DWORD prefmaxlen,
                                                 LPDWORD entriesread,
                                                 LPDWORD totalentries
                                                 )

{

  dprintf(("NETAPI32: NetUserGetLocalGroups(%08x, %08x, %d, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,servername, username, level, flags, *bufptr, prefmaxlen, entriesread, totalentries
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserModalsGet
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:10:22]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserModalsGet(
                                            LPWSTR servername,
                                            DWORD level,
                                            LPBYTE *bufptr
                                            )

{

  dprintf(("NETAPI32: NetUserModalsGet(%08x, %d, %08x) not implemented\n"
           ,servername, level, *bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserModalsSet
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:10:31]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserModalsSet(
                                            LPWSTR servername,
                                            DWORD level,
                                            LPBYTE buf,
                                            LPDWORD parm_err
                                            )

{

  dprintf(("NETAPI32: NetUserModalsSet(%08x, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserSetGroups
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR username
 *             DWORD level
 *             LPBYTE buf
 *             DWORD num_entries
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:10:44]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserSetGroups(
                                            LPWSTR servername,
                                            LPWSTR username,
                                            DWORD level,
                                            LPBYTE buf,
                                            DWORD num_entries
                                            )

{

  dprintf(("NETAPI32: NetUserSetGroups(%08x, %08x, %d, %08x, %d) not implemented\n"
           ,servername, username, level, buf, num_entries
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetUserSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR username
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:11:04]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetUserSetInfo(
                                          LPWSTR servername,
                                          LPWSTR username,
                                          DWORD level,
                                          LPBYTE buf,
                                          LPDWORD parm_err
                                          )

{

  dprintf(("NETAPI32: NetUserSetInfo(%08x, %08x, %d, %08x, %08x) not implemented\n"
           ,servername, username, level, buf, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:11:21]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetWkstaGetInfo(
                                            LPWSTR servername,
                                            DWORD level,
                                            LPBYTE *bufptr
                                            )

{

  dprintf(("NETAPI32: NetWkstaGetInfo(%s, %d, %08x) not implemented\n"
           ,servername, level, *bufptr
         ));

  return (NERR_BASE);
}


 /*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buffer
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:11:39]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetWkstaSetInfo(
                                            LPWSTR servername,
                                            DWORD level,
                                            LPBYTE buffer,
                                            LPDWORD parm_err
                                            )

{

  dprintf(("NETAPI32: NetWkstaSetInfo(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buffer, parm_err
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaTransportAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:12:18]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetWkstaTransportAdd(
                                                 LPWSTR servername,
                                                 DWORD level,
                                                 LPBYTE buf
                                                 )

{

  dprintf(("NETAPI32: NetWkstaTransportAdd(%s, %d, %08x)not implemented\n"
           ,servername, level, buf
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaTransportDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR transportname
 *             DWORD ucond
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:13:11]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetWkstaTransportDel(
                                                LPWSTR servername,
                                                LPWSTR transportname,
                                                DWORD ucond
                                                )

{

  dprintf(("NETAPI32: NetWkstaTransportDel(%08x, %08x, %d) not implemented\n"
           ,servername, transportname, ucond
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaTransportEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resumehandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:13:44]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetWkstaTransportEnum(
                                                 LPWSTR servername,
                                                 DWORD level,
                                                 LPBYTE *bufptr,
                                                 DWORD prefmaxlen,
                                                 LPDWORD entriesread,
                                                 LPDWORD totalentries,
                                                 LPDWORD resumehandle
                                                 )

{

  dprintf(("NETAPI32: NetWkstaTransportEnum(%08x, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaUserEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             LPDWORD resumehandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:14:05]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetWkstaUserEnum(
                                            LPWSTR servername,
                                            DWORD level,
                                            LPBYTE *bufptr,
                                            DWORD prefmaxlen,
                                            LPDWORD entriesread,
                                            LPDWORD totalentries,
                                            LPDWORD resumehandle
                                            )

{

  dprintf(("NETAPI32: NetWkstaUserEnum(%08x, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaUserGetInfo
 * Purpose   :
 * Parameters: LPWSTR reserved
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:14:17]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetWkstaUserGetInfo(
                                               LPWSTR reserved,
                                               DWORD level,
                                               LPBYTE *bufptr
                                               )

{

  dprintf(("NETAPI32: NetWkstaUserGetInfo(%08x, %d, %08x) not implemented\n"
           ,reserved, level, *bufptr
         ));

  return (NERR_BASE);
}

 /*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaUserSetInfo
 * Purpose   :
 * Parameters: LPWSTR reserved
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:16:08]
 *****************************************************************************/
NET_API_STATUS WIN32API OS2NetWkstaUserSetInfo(
                                               LPWSTR reserved,
                                               DWORD level,
                                               LPBYTE buf,
                                               LPDWORD parm_err
                                               )

{

  dprintf(("NETAPI32: NetWkstaUserSetInfo(%08x, %d, %08x, %08x) not implemented\n"
           ,reserved, level, buf, parm_err
         ));

  return (NERR_BASE);
}

