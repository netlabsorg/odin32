// $Id: callwrap.h,v 1.1 2004-01-11 12:03:13 sandervl Exp $

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

#ifdef __cplusplus
}
#endif

#endif
