/* $Id: d16ProbeKrnl.c,v 1.4 2002-12-16 02:24:27 bird Exp $
 *
 * Description:   Autoprobes the os2krnl file and os2krnl[*].sym files.
 *                Another Hack!
 *
 *                16-bit inittime code.
 *
 *                All data has to be initiated because this is 16-bit C code
 *                and is to be linked with 32-bit C/C++ code. Uninitialized
 *                data ends up in the BSS segment, and that segment can't
 *                both be 32-bit and 16-bit. I have not found any other way
 *                around this problem that initiating all data.
 *
 *                How this works:
 *                1. parses the device-line parameters and collects some "SysInfo".
 *                2. gets the kernel object table and kernel info like build no. (kKrnlHlp)
 *                3. if non-debug kernel the symbol database is scanned to get the syms
 *                4. if Syms not found THEN locates and scans the symbol-file(s) for the
 *                   entrypoints which are wanted.
 *                5. the entry points are verified. (kKrnlHlp)
 *                6. finished.
 *
 * Copyright (c) 1998-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Initial pmdfvers.lst parsing:
 * Copyright (c) 2001 Rafal Szymczak (rafalszymczak@discoverfinancial.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* Disable logging when doing extracts */
#if defined(EXTRACT) || defined(MKCALLTAB) || defined(RING0)
    #define NOLOGGING 1
#endif

#define fclose(a) DosClose(a)
#define SEEK_SET FILE_BEGIN
#define SEEK_END FILE_END

#define WORD unsigned short int
#define DWORD unsigned long int

/* "@#IBM:14.039#@    os2krnl... "*/
/* "@#IBM:8.264#@    os2krnl... "*/
#define KERNEL_ID_STRING_LENGTH  42
#define KERNEL_READ_SIZE        512

#define INCL_BASE
#define INCL_DOS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_LDR

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <exe386.h>
#include <strat2.h>
#include <reqpkt.h>

#include "devSegDf.h"
#undef  DATA16_INIT
#define DATA16_INIT
#undef  CODE16_INIT
#define CODE16_INIT
#include "os2krnl.h"                    /* must be included before dev1632.h! */
#include "sym.h"
#include "ProbKrnl.h"
#include "ProbKrnlErrors.h"
#include "dev16.h"
#include "dev1632.h"
#include "kkLlog.h"
#include "options.h"
#include "kKLInitHlp.h"
#include "kKLkernel.h"
#include "d16vprintf.h"
#include "d16crt.h"


/*******************************************************************************
*   Global Variables                                                           *
*   Note: must be inited or else we'll get BSS segment                         *
*******************************************************************************/
/**
 * szSymbolFile holds the name of the symbol file used.
 *
 */
char DATA16_GLOBAL  szSymbolFile[60] = {0};

/**
 * iProc holds the number of the procedure which failed during verify.
 */
int  DATA16_GLOBAL  iProc = -1;         /* The procedure number which failed Verify. */



/*
 * private data
 */
static char *   DATA16_INIT apszSym[]       =
{
    {"c:\\os2krnl.sym"},                              /* usual for debugkernel */
    {"c:\\os2\\pdpsi\\pmdf\\warp4\\os2krnlr.sym"},    /* warp 4 */
    {"c:\\os2\\pdpsi\\pmdf\\warp4\\os2krnld.sym"},    /* warp 4 */
    {"c:\\os2\\pdpsi\\pmdf\\warp4\\os2krnlb.sym"},    /* warp 4 */
    {"c:\\os2\\pdpsi\\pmdf\\warp45_u\\os2krnlr.sym"}, /* warp 45 */
    {"c:\\os2\\pdpsi\\pmdf\\warp45_u\\os2krnld.sym"}, /* warp 45 */
    {"c:\\os2\\pdpsi\\pmdf\\warp45_u\\os2krnlb.sym"}, /* warp 45 */
    {"c:\\os2\\pdpsi\\pmdf\\warp45_s\\os2krnlr.sym"}, /* warp 45 */
    {"c:\\os2\\pdpsi\\pmdf\\warp45_s\\os2krnld.sym"}, /* warp 45 */
    {"c:\\os2\\pdpsi\\pmdf\\warp45_s\\os2krnlb.sym"}, /* warp 45 */
    {"c:\\os2\\system\\pmdf\\os2krnlr.sym"},          /* warp 3 ?*/
    {"c:\\os2\\system\\pmdf\\os2krnld.sym"},          /* warp 3 ?*/
    {"c:\\os2\\system\\pmdf\\os2krnlb.sym"},          /* warp 3 ?*/
    {"c:\\os2\\system\\trace\\os2krnl.sym"},          /* warp 3 ?*/
    {"c:\\os2krnlr.sym"},                             /* custom */
    {"c:\\os2krnlb.sym"},                             /* custom */
    {"c:\\os2krnld.sym"},                             /* custom */
    NULL
};

