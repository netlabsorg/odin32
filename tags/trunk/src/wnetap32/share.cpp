/* $Id: share.cpp,v 1.1 2001-09-07 08:20:58 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * NETAPI32 share APIs
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

ODINDEBUGCHANNEL(WNETAP32-SHARE)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/


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