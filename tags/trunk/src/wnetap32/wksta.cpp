/* $Id: wksta.cpp,v 1.1 2001-09-06 22:23:41 phaller Exp $ */

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

ODINDEBUGCHANNEL(WNETAP32-WKSTA)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/


/*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaGetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    : NT understands modes 100, 101, 102, 302, 402, and 502
 *             The APIs allocate the buffer dynamically.
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:11:21]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetWkstaGetInfo,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr)

{
  dprintf(("NETAPI32: NetWkstaGetInfo(%s, %d, %08x)\n"
           ,servername, level, *bufptr
         ));
  
  // Convert servername to ASCII
  char *asciiServername = NULL;
  if (servername) asciiServername = UnicodeToAsciiString(servername);
  
  // @@@PH convert information modes!
  int iOS2Level = 100;
  switch (level)
  {
    case 100: iOS2Level = 10; break;
    case 101: iOS2Level = 1; break;
    case 102: iOS2Level = 1; break;
    case 302: iOS2Level = 1; break;
    case 402: iOS2Level = 1; break;
    case 502: iOS2Level = 1; break;
  }
  
  ULONG  ulBytesAvailable;
  DWORD  rc;
  
  // determine required size of buffer
  char pOS2Buffer[4096];
  rc = OSLibNetWkstaGetInfo((const unsigned char*)asciiServername, 
                            iOS2Level,
                            (unsigned char*)pOS2Buffer,
                            sizeof(pOS2Buffer),
                            &ulBytesAvailable);
  if (asciiServername) FreeAsciiString(asciiServername);
  
  if (rc == NERR_OK)
  {
    // @@@PH now convert the requested structure to UNICODE
    switch (level)
    {
      case 100: // system information - guest access
      {
        PWKSTA_INFO_100 pwki100;
        struct wksta_info_10 *pOS2wki10 = (struct wksta_info_10 *)pOS2Buffer;
        
        // calculate new size for target buffer
        int iSizeComputername = (lstrlenA((LPCSTR)pOS2wki10->wki10_computername) + 1) * 2;
        int iSizeLangroup = (lstrlenA((LPCSTR)pOS2wki10->wki10_langroup) + 1) * 2;
        int iSize = sizeof(WKSTA_INFO_100) + iSizeComputername + iSizeLangroup;

        rc = OS2NetApiBufferAllocate(iSize, (LPVOID*)&pwki100);
        if (!rc)
        {
          // pointer to the data area
          PBYTE pData = (PBYTE)pwki100 + sizeof(WKSTA_INFO_100);
          
          pwki100->wki100_platform_id = 0; //@@@PH dummy
          pwki100->wki100_computername = lstrcpyAtoW( (LPWSTR)pData, (LPCSTR)pOS2wki10->wki10_computername);
          pData += iSizeComputername;
          pwki100->wki100_langroup = lstrcpyAtoW( (LPWSTR)pData, (LPCSTR)pOS2wki10->wki10_langroup);
          pData += iSizeLangroup;
          pwki100->wki100_ver_major = pOS2wki10->wki10_ver_major;
          pwki100->wki100_ver_minor = pOS2wki10->wki10_ver_minor;
        }

        // the caller is responsible for freeing the memory!
        *bufptr = (LPBYTE)pwki100;
        break;
      }
      
      
      case 101: // system information - guest access
      {
        PWKSTA_INFO_101 pwki101;
        struct wksta_info_1 *pOS2wki1 = (struct wksta_info_1 *)pOS2Buffer;
        
        // calculate new size for target buffer
        int iSizeComputername = (lstrlenA((LPCSTR)pOS2wki1->wki1_computername) + 1) * 2;
        int iSizeLangroup = (lstrlenA((LPCSTR)pOS2wki1->wki1_langroup) + 1) * 2;
        int iSizeLanroot  = (lstrlenA((LPCSTR)pOS2wki1->wki1_root) + 1) * 2;
        int iSize = sizeof(WKSTA_INFO_101) + iSizeComputername + iSizeLangroup + iSizeLanroot;

        rc = OS2NetApiBufferAllocate(iSize, (LPVOID*)&pwki101);
        if (!rc)
        {
          // pointer to the data area
          PBYTE pData = (PBYTE)pwki101 + sizeof(WKSTA_INFO_101);
        
          pwki101->wki101_platform_id = 0; //@@@PH dummy
          pwki101->wki101_computername = lstrcpyAtoW( (LPWSTR)pData, (LPCSTR)pOS2wki1->wki1_computername);
          pData += iSizeComputername;
          pwki101->wki101_langroup = lstrcpyAtoW( (LPWSTR)pData, (LPCSTR)pOS2wki1->wki1_langroup);
          pData += iSizeLangroup;
          pwki101->wki101_ver_major = pOS2wki1->wki1_ver_major;
          pwki101->wki101_ver_minor = pOS2wki1->wki1_ver_minor;
          pwki101->wki101_lanroot = lstrcpyAtoW( (LPWSTR)pData, (LPCSTR)pOS2wki1->wki1_root);
          pData += iSizeLanroot;
        }

        // the caller is responsible for freeing the memory!
        *bufptr = (LPBYTE)pwki101;
        break;
      }
      
      
      case 102: // system information - guest access
      {
        PWKSTA_INFO_102 pwki102;
        struct wksta_info_1 *pOS2wki1 = (struct wksta_info_1 *)pOS2Buffer;
        
        // calculate new size for target buffer
        int iSizeComputername = (lstrlenA((LPCSTR)pOS2wki1->wki1_computername) + 1) * 2;
        int iSizeLangroup = (lstrlenA((LPCSTR)pOS2wki1->wki1_langroup) + 1) * 2;
        int iSizeLanroot  = (lstrlenA((LPCSTR)pOS2wki1->wki1_root) + 1) * 2;
        int iSize = sizeof(WKSTA_INFO_102) + iSizeComputername + iSizeLangroup + iSizeLanroot;

        rc = OS2NetApiBufferAllocate(iSize, (LPVOID*)&pwki102);
        if (!rc)
        {
          // pointer to the data area
          PBYTE pData = (PBYTE)pwki102 + sizeof(WKSTA_INFO_102);
        
          pwki102->wki102_platform_id = 0; //@@@PH dummy
          pwki102->wki102_computername = lstrcpyAtoW( (LPWSTR)pData, (LPCSTR)pOS2wki1->wki1_computername);
          pData += iSizeComputername;
          pwki102->wki102_langroup = lstrcpyAtoW( (LPWSTR)pData, (LPCSTR)pOS2wki1->wki1_langroup);
          pData += iSizeLangroup;
          pwki102->wki102_ver_major = pOS2wki1->wki1_ver_major;
          pwki102->wki102_ver_minor = pOS2wki1->wki1_ver_minor;
          pwki102->wki102_lanroot = lstrcpyAtoW( (LPWSTR)pData, (LPCSTR)pOS2wki1->wki1_root);
          pData += iSizeLanroot;
          pwki102->wki102_logged_on_users = 0; // @@@PH dummy
        }

        // the caller is responsible for freeing the memory!
        *bufptr = (LPBYTE)pwki102;
        break;
      }
      
      
      case 502: // system information - guest access
      {
        PWKSTA_INFO_502 pwki502;
        struct wksta_info_1 *pOS2wki1 = (struct wksta_info_1 *)pOS2Buffer;
        
        rc = OS2NetApiBufferAllocate(sizeof(WKSTA_INFO_502), (LPVOID*)&pwki502);;
        if (!rc)
        {
          char *hs = (char *)pOS2wki1->wki1_wrkheuristics;
          
          pwki502->wki502_char_wait = pOS2wki1->wki1_charwait;
          pwki502->wki502_collection_time = pOS2wki1->wki1_chartime;
          pwki502->wki502_maximum_collection_count = pOS2wki1->wki1_charcount;
          pwki502->wki502_keep_conn = pOS2wki1->wki1_keepconn;
          pwki502->wki502_max_cmds = pOS2wki1->wki1_maxcmds;
          pwki502->wki502_sess_timeout = pOS2wki1->wki1_sesstimeout;
          pwki502->wki502_siz_char_buf = pOS2wki1->wki1_sizcharbuf;
          pwki502->wki502_max_threads = pOS2wki1->wki1_maxthreads;
          
          pwki502->wki502_lock_quota = 0;
          pwki502->wki502_lock_increment = 0;
          pwki502->wki502_lock_maximum = 0;
          pwki502->wki502_pipe_increment = 0;
          pwki502->wki502_pipe_maximum = 0;
          pwki502->wki502_cache_file_timeout = 0;
          pwki502->wki502_dormant_file_limit = 0;
          pwki502->wki502_read_ahead_throughput = 0;
          
          pwki502->wki502_num_mailslot_buffers = pOS2wki1->wki1_mailslots;
          pwki502->wki502_num_srv_announce_buffers = pOS2wki1->wki1_numdgrambuf;
          pwki502->wki502_max_illegal_datagram_events = 0;
          pwki502->wki502_illegal_datagram_event_reset_frequency = 0;
          pwki502->wki502_log_election_packets = FALSE;
          
          pwki502->wki502_use_opportunistic_locking = (hs[0] != '0') ? TRUE : FALSE;
          pwki502->wki502_use_unlock_behind = (hs[1] != '0') ? TRUE : FALSE;
          pwki502->wki502_use_close_behind = (hs[2] != '0') ? TRUE : FALSE;
          pwki502->wki502_buf_named_pipes = (hs[3] != '0') ? TRUE : FALSE;
          pwki502->wki502_use_lock_read_unlock = (hs[4] != '0') ? TRUE : FALSE;
          pwki502->wki502_utilize_nt_caching = TRUE; // sure we do ;-)
          pwki502->wki502_use_raw_read = (hs[12] != '0') ? TRUE : FALSE;
          pwki502->wki502_use_raw_write = (hs[13] != '0') ? TRUE : FALSE;
          pwki502->wki502_use_write_raw_data = (hs[29] != '0') ? TRUE : FALSE;
          pwki502->wki502_use_encryption = (hs[21] != '0') ? TRUE : FALSE;
          pwki502->wki502_buf_files_deny_write = (hs[23] != '0') ? TRUE : FALSE;
          pwki502->wki502_buf_read_only_files = (hs[24] != '0') ? TRUE : FALSE;
          pwki502->wki502_force_core_create_mode = (hs[27] != '0') ? TRUE : FALSE;
          pwki502->wki502_use_512_byte_max_transfer =  FALSE; // @@@PH
        }
        
        // the caller is responsible for freeing the memory!
        *bufptr = (LPBYTE)pwki502;
        break;
      }
    }
  }
  
  // @@@PH convert return code to NT code
  return (rc);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buffer
 *             LPDWORD parm_err
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:11:39]
 *****************************************************************************/
