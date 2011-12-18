/* $Id: ImpDef.cpp,v 1.8 2002-02-24 02:44:40 bird Exp $ */
/*
 * ImpDef - Create export file which use internal names and ordinals.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
//#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <set>

#include "kTypes.h"
#include "kError.h"
#include "kFile.h"
#include "kFileInterfaces.h"
#include "kFileFormatBase.h"
#include "kFileDef.h"

#include "ImpDef.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void   syntax(void);
static long   processFile(const char *pszInput, const char *pszOutput, const POPTIONS pOptions);
static char  *generateExportName(const kExportEntry * pExport, char *pszBuffer, const POPTIONS pOptions);


/**
 * Main function.
 * @returns   0 on success.
 * @param     argc  Number of arguments
 * @param     argv  Array of arguments
 */
int main(int argc, char **argv)
{
    int     argi;
    KBOOL   fFatal = FALSE;
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
                            kFile::StdErr.printf("warning: internal functions starts at ordinal 0!\n");
                    }
                    else
                    {
                        kFile::StdErr.printf("incorrect parameter -I:<ord>. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
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
                    kFile::StdErr.printf("incorrect parameter. (argi=%d, argv[argi]=%s)\n", argi, argv[argi]);
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
                kFile::StdErr.printf("To many files are specified!\n");
                fFatal = TRUE;
            }
        }
        argi++;
    }

    if (pszInput == NULL)
    {
        fFatal = TRUE;
        kFile::StdErr.printf("Missing input file.\n");
    }
    else if (pszOutput == NULL)
    {
        fFatal = TRUE;
        kFile::StdErr.printf("Missing output file.\n");
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
    kFile::StdOut.printf(
        "\n"
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
    std::set<std::string> exports;

    long lRc = 0;

    try
    {
        kFile * pInput = new kFile(pszInput);
        try
        {
            kFileDef * pDefFile = new kFileDef(pInput);
            try
            {
                kFile * pOutput = new kFile(pszOutput, FALSE);
                kExportEntry exp;

                /* generate LIBRARY line */
                pOutput->printf(
                    ";Internal export definition file - autogenerated by ImpDef.\n"
                    "%s\n",
                    pDefFile->queryType());

                /* Description line */
                if (pDefFile->queryDescription())
                    pOutput->printf("DESCRIPTION '%s'\n", pDefFile->queryDescription());

                /* Exports */
                if (pDefFile->exportFindFirst(&exp))
                {
                    pOutput->printf("EXPORTS\n");
                    do
                    {
                        char        szName[MAXEXPORTNAME + 2 /*quotes*/];
                        const char *pszName;

                        /* validate export struct */
                        if (exp.achName[0] == '\0')
                        {
                            kFile::StdErr.printf(
                                "Warning export name is missing.\n"
                                "info:\texport.achIntName=%s\n\texport.achName=%s\n\texport.ulOrdinal=%ld\n",
                                exp.achIntName, exp.achName, exp.ulOrdinal);
                            continue;
                        }
                        if (exp.ulOrdinal == ~0UL)
                        {
                            kFile::StdErr.printf(
                                "warning: export is missing ordinal value. Export is ignored\n"
                                "info:\texport.achIntName=%s\n\texport.achName=%s\n\texport.ulOrdinal=%ld\n",
                                    exp.achIntName, exp.achName, exp.ulOrdinal);
                            continue;
                        }

                        /* real work */
                        pszName = generateExportName(&exp, &szName[1], pOptions);
                        if (exports.count(pszName) == 0) {
                            exports.insert(pszName);
                            szName[0] = '"';
                            strcat(szName, "\"");
                            pszName = szName;

                            pOutput->printf("    %-*s  @%ld\n", 40, pszName, exp.ulOrdinal);
                        }
                    } while (pDefFile->exportFindNext(&exp));
                pOutput->setSize();
                delete pOutput;
                }
            }
            catch (kError err)
            {
                kFile::StdErr.printf("error creating output file, '%s', errorcode 0x%x\n", pszOutput, err.getErrno());
                lRc = -4;
            }
            delete pDefFile;
        }
        catch (kError err)
        {
            kFile::StdErr.printf("%s is not a valid def file, errorcode 0x%x\n", pszInput, err.getErrno());
            lRc = -3;
        }
        //delete pInput; - not needed done by ~kFileFormatBase!
    }
    catch (kError err)
    {
        kFile::StdErr.printf( "error openining inputfile, '%s', errorcode 0x%x\n", pszInput, err.getErrno());
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
static char *generateExportName(const kExportEntry * pExport, char *pszBuffer, const POPTIONS pOptions)
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

