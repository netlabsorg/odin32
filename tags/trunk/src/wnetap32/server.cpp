/* $Id: server.cpp,v 1.1 2001-09-06 22:23:40 phaller Exp $ */

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

ODINDEBUGCHANNEL(WNETAP32-SERVER)


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/


/*****************************************************************************
 * Name      : NET_API_STATUS NetServerDiskEnum
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
 * Author    : Markus Montkowski [09.07.98 21:56:38]
 *****************************************************************************/
ODINFUNCTION7(NET_API_STATUS, OS2NetServerDiskEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, lpdwEntriesRead,
              LPDWORD, lpdwTotalEntries,
              LPDWORD, resume_handle)

{

  dprintf(("NETAPI32: NetServerDiskEnum(%s) not implemented\n",
           servername));

  return (NERR_BASE);
}
/*****************************************************************************
 * Name      : NET_API_STATUS NetServerEnum
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE *bufptr
 *             DWORD prefmaxlen
 *             LPDWORD entriesread
 *             LPDWORD totalentries
 *             DWORD servertype
 *             LPWSTR domain
 *             LPDWORD resume_handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Patrick Haller [2001-09-06]
 *****************************************************************************/
ODINFUNCTION9(NET_API_STATUS, OS2NetServerEnum,
              LPWSTR,         lpServerName,
              DWORD,          dwLevel,
              LPBYTE *,       bufptr,
              DWORD,          prefmaxlen,
              LPDWORD,        lpdwEntriesRead,
              LPDWORD,        lpdwTotalEntries,
              DWORD,          dwServerType,
              LPWSTR,         lpDomain,
              LPDWORD,        resume_handle)

