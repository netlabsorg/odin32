/* $Id: StateUpd.h,v 1.7 2001-09-07 10:33:10 bird Exp $ */
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
    BOOL        fRecursive;             /* s   */
    BOOL        fOld;                   /* Old */
    BOOL        fOS2;                   /* Ignore OS2 prefixes */
    BOOL        fCOMCTL32;              /* Ignore COMCTL32 prefixes */
    BOOL        fVERSION;               /* Ignore VERSION prefixes */
    char *      pszDLLName;             /* Name of the dll being processed */
    signed long lDllRefcode;            /* Database reference code of the dll */
    signed long lFileRefcode;           /* File reference code. */
    signed long lSeqFile;               /* Design note file sequence number. */
} OPTIONS, *POPTIONS;


#endif /*_StateUpd_h_*/
