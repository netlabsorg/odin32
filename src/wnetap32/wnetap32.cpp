/* $Id: wnetap32.cpp,v 1.15 2001-09-06 22:23:41 phaller Exp $ */

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
 * Note: functions that return structures/buffers seem to append strings
 * at the end of the buffer. Currently, we just allocate the strings
 * "normally". Therefore a caller that just does a NetApiBufferFree() on the
 * returned buffer will leak all allocated strings.
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

#include <odin.h>
#include <odinwrap.h>
#include <os2win.h>
#include <misc.h>
#include <unicode.h>
#include <heapstring.h>
#include <string.h>
#include <winconst.h>

#include "oslibnet.h"
#include "lanman.h"

ODINDEBUGCHANNEL(WNETAP32-WNETAP32)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

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
ODINFUNCTION1(UCHAR, OS2Netbios,
              PNCB, pncb)
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

ODINFUNCTION3(NET_API_STATUS, OS2NetAlertRaise,
              LPWSTR, AlertEventName,
              LPVOID, Buffer,
              DWORD, BufferSize)
{

  dprintf(("NETAPI32: NetAlertRaise not implemented\n"));

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

ODINFUNCTION4(NET_API_STATUS, OS2NetAlertRaiseEx,
              LPWSTR, AlertEventName,
              LPVOID, VariableInfo,
              DWORD, VariableInfoSize,
              LPWSTR, ServiceName)

{

  dprintf(("NETAPI32: NetAlertRaiseEx not implemented\n"));

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

ODINFUNCTION9(NET_API_STATUS, OS2NetFileEnum,
              LPWSTR, servername,
              LPWSTR, basepath,
              LPWSTR, username,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resume_handle)
{
  dprintf(("NETAPI32: NetFileEnum not implemented\n"));

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

ODINFUNCTION4(NET_API_STATUS,OS2NetFileGetInfo,LPWSTR,  servername,
                                               DWORD,   fileid,
                                               DWORD,   level,
                                               LPBYTE*, bufptr)
{
  dprintf(("NETAPI32: NetFileGetInfo not implemented\n"));

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

ODINFUNCTION3(NET_API_STATUS, OS2NetGetAnyDCName,
              LPCWSTR, ServerName,
              LPCWSTR, DomainName,
              LPBYTE *, Buffer)
{
  dprintf(("NETAPI32: NetGetAnyDCName not implemented\n"));

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

ODINFUNCTION3(NET_API_STATUS, OS2NetGetDCName,
              LPWSTR, servername,
              LPWSTR, domainname,
              LPBYTE *, bufptr)
{

  dprintf(("NETAPI32: NetGetDCName not implemented\n"));

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

ODINFUNCTION4(NET_API_STATUS, OS2NetGetDisplayInformationIndex,
              LPWSTR, ServerName,
              DWORD, Level,
              LPWSTR, Prefix,
              LPDWORD, Index)
{
  dprintf(("NETAPI32: NetGetDisplayInformationIndex not implemented\n"));

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
ODINFUNCTION5(NET_API_STATUS, OS2NetHandleGetInfo,
              UNSIGNED, SHORT,
              hHandle,, SHORT,
              sLevel,, CHAR,
              FAR *, pbBuffer,
              UNSIGNED, SHORTcbBuffer,
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
ODINFUNCTION5(NET_API_STATUS, OS2NetHandleSetInfo,
              UNSIGNED, SHORT,
              hHandle,, SHORT,
              sLevel,, CHAR,
              FAR *, pbBuffer,
              UNSIGNED, SHORTcbBuffer,
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
ODINFUNCTION5(NET_API_STATUS, OS2NetMessageBufferSend,
              LPWSTR, servername,
              LPWSTR, msgname,
              LPWSTR, fromname,
              LPBYTE, buf,
              DWORD, buflen)

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
ODINFUNCTION2(NET_API_STATUS, OS2NetMessageNameAdd,
              LPWSTR, servername,
              LPWSTR, msgname)

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
ODINFUNCTION2(NET_API_STATUS, OS2NetMessageNameDel,
              LPWSTR, servername,
              LPWSTR, msgname)

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
ODINFUNCTION7(NET_API_STATUS, OS2NetMessageNameEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resume_handle)

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
ODINFUNCTION4(NET_API_STATUS, OS2NetMessageNameGetInfo,
              LPWSTR, servername,
              LPWSTR, msgname,
              DWORD, level,
              LPBYTE *, bufptr)

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
ODINFUNCTION7(NET_API_STATUS, OS2NetQueryDisplayInformation,
              LPWSTR, ServerName,
              DWORD, Level,
              DWORD, Index,
              DWORD, EntriesRequested,
              DWORD, PreferredMaximumLength,
              LPDWORD, ReturnedEntryCount,
              PVOID *, SortedBuffer)

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
ODINFUNCTION2(NET_API_STATUS, OS2NetRemoteTOD,
              LPWSTR, UncServerName,
              LPBYTE *, BufferPtr)

{

  dprintf(("NETAPI32: NetRemoteTOD(%s, %08x) not implemented\n"
           ,UncServerName, *BufferPtr
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
ODINFUNCTION3(NET_API_STATUS, OS2NetSessionDel,
              LPWSTR, servername,
              LPWSTR, UncClientName,
              LPWSTR, username)

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
ODINFUNCTION9(NET_API_STATUS, OS2NetSessionEnum,
              LPWSTR, servername,
              LPWSTR, UncClientName,
              LPWSTR, username,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resume_handle)

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
ODINFUNCTION5(NET_API_STATUS, OS2NetSessionGetInfo,
              LPWSTR, servername,
              LPWSTR, UncClientName,
              LPWSTR, username,
              DWORD, level,
              LPBYTE *, bufptr)

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
ODINFUNCTION4(NET_API_STATUS, OS2NetShareAdd,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

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
ODINFUNCTION3(NET_API_STATUS, OS2NetShareCheck,
              LPWSTR, servername,
              LPWSTR, device,
              LPDWORD, type)

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
ODINFUNCTION3(NET_API_STATUS, OS2NetShareDel,
              LPWSTR, servername,
              LPWSTR, netname,
              DWORD, reserved)

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
ODINFUNCTION4(NET_API_STATUS, OS2NetShareGetInfo,
              LPWSTR, servername,
              LPWSTR, netname,
              DWORD, level,
              LPBYTE *, bufptr)

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
ODINFUNCTION5(NET_API_STATUS, OS2NetShareSetInfo,
              LPWSTR, servername,
              LPWSTR, netname,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

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
ODINFUNCTION5(NET_API_STATUS, OS2NetStatisticsGet2,
              LPWSTR, server,
              LPWSTR, service,
              DWORD, level,
              DWORD, options,
              LPBYTE *, bufptr)

{

  dprintf(("NETAPI32: NetStatisticsGet2(%s, %s, %d, %d, %08x) not implemented\n"
           ,server, service, level, options, *bufptr
         ));

  return (NERR_BASE);
}
*/


/*****************************************************************************
 * Name      : NET_API_STATUS NetConfigSet
 * Purpose   : configure a network component
 * Parameters: LPWSTR lpServer
 *             LPWSTR lpReserved1
 *             LPWSTR lpComponent
 *             DWORD  dwLevel
 *             DWORD  dwReserved2
 *             LPBYTE lpBuf
 *             DWORD  dwReserved3
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION7(NET_API_STATUS,OS2NetConfigSet,LPWSTR, lpServer,
                                             LPWSTR, lpReserved1,
                                             LPWSTR, lpComponent,
                                             DWORD,  dwLevel,
                                             DWORD,  dwReserved2,
                                             LPBYTE, lpBuf,
                                             DWORD,  dwReserved3)
{
  dprintf(("NETAPI32: NetConfigSet not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetConfigGet
 * Purpose   : get configuration from a network component
 * Parameters: LPWSTR lpServer
 *             LPWSTR lpComponent
 *             LPWSTR lpParameter
 *             LPBYTE lpBuf
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION4(NET_API_STATUS,OS2NetConfigGet,LPWSTR, lpServer,
                                             LPWSTR, lpComponent,
                                             LPWSTR, lpParameter,
                                             LPBYTE, lpBuf)
{
  dprintf(("NETAPI32: NetConfigGet not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS RxNetAccessSetInfo
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION6(NET_API_STATUS, OS2RxNetAccessSetInfo,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4,
              DWORD, x5,
              DWORD, x6)
{
  dprintf(("NETAPI32: RxNetAccessSetInfo(%08x, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS RxNetAccessGetInfo
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION6(NET_API_STATUS, OS2RxNetAccessGetInfo,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4,
              DWORD, x5,
              DWORD, x6)
{
  dprintf(("NETAPI32: RxNetAccessGetInfo(%08x, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS I_NetGetDCList
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION6(NET_API_STATUS, OS2I_NetGetDCList,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4,
              DWORD, x5,
              DWORD, x6)
{
  dprintf(("NETAPI32: I_NetGetDCList(%08x, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS I_NetNameCanonicalize
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION4(NET_API_STATUS, OS2I_NetNameCanonicalize,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4)
{
  dprintf(("NETAPI32: I_NetNameCanonicalize(%08x, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS I_NetNameCompare
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION4(NET_API_STATUS, OS2I_NetNameCompare,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4)
{
  dprintf(("NETAPI32: I_NetNameCompare(%08x, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS I_NetNameValidate
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION3(NET_API_STATUS, OS2I_NetNameValidate,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3)
{
  dprintf(("NETAPI32: I_NetNameValidate(%08x, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS I_NetPathCanonicalize
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION4(NET_API_STATUS, OS2I_NetPathCanonicalize,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4)
{
  dprintf(("NETAPI32: I_NetPathCanonicalize(%08x, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS I_NetPathCompare
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION4(NET_API_STATUS, OS2I_NetPathCompare,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4)
{
  dprintf(("NETAPI32: I_NetPathCompare(%08x, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS I_NetPathType
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION2(NET_API_STATUS, OS2I_NetPathType,
              DWORD, x1,
              DWORD, x2)
{
  dprintf(("NETAPI32: I_NetPathType(%08x, %08xh) not implemented\n",
           x1,
           x2));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetapipBufferAllocate
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION3(NET_API_STATUS, OS2NetapipBufferAllocate,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3)
{
  dprintf(("NETAPI32: NetapipBufferAllocate(%08x, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetUseAdd
 * Purpose   : add remote device association
 * Parameters: LPWSTR  lpUncServerName
 *             DWORD   dwLevel
 *             LPBYTE  lpBuf
 *             LPDWORD lpParmError
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION4(NET_API_STATUS, OS2NetUseAdd,
              LPWSTR, lpUncServerName,
              DWORD, dwLevel,
              LPBYTE, lpBuf,
              LPDWORD, lpParmError)
{
  dprintf(("NETAPI32: NetUseAdd(%08xh,%08xh,%08xh,%08xh) not implemented\n",
           lpUncServerName,
           dwLevel,
           lpBuf,
           lpParmError));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetUseDel
 * Purpose   : remove remote device association
 * Parameters: LPWSTR  lpUncServerName
 *             LPWSTR  lpUseName
 *             DWORD   dwForceCond
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION3(NET_API_STATUS, OS2NetUseDel,
              LPWSTR, lpUncServerName,
              LPWSTR, lpUseName,
              DWORD, dwForceCond)
{
  dprintf(("NETAPI32: NetUseDel(%08xh,%08xh,%08xh) not implemented\n",
           lpUncServerName,
           lpUseName,
           dwForceCond));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS NetServiceControl
 * Purpose   : controls operations of network services
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpService
 *             DWORD   dwOpcode
 *             DWORD   dwArgument
 *             LPBYTE* bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION5(NET_API_STATUS, OS2NetServiceControl,
              LPWSTR, lpServerName,
              LPWSTR, lpService,
              DWORD, dwOpcode,
              DWORD, dwArgument,
              LPBYTE*, bufptr)
{
  dprintf(("NETAPI32: NetServiceControl(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented\n",
           lpServerName,
           lpService,
           dwOpcode,
           dwArgument,
           bufptr));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetServiceEnum
 * Purpose   : retrieves information about all started services
 * Parameters: LPWSTR  lpServerName
 *             DWORD   dwLevel
 *             LPBYTE* bufptr
 *             DWORD   dwPrefMaxLen
 *             LPDWORD lpdEntriesRead
 *             LPDWORD lpdTotalEntries
 *             LPDWORD lpdResumeHandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION7(NET_API_STATUS, OS2NetServiceEnum,
              LPWSTR, lpServerName,
              DWORD, dwLevel,
              LPBYTE*, bufptr,
              DWORD, dwPrefMaxLen,
              LPDWORD, lpdEntriesRead,
              LPDWORD, lpdTotalEntries,
              LPDWORD, lpdResumeHandle)
{
  dprintf(("NETAPI32: NetServiceEnum(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented\n",
           lpServerName,
           dwLevel,
           bufptr,
           dwPrefMaxLen,
           lpdEntriesRead,
           lpdTotalEntries,
           lpdResumeHandle));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetServiceGetInfo
 * Purpose   : retrieves information about a particular started service
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpService
 *             DWORD   dwLevel
 *             LPBYTE* bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION4(NET_API_STATUS, OS2NetServiceGetInfo,
              LPWSTR, lpServerName,
              LPWSTR, lpService,
              DWORD, dwLevel,
              LPBYTE*, bufptr)
{
  dprintf(("NETAPI32: NetServiceGetInfo(%08xh,%08xh,%08xh,%08xh) not implemented\n",
           lpServerName,
           lpService,
           dwLevel,
           bufptr));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetServiceInstall
 * Purpose   : starts a network service
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpService
 *             DWORD   argc
 *             LPWSTR  argv[]
 *             LPBYTE* bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION5(NET_API_STATUS, OS2NetServiceInstall,
              LPWSTR, lpServerName,
              LPWSTR, lpService,
              DWORD, argc,
              LPWSTR*, argv,
              LPBYTE*, bufptr)
{
  dprintf(("NETAPI32: NetServiceInstall not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS I_NetLogonControl
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION6(NET_API_STATUS, OS2I_NetLogonControl,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4,
              DWORD, x5,
              DWORD, x6)
{
  dprintf(("NETAPI32: I_NetLogonControl(%08x, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS RxNetAccessAdd
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION6(NET_API_STATUS, OS2RxNetAccessAdd,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4,
              DWORD, x5,
              DWORD, x6)
{
  dprintf(("NETAPI32: RxNetAccessAdd(%08x, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS RxNetAccessDel
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION6(NET_API_STATUS, OS2RxNetAccessDel,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4,
              DWORD, x5,
              DWORD, x6)
{
  dprintf(("NETAPI32: RxNetAccessDel(%08x, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS RxNetAccessEnum
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION6(NET_API_STATUS, OS2RxNetAccessEnum,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4,
              DWORD, x5,
              DWORD, x6)
{
  dprintf(("NETAPI32: RxNetAccessEnum(%08x, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS RxNetAccessGetUserPerms
 * Purpose   :
 * Parameters: wrong
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION6(NET_API_STATUS, OS2RxNetAccessGetUserPerms,
              DWORD, x1,
              DWORD, x2,
              DWORD, x3,
              DWORD, x4,
              DWORD, x5,
              DWORD, x6)
{
  dprintf(("NETAPI32: RxNetAccessGetUserPerms(%08x, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented\n",
           x1,
           x2,
           x3,
           x4,
           x5,
           x6));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetConfigGetAll
 * Purpose   : retrieves all the configuration information for a given component
 *             on a local or remote computer
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpComponent
 *             LPBYTE* bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION3(NET_API_STATUS,OS2NetConfigGetAll,LPWSTR,  lpServerName,
                                                LPWSTR,  lpComponent,
                                                LPBYTE*, bufptr)
{
  dprintf(("NETAPI32: NetConfigGetAll not implemented\n"));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS NetConnectionEnum
 * Purpose   : lists all connections made to a shared resource
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpQualifier
 *             DWORD   dwLevel
 *             LPBYTE* bufptr
 *             DWORD   dwPrefMaxLen
 *             LPDWORD dwEntriesRead
 *             LPDWORD dwTotalEntries
 *             LPDWORD dwResumeHandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION8(NET_API_STATUS,OS2NetConnectionEnum,LPWSTR,  lpServerName,
                                                  LPWSTR,  lpQualifier,
                                                  DWORD,   dwLevel,
                                                  LPBYTE*, bufptr ,
                                                  DWORD,   dwPrefMaxLen,
                                                  LPDWORD, dwEntriesRead,
                                                  LPDWORD, dwTotalEntries,
                                                  LPDWORD, dwResumeHandle)
{
  dprintf(("NETAPI32: NetConnectionEnum not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetFileClose
 * Purpose   : forces a resource to close
 * Parameters: LPWSTR  lpServerName
 *             DWORD   fileid
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION2(NET_API_STATUS,OS2NetFileClose,LPWSTR,  lpServerName,
                                             DWORD,   fileid)
{
  dprintf(("NETAPI32: NetFileClose not implemented\n"));

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

ODINFUNCTION3(NET_API_STATUS,OS2NetShareDelSticky,LPWSTR, servername,
                                                  LPWSTR, netname,
                                                  DWORD,  reserved)
{

  dprintf(("NETAPI32: NetShareDelSticky not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetShareEnum
 * Purpose   : retrieves information about all shared devices
 * Parameters: LPWSTR  lpServerName
 *             DWORD   dwLevel
 *             LPBYTE* bufptr
 *             DWORD   dwPrefMaxLen
 *             LPDWORD lpdEntriesRead
 *             LPDWORD lpdTotalEntries
 *             LPDWORD lpdResumeHandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION7(NET_API_STATUS,OS2NetShareEnum,LPWSTR,  lpServerName,
                                             DWORD,   dwLevel,
                                             LPBYTE*, bufptr,
                                             DWORD,   dwPrefMaxLen,
                                             LPDWORD, lpdEntriesRead,
                                             LPDWORD, lpdTotalEntries,
                                             LPDWORD, lpdResumeHandle)
{
  dprintf(("NETAPI32: NetShareEnum not implemented\n"));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS NetShareEnumSticky
 * Purpose   : retrieves information about all sticky shared devices
 * Parameters: LPWSTR  lpServerName
 *             DWORD   dwLevel
 *             LPBYTE* bufptr
 *             DWORD   dwPrefMaxLen
 *             LPDWORD lpdEntriesRead
 *             LPDWORD lpdTotalEntries
 *             LPDWORD lpdResumeHandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION7(NET_API_STATUS,OS2NetShareEnumSticky,LPWSTR,  lpServerName,
                                                   DWORD,   dwLevel,
                                                   LPBYTE*, bufptr,
                                                   DWORD,   dwPrefMaxLen,
                                                   LPDWORD, lpdEntriesRead,
                                                   LPDWORD, lpdTotalEntries,
                                                   LPDWORD, lpdResumeHandle)
{
  dprintf(("NETAPI32: NetShareEnumSticky not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetUseEnum
 * Purpose   : retrieves information about all used devices
 * Parameters: LPWSTR  lpServerName
 *             DWORD   dwLevel
 *             LPBYTE* bufptr
 *             DWORD   dwPrefMaxLen
 *             LPDWORD lpdEntriesRead
 *             LPDWORD lpdTotalEntries
 *             LPDWORD lpdResumeHandle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION7(NET_API_STATUS,OS2NetUseEnum,LPWSTR,  lpServerName,
                                           DWORD,   dwLevel,
                                           LPBYTE*, bufptr,
                                           DWORD,   dwPrefMaxLen,
                                           LPDWORD, lpdEntriesRead,
                                           LPDWORD, lpdTotalEntries,
                                           LPDWORD, lpdResumeHandle)
{
  dprintf(("NETAPI32: NetUseEnum not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetUseGetInfo
 * Purpose   :
 * Parameters: LPWSTR lpServerName
 *             LPWSTR lpUseName
 *             DWORD  dwlevel
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

ODINFUNCTION4(NET_API_STATUS,OS2NetUseGetInfo,LPWSTR,  lpServerName,
                                              LPWSTR,  lpUseName,
                                              DWORD,   dwLevel,
                                              LPBYTE*, bufptr)

{

  dprintf(("NETAPI32: NetUseGetInfo not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetStatisticsGet
 * Purpose   : retrieves operating statistics for a service
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpService
 *             DWORD   dwLevel
 *             DWORD   dwOptions
 *             LPBYTE* bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

void UL2LI(PLARGE_INTEGER pli, ULONG ul)
{
  pli->LowPart = ul;
  pli->HighPart = 0;
}

void LOHI2LI(PLARGE_INTEGER pli, ULONG lo, ULONG hi)
{
  pli->LowPart = lo;
  pli->HighPart = hi;
}


ODINFUNCTION5(NET_API_STATUS,OS2NetStatisticsGet,LPWSTR,  lpServerName,
                                                 LPWSTR,  lpService,
                                                 DWORD,   dwLevel,
                                                 DWORD,   dwOptions,
                                                 LPBYTE*, bufptr)
{
  // Note: as we use the static addresses of the strings
  // for a faster comparsion, the compiler may NOT
  // merge duplicate static const strings.
  static LPSTR SERVICE_SERVER       = "SERVER";
  static LPSTR SERVICE_REQUESTER_NT = "REQUESTER";
  static LPSTR SERVICE_REQUESTER_LM = "REQUESTER";
  static LPSTR SERVICE_UNKNOWN      = "UNKNOWN";
  
  // Convert servername to ASCII
  // Convert service name to ASCII AND OS/2-Pendant
  // OS/2 only allowes "SERVER" and "REQUESTER"
  char *asciiServername = NULL;
  if (lpServerName) asciiServername = UnicodeToAsciiString(lpServerName);
  
  // server remains
  LPSTR lpstrOS2Service = NULL;
  if (lpService != NULL)
    if (lstrcmpiW((LPCWSTR)L"WORKSTATION",      (LPCWSTR)lpService) == 0) lpstrOS2Service = (LPSTR)SERVICE_REQUESTER_NT;
    else
    if (lstrcmpW((LPCWSTR)L"LanmanWorkstation", (LPCWSTR)lpService) == 0) lpstrOS2Service = (LPSTR)SERVICE_REQUESTER_LM;
    else
    if (lstrcmpiW((LPCWSTR)L"SERVER",           (LPCWSTR)lpService) == 0) lpstrOS2Service = (LPSTR)SERVICE_SERVER;
    else
    if (lstrcmpW((LPCWSTR)L"LanmanServer",      (LPCWSTR)lpService) == 0) lpstrOS2Service = (LPSTR)SERVICE_SERVER;
    else
    lpstrOS2Service = (LPSTR)SERVICE_UNKNOWN;  // to prevent crashes in NETAPI
  
  // Note: The Win32 docs say nothing about "LanmanWorkstation"
  // Probably this is a request for the LANMAN-Workstation specific data?
#ifdef DEBUG
  {
    char *asciiService = UnicodeToAsciiString(lpService);
    dprintf(("WINMM: NetStatisticsGet server=[%s], service=[%s]\n",
             asciiServername,
             asciiService));
    FreeAsciiString(asciiService);
  }
#endif
  
  
  // @@@PH convert information modes!
  int iOS2Level = dwLevel;     // both must be 0
  int iOS2Options = dwOptions; // seems to be identical
  
  ULONG  ulBytesAvailable;
  DWORD  rc;
  
  // determine required size of buffer
  char pOS2Buffer[4096];
  rc = OSLibNetStatisticsGet((const unsigned char*)asciiServername,
                             (const unsigned char*)lpstrOS2Service,
                             0,
                             iOS2Level,
                             iOS2Options,
                             (unsigned char*)pOS2Buffer,
                             sizeof(pOS2Buffer),
                             &ulBytesAvailable);
  
  if (asciiServername) FreeAsciiString(asciiServername);
  
  // convert the structures
  switch (dwLevel)
  {
    case 0:
      // Note: address comparsion is valid :)
      if (lpstrOS2Service == SERVICE_REQUESTER_NT)
      {
        PSTAT_WORKSTATION_NT_0 pstw0;
        struct stat_workstation_0 *pOS2stw0 = (struct stat_workstation_0 *)pOS2Buffer;

        // calculate new size for target buffer
        rc = OS2NetApiBufferAllocate(sizeof(STAT_WORKSTATION_NT_0), (LPVOID*)&pstw0);
        if (!rc)
        {
          // buffer is zeroed?
          //memset(pstw0, 0, sizeof(STAT_WORKSTATION_0));

          UL2LI  (&pstw0->StatisticsStartTime, pOS2stw0->stw0_start);
          LOHI2LI(&pstw0->BytesReceived,       pOS2stw0->stw0_bytesrcvd_r_lo, pOS2stw0->stw0_bytesrcvd_r_hi);
          pstw0->SmbsReceived;
          pstw0->PagingReadBytesRequested;
          pstw0->NonPagingReadBytesRequested;
          pstw0->CacheReadBytesRequested;
          pstw0->NetworkReadBytesRequested;
          LOHI2LI(&pstw0->BytesTransmitted,    pOS2stw0->stw0_bytessent_r_lo, pOS2stw0->stw0_bytessent_r_hi);
          pstw0->SmbsTransmitted;
          pstw0->PagingWriteBytesRequested;
          pstw0->NonPagingWriteBytesRequested;
          pstw0->CacheWriteBytesRequested;
          pstw0->NetworkWriteBytesRequested;
          
          pstw0->InitiallyFailedOperations;
          pstw0->FailedCompletionOperations;
          pstw0->ReadOperations;
          pstw0->RandomReadOperations;
          pstw0->ReadSmbs;
          pstw0->LargeReadSmbs;
          pstw0->SmallReadSmbs;
          pstw0->WriteOperations;
          pstw0->RandomWriteOperations;
          pstw0->WriteSmbs;
          pstw0->LargeWriteSmbs;
          pstw0->SmallWriteSmbs;
          pstw0->RawReadsDenied;
          pstw0->RawWritesDenied;
          
          pstw0->NetworkErrors;
          pstw0->Sessions                   = pOS2stw0->stw0_sesstart;
          pstw0->FailedSessions             = pOS2stw0->stw0_sessfailcon;
          pstw0->Reconnects                 = pOS2stw0->stw0_autorec;
          pstw0->CoreConnects;
          pstw0->Lanman20Connects;
          pstw0->Lanman21Connects;
          pstw0->LanmanNtConnects;
          pstw0->ServerDisconnects;
          pstw0->HungSessions               = pOS2stw0->stw0_sessbroke;
          pstw0->UseCount                   = pOS2stw0->stw0_uses;
          pstw0->FailedUseCount             = pOS2stw0->stw0_usefail;
          pstw0->CurrentCommands;
        }
        // the caller is responsible for freeing the memory!
        *bufptr = (LPBYTE)pstw0;
      }
      else
      if (lpstrOS2Service == SERVICE_REQUESTER_LM)
      {
        // LanmanWorkstation !
        PSTAT_WORKSTATION_LM_0 pstw0;
        struct stat_workstation_0 *pOS2stw0 = (struct stat_workstation_0 *)pOS2Buffer;

        // calculate new size for target buffer
        rc = OS2NetApiBufferAllocate(sizeof(STAT_WORKSTATION_LM_0), (LPVOID*)&pstw0);
        if (!rc)
        {
          // Note: the really nice thing is, the lanman structures are
          // exactly identical between OS/2 and NT ... :)
          memcpy(pstw0,
                 pOS2stw0,
                 sizeof(STAT_WORKSTATION_LM_0));
        }
        
        // the caller is responsible for freeing the memory!
        *bufptr = (LPBYTE)pstw0;
      }
      else
      if (lpstrOS2Service == SERVICE_SERVER)
      {
        // SERVER !
        PSTAT_SERVER_0 psts0;
        struct stat_server_0 *pOS2sts0 = (struct stat_server_0 *)pOS2Buffer;

        // calculate new size for target buffer
        rc = OS2NetApiBufferAllocate(sizeof(STAT_SERVER_0), (LPVOID*)&psts0);
        if (!rc)
        {
          // Note: the really nice thing is, the server structures are
          // exactly identical between OS/2 and NT ... :)
          memcpy(psts0,
                 pOS2sts0,
                 sizeof(STAT_SERVER_0));
        }
        
        // the caller is responsible for freeing the memory!
        *bufptr = (LPBYTE)psts0;
      }
    
    break;
  }
  
  return (rc);
}


