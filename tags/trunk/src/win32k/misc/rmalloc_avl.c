/* $Id: rmalloc_avl.c,v 1.7 2001-07-10 16:39:51 bird Exp $
 *
 * Resident Heap - AVL.
 *
 * Note: This heap does very little checking on input.
 *       Use with care! We're running at Ring-0!
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#pragma info(notrd)
#ifdef DEBUG
    #define DEBUG_ALLOC
    #undef ALLWAYS_HEAPCHECK
    #undef SOMETIMES_HEAPCHECK
#endif

#define HEAPANCHOR_SIGNATURE    0xBEEFFEEB
#define MEMBLOCK_SIGNATURE      0xFEEBBEEF


/*#define CB_HDR (sizeof(MEMBLOCK) - 1) /* size of MEMBLOCK header (in bytes) */
#define CB_HDR              ((int)&(((PMEMBLOCK)0)->achUserData[0]))
#define PNEXT_BLOCK(a)      ((PMEMBLOCK)((unsigned)(a) + CB_HDR + (a)->cbSize))
#define MEMBLOCKFREE_FROM_FREESIZENODE(a) \
                            ((PMEMBLOCKFREE)((unsigned)(a) - CB_HDR + 4))
#define BLOCKSIZE (1024*256)            /* 256KB */
#define ALIGNMENT 4

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
#include "asmutils.h"
#include "log.h"
#include "rmalloc.h"
#include "dev32.h"
#include "avl.h"
#include "macros.h"
#include <memory.h>


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)
typedef struct _MEMBLOCK /* MB */
{
    AVLNODECORE         core;
    #ifdef DEBUG_ALLOC
    unsigned long       ulSignature;    /* Contains MEMBLOCK_SIGNATURE (0xFEEBBEEF) */
    #endif
    unsigned            cbSize;         /* Size of user space (achBlock)*/
    unsigned char       achUserData[1]; /* User data */
} MEMBLOCK, *PMEMBLOCK;

typedef struct _MEMBLOCKFREE /* MBF */
{
    AVLNODECORE         core;
    #ifdef DEBUG_ALLOC
    unsigned long       ulSignature;    /* Contains MEMBLOCK_SIGNATURE (0xFEEBBEEF) */
    #endif
    AVLNODECORE         coreFree;       /* AVLNode core for the tree sorted on address */
    struct _MEMBLOCKFREE *pmbfNext;     /* Pointer to list of blocks with the same size */
} MEMBLOCKFREE, *PMEMBLOCKFREE;
#pragma pack()

typedef struct _HeapAnchor /* HA */
{
    #ifdef DEBUG_ALLOC
    unsigned long       ulSignature;    /* Contains HEAPANCHOR_SIGNATURE */
    #endif
    unsigned            cbSize;         /* Total amount of memory in this block. */
    struct _HeapAnchor *pNext;          /* Pointer to the next anchor block. */
    struct _HeapAnchor *pPrev;          /* Pointer to the previous anchor block. */
    PMEMBLOCK           pmbFree;        /* Pointer to the used-memblock chain. */
    PAVLNODECORE        pcoreFreeSize;  /* Pointer to free-size-tree. */
    unsigned            cbFree;         /* Amount of free memory. */
    PMEMBLOCK           pmbUsed;        /* Pointer to the used-memblock chain. */
    unsigned            cbUsed;         /* Amount of used memory. */

} HEAPANCHOR, *PHEAPANCHOR;


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
unsigned            cbResHeapMax;       /* Maximum amount of memory used by the heap. */

#ifndef RING0
char                fResInited;         /* init flag */
#endif


/******************************************************************************
*  Internal functions
******************************************************************************/
static void         resInsertUsed(PHEAPANCHOR pha, PMEMBLOCK pmb);
static void         resRemoveFromFreeSize(PHEAPANCHOR pha, PMEMBLOCKFREE pmbf);
static void         resInsertIntoFreeSize(PHEAPANCHOR pha, PMEMBLOCKFREE pmbf);
static void         resInsertFree(PHEAPANCHOR pha, PMEMBLOCK pmb);
static PMEMBLOCK    resGetFreeMemblock(PHEAPANCHOR *ppha, unsigned cbUserSize);
static PMEMBLOCK    resFindUsedBlock(PHEAPANCHOR *ppha, void *pvUser);
static PMEMBLOCK    resFindWithinUsedBlock(PHEAPANCHOR *ppha, void *pvUser);
#ifdef DEBUG_ALLOC
static int          resCheckAVLTree(PMEMBLOCK pmb);
static int          resCheckAVLTreeFree(PAVLNODECORE pNode);
#endif
static unsigned     _res_dump_subheaps_callback(PMEMBLOCK pmb, PSUBHEAPS_CALLBACK_PARAM pCb);
static unsigned     _res_dump_allocated_callback(PMEMBLOCK pmb, PALLOCATED_CALLBACK_PARAM pParam);


/**
 * Inserts a memblock into the used chain.
 * @param    pha  Pointer to the heap anchor which the block is to be inserted into.
 * @param    pmb  Pointer to memblock to insert into the free list.
 * @remark   Sorts on address.
 */
static void resInsertUsed(PHEAPANCHOR pha, PMEMBLOCK pmb)
{
    pha->cbUsed += pmb->cbSize;
    pmb->core.Key = (AVLKEY)pmb;
    AVLInsert((PPAVLNODECORE)&pha->pmbUsed, (PAVLNODECORE)pmb);
}


/**
 * Remove a given memblockfree from the free-size-tree.
 */
