/* $Id: kDump.cpp,v 1.2 2001-02-02 08:45:41 bird Exp $
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
        try
        {
            kFile               file(argv[argi]);
            kFileFormatBase *   pFile = NULL;
            try {pFile = new kFilePE(&file);}
            catch (int err)
            {
                try {pFile = new kFileLX(&file);}
                catch (int err)
                {
                    try {pFile = new kFileDef(&file);}
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
        }
        catch (int err)
        {
            fprintf(stderr, "Fatal: Failed to open file %s err=%d.\n", argv[argi], err);
            return -1;
        }
    }
    return 0;
}
