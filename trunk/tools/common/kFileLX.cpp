/* $Id: kFileLX.cpp,v 1.1 2000-03-27 10:18:41 bird Exp $
 *
 *
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
/* emx fixups */
#ifdef __EMX__
    #define __stdcall
     #define max(a,b) (((a) > (b)) ? (a) : (b))
     #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#define INCL_DOSERRORS
#define FOR_EXEHDR          1           /* exe386.h flag */
#define DWORD               ULONG       /* Used by exe386.h / newexe.h */
#define WORD                USHORT      /* Used by exe386.h / newexe.h */


/******************************************************************************
*   Header Files                                                              *
******************************************************************************/
#ifdef __EMX__
#define INT INT_
#define PCHAR PCHAR_
#endif
#include <os2.h>
#ifdef __EMX__
#undef PCHAR
#undef INT
#endif
#include <newexe.h>
#include <exe386.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <assert.h>

#include "kFileFormatBase.h"
#include "kFileLX.h"




/**
 * Create an LX file object from an LX executable image.
 * @param     pszFilename   LX executable image name.
 */
kFileLX::kFileLX(const char *pszFilename)
: pvBase(NULL)
{
    struct exe_hdr * pehdr;

    /* create filemapping */
    pvBase = kFileFormatBase::readfile(pszFilename);
    if (pvBase == NULL)
        throw(1);

    pehdr = (struct exe_hdr*)pvBase;
    if (pehdr->e_magic == EMAGIC)
        offLXHdr = pehdr->e_lfanew;
    else
        offLXHdr = 0;

    pe32 = (struct e32_exe*)((char*)pvBase + offLXHdr);
    if (*(PUSHORT)pe32 == E32MAGIC)
    {
        paObject = (struct o32_obj*)((char*)pvBase + pe32->e32_objtab + offLXHdr);
    }
    else
    {
        free(pvBase);
        pvBase = NULL;
        throw(2);
    }
}



/**
 * Destructor.
 */
kFileLX::~kFileLX()
{
    if (pvBase != NULL)
        free(pvBase);
    pvBase = NULL;
}



BOOL kFileLX::queryModuleName(char *pszBuffer)
{
    /* not implemented */
    assert("not implemented!");
    return FALSE;
}



BOOL kFileLX::findFirstExport(PEXPORTENTRY pExport)
{
    /* not implemented */
    assert("not implemented!");
    return FALSE;
}



BOOL kFileLX::findNextExport(PEXPORTENTRY pExport)
{
    /* not implemented */
    assert("not implemented!");
    return FALSE;
}



/**
 * Gets a specific LX object.
 * @returns   Pointer to object. NULL on error / invalid index.
 * @param     iObject   object number (0-based)
 */
struct o32_obj * kFileLX::getObject(int iObject)
{

    if (iObject < pe32->e32_objcnt)
        return &paObject[iObject];
    return NULL;
}