/* Location of PMDF list of custom directories */
static char DATA16_INIT szPmdfVers[]        =
    {"c:\\os2\\pdpsi\\pmdf\\pmdfvers.lst"};


/* Result from GetKernelInfo/ReadOS2Krnl. */
unsigned char DATA16_INIT  cObjects = 0;
POTE          DATA16_INIT  paKrnlOTEs = NULL;


/*
 * Fake data for Ring-3 testing.
 */
#ifdef R3TST
USHORT DATA16_INIT usFakeVerMajor = 0;
USHORT DATA16_INIT usFakeVerMinor = 0;
#ifdef R3TST
static DATA16_INIT ach[11] =  {0}; /* works around compiler/linker bug */
#endif
#endif

#if defined(MKCALLTAB) || defined(EXTRACT)
ULONG GetLookupKrnlEntry32Ptr(void)
{
    return 0xffffffff;
}
ULONG DATA16Base = 0xffff0000;
#endif

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
/* Workers */
int      LookupKrnlEntry(unsigned short usBuild, unsigned short fKernel, unsigned char cObjects);
int      VerifyPrologs(void);
int      ProbeSymFile(const char *pszFilename);
int      GetKernelInfo(void);

/* Ouput */
void     ShowResult(int rc);

/* Others used while debugging in R3. */
int      VerifyKernelVer(void);
int      ReadOS2Krnl(char *pszFilename);
int      ReadOS2Krnl2(HFILE hKrnl, unsigned long  cbKrnl);



/*******************************************************************************
*   Implementation of Internal Helper Functions                                *
*******************************************************************************/


