/* $Id: myldrOpen.cpp,v 1.2 1999-10-14 01:25:38 bird Exp $
 *
 * myldrOpen - _ldrOpen.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
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
#include "pe2lx.h"
#include "ldr.h"
#include "ldrCalls.h"

#include "elf.h"

/**
 * _ldrOpen override.
 * @returns   Return code.
 * @param     phFile       Pointer to file handler. Holds filehandle on output.
 * @param     pszFilename  Pointer to filename.
 * @parma     param3       Probably some flags.
 */
ULONG LDRCALL myldrOpen(PSFN phFile, char *pszFilename, ULONG param3)
{
    ULONG rc;
    int i;

    rc = _ldrOpen(phFile, pszFilename, param3);

    if (rc == NO_ERROR)
        kprintf(("_ldrOpen:  phFile=%#.4x, flags=%#.8x, pszFn=%s\n", *phFile, param3, pszFilename));

    if (rc == NO_ERROR)
    {
        static char achBuffer[sizeof(IMAGE_DOS_HEADER)];
        PIMAGE_DOS_HEADER   pMzHdr = (PIMAGE_DOS_HEADER)&achBuffer[0];
        char               *pach = &achBuffer[0];

        /**
         * See if this is an recognizable module format.
         * This costs up to two disk reads!
         */
        rc = _ldrRead(*phFile, 0UL, pMzHdr, 0UL, sizeof(*pMzHdr), NULL);
        if (rc == NO_ERROR)
        {
            if (pMzHdr->e_magic == IMAGE_DOS_SIGNATURE &&
                pMzHdr->e_lfanew > sizeof(IMAGE_DOS_HEADER) && pMzHdr->e_lfanew < 0x04000000UL) /* Larger than 64 bytes and less that 64MB. */
            {   /* MZ header found */
                /* read */
                rc = _ldrRead(*phFile, pMzHdr->e_lfanew, pMzHdr, 0UL, 4UL, NULL);
                if (rc == NO_ERROR && *(PULONG)pach == IMAGE_NT_SIGNATURE)
                {   /* PE signature found */
                    PPENODE pNode;

                    kprintf(("_ldrOpen: PE executable!(?)\n"));
                    #pragma info(none)
                    if (/* invoke pe.exe or do conversion now? */ 1)
                    {   /* pe2lx - win32k */
                    #pragma info(restore)
                        pNode = allocateNode();
                        if (pNode != NULL)
                        {
                            pNode->pPe2Lx = new Pe2Lx(*phFile);
                            if (pNode->pPe2Lx != NULL)
                            {
                                rc = pNode->pPe2Lx->init(pszFilename);
                                if (rc == NO_ERROR)
                                {
                                    kprintf(("_ldrOpen: Successfully init of Pe2Lx object.\n"));
                                    rc = insertNode(pNode);
                                    if (rc != NO_ERROR)
                                    {
                                        kprintf(("_ldrOpen: Failed to insert PeNode into tree. rc=%d\n"));
                                        delete pNode->pPe2Lx;
                                        pNode->pPe2Lx = NULL;
                                        freeNode(pNode);
                                        SetState(*phFile, HSTATE_OUR);
                                    }
                                }
                                else
                                {
                                    kprintf(("_ldrOpen: Failed to init Pe2Lx object. rc=%d\n"));
                                    delete pNode->pPe2Lx;
                                    pNode->pPe2Lx = NULL;
                                    freeNode(pNode);
                                }
                            }
                            else
                            {
                                kprintf(("_ldrOpen: Failed to allocate Pe2Lx object.\n"));
                                freeNode(pNode);
                            }
                        }
                        else
                            kprintf(("_ldrOpen: Failed to allocate node.\n"));
                    }
                    else
                    {   /* pe.exe */
                        kprintf(("_ldrOpen: pe.exe - opening\n"));
                        _ldrClose(*phFile);
                        rc = _ldrOpen(phFile, "pe.exe", param3);  /* path....! problems! */
                        kprintf(("_ldrOpen: pe.exe - open returned with rc = %d\n", rc));
                    }
                }
                rc = NO_ERROR;
            }
            else
            {
                if (*pach == '#')
                {
                    /* unix styled script...? must be more than 64 bytes long.... */
                    char *pszStart = pach+1;
                    char *pszEnd;
                    kprintf(("_ldrOpen: unix script?\n"));
                    /* skip blanks */
                    while (*pszStart != '\0' && (*pszStart == ' ' || *pszStart == '\t'))
                        pszStart++;
                    if (*pszStart != '\0' && *pszStart != '\r' && *pszStart != '\n')
                    {   /* find end-of-word */
                        while (*pszEnd != '\0' && *pszEnd != '\n' && *pszEnd != '\r'
                               && *pszEnd != '\t' && *pszEnd != ' ')
                            pszEnd++;
                        *pszEnd = '\0';
                        kprintf(("_ldrOpen: unix script - opening %s\n", pszStart));
                        _ldrClose(*phFile);
                        rc = _ldrOpen(phFile, pszStart, param3);
                        kprintf(("_ldrOpen: unix script - open returned with rc = %d\n", rc));
                    }
                    else
                        kprintf(("_ldrOpen: unix script - unexpected end of line/file. (line: %.10s\n", pach));
                }
                else if (pach[0] == ELFMAG0 && pach[1] == ELFMAG1 && pach[2] == ELFMAG2 && pach[3] == ELFMAG3)
                {
                    /* ELF signature found */
                    kprintf(("_ldrOpen: ELF executable! - not implemented yet!\n"));
                }
            }
        }
        else
        {
            kprintf(("_ldrOpen: _ldrRead failed with rc=%d when reading DosHdr.\n", rc));
            rc = NO_ERROR;
        }
    }
    return rc;
}
