/* $Id: kFileSDF.cpp,v 1.2 2002-02-24 02:47:27 bird Exp $
 *
 * kFileSDF- Structure Defintion File class Implementation.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <string.h>

#include "kTypes.h"
#include "kError.h"
#include "kFileInterfaces.h"
#include "kFile.h"
#include "kFileFormatBase.h"
#include "kFileSDF.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/



/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#if 0
static kFileSDF tst((kFile*)NULL);
#endif



/**
 * Parses the in memory SDF file.
 * @param   pvFile  Pointer to Filemapping.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 * The layout of a SDF file is something like this:
 *
 *  -----------------------
 *  |                     |
 *  | SDFHEADER           |
 *  |                     |
 *  -----------------------
 *  |                     |
 *  | Dynamic Array of    |
 *  | SDFSTRUCTs with     |
 *  | variable # members. |
 *  |                     |
 *  -----------------------
 *  |                     |
 *  | Array of SDFTYPE.   |
 *  |                     |
 *  -----------------------
 *
 */
void kFileSDF::parseSDFFile(void *pvFile) throw (kError)
{
    /*
     * Sanity check on header.
     */
    pHdr = (PSDFHEADER)pvFile;
    if (    pHdr->cStructs >= 0x8000    /* 32k is a reasonable limit. */
        ||  pHdr->cTypes   >= 0x8000)
        throw(kError(kError::SDF_TOO_MANY));

    /*
     * Initialize the papStructs array.
     */
    papStructs = NULL;
    PSDFSTRUCT  pStruct = (PSDFSTRUCT)(unsigned)(pHdr + 1);
    if (pHdr->cStructs > 0)
    {
        papStructs = new PSDFSTRUCT[pHdr->cStructs];
        for (int i = pHdr->cStructs; i > 0; i--)
        {
            papStructs[i] = pStruct;
            pStruct = (PSDFSTRUCT)&pStruct->aMembers[pStruct->cMembers];
        }
    }

    /*
     * typedefs starts where the structures ends. (it seems)
     */
    paTypes = (PSDFTYPE)pStruct;
}



/**
 * Find a typedef.
 * @returns Pointer to the typedef.
 *          NULL if not found.
 * @param   pszType     Name of the type to find.
 * @sketch  Linear search.
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  We might concider sorting the type array one day for binary search...
 */
PSDFTYPE kFileSDF::getType(const char *pszType)
{
    if (*pszType != '\0')
    {
        int i;
        for (i = 0; i < pHdr->cTypes; i++)
            if (strcmp(paTypes[i].szName, pszType) == 0)
                return &paTypes[i];     /* Found */
    }

    return NULL;                        /* Not Found */
}


/**
 * Find a struct.
 * @returns Pointer to struct.
 *          NULL if not found.
 * @param   pszStruct   Name of the struct to find.
 * @sketch  Linear search.
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  We might concider sorting the struct array one day for binary search...
 */
PSDFSTRUCT kFileSDF::getStruct(const char *pszStruct)
{
    if (*pszStruct != '\0')
    {
        int i;
        for (i = 0; i < pHdr->cStructs; i++)
            if (strcmp(papStructs[i]->szName, pszStruct) == 0)
                return papStructs[i];   /* Found */
    }

    return NULL;                        /* Not Found */
}



/**
 * Constructor.
 * @param   pFile   Pointer to file object to parse.
 *                  The status of this is very likely to be changed.
 * @sketch  Read the file in to memory block.
 *          Convert '\r\n' to '\n'.
 *          Call parser function which does the rest of the work.
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Will throw error codes.
 */
kFileSDF::kFileSDF(kFile *pFile) throw (kError) :
    kFileFormatBase(pFile), papStructs(NULL), pHdr(NULL), paTypes(NULL)
{
    long    cchFile = pFile->getSize();
    void *  pvFile = pFile->mapFile();

    /*
     * apply "rb"->"r" fix
     */
    char * pchW = (char*)pvFile;
    char * pchR = pchW;
    char * pchEnd = pchW + cchFile;
    while (cchFile > 0)
    {
        char ch = *pchR;
        if (ch == '\r' && cchFile >= 2 && pchR[1] == '\n')
        {
            cchFile--;
            pchR++;
            ch = '\n';
        }
        *pchW++ = ch;
        pchR++;
        cchFile--;
    }
    while (pchR < pchW)
        *pchR = 0;

    /*
     * Parse the file.
     */
    try
    {
        parseSDFFile(pvFile);
    }
    catch (int err)
    {
        kFile::mapFree(pvFile);
        if (papStructs);
            delete papStructs;
        throw(err);
    }
}


kFileSDF::~kFileSDF() throw (kError)
{
    if (papStructs);
        delete papStructs;
    kFile::mapFree(pHdr);
}



kDbgTypeEntry * kFileSDF::dbgtypeFindFirst(int flFlags)
{
    return NULL;
}

kDbgTypeEntry * kFileSDF::dbgtypeFindNext(kDbgTypeEntry *kDbgTypeEntry)
{
    return NULL;
}

void            kFileSDF::dbgtypeFindClose(kDbgTypeEntry *kDbgTypeEntry)
{
    return;
}

kDbgTypeEntry * kFileSDF::dbgtypeLookup(const char *pszName, int flFlags)
{
    return NULL;
}




KBOOL kFileSDF::dump(kFile *pOut)
{
    return FALSE;
}

