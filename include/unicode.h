/* $Id: unicode.h,v 1.6 1999-08-19 10:24:52 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __UNICODE_H__
#define __UNICODE_H__

#include <uniconv.h>

#ifdef __cplusplus
 extern "C" {
#endif

/* also used from Windows only files ported from WINE */
#ifndef WIN32API
#define WIN32API WINAPI
#endif

char *  WIN32API UnicodeToAsciiString(LPWSTR ustring);
int     WIN32API UnicodeToAscii(LPWSTR ustring, char *astring);
int     WIN32API UnicodeToAsciiN(LPWSTR ustring, char *astring, int unilen);
void    WIN32API FreeAsciiString(char *astring);
LPWSTR  WIN32API AsciiToUnicodeString(char *astring);
void    WIN32API AsciiToUnicode(char *ascii, LPWSTR unicode);
void    WIN32API AsciiToUnicodeN(char *ascii, LPWSTR unicode, int asciilen);

#ifdef __cplusplus
 }
#endif

#ifdef __cplusplus
char *  WIN32API UnicodeToAsciiStringN(WCHAR *ustring, ULONG length);
#endif

#endif
