/* $Id: probkrnl.c,v 1.9 2000-02-18 19:27:29 bird Exp $
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
 * aProcTab defines the imported and overloaded OS/2 kernel functions.
 * IMPORTANT: aProcTab has a sibling array in d32init.c, aulProc, which must
 *            match entry by entry. Adding/removing/shuffling aProcTab, aulProc
 *            has to be updated immediately!
 */
PROCS aProcTab[NUMBER_OF_PROCS] =
{/* iFound     cchName                  offObject     fType */
 /*      iObject      achName                 ulAddress     */
    {FALSE, -1,  8, "_ldrRead",             -1,  -1, EPT_PROC},        /* 0 */
    {FALSE, -1,  8, "_ldrOpen",             -1,  -1, EPT_PROC},        /* 1 */
    {FALSE, -1,  9, "_ldrClose",            -1,  -1, EPT_PROC},        /* 2 */
    {FALSE, -1, 12, "_LDRQAppType",         -1,  -1, EPT_PROC},        /* 3 */
    {FALSE, -1, 20, "_ldrEnum32bitRelRecs", -1,  -1, EPT_PROC},        /* 4 */
    {FALSE, -1, 10, "_IOSftOpen",           -1,  -1, EPT_PROCIMPORT},  /* 5 */
    {FALSE, -1, 11, "_IOSftClose",          -1,  -1, EPT_PROCIMPORT},  /* 6 */
    {FALSE, -1, 15, "_IOSftTransPath",      -1,  -1, EPT_PROCIMPORT},  /* 7 */
    {FALSE, -1, 12, "_IOSftReadAt",         -1,  -1, EPT_PROCIMPORT},  /* 8 */
    {FALSE, -1, 13, "_IOSftWriteAt",        -1,  -1, EPT_PROCIMPORT},  /* 9 */
    {FALSE, -1, 11, "_VMAllocMem",          -1,  -1, EPT_PROCIMPORT},  /* 10 */
    {FALSE, -1, 11, "_VMGetOwner",          -1,  -1, EPT_PROCIMPORT},  /* 11 */
    {FALSE, -1, 11, "g_tkExecPgm",          -1,  -1, EPT_PROC}         /* 12 */
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
static void  kmemcpy(char *p1, const char *p2, int len);
static int   kstrcmp(const char *p1, const char *p2);
static int   kstrncmp(const char *p1, const char *p2, int len);
static int   kstrlen(const char *p);
static HFILE fopen(char * filename, char * ignored);
static int   fread(void * pBuffer, USHORT i1, USHORT i2,  HFILE hfile);
static int   fseek(HFILE hfile, signed long off, int org);
static unsigned long fsize(HFILE hFile);
static void  puts(char *psz);
static int   kargncpy(char *pszTarget, const char *pszArg, unsigned cchMaxlen);

static int   VerifyPrologs(void);
static int   ProbeSymFile(char *pszFilename);
static int   VerifyKernelVer(void);
static int   ReadOS2Krnl(char *filename);
static int   ReadOS2Krnl2(HFILE krnl, unsigned long  cbKrnl);
static int   GetKernelOTEs(void);

static void  ShowDecNumber(unsigned long n);
static void  ShowHexNumber(unsigned long int n);
static void  ShowResult(int rc, int iSym);



/**
 * Quick implementation of fopen.
 * @param    filename   name of file to open (sz)
 * @param    ignored    whatever - it is ignored
 * @return   Handle to file. (not pointer to a FILE stream as in C-library)
 * @remark   binary and readonly is assumed!
 */
static HFILE fopen(char * filename, char * ignored)
{
    HFILE   hFile = 0;
    USHORT  rc;
    unsigned short Action = 0;

    rc = DosOpen(
                filename,
                &hFile,
                &Action,
                0,
                FILE_NORMAL,
                OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_SHARE_DENYNONE + OPEN_ACCESS_READONLY,
                NULL
                );

    ignored = ignored;
    return hFile;
}


/**
 * fread emulation
 * @returns   Number of blocks read.
 * @param     pBuffer  Buffer to read into
 * @param     i1       blocksize
 * @param     i2       block count
 * @param     hFile    Handle to file (HFILE)
 */
static int fread(void * pBuffer, USHORT cbBlock, USHORT cBlock,  HFILE hFile)
{
    USHORT  ulRead;
    USHORT  rc;

    rc = DosRead(hFile, pBuffer, (USHORT)(cbBlock*cBlock), &ulRead);
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
static int fseek(HFILE hFile, signed long off, int org)
{
    USHORT rc;
    ULONG ole;

    rc = DosChgFilePtr(hFile, off, org, &ole);

    return rc;
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
 * @param     p1  target
 * @param     p2  source
 * @param     len length
 */
static void kmemcpy(char * p1, const char * p2, int len)
{
    while (len != 0)
    {
        *p1 = *p2;
        p1++;
        p2++;
        len--;
    }
}

#if 0 /* not in use */
/**
 * kstrcmp - String compare
 * @returns   0 - equal else !0
 * @param     p1  String 1
 * @param     p2  String 2
 */
static int kstrcmp(const char * p1, const char * p2)
{
    while (*p1 == *p2 && *p1 != '\0' && *p2 != '\0')
    {
        p1++;
        p2++;
    }
    return *p1 - *p2;
}
#endif

/**
 * kstrncmp - String 'n' compare.
 * @returns
 * @returns   0 - equal else !0
 * @param     p1  String 1
 * @param     p2  String 2
 * @param     len length
 */
static int kstrncmp(const char * p1, const char * p2, int len)
{
    int i = 0;
    while (i < len && *p1 == *p2 && *p1 != '\0' && *p2 != '\0')
    {
        p1++;
        p2++;
        i++;
    }

    return i - len;
}


/**
 * kstrlen - String length.
 * @returns   Length of the string.
 * @param     psz  Pointer to string.
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 */
static int kstrlen(const char * psz)
{
    int cch = 0;
    while (psz[cch] != '\0')
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


/**
 * Verifies the that the addresses in aProcTab are valid.
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
 * Check a symbole file. Searches for the wanted entry-point addresses.
 * @returns   0 on success - something else on failiure.
 * @param     pszFilename  Name of file to probe.
 * @precond   Must be called after kernel-file is found and processed.
 * @remark    Error codes starts at -50.
 */
static int ProbeSymFile(char * pszFilename)
{
    HFILE          SymFile;
    MAPDEF         MapDef;
    SEGDEF         SegDef;
    SYMDEF32       SymDef32;
    SYMDEF16       SymDef16;
    char           Buffer[256];
    unsigned long  SymNum;
    unsigned long  SegOffset, SymOffset, SymPtrOffset;
    unsigned long  i;
    unsigned long  ulSegments;
    int            LeftToFind;
    int            rc;

    /* open symbole file */
    SymFile = fopen(pszFilename, "rb");
    if (SymFile==0)
    {
        dprintf(("Error opening file %s\n", pszFilename));
        return -50;
    }

    /* read header and display it */
    rc = fread(&MapDef, sizeof(MAPDEF), 1, SymFile);
    if (rc)
    {
        Buffer[0] = MapDef.achModName[0];
        fread(&Buffer[1], 1, MapDef.cbModName, SymFile);
        Buffer[MapDef.cbModName] = '\0';
        dprintf(("*Module name: %s\n", Buffer));
        dprintf(("*Segments: %d\n*MaxSymbolLength: %d\n", MapDef.cSegs, MapDef.cbMaxSym));
        dprintf(("*ppNextMap: 0x%x\n\n", MapDef.ppNextMap ));
    }
    else
    {
        fclose(SymFile);
        return -51;
    }

    /* verify module name */
    if (MapDef.cbModName == 7 && kstrncmp(Buffer, "OS2KRNL", 7) != 0)
    {
        dprintf(("Modulename verify failed\n"));
        fclose(SymFile);
        return -51;
    }

    /* verify correct number of segments */
    ulSegments = MapDef.cSegs;
    if (ulSegments != KrnlOTEs.cObjects)
    {
        dprintf(("Segment No. verify failed\n"));
        fclose(SymFile);
        return -52;
    }

    SegOffset= SEGDEFOFFSET(MapDef);
    /* skip to last segment - ASSUMES all imports located in 32-bit code segment. */
    for (i = 0; i < ulSegments; i++ )
    {
        if (fseek(SymFile, SegOffset, SEEK_SET))
        {
            fclose(SymFile);
            return -53;
        }
        rc = fread(&SegDef, sizeof(SEGDEF), 1, SymFile);
        if (i+1 < ulSegments)
            SegOffset = NEXTSEGDEFOFFSET(SegDef);
    }

    Buffer[0] = SegDef.achSegName[0];
    rc = fread(&Buffer[1], 1, SegDef.cbSegName, SymFile);

    /* verify that this is DOSHIGH32CODE */
    if (SegDef.cbSegName != 13 && kstrncmp(Buffer, "DOSHIGH32CODE", 13) != 0)
    {
        dprintf(("DOSHIGH32CODE verify failed \n"));
        fclose(SymFile);
        return -54;
    }

    for (i = 0; i < NUMBER_OF_PROCS; i++)
        aProcTab[i].fFound = 0 ;

    /* search for the entry-point names */
    for (LeftToFind = NUMBER_OF_PROCS, SymNum = 0; SymNum < SegDef.cSymbols && LeftToFind; SymNum++)
    {
        SymPtrOffset = SYMDEFOFFSET(SegOffset, SegDef, SymNum);
        rc = fseek(SymFile, SymPtrOffset, SEEK_SET);
        rc = fread(&SymOffset, sizeof(unsigned short int), 1, SymFile);
        SymOffset &=0xffff;
        rc = fseek(SymFile, SymOffset+SegOffset, SEEK_SET);

        if (SegDef.bFlags & 0x01)
        {
            rc = fread(&SymDef32, sizeof(SYMDEF32), 1, SymFile);
            Buffer[0] = SymDef32.achSymName[0];
            rc = fread(&Buffer[1], 1, SymDef32.cbSymName, SymFile);
            Buffer[SymDef32.cbSymName] = '\0';
        } else
        {
            rc = fread(&SymDef16, sizeof(SYMDEF16), 1, SymFile);
            Buffer[0] = SymDef16.achSymName[0];
            rc = fread(&Buffer[1], 1, SymDef16.cbSymName, SymFile);
            Buffer[SymDef16.cbSymName] = '\0';
        }

        /* search proctable */
        for (i = 0; i < NUMBER_OF_PROCS; i++)
        {
            if (!aProcTab[i].fFound && aProcTab[i].cchName == (SegDef.bFlags & 0x01 ? SymDef32.cbSymName : SymDef16.cbSymName))
            {
                if (kstrncmp(aProcTab[i].achName, Buffer, aProcTab[i].cchName) == 0)
                {
                    aProcTab[i].offObject = (SegDef.bFlags & 0x01 ? SymDef32.wSymVal : SymDef16.wSymVal);
                    aProcTab[i].ulAddress= aProcTab[i].offObject + KrnlOTEs.aObjects[KrnlOTEs.cObjects-1].ote_base;

                    if (aProcTab[i].offObject < KrnlOTEs.aObjects[KrnlOTEs.cObjects-1].ote_size)
                    {
                        aProcTab[i].fFound = TRUE;
                        LeftToFind--;
                        dprintf(("Found: %s at off 0x%lx addr 0x%lx\n", aProcTab[i].achName, aProcTab[i].offObject, aProcTab[i].ulAddress));
                        break;
                    }
                    else
                    {
                        fclose(SymFile);
                        return -56;
                    }
                }
            }
        }
    }

    fclose(SymFile);
    return LeftToFind == 0 ? VerifyPrologs() : -57;
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
 */
static int ReadOS2Krnl(char * filename)
{
    HFILE          krnl;
    unsigned long  cbKrnl;
    int            rc;

    krnl = fopen(filename, "rb");
    if (krnl != 0)
    {
        cbKrnl = fsize(krnl);
        if (!fseek(krnl, 0, SEEK_SET))
            rc = ReadOS2Krnl2(krnl, cbKrnl);
        else
            rc = -2;
        fclose(krnl);
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
 * @param
 * @equiv
 * @time
 * @sketch
 * @status
 * @author    knut st. osmundsen
 * @remark
 */
static int ReadOS2Krnl2(HFILE krnl, unsigned long  cbKrnl)
{
    int            i, j;
    int            rc = 0;
    char           Buffer[KERNEL_ID_STRING_LENGTH + KERNEL_READ_SIZE];
    unsigned long  offLXHdr;
    struct e32_exe *pLXHdr;


    /* find bldlevel string - "@#IBM:14.020#@  IBM OS/2 Kernel - 14.020F" */
    cbKrnl = fsize(krnl);
    if (fseek(krnl, 0, SEEK_SET))
        return -2;

    if (!fread(&Buffer[KERNEL_ID_STRING_LENGTH], 1, KERNEL_READ_SIZE, krnl))
        return -3;

    i = KERNEL_ID_STRING_LENGTH;
    while (cbKrnl > 0)
    {
        if (i == KERNEL_READ_SIZE)
        {

            kmemcpy(Buffer, &Buffer[KERNEL_READ_SIZE], KERNEL_ID_STRING_LENGTH);
            if (!fread(&Buffer[KERNEL_ID_STRING_LENGTH], 1, cbKrnl > KERNEL_READ_SIZE ? KERNEL_READ_SIZE : (int)cbKrnl, krnl))
                return -3;

            i = 0;
        }

        if (kstrncmp("@#IBM:", &Buffer[i], 6) == 0)
            break;

        /* next */
        i++;
        cbKrnl--;
    }

    if (cbKrnl == 0)
    {
        fclose(krnl);
        return -4;
    }

    /* displacement */
    j = 0;
    while (j < 6 && Buffer[i+10+j] != '#')
        j++;

    /* verify signature */
    if (kstrncmp(&Buffer[i+10+j], "#@  IBM OS/2 Kernel", 19) != 0)
        return -5;

    /* read ulBuild */
    ulBuild  = (char)(Buffer[i+6] - '0') * 1000;
    if (Buffer[i+7] != '.')
    {
        /* this code is for Warp5 */
        ulBuild *= 10;
        ulBuild += (char)(Buffer[i+7] - '0') * 1000;
        i++;
        j--;
        if (Buffer[i+7] != '.')
        {
            ulBuild = ulBuild * 10;
            ulBuild = ulBuild + (unsigned long)(Buffer[i+7] - '0') * 1000;
            i++;
            j--;
        }
    }

    if (j == 0)
    {
        ulBuild += (Buffer[i+ 8] - '0') * 10;
        ulBuild += (Buffer[i+ 9] - '0') * 1;
    } else
    {
        if (j == 3)
            return -9;
        ulBuild += (Buffer[i+ 8] - '0') * 100;
        ulBuild += (Buffer[i+ 9] - '0') * 10;
        ulBuild += (Buffer[i+10] - '0');
    }

    if (VerifyKernelVer())
        return -9;
    dprintf(("ulBuild: %d\n",ulBuild));

    /* get segment number */
    /* read-MZheader */
    if (fseek(krnl,0,SEEK_SET))
        return -2;

    if (!fread(Buffer, 1, 0x40, krnl))
        return -3;

    offLXHdr = *(unsigned long int *)&Buffer[0x3c];

    if (offLXHdr > 0x2000 && offLXHdr < 0x80) /* just to detect garbage */
        return -6;

    if (fseek(krnl, offLXHdr, SEEK_SET))
        return -2;

    if (!fread(Buffer, 1, sizeof(struct e32_exe), krnl))
        return -3;

    /* check LX-magic */
    if (Buffer[0] != 'L' || Buffer[1] != 'X')
        return -7;

#ifndef DEBUGR3
    /* check object count - match it with what we got from the kernel. */
    pLXHdr = (struct e32_exe *)Buffer;
    if ((UCHAR)pLXHdr->e32_objcnt != KrnlOTEs.cObjects)
        return -8;

    if (pLXHdr->e32_objcnt < 10)
        return -9;

    /* check objects (sizes and flags(?)) */
    if (!fseek(krnl, (LONG)offLXHdr + (LONG)pLXHdr->e32_objtab, SEEK_SET))
    {
        struct o32_obj *pObj = (struct o32_obj *)Buffer;
        for (i = 0; i < (int)KrnlOTEs.cObjects; i++)
        {
            if (!fread(Buffer, 1, sizeof(OTE), krnl))
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
    pLXHdr = (struct e32_exe *)Buffer;
    KrnlOTEs.cObjects = (UCHAR)pLXHdr->e32_objcnt;

    /* get OTEs */
    if (!fseek(krnl, (LONG)offLXHdr + (LONG)pLXHdr->e32_objtab, SEEK_SET))
    {
        struct o32_obj *pObj = (struct o32_obj *)Buffer;
        for (i = 0; i < (int)KrnlOTEs.cObjects; i++)
            if (!fread(&KrnlOTEs.aObjects[i], 1, sizeof(struct o32_obj), krnl))
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

            for (i = 0; i < NUMBER_OF_PROCS; i++)
            {
                if ((i % 2) == 0)
                    puts("\n\r    ");
                else
                    puts("    ");
                puts(aProcTab[i].achName);
                for (j = aProcTab[i].cchName; j < 20; j++)
                    puts(" ");

                puts(" at ");
                if (aProcTab[i].fFound)
                    ShowHexNumber(aProcTab[i].ulAddress);
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

