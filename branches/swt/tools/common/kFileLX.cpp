/* $Id: kFileLX.cpp,v 1.7 2002-02-24 02:47:26 bird Exp $
 *
 *
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/******************************************************************************
*   Header Files                                                              *
******************************************************************************/
#include <MZexe.h>
#include <LXexe.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kTypes.h"
#include "kError.h"
#include "kFile.h"
#include "kFileFormatBase.h"
#include "kFileInterfaces.h"
#include "kFileLX.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _LXExportState
{
    struct b32_bundle * pb32;           /* Pointer to current bundle. */
    int                 iOrdinalBundle; /* The ordinal the bundle starts at. */
    struct e32_entry *  pe32;           /* Pointer to the current bundle entry. */
    int                 iOrdinal;       /* The current ordinal. */
} EXPSTATE, *PEXPSTATE;


/**
 * relocFind*() state info.
 * Stored as kRelocEntry::pv1.
 */
typedef struct _LXRelocState
{
    unsigned long   ulSegment;          /* The current segment. (0-based) */
    unsigned long   ulPage;             /* The current page. (0-based index) */
    char *          pchFixupRec;        /* The current fixup record. */
    unsigned short  usSrcOffIdx;        /* The current source offset index. (0-based) */
} LXRELOCSTATE, *PLXRELOCSTATE;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#if 0
static kFileLX tst((kFile*)NULL);
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
inline void memcpyw(char *pch1, const char *pch2, size_t cch);
inline void memcpyb(char *pch1, const char *pch2, size_t cch);


/**
 * Internal worker which lookup the name corresponding to an ordinal.
 * @returns Success indicator.
 * @param   iOrdinal ( >= 0).
 * @param   pszBuffer.
 */
