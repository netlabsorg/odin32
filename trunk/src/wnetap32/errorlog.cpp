/* $Id: errorlog.cpp,v 1.1 2000-12-12 02:43:25 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 * NETAPI32 stubs
 *
 * Copyright 2000 Patrick Haller
 *
 * Note: functions that return structures/buffers seem to append strings
 * at the end of the buffer. Currently, we just allocate the strings
 * "normally". Therefore a caller that just does a NetApiBufferFree() on the
 * returned buffer will leak all allocated strings.
 *
 */
/*****************************************************************************
 * Name      : ERRORLOG.CPP
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
#include <heapstring.h>
#include <string.h>

#include "oslibnet.h"
#include "lanman.h"

ODINDEBUGCHANNEL(WNETAP32-ERRORLOG)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

#define NERR_OK   0
#define NERR_BASE 1 /* @@@PH DUMMY ! */


#undef NET_API_STATUS
#define NET_API_STATUS DWORD

typedef unsigned long LPHLOG;


/*****************************************************************************
 * Name      : NET_API_STATUS NetAuditClear
 * Purpose   : clears the audit log on a server and, optionally, saves the
 *             entrie sin a backup file
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpBackupFile
 *             LPWSTR  lpService
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION3(NET_API_STATUS,OS2NetAuditClear,LPWSTR,  lpServerName,
                                              LPWSTR,  lpBackupFile,
                                              LPWSTR,  lpService)
{
  dprintf(("NETAPI32: NetAuditClear not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetAuditRead
 * Purpose   : reads from the audit log on a server
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpService
 *             LPHLOG  auditloghandle
 *             DWORD   offset
 *             LPDWORD reserved1
 *             DWORD   reserved2
 *             DWORD   offsetflag
 *             LPBYTE* bufptr
 *             DWORD   prefmaxlen
 *             LPDWORD bytesread
 *             LPDWORD totalavailable
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

typedef unsigned long LPHLOG;
ODINFUNCTION11(NET_API_STATUS,OS2NetAuditRead,LPWSTR,  lpServerName,
                                              LPWSTR,  lpService,
                                              LPHLOG,  auditloghandle,
                                              DWORD,   offset,
                                              LPDWORD, reserved1,
                                              DWORD,   reserved2,
                                              DWORD,   offsetflag,
                                              LPBYTE*, bufptr,
                                              DWORD,   prefmaxlen,
                                              LPDWORD, bytesread,
                                              LPDWORD, totalavailable)
{
  dprintf(("NETAPI32: NetAuditRead not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetAuditWrite
 * Purpose   : write to the audit log on a server
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpService
 *             LPHLOG  auditloghandle
 *             DWORD   offset
 *             LPDWORD reserved1
 *             DWORD   reserved2
 *             DWORD   offsetflag
 *             LPBYTE* bufptr
 *             DWORD   prefmaxlen
 *             LPDWORD bytesread
 *             LPDWORD totalavailable
 * Variables :
 * Result    : wrong
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION11(NET_API_STATUS,OS2NetAuditWrite,LPWSTR,  lpServerName,
                                               LPWSTR,  lpService,
                                               LPHLOG,  auditloghandle,
                                               DWORD,   offset,
                                               LPDWORD, reserved1,
                                               DWORD,   reserved2,
                                               DWORD,   offsetflag,
                                               LPBYTE*, bufptr,
                                               DWORD,   prefmaxlen,
                                               LPDWORD, byteswritten,
                                               LPDWORD, totalavailable)
{
  dprintf(("NETAPI32: NetAuditWrite not implemented\n"));

  return (NERR_BASE);
}




/*****************************************************************************
 * Name      : NET_API_STATUS NetErrorLogClear
 * Purpose   : clears the error log and optionally saves the entires in a
 *             backup file
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpBackupFile
 *             LPBYTE* reserved
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION3(NET_API_STATUS,OS2NetErrorLogClear,LPWSTR,  lpServerName,
                                                 LPWSTR,  lpBackupFile,
                                                 LPBYTE*, reserved)
{
  dprintf(("NETAPI32: NetErrorLogClear not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetErrorLogRead
 * Purpose   : reads from the error log on a server
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  reserved1
 *             LPHLOG  errorloghandle
 *             DWORD   offset
 *             LPDWORD reserved2
 *             DWORD   reserved3
 *             DWORD   offsetflag
 *             LPBYTE* bufptr
 *             DWORD   prefmaxlen
 *             LPDWORD bytesread
 *             LPDWORD totalbytes
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION11(NET_API_STATUS,OS2NetErrorLogRead,LPWSTR,  lpServerName,
                                                 LPWSTR,  reserved1,
                                                 LPHLOG,  errorloghandle,
                                                 DWORD,   offset,
                                                 LPDWORD, reserved2,
                                                 DWORD,   reserved3,
                                                 DWORD,   offsetflag,
                                                 LPBYTE*, bufptr,
                                                 DWORD,   prefmaxlen,
                                                 LPDWORD, bytesread,
                                                 LPDWORD, totalbytes)
{
  dprintf(("NETAPI32: NetErrorLogRead not implemented\n"));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetErrorLogWrite
 * Purpose   : write to the error log on a server
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  reserved1
 *             LPHLOG  auditloghandle
 *             DWORD   offset
 *             LPDWORD reserved2
 *             DWORD   reserved3
 *             DWORD   offsetflag
 *             LPBYTE* bufptr
 *             DWORD   prefmaxlen
 *             LPDWORD byteswritten
 *             LPDWORD totalavailable
 * Variables :
 * Result    : wrong
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION11(NET_API_STATUS,OS2NetErrorLogWrite,LPWSTR,  lpServerName,
                                                  LPWSTR,  reserved1,
                                                  LPHLOG,  auditloghandle,
                                                  DWORD,   offset,
                                                  LPDWORD, reserved2,
                                                  DWORD,   reserved3,
                                                  DWORD,   offsetflag,
                                                  LPBYTE*, bufptr,
                                                  DWORD,   prefmaxlen,
                                                  LPDWORD, byteswritten,
                                                  LPDWORD, totalavailable)
{
  dprintf(("NETAPI32: NetErrorLogWrite not implemented\n"));

  return (NERR_BASE);
}