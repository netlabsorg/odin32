/* $Id: odinlx.h,v 1.6 2001-01-11 18:03:48 sandervl Exp $ */

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

#include <win\peexe.h>
#include <versionos2.h>

typedef ULONG (* WIN32API WIN32DLLENTRY)(ULONG hInstance, ULONG reason, LPVOID reserved);
typedef int (* WIN32API WINMAIN)(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

//******************************************************************************
//Create LX Dll object and send process attach message
//System dlls set EntryPoint to 0
//Parameters:
//  HINSTANCE hInstance		- OS/2 module handle
//  WIN32DLLENTRY EntryPoint    - Win32 dll entrypoint address
//  PVOID pResData              - pointer to win32 resource data
//  DWORD MajorImageVersion     - major image/os version (for fake win32 header)
//  DWORD MinorImageVersion     - minor image/os version (for fake win32 header)
//  DWORD Subsystem             - subsystem type (for fake win32 header)
//                                (IMAGE_SUBSYSTEM_WINDOWS_CUI/IMAGE_SUBSYSTEM_WINDOWS_GUI/IMAGE_SUBSYSTEM_WINDOWS_NATIVE)
//
//Returns: Odin32 module handle
//******************************************************************************
DWORD WIN32API RegisterLxDll(HINSTANCE hInstance, WIN32DLLENTRY EntryPoint, 
                             PVOID pResData, 
                             DWORD MajorImageVersion = ODINNT_MAJOR_VERSION, 
                             DWORD MinorImageVersion = ODINNT_MINOR_VERSION,
                             DWORD Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI);

//******************************************************************************
//Destroy LX Dll object
//******************************************************************************
BOOL WIN32API UnregisterLxDll(HINSTANCE hInstance);

//******************************************************************************
//Create LX Exe object and call entrypoint
//System dlls set EntryPoint to 0
//******************************************************************************
BOOL WIN32API RegisterLxExe(WINMAIN EntryPoint, PVOID pResData);

#endif
