/* $Id: krnlLoadKernelSymFile.c,v 1.2 2002-12-19 01:49:08 bird Exp $
 *
 * Loads one kernel file into the import table.
 *
 * Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>
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

#define NOFUNCTIONNAME
#ifndef NOFILEID
static const char szFileId[] = "$Id: krnlLoadKernelSymFile.c,v 1.2 2002-12-19 01:49:08 bird Exp $";
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <kLib/kTypes.h>
#include <kLib/kLog.h>
#include <kLib/format/SYMdbg.h>
#include "krnlImportTable.h"
#include "devErrors.h"
#include <os2def.h>
#define INCL_OS2KRNL_LDR
#include "os2krnl.h"

#include <stdio.h>
#include <string.h>



/**
 * Check a symbol file. Searches for the wanted entry-point addresses.
 * @returns   0 on success - something else on failiure.
 * @param     pszFilename  Name of file to probe.
 * @precond   Must be called after kernel-file is found and processed.
 * @remark    Error codes starts at -50.
 */
int krnlLoadKernelSymFile(const char *pszFilename, POTE paOTEs, int cOTEs)
{
    //HFILE          hSym;                /* Filehandle */
    FILE*          hSym;                /* Filehandle */
    int            cLeftToFind;         /* Symbols left to find */
    unsigned long  iSeg;                /* Outer search loop:  Segment number */
    unsigned       iSym;                /* Middle search loop: Symbol number */
    unsigned       i;                   /* Inner search loop:  ProcTab index */
    int            rc;

    MAPDEF         MapDef;              /* Mapfile header */
    SEGDEF         SegDef;              /* Segment header */
    SYMDEF32       SymDef32;            /* Symbol definition 32-bit */
    SYMDEF16       SymDef16;            /* Symbol definition 16-bit */
    char           achBuffer[256];      /* Name buffer */
    unsigned long  offSegment;          /* Segment definition offset */
    unsigned long  offSymPtr;           /* Symbol pointer(offset) offset */
    unsigned short offSym;              /* Symbol definition offset */


    /*
     * Open the symbol file
     */
    hSym = fopen(pszFilename, "rb");
    if (hSym==0)
    {
        dprintf(("Error opening file %s\n", pszFilename));
        return ERROR_PROB_SYM_FILE_NOT_FOUND;
    }
    dprintf(("\nSuccessfully opened symbolfile: %s\n", pszFilename));


    /*
     * (Open were successfully.)
     * Now read header and display it.
     */
    rc = fread(SSToDS(&MapDef), sizeof(MAPDEF), 1, hSym);
    if (!rc)
    {   /* oops! read failed, close file and fail. */
        fclose(hSym);
        return ERROR_PROB_SYM_READERROR;
    }
    achBuffer[0] = MapDef.achModName[0];
    fread(SSToDS(&achBuffer[1]), 1, MapDef.cbModName, hSym);
    achBuffer[MapDef.cbModName] = '\0';
    dprintf(("*Module name: %s\n", achBuffer));
    dprintf(("*Segments:    %d\n*MaxSymbolLength: %d\n", MapDef.cSegs, MapDef.cbMaxSym));
    dprintf(("*ppNextMap:   0x%x\n", MapDef.ppNextMap ));


    /*
     * Verify that the modulename of the symbol file is OS2KRNL.
     */
    if (MapDef.cbModName == 7 && strncmp(SSToDS(&achBuffer[0]), "OS2KRNL", 7) != 0)
    {   /* modulename was not OS2KRNL, fail. */
        dprintf(("Modulename verify failed\n"));
        fclose(hSym);
        return ERROR_PROB_SYM_INVALID_MOD_NAME;
    }


    /*
     * Verify that the number of segments is equal to the number objects in OS2KRNL.
     */
    if (!cOTEs && MapDef.cSegs != cOTEs)
    {   /* incorrect count of segments. */
        dprintf(("Segment No. verify failed\n"));
        fclose(hSym);
        return ERROR_PROB_SYM_SEGS_NE_OBJS;
    }


    /*
     * Reset ProcTab
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        aImportTab[i].fFound = 0;
        #ifdef DEBUG
        aImportTab[i].iObject = 0;
        aImportTab[i].offObject = 0;
        aImportTab[i].ulAddress = 0;
        aImportTab[i].usSel = 0;
        aImportTab[i].chOpcode = OPCODE_IGNORE;
        #endif
    }


    /*
     * Fileoffset of the first segment.
     * And set cLeftToFind.
     */
    offSegment = SEGDEFOFFSET(MapDef);
    cLeftToFind = NBR_OF_KRNLIMPORTS;

    /*
     * Search thru the entire file, segment by segment.
     *
     * ASSUME: last segment is the only 32-bit code segment.
     *
     */
    for (iSeg = 0; iSeg < MapDef.cSegs; iSeg++, offSegment = NEXTSEGDEFOFFSET(SegDef))
    {
        int     fCode;                  /* Set if this is a code segment, else clear. */

        /*
         * Read the segment definition.
         */
        if (fseek(hSym, offSegment, SEEK_SET))
        {   /* Failed to seek to the segment definition, fail! */
            fclose(hSym);
            return ERROR_PROB_SYM_SEG_DEF_SEEK;
        }
        rc = fread(SSToDS(&SegDef), sizeof(SEGDEF), 1, hSym);
        if (!rc)
        {   /* Failed to read the segment definition, fail! */
            fclose(hSym);
            return ERROR_PROB_SYM_SEG_DEF_READ;
        }

        /*
         * Some debugging info.
         */
        achBuffer[0] = SegDef.achSegName[0];
        fread(SSToDS(&achBuffer[1]), 1, SegDef.cbSegName, hSym);
        achBuffer[SegDef.cbSegName] = '\0';
        dprintf(("Segment %.2li Flags=0x%02x cSymbols=0x%04x Name=%s\n",
                 iSeg, SegDef.bFlags, SegDef.cSymbols, &achBuffer[0]));

        /*
         * Code segment?
         */
        fCode = strstr(achBuffer, "CODE") != NULL;

        /*
         * Search thru all the symbols in this segment
         * while we look for the requested symbols in aImportTab.
         */
        for (iSym = 0; iSym < SegDef.cSymbols && cLeftToFind; iSym++)
        {
            IMPORTKRNLSYM * pImport;
            unsigned cchName;

            /*
             * Fileoffset of the current symbol.
             * Set filepointer to that position.
             * Read word (which is the offset of the symbol).
             */
            offSymPtr = SYMDEFOFFSET(offSegment, SegDef, iSym);
            rc = fseek(hSym, offSymPtr, SEEK_SET);
            if (rc)
            {   /* Symboloffset seek failed, try read next symbol. */
                dprintf(("Warning: Seek failed (offSymPtr=%d, rc=%d)\n", offSymPtr, rc));
                continue;
            }
            rc = fread(SSToDS(&offSym), sizeof(offSym), 1, hSym);
            if (!rc)
            {   /* Symboloffset read failed, try read next symbol. */
                dprintf(("Warning: read failed (offSymPtr=%d, rc=%d)\n", offSymPtr, rc));
                continue;
            }
            rc = fseek(hSym, offSym + offSegment, SEEK_SET);
            if (rc)
            {   /* Symbol Seek failed, try read next symbol. */
                dprintf(("Warning: Seek failed (offSym=%d, rc=%d)\n", offSym, rc));
                continue;
            }


            /*
             * Read symbol and symbolname.
             */
            if (SEG32BitSegment(SegDef))
            {
                rc = fread(SSToDS(&SymDef32), sizeof(SYMDEF32), 1, hSym);
                achBuffer[0] = SymDef32.achSymName[0];
                cchName      = SymDef32.cbSymName;
            }
            else
            {
                rc = fread(SSToDS(&SymDef16), sizeof(SYMDEF16), 1, hSym);
                achBuffer[0] = SymDef16.achSymName[0];
                cchName      = SymDef16.cbSymName;
            }
            if (!rc)
            {   /* Symbol read failed, try read next symbol */
                dprintf(("Warning: Read(1) failed (offSym=%d, rc=%d)\n", offSym, rc));
                continue;
            }
            rc = fread(SSToDS(&achBuffer[1]), 1, cchName, hSym);
            if (!rc)
            {   /* Symbol read failed, try read next symbol */
                dprintf(("Warning: Read(2) failed (offSym=%d, rc=%d)\n", offSym, rc));
                continue;
            }
            achBuffer[cchName] = '\0';


            /*
             * Search proctable.
             */
            for (i = 0, pImport = &aImportTab[0]; i < NBR_OF_KRNLIMPORTS; i++, pImport++)
            {
                if (!pImport->fFound                                       /* Not allready found */
                    && (pImport->fType & EPT_VAR ? !fCode : fCode)         /* Don't look for code in a data segment */
                    && pImport->cchName == cchName                         /* Equal name length */
                    && strncmp(pImport->achName, SSToDS(&achBuffer[0]), cchName) == 0  /* Equal name */
                    )
                {   /* Symbol was found */
                    pImport->offObject = SEG32BitSegment(SegDef) ? SymDef32.wSymVal : SymDef16.wSymVal;
                    pImport->iObject   = (unsigned char)iSeg;
                    pImport->ulAddress = pImport->offObject + (paOTEs ? paOTEs[iSeg].ote_base : 0);
                    pImport->usSel     = (unsigned short)(paOTEs ? paOTEs[iSeg].ote_sel : 0);

                    /* Paranoia test! */
                    if (!paOTEs || pImport->offObject < paOTEs[iSeg].ote_size)
                    {
                        pImport->fFound = TRUE;
                        cLeftToFind--;
                        dprintf(("Found: %s at off 0x%lx addr 0x%lx, sel=0x%x\n",
                                 pImport->achName, pImport->offObject,
                                 pImport->ulAddress, pImport->usSel));
                    }
                    else/* test failed, continue on next symbol*/
                        dprintf(("Error: Paranoia test failed for %s\n", pImport->achName));;
                    break;
                }

            } /* aImportTab for-loop */

        } /* Symbol for-loop */

    } /* Segment for-loop */

    /*
     * Close symbol file.
     */
    fclose(hSym);

    /*
     * If not all procedures were found fail.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
        if (!aImportTab[i].fFound && !EPTNotReq(aImportTab[i]))
            return ERROR_PROB_SYM_IMPORTS_NOTFOUND;

    /*
     * Symbols successfully loaded.
     */
    return 0;
}

