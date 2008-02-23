/* $Id: smalloc.c,v 1.2 2002-03-31 19:01:18 bird Exp $
 *
 * Swappable Heap - AVL.
 *
 * Note: This heap does very little checking on input.
 *       Use with care! We're running at Ring-0!
 *
 * Copyright (c) 1999-2001 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: smalloc.c,v 1.2 2002-03-31 19:01:18 bird Exp $";
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#pragma info(notrd)
#ifdef DEBUG
    #define DEBUG_ALLOC
    #undef ALLWAYS_HEAPCHECK
    #undef SOMETIMES_HEAPCHECK
#endif

#define SIGNATURE_END           0xBEDAADEB
#define SIGNATURE_START         0xDABEEBAD
#define HEAPANCHOR_SIGNATURE    0xBEEFFEEB

#ifdef DEBUG_ALLOC
    #define CB_SIGNATURE_START  4
    #define CB_SIGNATURE_END    4
#else
    #define CB_SIGNATURE_START  0
    #define CB_SIGNATURE_END    0
#endif
#define CB_SIGNATURES           (CB_SIGNATURE_END + CB_SIGNATURE_START)

#define PNEXT_BLOCK(a)          ((AVLULKEY)(((PMEMBLOCK)(a))->u1.uData + ((PMEMBLOCK)(a))->u2.cbSize))
#define PDATA(a)                ((a) != NULL ? (PMEMBLOCK)(a)->u1.pvData : NULL)
#define MEMBLOCK_FROM_FREESIZENODE(a) \
                                ((PMEMBLOCK)((unsigned)(a) - (unsigned)(&((PMEMBLOCK)0)->u2.coreFree)))

#define BLOCKSIZE               (1024*256)            /* 256KB */
#define ALIGNMENT               (sizeof(unsigned))
#define MEMBLOCKS_PER_CHUNK     (256)


#define INCL_DOS
#define INCL_DOSERRORS
#ifdef RING0
    #define INCL_NOAPI
#else
    #define INCL_DOSMEMMGR
#endif


/******************************************************************************
*  Headerfiles
******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#ifdef RING0
    #include "dev32hlp.h"
#endif
#include "kLib/kTypes.h"
#include "kLib/kAVL.h"
#include "kLib/kLog.h"
#include "kKLsmalloc.h"
#include "kKLrmalloc.h"
#include "dev32.h"
#include <memory.h>


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(4)
typedef struct _MEMBLOCK /* mb */
{
    union
    {
        AVLULNODECORE    core;          /* AVLNode core for the tree sorted on address */
        void *           pvData;        /* Pointer to the memory (core.Key and uData) */
        unsigned         uData;         /* Address to the memory (core.Key and pvData) */
        unsigned *       puData;        /* Address to the memory (core.Key and pvData) */
    } u1;
    union
    {
        AVLULNODECORE   coreFree;       /* AVLNode core for the tree sorted on size */
        unsigned        cbSize;         /* Size of the memory block. (Key of the AVLNodeCore) */
    } u2;
    struct _MEMBLOCK   *pmbNext;        /* Pointer to list of blocks with the same size */
} MEMBLOCK, *PMEMBLOCK;


typedef struct _HeapAnchor /* ha */
{
    #ifdef DEBUG_ALLOC
    unsigned long       ulSignature;    /* Contains HEAPANCHOR_SIGNATURE */
    #endif
    void *              pvBlock;        /* Pointer to the start of this block. */
    unsigned            cbSize;         /* Total amount of memory in this block. */
    struct _HeapAnchor *pNext;          /* Pointer to the next anchor block. */
    struct _HeapAnchor *pPrev;          /* Pointer to the previous anchor block. */
    PAVLULNODECORE      pcoreFree;      /* Pointer to the used-memblock chain. */
    PAVLULNODECORE      pcoreFreeSize;  /* Pointer to free-size-tree. */
    unsigned            cbFree;         /* Amount of free memory. */
    PAVLULNODECORE      pcoreUsed;      /* Pointer to the used-memblock chain. */
    unsigned            cbUsed;         /* Amount of used memory. */

} HEAPANCHOR, *PHEAPANCHOR;


typedef struct _MemblockChunk /* mc */
{
    struct _MemblockChunk * pNext;      /* Pointer to next chunk */
    unsigned            cFree;          /* Number of free memblocks */
    char                achBitmap[MEMBLOCKS_PER_CHUNK/8]; /* Used(1)/Free(0) bitmap */
    MEMBLOCK            amb[MEMBLOCKS_PER_CHUNK]; /* Array of memblocks */
} MEMBLOCKCHUNK, *PMEMBLOCKCHUNK;


typedef struct _SubHeaps_Callback_param
{
    unsigned    cb;
    unsigned    c;
} SUBHEAPS_CALLBACK_PARAM, *PSUBHEAPS_CALLBACK_PARAM;


typedef struct _Allocated_Callback_param
{
    unsigned    cb;                    /* Number of bytes of user data to dump. */
    unsigned    cbAllocated;
    unsigned    cBlocks;
} ALLOCATED_CALLBACK_PARAM, *PALLOCATED_CALLBACK_PARAM;

/******************************************************************************
*  Global data
******************************************************************************/
static PHEAPANCHOR  phaFirst;           /* Pointer to the first anchor block.*/
static PHEAPANCHOR  phaLast;            /* Pointer to the last anchor block.*/
unsigned            cbSwpHeapMax;       /* Maximum amount of memory used by the heap. */
static PMEMBLOCKCHUNK pmcFirst;         /* Pointer to the first memblock chunk. */

#ifndef RING0
char                fSwpInited;         /* init flag */
#endif


/******************************************************************************
*  Internal functions
******************************************************************************/
INLINE PMEMBLOCK    swpAllocateMemblock(void);
INLINE void         swpReleaseMemblock(PMEMBLOCK pmb);
static void         swpInsertUsed(PHEAPANCHOR pha, PMEMBLOCK pmb);
static void         swpRemoveFromFreeSize(PHEAPANCHOR pha, PMEMBLOCK pmb);
static void         swpInsertIntoFreeSize(PHEAPANCHOR pha, PMEMBLOCK pmb);
static void         swpInsertFree(PHEAPANCHOR pha, PMEMBLOCK pmb);
static PMEMBLOCK    swpGetFreeMemblock(PHEAPANCHOR *ppha, unsigned cbUserSize);
static PMEMBLOCK    swpFindUsedBlock(PHEAPANCHOR *ppha, void *pvUser);
#if 0
static PMEMBLOCK    swpFindWithinUsedBlock(PHEAPANCHOR *ppha, void *pvUser);
#endif
#ifdef DEBUG_ALLOC
static int          swpCheckAVLTree(PMEMBLOCK pmb);
static int          swpCheckAVLTreeFree(PAVLULNODECORE pNode);
#endif
static unsigned     _swp_dump_subheaps_callback(PMEMBLOCK pmb, PSUBHEAPS_CALLBACK_PARAM pCb);
static unsigned     _swp_dump_allocated_callback(PMEMBLOCK pmb, PALLOCATED_CALLBACK_PARAM pParam);


/**
 * Allocates a new memblock.
 * @returns   Pointer to a Memblock.
 * @author    knut st. osmundsen
 * @remark    Quick and dirty implementation.
 */
