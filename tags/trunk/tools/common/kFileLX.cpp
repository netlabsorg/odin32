/* $Id: kFileLX.cpp,v 1.5 2001-04-17 00:26:11 bird Exp $
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

#include "kFile.h"
#include "kFileFormatBase.h"
#include "kInterfaces.h"
#include "kFileLX.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _export_state
{
    struct b32_bundle * pb32;           /* Pointer to current bundle. */
    int                 iOrdinalBundle; /* The ordinal the bundle starts at. */
    struct e32_entry *  pe32;           /* Pointer to the current bundle entry. */
    int                 iOrdinal;       /* The current ordinal. */
} EXPSTATE, *PEXPSTATE;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#if 0
static kFileLX tst((kFile*)NULL);
#endif


/**
 * Internal worker which lookup the name corresponding to an ordinal.
 * @returns Success indicator.
 * @param   iOrdinal ( >= 0).
 * @param   pszBuffer.
 */
BOOL kFileLX::queryExportName(int iOrdinal, char *pszBuffer)
{
    PUSHORT pus;
    PUCHAR  puch;

    /* resident name table */
    if (pe32->e32_restab)
    {
        puch = (PUCHAR)pvBase + offLXHdr + pe32->e32_restab;
        while (*puch != 0)
        {
            pus = (PUSHORT)(puch + 1 + *puch);
            if (*pus == iOrdinal)
            {
                memcpy(pszBuffer, puch + 1, *puch);
                pszBuffer[*puch] = '\0';
                return TRUE;
            }
            puch += *puch + 1 + 2;
        }
    }

    /* not found, check the non-resident name table too */
    if (pe32->e32_nrestab)
    {
        puch = (PUCHAR)pvBase + pe32->e32_nrestab;
        while (*puch != 0)
        {
            pus = (PUSHORT)(puch + 1 + *puch);
            if (*pus == iOrdinal)
            {
                memcpy(pszBuffer, puch + 1, *puch);
                pszBuffer[*puch] = '\0';
                return TRUE;
            }
            puch += *puch + 1 + 2;
        }
    }

    return FALSE;
}



/**
 * Create an LX file object from an LX executable image.
 * @param     pszFilename   LX executable image name.
 */
kFileLX::kFileLX(const char *pszFilename)  throw (int)
: pvBase(NULL)
{
    struct exe_hdr * pehdr;

    /* create filemapping */
    pvBase = kFile::readFile(pszFilename);
    if (pvBase == NULL)
        throw(1);

    pehdr = (struct exe_hdr*)pvBase;
    if (pehdr->e_magic == EMAGIC)
        offLXHdr = pehdr->e_lfanew;
    else
        offLXHdr = 0;

    pe32 = (struct e32_exe*)((char*)pvBase + offLXHdr);
    if (*(PUSHORT)pe32 != E32MAGIC)
    {
        free(pvBase);
        pvBase = NULL;
        throw(2);
    }

    paObject = pe32->e32_objtab && pe32->e32_objcnt
        ? (struct o32_obj*)((char*)pvBase + pe32->e32_objtab + offLXHdr) : NULL;
    paMap = pe32->e32_objmap
        ? (struct o32_map*)((char*)pvBase + pe32->e32_objmap + offLXHdr) : NULL;
}


/**
 * Create an LX file object from an LX executable image.
 * @param     pFile     Pointer to opened LX file.
 */
