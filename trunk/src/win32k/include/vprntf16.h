/* $Id: vprntf16.h,v 1.2 2000-09-02 21:08:04 bird Exp $
 *
 * printf/vprintf header file - 16bit.
 *
 * WARNING: Do not include stdarg.h! AND it's only for use during init!!!
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _vprintf_h_
#define _vprintf_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Argument definitions.
 */
typedef char _far *va_list;
#define va_start(ap,v) ap = (va_list)&v + sizeof(v)
#define va_arg(ap, t) ((t _far *)(ap += sizeof(t)))[-1]
#define va_end(ap) ap = NULL


/*
 * Function prototypes
 */
int FAR printf16(const char *pszFormat, ...);
int FAR vprintf16(const char *pszFormat, va_list args);

/*#pragma alloc_text(CODE16, printf16, vprintf16)*/


#ifdef __cplusplus
}
#endif
#endif
