/* $Id: kDef2Wat.cpp,v 1.2 2001-04-17 00:26:10 bird Exp $
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
#include <os2.h>

#include "kFile.h"
#include "kFileFormatBase.h"
#include "kInterfaces.h"
#include "kFileDef.h"

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

    /* check arguments */
    if (argc != 3 && argc != 4)
    {
        syntax();
        return -1;
    }
    pszInput  = argv[1];
    pszOutput = argv[2];
    pszAppend = (argc == 4) ? argv[3] : NULL;

    /*
     *
     */
    try
    {
        kFile    InFile(pszInput);
        kFileDef def(&InFile);
        try
        {
            kFile   OutFile(pszOutput, FALSE);
            OutFile.end();

            def.makeWatcomLinkFileAddtion(&OutFile);
            if (pszAppend)
            {
                try
                {
                    kFile   AppendFile(pszAppend);

                    OutFile += AppendFile;
                    kFile::StdOut.printf("Successfully converted %s to %s\n", pszInput, pszOutput);
                }
                catch (int errorcode)
                {
                    kFile::StdErr.printf("Append file %s to %s, errorcode=%d\n", pszAppend, pszInput, errorcode);
                    return -3;
                }
            }
        }
        catch (int errorcode)
        {
            kFile::StdErr.printf("Failed to read .DEF file (%s), errorcode=%d\n", pszInput, errorcode);
            return -3;
        }
    }
    catch (int errorcode)
    {
        kFile::StdErr.printf("Failed to read .DEF file (%s), errorcode=%d\n", pszInput, errorcode);
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
        "kDef2Wat v0.0.0 (untested)\n"
        "\n"
        "syntax: kDef2Wat.exe <def-file> <wlink-file> [extra-file]\n"
        "\n"
        "Where:\n"
        "    <def-file>     The .DEF file to convert.\n"
        "    <wlink-file>   The WLINK directive and option file.\n"
        "    [extra-file]   Extra file with directives which should be appended to\n"
        "                   the wlink-file\n"
        "\n"
        "Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)\n"
        );
}
