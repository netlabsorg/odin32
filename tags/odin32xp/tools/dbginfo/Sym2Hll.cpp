/* $Id: Sym2Hll.cpp,v 1.7 2000-08-31 03:02:27 bird Exp $
 *
 * Sym2Hll - Symbol file to HLL debuginfo converter.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define WORD    USHORT
#define DWORD   ULONG

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include <exe386.h>

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include <kList.h>
#include <kFile.h>
#include <kFileFormatBase.h>
#include <kFileLX.h>

#include "hll.h"
#include "sym.h"
#include "kHll.h"

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void            syntax(void);
void *          readfile(const char *pszFilename);
signed long     fsize(FILE *phFile);


/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
APIRET APIENTRY DosReplaceModule (PSZ pszOldModule, PSZ pszNewModule, PSZ pszBackupModule);



int main(int argc, char **argv)
{
    kHll *      pHll;
    kFileLX *   pFileLX;

    /*
     * Quite simple input currently:
     *      <filename.sym> <filename.exe>
     */
    if (argc != 3)
    {
        syntax();
        fprintf(stderr, "syntax error\n");
        return -87;
    }

    pHll = new kHll();
    assert(pHll != NULL);

    try {
        pFileLX = new kFileLX(argv[2]);
    } catch (int errcd)
    {
        fprintf(stderr, "failed to open/read LX file (%s). errcd=%d\n", argv[2], errcd);
        return -3;
    }



    /*
     * Start conversion.
     */
    PBYTE pbSym = (PBYTE)readfile(argv[1]);
    if (pbSym != NULL)
    {
        APIRET              rc;
        kHllModuleEntry *   pModule;
        PMAPDEF             pMapDef;        /* Mapfile header */

        pMapDef = (PMAPDEF)pbSym;
        while (pMapDef != NULL)
        {
            int         iSegment;
            PSEGDEF     pSegDef;            /* Segment header */

            /*
             * Map definition.
             */
            printf("- Map definition -\n"
                   "    ppNextMap    0x%04x  paragraph pointer to next map\n"
                   "    bFlags       0x%02x    symbol types\n"
                   "    bReserved1   0x%02x    reserved\n"
                   "    pSegEntry    0x%04x  segment entry point value\n"
                   "    cConsts      0x%04x  count of constants in map\n"
                   "    pConstDef    0x%04x  pointer to constant chain\n"
                   "    cSegs        0x%04x  count of segments in map\n"
                   "    ppSegDef     0x%04x  paragraph pointer to first segment\n"
                   "    cbMaxSym     0x%02x    maximum symbol-name length\n"
                   "    cbModName    0x%02x    length of module name\n"
                   "    achModName   %.*s\n"
                   "\n",
                   pMapDef->ppNextMap,
                   pMapDef->bFlags,
                   pMapDef->bReserved1,
                   pMapDef->pSegEntry,
                   pMapDef->cConsts,
                   pMapDef->pConstDef,
                   pMapDef->cSegs,
                   pMapDef->ppSegDef,
                   pMapDef->cbMaxSym,
                   pMapDef->cbModName,
                   pMapDef->cbModName,
                   pMapDef->achModName
                   );

            /*
             * Add Modulename.
             */
            pModule = pHll->addModule(pMapDef->achModName, pMapDef->cbModName, NULL);
            if (pModule == NULL)
            {
                fprintf(stderr, "addModule failed\n");
                return -3;
            }
            pModule->getSourceEntry()->addFile(pMapDef->achModName, pMapDef->cbModName);


            /*
             * Read and convert segments with info.
             */
            pSegDef = SEGDEFPTR(pbSym, *pMapDef);
            iSegment = 1;
            while (pSegDef != NULL)
            {
                struct o32_obj *pLXObject;
                PSYMDEF32       pSymDef32;  /* Symbol definition 32-bit */
                PSYMDEF16       pSymDef16;  /* Symbol definition 16-bit */
                int             iSym;


                /*
                 * Dump Segment definition.
                 */
                printf("    - Segment Definition -\n"
                       "      ppNextSeg   0x%04x  paragraph pointer to next segment\n"
                       "      cSymbols    0x%04x  count of symbols in list\n"
                       "      pSymDef     0x%04x  offset of symbol chain\n"
                       "      wSegNum     0x%04x  segment number (1 based)\n"
                       "      wReserved2  0x%04x  reserved\n"
                       "      wReserved3  0x%04x  reserved\n"
                       "      wReserved4  0x%04x  reserved\n"
                       "      bFlags      0x%04x  symbol types\n"
                       "      bReserved1  0x%04x  reserved\n"
                       "      ppLineDef   0x%04x  offset of line number record\n"
                       "      bReserved2  0x%04x  reserved\n"
                       "      bReserved3  0x%04x  reserved\n"
                       "      cbSegName   0x%04x  length of segment name\n"
                       "      achSegName  %.*s\n",
                       pSegDef->ppNextSeg,
                       pSegDef->cSymbols,
                       pSegDef->pSymDef,
                       pSegDef->wSegNum,
                       pSegDef->wReserved2,
                       pSegDef->wReserved3,
                       pSegDef->wReserved4,
                       pSegDef->bFlags,
                       pSegDef->bReserved1,
                       pSegDef->ppLineDef ,
                       pSegDef->bReserved2,
                       pSegDef->bReserved3,
                       pSegDef->cbSegName,
                       pSegDef->cbSegName,
                       pSegDef->achSegName
                       );

                /*
                 * Add segment to the module - FIXME - need info from the LX Object table...
                 */
                pLXObject = pFileLX->getObject((USHORT)iSegment-1);
                if (pLXObject)
                {
                    if (!pModule->addSegInfo((USHORT)iSegment, 0, pLXObject->o32_size))
                        fprintf(stderr, "warning: addseginfo failed!\n");
                }
                else
                    fprintf(stderr, "warning: pFileLX->getObject failed for iSegment=%d\n",
                            iSegment);

                /*
                 * Read and convert symbols
                 */
                for (iSym = 0; iSym < pSegDef->cSymbols; iSym++)
                {
                    unsigned long   offset;
                    int             cchName;
                    const char *    pachName;
                    pSymDef32 = SYMDEFPTR32(pbSym, pSegDef, iSym);
                    pSymDef16 = (PSYMDEF16)pSymDef32;

                    if (SEG32BitSegment(*pSegDef))
                    {   /* pSymDef32 */
                        offset = pSymDef32->wSymVal;
                        cchName = pSymDef32->cbSymName;
                        pachName = pSymDef32->achSymName;
                    }
                    else
                    {   /* pSymDef16 */
                        offset = pSymDef16->wSymVal;
                        cchName = pSymDef16->cbSymName;
                        pachName = pSymDef16->achSymName;
                    }

                    printf("      0x%08x  %.*s\n",
                           offset,
                           cchName,
                           pachName);

                    /*
                     * Add symbol - currently we define it as public - it's a symbol local to this module really.
                     */
                    pModule->addPublicSymbol(pachName, cchName, offset, (USHORT)iSegment, 0);
                }


                /*
                 * Next segment
                 */
                printf("\n");
                pSegDef = NEXTSEGDEFPTR(pbSym, *pSegDef);
                iSegment++;
            }


            /*
             * Next map
             */
            pMapDef = NEXTMAPDEFPTR(pbSym, *pMapDef);
            if (pMapDef != NULL)
            {
                if (pMapDef->ppNextMap == 0)
                {   /* last map */
                    PLAST_MAPDEF pLastMapDef = (PLAST_MAPDEF)pMapDef;
                    printf("- Last Map definition -\n"
                           "    ppNextMap    0x%04x  always zero\n"
                           "    version      0x%02x    release number (minor version number)\n"
                           "    release      0x%02x    major version number\n",
                           pLastMapDef->ppNextMap,
                           pLastMapDef->release,
                           pLastMapDef->version
                           );
                    break;
                }
            }
        } /* Map loop */

        /*
         * debug
         */
        pHll->write("debug.hll");
        rc = pHll->writeToLX(argv[2]);
        if (rc == ERROR_ACCESS_DENIED)
        {

            rc = DosReplaceModule(argv[2], NULL, NULL);
            if (rc == NO_ERROR)
            {
                rc = pHll->writeToLX(argv[2]);
            }
        }
    }
    else
    {
        fprintf(stderr, "failed to open/read .sym file.\n");
        return -2;
    }






    return -1;
}


/**
 * Syntax.
 */
void syntax(void)
{
    printf("Sym2Hll.exe <symfile> <lxfile>\n");
}




/**
 * Creates a memory buffer for a binary file.
 * @returns   Pointer to file memoryblock. NULL on error.
 * @param     pszFilename  Pointer to filename string.
 * @remark    This function is the one using most of the execution
 *            time (DosRead + DosOpen) - about 70% of the execution time!
 */
void *readfile(const char *pszFilename)
{
    void *pvFile = NULL;
    FILE *phFile;

    phFile = fopen(pszFilename, "rb");
    if (phFile != NULL)
    {
        signed long cbFile = fsize(phFile);
        if (cbFile > 0)
        {
            pvFile = malloc((size_t)cbFile + 1);
            if (pvFile != NULL)
            {
                memset(pvFile, 0, (size_t)cbFile + 1);
                if (fread(pvFile, 1, (size_t)cbFile, phFile) == 0)
                {   /* failed! */
                    free(pvFile);
                    pvFile = NULL;
                }
            }
            else
                fprintf(stderr, "warning/error: failed to open file %s\n", pszFilename);
        }
        fclose(phFile);
    }
    return pvFile;
}




#ifdef __IBMCPP__
#include "klist.cpp"
#endif
