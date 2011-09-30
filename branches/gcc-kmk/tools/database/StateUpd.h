/* $Id: StateUpd.h,v 1.8 2002-02-24 02:58:28 bird Exp $ */
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
    KBOOL       fIntegrityBefore;       /* ib  */
    KBOOL       fIntegrityAfter;        /* ie  */
    KBOOL       fIntegrityOnly;         /* io  */
    KBOOL       fRecursive;             /* s   */
    KBOOL       fOld;                   /* Old */
    KBOOL       fOS2;                   /* Ignore OS2 prefixes */
    KBOOL       fCOMCTL32;              /* Ignore COMCTL32 prefixes */
    KBOOL       fVERSION;               /* Ignore VERSION prefixes */
    char *      pszDLLName;             /* Name of the dll being processed */
    signed long lDllRefcode;            /* Database reference code of the dll */
    signed long lFileRefcode;           /* File reference code. */
    signed long lSeqFile;               /* Design note file sequence number. */
} OPTIONS, *POPTIONS;


#endif /*_StateUpd_h_*/
