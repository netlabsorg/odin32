/* $Id: probkrnl.c,v 1.29 2000-12-02 23:28:51 bird Exp $
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
 *                2. gets the kernel object table and kernel info like build no. (elf$)
 *                3. if non-debug kernel the symbol database is scanned to get the syms
 *                4. if Syms not found THEN locates and scans the symbol-file(s) for the
 *                   entrypoints which are wanted.
 *                5. the entry points are verified. (elf$)
 *                6. finished.
 *
 * Copyright (c) 1998-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
#define INCL_NOPMAPI

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
#include "probkrnl.h"
#include "dev16.h"
#include "dev1632.h"
#include "vprntf16.h"
#include "log.h"
#include "options.h"
#include "errors.h"

/*******************************************************************************
*   Global Variables                                                           *
*   Note: must be inited or else we'll get BSS segment                         *
*******************************************************************************/


/*
 * aImportTab defines the imported and overloaded OS/2 kernel functions.
 * IMPORTANT: aImportTab has three sibling arrays, two in dev32\d32init.c (aulProc
 *            and aTstFakers), and the calltab.asm, which must match entry by entry.
 *  - obsolete warning -
 *            When adding/removing/shuffling items in aImportTab, aulProc and
 *            calltab.asm has to be updated immediately!
 *            Use the mkcalltab.exe to generate calltab.asm and aTstFakers.
 *  - obsolete warning -
 *            We'll now generate both of these files from this table.
 *
 */
IMPORTKRNLSYM DATA16_GLOBAL aImportTab[NBR_OF_KRNLIMPORTS] =
{/* iFound     cchName                          offObject   usSel     fType    */
 /*      iObject      achName            achExtra     ulAddress  cProlog        */
    /* Overrides */
    {FALSE, -1, 12, "_ldrOpenPath",         "@20", -1,  -1,  -1,  -1, EPT_PROC32 | EPT_WRAPPED}, /* Must be [0]! See importTabInit. */
    {FALSE, -1,  8, "_ldrRead",             "@24", -1,  -1,  -1,  -1, EPT_PROC32},
    {FALSE, -1,  8, "_ldrOpen",             "@12", -1,  -1,  -1,  -1, EPT_PROC32},
    {FALSE, -1,  9, "_ldrClose",            "@4",  -1,  -1,  -1,  -1, EPT_PROC32},
    {FALSE, -1, 12, "_LDRQAppType",         "@8",  -1,  -1,  -1,  -1, EPT_PROC32},
    {FALSE, -1, 20, "_ldrEnum32bitRelRecs", "@24", -1,  -1,  -1,  -1, EPT_PROC32},
    {FALSE, -1, 11, "g_tkExecPgm",          "",    -1,  -1,  -1,  -1, EPT_PROC32},
    {FALSE, -1, 15, "_tkStartProcess",      "",    -1,  -1,  -1,  -1, EPT_PROC32},
    {FALSE, -1, 12, "_LDRClearSem",         "@0",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 21, "_ldrASMpMTEFromHandle","@4",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 14, "_ldrFindModule",       "@16", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 21, "_ldrValidateMteHandle","@4",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 11, "_VMAllocMem",          "@36", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 10, "_VMFreeMem",           "@12", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 11, "_VMGetOwner",          "@8",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 16, "_VMObjHandleInfo",     "@12", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 16, "_VMMapDebugAlias",     "@20", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 17, "_KSEMRequestMutex",    "@8",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 17, "_KSEMReleaseMutex",    "@4",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 15, "_KSEMQueryMutex",      "@8",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 12, "_TKPidToPTDA",         "@8",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1,  9, "_TKSuBuff",            "@16", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1,  9, "_TKFuBuff",            "@16", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 11, "_TKFuBufLen",          "@20", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 11, "_TKSuFuBuff",          "@16", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 11, "f_FuStrLenZ",          "",    -1,  -1,  -1,  -1, EPT_PROCIMPORT16},
    {FALSE, -1, 10, "f_FuStrLen",           "",    -1,  -1,  -1,  -1, EPT_PROCIMPORT16},
    {FALSE, -1,  8, "f_FuBuff",             "",    -1,  -1,  -1,  -1, EPT_PROCIMPORT16},
    {FALSE, -1, 12, "_SftFileSize",         "@8",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 16, "_ldrpFileNameBuf",     "",    -1,  -1,  -1,  -1, EPT_VARIMPORT32},
    {FALSE, -1,  7, "_LdrSem",              "",    -1,  -1,  -1,  -1, EPT_VARIMPORT32},
    {FALSE, -1,  8, "_pTCBCur",             "",    -1,  -1,  -1,  -1, EPT_VARIMPORT16},
    {FALSE, -1,  9, "_pPTDACur",            "",    -1,  -1,  -1,  -1, EPT_VARIMPORT16},
    {FALSE, -1, 10, "ptda_start",           "",    -1,  -1,  -1,  -1, EPT_VARIMPORT16},
    {FALSE, -1, 12, "ptda_environ",         "",    -1,  -1,  -1,  -1, EPT_VARIMPORT16},
    {FALSE, -1, 12, "ptda_ptdasem",         "",    -1,  -1,  -1,  -1, EPT_VARIMPORT16},
    {FALSE, -1, 11, "ptda_handle",          "",    -1,  -1,  -1,  -1, EPT_VARIMPORT16},
    {FALSE, -1, 11, "ptda_module",          "",    -1,  -1,  -1,  -1, EPT_VARIMPORT16},
    {FALSE, -1, 18, "ptda_pBeginLIBPATH",   "",    -1,  -1,  -1,  -1, EPT_VARIMPORT16},
    {FALSE, -1, 14, "SecPathFromSFN",       "",    -1,  -1,  -1,  -1, EPT_PROCIMPORTNR32},
#if 0 /* not used */
    {FALSE, -1,  9, "_KSEMInit",            "@12", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 10, "_IOSftOpen",           "@20", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 11, "_IOSftClose",          "@4",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 15, "_IOSftTransPath",      "@4",  -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 12, "_IOSftReadAt",         "@20", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 13, "_IOSftWriteAt",        "@20", -1,  -1,  -1,  -1, EPT_PROCIMPORT32},
    {FALSE, -1, 11, "_LDRLibPath",          "",    -1,  -1,  -1,  -1, EPT_VARIMPORT32},
#endif
#if 0/* experimenting...*/
    {FALSE, -1, 14, "_ldrSetVMflags",       "@16", -1,  -1,  -1,  -1, EPT_PROC32},
#endif
};

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
 * privat data
 */