static void resRemoveFromFreeSize(PHEAPANCHOR pha, PMEMBLOCKFREE pmbf)
{
    PMEMBLOCKFREE pmbfParent;
    PMEMBLOCKFREE pmbfTmp;

    pmbfTmp = (PMEMBLOCKFREE)AVLGetWithParent(&pha->pcoreFreeSize,
                                              (PPAVLNODECORE)SSToDS(&pmbfParent),
                                              pmbf->coreFree.Key);
    if (pmbfTmp != NULL)
    {
        pmbfTmp = MEMBLOCKFREE_FROM_FREESIZENODE(pmbfTmp);
        if (pmbfTmp != pmbf)
        {
            PMEMBLOCKFREE pmbfPrev;
            do
            {
                pmbfPrev = pmbfTmp;
                pmbfTmp = pmbfTmp->pmbfNext;
            } while (pmbfTmp != NULL && pmbfTmp != pmbf);

            if (pmbfTmp != NULL)
                pmbfPrev->pmbfNext = pmbfTmp->pmbfNext;
            else
                kprintf(("resRemoveFromFreeSize: internal heap error, pmbf not in list.\n"));
        }
        else
        {   /* pmbf is first in the list */
            if (pmbfTmp->pmbfNext == NULL)
            {   /* no list - no other nodes of this size: simply remove it. */
                AVLRemove(&pha->pcoreFreeSize, pmbf->coreFree.Key);
            }
            else
            {   /* other nodes of this size: replace pmbf with the first node in the chain. */
                memcpy((void*)&pmbfTmp->pmbfNext->coreFree, (void*)&pmbfTmp->coreFree, sizeof(pmbfTmp->coreFree));
                if (pmbfParent != NULL)
                {
                    pmbfParent = MEMBLOCKFREE_FROM_FREESIZENODE(pmbfParent);
                    if (pmbfTmp->coreFree.Key < pmbfParent->coreFree.Key)
                        pmbfParent->coreFree.pLeft = &pmbf->pmbfNext->coreFree;
                    else
                        pmbfParent->coreFree.pRight = &pmbf->pmbfNext->coreFree;
                }
                else
                    pha->pcoreFreeSize = &pmbf->pmbfNext->coreFree;
            }
        }
    }
    else
        kprintf(("resRemoveFromFreeSize: internal heap error, free-node not in free-size tree.\n"));
}


/**
 * Inserts a block into the free-size-tree.
 */
static void resInsertIntoFreeSize(PHEAPANCHOR pha, PMEMBLOCKFREE pmbf)
{
    PMEMBLOCKFREE pmbfTmp;

    pmbfTmp = (PMEMBLOCKFREE)AVLGet(&pha->pcoreFreeSize, pmbf->coreFree.Key);
    if (pmbfTmp != NULL)
    {
        pmbfTmp = MEMBLOCKFREE_FROM_FREESIZENODE(pmbfTmp);
        while (pmbfTmp->pmbfNext != NULL && pmbfTmp->pmbfNext > pmbf)
            pmbfTmp = pmbfTmp->pmbfNext;

        pmbf->pmbfNext = pmbfTmp->pmbfNext;
        pmbfTmp->pmbfNext = pmbf;
    }
    else
    {
        pmbf->pmbfNext = NULL;
        AVLInsert(&pha->pcoreFreeSize, &pmbf->coreFree);
    }
}


/**
 * Inserts a memblock into the free chain.
 * Merges blocks adjecent blocks.
 * @param    pha  Pointer to the heap anchor which the block is to be inserted into.
 * @param    pmb  Pointer to memblock to insert into the free list.
 * @remark   Sorts on address.
 */
