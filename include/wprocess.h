/* $Id: wprocess.h,v 1.11 1999-11-30 14:32:50 sandervl Exp $ */
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
typedef ULONG PDB;
#endif
#include <thread.h>

TEB *InitializeTIB(BOOL fMainThread = FALSE);
void DestroyTIB();

void   WIN32API RestoreOS2TIB();
USHORT WIN32API SetWin32TIB();

extern BOOL fExeStarted;
extern BOOL fFreeLibrary;
extern BOOL fIsOS2Image; //TRUE  -> Odin32 OS/2 application (not converted!)
                         //FALSE -> otherwise

//Flat pointer to thread TIB structure
extern DWORD  *TIBFlatPtr;

extern PDB ProcessPDB;

#define NtCurrentTeb GetThreadTEB
TEB  *WIN32API GetThreadTEB();
THDB *WIN32API GetThreadTHDB();

#ifndef OS2_INCLUDED
inline PDB * PROCESS_Current(void)
{
    return NtCurrentTeb()->process;
}
#endif

#endif 