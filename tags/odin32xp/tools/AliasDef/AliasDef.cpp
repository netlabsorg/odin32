/* $Id: AliasDef.cpp,v 1.1 2003-02-23 03:02:14 bird Exp $ */
/*
 * aliasdef - Create aliases for unmangled GCC stdcall symbols.
 *
 * Copyright (c) 1999-2003 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
//#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kTypes.h"
#include "kError.h"
#include "kFile.h"
#include "kFileInterfaces.h"
#include "kFileFormatBase.h"
#include "kFileDef.h"

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);
static long processFile(const char *pszInput, const char *pszOutput, KBOOL fNoPublic);


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
    KBOOL   fNoPublic = FALSE;

    /**************************************************************************
    * parse arguments.
    * options:  -h or -?     help
    *           -n           no public.
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
                case 'n':
                case 'N':
                    fNoPublic = TRUE;
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
        lRc = processFile(pszInput, pszOutput, fNoPublic);
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
        "AliasDef - Creates alias based on .def like file\n"
        "------------------------------------------------\n"
        "syntax: AliasDef.exe [-h|-?] [-S] <infile> <outfile>\n"
        "    -h or -?      Syntax help. (this)\n"
        "    -n            No public definition of the alias.\n"
        "    infile        Name of input file (.def file).\n"
        "    outfile       Name of output file (OMF object).\n"
        "\n"
        "Notes:\n"
        "    The input file is an .def file. EXPORTS is used on this form:\n"
        "          <alias> = <internalname>\n"
        );
}


/**
 *
 * @returns   0 on success, error code on error.
 * @param     pszInput  Input filename.
 * @param     pszOuput  Output filename.
 * @sketch    Open input file
 *            try create a kFileDef object from inputfile.
 *            Open output file.
 *            Generate output file.
 * @remark
 */
static long processFile(const char *pszInput, const char *pszOutput, KBOOL fNoPublic)
{
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
                kExportEntry export;
                #pragma pack(1)
                 struct OMFRec
                {
                    unsigned char   chType;
                    unsigned short  cb; /* excludes the header! */
                    char            ach[1024];
                } rec;
                #pragma pack()

                /* THEADR */
                rec.chType = 0x80;
                strcpy(&rec.ach[1], pszInput);
                rec.ach[0] = (char)strlen(pszInput);
                rec.cb = (short)(rec.ach[0] + 2);
                pOutput->write(&rec, rec.cb + 3);


                /* Exports */
                if (pDefFile->exportFindFirst(&export))
                {
                    //int     iExt = 0;
                    do
                    {
                        /* validate export struct */
                        if (export.achName[0] == '\0')
                        {
                            kFile::StdErr.printf(
                                "Warning export name is missing.\n"
                                "info:\texport.achIntName=%s\n\texport.achName=%s\n\texport.ulOrdinal=%ld\n",
                                export.achIntName, export.achName, export.ulOrdinal);
                            continue;
                        }

                        int cch    = strlen(export.achName);
                        int cchInt = strlen(export.achIntName);
                        #if 1 //aliased approach
                        #if 0
                        /* emit extdef record */
                        rec.chType = 0x8c;
                        strcpy(&rec.ach[1], export.achIntName);
                        rec.ach[0] = cchInt;
                        rec.ach[1 + cchInt + 1] = 0;
                        rec.cb = (short)(cchInt + 3);
                        pOutput->write(&rec, rec.cb + 3);
                        #endif

                        /* emit alias record */
                        rec.chType = 0xc6;
                        strcpy(&rec.ach[1], export.achName);
                        rec.ach[0] = (char)cch;
                        strcpy(&rec.ach[1 + cch + 1], export.achIntName);
                        rec.ach[1 + cch] = (char)cchInt;
                        rec.cb = (short)(cch + cchInt  + 3);
                        pOutput->write(&rec, rec.cb + 3);

                        if (!fNoPublic)
                        {
                            /* emit pubdef record */
                            rec.chType = 0x90;
                            rec.ach[0] = 0;
                            rec.ach[1] = 0;
                            rec.ach[2] = 0;
                            rec.ach[3] = 0;
                            rec.ach[4] = (char)cch;
                            strcpy(&rec.ach[5], export.achName);
                            rec.ach[5 + cch + 0] = 0;
                            rec.ach[5 + cch + 1] = 0;
                            rec.ach[5 + cch + 2] = 0;
                            rec.ach[5 + cch + 3] = 0;
                            rec.cb = cch + 4 + 5;
                            pOutput->write(&rec, rec.cb + 3);
                        }


                        #else //weak approach.

                        /* emit extdef record */
                        rec.chType = 0x8c;
                        strcpy(&rec.ach[1], export.achName);
                        rec.ach[0] = cch;
                        strcpy(&rec.ach[1 + cch + 2], export.achIntName);
                        rec.ach[1 + cch + 1] = cchInt;
                        rec.ach[1 + cch + 2 + cchInt + 1] = 0;
                        rec.cb = 1 + cch + 2 + cchInt + 2;
                        pOutput->write(&rec, rec.cb + 3);

                        /* emit weak comment record */
                        rec.chType = 0x88;
                        rec.ach[0] = 0;
                        rec.ach[1] = 0xa8;
                        rec.ach[2] = ++iExt;
                        rec.ach[3] = ++iExt;
                        rec.ach[4] = 0;
                        rec.cb = 5;
                        pOutput->write(&rec, rec.cb + 3);


                        #endif
                    } while (pDefFile->exportFindNext(&export));

                pOutput->setSize();
                }

                /* MODEND32 */
                rec.chType = 0x8b;
                rec.cb     = 2;
                rec.ach[0] = 1;
                rec.ach[1] = 0;
                pOutput->write(&rec, rec.cb + 3);

                delete pOutput;
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

