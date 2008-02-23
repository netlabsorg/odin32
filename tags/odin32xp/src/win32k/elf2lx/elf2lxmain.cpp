/* $Id: elf2lxmain.cpp,v 1.1 2000-02-27 02:12:34 bird Exp $
 *
 * Elf2Lx main program. (Ring 3 only!)
 *
 * Copyright (c) 1999 knut st. osmundsen
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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "types.h"
#include <newexe.h>
#include <exe386.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "OS2Krnl.h"
#include "elf.h"
#include "modulebase.h"
#include "elf2lx.h"
#include <stdio.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
char szBackupElfFilename[CCHMAXPATH]; /* too save stack/heap */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax();


/**
 * Main function of the Elf2Lx utillity.
 * @returns    0 on success.
 *             1 Help.
 *             2 Syntax error: Invalid argument, '<arg>'.
 *             3 Syntax error: Too many filenames.
 *             4 Syntax error: No Win32-file specified.
 *            80 Fatal error: Can't find Win32-file, <filename>.
 *            81 Fatal error: Failed to rename the Win32-file, <from> -> <to>
 *            82 Fatal error: Failed to open Win32-file, <filename>. rc=<rc>
 *            83 Fatal error: Failed to convertert the file. rc=<rc from init(..)>
 *            84 Fatal error: Failed to write the Lx-file. rc=<rc from writeLXFile(..)>
 * @param     argc  Count of arguments.
 * @param     argv  Array of argument pointers. argc entries.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
int main(int argc, char **argv)
{
    APIRET rc;
    ULONG  ulAction = 0;
    PCSZ   pszElfFilename = NULL;
    PCSZ   pszOS2NIXFilename = NULL;
    PCSZ   psz;
    int    argi;

    /* read parameters */
    for (argi = 1; argi < argc; argi++)
    {
        /* check if option or filname */
        if (argv[argi][0] == '-' || argv[argi][0] == '/')
        {   /* option */
            switch (argv[argi][1])
            {
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

                default:
                    printf("Syntax error: Invalid argument, '%s'\n", argv[argi]);
                    syntax();
                    return 2;
            }
        }
        else
        {
            if (pszElfFilename == NULL)
            {
                pszElfFilename = argv[argi];
                /* check if exists */
                rc = DosQueryPathInfo(pszElfFilename,FIL_QUERYFULLNAME,
                                      &szBackupElfFilename[0], sizeof(szBackupElfFilename));
                if (rc != NO_ERROR)
                {
                    printf("Fatal error: Can't find Win32-file, '%s'.\n", pszElfFilename);
                    return 80;
                }
            }
            else if (pszOS2NIXFilename == NULL)
                pszOS2NIXFilename = argv[argi];
            else
            {
                printf("Syntax error: Too many filenames!\n");
                syntax();
                return 3;
            }
        }
    } /* for */

    /* check if enough arguments */
    if (pszElfFilename == NULL)
    {
        printf("Syntax error: No Win32-file specified.\n\n");
        syntax();
        return 4;
    }

    /* rename files? */
    if (pszOS2NIXFilename == NULL)
    {
        char *pszExt = strrchr(pszElfFilename, '.');
        if (pszExt == NULL)
        {
            printf("warning: Elf-file don't have an extention\n");
            strcpy(szBackupElfFilename, pszElfFilename);
            strcat(szBackupElfFilename, "ebk");
        }
        else
        {
            memset(szBackupElfFilename, 0, sizeof(szBackupElfFilename));
            if (stricmp(pszExt + 1, "exe") == 0)
            {
                strncpy(szBackupElfFilename, pszElfFilename, pszExt - pszElfFilename + 1);
                strcat(szBackupElfFilename, "exf");
            }
            else
            {
                strncpy(szBackupElfFilename, pszElfFilename, pszExt - pszElfFilename + 3);
                strcat(szBackupElfFilename, "k");
            }
        }
        rc = DosMove(pszElfFilename, szBackupElfFilename);
        if (rc != NO_ERROR)
        {
            printf("Fatal error: Failed to rename the Elf-file, %s -> %s\n",
                    pszElfFilename, szBackupElfFilename);
            return 81;
        }
        printInf(("Backuped %s to %s\n", pszElfFilename, szBackupElfFilename));

        /* switch name */
        pszOS2NIXFilename = pszElfFilename;
        pszElfFilename = szBackupElfFilename;
    }

    /* open input file */
    HFILE hFileWin32;

    rc = DosOpen(pszElfFilename, &hFileWin32, &ulAction, 0UL,
                 FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_FLAGS_RANDOMSEQUENTIAL | OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE,
                 NULL);
    if (rc == NO_ERROR)
    {
        Elf2Lx elf2lx(hFileWin32);
        rc = elf2lx.init(pszOS2NIXFilename);
        if (rc == NO_ERROR)
        {
            rc = elf2lx.writeFile(pszOS2NIXFilename);
            if (rc != NO_ERROR)
            {
                printf("Fatal error: Failed to write the Lx-file. rc=%d\n", rc);
                rc = 84;
            }
            elf2lx.dumpVirtualLxFile();
        }
        else
        {
            printf("Fatal error: Failed to convertert the file. rc=%d\n", rc);
            rc = 83;
        }
        DosClose(hFileWin32);
    }
    else
    {
        printf("Fatal error: Failed to open Elf-file, %s. rc=%d\n",
                pszElfFilename, rc);
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
    printf("Syntax: elf2lx.exe -W<0|1|2|3> <ElfFile> [OS2NIXFile]\n"
           "\n"
           "  -W<0|1|2|3|4> Message filter level.\n"
           "                   -W0: Output only severe and unrecoverable error messages.\n"
           "                   -W1: Output error, severe and unrecoverable error messages.\n"
           "                   -W2: Output warning, error, severe and unrecoverable error\n"
           "                        messages.\n"
           "                   -W3: Output nearly all messages.\n"
           "                   -W4: Output absolutely all messages.\n"
           "                Default: -W3\n"
           "  ElfFile       Input Elf executable or share library file.\n"
           "  OS2NIXFile    Output OS2NIX-file. If not specified the Elf-file is\n"
           "                renamed and the OS2NIX-file will use the original name\n"
           "                of the Elf-file.\n"
           " Elf2Lx version 0.%02d\n",
           0
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
