/* $Id: ImpDef.h,v 1.2 1999-09-08 07:30:10 bird Exp $ */
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
        BOOL fSimilarToExported;   /* Merges the names of the exported and    *
                                    * internal function when possible/needed. */
        BOOL ulOrdStartInternalFunctions;
                                   /* Ordinal number where internal functions *
                                    * starts. API functions have lower        *
                                    * ordinal values that this number.        */
        BOOL fRemoveOS2APIPrefix;  /* For API exports any OS2 prefix is       *
                                    * removed (APIs have ordinals less than   *
                                    * ORD_START_INTERNAL_FUNCTIONS).          */
        BOOL fInternalFnExportStdCallsIntName;
                                   /* Fix! Export internal names for internal *
                                    * stdcall functions.                      */
    } OPTIONS, *POPTIONS;
#endif

