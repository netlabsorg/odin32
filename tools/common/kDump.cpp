/* $Id: kDump.cpp,v 1.1 2000-10-02 04:07:44 bird Exp $
 *
 * Generic dumper...
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_BASE

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <stdio.h>

#include "kFile.h"
#include "kFileFormatBase.h"
#include "kFileDef.h"
#include "kFileLX.h"
#include "kFilePE.h"




int main(int argc, char **argv)
{
    int argi;


    for (argi = 1; argi < argc; argi++)
    {
        FILE *phFile = fopen(argv[argi], "rb");
        if (phFile)
        {
            kFileFormatBase *pFile = NULL;
            try {pFile = new kFilePE(phFile);}
            catch (int err)
            {
                //try {pFile = new kFileLX(phFile);}
                //catch (int err)
                {
                    try {pFile = new kFileDef(phFile);}
                    catch (int err)
                    {
                        kFile::StdErr.printf("Failed to recognize file %s.\n", argv[argi]);
                        return -2;
                    }
                }
            }

            /*
             * If successfully opened, then dump it.
             */
            if (pFile != NULL)
            {
                pFile->dump(&kFile::StdOut);
                delete pFile;
            }
            fclose(phFile);
        }
        else
        {
            fprintf(stderr, "Fatal: Failed to open file %s.\n", argv[argi]);
            return -1;
        }
    }
    return 0;
}
