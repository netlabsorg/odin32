/* $Id: myldrClose.cpp,v 1.3 1999-10-27 02:02:58 bird Exp $
 *
 * myldrClose - _ldrClose
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <memory.h>
#include <stdlib.h>

#include "log.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "pe2lx.h"
#include "avl.h"
#include "ldr.h"
#include "ldrCalls.h"


/**
 * Close a file.
 * @returns    NO_ERROR on success?
 *             error code on error?
 * @param      hFile  Handle to file which is to be closed.
 */
ULONG LDRCALL myldrClose(SFN hFile)
{
    int rc;

    /* closes handle */
    kprintf(("_ldrClose: hFile = %.4x\n", hFile));
    if (GetState(hFile) == HSTATE_OUR)
    {
        rc = removeModule(hFile);
        if (rc != NO_ERROR)
            kprintf(("_ldrClose: removeModule retured rc=%d\n", rc));

        SetState(hFile, HSTATE_UNUSED);
    }

    return _ldrClose(hFile);
}
