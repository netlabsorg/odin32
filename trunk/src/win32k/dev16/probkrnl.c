/* $Id: probkrnl.c,v 1.16 2000-02-25 18:15:02 bird Exp $
 *
 * Description:   Autoprobes the os2krnl file and os2krnl[*].sym files.
 *                Another Hack!
 *
 *                16-bit inittime code.
 *
 *                All data has to be initiated because this is 16-bit C code
 *                and is to be linked with 32-bit C/C++ code. Uninitiazlied
 *                data ends up in the BSS segment, and that segment can't
 *                both be 32-bit and 16-bit. I have not found any other way
 *                around this problem that initiating all data.
 *
 *                How this works:
 *                1. parses the device-line parameters and collects some "SysInfo".
 *                2. gets the kernel object table. (elf$)
 *                3. finds the kernel image and scans it for a build number.
 *                4. locates and scans the symbol-file(s) for the entrypoints which are wanted.
 *                5. the entry points are verified. (elf$)
 *                6. finished.
 *
 * Copyright (c) 1998-2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* Disable logging when doing extracts */
#if defined(EXTRACT) || defined(RING0)
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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <exe386.h>
#include <strat2.h>
#include <reqpkt.h>

#include "os2krnl.h"                    /* must be included before dev1632.h! */
#include "sym.h"
#include "probkrnl.h"
#include "dev16.h"
#include "dev1632.h"
#include "vprntf16.h"
#include "log.h"


/*******************************************************************************
*   Global Variables                                                           *
*   Note: must be inited or else we'll get BSS segment                         *
*******************************************************************************/
/*
 * kernel data - !only valid during init!
 */

/*
 * aImportTab defines the imported and overloaded OS/2 kernel functions.
 * IMPORTANT: aImportTab has a sibling array in d32init.c, aulProc, which must
 *            match entry by entry. Adding/removing/shuffling aImportTab, aulProc
 *            has to be updated immediately!
 */