/*******************************************************************************
*   Implementation Of The Important Functions                                  *
*******************************************************************************/
/**
 * Checks if this kernel is within the kernel symbol database.
 * If an entry for the kernel is found, the data is copied from the
 * database entry to aImportTab.
 * @returns   NO_ERROR on succes (0)
 *            1 if not found.
 *            Error code on error.
 * @param     usBuild       Build level.
 * @param     fKernel       Kernel (type) flags. (KF_* from options.h)
 * @param     cObjects      Count of object in the running kernel.
 * @sketch    Loop thru the table.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int LookupKrnlEntry(unsigned short usBuild, unsigned short fKernel, unsigned char cObjects)
{
#ifdef DB_16BIT
    int i;

    /*
     * Loop tru the DB entries until a NULL pointer is found.
     */
    for (i = 0; aKrnlSymDB[i].usBuild != 0; i++)
    {
        if (   aKrnlSymDB[i].usBuild  == usBuild
            && aKrnlSymDB[i].fKernel  == fKernel
            && aKrnlSymDB[i].cObjects == cObjects)
        {   /* found matching entry! */
            int j;
            int rc;
            PKRNLDBENTRY pEntry = &aKrnlSymDB[i];

            dprintf(("LookUpKrnlEntry - found entry for this kernel!\n"));
            kstrcpy(szSymbolFile, "kKrnlLib Symbol Database");

            /*
             * Copy symbol data from the DB to aImportTab.
             */
            for (j = 0; j < NBR_OF_KRNLIMPORTS; j++)
            {
                aImportTab[j].offObject  = pEntry->aSyms[j].offObject;
                aImportTab[j].iObject    = pEntry->aSyms[j].iObject;
                aImportTab[j].chOpcode   = pEntry->aSyms[j].chOpcode;
                aImportTab[j].ulAddress  = paKrnlOTEs[pEntry->aSyms[j].iObject].ote_base
                                           + pEntry->aSyms[j].offObject;
                aImportTab[j].usSel      = paKrnlOTEs[pEntry->aSyms[j].iObject].ote_sel;
                aImportTab[j].fFound     = (char)((aImportTab[j].offObject != 0xFFFFFFFFUL) ? 1 : 0);

                dprintf(("  %-3d addr=0x%08lx off=0x%08lx  %s\n",
                         j, aImportTab[j].ulAddress, aImportTab[j].offObject,
                         aImportTab[j].achName));
            }

            /* Verify prologs*/
            rc = VerifyPrologs();

            /* set sym name on success or complain on error */
            if (rc != 0)
            {
                printf16("Warning: The kKrnlLib Symbol Database entry found.\n"
                         "         But, VerifyPrologs() returned rc=0x%x and ordinal=%d\n", rc,
                         iProc < NBR_OF_KRNLIMPORTS ? aImportTab[iProc].iOrdinal : 0xffff);
                DosSleep(3000);
            }

            return rc;
        }
    }

    /* not found */
    return ERROR_PROB_SYMDB_KRNL_NOT_FOUND;
#else

    static LKE32PARAM   lke32Param;
    APIRET          rc;
    KKLR0INITPARAM  param;
    KKLR0INITDATA   data;

    lke32Param.usBuild  = usBuild;
    lke32Param.fKernel  = fKernel;
    lke32Param.cObjects = cObjects;

    param.pfn = GetLookupKrnlEntry32Ptr();
    param.ulParam = (ULONG)((USHORT)&lke32Param) + DATA16Base;
    rc = DoDevIOCtl(&param, &data);
    if (rc == NO_ERROR)
    {
        if (!data.ulRc)
            kstrcpy(szSymbolFile, "kKrnlLib Symbol Database");
        else
        {
            /* set the appropriate return values. */
            rc = (USHORT)data.ulRc;
            iProc = ((PUSHORT)&data.ulRc)[1];
        }/* else success */
    }
    /* else: complaining has been done */

    return rc;

#endif
}


/**
 * Verifies the that the addresses in aImportTab are valid.
 * This is done at Ring-0 of course.
 * @returns     NO_ERROR (ie. 0) on success. iProc = -1
 *              The appropriate OS/2 or kKrnlLib return code on success. iProc
 *              is set to the failing procedure (if appliable).
 */
int VerifyPrologs(void)
{
#if !defined(EXTRACT) && !defined(MKCALLTAB)
    APIRET          rc;
    KKLR0INITPARAM  param;
    KKLR0INITDATA   data;
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;

    /* Set the failing procedure number to -1. */
    iProc = -1;


    param.pfn = GetVerifyImportTab32Ptr();
    param.ulParam = 0;
    rc = DoDevIOCtl(&param, &data);
    if (rc == NO_ERROR)
    {
        if (data.ulRc != NO_ERROR)
        {
            /* set the appropriate return values. */
            rc = (USHORT)data.ulRc;
            iProc = ((PUSHORT)&data.ulRc)[1];
        }/* else success */
    }
    /* else: complaining has been done */

    return rc;
#else
    return 0;
#endif
}


/**
 * Check a symbol file. Searches for the wanted entry-point addresses.
 * @returns   0 on success - something else on failiure.
 * @param     pszFilename  Name of file to probe.
 * @precond   Must be called after kernel-file is found and processed.
 * @remark    Error codes starts at -50.
 */
