/* $Id: kKrnlLib.h,v 1.1 2001-09-14 01:50:18 bird Exp $
 *
 * Top level header file for kKrnlLib exports.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
/*
 * Basis types and macros.
 */
#include "kTypes.h"


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
    #include "kKLvprintf.h"
#endif


/*
 * Logging.
 */
#ifdef INCL_KKL_LOG
    #include "kKLlog.h"
#endif


/*
 * Avl tree.
 */
#ifdef INCL_KKL_AVL
    #include "kAVL.h"
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
    #include "kKLasmutils.h"
#endif


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/



/*******************************************************************************
*   Exported Functions                                                         *
*******************************************************************************/



#endif