INLINE PMEMBLOCK swpAllocateMemblock(void)
{
    KLOGENTRY0("PMEMBLOCK");
    #if 0
    PMEMBLOCK pmb = (PMEMBLOCK)rmalloc(sizeof(MEMBLOCK));
    if (pmb != NULL)
        memset(pmb, 0, sizeof(*pmb));
{
    KLOGEXIT(pmb);
    return pmb;
}

    #else

    unsigned long *pul;
    int            i = 0;
    PMEMBLOCKCHUNK pmcLast = NULL;
    PMEMBLOCKCHUNK pmc = pmcFirst;
    while (pmc != NULL && pmc->cFree == 0)
    {
        pmcLast =  pmc;
        pmc = pmc->pNext;
    }

    /* allocate another chunk? */
    if (pmc == NULL)
    {
        pmc = rmalloc(sizeof(*pmc));
        if (pmc != NULL)
        {
            memset(pmc, 0, sizeof(*pmc));
            pmc->cFree = MEMBLOCKS_PER_CHUNK;
            if (pmcLast == NULL)
                pmcFirst = pmc;
            else
                pmcLast->pNext = pmc;
        }
        else
        {
            kprintf(("rmalloc failed\n"));
            KLOGEXIT(NULL);
            return NULL;
        }
    }

    /* find first free */
    pmc->cFree--;
    pul = (unsigned long*)pmc->achBitmap;
    while (*pul == 0xFFFFFFFF)
        pul++, i += 8*4;
    while (pmc->achBitmap[i/8] & (0x1 << (i%8)))
        i++;
    pmc->achBitmap[i/8] |=  (0x1 << (i%8));
    KLOGEXIT(&pmc->amb[i]);
    return &pmc->amb[i];
    #endif
}

/**
 * Releases a memblock.
 * @param     pmb  Pointer to the memory block which is to be release.
 * @author    knut st. osmundsen
 * @remark    Quick and dirty implementation.
 */
INLINE void swpReleaseMemblock(PMEMBLOCK pmb)
{
    KLOGENTRY1("void","PMEMBLOCK pmb", pmb);
    #if 0
    if (pmb != NULL)
        rfree(pmb);
    #else
    int             i;
    PMEMBLOCKCHUNK  pmc = pmcFirst;
    while (pmc != NULL && !((void*)pmc < (void*)pmb && (unsigned)pmc + sizeof(*pmc) > (unsigned)pmb))
        pmc = pmc->pNext;

    if (pmc != NULL)
    {
        i = ((unsigned)pmb - (unsigned)pmc->amb) / sizeof(pmc->amb[0]);
        #ifdef DEBUG
        if ((pmc->achBitmap[i / 8] & (1 << (i % 8))) == 0)
        {
            kprintf(("the memblock requested to be freed are allread free!\n"));
            pmc->cFree--;
        }
        #endif
        pmc->cFree++;
        pmc->achBitmap[i / 8] &= pmc->achBitmap[i / 8] & ~(0x1 << (i % 8));
    }
    else
        kprintf(("hmm pmb is not found within any pmc.\n"));
    #endif
    KLOGEXITVOID();
}


/**
 * Inserts a memblock into the used chain.
 * @param    pha  Pointer to the heap anchor which the block is to be inserted into.
 * @param    pmb  Pointer to memblock to insert into the free list.
 * @remark   Sorts on address.
 */
static void swpInsertUsed(PHEAPANCHOR pha, PMEMBLOCK pmb)
{
    KLOGENTRY2("void","PHEAPANCHOR pha, PMEMBLOCK pmb", pha, pmb);
    pha->cbUsed += pmb->u2.cbSize;
    AVLULInsert(&pha->pcoreUsed, &pmb->u1.core);
    KLOGEXITVOID();
}


/**
 * Remove a given memblockfree from the free-size-tree.
 */
static void swpRemoveFromFreeSize(PHEAPANCHOR pha, PMEMBLOCK pmb)
{
    KLOGENTRY2("void","PHEAPANCHOR pha, PMEMBLOCK pmb", pha, pmb);
    PMEMBLOCK pmbParent;
    PMEMBLOCK pmbTmp;

    pmbTmp = (PMEMBLOCK)AVLULGetWithParent(&pha->pcoreFreeSize,
                                           (PPAVLULNODECORE)SSToDS(&pmbParent),
                                           pmb->u2.coreFree.Key);
    if (pmbTmp != NULL)
    {
        pmbTmp = MEMBLOCK_FROM_FREESIZENODE(pmbTmp);
        if (pmbTmp != pmb)
        {
            PMEMBLOCK pmbPrev;
            do
            {
                pmbPrev = pmbTmp;
                pmbTmp = pmbTmp->pmbNext;
            } while (pmbTmp != NULL && pmbTmp != pmb);

            if (pmbTmp != NULL)
                pmbPrev->pmbNext = pmbTmp->pmbNext;
            else
                kprintf(("internal heap error, pmb not in list.\n"));
        }
        else
        {   /* pmb is first in the list */
            if (pmbTmp->pmbNext == NULL)
            {   /* no list - no other nodes of this size: simply remove it. */
                AVLULRemove(&pha->pcoreFreeSize, pmb->u2.coreFree.Key);
            }
            else
            {   /* other nodes of this size: replace pmb with the first node in the chain. */
                memcpy((void*)&pmbTmp->pmbNext->u2.coreFree, (void*)&pmbTmp->u2.coreFree, sizeof(pmbTmp->u2.coreFree));
                if (pmbParent != NULL)
                {
                    pmbParent = MEMBLOCK_FROM_FREESIZENODE(pmbParent);
                    if (pmbTmp->u2.coreFree.Key < pmbParent->u2.coreFree.Key)
                        pmbParent->u2.coreFree.pLeft = &pmb->pmbNext->u2.coreFree;
                    else
                        pmbParent->u2.coreFree.pRight = &pmb->pmbNext->u2.coreFree;
                }
                else
                    pha->pcoreFreeSize = &pmb->pmbNext->u2.coreFree;
            }
        }
    }
    else
        kprintf(("internal heap error, free-node not in free-size tree.\n"));
    KLOGEXITVOID();
}


/**
 * Inserts a block into the free-size-tree.
 */
static void swpInsertIntoFreeSize(PHEAPANCHOR pha, PMEMBLOCK pmb)
{
    KLOGENTRY2("void","PHEAPANCHOR pha, PMEMBLOCK pmb", pha, pmb);
    PMEMBLOCK pmbTmp;

    pmbTmp = (PMEMBLOCK)AVLULGet(&pha->pcoreFreeSize, pmb->u2.coreFree.Key);
    if (pmbTmp != NULL)
    {
        pmbTmp = MEMBLOCK_FROM_FREESIZENODE(pmbTmp);
        while (pmbTmp->pmbNext != NULL && pmbTmp->pmbNext > pmb)
            pmbTmp = pmbTmp->pmbNext;

        pmb->pmbNext = pmbTmp->pmbNext;
        pmbTmp->pmbNext = pmb;
    }
    else
    {
        pmb->pmbNext = NULL;
        AVLULInsert(&pha->pcoreFreeSize, &pmb->u2.coreFree);
    }
    KLOGEXITVOID();
}


/**
 * Inserts a memblock into the free chain.
 * Merges blocks adjacent blocks.
 * @param    pha  Pointer to the heap anchor which the block is to be inserted into.
 * @param    pmb  Pointer to memblock to insert into the free list.
 * @remark   Sorts on address.
 */
