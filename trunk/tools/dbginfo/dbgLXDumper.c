/* $Id: dbgLXDumper.c,v 1.7 2000-04-07 02:48:03 bird Exp $
 *
 * dbgLXDumper - reads and interprets the debuginfo found in an LX executable.
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
#define FOR_EXEHDR          1           /* exe386.h flag */
#define DWORD               ULONG       /* Used by exe386.h / newexe.h */
#define WORD                USHORT      /* Used by exe386.h / newexe.h */

#define HLLVERSION100       0x0100
#define HLLVERSION300       0x0300
#define HLLVERSION400       0x0400
#define HLLVERSION500       0x0500


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <newexe.h>
#include <exe386.h>

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>

#include "hll.h"



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int          dbgLXDump(void *pvFile);
int          dumpHLL(FILE *phOut, PBYTE pb, int cb);
void         dumpHex(FILE *phOut, PBYTE pb, int cb);
void *       readfile(const char *pszFilename);
signed long  fsize(FILE *phFile);



/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static char achBufferDummy64[0x10000] = {0};
char achBufferDummy128[0x20000] = {0};

int main(int argc, char **argv)
{
    void * pvFile;

    if (argc != 2)
    {
        fprintf(stderr, "error parameters!\n");
        return -1;
    }

    pvFile = readfile(argv[1]);
    if (pvFile == NULL)
    {
        fprintf(stderr, "error reading file %s\n", argv[1]);
        return -2;
    }

    return dbgLXDump(pvFile);
}



/**
 * Dumps the internals of LX dubug info.
 * @returns   error code. (0 is ok)
 * @param     pvFile  Pointer to filemapping.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int dbgLXDump(void *pvFile)
{
    struct exe_hdr * pehdr = (struct exe_hdr *) pvFile;
    struct e32_exe * pe32;
    unsigned         offe32;
    PBYTE pbFile = (PBYTE)pvFile;

    /*
     * Find LX header.
     */
    if (pehdr->e_magic == EMAGIC)
        offe32 = pehdr->e_lfanew;
    else
        offe32 = 0;
    pe32 = (struct e32_exe *)((unsigned)pvFile + offe32);
    if (pe32->e32_magic[0] != E32MAGIC1 || pe32->e32_magic[1] != E32MAGIC2)
    {
        fprintf(stderr, "not LX executable\n");
        return ERROR_INVALID_EXE_SIGNATURE;
    }

    /*
     * Check if there is any debuginfo in this executable.
     */
    printf("e32_debuginfo   0x%08x (%d)\n"
           "e32_debuglen    0x%08x (%d)\n",
           pe32->e32_debuginfo, pe32->e32_debuglen);

    if (pe32->e32_debuginfo != 0 && pe32->e32_debuglen > 0)
    {
        PBYTE pbDbg = pbFile + pe32->e32_debuginfo;
        /*
         * Check signature. 'NB0'
         */
        printf("Debug signature: %c%c%c%c\n",
               pbDbg[0], pbDbg[1], pbDbg[2], pbDbg[3]);
        if (pbDbg[0] == 'N' && pbDbg[1] == 'B' && pbDbg[2] == '0')
        {
            int i;

            /*
             * Switch on debug datatype.
             */
            switch (pbDbg[3])
            {
                case '0':
                    printf("Found 32-bit Codeview format\n");
                    break;

                case '1':
                    printf("Found AIX Debugger format - unsupported\n");
                    break;

                case '2':
                    printf("Found 16-bit Codeview format\n");
                    break;

                case '4':
                    printf("Found 32-bit OS/2 PM Debugger format (HLL)\n");
                    return dumpHLL(stdout, pbDbg, pe32->e32_debuglen);

                default:
                    printf("Invalid debug type, %c (%d)\n", pbDbg[3], pbDbg[3]);
                    return ERROR_INVALID_DATA;
            }

            /*
             * Dump debug data
             */
            printf("\ndumps debug data\n");
            dumpHex(stdout, pbDbg + 4, pe32->e32_debuglen - 4);
        }
        else
        {
            printf("Invalid debug signature\n");
            return ERROR_INVALID_DATA;
        }
    }
    else
        printf(" - no debug info -\n");

    return NO_ERROR;
}


/**
 * Dumps binary data to file handle.
 * @param     phOut  Output file handle.
 * @param     pb     Pointer to debug data. (Starts with signature ('NB04').)
 * @param     cb     Size of debug data.
 *
 *  HLL:
 *  Starts with a 4 byte word with the offset (from start of HLL data) to
 *  the number of entries. (what entries is yet to come)
 *
 *
 */
