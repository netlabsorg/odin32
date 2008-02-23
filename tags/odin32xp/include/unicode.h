/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __UNICODE_H__
#define __UNICODE_H__

#include <heapstring.h>


#define UnicodeToAsciiString(u)	 HEAP_strdupWtoA(GetProcessHeap(), 0, u)
#define AsciiToUnicodeString(a)  HEAP_strdupAtoW(GetProcessHeap(), 0, a)
#define FreeAsciiString(a)       HEAP_free(a)

#define UnicodeToAscii(u, a)     lstrcpyWtoA(a, u)
#define UnicodeToAsciiN(u, a, n) lstrcpynWtoA(a, u, n)
#define AsciiToUnicode(a, u)     lstrcpyAtoW(u, a)
#define AsciiToUnicodeN(a, u, n) lstrcpynAtoW(u, a, n)

#endif
