/* $Id: dbgLXDumper.c,v 1.1 2000-03-24 18:13:44 bird Exp $
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
#define DWORD ULONG
#define WORD USHORT

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <newexe.h>
#include <exe386.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

#include "hll.h"



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static int          dbgLXDump(void *pvFile);
static int          dumpHLL(FILE *phOut, PBYTE pb, int cb);
static void         dumpHex(FILE *phOut, PBYTE pb, int cb);
static void *       readfile(const char *pszFilename);
static signed long  fsize(FILE *phFile);





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
static int dbgLXDump(void *pvFile)
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
                    return dumpHLL(stdout, pbDbg + 4, pe32->e32_debuglen - 4);

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
 * @param     pb     Pointer to debug data.
 * @param     cb     Size of debug data.
 *
 *  HLL:
 *  Starts with a 4 byte word with the offset (from start of HLL data) to
 *  the number of entries. (what entries is yet to come)
 *
 *
 */
static int dumpHLL(FILE *phOut, PBYTE pb, int cb)
{
    PHLLDIR     pDir;
    ULONG       offDir;
    int         i;

    /*
     * Get number of entries.
     */
    offDir = *(PULONG)pb;
    if (offDir + 4 >= cb)
    {
        fprintf(phOut, "error: offcEntries is incorrect!\n");
        return ERROR_INVALID_DATA;
    }
    pDir = (PHLLDIR)(pb + offDir);
    fprintf(phOut, "Directory offset=0x%08x  cEntries=%d(0x%x)\n",
            offDir, pDir->cEntries, pDir->cEntries);


    /*
     * Directory sanity check.
     */
    if ((PBYTE)&pDir->aEntries[pDir->cEntries] - pb >= cb)
    {
        fprintf(phOut, "Error: Directory is to big!\n");
        return ERROR_INVALID_DATA;
    }



    /*
     *  Loop thru the directory.
     */
    for (i = 0; i < pDir->cEntries; i++)
    {
        fprintf(phOut, "Directory Entry %d (0x%x):\n", i, i);
        fprintf(phOut, "    usType  0x%08x (%d)\n"
                       "    iMod    0x%08x (%d)\n"
                       "    off     0x%08x (%d)\n"
                       "    cb      0x%08x (%d)\n",
                       pDir->aEntries[i].usType,
                       pDir->aEntries[i].usType,
                       pDir->aEntries[i].iMod,
                       pDir->aEntries[i].iMod,
                       pDir->aEntries[i].off,
                       pDir->aEntries[i].off,
                       pDir->aEntries[i].cb,
                       pDir->aEntries[i].cb
                );

        switch (pDir->aEntries[i].usType)
        {
            case HLL_DE_MODULES:        /* Filename */
                printf("    Modulename:     %*.s\n",
                       pDir->aEntries[i].cb,
                       pDir->aEntries[i].off + pb);
                dumpHex(phOut, pDir->aEntries[i].off + pb, pDir->aEntries[i].cb);
                break;

            case HLL_DE_PUBLICS:        /* Public symbols */
                break;

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
static void dumpHex(FILE *phOut, PBYTE pb, int cb)
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
static void *readfile(const char *pszFilename)
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
static signed long fsize(FILE *phFile)
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

