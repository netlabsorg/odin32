/* $Id: kFileFormatBase.cpp,v 1.3 2001-02-02 08:45:41 bird Exp $
 *
 * kFileFormatBase - Base class for kFile<format> classes.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "kFile.h"
#include "kFileFormatBase.h"




/**
 * Creates a memory buffer for a binary file.
 * @returns   Pointer to file memoryblock. NULL on error.
 * @param     pszFilename  Pointer to filename string.
 * @remark    This function is the one using most of the execution
 *            time (DosRead + DosOpen) - about 70% of the execution time!
 */
void *kFileFormatBase::readfile(const char *pszFilename)
{
    void *pvFile = NULL;
    FILE *phFile;

    phFile = fopen(pszFilename, "rb");
    if (phFile != NULL)
    {
        long int cbFile;

        if (ftell(phFile) < 0
            ||
            fseek(phFile, 0, SEEK_END) != 0
            ||
            (cbFile = ftell(phFile)) < 0
            ||
            fseek(phFile, 0, SEEK_SET) != 0
            )
            cbFile = -1;

        if (cbFile > 0)
        {
            pvFile = malloc((size_t)cbFile + 1);
            if (pvFile != NULL)
            {
                memset(pvFile, 0, (size_t)cbFile + 1);
                if (fread(pvFile, 1, (size_t)cbFile, phFile) == 0)
                {   /* failed! */
                    free(pvFile);
                    pvFile = NULL;
                }
            }
            else
                fprintf(stderr, "warning/error: failed to open file %s\n", pszFilename);
        }
        fclose(phFile);
    }
    return pvFile;
}

/**
 * Dump function.
 * @returns Successindicator.
 * @param   pOut    Output file.
 */
BOOL   kFileFormatBase::dump(kFile *pOut)
{
    pOut->printf("Sorry, dump() is not implemented for this file format.\n");
    return FALSE;
}

