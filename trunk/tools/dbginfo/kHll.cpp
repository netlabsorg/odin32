/* $Id: kHll.cpp,v 1.2 2000-03-25 23:50:11 bird Exp $
 *
 * kHll - Implementation of the class kHll.
 *        That class is used to create HLL debuginfo.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_TYPES
#define INCL_DOSERRORS
#define FOR_EXEHDR          1           /* exe386.h flag */
#define DWORD               ULONG       /* Used by exe386.h / newexe.h */
#define WORD                USHORT      /* Used by exe386.h / newexe.h */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include <newexe.h>
#include <exe386.h>

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "hll.h"
#include "kList.h"
#include "kHll.h"



kHLLPubSym::kHLLPubSym(
        const char *        pszName,
        unsigned long       off,
        unsigned short      iObj,
        unsigned short      iType
        );


kHLLPubSym::~kHLLPubSym();


int kHLLPubSym::write(FILE *phFile)
{

}












/**
 * Constructor - Creates an empty HLL object.
 */
kHll::kHll()
{
}


/**
 * Destructor.
 */
kHll::~kHll()
{
}


/**
 * Adds an LX object to the HLL info.
 * @returns   Object handle. NULL on error.
 * @param     pszName   Object name.
 * @param     cb        Size of object.
 */
const void *    kHll::addObject(
                    const char *        pszName,
                    unsigned long int   cb
                    )
{
    pszName = pszName;
    cb = cb;
    return NULL;
}


/**
 * Adds a module.
 * @returns  Object handle. NULL on error.
 * @param    pszName    Module name
 * @param    pvLib      Library module handle.
 * @param    cObjects   Number of objects in the array.
 * @param    paObjects  Pointer to an array of objects.
 */
const void *    kHll::addModule(
                    const char *        pszName,
                    const void *        pvLib,
                    unsigned            cObject,
                    PMODOBJECT          paObjects)
{
    return NULL;
}



/**
 * Adds a public symbol.
 * @returns   Handle to the symbol. NULL on error.
 * @param     pszName  Symbol name.
 * @param     off      Offset into the LX Object of the symbol.
 * @param     iObject  LX Object index.
 * @param     pvType   Type handle. NULL if not type.
 */
const void *    kHll::addPublicSymbol(
                    const char *        pszName,
                    unsigned long int   off,
                    unsigned short int  iObject,
                    const void *        pvType
                    )
{
    PHLLPUBLICSYM    pPubSym;

    assert(pszName != NULL);
    pPubSym = (PHLLPUBLICSYM)malloc(sizeof(HLLPUBLICSYM) + strlen(pszName));
    if (pPubSym != NULL)
    {
        strcpy(pPubSym->hll.achName, pszName);
        pPubSym->hll.cchName = strlen(pszName);
        pPubSym->hll.iObj = iObject;
        pPubSym->hll.off = off;
        pPubSym->hll.iType = pvType == NULL ? 0 : -1; //FIXME/TODO: Types->getIndex(pvType); check if 0 or -1.
        PublicSymbols.insert(pPubSym);
        free(pPubSym);
    }

    return NULL;
}



BOOL            kHll::write(
                     const char *pszFilename
                     )
{
    return FALSE;
}


