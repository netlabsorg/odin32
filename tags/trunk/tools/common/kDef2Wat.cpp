/* $Id: kDef2Wat.cpp,v 1.5 2002-08-25 22:35:46 bird Exp $
 *
 * Converter for IBM/MS linker definition files (.DEF) to Watcom linker directives and options.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "kTypes.h"
#include "kError.h"
#include "kFile.h"
#include "kFileInterfaces.h"
#include "kFileFormatBase.h"
#include "kFileDef.h"

#include <stdio.h>
#include <string.h>

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void syntax(void);



/**
 * Main function.
 * Parses arguments and tries to convert files.
 * @returns 0 on success.
 *          -1      Bad syntax.
 *          -2      Failed to read .DEF file.
 *          -3      Failed to generate output file.
 * @param   See syntax().
 */
int main(int argc, char **argv)
{
    const char *pszInput;
    const char *pszOutput;
    const char *pszAppend;
    const char *pszOS;
    int         enmOS;

    /* check arguments */
    if (argc != 4 && argc != 5)
    {
        syntax();
        return -1;
    }
    pszOS     = argv[1];
    pszInput  = argv[2];
    pszOutput = argv[3];
    pszAppend = (argc == 5) ? argv[4] : NULL;
    if (!stricmp(pszOS, "os2"))
        enmOS = kFileDef::os2;
    else if (!stricmp(pszOS, "os2-16"))
        enmOS = kFileDef::os2v1;
    else if (!stricmp(pszOS, "dos"))
        enmOS = kFileDef::dos;
    else if (!stricmp(pszOS, "win32"))
        enmOS = kFileDef::win32;
    else if (!stricmp(pszOS, "win16"))
        enmOS = kFileDef::win16;
    else if (!stricmp(pszOS, "nlm"))
        enmOS = kFileDef::nlm;
    else if (!stricmp(pszOS, "qnx"))
        enmOS = kFileDef::qnx;
    else if (!stricmp(pszOS, "elf"))
        enmOS = kFileDef::elf;
    else
    {
        printf("Invalid os string '%s'.\n", pszOS);
        syntax();
        return -1;
    }

    /*
     *
     */
    try
    {
        kFileDef def(new kFile(pszInput));
        try
        {
            kFile   OutFile(pszOutput, FALSE);
            OutFile.end();

            def.makeWatcomLinkFileAddtion(&OutFile, enmOS);
            if (pszAppend)
            {
                try
                {
                    kFile   AppendFile(pszAppend);

                    OutFile += AppendFile;
                    kFile::StdOut.printf("Successfully converted %s to %s\n", pszInput, pszOutput);
                }
                catch (kError err)
                {
                    kFile::StdErr.printf("Append file %s to %s, errorcode=%d\n", pszAppend, pszInput, err.getErrno());
                    return -3;
                }
            }
        }
        catch (kError err)
        {
            kFile::StdErr.printf("Failed to read .DEF file (%s), errorcode=%d\n", pszInput, err.getErrno());
            return -3;
        }
    }
    catch (kError err)
    {
        kFile::StdErr.printf("Failed to read .DEF file (%s), errorcode=%d\n", pszInput, err.getErrno());
        return -2;
    }

    return 0;
}


/**
 * Writes syntax to stdout.
 */
static void syntax(void)
{
    kFile::StdOut.printf(
        "kDef2Wat v0.0.1\n"
        "\n"
        "syntax: kDef2Wat.exe <os> <def-file> <wlink-file> [extra-file]\n"
        "\n"
        "Where:\n"
        "    <os>           Target os. os2, os2-16, dos, win32, win16, nlm, qnx or elf.\n"
        "    <def-file>     The .DEF file to convert.\n"
        "    <wlink-file>   The WLINK directive and option file.\n"
        "    [extra-file]   Extra file with directives which should be appended to\n"
        "                   the wlink-file\n"
        "\n"
        "Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)\n"
        );
}