int ProbeSymFile(const char * pszFilename)
{
    HFILE          hSym;                /* Filehandle */
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
    rc = fread(&MapDef, sizeof(MAPDEF), 1, hSym);
    if (!rc)
    {   /* oops! read failed, close file and fail. */
        fclose(hSym);
        return ERROR_PROB_SYM_READERROR;
    }
    achBuffer[0] = MapDef.achModName[0];
    fread(&achBuffer[1], 1, MapDef.cbModName, hSym);
    achBuffer[MapDef.cbModName] = '\0';
    dprintf(("*Module name: %s\n", achBuffer));
    dprintf(("*Segments: %d\n*MaxSymbolLength: %d\n", MapDef.cSegs, MapDef.cbMaxSym));
    dprintf(("*ppNextMap: 0x%x\n", MapDef.ppNextMap ));


    /*
     * Verify that the modulename of the symbol file is OS2KRNL.
     */
    if (MapDef.cbModName == 7 && kstrncmp(achBuffer, "OS2KRNL", 7) != 0)
    {   /* modulename was not OS2KRNL, fail. */
        dprintf(("Modulename verify failed\n"));
        fclose(hSym);
        return ERROR_PROB_SYM_INVALID_MOD_NAME;
    }


    /*
     * Verify that the number of segments is equal to the number objects in OS2KRNL.
     */
    #if !defined(EXTRACT) && !defined(MKCALLTAB)
    if (MapDef.cSegs != cObjects)
    {   /* incorrect count of segments. */
        dprintf(("Segment No. verify failed\n"));
        fclose(hSym);
        return ERROR_PROB_SYM_SEGS_NE_OBJS;
    }
    #endif /* !EXTRACT && !MKCALLTAB */


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
        int     fSegEPTBitType;         /* Type of segment, 16 or 32 bit, expressed in EPT_XXBIT flags */
        int     fCode;                  /* Set if this is a code segment, else clear. */

        /*
         * Read the segment definition.
         */
        if (fseek(hSym, offSegment, SEEK_SET))
        {   /* Failed to seek to the segment definition, fail! */
            fclose(hSym);
            return ERROR_PROB_SYM_SEG_DEF_SEEK;
        }
        rc = fread(&SegDef, sizeof(SEGDEF), 1, hSym);
        if (!rc)
        {   /* Failed to read the segment definition, fail! */
            fclose(hSym);
            return ERROR_PROB_SYM_SEG_DEF_READ;
        }

        /*
         * Some debugging info.
         */
        achBuffer[0] = SegDef.achSegName[0];
        fread(&achBuffer[1], 1, SegDef.cbSegName, hSym);
        achBuffer[SegDef.cbSegName] = '\0';
        dprintf(("Segment %.2li Flags=0x%02x cSymbols=0x%04x Name=%s\n",
                 iSeg, SegDef.bFlags, SegDef.cSymbols, &achBuffer[0]));

        /*
         * Determin segment bit type.
         */
        fSegEPTBitType = SEG32BitSegment(SegDef) ? EPT_32BIT : EPT_16BIT;
        fCode = kstrstr(achBuffer, "CODE") != NULL;

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
            rc = fread(&offSym, sizeof(unsigned short int), 1, hSym);
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
            if (SegDef.bFlags & 0x01)
                rc = fread(&SymDef32, sizeof(SYMDEF32), 1, hSym);
            else
                rc = fread(&SymDef16, sizeof(SYMDEF16), 1, hSym);
            if (!rc)
            {   /* Symbol read failed, try read next symbol */
                dprintf(("Warning: Read(1) failed (offSym=%d, rc=%d)\n", offSym, rc));
                continue;
            }
            achBuffer[0] = (SegDef.bFlags & 0x01) ? SymDef32.achSymName[0] : SymDef16.achSymName[0];
            cchName = (SegDef.bFlags & 0x01) ? SymDef32.cbSymName : SymDef16.cbSymName;
            rc = fread(&achBuffer[1], 1, cchName, hSym);
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
                    && kstrncmp(pImport->achName, achBuffer, cchName) == 0 /* Equal name */
                    )
                {   /* Symbol was found */
                    pImport->offObject = (SegDef.bFlags & 0x01 ? SymDef32.wSymVal : SymDef16.wSymVal);
                    pImport->ulAddress = pImport->offObject + paKrnlOTEs[iSeg].ote_base;
                    pImport->iObject   = (unsigned char)iSeg;
                    pImport->usSel     = paKrnlOTEs[iSeg].ote_sel;
                    dprintf(("debug: base=%lx, size=%lx iSeg=%d\n", paKrnlOTEs[iSeg].ote_base, paKrnlOTEs[iSeg].ote_size, iSeg));

                    /* Paranoia test! */
                    #if !defined(EXTRACT) && !defined(MKCALLTAB)
                    if (pImport->offObject < paKrnlOTEs[iSeg].ote_size)
                    {
                        pImport->fFound = TRUE;
                        cLeftToFind--;
                        dprintf(("Found: %s at off 0x%lx addr 0x%lx, sel=0x%x\n",
                                 pImport->achName, pImport->offObject,
                                 pImport->ulAddress, pImport->usSel));
                    }
                    else/* test failed, continue on next symbol*/
                        dprintf(("Error: Paranoia test failed for %s\n", pImport->achName));;
                    #else
                    pImport->fFound = TRUE;
                    cLeftToFind--;
                    #endif /* !EXTRACT */
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
     * Verify function prologs and return.
     */
    return VerifyPrologs();
}


