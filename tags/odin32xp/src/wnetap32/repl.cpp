/* $Id: repl.cpp,v 1.1 2001-09-06 22:23:40 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * NETAPI32 replication APIs
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

ODINDEBUGCHANNEL(WNETAP32-REPL)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

#define NCBNAMSZ        16
#define MAX_LANA       254


/*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirAdd
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
 * Author    : Markus Montkowski [09.07.98 21:47:52]
 *****************************************************************************/
ODINFUNCTION4(NET_API_STATUS, OS2NetReplExportDirAdd,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

{

  dprintf(("NETAPI32: NetReplExportDirAdd(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:48:42]
 *****************************************************************************/
ODINFUNCTION2(NET_API_STATUS, OS2NetReplExportDirDel,
              LPWSTR, servername,
              LPWSTR, dirname)

{

  dprintf(("NETAPI32: NetReplExportDirDel(%s, %s) not implemented\n"
           ,servername, dirname
         ));

  return (NERR_BASE);
}



/*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirEnum
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
 * Author    : Markus Montkowski [09.07.98 21:49:05]
 *****************************************************************************/
ODINFUNCTION7(NET_API_STATUS, OS2NetReplExportDirEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resumehandle)

{

  dprintf(("NETAPI32: NetReplExportDirEnum(%s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}
/*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:49:34]
 *****************************************************************************/
ODINFUNCTION4(NET_API_STATUS, OS2NetReplExportDirGetInfo,
              LPWSTR, servername,
              LPWSTR, dirname,
              DWORD, level,
              LPBYTE *, bufptr)

{

  dprintf(("NETAPI32: NetReplExportDirGetInfo(%s, %s, %d, %08x) not implemented\n"
           ,servername, dirname, level, *bufptr
         ));

  return (NERR_BASE);
}
/*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirLock
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:49:58]
 *****************************************************************************/
ODINFUNCTION2(NET_API_STATUS, OS2NetReplExportDirLock,
              LPWSTR, servername,
              LPWSTR, dirname)

{

  dprintf(("NETAPI32: NetReplExportDirLock(%s, %s) not implemented\n"
           ,servername, dirname
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
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
 * Author    : Markus Montkowski [09.07.98 21:50:31]
 *****************************************************************************/
ODINFUNCTION5(NET_API_STATUS, OS2NetReplExportDirSetInfo,
              LPWSTR, servername,
              LPWSTR, dirname,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

{

  dprintf(("NETAPI32: NetReplExportDirSetInfo(%s, %s, %d, %08x, %08x) not implemented\n"
           ,servername, dirname, level, buf, parm_err
         ));

  return (NERR_BASE);
}
/*****************************************************************************
 * Name      : NET_API_STATUS NetReplExportDirUnlock
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD unlockforce
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:51:12]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetReplExportDirUnlock,
              LPWSTR, servername,
              LPWSTR, dirname,
              DWORD, unlockforce)

{

  dprintf(("NETAPI32: NetReplExportDirUnlock(%s, %s, %d) not implemented\n"
           ,servername, dirname, unlockforce
         ));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetReplGetInfo
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
 * Author    : Markus Montkowski [09.07.98 21:51:37]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetReplGetInfo,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr)

{

  dprintf(("NETAPI32: NetReplGetInfo(%s, %d, %08x) not implemented\n"
           ,servername, level, *bufptr
         ));

  return (NERR_BASE);
}
/*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirAdd
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
 * Author    : Markus Montkowski [09.07.98 21:52:11]
 *****************************************************************************/
ODINFUNCTION4(NET_API_STATUS, OS2NetReplImportDirAdd,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

{

  dprintf(("NETAPI32: NetReplImportDirAdd(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:52:36]
 *****************************************************************************/
ODINFUNCTION2(NET_API_STATUS, OS2NetReplImportDirDel,
              LPWSTR, servername,
              LPWSTR, dirname)

{

  dprintf(("NETAPI32: NetReplImportDirDel(%s, %s) not implemented\n"
           ,servername, dirname
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirEnum
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
 * Author    : Markus Montkowski [09.07.98 21:52:56]
 *****************************************************************************/
ODINFUNCTION7(NET_API_STATUS, OS2NetReplImportDirEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resumehandle)

{

  dprintf(("NETAPI32: NetReplImportDirEnum(%s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}
/*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:53:24]
 *****************************************************************************/
ODINFUNCTION4(NET_API_STATUS, OS2NetReplImportDirGetInfo,
              LPWSTR, servername,
              LPWSTR, dirname,
              DWORD, level,
              LPBYTE *, bufptr)

{

  dprintf(("NETAPI32: NetReplImportDirGetInfo(%s, %s, %d, %08x) not implemented\n"
           ,servername, dirname, level, *bufptr
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirLock
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:53:45]
 *****************************************************************************/
ODINFUNCTION2(NET_API_STATUS, OS2NetReplImportDirLock,
              LPWSTR, servername,
              LPWSTR, dirname)

{

  dprintf(("NETAPI32: NetReplImportDirLock(%s, %s) not implemented\n"
           ,servername, dirname
         ));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetReplImportDirUnlock
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR dirname
 *             DWORD unlockforce
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:54:16]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetReplImportDirUnlock,
              LPWSTR, servername,
              LPWSTR, dirname,
              DWORD, unlockforce)

{

  dprintf(("NETAPI32: NetReplImportDirUnlock(%s, %s, %d) not implemented\n"
           ,servername, dirname, unlockforce
         ));

  return (NERR_BASE);
}
/*****************************************************************************
 * Name      : NET_API_STATUS NetReplSetInfo
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
 * Author    : Markus Montkowski [09.07.98 21:54:38]
 *****************************************************************************/
ODINFUNCTION4(NET_API_STATUS, OS2NetReplSetInfo,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

{

  dprintf(("NETAPI32: NetReplSetInfo(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, parm_err
         ));

  return (NERR_BASE);
}
