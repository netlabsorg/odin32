/* $Id: myldrClose.cpp,v 1.1.2.2 2002-04-01 09:06:08 bird Exp $
 *
 * myldrClose - ldrClose
 *
 * Copyright (c) 1998-2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: myldrClose.cpp,v 1.1.2.2 2002-04-01 09:06:08 bird Exp $";
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_LDR
#define INCL_KKL_LOG
#define INCL_KKL_AVL

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "LXexe.h"                      /* OS/2 LX structs and definitions. */
#include "PEexe.h"                      /* Wine PE structs and definitions. */
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include <memory.h>
#include <stdlib.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "ldr.h"
#include "ModuleBase.h"
#include "Pe2Lx.h"


/**
 * Close a file.
 * @returns    NO_ERROR on success?
 *             error code on error?
 * @param      hFile  Handle to file which is to be closed.
 */
ULONG LDRCALL myldrClose(SFN hFile)
{
    KLOGENTRY1("ULONG","SFN hFile", hFile);
    ULONG   rc;
    /* closes handle */
    kprintf(("myldrClose: hFile = %.4x\n", hFile));
    if (GetState(hFile) == HSTATE_OUR)
    {
        APIRET rc;

        #ifdef DEBUG
        PMODULE pMod = getModuleBySFN(hFile);
        if (pMod != NULL)
            pMod->Data.pModule->dumpVirtualLxFile();
        else
            kprintf(("myldrClose: getModuleBySFN failed!!!"));
        #endif

        rc = removeModule(hFile);
        if (rc != NO_ERROR)
            kprintf(("myldrClose: removeModule retured rc=%d\n", rc));

        #pragma info(notrd)
        SetState(hFile, HSTATE_UNUSED);
        #pragma info(restore)
    }
    /*
     * Invalidate the odin32path if kernel32 is closed.
     *  (Might possible not be needed as Pe2Lx does invalides
     *   the odin32path on object destruction.)
     */
    else if (Pe2Lx::getKernel32SFN() == hFile)
        Pe2Lx::invalidateOdin32Path();

    /*
     * Finally call the real close function.
     */
    rc = ldrClose(hFile);
    KLOGEXIT(rc);
    return rc;
}
