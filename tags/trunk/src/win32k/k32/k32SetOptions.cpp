/* $Id: k32SetOptions.cpp,v 1.3 2000-09-22 09:22:38 bird Exp $
 *
 * k32SetOptions - Sets the changable options of win32k.sys the options.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define FOR_EXEHDR 1                    /* To make all object flags OBJ???. */
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS

#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_SEM

#define NO_WIN32K_LIB_FUNCTIONS

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>                        /* OS/2 header file. */
#include <peexe.h>                      /* Wine PE structs and definitions. */
#include <neexe.h>                      /* Wine NE structs and definitions. */
#include <newexe.h>                     /* OS/2 NE structs and definitions. */
#include <exe386.h>                     /* OS/2 LX structs and definitions. */

#include "devSegDf.h"                   /* Win32k segment definitions. */

#include "malloc.h"                     /* win32k malloc (resident). Not C library! */
#include "smalloc.h"                    /* win32k swappable heap. */
#include "rmalloc.h"                    /* win32k resident heap. */

#include <string.h>                     /* C library string.h. */
#include <stdlib.h>                     /* C library stdlib.h. */
#include <stddef.h>                     /* C library stddef.h. */
#include <stdarg.h>                     /* C library stdarg.h. */

#include "vprintf.h"                    /* win32k printf and vprintf. Not C library! */
#include "dev1632.h"                    /* Common 16- and 32-bit parts */
#include "dev32.h"                      /* 32-Bit part of the device driver. (SSToDS) */
#include "OS2Krnl.h"                    /* kernel structs.  (SFN) */
#include "ldrCalls.h"                   /* ldr* calls. (ldrRead) */
#include "log.h"                        /* Logging. */
#include "avl.h"                        /* AVL tree. (ldr.h need it) */
#include "ldr.h"                        /* ldr helpers. (ldrGetExePath) */
#include "env.h"                        /* Environment helpers. */
#include "modulebase.h"                 /* ModuleBase class definitions, ++. */
#include "pe2lx.h"                      /* Pe2Lx class definitions, ++. */
#include <versionos2.h>                 /* Pe2Lx version. */
#include "options.h"                    /* Win32k options. */

#include "ProbKrnl.h"                   /* ProbKrnl variables and definitions. */
#include "win32k.h"                     /* Win32k API structures.  */
#include "k32.h"                        /* Internal Win32k API structures.  */


/**
 * Sets the changable options of win32k.sys the options.
 * @returns     OS2 returncode.
 * @param       pOptions    Pointer to options structure. (NULL is allowed)
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      One of the pointer may be NULL.
 */
APIRET k32SetOptions(PK32OPTIONS pOptions)
{
    APIRET  rc;
    ULONG   cb;

    /*
     * Validate parameters.
     *  Ensure that the buffer pointer is sensible.
     *  Ensure that the structure sizes are correct.
     */
    if ((ULONG)pOptions < 0x10000)
        rc = ERROR_INVALID_PARAMETER;

    if (pOptions != NULL)
    {
        rc = TKFuULongNF(SSToDS(&cb), &pOptions->cb);
        if (rc)
            return rc;
        if (cb != sizeof(K32OPTIONS))
            return ERROR_INVALID_PARAMETER;
    }


    /*
     * Get options.
     */
    K32OPTIONS  TmpOptions;

    rc = TKFuBuff(SSToDS(&TmpOptions), pOptions, sizeof(K32OPTIONS), TK_FUSU_NONFATAL);
    if (rc == NO_ERROR)
    {
        /*
         * Validate contents.
         */
        if (    TmpOptions.usCom != OUTPUT_COM1
            &&  TmpOptions.usCom != OUTPUT_COM2
            &&  TmpOptions.usCom != OUTPUT_COM3
            &&  TmpOptions.usCom != OUTPUT_COM4)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.fLogging > 1)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.fPE > 4)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.ulInfoLevel > 4)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.fElf > 1)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.fUNIXScript > 1)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.fREXXScript > 1)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.fJava > 1)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.fNoLoader > 1)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.fREXXScript > 1)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.cbSwpHeapMax > (32768*1024) || TmpOptions.cbSwpHeapMax < options.cbSwpHeapInit)
            return ERROR_INVALID_PARAMETER;
        if (TmpOptions.cbResHeapMax > (32768*1024) || TmpOptions.cbResHeapMax < options.cbResHeapInit)
            return ERROR_INVALID_PARAMETER;

        /*
         * Take loader semaphore. (We might accessing LDR structures.)
         */
        rc = LDRRequestSem();
        if (rc != NO_ERROR)
        {
            kprintf(("k32QueryOptionsStatus: LDRRequestSem failed with rc = %d\n", rc));
            return rc;
        }


        /*
         * Apply changes
         */
        options.usCom       = TmpOptions.usCom;         /* Output port no. */
        options.fLogging    = (USHORT)TmpOptions.fLogging;/* Logging. */
        options.fPE         = TmpOptions.fPE;           /* Flags set the type of conversion. */
        options.ulInfoLevel = TmpOptions.ulInfoLevel;   /* Pe2Lx InfoLevel. */
        options.fElf        = TmpOptions.fElf;          /* Elf flags. */
        options.fUNIXScript = TmpOptions.fUNIXScript;   /* UNIX script flags. */
        options.fREXXScript = TmpOptions.fREXXScript;   /* REXX script flags. */
        options.fJava       = TmpOptions.fJava;         /* Java flags. */
        options.fNoLoader   = TmpOptions.fNoLoader;     /* No loader stuff. !FIXME! We should import / functions even if this flag is set!!! */

        options.cbSwpHeapMax = TmpOptions.cbSwpHeapMax; /* Maximum heapsize. */
        cbSwpHeapMax = (unsigned)options.cbSwpHeapMax;
        options.cbResHeapMax = TmpOptions.cbResHeapMax; /* Maxiumem residentheapsize. */
        cbResHeapMax = (unsigned)options.cbResHeapMax;

        /*
         * Release loader semaphore and return
         */
        LDRClearSem();

    }

    return rc;
}

