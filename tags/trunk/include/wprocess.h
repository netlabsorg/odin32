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
#include <FastInfoBlocks.h>

#define SELECTOR_OS2_FS		0x150b

#define MAKE_THREADID(processid, threadid) 	((processid << 16) | threadid)
#define ODIN_TO_OS2_THREADID(threadid)          (threadid & 0xFFFF)

#define ODIN_GetCurrentThreadId()    MAKE_THREADID(fibGetPid(), fibGetTid())
#define ODIN_GetCurrentProcessId()   fibGetPid()

TEB   *WIN32API CreateTEB(HANDLE hThread, DWORD dwThreadId);
TEB   *WIN32API InitializeMainThread();

//******************************************************************************
// Set up the TEB structure of the CURRENT (!) thread
//******************************************************************************
BOOL   WIN32API InitializeThread(TEB *teb, BOOL fMainThread = FALSE);

void   WIN32API DestroyTEB(TEB *teb);
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
extern BOOL fIsOS2Image;   //TRUE  -> Odin32 OS/2 application (not converted!)
                           //FALSE -> otherwise
extern BOOL fSwitchTIBSel; // TRUE  -> switch TIB selectors
                           // FALSE -> don't
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
BOOL  InitLoaders();

UINT WIN32API GetProcModuleFileNameA(ULONG lpvAddress, LPSTR lpszFileName, UINT cchFileNameMax);

#endif