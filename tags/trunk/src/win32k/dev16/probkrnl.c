/* $Id: probkrnl.c,v 1.13 2000-02-21 04:45:45 bird Exp $
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
#ifdef DEBUGR3
    #if 1
        int printf(const char *, ...);
        #define dprintf(a) printf a
    #else
        #define dprintf(a)
    #endif
#else
    #define dprintf(a)
    #define static
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
    {FALSE, -1, 12, "_LDRQAppType",         -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 3 */ /* to be removed? */
    {FALSE, -1, 20, "_ldrEnum32bitRelRecs", -1,  -1,  -1,  -1, EPT_PROC32},        /* 4 */
    {FALSE, -1, 10, "_IOSftOpen",           -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 5 */
    {FALSE, -1, 11, "_IOSftClose",          -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 6 */
    {FALSE, -1, 15, "_IOSftTransPath",      -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 7 */
    {FALSE, -1, 12, "_IOSftReadAt",         -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 8 */
    {FALSE, -1, 13, "_IOSftWriteAt",        -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 9 */
    {FALSE, -1, 12, "_SftFileSize",         -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 10 */
    {FALSE, -1, 11, "_VMAllocMem",          -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 11 */
    {FALSE, -1, 11, "_VMGetOwner",          -1,  -1,  -1,  -1, EPT_PROCIMPORT32},  /* 12 */
    {FALSE, -1, 11, "g_tkExecPgm",          -1,  -1,  -1,  -1, EPT_PROC32},        /* 13 */
    {FALSE, -1, 11, "f_FuStrLenZ",          -1,  -1,  -1,  -1, EPT_PROCIMPORT16},  /* 14 */
    {FALSE, -1,  8, "f_FuBuff",             -1,  -1,  -1,  -1, EPT_PROCIMPORT16}   /* 15 */
/*    {FALSE, -1, 11, "",          -1,  -1, -1, EPT_PROCIMPORT16} */ /* 16 */
};

unsigned long int   ulBuild          = 0;
unsigned short      usVerMajor       = 0;
unsigned short      usVerMinor       = 0;


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

static KRNLOBJTABLE KrnlOTEs = {0};

/* messages */
static char szBanner[]   = "Win32k - Odin32 support driver.";
static char szMsg1[]     = "\n\r    Found kernel:     ";
static char szMsg1a[]    = "\n\r    Build:            ";
static char szMsg2[]     = "\n\r    Found symbolfile: ";
static char szMsg4[]     = "\n\r    Failed to find symbolfile!\n\r";
static char szMsgfailed[]= "failed!   ";


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
static int      kargncpy(char *pszTarget, const char *pszArg, unsigned cchMaxlen);

/* Workers */
static int      VerifyPrologs(void);
static int      ProbeSymFile(char *pszFilename);
static int      VerifyKernelVer(void);
static int      ReadOS2Krnl(char *pszFilename);
static int      ReadOS2Krnl2(HFILE hKrnl, unsigned long  cbKrnl);
static int      GetKernelOTEs(void);

/* Ouput */
static void     ShowDecNumber(unsigned long ul);
static void     ShowHexNumber(unsigned long ul);
static void     ShowResult(int rc, int iSym);




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

/**
 * Verifies the that the addresses in aImportTab are valid.
 * This is done at Ring-0 of course.
 * @returns   0 if ok, not 0 if not ok.
 */
static int VerifyPrologs(void)
{
#ifndef DEBUGR3
    APIRET          rc;
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;

    rc = DosOpen("\\dev\\elf$", &hDev0, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
    if (rc == NO_ERROR)
    {
        rc = DosDevIOCtl("", "", D16_IOCTL_VERIFYPROCTAB, D16_IOCTL_CAT, hDev0);
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
static int ProbeSymFile(char * pszFilename)
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
    if (MapDef.cSegs != KrnlOTEs.cObjects)
    {   /* incorrect count of segments. */
        dprintf(("Segment No. verify failed\n"));
        fclose(hSym);
        return -52;
    }


    /*
     * Reset ProcTab
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
        aImportTab[i].fFound = 0;


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
                    aImportTab[i].ulAddress = aImportTab[i].offObject + KrnlOTEs.aObjects[iSeg].ote_base;
                    aImportTab[i].usSel     = KrnlOTEs.aObjects[iSeg].ote_sel;

                    /* Paranoia test! */
                    if (aImportTab[i].offObject < KrnlOTEs.aObjects[iSeg].ote_size)
                    {
                        aImportTab[i].fFound = TRUE;
                        cLeftToFind--;
                        dprintf(("Found: %s at off 0x%lx addr 0x%lx, sel=0x%x\n",
                                 aImportTab[i].achName, aImportTab[i].offObject,
                                 aImportTab[i].ulAddress, aImportTab[i].usSel));
                    }
                    else/* test failed, continue on next symbol*/
                        dprintf(("Error: Paranoia test failed for %s\n", aImportTab[i].achName));;
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
 * Verifies that build no, matches kernel number.
 * @returns   0 on equal, !0 on error.
 */
static int VerifyKernelVer(void)
{
    int VerMinor, VerMajor;

    VerMajor = ulBuild < 20000 ? 20 : 30/*?*/;
    VerMinor = ulBuild < 6600 ? 10 :  ulBuild < 8000 ? 11 : ulBuild < 9000 ? 30 :
               ulBuild < 10000 ? 40 :  ulBuild < 15000 ? 45 : 50;

    return VerMajor - (int)usVerMajor | VerMinor - (int)usVerMinor;
}


/**
 * Reads and verifies OS/2 kernel.
 * @returns   0 on success, not 0 on failure.
 * @param     filename   Filename of the OS/2 kernel.
 * @result    ulBuild is set.
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

    /* read ulBuild */
    ulBuild  = (char)(achBuffer[i+6] - '0') * 1000;
    if (achBuffer[i+7] != '.')
    {
        /* this code is for Warp5 */
        ulBuild *= 10;
        ulBuild += (char)(achBuffer[i+7] - '0') * 1000;
        i++;
        j--;
        if (achBuffer[i+7] != '.')
        {
            ulBuild = ulBuild * 10;
            ulBuild = ulBuild + (unsigned long)(achBuffer[i+7] - '0') * 1000;
            i++;
            j--;
        }
    }

    if (j == 0)
    {
        ulBuild += (achBuffer[i+ 8] - '0') * 10;
        ulBuild += (achBuffer[i+ 9] - '0') * 1;
    } else
    {
        if (j == 3)
            return -9;
        ulBuild += (achBuffer[i+ 8] - '0') * 100;
        ulBuild += (achBuffer[i+ 9] - '0') * 10;
        ulBuild += (achBuffer[i+10] - '0');
    }

    if (VerifyKernelVer())
        return -9;
    dprintf(("ulBuild: %d\n",ulBuild));

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

#ifndef DEBUGR3
    /* check object count - match it with what we got from the kernel. */
    pLXHdr = (struct e32_exe *)achBuffer;
    if ((UCHAR)pLXHdr->e32_objcnt != KrnlOTEs.cObjects)
        return -8;

    if (pLXHdr->e32_objcnt < 10)
        return -9;

    /* check objects (sizes and flags(?)) */
    if (!fseek(hKrnl, (LONG)offLXHdr + (LONG)pLXHdr->e32_objtab, SEEK_SET))
    {
        struct o32_obj *pObj = (struct o32_obj *)achBuffer;
        for (i = 0; i < (int)KrnlOTEs.cObjects; i++)
        {
            if (!fread(achBuffer, 1, sizeof(OTE), hKrnl))
                return -11;
            if (pObj->o32_size < KrnlOTEs.aObjects[i].ote_size)
                return -12;

            #if 0 /* don't work! */
            if ((pObj->o32_flags & 0xffffUL) != (KrnlOTEs.aObjects[i].ote_flags & 0xffffUL))
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
    KrnlOTEs.cObjects = (UCHAR)pLXHdr->e32_objcnt;

    /* get OTEs */
    if (!fseek(hKrnl, (LONG)offLXHdr + (LONG)pLXHdr->e32_objtab, SEEK_SET))
    {
        struct o32_obj *pObj = (struct o32_obj *)achBuffer;
        for (i = 0; i < (int)KrnlOTEs.cObjects; i++)
            if (!fread(&KrnlOTEs.aObjects[i], 1, sizeof(struct o32_obj), hKrnl))
                return -11;
    }
    else
        return -10;
#endif

    return 0;
}


/**
 * Gets the os/2 kernel OTE's (object table entries).
 * @returns   0 on success. Not 0 on error.
 */
static int   GetKernelOTEs(void)
{
#ifndef DEBUGR3
    APIRET          rc;
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;

    rc = DosOpen("\\dev\\elf$", &hDev0, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
    if (rc == NO_ERROR)
    {
        rc = DosDevIOCtl(&KrnlOTEs, "", D16_IOCTL_GETKRNLOTES, D16_IOCTL_CAT, hDev0);
        DosClose(hDev0);
    }

    if (rc != NO_ERROR)
        puts("Failed to get kernel OTEs\r\n");

    return rc;
#else
    KrnlOTEs.cObjects = 23;
    return 0;
#endif
}


/**
 * Displays an ULONG in decimal notation using DosPutMessage
 * @param     n   ULONG to show.
 */
static void ShowDecNumber(unsigned long n)
{
    int f = 0;
    unsigned long div;
    char sif;

    for (div = 1000000; div > 0; div /= 10)
    {
        sif = (char)(n/div);
        n %= div;
        if (sif != 0 || f)
        {
            f = 1;
            sif += '0';
            DosPutMessage(0, 1, &sif);
        }
    }
}


/**
 * Displays an ULONG in hexadecimal notation using DosPutMessage
 * @param     n   ULONG to show.
 */
static void ShowHexNumber(unsigned long int n)
{
    signed int div;
    char sif;
    DosPutMessage(0, 2, "0x");
    for (div = 28; div >= 0; div -= 4)
    {
        sif = (char)(n >> div) & (char)0xF;
        sif += (sif < 10  ? '0' : 'a' - 10);
        DosPutMessage(0, 1, &sif);
    }
}



/**
 * Shows result of kernelprobing if not quiet or on error.
 * @param     rc          Return code.
 * @param     iSym        index of .sym-file into static struct.
 */
static void ShowResult(int rc, int iSym)
{
    int i, j;

    /* complain even if quiet on error */
    if (!fQuiet || rc != 0)
    {
        puts(szBanner);

        /* kernel stuff */
        puts(szMsg1);
        if (rc <= -50 || rc == 0)
        {
            puts(szOS2Krnl);
            puts(szMsg1a);
            ShowDecNumber(ulBuild);
            puts(" - v");
            ShowDecNumber(usVerMajor);
            puts(".");
            ShowDecNumber(usVerMinor);
        }
        else
            puts(szMsgfailed);

        /* functions */
        if (rc == 0)
        {
            puts(szMsg2);
            if (szUsrSym[0] == '\0')
                puts(apszSym[iSym]);
            else
                puts(szUsrSym);

            for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
            {
                if ((i % 2) == 0)
                    puts("\n\r    ");
                else
                    puts("    ");
                puts(aImportTab[i].achName);
                for (j = aImportTab[i].cchName; j < 20; j++)
                    puts(" ");

                puts(" at ");
                if (aImportTab[i].fFound)
                    ShowHexNumber(aImportTab[i].ulAddress);
                else
                    puts(szMsgfailed);
            }
        }
        else
            puts(szMsg4);
        puts("\n\r");
    }

    /* if error: write rc */
    if (rc != 0)
    {
        puts("rc = ");
        ShowHexNumber((unsigned long)rc);
        puts("\n\r");
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
    /* get kernel OTEs */
    /*-----------------*/
    rc = GetKernelOTEs();
    if (rc != NO_ERROR)
        return rc;

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
        if (rc != 0) /* if user sym failed or don't exists */
        {
            i = 0;
            while (apszSym[i] != NULL && (rc = ProbeSymFile(apszSym[i])) != 0)
                i++;
        }
    }

    /* show the result and set return-value */
    dprintf(("rc=%d; i=%d\n", rc, i));
    ShowResult(rc, i);

    return rc;
}



#ifdef DEBUGR3
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

