/* $Id: accelerator.cpp,v 1.3 1999-06-10 17:08:51 phaller Exp $ */

/*
 * PE2LX accelerator resource support code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_GPIBITMAPS
#define INCL_BITMAPFILEFORMAT
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
#include "pefile.h"
#include "lx.h"
#include "icon.h"
#include "accelerator.h"
#include "misc.h"

//******************************************************************************
//******************************************************************************
void ShowAccelerator(int id, WINACCEL *accdata, int size)
{
 ACCELTABLE *OS2Acc;
 int         os2size,i;

   //First save original win32 resource
   OS2Exe.StoreWin32Resource(id, RT_ACCELTABLE, size, (char *)accdata);

   os2size = sizeof(ACCELTABLE) + ((size/8)-1)*sizeof(ACCEL);
   OS2Acc  = (ACCELTABLE *)malloc(os2size);
   memset(OS2Acc, 0, os2size);
   OS2Acc->cAccel   = size/8;
   OS2Acc->codepage = 437;

   for(i=0;i<OS2Acc->cAccel;i++) {
    OS2Acc->aaccel[i].key = accdata->key;
    OS2Acc->aaccel[i].cmd = accdata->cmd;
    if(accdata[i].fVirt & FVIRTKEY)
        OS2Acc->aaccel[i].fs |= AF_VIRTUALKEY;
    if(accdata[i].fVirt & FNOINVERT)
        OS2Acc->aaccel[i].fs |= AF_CHAR;
    if(accdata[i].fVirt & FSHIFT)
        OS2Acc->aaccel[i].fs |= AF_SHIFT;
    if(accdata[i].fVirt & FCONTROL)
        OS2Acc->aaccel[i].fs |= AF_CONTROL;
    if(accdata[i].fVirt & FALT)
        OS2Acc->aaccel[i].fs |= AF_ALT;
   }
   OS2Exe.StoreResource(id, RT_ACCELTABLE, os2size, (char *)OS2Acc);
   free(OS2Acc);
}
//******************************************************************************
//******************************************************************************
