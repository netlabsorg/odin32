/* $Id: StateUpd.h,v 1.1 1999-09-05 02:53:05 bird Exp $ */
/*
 * StateUpd - Scans source files for API functions and imports data on them.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _StateUpd_h_
#define _StateUpd_h_

#define MAJOR_VER   0
#define MINOR_VER   4

#pragma pack()

typedef struct _options
{
    BOOL fIntegrityBefore;  /* ib  */
    BOOL fIntegrityAfter;   /* ie  */
    BOOL fIntegrityOnly;    /* io  */
    BOOL fRecursive;        /* s   */
    BOOL fOld;              /* Old */
    BOOL fOS2;              /* Ignore OS2 prefixes */
    BOOL fCOMCTL32;         /* Ignore COMCTL32 prefixes */
    BOOL fVERSION;          /* Ignore VERSION prefixes */
} OPTIONS, *POPTIONS;


#endif /*_StateUpd_h_*/