static char *   DATA16_INIT apszSym[]        =
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
unsigned char DATA16_INIT  cObjects = 0;
POTE          DATA16_INIT  paKrnlOTEs = NULL;


/*
 *
 */
static struct
{
    short       sErr;
    const char *pszMsg;
} DATA16_INIT aErrorMsgs[] =
{
    {ERROR_PROB_KRNL_OPEN_FAILED,       "Krnl: Failed to open kernel file."},
    {ERROR_PROB_KRNL_SEEK_SIZE,         "Krnl: Failed to seek to end to of file."},
    {ERROR_PROB_KRNL_SEEK_FIRST,        "Krnl: Failed to start of file."},
    {ERROR_PROB_KRNL_READ_FIRST,        "Krnl: Failed to read (first)."},
    {ERROR_PROB_KRNL_READ_NEXT,         "Krnl: Failed to read."},
    {ERROR_PROB_KRNL_TAG_NOT_FOUND,     "Krnl: Build level tag was not found."},
    {ERROR_PROB_KRNL_INV_SIGANTURE,     "Krnl: Invalid build level signature."},
    {ERROR_PROB_KRNL_INV_BUILD_NBR,     "Krnl: Invalid build level number."},
    {ERROR_PROB_KRNL_BUILD_VERSION,     "Krnl: Invalid build level version."},
    {ERROR_PROB_KRNL_MZ_SEEK,           "Krnl: Failed to seek to start of file. (MZ)"},
    {ERROR_PROB_KRNL_MZ_READ,           "Krnl: Failed to read MZ header."},
    {ERROR_PROB_KRNL_NEOFF_INVALID,     "Krnl: Invalid new-header offset in MZ header."},
    {ERROR_PROB_KRNL_NEOFF_SEEK,        "Krnl: Failed to seek to new-header offset."},
    {ERROR_PROB_KRNL_LX_READ,           "Krnl: Failed to read LX header."},
    {ERROR_PROB_KRNL_LX_SIGNATURE,      "Krnl: Invalid LX header signature."},
    {ERROR_PROB_KRNL_OBJECT_CNT,        "Krnl: Object count don't match the running kernel."},
    {ERROR_PROB_KRNL_OBJECT_CNR_10,     "Krnl: Less than 10 objects - not a valid kernel file!"},
    {ERROR_PROB_KRNL_OTE_SEEK,          "Krnl: Failed to seek to OTEs."},
    {ERROR_PROB_KRNL_OTE_READ,          "Krnl: Failed to read OTEs."},
    {ERROR_PROB_KRNL_OTE_SIZE_MIS,      "Krnl: Size of a OTE didn't match the running kernel."},

    /*
     * ProbeSymFile error messages + some extra ones.
     */
    {ERROR_PROB_SYM_FILE_NOT_FOUND,     "Sym: Symbol file was not found."},
    {ERROR_PROB_SYM_READERROR,          "Sym: Read failed."},
    {ERROR_PROB_SYM_INVALID_MOD_NAME,   "Sym: Invalid module name (not OS2KRNL)."},
    {ERROR_PROB_SYM_SEGS_NE_OBJS,       "Sym: Number of segments don't match the object count of the kernel."},
    {ERROR_PROB_SYM_SEG_DEF_SEEK,       "Sym: Failed to seek to a segment definition."},
    {ERROR_PROB_SYM_SEG_DEF_READ,       "Sym: Failed to read a segment definition."},
    {ERROR_PROB_SYM_IMPORTS_NOTFOUND,   "Sym: Some of the imports wasn't found."},
    {ERROR_PROB_SYM_V_PROC_NOT_FND,     "Sym: Verify failed: Procedure not found."},
    {ERROR_PROB_SYM_V_OBJ_OR_ADDR,      "Sym: Verify failed: Invalid object or address."},
    {ERROR_PROB_SYM_V_ADDRESS,          "Sym: Verify failed: Invalid address."},
    {ERROR_PROB_SYM_V_PROLOG,           "Sym: Verify failed: Invalid prolog."},
    {ERROR_PROB_SYM_V_NOT_IMPL,         "Sym: Verify failed: Not implemented."},
    {ERROR_PROB_SYM_V_GETOS2KRNL,       "GetOs2Krnl: failed."},
    {ERROR_PROB_SYM_V_NO_SWAPMTE,       "GetOs2Krnl: No Swap MTE."},
    {ERROR_PROB_SYM_V_OBJECTS,          "GetOs2Krnl: Too many objects."},
    {ERROR_PROB_SYM_V_OBJECT_TABLE,     "GetOs2Krnl: No object table."},
    {ERROR_PROB_SYM_V_BUILD_INFO,       "GetOs2Krnl: Build info not found."},
    {ERROR_PROB_SYM_V_INVALID_BUILD,    "GetOs2Krnl: Unsupported build."},
    {ERROR_PROB_SYM_V_VERIFY,           "importTabInit: Import failed."},
    {ERROR_PROB_SYM_V_IPE,              "importTabInit: Internal-Processing-Error."},
    {ERROR_PROB_SYM_V_HEAPINIT,         "R0Init32: HeapInit Failed."},
    {ERROR_PROB_SYM_V_D32_LDR_INIT,     "R0Init32: ldrInit Failed."},

    {ERROR_PROB_SYMDB_KRNL_NOT_FOUND,   "SymDB: Kernel was not found."}
};

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


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
/* File an output replacements */
HFILE    fopen(const char * pszFilename, const char * pszIgnored);
int      fread(void * pvBuffer, USHORT cbBlock, USHORT cBlock,  HFILE hFile);
int      fseek(HFILE hfile, signed long off, int iOrg);
unsigned long fsize(HFILE hFile);

