/* $Id: StateUpd.h,v 1.6 2001-09-07 10:24:06 bird Exp $ */
/*
 * StateUpd - Scans source files for API functions and imports data on them.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _StateUpd_h_
#define _StateUpd_h_

#define MAJOR_VER   0
#define MINOR_VER   5

#pragma pack()

typedef struct _options
{
    BOOL        fIntegrityBefore;       /* ib  */
    BOOL        fIntegrityAfter;        /* ie  */
    BOOL        fIntegrityOnly;         /* io  */
    BOOL        fRecursive;             /* s  */
    BOOL        fOld;                   /* Old */
    BOOL        fOS2;                   /* Ignore OS2 prefixes. */
    BOOL        fCOMCTL32;              /* Ignore COMCTL32 prefixes. */
    BOOL        fVERSION;               /* Ignore VERSION prefixes. */
    char *      pszModName;             /* Name of the module being processed. */
    char        chModType;              /* Type of module being processed. */
    signed long lModRefcode;            /* Database reference code of the module. */
    signed long lFileRefcode;           /* File reference code. */
    char **     papszDirs;              /* Pointer to NULL terminated array of directories*/
                                        /* to be searched when .obj files are specified. */
} OPTIONS, *POPTIONS;


#endif /*_StateUpd_h_*/
