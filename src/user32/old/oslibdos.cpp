/* $Id: oslibdos.cpp,v 1.1 2001-05-11 08:37:24 sandervl Exp $ */
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
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#include <os2wrap.h>
#include <win32type.h>
#include <misc.h>
#include "oslibdos.h"

#define DBG_LOCALLOG	DBG_oslibdos
#include "dbglocal.h"

//******************************************************************************
//Map shared memory into our address space
//******************************************************************************
DWORD OSLibDosGetSharedMem(PVOID addr, DWORD size, DWORD flags)
{
 DWORD os2flags = 0, rangesize, rangeflags;

  if(flags & OSLIB_PAG_READ)
	os2flags |= PAG_READ;
  if(flags & OSLIB_PAG_WRITE)
	os2flags |= PAG_WRITE;

  return DosGetSharedMem(addr, os2flags);
}
//******************************************************************************
//******************************************************************************

