/* $Id: unicode.h,v 1.1 1999-05-24 20:19:07 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __UNICODE_H__
#define __UNICODE_H__

#include <uconv.h>

#ifdef __cplusplus
 extern "C" {
#endif

char *WIN32API UnicodeToAsciiString(LPWSTR ustring);
int   WIN32API UnicodeToAscii(LPWSTR ustring, char *astring);
void  WIN32API FreeAsciiString(char *astring);
void  WIN32API AsciiToUnicode(char *ascii, LPWSTR unicode);

#ifdef __cplusplus
 }
#endif

#ifdef __cplusplus
char *UnicodeToAscii(int length, WCHAR *NameString);
#endif

#endif
