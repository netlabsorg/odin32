/* $Id: myldrClose.cpp,v 1.1 1999-09-06 02:20:01 bird Exp $
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

#include "OS2Krnl.h"
#include "log.h"
#if 0
    #include "lxFile.h"
#else
    #define LXFile class { public: BOOL queryIsModuleName(const char *) {return FALSE;}}
#endif
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
    if (GetState(hFile) == HSTATE_CHECK)
        freeUncertainEntry(hFile);
    else
    {
        if (GetState(hFile) == HSTATE_PE)
        {
            rc = deleteNode(hFile);
            if (rc != NO_ERROR)
                kprintf(("Funny! deleteNode failed rc = %d\n", rc));
        }
    }

    SetState(hFile, HSTATE_UNUSED);
    return _ldrClose(hFile);
}
