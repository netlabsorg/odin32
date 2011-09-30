/* $Id: wnetap32.cpp,v 1.18 2004-02-27 20:15:45 sandervl Exp $ */

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

#include "wnetap32.h"
#include "lanman.h"
#include "oslibnet.h"

ODINDEBUGCHANNEL(WNETAP32-WNETAP32)


extern DWORD WIN32API OSLibNetbiosHlpHandler(LPVOID lpParam);


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/


//******************************************************************************
// Note:
// The major difference between OS/2 and Win32 regarding NetBIOS is
// all operations and resources are per-process for Win32 and
// global for OS/2. So we might probably end up with stray netbios names etc.
// long after a process has vanished.
//******************************************************************************
//#define NETBIOS_ENABLED
ODINFUNCTION1(UCHAR, OS2Netbios,
              PNCB_w, pncb)
{
#ifndef NETBIOS_ENABLED
  pncb->ncb_retcode = NRC_OPENERR_w;
  return pncb->ncb_retcode;
  
#else
  UCHAR rc;

  // Note: OS/2 specific documentation is found in DSSPGR1+DSSPGR2
  
  // fork for asynchronous operation:
  if (pncb->ncb_command & ASYNCH_w)
  {
    // either event or post may be specified
    if ( (pncb->ncb_event != 0) &&
         (pncb->ncb_post  != 0) )
    {
      pncb->ncb_retcode = NRC_ILLCMD_w;
      return NRC_ILLCMD_w;
    }
    
    
    // @@@PH
    // we might go for one or more statically allocated
    // worker threads instead of creating and destroying
    // a thread for each single request.
    
    // we're to start an ODIN thread ourself and do
    // the operation itself synchronously!
    
    // say the netbios operation is still pending ...
    pncb->ncb_cmd_cplt = NRC_PENDING_w;
    
    UCHAR  ucCommand = pncb->ncb_command;
    DWORD  tidNetbios;
    HANDLE hNetbiosThread = CreateThread(NULL,
                                         0x8000,
                                         OSLibNetbiosHlpHandler,
                                         (LPVOID)pncb,
                                         0, // thread creation flags
                                         &tidNetbios);
    if (hNetbiosThread == NULL)
    {
      // in case the thread could not be launched, 
      // return with error
      pncb->ncb_retcode  = NRC_SYSTEM_w;
      pncb->ncb_cmd_cplt = NRC_SYSTEM_w;
      return pncb->ncb_retcode;
    }
    else
    {
      dprintf(("NETBIOS: Netbios helper thread %d started for command %02xh\n",
               tidNetbios,
               ucCommand));
      
      // verify if the operation has completed already
      if (pncb->ncb_cmd_cplt != NRC_PENDING_w)
      {
        // Docs say in this case return as if the request was synchronous
        rc = pncb->ncb_retcode;
      }
      else
      {
        // this is the "operation pending" return code
        rc = 0;
      }
    }
  }
  else
  {
    // verify request
    if ( (pncb->ncb_event != 0) ||
         (pncb->ncb_post  != 0) )
    {
      pncb->ncb_retcode = NRC_ILLCMD_w;
      return NRC_ILLCMD_w;
    }
    
    // call the Request Router
    rc = OSLibNetBIOS( pncb );
  }

  return( rc );
#endif /* NETBIOS_ENABLED */
}


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
#ifndef NETBIOS_ENABLED
  dprintf(("NetStatisticsGet not implemented!"));
  return (NERR_BASE);
#else
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
#endif
}


