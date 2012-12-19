/* $Id: ddclipper.cpp,v 1.1 2002-12-04 10:34:57 sandervl Exp $ */

/*
 * DXDrawClipper DLL implementaion
 *
 * Copyright 2001 Patrick Haller <patrick.haller@innotek.de>
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

// define the following as we include winnt.h
#define _OS2WIN_H
#define FAR

#include <odin.h>
#include <winbase.h>
#include <memory.h>
#include <stdio.h>

#define CINTERFACE
#include "ddraw2d.h"
#include "clipper.h"
#include "winerror.h"

#include <misc.h>

//******************************************************************************
//******************************************************************************
HRESULT WIN32API OS2DirectDrawCreateClipper(DWORD dwFlags,
                                            LPDIRECTDRAWCLIPPER FAR *lplpDDClipper,
                                            IUnknown FAR *pUnkOuter)
{
    OS2IDirectDrawClipper *newDDClipper;
    HRESULT         rc;

    dprintf(("DDRAW-CLIPPER: DirectDrawCreateClipper(%08xh,%08xh,%08xh)\n",
             dwFlags,
             lplpDDClipper,
             pUnkOuter));

    // Verify parameters according to the docs
    if (pUnkOuter != NULL)
        return CLASS_E_NOAGGREGATION;

    if (dwFlags != 0)
        return DDERR_INVALIDPARAMS;


    newDDClipper = new OS2IDirectDrawClipper();

    if(newDDClipper == NULL)
    {
        rc = DDERR_OUTOFMEMORY;
    }
    else
    {
        newDDClipper->Vtbl.fnAddRef((IDirectDrawClipper *)newDDClipper);

        rc = newDDClipper->GetLastError();
        if(rc != DD_OK)
        {
            *lplpDDClipper = NULL;

            delete newDDClipper;
        }
        else
            *lplpDDClipper = (LPDIRECTDRAWCLIPPER)newDDClipper;
    }
    return(rc);
}