static void resInsertFree(PHEAPANCHOR pha, PMEMBLOCK pmb)
{
    PMEMBLOCKFREE   pmbf = (PMEMBLOCKFREE)pmb;
    PMEMBLOCKFREE   pmbfRight;
    PMEMBLOCKFREE   pmbfRightParent;
    PMEMBLOCKFREE   pmbfLeft;

    pha->cbFree += pmb->cbSize;

    /*
     * Get both right and left to determin which case we have here.
     * Four cases are possible:
     *    1 - insert no merge.
     *    2 - insert left merge.
     *    3 - insert right merge.
     *    4 - insert both left and right merge.
     */
    pmbfRight = (PMEMBLOCKFREE)AVLGetWithParent((PPAVLNODECORE)&pha->pmbFree,
                                                (PPAVLNODECORE)SSToDS(&pmbfRightParent),
                                                (AVLKEY)PNEXT_BLOCK(pmb));
    pmbfLeft = (PMEMBLOCKFREE)AVLGetBestFit((PPAVLNODECORE)&pha->pmbFree,
                                            (AVLKEY)pmbf, FALSE);
    if (pmbfLeft != NULL && (PMEMBLOCKFREE)PNEXT_BLOCK((PMEMBLOCK)pmbfLeft) != pmbf)
        pmbfLeft = NULL;

    if (pmbfLeft == NULL && pmbfRight == NULL)
    {   /* 1 - insert no merge */
        pmbf->core.Key = (AVLKEY)pmbf;
        AVLInsert((PPAVLNODECORE)&pha->pmbFree, (PAVLNODECORE)pmbf);
        resInsertIntoFreeSize(pha, pmbf);
    }
    else if (pmbfLeft != NULL && pmbfRight == NULL)
    {   /* 2 - insert left merge: just add pmbf to pmbfLeft. */
        resRemoveFromFreeSize(pha, pmbfLeft);
        pmbfLeft->coreFree.Key += CB_HDR + pmbf->coreFree.Key;
        pha->cbFree += CB_HDR;
        resInsertIntoFreeSize(pha, pmbfLeft);
    }
    else if (pmbfLeft == NULL && pmbfRight != NULL)
    {   /* 3 - insert right merge: replace pmbfRight with pmbf in the pmbfFree tree; */
        resRemoveFromFreeSize(pha, pmbfRight);
        memcpy((void*)&pmbf->core, (void*)&pmbfRight->core, sizeof(pmbf->core));
        pmbf->core.Key = (AVLKEY)pmbf;
        pmbf->coreFree.Key += CB_HDR + pmbfRight->coreFree.Key;
        pha->cbFree += CB_HDR;
        resInsertIntoFreeSize(pha, pmbf);
        if (pmbfRightParent != NULL)
        {
            if (pmbf < pmbfRightParent)
                pmbfRightParent->core.pLeft = (PAVLNODECORE)pmbf;
            else
                pmbfRightParent->core.pRight = (PAVLNODECORE)pmbf;
        }
        else
            pha->pmbFree = (PMEMBLOCK)pmbf;
    }
    else
    {   /* 4 -insert both left and right merge */
        if (AVLRemove((PPAVLNODECORE)&pha->pmbFree, (AVLKEY)pmbfRight) != (PAVLNODECORE)pmbfRight)
        {
            kprintf(("resInsertFree: AVLRemove on pmbfRight failed.\n"));
            return;
        }
        resRemoveFromFreeSize(pha, pmbfLeft);
        resRemoveFromFreeSize(pha, pmbfRight);
        pmbfLeft->coreFree.Key += CB_HDR*2 + pmbf->coreFree.Key + pmbfRight->coreFree.Key;
        pha->cbFree += CB_HDR*2;
        resInsertIntoFreeSize(pha, pmbfLeft);
    }
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
static PMEMBLOCK resGetFreeMemblock(PHEAPANCHOR *ppha, unsigned cbUserSize)
{
    unsigned cbBlockSize;
    unsigned cbTotalSize = 0;

    #ifdef DEBUG
    cbUserSize = MAX(ALIGN(cbUserSize, ALIGNMENT), sizeof(MEMBLOCKFREE) - CB_HDR);
    #else
    cbUserSize = MAX(ALIGN(cbUserSize + 16, ALIGNMENT), sizeof(MEMBLOCKFREE) - CB_HDR); /* add 16 bytes for safety... */
    #endif

    *ppha = phaFirst;
    while (*ppha != NULL)
    {
        if ((*ppha)->cbFree >= cbUserSize + CB_HDR)
        {
            PMEMBLOCKFREE pmbf;

            pmbf = (PMEMBLOCKFREE)AVLGetBestFit(&(*ppha)->pcoreFreeSize, cbUserSize, TRUE);
            if (pmbf != NULL)
            {
                pmbf = MEMBLOCKFREE_FROM_FREESIZENODE(pmbf);
                resRemoveFromFreeSize(*ppha, pmbf);
                (*ppha)->cbFree -= pmbf->coreFree.Key;

                /* pmbf is now the block which we are to return, but do we have to split it? */
                if (pmbf->coreFree.Key > sizeof(MEMBLOCKFREE) + cbUserSize)
                {
                    PMEMBLOCKFREE pmbfTmp = pmbf;
                    pmbf = (PMEMBLOCKFREE)((unsigned)pmbfTmp + pmbfTmp->coreFree.Key - cbUserSize);
                    #ifdef DEBUG_ALLOC
                        pmbf->ulSignature = MEMBLOCK_SIGNATURE;
                    #endif
                    pmbfTmp->coreFree.Key -= CB_HDR + cbUserSize;
                    pmbf->coreFree.Key = cbUserSize;
                    (*ppha)->cbFree += pmbfTmp->coreFree.Key;
                    resInsertIntoFreeSize(*ppha, pmbfTmp);
                }
                else
                {
                    PMEMBLOCKFREE pmbfTmp = (PMEMBLOCKFREE)AVLRemove((PPAVLNODECORE)&(*ppha)->pmbFree, (AVLKEY)pmbf);
                    if (pmbfTmp != pmbf)
                    {
                        kprintf(("resGetFreeMemblock: Internal heap error - failed to Remove best fit block!\n"));
                        return NULL;
                    }
                }
                return (PMEMBLOCK)pmbf;
            }
        }

        cbTotalSize += (*ppha)->cbSize;

        /* next heap anchor */
        *ppha = (*ppha)->pNext;
    }


    /*
     * Add a new heap anchor?
     */
    cbBlockSize = ALIGN(cbUserSize + CB_HDR * 2, BLOCKSIZE);
    if (cbResHeapMax >= cbTotalSize + cbBlockSize)
    {
        #ifdef RING0
            *ppha = D32Hlp_VMAlloc(0UL, cbBlockSize, ~0UL);
        #else
            if (DosAllocMem((void*)ppha, cbBlockSize, PAG_COMMIT | PAG_READ | PAG_WRITE) != 0)
                *ppha = NULL;
        #endif

        if (*ppha != NULL)
        {
            register PHEAPANCHOR pha = *ppha;
            PMEMBLOCK pmb;
            memset(pha, 0, sizeof(*pha));

            /* anchor block */
            #ifdef DEBUG_ALLOC
                pha->ulSignature = HEAPANCHOR_SIGNATURE;
            #endif
            pha->cbSize = cbBlockSize;

            /* free memblock */
            pmb = (PMEMBLOCK)((unsigned)pha + sizeof(*pha));
            #ifdef DEBUG_ALLOC
                pmb->ulSignature = MEMBLOCK_SIGNATURE;
            #endif
            pmb->cbSize = cbBlockSize - sizeof(*pha) - CB_HDR * 2 - cbUserSize;
            resInsertFree(pha, pmb);

            /* used memblock */
            pmb = (PMEMBLOCK)((unsigned)pha + cbBlockSize - cbUserSize - CB_HDR);
            #ifdef DEBUG_ALLOC
                pmb->ulSignature = MEMBLOCK_SIGNATURE;
            #endif
            pmb->cbSize = cbUserSize;

            /* insert into list */
            pha->pPrev = phaLast;
            pha->pNext = NULL;
            if (phaLast == NULL) /* Might never happen but just in case it does. */
                phaLast = phaFirst = pha;
            else
                phaLast->pNext = pha;
            phaLast = pha;

            return pmb;
        }
        else
            kprintf(("resGetFreeMemblock: Unable to allocate heap memory.\n"));
    }
    else
    {
        kprintf(("resGetFreeMemblock: Allocation failed, limit reached. \n"
                 "    %d(=cbResHeapMax) < %d(=cbTotalSize) + %d(=cbBlockSize)\n",
                  cbResHeapMax, cbTotalSize, cbBlockSize));
    }

    return NULL;
}


/**
 * Finds a used memory block.
 * @returns   Pointer to memblock if found.
 * @param     ppha       Pointer to pointer to heap anchor block the returned memblock is located in. (output)
 *                       NULL is allowed.
 * @param     pvUser     User pointer to find the block to.
 */
static PMEMBLOCK resFindUsedBlock(PHEAPANCHOR *ppha, void *pvUser)
{
    if (pvUser != NULL)
    {
        register PHEAPANCHOR pha = phaFirst;
        while (pha != NULL
               && !((unsigned)pvUser > (unsigned)pha
                    && (unsigned)pvUser < (unsigned)pha + pha->cbSize))
            pha = pha->pNext;

        if (ppha != NULL)
            *ppha = pha;
        if (pha != NULL)
        {
            register PMEMBLOCK pmb;
            #ifdef DEBUG_ALLOC
                if (pha->ulSignature != HEAPANCHOR_SIGNATURE)
                {
                    kprintf(("resFindUsedBlock: Invalid heapanchor signature.\n"));
                    return NULL;
                }
            #endif
            pmb = (PMEMBLOCK)AVLGet((PPAVLNODECORE)&pha->pmbUsed, (AVLKEY)((unsigned)pvUser - CB_HDR));
            #ifdef DEBUG_ALLOC
                if (pmb != NULL && pmb->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("resFindUsedBlock: Invalid memblock signature.\n"));
                    pmb = NULL;
                }
            #endif
            return pmb;
        }
    }

    return NULL;
}