KBOOL kFileLX::queryExportName(int iOrdinal, char *pszBuffer)
{
    unsigned short *pus;
    unsigned char * puch;

    /* resident name table */
    if (pe32->e32_restab)
    {
        puch = (unsigned char *)pvBase + offLXHdr + pe32->e32_restab;
        while (*puch != 0)
        {
            pus = (unsigned short *)(puch + 1 + *puch);
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
        puch = (unsigned char *)pvBase + pe32->e32_nrestab;
        while (*puch != 0)
        {
            pus = (unsigned short *)(puch + 1 + *puch);
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
 * Converts a FLAT address to an obj:offset address.
 * @returns The offset into the object.
 *          -1 (0xffffffff) on error.
 * @param   ulAddress   Address to map.
 * @param   pulObject   Pointer to variable which will receive the object number (0-based).
 */
unsigned long kFileLX::lxAddressToObjectOffset(unsigned long ulAddress, unsigned long * pulObject) const
{
    int     i;

    /* look for a object which contains the given address */
    for (i = 0; i < pe32->e32_objcnt; i++)
    {
        if (    paObject[i].o32_base <= ulAddress
            &&  paObject[i].o32_base + paObject[i].o32_size > ulAddress)
        {
            if (pulObject)
                *pulObject = i;
            return ulAddress - paObject[i].o32_base;
        }
    }

    return ~0UL;                        /* not found */
}


/**
 * Converts an sel:off address, where sel is one of the special selectors,
 * to a obj:off address.
 * @returns The offset into the object.
 *          -1 (0xffffffff) on error.
 * @param   offObject   Offset into the selector described by *pulObject.
 * @param   pulObject   IN: Special selector.
 *                      OUT: Pointer to variable which will receive the object number (0-based).
 */
unsigned long kFileLX::lxSpecialSelectorToObjectOffset(unsigned long offObject, unsigned long * pulObject) const
{
    switch (*pulObject)
    {
        case kRelocEntry::enmRVASelector:
            return lxAddressToObjectOffset(offObject + paObject[0].o32_base, pulObject);

        case kRelocEntry::enmVASelector:
            return lxAddressToObjectOffset(offObject, pulObject);

        default:
            kASSERT(!"Internal error line");
    }

    return ~0UL;
}


/**
 * Validates a FLAT address.
 * @returns TRUE if valid, FALSE if invalid.
 * @param   ulAddress   FLAT address to validate.
 */
KBOOL kFileLX::lxValidateAddress(unsigned long ulAddress) const
{
    int     i;

    /* look for a object which contains the given address */
    for (i = 0; i < pe32->e32_objcnt; i++)
    {
        if (    paObject[i].o32_base <= ulAddress
            &&  paObject[i].o32_base + paObject[i].o32_size > ulAddress)
        {
            return TRUE;
        }
    }

    return FALSE;
}


/**
 * Validates a object:offset address.
 * @returns TRUE if valid, FALSE if invalid.
 * @param   ulObject    Object number. (0-based)
 * @param   offObject   Offset into that object.
 */
KBOOL kFileLX::lxValidateObjectOffset(unsigned long ulObject, unsigned long offObject) const
{
    return pe32->e32_objcnt > ulObject && offObject < paObject[ulObject].o32_size;
}



/**
 * Gets the module name corresponding to the given ordinal.
 * @returns Pointer to sz of the modulename. The caller is responsible for freeing it.
 *          NULL if invalid ordinal.
 * @param   ordModule   Ordinal number of the module name. (1-based)
 */
char *kFileLX::lxGetImportModuleName(unsigned long ordModule) const
{
    if (pe32->e32_impmodcnt < ordModule)
        return NULL;

    char *pch = (char*)pe32 + pe32->e32_impmod;
    while (--ordModule)
        pch += *pch + 1;

    if (!*pch)
        return NULL;

    /* Copy the module name. */
    char *psz = new char[*pch + 1];
    strncpy(psz, pch + 1, *pch);
    psz[*pch] = '\0';
    return psz;
}


/**
 * Gets the proc name at the given offset.
 * @returns Pointer to sz of the proc name. The caller is responsible for freeing it.
 *          NULL if invalid offset.
 * @param   offProc     Offset of the proc name.
 */
char *kFileLX::lxGetImportProcName(unsigned long offProc) const
{
    char *pch = (char*)pe32 + pe32->e32_impproc + offProc;
    if (!*pch)
        return NULL;

    /* Copy the proc name. */
    char *psz = new char[*pch + 1];
    strncpy(psz, pch + 1, *pch);
    psz[*pch] = '\0';
    return psz;
}



/**
 * Create an LX file object from an LX executable image.
 * @param     pszFilename   LX executable image name.
 */
kFileLX::kFileLX(const char *pszFilename) throw (kError) :
    kFileFormatBase(NULL), pvBase(NULL)
{
    struct exe_hdr * pehdr;

    /* create filemapping */
    pvBase = kFile::mapFile(pszFilename);
    pehdr = (struct exe_hdr*)pvBase;
    if (pehdr->e_magic == EMAGIC)
        offLXHdr = pehdr->e_lfanew;
    else
        offLXHdr = 0;

    pe32 = (struct e32_exe*)((char*)pvBase + offLXHdr);
    if (*(unsigned short *)pe32 != E32MAGIC)
    {
        kFile::mapFree(pvBase);
        pvBase = NULL;
        throw(kError(kError::INVALID_EXE_SIGNATURE));
    }

    paObject = pe32->e32_objtab && pe32->e32_objcnt
        ? (struct o32_obj *)((char *)pe32 + pe32->e32_objtab) : NULL;
    paMap = pe32->e32_objmap
        ? (struct o32_map *)((char *)pe32 + pe32->e32_objmap) : NULL;
    paulFixupPageTable = pe32->e32_fpagetab
        ? (unsigned long *)((char *)pe32 + pe32->e32_fpagetab) : NULL;
    pchFixupRecs = pe32->e32_frectab
        ? (char *)((char *)pe32 + pe32->e32_frectab) : NULL;
}


/**
 * Create an LX file object from an LX executable image.
 * @param     pFile     Pointer to opened LX file.
 */
kFileLX::kFileLX(kFile *pFile) throw (kError) :
    kFileFormatBase(pFile), pvBase(NULL)
{
    struct exe_hdr * pehdr;

    /* create filemapping */
    pFile->setThrowOnErrors();
    pvBase = pFile->mapFile();

    pehdr = (struct exe_hdr*)pvBase;
    if (pehdr->e_magic == EMAGIC)
        offLXHdr = pehdr->e_lfanew;
    else
        offLXHdr = 0;

    pe32 = (struct e32_exe*)((char*)pvBase + offLXHdr);
    if (*(unsigned short *)pe32 != E32MAGIC)
    {
        kFile::mapFree(pvBase);
        pvBase = NULL;
        throw(kError(kError::INVALID_EXE_SIGNATURE));
    }

    paObject = pe32->e32_objtab && pe32->e32_objcnt
        ? (struct o32_obj *)((char *)pe32 + pe32->e32_objtab) : NULL;
    paMap = pe32->e32_objmap
        ? (struct o32_map *)((char *)pe32 + pe32->e32_objmap) : NULL;
    paulFixupPageTable = pe32->e32_fpagetab
        ? (unsigned long *)((char *)pe32 + pe32->e32_fpagetab) : NULL;
    pchFixupRecs = pe32->e32_frectab
        ? (char *)((char *)pe32 + pe32->e32_frectab) : NULL;
}



/**
 * Destructor.
 */
kFileLX::~kFileLX() throw (kError)
{
    if (pvBase != NULL)
        kFile::mapFree(pvBase);
    pvBase = NULL;
}



/**
 * Query for the module name.
 * @returns Success indicator. TRUE / FALSE.
 * @param   pszBuffer   Pointer to buffer which to put the name into.
 * @param   cchBuffer   Size of the buffer (defaults to 260 chars).
 */
KBOOL kFileLX::moduleGetName(char *pszBuffer, int cchBuffer/*= 260*/)
{
    /* The module name is the 0 ordinal entry in resident name table */
    kASSERT(cchBuffer >= 255);
    return queryExportName(0, pszBuffer);
}


/**
 * Finds the first exports.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 */
KBOOL kFileLX::exportFindFirst(kExportEntry *pExport)
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
                        kASSERT(!"ARG!!!! invalid bundle type!");
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
KBOOL kFileLX::exportFindNext(kExportEntry *pExport)
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
                    kASSERT(!"ARG!!!! invalid bundle type!");
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
KBOOL kFileLX::exportLookup(unsigned long ulOrdinal, kExportEntry *pExport)
{
    kASSERT(!"not implemented.");
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
KBOOL kFileLX::exportLookup(const char *  pszName, kExportEntry *pExport)
{
    kASSERT(!"not implemented.");
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


/**
 * Get a code, data or resource page from the file.
 * @returns 0 on success. kError number on error.
 * @param   pachPage    Pointer to a buffer of the size of a page which
 *                      will receive the page data on the specified address.
 * @param   ulAddress   Page address. This must be page aligned.
 */
int     kFileLX::pageGet(char *pachPage, unsigned long ulAddress) const
{
    int iObj;

    for (iObj = 0; iObj < pe32->e32_objcnt; iObj++)
        if (    paObject[iObj].o32_base <= ulAddress
            &&  paObject[iObj].o32_base + paObject[iObj].o32_size > ulAddress
            )
            return pageGet(pachPage, iObj, ulAddress - paObject[iObj].o32_base);

    return kError::INVALID_ADDRESS;
}

/**
 * Get a code, data or resource page from the file.
 * @returns 0 on success. kError number on error.
 * @param   pachPage    Pointer to a buffer of the size of a page which
 *                      will receive the page data on the specified address.
 * @param   iSegment    Segment number.  (0-based)
 * @param   offObject   Offset into the object. This must be page aligned.
 * @remark  Object = Section.
 */
int     kFileLX::pageGet(char *pachPage, int iSegment, int offObject) const
{
    /*
     * Validation.
     */
    if (offObject & (pe32->e32_pagesize - 1))
        return kError::BAD_ALIGNMENT;
    if (iSegment >= pe32->e32_objcnt)
        return kError::INVALID_SEGMENT_NUMBER;
    if (offObject >= paObject[iSegment].o32_size)
        return kError::INVALID_OFFSET;

    /*
     * Is there a pagemap entry for the requested page?
     */
    if ((offObject / pe32->e32_pagesize) < paObject[iSegment].o32_mapsize)
    {   /* yes */
        int     iPage = (offObject / pe32->e32_pagesize) + paObject[iSegment].o32_pagemap - 1;
        char *  pchPageData = (char*)((paMap[iPage].o32_pagedataoffset << pe32->e32_pageshift) + pe32->e32_datapage + (char*)pvBase);

        switch (paMap[iPage].o32_pageflags)
        {
            case VALID:
                memcpy(pachPage, pchPageData, paMap[iPage].o32_pagesize);
                if (paMap[iPage].o32_pagesize < pe32->e32_pagesize)
                    memset(pachPage + paMap[iPage].o32_pagesize, 0, pe32->e32_pagesize - paMap[iPage].o32_pagesize);
                break;

            case ITERDATA:
            {
                char achTempBuffer[0x1004];
                memcpy(achTempBuffer, pchPageData, paMap[iPage].o32_pagesize);
                memset(achTempBuffer + paMap[iPage].o32_pagesize, 0, 4);
                return kFileLX::expandPage1(pachPage, pe32->e32_pagesize, achTempBuffer, paMap[iPage].o32_pagesize);
            }

            case INVALID:
                return kError::INVALID_PAGE;

            case ZEROED:
                memset(pachPage, 0, pe32->e32_pagesize);
                break;

            case ITERDATA2:
        {
                char achTempBuffer[0x1004];
                memcpy(achTempBuffer, pchPageData, paMap[iPage].o32_pagesize);
                memset(achTempBuffer + paMap[iPage].o32_pagesize, 0, 4);
                return kFileLX::expandPage2(pachPage, pe32->e32_pagesize, achTempBuffer, paMap[iPage].o32_pagesize);
            }

            case RANGE:
            default:
                return kError::BAD_EXE_FORMAT;
        }

    }
    else
    {   /* no, so it's a zerofilled page */
        memset(pachPage, 0, pe32->e32_pagesize);
    }

    return 0;
}


/**
 * Updates or adds a code, data, or resource page to the file.
 * @returns 0 on success. kError number on error.
 * @param   pachPage    Pointer to a buffer of the size of a page which
 *                      holds the page data.
 * @param   ulAddress   Page address. This must be page aligned.
 */
int kFileLX::pagePut(const char *pachPage, unsigned long ulAddress)
{
    int iObj;

    for (iObj = 0; iObj < pe32->e32_objcnt; iObj++)
        if (    paObject[iObj].o32_base <= ulAddress
            &&  paObject[iObj].o32_base + paObject[iObj].o32_size > ulAddress
            )
            return pagePut(pachPage, iObj, ulAddress - paObject[iObj].o32_base);

    return kError::INVALID_ADDRESS;
}


/**
 * Updates or adds a code, data, or resource page to the file.
 * @returns 0 on success. kError number on error.
 * @param   pachPage    Pointer to a buffer of the size of a page which
 *                      holds the page data.
 * @param   iSegment    Segment number. (0-based)
 * @param   offObject   Offset into the object. This must be page aligned.
 */
int kFileLX::pagePut(const char *pachPage, int iSegment, int offObject)
{
    /*
     * Validation.
     */
    if (offObject & (pe32->e32_pagesize - 1))
        return kError::BAD_ALIGNMENT;
    if (iSegment >= pe32->e32_objcnt)
        return kError::INVALID_SEGMENT_NUMBER;
    if (offObject >= paObject[iSegment].o32_size)
        return kError::INVALID_OFFSET;

    /*
     * Is there a pagemap entry for the page?
     */
    if ((offObject / pe32->e32_pagesize) < paObject[iSegment].o32_mapsize)
    {   /* yes */
        int         iPage = (offObject / pe32->e32_pagesize) + paObject[iSegment].o32_pagemap - 1;
        char *      pchPageData = (char*)((paMap[iPage].o32_pagedataoffset << pe32->e32_pageshift) + pe32->e32_datapage + (char*)pvBase);
        const char *pchPageToWrite = NULL;
        int         cchPageToWrite = 0;
        long        offPageToWrite = (paMap[iPage].o32_pagedataoffset << pe32->e32_pageshift) + pe32->e32_datapage;

        switch (paMap[iPage].o32_pageflags)
        {
            case VALID:
                pchPageToWrite = pachPage;
                cchPageToWrite = 0x1000;
                if (paMap[iPage].o32_pagesize != 0x1000)
                {
                    /*
                     * Check that everything after pagesize is zero.
                     */
                    for (const char *pch = &pachPage[paMap[iPage].o32_pagesize]; pch < &pachPage[0x1000]; pch++)
                        if (*pch)
                            return kError::NOT_SUPPORTED;
                    cchPageToWrite = paMap[iPage].o32_pagesize;
}
                break;

            case ITERDATA:
            case ITERDATA2:
            {
                char * pchCompressedPage = (char*)alloca(pe32->e32_pagesize+4);

                /* remove spaces at the end of the page. */
                int cchPage = pe32->e32_pagesize;
                while (pachPage[cchPage-1] == '\0')
                    cchPage--;

                /* call the compression function */
                if (paMap[iPage].o32_pageflags == ITERDATA)
                    cchPageToWrite = kFileLX::compressPage1(pchCompressedPage, pachPage, cchPage);
                else
                    cchPageToWrite = kFileLX::compressPage2(pchCompressedPage, pachPage, cchPage);
                //if (cchPageToWrite != paMap[iPage].o32_pagesize)
                //    printf("compressPageX returned %d (%x)  previous size %d (%x)\n", cchPageToWrite, cchPageToWrite, paMap[iPage].o32_pagesize, paMap[iPage].o32_pagesize);
                if (cchPageToWrite < 0)
                    return kError::NOT_SUPPORTED;
                #if 0
                int cbLeft = KMIN(2, paMap[iPage].o32_pagesize - cchPageToWrite);
                if (cbLeft > 0)
{
                   memset(&pchCompressedPage[cchPageToWrite], 0, cbLeft);
                   cchPageToWrite += cbLeft;
                }
                #else
                kASSERT(paMap[iPage].o32_pagesize - cchPageToWrite >= 0);
                memset(&pchCompressedPage[cchPageToWrite], 0, paMap[iPage].o32_pagesize - cchPageToWrite);
                cchPageToWrite = paMap[iPage].o32_pagesize;
                #endif
                pchPageToWrite = pchCompressedPage;
                break;
}

            case ZEROED:
{
                /*
                 * If the passed in page is not a zero page we'll fail.
                 */
                for (unsigned long * pul = (unsigned long *)pachPage; (char*)pul < &pachPage[0x1000]; pul++)
                    if (*pul)
                        return kError::NOT_SUPPORTED;
                return 0;
            }

            case INVALID:
                return kError::INVALID_PAGE;

            case RANGE:
            default:
                return kError::BAD_EXE_FORMAT;
        }

        /*
         * Write stuff to file.
         */
        pFile->setFailOnErrors();
        return pFile->writeAt((void*)pchPageToWrite, cchPageToWrite, offPageToWrite);
    }
    else
    {   /*
         * No, it's a zerofilled page
         * If the passed in page is not a zero page we'll fail.
         */
        for (unsigned long *  pul = (unsigned long *)pachPage; (char*)pul < &pachPage[0x1000]; pul++)
             if (*pul)
                 return kError::NOT_SUPPORTED;
}

    return 0;
}


/**
 * Get pagesize for the file.
 * @returns Pagesize in bytes.
 */
int kFileLX::pageGetPageSize() const
{
    return pe32->e32_pagesize;
}


/**
 * Expands a page compressed with the old exepack method introduced with OS/2 2.0.
 * (/EXEPACK or just /EXEPACK)
 * @returns 0 on success. kError error code on error.
 * @param   pachPage        Pointer to output page. size: cchPage
 *                          Upon successful return this will contain the expanded page data.
 * @param   cchPage         Page size.
 * @param   pachSrcPage     Pointer to source data. size: cchSrcPage.
 *                          This data should be compressed with EXEPACK:2!
 * @param   cchSrcPage      Size of compressed data.
 * @sketch  Have no good idea right now.
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
int kFileLX::expandPage1(char *pachPage, int cchPage, const char * pachSrcPage, int cchSrcPage)
{
    struct LX_Iter *pIter = (struct LX_Iter *)pachSrcPage;
    char *          pachDestPage = pachPage; /* Store the pointer for boundrary checking. */

    /* Validate size of data. */
    if (cchSrcPage >= cchPage - 2)
        return kError::BAD_ITERPAGE;

    /*
     * Expand the page.
     */
    while (pIter->LX_nIter && cchSrcPage > 0)
    {
        /* Check if we're out of bound. */
        if (    pachPage - pachDestPage + pIter->LX_nIter * pIter->LX_nBytes > cchPage
            ||  cchSrcPage <= 0)
            return kError::BAD_ITERPAGE;

        //if (pIter->LX_nIter == 1)
        //    printf("get 0x%03x  stored  %3d bytes\n", pachPage - pachDestPage, pIter->LX_nBytes);
        //else
        //    printf("get 0x%03x   %3d iterations  %3d bytes\n", pachPage - pachDestPage, pIter->LX_nIter, pIter->LX_nBytes);

        if (pIter->LX_nBytes == 1)
        {   /* one databyte */
            memset(pachPage, pIter->LX_Iterdata, pIter->LX_nIter);
            pachPage += pIter->LX_nIter;
            cchSrcPage -= 4 + 1;
            pIter++;
        }
        else
        {
            for (int i = pIter->LX_nIter; i > 0; i--, pachPage += pIter->LX_nBytes)
                memcpy(pachPage, &pIter->LX_Iterdata, pIter->LX_nBytes);
            cchSrcPage -= 4 + pIter->LX_nBytes;
            pIter   = (struct LX_Iter *)((char*)pIter + 4 + pIter->LX_nBytes);
        }
        }

    /*
     * Zero remaining part of the page.
     */
    if (pachPage - pachDestPage < cchPage)
        memset(pachPage, 0, cchPage - (pachPage - pachDestPage));

    return 0;
    }


/**
 * Expands a page compressed with the exepack method introduced with OS/2 Warp 3.0.
 * (/EXEPACK:2)
 * @returns 0 on success. kError error code on error.
 * @param   pachPage        Pointer to output page. size: PAGESIZE
 *                          Upon successful return this will contain the expanded page data.
 * @param   cchPage         Page size.
 * @param   pachSrcPage     Pointer to source data. size: cchSrcPage.
 *                          This data should be compressed with EXEPACK:2!
 * @param   cchSrcPage      Size of compressed data.
 * @sketch  Have no good idea right now.
 * @status  Completely implemented and tested.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  tested.
 */
int kFileLX::expandPage2(char *pachPage, int cchPage, const char * pachSrcPage, int cchSrcPage)
{
    char *          pachDestPage = pachPage; /* Store the pointer for boundrary checking. */

    while (cchSrcPage > 0)
    {
        /*
         * Bit 0 and 1 is the encoding type.
         */
        switch (*pachSrcPage & 0x03)
        {
            /*
             *
             *  0  1  2  3  4  5  6  7
             *  type  |              |
             *        ----------------
             *             cch        <cch bytes of data>
             *
             * Bits 2-7 is, if not zero, the length of an uncompressed run
             *   starting at the following byte.
             *
             *  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
             *  type  |              |  |                    | |                     |
             *        ----------------  ---------------------- -----------------------
             *             zero                 cch                char to multiply
             *
             * If the bits are zero, the following two bytes describes a
             *   1 byte interation run. First byte is count, second is the byte to copy.
             *   A count of zero is means end of data, and we simply stops. In that case
             *   the rest of the data should be zero.
             */
            case 0:
            {
                if (*pachSrcPage)
                {
                    int cch = *pachSrcPage >> 2;
                    if (cchPage  < cch || cchSrcPage < cch + 1)
                        return kError::BAD_COMPESSED_PAGE;
                    memcpy(pachPage, pachSrcPage+1, cch);
                    pachPage += cch, cchPage -= cch;
                    pachSrcPage += cch + 1, cchSrcPage -= cch + 1;
                    break;
                }
                if (cchSrcPage < 2)
                    return kError::BAD_COMPESSED_PAGE;
                int cch = pachSrcPage[1];
                if (cch)
    {
                    if (cchSrcPage < 3 || cchPage < cch)
                        return kError::BAD_COMPESSED_PAGE;
                    memset(pachPage, pachSrcPage[2], cch);
                    pachPage += cch, cchPage -= cch;
                    pachSrcPage += 3, cchSrcPage -= 3;
                    break;
                }
                goto endloop;
    }


            /*
             *  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
             *  type  |  |  |     |  |                       |
             *        ----  -------  -------------------------
             *        cch1  cch2 - 3          offset            <cch1 bytes of data>
             *
             *  Two bytes layed out as described above, followed by cch1 bytes of data to be copied.
             *  The cch2(+3) and offset describes an amount of data to be copied from the expanded
             *    data relative to the current position. The data copied as you would expect it to be.
             */
            case 1:
    {
                if (cchSrcPage < 2)
                    return kError::BAD_COMPESSED_PAGE;
                int off = *(unsigned short*)pachSrcPage >> 7;
                int cch1 = *pachSrcPage >> 2 & 3;
                int cch2 = (*pachSrcPage >> 4 & 7) + 3;
                pachSrcPage += 2, cchSrcPage -= 2;
                if (cchSrcPage < cch1 || cchPage < cch1 + cch2 || pachPage + cch1 - off < pachDestPage)
                    return kError::BAD_COMPESSED_PAGE;
                memcpy(pachPage, pachSrcPage, cch1);
                pachPage += cch1, cchPage -= cch1;
                pachSrcPage += cch1, cchSrcPage -= cch1;
                memcpyb(pachPage, pachPage - off, cch2); //memmove doesn't do a good job here for some stupid reason.
                pachPage += cch2, cchPage -= cch2;
                break;
            }


            /*
             *  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
             *  type  |  |  |                                |
             *        ----  ----------------------------------
             *       cch-3              offset
             *
             *  Two bytes layed out as described above.
             *  The cch(+3) and offset describes an amount of data to be copied from the expanded
             *  data relative to the current position.
             *
             *  If offset == 1 the data is not copied as expected, but in the memcpyw manner.
             */
            case 2:
        {
                if (cchSrcPage < 2)
                    return kError::BAD_COMPESSED_PAGE;
                int off = *(unsigned short*)pachSrcPage >> 4;
                int cch = (*pachSrcPage >> 2 & 3) + 3;
                pachSrcPage += 2, cchSrcPage -= 2;
                if (cchPage < cch || pachPage - off < pachDestPage)
                    return kError::BAD_COMPESSED_PAGE;
                memcpyw(pachPage, pachPage - off, cch);
                pachPage += cch, cchPage -= cch;
                break;
        }


            /*
             *  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
             *  type  |        |  |              |  |                                |
             *        ----------  ----------------  ----------------------------------
             *           cch1           cch2                     offset                <cch1 bytes of data>
             *
             *  Three bytes layed out as described above, followed by cch1 bytes of data to be copied.
             *  The cch2 and offset describes an amount of data to be copied from the expanded
             *  data relative to the current position.
             *
             *  If offset == 1 the data is not copied as expected, but in the memcpyw manner.
             */
            case 3:
        {
                if (cchSrcPage < 3)
                    return kError::BAD_COMPESSED_PAGE;
                int cch1 = *pachSrcPage >> 2 & 0x000f;
                int cch2 = *(unsigned short*)pachSrcPage >> 6 & 0x003f;
                int off  = *(unsigned short*)(pachSrcPage+1) >> 4;
                pachSrcPage += 3, cchSrcPage -= 3;
                if (cchSrcPage < cch1 || cchPage < cch1 + cch2 || pachPage - off + cch1 < pachDestPage)
                    return kError::BAD_COMPESSED_PAGE;
                memcpy(pachPage, pachSrcPage, cch1);
                pachPage += cch1, cchPage -= cch1;
                pachSrcPage += cch1, cchSrcPage -= cch1;
                memcpyw(pachPage, pachPage - off, cch2);
                pachPage += cch2, cchPage -= cch2;
                break;
            }
        }
        }

endloop:;


    /*
     * Zero the rest of the page.
     */
    if (cchPage > 0)
        memset(pachPage, 0, cchPage);

    return 0;
        }


/**
 * This is a special memcpy for expandPage2 which performs a word based copy.
 * The difference between this, memmove and memcpy is that we'll allways read words.
 * @param   pch1    Target pointer.
 * @param   pch2    Source pointer.
 * @param   cch     size of memory block to copy from pch2 to pch1.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark
 */
inline void memcpyw(char *pch1, const char *pch2, size_t cch)
{
    /*
     * Use memcpy if possible.
     */
    if ((pch2 > pch1 ? pch2 - pch1 : pch1 - pch2) >= 4)
    {
        memcpy(pch1, pch2, cch);        /* BUGBUG! ASSUMES that memcpy move NO more than 4 bytes at the time! */
        return;
    }

    /*
     * Difference is less than 3 bytes.
     */
    if (cch & 1)
        *pch1++ = *pch2++;

    #if 0 //haven't found anyone yet. (not a big surprize!)
    /* Looking for a very special case! I wanna see if my theory is right. */
    if ((pch2 > pch1 ? pch2 - pch1 : pch1 - pch2) <= 1)
        INT3();
#endif

    for (cch >>= 1; cch > 0; cch--, pch1 += 2, pch2 += 2)
        *(unsigned short *)pch1 = *(unsigned short *)pch2;
}


/**
 * This is a special memcpy for expandPage2 which performs a memmove operation.
 * The difference between this and memmove is that this one works.
 *
 * @param   pch1    Target pointer.
 * @param   pch2    Source pointer.
 * @param   cch     size of memory block to copy from pch2 to pch1.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 */
inline void memcpyb(char *pch1, const char *pch2, size_t cch)
{
    /*
     * Use memcpy if possible.
 */
    if ((pch2 > pch1 ? pch2 - pch1 : pch1 - pch2) >= 4)
{
        memcpy(pch1, pch2, cch);        /* BUGBUG! ASSUMES that memcpy move NO more than 4 bytes at the time! */
        return;
    }

    /*
     * Difference is less than 3 bytes.
     */
    while(cch--)
        *pch1++ = *pch2++;
}



/**
 * Compresses a page using the old exepack method introduced with OS/2 2.0.
 * (/EXEPACK:1 or just /EXEPACK)
 * @returns Size of the compressed page in the pachPage buffer.
 *          PAGESIZE if failed to compress the page.
 *          -1 on error.
 * @param   pachPage        Pointer to output buffer. size: cchSrcPage.
 *                          This will hold the compressed page data upon return.
 * @param   pachSrcPage     Pointer to page to compress. size: cchSrcPage.
 * @param   cchSrcPage      Page size.
 * @sketch
 * @status  stub.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Not implemented.
 */

int compressPage1Search(const char *pch, const char **ppchSearch, const char *pchEnd, int *pcIterations)
{
    int         cch;
    const char* pchStart = pch;
    const char* pchSearch = *ppchSearch;

//    if (pch < pchSearch)
//        pch = pchSearch - 1;
    while ((cch = pchSearch - pch) > 0)
{
        int cchLeft = pchEnd - pchSearch;
        if (cchLeft < cch)
            {pch++; continue;}
        int cIter = 1;
        const char *pchMatch = pchSearch;
        while (pchMatch < pchEnd && !memcmp(pchMatch, pch, cch))
            pchMatch += cch, cIter++;

        int cchCost = pchStart < pch ? cch + 8 : cch + 4;
        if (cIter == 1 || cIter * cch < cchCost)
            {pch++; continue;}
        *ppchSearch = pch;
        *pcIterations = cIter;
        return cch;
    }

    return -1;
}

int compressPage1SearchStored(const char *pchStart, const char *pchEnd, int *pcchRun)
{
    if (pchStart+1 == pchEnd)
        return FALSE;

    const char *pch = pchStart;
    char ch = *pch++;
    while (pch < pchEnd)
        if (*pch++ != ch)
            break;

    if (pch == pchEnd)
    {
        *pcchRun = 1;
        return TRUE;
    }

    int cch = pchEnd - pchStart;
    if (cch < 4 && cch % 2 != 0)
        return FALSE;

    unsigned short* pus = (unsigned short*)pchStart;
    unsigned short  us = *pus++;
    while ((const char*)pus < pchEnd)
        if (*pus++ != us)
            return 0;

    *pcchRun = 2;
    return TRUE;
}


int kFileLX::compressPage1(char *pachPage, const char * pachSrcPage, int cchSrcPage)
{
    union
    {
        char           *  pch;
        unsigned short *  push;
        struct LX_Iter *  pIter;
    } Dst;
    Dst.pch = pachPage;
    const char *pch = pachSrcPage;
    const char *pchSearch = pch;
    const char *pchEnd = pch + cchSrcPage;
    while (pchSearch < pchEnd)
    {
        int cIterations;
        int cchPattern = compressPage1Search(pch, &pchSearch, pchEnd, &cIterations);
        if (cchPattern > 0)
        {   /* Found pattern. */
            /* make uncompressed chunk. */
            if (pch < pchSearch)
            {
                int cchRun;
                int cch = pchSearch - pch;
                if (cch < 9 && cch > 1 && compressPage1SearchStored(pch, pchSearch, &cchRun))
                {
                    /* make compressed chunk. */
                    Dst.pIter->LX_nIter = cch / cchRun;
                    Dst.pIter->LX_nBytes = cchRun;
                    memcpy(&Dst.pIter->LX_Iterdata, pch, cchPattern);
                    //printf("put 0x%03x   %3d iterations  %3d bytes\n", pch - pachSrcPage, Dst.pIter->LX_nIter, Dst.pIter->LX_nBytes);
                    Dst.pch += cchRun + 4;
                }
                else
                {   /* make uncompressed chunk */
                    Dst.pIter->LX_nIter = 1;
                    Dst.pIter->LX_nBytes = cch;
                    memcpy(&Dst.pIter->LX_Iterdata, pch, cch);
                    //printf("put 0x%03x  stored  %3d bytes\n", pch - pachSrcPage, Dst.pIter->LX_nBytes);
                    Dst.pch += cch + 4;
                }
                pch += cch;
            }

            /* make compressed chunk. */
            Dst.pIter->LX_nIter = cIterations;
            Dst.pIter->LX_nBytes = cchPattern;
            memcpy(&Dst.pIter->LX_Iterdata, pch, cchPattern);
            //printf("put 0x%03x   %3d iterations  %3d bytes\n", pch - pachSrcPage, Dst.pIter->LX_nIter, Dst.pIter->LX_nBytes);
            Dst.pch += cchPattern + 4;
            pchSearch = pch += cchPattern * cIterations;
        }
        else
        {   /* No pattern - got the next byte in the source page. */
            pchSearch++;
        }
    }

    /* make final uncompressed chunk(s) */
    if (pch < pchSearch)
    {
        int cch = pchSearch - pch;
        Dst.pIter->LX_nIter = 1;
        Dst.pIter->LX_nBytes = cch;
        memcpy(&Dst.pIter->LX_Iterdata, pch, cch);
        //printf("put 0x%03x  stored  %3d bytes %d\n", pch - pachSrcPage, Dst.pIter->LX_nBytes, cch);
        Dst.pch += cch + 4;
        pch += cch;
    }



    /* write terminating word - this is really not needed AFAIK, but it makes debugging easier. */
    //*Dst.push++ = 0;
    return Dst.pch - pachPage;
}



/**
 * Compresses a page using the exepack method introduced with OS/2 Warp 3.0.
 * (/EXEPACK:2)
 * @returns Size of the compressed page in the pachPage buffer.
 *          PAGESIZE if failed to compress the page.
 *          -1 on error.
 * @param   pachPage        Pointer to output buffer. size: cchSrcPage.
 *                          This will hold the compressed page data upon return.
 * @param   pachSrcPage     Pointer to page to compress. size: cchSrcPage.
 * @param   cchSrcPage      Page size.
 * @sketch  Have no idea!
 * @status  stub.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Not implemented.
 */
int kFileLX::compressPage2(char *pachPage, const char * pachSrcPage, int cchSrcPage)
{
    KNOREF(pachPage);
    KNOREF(pachSrcPage);
    KNOREF(cchSrcPage);
    return -1;
}


/**
 * Start a relocation enumeration.
 * @returns TRUE:   Found relocation.
 *          FALSE:  No more relocations.
 * @param   ulSegment   Segment number. Special selector may be specified.
 * @param   offSegment  Offset into the segment described by ulSegment.
 * @param   preloc      Pointer to a relocation 'handle' structure.
 *                      This is used for communicating the relocations in
 *                      a generic format and for keeping track of the current position.
 *                      Please, DO NOT change any members of this structure.
 */
KBOOL kFileLX::relocFindFirst(unsigned long ulSegment, unsigned long offSegment, kRelocEntry *preloc)
{
    if (ulSegment >= kRelocEntry::enmFirstSelector)
        offSegment = lxSpecialSelectorToObjectOffset(offSegment, &ulSegment);

    if (    lxValidateObjectOffset(ulSegment, offSegment)
        &&  pe32->e32_frectab != 0
        &&  pe32->e32_fpagetab != 0
        )
    {
        PLXRELOCSTATE  pState = (PLXRELOCSTATE)malloc(sizeof(LXRELOCSTATE));
        pState->ulSegment = ulSegment;
        pState->ulPage = paObject[ulSegment].o32_pagemap - 1;
        pState->pchFixupRec = pchFixupRecs + paulFixupPageTable[pState->ulPage];
        pState->usSrcOffIdx = 0;
        preloc->pv1 = (void*)pState;
        preloc->fFlags = 0;
        preloc->Info.Name.pszModule = NULL;
        preloc->Info.Name.pszName = NULL;
        return relocFindNext(preloc);
}

    return FALSE;
}


/**
 * Start a relocation enumeration.
 * @returns TRUE:   Found relocation.
 *          FALSE:  No more relocations.
 * @param   ulAddress   Address to start from.
 * @param   preloc      Pointer to a relocation 'handle' structure.
 *                      This is used for communicating the relocations in
 *                      a generic format and for keeping track of the current position.
 *                      Please, DO NOT change any members of this structure.
 */
KBOOL kFileLX::relocFindFirst(unsigned long ulAddress, kRelocEntry *preloc)
{
    return relocFindFirst(kRelocEntry::enmVASelector, ulAddress, preloc);
}


/**
 * Get the next relocation.
 * @returns TRUE:   Found relocation.
 *          FALSE:  No more relocations.
 * @param   preloc  Pointer to a relocation 'handle' structure.
 * @remark  preloc have to be opened by relocFindFirst before calling relocFindNext!
 */
KBOOL kFileLX::relocFindNext(kRelocEntry *preloc)
{
    PLXRELOCSTATE   pState = (PLXRELOCSTATE)preloc->pv1;
    int             iObj = pState->ulSegment;
    KBOOL           fFound = FALSE;

    while (iObj < pe32->e32_objcnt)
    {
        int iPage = pState->ulPage;

        while (iPage < paObject[iObj].o32_mapsize + paObject[iObj].o32_pagemap - 1)
        {
            char *  pchFixupRecEnd = pchFixupRecs + paulFixupPageTable[iPage + 1];

            while (pState->pchFixupRec < pchFixupRecEnd)
            {
                char *  pch;            /* Fixup record walker. */
                union _rel
                {
                    unsigned long   ul;
                    char *          pch;
                    struct r32_rlc *prlc;
                } rel;                  /* Fixup record */

                /*
                 * If we're completed here we'll simply return.
                 */
                if (fFound)
                    return TRUE;


                /*
                 * Do some init and cleanup.
                 */
                rel.pch = pState->pchFixupRec;
                if (preloc->isName())
                {
                    delete preloc->Info.Name.pszModule;
                    delete preloc->Info.Name.pszName;
                    preloc->Info.Name.pszName = preloc->Info.Name.pszModule = NULL;
                }


                /*
                 * We've got a problem! chained relocation records!
                 */
                switch (rel.prlc->nr_stype & NRSTYP)
                {
                    case NRSBYT:    preloc->fFlags = kRelocEntry::enm8      | kRelocEntry::enmOffset; break;
                    case NRSSEG:    preloc->fFlags = kRelocEntry::enm16_00  | kRelocEntry::enmOffset; break;
                    case NRSPTR:    preloc->fFlags = kRelocEntry::enm16_16  | kRelocEntry::enmOffset; break;
                    case NRSOFF:    preloc->fFlags = kRelocEntry::enm16     | kRelocEntry::enmOffset; break;
                    case NRPTR48:   preloc->fFlags = kRelocEntry::enm16_32  | kRelocEntry::enmOffset; break;
                    case NROFF32:   preloc->fFlags = kRelocEntry::enm32     | kRelocEntry::enmOffset; break;
                    case NRSOFF32:  preloc->fFlags = kRelocEntry::enm32     | kRelocEntry::enmRelEnd; break;
                    default:
                        kASSERT(FALSE);
                        return FALSE;
                }

                /* Set endian flag. */
                if (pe32->e32_border == E32LEBO && pe32->e32_border == E32LEWO)
                    preloc->fFlags |= kRelocEntry::enmLittleEndian;
                else if (pe32->e32_border == E32BEBO && pe32->e32_border == E32BEWO)
                    preloc->fFlags |= kRelocEntry::enmBigEndian;
                else
                {
                    kASSERT(FALSE);
                    return FALSE;
                }


                /*
                 * LX target type.
                 */
                pch = rel.pch + 3 + (rel.prlc->nr_stype & NRCHAIN ? 0 : 1); /* place pch at the 4th member. */
                switch (rel.prlc->nr_flags & NRRTYP)
                {
                    case NRRINT:
                        preloc->fFlags |= kRelocEntry::enmInternal;
                        if (rel.prlc->nr_flags & NR16OBJMOD)
                        {
                            preloc->Info.Internal.ulSegment = *(unsigned short *)pch - 1;
                            pch += 2;
                        }
                        else
                        {
                            preloc->Info.Internal.ulSegment = *(unsigned char *)pch - 1;
                            pch += 1;
                        }
                        if (rel.prlc->nr_flags & NR32BITOFF)
                        {
                            preloc->Info.Internal.offSegment = *(unsigned long *)pch;
                            pch += 4;
                        }
                        else
                        {
                            preloc->Info.Internal.offSegment = *(unsigned short *)pch;
                            pch += 2;
                        }
                        break;

                    case NRRORD:
                        preloc->fFlags |= kRelocEntry::enmOrdDLL;
                        if (rel.prlc->nr_flags & NR16OBJMOD)
                        {
                            preloc->Info.Name.pszModule = lxGetImportModuleName(*(unsigned short *)pch);
                            pch += 2;
                        }
                        else
                        {
                            preloc->Info.Name.pszModule = lxGetImportModuleName(*(unsigned char *)pch);
                            pch += 1;
                        }
                        if (rel.prlc->nr_flags & NR32BITOFF)
                        {
                            sprintf(preloc->Info.Name.pszName = new char[16], "#%lu", *(unsigned long *)pch);
                            preloc->Info.Name.ulOrdinal = (unsigned)*(unsigned long *)pch;
                            pch += 4;
                        }
                        else if (rel.prlc->nr_flags & NR8BITORD)
                        {
                            sprintf(preloc->Info.Name.pszName = new char[8], "#%u", (unsigned)*(unsigned char *)pch);
                            preloc->Info.Name.ulOrdinal = (unsigned)*(unsigned char *)pch;
                            pch += 1;
                        }
                        else
                        {
                            sprintf(preloc->Info.Name.pszName = new char[12], "#%u", (unsigned)*(unsigned short *)pch);
                            preloc->Info.Name.ulOrdinal = (unsigned)*(unsigned short *)pch;
                            pch += 2;
                        }
                        break;

                    case NRRNAM:
                        preloc->fFlags |= kRelocEntry::enmNameDLL;
                        if (rel.prlc->nr_flags & NR16OBJMOD)
                        {
                            preloc->Info.Name.pszModule = lxGetImportModuleName(*(unsigned short *)pch);
                            pch += 2;
                        }
                        else
                        {
                            preloc->Info.Name.pszModule = lxGetImportModuleName(*(unsigned char *)pch);
                            pch += 1;
                        }
                        preloc->Info.Name.ulOrdinal = ~0UL;;
                        if (rel.prlc->nr_flags & NR32BITOFF)
                        {
                            preloc->Info.Name.pszName = lxGetImportProcName(*(unsigned long *)pch);
                            pch += 4;
                        }
                        else
                        {
                            preloc->Info.Name.pszName = lxGetImportProcName(*(unsigned short *)pch);
                            pch += 2;
                        }
                        break;

                    case NRRENT:
                        preloc->fFlags |= kRelocEntry::enmInternal;
                        kASSERT(!"Not Implemented");
                        return FALSE;
                }

                /* addiative fixup? */
                if (rel.prlc->nr_flags & NRADD)
                {
                    preloc->fFlags |= kRelocEntry::enmAdditive;
                    if (rel.prlc->nr_flags & NR32BITADD)
                    {
                        preloc->ulAdd = *(unsigned long *)pch;
                        pch += 4;
                    }
                    else
                    {
                        preloc->ulAdd = *(unsigned short *)pch;
                        pch += 2;
                    }
                }

                /*
                 * obj:offset of the fixup
                 * If the offset is negative we'll skip it (ie. not set the found flag).
                 */
                preloc->ulSegment = iObj;
                preloc->offSegment = (iPage - paObject[iObj].o32_pagemap + 1) * pe32->e32_pagesize;
                if (rel.prlc->nr_stype & NRCHAIN)
                {
                    while (pState->usSrcOffIdx < (unsigned char)rel.prlc->r32_soff)
                    {
                        if (!(((unsigned short *)pch)[pState->usSrcOffIdx] & 0x8000))
                        {
                            preloc->offSegment += ((unsigned short*)pch)[pState->usSrcOffIdx];
                            fFound = TRUE;
                            break;
                        }
                        pState->usSrcOffIdx++;
                    }
                }
                else
                {
                    preloc->offSegment += rel.prlc->r32_soff;
                    if (!(rel.prlc->r32_soff & 0x8000))
                        fFound = TRUE;
                }


                /*
                 * Go to the next relocation.
                 */
                if (rel.prlc->nr_stype & NRCHAIN)
                {
                    if (++pState->usSrcOffIdx >= (unsigned char)rel.prlc->r32_soff)
                    {
                        pState->pchFixupRec = pState->pchFixupRec + (KSIZE)pch - rel.ul + ((unsigned char)rel.prlc->r32_soff) * 2;
                        pState->usSrcOffIdx = 0;
                    }
                }
                else
                    pState->pchFixupRec = pState->pchFixupRec + (KSIZE)pch - rel.ul;
            }


            /* next page */
            pState->ulPage = ++iPage;
            pState->pchFixupRec = pchFixupRecEnd;
            pState->usSrcOffIdx = 0;            /* Source Offset Index */
        }


        /* next object */
        pState->ulSegment = ++iObj;
        if (iObj >= pe32->e32_objcnt)
            break;
        pState->ulPage = paObject[iObj].o32_pagemap - 1;
        pState->pchFixupRec = pchFixupRecs + paulFixupPageTable[pState->ulPage];
        pState->usSrcOffIdx = 0;                /* Source Offset Index */
    }

    return fFound;
}


/**
 * Closes a relocatation search/enumeration.
 * Will free any internally allocated resources.
 *
 * @param   preloc  The enumeration 'handel'.
 * @remark  This function *must* be called to close a search/enumeration.
 */
void kFileLX::relocFindClose(kRelocEntry *preloc)
{
    /* free name storage */
    if (preloc->isName())
    {
        delete preloc->Info.Name.pszModule;
        delete preloc->Info.Name.pszName;
        preloc->Info.Name.pszName = preloc->Info.Name.pszModule = NULL;
    }

    /* free state info */
    memset(preloc->pv1, 0xff, sizeof(LXRELOCSTATE));
    free(preloc->pv1);
    preloc->pv1 = (void*)0xdeadbeef;
}



