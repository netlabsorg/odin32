/* $Id: ras.h,v 1.1 1999-05-24 20:19:18 ktk Exp $ */

#ifndef __WINE_RAS_H
#define __WINE_RAS_H

#include "windef.h"

#define RAS_MaxEntryName	256

typedef struct tagRASCONNA {
	DWORD		dwSize;
	HRASCONN	hRasConn;
	CHAR		szEntryName[RAS_MaxEntryName+1];
} RASCONNA,*LPRASCONNA;

DWORD WINAPI RasEnumConnectionsA( LPRASCONNA rc, LPDWORD x, LPDWORD y);
#endif