int dumpHLL(FILE *phOut, PBYTE pb, int cb)
{
    int             i, j, k;            /* loop variables! */
    unsigned long   ulHLLVersion = 0;   /* HLL version of the last module. */
    PHLLDIR         pDir;
    PHLLHDR         pHdr = (PHLLHDR)pb;

    /*
     * Dump header.
     */
    fprintf(phOut,
            "- HLL header -\n"
            "    Signature          %.4s\n"
            "    Directory offset   0x%08x (%d)\n"
            "\n",
            pHdr->achSignature,
            pHdr->offDirectory,
            pHdr->offDirectory);


    /*
     * Get and Dump directory
     */
    if (pHdr->offDirectory + sizeof(HLLDIR) > cb)
    {
        fprintf(phOut, "error: offDirectory is incorrect! (cb=%d, off=%d)\n",
                cb, pHdr->offDirectory);
        return ERROR_INVALID_DATA;
    }
    pDir = (PHLLDIR)(pb + pHdr->offDirectory);
    fprintf(phOut,
            "- HLL Directory -\n"
            "    Size of this struct  0x%02x (%d)\n"
            "    Size directory entry 0x%02x (%d)\n"
            "    Number of entries    0x%08x (%d)\n",
            pDir->cb,
            pDir->cb,
            pDir->cbEntry,
            pDir->cbEntry,
            pDir->cEntries,
            pDir->cEntries);


    /*
     * Directory sanity check - check that it's not too big
     */
    if ((PBYTE)&pDir->aEntries[pDir->cEntries] - pb > cb)
    {
        fprintf(phOut, "Error: Directory is to big!\n");
        return ERROR_INVALID_DATA;
    }



    /*
     *  Loop thru the directory.
     */
    for (i = 0; i < pDir->cEntries; i++)
    {
        /*
         * Directory entry type descriptions.
         */
        static const char * apsz[] =
        {
            "HLL_DE_MODULES",
            "HLL_DE_PUBLICS",
            "HLL_DE_TYPES",
            "HLL_DE_SYMBOLS",
            "HLL_DE_SRCLINES",
            "HLL_DE_LIBRARIES",
            "unknown",
            "unknown",
            "HLL_DE_SRCLNSEG",
            "unknown",
            "HLL_DE_IBMSRC"
        };
        const char *pszType = pDir->aEntries[i].usType >= HLL_DE_MODULES
                              && pDir->aEntries[i].usType <= HLL_DE_IBMSRC
                              ? apsz[pDir->aEntries[i].usType - HLL_DE_MODULES]
                              : "unknown";

        /*
         * Dump directroy info.
         */
        fprintf(phOut, "\n"
                       "- HLL Directory Entry %d (0x%x): -\n", i, i);
        fprintf(phOut, "    usType  0x%08x (%d) %s\n"
                       "    iMod    0x%08x (%d)\n"
                       "    off     0x%08x (%d)\n"
                       "    cb      0x%08x (%d)\n",
                       pDir->aEntries[i].usType,
                       pDir->aEntries[i].usType,
                       pszType,
                       pDir->aEntries[i].iMod,
                       pDir->aEntries[i].iMod,
                       pDir->aEntries[i].off,
                       pDir->aEntries[i].off,
                       pDir->aEntries[i].cb,
                       pDir->aEntries[i].cb
                );



        /*
         * Switch between the different entry types to do individual
         * processing.
         */
        switch (pDir->aEntries[i].usType)
        {
            /*
             * Module - debuginfo on an object/source module.
             */
            case HLL_DE_MODULES:
            {
                PHLLMODULE  pModule = (PHLLMODULE)(pDir->aEntries[i].off + pb);
                PHLLSEGINFO paSegInfo;
                int         c;

                /*
                 * Dump module entry data.
                 */
                fprintf(phOut,
                        "    Modulename:   %.*s\n"
                        "    overlay       %d\n"
                        "    ilib          %d\n"
                        "    pad           %d\n"
                        "    cSegInfo      %d\n"
                        "    usDebugStyle  %#04x %c%c\n"
                        "    HLL Version   %d.%d\n"
                        "    cchName       %d\n"
                        ,
                        pModule->cchName,
                        &pModule->achName[0],
                        pModule->overlay,
                        pModule->iLib,
                        pModule->pad,
                        pModule->cSegInfo,
                        pModule->usDebugStyle,
                        pModule->usDebugStyle & 0xFF,
                        pModule->usDebugStyle >> 8,
                        pModule->chVerMajor,
                        pModule->chVerMinor,
                        pModule->cchName
                        );

                ulHLLVersion = pModule->chVerMajor*0x100 + pModule->chVerMinor;


                /*
                 * Dump Segment info
                 */
                fprintf(phOut,
                        "    SegmentInfo %d\n"
                        "      iObject   %#x\n"
                        "      off       %#x\n"
                        "      cb        %#x\n",
                        0,
                        pModule->SegInfo0.iObject,
                        pModule->SegInfo0.off,
                        pModule->SegInfo0.cb);

                c = pModule->cSegInfo > 0 ? pModule->cSegInfo : 0;
                paSegInfo = (PHLLSEGINFO)((void*)&pModule->achName[pModule->cchName]);
                for (j = 0; j + 1 < c; j++)
                {
                    fprintf(phOut,
                        "    SegmentInfo %d\n"
                        "      iObject   %#x\n"
                        "      off       %#x\n"
                        "      cb        %#x\n",
                        j + 1,
                        paSegInfo[j].iObject,
                        paSegInfo[j].off,
                        paSegInfo[j].cb);
                }
                break;
            }


            case HLL_DE_PUBLICS:        /* Public symbols */
            {
                PHLLPUBLICSYM   pPubSym = (PHLLPUBLICSYM)(pDir->aEntries[i].off + pb);

                while ((char *)pPubSym - pb - pDir->aEntries[i].off < pDir->aEntries[i].cb)
                {
                    fprintf(phOut,
                            "    %#03x:%#08x iType=%#2x  name=%.*s\n",
                            pPubSym->iObject,
                            pPubSym->off,
                            pPubSym->iType,
                            pPubSym->cchName,
                            pPubSym->achName);

                    /* next */
                    pPubSym = (PHLLPUBLICSYM)&pPubSym->achName[pPubSym->cchName];
                }
                break;
            }


            case HLL_DE_TYPES:          /* Types */
                break;

            case HLL_DE_SYMBOLS:        /* Symbols */
                break;

            case HLL_DE_LIBRARIES:      /* Libraries */
                break;

            case HLL_DE_SRCLINES:       /* Line numbers - (IBM C/2 1.1) */
                break;

            case HLL_DE_SRCLNSEG:       /* Line numbers - (MSC 6.00) */
                break;

            /*
             * Line numbers - (IBM HLL)
             *
             * HLL 04 have a FirstEntry before each table.
             * HLL 03 don't seem to have... This is not implemented yet.
             */
            case HLL_DE_IBMSRC:
            {
                PHLLFIRSTENTRY      pFirstEntry = (PHLLFIRSTENTRY)(pb + pDir->aEntries[i].off);
                int                 cbFirstEntry;
                int                 cb;


                /*
                 * Set the size of an first entry struct based on the HLL version.
                 */
                if (ulHLLVersion == HLLVERSION100)
                    cbFirstEntry = sizeof(pFirstEntry->hll01);
                else
                    cbFirstEntry = sizeof(pFirstEntry->hll04);


                /*
                 * Loop thru all the arrays in this data directory.
                 * Each array starts with an HLLFIRSTENTRY structure.
                 */
                cb = pDir->aEntries[i].cb;
                while (cb >= cbFirstEntry)
                {
                    int     cbEntries;

                    /*
                     * Dump the special first entry.
                     */
                    fprintf(phOut,
                            "    First entry:\n"
                            "      usLine           0x%04x\n"
                            "      uchType          0x%02x\n"
                            "      uchReserved      0x%02x\n"
                            "      cEntries         0x%04x\n"
                            "      iSeg             0x%04x\n"
                            "      offBase/cb       0x%08x\n",
                            pFirstEntry->hll04.usLine,
                            pFirstEntry->hll04.uchType,
                            pFirstEntry->hll04.uchReserved,
                            pFirstEntry->hll04.cEntries,
                            pFirstEntry->hll04.iSeg,
                            pFirstEntry->hll04.u1.offBase
                            );

                    switch (pFirstEntry->hll03.uchType)
                    {
                        /*
                         * Source File information and offset only.
                         */
                        case 0:
                        {
                            int                 cbLine;
                            PHLLLINENUMBERENTRY pLines =
                                (PHLLLINENUMBERENTRY)((char*)pFirstEntry + cbFirstEntry);

                            /*
                             * Determin size of a line entry.
                             */
                            if (ulHLLVersion == HLLVERSION100)
                                cbLine = sizeof(pLines->hll01);
                            else
                                cbLine = sizeof(pLines->hll03);

                            /*
                             * Loop thru all the line info and dump it.
                             */
                            fprintf(phOut, "      Lineinfo:\n");
                            for (k = 0; k < pFirstEntry->hll01.cEntries; k++) /* cEntries is similar for all formats. */
                            {
                                fprintf(phOut,
                                        "          usLine=%4d (0x%02x)  iusSourceFile=0x%04x  off=0x%08x\n",
                                        pLines->hll04.usLine,
                                        pLines->hll04.usLine,
                                        pLines->hll04.iusSourceFile,
                                        pLines->hll04.off
                                        );
                                /* next */
                                pLines = (PHLLLINENUMBERENTRY)((char*)pLines + cbLine);
                            }

                            cbEntries = cbLine * pFirstEntry->hll01.cEntries; /* cEntries is similar for all formats. */
                            break;
                        }


                        /*
                         * Filenames.
                         */
                        case 3:
                        {
                            PCHAR               pch;
                            PHLLFILENAMEENTRY   pFilenameEntry =
                                (PHLLFILENAMEENTRY)((char*)pFirstEntry + cbFirstEntry);

                            fprintf(phOut,
                                    "    FilenameEntry:\n"
                                    "      offSource        0x%08x\n"
                                    "      cSourceRecords   0x%08x\n"
                                    "      cSourceFiles     0x%08x\n"
                                    "      cchName          0x%02x\n",
                                    pFilenameEntry->offSource,
                                    pFilenameEntry->cSourceRecords,
                                    pFilenameEntry->cSourceFiles,
                                    pFilenameEntry->cchName
                                    );

                            fprintf(phOut,
                                    "      Filenames:\n");
                            pch = &pFilenameEntry->cchName;
                            cbEntries = 0;
                            for (k = 0; k < pFilenameEntry->cSourceFiles; k++)
                            {
                                fprintf(phOut,
                                        "        %.*s\n", *pch, pch+1);
                                /* next */
                                cbEntries += 1 + *pch;
                                pch += 1 + *pch;
                            }

                            if (ulHLLVersion == HLLVERSION100)
                                cbEntries = pFirstEntry->hll01.u1.cbFileNameTable;
                            else
                                cbEntries = pFirstEntry->hll03.u1.cbFileNameTable;
                            cbEntries += offsetof(HLLFILENAMEENTRY, cchName);
                            break;
                        }


                        default:
                            fprintf(phOut, "warning: unsupported entry type, %d\n", pFirstEntry->hll03.uchType);
                            cbEntries = cb = 0;
                    }


                    /*
                     * Next
                     */
                    cb -= cbEntries + sizeof(HLLFIRSTENTRY);
                    pFirstEntry = (PHLLFIRSTENTRY)((char*)pFirstEntry + cbEntries + sizeof(HLLFIRSTENTRY));
                }

                dumpHex(phOut,
                        pDir->aEntries[i].off + pb,
                        pDir->aEntries[i].cb);
                break;
            }

            default:
                fprintf(phOut, "    Error - unknown entry type. (%x)\n", pDir->aEntries[i].usType);
        }

    }


    /* - temporary - */
    printf("\ndumps debug data\n");
    dumpHex(phOut, pb, cb);

    return NO_ERROR;
}



