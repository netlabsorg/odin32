// $Id: callwrap.h,v 1.2 2004-05-11 09:08:20 sandervl Exp $

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Exception handling + misc functions for OS/2
 *
 * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
 *
 */
#ifndef __CALLWRAP_H__
#define __CALLWRAP_H__

#ifdef __cplusplus
extern "C" {
#endif

// Callback wrapper that supports handlers with the wrong (non-stdcall) 
// calling convention
DWORD CDECL WrapCallback2(WNDPROC lpfnFunction, DWORD param1, DWORD param2);

DWORD WIN32API WrapCallback4(WNDPROC lpfnFunction, DWORD param1, DWORD param2, DWORD param3, DWORD param4);

#ifdef __cplusplus
}
#endif

#endif
