/* $Id: k32QueryOptionsStatus.cpp,v 1.1.2.3 2002-04-01 09:57:14 bird Exp $
 *
 * k32QueryOptionsStatus  - Queries the options and/or the status of
 *                          Win32k.sys driver.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: k32QueryOptionsStatus.cpp,v 1.1.2.3 2002-04-01 09:57:14 bird Exp $";
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define FOR_EXEHDR 1                    /* To make all object flags OBJ???. */
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS

#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_LDR
#define NO_WIN32K_LIB_FUNCTIONS
#define INCL_KKL_LOG
#define INCL_KKL_HEAP
#define INCL_KKL_AVL

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>                        /* OS/2 header file. */
#include <NEexe.h>                      /* Wine NE structs and definitions. */
#include <LXexe.h>                      /* OS/2 LX structs and definitions. */
#include <PEexe.h>                      /* Wine PE structs and definitions. */
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */

#include <string.h>                     /* C library string.h. */
#include <stdlib.h>                     /* C library stdlib.h. */
#include <stddef.h>                     /* C library stddef.h. */
#include <stdarg.h>                     /* C library stdarg.h. */

#include "dev1632.h"                    /* Common 16- and 32-bit parts */
#include "dev32.h"                      /* 32-Bit part of the device driver. (SSToDS) */
#include "ldr.h"                        /* ldr helpers. (ldrGetExePath) */
#include "env.h" //todo                       /* Environment helpers. */
#include "modulebase.h"                 /* ModuleBase class definitions, ++. */
#include "pe2lx.h"                      /* Pe2Lx class definitions, ++. */
#include <versionos2.h>                 /* Pe2Lx version. */
#include "options.h"                    /* Win32k options. */

#include "win32k.h"                     /* Win32k API structures.  */
#include "k32.h"                        /* Internal Win32k API structures.  */


/**
 * Queries the options and/or the status of Win32k.sys driver.
 * @returns     OS2 returncode.
 * @param       pOptions    Pointer to options structure. (NULL is allowed)
 * @param       pStatus     Pointer to status structure. (NULL is allowed)
 * @status      completely implelemented.
 * @author      knut st. osmundsen (bird@anduin.net)
 * @remark      One of the pointer may be NULL.
 */
