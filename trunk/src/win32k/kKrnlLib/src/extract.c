/* $Id: extract.c,v 1.2 2002-03-31 19:01:16 bird Exp $
 *
 * Description:     SymDB entry generator.
 *                  Builds SymDB entry from one or more symbol files.
 *
 * WARNING: For some stupid reason we usually crash when not redirecting STDOUT.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* Disable logging */
#define NOLOGGING 1

#define DB_ASM

#define fclose(a) DosClose(a)
#define SEEK_SET FILE_BEGIN
#define SEEK_END FILE_END

#define WORD unsigned short int
#define DWORD unsigned long int

#define INCL_BASE
#define INCL_DOS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_LDR

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <strat2.h>
#include <reqpkt.h>

#include "devSegDf.h"
#undef  DATA16_INIT
#define DATA16_INIT
#undef  CODE16_INIT
#define CODE16_INIT
#include "os2Krnl.h"                    /* must be included before dev1632.h! */
#include "probkrnl.h"
#include "dev1632.h"
#include "options.h"
#include "d16vprintf.h"
#include "d16crt.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/* Result from GetKernelInfo/ReadOS2Krnl. */
extern unsigned char    cObjects;
extern POTE             paKrnlOTEs;
unsigned long           cbSize;
unsigned long           cbAllSize;

/* dummy replacement for SymDB.c */
KRNLDBENTRY   DATA16_INIT    aKrnlSymDB[] = {{0}};

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int      processFile(const char *pszFilename);


/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
/* Workers */
extern  int      ProbeSymFile(const char *pszFilename);

/* 32-bit functions */
extern USHORT cdecl far GetOpcodes(const char far * pszKrnlFile, unsigned short cSymObjects);
extern void   cdecl far Init32bitCrt(void);



/**
 * Dumps writes a KRNLDBENTRY struct to stderr for the given .sym-file.
 * The filesnames are on this format:
 *    nnnn[n]tm[r].SYM
 * Where: n - are the build number 4 or 5 digits.
 *        t - kernel type. R = retail, H = half strict, A = all strict.
 *        m - UNI or SMP.  U = UNI processor kernel. S = SMP processor kernel. 4 = Warp 4 FP13+
 *        r - revision letter. Currently only 'A' is supported.
 * @returns   NO_ERROR on success. Untracable error code on error.
 * @param     pszFilename  Pointer to read only filename of the .sym-file.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark    Currently only retail kernels are processed. See note below.
 */