/* C-library replacements and additions. */
void     kmemcpy(char *psz1, const char *psz2, int cch);
char *   kstrstr(const char *psz1, const char *psz2);
int      kstrcmp(const char *psz1, const char *psz2);
int      kstrncmp(const char *psz1, const char *psz2, int cch);
int      kstrnicmp(const char *psz1, const char *psz2, int cch);
int      kstrlen(const char *psz);
char *   kstrcpy(char * pszTarget, const char * pszSource);
int      kargncpy(char *pszTarget, const char *pszArg, unsigned cchMaxlen);

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

/**
 * Quick implementation of fopen.
 * @param    pszFilename   name of file to open (sz)
 * @param    pszIgnored    whatever - it is ignored
 * @return   Handle to file. (not pointer to a FILE stream as in C-library)
 * @remark   binary and readonly is assumed!
 */
HFILE fopen(const char * pszFilename, const char * pszIgnored)
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
int fread(void * pvBuffer, USHORT cbBlock, USHORT cBlock,  HFILE hFile)
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
int fseek(HFILE hFile, signed long off, int iOrg)
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
unsigned long fsize(HFILE hFile)
{
    USHORT rc;
    unsigned long cb;

    rc = DosChgFilePtr(hFile, 0, FILE_END, &cb);

    return cb;
}


/**
 * kmemcpy - memory copy - slow!
 * @param     psz1  target
 * @param     psz2  source
 * @param     cch length
 */
