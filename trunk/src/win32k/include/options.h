/* $Id: options.h,v 1.1 1999-09-06 02:19:59 bird Exp $
 * Options.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */

#ifndef _options_h_
#define _options_h_

/* fKernel */
#define KF_UNI      0x00000000UL
#define KF_SMP      0x00000001UL


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
    int noExeTIBFix;        /* ask win32k for initstuff (win32k only) */
    int AlignSections;      /* don't align sections */
    //int TIBFixObject;       /* make a TIBFix object - AlignSections = 0 */

    int lxoffset;           /* if true use the fixed LXOFFSET if not ulPEOffset */
    int ourstub;            /* our DOS stub or the one from the PE-file */
};


extern struct options options;

#define SET_OPTIONS_TO_DEFAULT(o)            \
            o.fQuiet         = 0;            \
            o.usCom          = OUTPUT_COM2;  \
            o.fLogging       = 0;            \
            o.ulBuild        = 8259;         \
            o.fKernel        = KF_UNI;       \
            o.usVerMajor     = 20;           \
            o.usVerMinor     = 30;           \
            o.noExeTIBFix    = 0;            \
            o.AlignSections  = 1;            \
            o.lxoffset       = 0;            \
            o.ourstub        = 0


#define SET_OPTIONS_WIN32K(o)                \
            SET_OPTIONS_TO_DEFAULT(o)

#define SET_OPTIONS_WIN32K_SAFE(o)           \
            SET_OPTIONS_TO_DEFAULT(o);       \
            o.noExeTIBFix    = 0;            \
            o.AlignSections  = 1


#define SET_OPTIONS_PE2LX(o)                 \
            SET_OPTIONS_TO_DEFAULT(o);       \
            o.fLogging       = 1;            \
            o.noExeTIBFix    = 0;            \
            o.AlignSections  = 1;            \
            o.lxoffset       = 0;            \
            o.ourstub        = 0

#endif

