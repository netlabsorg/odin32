/* $Id: kFileLX.cpp,v 1.2 2000-08-31 03:00:13 bird Exp $
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
    /* The module name is the 0 ordinal entry in resident name table */
    return queryExportName(0, pszBuffer);
}



BOOL kFileLX::findFirstExport(PEXPORTENTRY pExport)
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
                        pExport->offset = pEntry->e32_variant.e32_offset.offset16;
                        break;

                    case ENTRY32:
                        pExport->offset = pEntry->e32_variant.e32_offset.offset32;
                        break;

                    case GATE16:
                        pExport->offset = pEntry->e32_variant.e32_callgate.offset;
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
                return TRUE;
            }
        }

    }

    return FALSE;
}



BOOL kFileLX::findNextExport(PEXPORTENTRY pExport)
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

        /* offset */
        switch (pExpState->pb32->b32_type & ~TYPEINFO)
        {
            case ENTRY16:
                pExport->offset = pExpState->pe32->e32_variant.e32_offset.offset16;
                break;

            case ENTRY32:
                pExport->offset = pExpState->pe32->e32_variant.e32_offset.offset32;
                break;

            case GATE16:
                pExport->offset = pExpState->pe32->e32_variant.e32_callgate.offset;
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
                    pExport->offset = pExpState->pe32->e32_variant.e32_offset.offset16;
                    break;

                case ENTRY32:
                    pExport->offset = pExpState->pe32->e32_variant.e32_offset.offset32;
                    break;

                case GATE16:
                    pExport->offset = pExpState->pe32->e32_variant.e32_callgate.offset;
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

