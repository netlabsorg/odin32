/* $Id: myldrOpen.cpp,v 1.6 2000-01-22 18:21:02 bird Exp $
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

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "malloc.h"
#include <memory.h>
#include <stdlib.h>

#include "log.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "ModuleBase.h"
#include "pe2lx.h"
#include "elf.h"
#include "avl.h"
#include "ldr.h"
#include "ldrCalls.h"
#include "options.h"


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

    rc = ldrOpen(phFile, pszFilename, param3);

    if (rc == NO_ERROR)
        kprintf(("ldrOpen:  phFile=%#.4x, flags=%#.8x, pszFn=%s\n", *phFile, param3, pszFilename));

    if (rc == NO_ERROR && (options.fElf || options.fPE != FLAGS_PE_NOT || options.fScript))
    {
        static char achBuffer[sizeof(IMAGE_DOS_HEADER)];
        PIMAGE_DOS_HEADER   pMzHdr = (PIMAGE_DOS_HEADER)&achBuffer[0];
        PIMAGE_NT_HEADERS   pNtHdrs = (PIMAGE_NT_HEADERS)&achBuffer[0]; /* oops. Last accessible field is OptionalHeader.FileAlignment */
        char               *pach = &achBuffer[0];

        /**
         * See if this is an recognizable module format.
         * This costs up to two disk reads!
         */
        rc = ldrRead(*phFile, 0UL, pMzHdr, 0UL, sizeof(IMAGE_DOS_HEADER), NULL);
        if (rc == NO_ERROR)
        {
            if ((pMzHdr->e_magic == IMAGE_DOS_SIGNATURE &&
                 pMzHdr->e_lfanew > sizeof(IMAGE_DOS_HEADER) && pMzHdr->e_lfanew < 0x04000000UL) /* Larger than 64 bytes and less that 64MB. */
                || *(PULONG)pach == IMAGE_NT_SIGNATURE)
            {   /* MZ or PE header found */
                if (options.fPE == FLAGS_PE_NOT)
                    return NO_ERROR;

                if (*(PULONG)pach != IMAGE_NT_SIGNATURE)
                    rc = ldrRead(*phFile, pMzHdr->e_lfanew, pach, 0UL, sizeof(achBuffer), NULL);

                if (rc == NO_ERROR && *(PULONG)pach == IMAGE_NT_SIGNATURE)
                {   /* PE signature found */
                    kprintf(("ldrOpen: PE executable...\n"));
                    if (options.fPE == FLAGS_PE_PE2LX
                        || (options.fPE == FLAGS_PE_MIXED
                            && !((pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL == 0UL)
                                 && pNtHdrs->OptionalHeader.ImageBase >= 0x04000000UL /* 64MB */
                                 )
                            )
                        )
                    {   /* pe2lx */
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
                        if (options.fPE == FLAGS_PE_PE || options.fPE == FLAGS_PE_MIXED)
                        {   /* pe.exe */
                            kprintf(("ldrOpen: pe.exe - opening\n"));
                            ldrClose(*phFile);
                            rc = ldrOpen(phFile, "pe.exe", param3);  /* path....! problems! */
                            kprintf(("ldrOpen: pe.exe - open returned with rc = %d\n", rc));
                            return rc;
                        }
                }
                rc = NO_ERROR;
            }
            else
            {
                if (pach[0] == ELFMAG0 && pach[1] == ELFMAG1 && pach[2] == ELFMAG2 && pach[3] == ELFMAG3)
                {
                    /* ELF signature found */
                    kprintf(("ldrOpen: ELF executable! - not implemented yet!\n"));
                }
                else
                    if (*pach == '#' && pach[1] == '!')
                    {
                        /* unix styled script...? Must be more than 64 bytes long? No options. firstline < 64 bytes. */
                        char *pszStart = pach+2;
                        char *pszEnd;
                        kprintf(("ldrOpen: unix script?\n"));

                        achBuffer[sizeof(achBuffer)-1] = '\0'; /* just to make sure we don't read to much... */

                        /* skip blanks */
                        while (*pszStart != '\0' && (*pszStart == ' ' || *pszStart == '\t'))
                            pszStart++;
                        if (*pszStart != '\0' && *pszStart != '\r' && *pszStart != '\n')
                        {   /* find end-of-word */
                            pszEnd = pszStart + 1;
                            while (*pszEnd != '\0' && *pszEnd != '\n' && *pszEnd != '\r'
                                   && *pszEnd != '\t' && *pszEnd != ' ')
                                pszEnd++;
                            if (*pszEnd != '\0')
                            {
                                *pszEnd = '\0';
                                kprintf(("ldrOpen: unix script - opening %s\n", pszStart));
                                ldrClose(*phFile);
                                rc = ldrOpen(phFile, pszStart, param3);
                                kprintf(("ldrOpen: unix script - open returned with rc = %d\n", rc));
                            }
                        }
                        else
                            kprintf(("ldrOpen: unix script - unexpected end of line/file. (line: %.10s\n", pach));
                    }
            }
        }
        else
        {
            kprintf(("ldrOpen: ldrRead failed with rc=%d when reading DosHdr.\n", rc));
            rc = NO_ERROR;
        }
    }
    return rc;
}
