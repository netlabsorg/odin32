/* $Id: oleaut32.cpp,v 1.8 2001-07-12 17:16:12 sandervl Exp $ */
/* 
 * OLEAUT32 
 * 
 * Copyright 1999 Sander van Leeuwen (OS/2 Port 990815)
 *
 * Based on Wine code: (ole\compobj.c)
 * 
 *	Copyright 1995	Martin von Loewis
 *	Copyright 1998	Justin Bradford
 *      Copyright 1999  Francis Beaudet
 *  	Copyright 1999  Sylvain St-Germain
 *
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 * TODO: OaBuildVersion has to be changed (as well as GetVersion in kernel32)
 */

#include "oleaut32.h"
#ifdef DEBUG
#define DEBUG_RUNTIME
#endif

#include <debugdefs.h>

//******************************************************************************
//Used to open any private logfiles used in oleaut32
//******************************************************************************
void OpenPrivateLogFiles()
{
#ifdef DEBUG
    OpenPrivateLogFileTypelib();
#endif
}
//******************************************************************************
//Used to close all private logfiles used in oleaut32
//******************************************************************************
void ClosePrivateLogFiles()
{
#ifdef DEBUG
    ClosePrivateLogFileTypelib();
#endif
}