static void swpInsertFree(PHEAPANCHOR pha, PMEMBLOCK pmb)
{
    KLOGENTRY2("void","PHEAPANCHOR pha, PMEMBLOCK pmb", pha, pmb);
    PMEMBLOCK   pmbRight;
    PMEMBLOCK   pmbRightParent;
    PMEMBLOCK   pmbLeft;

    pha->cbFree += pmb->u2.cbSize;

    /*
     * Get both right and left to determin which case we have here.
     * Four cases are possible:
     *    1 - insert no merge.
     *    2 - insert left merge.
     *    3 - insert right merge.
     *    4 - insert both left and right merge.
     */
    pmbRight = (PMEMBLOCK)AVLULGetWithParent(&pha->pcoreFree,
                                           (PPAVLULNODECORE)SSToDS(&pmbRightParent),
                                           PNEXT_BLOCK(pmb));
    pmbLeft = (PMEMBLOCK)AVLULGetBestFit(&pha->pcoreFree, pmb->u1.core.Key, FALSE);
    if (pmbLeft != NULL && PNEXT_BLOCK(pmbLeft) != pmb->u1.core.Key)
        pmbLeft = NULL;

    if (pmbLeft == NULL && pmbRight == NULL)
    {   /* 1 - insert no merge */
        AVLULInsert(&pha->pcoreFree, &pmb->u1.core);
        swpInsertIntoFreeSize(pha, pmb);
    }
    else if (pmbLeft != NULL && pmbRight == NULL)
    {   /* 2 - insert left merge: just add pmb to pmbLeft. */
        swpRemoveFromFreeSize(pha, pmbLeft);
        pmbLeft->u2.cbSize += pmb->u2.cbSize;
        swpInsertIntoFreeSize(pha, pmbLeft);
        swpReleaseMemblock(pmb);
    }
    else if (pmbLeft == NULL && pmbRight != NULL)
    {   /* 3 - insert right merge: replace pmbRight with pmb in the pmbFree tree; */
        swpRemoveFromFreeSize(pha, pmbRight);
        pmb->u1.core.uchHeight = pmbRight->u1.core.uchHeight;
        pmb->u1.core.pLeft = pmbRight->u1.core.pLeft;
        pmb->u1.core.pRight = pmbRight->u1.core.pRight;
        pmb->u2.cbSize += pmbRight->u2.cbSize;
        swpInsertIntoFreeSize(pha, pmb);
        if (pmbRightParent != NULL)
        {
            if (pmb->u1.core.Key < pmbRightParent->u1.core.Key)
                pmbRightParent->u1.core.pLeft = &pmb->u1.core;
            else
                pmbRightParent->u1.core.pRight = &pmb->u1.core;
        }
        else
            pha->pcoreFree = &pmb->u1.core;
        swpReleaseMemblock(pmbRight);
    }
    else
    {   /* 4 -insert both left and right merge */
        if (AVLULRemove(&pha->pcoreFree, pmbRight->u1.core.Key) != (PAVLULNODECORE)pmbRight)
        {
            kprintf(("AVLULRemove on pmbRight failed.\n"));
            KLOGEXITVOID();
            return;
        }
        swpRemoveFromFreeSize(pha, pmbLeft);
        swpRemoveFromFreeSize(pha, pmbRight);
        pmbLeft->u2.cbSize += pmb->u2.cbSize + pmbRight->u2.cbSize;
        swpInsertIntoFreeSize(pha, pmbLeft);
        swpReleaseMemblock(pmb);
        swpReleaseMemblock(pmbRight);
    }
    KLOGEXITVOID();
}


/**
 * Finds a free block at the requested size.
 * @returns  Pointer to block (not in free list any longer).
 * @param    ppha        Upon return the pointer pointed to contains the heap
 *                       anchor which the memory block was allocated from.
 * @param    cbUserSize  Bytes the user have requested.
 * @sketch   cbUserSize is aligned to nearest 4 bytes.
 *           ...
 *
 */
static PMEMBLOCK swpGetFreeMemblock(PHEAPANCHOR *ppha, unsigned cbUserSize)
{
    KLOGENTRY2("PMEMBLOCK","PHEAPANCHOR * ppha, unsigned cbUserSize", ppha, cbUserSize);
    unsigned cbBlockSize;
    unsigned cbTotalSize = 0;

    cbUserSize = KALIGNUP(ALIGNMENT, cbUserSize) + CB_SIGNATURES;

    *ppha = phaFirst;
    while (*ppha != NULL)
    {
        if ((*ppha)->cbFree >= cbUserSize)
        {
            PMEMBLOCK pmb;

            pmb = (PMEMBLOCK)AVLULGetBestFit(&(*ppha)->pcoreFreeSize, cbUserSize, TRUE);
            if (pmb != NULL)
            {
                pmb = MEMBLOCK_FROM_FREESIZENODE(pmb);
                swpRemoveFromFreeSize(*ppha, pmb);
                (*ppha)->cbFree -= pmb->u2.cbSize;

                /* pmb is now the block which we are to return, but do we have to split it? */
                if (pmb->u2.cbSize > cbUserSize + CB_SIGNATURES)
                {
                    PMEMBLOCK pmbTmp = pmb;
                    pmb = swpAllocateMemblock();
                    if (pmb == NULL)
                    {
                        kprintf(("swpAllocateMemblock failed.\n"));
                        swpInsertIntoFreeSize(*ppha, pmbTmp);
                        (*ppha)->cbFree += pmbTmp->u2.cbSize;
                        KLOGEXIT(NULL);
                        return NULL;
                    }
                    pmb->u1.uData = pmbTmp->u1.uData + pmbTmp->u2.cbSize - cbUserSize;
                    pmbTmp->u2.cbSize -= cbUserSize;
                    pmb->u2.cbSize = cbUserSize;
                    (*ppha)->cbFree += pmbTmp->u2.cbSize;
                    #ifdef DEBUG_ALLOC
                        pmb->u1.puData[-1] = SIGNATURE_END;
                        pmb->u1.puData[0] = SIGNATURE_START;
                    #endif
                    swpInsertIntoFreeSize(*ppha, pmbTmp);
                }
                else
                {
                    PMEMBLOCK pmbTmp = (PMEMBLOCK)AVLULRemove(&(*ppha)->pcoreFree, pmb->u1.core.Key);
                    if (pmbTmp != pmb)
                    {
                        kprintf(("Internal heap error - failed to Remove best fit block!\n"));
                        KLOGEXIT(NULL);
                        return NULL;
                    }
                }
                KLOGEXIT(pmb);
                return pmb;
            }
        }

        cbTotalSize += (*ppha)->cbSize;

        /* next heap anchor */
        *ppha = (*ppha)->pNext;
    }


    /*
     * Add a new heap anchor?
     */
    cbBlockSize = KALIGNUP(BLOCKSIZE, cbUserSize + CB_SIGNATURES + 1);
    if (cbSwpHeapMax >= cbTotalSize + cbBlockSize)
    {
        /* allocate new heapanchor */
        *ppha = rmalloc(sizeof(**ppha));
        if (ppha != NULL)
        {
            PMEMBLOCK pmbFree = swpAllocateMemblock();
            PMEMBLOCK pmbUsed = swpAllocateMemblock();
            if (pmbFree != NULL && pmbUsed != NULL)
            {
                register PHEAPANCHOR pha = *ppha;
                memset(pha, 0, sizeof(*pha));
                #ifdef RING0
                    pha->pvBlock = D32Hlp_VMAlloc(VMDHA_SWAP, cbBlockSize, ~0UL);
                #else
                    if (DosAllocMem(&pha->pvBlock, cbBlockSize, PAG_COMMIT | PAG_READ | PAG_WRITE) != 0)
                        pha->pvBlock = NULL;
                #endif
                if (pha->pvBlock != NULL)
                {
                    /* anchor block */
                    #ifdef DEBUG_ALLOC
                        pha->ulSignature = HEAPANCHOR_SIGNATURE;
                    #endif
                    pha->cbSize = cbBlockSize;

                    /* free memblock */
                    pmbFree->u1.uData = (unsigned)pha->pvBlock;
                    pmbFree->u2.cbSize = cbBlockSize - cbUserSize;
                    #ifdef DEBUG_ALLOC
                        pmbFree->u1.puData[0] = SIGNATURE_START;
                        pmbFree->u1.puData[(pmbFree->u2.cbSize / sizeof(unsigned)) - 1] = SIGNATURE_END;
                    #endif
                    swpInsertFree(pha, pmbFree);

                    /* used memblock */
                    pmbUsed->u1.uData = pmbFree->u1.uData + pmbFree->u2.cbSize;
                    pmbUsed->u2.cbSize = cbUserSize;
                    #ifdef DEBUG_ALLOC
                        pmbUsed->u1.puData[0] = SIGNATURE_START;
                        pmbUsed->u1.puData[(pmbUsed->u2.cbSize / sizeof(unsigned)) - 1] = SIGNATURE_END;
                    #endif

                    /* insert into list */
                    pha->pPrev = phaLast;
                    pha->pNext = NULL;
                    if (phaLast == NULL) /* Might never happen but just in case it does. */
                        phaLast = phaFirst = pha;
                    else
                        phaLast->pNext = pha;
                    phaLast = pha;

                    KLOGEXIT(pmbUsed);
                    return pmbUsed;
                }
                else
                    kprintf(("unable to allocate a new heap block.\n"));
            }
            else
                kprintf(("swpAllocateMemblock failed.\n"));
            swpReleaseMemblock(pmbFree);
            swpReleaseMemblock(pmbUsed);
            rfree(*ppha);
        }
        else
            kprintf(("rmalloc failed for when allocating a new heap anchor.\n"));

        KLOGEXIT(NULL);
        return NULL;
    }
    else
    {
        kprintf(("Allocation failed, limit reached. \n"
                 "    %d(=cbResHeapMax) < %d(=cbTotalSize) + %d(=cbBlockSize)\n",
                  cbSwpHeapMax, cbTotalSize, cbBlockSize));
    }

    KLOGEXIT(NULL);
    return NULL;
}


