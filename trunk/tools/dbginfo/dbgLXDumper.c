/* $Id: dbgLXDumper.c,v 1.3 2000-03-26 21:56:37 bird Exp $
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
#define INCL_TYPES
#define INCL_DOSERRORS
#define FOR_EXEHDR          1           /* exe386.h flag */
#define DWORD               ULONG       /* Used by exe386.h / newexe.h */
#define WORD                USHORT      /* Used by exe386.h / newexe.h */

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
    PHLLDIR     pDir;
    int         i;
    PHLLHDR     pHdr = (PHLLHDR)pb;

    /*
     * Dump header.
     */
    fprintf(phOut,
            "- HLL header -\n"
            "    Signature          %.4s\n"
            "    Directory offset   0x%08x (%d)\n"
            "    reserved           0x%08x (%d)\n"
            "\n",
            pHdr->achSignature,
            pHdr->offDirectory,
            pHdr->offDirectory,
            pHdr->ulReserved,
            pHdr->ulReserved);


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
            "    Reserved           0x%08x (%d)\n"
            "    Number of entries  0x%08x (%d)\n",
            pDir->ulReserved,
            pDir->ulReserved,
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
                int         j, c;

                /*
                 * Dump module entry data.
                 */
                fprintf(phOut,
                        "    Modulename:   %.*s\n"
                        "    overlay       %d\n"
                        "    ilib          %d\n"
                        "    pad           %d\n"
                        "    usDebugStyle  %#04x %c%c\n"
                        "    HLL Version   %d.%d\n"
                        "    cchName       %d\n"
                        ,
                        pModule->cchName,
                        &pModule->achName[0],
                        pModule->overlay,
                        pModule->iLib,
                        pModule->pad,
                        pModule->usDebugStyle,
                        pModule->usDebugStyle & 0xFF,
                        pModule->usDebugStyle >> 8,
                        pModule->chVerMajor,
                        pModule->chVerMinor,
                        pModule->cchName
                        );


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
                for (j = 0; j < c; j++)
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

            case HLL_DE_IBMSRC:         /* Line numbers - (IBM HLL) */
                break;

            default:
                fprintf(phOut, "    Error - unknown entry type. (%x)\n", pDir->aEntries[i].usType);
        }

    }


    /* - temporary - */
    printf("\ndumps debug data\n");
    dumpHex(stdout, pb, cb);

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

