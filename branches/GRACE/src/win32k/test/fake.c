/* $Id: fake.c,v 1.1.4.1 2000-08-11 02:21:09 bird Exp $
 *
 * Fake stubs for the ldr and kernel functions we imports or overloads.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_BASE
#define FOR_EXEHDR 1                    /* To make all object flags OBJ???. */
#define INCL_OS2KRNL_ALL
#define DWORD  ULONG
#define WORD   USHORT

#define DUMMY()     int dummy; dummy = 0; dummy = dummy + 1;

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <newexe.h>                     /* OS/2 NE structs and definitions. */
#include <exe386.h>                     /* OS/2 LX structs and definitions. */

#include <string.h>

#include "log.h"
#include "OS2Krnl.h"
#include "dev32.h"
#include "ldrcalls.h"
#include "test.h"
#include "macros.h"

#include "malloc.h"

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct ldrrei_s /* #memb 7 size 20 (0x014) - from 14040W4 kernel.sdf */
{
    PVOID                   ei_startaddr;      /* off:   0(00)  size:  4(04) */
    PVOID                   ei_stackaddr;      /* off:   4(04)  size:  4(04) */
    PVOID                   ei_pstackmax;      /* off:   8(08)  size:  4(04) */
    USHORT                  ei_ds;             /* off:  12(0c)  size:  2(02) */
    USHORT                  ei_stacksize;      /* off:  14(0e)  size:  2(02) */
    USHORT                  ei_heapsize;       /* off:  16(10)  size:  2(02) */
    USHORT                  ei_loadtype;       /* off:  18(12)  size:  2(02) */
} ldrrei_t;



/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*
 * Pointer to the loader semaphore.
 */
static LONG     lfakeLDRSem = 0;        /* Used as a counter. */
HKMTX           fakeLDRSem = (HKMTX)&lfakeLDRSem;

static CHAR     achHeaderBuffer[256];   /* Buffer to read exe header into. */
PVOID           pheaderbuf = &achHeaderBuffer[0];

/*
 * table use by fakeldrMTEValidatePtrs.
 */
USHORT          validatetbl[] =
{
    FIELDOFFSET(SMTE, smte_objmap),
    FIELDOFFSET(SMTE, smte_rsrctab),
    FIELDOFFSET(SMTE, smte_restab),
    FIELDOFFSET(SMTE, smte_enttab),
    FIELDOFFSET(SMTE, smte_fpagetab),
    FIELDOFFSET(SMTE, smte_frectab),
    FIELDOFFSET(SMTE, smte_impmod),
    FIELDOFFSET(SMTE, smte_impproc),
    0xDEAD
};


/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
#define QHINF_EXEINFO       1 /* NE exeinfo. */
#define QHINF_READRSRCTBL   2 /* Reads from the resource table. */
#define QHINF_READFILE      3 /* Reads from the executable file. */
#define QHINF_LIBPATHLENGTH 4 /* Gets the libpath length. */
#define QHINF_LIBPATH       5 /* Gets the entire libpath. */
#define QHINF_FIXENTRY      6 /* NE only */
#define QHINF_STE           7 /* NE only */
#define QHINF_MAPSEL        8 /* NE only */

APIRET APIENTRY DosQueryHeaderInfo(HMODULE hmod,
                                   ULONG   ulIndex,
                                   PVOID   pvBuffer,
                                   ULONG   cbBuffer,
                                   ULONG   ulSubFunction);

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
ULONG LDRCALL   fakeLDRLoadExe(PSZ pszFilename, ldrrei_t *pEI);
ULONG LDRCALL   fakeldrGetModule(PSZ pszFilename, ULONG ul);
ULONG LDRCALL   fakeldrGetMte(PCHAR pachFilename, USHORT cchFilename, UCHAR fchType, UCHAR fchClass, PPMTE ppmte);
ULONG LDRCALL   fakeldrOpenNewExe(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PUSHORT pus);
ULONG LDRCALL   fakeldrCreateMte(struct e32_exe * pe32, ldrlv_t *plv);
ULONG LDRCALL   fakeldrLoadImports(PMTE pmte);
VOID  LDRCALL   fakeldrUCaseString(PCHAR pachString, USHORT cchString);
ULONG LDRCALL   fakeldrMTEValidatePtrs(PSMTE psmte, ULONG ulMaxAddr, ULONG off);


/**
 * Fake ldrClose
 * @returns OS2 return code.
 * @param   hFile    Filehandle of the file to be closed.
 * @status  completely impelemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL fakeldrClose(SFN hFile)
{
    APIRET  rc;
    BOOL    f32Stack = ((int)&hFile > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    rc =  DosClose(hFile);

    if (!f32Stack) ThunkStack32To16();

    printf("fakeldrClose:                   hFile = 0x%04x, rc = %d\n", hFile, rc);

    return rc;
}

/**
 * Fake ldrOpen.
 * @returns   OS2 return code.
 * @param     phFile        Pointer to return handle.
 * @param     pszFilename   Pointer to filename.
 * @param     pfl           Pointer to media flags? *pfl is set to zero!
 * @sketch    Do a DosOpen on the filename.
 * @status    partially implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL fakeldrOpen(PSFN phFile, PSZ pszFilename, PULONG pfl)
{
    ULONG   ulAction;
    HFILE   hFile;
    APIRET  rc;
    BOOL    f32Stack = ((int)&hFile > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    hFile = 0;
    ulAction = 0;
    rc =  DosOpen(pszFilename, &hFile, &ulAction, 0, FILE_NORMAL,
                  OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                  OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE,
                  NULL);
    if (rc == NO_ERROR)
    {
        *phFile = (USHORT)hFile;
        if (pfl != NULL)
            *pfl = 0;
    }

    if (!f32Stack) ThunkStack32To16();

    printf("fakeldrOpen:                    phFile = %p; *phFile = 0x%04x, pszFilename = %s, pfl = %p, rc = %d\n",
           phFile, *phFile, pszFilename, pfl, rc);

    return rc;
}


/**
 * ldrRead fake.
 * @returns   OS/2 return code. (NO_ERROR on success...)
 * @param     hFile     Handle to file.
 * @param     ulOffset  Offset in the file to start reading at.
 * @param     pvBuffer  Pointer to output buffer.
 * @param     fpBuffer  Far pointer to buffer. (ignored)
 * @param     cbToRead  Count of bytes to read.
 * @param     pMTE      MTE pointer (used for caching?)
 *
 * @sketch    Set Change filepointer to ulOffset.
 *            Read cbToRead into pvBufer.
 * @status    completely tested.
 * @author    knut st. osmundsen
 * @remark
 */
