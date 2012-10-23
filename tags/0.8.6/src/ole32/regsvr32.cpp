/* $Id: regsvr32.cpp,v 1.8 2002-05-28 07:35:07 sandervl Exp $ */
/* 
 * RegSvr32 for OS/2
 * 
 * 1/9/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#include <windows.h>
#include <odinlx.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <heapstring.h>

// ======================================================================
// Local Data
// ======================================================================
static	int	fRegister = 1;
static	int	fSilent = 0;
static  char	szFile[1024];
static	HANDLE	hdlObject;

// ======================================================================
// Prototypes.
// ======================================================================

static	HRESULT	parse_command(int argc, char * argv[]);
static	HRESULT	load_object(void);
static	HRESULT	register_object(void);
static	HRESULT	deregister_object(void);
static	HRESULT	unload_object(void);
static	HRESULT	sign_on(void);
static	HRESULT	sign_off(void);
static	HRESULT	report_msg(const char * fmt, ...);
static	HRESULT	report_err(HRESULT hr, const char * fmt, ...);
static	HRESULT	usage(const char * fmt, ...);
static int WIN32API EntryPoint(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		    LPSTR lpCmdLine, int nCmdShow);

// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
// main
// ----------------------------------------------------------------------
int main(int argc, char *argv[])
{
    HRESULT	hr;

    if((hr = parse_command(argc, argv)) != S_OK)
	return hr;

    RegisterLxExe(EntryPoint, NULL);

    return 0;
}

// ======================================================================
// Private functions.
// ======================================================================

// ----------------------------------------------------------------------
// EntryPoint
// ----------------------------------------------------------------------
static int WIN32API EntryPoint
   (HINSTANCE	hInstance,
    HINSTANCE	hPrevInstance,
    LPSTR	lpCmdLine,
    int		nCmdShow)
{
    HRESULT	hr;

    if((hr = sign_on()) != S_OK)
	return hr;

    if((hr = load_object()) != S_OK)
	return hr;

    if (fRegister)
	hr = register_object();
    else
	hr = deregister_object();

    if (hr != S_OK)
	return hr;

    if((hr = unload_object()) != S_OK)
	return hr;

    return sign_off();
}

// ----------------------------------------------------------------------
// sign_on
// ----------------------------------------------------------------------
static	HRESULT	sign_on(void)
{
    HRESULT	hr;

    hr = OleInitialize(NULL);
    if (hr != S_OK)
	return report_err(hr, "Error initialising OLE");

    return S_OK;
}

// ----------------------------------------------------------------------
// sign_off
// ----------------------------------------------------------------------
static	HRESULT	sign_off(void)
{
    OleUninitialize();
    return report_msg("%s initialised OK", szFile);
}

// ----------------------------------------------------------------------
// parse_command
// ----------------------------------------------------------------------
static	HRESULT	parse_command(int argc, char * argv[])
{
    int		ii;
    char *	pArg;

    // Initial conditions.
    fRegister = 1;
    fSilent = 0;
    szFile[0] = 0;

    // Loop through args...
    for (ii = 1; ii < argc; ii++)
    {
	pArg = argv[ii];

	if ((pArg[0] == '-') || (pArg[0] == '/'))
	{
	    switch(toupper(pArg[1]))
	    {
		case 'S':
		{
		    fSilent = 1;
		    break;
		}

		case 'U':
		{
		    fRegister = 0;
		    break;
		}

		default:
		{
		    return usage("Unknown switch '%c'", pArg[1] );
		}
	    }
	}
	else if (strlen(szFile) == 0) 
	{
	    strcpy(szFile, pArg);
	}
	else
	{
	    return usage("Too many files specified");
	}
    }
    if (strlen(szFile) == 0) 
	return usage("No file specified");

    return S_OK;
}

// ----------------------------------------------------------------------
// load_object
// ----------------------------------------------------------------------
static	HRESULT	load_object(void)
{
//SvL: Use LoadLibrary here, not CoLoadLibrary
//     Some dlls (divx codec) call CoFreeUnusedLibraries which unloads our dll 
//     if we use CoLoadLibrary
//     NT's regsvr32 doesn't use CoLoadLibrary either. 
#if 1
    hdlObject = LoadLibraryA(szFile);
#else
    LPOLESTR libnameW = (LPOLESTR)HEAP_strdupAtoW(GetProcessHeap(), 0, szFile);
    hdlObject = LoadLibraryW(libnameW);
    HeapFree(GetProcessHeap(), 0, libnameW);
#endif
    if (hdlObject == 0)
	return report_err(S_FALSE, "Failed to load '%s'", szFile);

    return S_OK;
}

// ----------------------------------------------------------------------
// register_object
// ----------------------------------------------------------------------
static	HRESULT	register_object(void)
{
    typedef HRESULT(*LPFNREGISTER)(void);

    LPFNREGISTER	pfnRegister;
    HRESULT		hr;

    pfnRegister = (LPFNREGISTER)GetProcAddress(hdlObject, "DllRegisterServer");

    if (pfnRegister == NULL)
	return report_err(S_FALSE, "Unable to locate registration entry point in '%s'", szFile);

    hr = pfnRegister();
    if (hr != S_OK)
	return report_err(hr, "Registration for '%s' returned error 0x%08lx", szFile, hr);

    return S_OK;
}

// ----------------------------------------------------------------------
// deregister_object
// ----------------------------------------------------------------------
static	HRESULT	deregister_object(void)
{
    typedef HRESULT(*LPFNUNREGISTER)(void);

    LPFNUNREGISTER	pfnUnregister;
    HRESULT		hr;

    pfnUnregister = (LPFNUNREGISTER)GetProcAddress(hdlObject, "DllUnregisterServer");

    if (pfnUnregister == NULL)
	return report_err(S_FALSE, "Unable to locate unregistration entry point in '%s'", szFile);

    hr = pfnUnregister();
    if (hr != S_OK)
	return report_err(hr, "Unregistration for '%s' returned error 0x%08lx", szFile, hr);

    return S_OK;
}

// ----------------------------------------------------------------------
// unload_object
// ----------------------------------------------------------------------
static	HRESULT	unload_object(void)
{
#if 1
    FreeLibrary(hdlObject);
#else
    CoFreeLibrary(hdlObject);
#endif
    return S_OK;
}

// ----------------------------------------------------------------------
// report_msg
// ----------------------------------------------------------------------
static	HRESULT	report_msg(const char * fmt, ...)
{
    if (!fSilent)
    {
	va_list	va_ptr;
	HWND	hwnd = GetDesktopWindow();
	char	text[1024];

	va_start(va_ptr, fmt);
	vsprintf(text, fmt, va_ptr);
	va_end(va_ptr);

	MessageBoxA(hwnd, text, "RegSvr32", MB_OK | MB_ICONINFORMATION);
    }
    return S_OK;
}

// ----------------------------------------------------------------------
// report_err
// ----------------------------------------------------------------------
static	HRESULT	report_err(HRESULT hr, const char * fmt, ...)
{
    va_list	va_ptr;
    HWND	hwnd = GetDesktopWindow();
    char	text[1024];

    va_start(va_ptr, fmt);
    vsprintf(text, fmt, va_ptr);
    va_end(va_ptr);

    MessageBoxA(hwnd, text, "RegSvr32", MB_OK | MB_ICONERROR);

    return hr;
}

// ----------------------------------------------------------------------
// usage
// ----------------------------------------------------------------------
static	HRESULT	usage(const char * fmt, ...)
{
    va_list	va_ptr;
    HWND	hwnd = GetDesktopWindow();
    char	text[256];
    char	msg[1024];

    va_start(va_ptr, fmt);
    vsprintf(text, fmt, va_ptr);
    va_end(va_ptr);
    sprintf(msg, "Error - %s\n\n"
    		 "Usage:\n"
    		 "regsvr32 [/s] [/u] filename\n"
    		 "where\n"
    		 "/s - silent completion\n"
    		 "/u - unregister", text);

    MessageBoxA(hwnd, msg, "RegSvr32", MB_OK | MB_ICONWARNING);

    return S_FALSE;
}