ODINFUNCTION4(NET_API_STATUS, OS2NetWkstaSetInfo,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buffer,
              LPDWORD, parm_err)

{

  dprintf(("NETAPI32: NetWkstaSetInfo(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buffer, parm_err
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaTransportAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:12:18]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetWkstaTransportAdd,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf)

{

  dprintf(("NETAPI32: NetWkstaTransportAdd(%s, %d, %08x)not implemented\n"
           ,servername, level, buf
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaTransportDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR transportname
 *             DWORD ucond
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:13:11]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetWkstaTransportDel,
              LPWSTR, servername,
              LPWSTR, transportname,
              DWORD, ucond)

{

  dprintf(("NETAPI32: NetWkstaTransportDel(%08x, %08x, %d) not implemented\n"
           ,servername, transportname, ucond
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaTransportEnum
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
 * Author    : Markus Montkowski [09.07.98 22:13:44]
 *****************************************************************************/
ODINFUNCTION7(NET_API_STATUS, OS2NetWkstaTransportEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resumehandle)

{

  dprintf(("NETAPI32: NetWkstaTransportEnum(%08x, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaUserEnum
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
 * Author    : Markus Montkowski [09.07.98 22:14:05]
 *****************************************************************************/
ODINFUNCTION7(NET_API_STATUS, OS2NetWkstaUserEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resumehandle)

{

  dprintf(("NETAPI32: NetWkstaUserEnum(%08x, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaUserGetInfo
 * Purpose   :
 * Parameters: LPWSTR reserved
 *             DWORD level
 *             LPBYTE *bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 22:14:17]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetWkstaUserGetInfo,
              LPWSTR, reserved,
              DWORD, level,
              LPBYTE *, bufptr)

{

  dprintf(("NETAPI32: NetWkstaUserGetInfo(%08x, %d, %08x) not implemented\n"
           ,reserved, level, *bufptr
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetWkstaUserSetInfo
 * Purpose   :
 * Parameters: LPWSTR reserved
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
 * Author    : Markus Montkowski [09.07.98 22:16:08]
 *****************************************************************************/
ODINFUNCTION4(NET_API_STATUS, OS2NetWkstaUserSetInfo,
              LPWSTR, reserved,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, parm_err)

{

  dprintf(("NETAPI32: NetWkstaUserSetInfo(%08x, %d, %08x, %08x) not implemented\n"
           ,reserved, level, buf, parm_err
         ));

  return (NERR_BASE);
}
