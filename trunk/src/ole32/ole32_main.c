/* $Id: ole32_main.c,v 1.2 2001-09-05 13:17:11 bird Exp $ */
/*
 *  OLE32 Initialization
 *
 */
#include "windef.h"
#ifdef __WIN32OS2__
#include "winbase.h"
#endif
#include "winerror.h"
#include "ole32_main.h"
#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(ole);

HINSTANCE OLE32_hInstance = 0;

/***********************************************************************
 *      DllEntryPoint (OLE32.@)
 */

BOOL WINAPI OLE32_DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    TRACE("0x%x 0x%lx %p\n", hinstDLL, fdwReason, fImpLoad);

    switch(fdwReason) {
    case DLL_PROCESS_ATTACH:
        OLE32_hInstance = hinstDLL;
    break;

    case DLL_PROCESS_DETACH:
        OLE32_hInstance = 0;
    break;
    }
    return TRUE;
}

/***********************************************************************
 *      DllRegisterServer (OLE32.@)
 */
HRESULT WINAPI OLE32_DllRegisterServer() {
    /* FIXME: what Interfaces should we register ... */
    FIXME("(), stub!\n");
    return S_OK;
}
