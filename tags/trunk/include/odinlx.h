/* $Id: odinlx.h,v 1.2 1999-09-18 15:55:45 sandervl Exp $ */

/*
 *
 * Interface for running Win32 programs (compiled in OS/2)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _ODINLX_H
#define _ODINLX_H


typedef ULONG (* WIN32API WIN32DLLENTRY)(ULONG hInstance, ULONG reason, LPVOID reserved);
typedef int (* WIN32API WINMAIN)(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

//******************************************************************************
//Create LX Dll object and send process attach message
//System dlls set EntryPoint to 0
//******************************************************************************
BOOL WIN32API RegisterLxDll(HINSTANCE hInstance, WIN32DLLENTRY EntryPoint, PVOID unused);

//******************************************************************************
//Destroy LX Dll object
//******************************************************************************
BOOL WIN32API UnregisterLxDll(HINSTANCE hInstance);

//******************************************************************************
//Create LX Exe object and call entrypoint
//System dlls set EntryPoint to 0
//******************************************************************************
BOOL WIN32API RegisterLxExe(WINMAIN EntryPoint, PVOID unused);

#ifdef __cplusplus
extern "C" {
#endif

void  OS2SetExceptionHandler(void *exceptframe);
void  OS2UnsetExceptionHandler(void *exceptframe);

#ifdef __cplusplus
}
#endif

#endif