/**
 * Get kernelinformation (OTEs (object table entries), build, type, debug...)
 * @returns 0 on success.
 *              ulKernelBuild, fchType, fDebug, cObjects and paKrnlOTEs is set on successful return.
 *          Not 0 on error.
 */
int   GetKernelInfo(void)
{
#if !defined(EXTRACT) && !defined(MKCALLTAB) /* This IOCtl is not available after inittime! */
    static KRNLINFO DATA16_INIT KrnlInfo = {0};
    APIRET          rc;
    KKLR0INITPARAM  param;
    KKLR0INITDATA   data;

    /*
     * Issue an IOCtl to kKrnlHlp to query kernel information.
     */
    param.pfn = GetGetKernelInfo32Ptr();
    param.ulParam = (ULONG)((USHORT)&KrnlInfo) + DATA16Base;
    rc = DoDevIOCtl(&param, &data);
    if (rc == NO_ERROR)
    {
        if (!data.ulRc)
        {
            #ifdef DEBUG
            unsigned i;
            #endif

            /*
             * Set the exported parameters
             */
            ulKernelBuild   = KrnlInfo.ulBuild;
            fKernel         = KrnlInfo.fKernel;
            cObjects        = KrnlInfo.cObjects;
            paKrnlOTEs      = &KrnlInfo.aObjects[0];

            /*
             * If debugging probkrnl dump kernel OTEs.
             */
            #ifdef DEBUG
            dprintf(("debug: kernel OTE:\n"));
            for (i = 0; i < cObjects; i++)
                dprintf(("debug: no.%2d base=%lx size=%lx sel=%x\n",
                         i,
                         paKrnlOTEs[i].ote_base,
                         paKrnlOTEs[i].ote_size,
                         paKrnlOTEs[i].ote_sel));
            #endif
        }
        else
            rc = (USHORT)data.ulRc;
    }
    /*else: complaining has been done */

    return rc;

#else
    return 0;
#endif
}


/**
 * Shows result of kernelprobing if not quiet or on error.
 * @param   rc      Return code.
 */
