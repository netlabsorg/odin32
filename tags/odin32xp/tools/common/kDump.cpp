/* $Id: kDump.cpp,v 1.4 2002-02-24 02:47:24 bird Exp $
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
#include <stdio.h>

#include "kTypes.h"
#include "kError.h"
#include "kFile.h"
#include "kFileFormatBase.h"
#include "kFileInterfaces.h"
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
            kFileFormatBase *   pFile = NULL;
            try {pFile = new kFilePE(new kFile(argv[argi]));}
            catch (kError err)
            {
                try {pFile = new kFileLX(new kFile(argv[argi]));}
                catch (kError err)
                {
                    try {pFile = new kFileDef(new kFile(argv[argi]));}
                    catch (kError err)
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
        catch (kError err)
        {
            fprintf(stderr, "Fatal: Failed to open file %s err=%d.\n", argv[argi], err.getErrno());
            return -1;
        }
    }
    return 0;
}