/**
 * Finds a used memory block.
 * @returns   Pointer to memblock if found.
 * @param     ppha       Pointer to pointer to heap anchor block the returned memblock is located in. (output)
 *                       NULL is allowed.
 * @param     pvUser     User pointer to find the block to.
 */
static PMEMBLOCK swpFindUsedBlock(PHEAPANCHOR *ppha, void *pvUser)
{
    KLOGENTRY2("PMEMBLOCK","PHEAPANCHOR * ppha, void * pvUser", ppha, pvUser);
    if (pvUser != NULL)
    {
        register PHEAPANCHOR pha = phaFirst;
        while (pha != NULL && !(pvUser > pha->pvBlock && (unsigned)pvUser < (unsigned)pha->pvBlock + pha->cbSize))
            pha = pha->pNext;

        if (ppha != NULL)
            *ppha = pha;
        if (pha != NULL)
        {
            register PMEMBLOCK pmb;
            #ifdef DEBUG_ALLOC
                if (pha->ulSignature != HEAPANCHOR_SIGNATURE)
                {
                    kprintf(("Invalid heapanchor signature.\n"));
                    KLOGEXIT(NULL);
                    return NULL;
                }
            #endif
            pmb = (PMEMBLOCK)AVLULGet(&pha->pcoreUsed, (AVLULKEY)((unsigned)pvUser - CB_SIGNATURE_START));
            #ifdef DEBUG_ALLOC
                if (pmb != NULL && pmb->u1.puData[0] != SIGNATURE_START)
                {
                    kprintf(("Invalid memblock start signature.\n"));
                    pmb = NULL;
                }
                if (pmb != NULL && pmb->u1.puData[(pmb->u2.cbSize/sizeof(unsigned)) - 1] != SIGNATURE_END)
                {
                    kprintf(("Invalid memblock end signature.\n"));
                    pmb = NULL;
                }
            #endif
            KLOGEXIT(pmb);
            return pmb;
        }
    }

    KLOGEXIT(NULL);
    return NULL;
}


#if 0
/**
 * Finds a used memory block from a pointer into the userdata.
 * @returns   Pointer to memblock if found.
 * @param     ppha       Pointer to pointer to heap anchor block the returned memblock is located in. (output)
 *                       NULL is allowed.
 * @param     pvUser     User pointer to find the block to.
 */
static PMEMBLOCK swpFindWithinUsedBlock(PHEAPANCHOR *ppha, void *pvUser)
{
    register PHEAPANCHOR pha = phaFirst;

    if (pvUser == NULL)
    {
        KLOGEXIT(NULL);
        return NULL;
    }

    while (pha != NULL && !(pvUser > pha->pvBlock && (unsigned)pvUser < (unsigned)pha->pvBlock + pha->cbSize))
        pha = pha->pNext;

    if (ppha != NULL)
        *ppha = pha;
    if (pha != NULL)
    {
        PMEMBLOCK pmb;

        #ifdef DEBUG_ALLOC
            if (pha->ulSignature != HEAPANCHOR_SIGNATURE)
            {
                kprintf(("Invalid heapanchor signature.\n"));
                KLOGEXIT(NULL);
                return NULL;
            }
        #endif


        pmb = (PMEMBLOCK)AVLULGetBestFit(&pha->pcoreUsed, (AVLULKEY)pvUser - CB_SIGNATURE_START, TRUE);
        if (pmb != NULL
            && pmb->u1.uData + pmb->u2.cbSize - CB_SIGNATURE_END > (unsigned)pvUser
            && pmb->u1.uData + CB_SIGNATURE_START <= (unsigned)pvUser
            )
        {
            #ifdef DEBUG_ALLOC
                if (pmb != NULL && pmb->u1.puData[0] != SIGNATURE_START)
                {
                    kprintf(("Invalid memblock start signature.\n"));
                    pmb = NULL;
                }
                if (pmb != NULL && pmb->u1.puData[(pmb->u2.cbSize/sizeof(unsigned)) - 1] != SIGNATURE_END)
                {
                    kprintf(("Invalid memblock end signature.\n"));
                    pmb = NULL;
                }
            #endif
            KLOGEXIT(pmb);
            return pmb;
        }
    }

    KLOGEXIT(NULL);
    return NULL;
}
#endif


/**
 * Initiate the heap "subsystem".
 * @returns   0 on success, not 0 on error.
 * @param     cbSizeInit  The initial size of the heap in bytes.
 * @param     cbSizeMax   Maximum heapsize in bytes.
 */
int swpHeapInit(unsigned cbSizeInit, unsigned cbSizeMax)
{
    KLOGENTRY2("int","unsigned cbSizeInit, unsigned cbSizeMax", cbSizeInit, cbSizeMax);
    PMEMBLOCK pmbFree;
    cbSwpHeapMax = cbSizeMax;
    pmcFirst = NULL;
    pmbFree = swpAllocateMemblock();
    if (pmbFree != NULL)
    {
        phaFirst = rmalloc(sizeof(*phaFirst));
        if (phaFirst != NULL)
        {
            unsigned  cbSize = KMAX(BLOCKSIZE, cbSizeInit);
            memset(phaFirst, 0, sizeof(*phaFirst));
            #ifdef RING0
            phaFirst->pvBlock = D32Hlp_VMAlloc(VMDHA_SWAP, cbSize, ~0UL);
            #else
            if (DosAllocMem(&phaFirst->pvBlock, cbSize, PAG_COMMIT | PAG_READ | PAG_WRITE) != 0)
                phaFirst->pvBlock = NULL;
            #endif
            if (phaFirst->pvBlock != NULL)
            {
                /* anchor block */
                #ifdef DEBUG_ALLOC
                phaFirst->ulSignature = HEAPANCHOR_SIGNATURE;
                #endif
                phaFirst->cbSize = cbSize;

                /* free memblock */
                pmbFree->u1.uData = (unsigned)phaFirst->pvBlock;
                pmbFree->u2.cbSize = cbSize;
                #ifdef DEBUG_ALLOC
                pmbFree->u1.puData[0] = SIGNATURE_START;
                pmbFree->u1.puData[(pmbFree->u2.cbSize / sizeof(unsigned)) - 1] = SIGNATURE_END;
                #endif
                swpInsertFree(phaFirst, pmbFree);

                /* insert into list */
                phaFirst->pPrev = phaFirst->pNext = NULL;
                phaLast = phaFirst;

                #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
                    if (!_swp_heap_check())
                    {   /* error! */
                        kprintf(("_swp_heap_check failed!\n"));
                        #ifdef DEBUG
                            INT3();
                        #endif
                        KLOGEXIT(-2);
                        return -2;
                    }
                #endif
                #ifndef RING0
                    fSwpInited = TRUE;
                #endif

                KLOGEXIT(0);
                return 0;
            }
            else
                kprintf(("failed to allocate heap block\n"));
            rfree(phaFirst);
            phaFirst = NULL;
        }
        else
        {
            kprintf(("rmalloc failed to allocate a heap anchor.\n"));
            #ifdef DEBUG
                INT3();
            #endif
        }
        swpReleaseMemblock(pmbFree);
    }
    else
        kprintf(("swpAllocateMemblock failed.\n"));
    KLOGEXIT(-1);
    return -1;
}


