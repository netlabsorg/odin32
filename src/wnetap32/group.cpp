/* $Id: group.cpp,v 1.1 2001-09-06 22:23:39 phaller Exp $ */

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

ODINDEBUGCHANNEL(WNETAP32-GROUP)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/


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

ODINFUNCTION4(NET_API_STATUS, OS2NetGroupAdd,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)
{
  dprintf(("NETAPI32: NetGroupAdd not implemented\n"));

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

ODINFUNCTION3(NET_API_STATUS, OS2NetGroupAddUser,
              LPWSTR, servername,
              LPWSTR, GroupName,
              LPWSTR, username)
{
  dprintf(("NETAPI32: NetGroupAddUser not implemented\n"));

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

ODINFUNCTION2(NET_API_STATUS, OS2NetGroupDel,
              LPWSTR, servername,
              LPWSTR, groupname)
{
  dprintf(("NETAPI32: NetGroupDel not implemented\n"));

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

ODINFUNCTION3(NET_API_STATUS, OS2NetGroupDelUser,
              LPWSTR, servername,
              LPWSTR, GroupName,
              LPWSTR, Username)
{
  dprintf(("NETAPI32: NetGroupDelUser not implemented\n"));

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

ODINFUNCTION7(NET_API_STATUS, OS2NetGroupEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resume_handle)
{
  dprintf(("NETAPI32: NetGroupEnum not implemented\n"));

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

ODINFUNCTION4(NET_API_STATUS, OS2NetGroupGetInfo,
              LPWSTR, servername,
              LPWSTR, groupname,
              DWORD, level,
              LPBYTE *, bufptr)
{
  dprintf(("NETAPI32: NetGroupGetInfo not implemented\n"));

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

ODINFUNCTION8(NET_API_STATUS, OS2NetGroupGetUsers,
              LPWSTR, servername,
              LPWSTR, groupname,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resumeHandle)
{
  dprintf(("NETAPI32: NetGroupGetUsers not implemented\n"));

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

ODINFUNCTION5(NET_API_STATUS, OS2NetGroupSetInfo,
              LPWSTR, servername,
              LPWSTR, groupname,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)
{
  dprintf(("NETAPI32: NetGroupSetInfo not implemented\n"));

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

ODINFUNCTION5(NET_API_STATUS, OS2NetGroupSetUsers,
              LPWSTR, servername,
              LPWSTR, groupname,
              DWORD, level,
              LPBYTE, buf,
              DWORD, NewMemberCount)
{
  dprintf(("NETAPI32: NetGroupSetUsers not implemented\n"));

  return (NERR_BASE);
}


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
ODINFUNCTION4(NET_API_STATUS, OS2NetLocalGroupAdd,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)
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
ODINFUNCTION5(NET_API_STATUS, OS2NetLocalGroupAddMembers,
              LPWSTR, servername,
              LPWSTR, LocalGroupName,
              DWORD, level,
              LPBYTE, buf,
              DWORD, membercount)

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
ODINFUNCTION2(NET_API_STATUS, OS2NetLocalGroupDel,
              LPWSTR, servername,
              LPWSTR, LocalGroupName)

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
ODINFUNCTION5(NET_API_STATUS, OS2NetLocalGroupDelMembers,
              LPWSTR, servername,
              LPWSTR, LocalGroupName,
              DWORD, level,
              LPBYTE, buf,
              DWORD, membercount)

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
ODINFUNCTION7(NET_API_STATUS, OS2NetLocalGroupEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resumehandle)

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
ODINFUNCTION4(NET_API_STATUS, OS2NetLocalGroupGetInfo,
              LPWSTR, servername,
              LPWSTR, LocalGroupName,
              DWORD, level,
              LPBYTE *, bufptr)

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
ODINFUNCTION8(NET_API_STATUS, OS2NetLocalGroupGetMembers,
              LPWSTR, servername,
              LPWSTR, localgroupname,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resumehandle)

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
ODINFUNCTION5(NET_API_STATUS, OS2NetLocalGroupSetInfo,
              LPWSTR, servername,
              LPWSTR, LocalGroupName,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

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
ODINFUNCTION5(NET_API_STATUS, OS2NetLocalGroupSetMembers,
              LPWSTR, servername,
              LPWSTR, LocalGroupName,
              DWORD, level,
              LPBYTE, buf,
              DWORD, totalentries)

{

  dprintf(("NETAPI32: NetLocalGroupSetMembers(%08x, %08x, %d, %08x, %d) not implemented\n"
           ,servername, LocalGroupName, level, buf, totalentries
         ));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS NetLocalGroupAddMember
 * Purpose   : gives an existing user account or global group membership in
 *             an existing local group
 * Parameters: LPWSTR  lpServerName
 *             LPWSTR  lpGroupName
 *             PSID    membersid
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/08/18 00:15]
 *****************************************************************************/

ODINFUNCTION3(NET_API_STATUS,OS2NetLocalGroupAddMember,LPWSTR,  lpServerName,
                                                       LPWSTR,  lpGroupName,
                                                       PSID,    membersid)
{
  dprintf(("NETAPI32: NetLocalGroupAddMember not implemented\n"));

  return (NERR_BASE);
}
