/* $Id: misc.h,v 1.1 1999-05-24 20:19:56 ktk Exp $ */

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

char *UnicodeToAscii(int length, WCHAR *NameString);
int UniStrlen(WCHAR *wstring);
char *UnicodeToAscii(WCHAR *wstring);
void UpCase(char *mixedcase);

#endif
