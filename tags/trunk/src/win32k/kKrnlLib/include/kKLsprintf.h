/* $Id: kKLsprintf.h,v 1.2 2002-04-01 10:04:26 bird Exp $
 *
 * sprintf/vsprintf header file.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _sprintf_h_
#define _sprintf_h_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef va_arg
#if 0
    #error "must include stdarg.h"
#else
    #include <stdarg.h>
#endif
#endif


/*
 * Function prototypes
 */
int sprintf(char *pszBuffer, const char *pszFormat, ...);
int vsprintf(char *pszBuffer, const char *pszFormat, va_list args);

/*
 * va_start have to be redeclared. Compiler don't accept SSToDS() as va_start argument.
 */
#undef va_start
#define va_start(ap, last) ap = ((va_list)SSToDS(&last)) + __nextword(last)

#ifdef __cplusplus
}
#endif
#endif