IMPORTKRNLSYM aImportTab[NBR_OF_KRNLIMPORTS] =
{/* iFound     cchName                  offObject    usSel     fType    */
 /*      iObject      achName                 ulAddress  cProlog        */
    {FALSE, -1,  8, "_ldrRead",             -1,  -1,  -1,  -1, EPT_PROC32},        /* 0 */
    {FALSE, -1,  8, "_ldrOpen",             -1,  -1,  -1,  -1, EPT_PROC32},        /* 1 */
    {FALSE, -1,  9, "_ldrClose",            -1,  -1,  -1,  -1, EPT_PROC32},        /* 2 */
    {FALSE, -1, 12, "_LDRQAppType",         -1,  -1,  -1,  -1, EPT_PROC32},        /* 3 */
    {FALSE, -1, 20, "_ldrEnum32bitRelRecs", -1,  -1,  -1,  -1, EPT_PROC32},        /* 4 */
    {FALSE, -1, 10, "_IOSftOpen",           -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 5 */
    {FALSE, -1, 11, "_IOSftClose",          -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 6 */
    {FALSE, -1, 15, "_IOSftTransPath",      -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 7 */
    {FALSE, -1, 12, "_IOSftReadAt",         -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 8 */
    {FALSE, -1, 13, "_IOSftWriteAt",        -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 9 */
    {FALSE, -1, 12, "_SftFileSize",         -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 10 */
    {FALSE, -1, 11, "_VMAllocMem",          -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 11 */
    {FALSE, -1, 11, "_VMGetOwner",          -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 12 */
    {FALSE, -1, 11, "g_tkExecPgm",          -1,  -1,  -1,  -1, EPT_PROC32},        /* 13 */ /* currently disabled! */
    {FALSE, -1, 11, "f_FuStrLenZ",          -1,  -1,  -1,  -1, EPT_PROCIMPORT16},  /* 14 */
    {FALSE, -1, 10, "f_FuStrLen",           -1,  -1,  -1,  -1, EPT_PROCIMPORT16},  /* 14 */
    {FALSE, -1,  8, "f_FuBuff",             -1,  -1,  -1,  -1, EPT_PROCIMPORT16}   /* 15 */
/*    {FALSE, -1, 11, "",          -1,  -1, -1, EPT_PROCIMPORT16} */ /* 16 */
};

unsigned short int  usBuild          = 0;
unsigned short      usVerMajor       = 0;
unsigned short      usVerMinor       = 0;
unsigned char       fSMP             = FALSE;
unsigned char       fDebug           = FALSE;


/*
 * privat data
 */
static int          fQuiet           = 0;
static char         szUsrOS2Krnl[50] = {0};
static char         szOS2Krnl[]      = {"c:\\os2krnl"};

static char         szUsrSym[50]     = {0};
static char *       apszSym[]        =
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
    {"c:\\os2\\system\\trace\\os2krnl.sym"},          /* warp 3 ?*/
    {"c:\\os2\\system\\pmdf\\os2krnlr.sym"},          /* warp 3 ?*/
    {"c:\\os2krnlr.sym"},                             /* custom */
    {"c:\\os2krnlh.sym"},                             /* custom */
    {"c:\\os2krnld.sym"},                             /* custom */
    NULL
};

/* Result from GetKernelInfo/ReadOS2Krnl. */
static unsigned char  cObjects = 0;
static POTE           paKrnlOTEs = NULL;



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
/* File an output replacements */
static HFILE    fopen(const char * pszFilename, const char * pszIgnored);
static int      fread(void * pvBuffer, USHORT cbBlock, USHORT cBlock,  HFILE hFile);
static int      fseek(HFILE hfile, signed long off, int iOrg);
static unsigned long fsize(HFILE hFile);
static void     puts(char *psz);

/* C-library replacements. */
static void     kmemcpy(char *psz1, const char *psz2, int cch);
static char *   kstrstr(const char *psz1, const char *psz2);
static int      kstrcmp(const char *psz1, const char *psz2);
static int      kstrncmp(const char *psz1, const char *psz2, int cch);
static int      kstrnicmp(const char *psz1, const char *psz2, int cch);
static int      kstrlen(const char *psz);
static char *   kstrcpy(char * pszTarget, const char * pszSource);
static int      kargncpy(char *pszTarget, const char *pszArg, unsigned cchMaxlen);

/* Workers */
static int      LookupKrnlEntry(unsigned uBuild, unsigned char chType,
                                unsigned char fSMP, unsigned char cObjects);
static int      VerifyPrologs(void);
static int      ProbeSymFile(const char *pszFilename);
static int      GetKernelInfo(void);

/* Ouput */
static void     ShowDecNumber(unsigned long ul);
static void     ShowHexNumber(unsigned long ul);
static void     ShowResult(int rc, int iSym);

/* Others used while debugging in R3. */
static int      VerifyKernelVer(void);
static int      ReadOS2Krnl(char *pszFilename);
static int      ReadOS2Krnl2(HFILE hKrnl, unsigned long  cbKrnl);
static int      processFile(const char *pszFilename);




/*******************************************************************************
*   Implementation of Internal Helper Functions                                *
*******************************************************************************/

/**
 * Quick implementation of fopen.
 * @param    pszFilename   name of file to open (sz)
 * @param    pszIgnored    whatever - it is ignored
 * @return   Handle to file. (not pointer to a FILE stream as in C-library)
 * @remark   binary and readonly is assumed!
 */
static HFILE fopen(const char * pszFilename, const char * pszIgnored)
{
    HFILE   hFile = 0;
    USHORT  rc;
    unsigned short Action = 0;

    rc = DosOpen(
        (char*)pszFilename,
        &hFile,
        &Action,
        0,
        FILE_NORMAL,
        OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
        OPEN_SHARE_DENYNONE + OPEN_ACCESS_READONLY,
        NULL);

    pszIgnored = pszIgnored;
    return hFile;
}


/**
 * fread emulation
 * @returns   Number of blocks read.
 * @param     pvBuffer  Buffer to read into
 * @param     cbBlock   Blocksize
 * @param     cBlock    Block count
 * @param     hFile     Handle to file (HFILE)
 */
static int fread(void * pvBuffer, USHORT cbBlock, USHORT cBlock,  HFILE hFile)
{
    USHORT  ulRead;
    USHORT  rc;

    rc = DosRead(hFile, pvBuffer, (USHORT)(cbBlock*cBlock), &ulRead);
    if (rc == 0)
        rc = (USHORT)cBlock;
    else
        rc = 0;

    return rc;
}


/**
 * fseek emultation
 * @returns   Same as DosChgFilePtr
 * @param     hFile   Filehandle
 * @param     off     offset into file from origin
 * @param     org     origin
 */
static int fseek(HFILE hFile, signed long off, int iOrg)
{
    ULONG  ul;
    return  (int)DosChgFilePtr(hFile, off, iOrg, &ul);
}


/**
 * Get filesize in bytes.
 * @returns   Filesize.
 * @param     hFile   Filehandle
 * @remark    This function sets the file position to end of file.
 */
static unsigned long fsize(HFILE hFile)
{
    USHORT rc;
    unsigned long cb;

    rc = DosChgFilePtr(hFile, 0, FILE_END, &cb);

    return cb;
}


/**
 * puts lookalike
 */
static void puts(char * psz)
{
    DosPutMessage(0, kstrlen(psz), psz);
}


/**
 * kmemcpy - memory copy - slow!
 * @param     psz1  target
 * @param     psz2  source
 * @param     cch length
 */
static void     kmemcpy(char *psz1, const char *psz2, int cch)
{
    while (cch-- != 0)
        *psz1++ = *psz2++;
}


/**
 * Finds psz2 in psz2.
 * @returns   Pointer to occurence of psz2 in psz1.
 * @param     psz1  String to be search.
 * @param     psz2  Substring to search for.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static char *kstrstr(const char *psz1, const char *psz2)
{
    while (*psz1 != '\0')
    {
        register int i = 0;
        while (psz2[i] != '\0' && psz1[i] == psz2[i])
            i++;

        /* found it? */
        if (psz2[i] == '\0')
            return (char*)psz1;
        if (psz1[i] == '\0' )
            break;
        psz1++;
    }

    return NULL;
}


#if 0 /* not in use */
/**
 * kstrcmp - String compare
 * @returns   0 - equal else !0
 * @param     psz1  String 1
 * @param     psz2  String 2
 */
static int      kstrcmp(const char *psz1, const char *psz2);
{
    while (*psz1 == *psz2 && *psz1 != '\0' && *psz2 != '\0')
    {
        psz1++;
        psz2++;
    }
    return *psz1 - *psz2;
}
#endif


/**
 * kstrncmp - String 'n' compare.
 * @returns   0 - equal else !0
 * @param     p1  String 1
 * @param     p2  String 2
 * @param     len length
 */
static int      kstrncmp(register const char *psz1, register const char *psz2, int cch)
{
    int i = 0;
    while (i < cch && *psz1 == *psz2 && *psz1 != '\0' && *psz2 != '\0')
    {
        psz1++;
        psz2++;
        i++;
    }

    return i - cch;
}


#if 0 /* not in use */
/**
 * kstrnicmp - String 'n' compare, case-insensitive.
 * @returns   0 - equal else !0
 * @param     p1  String 1
 * @param     p2  String 2
 * @param     len length
 */
static int      kstrnicmp(const char *psz1, const char *psz2, int cch)
{
    register char ch1, ch2;

    do
    {
        ch1 = *psz1++;
        if (ch1 >= 'A' && ch1 <= 'Z')
            ch1 += 'a' - 'A';           /* to lower case */
        ch2 = *psz2++;
        if (ch2 >= 'A' && ch2 <= 'Z')
            ch2 += 'a' - 'A';           /* to lower case */
    } while (--cch > 0 && ch1 == ch2 && ch1 != '\0' && ch2 != '\0');

    return ch1 - ch2;
}
#endif


/**
 * kstrlen - String length.
 * @returns   Length of the string.
 * @param     psz  Pointer to string.
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 */
static int kstrlen(register const char * psz)
{
    register int cch = 0;
    while (*psz++ != '\0')
        cch++;
    return cch;
}


/**
 * String copy (strcpy).
 * @returns   Pointer to target string.
 * @param     pszTarget  Target string.
 * @param     pszSource  Source string.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static char * kstrcpy(char * pszTarget, register const char * pszSource)
{
    register char *psz = pszTarget;

    while (*pszSource != '\0')
        *psz++ = *pszSource++;

    return pszTarget;
}




/**
 * Copy an argument to a buffer. Ie. "-K[=|:]c:\os2krnl ....". Supports quotes
 * @returns   Number of chars of pszArg that has been processed.
 * @param     pszTarget  -  pointer to target buffer.
 * @param     pszArg     -  pointer to source argument string.
 * @param     cchMaxlen  -  maximum chars to copy.
 */
static int kargncpy(char * pszTarget, const char * pszArg, unsigned cchMaxlen)
{
    int i = 0;
    int fQuote = FALSE;

    /* skip option word/letter */
    while (*pszArg != '\0' && *pszArg != ' ' && *pszArg != ':' &&
           *pszArg != '='  && *pszArg != '-' && *pszArg != '/')
    {
        pszArg++;
        i++;
    }

    if (*pszArg == ' ' || *pszArg == '-' || *pszArg == '/' || *pszArg == '\0')
        return 0;


    do
    {
        pszArg++;
        i++;
    } while (*pszArg != '\0' && *pszArg == ' ');

    /* copy maxlen or less */
    /* check for quotes */
    if (*pszArg == '"')
    {
        fQuote = TRUE;
        pszArg++;
        i++;
    }
    /* copy loop */
    while (cchMaxlen > 1 && (fQuote ? *pszArg != '"' : *pszArg != ' ') && *pszArg != '\0')
    {
        *pszTarget++ = *pszArg++;
        i++;
        cchMaxlen--;
    }

    /* terminate pszTarget */
    pszTarget = '\0';

    return i;
}



/*******************************************************************************
*   Implementation Of The Important Functions                                  *
*******************************************************************************/
#ifndef EXTRACT
/**
 * Checks if this kernel is within the kernel symbol database.
 * If an entry for the kernel is found, the data is copied from the
 * database entry to aImportTab.
 * @returns   NO_ERROR on succes (0)
 *            1 if not found.
 *            Error code on error.
 * @param     uBuild    Build level.
 * @param     chType    'A' all strict
 *                      'H' half strict
 *                      'R' retail
 * @param     fSMP      TRUE: SMP
 *                      FALSE: UNI
 * @param     cObjects  Count of object in the running kernel.
 * @sketch    Loop thru the table.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static int LookupKrnlEntry(unsigned uBuild, unsigned char chType, unsigned char fSMP, unsigned char cObjects)
{
    int i;

    /*
     * Loop tru the DB entries until a NULL pointer is found.
     */
    for (i = 0; aKrnlSymDB[i].usBuild != 0; i++)
    {
        if (aKrnlSymDB[i].usBuild      == uBuild
            && aKrnlSymDB[i].chType    == chType
            && aKrnlSymDB[i].fSMP      == fSMP
            && aKrnlSymDB[i].cObjects  == cObjects)
        {   /* found matching entry! */
            int j;
            int rc;
            register PKRNLDBENTRY pEntry = &aKrnlSymDB[i];

            dprintf(("LookUpKrnlEntry - found entry for this kernel!\n"));

            /*
             * Copy symbol data from the DB to aImportTab.
             */
            for (j = 0; j < NBR_OF_KRNLIMPORTS; j++)
            {
                aImportTab[j].offObject  = pEntry->aSyms[j].offObject;
                aImportTab[j].iObject    = pEntry->aSyms[j].iObject;
                aImportTab[j].ulAddress  = paKrnlOTEs[pEntry->aSyms[j].iObject].ote_base
                                           + pEntry->aSyms[j].offObject;
                aImportTab[j].usSel      = paKrnlOTEs[pEntry->aSyms[j].iObject].ote_sel;
                aImportTab[j].fFound     = TRUE;
                dprintf(("  %-3d addr=0x%08lx off=0x%08lx  %s\n",
                         j, aImportTab[j].ulAddress, aImportTab[j].offObject,
                         aImportTab[j].achName));
            }

            /* Verify prologs*/
            rc = VerifyPrologs();

            /* set sym name */
            if (rc == 0)
                kstrcpy(szUsrSym, "Win32k Symbol Database");
            return rc;
        }
    }

    /* not found */
    return 1;
}
#endif /* !EXTRACT */

/**
 * Verifies the that the addresses in aImportTab are valid.
 * This is done at Ring-0 of course.
 * @returns   0 if ok, not 0 if not ok.
 */
static int VerifyPrologs(void)
{
#if !defined(DEBUGR3) && !defined(EXTRACT)
    APIRET          rc;
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;

    rc = DosOpen("\\dev\\elf$", &hDev0, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
    if (rc == NO_ERROR)
    {
        rc = DosDevIOCtl("", "", D16_IOCTL_VERIFYIMPORTTAB, D16_IOCTL_CAT, hDev0);
        DosClose(hDev0);
    }

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
static int ProbeSymFile(const char * pszFilename)
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
        return -50;
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
        return -51;
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
        return -51;
    }


    /*
     * Verify that the number of segments is equal to the number objects in OS2KRNL.
     */
    #ifndef EXTRACT
    if (MapDef.cSegs != cObjects)
    {   /* incorrect count of segments. */
        dprintf(("Segment No. verify failed\n"));
        fclose(hSym);
        return -52;
    }
    #endif /* !EXTRACT */


    /*
     * Reset ProcTab
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        aImportTab[i].fFound = 0;
        #ifdef DEBUG
        aImportTab[i].offObject = 0;
        aImportTab[i].ulAddress = 0;
        aImportTab[i].usSel = 0;
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
            return -53;
        }
        rc = fread(&SegDef, sizeof(SEGDEF), 1, hSym);
        if (!rc)
        {   /* Failed to read the segment definition, fail! */
            fclose(hSym);
            return -53;
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
            for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
            {
                if (!aImportTab[i].fFound                                     /* Not allready found */
                    && (aImportTab[i].fType & EPT_BIT_MASK) == fSegEPTBitType /* Equal bittype */
                    && (fCode || (aImportTab[i].fType & EPT_VARIMPORT))       /* Don't look for code in a data segment and vice versa */
                    && aImportTab[i].cchName == cchName                       /* Equal name length */
                    && kstrncmp(aImportTab[i].achName, achBuffer, cchName) == 0  /* Equal name */
                    )
                {   /* Symbol was found */
                    aImportTab[i].offObject = (SegDef.bFlags & 0x01 ? SymDef32.wSymVal : SymDef16.wSymVal);
                    aImportTab[i].ulAddress = aImportTab[i].offObject + paKrnlOTEs[iSeg].ote_base;
                    aImportTab[i].iObject   = (unsigned char)iSeg;
                    aImportTab[i].usSel     = paKrnlOTEs[iSeg].ote_sel;
                    dprintf(("debug: base=%lx, size=%lx iSeg=%d\n", paKrnlOTEs[iSeg].ote_base, paKrnlOTEs[iSeg].ote_size, iSeg));

                    /* Paranoia test! */
                    #ifndef EXTRACT
                    if (aImportTab[i].offObject < paKrnlOTEs[iSeg].ote_size)
                    {
                        aImportTab[i].fFound = TRUE;
                        cLeftToFind--;
                        dprintf(("Found: %s at off 0x%lx addr 0x%lx, sel=0x%x\n",
                                 aImportTab[i].achName, aImportTab[i].offObject,
                                 aImportTab[i].ulAddress, aImportTab[i].usSel));
                    }
                    else/* test failed, continue on next symbol*/
                        dprintf(("Error: Paranoia test failed for %s\n", aImportTab[i].achName));;
                    #else
                    aImportTab[i].fFound = TRUE;
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
    if (cLeftToFind != 0)
        return -57;

    /*
     * Verify function prologs and return.
     */
    return VerifyPrologs();
}


/**
 * Gets the os/2 kernel OTE's (object table entries).
 * @returns   0 on success. Not 0 on error.
 */
static int   GetKernelInfo(void)
{
#if !defined(DEBUGR3) && !defined(EXTRACT) /* IOCtl not available after inittime! */
    static KRNLINFO KrnlInfo = {0};
    APIRET          rc;
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;

    rc = DosOpen("\\dev\\elf$", &hDev0, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
    if (rc == NO_ERROR)
    {
        rc = DosDevIOCtl(&KrnlInfo, "", D16_IOCTL_GETKRNLINFO, D16_IOCTL_CAT, hDev0);
        if (rc == NO_ERROR)
        {
            int i;

            /* Set the exported parameters */
            usBuild  = KrnlInfo.usBuild;
            fSMP     = KrnlInfo.fSMP;
            fDebug   = KrnlInfo.fDebug;
            cObjects = KrnlInfo.cObjects;
            paKrnlOTEs = &KrnlInfo.aObjects[0];
            #ifdef DEBUG
            for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
                dprintf(("debug: no.%2d base=%lx size=%lx sel=%x\n",
                         i,
                         paKrnlOTEs[i].ote_base,
                         paKrnlOTEs[i].ote_size,
                         paKrnlOTEs[i].ote_sel));
            #endif

        }
        DosClose(hDev0);
    }

    if (rc != NO_ERROR)
        printf16("Failed to get kernel OTEs\r\n");

    return rc;

#else
    #ifndef EXTRACT
        APIRET          rc;

        /*--------------*/
        /* read kernel  */
        /*--------------*/
        if (szUsrOS2Krnl[0] != '\0')
        {
            rc = ReadOS2Krnl(szUsrOS2Krnl);
            if (rc != 0)
            {
                puts("Warning: Invalid kernel file specified. Tries defaults.\n\r");
                szUsrOS2Krnl[0] = '\0';
                rc = ReadOS2Krnl(szOS2Krnl);
            }
        }
        else
            rc = ReadOS2Krnl(szOS2Krnl);
        return rc;
    #else
        return 0;
    #endif
#endif
}


/**
 * Shows result of kernelprobing if not quiet or on error.
 * @param     rc          Return code.
 * @param     iSym        index of .sym-file into static struct.
 */
#ifndef EXTRACT
static void ShowResult(int rc, int iSym)
{
    int i;

    /*
     * Complain even if quiet on error
     */
    if (!fQuiet || rc != 0)
    {
        printf16("Win32k - Odin32 support driver.\n");

        /*
         * kernel stuff
         */
        if (rc <= -50 || rc == 0)
        {
            #ifdef DEBUGR3
            printf16("    Found kernel:     %s\n", szOS2Krnl);
            #endif
            printf16("    Build:            %d - v%d.%d\n",
                     usBuild, usVerMajor, usVerMinor);
        }
        else
            printf16("    Kernel probing failed with rc=%d.\n", rc);

        /*
         * symbol-file
         */
        if (rc == 0)
            printf16("    Found symbolfile: %s\n",
                     szUsrSym[0] == '\0' ? apszSym[iSym] : szUsrSym);
        else
            printf16("    Failed to find symbolfile! rc=%d\n", rc);

        /*
         * function listing
         */
        for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
        {
            printf16("  %-20s at ",aImportTab[i].achName);
            if (aImportTab[i].fFound)
                printf16("0x%08lx%s", aImportTab[i].ulAddress, (i % 2) == 0 ? "" : "\n");
            else
                printf16("failed!%s", (i % 2) == 0 ? "" : "\n");
        }
        if (i % 2) printf16("\n");
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
    SEL         GDT;
    SEL         LDT;
    PGINFOSEG   pInfoSeg;
    USHORT      usBootDrive;

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
                    case 'k':
                    case 'K': /* Kernel file */
                        i++;
                        i += kargncpy(szUsrOS2Krnl, &pReqPack->InitArgs[i], sizeof(szUsrOS2Krnl));
                        break;

                    case 'n':
                    case 'N': /* NoLoader */
                        return 0;

                    case 'q':
                    case 'Q': /* Quiet */
                        fQuiet = 1;
                        break;

                    case 's':
                    case 'S': /* Symbol file */
                        i++;
                        if (pReqPack->InitArgs[i] == 'Y' || pReqPack->InitArgs[i] == 'y')
                            i += kargncpy(szUsrSym, &pReqPack->InitArgs[i], sizeof(szUsrSym));
                        break;

                    case 'v':
                    case 'V': /* Verbose */
                        fQuiet = 0;
                        break;
                }
            }
        }
    }

    /*---------------------*/
    /* determin boot drive */
    /*---------------------*/
    rc = DosGetInfoSeg(&GDT, &LDT);
    if (rc != NO_ERROR)
        return rc;

    pInfoSeg  = MAKEPGINFOSEG(GDT);
    usBootDrive = pInfoSeg->bootdrive;
    usVerMajor  = pInfoSeg->uchMajorVersion;
    usVerMinor  = pInfoSeg->uchMinorVersion;
    dprintf(("BootDrive: %d\n", usBootDrive));

    /* set driveletter in constants strings */
    usBootDrive = (char)usBootDrive + (char)'a' - 1;
    szOS2Krnl[0] = (char)usBootDrive;
    for (i = 0; apszSym[i] != NULL; i++)
        apszSym[i][0] = (char)usBootDrive;

    /*-----------------*/
    /* get kernel info */
    /*-----------------*/
    rc = GetKernelInfo();
    if (rc != NO_ERROR)
        return rc;

    /*--------------*/
    /* read symfile */
    /*--------------*/
    if (!rc)
    {
        rc = 1;
        if (szUsrSym[0] != '\0')
        {
            rc = ProbeSymFile(szUsrSym);
            if (rc)
            {
                puts("Warning: Invalid symbol file specified. Tries defaults.\n\r");
                szUsrSym[0] = '\0';
            }
        }
        if (rc != 0) /* if user sym failed or don't exists. */
        {
            /*
             * Check database - only if not a debug kernel!
             * You usually have a .sym-file when using a debug kernel.
             * (Currently I am not able to distinguish between half and all strict kernels...)
             */
            if (fDebug ||
                (rc = LookupKrnlEntry((unsigned short)usBuild, 'R', fSMP, cObjects)) != 0
                )
            {
                /* search on disk */
                i = 0;
                while (apszSym[i] != NULL && (rc = ProbeSymFile(apszSym[i])) != 0)
                    i++;
            }
        }
    }

    /* show the result and set return-value */
    dprintf(("rc=%d; i=%d\n", rc, i));
    ShowResult(rc, i);

    return rc;
}
#endif


#ifdef DEBUGR3
/**
 * Verifies that build no, matches kernel number.
 * @returns   0 on equal, !0 on error.
 */
static int VerifyKernelVer(void)
{
    int VerMinor, VerMajor;

    VerMajor = usBuild < 20000 ? 20 : 30/*?*/;
    VerMinor = usBuild <  6600 ? 10 :  usBuild <  8000 ? 11 : usBuild < 9000 ? 30 :
               usBuild < 10000 ? 40 :  usBuild < 15000 ? 45 : 50;

    return VerMajor - (int)usVerMajor | VerMinor - (int)usVerMinor;
}


/**
 * Reads and verifies OS/2 kernel.
 * @returns   0 on success, not 0 on failure.
 * @param     filename   Filename of the OS/2 kernel.
 * @result    usBuild is set.
 * @remark    This step will be eliminated by searching thru the DOSGROUP datasegment
 *            in the kernel memory. This segment have a string "Internal revision 9.034[smp|uni]"
 *            This would be much faster than reading the kernel file. It will also give us a more precise
 *            answer to the question! This is currently a TODO issue. !FIXME!
 */
static int ReadOS2Krnl(char * pszFilename)
{
    HFILE          hKrnl;
    unsigned long  cbKrnl;
    int            rc;

    hKrnl = fopen(pszFilename, "rb");
    if (hKrnl != 0)
    {
        cbKrnl = fsize(hKrnl);
        if (!fseek(hKrnl, 0, SEEK_SET))
            rc = ReadOS2Krnl2(hKrnl, cbKrnl);
        else
            rc = -2;
        fclose(hKrnl);
    }
    else
    {
        dprintf(("Could not open file\n"));
        rc = -1;
    }
    return rc;
}


/**
 * Worker function for ReadOS2Krnl
 * @returns   0 on success.
 *            errorcodes on failure. (-1 >= rc >= -14)
 * @param     hKrnl   Handle to the kernel file.
 * @param     cbKrnl  Size of the kernel file.
 * @author    knut st. osmundsen
 */
static int ReadOS2Krnl2(HFILE hKrnl, unsigned long  cbKrnl)
{
    #if defined(DEBUGR3) || !defined(EXTRACT)
    static KRNLINFO KrnlInfo = {0};
    #endif
    int            i, j;
    int            rc = 0;
    char           achBuffer[KERNEL_ID_STRING_LENGTH + KERNEL_READ_SIZE];
    unsigned long  offLXHdr;
    struct e32_exe *pLXHdr;


    /* find bldlevel string - "@#IBM:14.020#@  IBM OS/2 Kernel - 14.020F" */
    if (fseek(hKrnl, 0, SEEK_SET))
        return -2;

    if (!fread(&achBuffer[KERNEL_ID_STRING_LENGTH], 1, KERNEL_READ_SIZE, hKrnl))
        return -3;

    i = KERNEL_ID_STRING_LENGTH;
    while (cbKrnl > 0)
    {
        if (i == KERNEL_READ_SIZE)
        {

            kmemcpy(achBuffer, &achBuffer[KERNEL_READ_SIZE], KERNEL_ID_STRING_LENGTH);
            if (!fread(&achBuffer[KERNEL_ID_STRING_LENGTH], 1, cbKrnl > KERNEL_READ_SIZE ? KERNEL_READ_SIZE : (int)cbKrnl, hKrnl))
                return -3;

            i = 0;
        }

        if (kstrncmp("@#IBM:", &achBuffer[i], 6) == 0)
            break;

        /* next */
        i++;
        cbKrnl--;
    }

    if (cbKrnl == 0)
    {
        fclose(hKrnl);
        return -4;
    }

    /* displacement */
    j = 0;
    while (j < 6 && achBuffer[i+10+j] != '#')
        j++;

    /* verify signature */
    if (kstrncmp(&achBuffer[i+10+j], "#@  IBM OS/2 Kernel", 19) != 0)
        return -5;

    /* read usBuild */
    usBuild  = (char)(achBuffer[i+6] - '0') * 1000;
    if (achBuffer[i+7] != '.')
    {
        /* this code is for Warp5 */
        usBuild *= 10;
        usBuild += (char)(achBuffer[i+7] - '0') * 1000;
        i++;
        j--;
        if (achBuffer[i+7] != '.')
        {
            usBuild = usBuild * 10;
            usBuild = usBuild + (unsigned short)(achBuffer[i+7] - '0') * 1000;
            i++;
            j--;
        }
    }

    if (j == 0)
    {
        usBuild += (achBuffer[i+ 8] - '0') * 10;
        usBuild += (achBuffer[i+ 9] - '0') * 1;
    }
    else
    {
        if (j == 3)
            return -9;
        usBuild += (achBuffer[i+ 8] - '0') * 100;
        usBuild += (achBuffer[i+ 9] - '0') * 10;
        usBuild += (achBuffer[i+10] - '0');
    }

    if (VerifyKernelVer())
        return -9;
    dprintf(("usBuild: %d\n", usBuild));

    /* get segment number */
    /* read-MZheader */
    if (fseek(hKrnl,0,SEEK_SET))
        return -2;

    if (!fread(achBuffer, 1, 0x40, hKrnl))
        return -3;

    offLXHdr = *(unsigned long int *)&achBuffer[0x3c];

    if (offLXHdr > 0x2000 && offLXHdr < 0x80) /* just to detect garbage */
        return -6;

    if (fseek(hKrnl, offLXHdr, SEEK_SET))
        return -2;

    if (!fread(achBuffer, 1, sizeof(struct e32_exe), hKrnl))
        return -3;

    /* check LX-magic */
    if (achBuffer[0] != 'L' || achBuffer[1] != 'X')
        return -7;

#if !defined(DEBUGR3) && !defined(EXTRACT)
    /* check object count - match it with what we got from the kernel. */
    pLXHdr = (struct e32_exe *)achBuffer;
    if ((UCHAR)pLXHdr->e32_objcnt != cObjects)
        return -8;

    if (pLXHdr->e32_objcnt < 10)
        return -9;

    /* check objects (sizes and flags(?)) */
    if (!fseek(hKrnl, (LONG)offLXHdr + (LONG)pLXHdr->e32_objtab, SEEK_SET))
    {
        struct o32_obj *pObj = (struct o32_obj *)achBuffer;
        for (i = 0; i < (int)cObjects; i++)
        {
            if (!fread(achBuffer, 1, sizeof(OTE), hKrnl))
                return -11;
            if (pObj->o32_size < paKrnlOTEs[i].ote_size)
                return -12;

            #if 0 /* don't work! */
            if ((pObj->o32_flags & 0xffffUL) != (paKrnlOTEs[i].ote_flags & 0xffffUL))
                return -14;
            #endif
        }
    }
    else
        return -10;
#else
    /* Since we can't get the OTEs from the kernel when debugging in RING-3,
     * we'll use what we find in the kernel.
     */

    /*  object count */
    pLXHdr = (struct e32_exe *)achBuffer;
    cObjects = (UCHAR)pLXHdr->e32_objcnt;
    paKrnlOTEs = &KrnlInfo.aObjects[0];

    /* get OTEs */
    if (!fseek(hKrnl, (LONG)offLXHdr + (LONG)pLXHdr->e32_objtab, SEEK_SET))
    {
        struct o32_obj *pObj = (struct o32_obj *)achBuffer;
        for (i = 0; i < (int)cObjects; i++)
            if (!fread(&paKrnlOTEs[i], 1, sizeof(struct o32_obj), hKrnl))
                return -11;
    }
    else
        return -10;
#endif

    return 0;
}



/**
 * Debug - Main procedure for standalone testing.
 */
void main(int argc, char **argv)
{
    char     szParm[512];
    RPINITIN ReqPack;
    int      j;
    int      i    = 0;
    int      argi = 0;

    while (argi < argc)
    {
        j = 0;
        while (argv[argi][j] != '\0')
            szParm[i++] = argv[argi][j++];
        szParm[i++] = ' ';

        /* next */
        argi++;
    }
    szParm[i++] = '\0';

    ReqPack.InitArgs = szParm;

    ProbeKernel(&ReqPack);
}
#endif


#ifdef EXTRACT
/**
 * Dumps writes a KRNLDBENTRY struct to stderr for the given .sym-file.
 * The filesnames are on this format:
 *    nnnn[n]tm.SYM
 * Where: n - are the build number 4 or 5 digits.
 *        t - kernel type. R = retail, H = half strict, A = all strict.
 *        m - UNI or SMP.  U = UNI processor kernel. S = SMP processor kernel.
 * @returns   NO_ERROR on success. Untracable error code on error.
 * @param     pszFilename  Pointer to read only filename of the .sym-file.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    Currently only retail kernels are processed. See note below.
 */
static int processFile(const char *pszFilename)
{
    APIRET   rc;
    int      cch = kstrlen(pszFilename);

    /* Filename check */
    if (cch < 10 || cch > 11
        || !(pszFilename[0] >= '0' && pszFilename[0] <= '9')
        || !(pszFilename[1] >= '0' && pszFilename[1] <= '9')
        || !(pszFilename[2] >= '0' && pszFilename[2] <= '9')
        || !(pszFilename[3] >= '0' && pszFilename[3] <= '9')
        || !(pszFilename[cch-7] >= '0' && pszFilename[cch-7] <= '9')
        || !(pszFilename[cch-6] == 'A' || pszFilename[cch-6] == 'H' || pszFilename[cch-6] == 'R')
        || !(pszFilename[cch-5] == 'S' || pszFilename[cch-5] == 'U')
        )
    {
        printf16("invalid filename: %s\n", pszFilename);
        return 2;
    }

    /** @remark
     * All a/h-strict files are currently ignored.
     * When a debug kernel is used we'll have to use the
     * .sym-file for it. This is so because I can't distinguish
     * between a all-strick and a half-strick kernel (yet).
     */
    if (pszFilename[cch-6] != 'R')
        return 0;

    /*
     * Probe kernelfile
     */
    rc = ProbeSymFile(pszFilename);


    /*
     * on success dump a struct for this kernel
     */
    if (rc == 0)
    {
        int i;

        printf16("    { /* %s */\n"
                 "        %.*s, \'%c\', %s, %d,\n"
                 "        {\n",
                 pszFilename,
                 cch - 6, &pszFilename[0],                       /* build number */
                 pszFilename[cch - 6],                           /* Type, A=astrict, H=halfstrict, R=Retail */
                 pszFilename[cch - 5] == 'S' ? "TRUE" : "FALSE", /* UNI: TRUE  SMP: FALSE */
                 aImportTab[0].iObject + 1); /* ASSUMES that DOSCODE32 is the last object. */

        for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
        {
            char *psz = aImportTab[i].achName;
            printf16("            {%2d, 0x%08lx}, /* %s */\n",
                     aImportTab[i].iObject,
                     aImportTab[i].offObject,
                     (char *)&aImportTab[i].achName[0]
                     );
        }
        printf16("        }\n"
                 "    },\n");
    }
    else
        printf16("ProbeSymFile failed with rc=%d\n", rc);

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

    if (argc > 1)
    {
        /*
         * Arguments: extract.exe <symfiles...>
         */
        int i;
        for (i = 0; i < argc; i++)
        {
            rc = processFile(argv[i]);
            if (rc != NO_ERROR)
            {
                printf16("processFile failed with rc=%d for file %s\n",
                          rc, argv[i]);
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
        USHORT      usSearch = 1;
        HDIR        hDir = HDIR_CREATE;
        FILEFINDBUF ffb;
        int         i;

        printf16("/* $Id: probkrnl.c,v 1.16 2000-02-25 18:15:02 bird Exp $\n"
                 "*\n"
                 "* Autogenerated kernel symbol database.\n"
                 "*\n"
                 "* Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)\n"
                 "*\n"
                 "* Project Odin Software License can be found in LICENSE.TXT\n"
                 "*\n"
                 "*/\n");

        printf16("\n"
                 "#include <os2.h>\n"
                 "#include \"probkrnl.h\"\n"
                 "\n");

        printf16("KRNLDBENTRY aKrnlSymDB[] = \n"
                 "{\n");

        rc = DosFindFirst("*.sym", &hDir, FILE_NORMAL,
                          &ffb, sizeof(ffb),
                          &usSearch, 0UL);
        while (rc == NO_ERROR & usSearch > 0)
        {
            rc = processFile(&ffb.achName[0]);
            if (rc != NO_ERROR)
            {
                printf16("processFile failed with rc=%d for file %s\n",
                         rc, &ffb.achName[0]);
                return rc;
            }

            /* next file */
            rc = DosFindNext(hDir, &ffb, sizeof(ffb), &usSearch);
        }
        DosFindClose(hDir);

        printf16("    { /* Terminating entry */\n"
                 "        0,0,0,0,\n"
                 "        {\n");
        for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
            printf16("            {0,0},\n");
        printf16("        }\n"
                 "    }\n"
                 "}; /* end of aKrnlSymDB[] */\n"
                 );
    }


    return rc;
}
#endif /* EXTRACT */

