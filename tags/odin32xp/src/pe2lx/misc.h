/* $Id: misc.h,v 1.4 1999-07-05 12:36:12 sandervl Exp $ */

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
int UniStrlen(WCHAR *wstring);
void UpCase(char *mixedcase);

#endif
