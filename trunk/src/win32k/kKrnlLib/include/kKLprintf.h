/* $Id: kKLprintf.h,v 1.3 2001-10-19 00:04:45 bird Exp $
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
#include <stdarg.h>                     /* C library stdarg.h. */
#endif


/*
 * Function prototypes
 */
int printf(const char *pszFormat, ...);
int vprintf(const char *pszFormat, va_list args);
int vprintf2(const char *pszFormat, va_list args);

#include <kKLstdarg.h>

#ifdef __cplusplus
}
#endif
#endif
