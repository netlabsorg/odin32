/* $Id: user.cpp,v 1.1 2001-09-06 22:23:41 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * NETAPI32 user functions
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

ODINDEBUGCHANNEL(WNETAP32-USER)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/


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
ODINFUNCTION4(NET_API_STATUS, OS2NetUserAdd,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

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
ODINFUNCTION4(NET_API_STATUS, OS2NetUserChangePassword,
              LPWSTR, domainname,
              LPWSTR, username,
              LPWSTR, oldpassword,
              LPWSTR, newpassword)

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
ODINFUNCTION2(NET_API_STATUS, OS2NetUserDel,
              LPWSTR, servername,
              LPWSTR, username)

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
ODINFUNCTION8(NET_API_STATUS, OS2NetUserEnum,
              LPWSTR, servername,
              DWORD, level,
              DWORD, filter,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resume_handle)

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
ODINFUNCTION7(NET_API_STATUS, OS2NetUserGetGroups,
              LPWSTR, servername,
              LPWSTR, username,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries)

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
ODINFUNCTION4(NET_API_STATUS, OS2NetUserGetInfo,
              LPWSTR, servername,
              LPWSTR, username,
              DWORD, level,
              LPBYTE *, bufptr)

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
ODINFUNCTION8(NET_API_STATUS, OS2NetUserGetLocalGroups,
              LPWSTR, servername,
              LPWSTR, username,
              DWORD, level,
              DWORD, flags,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries)

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
ODINFUNCTION3(NET_API_STATUS, OS2NetUserModalsGet,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr)

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
ODINFUNCTION4(NET_API_STATUS, OS2NetUserModalsSet,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

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
ODINFUNCTION5(NET_API_STATUS, OS2NetUserSetGroups,
              LPWSTR, servername,
              LPWSTR, username,
              DWORD, level,
              LPBYTE, buf,
              DWORD, num_entries)

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
ODINFUNCTION5(NET_API_STATUS, OS2NetUserSetInfo,
              LPWSTR, servername,
              LPWSTR, username,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

{

  dprintf(("NETAPI32: NetUserSetInfo(%08x, %08x, %d, %08x, %08x) not implemented\n"
           ,servername, username, level, buf, parm_err
         ));

  return (NERR_BASE);
}