/**
 * malloc - allocates a given amount of memory.
 * @returns  Pointer to allocated memory.
 *           NULL if out of memory. (Or memory to fragmented.)
 * @param    cbSize  Bytes user requests us to allocate. This is aligned
 *                   to four bytes.
 */
void * smalloc(unsigned cbSize)
{
    KLOGENTRY1("void *","unsigned cbSize", cbSize);
    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_swp_heap_check())
    {
        kprintf(("_swp_heap_check failed!\n"));
        KLOGEXIT(NULL);
        return NULL;
    }
    #endif

    if (cbSize != 0)
    {
        PHEAPANCHOR pha;
        PMEMBLOCK   pmb = swpGetFreeMemblock(SSToDS(&pha), cbSize);
        if (pmb != NULL)
        {
            swpInsertUsed(pha, pmb);
            KLOGEXIT((void*)(pmb->u1.uData + CB_SIGNATURE_START));
            return (void*)(pmb->u1.uData + CB_SIGNATURE_START);
        }
    }
    else
        kprintf(("error cbSize = 0\n"));

    KLOGEXIT(NULL);
    return NULL;
}


/**
 * Reallocate a heapblock.
 * @returns   Pointer to new heapblock.
 * @param     pv     Pointer to the block to realloc.
 *                   If pv is NULL, this call is equal to malloc.
 * @param     cbNew  The new block size.
 */
void *srealloc(void *pv, unsigned cbNew)
{
    KLOGENTRY2("void *","void * pv, unsigned cbNew", pv, cbNew);
    PMEMBLOCK pmb;
    PHEAPANCHOR pha;

    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_swp_heap_check())
    {
        kprintf(("_swp_heap_check failed!\n"));
        KLOGEXIT(NULL);
        return NULL;
    }
    #endif
    pmb = swpFindUsedBlock(SSToDS(&pha), pv);
    if (pmb != NULL)
    {
        void *pvRet;

        cbNew = KALIGNUP(ALIGNMENT, cbNew) + CB_SIGNATURES;
        if (cbNew <= pmb->u2.cbSize)
        {   /* shrink block */
            pvRet = pv;
            if (cbNew + CB_SIGNATURES < pmb->u2.cbSize)
            {   /* split block */
                PMEMBLOCK pmbNew = swpAllocateMemblock();
                if (pmbNew != NULL)
                {
                    pmbNew->u1.uData = pmb->u1.uData + cbNew;
                    pmbNew->u2.cbSize = pmb->u2.cbSize - cbNew;
                    pha->cbUsed -= pmb->u2.cbSize - cbNew;
                    pmb->u2.cbSize = cbNew;
                    #ifdef DEBUG_ALLOC
                    pmbNew->u1.puData[-1] = SIGNATURE_END;
                    pmbNew->u1.puData[0] = SIGNATURE_START;
                    pmbNew->u1.puData[(pmbNew->u2.cbSize / sizeof(unsigned)) - 1] = SIGNATURE_END;
                    #endif
                    swpInsertFree(pha, pmbNew);
                    #ifdef ALLWAYS_HEAPCHECK
                    if (!_swp_heap_check())
                    {
                        kprintf(("_swp_heap_check failed!\n"));
                        KLOGEXIT(NULL);
                        return NULL;
                    }
                    #endif
                }
                else
                    kprintf(("swpAllocateMemblock failed\n"));
            }
        }
        else
        {   /* expand block - this code may be optimized... */
            #if 0
            pvRet = smalloc(cbNew);
            if (pvRet != NULL)
            {
                memcpy(pvRet, pv, pmb->u2.cbSize);
                sfree(pv);
            }
            #else
            /* optimized FIXME! */
            PMEMBLOCK pmbRight = (PMEMBLOCK)AVLULGet(&pha->pcoreFree, PNEXT_BLOCK(pmb));
            if (pmbRight != NULL && pmbRight->u2.cbSize + pmb->u2.cbSize >= cbNew)
            {
                pvRet = pv;
                /* split the free block? */
                if (pmbRight->u2.cbSize + pmb->u2.cbSize - CB_SIGNATURES > cbNew)
                {
                    pha->cbFree -= pmbRight->u2.cbSize;
                    swpRemoveFromFreeSize(pha, pmbRight);
                    pmbRight->u1.uData = pmb->u1.uData + cbNew;
                    pmbRight->u2.cbSize = pmbRight->u2.cbSize + pmb->u2.cbSize - cbNew;
                    #ifdef DEBUG_ALLOC
                    pmbRight->u1.puData[-1] = SIGNATURE_END;
                    pmbRight->u1.puData[0] = SIGNATURE_START;
                    pmbRight->u1.puData[(pmbRight->u2.cbSize / sizeof(unsigned)) - 1] = SIGNATURE_END;
                    #endif
                    swpInsertIntoFreeSize(pha, pmbRight);
                    pha->cbUsed += cbNew - pmb->u2.cbSize;
                    pmb->u2.cbSize = cbNew;
                    pha->cbFree += pmbRight->u2.cbSize;
                }
                else
                {
                    if (AVLULRemove(&pha->pcoreFree, pmbRight->u1.core.Key) != &pmbRight->u1.core)
                    {
                        kprintf(("AVLULRemove failed for pmbRight - hmm!\n"));
                        KLOGEXIT(NULL);
                        return NULL;
                    }
                    swpRemoveFromFreeSize(pha, pmbRight);
                    pmb->u2.cbSize += pmbRight->u2.cbSize;
                    pha->cbFree -= pmbRight->u2.cbSize;
                    pha->cbUsed += pmbRight->u2.cbSize;
                    swpReleaseMemblock(pmbRight);
                }

            }
            else
            {   /* worst case: allocate a new block, copy data and free the old block. */
                pvRet = smalloc(cbNew);
                if (pvRet != NULL)
                {
                    memcpy(pvRet, pv, pmb->u2.cbSize-CB_SIGNATURES);
                    sfree(pv);
                }
            }
            #endif
        }
        KLOGEXIT(pvRet);
        return pvRet;
    }
    else
    {
        if (pv == NULL)
        {
            pv = smalloc(cbNew);
            KLOGEXIT(pv);
            return pv;
        }
        kprintf(("invalid user pointer, pv=0x%08x\n", pv));
    }
    KLOGEXIT(NULL);
    return NULL;
}


/**
 * Frees a block.
 * @param    pv  User pointer.
 */
void sfree(void *pv)
{
    KLOGENTRY1("void","void * pv", pv);
    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_swp_heap_check())
    {
        kprintf(("_swp_heap_check failed!\n"));
        KLOGEXITVOID();
        return;
    }
    #endif

    if (pv != NULL)
    {
        PHEAPANCHOR pha = phaFirst;

        while (pha != NULL && !(pv > pha->pvBlock
                                && (unsigned)pv < (unsigned)pha->pvBlock + pha->cbSize))
            pha = pha->pNext;

        if (pha != NULL)
        {
            PMEMBLOCK pmb = (PMEMBLOCK)AVLULRemove(&pha->pcoreUsed, ((AVLULKEY)pv - CB_SIGNATURE_START));
            if (pmb != NULL)
            {
                pha->cbUsed -= pmb->u2.cbSize;
                swpInsertFree(pha, pmb);
            }
            else
                kprintf(("heap block not found!\n"));
        }
        else
            kprintf(("heap block not within the large blocks!\n"));
    }
    else
        kprintf(("Free received a NULL pointer!\n"));
    KLOGEXITVOID();
}


