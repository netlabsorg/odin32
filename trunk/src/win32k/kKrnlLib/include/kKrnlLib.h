/* $Id: kKrnlLib.h,v 1.7 2002-04-01 13:51:16 bird Exp $
 *
 * Top level header file for kKrnlLib exports.
 *
 * NOTE: This there is no need to include any kLib stuff.
 *       Everything goes thru this header file.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
 *
 */
#ifndef _kKrnlLib_h_
#define _kKrnlLib_h_


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#ifdef INCL_KKL_ALL
    #define INCL_KKL_HEAP
    #define INCL_KKL_SPRINTF
    #define INCL_KKL_PRINTF
    #define INCL_KKL_AVL
    #define INCL_KKL_FUNC
    #define INCL_KKL_MISC
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
/*
 * Basis types and macros.
 */
#include <kLib/kTypes.h>


/*
 * Basic kernel info.
 */
#ifndef NO_KKL_KERNEL
    #include <kKLkernel.h>
#endif


/*
 * Standard arg stuff.
 */
#ifndef NO_KKL_STDARG
    #include "kKLstdarg.h"
#endif


/*
 * Memory allocation.
 */
#ifdef INCL_KKL_HEAP
    #include "kKLmalloc.h"
    #include "kKLrmalloc.h"
    #include "kKLsmalloc.h"
    #ifdef __cpluscplus
        #include "kKLnew.h"
    #endif
#endif


/*
 * String/IO stuff.
 */
#ifdef INCL_KKL_SPRINTF
    #include "kKLsprintf.h"
#endif
#ifdef INCL_KKL_PRINTF
    #include "kKLprintf.h"
#endif


/*
 * Avl tree.
 */
#ifdef INCL_KKL_AVL
    #include <kLib/kAVL.h>
#endif


/*
 * Logging.
 */
#ifndef NO_KKL_LOG
    #include <kLib/kLog.h>
#endif


/*
 * Function helpers.
 */
#ifdef INCL_KKL_FUNC
    #include "kKLFunc.h"
#endif


/*
 * Misc.
 */
#ifdef INCL_KKL_MISC
    #include <kLib/kMisc.h>
#endif



#endif

