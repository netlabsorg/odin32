/* $Id: service.cpp,v 1.1 2001-09-07 08:20:58 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * NETAPI32 service manager APIs
 *
 * Copyright 1998 Patrick Haller
 *
 * Note: functions that return structures/buffers seem to append strings
 * at the end of the buffer. Currently, we just allocate the strings
 * "normally". Therefore a caller that just does a NetApiBufferFree() on the
 * returned buffer will leak all allocated strings.
 *
 */


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

ODINDEBUGCHANNEL(WNETAP32-SERVICE)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

#define NCBNAMSZ        16
#define MAX_LANA       254


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