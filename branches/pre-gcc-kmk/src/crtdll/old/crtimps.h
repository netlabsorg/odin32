/* $Id: crtimps.h,v 1.1 2000-11-21 23:49:03 phaller Exp $ */

/* Definitions for the CRTDLL internal imports 
 *
 * Copyright 1999-2000 Jens Wiessner
 *
 */

#ifndef _CRTINTIMP_H
#define _CRTINTIMP_H


int 		CDECL CRTDLL__getch(void);
VOID* 		CDECL CRTDLL_malloc(DWORD size);
int    		CDECL CRTDLL__wcsnicmp(LPCWSTR, LPCWSTR, int);
extern USHORT 	*CRTDLL_pwctype_dll;

#endif
