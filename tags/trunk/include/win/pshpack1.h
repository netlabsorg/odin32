/* $Id: pshpack1.h,v 1.1 1999-05-24 20:19:17 ktk Exp $ */

#ifndef __WINE_PSHPACK_H
#define __WINE_PSHPACK_H 1

#if defined(__GNUC__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC) || (defined(__IBMC__) || defined(__IBMCPP__))
#pragma pack(1)
#elif !defined(RC_INVOKED)
#error "1 as alignment isn't supported by the compiler"
#endif /* defined(__GNUC__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC) ; !defined(RC_INVOKED) */

#else /* !defined(__WINE_PSHPACK_H) */
#error "Nested pushing of alignment isn't supported by the compiler"
#endif /* !defined(__WINE_PSHPACK_H) */