static int processFile(const char *pszFilename)
{
    APIRET   rc;
    int      cch;
    int      cchNum;
    const char *psz = pszFilename + kstrlen(pszFilename);

    /* find filename */
    cch = 0;
    while (psz >= pszFilename && *psz != '\\' && *psz != '/' && *psz != ':')
        psz--, cch++;
    psz++;
    cch--;

    /* Progress information */
    DosWrite(2, (char*)pszFilename, cch, &rc);
    DosWrite(2, "\r\n", 2, &rc);

    /* Filename check */
    cchNum = psz[0] > '2' ? 4 : 5;      /* build number length. */
    if (cch < 10 || cch > 12
        || !(psz[0] >= '0' && psz[0] <= '9')
        || !(psz[1] >= '0' && psz[1] <= '9')
        || !(psz[2] >= '0' && psz[2] <= '9')
        || !(psz[3] >= '0' && psz[3] <= '9')
        || !(cchNum == 4 || (psz[4] >= '0' && psz[4] <= '9'))
        || !(psz[cchNum] == 'A' || psz[cchNum] == 'H' || psz[cchNum] == 'R')
        || !(psz[cchNum+1] == 'S' || psz[cchNum+1] == 'U' || psz[cchNum+1] == '4')
/*        || !(cch != 12 || psz[cchNum+2] == 'A') */
        )
    {
        printf16("invalid filename: %s\n", pszFilename);
        return 2;
    }

    /*
     * Probe kernelfile
     */
    rc = ProbeSymFile(pszFilename);


    /*
     * on success dump a struct for this kernel
     */
    if (rc == 0)
    {
        int     i;
        char    szKrnlFile[CCHMAXPATH];
        int     cObjects;

        /*
         * Determin the total number of segments.
         */
        for (i = 0, cObjects = 0; i < NBR_OF_KRNLIMPORTS; i++)
        {
            if (aImportTab[i].fFound && (int)aImportTab[i].iObject >= cObjects)
                cObjects = aImportTab[i].iObject + 1;
        }

        /*
         * Call 32-bit helper to check if there is a kernel.
         */
        kstrcpy(szKrnlFile, pszFilename);
        szKrnlFile[kstrlen(szKrnlFile) - 4] = '\0';
        if (GetOpcodes(szKrnlFile, cObjects))
        {
            printf16("GetOpcodes failed: %s\n", szKrnlFile);
            return 3;
        }


        /** @remark
         * Currently information for retail kernels are usable, but we'll
         * generate it for the debug kernels too, but this information
         * is enclaved within an "#ifdef ALLKERNELS ... #endif".
         */
        #ifndef DB_ASM
            if (psz[cchNum] != 'R')
                printf16("#ifdef ALLKERNELS\n");

            printf16("    { /* %s */\n"
                     "        %.*s, ",
                     psz,
                     cchNum, &psz[0]    /* build number */
                     );

            switch (psz[cchNum + 1])
            {
                case 'S':   printf16("KF_SMP"); break;
                case '4':   printf16("KF_UNI | KF_W4"); break;
                case 'U':   printf16("KF_UNI"); break;
            }
            switch (psz[cchNum])
            {
                case 'A':   printf16(" | KF_ALLSTRICT"); break;
                case 'H':   printf16(" | KF_HALFSTRICT"); break;
            }
            if (psz[cchNum + 2] >= 'A' && psz[cchNum + 2] <= 'Z')
                printf16(" | KF_REV_%c", psz[cchNum + 2]);

            printf16(", %d,\n"
                     "        {\n",
                     cObjects);

            for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
            {
                char *psz = aImportTab[i].achName;
                printf16("            {%-2d, 0x%08lx, 0x%02x}, /* %s */\n",
                         aImportTab[i].fFound ? aImportTab[i].iObject   : 0,
                         aImportTab[i].fFound ? aImportTab[i].offObject : 0xFFFFFFFFUL,
                         aImportTab[i].fFound ? aImportTab[i].chOpcode  : OPCODE_IGNORE,
                         (char *)&aImportTab[i].achName[0]
                         );
            }
            printf16("        }\n"
                     "    },\n");

            /** @remark
             * We generate #ifdef ALLKERNELS for debug kernels since we usually
             * don't include symbol info in the database.
             * OLD:
             * Currently information for retail kernels are usable, but we'll
             * generate it for the debug kernels too, but this information
             * is enclaved within an "#ifdef ALLKERNELS ... #endif".
             */
            if (psz[cchNum] != 'R')
                printf16("#endif\n");
        #else
            if (psz[cchNum] != 'R')
                printf16("ifdef ALLKERNELS\n");

            printf16(";/* %s */\n"
                     "  dw %.*s %*s; build no.\n",
                     psz,
                     cchNum, &psz[0], 4-cchNum, ""
                     );

            i = 0;                      /* flags */
            switch (psz[cchNum + 1])
            {
                case 'S':   i |= KF_SMP; break;
                case '4':   i |= KF_UNI | KF_W4; break;
                case 'U':   i |= KF_UNI; break;
            }
            switch (psz[cchNum])
            {
                case 'A':   i |= KF_ALLSTRICT; break;
                case 'H':   i |= KF_HALFSTRICT; break;
            }
            if (psz[cchNum + 2] >= 'A' && psz[cchNum + 2] <= 'Z')
                i |= (psz[cchNum + 2] - 'A' + (KF_REV_A >> KF_REV_SHIFT)) << KF_REV_SHIFT;

            printf16("  dw 0%04xh ; fKernel\n"
                     "  db 0%2xh   ; cObjects\n",
                     i,
                     cObjects);

            for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
            {
                PIMPORTKRNLSYM  pEntry = &aImportTab[i];
                printf16("    db 0%02xh      ; %s\n"
                         "    db 0%02xh\n"
                         "    dd 0%08lxh\n",
                         pEntry->fFound ? pEntry->iObject   : 0,
                         pEntry->achName,
                         pEntry->fFound ? pEntry->chOpcode  : OPCODE_IGNORE,
                         pEntry->fFound ? pEntry->offObject : 0xFFFFFFFFUL
                         );
            }

            /** @remark
             * We generate #ifdef ALLKERNELS for debug kernels since we usually
             * don't include symbol info in the database.
             * OLD:
             * Currently information for retail kernels are usable, but we'll
             * generate it for the debug kernels too, but this information
             * is enclaved within an "#ifdef ALLKERNELS ... #endif".
             */
            if (psz[cchNum] != 'R')
                printf16("endif\n");
        #endif

        /* update size */
        if (psz[cchNum] == 'R')
            cbSize += sizeof(KRNLDBENTRY);
        cbAllSize += sizeof(KRNLDBENTRY);
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
    const char *  psz;
    int     i;

    /*
     * Set paKrnlOTEs to point to an zeroed array of OTEs.
     */
    static KRNLINFO DATA16_INIT  KrnlInfo = {0};
    paKrnlOTEs = &KrnlInfo.aObjects[0];
    cbAllSize = cbSize = 0;

    /*
     * Init 32-bit CRT.
     */
    Init32bitCrt();

    /*
     * Check name lengths.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if (kstrlen(aImportTab[i].achName) != (int)aImportTab[i].cchName)
        {
            printf16("internal error - bad length of entry %d - %s. %d should be %d.\n",
                     i, aImportTab[i].achName, aImportTab[i].cchName, kstrlen(aImportTab[i].achName));
            return -1;
        }
    }

    /*
     * Do work.
     */
    if (argc > 1)
    {
        /*
         * Arguments: extract.exe <symfiles...>
         */
        int i;
        for (i = 1; i < argc; i++)
        {
            rc = processFile(argv[i]);
            if (rc != NO_ERROR)
            {
                printf16("processFile failed with rc=%d for file %s\n",
                         rc, argv[i]);
                if (psz = GetErrorMsg(rc))
                    printf16("%s\n", psz);
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

        #ifndef DB_ASM
            printf16("/* $Id: extract.c,v 1.2 2002-03-31 19:01:16 bird Exp $\n"
                     "*\n"
                     "* Autogenerated kernel symbol database.\n"
                     "*\n"
                     "* Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.com)\n"
                     "*\n"
                     "* Project Odin Software License can be found in LICENSE.TXT\n"
                     "*\n"
                     "*/\n");

            printf16("\n"
                     "#define INCL_NOPMAPI\n"
                     "#define INCL_NOBASEAPI\n"
                     "#include <os2.h>\n"
                     "#include \"DevSegDf.h\"\n"
                     "#include \"probkrnl.h\"\n"
                     "#include \"options.h\"\n"
                     "\n");

            #ifdef DB_16BIT
            printf16("const KRNLDBENTRY DATA16_INIT aKrnlSymDB[] = \n"
                     "{\n");
            #else /* 32-bit */
            printf16("#pragma data_seg(SYMBOLDB32)\n"
                     "const KRNLDBENTRY aKrnlSymDB32[] = \n"
                     "{\n");
            #endif
        #else
            printf16(";/* $Id: extract.c,v 1.2 2002-03-31 19:01:16 bird Exp $\n"
                     ";*\n"
                     ";* Autogenerated kernel symbol database.\n"
                     ";*\n"
                     ";* Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.com)\n"
                     ";*\n"
                     ";* Project Odin Software License can be found in LICENSE.TXT\n"
                     ";*\n"
                     ";*/\n");

            printf16("    .386p\n"
                     "\n"
                     ";\n"
                     "; Include files\n"
                     ";\n"
                     "    include devsegdf.inc\n"
                     "\n"
                     "\n"
                     ";\n"
                     "; Exported symbols\n"
                     ";\n"
            #ifdef DB_16BIT
                     "    public _aKrnlSymDB\n"
            #else /* 32-bit */
                     "    public aKrnlSymDB32\n"
            #endif
                     "\n"
                     "\n");

            #ifdef DB_16BIT
            printf16("DATA16_INIT segment\n"
                     "_aKrnlSymDB:\n");
            #else /* 32-bit */
            printf16("SYMBOLDB32 segment\n"
                     "aKrnlSymDB32:\n");
            #endif
        #endif

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
                if (psz = GetErrorMsg(rc))
                    printf16("%s\n", psz);
                return rc;
            }

            /* next file */
            rc = DosFindNext(hDir, &ffb, sizeof(ffb), &usSearch);
        }
        DosFindClose(hDir);

        #ifndef DB_ASM
            printf16("    { /* Terminating entry */\n"
                     "        0,0,0,\n"
                     "        {\n");
            for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
                printf16("            {0,0,0},\n");
            printf16("        }\n"
                     "    }\n"
            #ifdef DB_16BIT
                     "}; /* end of aKrnlSymDB[] */\n"
            #else
                     "}; /* end of aKrnlSymDB32[] */\n"
            #endif
                     );
        #else
            printf16(";/* Terminating entry */\n"
                     "    db %d dup(0)\n",
                     sizeof(KRNLDBENTRY));
            i = i;
        #endif

        cbSize += sizeof(KRNLDBENTRY);
        cbAllSize += sizeof(KRNLDBENTRY);
    }

    #ifndef DB_ASM
        printf16("\n"
                 "/* cbAllSize = %ld  %ld\n"
                 " * cbSize = %ld      %ld\n"
                 " */\n",
                 cbAllSize,  cbAllSize / sizeof(KRNLDBENTRY),
                 cbSize,     cbSize    / sizeof(KRNLDBENTRY));
    #else
        printf16("\n"
                 ";/* cbAllSize = %ld  %ld\n"
                 "; * cbSize = %ld      %ld\n"
                 "; */\n"
        #ifdef DB_16BIT
                 "DATA16_INIT ends\n"
        #else /* 32-bit */
                 "SYMBOLDB32 ends\n"
        #endif
                 "end\n",
                 cbAllSize,  cbAllSize / sizeof(KRNLDBENTRY),
                 cbSize,     cbSize    / sizeof(KRNLDBENTRY));
    #endif

    return rc;
}

