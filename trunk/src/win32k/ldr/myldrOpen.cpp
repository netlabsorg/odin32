/* $Id: myldrOpen.cpp,v 1.8 2000-02-21 04:45:47 bird Exp $
 *
 * myldrOpen - ldrOpen.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
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

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "rmalloc.h"
#include "malloc.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "dev32.h"
#include "ModuleBase.h"
#include "pe2lx.h"
#include "elf.h"
#include "avl.h"
#include "ldr.h"
#include "ldrCalls.h"
#include "options.h"
#include "myExecPgm.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static unsigned getArgsLength(const char *pachArgs);


/**
 * ldrOpen override.
 * @returns   Return code.
 * @param     phFile       Pointer to file handler. Holds filehandle on output.
 * @param     pszFilename  Pointer to filename.
 * @parma     param3       Probably some flags.
 */
ULONG LDRCALL myldrOpen(PSFN phFile, char *pszFilename, ULONG param3)
{
    ULONG rc;

    /*
     * Try open the file (thats why this function is called anyway)
     */
    rc = ldrOpen(phFile, pszFilename, param3);

    /* log sucesses */
    if (rc == NO_ERROR)
        kprintf(("ldrOpen:  phFile=%#.4x, flags=%#.8x, pszFn=%s\n", *phFile, param3, pszFilename));

    /*
     * Are we to intercept the loading?
     * - Only if open were succesful and one of the loaders are enabled.
     */
    if (rc == NO_ERROR && (options.fElf || options.fPE != FLAGS_PE_NOT || options.fScript))
    {
        char               *pszBuffer = (char*)rmalloc(640);        /* Read buffer. */
        PIMAGE_DOS_HEADER   pMzHdr = (PIMAGE_DOS_HEADER)pszBuffer;  /* Pointer to the buffer as it were a dosheader. */
        PIMAGE_NT_HEADERS   pNtHdrs = (PIMAGE_NT_HEADERS)pszBuffer; /* Pointer to the buffer as if it were an NT header. */
        char               *pach = pszBuffer;                       /* Finally an pointer to the buffer as if it were chars.. (which it is!) */
        PEXECPGMBUFFER      pBuffer;                                /* Pointer to a buffer containing the programname and arguments. */
                                                                    /* For scripts and PE.EXE this has to be changed to have correct */
                                                                    /* parameters sendt in to the program. */
        unsigned            cchRead = sizeof(IMAGE_DOS_HEADER);     /* Amount of the buffer which contains valid data. */
        unsigned            cbFile;                                 /* Filesize (0xffffffff if call to SftFileSize failed - should _never_ happen though)  */

        /*
         * Verify that rmalloc completed successfully.
         */
        if (pszBuffer == NULL)
        {
            kprintf(("ldrOpen: rmalloc(1024) failed\n"));
            return NO_ERROR;
        }

        /*
         * Try get the filesize
         */
        /*
        rc = SftFileSize(*phFile, (PULONG)SSToDS(&cbFile));
        if (rc != NO_ERROR)
        {
            kprintf(("ldrOpen: SftFileSize failed with rc=%d\n", rc));
        */
            cbFile = (unsigned)~0;
        /*
        } */

        /*
         * See if this is an recognizable module format.
         * This costs up to two disk reads!
         */
        rc = ldrRead(*phFile, 0UL, pMzHdr, 0UL, cchRead, NULL);
        if (rc == NO_ERROR)
        {
            /*
             * PE header?
             *  - If DOS Magic is found AND a valid e_lfanew (offset of NE/LX/LE/PE header) is found
             *  - OR if PE siganture is found.
             */
            if ((pMzHdr->e_magic == IMAGE_DOS_SIGNATURE &&
                 pMzHdr->e_lfanew > sizeof(IMAGE_DOS_HEADER) && pMzHdr->e_lfanew < 0x04000000UL) /* Larger than 64 bytes and less that 64MB. */
                || *(PULONG)pach == IMAGE_NT_SIGNATURE)
            {   /*
                 * MZ or PE header found
                 */

                /* if PE loading is diable return to the caller */
                if (options.fPE == FLAGS_PE_NOT)
                {
                    rfree(pszBuffer);
                    return NO_ERROR;
                }

                /*
                 * Read the PE header if it isn't what we allready have!
                 */
                cchRead = sizeof(IMAGE_NT_HEADERS);
                if (*(PULONG)pach != IMAGE_NT_SIGNATURE)
                    rc = ldrRead(*phFile, pMzHdr->e_lfanew, pach, 0UL, cchRead, NULL);
                else
                    rc = ldrRead(*phFile, 0UL, pach, 0UL, cchRead, NULL);

                /*
                 * If successfully read, and a PE signature is present the continue and try load it!
                 * Else don't do anything, simply return NO_ERROR to the caller. (probably NE or LX exec)
                 */
                if (rc == NO_ERROR && *(PULONG)pach == IMAGE_NT_SIGNATURE)
                {   /*
                     * PE signature found.
                     */
                    kprintf(("ldrOpen: PE executable...\n"));

                    /*
                     * PE2LX?
                     *  - When PE2LX flag is set
                     *  - OR when the MIXED flag is set and the image is with the first 64MB of memory.
                     */
                    if (options.fPE == FLAGS_PE_PE2LX
                        || (options.fPE == FLAGS_PE_MIXED
                            && !((pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL == 0UL)
                                 && pNtHdrs->OptionalHeader.ImageBase >= 0x04000000UL /* 64MB */
                                 )
                            )
                        )
                    {   /*
                         * Pe2Lx (Ring0 of course)
                         * - Create a Pe2Lx class,
                         * - initiate it
                         * - Add the module to the module tree so we may find it later...
                         * - Set the handle state to 'our'.
                         */
                        Pe2Lx * pPe2Lx = new Pe2Lx(*phFile);
                        if (pPe2Lx != NULL)
                        {
                            rc = pPe2Lx->init(pszFilename);
                            if (rc == NO_ERROR)
                            {
                                kprintf(("ldrOpen: Successfully init of Pe2Lx object.\n"));
                                rc = addModule(*phFile, NULL, MOD_TYPE_PE2LX, pPe2Lx);
                                if (rc == NO_ERROR)
                                    #pragma info(notrd)
                                    SetState(*phFile, HSTATE_OUR);
                                    #pragma info(restore)
                                else
                                    kprintf(("ldrOpen: Failed to add the module. rc=%d\n"));
                            }
                            else
                                kprintf(("ldrOpen: Failed to init Pe2Lx object. rc=%d\n"));
                            if (rc != NO_ERROR)
                                delete pPe2Lx;
                        }
                        else
                            kprintf(("ldrOpen: Failed to allocate Pe2Lx object.\n"));
                    }
                    else
                    {
                        /*
                         * Starting of PE.EXE enable?
                         */
                        if (options.fPE == FLAGS_PE_PE || options.fPE == FLAGS_PE_MIXED)
                        {   /*
                             * pe.exe - need the path!
                             */
                            kprintf(("ldrOpen: pe.exe - opening\n"));
                            ldrClose(*phFile);
                            rc = ldrOpen(phFile, "pe.exe", param3);  /* path....! problems! */
                            kprintf(("ldrOpen: pe.exe - open returned with rc = %d\n", rc));
                            rfree(pszBuffer);
                            return rc;
                        }
                    }
                }
                rfree(pszBuffer);
                return NO_ERROR;
            }
            else
            {
                /*
                 * ELF image?
                 */
                if (pach[0] == ELFMAG0 && pach[1] == ELFMAG1 && pach[2] == ELFMAG2 && pach[3] == ELFMAG3)
                {
                    /*
                     * ELF signature found.
                     */
                    kprintf(("ldrOpen: ELF executable! - not implemented yet!\n"));

                    /*
                     * Do nothing more yet. NEED AN ELF LOADER!!!
                     */
                    rfree(pszBuffer);
                    return NO_ERROR;
                }
            }

            /*
             * Only unreconized files and readerror passes this point!
             *
             * * Fileformats with lower priority should reside here. *
             *
             */

            /*
             * If the initial readoperation failed try to read a smaller amount, in case it is a small script...
             * 4 bytes is a small amount isn't it?
             */
            if (rc != NO_ERROR)
            {
                kprintf(("ldrOpen: first ldrread failed with rc=%d. tries to read 4 byte.\n", rc));
                cchRead = 4;
                if ((rc = ldrRead(*phFile, 0UL, pach, 0UL, cchRead, NULL)) != NO_ERROR)
                    kprintf(("ldrOpen: second ldrread failed with rc=%d.\n ", rc));
            }

            /*
             * Now we'll try again, UNIX styled script?
             */
            if (rc == NO_ERROR && *pach == '#' && pach[1] == '!')
            {
                /*
                 * UNIX styled script?
                 * FIXME! Must be more than 64 bytes long?
                 *        No options!
                 *        Firstline < 64 bytes!
                 */
                kprintf(("ldrOpen: unix script?\n"));

                cchRead = min(cbFile, 256);
                rc = ldrRead(*phFile, 0UL, pach, 0UL, cchRead, NULL);
                if (rc != NO_ERROR)
                {
                    char *pszStart = pach+2;
                    kprintf(("ldrOpen: scrtip debug 1\n"));

                    /* Make sure we don't read to much... */
                    pszBuffer[cchRead] = '\0';

                    /*
                     * Skip blanks
                     */
                    pszStart = pszBuffer + 2; /* skips the "#!" stuff. */
                    while (*pszStart != '\0' && (*pszStart == ' ' || *pszStart == '\t'))
                        pszStart++;
                    kprintf(("ldrOpen: scrtip debug 2\n"));

                    /* anything left on the line? */
                    if (*pszStart != '\0' && *pszStart != '\r' && *pszStart != '\n')
                    {
                        char *      pszEnd;         /* Pointer to the end of the string(s) when the next step is finished. */
                        //char *      pszFirstArg;    /* Pointer to the first argument, the one after the interpreter name. */
                        unsigned    cchToAdd = 1;   /* Chars to add */
                        int         f = TRUE;       /* flag which tells me if I have just closed the last argument.  */
                        /*
                         * find linesize and make parameters ready for copying
                         */
                        pszEnd = pszStart;
                        kprintf(("ldrOpen: scrtip debug 3\n"));
                        //pszFirstArg = NULL;
                        while (*pszEnd != '\0' && *pszEnd != '\r' && *pszEnd != '\n')
                        {
                            if (f)
                            {
                                f = FALSE;
                                //if (pszFirstArg != NULL) pszFirstArg = pszEnd + 1;
                            }
                            else if (!f && (*pszEnd == ' ' || *pszEnd == '\t'))
                            {
                                 *pszEnd = '\0';
                                 f = TRUE;
                            }

                            /* next */
                            pszEnd++;
                            cchToAdd++;
                        }
                        *pszEnd = '\0';
                        kprintf(("ldrOpen: scrtip debug 4\n"));

                        /*
                         * Find the ExecPgm buffer.
                         */
                        pBuffer = QueryBufferPointerFromFilename(pszFilename);
                        kprintf(("ldrOpen: scrtip debug 5\n"));
                        if (pBuffer != NULL)
                        {
                            unsigned cchArguments = getArgsLength(pBuffer->achArgs);
                            kprintf(("ldrOpen: debug1\n"));

                            kprintf(("ldrOpen: scrtip debug 6\n"));

                            /*
                             * Is there enough space in the struct?
                             */
                            if (cchArguments + cchToAdd < sizeof(pBuffer->achArgs))
                            {
                                kprintf(("ldrOpen: scrtip debug 7\n"));
                                /*
                                 * Open the interpreter.
                                 */
                                rc = ldrClose(*phFile);
                                rc = ldrOpen(phFile, pszStart, param3); /* FIXME, recusion! check that name not equal! Use flags to prevent race? */
                                if (rc == NO_ERROR)
                                {
                                    kprintf(("ldrOpen: scritp debug 8\n"));
                                    /* Make space for the addition arguments. */
                                    memmove(&pBuffer->achArgs[cchToAdd], &pBuffer->achArgs[0], cchArguments);
                                    memcpy(&pBuffer->achArgs[0], pszBuffer, cchToAdd);
                                    kprintf(("ldrOpen: scrtip debug 9\n"));
                                }
                                else
                                    kprintf(("ldrOpen: failed to open interpreter (%s), rc=%d\n", pszStart, rc));
                            }
                            else
                            {
                                kprintf(("ldrOpen: Argument buffer too small, %d\n", cchArguments + cchToAdd));
                                rc = ERROR_BAD_EXE_FORMAT;
                            }
                        }
                        else
                        {
                            kprintf(("ldrOpen: QueryBufferPointerFromFilename failed.\n"));
                            rc = ERROR_BAD_EXE_FORMAT; /*?*/
                        }
                    }
                    else
                    {
                        kprintf(("ldrOpen: no interpereter on the first line.\n"));
                        rc = ERROR_BAD_EXE_FORMAT; /*?*/
                    }
                }
                else
                {
                    kprintf(("ldrOpen: read of min(cbFile, 256) = %d failed, rc = %d\n", cchRead, rc));
                }
            } /* else inn other formats here. */
        }
        else
        {
            kprintf(("ldrOpen: ldrRead failed with rc=%d when reading DosHdr.\n", rc));
            rc = NO_ERROR;
        }
        rfree(pszBuffer);
    }
    return rc;
}


/**
 * Get the lenght of the arguments.
 * @returns   Lenght in char, includes the two '\0's.
 * @param     pachArgs  Pointer to the ASCIIZs which makes up the arguments.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static unsigned getArgsLength(const char *pachArgs)
{
    unsigned    cch = 1;
    const char *psz = pachArgs;

    while (*psz != '\0')
    {
        register unsigned cch2 = strlen(psz);
        cch += cch2;
        psz += cch2 + 1;
    }

    return cch;
}
