/* $Id: kKLstdarg.h,v 1.1 2001-10-19 00:05:12 bird Exp $
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

#endif