/**
 * Gets the size of a block.
 * @returns  Bytes in a block.
 */
unsigned _swp_msize(void *pv)
{
    KLOGENTRY1("unsigned","void * pv", pv);
    PHEAPANCHOR pha;
    PMEMBLOCK   pmb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_swp_heap_check())
        kprintf(("_swp_heap_check failed!\n"));
    #endif

    pmb = swpFindUsedBlock(SSToDS(&pha), pv);
    KLOGEXIT(pmb != NULL ? pmb->u2.cbSize - CB_SIGNATURES : 0);
    return pmb != NULL ? pmb->u2.cbSize - CB_SIGNATURES : 0;
}


/**
 * Get amount of free memory (in bytes)
 * @returns  Amount of free memory (in bytes).
 * @remark   Note that this amount is of all free memory blocks and
 *           that these blocks are fragmented.
 *           You'll probably not be able to allocate a single block
 *           of the returned size.
 */
unsigned _swp_memfree(void)
{
    KLOGENTRY0("unsigned");
    PHEAPANCHOR pha = phaFirst;
    unsigned    cb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_swp_heap_check())
        kprintf(("_swp_heap_check failed!\n"));
    #endif

    for (cb = 0; pha != NULL; pha = pha->pNext)
        cb += pha->cbFree;

    KLOGEXIT(cb);
    return cb;
}


/**
 * Get amount of used memory (in bytes)
 * @returns  Amount of used memory (in bytes).
 */
unsigned    _swp_memused(void)
{
    KLOGENTRY0("unsigned");
    PHEAPANCHOR pha = phaFirst;
    unsigned    cb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_swp_heap_check())
        kprintf(("_swp_heap_check failed!\n"));
    #endif

    for (cb = 0; pha != NULL; pha = pha->pNext)
        cb += pha->cbUsed;

    KLOGEXIT(cb);
    return cb;
}


/**
 * Collects the heap state.
 * @returns     0 on success.
 *              -1 on error.
 * @param       pState  Pointer to a HEAPSTATE structure which is
 *                      filled upon successful return.
 */
int         _swp_state(PHEAPSTATE pState)
{
    KLOGENTRY1("int","PHEAPSTATE pState", pState);
    PHEAPANCHOR pha;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
    {
        kprintf(("_res_heap_check failed!\n"));
        KLOGEXIT(-1);
        return -1;
    }
    #endif

    if (pState == NULL)
    {
        KLOGEXIT(-1);
        return -1;
    }

    /*
     * Loop thru all the anchor blocks and all memory blocks
     * building the stats.
     */
    memset(pState, 0, sizeof(HEAPSTATE));
    for (pha = phaFirst; pha != NULL; pha = pha->pNext)
    {
        AVLULENUMDATA   EnumData;
        PAVLULENUMDATA  pEnumData = SSToDS(&EnumData);
        PMEMBLOCK       pmb;

        /* count of free blocks */
        pmb = (PMEMBLOCK)AVLULBeginEnumTree((PPAVLULNODECORE)&pha->pcoreFree, pEnumData, TRUE);
        while (pmb)
        {
            pState->cBlocksFree++;
            pmb = (PMEMBLOCK)AVLULGetNextNode(pEnumData);
        }

        /* count of used blocks */
        pmb = (PMEMBLOCK)AVLULBeginEnumTree((PPAVLULNODECORE)&pha->pcoreUsed, pEnumData, TRUE);
        while (pmb)
        {
            pState->cBlocksUsed++;
            pmb = (PMEMBLOCK)AVLULGetNextNode(pEnumData);
        }

        /* sizes */
        pState->cbHeapSize  += pha->cbSize;
        pState->cbHeapFree  += pha->cbFree;
        pState->cbHeapUsed  += pha->cbUsed;
    }

    KLOGEXIT(0);
    return 0;
}


/**
 * Checks heap integrety.
 * @returns  TRUE  when ok.
 *           FALSE on error.
 *           NULL if out of memory. (Or memory to fragmented.)
 */
