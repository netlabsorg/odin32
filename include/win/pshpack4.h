/* $Id: pshpack4.h,v 1.2 2000-08-23 18:05:56 sandervl Exp $ */

#ifndef __WINE_PSHPACK_H
#define __WINE_PSHPACK_H 4

#if defined(__GNUC__) || defined(__SUNPRO_CC) || (defined(__IBMC__) || defined(__IBMCPP__)) || defined(__WATCOMC__)
#pragma pack(4)
#elif defined(__SUNPRO_C)
#warning "Assumes default alignment is 4"
#pragma pack()
#elif !defined(RC_INVOKED)
#error "4 as alignment isn't supported by the compiler"
#endif /* defined(__GNUC__) || defined(__SUNPRO_CC) ; !defined(RC_INVOKED) */

#else /* !defined(__WINE_PSHPACK_H) */
#error "Nested pushing of alignment isn't supported by the compiler"
#endif /* !defined(__WINE_PSHPACK_H) */

