/* $Id: sprintf.h,v 1.2 1999-11-10 01:45:33 bird Exp $
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
    #error "must include stdarg.h"
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
