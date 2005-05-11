/* $Id: poppack.h,v 1.3 2005-05-11 21:28:20 sao2l02 Exp $ */

#ifdef __WINE_PSHPACK_H
#undef __WINE_PSHPACK_H

#if defined(__GNUC__) || defined(__SUNPRO_C) || (defined(__IBMC__) || defined(__IBMCPP__)) || defined(__WATCOMC__)

#if !defined(__WIDL__)

#pragma pack()

#endif /* !defined(__WIDL__) */

#elif defined(__SUNPRO_CC)
#warning "Assumes default alignment is 4"
#pragma pack(4)
#elif !defined(RC_INVOKED)
#error "Restoration of the previous alignment isn't supported by the compiler"
#endif /* defined(__GNUC__) || defined(__SUNPRO_C) ; !defined(RC_INVOKED) */

#else /* defined(__WINE_PSHPACK_H) */
#error "Popping alignment isn't possible since no alignment has been pushed"
#endif /* defined(__WINE_PSHPACK_H) */
