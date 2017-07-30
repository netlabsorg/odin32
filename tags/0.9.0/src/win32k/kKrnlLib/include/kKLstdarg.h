/* $Id: kKLstdarg.h,v 1.2 2002-01-09 03:21:58 bird Exp $
 *
 * Stdarg stuff, with additions to get around some SS != DS trouble.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _kKLstdarg_h_
#define _kKLstdarg_h_

#include <stdarg.h>

/*
 * va_start have to be redeclared. Compiler don't accept SSToDS() as va_start argument.
 */
#undef va_start
#define va_start(ap, last) ap = ((va_list)SSToDS(&last)) + __nextword(last)

/*
 * In order to get rid of the stupid warnings..
 */
#undef va_arg
#define va_arg(ap, type)   ((type *)(void *)((ap += (int) __nextword(type)) - __nextword(type)))[0]


#endif
