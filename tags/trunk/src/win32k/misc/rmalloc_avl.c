/* $Id: rmalloc_avl.c,v 1.1 2000-01-22 18:21:03 bird Exp $
 *
 * Resident Heap - AVL.
 *
 * Note: This heap does very little checking on input.
 *       Use with care! We're running at Ring-0!
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define static
/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#ifdef DEBUG
    #define DEBUG_ALLOC
    #define ALLWAYS_HEAPCHECK
#endif

#define HEAPANCHOR_SIGNATURE    0xBEEFFEEB
#define MEMBLOCK_SIGNATURE      0xFEEBBEEF


/*#define CB_HDR (sizeof(MEMBLOCK) - 1) /* size of MEMBLOCK header (in bytes) */
#define CB_HDR              ((int)&(((PMEMBLOCK)0)->achUserData[0]))
#define PNEXT_BLOCK(a)      ((PMEMBLOCK)((unsigned)(a) + CB_HDR + (a)->cbSize))
#define MEMBLOCKFREE_FROM_FREESIZENODE(a) \
                            ((PMEMBLOCKFREE)((unsigned)(a) - CB_HDR + 4))
#define BLOCKSIZE (1024*256)            /* 256KB */


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
#ifdef RING0
    #include "dev32hlp.h"
    #include "asmutils.h"
#else
    #include <builtin.h>
    #define Int3() __interrupt(3)
