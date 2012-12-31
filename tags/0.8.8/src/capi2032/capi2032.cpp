/* $Id: capi2032.cpp,v 1.6 2001-08-16 18:13:24 sandervl Exp $ */

/*
 * CAPI2032 implementation
 *
 * first implementation 1998 Felix Maschek
 *
 * rewritten 2000 Carsten Tenbrink
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_DOS
#include <os2wrap.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory.h>

#include "misc.h"
#include "capi2032.h"
#include "unicode.h"

// Implementation note:
//
// The Implementation for OS/2 and Win32 are nearly the same. Most of the
// functions are implemented exactly the same way. The only difference is
// the signal handling. The OS/2 implementation needs a event semaphore
// which will be posted if an event occures. The Win32 implementation
// only provides a function CAPI_WAIT_FOR_EVENT; the function returns,
// if an event occures.

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_REGISTER(
    DWORD MessageBufferSize,
    DWORD maxLogicalConnection,
    DWORD maxBDataBlocks,
    DWORD maxBDataLen,
    DWORD * pApplID )
{
   DWORD dwResult;
   APIRET rc;
   USHORT sel = RestoreOS2FS();
   HEV    hEvent = NULLHANDLE;
   char   szSemName[CCHMAXPATHCOMP];

   dprintf(("CAPI2032: CAPI_REGISTER"));

   dwResult = CAPI_REGISTER( MessageBufferSize, maxLogicalConnection,
                             maxBDataBlocks, maxBDataLen, pApplID );

   if( dwResult )
   {
      dprintf((" failed (%X)!\n", dwResult ));
      SetFS(sel);
      return(0x1108);
   }
   dprintf((" successfull (ApplID: %d)\n", *pApplID ));

   // create named semaphore with Application ID as last character sequenz
   rc = DosCreateEventSem( szSemName, &hEvent, DC_SEM_SHARED, FALSE );
   if( rc )
   {
      dprintf((" failed (DosCreateEventSem): %d!\n",rc));
      SetFS(sel);
      return 0x1108; // OS ressource error (error class 0x11..)
   }
   dprintf((" ok (DosCreateEventSem): hEvent:%d %s!\n", hEvent, szSemName));

   dwResult = CAPI_SET_SIGNAL( *pApplID, hEvent );
   if( dwResult )
   {
      dprintf((" failed (CAPI_SET_SIGNAL: %X)!\n", dwResult));
      SetFS(sel);
      return 0x1108; // OS ressource error (error class 0x11..)
   }

   SetFS(sel);
   return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_RELEASE(
    DWORD ApplID )
{
   DWORD dwResult;
   ULONG rc;
   HEV    hEvent = NULLHANDLE;
   char   szSemName[CCHMAXPATHCOMP];
   USHORT sel = RestoreOS2FS();

   dprintf(("CAPI2032: CAPI_RELEASE (ApplID: %d)\n", ApplID));

   // open semaphore ( get the event Handle )
   sprintf( szSemName, "\\SEM32\\CAPI2032\\SEM%d" , ApplID );

   // enter critical section, so no other thread can access the sem
   DosEnterCritSec();
   // open semaphore to get the event handle
   rc = DosOpenEventSem( szSemName, &hEvent );
   if(rc)
   {
      dprintf((" failed (DosOpenEventSem) rc: %d!\n",rc));
   }
   // close semaphore, this will decrement the open count
   DosCloseEventSem( hEvent );

   // cleanup
   if( hEvent )
   {
      dprintf(("(DosCloseEventSem) hEvent:%d %s!\n",hEvent, szSemName));
      // final cleanup semaphore.
      rc = DosCloseEventSem( hEvent );
      if(rc)
      {
         dprintf((" failed (DosCloseEventSem) rc: %d!\n",rc));
      }

   }
   DosExitCritSec();

   dwResult = CAPI_RELEASE( ApplID );

   SetFS(sel);
   return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_PUT_MESSAGE(
    DWORD ApplID,
    PVOID pCAPIMessage )
{
   DWORD dwResult;
   USHORT sel = RestoreOS2FS();

   dprintf(("CAPI2032: CAPI_PUT_MESSAGE (ApplID: %d)", ApplID));

   dwResult = CAPI_PUT_MESSAGE( ApplID, pCAPIMessage );

   dprintf((" Result: %X\n", dwResult));

   SetFS(sel);
   return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_MESSAGE(
    DWORD ApplID,
    PVOID * ppCAPIMessage )
{
   DWORD dwResult;
   USHORT sel = RestoreOS2FS();
   dprintf(("CAPI2032: CAPI_GET_MESSAGE (ApplID: %d)", ApplID));

   dwResult = CAPI_GET_MESSAGE( ApplID, ppCAPIMessage );

   dprintf((" Result: %X\n", dwResult));

   SetFS(sel);
   return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_WAIT_FOR_SIGNAL(
    DWORD ApplID )
{
   DWORD dwResult;
   ULONG ulPostCount;
   APIRET rc;
   HEV    hEvent = NULLHANDLE;
   char   szSemName[CCHMAXPATHCOMP];

   dprintf(("CAPI2032: CAPI_WAIT_FOR_SIGNAL (ApplID: %d)", ApplID));

   sprintf( szSemName, "\\SEM32\\CAPI2032\\SEM%d" , ApplID );

   // enter critical section, so no other thread can access the sem
   DosEnterCritSec();
   // open semaphore to get the event handle
   rc = DosOpenEventSem( szSemName, &hEvent );
   if(rc)
   {
      DosExitCritSec();
      dprintf((" failed (DosOpenEventSem) rc: %d!\n",rc));
      return 0x1101;
   }
   // close semaphore
   DosCloseEventSem( hEvent );
   DosExitCritSec();

   // wait for event
   rc = DosWaitEventSem( hEvent, SEM_INDEFINITE_WAIT );
   if(rc)
   {
      dprintf((" failed (DosWaitEventSem) rc: %d!\n",rc));
      return 0x1101;
   }
   dprintf((" ok got hEvent: %u!\n",hEvent));

   DosResetEventSem( hEvent, &ulPostCount );

   return 0;
}

//******************************************************************************
//******************************************************************************
VOID WIN32API OS2CAPI_GET_MANUFACTURER(
    char * SzBuffer )
{
   USHORT sel = RestoreOS2FS();
   dprintf(("CAPI2032: CAPI_GET_MANUFACTURER\n"));

   CAPI_GET_MANUFACTURER( SzBuffer );
   SetFS(sel);
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_VERSION(
    DWORD * pCAPIMajor,
    DWORD * pCAPIMinor,
    DWORD * pManufacturerMajor,
    DWORD * pManufacturerMinor )
{
   DWORD dwResult;
   USHORT sel = RestoreOS2FS();
   dprintf(("CAPI2032: CAPI_GET_VERSION\n"));

   dwResult = CAPI_GET_VERSION( pCAPIMajor, pCAPIMinor,
                            pManufacturerMajor, pManufacturerMinor );
   SetFS(sel);
   return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_SERIAL_NUMBER(
    char * SzBuffer )
{
   DWORD dwResult;
   USHORT sel = RestoreOS2FS();
   dprintf(("CAPI2032: CAPI_GET_SERIAL_NUMBER\n"));

   dwResult = CAPI_GET_SERIAL_NUMBER( SzBuffer );
   SetFS(sel);
   return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_PROFILE(
    PVOID SzBuffer,
    DWORD CtrlNr )
{
   DWORD dwResult;
   USHORT sel = RestoreOS2FS();
   dprintf(("CAPI2032: CAPI_GET_PROFILE\n"));

   dwResult = CAPI_GET_PROFILE( SzBuffer, CtrlNr );

   SetFS(sel);
   return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_INSTALLED( VOID )
{
   DWORD dwResult;
   USHORT sel = RestoreOS2FS();
   dprintf(("CAPI2032: CAPI_INSTALLED"));

   dwResult = CAPI_INSTALLED( );

   dprintf((" result: %d\n",dwResult));
   SetFS(sel);
   return dwResult;
}
