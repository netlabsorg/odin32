/* $Id: resstring.h,v 1.1 1999-07-14 08:35:33 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 resource string functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __STRING_H__
#define __STRING_H__

#ifdef __cplusplus
      extern "C" {
#endif

int OS2LoadStringAscii(HINSTANCE hinst, UINT wID, LPSTR lpBuffer, int cchBuffer);
int OS2LoadStringUnicode(HINSTANCE hinst, UINT wID, LPWSTR lpBuffer, int cchBuffer);

#ifdef __cplusplus
      }
#endif

#endif
