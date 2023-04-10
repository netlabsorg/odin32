/* $Id: xx2lxmain.cpp,v 1.2 2004-01-15 10:14:48 sandervl Exp $
 *
 * Xx2Lx main program. (Ring 3 only!)
 *
 * Copyright (c) 1999-2001 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define FOR_EXEHDR 1
#define INCL_BASE
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_OS2KRNL_LDR

#define DATA16_GLOBAL
#define OUTPUT_COM2     0x2f8

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <peexe.h>
#include <neexe.h>
#include <newexe.h>
#include <exe386.h>
#include "elf.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "OS2Krnl.h"
#include "modulebase.h"
#include "pe2lx.h"
#include "elf2lx.h"
#include "options.h"
#include <stdio.h>
#include <versionos2.h>
extern "C" {int     stricmp(const char *, const char *);}
extern "C" {off_t _filelength (int);}

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
char            szBackupXxFilename[CCHMAXPATH];        /* too save stack/heap */
struct options  options = DEFAULT_OPTION_ASSIGMENTS;

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax();


/**
 * Main function of the Pe2Lx utillity.
 * @returns    0 on success.
 *             1 Help.
 *             2 Syntax error: Invalid argument, '<arg>'.
 *             3 Syntax error: Too many filenames.
 *             4 Syntax error: No Xx-file specified.
 *            80 Fatal error: Can't find Xx-file, <filename>.
 *            81 Fatal error: Failed to rename the Xx-file, <from> -> <to>
 *            82 Fatal error: Failed to open Xx-file, <filename>. rc=<rc>
 *            83 Fatal error: Failed to read file signature. rc=<rc from DosRead(..)>
 *            84 Fatal error: Failed to convertert the file. rc=<rc from init(..)>
 *            85 Fatal error: Failed to write the Lx-file. rc=<rc from writeFile(..)>
 * @param     argc  Count of arguments.
 * @param     argv  Array of argument pointers. argc entries.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
int main(int argc, char **argv)
{
    APIRET rc;
    ULONG  ulAction = 0;
    PCSZ   pszXxFilename = NULL;
    PCSZ   pszLxFilename = NULL;
    PCSZ   psz;
    int    argi;

    /* special edition for Dave Evans */
    options.fPEOneObject = FLAGS_PEOO_DISABLED;

    /* read parameters */
    for (argi = 1; argi < argc; argi++)
    {
        /* check if option or filname */
        if (argv[argi][0] == '-' || argv[argi][0] == '/')
        {   /* option */
            switch (argv[argi][1])
            {
                case '1':   /* All-In-One-Object fix - temporary...- -1<-|+|*> */
                    if (argv[argi][2] == '-')
                        options.fPEOneObject = FLAGS_PEOO_DISABLED;
                    else if (argv[argi][2] == '+')
                        options.fPEOneObject = FLAGS_PEOO_ENABLED;
                    else
                        options.fPEOneObject = FLAGS_PEOO_FORCED;
                    break;


                case 'h': /* syntax help */
                case 'H':
                case '?':
                    syntax();
                    return 1;

                case 'w': /* warning level */
                case 'W':
                    psz = argv[argi] + (argv[argi][2] == ':' || argv[argi][2] == '=') + 2;
                    if (*psz >= '0' && *psz <= '4' && psz[1] == '\0')
                    {
                        switch (*psz)
                        {
                            case '0': ModuleBase::ulInfoLevel = ModuleBase::Quiet; break;
                            case '1': ModuleBase::ulInfoLevel = ModuleBase::Error; break;
                            case '2': ModuleBase::ulInfoLevel = ModuleBase::Warning; break;
                            case '3': ModuleBase::ulInfoLevel = ModuleBase::Info; break;
                            case '4': ModuleBase::ulInfoLevel = ModuleBase::InfoAll; break;
                        }
                    }
                    else
                    {
                        printf("Syntax error: Incorrect use of argument '%.2s'.\n\n", argv[argi]);
                        return 5;
                    }
                    break;


                case 'f': /* ignore internal fixups */
                case 'F':
                    options.fSkipFixups = TRUE;
                    break;

                case 'r': /* all read-write objects */
                case 'R':
                    if (argv[argi][2] != 'w' && argv[argi][2] != 'W')
                    {
                        printf("Syntax error: Invalid argument, '%s'\n", argv[argi]);
                        return 5;
                    }
                    options.fAllRWObjects = TRUE;
                    break;

                case 'c': /* custom odin dll name */
                case 'C':
                    if (!hasCustomExports())
                    {
                        printf("Syntax error: export table file not specified (-o:).\n\n");
                        return 5;
                    }
                    options.pszCustomDll = &argv[argi][3];
                    break;

                case 'o': /* custom odin dll ordinal mapping */
                case 'O':
                {
                    int fileIn = open(&argv[argi][3], O_RDONLY, S_IREAD);
                    int sizein = (int)_filelength(fileIn);

                    options.pszCustomExports = (PSZ)malloc(sizein+1);
                    memset(options.pszCustomExports, 0, sizein+1);
                    read(fileIn, options.pszCustomExports, sizein);
                    close(fileIn);
                    break;
                }

                case 'x': /* custombuild exclude dll */
                case 'X':
                {
                    int cch = strlen(&argv[argi][3]);
                    if (!cch)
                    {
                        printf("Syntax error: optino -x: requires a dll name!");
                        return 5;
                    }
                    int cchNew = cch + 4;
                    if (options.pszCustomDllExclude)
                        cchNew += strlen(options.pszCustomDllExclude);
                    options.pszCustomDllExclude = (char*)realloc(options.pszCustomDllExclude, cchNew);

                    char *psz = options.pszCustomDllExclude;
                    if (cchNew != cch + 4)
                        psz = psz + strlen(psz);

                    /* copy the name in uppercase with ';' at both ends. */
                    *psz++ = ';';
                    for (strcpy(psz, &argv[argi][3]); *psz; psz++)
                        if (*psz >= 'a' && *psz <= 'z')
                            *psz += ('A' - 'a');
                    *psz++ =';';
                    *psz = '\0';
                    break;
                }

                default:
                    printf("Syntax error: Invalid argument, '%s'\n", argv[argi]);
                    syntax();
                    return 2;
            }
        }
        else
        {
            if (pszXxFilename == NULL)
            {
                pszXxFilename = argv[argi];
                /* check if exists */
                rc = DosQueryPathInfo(pszXxFilename,FIL_QUERYFULLNAME,
                                      &szBackupXxFilename[0], sizeof(szBackupXxFilename));
                if (rc != NO_ERROR)
                {
                    printf("Fatal error: Can't find Xx-file, '%s'.\n", pszXxFilename);
                    return 80;
                }
            }
            else if (pszLxFilename == NULL)
                pszLxFilename = argv[argi];
            else
            {
                printf("Syntax error: Too many filenames!\n");
                syntax();
                return 3;
            }
        }
    } /* for */

    /* check if enough arguments */
    if (pszXxFilename == NULL)
    {
        printf("Syntax error: No Xx-file specified.\n\n");
        syntax();
        return 4;
    }

    /* rename files? */
    if (pszLxFilename == NULL)
    {
        char *pszExt = strrchr(pszXxFilename, '.');
        if (pszExt == NULL)
        {
            //printf("warning: Xx-file don't have an extention\n");
            strcpy(szBackupXxFilename, pszXxFilename);
            strcat(szBackupXxFilename, "wbk");
        }
        else
        {
            memset(szBackupXxFilename, 0, sizeof(szBackupXxFilename));
            if (stricmp(pszExt + 1, "exe") == 0)
            {
                strncpy(szBackupXxFilename, pszXxFilename, pszExt - pszXxFilename + 1);
                strcat(szBackupXxFilename, "exf");
            }
            else
            {
                strncpy(szBackupXxFilename, pszXxFilename, pszExt - pszXxFilename + 3);
                strcat(szBackupXxFilename, "k");
            }
        }
        rc = DosMove(pszXxFilename, szBackupXxFilename);
        if (rc != NO_ERROR)
        {
            printf("Fatal error: Failed to rename the Xx-file, %s -> %s\n",
                    pszXxFilename, szBackupXxFilename);
            return 81;
        }
        printInf(("Backuped %s to %s\n", pszXxFilename, szBackupXxFilename));

        /* switch name */
        pszLxFilename = pszXxFilename;
        pszXxFilename = szBackupXxFilename;
    }

    /*
     * Open input file
     */
    HFILE hFileXx;

    rc = DosOpen(pszXxFilename, &hFileXx, &ulAction, 0UL,
                 FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_FLAGS_RANDOMSEQUENTIAL | OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE,
                 NULL);
    if (rc == NO_ERROR)
    {
        ULONG       ulSignature;        /* Signature (four first bytes) of the file */
        ULONG       cbRead;             /* Signature bytes read - ignored */

        /*
         * Read signature.
         * If Elf signature Then create elf2lx object.
         * Else create pe2lx object.
         * Init the module.
         * Write LX File.
         * Dump virtual LX file
         */
        rc = DosRead(hFileXx, &ulSignature, sizeof(ulSignature), &cbRead);
        if (rc == NO_ERROR)
        {
            ModuleBase *pModule;        /* Pointer to module. */

            if (ulSignature == ELFMAGICLSB)
                pModule = new Elf2Lx(hFileXx);
            else
                pModule = new Pe2Lx(hFileXx);

            rc = pModule->init(pszLxFilename);
            if (rc == NO_ERROR)
            {
                rc = pModule->writeFile(pszLxFilename);
                if (rc != NO_ERROR)
                {
                    printf("Fatal error: Failed to write the Lx-file. rc=%d\n", rc);
                    rc = 85;
                }
                pModule->dumpVirtualLxFile();
            }
            else
            {
                printf("Fatal error: Failed to convert the file. rc=%d\n", rc);
                rc = 84;
            }
        }
        else
        {
            printf("Fatal error: Failed to read file signature. rc=%d\n", rc);
            rc = 83;
        }

        DosClose(hFileXx);
    }
    else
    {
        printf("Fatal error: Failed to open Xx-file, %s. rc=%d\n",
                pszXxFilename, rc);
        rc = 82;
    }
    return (int)rc;
}


