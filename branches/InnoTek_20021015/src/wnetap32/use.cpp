/* $Id: use.cpp,v 1.1 2001-09-07 08:20:59 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * NETAPI32 "use" APIs
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

ODINDEBUGCHANNEL(WNETAP32-USE)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/


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