/* $Id: pmkbdhkp.h,v 1.4 2003-10-22 15:56:07 sandervl Exp $ */
/*
 * Window low-level keyboard hook
 *
 * Copyright 2001 Patrick Haller (patrick.haller@innotek.de)
 * Copyright (C) 1999-2000 Ulrich M”ller.
 * Copyright 2002-2003 Innotek Systemberatung GmbH
 */

#ifndef __PMKBDHK_PRIVATE_H__
#define __PMKBDHK_PRIVATE_H__

#include <odin.h>

/*
 * HK_PREACCEL:
 *      additional undocumented PM hook type
 */

#ifndef HK_PREACCEL
#define HK_PREACCEL             17
#endif


/*
 * Note: this won't interfere with any WM_USER value,
 * WM_PENFIRST is 0x04c0
 */
#ifndef WM_CHAR_SPECIAL
#define WM_CHAR_SPECIAL 0x04bf
#endif

typedef struct _HOOKDATA
{
   HAB         habDaemonObject;    // anchor block of hwndDaemonObject; cached for speed
   BOOL        fPreAccelHooked;
   HMODULE     hmodDLL;            // module handle
   char        szWindowClass[256];
} HOOKDATA, *PHOOKDATA;


extern HOOKDATA G_HookData;

extern APIRET APIENTRY DosQuerySysState (ULONG func,
      				ULONG par1, ULONG pid, ULONG _reserved_,
  				PVOID buf,
				ULONG bufsz);

#endif //__PMKBDHK_PRIVATE_H__