void ShowResult(int rc)
{
    int i;

    /*
     * Complain even if quiet on error
     */
    if (!options.fQuiet || rc != NO_ERROR)
    {
        printf16("\nkKrnlLib - Odin32 Ring-0 Kernel Library. (Built %s %s)\n",
                 (NPSZ)szBuildTime, (NPSZ)szBuildDate);

        /*
         * kernel stuff
         */
        printf16("    Kernel Build:     ");
        if (rc == NO_ERROR || rc > ERROR_PROB_KRNL_LAST)
            printf16("%ld - v%d.%d\n",
                     ulKernelBuild, usVerMajor, usVerMinor);
        else if (rc >= ERROR_PROB_KRNL_FIRST)
            printf16("Kernel probing failed with rc=%d.\n", rc);
        else
            printf16("Failed before probing kernel.\n");

        /*
         * symbol-file
         */
        printf16("    Using symbolfile: ");
        if (rc == NO_ERROR || (rc > ERROR_PROB_SYM_LAST && szSymbolFile[0] != '\0'))
            printf16("%s\n", (NPSZ)szSymbolFile);
        else if (rc >= ERROR_PROB_SYM_FIRST)
            printf16("Failed to find symbolfile!\n");
        else
            printf16("Failed before searching for symbolfile.\n");

        /*
         * function listing
         */
        if (options.fLogging)/* || rc != NO_ERROR)*/
        {
            for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
                if (aImportTab[i].fFound)
                    break;

            if (i < NBR_OF_KRNLIMPORTS)
            {
                for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
                {
                    printf16("  %-21s at ", aImportTab[i].achName);
                    if (aImportTab[i].fFound)
                        printf16("0x%08lx%s", aImportTab[i].ulAddress, (i % 2) == 0 ? "" : "\n");
                    else
                        printf16("not found!%s", (i % 2) == 0 ? "" : "\n");
                }
                if (i % 2) printf16("\n");
            }
            else
                printf16("Error: No functions was found!\n");
        }

        /*
         * Display error code.
         */
        if (rc != NO_ERROR)
        {
            if (iProc >= 0)
                printf16("Error: ProbeKernel failed with rc=0x%x. ordinal=%d(%s)\n",
                         rc,
                         iProc < NBR_OF_KRNLIMPORTS ? aImportTab[iProc].iOrdinal : 0xffff,
                         iProc < NBR_OF_KRNLIMPORTS ? aImportTab[iProc].achName : "");
            else
                printf16("Error: ProbeKernel failed with rc=0x%x.\n", rc);
        }
    }
}


/**
 * "main" function.
 * Note that the option -Noloader causes nothing to be done.
 * @returns   0 on success, something else on error.
 * @param     pReqPack  Pointer to init request packet
 * @remark
 */
