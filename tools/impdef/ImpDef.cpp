/* $Id: ImpDef.cpp,v 1.3 1999-09-08 07:30:09 bird Exp $ */
/*
 * ImpDef - Create export file which use internal names and ordinals.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ImpDef.h"
#include "kFileFormatBase.h"
#include "kFileDef.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void   syntax(void);
static long   processFile(const char *pszInput, const char *pszOutput, const POPTIONS pOptions);
static char  *generateExportName(const PEXPORTENTRY pExport, char *pszBuffer, const POPTIONS pOptions);


/**
 * Main function.
 * @returns   0 on success.
 * @param     argc  Number of arguments
 * @param     argv  Array of arguments
 */
int main(int argc, char **argv)
{
    int     argi;
    BOOL    fFatal = FALSE;
    long    lRc = 0;
    char   *pszInput = NULL;
    char   *pszOutput = NULL;
    OPTIONS options = {TRUE, ORD_START_INTERNAL_FUNCTIONS, FALSE, TRUE};

    /**************************************************************************
    * parse arguments.
    * options:  -h or -?     help
    *           -S<[+]|->    Similar to exported. (stdcall)
    *           -O<[+]|->    Remove OS2 prefix on APIs.
    *           -I:<num>     Start ordinal number of internal functions.
    *           -F<[+]|->    Export intname for internal stdcall functions.
    **************************************************************************/
    if (argc == 1)
        syntax();
    argi = 1;
    while (argi < argc && !fFatal)
    {
        if(argv[argi][0] == '-' || argv[argi][0] == '/')
        {
            switch (argv[argi][1])
            {
                case 's':
                case 'S':
                    options.fSimilarToExported = argv[argi][2] != '-';
                    break;

                case 'o':
                case 'O':
                    options.fRemoveOS2APIPrefix = argv[argi][2] != '-';
                    break;

                case 'i':
                case 'I':
                    if (strlen(argv[argi]) >= 3)
                    {
                        options.ulOrdStartInternalFunctions = atol(&argv[argi][3]);
                        if (options.ulOrdStartInternalFunctions == 0)
                            fprintf(stderr, "warning: internal functions starts at ordinal 0!\n");
                    }
                    else
                    {
                        fprintf(stderr, "incorrect parameter -I:<ord>. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
                        fFatal = TRUE;
                    }
                    break;

                case 'f':
                case 'F':
                    options.fInternalFnExportStdCallsIntName = argv[argi][2] != '-';
                    break;

                case '?':
                case 'h':
                case 'H':
                    syntax();
                    return 0;

                default:
                    fprintf(stderr, "incorrect parameter. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
                    fFatal = TRUE;
                    break;
            }
        }
        else
        {
            if (pszInput == NULL)
                pszInput = argv[argi];
            else if (pszOutput == NULL)
                pszOutput = argv[argi];
            else
            {
                fprintf(stderr, "To many files are specified!\n");
                fFatal = TRUE;
            }
        }
        argi++;
    }

    if (pszInput == NULL)
    {
        fFatal = TRUE;
        fprintf(stderr, "Missing input file.\n");
    }
    else if (pszOutput == NULL)
    {
        fFatal = TRUE;
        fprintf(stderr, "Missing output file.\n");
    }

    if (!fFatal)
        lRc = processFile(pszInput, pszOutput, &options);
    else
        lRc = -1;

    return (int)lRc;
}


/**
 * Print syntax/help message.
 */
static void syntax(void)
{
    printf("\n"
           "ImpDef - Creates internal import definition file\n"
           "------------------------------------------------\n"
           "syntax: ImpDef.exe [-h|-?] [-S] <infile> <outfile>\n"
           "    -h or -?      Syntax help. (this)\n"
           "    -F<[+]|->     Fix! Export int.name for int.functions. default: F+\n"
           "    -I:<ord>      Start of internal function.  default: I:%d\n"
           "    -O<[+]|->     Remove OS2 prefix on APIs.   default: O-\n"
           "    -S<[+]|->     Similar to exported name.    default: S+\n"
           "    infile        Name of input file\n"
           "    outfile       Name of output file\n"
           "\n"
           "Notes:\n"
           "   -S+ only works on stdcall functions (having '@' in the internal name).\n"
           "   -S+ takes the '_' and the '@..' parts from the internal name and adds it\n"
           "   to the exported name. This way the OS2 prefix is removed.\n"
           "   -O+ has no effect on stdcall functions when -S+ is set. -S+ has higher\n"
           "   precedence than -O+.\n"
           "   -O+ only removes the OS2 prefix from internal names.\n",
           ORD_START_INTERNAL_FUNCTIONS
           );
}


/**
 *
 * @returns   0 on success, error code on error.
 * @param     pszInput  Input filename.
 * @param     pszOuput  Output filename.
 * @param     pOptions  Pointer to options struct.
 * @sketch    Open input file
 *            try create a kFileDef object from inputfile.
 *            Open output file.
 *            Generate output file.
 * @remark
 */
static long processFile(const char *pszInput, const char *pszOutput, const POPTIONS pOptions)
{
    FILE *phInput;
    FILE *phOutput;
    long lRc = 0;

    phInput = fopen(pszInput, "rb");
    if (phInput != NULL)
    {
        try
        {
            kFileDef DefFile(phInput);
            phOutput = fopen(pszOutput, "w");
            if (phOutput != NULL)
            {
                EXPORTENTRY export;

                /* generate LIBRARY line */
                fputs(";Internal export definition file - autogenerated by ImpDef.", phOutput);
                fputc('\n', phOutput);
                fputs(DefFile.queryType(), phOutput);
                fputc('\n', phOutput);
                if (DefFile.queryDescription())
                {
                    fputs(DefFile.queryDescription(), phOutput);
                    fputc('\n', phOutput);
                }
                if (DefFile.findFirstExport(&export))
                {
                    fputs("EXPORTS\n", phOutput);
                    do
                    {
                        char        szName[MAXEXPORTNAME];
                        const char *pszName;

                        /* validate export struct */
                        if (export.achName[0] == '\0')
                        {
                            fprintf(stderr, "Warning export name is missing.\n");
                            fprintf(stderr, "info:\texport.achIntName=%s\n\texport.achName=%s\n\texport.ulOrdinal=%ld\n", export.achIntName, export.achName, export.ulOrdinal);
                            continue;
                        }
                        if (export.ulOrdinal == ~0UL)
                        {
                            fprintf(stderr, "warning: export is missing ordinal value. Export is ignored\n");
                            fprintf(stderr, "info:\texport.achIntName=%s\n\texport.achName=%s\n\texport.ulOrdinal=%ld\n", export.achIntName, export.achName, export.ulOrdinal);
                            continue;
                        }

                        /* real work */
                        pszName = generateExportName(&export, &szName[0], pOptions);

                        fprintf(phOutput, "    %-*s  @%ld\n", 40, pszName, export.ulOrdinal);
                    } while (DefFile.findNextExport(&export));
                }
                fclose(phOutput);
            }
            else
            {
                fprintf(stderr, "error creating output file, '%s'\n", pszOutput);
                lRc = -4;
            }
        }
        catch (int errorcode)
        {
            fprintf(stderr, "%s is not a valid def file, errorcode 0x%x\n", pszInput, errorcode);
            lRc = -3;
        }
        fclose(phInput);
    }
    else
    {
        fprintf(stderr, "error openining inputfile, '%s'\n", pszInput);
        lRc = -2;
    }

    return lRc;
}


/**
 * Generate export names according to options defines.
 * fSimilarToExported only applies to stdcall API functions.
 * fRemoveOS2APIPrefix only applies to APIs.
 * fRemoveOS2APIPrefix have no effect on stdcall functions when fSimilarToExported is set.
 * fRemoveOS2APIPrefix only applies to the internal names.
 * @returns   Pointer to buffer.
 * @param     pExport    Export entry.
 * @param     pszBuffer  Pointer to a string buffer which the result is returned in.
 * @param     pOptions   Pointer to options-struct.
 * @precond   The export data (pExport) is valiaded.
 * @sketch    write only code... but it works (I hope).
 * @remark
 */
static char *generateExportName(const PEXPORTENTRY pExport, char *pszBuffer, const POPTIONS pOptions)
{
    if (pExport->ulOrdinal < pOptions->ulOrdStartInternalFunctions)
    {
        /* API */
        if (pOptions->fSimilarToExported)
        {
            if (pExport->achIntName[0] != '\0')
            {
                char *pszAt = strchr(&pExport->achIntName[0], '@');
                if (pszAt != NULL && pExport->achIntName[0] == '_' && pExport->achName[0] != '"')
                {   /* stdcall - merge */
                    strcpy(pszBuffer, "_");
                    /* test for "reserved" definition file names (like HeapSize) in original def-file. */
                    if (pExport->achName[0] != '"')
                        strcat(pszBuffer, &pExport->achName[0]);
                    else
                    {
                        strcat(pszBuffer, &pExport->achName[1]);
                        pszBuffer[strlen(pszBuffer)-1] = '\0'; //remove tail '"'
                    }

                    strcat(pszBuffer, pszAt);
                }
                else
                {   /* not a stdcall - no merge but check for OS2prefix */
                    if (pOptions->fRemoveOS2APIPrefix)
                    {
                        int i = 0;
                        char *psz = pszBuffer;
                        if (pExport->achIntName[i] == '_')
                            *psz++ = pExport->achIntName[i++];
                        if (strncmp(&pExport->achIntName[i], "OS2", 3) == 0)
                            i += 3;
                        strcpy(psz, &pExport->achIntName[i]);
                    }
                    else
                        strcpy(pszBuffer, &pExport->achIntName[0]);
                }
            }
            else
                strcpy(pszBuffer, &pExport->achName[0]);
        }
        else if (pOptions->fRemoveOS2APIPrefix)
        {   /* removes OS2 prefix */
            if (pExport->achIntName[0] != '\0')
            {
                int i = 0;
                char *psz = pszBuffer;
                if (pExport->achIntName[i] == '_')
                    *psz++ = pExport->achIntName[i++];
                if (strncmp(&pExport->achIntName[i], "OS2", 3) == 0)
                    i += 3;
                strcpy(psz, &pExport->achIntName[i]);
            }
            else
                strcpy(pszBuffer, &pExport->achName[0]);
        }
        else
            if (pExport->achIntName[0] != '\0')
                strcpy(pszBuffer, &pExport->achIntName[0]);
            else
                strcpy(pszBuffer, &pExport->achName[0]);
    }
    else
    {   /* non-API functions */
        if ((pExport->achName[0] == '\0' || (pOptions->fInternalFnExportStdCallsIntName && strchr(&pExport->achIntName[0], '@')))
            && pExport->achIntName[0] != '\0'
            )
            strcpy(pszBuffer, &pExport->achIntName[0]);
        else
            strcpy(pszBuffer, &pExport->achName[0]);
    }

    return pszBuffer;
}

