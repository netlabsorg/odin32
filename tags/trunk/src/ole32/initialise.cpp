/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 OLE stubs for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 */

#include "ole32.h"

// ----------------------------------------------------------------------
// Local Variables
// ----------------------------------------------------------------------
static int	s_COMLockCount = 0;

// ----------------------------------------------------------------------
// CoInitialize()
// ----------------------------------------------------------------------
HRESULT WIN32API CoInitialize(LPVOID lpReserved)
{
    dprintf(("OLE32.CoInitialize\n"));

    return CoInitializeEx(lpReserved, COINIT_APARTMENTTHREADED);
}

// ----------------------------------------------------------------------
// CoInitializeEx()
// ----------------------------------------------------------------------
HRESULT WIN32API CoInitializeEx(
	LPVOID 	lpReserved,	// [in] pointer to win32 malloc interface
	DWORD 	dwCoInit)	// [in] A value from COINIT specifies the thread
{
    HRESULT hr = S_FALSE;

    dprintf(("OLE32.CoInitializeEx(%p, %lx)\n", lpReserved, dwCoInit));

    if (lpReserved != NULL)
    {
	dprintf(("Warning: Bad parameter %p, must be an old Windows Application", lpReserved));
    }

    /*
     * Check for unsupported features.
     */
    if (dwCoInit != COINIT_APARTMENTTHREADED)
    {
	dprintf(("Warning: Unsupported flag %lx", dwCoInit));
	/* Hope for the best and continue anyway */
    }

    /*
     * Check the lock count.
     * If this is the first time going through the initialize
     * process, we have to initialize the libraries.
     */
    if (s_COMLockCount == 0)
    {
	/*
	 * Initialize the various COM libraries and data structures.
	 */
	dprintf(("OLE32.CoInitializeEx - Initializing COM libraries"));

//	RunningObjectTableImpl_Initialize();

	hr = S_OK;
    }

    /*
     * Crank-up that lock count.
     */
    s_COMLockCount++;

    return hr;
}