/**
 * Finds a used memory block from a pointer into the userdata.
 * @returns   Pointer to memblock if found.
 * @param     ppha       Pointer to pointer to heap anchor block the returned memblock is located in. (output)
 *                       NULL is allowed.
 * @param     pvUser     User pointer to find the block to.
 */
static PMEMBLOCK resFindWithinUsedBlock(PHEAPANCHOR *ppha, void *pvUser)
{
    register PHEAPANCHOR pha = phaFirst;

    while (pha != NULL
           && !((unsigned)pvUser > (unsigned)pha
                && (unsigned)pvUser < (unsigned)pha + pha->cbSize))
        pha = pha->pNext;

    if (ppha != NULL)
        *ppha = pha;
    if (pha != NULL)
    {
        PMEMBLOCK pmb;

        #ifdef DEBUG_ALLOC
            if (pha->ulSignature != HEAPANCHOR_SIGNATURE)
            {
                kprintf(("resFindWithinUsedBlock: Invalid heapanchor signature.\n"));
                return NULL;
            }
        #endif


        pmb = (PMEMBLOCK)AVLGetBestFit((PPAVLNODECORE)&pha->pmbUsed,
                                       (AVLKEY)pvUser, TRUE);
        if (pmb != NULL
            && (unsigned)pmb + pmb->cbSize + CB_HDR > (unsigned)pvUser
            && (unsigned)pmb + CB_HDR <= (unsigned)pvUser
            )
        {
            #ifdef DEBUG_ALLOC
                if (pmb->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("resFindWithinUsedBlock: Invalid memblock signature.\n"));
                    pmb = NULL;
                }
            #endif
            return pmb;
        }
    }

    return NULL;
}


/**
 * Initiate the heap "subsystem".
 * @returns   0 on success, not 0 on error.
 * @param     cbSizeInit  The initial size of the heap in bytes.
 * @param     cbSizeMax   Maximum heapsize in bytes.
 */
int resHeapInit(unsigned cbSizeInit, unsigned cbSizeMax)
{
    unsigned  cbSize = MAX(BLOCKSIZE, cbSizeInit);
    PMEMBLOCK pmb;

    cbResHeapMax = cbSizeMax;

    #ifdef RING0
        phaFirst = D32Hlp_VMAlloc(0UL, cbSize, ~0UL);
    #else
        if (DosAllocMem((void*)&phaFirst, cbSize, PAG_COMMIT | PAG_READ | PAG_WRITE) != 0)
            phaFirst = NULL;
    #endif
    if (phaFirst == NULL)
    {
        kprintf(("unable to allocate heap memory.\n"));
        Int3();
        return -1;
    }

    #ifdef DEBUG_ALLOC
        phaFirst->ulSignature = HEAPANCHOR_SIGNATURE;
    #endif
    phaFirst->cbSize = cbSize;
    phaFirst->pNext = NULL;
    phaFirst->pPrev = NULL;
    phaFirst->pmbFree = NULL;
    phaFirst->pcoreFreeSize = NULL;
    phaFirst->cbFree = 0;
    phaFirst->pmbUsed = NULL;
    phaFirst->cbUsed = 0UL;
    phaLast = phaFirst;

    pmb = (PMEMBLOCK)((unsigned)phaFirst+sizeof(*phaFirst));
    pmb->cbSize = cbSize - sizeof(*phaFirst) - CB_HDR;
    #ifdef DEBUG_ALLOC
        pmb->ulSignature = MEMBLOCK_SIGNATURE;
    #endif
    resInsertFree(phaFirst, pmb);

    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
        if (!_res_heap_check())
        {   /* error! */
            kprintf(("resHeapInit: _res_heap_check failed!\n"));
            #ifdef DEBUG
            Int3();
            #endif
            return -2;
        }
    #endif
    #ifndef RING0
        fResInited = TRUE;
    #endif
    return 0;
}


/**
 * malloc - allocates a given amount of memory.
 * @returns  Pointer to allocated memory.
 *           NULL if out of memory. (Or memory to fragmented.)
 * @param    cbSize  Bytes user requests us to allocate. This is aligned
 *                   to four bytes.
 */
void * rmalloc(unsigned cbSize)
{
    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_res_heap_check())
    {
        kprintf(("rmalloc: _res_heap_check failed!\n"));
        return NULL;
    }
    #endif

    if (cbSize != 0)
    {
        PHEAPANCHOR pha;
        PMEMBLOCK   pmb = resGetFreeMemblock(SSToDS(&pha), cbSize);
        if (pmb != NULL)
        {
            resInsertUsed(pha, pmb);
            return &pmb->achUserData[0];
        }
    }
    else
        kprintf(("rmalloc: error cbSize = 0\n"));

    return NULL;
}


/**
 * Reallocate a heapblock.
 * @returns   Pointer to new heapblock.
 * @param     pv     Pointer to the block to realloc.
 *                   If pv is NULL, this call is equal to malloc.
 * @param     cbNew  The new block size.
 */
