/* $Id: capi2032.cpp,v 1.3 1999-06-10 14:27:04 phaller Exp $ */

/*
 * CAPI2032 implementation
 *
 * Copyright 1998 Felix Maschek
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory.h>

#include "misc.h"
#include "capi2032.h"
#include "unicode.h"

// definitions from OS/2 toolkit

typedef unsigned long APIRET;
typedef unsigned long HEV;

#define SEM_INDEFINATE_WAIT -1

extern "C"
{
    APIRET _System DosCloseEventSem( HEV );
    APIRET _System DosCreateEventSem( char*, HEV*, unsigned long, unsigned long );
    APIRET _System DosWaitEventSem( HEV, unsigned long );
}

// Implementation note:
//
// The Implementation for OS/2 and Win32 are nearly the same. Most of the
// functions are implemented exactly the same way. The only difference is
// the signal handling. The OS/2 implementation needs a event semaphore
// which will be posted if an event occures. The Win32 implementation
// only provides a function CAPI_WAIT_FOR_EVENT; the function returns,
// if an event occures.
// The internal structure ImplCapi gives us the room for the event semaphore
// handle. The event semaphore will be created the first time,
// CAPI_WAIT_FOR_EVENT is called since some applications don't need this
// functionality (actually they poll the CAPI)

typedef struct _ImplCapi
    {
    DWORD ApplID;
    HEV   hEvent;
    } ImplCapi;

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_REGISTER(
    DWORD MessageBufferSize,
    DWORD maxLogicalConnection,
    DWORD maxBDataBlocks,
    DWORD maxBDataLen,
    DWORD * pApplID )
{
    ImplCapi* pImplCapi;
    DWORD dwResult;

#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_REGISTER");
#endif

    pImplCapi = new ImplCapi;
    if( !pImplCapi )
    {
#ifdef DEBUG
        WriteLog(" failed (no memory)!\n");
#endif
        return 0x0108; // OS ressource error (error class 0x10..)
    }

    memset( pImplCapi, 0, sizeof( ImplCapi ) );

    dwResult = CAPI_REGISTER( MessageBufferSize, maxLogicalConnection,
                              maxBDataBlocks, maxBDataLen, &pImplCapi->ApplID );

    // application successfully registered? -> save internal structure as Application ID
    if( dwResult )
    {
        *pApplID = (DWORD) pImplCapi;

#ifdef DEBUG
        WriteLog(" successfull (ApplID: %d)\n", pImplCapi->ApplID );
#endif
    }
    else
    {
#ifdef DEBUG
        WriteLog(" failed (%X)!\n", dwResult );
#endif
        delete pImplCapi;
    }

    return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_RELEASE(
    DWORD ApplID )
{
    ImplCapi* pImplCapi = (ImplCapi*) ApplID;
    DWORD dwResult;

#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_RELEASE (ApplID: %d)\n", pImplCapi->ApplID);
#endif

    dwResult = CAPI_RELEASE( pImplCapi->ApplID );

    // cleanup
    if( pImplCapi->hEvent )
        DosCloseEventSem( pImplCapi->hEvent );
    delete pImplCapi;

    return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_PUT_MESSAGE(
    DWORD ApplID,
    PVOID pCAPIMessage )
{
    ImplCapi* pImplCapi = (ImplCapi*) ApplID;
    DWORD dwResult;

    dwResult = CAPI_PUT_MESSAGE( pImplCapi->ApplID, pCAPIMessage );

#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_PUT_MESSAGE (ApplID: %d) Result: %X\n", pImplCapi->ApplID, dwResult);
#endif

    return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_MESSAGE(
    DWORD ApplID,
    PVOID * ppCAPIMessage )
{
    ImplCapi* pImplCapi = (ImplCapi*) ApplID;
    DWORD dwResult;

    dwResult = CAPI_GET_MESSAGE( pImplCapi->ApplID, ppCAPIMessage );

#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_GET_MESSAGE (ApplID: %d) Result: %X\n", pImplCapi->ApplID, dwResult);
#endif

    return dwResult;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_WAIT_FOR_SIGNAL(
    DWORD ApplID )
{
    ImplCapi* pImplCapi = (ImplCapi*) ApplID;
    DWORD dwResult;
    APIRET rc;

#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_WAIT_FOR_SIGNAL (ApplID: %d)", pImplCapi->ApplID);
#endif

    // create semaphore if neccessary
    if( !pImplCapi->hEvent )			// FM 980706: fixed condition...
    {
        rc = DosCreateEventSem( NULL, &pImplCapi->hEvent, 0, 0 );
        if( !rc )
        {
#ifdef DEBUG
    WriteLog(" failed (DosCreateEventSem)!\n");
#endif
            return 0x1108; // OS ressource error (error class 0x11..)
        }

        dwResult = CAPI_SET_SIGNAL( pImplCapi->ApplID, pImplCapi->hEvent );
        if( !dwResult )
        {
#ifdef DEBUG
    WriteLog(" failed (CAPI_SET_SIGNAL: %X)!\n", dwResult);
#endif
            return 0x1108; // OS ressource error (error class 0x11..)
        }
    }

    // wait for event
    DosWaitEventSem( pImplCapi->hEvent, SEM_INDEFINATE_WAIT );

#ifdef DEBUG
    WriteLog("\n");
#endif

    return 0;
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_MANUFACTURER(
    char * SzBuffer )
{
#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_GET_MANUFACTURER\n");
#endif

    return CAPI_GET_MANUFACTURER( SzBuffer );
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_VERSION(
    DWORD * pCAPIMajor,
    DWORD * pCAPIMinor,
    DWORD * pManufacturerMajor,
    DWORD * pManufacturerMinor )
{
#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_GET_VERSION\n");
#endif

    return CAPI_GET_VERSION( pCAPIMajor, pCAPIMinor,
                             pManufacturerMajor, pManufacturerMinor );
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_SERIAL_NUMBER(
    char * SzBuffer )
{
#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_GET_SERIAL_NUMBER\n");
#endif

    return CAPI_GET_SERIAL_NUMBER( SzBuffer );
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_GET_PROFILE(
    PVOID SzBuffer,
    DWORD CtrlNr )
{
#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_GET_PROFILE\n");
#endif

    return CAPI_GET_PROFILE( SzBuffer, CtrlNr );
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2CAPI_INSTALLED()
{
#ifdef DEBUG
    WriteLog("CAPI2032: CAPI_INSTALLED\n");
#endif

    return CAPI_INSTALLED( );
}