{
  // convert information modes!
  ULONG ulOS2Level = dwLevel;     // can be 100 and 101
  switch (dwLevel)
  {
    case 100: ulOS2Level = 1; break;
    case 101: ulOS2Level = 1; break;
    default:  ulOS2Level = 1; break;
  }
  
  // 2001-09-05 PH doc says the SV_TYPE_ definitions from
  // LanMan and NT are identical.
  int iOS2ServerType = dwServerType;

  // Convert servername to ASCII
  // Convert domain to ASCII
  char *asciiServername = NULL;
  char *asciiDomain     = NULL;
  
  if (lpServerName) 
    asciiServername = UnicodeToAsciiString(lpServerName);
  
  if (lpDomain)
    asciiDomain = UnicodeToAsciiString(lpDomain);
  
  dprintf(("WNETAP32: NetServerEnum server=[%s], domain=[%s]\n",
           asciiServername,
           asciiDomain));

  ULONG  ulBytesAvailable;
  DWORD  rc;
  
  // determine required size of buffer
  char pOS2Buffer[8192];
  ULONG ulBufferLength = sizeof(pOS2Buffer);
  ULONG ulEntriesRead;
  ULONG ulEntriesTotal;
  rc = OSLibNetServerEnum((const unsigned char*)asciiServername,
                          ulOS2Level,
                          (unsigned char*)pOS2Buffer,
                          ulBufferLength,
                          &ulEntriesRead,
                          &ulEntriesTotal,
                          dwServerType,
                          (unsigned char*)asciiDomain);
  
  if (asciiServername) FreeAsciiString(asciiServername);
  if (asciiDomain)     FreeAsciiString(asciiDomain);
  
  // convert the structures
  switch (dwLevel)
  {
    case 100:
    {
        PSERVER_INFO_100 psiw0;
        struct server_info_1 *pOS2ssi1 = (struct server_info_1 *)pOS2Buffer;
        ULONG ulConvertedEntries = 0;
        ULONG ulSpace = 0;
        ULONG ulAllocated = 0;
        ULONG ulEntriesConvertable = 0;
        BOOLEAN flagMoreData = FALSE;
        
        // allocate as much memory as required or
        // the amount specified in "prefmaxlen"
        
        // we need to calculate the overall size of all returned entries
        for (ULONG i = 0;
             i < *lpdwEntriesRead;
             i++)
        {
            ulSpace += sizeof(SERVER_INFO_100);
            ulSpace += (strlen( (const char*) pOS2ssi1->sv1_name) + 1) * 2;

            // count how many entries we can convert
            if (ulSpace < prefmaxlen)
                ulEntriesConvertable++;
            else
                flagMoreData = TRUE;
        }
        // ulSpace now contains the number of bytes required for the overall structure
        // ulEntriesConvertable contains the number of entries that fit in the target buffer

        ulAllocated = min(ulSpace, prefmaxlen);
        
        // reset the structure pointer
        pOS2ssi1 = (struct server_info_1 *)pOS2Buffer;

        rc = OS2NetApiBufferAllocate(ulAllocated, (LPVOID*)&psiw0);
        if (!rc)
        {
            // zero out the memory
            memset(psiw0, 0, ulAllocated);

			// set data pointer "behind" the converted entries
            // that's where the strings are placed
            PBYTE pbData = (PBYTE)psiw0 + ulEntriesConvertable * sizeof(PSERVER_INFO_100);

            for (;
                 ulConvertedEntries < ulEntriesConvertable;
                 ulConvertedEntries++)
	        {
                 // convert that single structure
                 psiw0->sv100_platform_id = 0; // @@@PH unavailable?
                 lstrcpyAtoW((LPWSTR)pbData, (LPCSTR)pOS2ssi1->sv1_name );
                 psiw0->sv100_name = (LPWSTR)pbData;

                 // advance to next free data slot
                 pbData += (strlen( (const char*)pOS2ssi1->sv1_name ) + 1) * 2;
          
                 // skip to the next entry
                 pOS2ssi1++;
                 psiw0++;
	        }
        }
        
        // write back actual number of converted entries
        // (as place fits in the target buffer)
        *lpdwEntriesRead = ulConvertedEntries;

        // check if there would have been more data
        if (flagMoreData)
            rc = ERROR_MORE_DATA_W;

        break;
    }


    case 101:
    {
        PSERVER_INFO_101 psiw1;
        struct server_info_1 *pOS2ssi1 = (struct server_info_1 *)pOS2Buffer;
        ULONG ulConvertedEntries = 0;
        ULONG ulSpace = 0;
        ULONG ulAllocated = 0;
        ULONG ulEntriesConvertable = 0;
        BOOLEAN flagMoreData = FALSE;
        
        // allocate as much memory as required or
        // the amount specified in "prefmaxlen"
        
        // we need to calculate the overall size of all returned entries
        for (ULONG i = 0;
             i < *lpdwEntriesRead;
             i++)
        {
            ulSpace += sizeof(SERVER_INFO_101);
            ulSpace += (strlen( (const char*)pOS2ssi1->sv1_name ) + 1) * 2;
            ulSpace += (strlen( (const char*)pOS2ssi1->sv1_comment ) + 1) * 2;

            // count how many entries we can convert
            if (ulSpace < prefmaxlen)
                ulEntriesConvertable++;
            else
                flagMoreData = TRUE;
        }
        // ulSpace now contains the number of bytes required for the overall structure
        // ulEntriesConvertable contains the number of entries that fit in the target buffer

        ulAllocated = min(ulSpace, prefmaxlen);
        
        // reset the structure pointer
        pOS2ssi1 = (struct server_info_1 *)pOS2Buffer;

        rc = OS2NetApiBufferAllocate(ulAllocated, (LPVOID*)&psiw1);
        if (!rc)
        {
            // zero out the memory
            memset(psiw1, 0, ulAllocated);

			// set data pointer "behind" the converted entries
            // that's where the strings are placed
            PBYTE pbData = (PBYTE)psiw1 + ulEntriesConvertable * sizeof(PSERVER_INFO_101);

            for (;
                 ulConvertedEntries < ulEntriesConvertable;
                 ulConvertedEntries++)
	        {
                 // convert that single structure
                 psiw1->sv101_platform_id = 0; // @@@PH unavailable?
                 lstrcpyAtoW((LPWSTR)pbData, (LPCSTR)pOS2ssi1->sv1_name);
                 psiw1->sv101_name = (LPWSTR)pbData;
                 pbData += (strlen( (const char*)pOS2ssi1->sv1_name ) + 1) * 2;

                 lstrcpyAtoW((LPWSTR)pbData, (LPCSTR)pOS2ssi1->sv1_comment);
                 psiw1->sv101_comment = (LPWSTR)pbData;
                 pbData += (strlen( (const char*)pOS2ssi1->sv1_comment ) + 1) * 2;

                 psiw1->sv101_version_major = pOS2ssi1->sv1_version_major;
                 psiw1->sv101_version_minor = pOS2ssi1->sv1_version_minor;
                 psiw1->sv101_type = pOS2ssi1->sv1_type;

                 // skip to the next entry
                 pOS2ssi1++;
                 psiw1++;
	        }
        }
        
        // write back actual number of converted entries
        // (as place fits in the target buffer)
        *lpdwEntriesRead = ulConvertedEntries;

        // check if there would have been more data
        if (flagMoreData)
            rc = ERROR_MORE_DATA_W;

        break;
    }


  }
  
  return (rc);
  
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetServerGetInfo
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
 * Author    : Markus Montkowski [09.07.98 21:57:43]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetServerGetInfo,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr)

