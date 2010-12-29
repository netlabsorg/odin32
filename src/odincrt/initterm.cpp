/*
 * DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSERRORS
#include <os2wrap.h> // Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <misc.h>
#include <exitlist.h>
#include <initdll.h>

#ifdef __IBMCPP__

static void APIENTRY cleanup(ULONG reason);

/*
 * _DLL_InitTerm is the function that gets called by the operating system
 * loader when it loads and frees this DLL for each process that accesses
 * this DLL.  However, it only gets called the first time the DLL is loaded
 * and the last time it is freed for a particular process.  The system
 * linkage convention MUST be used because the operating system loader is
 * calling this function.
 *
 * If ulFlag is zero then the DLL is being loaded so initialization should
 * be performed.  If ulFlag is 1 then the DLL is being freed so
 * termination should be performed.
 *
 * A non-zero value must be returned to indicate success.
 */
unsigned long SYSTEM _DLL_InitTerm(unsigned long hModule, unsigned long ulFlag)
{
    APIRET rc;

    switch (ulFlag)
    {
    case 0:
    {
#ifdef WITH_KLIB
        /*
         * We need to reserve memory for the executable image
         * before initiating any heaps. Lets do reserve 32MBs
         */
        PVOID pvReserved = NULL;
        DosAllocMem(&pvReserved, 32*1024*1024, PAG_READ);
#endif
        /* initialize C and C++ runtime */
        if (_CRT_init() == -1)
            return 0UL;
        ctordtorInit();

        /*
         * Register an exit list routine to clean up runtime at termination.
         * We can't simply do it at DLL unload time because this is forbidden
         * for VAC runtime Odin runtime is based on (see CPPLIB.INF from VAC
         * for details).
         */
        rc = DosExitList(EXITLIST_ODINCRT|EXLST_ADD, cleanup);
        if(rc)
            return 0UL;
#if 1
        /*
         * Experimental console hack. Sets apptype to PM anyhow.
         * First Dll to be initiated should now allways be OdinCrt!
         * So include odincrt first!
         */
        PPIB pPIB;
        PTIB pTIB;
        rc = DosGetInfoBlocks(&pTIB, &pPIB);
        if (rc != NO_ERROR)
            return 0UL;
        pPIB->pib_ultype = 3;
#endif

#ifdef WITH_KLIB
        /* cleanup - hacking is done */
        DosFreeMem(pvReserved);
#endif
        break;
    }
    case 1:
        break;
    default:
        return 0UL;
    }

    /* success */
    return 1UL;
}

static void APIENTRY cleanup(ULONG /*ulReason*/)
{
    /* cleanup C++ and C runtime */
    ctordtorTerm();
    _CRT_term();
    DosExitList(EXLST_EXIT, cleanup);
    return ;
}


#elif defined(__WATCOM_CPLUSPLUS__)

int __dll_initialize(unsigned long hModule, unsigned long ulFlag)
{
#if 1
    /*
     * Experimental console hack. Sets apptype to PM anyhow.
     * First Dll to be initiated should now allways be OdinCrt!
     * So include odincrt first!
     */
    APIRET rc;
    PPIB pPIB;
    PTIB pTIB;
    rc = DosGetInfoBlocks(&pTIB, &pPIB);
    if (rc != NO_ERROR)
        return 0UL;
    pPIB->pib_ultype = 3;
#endif
    return 1;
}

int __dll_terminate(unsigned long hModule, unsigned long ulFlag)
{
    return 1;
}

#else
#error message("compiler is not supported");
#endif