ULONG LDRCALL fakeldrRead(SFN hFile, ULONG ulOffset, PVOID pvBuffer, ULONG fpBuffer, ULONG cbToRead, PMTE pMTE)
{
    ULONG   cbRead,
            ulMoved;
    APIRET  rc;
    BOOL    f32Stack = ((int)&hFile > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    rc = DosSetFilePtr(hFile, ulOffset, FILE_BEGIN, &ulMoved);
    if (rc == NO_ERROR)
        rc = DosRead(hFile, pvBuffer, cbToRead, &cbRead);
    else
        kprintf(("fakeldrRead: DosSetFilePtr(hfile, 0x%08x(%d),..) failed with rc = %d.\n",
                 ulOffset, ulOffset, rc));

    if (!f32Stack) ThunkStack32To16();

    printf("fakeldrRead:                    hFile = 0x%04x, ulOffset = 0x%08x, pvBuffer = %p, fpBuffer = 0x%08x, cbToRead = 0x%08x, pMte = %p, rc = %d\n",
           hFile, ulOffset, pvBuffer, fpBuffer, cbToRead, pMTE, rc);

    return rc;
}


/**
 * LDRQAppType faker.
 * @returns   OS/2 return code.
 * @param     pszFilename   Pointer to executable to query app type for.
 * @param     pul           Pointer to flag variable to return apptype flags in.
 * @sketch
 * @status    stub.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL fakeLDRQAppType(PSZ pszFilename, PULONG pul)
{
    DUMMY();
    printf("fakeLDRQAppType:                pszFilename = %s, pul = %p, rc = 0\n", pszFilename, pul);
    return NO_ERROR;
}


/**
 * ldrEnum32bitRelRecs
 * @param   pMTE           Pointer to MTE for this module.
 * @param   iObject        Object index. 0-based!
 * @param   iPageTabl      Page index. 0-based!
 * @param   pvPage         Pointer to page buffer.
 * @param   ulPageAddress  Note! Page is not present.
 * @param   pPTDA
 * @status  stub
 */
ULONG LDRCALL fakeldrEnum32bitRelRecs(
    PMTE    pMTE,
    ULONG   iObject,
    ULONG   iPageTable,
    PVOID   pvPage,
    ULONG   ulPageAddress,
    PVOID   pvPTDA
    )
{
    DUMMY();
    printf("fakeldrEnum32bitRelRecs:        pMTE = %p, iObject = 0x%08x, iPageTable = 0x%08x, pvPage = 0x%08x, ulPageAddress = 0x%08x, pvPTDA = %p\n",
           pMTE, iObject, iPageTable, pvPage, ulPageAddress, pvPTDA);

    return NO_ERROR;
}

/**
 * Opens a given file.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    pszFilename   Pointer to filename.
 * @param    flOpenFlags   Open flags. (similar to DosOpen)
 * @param    fsOpenMode    Open mode flags. (similar to DosOpen)
 * @param    phFile        Pointer to filehandle.
 * @param    pulsomething  16-bit near (?) pointer to a variable - unknown. NULL is allowed. EA?
 */
APIRET KRNLCALL fakeIOSftOpen(
    PSZ pszFilename,
    ULONG flOpenFlags,
    ULONG fsOpenMode,
    PSFN phFile,
    PULONG pulsomething
    )
{
    ULONG   ulAction;
    HFILE   hFile;
    APIRET  rc;
    BOOL    f32Stack = ((int)&hFile > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    hFile = 0;
    ulAction = 0;
    rc =  DosOpen(pszFilename, &hFile, &ulAction, 0, FILE_NORMAL,
                  flOpenFlags,
                  fsOpenMode,
                  NULL);
    if (rc == NO_ERROR)
        *phFile = (USHORT)hFile;

    if (!f32Stack) ThunkStack32To16();

    printf("fakeIOSftOpen:                  pszFilename = %s, flOpenFlags = 0x%08x, fsOpenMode = 0x%08x, phFile = %p; *phFile = 0x%04, pulsomething = %p, rc = %d\n",
           pszFilename, flOpenFlags, fsOpenMode, phFile, *phFile, pulsomething, rc);

    return rc;
}


/**
 * Closes the specified file.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    hFile      File handle - System File Number.
 */
APIRET KRNLCALL fakeIOSftClose(
    SFN hFile
    )
{
    APIRET  rc;
    BOOL    f32Stack = ((int)&hFile > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    rc =  DosClose(hFile);

    if (!f32Stack) ThunkStack32To16();

    printf("fakeIOSftClose:                 hFile = 0x%04, rc = %d\n",
           hFile, rc);

    return rc;
}


/**
 * Probably this function will expand a relative path to a full path.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    pszPath  Pointer to path to expand. Contains the full path upon return. (?)
 *                    This buffer should probably be of CCHMAXPATH length.
 * @status   completely implemented.
 */
APIRET KRNLCALL fakeIOSftTransPath(
    PSZ pszPath
    )
{
    char    szBuffer[CCHMAXPATH];
    APIRET  rc;
    BOOL    f32Stack = ((int)&pszPath > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    rc = DosQueryPathInfo(pszPath, FIL_QUERYFULLNAME, &szBuffer, sizeof(szBuffer));
    printf("fakeIOSftTransPath:              pszPath(in) = %s, pszPath(out) = %s, rc = %d\n",
           pszPath, szBuffer, rc);
    if (rc == NO_ERROR)
        strcpy(pszPath, szBuffer);
    else
        kprintf(("fakeIOSftTransPath: DosQueryPathInfo failed with rc=%d\n", rc));


    if (!f32Stack) ThunkStack32To16();

    return rc;
}


/**
 * Read at a given offset in the a file.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    hFile      File handle - System File Number.
 * @param    pcbActual  Pointer to variable which upon input holds the number
 *                      of bytes to read, on output the actual number of bytes read.
 * @param    pvBuffer   Pointer to the read buffer.
 * @param    fpBuffer   16 far pointer to buffer. (ignored)
 * @param    ulOffset   File offset to read from. (0=start of file)
 */
APIRET KRNLCALL fakeIOSftReadAt(
    SFN hFile,
    PULONG pcbActual,
    PVOID pvBuffer,
    ULONG fpBuffer,
    ULONG ulOffset
    )
{
    ULONG   ulMoved;
    APIRET  rc;
    BOOL    f32Stack = ((int)&hFile > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    rc = DosSetFilePtr(hFile, ulOffset, FILE_BEGIN, &ulMoved);
    if (rc == NO_ERROR)
        rc = DosRead(hFile, pvBuffer, *pcbActual, pcbActual);
    else
        kprintf(("fakeIOSftReadAt: DosSetFilePtr(hfile, 0x%08x(%d),..) failed with rc = %d.",
                 ulOffset, ulOffset, rc));

    if (!f32Stack) ThunkStack32To16();

    printf("fakeIOSftReadAt:                hFile = 0x%04, pcbActual = %p; *pcbActual = 0x%08x, pvBuffer = %p, fpBuffer = %x, ulOffset = 0x%08x, rc = %d\n",
           hFile, pcbActual, pvBuffer, fpBuffer, ulOffset, rc);

    return rc;
}


/**
 * Write at a given offset in the a file.
 * @returns  NO_ERROR on success. OS/2 error code on error.
 * @param    hFile      File handle - System File Number.
 * @param    pcbActual  Pointer to variable which upon input holds the number
 *                      of bytes to write, on output the actual number of bytes write.
 * @param    pvBuffer   Pointer to the write buffer.
 * @param    fpBuffer   16 far pointer to buffer. (ignored)
 * @param    ulOffset   File offset to write from. (0=start of file)
 */
APIRET KRNLCALL fakeIOSftWriteAt(
    SFN hFile,
    PULONG pcbActual,
    PVOID pvBuffer,
    ULONG fpBuffer,
    ULONG ulOffset
    )
{
    ULONG   ulMoved;
    APIRET  rc;
    BOOL    f32Stack = ((int)&hFile > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    rc = DosSetFilePtr(hFile, ulOffset, FILE_BEGIN, &ulMoved);
    if (rc == NO_ERROR)
        rc = DosWrite(hFile, pvBuffer, *pcbActual, pcbActual);
    else
        kprintf(("fakeIOSftWriteAt: DosSetFilePtr(hfile, 0x%08x(%d),..) failed with rc = %d.",
                 ulOffset, ulOffset, rc));

    if (!f32Stack) ThunkStack32To16();

    printf("fakeIOSftWriteAt:               hFile = 0x%04, pcbActual = %p; *pcbActual = 0x%08x, pvBuffer = %p, fpBuffer = %x, ulOffset = 0x%08x, rc = %d\n",
           hFile, pcbActual, pvBuffer, fpBuffer, ulOffset, rc);

    return rc;
}


/**
 * Gets the filesize.
 * @returns   NO_ERROR on success; OS/2 error code on error.
 * @param     hFile    File handle - System File Number.
 * @param     pcbFile  Pointer to ULONG which will hold the file size upon return.
 */
APIRET KRNLCALL fakeSftFileSize(
    SFN hFile,
    PULONG pcbFile
    )
{
    FILESTATUS3     fsts3;
    APIRET          rc;
    BOOL            f32Stack = ((int)&hFile > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    rc = DosQueryFileInfo(hFile,
                          FIL_STANDARD,
                          &fsts3,
                          sizeof(fsts3));
    if (rc == NO_ERROR)
        *pcbFile = fsts3.cbFile;
    else
        kprintf(("fakeSftFileSize: DosQueryFileInfo failed with rc=%d\n", rc));

    if (!f32Stack) ThunkStack32To16();

    printf("fakeSftFileSize:                hFile = 0x%04x, pcbFile = %p; *pcbFile = 0x%08x, rc = %d\n",
           hFile, pcbFile, *pcbFile, rc);

    return rc;
}


/**
 * @status    stub
 */
HMTE KRNLCALL fakeVMGetOwner(
    ULONG ulCS,
    ULONG ulEIP)
{
    DUMMY();

    printf("fakeVMGetOwner:                 ulCS = 0x%04x, ulEiP = 0x%08x, rc = %d\n",
           ulCS, ulEIP, 0);

    return 0;
}


/**
 * @status    stub
 */
APIRET KRNLCALL fakeVMAllocMem(
    ULONG   cbSize,
    ULONG   cbCommit,
    ULONG   flFlags1,
    HPTDA   hPTDA,
    USHORT  usVMOwnerId,
    HMTE    hMTE,
    ULONG   flFlags2,
    ULONG   SomeArg2,
    PVMAC   pvmac)
{
    DUMMY();
    printf("fakeVMAllocMem:                 cbSize = 0x%08x, cbCommit = 0x%08x, flFlags1 = 0x%08x, hPTDA = 0x%04x, usVMOwnerId = 0x%04x, hMTE = 0x%04x, flFlags2 = 0x%08x, SomeArg2 = 0x%08x, pvmac = %p, rc = %d\n",
           cbSize, cbCommit, flFlags1, hPTDA, usVMOwnerId, hMTE, flFlags2, SomeArg2, pvmac, 0);

    return ERROR_NOT_SUPPORTED;
}


/**
 * @status    stub
 * @remark    Used by environment stuff...
 */
APIRET KRNLCALL fakeVMObjHandleInfo(
    USHORT  usHob,
    PULONG  pulAddr,
    PUSHORT pushPTDA)
{
    APIRET          rc = NO_ERROR;
    BOOL            f32Stack = ((int)&usHob > 0x10000);

    if (!f32Stack) ThunkStack16To32();

    if (pulAddr > (PULONG)0x10000 && pushPTDA > (PUSHORT)0x10000)
    {
        PTIB pTib;
        PPIB pPib;
        DosGetInfoBlocks(&pTib, &pPib);
        switch (usHob)
        {
            case 1: *pulAddr = (ULONG)pPib->pib_pchenv; break; //PTDA environ (fakea.asm)
            default:
                printf("fakeVMObjHandleInfo:            Invalid handle! (usHob=0x%#4x)\n", usHob);
                rc = ERROR_INVALID_HANDLE;
        }
        *pushPTDA = 10; //dummy
    }
    else
        rc = ERROR_INVALID_PARAMETER;

    if (!f32Stack) ThunkStack32To16();

    printf("fakeVMObjHandleInfo:            usHob = 0x%04x, pulAddr = %p, pushPTDA = %p, rc = %d\n",
           usHob, pulAddr, pushPTDA, rc);
    return rc;
}


/**
 * myldrOpenPath - opens file eventually searching loader specific paths
 *
 * @returns   OS2 return code.
 *            pLdrLv->lv_sfn  is set to filename handle.
 * @param     pachFilename  Pointer to modulename. Not zero terminated?
 * @param     cchFilename   Modulename length.
 * @param     plv           Loader local variables? (Struct from KERNEL.SDF)
 * @param     pful          Pointer to flags which are passed on to ldrOpen.
 * @status    stub
 */
ULONG LDRCALL fakeldrOpenPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful)
{
    static char     szPath[1024];       /* Path buffer. Used to store pathlists. 1024 should be enough */
                                        /* for LIBPATH (which at had a limit of ca. 750 chars). */
    static char     sz[CCHMAXPATH];     /* Filename/path buffer. (Normally used to build filenames */
                                        /* which are passed in as search experessions to DosFindFirst.) */

    APIRET          rc;                 /* Return value. (Pessimistic attitude! Init it to FALSE...) */

    printf("fakeldrOpenPath:        *entry* pachFilename = %.*s, cchFilename = %d,  plv = %p, pful = %p\n",
           cchFilename, pachFilename, cchFilename, plv, pful);

    if (plv->lv_class != CLASS_GLOBAL)
        rc = fakeldrOpen(&plv->lv_sfn, pachFilename, pful);
    else
    {
        int iPath;                      /* Current path or pathlist being examined. This is the loop */
                                        /* variable looping on the FINDDLL_* defines. */

        /* These defines sets the order the paths and pathlists are examined. */
        #define FINDDLL_BEGINLIBPATH    7
        #define FINDDLL_LIBPATH         8
        #define FINDDLL_ENDLIBPATH      9
        #define FINDDLL_FIRST           FINDDLL_BEGINLIBPATH
        #define FINDDLL_LAST            FINDDLL_ENDLIBPATH

        /*
         * Remove DLL extention from the filename.
         */
        if (cchFilename > 4 && !strncmp(&pachFilename[cchFilename - 4], ".DLL", 4))
        {
            cchFilename -= 4;
            pachFilename[cchFilename] = '\0';
        }

        /** @sketch
         * Loop thru the paths and pathlists searching them for the filename.
         */
        rc = ERROR_FILE_NOT_FOUND;
        for (iPath = FINDDLL_FIRST; iPath <= FINDDLL_LAST; iPath++)
        {
            const char  *   pszPath;        /* Pointer to the path being examined. */

            /** @sketch Get the path/dir to examin. (This is determined by the value if iPath.) */
            switch (iPath)
            {
                case FINDDLL_BEGINLIBPATH:
                    rc = DosQueryExtLIBPATH(szPath, BEGIN_LIBPATH);
                    break;
                case FINDDLL_LIBPATH:
                    rc = DosQueryHeaderInfo(NULLHANDLE, 0, szPath, sizeof(szPath), QHINF_LIBPATH);
                    break;
                case FINDDLL_ENDLIBPATH:
                    rc = DosQueryExtLIBPATH(szPath, END_LIBPATH);
                    break;
                default: /* !internalerror! */
                    return ERROR_FILE_NOT_FOUND;
            }
            if (rc != NO_ERROR)
            {
                printf("fakeldrOpenPath: DosQueryExtLIBPATH/DosQueryHeadInfo failed with rc=%d, iPath=%d", rc, iPath);
                continue;
            }
            pszPath = szPath;


            /** @sketch
             * pszPath is now set to the pathlist to be searched.
             * So we'll loop thru all the paths in the list.
             */
            while (pszPath != NULL && *pszPath != '\0')
            {
                char *  pszNext;            /* Pointer to the next pathlist path */
                int     cch;                /* Length of path (including the slash after the slash is added). */

                /** @sketch
                 * Find the end of the path.
                 * Copy the path into the sz buffer.
                 * Set pszNext.
                 */
                pszNext = strchr(pszPath, ';');
                if (pszNext != NULL)
                {
                    cch = pszNext - pszPath;
                    pszNext++;
                }
                else
                    cch = strlen(pszPath);

                if (cch + cchFilename + 1 >= sizeof(sz)) /* assertion */
                {
                    printf("fakeldrOpenPath: cch (%d) + cchFilename (%d) + 1 < sizeof(plv->sz) (%d) - paths too long!, iPath=%d",
                           cch, cchFilename, sizeof(sz), iPath);
                    pszPath = pszNext;
                    continue;
                }
                memcpy(sz, pszPath, cch);

                /** @sketch
                 * Add a '\\' and the filename (pszFullname) to the path;
                 * then we'll have a fullpath.
                 */
                sz[cch++] = '\\';
                memcpy(&sz[cch], pachFilename, cchFilename);
                memcpy(&sz[cch + cchFilename], ".DLL", 5);

                rc = fakeldrOpen(&plv->lv_sfn, sz, pful);
                switch (rc)
                {
                    case ERROR_FILE_NOT_FOUND:          case ERROR_PATH_NOT_FOUND:          case ERROR_ACCESS_DENIED:           case ERROR_INVALID_ACCESS:
                    case ERROR_INVALID_DRIVE:           case ERROR_NOT_DOS_DISK:            case ERROR_REM_NOT_LIST:            case ERROR_BAD_NETPATH:
                    case ERROR_NETWORK_BUSY:            case ERROR_DEV_NOT_EXIST:           case ERROR_TOO_MANY_CMDS:           case ERROR_ADAP_HDW_ERR:
                    case ERROR_UNEXP_NET_ERR:           case ERROR_BAD_REM_ADAP:            case ERROR_NETNAME_DELETED:         case ERROR_BAD_DEV_TYPE:
                    case ERROR_NETWORK_ACCESS_DENIED:   case ERROR_BAD_NET_NAME:            case ERROR_TOO_MANY_SESS:           case ERROR_REQ_NOT_ACCEP:
                    case ERROR_INVALID_PASSWORD:        case ERROR_OPEN_FAILED:             case ERROR_INVALID_NAME:            case ERROR_FILENAME_EXCED_RANGE:
                    case ERROR_VC_DISCONNECTED:
                        rc = ERROR_FILE_NOT_FOUND;
                        pszPath = pszNext;
                        continue;
                }

                /*
                 * Fatal error or success.
                 */
                printf("fakeldrOpenPath:        *exit*  plv->lv_sfn = 0x%04x, rc = %d\n", plv->lv_sfn, rc);
                return rc;
            }
        } /* for iPath */
    }

    printf("fakeldrOpenPath:        *exit*  plv->lv_sfn = 0x%04x, rc = %d\n", plv->lv_sfn, rc);

    return rc;
}


/**
 * LDRClearSem - Clears the loader semaphore.
 * (It does some garbage collection on release.)
 * @returns   NO_ERROR on sucess.
 *            OS/2 error on failure. (ERROR_INTERRUPT?)
 * @status    completely implemented.
 */
ULONG LDRCALL fakeLDRClearSem(void)
{
    if (lfakeLDRSem == 0)
        kprintf(("fakeLDRClearSem: lfakeLDRSem is 0 allready\n"));
    else
        lfakeLDRSem--;

    printf("fakeLDRClearSem:                usage count = %d, rc = %d\n", lfakeLDRSem, NO_ERROR);

    return NO_ERROR;
}

/**
 * KSEMReuqestMutex faker.
 * @returns   NO_ERROR
 * @param     hkmtx
 * @param     ulTimeout
 * @status    completely implemented.
 * @remark
 */
ULONG KRNLCALL fakeKSEMRequestMutex(HKMTX hkmtx, ULONG ulTimeout)
{
    (*(PLONG)hkmtx)++;

    printf("fakeKSEMRequestMutex:           hkmtx = %p, ulTimeout = 0x%x, usage count = %d, rc = %d\n",
           hkmtx, ulTimeout, *(PLONG)hkmtx, NO_ERROR);

    return NO_ERROR;
}


/**
 * Copy user memory into system memory.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pv     Pointer to target (system) data area.
 * @param     pvUsr  Pointer to source (user) data area.
 * @param     cb     Count of bytes to copy.
 * @param     fl     Flags.
 * @status  Partially implemented.
 * @remark  Ignores fl. Crashes on error.
 */
ULONG KRNLCALL   fakeTKFuBuff(PVOID pv, PVOID pvUsr, ULONG cb, ULONG fl)
{
    memcpy(pv, pvUsr, cb);

    printf("fakeTKFuBuff:                   pv = %p, pvUsr = %p, cb = 0x%08x, fl = 0x%08x, rc = %d\n",
           pv, pvUsr, cb, fl, NO_ERROR);

    return NO_ERROR;
}


/**
 * Copy system memory to user memory.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pvUsr  Pointer to target (user) data area.
 * @param     pv     Pointer to source (system) data area.
 * @param     cb     Count of bytes to copy.
 * @param     fl     Flags.
 * @status  Partially implemented.
 * @remark  Ignores fl. Crashes on error.
 */
ULONG KRNLCALL   fakeTKSuBuff(PVOID pvUsr, PVOID pv, ULONG cb, ULONG fl)
{
    memcpy(pvUsr, pv, cb);

    printf("fakeTKSuBuff:                   pvUsr = %p, pv = %p, cb = 0x%08x, fl = 0x%08x, rc = %d\n",
           pvUsr, pv, cb, fl, NO_ERROR);

    return NO_ERROR;
}


/**
 * String length. (includes NULL(s))
 * @returns   OS/2 return code. (NO_ERROR is success)
 *            ERROR_TERMINATOR_NOT_FOUND
 * @param     pcch      Pointer to length variable.
 * @param     pvUsr     Pointer to user data to preform string length on.
 * @param     cchMax    Max string length.
 * @param     fl        Flags.
 * @param     fDblNULL  TRUE:  Double '\0' (ie. '\0\0') terminated. Usefull for scanning environments.
 *                      FALSE: Single string. (ie. one '\0').
 * @status  Partially implemented.
 * @remark  Ignores fl. Crashes on error.
 */
ULONG KRNLCALL   fakeTKFuBufLen(PLONG pcch, PVOID pvUsr, ULONG cchMax, ULONG fl, BOOL fDblNULL)
{
    ULONG   rc;
    PSZ     psz = pvUsr;

    cchMax += (ULONG)pvUsr;

    while ((ULONG)psz < cchMax)
    {
        if (*psz++ == '\0')
            if (!fDblNULL || (ULONG)psz == cchMax || *psz++ != '\0')
                break;
    }

    *pcch = (ULONG)psz - (ULONG)pvUsr;

    /* Hope this is the correct definition of max length: Not inclusive. */
    if ((ULONG)psz == cchMax)
        rc = ERROR_TERMINATOR_NOT_FOUND;
    else
        rc = NO_ERROR;

    printf("fakeTKFuBufLen:                 pcch = %p; *pcch = 0x%08x, pvUsr = %p, cchMax = 0x%08x, fl = 0x%08x, fDblNULL = %x, rc = %d\n",
           pcch, *pcch, pvUsr, cchMax, fl, fDblNULL, NO_ERROR);

    return rc;
}


/**
 * Validates an hMTE and gets the MTE pointer - FAKE.
 * @returns Pointer to MTE on success.
 *          NULL on error.
 * @param   hMTE  MTE handle.
 * @remark  If you wan't to this faster:
 *            Use the hMTE as a HOB and get the HOB address (by using VMGetHandleInfo).
 * @status  stub.
 */
PMTE LDRCALL fakeldrValidateMteHandle(HMTE hMTE)
{
    DUMMY();

    printf("fakeldrValidateMteHandle:       hMTE = 0x%04x, pMTE (rc) = %p\n",
           hMTE, NULL);

    return NULL;
}


/**
 * C worker function for fakeg_tkExecPgm. This is called by fakeg_tkExecPgm (fakea.asm).
 * @returns   OS/2 return code.
 * @param     execFlag      Exec flag (DosExecPgm execFlag).
 * @param     pArg          Pointer to arguments. (NULL allowed)
 * @param     pEnv          Pointer to environment. (NULL allowed)
 * @param     pszFilename   Pointer to filename.
 * @sketch
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG _Optlink tkExecPgmWorker(ULONG execFlag, PSZ pArg, PSZ pEnv, PSZ pszFilename)
{
    APIRET      rc;
    ldrrei_t    rei;

    printf("tkExecPgmWorker:                execFlag = %d, pArg = %p, pEnv = %p, pszFilename = %s\n", execFlag, pArg, pEnv, pszFilename);

    /*
     * Simulate loading.
     */
    rc = fakeLDRLoadExe(pszFilename, SSToDS(&rei));

    NOREF(pArg);
    NOREF(pEnv);
    NOREF(execFlag);

    return rc;
}


/**
 * Fake implementation of LDRLoadExe.
 * Not currently overriden, but used by tkExecPgmWorker to load executables.
 * @returns   OS/2 return code.
 * @param     pszFilename   Name of executable image to load.
 * @param     pEI           It seems this is a pointer to a ldrrei_s struct.
 *                          (currently ignored)
 * @sketch
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL fakeLDRLoadExe(PSZ pszFilename, ldrrei_t *pEI)
{
    APIRET  rc;
    int     cchFilename = strlen(pszFilename);

    printf("fakeLDRLoadExe:         *entry* pszFilename = %s, pEI = %p\n",
           pszFilename, pEI);

    rc = fakeldrGetModule(pszFilename, (cchFilename << 16) | (LVTYPE_EXE << 8) | CLASS_PROGRAM);

    printf("fakeLDRLoadExe:         *exit*  pszFilename = %s, pEI = %p, rc = %d\n",
           pszFilename, pEI, rc);

    return rc;
}



/**
 * Fake implementeation of ldrGetModule.
 * Not currently overriden, but used by fakeLDRLoadExe to load executables.
 * @returns   OS/2 return code.
 * @param     pszFilename   Name of executable image to load.
 * @param     ul            low 8 bits:     Module CLASS_*.
 *                          high 8 bits:    Executable type LVTYPE_* ?
 *                          high 16 bits:   Filename length.
 * @sketch
 *  OS/2 kernel seems to do:
 *      Call Sec32GetModule
 *      Call ldrGetMte which does the necessary loading / attaching.
 *      Call ldrLoadImports for all imported DLLs. (and the exe?)
 *      Call ldrProcessObjects for LX and ldrPreloadLDT for NE imported DLLs. (and the exe?)
 *      Call ldrProcessImports.
 *
 *  We do:
 *      Call fakeldrGetMte.
 *      Call fakeldrLoadImports.
 *
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL fakeldrGetModule(PSZ pszFilename, ULONG ul)
{
    APIRET  rc;
    PMTE    pmte = NULL;

    printf("fakeldrGetModule:       *entry* pszFilename = %s, ul = 0x%08x\n",
           pszFilename, ul);

    rc = fakeldrGetMte(pszFilename, (USHORT)(ul >> 16), (UCHAR)((ul & 0xff00) >> 8), (UCHAR)(ul & 0x00ff), SSToDS(&pmte));
    if (rc == NO_ERROR)
    {
        rc = fakeldrLoadImports(pmte);
    }

    printf("fakeldrGetModule:       *exit*  pszFilename = %s, ul = 0x%08x, rc = %d\n",
           pszFilename, ul, rc);

    return rc;
}


/**
 *
 * @returns
 * @param
 * @equiv
 * @time
 * @sketch
 *  OS/2 kernel seems to do:
 *      IF not device driver THEN
 *          Uppercase the name.
 *          Call ldrFindModule to try find the module within currenly loaded modules.
 *      ENDIF
 *
 *    tryagain:
 *      IF found THEN
 *          Set MTEGETMTEDONE flag in mte_flags2.
 *          Call ldrChkLoadType to ensure correct load type.
 *          IF attached or special modules (DOSMOD, VDDMOD, FSHMOD or MVDMMOD)
 *              IF miniIFS and !FSHMOD THEN Fail with rc=ERROR_INVALID_NAME
 *              return successfully.
 *          ENDIF
 *          lv_sfn <- mte_sfn
 *          Set the USED flag in mte_flags1
 *          Set the MTELOADORATTACH flag in mte_flags2
 *          Some processing of packed segments???? Dll init flags???
 *      ELSE *not found*
 *          IF class is CLASS_SPECIFIC AND ldrCheckGlobal returned TRUE THEN
 *              * this is because some specific modules is promoted to global
 *              * modules for some reasons I haven't ivestigated. (ldrPrmoteMTE)
 *              tryagain.
 *          ENDIF
 *          Set some flag to false.
 *          Calls to ldrOpenNewExe to load the exectuble from disk.
 *          IF this failes THEN fail.
 *          IF class is CLASS_GLOBAL and module is loaded as specific
 *              (ldrCheckSpecific returns TRUE) THEN
 *              Close and retry with the mte returned ldrCheckSpecific
 *              tryagain.
 *          ENDIF
 *          Sets some local variable to point at pheaderbuf, where ldrOpenNewExe
 *              has put the executable header.
 *          Call ldrCheckLoadType and fail if this failes.
 *          Call ldrCreateMte and fail if this failes.
 *          Set some local variables to lv_pmte and lv_pmte->mte_swapmte.
 *          Set the MTELOADORATTACH and MTEGETMTEDONE flags.
 *          Call ldrInvTgtErrTxt.
 *          IF DDInitTime AND DEVDRVMOD OR FSDMOD THEN
 *              ptda_module of current PTDA to the mtehandle of the module.
 *          ENDIF
 *          IF DOSCALL1 (checks name) THEN
 *              Set DOSLIB flag of mte_flags1.
 *              Set global pmteDoscall1 to point to the mte.
 *              Set global hmteDoscall1 to the handle of the mte.
 *          ENDIF
 *      ENDIF
 *      IF VDDMOD THEN
 *          call VDMMLoadingVDD with the mte handle.
 *      ENDIF
 *      IF NE executable THEN
 *          Call ldrAllocSegments
 *      ELSE
 *          Call ldrAllocObjects
 *      ENDIF
 *      IF failed THEN fail.
 *      IF LVTYPE_EXE THEN
 *          Set the ptda_module of the ExecChild PTDA to mtehandle.
 *          IF MTELONGNAMES THEN
 *              Set ptda_NewFiles to something.
 *          ENDIF
 *          Increment usage count, ie. mte_usecnt-
 *      ELSE IF LVTYPE_DLL AND !somelocalflag AND NOINTERNFIXUPS THEN
 *          Remove Internal fixups?
 *          (Calls ldrIsInternalFixup and VMReallocKHB during this processing.)
 *      ENDIF
 *      Clears the MTEPROCESSED flag of mte_flags1.
 *      Sets the return mte pointer. (ppmte)
 *      return successfully.
 *
 *  We do:
 *      Uppercase the string. (do we have to switch stack?)
 *      Call fakeldrOpenNewExe to do it's work.
 *      Call fakeldrCreateMte to create a fake mte.
 *      return.
 *
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL   fakeldrGetMte(PCHAR pachFilename, USHORT cchFilename, UCHAR fchType, UCHAR fchClass, PPMTE ppmte)
{
    ldrlv_t         lv;
    APIRET          rc;

    printf("fakeldrGetMte:          *entry* pachFilename = %.*s, cchFilename = %#02x, fchType %#2x, fchClass = %#2x, ppmte = %p\n",
           cchFilename, pachFilename, cchFilename, fchType, fchClass, ppmte);

    /*
     * Initiate the local variables.
     */
    memset(SSToDS(&lv), 0, sizeof(lv));
    lv.lv_sfn = (USHORT)-1;
    lv.lv_type = fchType;
    lv.lv_class = fchClass;

    /*
     * Uppercase the string.
     */
    fakeldrUCaseString(pachFilename, cchFilename);

    /*
     * Open the new executable.
     */
    rc = fakeldrOpenNewExe(pachFilename, cchFilename, SSToDS(&lv), NULL);
    if (rc != NO_ERROR)
    {
        fakeldrClose(lv.lv_sfn);
        printf("fakeldrGetMte:          *exit*  pachFilename = %.*s, ppmte = %p; *ppmte = %p, rc = %d\n",
               cchFilename, pachFilename, ppmte, *ppmte, rc);
        return rc;
    }

    /*
     * Create MTE for the opened file.
     */
    rc = fakeldrCreateMte(pheaderbuf, SSToDS(&lv));
    if (rc == NO_ERROR)
    {
        /*
         * Set some flags... More?
         */
        lv.lv_pmte->mte_flags2 |= MTEGETMTEDONE | MTELOADORATTACH;
        *ppmte = lv.lv_pmte;
    }

    printf("fakeldrGetMte:          *exit*  pachFilename = %.*s, ppmte = %p; *ppmte = %p, rc = %d\n",
           cchFilename, pachFilename, ppmte, *ppmte, rc);

    return rc;
}



/**
 * Opens an executable module.
 * @returns   OS2 error code.
 * @param     pachFilename      Filename of the executable module to open.
 * @param     cchFilename       Filename length.
 * @param     plv               Pointer to local variables.
 * @param     pus               Pointer to some flags (LDRQAppType flags?!).
 * @sketch
 *  OS/2 kernel seems to do:
 *      Call ldrOpenPath to open the file.
 *      IF this fail THEN
 *          Set lv_sfn to -1 and return errorcode.
 *      ENDIF
 *      Set local variable to pheaderbuf.
 *      IF pus THEN set *pus to 1
 *      Call ldrRead to read the MZ header.
 *      Fail if this failed. (Don't close file caller does that.)
 *      IF LXMAGIC of NEMAGIC found THEN
 *          lv_new_exe_off <- 0;
 *      ELSE
 *          IF !EMAGIC or 'OZ' THEN
 *              Fail with rc=ERROR_INVALID_EXE_SIGNATURE.
 *          ENDIF
 *          IF pus THEN set *pus to 2.
 *          lv_new_exe_off <- e_lfanew.
 *      ENDIF
 *      Call ldrRead to read LX/NE header at lv_new_exe_off.
 *      Fail if this failed.
 *      IF NOT  LXMAGIC OR NEMAGIC THEN
 *          IF LEMAGIC AND pus THEN Set *pus to 0.
 *          IF PEMAGIC and pus THEN Set *pus to 3.
 *          Fail with rc=ERROR_INVALID_EXE_SIGNATURE.
 *      ENDIF
 *      IF pus THEN Set *pus to 0.
 *      IF invalid LX exeheader THEN
 *          Fail with rc=ERROR_INVALID_EXE_SIGNATURE.
 *      ENDIF
 *      IF NEMAGIC THEN
 *          Call ldrExpandHeader
 *      ELSE
 *          Set the MTELONGNAMES flag in e32_mflags.
 *      ENDIF
 *      IF LDRINVALID (in e32_mflags) THEN
 *          Fail with rc=ERROR_EXE_MARKED_INVALID.
 *      ENDIF
 *      Clear some e32_mflags, MTE_MEDIAFIXED, MTEMEDIACONTIG and MTEMEDIA16M.
 *      Or in flags set by ldrOpenPath into e32_mflags. (media type flags?!)
 *      Call ldrMungeFlags
 *      return retur code from ldrMungeFlags.
 *
 *  We do:
 *      Call fakeldrOpenPath to open the file.
 *      IF this fail THEN
 *          Set lv_sfn to -1 and return errorcode.
 *      ENDIF
 *      IF pus THEN set *pus to 1
 *      Call ldrRead to read the MZ header.
 *      Fail if this failed. (Don't close file caller does that.)
 *      If MZ header Then
 *          Set lv_new_exe_off to e_lfanew.
 *      else
 *          Set lv_new_exe_off to 0.
 *      Call ldrRead to read the LX header.
 *      Fail if this failed. (Don't close file caller does that.)
 *      If Not LX signature Then
 *          Fail.
 *      return successfully.
 *
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL   fakeldrOpenNewExe(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PUSHORT pus)
{
    APIRET  rc;
    ULONG   ful;

    printf("fakeldrOpenNewExe:      *entry* pachFilename = %.*s, cchFilename = %#2, plv = %p, pus = %p\n",
           cchFilename, pachFilename, cchFilename, plv, pus);

    rc = fakeldrOpenPath(pachFilename, cchFilename, plv, SSToDS(&ful));
    if (rc != NO_ERROR)
    {
        plv->lv_sfn = 0xffff;
        printf("fakeldrOpenNewExe:      *exit*  pachFilename = %.*s, plv->lv_sfn = %#4x, rc = %d\n",
               cchFilename, pachFilename, plv->lv_sfn, rc);
        return rc;
    }
    if (pus) *pus = 1;

    memset(pheaderbuf, 0, sizeof(struct exe_hdr)); //zero it just in case....
    rc = fakeldrRead(plv->lv_sfn, 0, pheaderbuf, 0, sizeof(struct exe_hdr), NULL);
    if (rc != NO_ERROR)
    {
        plv->lv_sfn = 0xffff;
        printf("fakeldrOpenNewExe:      *exit*  pachFilename = %.*s, plv->lv_sfn = %#4x, rc = %d\n",
               cchFilename, pachFilename, plv->lv_sfn, rc);
        return rc;
    }

    if (*(PUSHORT)pheaderbuf == EMAGIC)
        plv->lv_new_exe_off = ((struct exe_hdr *)pheaderbuf)->e_lfanew;
    else
        plv->lv_new_exe_off = 0;

    memset(pheaderbuf, 0, sizeof(struct e32_exe)); //zero it just in case....
    rc = fakeldrRead(plv->lv_sfn, plv->lv_new_exe_off, pheaderbuf, 0, sizeof(struct e32_exe), NULL);
    if (rc != NO_ERROR)
    {
        plv->lv_sfn = 0xffff;
        printf("fakeldrOpenNewExe:      *exit*  pachFilename = %.*s, plv->lv_sfn = %#4x, rc = %d\n",
               cchFilename, pachFilename, plv->lv_sfn, rc);
        return rc;
    }

    if (*(PUSHORT)pheaderbuf != E32MAGIC)
    {
        plv->lv_sfn = 0xffff;
        rc = ERROR_INVALID_EXE_SIGNATURE;
        printf("fakeldrOpenNewExe:      *exit*  pachFilename = %.*s, plv->lv_sfn = %#4x, rc = %d\n",
               cchFilename, pachFilename, plv->lv_sfn, rc);
        return rc;
    }

    printf("fakeldrOpenNewExe:      *exit*  pachFilename = %.*s, plv->lv_sfn = %#4x, rc = %d\n",
           cchFilename, pachFilename, plv->lv_sfn, rc);

    return NO_ERROR;
}


/**
 * Fake ldrCreateMte implementation.
 * @returns
 * @param     pe32  Pointer to the LX/NE header we've just read.
 * @param     plv   Pointer to local variables.
 * @sketch    !This does a lot of thing!
 *
 *  OS/2 Kernel seems to do:
 *      Allocate a kernel heap block for the MTE
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *  We do:
 *
 *
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL   fakeldrCreateMte(struct e32_exe *pe32, ldrlv_t *plv)
{
    ULONG   rc;
    PMTE    pMte;
    PSMTE   pSMte;
    PCHAR   pch;

    printf("fakeldrCreateMte:       *entry* pe32 = %p, plv = %d\n",
           pe32, plv);

    /*
     * Allocate mte and smte.
     * The MTE has an array of mte pointers at the end for imported modules + 1.
     * The SMTE has the fullpath filename, loader section and fixup section.
     */
    pMte = malloc(sizeof(MTE) + sizeof(PMTE) + sizeof(PMTE)*pe32->e32_impmodcnt);
    if (pMte == NULL)
    {
        rc = ERROR_NOT_ENOUGH_MEMORY;
        goto createmte_exit;
    }

    plv->lv_csmte = sizeof(SMTE) + 260 + pe32->e32_ldrsize + pe32->e32_fixupsize;
    pSMte = malloc(plv->lv_csmte);
    if (pSMte == NULL)
    {
        rc = ERROR_NOT_ENOUGH_MEMORY;
        goto createmte_exit;
    }
    pch = (PCHAR)(PVOID)pSMte + sizeof(SMTE) + 260;


    /*
     * Initialize the MTE.
     */
    pMte->mte_flags2    = MTEFORMATLX;
    pMte->mte_handle    = 20;
    pMte->mte_swapmte   = pSMte;
    pMte->mte_link      = NULL;
    pMte->mte_flags1    = pe32->e32_mflags;
    pMte->mte_impmodcnt = pe32->e32_impmodcnt;
    pMte->mte_sfn       = plv->lv_sfn;
    pMte->mte_usecnt    = 0;
    pMte->mte_modname[0]= '\0';

    /*
     * Initialize the SMTE.
     */
    pSMte->smte_mpages = pe32->e32_mpages;
    /*- magic,border,worder,level,cpu,os,ver,mflags */
    pSMte->smte_mpages      = pe32->e32_mpages;        /* 00  Module # pages */
    pSMte->smte_startobj    = pe32->e32_startobj;      /* 04  Object # for instruction */
    pSMte->smte_eip         = pe32->e32_eip;           /* 08  Extended instruction pointer */
    pSMte->smte_stackobj    = pe32->e32_stackobj;      /* 0c  Object # for stack pointer */
    pSMte->smte_esp         = pe32->e32_esp;           /* 10  Extended stack pointer */
        /*- pagesize*/
    pSMte->smte_pageshift   = pe32->e32_pageshift;     /* 14  Page alignment shift in .EXE */
    pSMte->smte_fixupsize   = pe32->e32_fixupsize;     /* 18  Fixup section size */
        /*- fixupsum,ldrsize,ldrsum*/
    pSMte->smte_objtab      = (POTE)pe32->e32_objtab;  /* 1c  Object table offset - POINTER */
    pSMte->smte_objcnt      = pe32->e32_objcnt;        /* 20  Number of objects in module */
    pSMte->smte_objmap      = pe32->e32_objmap;        /* 24  Object page map offset - POINTER */
    pSMte->smte_itermap     = pe32->e32_itermap;       /* 28  Object iterated data map offset */
    pSMte->smte_rsrctab     = pe32->e32_rsrctab;       /* 2c  Offset of Resource Table */
    pSMte->smte_rsrccnt     = pe32->e32_rsrccnt;       /* 30  Number of resource entries */
    pSMte->smte_restab      = pe32->e32_restab;        /* 34  Offset of resident name table - POINTER */
    pSMte->smte_enttab      = pe32->e32_enttab;        /* 38  Offset of Entry Table - POINTER */
    pSMte->smte_fpagetab    = pe32->e32_fpagetab;      /* 3c  Offset of Fixup Page Table - POINTER */
    pSMte->smte_frectab     = pe32->e32_frectab;       /* 40  Offset of Fixup Record Table - POINTER */
    pSMte->smte_impmod      = pe32->e32_impmod;        /* 44  Offset of Import Module Name Table - POINTER */
        /*- impmodcnt*/
    pSMte->smte_impproc     = pe32->e32_impproc;       /* 48  Offset of Imp Procedure Name Tab - POINTER */
        /*- pagesum*/
    pSMte->smte_datapage    = pe32->e32_datapage;      /* 4c  Offset of Enumerated Data Pages */
        /*- preload*/
    pSMte->smte_nrestab     = pe32->e32_nrestab;       /* 50  Offset of Non-resident Names Table */
    pSMte->smte_cbnrestab   = pe32->e32_cbnrestab;     /* 54  Size of Non-resident Name Table */
        /*- nressum*/
    pSMte->smte_autods      = pe32->e32_autodata;      /* 58  Object # for automatic data object */
    pSMte->smte_debuginfo   = pe32->e32_debuginfo;     /* 5c  Offset of the debugging info */
    pSMte->smte_debuglen    = pe32->e32_debuglen;      /* 60  The len of the debug info in */
        /*- instpreload,instdemand*/
    pSMte->smte_heapsize    = pe32->e32_heapsize;      /* 64  use for converted 16-bit modules */
        /*- res3*/
    /* extra */
    pSMte->smte_path        = 0;                       /* 68  full pathname - POINTER */
    pSMte->smte_semcount    = 0;                       /* 6c  Count of threads waiting on MTE semaphore. 0 => semaphore is free */
    pSMte->smte_semowner    = 0;                       /* 6e  Slot number of the owner of MTE semahore */
    pSMte->smte_pfilecache  = 0;                       /* 70  Pointer to file cache for Dos32CacheModule */
    pSMte->smte_stacksize   = 0;                       /* 74  Thread 1 Stack size from the exe header */
    pSMte->smte_alignshift  = 0;                       /* 78  use for converted 16-bit modules */
    pSMte->smte_NEexpver    = 0;                       /* 7a  expver from NE header */
    pSMte->smte_pathlen     = 0;                       /* 7c  length of full pathname */
    pSMte->smte_NEexetype   = 0;                       /* 7e  exetype from NE header */
    pSMte->smte_csegpack    = 0;                       /* 80  count of segs to pack */

    /*
     * Copy filename (from where?)
     */
    pSMte->smte_path        = (PCHAR)(PVOID)pSMte + sizeof(SMTE);
    pSMte->smte_pathlen     = 0;
    memcpy(pSMte->smte_path, "", pSMte->smte_pathlen);


    /*
     * Read loader and fixup sections.
     */
    rc = fakeldrRead(plv->lv_sfn,
                     (ULONG)pSMte->smte_objtab + plv->lv_new_exe_off,
                     pch,
                     0,
                     pe32->e32_ldrsize + pe32->e32_fixupsize,
                     NULL);
    if (rc)
    {
        goto createmte_exit;
    }

    /*
     * Set pointers.
     */
    pSMte->smte_objtab = (POTE)(PVOID)pch;
    rc = fakeldrMTEValidatePtrs(pSMte,
                                (ULONG)pch + plv->lv_csmte,
                                (ULONG)pch - (ULONG)pe32->e32_objtab);
    if (rc)
    {
        goto createmte_exit;
    }

    /*
     * Set mte pointer in plv.
     * Set hobmte in plv.
     */
    plv->lv_pmte = pMte;
    plv->lv_hobmte = pMte->mte_handle;

    /*
     * Fix flags...
     */
    pMte->mte_flags1 |= plv->lv_class;

createmte_exit:
    printf("fakeldrCreateMte:       *exit*  pe32 = %p, plv = %d, rc = %d\n",
           pe32, plv, rc);

    return rc;
}



/**
 * Fake ldrLoadImports - loads the import modules for the given module (pmte).
 * @returns   OS2 error code.
 * @param     pmte  Pointer to mte to load imports for.
 * @sketch
 *  OS/2 kernel seems to do:
 *      Loop thru the importe module table (reverse order for NE files)
 *          IF VDD module THEN
 *              Uppercase the name.
 *              try find it using ldrFindModule
 *              fail if error, not found, not VDDMOD/MVDMMOD.
 *              Add to array of imported module.
 *          ENDIF
 *          IF Not found OR !MTEGETMTEDONE THEN
 *              call ldrGetMte to load the module (class global).
 *              fail if error.
 *              set MTEGETMTEDONE
 *              Add to array of imported module.
 *          ENDIF
 *          IF DOSMOD and !ldrMiniFileIOFlag THEN
 *              Replace module pointer with doscall1 pmte.
 *          ENDIF
 *      ENDLOOP
 *
 *  We do:
 *      Parameter validation.
 *      Loop thru the imported module table issuing fakeldrGetMte on each entry.
 *      'DOSCALLS' is ignored.
 *
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL   fakeldrLoadImports(PMTE pmte)
{
    int     i;                          /* Module index. */
    PSZ     psz;                        /* Module name pointer. (really pointer to pascal strings...) */
    PPMTE   papmte;                     /* Pointer to array of PMTEs for imported modules. */
    APIRET  rc;

    printf("fakeldrLoadImport:      *entry* pmte = %p, pmte->mte_impmodcnt = %d\n",
           pmte, pmte->mte_impmodcnt);

    /*
     * Parameter validation.
     */
    if (pmte == NULL)
    {
        kprintf(("fakeldrLoadImports: !ASSERTION! pmte == NULL\n"));
        return ERROR_INVALID_PARAMETER;
    }
    if (pmte->mte_swapmte == NULL)
    {
        kprintf(("fakeldrLoadImports: !ASSERTION! pmte->mte_swapmte == NULL\n"));
        return ERROR_INVALID_PARAMETER;
    }

    /*
     * Any imported modules?
     */
    if (pmte->mte_impmodcnt == 0)
    {
        printf("fakeldrLoadImport:      *exit*  pmte = %p, rc = %d\n",
               pmte, rc);
        return NO_ERROR;
    }

    /* We have reserved space for imported module pointers after the mte. */
    papmte = (PPMTE)((unsigned)pmte  + sizeof(MTE));

    /*
     * Process the imported module table.
     */
    for (i = 0, rc = NO_ERROR, psz = (PSZ)pmte->mte_swapmte->smte_impmod;
         i < pmte->mte_impmodcnt && rc == NO_ERROR;
         i++, psz += 1 + *psz)
    {
        if (*psz == 8 && !strnicmp(psz+1, "DOSCALLS", 8))
            continue;

        rc = fakeldrGetMte(psz + 1, *psz, LVTYPE_DLL, CLASS_GLOBAL, &papmte[i]);
    }

    printf("fakeldrLoadImport:      *exit*  pmte = %p, rc = %d\n",
           pmte, rc);

    return rc;
}



/**
 * Uppercases the string.
 * @param     pachString    String to uppercase.
 * @param     cchString     Length of string.
 * @sketch    Loop thru the string converting all english letters to uppercase.
 * @status    partially implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    This is probably written in assembly and does DBCS checks...
 */
VOID LDRCALL    fakeldrUCaseString(PCHAR pachString, USHORT cchString)
{
    printf("fakeldrUCaseString:             pachString = %.*s, cchString = %#8x\n",
           cchString, pachString, cchString);

    while (cchString-- != 0)
    {
        if (*pachString >= 'a' && *pachString <= 'z')
            *pachString -= ('a' - 'A');
        pachString++;
    }
}




/**
 * Checks and updates the members of the swap-mte which is to be
 * pointers into the loader/fixup section. An array, validatetbl, contains
 * the offsets of the smte pointers.
 *
 * @returns     NO_ERROR    on success.
 *              ERROR_BAD_EXE_FORMAT on error.
 * @param       psmte
 * @param       ulMaxAddr   Maximum (exclusive) address.
 * @param       off         Offset delta to add to the pointer members (which
 *                          are all offsets relative to the start of the new
 *                          header) to make them pointers.
 * @sketch
 *  Loop thru the validatetbl and update each field (which isn't null) by adding
 *  the off value, and validate that the pointer is ok.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
ULONG LDRCALL   fakeldrMTEValidatePtrs(PSMTE psmte, ULONG ulMaxAddr, ULONG off)
{
    int     i;

    printf("fakeldrMTEValidatePtrs: *entry* psmte = %p, ulMaxAddr = %p, off = %p\n",
           psmte, ulMaxAddr, off);

    for (i = 0; validatetbl[i] != 0xDEAD; i++)
    {
        PULONG  pul = (PULONG)(PVOID)((PCHAR)(PVOID)psmte + validatetbl[i]);
        if (*pul != 0UL)
        {
            *pul += off;
            if (*pul >= ulMaxAddr)
            {
                printf("fakeldrMTEValidatePtrs: *exit*  psmte = %p, ulMaxAddr = %p, off = %p, rc = %d\n",
                       psmte, ulMaxAddr, off, ERROR_BAD_EXE_FORMAT);
                return ERROR_BAD_EXE_FORMAT;
            }
        }
    }

    printf("fakeldrMTEValidatePtrs: *exit*  psmte = %p, ulMaxAddr = %p, off = %p, rc = %d\n",
           psmte, ulMaxAddr, off, NO_ERROR);
    return NO_ERROR;
}

