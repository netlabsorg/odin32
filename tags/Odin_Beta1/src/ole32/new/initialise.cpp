/* $Id: initialise.cpp,v 1.1 2000-04-27 22:18:20 davidr Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * COM/OLE Startup shutdown.
 * 
 * 1/7/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Some portions from Wine Implementation
 *   Copyright 1995  Martin von Loewis
 *   Copyright 1998  Justin Bradford
 *   Copyright 1999  Francis Beaudet
 *   Copyright 1999  Sylvain St-Germain
 */

#include "ole32.h"
#include <vmutex.h>

#include "oString.h"
#include "oTls.h"
#include "apartment.h"
#include "moniker.h"		// RunningObjectTableImpl_***
#include "filemoniker.h"	// FileMonikerImpl_***

extern void COM_RevokeAllClasses();
extern void COM_ExternalLockFreeList();

// Process wide variables
Apartment *		g_pAptMT;
Apartment *		g_pAptTN;
Apartment *		g_pAptMain;
Apartment *		g_pAptHost;

VMutex			g_AptMutex;

LONG			g_ComRef = 0;
VMutex			g_ComRefMutex;

// Thread level variables
oTlsValue<ThreadInfo *>	t_pThreadInfo;	// ThreadLevel Information

// ----------------------------------------------------------------------
// CoInitialize()
// ----------------------------------------------------------------------
HRESULT WIN32API CoInitialize(LPVOID lpReserved)
{
    dprintf(("OLE32: CoInitialize\n"));

    return CoInitializeEx(lpReserved, COINIT_APARTMENTTHREADED);
}

// ----------------------------------------------------------------------
// CreateHost
// ----------------------------------------------------------------------
static Apartment * CreateHost(void )
{
    // Thread Sync. protocol required here.
    // CreateThread(blah, &g_pAptHost)
    return new Apartment(APT_HOST);
}

// ----------------------------------------------------------------------
// GetDistinguishedApt()
// ----------------------------------------------------------------------
static Apartment * GetDistinguishedApt(enAptType aptType)
{
    switch (aptType)
    {
	case APT_MT:
	    g_AptMutex.enter();
	    if (g_pAptMT == NULL)
	    {
		g_pAptMT = new Apartment(APT_MT);
	    }
	    g_AptMutex.leave();

	    return g_pAptMT;

	case APT_TN:
	    g_AptMutex.enter();
	    if (g_pAptTN == NULL)
	    {
		g_pAptTN = new Apartment(APT_TN);
	    }
	    g_AptMutex.leave();

	    return g_pAptTN;

	case APT_HOST:
	    g_AptMutex.enter();
	    if (g_pAptHost == NULL)
	    {
		g_pAptHost = CreateHost();
		if (g_pAptMain == NULL)
		{
		    g_pAptMain = g_pAptHost;
		}
	    }
	    g_AptMutex.leave();

	    return g_pAptHost;

	case APT_MAIN:
	{
	    g_AptMutex.enter();
	    if (g_pAptMain == NULL)
	    {
		g_pAptMain = CreateHost();
	    }
	    g_AptMutex.leave();

	    return g_pAptMain;
	}
    }

    return NULL;
}

// ----------------------------------------------------------------------
// GetSingleThreadedApt()
// ----------------------------------------------------------------------
static Apartment * GetSingleThreadedApt()
{
    Apartment *	retObj;

    g_AptMutex.enter();

    // If Main not defined yet we become main...
    if (g_pAptMain == NULL)
    {
	g_pAptMain = new Apartment(APT_MAIN);
	retObj = g_pAptMain;
    }
    else
	retObj = new Apartment(APT_ST);

    g_AptMutex.leave();
    return retObj;
}

// ----------------------------------------------------------------------
// CoInitializeEx()
// ----------------------------------------------------------------------
HRESULT WIN32API CoInitializeEx(
	LPVOID 	lpReserved,	// [in] pointer to win32 malloc interface
	DWORD 	dwCoInit)	// [in] A value from COINIT specifies the thread
{
    HRESULT		hr;

    dprintf(("OLE32: CoInitializeEx(%p, %lx)\n", lpReserved, dwCoInit));

    // Does com require initialisation?
    g_ComRefMutex.enter();

    if (++g_ComRef == 1)
	RunningObjectTableImpl_Initialize();

    g_ComRefMutex.leave();

    // Does this thread require initialisation?
    if (t_pThreadInfo == NULL)	// 1'st time through
    {
	ThreadInfo *	l_pThreadInfo;

	l_pThreadInfo = new ThreadInfo;

	if (dwCoInit == COINIT_APARTMENTTHREADED)
	{
	    l_pThreadInfo->SetHomeApt(GetSingleThreadedApt());
	}
	else if (dwCoInit == COINIT_MULTITHREADED)
	{
	    l_pThreadInfo->SetHomeApt(GetDistinguishedApt(APT_MT));
	}
	else
	{
	    delete l_pThreadInfo;
	    return E_INVALIDARG;
	}

	// Initialise the current object context...
	l_pThreadInfo->SetCurContext(l_pThreadInfo->GetHomeApt()->GetDefaultContext());

	t_pThreadInfo.SetValue(l_pThreadInfo);	// Stash in TLS

	hr = S_OK;
    }
    else	// Subsequent call for this thread...
    {
	if (dwCoInit == COINIT_APARTMENTTHREADED)
	{
	    if (t_pThreadInfo->GetHomeApt()->GetAptType() == APT_MT)
		return RPC_E_CHANGED_MODE;
	}
	else if (dwCoInit == COINIT_MULTITHREADED)
	{
	    if (t_pThreadInfo->GetHomeApt()->GetAptType() != APT_MT)
		return RPC_E_CHANGED_MODE;
	}
	else
	    return E_INVALIDARG;

	t_pThreadInfo->AddRef();

	hr = S_FALSE;
    }

    return hr;
}

// ----------------------------------------------------------------------
// CoUninitialize()
// ----------------------------------------------------------------------
void WIN32API CoUninitialize(void)
{
    dprintf(("OLE32: CoUninitialize"));

    // Does thread require termination?
    if (t_pThreadInfo == NULL)
	return;

    if (t_pThreadInfo->Release() == 0)
    {
        ThreadInfo *    l_ThreadInfo = t_pThreadInfo;

	delete l_ThreadInfo;

	t_pThreadInfo.SetValue(NULL);
    }

    // Does com require termination?
    g_ComRefMutex.enter();
    if (--g_ComRef == 0)
    {
	dprintf(("OLE32: Releasing COM libraries"));

	RunningObjectTableImpl_UnInitialize();

	COM_RevokeAllClasses();

	CoFreeAllLibraries();
	
	COM_ExternalLockFreeList();
    }
    g_ComRefMutex.leave();
}
