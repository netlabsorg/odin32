/* $Id: initterm.h,v 1.1 2002-06-02 10:09:29 sandervl Exp $ */

/*
 * Win32 icon conversion functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __INITTERM_H__
#define __INITTERM_H__

extern DWORD hInstanceUser32;
extern BOOL  fVersionWarp3;

//winicon.cpp
void RestoreCursor();

#endif //__INITTERM_H__