/**
 * Display syntax for this program.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
static void syntax()
{
    printf("Syntax: xx2lx.exe [-W<0|1|2|3>] [-1<+|-|[*]>] <XxFile> [LxFile]\n"
           "\n"
           " Global options:\n"
           "  -W<0|1|2|3|4> Message filter level.\n"
           "                   -W0: Output only severe and unrecoverable error messages.\n"
           "                   -W1: Output error, severe and unrecoverable error messages.\n"
           "                   -W2: Output warning, error, severe and unrecoverable error\n"
           "                        messages.\n"
           "                   -W3: Output nearly all messages.\n"
           "                   -W4: Output absolutely all messages.\n"
           "                Default: -W3\n"
           "\n"
           " Pe2Lx options:\n"
           "  -1<+|-|[*]>   All-in-one-object fix.\n"
           "                     +: Fix applied when necessary.\n"
           "                     -: Disabled. Never applied.\n"
           "                     *: Forced. Applied every time.\n"
           "                Default: -1*\n"
           "  -rw           Make all segments writable. For use with -1+. A trick to make\n"
           "                it possible for OS/2 to load the objects following on another.\n"
           "                This of course doesn't solve the alignment difference. So if\n"
           "                you build the program pass /ALIGN:0x10000 to the linker.\n"
           "  -f            Strip fixups forcing. Don't use with DLLs, may cause traps.\n"
           "\n"
           " Custombuild options:\n"
           "  -o:<ordfile>  Ordinal file. form: <W32DLL>.<name/ord> @<CustDLLOrd>\n"
           "  -c:<custdll>  Custombuild dll. After -o:!\n"
           "  -x:<dll>      Exclude from custombuild. -x:MSVCRT for example.\n"
           "\n"
           "  XxFile        Input PE .\n"
           "  LxFile        Output Lx-file. If not specified the Xx-file is\n"
           "                renamed and the Lx-file will use the original name\n"
           "                of the Xx-file.\n"
           " Xx2Lx version 0.%02d\n",
           PE2LX_VERSION
           );
}



#if 0
/**
 * Debug - see how much of the stack that have been used.
 * Padd stack, and look in the debug storage view on program end.
 * @param
 * @status
 * @author    knut st. osmundsen
 */
static void initStack()
{
    PTIB  pTib;
    PPIB  pPib;

    DosGetInfoBlocks(&pTib, &pPib);
    memset((PVOID)pTib->tib_pstack, 'k', (size_t)&pTib - 0x30 - (size_t)pTib->tib_pstack);
}
#endif
