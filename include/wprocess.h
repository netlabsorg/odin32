/* $Id: wprocess.h,v 1.6 1999-07-17 09:19:15 sandervl Exp $ */
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

#ifdef OS2_INCLUDED
typedef DWORD TEB;
typedef DWORD THDB;
#else
#include <winprocess.h>
#include <thread.h>
#endif

TEB *InitializeTIB(BOOL fMainThread = FALSE);
void DestroyTIB();

void   WIN32API RestoreOS2TIB();
USHORT WIN32API SetWin32TIB();

extern BOOL fExeStarted;

//Flat pointer to thread TIB structure
extern DWORD  *TIBFlatPtr;

TEB  *WIN32API GetThreadTEB();
THDB *WIN32API GetThreadTHDB();

#endif 