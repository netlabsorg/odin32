/* $Id: myldrSetVMflags.cpp,v 1.1.2.3 2002-04-01 09:57:19 bird Exp $
 *
 * myldrSetVMflags - ldrSetVMflags
 *
 * Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: myldrSetVMflags.cpp,v 1.1.2.3 2002-04-01 09:57:19 bird Exp $";
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

#define INCL_OS2KRNL_VM
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


/**
 * Sets the VM flags for an executable object.
 *
 * We'll overload this to change the VMA_SELALLOC flag into a VMA_VDM flag
 * for our executables. This will hopefully bring us a step closer to the
 * 100% working loaders, page aligning pages - hopefully.
 *
 * @returns     void
 * @param       pMTE        Pointer to the module table entry.
 * @param       flObj       LX Object flags.
 * @param       pflFlags1   Pointer to the flFlags1 of VMAllocMem (out).
 * @param       pflFlags2   Pointer to the flFlags2 of VMAllocMem (out).
 */
VOID LDRCALL myldrSetVMflags(PMTE pMTE, ULONG flObj, PULONG pflFlags1, PULONG pflFlags2)
{
    KLOGENTRY4("VOID","PMTE pMTE, ULONG flObj, PULONG pflFlags1, PULONG pflFlags2", pMTE, flObj, pflFlags1, pflFlags2);
    /*
     * call the orignal and let it do it's work.
     */
    ldrSetVMflags(pMTE, flObj, pflFlags1, pflFlags2);


    /*
     * If it's our handle then ...
     */
    if (GetState(pMTE->mte_sfn) == HSTATE_OUR)
    {
        ULONG ul = *pflFlags1;

        *pflFlags1 = ( (*pflFlags1) & ~VMA_SELALLOC ) | VMA_VDM;

        kprintf(("myldrSetVMflags: hMte=%04x %.8s flObj=%08x  *pflFlags1=%08x (before)  *pflFlags1=%08x (after) \n",
                 pMTE->mte_modname,  pMTE->mte_handle, flObj, ul, *pflFlags1
                 ));
    }
    KLOGEXITVOID();
}
