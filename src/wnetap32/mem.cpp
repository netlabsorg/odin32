/* $Id: mem.cpp,v 1.1 2001-09-06 22:23:39 phaller Exp $ */

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

ODINDEBUGCHANNEL(WNETAP32-BUFFER)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/


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

ODINFUNCTION2(NET_API_STATUS, OS2NetApiBufferAllocate,
              DWORD, ByteCount,
              LPVOID *, Buffer)

{
  *Buffer = HEAP_malloc(ByteCount);
  return (NERR_OK);
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

ODINFUNCTION1(NET_API_STATUS, OS2NetApiBufferFree,
              LPVOID, Buffer)

{
  HEAP_free(Buffer);
  return (NERR_OK);
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

ODINFUNCTION3(NET_API_STATUS, OS2NetApiBufferReallocate,
              LPVOID, OldBuffer,
              DWORD, NewByteCount,
              LPVOID*, NewBuffer)
{
  *NewBuffer = HEAP_realloc(OldBuffer, NewByteCount);
  return (NERR_OK);
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

ODINFUNCTION2(NET_API_STATUS, OS2NetApiBufferSize,
              LPVOID, buffer,
              LPDWORD, lpByteCount)
{
  *lpByteCount = HEAP_size(buffer);
  return (NERR_OK);
}
