/* $Id: options.h,v 1.2 1999-10-27 02:02:57 bird Exp $
 *
 * Options.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _options_h_
#define _options_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* fKernel */
#define KF_UNI      0x00000000UL
#define KF_SMP      0x00000001UL

/* Set defaults. */
#define SET_OPTIONS_TO_DEFAULT(o)            \
            o.fQuiet         = 0;            \
            o.usCom          = OUTPUT_COM2;  \
            o.fLogging       = 0;            \
            o.ulBuild        = ~0;           \
            o.fKernel        = KF_UNI;       \
            o.usVerMajor     = ~0;           \
            o.usVerMinor     = ~0;


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/** Option struct */
struct options
{
    //*************/
    /** @cat misc */
    //*************/
    BOOL        fQuiet;              /* quiet initialization */
    USHORT      usCom;               /* Output port no. */

    //************************/
    /** @cat logging options */
    //************************/
    BOOL        fLogging;            /* logging */

    //*************************/
    /** @cat kernel selection */
    //*************************/
    ULONG       fKernel;            /* smp or uni kernel */
    ULONG       ulBuild;            /* kernel build */
    USHORT      usVerMajor;         /* OS/2 major ver - 20 */
    USHORT      usVerMinor;         /* OS/2 minor ver - 30,40 */

    //******************************************************/
    /** @cat Options affecting the generated LX executable */
    //******************************************************/
    /* none yet... */
};


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern struct options options; /* defined in d32globals.c */

#endif