void     kmemcpy(char *psz1, const char *psz2, int cch)
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
char *kstrstr(const char *psz1, const char *psz2)
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
int      kstrcmp(const char *psz1, const char *psz2);
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
int      kstrncmp(register const char *psz1, register const char *psz2, int cch)
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
int      kstrnicmp(const char *psz1, const char *psz2, int cch)
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
int kstrlen(register const char * psz)
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
char * kstrcpy(char * pszTarget, register const char * pszSource)
{
    register char *psz = pszTarget;

    while (*pszSource != '\0')
        *psz++ = *pszSource++;
    *psz = '\0';
    return pszTarget;
}




/**
 * Copy an argument to a buffer. Ie. "-K[=|:]c:\os2krnl ....". Supports quotes
 * @returns   Number of chars of pszArg that has been processed.
 * @param     pszTarget  -  pointer to target buffer.
 * @param     pszArg     -  pointer to source argument string.
 * @param     cchMaxlen  -  maximum chars to copy.
 */
int kargncpy(char * pszTarget, const char * pszArg, unsigned cchMaxlen)
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


/**
 * Get the message text for an error message.
 * @returns Pointer to error text. NULL if not found.
 * @param   sErr  Error code id.
 * @status  completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
const char * GetErrorMsg(short sErr)
{
    int i;
    for (i = 0; i < sizeof(aErrorMsgs) / sizeof(aErrorMsgs[0]); i++)
    {
        if (aErrorMsgs[i].sErr == sErr)
            return aErrorMsgs[i].pszMsg;
    }
    return NULL;
}



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
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int LookupKrnlEntry(unsigned short usBuild, unsigned short fKernel, unsigned char cObjects)
{
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
            kstrcpy(szSymbolFile, "Win32k Symbol Database");

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
                printf16("Warning: The Win32k Symbol Database entry found.\n"
                         "         But, VerifyPrologs() returned rc=0x%x and iProc=%d\n", rc, iProc);
                DosSleep(3000);
            }

            return rc;
        }
    }

    /* not found */
    return ERROR_PROB_SYMDB_KRNL_NOT_FOUND;
}


/**
 * Verifies the that the addresses in aImportTab are valid.
 * This is done at Ring-0 of course.
 * @returns     NO_ERROR (ie. 0) on success. iProc = -1
 *              The appropriate OS/2 or Win32k return code on success. iProc
 *              is set to the failing procedure (if appliable).
 */