APIRET k32QueryOptionsStatus(PK32OPTIONS pOptions, PK32STATUS pStatus)
{
    KLOGENTRY2("APIRET","PK32OPTIONS pOptions, PK32STATUS pStatus", pOptions, pStatus);
    APIRET  rc;
    ULONG   cb;

    /*
     * Validate parameters.
     *  Ensure that the buffer pointer is sensible.
     *  Ensure that the structure sizes are correct.
     */
    if (((ULONG)pOptions < 0x10000 && pOptions != NULL)
        ||
        ((ULONG)pStatus  < 0x10000 && pStatus  != NULL)
        ||
        (pOptions == NULL && pStatus == NULL)
        )
        rc = ERROR_INVALID_PARAMETER;

    if (pOptions != NULL)
    {
        rc = TKFuULongNF(SSToDS(&cb), &pOptions->cb);
        if (rc)
        {
            KLOGEXIT(rc);
            return rc;
        }
        if (cb != sizeof(K32OPTIONS))
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
    }

    if (pStatus != NULL)
    {
        rc = TKFuULongNF(SSToDS(&cb), &pStatus->cb);
        if (rc)
        {
            KLOGEXIT(rc);
            return rc;
        }
        if (cb != sizeof(K32STATUS))
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
    }


    /*
     * Take loader semaphore. (We might accessing LDR structures.)
     */
    rc = LDRRequestSem();
    if (rc != NO_ERROR)
    {
        kprintf(("k32QueryOptionsStatus: LDRRequestSem failed with rc = %d\n", rc));
        KLOGEXIT(rc);
        return rc;
    }

    /*
     * Get options.
     */
    if (pOptions)
    {
        K32OPTIONS  TmpOptions;

        TmpOptions.cb           = sizeof(K32OPTIONS);
        #if 0
        TmpOptions.usCom        = options.usCom;
        #endif
        TmpOptions.fLogging     = options.fLogging;
        TmpOptions.fPE          = options.fPE;
        TmpOptions.fPEOneObject = options.fPEOneObject;
        TmpOptions.ulInfoLevel  = options.ulInfoLevel;
        TmpOptions.fElf         = options.fElf;
        TmpOptions.fUNIXScript  = options.fUNIXScript;
        TmpOptions.fREXXScript  = options.fREXXScript;
        TmpOptions.fJava        = options.fJava;
        TmpOptions.fNoLoader    = options.fNoLoader;
        TmpOptions.fDllFixes    = options.fDllFixes;
        TmpOptions.fExeFixes    = options.fExeFixes;
        TmpOptions.fForcePreload= options.fForcePreload;
        TmpOptions.fApiEnh      = options.fApiEnh;
        #if 0
        TmpOptions.cbSwpHeapMax = options.cbSwpHeapMax;
        TmpOptions.cbResHeapMax = options.cbResHeapMax;
        #endif

        rc = TKSuBuff(pOptions, SSToDS(&TmpOptions), sizeof(K32OPTIONS), TK_FUSU_NONFATAL);
        if (rc != NO_ERROR) /* retry once if we fail. (For some reason it seems to work.) */
            rc = TKSuBuff(pOptions, SSToDS(&TmpOptions), sizeof(K32OPTIONS), TK_FUSU_NONFATAL);
    }

    /*
     * Get status.
     */
    if (rc == NO_ERROR && pStatus)
    {
        K32STATUS TmpStatus;
        HEAPSTATE HeapState;

        /* initate the temporary structure */
        memset(SSToDS(&TmpStatus), 0, sizeof(K32STATUS));
        TmpStatus.cb = sizeof(K32STATUS);

        /* options */
        TmpStatus.fQuiet            = options.fQuiet;
        #if 0 //todo
        TmpStatus.fKernel           = options.fKernel;              /* Smp or uni kernel. */
        TmpStatus.ulBuild           = options.ulBuild;              /* Kernel build. */
        TmpStatus.usVerMajor        = options.usVerMajor;           /* OS/2 major ver - 20 */
        TmpStatus.usVerMinor        = options.usVerMinor;           /* OS/2 minor ver - 30,40 */
        #endif

        /* swappable heap */
        if (_swp_state((PHEAPSTATE)SSToDS(&HeapState)) == 0)
        {
            #if 0
            TmpStatus.cbSwpHeapInit  = options.cbSwpHeapInit;       /* Initial heapsize. */
            #endif
            TmpStatus.cbSwpHeapFree  = HeapState.cbHeapFree;        /* Amount of used space. */
            TmpStatus.cbSwpHeapUsed  = HeapState.cbHeapUsed;        /* Amount of free space reserved. */
            TmpStatus.cbSwpHeapSize  = HeapState.cbHeapSize;        /* Amount of memory used by the heap free and used++. */
            TmpStatus.cSwpBlocksUsed = HeapState.cBlocksUsed;       /* Count of used blocks. */
            TmpStatus.cSwpBlocksFree = HeapState.cBlocksFree;       /* Count of free blocks. */
        }

        /* resident heap */
        if (_res_state((PHEAPSTATE)SSToDS(&HeapState)) == 0)
        {
            #if 0
            TmpStatus.cbResHeapInit  = options.cbResHeapInit;       /* Initial heapsize. */
            #endif
            TmpStatus.cbResHeapFree  = HeapState.cbHeapFree;        /* Amount of used space. */
            TmpStatus.cbResHeapUsed  = HeapState.cbHeapUsed;        /* Amount of free space reserved. */
            TmpStatus.cbResHeapSize  = HeapState.cbHeapSize;        /* Amount of memory used by the heap free and used++. */
            TmpStatus.cResBlocksUsed = HeapState.cBlocksUsed;       /* Count of used blocks. */
            TmpStatus.cResBlocksFree = HeapState.cBlocksFree;       /* Count of free blocks. */
        }

        /* Win32k Build and version; and symfile name or SymDB. */
        strcpy((char*)SSToDS(TmpStatus.szBuildDate), szBuildDate);  /* Date of the win32k build. */
        strcpy((char*)SSToDS(TmpStatus.szBuildTime), szBuildTime);  /* Time of the win32k build. */
        TmpStatus.ulVersion         = PE2LX_VERSION;                /* Win32k version */
        #if 0
        strcpy((char*)SSToDS(TmpStatus.szSymFile), szSymbolFile);   /* The name of the symbol file or sym database. */
        #endif

        /* Module counts */
        TmpStatus.cPe2LxModules     = Pe2Lx::getLoadedModuleCount();/* Number of Pe2Lx modules currently loaded. */
        TmpStatus.cElf2LxModules    = /*Elf2Lx::getLoadedModuleCount()*/ 0; /* Number of Elf2Lx modules currently loaded. */

        rc = TKSuBuff(pStatus, SSToDS(&TmpStatus), sizeof(K32STATUS), TK_FUSU_NONFATAL);
        if (rc != NO_ERROR) /* retry once if we fail. (For some reason it seems to work.) */
            rc = TKSuBuff(pStatus, SSToDS(&TmpStatus), sizeof(K32STATUS), TK_FUSU_NONFATAL);
    }

    /*
     * Release loader semaphore and return.
     */
    LDRClearSem();

    KLOGEXIT(rc);
    return rc;
}

