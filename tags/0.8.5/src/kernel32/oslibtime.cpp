/* $Id: oslibtime.cpp,v 1.1 2002-06-26 07:14:18 sandervl Exp $ */
/*
 * OS/2 time procedures
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_WIN
#define INCL_BASE
#define INCL_DOSPROCESS
#define INCL_DOSSEL
#define INCL_DOSNLS
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <string.h>
#include <stdlib.h>
#include <stdio.h>  
#include <malloc.h> 
#include "oslibtime.h"
#include <dbglog.h>

#define DBG_LOCALLOG	DBG_oslibtime
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
ULONG OSLibDosGetTickCount()
{
    return WinGetCurrentTime(0);
}
//******************************************************************************
//******************************************************************************
