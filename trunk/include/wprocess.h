/* $Id: wprocess.h,v 1.10 1999-11-30 14:19:02 sandervl Exp $ */
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
#include <win\process.h>
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

inline PDB * WINE_UNUSED PROCESS_Current(void)
{
    return NtCurrentTeb()->process;
}

#endif 