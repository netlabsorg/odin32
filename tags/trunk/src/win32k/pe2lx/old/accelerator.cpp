/* $Id: accelerator.cpp,v 1.1 1999-10-06 18:50:49 bird Exp $ */

/*
 * PE2LX accelerator resource support code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "pe2lx.h"
#include "accelerator.h"

//******************************************************************************
//******************************************************************************
BOOL ShowAccelerator(LXHeaderSuper &OS2Exe, int id, WINACCEL *accdata, int size)
{
    ACCELTABLE *OS2Acc;
    ULONG       os2size;
    int         rc;

    ltassert((ULONG)accdata > MINPTR && (ULONG)accdata < MAXPTR);

    //First save original win32 resource
    ltassert(OS2Exe.StoreWin32Resource(id, RT_ACCELTABLE, size, (char *)accdata));

    OS2Acc = (ACCELTABLE*)ConvertAccelerator(accdata, size, (PULONG)SSToDS(&os2size));
    ltasserthp(OS2Acc);

    rc = OS2Exe.StoreResource(id, RT_ACCELTABLE, os2size, (char *)OS2Acc);
    free(OS2Acc);
    return rc;
}
//******************************************************************************
//******************************************************************************
void *ConvertAccelerator(WINACCEL *accdata, int size, PULONG pulSize)
{
    ACCELTABLE *OS2Acc;
    int         i;
    int         rc;

    ltassert((ULONG)accdata > MINPTR && (ULONG)accdata < MAXPTR);

    *pulSize = sizeof(ACCELTABLE) + ((size/8)-1)*sizeof(ACCEL);
    OS2Acc  = (ACCELTABLE *)malloc(*pulSize);
    ltassert(OS2Acc != NULL);
    memset(OS2Acc, 0, *pulSize);
    OS2Acc->cAccel   = size/8;
    OS2Acc->codepage = 437;

    for (i = 0; i < OS2Acc->cAccel; i++)
    {
        OS2Acc->aaccel[i].key = accdata->key;
        OS2Acc->aaccel[i].cmd = accdata->cmd;
        if (accdata[i].fVirt & FVIRTKEY)
            OS2Acc->aaccel[i].fs |= AF_VIRTUALKEY;
        if (accdata[i].fVirt & FNOINVERT)
            OS2Acc->aaccel[i].fs |= AF_CHAR;
        if (accdata[i].fVirt & FSHIFT)
            OS2Acc->aaccel[i].fs |= AF_SHIFT;
        if (accdata[i].fVirt & FCONTROL)
            OS2Acc->aaccel[i].fs |= AF_CONTROL;
        if (accdata[i].fVirt & FALT)
            OS2Acc->aaccel[i].fs |= AF_ALT;
    }

    return (void*)OS2Acc;
}
//******************************************************************************
//******************************************************************************
ULONG QuerySizeAccelerator(WINACCEL accdata, int size)
{
    return sizeof(ACCELTABLE) + ((size/8)-1)*sizeof(ACCEL);
}

