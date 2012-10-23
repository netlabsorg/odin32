/* $Id: umon.cpp,v 1.2 2000-08-02 15:50:41 bird Exp $ */
/*
 * UrlMon
 *
 * Copyright 1999 Corel Corporation
 *
 * Ulrich Czekalla
 *
 */

#include "windows.h"
#include "objbase.h"
#include "debugtools.h"

#include "urlmon.h"
/*
DEFAULT_DEBUG_CHANNEL(win32);

#include <os2win.h>
#include <odinwrap.h>
#include <heapstring.h>
#include <winnt.h>
#include "urlmon.h"
*/
// ODINDEBUGCHANNEL(imagehlp)


/***********************************************************************
 *           CreateURLMoniker (URLMON.6)
 *
 * Create a url moniker
 *
 * RETURNS
 *    S_OK 		success
 *    E_OUTOFMEMORY	out of memory
 *    MK_E_SYNTAX	not a valid url
 *
 */
extern "C"
HRESULT CreateURLMoniker(IMoniker *pmkContext, LPWSTR szURL, IMoniker **ppmk)
{
   if (NULL != pmkContext)
	dprintf(("Non-null pmkContext not implemented\n"));

   return CreateFileMoniker(szURL, ppmk);
}
