/* $Id: apartment.cpp,v 1.1 2000-04-27 22:18:14 davidr Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * COM Apartment handling
 * 
 * 22/04/2000
 * 
 * Copyright 2000 David J. Raison
 * 
 */

#include "ole32.h"
#include "oString.h"
#include "oTls.h"
#include "apartment.h"
#include "rpcdce.h"

CHAR ComMessagePumpWndClass[] = "OleMainThreadWndClass";
CHAR ComMessagePumpWndName[] = "OleMainThreadWndName";

// ======================================================================
// OleMainThreadWndProc
// ======================================================================
LRESULT CALLBACK ComMessagePumpWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
	case WM_USER:

	    // Dispatch method call...

	    break;

	default:
	    return DefWindowProcA(hWnd, msg, wParam, lParam);
    }

    return 0;
}

// ======================================================================
// Apartment
// ======================================================================

// ----------------------------------------------------------------------
// Apartment::Apartment
// ----------------------------------------------------------------------
Apartment::Apartment(enAptType aptType)
{
    WNDCLASSEXA		wcex;

    m_AptType = aptType;

    UuidCreate(&m_Guid);

    m_pDefaultContext = new ObjectContext;
    m_pDefaultContext->SetApt(this);

    if (IsMessageBased())
    {
	// First STA shall always be MAIN...
	if (m_AptType == APT_MAIN)
	{
	    // Register our window class...
	    ZeroMemory( &wcex, sizeof(WNDCLASSEXA));
	    wcex.cbSize         = sizeof(WNDCLASSEXA);
	    wcex.style          = CS_GLOBALCLASS;
	    wcex.lpfnWndProc    = (WNDPROC)ComMessagePumpWndProc;
	    wcex.hInstance      = 0;
	    wcex.lpszClassName  = ComMessagePumpWndClass;

	    RegisterClassExA(&wcex);
	}

	// Initialise message pump window...
	m_hwndPump = CreateWindowA(ComMessagePumpWndClass,
				   ComMessagePumpWndName,
				   WS_POPUP | WS_CLIPSIBLINGS | WS_OVERLAPPED,
				   CW_USEDEFAULT, CW_USEDEFAULT,
				   CW_USEDEFAULT, CW_USEDEFAULT,
				   0,
				   0,
				   0,
				   0);
    }
}

// ----------------------------------------------------------------------
// Apartment::~Apartment
// ----------------------------------------------------------------------
Apartment::~Apartment()
{
    if (IsMessageBased())
	DestroyWindow(m_hwndPump);

    delete m_pDefaultContext;
}

// ----------------------------------------------------------------------
// Apartment::IsCompatible
// ----------------------------------------------------------------------
BOOL		Apartment::IsCompatible(Attributes * const pTarget) const
{
    switch (pTarget->GetThreadingModel())
    {
	case  THM_BOTH:
	    return TRUE;

	case  THM_FREE:
	    return m_AptType == APT_MT;

	case  THM_NEUTRAL:
	    return m_AptType == APT_TN;

	case  THM_UNDEFINED:
	    return m_AptType == APT_MAIN;

	case  THM_APARTMENT:
	    return IsMessageBased();

	default:
	    // drop thru to warning msg.
	    break;
    }

    dprintf(("Apartment::IsCompatible() - Invalid Threading model (%d)",
							pTarget->GetThreadingModel()));

    return FALSE;
}
