/* $Id: myldrOpen.cpp,v 1.16 2001-06-14 12:19:41 bird Exp $
 *
 * myldrOpen - ldrOpen.
 *
 * Copyright (c) 1998-2001 knut st. osmundsen <knut.stange.osmundsen@mynd.no>
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

#define INCL_OS2KRNL_IO
#define INCL_OS2KRNL_TCB
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_SEC
#define INCL_OS2KRNL_LDR

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "rmalloc.h"
#include "malloc.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "log.h"
#include "avl.h"
#include "options.h"
#include <peexe.h>
#include <exe386.h>
#include "elf.h"
#include "OS2Krnl.h"
#include "dev32.h"
#include "ldr.h"
#include "ModuleBase.h"
#include "pe2lx.h"
#include "myExecPgm.h"
#include "env.h"
#include "vprintf.h"                    /* Make 100% sure we have va_start.  */



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
/* static */ APIRET     AddArgsToFront(int cArgs,  ...);
/* static */ APIRET     SetExecName(const char *pszExeName);
/* static */ APIRET     OpenPATH(PSFN phFile, char *pszFilename, PULONG pfl);


/**
 * ldrOpen override.
 * @returns   Return code.
 * @param     phFile       Pointer to file handler. Holds filehandle on output.
 * @param     pszFilename  Pointer to filename.
 * @parma     pfl          Pointer to some flags.
 */
