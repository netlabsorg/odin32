/* $Id: cvtaccel.cpp,v 1.1 1999-08-19 14:19:14 sandervl Exp $ */

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
#define INCL_DOSNLS
#define INCL_WIN
#include <os2wrap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <win32type.h>
#include <winaccel.h>
#include <misc.h>

//******************************************************************************
//******************************************************************************
void *ConvertAccelerator(WINACCEL *accdata, int size, int cp)
{
 ACCELTABLE *OS2Acc;
 int         os2size,i;
 ULONG       ulCpSize, ulCP;

   os2size = sizeof(ACCELTABLE) + ((size/8)-1)*sizeof(ACCEL);
   OS2Acc  = (ACCELTABLE *)malloc(os2size);
   memset(OS2Acc, 0, os2size);
   OS2Acc->cAccel   = size/8;
   if(cp == 0) {
    	OS2Acc->codepage = 437;
   }
   else
   {
    	DosQueryCp(sizeof(ulCP), &ulCP, &ulCpSize);
    	OS2Acc->codepage = ulCP;
   }


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
   return (void *)OS2Acc;
}
//******************************************************************************
//******************************************************************************