kFileLX::kFileLX(kFile *pFile) throw (int)
: pvBase(NULL)
{
    struct exe_hdr * pehdr;

    /* create filemapping */
    pvBase = pFile->readFile();
    if (pvBase == NULL)
        throw(1);

    pehdr = (struct exe_hdr*)pvBase;
    if (pehdr->e_magic == EMAGIC)
        offLXHdr = pehdr->e_lfanew;
    else
        offLXHdr = 0;

    pe32 = (struct e32_exe*)((char*)pvBase + offLXHdr);
    if (*(PUSHORT)pe32 != E32MAGIC)
    {
        free(pvBase);
        pvBase = NULL;
        throw(2);
    }

    paObject = pe32->e32_objtab && pe32->e32_objcnt
        ? (struct o32_obj*)((char*)pvBase + pe32->e32_objtab + offLXHdr) : NULL;
    paMap = pe32->e32_objmap
        ? (struct o32_map*)((char*)pvBase + pe32->e32_objmap + offLXHdr) : NULL;
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



/**
 * Query for the module name.
 * @returns Success indicator. TRUE / FALSE.
 * @param   pszBuffer   Pointer to buffer which to put the name into.
 * @param   cchBuffer   Size of the buffer (defaults to 260 chars).
 */
BOOL kFileLX::moduleGetName(char *pszBuffer, int cchBuffer/*= 260*/)
{
    /* The module name is the 0 ordinal entry in resident name table */
    assert(cchBuffer >= 255);
    return queryExportName(0, pszBuffer);
}


/**
 * Finds the first exports.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 */
BOOL kFileLX::exportFindFirst(kExportEntry *pExport)
{
    struct b32_bundle * pBundle = (struct b32_bundle*)((char*)pvBase + pe32->e32_enttab + offLXHdr);
    struct e32_entry *  pEntry;
    int                 iOrdinal = 1;

    if (pe32->e32_enttab)
    {
        while (pBundle->b32_cnt != 0)
        {
            /* skip empty bundles */
            while (pBundle->b32_cnt != 0 && pBundle->b32_type == EMPTY)
            {
                iOrdinal += pBundle->b32_cnt;
                pBundle = (struct b32_bundle*)((char*)pBundle + 2);
            }

            /* FIXME forwarders are not implemented so we'll skip them too. */
            while (pBundle->b32_cnt != 0 && (pBundle->b32_type & ~TYPEINFO) == ENTRYFWD)
            {
                iOrdinal += pBundle->b32_cnt;
                pBundle = (struct b32_bundle*)((char*)(pBundle + 1) + pBundle->b32_cnt * 7);
            }

            /* we'll ignore any flags for the moment - TODO */
            if (pBundle->b32_cnt != 0)
            {
                pExport->ulOrdinal = iOrdinal;
                pExport->iObject = pBundle->b32_obj;

                /* look for name */
                pExport->achIntName[0] = '\0';
                if (!queryExportName(iOrdinal, pExport->achName))
                    pExport->achName[0] = '\0';

                pEntry = (struct e32_entry*)(pBundle+1);
                switch (pBundle->b32_type & ~TYPEINFO)
                {
                    case ENTRY16:
                        pExport->ulOffset = pEntry->e32_variant.e32_offset.offset16;
                        break;

                    case ENTRY32:
                        pExport->ulOffset = pEntry->e32_variant.e32_offset.offset32;
                        break;

                    case GATE16:
                        pExport->ulOffset = pEntry->e32_variant.e32_callgate.offset;
                        break;
                    default:
                        assert(!"ARG!!!! invalid bundle type!");
                }

                /* store status - current export entry */
                PEXPSTATE pExpState = (PEXPSTATE)malloc(sizeof(EXPSTATE));
                pExport->pv         = pExpState;
                pExpState->pb32     = pBundle;
                pExpState->iOrdinalBundle = iOrdinal;
                pExpState->pe32     = pEntry;
                pExpState->iOrdinal = iOrdinal;
                pExport->ulAddress = ~0UL; /* TODO */
                return TRUE;
            }
        }

    }

    return FALSE;
}


/**
 * Finds the next export.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 */
BOOL kFileLX::exportFindNext(kExportEntry *pExport)
{
    static int      acbEntry[] =
    {
        0,                              /* EMPTY    */
        3,                              /* ENTRY16  */
        5,                              /* GATE16   */
        5,                              /* ENTRY32  */
        7                               /* ENTRYFWD */
    };

    PEXPSTATE pExpState = (PEXPSTATE)pExport->pv;
    pExport->ulAddress = ~0UL; /* TODO */

    /*
     * Get more ordinals from the current bundle if any left.
     */
    if (pExpState->pb32->b32_cnt > (pExpState->iOrdinal - pExpState->iOrdinalBundle + 1))
    {
        /* skip to the next entry */
        pExpState->iOrdinal++;
        pExpState->pe32 = (struct e32_entry*)((char*)pExpState->pe32
                                              + acbEntry[pExpState->pb32->b32_type & ~TYPEINFO]);

        /* fill output struct */
        pExport->ulOrdinal = pExpState->iOrdinal;
        pExport->iObject = pExpState->pb32->b32_obj;

        /* look for name */
        pExport->achIntName[0] = '\0';
        if (!queryExportName(pExpState->iOrdinal, pExport->achName))
            pExport->achName[0] = '\0';

        /* ulOffset */
        switch (pExpState->pb32->b32_type & ~TYPEINFO)
        {
            case ENTRY16:
                pExport->ulOffset = pExpState->pe32->e32_variant.e32_offset.offset16;
                break;

            case ENTRY32:
                pExport->ulOffset = pExpState->pe32->e32_variant.e32_offset.offset32;
                break;

            case GATE16:
                pExport->ulOffset = pExpState->pe32->e32_variant.e32_callgate.offset;
                break;
        }

        return TRUE;
    }

    /*
     * next bundle.
     */
    pExpState->pb32 = (struct b32_bundle*)((char*)(pExpState->pb32 + 1) +
                        pExpState->pb32->b32_cnt * acbEntry[pExpState->pb32->b32_type & ~TYPEINFO]);
    while (pExpState->pb32->b32_cnt != 0)
    {
        /* skip empty bundles */
        while (pExpState->pb32->b32_cnt != 0 && pExpState->pb32->b32_type == EMPTY)
        {
            pExpState->iOrdinal += pExpState->pb32->b32_cnt;
            pExpState->pb32 = (struct b32_bundle*)((char*)pExpState->pb32 + 2);
        }

        /* FIXME forwarders are not implemented so we'll skip them too. */
        while (pExpState->pb32->b32_cnt != 0 && (pExpState->pb32->b32_type & ~TYPEINFO) == ENTRYFWD)
        {
            pExpState->iOrdinal += pExpState->pb32->b32_cnt;
            pExpState->pb32 = (struct b32_bundle*)((char*)(pExpState->pb32 + 1) + pExpState->pb32->b32_cnt * 7);
        }

        /* we'll ignore any flags for the moment - TODO */
        if (pExpState->pb32->b32_cnt != 0)
        {
            pExpState->iOrdinalBundle = pExpState->iOrdinal;

            pExport->ulOrdinal = pExpState->iOrdinal;
            pExport->iObject = pExpState->pb32->b32_obj;

            /* look for name */
            pExport->achIntName[0] = '\0';
            if (!queryExportName(pExpState->iOrdinal, pExport->achName))
                pExport->achName[0] = '\0';

            pExpState->pe32 = (struct e32_entry*)(pExpState->pb32+1);
            switch (pExpState->pb32->b32_type & ~TYPEINFO)
            {
                case ENTRY16:
                    pExport->ulOffset = pExpState->pe32->e32_variant.e32_offset.offset16;
                    break;

                case ENTRY32:
                    pExport->ulOffset = pExpState->pe32->e32_variant.e32_offset.offset32;
                    break;

                case GATE16:
                    pExport->ulOffset = pExpState->pe32->e32_variant.e32_callgate.offset;
                    break;
                default:
                    assert(!"ARG!!!! invalid bundle type!");
            }

            return TRUE;
        }
    }


    /*
     * No more exports - clean up
     */
    free(pExport->pv);
    pExport->pv = NULL;
    return FALSE;
}


/**
 * Frees resources associated with the communicatin area.
 * It's not necessary to call this when exportFindNext has return FALSE.
 * @param   pExport     Communication area which has been successfully
 *                      processed by findFirstExport.
 */
void kFileLX::exportFindClose(kExportEntry *pExport)
{
    free(pExport->pv);
    pExport->pv = NULL;
    return;
}


/**
 * Lookup information on a spesific export given by ordinal number.
 * @returns Success indicator.
 * @param   pExport     Communication area containing export information
 *                      on successful return.
 * @remark  stub
 */
BOOL kFileLX::exportLookup(unsigned long ulOrdinal, kExportEntry *pExport)
{
    assert(!"not implemented.");
    ulOrdinal = ulOrdinal;
    pExport = pExport;
    return FALSE;
}

/**
 * Lookup information on a spesific export given by name.
 * @returns Success indicator.
 * @param   pExport     Communication area containing export information
 *                      on successful return.
 * @remark  stub
 */
BOOL kFileLX::exportLookup(const char *  pszName, kExportEntry *pExport)
{
    assert(!"not implemented.");
    pszName = pszName;
    pExport = pExport;
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


/**
 * Gets the count of LX objects.
 * @returns     Count of LX objects.
 */
int kFileLX::getObjectCount()
{
    return (int)pe32->e32_objcnt;
}

#if 0

/** @cat Get and put page methods. (the first ones are generic) */
BOOL kFileLX::getPage(char *pachPage, ULONG ulAddress)
{
    int iObj;

    for (iObj = 0; iObj < pe32->e32_objcnt; iObj++)
        if (    paObject[iObj].o32_base > ulAddress
            &&  paObject[iObj].o32_base + paObject[iObj].o32_size > ulAddress
            )
            return getPage(pachPage, iObj, ulAddress - paObject[iObj].o32_base);

    return FALSE;
}

BOOL kFileLX::getPage(char *pachPage, int iObject, int offObject)
{
    offObject &= ~(PAGESIZE - 1);
    if (    iObject >= pe32->e32_objcnt
        &&  offObject >= paObject[iObject].o32_size
        )
        return FALSE;

    /*
     * Is there a pagemap entry for the requested page?
     */
    if ((offObject >> PAGESHIFT) < paObject[iObject].o32_mapsize)
    {   /* yes */
        int     iPage = (offObject >> PAGESIZE) + paObject[iObject].o32_pagemap - 1;
        char *  pchPageData = (char*)((paMap[iPage].o32_pagedataoffset << pe32->e32_pageshift) + pe32->e32_datapage + (char*)pvBase);

        if (pchPageData )
        {
        }

    }
    else
    {   /* no, so it's a zero page */
        memset(pachPage, 0, PAGESIZE);
    }

    return FALSE;
}

BOOL kFileLX::putPage(const char *pachPage, ULONG ulAddress)
{
    return FALSE;
}

BOOL kFileLX::putPage(const char *pachPage, int iObject, int offObject)
{
    return FALSE;
}

BOOL kFileLX::getPageLX(char *pachPage, int iObject, int iPage)
{
    return FALSE;
}

BOOL kFileLX::getPageLX(char *pachPage, int iPage)
{
    return FALSE;
}

BOOL kFileLX::putPageLX(const char *pachPage, int iObject, int iPage)
{
    return FALSE;
}

BOOL kFileLX::putPageLX(const char *pachPage, int iPage)
{
    return FALSE;
}


/**
 * Expands a page compressed with the old exepack method introduced with OS/2 2.0.
 * (/EXEPACK or just /EXEPACK)
 * @returns Successindicator.
 * @param   pachPage        Pointer to output page. size: PAGESIZE
 *                          Upon successful return this will contain the expanded page data.
 * @param   pachSrcPage     Pointer to source data. size: cchSrcPage.
 *                          This data should be compressed with EXEPACK:2!
 * @sketch  Have no good idea right now.
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
BOOL kFileLX::expandPage1(char *pachPage, const char * pachSrcPage, int cchSrcPage)
{
    struct LX_Iter *pIter = (struct LX_Iter *)pachSrcPage;
    char *          pachDestPage = pachPage; /* Store the pointer for boundrary checking. */

    /* Validate size of data. */
    if (cchSrcPage >= PAGESIZE - 2)
        return FALSE;

    /*
     * Expand the page.
     */
    while (pIter->LX_nIter)
    {
        /* Check if we're out of bound. */
        if (    pachPage - pachDesPage + pIter->LX_nIter * pIter->LX_nBytes > PAGESIZE
            ||  cchSrcPage <= 0)
            return FALSE;

        if (pIter->LX_nBytes == 1)
        {   /* one databyte */
            memset(pachPage, &pIter->LX_Iterdata, pIter->LX_nIter);
            pchPage += pIter->LX_nIter;
            cchSrcPage -= pIter->LX_nIter + 4;
            pIter++;
        }
        else
        {
            for (int i = 0; i < pIter->LX_nIter; i++, pachPage += pIter->LX_nBytes)
                memcpy(pachPage, &pIter->LX_Iterdata, pIter->LX_nBytes);
            cchSrcPage -= 4 + pIter->LX_nBytes;
            pIter   = (pIter)((char*)pIter 4 + pIter->LX_nBytes);
        }
    }
    return TRUE;
#if 0
/* example code */
    int             off;
    struct LX_Iter *pIter;
    char *          pch = pvPage; /* Current position on page */

    off = pe32->e32_datapage + (pObjMap->o32_pagedataoffset << pe32->e32_pageshift);
    if (pObjMap->o32_pagesize && (off + pObjMap->o32_pagesize - 1) >= cbFile)
    {
        fprintf(stderr, "Error: Page resides outside of the file.\n");
        return 1;
    }
    pIter = (struct LX_Iter *)((char*)pvFile + off);

    /* expand page */
    while (pIter->LX_nIter > 0)
    {
        if (pch + (pIter->LX_nBytes * pIter->LX_nIter) > (char*)pvPage + OBJPAGELEN)
        {
            fprintf(stderr, "Error: Iterated page expands to more than a page!\n");
            return 1;
        }

        if (pIter->LX_nBytes == 1)
        {
            memset(pch, pIter->LX_Iterdata, pIter->LX_nIter);
            pch += pIter->LX_nIter;
            pIter++;
        }
        else
        {
            int i;
            for (i = 0; i < pIter->LX_nIter; i++, pch += pIter->LX_nBytes)
                memcpy(pch, &pIter->LX_Iterdata, pIter->LX_nBytes);
            #if 1 /* curious */
            if (pIter->LX_nBytes > 2) fprintf(stdout, "pIter->LX_nBytes = %\n", pIter->LX_nBytes);
            #endif

            pIter = (struct LX_Iter*)((char*)pIter + 4 + pIter->LX_nBytes);
        }
    }

    return FALSE;
#endif
}


/**
 * Expands a page compressed with the exepack method introduced with OS/2 Warp 3.0.
 * (/EXEPACK:2)
 * @returns Successindicator.
 * @param   pachPage        Pointer to output page. size: PAGESIZE
 *                          Upon successful return this will contain the expanded page data.
 * @param   pachSrcPage     Pointer to source data. size: cchSrcPage.
 *                          This data should be compressed with EXEPACK:2!
 * @sketch  Have no good idea right now.
 * @status  Stub.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
BOOL kFileLX::expandPage2(char *pachPage, const char * pachSrcPage, int cchSrcPage)
{
    NOREF(pachPage);
    NOREF(pachSrcPage);
    NOREF(cchSrcPage);
    return FALSE;
}


/**
 * Compresses a page using the old exepack method introduced with OS/2 2.0.
 * (/EXEPACK:1 or just /EXEPACK)
 * @returns Size of the compressed page in the pachPage buffer.
 *          PAGESIZE if failed to compress the page.
 *          -1 on error.
 * @param   pachPage        Pointer to output buffer. size: PAGESIZE.
 *                          This will hold the compressed page data upon return.
 * @param   pachSrcPage     Pointer to page to compress. size: PAGESIZE.
 * @sketch
 * @status  stub.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Not implemented.
 */
int kFileLX::compressPage1(char *pachPage, const char * pachSrcPage)
{
    NOREF(pachPage);
    NOREF(pachSrcPage);
    return -1;
}


/**
 * Compresses a page using the exepack method introduced with OS/2 Warp 3.0.
 * (/EXEPACK:2)
 * @returns Size of the compressed page in the pachPage buffer.
 *          PAGESIZE if failed to compress the page.
 *          -1 on error.
 * @param   pachPage        Pointer to output buffer. size: PAGESIZE.
 *                          This will hold the compressed page data upon return.
 * @param   pachSrcPage     Pointer to page to compress. size: PAGESIZE.
 * @sketch  Have no idea!
 * @status  stub.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Not implemented.
 */
int kFileLX::compressPage2(char *pachPage, const char * pachSrcPage)
{
    NOREF(pachPage);
    NOREF(pachSrcPage);
    return -1;
}

#endif
