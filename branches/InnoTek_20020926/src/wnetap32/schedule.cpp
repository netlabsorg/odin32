/* $Id: schedule.cpp,v 1.1 2001-09-06 22:23:40 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * NETAPI32 schedule APIs
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

ODINDEBUGCHANNEL(WNETAP32-SCHEDULE)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

#define NCBNAMSZ        16
#define MAX_LANA       254


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
ODINFUNCTION3(NET_API_STATUS, OS2NetScheduleJobAdd,
              LPWSTR, Servername,
              LPBYTE, Buffer,
              LPDWORD, JobId)

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
ODINFUNCTION3(NET_API_STATUS, OS2NetScheduleJobDel,
              LPWSTR, Servername,
              DWORD, MinJobId,
              DWORD, MaxJobId)

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
ODINFUNCTION6(NET_API_STATUS, OS2NetScheduleJobEnum,
              LPWSTR, Servername,
              LPBYTE *, PointerToBuffer,
              DWORD, PreferredMaximumLength,
              LPDWORD, EntriesRead,
              LPDWORD, TotalEntries,
              LPDWORD, ResumeHandle)

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
ODINFUNCTION3(NET_API_STATUS, OS2NetScheduleJobGetInfo,
              LPWSTR, Servername,
              DWORD, JobId,
              LPBYTE *, PointerToBuffer)

{

  dprintf(("NETAPI32: NetScheduleJobGetInfo(%08x, %d, %08x) not implemented\n"
           ,Servername, JobId, *PointerToBuffer
         ));

  return (NERR_BASE);
}
