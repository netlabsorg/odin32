/* $Id: ImpDef.h,v 1.1 1999-09-05 02:25:48 bird Exp $ */
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
        BOOL fRemoveOS2APIPrefix;  /* For API exports any OS2 prefix is       *
                                    * removed (APIs have ordinals less than   *
                                    * ORD_START_INTERNAL_FUNCTIONS).          */
    } OPTIONS, *POPTIONS;
#endif

