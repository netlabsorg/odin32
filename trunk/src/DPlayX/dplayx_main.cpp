// $Id: dplayx_main.cpp,v 1.4 2001-03-13 23:13:27 hugh Exp $
/*
 * DPLAYX.DLL LibMain
 *
 * Copyright 1999,2000 - Peter Hunnisett
 *
 * contact <hunnise@nortelnetworks.com>
 */
#include <string.h>
#include <odin.h>
#define ICOM_CINTERFACE 1
#define CINTERFACE
#include "winerror.h"
#include "winbase.h"
#include "debugtools.h"
#define INITGUID
#include "initguid.h"  /* To define the GUIDs */
#include "dplaysp.h"
#include "dplayx_global.h"
#ifdef __WIN32OS2__
#include <initdll.h>
#endif

DEFAULT_DEBUG_CHANNEL(dplay);
DEFINE_GUID(GUID_NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

/* This is a globally exported variable at ordinal 6 of DPLAYX.DLL */
DWORD gdwDPlaySPRefCount = 0; /* FIXME: Should it be initialized here? */

BOOL WINAPI DPLAYX_LibMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{

  TRACE( "(%u,0x%08lx,%p) & 0x%08lx\n", hinstDLL, fdwReason, lpvReserved, gdwDPlaySPRefCount );

  switch ( fdwReason )
  {
    case DLL_PROCESS_ATTACH:
        /* First instance perform construction of global processor data */
        return DPLAYX_ConstructData();

    case DLL_PROCESS_DETACH:
    {
      BOOL rc;
      /* Last instance performs destruction of global processor data */
      rc = DPLAYX_DestructData();
#ifdef __WIN32OS2__
      if(gdwDPlaySPRefCount==0) // only do this the last time ?
        ctordtorTerm();
#endif
        return rc;

      break;
    }

    case DLL_THREAD_ATTACH: /* Do nothing */
    case DLL_THREAD_DETACH: /* Do nothing */
      break;
    default:
      break;

  }

  return TRUE;
}

/***********************************************************************
 *              DllCanUnloadNow (DPLAYX.10)
 */
HRESULT WINAPI DPLAYX_DllCanUnloadNow(void)
{
  HRESULT hr = ( gdwDPlaySPRefCount > 0 ) ? S_FALSE : S_OK;

  /* FIXME: Should I be putting a check in for class factory objects
   *        as well
   */

  TRACE( ": returning 0x%08lx\n", hr );

  return hr;
}


