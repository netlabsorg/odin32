/* $Id: unicode.h,v 1.3 1999-06-08 09:44:27 achimha Exp $ */

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
