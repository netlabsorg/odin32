/* $Id: wprocess.h,v 1.18 2000-11-21 11:33:59 sandervl Exp $ */
/*
 * Process help functions
 *
 * Copyright (C) 1999 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _WPROCESS_H__
#define _WPROCESS_H__

#define WIN32_TIBSEL

#ifndef OS2_INCLUDED
#include <winprocess.h>
#else
#include <winconst.h>
typedef ULONG PDB;
#endif
#include <thread.h>

TEB *InitializeTIB(BOOL fMainThread = FALSE);
void DestroyTIB();

void   WIN32API RestoreOS2TIB();
USHORT WIN32API SetWin32TIB();
void   SetPDBInstance(HINSTANCE hInstance);

extern BOOL fExeStarted;
extern BOOL fFreeLibrary;
extern BOOL fIsOS2Image; //TRUE  -> Odin32 OS/2 application (not converted!)
                         //FALSE -> otherwise
extern BOOL fExitProcess;

//Flat pointer to thread TIB structure
extern DWORD  *TIBFlatPtr;

extern PDB ProcessPDB;

#define NtCurrentTeb GetThreadTEB
TEB  *WIN32API GetThreadTEB();
TEB  *WIN32API GetTEBFromThreadId(ULONG threadId);
TEB  *WIN32API GetTEBFromThreadHandle(HANDLE hThread);

#ifndef OS2_INCLUDED
inline PDB * PROCESS_Current(void)
{
    return NtCurrentTeb()->process;
}
#endif

ULONG InitCommandLine(const char *pszPeExe);

#endif