#endif
#include "log.h"
#include "rmalloc.h"
#include <memory.h>
#include "dev32.h"
#include "avl.h"
#include "macros.h"


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
static PHEAPANCHOR  phaLast;            /* Pointer to the first anchor block.*/
static unsigned     cbResHeapMax;       /* Maximum amount of memory used by the heap. */

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

    pmbfTmp = (PMEMBLOCKFREE)AVLGetWithParent((PPAVLNODECORE)&pha->pcoreFreeSize,
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
                AVLRemove((PPAVLNODECORE)&pha->pcoreFreeSize, pmbf->coreFree.Key);
            }
            else
            {   /* other nodes of this size: replace pmbf with the first node in the chain. */
                memcpy((void*)&pmbfTmp->pmbfNext->coreFree, (void*)&pmbfTmp->coreFree,
                       sizeof(pmbfTmp->coreFree));
                if (pmbfParent != NULL)
                {
                    pmbfParent = MEMBLOCKFREE_FROM_FREESIZENODE(pmbfParent);
                    if (pmbfTmp->coreFree.Key > pmbfParent->coreFree.Key)
                        pmbfParent->coreFree.pLeft = (PAVLNODECORE)pmbf->pmbfNext;
                    else
                        pmbfParent->coreFree.pRight = (PAVLNODECORE)pmbf->pmbfNext;
                }
                else
                    pha->pcoreFreeSize = (PAVLNODECORE)pmbf->pmbfNext;
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

    pmbfTmp = (PMEMBLOCKFREE)AVLGet((PPAVLNODECORE)&pha->pcoreFreeSize, pmbf->coreFree.Key);
    if (pmbfTmp != NULL)
    {
        pmbfTmp = MEMBLOCKFREE_FROM_FREESIZENODE(pmbfTmp);
        while (pmbfTmp->pmbfNext != NULL && pmbfTmp->pmbfNext > pmbf)
            pmbfTmp = pmbfTmp->pmbfNext;

        pmbf->pmbfNext = pmbfTmp->pmbfNext;
        pmbfTmp->pmbfNext = pmbf;
    }
    else
        AVLInsert((PPAVLNODECORE)&pha->pcoreFreeSize, &pmbf->coreFree);
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
    /* some more work left here... */
    PMEMBLOCKFREE   pmbf = (PMEMBLOCKFREE)pmb;
    PMEMBLOCKFREE   pmbfRight;
    PMEMBLOCKFREE   pmbfRightParent;
    PMEMBLOCKFREE   pmbfLeft;
    PMEMBLOCKFREE   pmbfTmp;

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
        AVLInsert((PPAVLNODECORE)&pha->pmbFree, (PAVLNODECORE)pmbf);
        resInsertIntoFreeSize(pha, pmbf);
    }
    else if (pmbfLeft != NULL && pmbfRight == NULL)
    {   /* 2 - insert left merge */
        resRemoveFromFreeSize(pha, pmbfLeft);
        pmbfLeft->coreFree.Key += CB_HDR + pmbf->coreFree.Key;
        pha->cbFree += CB_HDR;
        resInsertIntoFreeSize(pha, pmbfLeft);
    }
    else if (pmbfLeft == NULL && pmbfRight != NULL)
    {   /* 3 - insert right merge */
        resRemoveFromFreeSize(pha, pmbfRight);
        memcpy((void*)&pmbf->pmbfNext->core, (void*)&pmbfRight->core,
               sizeof(pmbf->core));
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
        resRemoveFromFreeSize(pha, pmbfLeft);
        resRemoveFromFreeSize(pha, pmbfRight);
        pmbfRight = (PMEMBLOCKFREE)AVLRemove((PPAVLNODECORE)&pha->pmbFree,
                                             (AVLKEY)PNEXT_BLOCK(pmb));
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

    cbUserSize = MAX(ALIGN(cbUserSize, 4), sizeof(MEMBLOCKFREE) - CB_HDR);

    *ppha = phaFirst;
    while (*ppha != NULL)
    {
        if ((*ppha)->cbFree >= cbUserSize + CB_HDR)
        {
            PMEMBLOCKFREE pmbfTmp;
            PMEMBLOCKFREE pmbf;

            pmbf = (PMEMBLOCKFREE)AVLGetBestFit(&(*ppha)->pcoreFreeSize, cbUserSize, TRUE);
            if (pmbf != NULL)
            {
                pmbf = MEMBLOCKFREE_FROM_FREESIZENODE(pmbf);
                if (pmbf->pmbfNext == NULL)
                {
                    if (AVLRemove(&(*ppha)->pcoreFreeSize, pmbf->coreFree.Key) == NULL)
                    {
                        kprintf(("resGetFreeMemblock: internal error AVLRemove from the free-size list failed.\n"));
                        continue;
                    }
                }
                else
                {
                    pmbfTmp = pmbf;
                    pmbf = pmbf->pmbfNext;
                    pmbfTmp->pmbfNext = pmbf->pmbfNext;
                }
                AVLRemove((PPAVLNODECORE)&(*ppha)->pmbFree, (AVLKEY)pmbf);
                (*ppha)->cbFree -= pmbf->coreFree.Key;

                /* pmbf is now the block which we are to return, but do we have to split it? */
                if (pmbf->coreFree.Key > sizeof(MEMBLOCKFREE) + cbUserSize)
                {
                    pmbfTmp = (PMEMBLOCKFREE)((unsigned)pmbf + CB_HDR + cbUserSize);
                    #ifdef DEBUG_ALLOC
                        pmbfTmp->ulSignature = MEMBLOCK_SIGNATURE;
                    #endif
                    pmbfTmp->coreFree.Key = pmbf->coreFree.Key - CB_HDR - cbUserSize;
                    pmbf->coreFree.Key = cbUserSize;
                    resInsertFree(*ppha, (PMEMBLOCK)pmbfTmp);
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

            /* anchor block */
            #ifdef DEBUG_ALLOC
                pha->ulSignature = HEAPANCHOR_SIGNATURE;
            #endif
            pha->cbSize = cbBlockSize;
            pha->pmbUsed = NULL;
            pha->cbUsed = 0;
            pha->cbFree = 0;
            pha->pcoreFreeSize = NULL;

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
 * @param     fWithin    When this flag is set, the pointer may point anywhere within the block.
 *                       When clear, it has to point exactly at the start of the user data area.
 */
static PMEMBLOCK resFindUsedBlock(PHEAPANCHOR *ppha, void *pvUser)
{
    if (pvUser != NULL && ppha != NULL)
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
                kprintf(("resFindUsedBlock: Invalid heapanchor signature.\n"));
                return NULL;
            }
        #endif


        pmb = (PMEMBLOCK)AVLGetBestFit((PPAVLNODECORE)&pha->pmbUsed,
                                       (AVLKEY)pvUser, TRUE);
        if (pmb != NULL
            && (unsigned)pmb + pmb->cbSize + CB_HDR > (unsigned)pvUser
            && (unsigned)pmb + CB_HDR >= (unsigned)pvUser
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
int ResHeapInit(unsigned cbSizeInit, unsigned cbSizeMax)
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

    #ifdef ALLWAYS_HEAPCHECK
        if (!_res_heap_check())
        {
            /* error! */
            kprintf(("%s: _res_heap_check failed!\n", "heapInit"));
            Int3();
            return -2;
        }
    #endif
    #ifdef RING3
        fInited = TRUE;
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
    void *pvRet = NULL;

    #ifdef ALLWAYS_HEAPCHECK
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
            pvRet = &pmb->achUserData[0];
        }
    }
    else
    {   /* error! */
        kprintf(("rmalloc: error cbSize = 0\n"));
    }

    return pvRet;
}


/**
 * Reallocate a heapblock.
 * @returns   Pointer to new heapblock.
 * @param     pv     Pointer to the block to realloc.
 * @param     cbNew  The new block size.
 */
void *rrealloc(void *pv, unsigned cbNew)
{
    PMEMBLOCK pmb;
    PHEAPANCHOR pha;

    pmb = resFindUsedBlock(SSToDS(&pha), pv);
    if (pmb != NULL)
    {
        void *pvRet;

        cbNew = ALIGN(cbNew, 4);
        if (cbNew <= pmb->cbSize)
        {   /* shrink block */
            pvRet = pv;
            if (cbNew + sizeof(MEMBLOCKFREE) <= pmb->cbSize)
            {   /* split block */
                PMEMBLOCK pmbNew = (PMEMBLOCK)((unsigned)pmb + CB_HDR + cbNew);
                #ifdef DEBUG_ALLOC
                    pmbNew->ulSignature = MEMBLOCK_SIGNATURE;
                #endif
                pmbNew->cbSize = pmb->cbSize - cbNew - CB_HDR;
                pmb->cbSize = cbNew;
                resInsertFree(pha, pmbNew);
            }
        }
        else
        {   /* expand block - this code may be more optimized... */
            pvRet = rmalloc(cbNew);
            if (pvRet != NULL)
            {
                memcpy(pvRet, pv, pmb->cbSize);
                rfree(pv);
            }
        }
        return pvRet;
    }
    return NULL;
}


/**
 * Frees a block.
 * @param    pv  User pointer.
 */
void rfree(void *pv)
{
    #ifdef ALLWAYS_HEAPCHECK
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
            kprintf(("_msize: _res_heap_check failed!\n"));
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
    PHEAPANCHOR pha;
    PMEMBLOCK   pmb;

    #ifdef ALLWAYS_HEAPCHECK
        if (!_res_heap_check())
            kprintf(("_validptr: _res_heap_check failed!\n"));
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
    PHEAPANCHOR pha;
    PMEMBLOCK   pmb;

    #ifdef ALLWAYS_HEAPCHECK
        if (!_res_heap_check())
            kprintf(("_validptr: _res_heap_check failed!\n"));
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
            kprintf(("res_memfree: _res_heap_check failed!\n"));
    #endif

    for (cb = 0; pha != NULL; pha = pha->pNext)
        cb += pha->cbFree;

    return cb;
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
        PMEMBLOCK pmbFree = (PMEMBLOCK)AVLBeginEnumTree((PPAVLNODECORE)&pha->pmbFree, &FreeEnumData, TRUE);
        PMEMBLOCK pmbFreeNext = (PMEMBLOCK)AVLGetNextNode(&FreeEnumData);
        PMEMBLOCK pmbUsed = (PMEMBLOCK)AVLBeginEnumTree((PPAVLNODECORE)&pha->pmbUsed, &UsedEnumData, TRUE);
        PMEMBLOCK pmbUsedNext = (PMEMBLOCK)AVLGetNextNode(&UsedEnumData);
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
            return FALSE;
        }

        if (pha->cbFree + pha->cbUsed >= pha->cbSize || pha->cbSize == 0)
        {
            kprintf(("_res_heap_check: cbFree + cbUsed >= cbSize for pha=0x%08x\n", pha));
            return FALSE;
        }

        if (pha->pPrev != phaPrev)
        {
            kprintf(("_res_heap_check: error in heap anchor chain.\n"));
            return FALSE;
        }

        if ((unsigned)MINNOTNULL(pmbFree, pmbUsed) != (unsigned)(pha+1))
        {
            kprintf(("_res_heap_check: The first free/used block don't start at start of memory\n"
                     "    block. pmbFree=0x%08x, pmbUsed=0x%08x, pha+1=0x%08x\n",
                     pmbFree, pmbUsed, pha+1));
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
                return FALSE;
                }

            /* check signature and advance to the next block */
            if (pmbUsed != NULL && (pmbFree == NULL || pmbUsed < pmbFree))
            {
                cbSize += CB_HDR + pmbUsed->cbSize;
                cbUsed += pmbUsed->cbSize;
                if (pmbUsed->ulSignature != MEMBLOCK_SIGNATURE)
                    return FALSE;
                pmbLast = pmbUsed;
                pmbUsed = pmbUsedNext;
                pmbUsedNext = (PMEMBLOCK)AVLGetNextNode(&UsedEnumData);
            }
            else
            {
                cbSize += CB_HDR + pmbFree->cbSize;
                cbFree += pmbFree->cbSize;
                if (pmbFree->ulSignature != MEMBLOCK_SIGNATURE)
                    return FALSE;
                pmbLast = pmbFree;
                pmbFree = pmbFreeNext;
                pmbFreeNext = (PMEMBLOCK)AVLGetNextNode(&FreeEnumData);
            }
        }


        /*
         * Check the cbFree and cbUsed.
         */
        if (cbFree != pha->cbFree)
        {
            kprintf(("_res_heap_check: cbFree(%d) != pha->cbFree(%d)\n", cbFree, pha->cbFree));
            return FALSE;
        }

        if (cbUsed != pha->cbUsed)
        {
            kprintf(("_res_heap_check: cbUsed(%d) != pha->cbUsed(%d)\n", cbUsed, pha->cbUsed));
            return FALSE;
        }

        if (cbSize != pha->cbSize)
        {
            kprintf(("_res_heap_check: cbTotal(%d) != pha->cbSize(%d)\n", cbSize, pha->cbSize));
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
        return FALSE;
    }

#endif

    return TRUE;
}



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
                kprintf(("_res_heapmin: DosFreeMem failed for pha=0x%08x, rc = %d\n",
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
        PMEMBLOCK pmb;
        SUBHEAPS_CALLBACK_PARAM FreeParam = {0};
        SUBHEAPS_CALLBACK_PARAM UsedParam = {0};

        AVLDoWithAll((PPAVLNODECORE)&pha->pmbUsed, 1,
                     (PAVLCALLBACK)_res_dump_subheaps_callback, &UsedParam);
        AVLDoWithAll((PPAVLNODECORE)&pha->pmbFree, 1,
                     (PAVLCALLBACK)_res_dump_subheaps_callback, &FreeParam);

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
    ALLOCATED_CALLBACK_PARAM Param = {0};

    kprintf(("----------------------------\n"
             "- Dumping allocated blocks -\n"
             "----------------------------\n"));

    pha = phaFirst;
    while (pha != NULL)
    {
        /* iterate thru tree using callback */
        AVLDoWithAll((PPAVLNODECORE)&pha->pmbUsed, TRUE,
                     (PAVLCALLBACK)_res_dump_allocated_callback, &Param);

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

