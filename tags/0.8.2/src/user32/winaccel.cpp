/* $Id: winaccel.cpp,v 1.12 2003-07-31 15:56:47 sandervl Exp $ */
/*
 * Win32 accelerator key functions for OS/2
 *
 * Based on Wine: (windows\input.c; loader\resource.c) (20000130)
 * Copyright 1993 Bob Amstadt
 * Copyright 1996 Albrecht Kleine
 * Copyright 1997 David Faure
 * Copyright 1998 Morten Welinder
 * Copyright 1998 Ulrich Weigand
 * Copyright 1993 Robert J. Amstadt
 * Copyright 1995 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <heapstring.h>
#include <win/winnls.h>

#define DBG_LOCALLOG    DBG_winaccel
#include "dbglocal.h"

/**********************************************************************
 *			LoadAccelerators32W	[USER.177]
 * The image layout seems to look like this (not 100% sure):
 * 00:	BYTE	type		type of accelerator
 * 01:	BYTE	pad		(to WORD boundary)
 * 02:	WORD	event
 * 04:	WORD	IDval
 * 06:	WORD	pad		(to DWORD boundary)
 */
HACCEL WINAPI LoadAcceleratorsW(HINSTANCE instance,LPCWSTR lpTableName)
{
    HRSRC hRsrc;
    HACCEL hMem,hRetval=0;
    DWORD size;

    if (!(hRsrc = FindResourceW( instance, lpTableName, RT_ACCELERATORW )))
    {
        dprintf(("LoadAcceleratorsW couldn't find accelerator table resource %x %x", instance, lpTableName));
        return 0;
    }
    else {
        hMem = LoadResource( instance, hRsrc );
        size = SizeofResource( instance, hRsrc );
        if(size >= sizeof(PE_ACCEL))
        {
	        LPPE_ACCEL accel_table = (LPPE_ACCEL) hMem;
	        LPACCEL accel;
	        int i,nrofaccells = size/sizeof(PE_ACCEL);

	        hRetval = GlobalAlloc(0,sizeof(ACCEL)*nrofaccells);
	        accel   = (LPACCEL)GlobalLock(hRetval);

	        for (i=0;i<nrofaccells;i++) {
		        accel[i].fVirt = accel_table[i].fVirt;
		        accel[i].key = accel_table[i].key;
		        accel[i].cmd = accel_table[i].cmd;
	        }
	        accel[i-1].fVirt |= 0x80;
        }
    }
    dprintf(("LoadAcceleratorsW returned %x %x %x\n", instance, lpTableName, hRetval));
    return hRetval;
}

HACCEL WINAPI LoadAcceleratorsA(HINSTANCE instance,LPCSTR lpTableName)
{
	LPWSTR	 uni;
	HACCEL result;
	if (HIWORD(lpTableName))
		uni = HEAP_strdupAtoW( GetProcessHeap(), 0, lpTableName );
	else
		uni = (LPWSTR)lpTableName;
	result = LoadAcceleratorsW(instance,uni);
	if (HIWORD(uni)) HeapFree( GetProcessHeap(), 0, uni);
	return result;
}

/**********************************************************************
 *             CopyAcceleratorTable32A   (USER32.58)
 */
INT WINAPI CopyAcceleratorTableA(HACCEL src, LPACCEL dst, INT entries)
{
  return CopyAcceleratorTableW(src, dst, entries);
}

/**********************************************************************
 *             CopyAcceleratorTable32W   (USER32.59)
 *
 * By mortene@pvv.org 980321
 */
