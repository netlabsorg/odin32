/* $Id: pe2lx.h,v 1.1 1999-10-08 00:39:52 bird Exp $
 *
 * Common header file for pe2lx.
 *
 * Copyright (c) 1999 knut  St.  osmundsen
 *
 */


#ifndef _pe2lx_h_
    #define _pe2lx_h_

    #if defined(RING0)
/*******************************************************************************
*   Ring - 0, Win32k                                                           *
*******************************************************************************/
        #define INCL_GPIBITMAPS
        #define INCL_BITMAPFILEFORMAT
        #define INCL_DOSFILEMGR          /* File Manager values      */
        #define INCL_DOSERRORS           /* DOS Error values         */
        #define INCL_DOSPROCESS          /* DOS Process values       */
        #define INCL_DOSMISC             /* DOS Miscellanous values  */
        #define INCL_WIN
        #ifndef RING0_DEBUG_IN_RING3
            #define INCL_NOAPI
        #else
            #define INCL_BASE
        #endif
        #include <os2.h>
        #include <pmmenu.h>

        #include <string.h>
        #include <builtin.h>
        #if 1
            #define __malloc_h /* to prevent using the VAC++ malloc.h */
            #include <stdlib.h>
        #endif
        #define itoa #error "this function don't work at RING-0"

        #include "malloc.h"
        #include "asmutils.h"
        #include "options.h"
        #include "log.h"
        #include "cout.h"                       /* logging - cout ios */
        #include "dev32.h" /* SSToDS(a) */
        #ifdef RING0_DEBUG_IN_RING3
            #undef SSToDS
            #define SSToDS(a) a
            #define D32Hlp_Yield()
        #endif

        #include <pefile.h>
        #include "lx.h"
        #include "misc.h"

    #else  /* #if defined(RING0) */

/*******************************************************************************
*   Ring - 3, PE2LX                                                            *
*******************************************************************************/
        #define INCL_GPIBITMAPS
        #define INCL_BITMAPFILEFORMAT
        #define INCL_DOSFILEMGR          /* File Manager values      */
        #define INCL_DOSERRORS           /* DOS Error values         */
        #define INCL_DOSPROCESS          /* DOS Process values       */
        #define INCL_DOSMISC             /* DOS Miscellanous values  */
        #define INCL_WIN
        #include <os2.h>
        #include <pmmenu.h>
        #include <stdio.h>
        #include <string.h>
        #include <stdlib.h>
        #include <iostream.h>
        #include <builtin.h>
        #include "pefile.h"
        #include "lx.h"
        #include "misc.h"
        #include "options.h"

        #define SSToDS(a) a
        #define D32Hlp_Yield()

        #undef  kprintf
        #define kprintf(a) printf a
        #define _kprintf printf

    #endif /* #if defined(RING0) */

/*******************************************************************************
*   Common Ring-0 and Ring-3 definitions                                       *
*******************************************************************************/
    /* assert macro */
    #define assert(expr)        ((expr) ? (void)0 : (void)_assert(#expr, __FILE__, __LINE__))
    #define _assert(a,b,c)      _kprintf("assert: %s in %s line %d\n",a,b,c), _interrupt(3)

    /* light assert */
    #define ltassert( expr )    if (!(expr)) { _ltassert( #expr, __FILE__, __LINE__); return FALSE; }
    #define vltassert( expr )   if (!(expr)) { _ltassert( #expr, __FILE__, __LINE__); return; }
    #define cltassert( expr )   if (!(expr)) { _ltassert( #expr, __FILE__, __LINE__); return -1; }
    #define ltassert2(expr, freeexpr) if (!(expr)){ _ltassert(#expr,__FILE__,__LINE__); freeexpr; return FALSE;}
    #define _ltassert(a,b,c)    _kprintf("ltassert: %s in %s line %d - returns FALSE\n",a,b,c)


#endif