void *rrealloc(void *pv, unsigned cbNew)
{
    PMEMBLOCK pmb;
    PHEAPANCHOR pha;

    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_res_heap_check())
    {
        kprintf(("rrealloc: _res_heap_check failed!\n"));
        return NULL;
    }
    #endif
    pmb = resFindUsedBlock(SSToDS(&pha), pv);
    if (pmb != NULL)
    {
        void *pvRet;

        cbNew = MAX(ALIGN(cbNew, ALIGNMENT), sizeof(MEMBLOCKFREE) - CB_HDR);
        if (cbNew <= pmb->cbSize)
        {   /* shrink block */
            pvRet = pv;
            if (cbNew + sizeof(MEMBLOCKFREE) <= pmb->cbSize)
            {   /* split block */
                PMEMBLOCKFREE pmbfNew = (PMEMBLOCKFREE)((unsigned)pmb + CB_HDR + cbNew);
                #ifdef DEBUG_ALLOC
                pmbfNew->ulSignature = MEMBLOCK_SIGNATURE;
                #endif
                pha->cbUsed -= pmb->cbSize - cbNew;
                pmbfNew->coreFree.Key = pmb->cbSize - cbNew - CB_HDR;
                pmb->cbSize = cbNew;
                resInsertFree(pha, (PMEMBLOCK)pmbfNew);
                #ifdef ALLWAYS_HEAPCHECK
                if (!_res_heap_check())
                {
                    kprintf(("rrealloc: _res_heap_check failed!\n"));
                    return NULL;
                }
                #endif
            }
        }
        else
        {   /* expand block - this code may be optimized... */
            #if 0
            pvRet = rmalloc(cbNew);
            if (pvRet != NULL)
            {
                memcpy(pvRet, pv, pmb->cbSize);
                rfree(pv);
            }
            #else
            /* optimized FIXME! */
            PMEMBLOCKFREE pmbfRightParent;
            PMEMBLOCKFREE pmbfRight = (PMEMBLOCKFREE)AVLGetWithParent((PPAVLNODECORE)&pha->pmbFree,
                                                                      (PPAVLNODECORE)SSToDS(&pmbfRightParent),
                                                                      (AVLKEY)PNEXT_BLOCK(pmb));
            if (pmbfRight != NULL && pmbfRight->coreFree.Key + pmb->cbSize + CB_HDR >= cbNew)
            {
                pvRet = pv;
                /* split the free block? */
                if (pmbfRight->coreFree.Key + pmb->cbSize + CB_HDR - sizeof(MEMBLOCKFREE) >= cbNew)
                {
                    unsigned      cb = pmbfRight->coreFree.Key;
                    PMEMBLOCKFREE pmbfNew = (PMEMBLOCKFREE)((unsigned)pmb + CB_HDR + cbNew);
                    resRemoveFromFreeSize(pha, pmbfRight);
                    pmbfNew->coreFree.Key = cb + pmb->cbSize - cbNew;
                    memmove((void*)&pmbfNew->core, (void*)&pmbfRight->core, sizeof(pmbfNew->core));
                    pmbfNew->core.Key = (AVLKEY)pmbfNew;
                    #ifdef DEBUG_ALLOC
                    pmbfNew->ulSignature = MEMBLOCK_SIGNATURE;
                    #endif
                    if (pmbfRightParent != NULL)
                    {
                        if (pmbfNew < pmbfRightParent)
                            pmbfRightParent->core.pLeft = &pmbfNew->core;
                        else
                            pmbfRightParent->core.pRight = &pmbfNew->core;
                    }
                    else
                        pha->pmbFree = (PMEMBLOCK)pmbfNew;

                    resInsertIntoFreeSize(pha, pmbfNew);
                    pha->cbUsed += cbNew - pmb->cbSize;
                    pha->cbFree -= cb - pmbfNew->coreFree.Key;
                    pmb->cbSize = cbNew;
                }
                else
                {
                    if (AVLRemove((PPAVLNODECORE)&pha->pmbFree, (AVLKEY)pmbfRight) != (PAVLNODECORE)pmbfRight)
                    {
                        kprintf(("rrealloc: AVLRemove failed for pmbfRight - hmm!\n"));
                        return NULL;
                    }
                    resRemoveFromFreeSize(pha, pmbfRight);
                    pmb->cbSize += pmbfRight->coreFree.Key + CB_HDR;
                    pha->cbFree -= pmbfRight->coreFree.Key;
                    pha->cbUsed += pmbfRight->coreFree.Key + CB_HDR;
                }

            }
            else
            {   /* worst case: allocate a new block, copy data and free the old block. */
                pvRet = rmalloc(cbNew);
                if (pvRet != NULL)
                {
                    memcpy(pvRet, pv, pmb->cbSize);
                    rfree(pv);
                }
            }
            #endif
        }
        return pvRet;
    }
    else
    {
        if (pv == NULL)
            return rmalloc(cbNew);
        kprintf(("rrealloc: invalid user pointer, pv=0x%08x\n", pv));
    }
    return NULL;
}


/**
 * Frees a block.
 * @param    pv  User pointer.
 */
void rfree(void *pv)
{
    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_res_heap_check())
    {
        kprintf(("rfree: _res_heap_check failed!\n"));
        return;
    }
    #endif

    if (pv != NULL)
    {
        PHEAPANCHOR pha = phaFirst;

        while (pha != NULL
               && !((unsigned)pv > (unsigned)pha
                    && (unsigned)pv < (unsigned)pha + pha->cbSize))
            pha = pha->pNext;

        if (pha != NULL)
        {
            PMEMBLOCK pmb = (PMEMBLOCK)AVLRemove((PPAVLNODECORE)&pha->pmbUsed,
                                                 (AVLKEY)((unsigned)pv - CB_HDR));
            if (pmb != NULL)
            {
                pha->cbUsed -= pmb->cbSize;
                resInsertFree(pha, pmb);
            }
            else
                kprintf(("rfree: heap block not found!\n"));
        }
        else
            kprintf(("rfree: heap block not within the large blocks!\n"));
    }
    else
        kprintf(("rfree: Free received a NULL pointer!\n"));
}


/**
 * Gets the size of a block.
 * @returns  Bytes in a block.
 */
unsigned _res_msize(void *pv)
{
    PHEAPANCHOR pha;
    PMEMBLOCK   pmb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
        kprintf(("_res_msize: _res_heap_check failed!\n"));
    #endif

    pmb = resFindUsedBlock(SSToDS(&pha), pv);
    return pmb != NULL ? pmb->cbSize : 0;
}


/**
 * Checks if pv is a valid heappointer.
 * @returns   1 if valid. 0 if invalid.
 * @param     pv  User data pointer.
 */
int _res_validptr(void *pv)
{
    PMEMBLOCK   pmb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
        kprintf(("_res_validptr: _res_heap_check failed!\n"));
    #endif

    pmb = resFindWithinUsedBlock(NULL, pv);
    return pmb != NULL;
}


/**
 * Checks that the dataaera made up by pv and cbSize valid with in the heap.
 * @returns   1 if valid. 0 if invalid.
 * @param     pv      User data pointer.
 * @param     cbSize  Size of data which has to be valid.
 */
