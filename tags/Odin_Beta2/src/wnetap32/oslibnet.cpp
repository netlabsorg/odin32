/*  $Id: oslibnet.cpp,v 1.6 2002-03-08 11:37:10 sandervl Exp $
 *
 * Wrappers for OS/2 Netbios/Network/LAN API
 *
 * Copyright 2000 Patrick Haller (patrick.haller@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_NPIPES
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <win32api.h>
#include <winconst.h>
#include <misc.h>

#define PURE_32
#include <netcons.h>
#include <wksta.h>
#include <neterr.h>
#include <netstats.h>
#include <server.h>
#include <ncb.h>
#include <netbios.h>
// #include <netb_1_c.h>
// #include <netb_2_c.h>
// #include <netb_4_c.h>

#include "wnetap32.h"
#include "oslibnet.h"


//******************************************************************************
// translate OS/2 error codes to Windows codes
// NOTE: add all codes you need, list is not complete!
//******************************************************************************
static DWORD error2WinError(APIRET rc)
{
  switch (rc)
  {
    // NT/LAN Manager specific error codes
  case NERR_NetNotStarted:     return NERR_NetNotStarted;
  case NERR_UnknownServer:     return NERR_UnknownServer;
  case NERR_ShareMem:          return NERR_ShareMem;
  case NERR_NoNetworkResource: return NERR_NoNetworkResource;
  case NERR_RemoteOnly:        return NERR_RemoteOnly;
  case NERR_DevNotRedirected:  return NERR_DevNotRedirected;
  case NERR_ServerNotStarted:  return NERR_ServerNotStarted;
  case NERR_ItemNotFound:      return NERR_ItemNotFound;
  case NERR_UnknownDevDir:     return NERR_UnknownDevDir;
  case NERR_RedirectedPath:    return NERR_RedirectedPath;
  case NERR_DuplicateShare:    return NERR_DuplicateShare;
  case NERR_NoRoom:            return NERR_NoRoom;
  case NERR_TooManyItems:      return NERR_TooManyItems;
  case NERR_InvalidMaxUsers:   return NERR_InvalidMaxUsers;
  case NERR_BufTooSmall:       return NERR_BufTooSmall;
  case NERR_RemoteErr:         return NERR_RemoteErr;
  case NERR_LanmanIniError:    return NERR_LanmanIniError;
    // case NERR_OS2IoctlError
  case NERR_NetworkError:      return NERR_NetworkError;
  case NERR_WkstaNotStarted:   return NERR_WkstaNotStarted;
  case NERR_BASE + 39:         return NERR_BASE + 39;    // NERR_BrowserNotStarted
  case NERR_InternalError:     return NERR_InternalError;
  case NERR_BadTransactConfig: return NERR_BadTransactConfig;
  case NERR_InvalidAPI:        return NERR_InvalidAPI;
  case NERR_BadEventName:      return NERR_BadEventName;
  case NERR_BASE + 44:         return NERR_BASE + 44; // NERR_DupNameReboot

  // ...

    case NO_ERROR: //0
        return ERROR_SUCCESS_W;

    case ERROR_INVALID_FUNCTION: //1
        return ERROR_INVALID_FUNCTION_W;

    case ERROR_FILE_NOT_FOUND: //2
        return ERROR_FILE_NOT_FOUND_W;

    case ERROR_PATH_NOT_FOUND: //3
        return ERROR_PATH_NOT_FOUND_W;

    case ERROR_TOO_MANY_OPEN_FILES: //4
        return ERROR_TOO_MANY_OPEN_FILES_W;

    case ERROR_ACCESS_DENIED: //5
        return ERROR_ACCESS_DENIED_W;

    case ERROR_INVALID_HANDLE: //6
        return ERROR_INVALID_HANDLE_W;

    case ERROR_NOT_ENOUGH_MEMORY: //8
        return ERROR_NOT_ENOUGH_MEMORY_W;

    case ERROR_BAD_FORMAT: //11
        return ERROR_BAD_FORMAT_W;

    case ERROR_INVALID_ACCESS: //12
      return ERROR_INVALID_ACCESS_W;

    case ERROR_NO_MORE_FILES: //18
        return ERROR_NO_MORE_FILES_W;

    case ERROR_WRITE_PROTECT: //19
        return ERROR_WRITE_PROTECT_W;

    case ERROR_NOT_DOS_DISK: //26
        return ERROR_NOT_DOS_DISK_W;

    case ERROR_WRITE_FAULT: //29
        return ERROR_WRITE_FAULT_W;

    case ERROR_SHARING_VIOLATION: //32
        return ERROR_SHARING_VIOLATION_W;

    case ERROR_LOCK_VIOLATION: //32
        return ERROR_LOCK_VIOLATION_W;

    case ERROR_SHARING_BUFFER_EXCEEDED: //36
        return ERROR_SHARING_BUFFER_EXCEEDED_W;

    case ERROR_CANNOT_MAKE: //82
        return ERROR_CANNOT_MAKE_W;

    case ERROR_OUT_OF_STRUCTURES: //84
        return ERROR_OUT_OF_STRUCTURES_W;

    case ERROR_INVALID_PARAMETER: //87
        return ERROR_INVALID_PARAMETER_W;

    case ERROR_INTERRUPT: //95
        return ERROR_INVALID_AT_INTERRUPT_TIME_W; //CB: right???

    case ERROR_DEVICE_IN_USE: //99
  return ERROR_DEVICE_IN_USE_W;

    case ERROR_DRIVE_LOCKED: //108
        return ERROR_DRIVE_LOCKED_W;

    case ERROR_BROKEN_PIPE: //109
        return ERROR_BROKEN_PIPE_W;

    case ERROR_OPEN_FAILED: //110
  return ERROR_OPEN_FAILED_W;

    case ERROR_BUFFER_OVERFLOW: //111
        return ERROR_BUFFER_OVERFLOW_W;

    case ERROR_DISK_FULL: //112
  return ERROR_DISK_FULL_W;

    case ERROR_NO_MORE_SEARCH_HANDLES: //113
        return ERROR_NO_MORE_SEARCH_HANDLES_W;

    case ERROR_SEM_TIMEOUT: //121
        return ERROR_SEM_TIMEOUT_W;

    case ERROR_DIRECT_ACCESS_HANDLE: //130
        return ERROR_DIRECT_ACCESS_HANDLE_W;

    case ERROR_NEGATIVE_SEEK: //131
        return ERROR_NEGATIVE_SEEK;

    case ERROR_SEEK_ON_DEVICE: //132
        return ERROR_SEEK_ON_DEVICE_W;

    case ERROR_DISCARDED: //157
        return ERROR_DISCARDED_W;

    case ERROR_FILENAME_EXCED_RANGE: //206
        return ERROR_FILENAME_EXCED_RANGE_W;

    case ERROR_META_EXPANSION_TOO_LONG: //208
        return ERROR_META_EXPANSION_TOO_LONG_W;

    case ERROR_BAD_PIPE: //230
        return ERROR_BAD_PIPE_W;

    case ERROR_PIPE_BUSY: //231
        return ERROR_PIPE_BUSY_W;

    case ERROR_NO_DATA: //232
        return ERROR_NO_DATA_W;

    case ERROR_PIPE_NOT_CONNECTED: //233
        return ERROR_PIPE_NOT_CONNECTED_W;

    case ERROR_MORE_DATA: //234
        return ERROR_MORE_DATA_W;

    case ERROR_INVALID_EA_NAME: //254
        return ERROR_INVALID_EA_NAME_W;

    case ERROR_EA_LIST_INCONSISTENT: //255
        return ERROR_EA_LIST_INCONSISTENT_W;

    case ERROR_EAS_DIDNT_FIT: //275
        return ERROR_EAS_DIDNT_FIT;

    default:
      dprintf(("WARNING: error2WinError: error %d not included!!!!", rc));
        return rc;
  }
}


/*****************************************************************************
 * Name      : NET_API_STATUS OSLibNetWkstaGetInfo
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller 2000/01/10 01:42
 *****************************************************************************/

