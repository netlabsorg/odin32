/* $Id: kKLprintf.h,v 1.1 2001-09-14 01:50:18 bird Exp $
 *
 * printf/vprintf header file.
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

#ifndef va_arg
    #error "must include stdarg.h"
#endif


/*
 * Function prototypes
 */
int printf(const char *pszFormat, ...);
int vprintf(const char *pszFormat, va_list args);
int vprintf2(const char *pszFormat, va_list args);

/*
 * va_start have to be redeclared. Compiler don't accept SSToDS() as va_start argument.
 */
#undef va_start
#define va_start(ap, last) ap = ((va_list)SSToDS(&last)) + __nextword(last)

#ifdef __cplusplus
}
#endif
#endif
