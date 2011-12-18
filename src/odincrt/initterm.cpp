/*
 * ODINCRT DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSERRORS
#include <os2wrap.h> // Odin32 OS/2 api wrappers
#include <exitlist.h>
#include <initdll.h>

ULONG SYSTEM DLL_Init(ULONG hModule)
{
#ifdef WITH_KLIB
    /*
     * We need to reserve memory for the executable image
     * before initiating any heaps. Lets do reserve 32MBs
     */
    PVOID pvReserved = NULL;
    DosAllocMem(&pvReserved, 32*1024*1024, PAG_READ);
#endif

    if (DLL_InitDefault(hModule) == -1)
        return -1;

#if 1
    /*
     * Experimental console hack. Sets apptype to PM anyhow.
     * First Dll to be initiated should now allways be OdinCrt!
     * So include odincrt first!
     */
    PPIB pPIB;
    PTIB pTIB;
    APIRET rc = DosGetInfoBlocks(&pTIB, &pPIB);
    if (rc != NO_ERROR)
        return -1;
    pPIB->pib_ultype = 3;
#endif

#ifdef WITH_KLIB
    /* cleanup - hacking is done */
    DosFreeMem(pvReserved);
#endif

    return EXITLIST_ODINCRT;
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermDefault(hModule);
}

