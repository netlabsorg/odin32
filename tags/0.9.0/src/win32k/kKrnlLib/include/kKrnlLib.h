/* $Id: kKrnlLib.h,v 1.8 2002-12-16 02:25:07 bird Exp $
 *
 * Top level header file for kKrnlLib exports.
 *
 * NOTE: This there is no need to include any kLib stuff.
 *       Everything goes thru this header file.
 *
 * Copyright (c) 2002-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
    #include "kKLkernel.h"
#endif


/*
 * Standard arg stuff.
 */
#ifndef NO_KKL_STDARG
    #include <kLib/kStdArg.h>
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
    #include <kLib/kString.h>
    #define sprintf     kStrFormat
    #define vsprintf    kStrVFormat
#endif
// replaced by kLib/kLog.h
//#ifdef INCL_KKL_PRINTF
//    #include "kKLprintf.h"
//#endif


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

