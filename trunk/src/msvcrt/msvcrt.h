/* $Id: msvcrt.h,v 1.3 2000-08-22 08:09:21 phaller Exp $ */

/* Definitions for the MS Visual C-Runtime library (MSVCRT/MSVCRT20/MSVCRT40)
 *
 * Copyright 1999 Jens Wiessner
 * Copyright 2000 Patrick Haller
 */

#ifndef _H_MSVCRT_
#define _H_MSVCRT_


typedef struct
{
    HANDLE handle;
    int      pad[7];
} CRTDLL_FILE, *PCRTDLL_FILE;

typedef VOID (*new_handler_type)(VOID);

// extern Defs
extern CRTDLL_FILE _iob;


#endif /* _H_MSVCRT_ */