/**
 * Dumps binary data to file handle.
 * @param     phOut  Output file handle.
 * @param     pb     Pointer to data.
 * @param     cb     Count of bytes to dump.
 */
void dumpHex(FILE *phOut, PBYTE pb, int cb)
{
    int i;

    for (i = 0; i < cb; i += 16)
    {
        int j;
        /* write offset */
        fprintf(phOut, "%08x  ", i);

        /* write data (hex value) */
        for (j = 0; j < 16; j++)
        {
            int           f     = i + j < cb;
            unsigned char uch   = f ? pb[i + j] : 0;
            if (j == 3 || j == 11)
                fprintf(phOut, f ? "%02x-"   : "  -",   uch);
            else if (j == 7)
                fprintf(phOut, f ? "%02x - " : "   - ", uch);
            else
                fprintf(phOut, f ? "%02x " :   "   ",   uch);
        }
        fprintf(phOut, "  ");

        /* write ASCII */
        for (j = 0; j < 16; j++)
        {
            if (i + j < cb)
            {
                if (isprint(pb[i + j]))
                    fprintf(phOut, "%c", pb[i + j]);
                else
                    fprintf(phOut, ".");
            }
            else
                fprintf(phOut, " ");
        }
        fprintf(phOut, "\n");
    }
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
            pvFile = malloc(cbFile + 1);
            if (pvFile != NULL)
            {
                memset(pvFile, 0, cbFile + 1);
                if (fread(pvFile, 1, cbFile, phFile) == 0)
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



/**
 * Find the size of a file.
 * @returns   Size of file. -1 on error.
 * @param     phFile  File handle.
 */
signed long fsize(FILE *phFile)
{
    int ipos;
    signed long cb;

    if ((ipos = ftell(phFile)) < 0
        ||
        fseek(phFile, 0, SEEK_END) != 0
        ||
        (cb = ftell(phFile)) < 0
        ||
        fseek(phFile, ipos, SEEK_SET) != 0
        )
        cb = -1;
    return cb;
}

