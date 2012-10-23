/* $Id: kdbhookhlp.cpp,v 1.1 2004-01-12 09:55:26 sandervl Exp $ */
/*
 * OS/2 native Presentation Manager hooks
 *
 *
 * Large Portions (C) Ulrich M”ller, XWorkplace
 * Copyright 2001 Patrick Haller (patrick.haller@innotek.de)
 * Copyright 2002-2003 Innotek Systemberatung GmbH
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define  INCL_WIN
#define  INCL_WININPUT
#define  INCL_WINMESSAGEMGR
#define  INCL_WINHOOKS
#define  INCL_PM
#define  INCL_BASE
#define  INCL_ERRORS
#include <os2wrap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pmscan.h>

#include <odin.h>
#include <pmkbdhk.h>
#include <kbdhook.h>
#include <custombuild.h>


//******************************************************************************
//******************************************************************************
const char *WIN32API QueryCustomStdClassName()
{
   return ODIN_WIN32_STDCLASS;
}
//******************************************************************************
//******************************************************************************
