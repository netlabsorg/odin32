/* $Id: extract.c,v 1.5 2002-12-19 01:49:08 bird Exp $
 *
 * SymDB32 entry generator.
 * Buils SymDB32 entries from one ofr more symbol files.
 *
 * Copyright (c) 1999-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* Disable logging */
#define NOLOGGING 1

#define DB_ASM

#define INCL_BASE
#define INCL_DOS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_LDR

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "os2Krnl.h"                    /* must be included before dev1632.h! */
#include "krnlImportTable.h"
#include "dev1632.h"
#include "options.h"

#include <kKLkernel.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/* Statistics. */
unsigned long           cbSize;
unsigned long           cbAllSize;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int      processFile(const char *pszFilename);
extern int GetOpcodes(char * pszKrnlFile, unsigned cSymObjects); /* extract32.c */



/**
 * Dumps writes a KRNLDBENTRY struct to stderr for the given .sym-file.
 * The filesnames are on this format:
 *    nnnn[n]tm[r].SYM
 * Where: n - are the build number 4 or 5 digits.
 *        t - kernel type. R = retail, H = half strict, A = all strict.
 *        m - UNI or SMP.  U = UNI processor kernel. S = SMP processor kernel. 4 = Warp 4 FP13+
 *        r - revision letter. Currently only 'A' is supported.
 * @returns   NO_ERROR on success. Untracable error code on error.
 * @param     pszFilename  Pointer to read only filename of the .sym-file.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark    Currently only retail kernels are processed. See note below.
 */
static int processFile(const char *pszFilename)
{
    int     rc;
    int     cch;
    int     cchNum;
    const char *psz = pszFilename + strlen(pszFilename);

    /* find filename */
    cch = 0;
    while (psz >= pszFilename && *psz != '\\' && *psz != '/' && *psz != ':')
        psz--, cch++;
    psz++;
    cch--;

    /* Progress information */
    fprintf(stderr, "%s\n", pszFilename);

    /* Filename check */
    cchNum = psz[0] > '2' ? 4 : 5;      /* build number length. */
    if (cch < 10 || cch > 12
        || !(psz[0] >= '0' && psz[0] <= '9')
        || !(psz[1] >= '0' && psz[1] <= '9')
        || !(psz[2] >= '0' && psz[2] <= '9')
        || !(psz[3] >= '0' && psz[3] <= '9')
        || !(cchNum == 4 || (psz[4] >= '0' && psz[4] <= '9'))
        || !(psz[cchNum] == 'A' || psz[cchNum] == 'H' || psz[cchNum] == 'R')
        || !(psz[cchNum+1] == 'S' || psz[cchNum+1] == 'U' || psz[cchNum+1] == '4')
/*        || !(cch != 12 || psz[cchNum+2] == 'A') */
        )
    {
        fprintf(stderr, "invalid filename: %s (%s)\n", psz, pszFilename);
        return 2;
    }

    /*
     * Probe kernelfile
     */
    rc = krnlLoadKernelSymFile(pszFilename, NULL, 0);


    /*
     * on success dump a struct for this kernel
     */
    if (rc == 0)
    {
        int     i;
        char    szKrnlFile[CCHMAXPATH];
        int     cObjects;

        /*
         * Determin the total number of segments.
         */
        for (i = 0, cObjects = 0; i < NBR_OF_KRNLIMPORTS; i++)
        {
            if (aImportTab[i].fFound && (int)aImportTab[i].iObject >= cObjects)
                cObjects = aImportTab[i].iObject + 1;
        }

        /*
         * Call 32-bit helper to check if there is a kernel.
         */
        strcpy(szKrnlFile, pszFilename);
        szKrnlFile[strlen(szKrnlFile) - 4] = '\0';
        if (GetOpcodes(szKrnlFile, cObjects))
        {
            fprintf(stderr, "GetOpcodes failed: %s\n", szKrnlFile);
            return 3;
        }


        /** @remark
         * Currently information for retail kernels are usable, but we'll
         * generate it for the debug kernels too, but this information
         * is enclaved within an "#ifdef ALLKERNELS ... #endif".
         */
        if (psz[cchNum] != 'R')
            printf("ifdef ALLKERNELS\n");

        printf(";/* %s */\n"
               "  dw %.*s %*s; build no.\n",
               psz,
               cchNum, &psz[0], 4-cchNum, ""
               );

        i = 0;                      /* flags */
        switch (psz[cchNum + 1])
        {
            case 'S':   i |= KF_SMP; break;
            case '4':   i |= KF_UNI | KF_W4; break;
            case 'U':   i |= KF_UNI; break;
        }
        switch (psz[cchNum])
        {
            case 'A':   i |= KF_ALLSTRICT; break;
            case 'H':   i |= KF_HALFSTRICT; break;
        }
        if (psz[cchNum + 2] >= 'A' && psz[cchNum + 2] <= 'Z')
            i |= (psz[cchNum + 2] - 'A' + (KF_REV_A >> KF_REV_SHIFT)) << KF_REV_SHIFT;

        printf("  dw 0%04xh ; fKernel\n"
               "  db 0%2xh   ; cObjects\n",
               i,
               cObjects);

        for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
        {
            PIMPORTKRNLSYM  pEntry = &aImportTab[i];
            printf("    db 0%02xh      ; %s\n"
                   "    db 0%02xh\n"
                   "    dd 0%08lxh\n",
                   pEntry->fFound ? pEntry->iObject   : 0,
                   pEntry->achName,
                   pEntry->fFound ? pEntry->chOpcode  : OPCODE_IGNORE,
                   pEntry->fFound ? pEntry->offObject : 0xFFFFFFFFUL
                   );
        }

        /** @remark
         * We generate #ifdef ALLKERNELS for debug kernels since we usually
         * don't include symbol info in the database.
         * OLD:
         * Currently information for retail kernels are usable, but we'll
         * generate it for the debug kernels too, but this information
         * is enclaved within an "#ifdef ALLKERNELS ... #endif".
         */
        if (psz[cchNum] != 'R')
            printf("endif\n");

        /* update size */
        if (psz[cchNum] == 'R')
            cbSize += sizeof(KRNLDBENTRY);
        cbAllSize += sizeof(KRNLDBENTRY);
    }
    else
        fprintf(stderr, "krnlLoadKernelSymFile failed with rc=0x%x (%d)\n", rc, rc);

    return rc;
}