int _res_validptr2(void *pv, unsigned cbSize)
{
    PMEMBLOCK   pmb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
        kprintf(("_res_validptr: _res_heap_check failed!\n"));
    #endif

    pmb = resFindWithinUsedBlock(NULL, pv);
    return pmb != NULL ? (pmb->cbSize - ((unsigned)pv - (unsigned)pmb - CB_HDR)) >= cbSize : FALSE;
}


/**
 * Get amount of free memory (in bytes)
 * @returns  Amount of free memory (in bytes).
 * @remark   Note that this amount is of all free memory blocks and
 *           that these blocks are fragmented.
 *           You'll probably not be able to allocate a single block
 *           of the returned size.
 */
unsigned _res_memfree(void)
{
    PHEAPANCHOR pha = phaFirst;
    unsigned    cb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
        kprintf(("_res_memfree: _res_heap_check failed!\n"));
    #endif

    for (cb = 0; pha != NULL; pha = pha->pNext)
        cb += pha->cbFree;

    return cb;
}


/**
 * Get amount of used memory (in bytes)
 * @returns  Amount of used memory (in bytes).
 */
unsigned    _res_memused(void)
{
    PHEAPANCHOR pha = phaFirst;
    unsigned    cb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
        kprintf(("_res_memused: _res_heap_check failed!\n"));
    #endif

    for (cb = 0; pha != NULL; pha = pha->pNext)
        cb += pha->cbUsed;

    return cb;
}


/**
 * Collects the heap state.
 * @returns     0 on success.
 *              -1 on error.
 * @param       pState  Pointer to a RES_HEAPSTATE structure which is
 *                      filled upon successful return.
 */
int         _res_state(PHEAPSTATE pState)
{
    PHEAPANCHOR pha;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
    {
        kprintf(("_res_state: _res_heap_check failed!\n"));
        return -1;
    }
    #endif

    if (pState == NULL)
        return -1;

    /*
     * Loop thru all the anchor blocks and all memory blocks
     * building the stats.
     */
    memset(pState, 0, sizeof(HEAPSTATE));
    for (pha = phaFirst; pha != NULL; pha = pha->pNext)
    {
        AVLENUMDATA     EnumData;
        PAVLENUMDATA    pEnumData = SSToDS(&EnumData);
        PMEMBLOCK       pmb;

        /* count of free blocks */
        pmb = (PMEMBLOCK)AVLBeginEnumTree((PPAVLNODECORE)&pha->pmbFree, pEnumData, TRUE);
        while (pmb)
        {
            pState->cBlocksFree++;
            pmb = (PMEMBLOCK)AVLGetNextNode(pEnumData);
        }

        /* count of used blocks */
        pmb = (PMEMBLOCK)AVLBeginEnumTree((PPAVLNODECORE)&pha->pmbUsed, pEnumData, TRUE);
        while (pmb)
        {
            pState->cBlocksUsed++;
            pmb = (PMEMBLOCK)AVLGetNextNode(pEnumData);
        }

        /* sizes */
        pState->cbHeapSize  += pha->cbSize;
        pState->cbHeapFree  += pha->cbFree;
        pState->cbHeapUsed  += pha->cbUsed;
    }

    return 0;
}


/**
 * Checks heap integrety.
 * @returns  TRUE  when ok.
 *           FALSE on error.
 *           NULL if out of memory. (Or memory to fragmented.)
 */
