/* $Id: misc.h,v 1.1 1999-10-06 18:50:55 bird Exp $ */

/*
 * PE2LX ascii to unicode
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __MISC_H__
#define __MISC_H__

char *UnicodeToAscii(int length, WCHAR *NameString, int cp=0);
char *UnicodeToAscii(WCHAR *wstring, int cp=0);
int   UniStrlen(WCHAR *wstring);
void  UpCase(char *mixedcase);

#endif