ULONG LDRCALL myldrOpen(PSFN phFile, PSZ pszFilename, PULONG pfl)
{
    static  int cNesting = 0;           /* This is an variable which hold the nesting */
                                        /* level of this function. This is useful     */
                                        /* when we call it recurcively.               */
                                        /* The maximum nesting level is currently 3.  */
                                        /* When the maximum depth has been reached    */
                                        /* we'll not intercept loading any longer!    */
    ULONG   rc;                         /* Return value. */

    /** @sketch
     * Try open the file (that's why this function is called)
     *   Apply Extention fix if this is requested.
     */
    if (fldrOpenExtentionFix)
    {
        int cchFilename = strlen(pszFilename);
        pszFilename[cchFilename - 4] = '\0';
        rc = ldrOpen(phFile, pszFilename, pfl);
        if (rc != NO_ERROR)
        {
            pszFilename[cchFilename - 4] = '.';
            rc = ldrOpen(phFile, pszFilename, pfl);
        }
    }
    else
        rc = ldrOpen(phFile, pszFilename, pfl);

    if (rc == NO_ERROR)
        kprintf(("myldrOpen-%d:  phFile=%#.4x, flags=%#.8x, pszFn=%s\n", cNesting, *phFile, pfl, pszFilename));


    /** @sketch
     * Are we to intercept the loading?
     * - If open were successful.
     * - And Not too deep nesting.
     * - And that this isn't an unsupported load.
     * - And one of the loaders are enabled.
     */
    if (rc == NO_ERROR
        && cNesting < 3
        && !isLdrStateLoadingUnsupported()
        && isAnyLoaderEnabled()
        )
    {
        union _u_ReadBufferPointers         /* Read buffer pointer(s). */
        {
            char               *pach;       /* Pointer to the buffer as char. */
            unsigned long      *pul;        /* Pointer to the buffer as unsigned long. */
            PIMAGE_DOS_HEADER   pMzHdr;     /* Use the buffer as a dosheader. */
            PIMAGE_NT_HEADERS   pNtHdrs;    /* Use the buffer as a NT header. */
        } u1;
        unsigned            cbFile;         /* Filesize (0xffffffff if call to SftFileSize failed - should _never_ happen though) */
        unsigned            cbRead;         /* Amount of the buffer which contains valid data. */
        char *              psz;            /* Multipurpose string pointer no.1. */
        char *              psz2;           /* Multipurpose string pointer no.2. */
        char *              psz3;           /* Multipurpose string pointer no.3. */

        /** @sketch
         * Allocate read buffer from resident heap.
         * IF this fails THEN we'll simply return NO_ERROR.
         */
        u1.pach = (char*)rmalloc(640);
        if (u1.pach == NULL)
        {
            kprintf(("myldrOpen-%d: rmalloc(640) failed\n", cNesting));
            goto ret;
        }


        /** @sketch
         * Increment nesting level.
         */
        cNesting++;


        /** @sketch
         * Get the filesize. On failure filesize is set to ~0.
         */
        rc = SftFileSize(*phFile, (PULONG)SSToDS(&cbFile));
        if (rc != NO_ERROR)
        {
            kprintf(("myldrOpen-%d: SftFileSize failed with rc=%d\n", cNesting, rc));
            cbFile = (unsigned)~0;
        }


        /** @sketch
         *  Read the size of a DOS (ie. MZ) header.
         *  IF successful and more stuff in file THEN
         *      See if this is an recognizable module binary format:
         */
        cbRead = min(sizeof(IMAGE_DOS_HEADER), cbFile);
        rc = ldrRead(*phFile, 0UL, u1.pMzHdr, 0UL, cbRead, NULL);
        if (rc == NO_ERROR && cbRead < cbFile)
        {
            /** @sketch
             * If LX header just give up at once.
             */
            if (u1.pMzHdr->e_magic == E32MAGIC)
                goto cleanup;

            /** @sketch
             * IF PE or MZ header THEN
             */
            if (u1.pMzHdr->e_magic == IMAGE_DOS_SIGNATURE
                || u1.pNtHdrs->Signature == IMAGE_NT_SIGNATURE)
            {
                ULONG   offPe;          /* Offset to PE header. */

                /** @sketch
                 * ---
                 * We now known that this is file has a MZ or a PE header. If it's
                 * a MZ header, we might end up with no "New" header or the "New"
                 * header might turn out to be a NE, LE, or LX header. I any of
                 * these non PE headers occur OS/2 will take care of it, we'll do nothing.
                 * ---
                 * IF PE loading is disable or MZ header and e_lfanew is invalid THEN
                 *     return (successfully) to the caller.
                 * ENDIF
                 * (Find the offset of the PE header while testing (offPe).)
                 */
                if (isPELoaderDisabled())
                    goto cleanup;
                if (u1.pMzHdr->e_magic == IMAGE_DOS_SIGNATURE)
                {
                    offPe = u1.pMzHdr->e_lfanew;
                    if (offPe < sizeof(IMAGE_DOS_HEADER) || offPe > 0x04000000UL)
                        goto cleanup;
                }
                else
                    offPe = 0;


                /** @sketch
                 * Read the PE header.
                 * If the read failes or not PE signature, there isn't anything for us to do.
                 */
                rc = ldrRead(*phFile, offPe, u1.pach, 0UL, sizeof(IMAGE_NT_HEADERS), NULL);
                if (rc != NO_ERROR || u1.pNtHdrs->Signature != IMAGE_NT_SIGNATURE)
                    goto cleanup_noerror;


                /** @sketch
                 * PE signature found!
                 */
                kprintf(("myldrOpen-%d: PE executable...\n", cNesting));


                /** @sketch
                 * Use Pe2Lx?
                 *  - When Pe2Lx flag is set
                 *  - When the MIXED flag is set and the image isn't an executable
                 *    above the first 64MB private limit without relocations
                 */
                if (isPe2LxLoaderEnabled()
                    || (isMixedPeLoaderEnabled()
                        && ((u1.pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL)
                            || !(u1.pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
                            || u1.pNtHdrs->OptionalHeader.ImageBase < 0x04000000UL /* 64MB */
                            )
                        )
                    )
                {   /** @sketch
                     *  Pe2Lx (Ring0 of course)
                     *      - Create a Pe2Lx class,
                     *      - initiate it
                     *      - Add the module to the module tree so we may find it later...
                     *      - Set the (file)handle state to 'our'.
                     *      - Set pExeModule to module pointer and loaderstate to our exe.
                     */
                    Pe2Lx * pPe2Lx = new Pe2Lx(*phFile);
                    if (pPe2Lx != NULL)
                    {
                        rc = pPe2Lx->init(pszFilename);
                        if (rc == NO_ERROR)
                        {
                            kprintf(("myldrOpen-%d: Successfully init of Pe2Lx object.\n", cNesting));
                            rc = addModule(*phFile, NULL, MOD_TYPE_PE2LX, pPe2Lx);
                            if (rc == NO_ERROR)
                            {
                                #pragma info(notrd)
                                SetState(*phFile, HSTATE_OUR);
                                #pragma info(restore)
                                if (pPe2Lx->isExe())
                                {
                                    setLdrStateLoadingOurEXE();
                                    pExeModule = getModuleBySFN(*phFile);
                                    #ifdef DEBUG
                                    if (pExeModule == NULL)
                                        kprintf(("myldrOpen-%d: getModuleBySFN failed when setting pExeModule! FATAL!\n", cNesting));
                                    #endif
                                }
                            }
                            else
                                kprintf(("myldrOpen-%d: Failed to add the module. rc=%d\n", cNesting));
                        }
                        else
                            kprintf(("myldrOpen-%d: Failed to init Pe2Lx object. rc=%d\n", cNesting));
                        if (rc != NO_ERROR)
                            delete pPe2Lx;
                    }
                    else
                    {
                        kprintf(("myldrOpen-%d: Failed to allocate Pe2Lx object.\n", cNesting));
                        rc = ERROR_NOT_ENOUGH_MEMORY;
                    }

                    goto cleanup;
                }


                /** @sketch
                 *  Using PE.EXE to start EXE?
                 *      - When the file is an EXE file and PE.EXE is enabled.
                 */
                if ((u1.pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0UL
                    && (options.fPE == FLAGS_PE_PE || options.fPE == FLAGS_PE_MIXED)
                    && (isLdrStateExecPgm() || isLdrStateQAppType())
                    )
                {
                    /** @sketch
                     * PE.EXE:
                     * Find pe.exe - look in current directory and thru the PATH.
                     * Note! We use the read buffer (u1.p*) as a storage for the
                     * pe.exe filename and path.
                     */
                    kprintf(("myldrOpen-%d: pe.exe - opening\n", cNesting));
                    ldrClose(*phFile);
                    *phFile = 0xFFFF;
                    strcpy(u1.pach, "PE.EXE");
                    rc = ldrOpen(phFile, u1.pach, pfl); /* This isn't recusive! */
                    if (rc != NO_ERROR)
                        rc = OpenPATH(phFile, u1.pach, pfl);
                    if (rc == NO_ERROR)
                    {
                        /** @sketch
                         * If we're in tkExecPgm state we'll have to shuffle the parameters
                         * and executable filename tkExecPgm were called with.
                         * If not tkExecPgm we can't do anything about parameters (and there is
                         * probably nothing to do either).
                         * We'll always enclose the PE executable name in quotes.
                         */
                        kprintf(("myldrOpen-%d: pe.exe - %s\n", cNesting, u1.pach));
                        if (isLdrStateExecPgm() && fTkExecPgm)
                        {
                            u1.pach[0] = '"';
                            strcpy(&u1.pach[1], achTkExecPgmFilename);
                            u1.pach[strlen(u1.pach)] = '\0';
                            rc = AddArgsToFront(2, ldrpFileNameBuf, u1.pach);
                            if (rc == NO_ERROR)
                            {
                                rc = SetExecName(ldrpFileNameBuf);
                                if (rc != NO_ERROR)
                                    kprintf(("myldrOpen-%d: pe.exe - failed to set pe.exe as execname. rc=%d\n", cNesting));
                            }
                            else
                                kprintf(("myldrOpen-%d: pe.exe - failed to add programname as argument. rc=%d\n", cNesting, rc));
                            goto cleanup_noerror;
                        }
                    }
                    else
                        kprintf(("myldrOpen-%d: pe.exe - couldn't find/open pe.exe\n", cNesting));
                }
                goto cleanup;
            }
            /** @sketch End of PE Loading. */


            /** @sketch
             * ELF image?
             */
            if (*u1.pul == ELFMAGICLSB)
            {
                if (isELFDisabled())
                    goto cleanup_noerror;

                /*
                 * ELF signature found.
                 */
                kprintf(("myldrOpen-%d: ELF image! - not implemented yet!\n", cNesting));

                /*
                 * Do nothing more yet. NEED AN ELF LOADER!!!
                 */
                goto cleanup;
            }


            /** @sketch
             * Java image?
             */
            if (*u1.pul == 0xBEBAFECAUL) //CAh FEh BAh BEh
            {
                char *pszName = NULL;
                int   cchName;

                if (isJAVADisabled())
                    goto cleanup_noerror;

                /** @sketch
                 * Java signature found.
                 * Copy the name to a temporary buffer. (only if necessary)
                 * Remove the extention (.class) and insert a space between the name and the path.
                 *  (This is the needed processing of the class filename to make it a classpath
                 *   entry (path) and a class name (filename).)
                 * Try find the java executor in current dir or PATH: java.exe
                 */
                kprintf(("myldrOpen-%d: Jave image!\n", cNesting));

                if (isLdrStateExecPgm() && fTkExecPgm)
                {
                    /* Ooops we had to get the file name from the MFT. ldrpFileNameBuf is allways uppercased... */
                    /* MFT seems to hold uppercased filenames! ARG! But (by pure luck?) achTkExecPgmArguments is
                     * not uppercased (yet). Nothing could be simpler!
                     */
                    #if 1
                    psz3 = achTkExecPgmArguments;
                    #elif 0
                    psz3 = SecPathFromSFN(*phFile);
                    if (psz3 == NULL)
                        psz3 = ldrpFileNameBuf;
                    #else
                    psz3 = ldrpFileNameBuf;
                    #endif
                    cchName = strlen(psz3);
                    pszName = (char*)rmalloc(cchName + 2);
                    if (pszName == NULL)
                    {
                        rc = ERROR_NOT_ENOUGH_MEMORY;
                        goto cleanup;
                    }
                    memcpy(pszName, psz3, cchName+1);

                    psz = pszName + strlen(pszName) - 1;
                    while (psz > pszName && *psz != '.' && *psz != '\\' && *psz != '/')
                        psz--;
                    if (*psz == '.')
                    {
                        cchName = psz - pszName;
                        *psz-- = '\0';
                        while (psz > pszName && *psz != '\\' && *psz != '/')
                            psz--;

                        /* check for root and evt. make room for an extra slash. */
                        if (psz - pszName == 2)
                        {
                            memmove(psz + 1, psz, cchName - 1);
                            *psz++ = '\\';
                        }
                    }
                    /* check if no path */
                    if (psz == pszName)
                        memmove(pszName + 1, pszName, cchName + 1);
                    *psz = ' ';
                }

                ldrClose(*phFile);
                *phFile = 0xFFFF;
                rc = ldrOpen(phFile, ".\\JAVA.EXE", pfl);
                if (rc != NO_ERROR)
                    rc = OpenPATH(phFile, "JAVA.EXE", pfl);
                if (rc == NO_ERROR)
                {
                    kprintf(("myldrOpen-%d: java - %s\n", cNesting, ldrpFileNameBuf));

                    /** @sketch
                     * To be able to execute any given class name we'll have to pass in the
                     * directory as -classpath. But -classpath seems to override the default
                     * and environmental CLASSPATHs. So, we'll have to pass in the value of
                     * the CLASSPATH env.var. or generate the default class path (what ever that is).
                     *
                     * TODO: spaces in class path.
                     */
                    if (isLdrStateExecPgm() && fTkExecPgm)
                    {
                        psz = u1.pach;

                        /*
                         * Get classpath and add it as a parameter
                         */
                        strcpy(u1.pach, "-classpath ");
                        psz = u1.pach + strlen(u1.pach);

                        psz3 = (char*)ScanEnv(GetEnv(TRUE), "CLASSPATH");
                        if (psz3 != NULL)
                        {   /* environment variable set */
                            if (strlen(psz3) > 640 - 11 - 1 - cchName) //check for overflow
                            {   // TODO? should reallocate...
                                memcpy(psz, psz3, 640 - 11 - 1 - cchName);
                                psz[640 - 11 - 1 - cchName] = '\0';
                            }
                            else
                                strcpy(psz, psz3);
                            psz += strlen(psz);
                        }
                        else
                        {
                            /* Make default classpath by taking the java.exe path + '..\lib\classes.zip' */
                            strcpy(psz, ldrpFileNameBuf);
                            psz3 = psz + strlen(psz) - 1;
                            while (psz3 > psz && *psz3 != '\\' && *psz3 != '/')
                                psz3--;
                            strcpy(++psz3, "..\\lib\\classes.zip");
                            psz = psz3 + strlen(psz3);
                        }

                        /*
                         * Add the class directory (as the last classpath entry) and the class name.
                         * (Note. I may happen that there is no directory, but that don't matter
                         *        a space is allways preceding the class name.)
                         */
                        *psz++ = ';';
                        strcpy(psz, pszName);
                        if (pszName != NULL)
                            rfree(pszName);

                        /*
                         * Setup JAVA.EXE as executable with the parameters we've build.
                         */
                        rc = AddArgsToFront(2, ldrpFileNameBuf, u1.pach);
                        kprintf(("myldrOpen-%d: java - Exe: %s  Args: %s\n", cNesting, ldrpFileNameBuf, u1.pach));
                        if (rc == NO_ERROR)
                        {
                            rc = SetExecName(ldrpFileNameBuf);
                            if (rc != NO_ERROR)
                                kprintf(("myldrOpen-%d: java - failed to set java.exe as execname. rc=%d\n", cNesting, rc));
                        }
                        else
                            kprintf(("myldrOpen-%d: java - failed to setup the parameters. rc=%d\n", cNesting, rc));

                        goto cleanup_noerror;
                    }
                }
                else
                    kprintf(("myldrOpen-%d: java - couldn't find/open java.exe\n", cNesting));


                /** @sketch
                 *  End of Java loading. (return)
                 */
                if (pszName != NULL)
                    rfree(pszName);
                goto cleanup;
            }

        }
        else
        {
            /** @sketch
             *  ELSE - the reading size of a DOS header failed or file is smaller than the dos header.
             *      IF read failed or filesize is less than 4 bytes THEN
             *          return no_error to the caller.
             *      ENDIF
             */
            #ifdef DEBUG
            if (rc != NO_ERROR)
            {
                kprintf(("myldrOpen-%d: ldrRead failed cbRead=%d, cbFile=%d,  rc=%d\n", cNesting, cbRead, cbFile, rc));
                goto cleanup_noerror;
            }
            if (cbRead < 4)
            {
                kprintf(("myldrOpen-%d: File too small! cbFile=%d\n", cNesting, cbFile));
                goto cleanup_noerror;
            }
            #else
            if (rc != NO_ERROR || cbRead < 4) //just forget files less than 4 bytes!
                goto cleanup_noerror;
            #endif
        }
        /** @sketch ENDIF (dos header read) */



        /*
         * Only unreconized files passes this point!
         *
         * * Fileformats with lower priority should reside here. *
         *
         */

        /** @sketch
         *  UNIX styled script?
         *      - Starts with a hash (#)
         *      - And we're loading an EXE
         *      - And we're either in QAppType or ExecPgm state.
         *      - And that a bang (!) is the first char after the hash (ignoring blanks).
         *
         * FIXME: spaces script name.
         */
        if (*u1.pach == '#'
            && isLdrStateLoadingEXE()
            && (isLdrStateQAppType() || isLdrStateExecPgm())
            )
        {
            if (isUNIXScriptDisabled())
                goto cleanup_noerror;
            /*
             * Look for a bang (!). Tabs and spaces are skipped, anything else result in error.
             */
            psz = u1.pach + 1;
            while ((*psz == ' ' || *psz == '\t') && psz - u1.pach < cbRead)
                psz++;
            if (*psz == '!')
            {
                /** @sketch Found UNIX styled script! */

                /** @sketch
                 * Read more of the script if necessary. (max is 256 chars (- Linux max is 127))
                 * Terminate the string read from the file to make sure with stop somewhere!
                 */
                if (cbRead < cbFile /*&& cbRead != 256*/)
                {
                    cbRead = min(256, cbFile);
                    rc = ldrRead(*phFile, 0UL, u1.pach, 0UL, cbRead, NULL);
                }
                u1.pach[cbRead] = '\0';

                if (rc == NO_ERROR)
                {
                    /** @sketch
                     * Parse out filename and optional arguments (if any).
                     * The result of the parsing is that:
                     *      psz will point at the executable name.
                     *      psz2 will point at the arguments.
                     *  Both strings are trimmed.
                     */
                    psz++;                                                  /* psz points to the bang, skip it. */
                    while (*psz == ' ' || *psz == '\t')                     /* skip blanks after bang */
                        psz++;
                    if (*psz == '\r' || *psz == '\n' || *psz == '\0')       /* End-of-line? */
                    {
                        kprintf(("myldrOpen-%d: script no executable name.\n", cNesting));
                        goto cleanup_noerror; /* other error code? */
                    }
                    psz2 = psz + 1;                                         /* Not end-of-line, so add 1 before searching for args. */
                    while (*psz2 != '\0' && *psz2 != '\n' && *psz2 != '\r'  /* skip executable name. */
                           && *psz2 != ' ' && *psz2 != '\t')
                        psz2++;
                    while (*psz2 == ' ' || *psz2 == '\t')                    /* skip blanks after executable - pad them with '\0'! */
                        *psz2++ = '\0';

                    psz3 = psz2;
                    while (*psz3 != '\n' && *psz3 != '\r' && *psz3 != '\0') /* find end of parameters and terminate the string. */
                        psz3++;
                    *psz3 = '\0';
                    while (psz3 >= psz2 && (*psz3 == '\0' || *psz3 == ' ' || *psz3 == '\t')) /* trim args */
                        *psz3-- = '\0';


                    /** @sketch
                     *  IF tkExecPgm THEN
                     *      Correct parameters - ie. add exec name (as argv[0]),
                     *        arguments (psz2) as argv[1+], old exec name, and finally
                     *        the existing parameters (current argv[1+]).
                     *      Set the executable name.
                     *  ENDIF
                     *  Open the new executable file recursively. (psz)
                     */
                    if (isLdrStateExecPgm())
                    {
                        if (*psz2)
                           rc = AddArgsToFront(3, psz, psz2, achTkExecPgmFilename);
                        else
                           rc = AddArgsToFront(2, psz, achTkExecPgmFilename);
                        if (rc != NO_ERROR)
                        {
                            kprintf(("myldrOpen-%d: AddArgsToFront failed with rc=%d\n", cNesting));
                            goto cleanup_noerror;
                        }
                        rc = SetExecName(psz);
                        if (rc != NO_ERROR)
                            kprintf(("myldrOpen-%d: SetExecName failed with rc=%d\n", cNesting));
                    }
                    ldrClose(*phFile);
                    *phFile = 0xFFFF;
                    rc = myldrOpen(phFile, psz, pfl);
                    if (rc != NO_ERROR)
                    {
                        psz2 = psz + strlen(psz);
                        if (psz + 4 >= psz2 || strcmp(psz2 - 4, ".EXE") != 0)
                        {
                            strcpy(psz2, ".EXE");
                            rc = myldrOpen(phFile, psz, pfl);
                            *psz2 = '\0';
                        }
                        else
                            psz2 = NULL;

                        //should we search the PATH??? For a starting, we'll do it.
                        if (rc != NO_ERROR
                            && (rc = OpenPATH(phFile, psz, pfl)) != NO_ERROR
                            && psz2 != NULL)
                        {
                            *psz2 = '.';
                            rc = OpenPATH(phFile, psz, pfl);
                        }
                    }
                }
                else
                {
                    kprintf(("myldrOpen-%d: script - failed to read more of the script!, rc=%d cbRead=%d cbFile=%d.\n",
                             cNesting, rc, cbRead, cbFile));
                }

                goto cleanup;
            }
            else
            {
                kprintf(("myldrOpen-%d: script - hash found but no bang (!).\n", cNesting));
            }
        } /**@sketch ENDIF - UNIX styled script. */



        /** @sketch
         *  REXX script?
         *      - Starts with a REXX start comment ('/','*')
         *      - And we're loading an EXE
         *      - And we're either in QAppType or ExecPgm state.
         *      - Extention:
         *          .RX and .REX are known to be pure REXX scripts.
         *          While .CMD has to invoked used the commandline OS2_SHELL or COMSPEC variable.
         *
         * FIXME: spaces script name.
         */
        psz2 = pszFilename + strlen(pszFilename) - 1;
        while (psz2 > pszFilename && *psz2 != '.')
            psz2--;
        if (*psz2 == '.'
            && *u1.pach == '/' && u1.pach[1] == '*'
            && isLdrStateLoadingEXE()
            && (isLdrStateQAppType() || isLdrStateExecPgm())
            && (stricmp(psz2, ".RX") == 0 || stricmp(psz2, ".REX") == 0)
            )
        {
            if (isREXXScriptDisabled())
                goto cleanup_noerror;

            /** @sketch
             * Found REXX styled script!
             * Find the REXX interpreter. We'll use kRx.exe to execute the REXX scripts.
             * (This interpreter could be embedded as a child of ModuleBase as it turned out
             * to be quite small about 700 bytes.)
             */
            kprintf(("myldrOpen-%d: Found REXX script\n", cNesting));
            ldrClose(*phFile);
            *phFile = 0xFFFF;
            psz = "KRX.EXE";
            rc = ldrOpen(phFile, psz, pfl);
            if (rc != NO_ERROR)
                rc = OpenPATH(phFile, psz, pfl);

            /** @sketch
             *  IF tkExecPgm THEN
             *      Correct parameters - ie. add exec name (as argv[0]), old exec name,
             *        and finally the existing parameters (current argv[1+]).
             *      Set the executable name.
             *  ENDIF
             */
             if (rc == NO_ERROR && isLdrStateExecPgm())
             {
                 rc = AddArgsToFront(2, ldrpFileNameBuf, achTkExecPgmFilename);
                 if (rc != NO_ERROR)
                 {
                     kprintf(("myldrOpen-%d: AddArgsToFront failed with rc=%d\n", cNesting));
                     goto cleanup_noerror;
                 }
                 rc = SetExecName(ldrpFileNameBuf);
                 if (rc != NO_ERROR)
                     kprintf(("myldrOpen-%d: SetExecName failed with rc=%d\n", cNesting));

                 goto cleanup_noerror;
             }
             goto cleanup;
        } /**@sketch ENDIF - REXX styled script. */


        /*
         * Cleanup with rc set to NO_ERROR.
         */
    cleanup_noerror:
        rc = NO_ERROR;

        /*
         * Cleanup without having rc set to NO_ERROR.
         * Decrement the nesting count.
         */
    cleanup:
        if (rc && *phFile != 0xFFFF)
        {
            ldrClose(*phFile);
            *phFile = 0xFFFF;
        }
        rfree(u1.pach);
        cNesting--;
    }
    #ifdef DEBUG
    else if (cNesting >= 3)
        kprintf(("myldrOpen-%d: cNesting = %d, which is too deep!\n", cNesting, cNesting));
    #endif

ret:
    /** @sketch
     *  If successful and force preload enabled and media flag pointer valid Then
     *      Set removable media.
     *  Return rc.
     */
    if (rc == NO_ERROR && isForcePreloadEnabled() && pfl)
        *pfl &= ~0x1000UL; /* 0x1000 is the fixed media flag. */

    return rc;
}


/**
 * Adds new arguments to the front of the startup arguments for the program about to be
 * executed.
 *
 * @returns   OS/2 return code.
 * @param     cArgs     Count of arguments to add. At least 1!!!
 * @param     ...       Pointers to the arguments to add.
 *                      The first argument have to be the executable name. This have to the
 *                        the only argument in the first string.
 *                      The other arguements are space separated, so you could add a bunch
 *                        of arguments in a single string!
 *                      The last argument should be the old first parameter if this is to be
 *                        preserved. The old first parameter is overwritten since it's
 *                        normally the executable name.
 *
 * @status    completly implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark    Implementation note:
 *  The arguments convention is as follows:
 *      First argument, which should be the executable name, is terminated with a '\0'.
 *        It starts at offset 0 into the argument buffer, of course.
 *      All other arguemnts are separated by a space and follows the immediately after the
 *        first argument.
 *      The arguments are terminated by a double nulltermination: '\0\0'.
 */
APIRET AddArgsToFront(int cArgs,  ...)
{
    va_list     vaarg;                  /* Variable length argument list. */
    int         cchOldArgs;             /* Length of the old arguments (including the first argument). */
                                        /* cchOldArgs = 1 means no arguments. It don't include the very last '\0' */
                                        /* (remember argumets are terminated with two '\0's). */
    int         iSecondArg;             /* Index of the second argument. (Used to skip the first argument.) */
                                        /* Used first on the original arguments and them when adding the first */
                                        /* new argument. */
    int         cchNewArgs;             /* Length of the new arguments to be inserted. */
    int         i;                      /* Loop variable. Current function argument. */
    char *      psz;                    /* General string pointer. */


    /** @sketch
     *  Assert that we're in the right state.
     *  Calc the length of the existing parameters.
     *  Calc the length of the new arguments to determin.
     *  Assert that the new arguments have length > 0.
     */
    #ifdef DEBUG
    if (!isLdrStateExecPgm())
    {
        kprintf(("AddArgsToFront: not in tkExecPgm state.\n"));
        return ERROR_INVALID_PARAMETER;
    }
    #endif
    if (!fTkExecPgm)
    {
        kprintf(("AddArgsToFront: called when not in tkExecPgm data is invalid!\n"));
        return ERROR_INVALID_PARAMETER;
    }

    iSecondArg = strlen(&achTkExecPgmArguments[0]) + 1;
    psz = &achTkExecPgmArguments[iSecondArg];
    while (*psz != '\0')
        psz += strlen(psz) + 1;
    cchOldArgs = psz - &achTkExecPgmArguments[iSecondArg];

    va_start(vaarg, cArgs);
    for (cchNewArgs = i = 0; i < cArgs; i++)
        cchNewArgs += strlen(va_arg(vaarg, char *)) + 1; /* 1 is for space or '\0'. */
    va_end(vaarg);
    #ifdef DEBUG
    if (cchNewArgs == 0)
    {
        kprintf(("AddArgsToFront: the size of the arguments to add is zero!\n"));
        return ERROR_INVALID_PARAMETER;
    }
    #endif


    /** @sketch
     *  Check if we have enough room for the new arguments. Fail if not enough.
     *  Move the existing arguments to make room for the new ones.
     *    !IMPORTANT! The first existing arguments (executable name) is skipped !IMPORTANT!
     *    !IMPORTANT! in this move as this have to be re-added in this call!    !IMPORTANT!
     */
    if (cchOldArgs + cchNewArgs + 1 > CCHARGUMENTS)
    {
        kprintf(("AddArgsToFront: argument buffer is too small to hold the arguments to add, cchOldArgs=%d, cchNewArgs=%d\n",
                 cchOldArgs, cchNewArgs));
        return ERROR_BAD_ARGUMENTS;
    }

    if (cchOldArgs > 0)
    {
        memmove(&achTkExecPgmArguments[cchNewArgs], &achTkExecPgmArguments[iSecondArg],
                cchOldArgs + 1);
    }
    else
        achTkExecPgmArguments[cchNewArgs] = '\0';


    /** @sketch
     *  Copy new arguments.
     *  Since the first argument is special case we'll do it separately. (Uses '\0' as separator.)
     *    We assume that the entire first argument passed into this function should be the first argument!
     *    (This don't have to be true for the other arguments since these are space separated. You could
     *     pass in more than argument in a single string.)
     *  Loop thru the rest of the new arguments and add them with space as separator.
     */
    va_start(vaarg, cArgs);
    psz = va_arg(vaarg, char *);
    memcpy(&achTkExecPgmArguments[0], psz, (i = strlen(psz) + 1));
    psz = &achTkExecPgmArguments[i];

    for (i = 1; i < cArgs; i++)
    {
        if (i > 1) *psz++ = ' ';    //Add space if not second argument.
        strcpy(psz, va_arg(vaarg, char *));
        psz += strlen(psz);
    }
    va_end(vaarg);
    if (cchOldArgs > 0) *psz++ = ' ';    //Add space if old arguments

    #ifdef DEBUG /* assertion */
    if (psz != &achTkExecPgmArguments[cchNewArgs])
    {
        kprintf(("AddArgsToFront: !Assertion failed! psz didn't end up where it should! (psz -> %d should be %d)\n",
                 psz - &achTkExecPgmArguments[0], cchNewArgs));
        if (cchOldArgs <= 1)
            psz[0] = psz[1] = '\0';
    }
    #endif

    return NO_ERROR;
}


/**
 * Sets the executable name of the module.
 * This function is normally invoked after a different executable than the one requested was
 * opened. It does _NOT_ set the new executable name as the first argument, since it is more
 * convenient to this while calling AddArgsToFront to add other arguments.
 *
 * @returns   OS/2 return code.
 * @param     pszExecName   Pointer to new executable name.
 * @status    completly implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark    .
 */
APIRET SetExecName(const char *pszExecName)
{
    #ifdef DEBUG
    int cch;
    cch = strlen(pszExecName);
    if (cch > CCHMAXPATH)
    {
        kprintf(("ChangeExecName: filename is too long! cch=%d. name=%s\n", cch, pszExecName));
        return ERROR_FILENAME_EXCED_RANGE;
    }
    if (!isLdrStateExecPgm())
    {
        kprintf(("ChangeExecName: called when not in tkExecPgm state!!! FATAL ERROR!\n"));
        return ERROR_INVALID_PARAMETER;
    }
    #endif
    if (!fTkExecPgm)
    {
        kprintf(("ChangeExecName: called when not in tkExecPgm data is invalid!!! FATAL ERROR!\n"));
        return ERROR_INVALID_PARAMETER;
    }

    strcpy(achTkExecPgmFilename, pszExecName);

    return 0;
}


/**
 * Opens a file using the PATH environment variable of the current process.
 * @returns   OS2 return code.
 * @param     phFile        Pointer to filehandle. The filehandle is set to the SFN for the opened
 *                          file on successful return.
 *                          The filehandle is 0 on failure.
 * @param     pszFilename   Pointer to filename buffer. This will hold the filename on input.
 *                          On successful return it holds the filepath found.
 *                          On failiure it's undefined.
 * @param     pfl           Some flags set by ldrOpen.
 * @sketch    stub
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
APIRET OpenPATH(PSFN phFile, char *pszFilename, PULONG pfl)
{
    APIRET      rc;
    USHORT      TCBFailErr_save;
    int         cchFile;                /* Filename length + 1. */
    const char *pszFile;                /* Pointer to filename portion. */
    const char *pszPath = ScanEnv(GetEnv(FALSE), "PATH"); /* Current Process environment? */

    /**@sketch
     *  No PATH environment.
     */
    if (pszPath == NULL)
        return ERROR_FILE_NOT_FOUND;

    /**@sketch
     * Skip any paths in the filename.
     */
    pszFile = pszFilename + (cchFile = strlen(pszFilename));
    while (pszFile >= pszFilename && *pszFile != '\\' && *pszFile != '/')
        pszFile--;
    cchFile -= pszFile - pszFilename;
    pszFile++;

    /**@sketch
     * We'll have to save the TCBFailErr since we don't want to cause
     * Hard Errors while searching invalid paths, etc. (ldrOpenPath does this!)
     */
    TCBFailErr_save = tcbGetTCBFailErr(tcbGetCur());

    /**@ sketch
     *  Loop thru the PATH trying to open the specified file in each
     *  directory.
     */
    while (*pszPath != '\0')
    {
        const char *  pszNext;
        int           cchPath;
        char          chEnd;
        register char ch;

        /*
         * Find end of this path.
         */
        while (*pszPath == ' ') pszPath++; //skip leading spaces.
        if (*pszPath == '"')
        {
            chEnd = '"';
            pszPath++;
        }
        else
            chEnd = ';';
        pszNext = pszPath;
        while ((ch = *pszNext) != chEnd && ch != '\0')
            pszNext++;

        cchPath = pszNext - pszPath;
        if (chEnd == '"')
        {
            /* Skip anything between the " and the ; or string end. */
            while ((ch = *pszNext) != ';' && ch != '\0')
                pszNext++;
        }
        else
        {
            /* Trim the string. */
            while (cchPath > 0 && pszPath[cchPath-1] == ' ') //??
                cchPath--;
        }

        /*
         *  No length? No Path! Or path'\'filename too long? => Next
         */
        if (cchPath > 0 && cchPath + cchFile + 1 < CCHMAXPATH)
        {
            static char     achFilename[CCHMAXPATH];
            /*
             * Build filename
             */
            memcpy(achFilename, pszPath, cchPath);
            if ((ch = achFilename[cchPath - 1]) == '\\' || ch == '/')
                cchPath--;
            else
                achFilename[cchPath] = '\\';
             memcpy(&achFilename[cchPath + 1], pszFile, cchFile); /* cchFile = length + 1; hence we copy the terminator too. */

            /*
             * Try open the file.
             */
            rc = myldrOpen(phFile, achFilename, pfl);
            switch (rc)
            {
                case ERROR_FILE_NOT_FOUND:          case ERROR_PATH_NOT_FOUND:          case ERROR_ACCESS_DENIED:           case ERROR_INVALID_ACCESS:
                case ERROR_INVALID_DRIVE:           case ERROR_NOT_DOS_DISK:            case ERROR_REM_NOT_LIST:            case ERROR_BAD_NETPATH:
                case ERROR_NETWORK_BUSY:            case ERROR_DEV_NOT_EXIST:           case ERROR_TOO_MANY_CMDS:           case ERROR_ADAP_HDW_ERR:
                case ERROR_UNEXP_NET_ERR:           case ERROR_BAD_REM_ADAP:            case ERROR_NETNAME_DELETED:         case ERROR_BAD_DEV_TYPE:
                case ERROR_NETWORK_ACCESS_DENIED:   case ERROR_BAD_NET_NAME:            case ERROR_TOO_MANY_SESS:           case ERROR_REQ_NOT_ACCEP:
                case ERROR_INVALID_PASSWORD:        case ERROR_OPEN_FAILED:             case ERROR_INVALID_NAME:            case ERROR_FILENAME_EXCED_RANGE:
                case ERROR_VC_DISCONNECTED:
                    break;

                case NO_ERROR:
                    strcpy(pszFilename, achFilename);
                default:
                    tcbSetTCBFailErr(tcbGetCur(), TCBFailErr_save);
                    return rc;
            }
        }
        #ifdef DEBUG
        else if (cchPath > 0)   kprintf(("OpenPATH: Path component is too long\n"));
        #endif

        /*
         * Next
         */
        if (*pszNext == '\0')
            break;
        pszPath = pszNext + 1;
    }


    /*
     * File is not found.
     */
    *phFile = 0;
    tcbSetTCBFailErr(tcbGetCur(), TCBFailErr_save);
    return ERROR_FILE_NOT_FOUND;
}