DWORD OSLibNetWkstaGetInfo (const unsigned char * pszServer,
                    unsigned long         ulLevel,
                    unsigned char       * pbBuffer,
                    unsigned long         ulBuffer,
                    unsigned long       * pulTotalAvail)
{
  USHORT sel = RestoreOS2FS();

  APIRET rc = error2WinError(Net32WkstaGetInfo(pszServer, ulLevel, pbBuffer, ulBuffer, pulTotalAvail));
  SetFS(sel);
  return rc;
}


/*****************************************************************************
 * Name      : NET_API_STATUS OSLibNetStatisticsGet
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller 2000/01/10 01:42
 *****************************************************************************/

DWORD OSLibNetStatisticsGet(const unsigned char * pszServer,
                            const unsigned char * pszService,
                            unsigned long         ulReserved,
                            unsigned long         ulLevel,
                            unsigned long         flOptions,
                            unsigned char       * pbBuffer,
                            unsigned long         ulBuffer,
                            unsigned long       * pulTotalAvail)
{
  USHORT sel = RestoreOS2FS();
  APIRET rc = error2WinError(Net32StatisticsGet2(pszServer,
                                  pszService,
                                  ulReserved,
                                  ulLevel,
                                  flOptions,
                                  pbBuffer,
                                  ulBuffer,
                                  pulTotalAvail));
  SetFS(sel);
  return rc;
}


/*****************************************************************************
 * Name      : NET_API_STATUS OSLibNetStatisticsGet
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller 2000/01/10 01:42
 *****************************************************************************/

