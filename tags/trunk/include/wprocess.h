/* $Id: wprocess.h,v 1.8 1999-09-26 22:24:51 sandervl Exp $ */
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
#endif
#include <thread.h>

TEB *InitializeTIB(BOOL fMainThread = FALSE);
void DestroyTIB();

void   WIN32API RestoreOS2TIB();
USHORT WIN32API SetWin32TIB();

extern BOOL fExeStarted;
extern BOOL fFreeLibrary;

//Flat pointer to thread TIB structure
extern DWORD  *TIBFlatPtr;

TEB  *WIN32API GetThreadTEB();
THDB *WIN32API GetThreadTHDB();

#endif 