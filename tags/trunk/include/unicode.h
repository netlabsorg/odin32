/* $Id: unicode.h,v 1.7 1999-11-28 23:23:45 bird Exp $ */

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

char *  WIN32API UnicodeToAsciiString(LPCWSTR ustring);
int     WIN32API UnicodeToAscii(LPCWSTR ustring, char *astring);
int     WIN32API UnicodeToAsciiN(LPCWSTR ustring, char *astring, int unilen);
void    WIN32API FreeAsciiString(char *astring);
LPWSTR  WIN32API AsciiToUnicodeString(const char *astring);
void    WIN32API AsciiToUnicode(const char *ascii, LPWSTR unicode);
void    WIN32API AsciiToUnicodeN(const char *ascii, LPWSTR unicode, int asciilen);

#ifdef __cplusplus
 }
#endif

#ifdef __cplusplus
char *  WIN32API UnicodeToAsciiStringN(LPCWSTR ustring, ULONG length);
#endif

#endif
