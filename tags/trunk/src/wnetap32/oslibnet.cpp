/* $Id */
/*
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


//******************************************************************************
// translate OS/2 error codes to Windows codes
// NOTE: add all codes you need, list is not complete!
//******************************************************************************
static DWORD error2WinError(APIRET rc,DWORD defaultCode = ERROR_NOT_ENOUGH_MEMORY_W)
{
  switch (rc)
  {
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
        return defaultCode;
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
  APIRET rc = Net32WkstaGetInfo(pszServer, ulLevel, pbBuffer, ulBuffer, pulTotalAvail);
  return error2WinError(rc);
}