int _res_heap_check(void)
{
#ifdef DEBUG_ALLOC
    PHEAPANCHOR pha = phaFirst;
    PHEAPANCHOR phaPrev = NULL;


    while (pha != NULL)
    {
        AVLENUMDATA FreeEnumData;
        AVLENUMDATA UsedEnumData;
        PMEMBLOCK pmbFree = (PMEMBLOCK)AVLBeginEnumTree((PPAVLNODECORE)&pha->pmbFree,
                                                        SSToDS(&FreeEnumData), TRUE);
        PMEMBLOCK pmbFreeNext = (PMEMBLOCK)AVLGetNextNode(SSToDS(&FreeEnumData));
        PMEMBLOCK pmbUsed = (PMEMBLOCK)AVLBeginEnumTree((PPAVLNODECORE)&pha->pmbUsed,
                                                        SSToDS(&UsedEnumData), TRUE);
        PMEMBLOCK pmbUsedNext = (PMEMBLOCK)AVLGetNextNode(SSToDS(&UsedEnumData));
        PMEMBLOCK pmbLast = NULL;
        unsigned  cbFree = 0;
        unsigned  cbUsed = 0;
        unsigned  cbSize = sizeof(*pha);

        /*
         * Check the heap anchor.
         */
        if (pha->ulSignature != HEAPANCHOR_SIGNATURE)
        {
            kprintf(("_res_heap_check: invalid signature for pha=0x%08x\n", pha));
            Int3();
            return FALSE;
        }

        if (pha->cbFree + pha->cbUsed >= pha->cbSize || pha->cbSize == 0)
        {
            kprintf(("_res_heap_check: cbFree + cbUsed >= cbSize for pha=0x%08x\n", pha));
            Int3();
            return FALSE;
        }

        if (pha->pPrev != phaPrev)
        {
            kprintf(("_res_heap_check: error in heap anchor chain.\n"));
            Int3();
            return FALSE;
        }

        if ((unsigned)MINNOTNULL(pmbFree, pmbUsed) != (unsigned)(pha+1))
        {
            kprintf(("_res_heap_check: The first free/used block don't start at start of memory\n"
                     "    block. pmbFree=0x%08x, pmbUsed=0x%08x, pha+1=0x%08x\n",
                     pmbFree, pmbUsed, pha+1));
            Int3();
            return FALSE;
        }

        /*
         * Check the tree AVL-trees - !extra debug test!
         */
        if (resCheckAVLTree(pha->pmbUsed) != 0)
        {
            kprintf(("_res_heap_check: the Used tree is invalid.\n"));
            Int3();
            return FALSE;
        }
        if (resCheckAVLTree(pha->pmbFree) != 0)
        {
            kprintf(("_res_heap_check: the Free tree is invalid.\n"));
            Int3();
            return FALSE;
        }
        if (resCheckAVLTreeFree(pha->pcoreFreeSize) != 0)
        {
            kprintf(("_res_heap_check: the free-size tree is invalid.\n"));
            Int3();
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
             *  1) pmbUsed adjecent to pmbUsed->pNext
             *  2) pmbUsed adjecent to pmbFree
             *  3) pmbFree adjecent to pmbFree->pNext
             *  4) pmbFree adjecent to pmbUsed
             *  5) pmbUsed is the last block
             *  6) pmbFree is the last block
             */
            if (!( (pmbUsed != NULL && PNEXT_BLOCK(pmbUsed) == pmbUsedNext)     /* 1.*/
                || (pmbUsed != NULL && PNEXT_BLOCK(pmbUsed) == pmbFree)         /* 2.*/
                || (pmbFree != NULL && PNEXT_BLOCK(pmbFree) == pmbFreeNext)     /* 3.*/
                || (pmbFree != NULL && PNEXT_BLOCK(pmbFree) == pmbUsed)         /* 4.*/
                || (pmbUsed != NULL && pmbFree == NULL && pmbUsedNext == NULL)  /* 5.*/
                || (pmbFree != NULL && pmbUsed == NULL && pmbFreeNext == NULL)  /* 6.*/
                 )
               )
                {
                /* error hole */
                kprintf(("_res_heap_check: internal error - memory hole!\n"));
                Int3();
                return FALSE;
                }

            /* check signature and advance to the next block */
            if (pmbUsed != NULL && (pmbFree == NULL || pmbUsed < pmbFree))
            {
                cbSize += CB_HDR + pmbUsed->cbSize;
                cbUsed += pmbUsed->cbSize;
                if (pmbUsed->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("_res_heap_check: invalid signature on used block, pmbUsed=0x%08x\n", pmbUsed));
                    Int3();
                    return FALSE;
                }
                if (pmbUsed->cbSize < sizeof(MEMBLOCKFREE) - CB_HDR)
                {
                    kprintf(("_res_heap_check: internal error - cbSize is to small! (used), cbSize=%d\n", pmbUsed->cbSize));
                    Int3();
                    return FALSE;
                }
                pmbLast = pmbUsed;
                pmbUsed = pmbUsedNext;
                pmbUsedNext = (PMEMBLOCK)AVLGetNextNode(SSToDS(&UsedEnumData));
            }
            else
            {
                PMEMBLOCKFREE pmbf = (PMEMBLOCKFREE)AVLGet(&pha->pcoreFreeSize, pmbFree->cbSize);
                if (pmbf != NULL)
                {
                    pmbf = MEMBLOCKFREE_FROM_FREESIZENODE(pmbf);
                    while (pmbf != NULL && pmbf != (PMEMBLOCKFREE)pmbFree)
                        pmbf = pmbf->pmbfNext;
                    if (pmbf == NULL)
                    {
                        kprintf(("_res_heap_check: pmbFree=0x%08 is not found in the free-size-tree.\n", pmbFree));
                        Int3();
                        return FALSE;
                    }
                }
                else
                {
                    kprintf(("_res_heap_check: pmbFree=0x%08 is not found in the free-size-tree.\n", pmbFree));
                    Int3();
                    return FALSE;
                }

                cbSize += CB_HDR + pmbFree->cbSize;
                cbFree += pmbFree->cbSize;
                if (pmbFree->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("_res_heap_check: invalid signature on free block, pmbUsed=0x%08x\n", pmbFree));
                    Int3();
                    return FALSE;
                }
                if (pmbFree->cbSize < sizeof(MEMBLOCKFREE) - CB_HDR)
                {
                    kprintf(("_res_heap_check: internal error - cbSize is to small! (free), cbSize=%d\n", pmbFree->cbSize));
                    Int3();
                    return FALSE;
                }
                pmbLast = pmbFree;
                pmbFree = pmbFreeNext;
                pmbFreeNext = (PMEMBLOCK)AVLGetNextNode(SSToDS(&FreeEnumData));
            }
        }


        /*
         * Check the cbFree and cbUsed.
         */
        if (cbFree != pha->cbFree)
        {
            kprintf(("_res_heap_check: cbFree(%d) != pha->cbFree(%d)\n", cbFree, pha->cbFree));
            Int3();
            return FALSE;
        }

        if (cbUsed != pha->cbUsed)
        {
            kprintf(("_res_heap_check: cbUsed(%d) != pha->cbUsed(%d)\n", cbUsed, pha->cbUsed));
            Int3();
            return FALSE;
        }

        if (cbSize != pha->cbSize)
        {
            kprintf(("_res_heap_check: cbTotal(%d) != pha->cbSize(%d)\n", cbSize, pha->cbSize));
            Int3();
            return FALSE;
        }
        /*
         * Check right most node.
         */
        if (pmbLast == NULL || (unsigned)pha + pha->cbSize != (unsigned)PNEXT_BLOCK(pmbLast))
        {
            kprintf(("_res_heap_check: The last free/used block dont end at the end of memory block.\n"
                     "    pmbLast(end)=0x%08x, pha+pha->cbSize=0x%08x\n",
                     pmbLast != NULL ? PNEXT_BLOCK(pmbLast) : NULL, (unsigned)pha + pha->cbSize));
            Int3();
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
        kprintf(("_res_heap_check: internal error - heap anchor chain didn't end on phaLast\n"));
        Int3();
        return FALSE;
    }

#endif

    return TRUE;
}


#ifdef DEBUG_ALLOC
/**
 * Check the integrity of the a Free/Used AVL tree where Key == node pointer.
 */
static int resCheckAVLTree(PMEMBLOCK pmb)
{
    if (pmb == NULL)
        return 0;
    if (pmb->core.Key != (AVLKEY)pmb)
    {
        kprintf(("resCheckAVLTree: Invalid Key!\n"));
        Int3();
        return -1;
    }
    if (pmb->core.Key % ALIGNMENT != 0)
    {
        kprintf(("resCheckAVLTree: Data is not correctly aligned, uData=0x%08x\n", pmb->core.Key));
        Int3();
        return -1;
    }
    if (pmb->core.pLeft != NULL && (pmb->core.pLeft < (PAVLNODECORE)0x10000 || pmb->core.Key <= (AVLKEY)pmb->core.pLeft))
    {
        kprintf(("resCheckAVLTree: Invalid pLeft!\n"));
        Int3();
        return -1;
    }
    if (pmb->core.pRight != NULL && (pmb->core.pRight < (PAVLNODECORE)0x10000 || pmb->core.Key >= (AVLKEY)pmb->core.pRight))
    {
        kprintf(("resCheckAVLTree: Invalid pRight!\n"));
        Int3();
        return -1;
    }
    if (pmb->core.pLeft != NULL && resCheckAVLTree((PMEMBLOCK)pmb->core.pLeft) != 0)
        return -1;
    if (pmb->core.pRight != NULL && resCheckAVLTree((PMEMBLOCK)pmb->core.pRight) != 0)
        return -1;
    return 0;
}


