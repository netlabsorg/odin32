/* $Id: urlmon.cpp,v 1.2 2000-08-02 15:50:41 bird Exp $ */
/*
 * UrlMon
 *
 * Copyright (c) 2000 Patrik Stridvall
 *
 */

#include <os2win.h>
#include <odinwrap.h>
#include <heapstring.h>
#include "wtypes.h"
// #include "urlmon.h"

/***********************************************************************
 *		URLMON_DllInstall (URLMON.@)
 */
HRESULT WINAPI URLMON_DllInstall(BOOL bInstall, LPCWSTR cmdline)
{
  dprintf(("URLMON_DllInstall: stub\n"));

  return S_OK;
}

/***********************************************************************
 *		DllCanUnloadNow (URLMON.7)
 */
HRESULT WINAPI URLMON_DllCanUnloadNow(void)
{
    dprintf(("(void): stub\n"));

    return S_FALSE;
}

/***********************************************************************
 *		DllGetClassObject (URLMON.8)
 */
HRESULT WINAPI URLMON_DllGetClassObject(REFCLSID rclsid, REFIID riid,
					LPVOID *ppv)
{
    dprintf(("DllGetClassObject: stub\n"));

    return CLASS_E_CLASSNOTAVAILABLE;
}

/***********************************************************************
 *		DllRegisterServer (URLMON.9)
 */
HRESULT WINAPI URLMON_DllRegisterServer(void)
{
    dprintf(("(void): stub\n"));

    return S_OK;
}

/***********************************************************************
 *		DllRegisterServerEx (URLMON.10)
 */
HRESULT WINAPI URLMON_DllRegisterServerEx(void)
{
    dprintf(("(void): stub\n"));

    return E_FAIL;
}

/***********************************************************************
 *		DllUnregisterServer (URLMON.11)
 */
HRESULT WINAPI URLMON_DllUnregisterServer(void)
{
    dprintf(("(void): stub\n"));

    return S_OK;
}

/***********************************************************************
 *           CopyStgMedium (URLMON.@)
 */
HRESULT WINAPI CopyStgMedium(const void *src, void *dst)
{
    dprintf(("URLMON::CopyStgMedium(%p %p). Not Implemented", src, dst));
    return S_OK;
}