int _swp_heap_check(void)
{
    KLOGENTRY0("int");
#ifdef DEBUG_ALLOC
    PHEAPANCHOR pha = phaFirst;
    PHEAPANCHOR phaPrev = NULL;


    while (pha != NULL)
    {
        AVLULENUMDATA FreeEnumData;
        AVLULENUMDATA UsedEnumData;
        PMEMBLOCK pmbFree = (PMEMBLOCK)AVLULBeginEnumTree(&pha->pcoreFree,
                                                        (PAVLULENUMDATA)SSToDS(&FreeEnumData), TRUE);
        PMEMBLOCK pmbFreeNext = (PMEMBLOCK)AVLULGetNextNode((PAVLULENUMDATA)SSToDS(&FreeEnumData));
        PMEMBLOCK pmbUsed = (PMEMBLOCK)AVLULBeginEnumTree(&pha->pcoreUsed,
                                                        (PAVLULENUMDATA)SSToDS(&UsedEnumData), TRUE);
        PMEMBLOCK pmbUsedNext = (PMEMBLOCK)AVLULGetNextNode((PAVLULENUMDATA)SSToDS(&UsedEnumData));
        PMEMBLOCK pmbLast = NULL;
        unsigned  cbFree = 0;
        unsigned  cbUsed = 0;
        unsigned  cbSize = 0;

        /*
         * Check the heap anchor.
         */
        if (pha->ulSignature != HEAPANCHOR_SIGNATURE)
        {
            kprintf(("invalid signature for pha=0x%08x\n", pha));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }

        if (pha->cbFree + pha->cbUsed != pha->cbSize || pha->cbSize == 0)
        {
            kprintf(("cbFree + cbUsed >= cbSize for pha=0x%08x\n", pha));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }

        if (pha->pPrev != phaPrev)
        {
            kprintf(("error in heap anchor chain.\n"));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }

        if ((void*)KMINNOTNULL(PDATA(pmbFree), PDATA(pmbUsed)) != pha->pvBlock)
        {
            kprintf(("The first free/used block don't start at start of memory block.\n"
                     "    PDATA(pmbFree)=0x%08x, PDATA(pmbUsed)=0x%08x, pha->pvBlock=0x%08x\n",
                     PDATA(pmbFree), PDATA(pmbUsed), pha->pvBlock));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }

        /*
         * Check the tree AVL-trees - !extra debug test!
         */
        if (swpCheckAVLTree((PMEMBLOCK)pha->pcoreUsed) != 0)
        {
            kprintf(("the Used tree is invalid.\n"));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }
        if (swpCheckAVLTree((PMEMBLOCK)pha->pcoreFree) != 0)
        {
            kprintf(("the Free tree is invalid.\n"));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }
        if (swpCheckAVLTreeFree(pha->pcoreFreeSize) != 0)
        {
            kprintf(("the free-size tree is invalid.\n"));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }

        /*
         * Check the lists
         */
        while (pmbFree != NULL || pmbUsed != NULL)
        {
            /** @sketch:
             * Check signatures and for lost memory.
             *
             * three cases:
             *  1) pmbUsed adjacent to pmbUsed->pNext
             *  2) pmbUsed adjacent to pmbFree
             *  3) pmbFree adjacent to pmbFree->pNext
             *  4) pmbFree adjacent to pmbUsed
             *  5) pmbUsed is the last block
             *  6) pmbFree is the last block
             */
            if (!( (pmbUsed != NULL && PNEXT_BLOCK(pmbUsed) == (AVLULKEY)PDATA(pmbUsedNext))  /* 1.*/
                || (pmbUsed != NULL && PNEXT_BLOCK(pmbUsed) == (AVLULKEY)PDATA(pmbFree))      /* 2.*/
                || (pmbFree != NULL && PNEXT_BLOCK(pmbFree) == (AVLULKEY)PDATA(pmbFreeNext))  /* 3.*/
                || (pmbFree != NULL && PNEXT_BLOCK(pmbFree) == (AVLULKEY)PDATA(pmbUsed))      /* 4.*/
                || (pmbUsed != NULL && pmbFree == NULL && pmbUsedNext == NULL)              /* 5.*/
                || (pmbFree != NULL && pmbUsed == NULL && pmbFreeNext == NULL)              /* 6.*/
                 )
               )
                {
                /* error hole */
                kprintf(("internal error - memory hole!\n"));
                INT3();
                KLOGEXIT(FALSE);
                return FALSE;
                }

            /* check signatures and advance to the next block */
            if (pmbUsed != NULL && (pmbFree == NULL || pmbUsed->u1.core.Key < pmbFree->u1.core.Key))
            {
                cbSize += pmbUsed->u2.cbSize;
                cbUsed += pmbUsed->u2.cbSize;
                if (pmbUsed->u1.puData[0] != SIGNATURE_START)
                {
                    kprintf(("invalid start signature on used block, pv=0x%08x\n", pmbUsed->u1.pvData));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }
                if (pmbUsed->u1.puData[(pmbUsed->u2.cbSize / sizeof(unsigned)) - 1] != SIGNATURE_END)
                {
                    kprintf(("invalid end signature on used block, pv=0x%08x\n", pmbUsed->u1.pvData));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }
                pmbLast = pmbUsed;
                pmbUsed = pmbUsedNext;
                pmbUsedNext = (PMEMBLOCK)AVLULGetNextNode((PAVLULENUMDATA)SSToDS(&UsedEnumData));
            }
            else
            {
                PMEMBLOCK pmb = (PMEMBLOCK)AVLULGet(&pha->pcoreFreeSize, pmbFree->u2.cbSize);
                if (pmb != NULL)
                {
                    pmb = MEMBLOCK_FROM_FREESIZENODE(pmb);
                    while (pmb != NULL && pmb != pmbFree)
                        pmb = pmb->pmbNext;
                    if (pmb == NULL)
                    {
                        kprintf(("pmbFree=0x%08 is not found in the free-size-tree.\n", pmbFree));
                        INT3();
                        KLOGEXIT(FALSE);
                        return FALSE;
                    }
                }
                else
                {
                    kprintf(("pmbFree=0x%08 is not found in the free-size-tree.\n", pmbFree));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }

                cbSize += pmbFree->u2.cbSize;
                cbFree += pmbFree->u2.cbSize;
                if (pmbFree->u1.puData[0] != SIGNATURE_START)
                {
                    kprintf(("invalid start signature on free block, pv=0x%08x\n", pmbUsed->u1.pvData));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }
                if (pmbFree->u1.puData[(pmbFree->u2.cbSize / sizeof(unsigned)) - 1] != SIGNATURE_END)
                {
                    kprintf(("invalid end signature on free block, pv=0x%08x\n", pmbUsed->u1.pvData));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }
                pmbLast = pmbFree;
                pmbFree = pmbFreeNext;
                pmbFreeNext = (PMEMBLOCK)AVLULGetNextNode((PAVLULENUMDATA)SSToDS(&FreeEnumData));
            }
        }


        /*
         * Check the cbFree and cbUsed.
         */
        if (cbFree != pha->cbFree)
        {
            kprintf(("cbFree(%d) != pha->cbFree(%d)\n", cbFree, pha->cbFree));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }

        if (cbUsed != pha->cbUsed)
        {
            kprintf(("cbUsed(%d) != pha->cbUsed(%d)\n", cbUsed, pha->cbUsed));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }

        if (cbSize != pha->cbSize)
        {
            kprintf(("cbTotal(%d) != pha->cbSize(%d)\n", cbSize, pha->cbSize));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }
        /*
         * Check right most node.
         */
        if (pmbLast == NULL || (unsigned)pha->pvBlock + pha->cbSize != (unsigned)PNEXT_BLOCK(pmbLast))
        {
            kprintf(("The last free/used block dont end at the end of memory block.\n"
                     "    pmbLast(end)=0x%08x, pha->pvBlock+pha->cbSize=0x%08x\n",
                     pmbLast != NULL ? PNEXT_BLOCK(pmbLast) : 0, (unsigned)pha->pvBlock + pha->cbSize));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }


        /*
         * Next heap anchor
         */
        phaPrev = pha;
        pha = pha->pNext;
    }

    /* check that end of chain is phaLast */
    if (phaPrev != phaLast)
    {
        kprintf(("internal error - heap anchor chain didn't end on phaLast\n"));
        INT3();
        KLOGEXIT(FALSE);
        return FALSE;
    }

#endif

    KLOGEXIT(TRUE);
    return TRUE;
}

#ifdef DEBUG_ALLOC
/**
 * Check the integrity of the a Free/Used AVL tree where Key == node pointer.
 */
static int swpCheckAVLTree(PMEMBLOCK pmb)
{
    KLOGENTRY1("int","PMEMBLOCK pmb", pmb);
    if (pmb == NULL)
    {
        KLOGEXIT(0);
        return 0;
    }
    if (pmb->u1.core.Key < 0x10000)
    {
        kprintf(("Invalid Key!\n"));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->u1.uData % ALIGNMENT != 0)
    {
        kprintf(("Data is not correctly aligned, uData=0x%08x\n", pmb->u1.uData));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->u1.core.pLeft != NULL && (pmb->u1.core.pLeft < (PAVLULNODECORE)0x10000
                                       || pmb->u1.core.Key <= (AVLULKEY)pmb->u1.core.pLeft->Key))
    {
        kprintf(("Invalid pLeft!\n"));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->u1.core.pRight != NULL && (pmb->u1.core.pRight < (PAVLULNODECORE)0x10000
                                        || pmb->u1.core.Key >= (AVLULKEY)pmb->u1.core.pRight->Key))
    {
        kprintf(("Invalid pRight!\n"));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->u1.core.pLeft != NULL && swpCheckAVLTree((PMEMBLOCK)pmb->u1.core.pLeft) != 0)
    {
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->u1.core.pRight != NULL && swpCheckAVLTree((PMEMBLOCK)pmb->u1.core.pRight) != 0)
    {
        KLOGEXIT(-1);
        return -1;
    }
    KLOGEXIT(0);
    return 0;
}


/**
 * Check the integrity of the a Free/Used AVL tree where Key == node pointer.
 */