/**
 * Check the integrity of the a Free/Used AVL tree where Key == node pointer.
 */
static int resCheckAVLTreeFree(PAVLNODECORE pNode)
{
    PMEMBLOCKFREE pmbf;
    if (pNode == NULL)
        return 0;
    if (pNode->Key < 4 || (pNode->Key % ALIGNMENT) != 0)
    {
        kprintf(("resCheckAVLTreeFree: Invalid Key! 0x%08x\n", pNode->Key));
        Int3();
        return -1;
    }
    if (pNode->pLeft != NULL && (pNode->pLeft < (PAVLNODECORE)0x10000 || pNode->Key <= pNode->pLeft->Key))
    {
        kprintf(("resCheckAVLTreeFree: Invalid pLeft!\n"));
        Int3();
        return -1;
    }
    if (pNode->pRight != NULL && (pNode->pRight < (PAVLNODECORE)0x10000 || pNode->Key >= pNode->pRight->Key))
    {
        kprintf(("resCheckAVLTreeFree: Invalid pRight!\n"));
        Int3();
        return -1;
    }
    pmbf = MEMBLOCKFREE_FROM_FREESIZENODE(pNode);
    if (pmbf->pmbfNext != NULL && pmbf->pmbfNext < (PMEMBLOCKFREE)0x10000)
    {
        kprintf(("resCheckAVLTreeFree: invalid pmbfNext pointer, pmbfNext=0x%08x\n", pmbf->pmbfNext));
        Int3();
        return -1;
    }
    while (pmbf->pmbfNext != NULL)
    {
        if (pmbf->pmbfNext < (PMEMBLOCKFREE)0x10000)
        {
            kprintf(("resCheckAVLTreeFree: invalid pmbfNext pointer, pmbfNext=0x%08x\n", pmbf->pmbfNext));
            Int3();
            return -1;
        }
        if (pmbf->coreFree.Key != pNode->Key)
        {
            kprintf(("resCheckAVLTreeFree: invalid key, pmbf->coreFree.Key=%d, pNode->Key=%d\n", pmbf->coreFree.Key, pNode->Key));
            Int3();
            return -1;
        }
        /* next */
        pmbf = pmbf->pmbfNext;
    }

    if (pNode->pLeft != NULL && resCheckAVLTreeFree(pNode->pLeft) != 0)
        return -1;
    if (pNode->pRight != NULL && resCheckAVLTreeFree(pNode->pRight) != 0)
        return -1;
    return 0;
}
#endif


/**
 * Frees unused heapanchors.
 */
void _res_heapmin(void)
{
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
                rc = D32Hlp_VMFree(phaToBeFreed);
            #else
                rc = DosFreeMem(phaToBeFreed);
            #endif
            if (rc != NO_ERROR)
                kprintf(("_res_heapmin: DosFreeMem/D32Help_VMFree failed for pha=0x%08x, rc = %d\n",
                         pha, rc));
        }
        else
            pha = pha->pPrev;
    }
}


/**
 * Dumps a summary of the subheaps (heapanchor chain).
 */
void _res_dump_subheaps(void)
{
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

        AVLDoWithAll((PPAVLNODECORE)&pha->pmbUsed, 1,
                     (PAVLCALLBACK)_res_dump_subheaps_callback, SSToDS(&UsedParam));
        AVLDoWithAll((PPAVLNODECORE)&pha->pmbFree, 1,
                     (PAVLCALLBACK)_res_dump_subheaps_callback, SSToDS(&FreeParam));

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
}



/**
 * Callbackfunction used by _res_dump_subheaps to summarize a tree.
 */
static unsigned _res_dump_subheaps_callback(PMEMBLOCK pmb, PSUBHEAPS_CALLBACK_PARAM pParam)
{
    pParam->cb += pmb->cbSize;
    pParam->c++;
    return 0;
}


/**
 * Dumps all allocated memory.
 */
void _res_dump_allocated(unsigned cb)
{
    PHEAPANCHOR pha;
    ALLOCATED_CALLBACK_PARAM Param = {cb, 0, 0};

    kprintf(("----------------------------\n"
             "- Dumping allocated blocks -\n"
             "----------------------------\n"));

    pha = phaFirst;
    while (pha != NULL)
    {
        /* iterate thru tree using callback */
        AVLDoWithAll((PPAVLNODECORE)&pha->pmbUsed, TRUE,
                     (PAVLCALLBACK)_res_dump_allocated_callback, SSToDS(&Param));

        /* next */
        pha = pha->pNext;
    }

    kprintf((
             "---------------------------------------------\n"
             " #Blocks=%6d  Allocated=%9d (bytes)\n"
             "---------------------------------------------\n",
             Param.cBlocks, Param.cbAllocated
             ));
}

/**
 * Callback function which dumps a node, and update statistics.
 */
static unsigned _res_dump_allocated_callback(PMEMBLOCK pmb, PALLOCATED_CALLBACK_PARAM pParam)
{
    int i;

    pParam->cBlocks++;
    pParam->cbAllocated += pmb->cbSize;

    kprintf(("pvUserData=0x%08x  cbSize=%6d\n",
             &pmb->achUserData[0], pmb->cbSize
             ));
    /* dump cb of the block */
    i = 0;
    while (i < pParam->cb)
    {
        int j;

        /* hex */
        j = 0;
        while (j < 16)
        {
            if (j+i < pParam->cb && j+i < pmb->cbSize)
                kprintf((" %02x", pmb->achUserData[j+i]));
            else
                kprintf(("   "));
            if (j == 7)
                kprintf((" -"));
            j++;
        }

        /* ascii */
        j = 0;
        kprintf(("  "));
        while (j < 16 && j+i < pmb->cbSize && j+i < pParam->cb)
        {
            kprintf(("%c", pmb->achUserData[j+i] < 0x20 ? '.' : pmb->achUserData[j+i]));
            j++;
        }
        kprintf(("\n"));
        i += j;
    }

    return 0;
}

