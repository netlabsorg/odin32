/* $Id: initterm.h,v 1.6 2004-05-03 12:09:00 sandervl Exp $ */

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

#ifdef __cplusplus
extern "C" {
#endif

extern DWORD hInstanceUser32;
extern BOOL  fVersionWarp3;

//winicon.cpp
void RestoreCursor(void);
BOOL WIN32API InitClipboardFormats(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__INITTERM_H__
