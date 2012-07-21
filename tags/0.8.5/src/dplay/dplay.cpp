/* $Id: dplay.cpp,v 1.2 1999-10-04 09:55:56 sandervl Exp $ */

/* Direct Play 3 and Direct Play Lobby 2 Implementation
 *
 * Copyright (C) 1999 Patrick Haller   <phaller@gmx.net>
 *
 */

#include <odin.h>
#include <os2win.h>

#define CINTERFACE

#include "winerror.h"
#include "winnt.h"
#include "winreg.h"
#include "dplay.h"
#include "dplobby.h"
#include "heap.h"
#include "debugtools.h"

#include <odinwrap.h>
#include <heapstring.h>
#include <misc.h>

ODINDEBUGCHANNEL(DPLAY)


/***************************************************************************
 *  DirectPlayEnumerateA (DPLAYX.2)
 *
 *  The pointer to the structure lpContext will be filled with the
 *  appropriate data for each service offered by the OS. These services are
 *  not necessarily available on this particular machine but are defined
 *  as simple service providers under the "Service Providers" registry key.
 *  This structure is then passed to lpEnumCallback for each of the different
 *  services.
 *
 *  This API is useful only for applications written using DirectX3 or
 *  worse. It is superceeded by IDirectPlay3::EnumConnections which also
 *  gives information on the actual connections.
 *
 * defn of a service provider:
 * A dynamic-link library used by DirectPlay to communicate over a network.
 * The service provider contains all the network-specific code required
 * to send and receive messages. Online services and network operators can
 * supply service providers to use specialized hardware, protocols, communications
 * media, and network resources.
 *
 * TODO: Allocate string buffer space from the heap (length from reg)
 *       Pass real device driver numbers...
 *       Get the GUID properly...
 */
HRESULT WINAPI DirectPlayEnumerateA( LPDPENUMDPCALLBACKA lpEnumCallback,
                                     LPVOID lpContext )
{

  HKEY hkResult;
  LPCSTR searchSubKey    = "SOFTWARE\\Microsoft\\DirectPlay\\Service Providers";
  LPCSTR guidDataSubKey  = "Guid";
  LPCSTR majVerDataSubKey= "dwReserved1";
  DWORD dwIndex, sizeOfSubKeyName=50;
  char subKeyName[51];

  TRACE(": lpEnumCallback=%p lpContext=%p\n", lpEnumCallback, lpContext );

  if( !lpEnumCallback )
  {
     return DPERR_INVALIDPARAMS;
  }

  /* Need to loop over the service providers in the registry */
  if( RegOpenKeyExA( HKEY_LOCAL_MACHINE, searchSubKey,
                       0, KEY_ENUMERATE_SUB_KEYS, &hkResult ) != ERROR_SUCCESS )
  {
    /* Hmmm. Does this mean that there are no service providers? */
    ERR(": no service providers?\n");
    return DP_OK;
  }

  /* Traverse all the service providers we have available */
  for( dwIndex=0;
       RegEnumKeyA( hkResult, dwIndex, subKeyName, sizeOfSubKeyName ) !=
         ERROR_NO_MORE_ITEMS;
       ++dwIndex )
  {
    HKEY     hkServiceProvider;
    GUID     serviceProviderGUID;
    DWORD    returnTypeGUID, returnTypeReserved1, sizeOfReturnBuffer=50;
    char     returnBuffer[51];
    DWORD    majVersionNum /*, minVersionNum */;
    LPWSTR   lpWGUIDString;

    TRACE(" this time through: %s\n", subKeyName );

    /* Get a handle for this particular service provider */
    if( RegOpenKeyExA( hkResult, subKeyName, 0, KEY_QUERY_VALUE,
                         &hkServiceProvider ) != ERROR_SUCCESS )
    {
      ERR(": what the heck is going on?\n" );
      continue;
    }

    /* Get the GUID, Device major number and device minor number
     * from the registry.
     */
    if( RegQueryValueExA( hkServiceProvider, guidDataSubKey,
                            NULL, &returnTypeGUID, (LPBYTE)returnBuffer,
                            &sizeOfReturnBuffer ) != ERROR_SUCCESS )
    {
      ERR(": missing GUID registry data members\n" );
      continue;
    }

    /* FIXME: Check return types to ensure we're interpreting data right */
    lpWGUIDString = HEAP_strdupAtoW( GetProcessHeap(), 0, returnBuffer );
    CLSIDFromString( (LPCOLESTR)lpWGUIDString, &serviceProviderGUID );
    HeapFree( GetProcessHeap(), 0, lpWGUIDString );

    sizeOfReturnBuffer = 50;

    if( RegQueryValueExA( hkServiceProvider, majVerDataSubKey,
                            NULL, &returnTypeReserved1, (LPBYTE)returnBuffer,
                            &sizeOfReturnBuffer ) != ERROR_SUCCESS )
    {
      ERR(": missing dwReserved1 registry data members\n") ;
      continue;
    }
    /* FIXME: This couldn't possibly be right...*/
    majVersionNum = GET_DWORD( returnBuffer );

    /* The enumeration will return FALSE if we are not to continue */
    if( !lpEnumCallback( &serviceProviderGUID , subKeyName,
                         majVersionNum, (DWORD)0, lpContext ) )
    {
      WARN("lpEnumCallback returning FALSE\n" );
      break;
    }
  }

  return DP_OK;

}

/***************************************************************************
 *  DirectPlayEnumerateW (DPLAYX.3)
 *
 */
HRESULT WINAPI DirectPlayEnumerateW( LPDPENUMDPCALLBACKW lpEnumCallback, LPVOID lpContext )
{

  FIXME(":stub\n");

  return DPERR_OUTOFMEMORY;

}

/***************************************************************************
 *  DirectPlayCreate (DPLAYX.1) (DPLAY.1)
 *
 */
HRESULT WINAPI DirectPlayCreate
( LPGUID lpGUID, LPDIRECTPLAY2 *lplpDP, IUnknown *pUnk)
{
  //@@@PH stub
  FIXME("lpGUID=%p lplpDP=%p pUnk=%p\n", lpGUID,lplpDP,pUnk);

  if( pUnk != NULL )
  {
    /* Hmmm...wonder what this means! */
    ERR("What does a NULL here mean?\n" );
    return DPERR_OUTOFMEMORY;
  }


  /* Unknown interface type */
  return DPERR_NOINTERFACE;

}

