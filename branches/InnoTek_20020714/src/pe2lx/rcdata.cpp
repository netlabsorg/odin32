/* $Id: rcdata.cpp,v 1.3 1999-06-10 17:08:55 phaller Exp $ */

/*
 * PE2LX RCDATA resources
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "lx.h"
#include "rcdata.h"

//******************************************************************************
//******************************************************************************
void ShowRCData(int id, char *data, int size)
{
  cout << "RCData/Version resource with id " << id << " , size " << size << endl;
  OS2Exe.StoreResource(id, RT_RCDATA, size, data);
}
//******************************************************************************
//******************************************************************************
