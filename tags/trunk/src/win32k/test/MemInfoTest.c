/* $Id: MemInfoTest.c,v 1.1 2001-02-11 15:25:51 bird Exp $
 *
 * Test program for the k32/W32kQuerySystemMemInfo IOCtl.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_BASE

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <stdio.h>
#include <string.h>

#include "win32k.h"



int main(void)
{
    APIRET rc;

    rc = libWin32kInit();
    if (rc)
    {
        fprintf(stderr, "Win32k init failed with rc=0x%x(%d)\n", rc, rc);
        return rc;
    }

    for (;;)
    {
        K32SYSTEMMEMINFO MemInfo;

        memset(&MemInfo, 0xFE, sizeof(MemInfo));
        MemInfo.cb = sizeof(K32SYSTEMMEMINFO);
        MemInfo.flFlags = K32_SYSMEMINFO_ALL;
        rc = W32kQuerySystemMemInfo(&MemInfo);
        if (rc)
        {
            fprintf(stderr, "W32kQuerySystemMemInfo failed with rc=0x%x(%d)\n", rc, rc);
            break;
        }

        printf("\n"
               " * Swap File *\n"
               " -------------\n");
        printf(" fSwapFile               0x%08x (%d)\n", MemInfo.fSwapFile            , MemInfo.fSwapFile);
        printf(" cbSwapFileSize          0x%08x (%d)\n", MemInfo.cbSwapFileSize       , MemInfo.cbSwapFileSize);
        printf(" cbSwapFileAvail         0x%08x (%d)\n", MemInfo.cbSwapFileAvail      , MemInfo.cbSwapFileAvail);
        printf(" cbSwapFileUsed          0x%08x (%d)\n", MemInfo.cbSwapFileUsed       , MemInfo.cbSwapFileUsed);
        printf(" cbSwapFileMinFree       0x%08x (%d)\n", MemInfo.cbSwapFileMinFree    , MemInfo.cbSwapFileMinFree);
        printf(" cbSwapFileCFGMinFree    0x%08x (%d)\n", MemInfo.cbSwapFileCFGMinFree , MemInfo.cbSwapFileCFGMinFree);
        printf(" cbSwapFileCFGSwapSize   0x%08x (%d)\n", MemInfo.cbSwapFileCFGSwapSize, MemInfo.cbSwapFileCFGSwapSize);
        printf(" cSwapFileBrokenDF       0x%08x (%d)\n", MemInfo.cSwapFileBrokenDF    , MemInfo.cSwapFileBrokenDF);
        printf(" cSwapFileGrowFails      0x%08x (%d)\n", MemInfo.cSwapFileGrowFails   , MemInfo.cSwapFileGrowFails);
        printf(" cSwapFileInMemFile      0x%08x (%d)\n", MemInfo.cSwapFileInMemFile   , MemInfo.cSwapFileInMemFile);


        printf("\n"
               " * Physical Memory *\n"
               " -------------------\n");
        printf(" cbPhysSize              0x%08x (%d)\n", MemInfo.cbPhysSize , MemInfo.cbPhysSize );
        printf(" cbPhysAvail             0x%08x (%d)\n", MemInfo.cbPhysAvail, MemInfo.cbPhysAvail);
        printf(" cbPhysUsed              0x%08x (%d)\n", MemInfo.cbPhysUsed , MemInfo.cbPhysUsed );


        printf("\n"
               " * Other paging info *\n"
               " ---------------------\n");
        printf(" fPagingSwapEnabled      0x%08x (%d)\n", MemInfo.fPagingSwapEnabled     , MemInfo.fPagingSwapEnabled     );
        printf(" cPagingPageFaults       0x%08x (%d)\n", MemInfo.cPagingPageFaults      , MemInfo.cPagingPageFaults      );
        printf(" cPagingPageFaultsActive 0x%08x (%d)\n", MemInfo.cPagingPageFaultsActive, MemInfo.cPagingPageFaultsActive);
        printf(" cPagingPhysPages        0x%08x (%d)\n", MemInfo.cPagingPhysPages       , MemInfo.cPagingPhysPages       );
        printf(" ulPagingPhysMax         0x%08x (%d)\n", MemInfo.ulPagingPhysMax        , MemInfo.ulPagingPhysMax        );
        printf(" cPagingResidentPages    0x%08x (%d)\n", MemInfo.cPagingResidentPages   , MemInfo.cPagingResidentPages   );
        printf(" cPagingSwappablePages   0x%08x (%d)\n", MemInfo.cPagingSwappablePages  , MemInfo.cPagingSwappablePages  );
        printf(" cPagingDiscardableInmem 0x%08x (%d)\n", MemInfo.cPagingDiscardableInmem, MemInfo.cPagingDiscardableInmem);
        printf(" cPagingDiscardablePages 0x%08x (%d)\n", MemInfo.cPagingDiscardablePages, MemInfo.cPagingDiscardablePages);


        printf("\n"
               " * Other paging info *\n"
               " ---------------------\n");
        printf(" ulAddressLimit          0x%08x (%d)\n", MemInfo.ulAddressLimit        , MemInfo.ulAddressLimit         );
        printf(" ulVMArenaPrivMax        0x%08x (%d)\n", MemInfo.ulVMArenaPrivMax      , MemInfo.ulVMArenaPrivMax       );
        printf(" ulVMArenaSharedMin      0x%08x (%d)\n", MemInfo.ulVMArenaSharedMin    , MemInfo.ulVMArenaSharedMin     );
        printf(" ulVMArenaSharedMax      0x%08x (%d)\n", MemInfo.ulVMArenaSharedMax    , MemInfo.ulVMArenaSharedMax     );
        printf(" ulVMArenaSystemMin      0x%08x (%d)\n", MemInfo.ulVMArenaSystemMin    , MemInfo.ulVMArenaSystemMin     );
        printf(" ulVMArenaSystemMax      0x%08x (%d)\n", MemInfo.ulVMArenaSystemMax    , MemInfo.ulVMArenaSystemMax     );
        printf(" ulVMArenaHighPrivMax    0x%08x (%d)\n", MemInfo.ulVMArenaHighPrivMax  , MemInfo.ulVMArenaHighPrivMax   );
        printf(" ulVMArenaHighSharedMin  0x%08x (%d)\n", MemInfo.ulVMArenaHighSharedMin, MemInfo.ulVMArenaHighSharedMin );
        printf(" ulVMArenaHighSharedMax  0x%08x (%d)\n", MemInfo.ulVMArenaHighSharedMax, MemInfo.ulVMArenaHighSharedMax );

        DosSleep(2000);
    }

    libWin32kTerm();
    return rc;
}

