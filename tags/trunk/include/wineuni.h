/*
 * WINE Unicode functions to ODIN mappers
 *
 * Copyright (C) 1999 Achim Hasenmueller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef H_WINEUNI
#define H_WINEUNI

#define lstrcpyAtoW(uni, ascii) AsciiToUnicode(ascii, uni)
#define lstrcpynAtoW(uni, ascii, len) AsciiToUnicodeN(ascii, uni, len)
#define lstrcpyWtoA(ascii, uni) UnicodeToAscii(uni, ascii)
#define lstrcpynWtoA(ascii, uni, len) UnicodeToAsciiN(uni, ascii, len)


#endif 