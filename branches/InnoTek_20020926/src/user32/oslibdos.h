/* $Id: oslibdos.h,v 1.3 2000-01-18 20:08:10 sandervl Exp $ */
/*
 * Dos* api wrappers for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBDOS_H__
#define __OSLIBDOS_H__

#define OSLIB_PAG_READ	1
#define OSLIB_PAG_WRITE	2

DWORD OSLibDosGetSharedMem(PVOID addr, DWORD size, DWORD flags);

#endif //__OSLIBDOS_H__