int ProbeKernel(PRPINITIN pReqPack)
{
    int         rc;
    int         i;
    int         n;
    APIRET      rc2;
    SEL         selGIS;
    SEL         selLIS;
    PGINFOSEG   pGIS;
    PLINFOSEG   pLIS;
    USHORT      usBootDrive;
    HFILE       hPmdfVers;

    /*----------------*/
    /* parse InitArgs */
    /*----------------*/
    if (pReqPack != NULL && pReqPack->InitArgs != NULL)
    {
        n = kstrlen(pReqPack->InitArgs);
        for (i = 0; i < n; i++)
        {
            if ((pReqPack->InitArgs[i] == '/' || pReqPack->InitArgs[i] == '-') && (i+1) < n)
            {
                i++;
                switch (pReqPack->InitArgs[i])
                {
                    case 'q':
                    case 'Q': /* Quiet */
                        options.fQuiet = TRUE;
                        break;

                    case 's':
                    case 'S': /* Symbol file */
                        i++;
                        if (   pReqPack->InitArgs[i] != 'c' && pReqPack->InitArgs[i] != 'C'
                            && pReqPack->InitArgs[i] != 'm' && pReqPack->InitArgs[i] != 'M'
                            ) /* -script and -smp is ignored */
                            i += kargncpy(szSymbolFile, &pReqPack->InitArgs[i], sizeof(szSymbolFile));
                        break;

                    case 'v':
                    case 'V': /* Verbose */
                        options.fQuiet = FALSE;
                        break;
                }
            }
        }
    }

    /*---------------------*/
    /* determin boot drive */
    /*---------------------*/
    rc = DosGetInfoSeg(&selGIS, &selLIS);
    if (rc != NO_ERROR)
        return rc;

    pLIS = MAKEPLINFOSEG(selLIS);
    pGIS = MAKEPGINFOSEG(selGIS);
    usBootDrive = pGIS->bootdrive;
#ifndef R3TST
    usVerMajor  = pGIS->uchMajorVersion;
    usVerMinor  = pGIS->uchMinorVersion;
#else
    if (usFakeVerMajor == 0)
    {
        usFakeVerMajor = pGIS->uchMajorVersion;
        usFakeVerMinor = pGIS->uchMinorVersion;
    }
    usVerMajor  = usFakeVerMajor;
    usVerMinor  = usFakeVerMinor;
#endif
    dprintf(("BootDrive: %d\n", usBootDrive));

    /* set driveletter in constants strings */
    usBootDrive = (char)usBootDrive + (char)'a' - 1;
    for (i = 0; apszSym[i] != NULL; i++)
        apszSym[i][0] = (char)usBootDrive;

    /*-----------------*/
    /* get kernel info */
    /*-----------------*/
    rc = GetKernelInfo();

    /*--------------*/
    /* read symfile */
    /*--------------*/
    if (!rc)
    {
        rc = 1;
        if (szSymbolFile[0] != '\0')
        {
            rc = ProbeSymFile(szSymbolFile);
            if (rc)
            {
                printf16("Warning: Invalid symbol file specified. rc=%x ordinal=%d\n"
                         "         Tries defaults.\n",
                         rc,
                         iProc < NBR_OF_KRNLIMPORTS ? aImportTab[iProc].iOrdinal : 0xffff);
                szSymbolFile[0] = '\0';
                DosSleep(3000);
            }
        }
        if (rc != NO_ERROR) /* if user sym failed or don't exists. */
        {
            /*
             * Check database - only if not a debug kernel!
             * You usually have a .sym-file when using a debug kernel.
             */
            if ((fKernel & KF_DEBUG) ||
                (rc = LookupKrnlEntry((unsigned short)ulKernelBuild,
                                      (unsigned short)fKernel,
                                      cObjects)
                 ) != NO_ERROR
                )
            {
                /* search on disk */
                i = 0;
                while (apszSym[i] != NULL
                       && (rc2 = ProbeSymFile(apszSym[i])) != NO_ERROR
                       )
                {
                    i++;
                    if (rc2 >= ERROR_D32_FIRST)
                        rc = rc2;
                }
                if (rc2 == NO_ERROR)
                {
                    kstrcpy(szSymbolFile, apszSym[i]);
                    rc = NO_ERROR;
                }
            }
        }

        if (rc != NO_ERROR) /* if symbol file still not found. */
        {
            /*
             * Search pmdfvers.lst for custom directories.
             */
            szPmdfVers[0] = (char)usBootDrive;
            hPmdfVers = fopen(szPmdfVers, "r");
            if (hPmdfVers)
            {
                if (!feof(hPmdfVers))
                {
                    char    achBuf[CCHMAXPATH];
                    while (fgets(achBuf, sizeof(achBuf), hPmdfVers))
                    {
                        char *      pszDirTk = kstrtok(achBuf, ":;,");
                        char *      pszBuild = kstrtok(NULL, ":;,");
                        ULONG       ulPMDFBuild = 0;
                        ULONG       fPMDFKernel = 0;

                        /*
                         * Parse build number.
                         * (Note, not all kernel flags are set.)
                         */
                        if (pszBuild)
                        {
                            for (; (*pszBuild >= '0' && *pszBuild <= '9') || *pszBuild == '.'; pszBuild++)
                                if (*pszBuild != '.')
                                    ulPMDFBuild = (ulPMDFBuild * 10) + *pszBuild - '0';

                            if ((*pszBuild >= 'A' && *pszBuild <= 'Z') || (*pszBuild >= 'a' && *pszBuild <= 'z'))
                            {
                                fPMDFKernel |= (USHORT)((*pszBuild - (*pszBuild >= 'a' ? 'a'-1 : 'A'-1)) << KF_REV_SHIFT);
                                pszBuild++;
                            }
                            if (*pszBuild == ',') /* This is ignored! */
                                *pszBuild++;

                            if (pszBuild[0] == '_' && (pszBuild[1] == 'S' || pszBuild[1] == 's'))  /* _SMP  */
                                fPMDFKernel |= KF_SMP;
                            else
                                if (*pszBuild != ','
                                    && (   (pszBuild[0] == '_' && pszBuild[1] == 'W' && pszBuild[2] == '4')  /* _W4 */
                                        || (pszBuild[0] == '_' && pszBuild[1] == 'U' && pszBuild[2] == 'N' && pszBuild[3] == 'I' && pszBuild[4] == '4')  /* _UNI4 */
                                        )
                                    )
                                fPMDFKernel |= KF_W4 | KF_UNI;
                            else
                                fPMDFKernel |= KF_UNI;
                        }
                        else
                        {
                            ulPMDFBuild = ulKernelBuild;
                            fPMDFKernel = fKernel;
                        }

                        /*
                         * Consider this entry?
                         */
                        if (    pszDirTk
                            && *pszDirTk
                            &&  ulPMDFBuild == ulKernelBuild
                            &&  (fPMDFKernel & (KF_REV_MASK | KF_UNI | KF_SMP | KF_W4)) == (fKernel & (KF_REV_MASK | KF_UNI | KF_SMP | KF_W4))
                            &&  (kstrlen(pszDirTk) + 1 + sizeof(szPmdfVers)) < CCHMAXPATH /* no -13 because of os2krnl.sym is appended. */
                            )
                        {
                            char    szName[CCHMAXPATH];
                            char   *pszName;
                            kstrcpy(szName, szPmdfVers);
                            kstrcpy(&szName[sizeof(szPmdfVers) - 13], pszDirTk); /* 13 = strlen("pmdfvers.lst")+1 */
                            pszName = &szName[kstrlen(szName)];
                            *pszName++ = '\\';

                            /* search custom kernel first */
                            kstrcpy(pszName, "os2krnl.sym");
                            rc2 = ProbeSymFile(szName);
                            if (rc2 >= ERROR_D32_FIRST)
                                rc = rc2;
                            if (rc2 == NO_ERROR)
                            {
                                kstrcpy(szSymbolFile, szName);
                                rc = NO_ERROR;
                                break;
                            }

                            /* search retail kernel */
                            if (!(fKernel & KF_DEBUG))
                            {
                                kstrcpy(pszName, "os2krnlr.sym");
                                rc2 = ProbeSymFile(szName);
                                if (rc2 >= ERROR_D32_FIRST)
                                    rc = rc2;
                                if (rc2 == NO_ERROR)
                                {
                                    kstrcpy(szSymbolFile, szName);
                                    rc = NO_ERROR;
                                    break;
                                }
                            }

                            /* search allstrict kernel */
                            if (fKernel & KF_DEBUG)
                            {
                                kstrcpy(pszName, "os2krnld.sym");
                                rc2 = ProbeSymFile(szName);
                                if (rc2 >= ERROR_D32_FIRST)
                                    rc = rc2;
                                if (rc2 == NO_ERROR)
                                {
                                    kstrcpy(szSymbolFile, szName);
                                    rc = NO_ERROR;
                                    break;
                                }
                            }

                        }
                    } /* while */
                }
                fclose(hPmdfVers);
            }
        }
    }

    /* Show the result and set return-value */
    dprintf(("rc=%d(0x%x); i=%d; Ordinal=%d\n",
             rc, rc, i, iProc < NBR_OF_KRNLIMPORTS ? aImportTab[iProc].iOrdinal : 0xffff));
    ShowResult(rc);

    return rc;
}