{

  dprintf(("NETAPI32: NetServerGetInfo(%s, %d, %08x) not implemented\n"
           ,servername, level, *bufptr
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetServerSetInfo
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE buf
 *             LPDWORD ParmError
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:58:14]
 *****************************************************************************/
ODINFUNCTION4(NET_API_STATUS, OS2NetServerSetInfo,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, buf,
              LPDWORD, ParmError)

{

  dprintf(("NETAPI32: NetServerSetInfo(%s, %d, %08x, %08x) not implemented\n"
           ,servername, level, buf, ParmError
         ));

  return (NERR_BASE);
}


/*****************************************************************************
 * Name      : NET_API_STATUS NetServerTransportAdd
 * Purpose   :
 * Parameters: LPWSTR servername
 *             DWORD level
 *             LPBYTE bufptr
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:58:34]
 *****************************************************************************/
ODINFUNCTION3(NET_API_STATUS, OS2NetServerTransportAdd,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE, bufptr)

{

  dprintf(("NETAPI32: NetServerTransportAdd(%s, %d, %08x) not implemented\n"
           ,servername, level, bufptr
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetServerTransportDel
 * Purpose   :
 * Parameters: LPWSTR servername
 *             LPWSTR transportname
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 21:59:12]
 *****************************************************************************/
ODINFUNCTION2(NET_API_STATUS, OS2NetServerTransportDel,
              LPWSTR, servername,
              LPWSTR, transportname)

{

  dprintf(("NETAPI32: NetServerTransportDel(%08x, %08x) not implemented\n"
           ,servername, transportname
         ));

  return (NERR_BASE);
}

/*****************************************************************************
 * Name      : NET_API_STATUS NetServerTransportEnum
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
 * Author    : Markus Montkowski [09.07.98 21:59:37]
 *****************************************************************************/
ODINFUNCTION7(NET_API_STATUS, OS2NetServerTransportEnum,
              LPWSTR, servername,
              DWORD, level,
              LPBYTE *, bufptr,
              DWORD, prefmaxlen,
              LPDWORD, entriesread,
              LPDWORD, totalentries,
              LPDWORD, resumehandle)

{

  dprintf(("NETAPI32: NetServerTransportEnum(%s, %d, %08x, %d, %08x, %08x, %08x) not implemented\n"
           ,servername, level, *bufptr, prefmaxlen, entriesread, totalentries, resumehandle
         ));

  return (NERR_BASE);
}