DWORD OSLibNetServerEnum(const unsigned char * pszServer,
                         unsigned long         ulLevel,
                         unsigned char       * pbBuffer,
                         unsigned long         ulBufferLength,
                         unsigned long       * pulEntriesReturned,
                         unsigned long       * pulEntriesAvail,
                         unsigned long         ulServerType,
                         unsigned char       * pszDomain)
{
  USHORT sel = RestoreOS2FS();
  
  APIRET rc = error2WinError(Net32ServerEnum2(pszServer,
                                              ulLevel,
                                              pbBuffer,
                                              ulBufferLength,
                                              pulEntriesReturned,
                                              pulEntriesAvail,
                                              ulServerType,
                                              pszDomain));
  SetFS(sel);
  return rc;
}


/*****************************************************************************
 * Name      : OSLibNetBIOS
 * Purpose   :
 * Parameters: PNCB pncb (Win32-style of the structure!!!)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : TESTED
 *
 * Author    : Patrick Haller 2002/02/26 01:42
 *****************************************************************************/

UCHAR OSLibNetBIOS_Passthru(PNCB_w pncb)
{
  // IBM NETBIOS 3.0 as defined in ncb.h
  // UCHAR rc = NetBios( (struct ncb LSFAR *)pncb );
  
  // Note:
  // asynchronous operation probably not really supported here,
  // this has to be done with a wrapper thread!
  
  // @@@PH
  // open / close the right adapter based upon
  // the LANA number specified inside the NCB
  
  USHORT sel = RestoreOS2FS();
  UCHAR rc = NetBios32Submit(0, // send automatically to first adapter
                                // open & close are implicit
                             0, // single NCB w/o error retry!
                             (struct ncb LSFAR *)pncb );
  SetFS(sel);
  return rc;
}


/*****************************************************************************
 * Name      : OSLibNetBIOSEnum
 * Purpose   :
 * Parameters: PNCB pncb (Win32-style of the structure!!!)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : TESTED
 *
 * Author    : Patrick Haller 2002/02/26 01:42
 *****************************************************************************/

UCHAR OSLibNetBIOS_Enum(PNCB_w pncb)
{
  static BOOL flagEnumAvailable = FALSE;
  static UCHAR arrLANA[ MAX_LANA_w ];
  static int iLANALength = 0;
  USHORT sel = RestoreOS2FS();

  if (FALSE == flagEnumAvailable)
  {
    ULONG ulEntriesReturned  = 0;
    ULONG ulEntriesAvailable = 0;
    ULONG rc;
    PVOID pBuffer;
    ULONG ulBufferLength;
    struct netbios_info_1* pNBI1;
    
    // reset the array first
    memset( arrLANA, 0, sizeof( arrLANA ) );
    
    
    // determine number of available entries
    rc = NetBios32Enum(NULL,
                       0,
                       NULL,
                       0,
                       &ulEntriesReturned,
                       &ulEntriesAvailable);
    
    // if network adapters are available ...
    iLANALength = ulEntriesAvailable;
    if (0 != ulEntriesAvailable)
    {
      // allocate buffer of sufficient size
      ulBufferLength = ulEntriesAvailable * sizeof( struct netbios_info_1 );
      pBuffer = malloc( ulBufferLength );
      if (NULL == pBuffer)
      {
        SetFS(sel);
        pncb->ncb_retcode  = NRC_SYSTEM_w;
        return NRC_SYSTEM_w;
      }
        
      // enumerate all network drivers (net1, net2, ...)
      // and build the array of LANA numbers
      rc = NetBios32Enum(NULL,  // local machine
                         1,
                         (PUCHAR)pBuffer,
                         ulBufferLength,
                         &ulEntriesReturned,
                         &ulEntriesAvailable);
      pNBI1 = (struct netbios_info_1 *)pBuffer;
      for (ULONG ulCount = 0;
           ulCount < ulEntriesReturned;
           ulCount++)
      {
        arrLANA[ ulCount ] = pNBI1->nb1_lana_num;
        pNBI1++;
      }
      
      free( pBuffer );
    }
    // else no network adapters are available
    
    flagEnumAvailable = TRUE;
  }
  
  // copy the result
  PLANA_ENUM_w pLE = (PLANA_ENUM_w)pncb->ncb_buffer;
  pLE->length      = iLANALength;
  
  // compensate UCHAR size for the length field
  memcpy( pLE->lana,
          arrLANA,
          pncb->ncb_length - sizeof(UCHAR) );
  pncb->ncb_retcode  = NRC_GOODRET_w;
         
  SetFS(sel);
  return pncb->ncb_retcode;
}