INT WINAPI CopyAcceleratorTableW(HACCEL src, LPACCEL dst, INT entries)
{
  int i,xsize;
  LPACCEL accel = (LPACCEL)GlobalLock(src);
  BOOL done = FALSE;

  /* Do parameter checking to avoid the explosions and the screaming
     as far as possible. */
  if((dst && (entries < 1)) || (src == (HACCEL)NULL) || !accel)
  {
        dprintf(("CopyAcceleratorTableW: Application sent invalid parameters (%p %p %d).\n", (LPVOID)src, (LPVOID)dst, entries));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
  }
  xsize = GlobalSize(src)/sizeof(ACCEL);
  if (xsize>entries) entries=xsize;

  i=0;
  while(!done) {
    /* Spit out some debugging information. */
//    TRACE_(accel)("accel %d: type 0x%02x, event '%c', IDval 0x%04x.\n",
//	  i, accel[i].fVirt, accel[i].key, accel[i].cmd);

    /* Copy data to the destination structure array (if dst == NULL,
       we're just supposed to count the number of entries). */
    if(dst) {
        dst[i].fVirt = accel[i].fVirt;
        dst[i].key = accel[i].key;
        dst[i].cmd = accel[i].cmd;

        /* Check if we've reached the end of the application supplied
           accelerator table. */
        if(i+1 == entries) {
	        /* Turn off the high order bit, just in case. */
	        dst[i].fVirt &= 0x7f;
	        done = TRUE;
        }
    }

    /* The highest order bit seems to mark the end of the accelerator
       resource table, but not always. Use GlobalSize() check too. */
    if((accel[i].fVirt & 0x80) != 0) done = TRUE;

    i++;
  }

  return i;
}

/*********************************************************************
 *                    CreateAcceleratorTable   (USER32.64)
 *
 * By mortene@pvv.org 980321
 */
HACCEL WINAPI CreateAcceleratorTableA(LPACCEL lpaccel, INT cEntries)
{
  HACCEL	hAccel;
  LPACCEL	accel;
  int		i;

  /* Do parameter checking just in case someone's trying to be
     funny. */
  if(cEntries < 1) {
        dprintf(("CreateAcceleratorTableA: Application sent invalid parameters (%p %d).\n", lpaccel, cEntries));
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
  }
  dprintf(("FIXME: CreateAcceleratorTableA: should check that the accelerator descriptions are valid return NULL and SetLastError() if not"));

  /* Allocate memory and copy the table. */
  hAccel = GlobalAlloc(0,cEntries*sizeof(ACCEL));

  if(!hAccel) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return (HACCEL)NULL;
  }
  accel = (LPACCEL)GlobalLock(hAccel);
  for (i=0;i<cEntries;i++) {
  	    accel[i].fVirt = lpaccel[i].fVirt;
  	    accel[i].key = lpaccel[i].key;
  	    accel[i].cmd = lpaccel[i].cmd;
  }
  /* Set the end-of-table terminator. */
  accel[cEntries-1].fVirt |= 0x80;

  dprintf(("CreateAcceleratorTableA %x %x returned %x\n", lpaccel, cEntries, hAccel));
  return hAccel;
}

/*********************************************************************
 *                    CreateAcceleratorTableW   (USER32.64)
 *
 *
 */
HACCEL WINAPI CreateAcceleratorTableW(LPACCEL lpaccel, INT cEntries)
{
  HACCEL	hAccel;
  LPACCEL	accel;
  int		i;
  char		ckey;

  /* Do parameter checking just in case someone's trying to be
     funny. */
  if(cEntries < 1) {
        dprintf(("CreateAcceleratorTableW: Application sent invalid parameters (%p %d).\n", lpaccel, cEntries));
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
  }
  dprintf(("FIXME: CreateAcceleratorTableW: should check that the accelerator descriptions are valid return NULL and SetLastError() if not"));

  /* Allocate memory and copy the table. */
  hAccel = GlobalAlloc(0,cEntries*sizeof(ACCEL));

  if(!hAccel) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return (HACCEL)NULL;
  }
  accel = (LPACCEL)GlobalLock(hAccel);

  for (i=0;i<cEntries;i++) {
       accel[i].fVirt = lpaccel[i].fVirt;
       if( !(accel[i].fVirt & FVIRTKEY) ) {
    	    ckey = (char) lpaccel[i].key;
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &ckey, 1, (LPWSTR)&accel[i].key, 1);
       }
       else  accel[i].key = lpaccel[i].key;
       accel[i].cmd = lpaccel[i].cmd;
  }

  /* Set the end-of-table terminator. */
  accel[cEntries-1].fVirt |= 0x80;

  dprintf(("CreateAcceleratorTableW %x %x returned %x\n", lpaccel, cEntries, hAccel));
  return hAccel;
}

/******************************************************************************
 * DestroyAcceleratorTable [USER32.130]
 * Destroys an accelerator table
 *
 * NOTES
 *    By mortene@pvv.org 980321
 *
 * PARAMS
 *    handle [I] Handle to accelerator table
 *
 * RETURNS STD
 */
BOOL WINAPI DestroyAcceleratorTable( HACCEL handle )
{
    return GlobalFree(handle);
}

