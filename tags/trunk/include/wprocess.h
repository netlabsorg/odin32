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

#define MAKE_THREADID(processid, threadid) 	((processid << 16) | threadid)
#define ODIN_TO_OS2_THREADID(threadid)          (threadid & 0xFFFF)

TEB *  WIN32API InitializeTIB(BOOL fMainThread = FALSE);
void   WIN32API DestroyTIB();
ULONG  WIN32API GetProcessTIBSel();

#define TIB_SWITCH_DEFAULT	0  //executable type determines whether or not FS is changed
#define TIB_SWITCH_FORCE_WIN32  1  //always switch to win32 TIB (FS is changed)
//
//Switch to WIN32 TIB (FS selector)
//NOTE: This is not done for Odin32 applications (LX), unless
//      fForceSwitch is TRUE)
USHORT WIN32API SetWin32TIB(BOOL fForceSwitch = TIB_SWITCH_DEFAULT);
void   WIN32API RestoreOS2TIB();

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