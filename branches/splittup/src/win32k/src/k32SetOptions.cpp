/* $Id: k32SetOptions.cpp,v 1.1.2.3 2002-04-01 09:57:15 bird Exp $
 *
 * k32SetOptions - Sets the changable options of win32k.sys the options.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: k32SetOptions.cpp,v 1.1.2.3 2002-04-01 09:57:15 bird Exp $";
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
#define INCL_KKL_AVL

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>                        /* OS/2 header file. */
#include "LXexe.h"                      /* OS/2 LX structs and definitions. */
#include "PEexe.h"                      /* Wine PE structs and definitions. */
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
 * Sets the changable options of win32k.sys the options.
 * @returns     OS2 returncode.
 * @param       pOptions    Pointer to options structure. (NULL is allowed)
 * @status      completely implelemented.
 * @author      knut st. osmundsen (bird@anduin.net)
 * @remark      One of the pointer may be NULL.
 */
APIRET k32SetOptions(PK32OPTIONS pOptions)
{
    KLOGENTRY1("APIRET","PK32OPTIONS pOptions", pOptions);
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
        {
            KLOGEXIT(rc);
            return rc;
        }
        if (cb != sizeof(K32OPTIONS))
            KLOGEXIT(ERROR_INVALID_PARAMETER);
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
        #if 0
        if (    TmpOptions.usCom != OUTPUT_COM1
            &&  TmpOptions.usCom != OUTPUT_COM2
            &&  TmpOptions.usCom != OUTPUT_COM3
            &&  TmpOptions.usCom != OUTPUT_COM4)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        #endif
        if (TmpOptions.fLogging > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fPE > 4)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fPEOneObject > 2)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.ulInfoLevel > 4)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fElf > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fUNIXScript > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fREXXScript > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fJava > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fNoLoader > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fREXXScript > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fDllFixes > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fExeFixes > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fForcePreload > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.fApiEnh > 1)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        #if 0
        if (TmpOptions.cbSwpHeapMax > (32768*1024) || TmpOptions.cbSwpHeapMax < options.cbSwpHeapInit)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        if (TmpOptions.cbResHeapMax > (32768*1024) || TmpOptions.cbResHeapMax < options.cbResHeapInit)
        {
            KLOGEXIT(ERROR_INVALID_PARAMETER);
            return ERROR_INVALID_PARAMETER;
        }
        #endif

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
         * Apply changes
         */
        #if 0
        options.usCom       = TmpOptions.usCom;         /* Output port no. */
        #endif
        options.fLogging    = (USHORT)TmpOptions.fLogging;/* Logging. */
        options.fPE         = TmpOptions.fPE;           /* Flags set the type of conversion. */
        options.fPEOneObject= TmpOptions.fPEOneObject;  /* All in One Object Forces fix. */
        options.ulInfoLevel = TmpOptions.ulInfoLevel;   /* Pe2Lx InfoLevel. */
        options.fElf        = TmpOptions.fElf;          /* Elf flags. */
        options.fUNIXScript = TmpOptions.fUNIXScript;   /* UNIX script flags. */
        options.fREXXScript = TmpOptions.fREXXScript;   /* REXX script flags. */
        options.fJava       = TmpOptions.fJava;         /* Java flags. */
        options.fNoLoader   = TmpOptions.fNoLoader;     /* No loader stuff. !FIXME! We should import / functions even if this flag is set!!! */
        options.fDllFixes   = TmpOptions.fDllFixes;     /* Enables the long DLL name and non .DLL extention fixes. */
        options.fExeFixes   = TmpOptions.fExeFixes;     /* Enables EXE files to export entry points. */
        options.fForcePreload=TmpOptions.fForcePreload; /* Forces the loader to preload executable images. Handy for ICAT Ring-3 debugging. */
        options.fApiEnh     = TmpOptions.fApiEnh;       /* Enables the API enhancements */

        #if 0
        options.cbSwpHeapMax = TmpOptions.cbSwpHeapMax; /* Maximum heapsize. */
        cbSwpHeapMax = (unsigned)options.cbSwpHeapMax;
        options.cbResHeapMax = TmpOptions.cbResHeapMax; /* Maxiumem residentheapsize. */
        cbResHeapMax = (unsigned)options.cbResHeapMax;
        #endif

        /*
         * Release loader semaphore and return
         */
        LDRClearSem();
    }

    KLOGEXIT(rc);
    return rc;
}