/**
 * Extract program.
 *
 * This is some initial trial-and-error for creating an "database" of
 * kernel entrypoints.
 *
 * Output to stderr the structs generated for the passed in *.sym file.
 *
 */
int main(int argc, char **argv)
{
    APIRET  rc;
    const char *  psz;
    int     i;

    /*
     * Set paKrnlOTEs to point to an zeroed array of OTEs.
     */
    cbAllSize = cbSize = 0;


    /*
     * Check name lengths.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if (strlen(aImportTab[i].achName) != (int)aImportTab[i].cchName)
        {
            fprintf(stderr,
                    "internal error - bad length of entry %d - %s. %d should be %d.\n",
                    i, aImportTab[i].achName, aImportTab[i].cchName, strlen(aImportTab[i].achName));
            return -1;
        }
    }

    /*
     * Do work.
     */
    if (argc > 1)
    {
        /*
         * Arguments: extract.exe <symfiles...>
         */
        int i;
        for (i = 1; i < argc; i++)
        {
            rc = processFile(argv[i]);
            if (rc != NO_ERROR)
            {
                fprintf(stderr, "processFile failed with rc=%d for file %s\n",
                        rc, argv[i]);
                //if (psz = GetErrorMsg(rc))
                //    fprintf(stderr, "%s\n", psz);
                return rc;
            }
        }
    }
    else
    {
        /*
         * Arguments: extract.exe
         *
         * Action:    Scan current directory for *.sym files.
         *
         */
        ULONG           cFiles = 1;
        HDIR            hDir = HDIR_CREATE;
        FILEFINDBUF3    ffb;
        int             i;

        printf(";/* $Id: extract.c,v 1.5 2002-12-19 01:49:08 bird Exp $\n"
               ";*\n"
               ";* Autogenerated kernel symbol database.\n"
               ";*\n"
               ";* Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.com)\n"
               ";*\n"
               ";* Project Odin Software License can be found in LICENSE.TXT\n"
               ";*\n"
               ";*/\n");

        printf("    .386p\n"
               "\n"
               ";\n"
               "; Include files\n"
               ";\n"
               "    include devsegdf.inc\n"
               "\n"
               "\n"
               ";\n"
               "; Exported symbols\n"
               ";\n"
               "    public aKrnlSymDB32\n"
               "\n"
               "\n");

        printf("SYMBOLDB32 segment\n"
               "aKrnlSymDB32:\n");

        rc = DosFindFirst("*.sym", &hDir, FILE_NORMAL, &ffb, sizeof(ffb), &cFiles, 0UL);
        while (rc == NO_ERROR & cFiles > 0)
        {
            rc = processFile(&ffb.achName[0]);
            if (rc != NO_ERROR)
            {
                fprintf(stderr, "processFile failed with rc=%d for file %s\n",
                        rc, &ffb.achName[0]);
                //if (psz = GetErrorMsg(rc))
                //    printf("%s\n", psz);
                return rc;
            }

            /* next file */
            rc = DosFindNext(hDir, &ffb, sizeof(ffb), &cFiles);
        }
        DosFindClose(hDir);

        printf(";/* Terminating entry */\n"
               "    db %d dup(0)\n",
               sizeof(KRNLDBENTRY));
        i = i;

        cbSize += sizeof(KRNLDBENTRY);
        cbAllSize += sizeof(KRNLDBENTRY);
    }

    printf("\n"
           ";/* cbAllSize = %ld  %ld\n"
           "; * cbSize = %ld      %ld\n"
           "; */\n"
           "SYMBOLDB32 ends\n"
           "end\n",
           cbAllSize,  cbAllSize / sizeof(KRNLDBENTRY),
           cbSize,     cbSize    / sizeof(KRNLDBENTRY));

    return rc;
}