static int swpCheckAVLTreeFree(PAVLULNODECORE pNode)
{
    KLOGENTRY1("int","PAVLULNODECORE pNode", pNode);
    PMEMBLOCK pmb;

    if (pNode == NULL)
    {
        KLOGEXIT(0);
        return 0;
    }
    if (pNode->Key < 4 || (pNode->Key % ALIGNMENT) != 0)
    {
        kprintf(("Invalid Key! 0x%08x\n", pNode->Key));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pNode->pLeft != NULL && (pNode->pLeft < (PAVLULNODECORE)0x10000 || pNode->Key <= pNode->pLeft->Key))
    {
        kprintf(("Invalid pLeft!\n"));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pNode->pRight != NULL && (pNode->pRight < (PAVLULNODECORE)0x10000 || pNode->Key >= pNode->pRight->Key))
    {
        kprintf(("Invalid pRight!\n"));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    pmb = MEMBLOCK_FROM_FREESIZENODE(pNode);
    if (pmb->pmbNext != NULL && pmb->pmbNext < (PMEMBLOCK)0x10000)
    {
        kprintf(("invalid pmbNext pointer, pmbNext=0x%08x\n", pmb->pmbNext));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    while (pmb->pmbNext != NULL)
    {
        if (pmb->pmbNext < (PMEMBLOCK)0x10000)
        {
            kprintf(("invalid pmbNext pointer, pmbNext=0x%08x\n", pmb->pmbNext));
            INT3();
            KLOGEXIT(-1);
            return -1;
        }
        if (pmb->u2.coreFree.Key != pNode->Key)
        {
            kprintf(("invalid key, pmb->coreFree.Key=%d, pNode->Key=%d\n", pmb->u2.coreFree.Key, pNode->Key));
            INT3();
            KLOGEXIT(-1);
            return -1;
        }
        /* next */
        pmb = pmb->pmbNext;
    }

    if (pNode->pLeft != NULL && swpCheckAVLTreeFree(pNode->pLeft) != 0)
    {
        KLOGEXIT(-1);
        return -1;
    }
    if (pNode->pRight != NULL && swpCheckAVLTreeFree(pNode->pRight) != 0)
    {
        KLOGEXIT(-1);
        return -1;
    }
    KLOGEXIT(0);
    return 0;
}
#endif


/**
 * Frees unused heapanchors.
 */
void _swp_heapmin(void)
{
    KLOGENTRY0("void");
    PMEMBLOCKCHUNK pmcLast;
    PMEMBLOCKCHUNK pmc;
    PHEAPANCHOR pha = phaLast;

    while (pha != NULL && pha != phaFirst)
    {
        if (pha->cbUsed == 0)
        {
            APIRET       rc;
            PHEAPANCHOR  phaToBeFreed = pha;
            if (pha->pPrev != NULL)
                pha->pPrev->pNext = pha->pNext;
            else
                phaFirst = pha->pPrev->pNext;
            if (pha->pNext != NULL)
                pha->pNext->pPrev = pha->pPrev;
            else
                phaLast = pha->pPrev;
            pha = pha->pPrev;

            /* free heap */
            #ifdef RING0
                rc = D32Hlp_VMFree(phaToBeFreed->pvBlock);
            #else
                rc = DosFreeMem(phaToBeFreed->pvBlock);
            #endif
            if (rc != NO_ERROR)
                kprintf(("DosFreeMem/D32Help_VMFree failed for 0x%08x, rc = %d\n",
                         pha, rc));
            rfree(phaToBeFreed);
        }
        else
            pha = pha->pPrev;
    }

    pmcLast = NULL;
    pmc = pmcFirst;
    while (pmc != NULL)
    {
        #ifdef DEBUG_ALLOC
        if (pmc->cFree > MEMBLOCKS_PER_CHUNK)
        {
            kprintf(("internal heap error - pmc->cFree(%d) > MEMBLOCKS_PER_CHUNK(%d)\n",
                     pmc->cFree, MEMBLOCKS_PER_CHUNK));
        }
        #endif
        if (pmc->cFree == MEMBLOCKS_PER_CHUNK && pmcLast != NULL)
        {
            pmcLast->pNext = pmc->pNext;
            rfree(pmc);
            pmc = pmcLast->pNext;
        }
        else
        {
            pmcLast = pmc;
            pmc = pmc->pNext;
        }
    }
    KLOGEXITVOID();
}


/**
 * Dumps a summary of the subheaps (heapanchor chain).
 */
void _swp_dump_subheaps(void)
{
    KLOGENTRY0("void");
    PHEAPANCHOR pha;
    int         i;
    unsigned    cbTotalFree;
    unsigned    cTotalFree;
    unsigned    cbTotalUsed;
    unsigned    cTotalUsed;

    kprintf(("------------------------------------\n"
             "- Dumping subheap blocks (summary) -\n"
             "------------------------------------\n"
             ));

    i = 0;
    cTotalFree = 0;
    cTotalUsed = 0;
    cbTotalFree = 0;
    cbTotalUsed = 0;
    pha = phaFirst;
    while (pha != NULL)
    {
        SUBHEAPS_CALLBACK_PARAM FreeParam = {0, 0};
        SUBHEAPS_CALLBACK_PARAM UsedParam = {0, 0};

        AVLULDoWithAll(&pha->pcoreUsed, 1,
                     (PAVLULCALLBACK)_swp_dump_subheaps_callback, SSToDS(&UsedParam));
        AVLULDoWithAll(&pha->pcoreFree, 1,
                     (PAVLULCALLBACK)_swp_dump_subheaps_callback, SSToDS(&FreeParam));

        kprintf(("HeapAnchor %2d addr=0x%08x cbSize=%6d cbFree=%6d cbUsed=%6d cUsed=%4d cFree=%d\n",
                 i, pha, pha->cbSize, pha->cbFree, pha->cbUsed, UsedParam.c, FreeParam.c));

        cTotalFree  += FreeParam.c;
        cbTotalFree += FreeParam.cb;
        cTotalUsed  += UsedParam.c;
        cbTotalUsed += UsedParam.cb;

        /* next */
        pha = pha->pNext;
        i++;
    }

    kprintf(("-----------------------------------------------------------------------------\n"
             " Free=%d #Free=%d AverageFree=%d Used=%d #Used=%d AverageUsed=%d\n"
             "-----------------------------------------------------------------------------\n",
             cbTotalFree, cTotalFree, cTotalFree > 0 ? cbTotalFree / cTotalFree : 0,
             cbTotalUsed, cTotalUsed, cTotalUsed > 0 ? cbTotalUsed / cTotalUsed : 0
             ));
    KLOGEXITVOID();
}



/**
 * Callbackfunction used by _swp_dump_subheaps to summarize a tree.
 */
static unsigned _swp_dump_subheaps_callback(PMEMBLOCK pmb, PSUBHEAPS_CALLBACK_PARAM pParam)
{
    KLOGENTRY2("unsigned","PMEMBLOCK pmb, PSUBHEAPS_CALLBACK_PARAM pParam", pmb, pParam);
    pParam->cb += pmb->u2.cbSize;
    pParam->c++;
    KLOGEXIT(0);
    return 0;
}


/**
 * Dumps all allocated memory.
 */
void _swp_dump_allocated(unsigned cb)
{
    KLOGENTRY1("void","unsigned cb", cb);
    PHEAPANCHOR pha;
    ALLOCATED_CALLBACK_PARAM Param = {cb, 0, 0};

    kprintf(("----------------------------\n"
             "- Dumping allocated blocks -\n"
             "----------------------------\n"));

    pha = phaFirst;
    while (pha != NULL)
    {
        /* iterate thru tree using callback */
        AVLULDoWithAll((PPAVLULNODECORE)&pha->pcoreUsed, TRUE,
                     (PAVLULCALLBACK)_swp_dump_allocated_callback, SSToDS(&Param));

        /* next */
        pha = pha->pNext;
    }

    kprintf((
             "---------------------------------------------\n"
             " #Blocks=%6d  Allocated=%9d (bytes)\n"
             "---------------------------------------------\n",
             Param.cBlocks, Param.cbAllocated
             ));
    KLOGEXITVOID();
}

/**
 * Callback function which dumps a node, and update statistics.
 */
static unsigned _swp_dump_allocated_callback(PMEMBLOCK pmb, PALLOCATED_CALLBACK_PARAM pParam)
{
    KLOGENTRY2("unsigned","PMEMBLOCK pmb, PALLOCATED_CALLBACK_PARAM pParam", pmb, pParam);
    int i;

    pParam->cBlocks++;
    pParam->cbAllocated += pmb->u2.cbSize;

    kprintf(("pvUserData=0x%08x  cbSize=%6d\n",
             pmb->u1.pvData, pmb->u2.cbSize
             ));
    /* dump cb of the block */
    i = 0;
    while (i < pParam->cb)
    {
        int j;
        char *pachUserData = (char*)pmb->u1.pvData;

        /* hex */
        j = 0;
        while (j < 16)
        {
            if (j+i < pParam->cb && j+i < pmb->u2.cbSize)
                kprintf((" %02x", pachUserData[j+i]));
            else
                kprintf(("   "));
            if (j == 7)
                kprintf((" -"));
            j++;
        }

        /* ascii */
        j = 0;
        kprintf(("  "));
        while (j < 16 && j+i < pmb->u2.cbSize && j+i < pParam->cb)
        {
            kprintf(("%c", pachUserData[j+i] < 0x20 ? '.' : pachUserData[j+i]));
            j++;
        }
        kprintf(("\n"));
        i += j;
    }

    KLOGEXIT(0);
    return 0;
}