int VerifyPrologs(void)
{
#if !defined(EXTRACT)
    APIRET          rc;
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;

    /* Set the failing procedure number to -1. */
    iProc = -1;

    /* Open the elf device driver. */
    rc = DosOpen("\\dev\\elf$", &hDev0, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
    if (rc == NO_ERROR)
    {
        D16VERIFYIMPORTTABDATA Data = {0};

        /* Issue the VerifyImportTab IOCtl call. */
        rc = DosDevIOCtl(&Data, "", D16_IOCTL_VERIFYIMPORTTAB, D16_IOCTL_CAT, hDev0);
        DosClose(hDev0);
        if (rc == NO_ERROR)
        {
            if (Data.usRc != NO_ERROR)
            {
                /* set the appropriate return values. */
                rc = (Data.usRc & ERROR_D32_ERROR_MASK) + ERROR_PROB_SYM_D32_FIRST;
                if (Data.usRc & ERROR_D32_PROC_FLAG)
                    iProc = (Data.usRc & ERROR_D32_PROC_MASK) >> ERROR_D32_PROC_SHIFT;
            }/* else success */
        }
        else
        {
            dprintf(("DosDevIOCtl failed with rc=%d\n", rc));
            DosSleep(3000);
        }
    }
    else
    {
        dprintf(("DosOpen Failed with rc=%d\n", rc));
        DosSleep(3000);
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
    #ifndef EXTRACT
    if (MapDef.cSegs != cObjects)
    {   /* incorrect count of segments. */
        dprintf(("Segment No. verify failed\n"));
        fclose(hSym);
        return ERROR_PROB_SYM_SEGS_NE_OBJS;
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
                    && (pImport->fType & EPT_VARIMPORT ? !fCode : fCode)   /* Don't look for code in a data segment */
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
                    #ifndef EXTRACT
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
 *              options.ulBuild, fchType, fDebug, cObjects and paKrnlOTEs is set on successful return.
 *          Not 0 on error.
 */
int   GetKernelInfo(void)
{
#if !defined(EXTRACT) /* This IOCtl is not available after inittime! */
    static KRNLINFO DATA16_INIT KrnlInfo = {0};
    APIRET          rc;
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;

    /*
     * Issue an IOCtl to elf$ to query kernel information.
     */
    rc = DosOpen("\\dev\\elf$", &hDev0, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
    if (rc == NO_ERROR)
    {
        rc = DosDevIOCtl(&KrnlInfo, "", D16_IOCTL_GETKRNLINFO, D16_IOCTL_CAT, hDev0);
        if (rc == NO_ERROR)
        {
            #ifdef DEBUG
            unsigned i;
            #endif

            /*
             * Set the exported parameters
             */
            options.ulBuild = KrnlInfo.ulBuild;
            options.fKernel = KrnlInfo.fKernel;
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
        DosClose(hDev0);
    }

    if (rc != NO_ERROR)
        printf16("Failed to get kernel OTEs. rc=%d\n", rc);

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
        printf16("Win32k - Odin32 support driver. (Built %s %s)\n",
                 (NPSZ)szBuildTime, (NPSZ)szBuildDate);

        /*
         * kernel stuff
         */
        if (rc == NO_ERROR || rc > ERROR_PROB_KRNL_LAST)
            printf16("    Build:            %ld - v%d.%d\n",
                     options.ulBuild, options.usVerMajor, options.usVerMinor);
        else if (rc >= ERROR_PROB_KRNL_FIRST)
            printf16("    Kernel probing failed with rc=%d.\n", rc);
        else
            printf16("    Failed before probing kernel.\n");

        /*
         * symbol-file
         */
        if (rc == NO_ERROR || (rc > ERROR_PROB_SYM_LAST && szSymbolFile[0] != '\0'))
            printf16("    Found symbolfile: %s\n", (NPSZ)szSymbolFile);
        else if (rc >= ERROR_PROB_SYM_FIRST)
            printf16("    Failed to find symbolfile!\n");
        else
            printf16("    Failed before searching for symbolfile.\n");

        /*
         * function listing
         */
        if (options.fLogging)/* || rc != NO_ERROR)*/
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

        /*
         * Display error code.
         */
        if (rc != NO_ERROR)
        {
            const char *psz = GetErrorMsg(rc);
            printf16("ProbeKernel failed with rc=%d. iProc=%x\n", rc, iProc);
            if (psz) printf16("%s\n", psz);
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
    SEL         selGIS;
    SEL         selLIS;
    PGINFOSEG   pGIS;
    PLINFOSEG   pLIS;
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
                    case 'n':
                    case 'N': /* NoLoader */
                        options.fNoLoader = TRUE;
                        return 0;

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
    options.usVerMajor  = pGIS->uchMajorVersion;
    options.usVerMinor  = pGIS->uchMinorVersion;
#else
    if (usFakeVerMajor == 0)
    {
        usFakeVerMajor = pGIS->uchMajorVersion;
        usFakeVerMinor = pGIS->uchMinorVersion;
    }
    options.usVerMajor  = usFakeVerMajor;
    options.usVerMinor  = usFakeVerMinor;
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
                printf16("Warning: Invalid symbol file specified. rc=%x iProc=%d\n"
                         "         Tries defaults.\n", rc, iProc);
                szSymbolFile[0] = '\0';
                DosSleep(3000);
            }
        }
        if (rc != NO_ERROR) /* if user sym failed or don't exists. */
        {
            /*
             * Check database - only if not a debug kernel!
             * You usually have a .sym-file when using a debug kernel.
             * (This is because I am unable to distinguish between half and
             *  all strict kernels...)
             */
            if ((options.fKernel & KF_DEBUG) ||
                (rc = LookupKrnlEntry((unsigned short)options.ulBuild,
                                      (unsigned short)options.fKernel,
                                      cObjects)
                 ) != NO_ERROR
                )
            {
                #if 1 /* ndef R3TST */
                APIRET rc2;
                /* search on disk */
                i = 0;
                while (apszSym[i] != NULL
                       && (rc2 = ProbeSymFile(apszSym[i])) != NO_ERROR
                       )
                {
                    i++;
                    if (rc2 >= ERROR_PROB_SYM_D32_FIRST)
                        rc = rc2;
                }
                if (rc2 == NO_ERROR)
                {
                    kstrcpy(szSymbolFile, apszSym[i]);
                    rc = NO_ERROR;
                }
                #endif
            }
        }
    }

    /* Show the result and set return-value */
    dprintf(("rc=%d(0x%x); i=%d; iProc=%d\n", rc, rc, i, iProc));
    ShowResult(rc);

    return rc;
}

