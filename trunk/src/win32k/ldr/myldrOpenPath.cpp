/* $Id: myldrOpenPath.cpp,v 1.4 2000-12-11 06:30:59 bird Exp $
 *
 * myldrOpenPath - ldrOpenPath used to open executables we'll override
 * this to altern the search path for DLLs.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_TCB
#define INCL_OS2KRNL_PTDA

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <memory.h>
#include <stdlib.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "log.h"
#include "dev32.h"
#include "dev32hlp.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "avl.h"
#include "ldrCalls.h"
#include "ldr.h"
#include "ModuleBase.h"
#include "options.h"


/**
 * ldrOpenPath.
 * myldrOpenPath - opens file eventually searching loader specific paths
 *
 * @returns   OS2 return code.
 *            plv->lv_sfn  is set to filename handle.
 * @param     pachFilename  Pointer to modulename. Not zero terminated!
 * @param     cchFilename   Modulename length.
 * @param     plv           Loader local variables? (Struct from KERNEL.SDF)
 * @param     pful          Pointer to flags which are passed on to ldrOpen.
 * @param     lLibPath      New parameter in build 14053()
 *                          ldrGetMte calls with 1
 *                          ldrOpenNewExe calls with 3
 *                          This is compared to the initial libpath index.
 *                              The libpath index is:
 *                                  BEGINLIBPATH    1
 *                                  LIBPATH         2
 *                                  ENDLIBPATH      3
 *                              The initial libpath index is either 1 or 2.
 *                          Currently we'll ignore it. (I don't know why ldrGetMte calls ldrOpenPath...)
 *
 * @sketch
 * This is roughly what the original ldrOpenPath does:
 *      Save pTCBCur->TCBFailErr.
 *      if !CLASS_GLOBAL or miniifs then
 *          ldrOpen(pachFilename)
 *      else
 *          if beglibpath != NULL then path = 1 else path = 2
 *          if (lLibPath < path)
 *              return ERROR_FILE_NOT_FOUND; (2)
 *          Allocate buffer.
 *          loop until no more libpath elements
 *              get next libpath element and add it to the modname.
 *              try open the modname
 *              if successfull then break the loop.
 *          endloop
 *          Free buffer.
 *      endif
 *      Restore pTCBCur->TCBFailErr.
 * @remark    This function will change the "Loader state".
 *
 */
ULONG LDRCALL myldrOpenPath(       /* retd  0x14 */
    PCHAR       pachFilename,       /* ebp + 0x08 */
    USHORT      cchFilename,        /* ebp + 0x0c */
    ldrlv_t *   plv,                /* ebp + 0x10 */
    PULONG      pful,               /* ebp + 0x14 */
    ULONG       lLibPath            /* ebp + 0x18 */
    )
{

    APIRET  rc;

    #ifdef DEBUG
    /* !paranoia!
     * Check that the passed in parameters are valid.
     * If they aren't we'll log this situation and forward the call to ldrOpenPath.
     */
    if ((unsigned)pachFilename < 0x10000
        || (unsigned)plv < 0x10000
        || cchFilename == 0
        || cchFilename >= CCHMAXPATH)
    {
        kprintf(("myldrOpenPath: Invalid parameters!!!!\n"
                 "    pachFilename 0x%08x  cchFilename 0x%04x  plv 0x%08x  pful=0x%08x\n",
                 pachFilename, cchFilename, plv, pful
                 ));
        return ldrOpenPath(pachFilename, cchFilename, plv, pful, lLibPath);
    }
    #endif


    /*
     * We'll check what type of image being opned and save that piece of
     * information for use in (my)ldrOpen.
     */
    if (plv->lv_type == LVTYPE_EXE)
        setLdrStateLoadingEXE();
    else if (plv->lv_type == LVTYPE_DLL)
        setLdrStateLoadingDLL();
    else
        setLdrStateLoadingUnsupported();


    /*
     * Check if we're to apply the extention fix in myldrOpen.
     * The required conditions are:
     *      1. Global class. (ie. DLL)
     *      2. Name must include a dot.
     *      3. The extention must not be .DLL.
     */
    fldrOpenExtentionFix =      isDllFixesEnabled()
                            &&  plv->lv_class == CLASS_GLOBAL
                            &&  memchr(pachFilename, '.', cchFilename)
                            &&  (   cchFilename < 4
                                 || memcmp(pachFilename + cchFilename - 4, ".DLL", 4));

    /*
     * Overload the behaviour of ldrOpenPath?
     *  - Currently only for DLL loading into the GLOBAL class
     */
    if (isLdrStateLoadingDLL()
        && (plv->lv_class == CLASS_GLOBAL || plv->lv_class == CLASS_ALL))
    {
        /*
         * If the executable being loaded is one of our executables we'll
         * use our method for finding dynamic link libraries.
         *
         * At tkExecPgm time this is quite easy. The executable must have been
         * opened allready and the OUREXE flag will be set if it's one of our
         * executables.
         *
         * At non-tkExecPgm time it's a bit more difficult. We'll have to get
         * the local infosegment for the process/thread running and check
         * if the executable (EXE) hMTE is one of ours. We'll do a lookup in
         * the AVL tree.
         *
         * In both cases the result will have to be a module pointer which we
         * will invoke the overriding ldrOpenPath by.
         */
        PMODULE     pExe = NULL;        /* Pointer to executable which we're to invoke ldrOpenPath by. */
        if (isLdrStateExecPgm())
        {
            if (isLdrStateLoadingOurEXE())
                pExe = pExeModule;
        }
        else
        {
            PPTDA   pPTDA = ptdaGetCur();
            if (pPTDA)
            {
                pExe = getModuleByhMTE(ptdaGet_ptda_module(pPTDA));
                #ifdef DEBUG            /* While testing! */
                kprintf(("myldrOpenPath: getModuleByhMTE returned 0x%08x for hmod=0x%04x\n",
                         pExe, ptdaGet_ptda_module(pPTDA)));
                #endif
            }
        }

        /*
         * If executable module was found the invoke the overriding ldrOpenPath function.
         */
        if (pExe != NULL)
        {
            /* We'll have to save the TCBFailErr since we don't want to cause
             * Hard Errors while searching invalid paths, etc. (ldrOpenPath does this!)
             */
            USHORT  TCBFailErr_save = tcbGetTCBFailErr(tcbGetCur());
            rc = pExe->Data.pModule->openPath(pachFilename, cchFilename, plv, pful, lLibPath);
            tcbSetTCBFailErr(tcbGetCur(), TCBFailErr_save);
        }
        else
            rc = ldrOpenPath(pachFilename, cchFilename, plv, pful, lLibPath);
    }
    else
        rc = ldrOpenPath(pachFilename, cchFilename, plv, pful, lLibPath);


    /*
     * Change Loader State - Clear the type part of the loading bits.
     */
    setLdrStateClearLoadingType();

    /*
     * Clear the extention fix flag.
     */
    fldrOpenExtentionFix = FALSE;

    return rc;
}



/**
 * Wrapper for the old (pre 14053) versions.
 * the new parameter is set to 3.
 */
ULONG LDRCALL myldrOpenPath_old( /* retd  0x10 */
    PCHAR       pachFilename,    /* ebp + 0x08 */
    USHORT      cchFilename,     /* ebp + 0x0c */
    ldrlv_t *   plv,             /* ebp + 0x10 */
    PULONG      pful             /* ebp + 0x14 */
    )
{
    return myldrOpenPath(pachFilename, cchFilename, plv, pful, 3);
}


