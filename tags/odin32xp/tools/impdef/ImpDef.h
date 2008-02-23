/* $Id: ImpDef.h,v 1.3 2002-02-24 02:44:40 bird Exp $ */
/*
 * ImpDef - Create export file which use internal names and ordinals.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _ImpDef_h_
#define _ImpDef_h_
    typedef struct _Options
    {
        KBOOL fSimilarToExported;    /* Merges the names of the exported and    *
                                      * internal function when possible/needed. */
        KBOOL ulOrdStartInternalFunctions;
                                     /* Ordinal number where internal functions *
                                      * starts. API functions have lower        *
                                      * ordinal values that this number.        */
        KBOOL fRemoveOS2APIPrefix;   /* For API exports any OS2 prefix is       *
                                      * removed (APIs have ordinals less than   *
                                      * ORD_START_INTERNAL_FUNCTIONS).          */
        KBOOL fInternalFnExportStdCallsIntName;
                                     /* Fix! Export internal names for internal *
                                      * stdcall functions.                      */
    } OPTIONS, *POPTIONS;
#endif