/*****************************************************************************
 * Name      : OSLibNetBIOS
 * Purpose   :
 * Parameters: PNCB pncb (Win32-style of the structure!!!)
 * Variables :
 * Result    :
 * Remark    : this is for synchronous operation only!
 * Status    : TESTED
 *
 * Author    : Patrick Haller 2002/02/26 01:42
 *****************************************************************************/

UCHAR OSLibNetBIOS(PNCB_w pncb)
{
  dprintf(("NETBIOS: Netbios NCB: command=%02xh, retcode=%d, lsn=%d, num=%d, buffer=%08xh, "
           "length=%d callname='%s' name='%s' rto=%d sto=%d pfnCallback=%08xh, lana=%d, "
           "cmd_cplt=%d event=%08xh\n",
           pncb->ncb_command,
           pncb->ncb_retcode,
           pncb->ncb_lsn,
           pncb->ncb_num,
           pncb->ncb_buffer,
           pncb->ncb_length,
           pncb->ncb_callname,
           pncb->ncb_name,
           pncb->ncb_rto,
           pncb->ncb_sto,
           pncb->ncb_post,
           pncb->ncb_lana_num,
           pncb->ncb_cmd_cplt,
           pncb->ncb_event));
  
  // Request Router
  // The request router is responsible for mapping the incoming commands
  // and NCBs to their OS/2 NetBIOS pendant plus support of synchronous /
  // asynchronous handling of NCB processing.
  
  // Note:
  // for a first attempt at NetBIOS support, we just try to pass through
  // all supported commands and assume the structures are essentially
  // compatible.
  UCHAR rc;

  switch (pncb->ncb_command & ~ASYNCH_w)
  {
    case NCBENUM_w:
    {
      // enumerate all network drivers (net1, net2, ...)
      // and build the array of LANA numbers
      rc = OSLibNetBIOS_Enum( pncb );
      break;
    }
  
    // Note: NCBLANSTALERT_w
    // seems to be supported in asynchronous manner only,
    // we just try to pass it through!
  
    case NCBACTION_w:
      dprintf(("NCBACTION not yet implemented"));
      break;
  
    case NCBTRACE_w:
      dprintf(("NCBTRACE not yet implemented"));
      break;
    
    case NCBRESET_w:
      // This seems to cause the requester to go wild
      // (netbios session limit exceeded)
      // rc = OSLibNetBIOS_Passthru( pncb );
    
      // Win32 and OS/2 do have different behaviour upon RESET calls.
      // Returning OK here is experimental.
      pncb->ncb_retcode  = NRC_GOODRET_w;
      pncb->ncb_cmd_cplt = NRC_GOODRET_w;
      rc = NRC_GOODRET_w;
      break;
    
    
    default:
      rc = OSLibNetBIOS_Passthru( pncb );
      break;
  }
  
  dprintf(("NETBIOS: Netbios NCB: command=%02xh --> rc=%d\n", 
           pncb->ncb_command,
           rc));
  return rc;
}


/*****************************************************************************
 * Name      : 
 * Purpose   :
 * Parameters: PNCB pncb (Win32-style of the structure!!!)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : TESTED
 *
 * Author    : Patrick Haller 2002/02/26 01:42
 *****************************************************************************/

DWORD WIN32API OSLibNetbiosHlpHandler(LPVOID lpParam)
{
  PNCB_w pncb = (PNCB_w)lpParam;
  UCHAR ncb_cmd_original = pncb->ncb_command;
  void (* CALLBACK ncb_post_original)( struct _NCB_w * ) = pncb->ncb_post;

  // rewrite ncb for synchronous operation
  pncb->ncb_command &= ~ASYNCH_w;
  pncb->ncb_post    = 0;
  // ncb_cmd_cplt is expected to be NRC_PENDING_w
  // when we come here (set by the main Netbios() function)
  
  // synchronous operation
  // return code is expected to be stored inside pncb
  dprintf(("NETBIOS: async NETBIOS for command %02xh\n",
           pncb->ncb_command));
  
  OSLibNetBIOS( pncb );
  
  dprintf(("NETBIOS: async NETBIOS for command %02xh returned %d\n",
           pncb->ncb_command,
           pncb->ncb_retcode));
  
  // restore original command
  pncb->ncb_command = ncb_cmd_original;
  pncb->ncb_post    = ncb_post_original;
  
  // propagate the command complete value as netbios is expected
  // to do for asynchronous operation
  pncb->ncb_cmd_cplt= pncb->ncb_retcode;
  
  // perform post-operation notification!
  if (pncb->ncb_event)
  {
    // Netbios seems to use PulseEvent instead of SetEvent.
    
    // SetEvent( pncb->ncb_event );
    PulseEvent( pncb->ncb_event );
  }
  
  if (pncb->ncb_post != NULL)
  {
    // callback into post function
    (pncb->ncb_post)( pncb );
  }
  
  return pncb->ncb_retcode;
}
