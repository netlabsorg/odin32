/* $Id: rmalloc.c,v 1.2 2002-03-31 19:01:18 bird Exp $
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
#ifndef NOFILEID
static const char szFileId[] = "$Id: rmalloc.c,v 1.2 2002-03-31 19:01:18 bird Exp $";
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
#include "dev32.h"

#include <kLib/kTypes.h>
#include <kLib/kAVL.h>
#include <kLib/kLog.h>
#include "kKLrmalloc.h"

#include <memory.h>


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)
typedef struct _MEMBLOCK /* MB */
{
    AVLULNODECORE       core;
    #ifdef DEBUG_ALLOC
    unsigned long       ulSignature;    /* Contains MEMBLOCK_SIGNATURE (0xFEEBBEEF) */
    #endif
    unsigned            cbSize;         /* Size of user space (achBlock)*/
    unsigned char       achUserData[1]; /* User data */
} MEMBLOCK, *PMEMBLOCK;

typedef struct _MEMBLOCKFREE /* MBF */
{
    AVLULNODECORE       core;
    #ifdef DEBUG_ALLOC
    unsigned long       ulSignature;    /* Contains MEMBLOCK_SIGNATURE (0xFEEBBEEF) */
    #endif
    AVLULNODECORE       coreFree;       /* AVLNode core for the tree sorted on address */
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
    PAVLULNODECORE      pcoreFreeSize;  /* Pointer to free-size-tree. */
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
#if 0
static PMEMBLOCK    resFindWithinUsedBlock(PHEAPANCHOR *ppha, void *pvUser);
#endif
#ifdef DEBUG_ALLOC
static int          resCheckAVLTree(PMEMBLOCK pmb);
static int          resCheckAVLTreeFree(PAVLULNODECORE pNode);
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
    KLOGENTRY2("void","PHEAPANCHOR pha, PMEMBLOCK pmb", pha, pmb);
    pha->cbUsed += pmb->cbSize;
    pmb->core.Key = (AVLULKEY)pmb;
    AVLULInsert((PPAVLULNODECORE)&pha->pmbUsed, (PAVLULNODECORE)pmb);
    KLOGEXITVOID();
}


/**
 * Remove a given memblockfree from the free-size-tree.
 */
static void resRemoveFromFreeSize(PHEAPANCHOR pha, PMEMBLOCKFREE pmbf)
{
    KLOGENTRY2("void","PHEAPANCHOR pha, PMEMBLOCKFREE pmbf", pha, pmbf);
    PMEMBLOCKFREE pmbfParent;
    PMEMBLOCKFREE pmbfTmp;

    pmbfTmp = (PMEMBLOCKFREE)AVLULGetWithParent(&pha->pcoreFreeSize,
                                                (PPAVLULNODECORE)SSToDS(&pmbfParent),
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
                kprintf(("internal heap error, pmbf not in list.\n"));
        }
        else
        {   /* pmbf is first in the list */
            if (pmbfTmp->pmbfNext == NULL)
            {   /* no list - no other nodes of this size: simply remove it. */
                AVLULRemove(&pha->pcoreFreeSize, pmbf->coreFree.Key);
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
        kprintf(("internal heap error, free-node not in free-size tree.\n"));
    KLOGEXITVOID();
}


/**
 * Inserts a block into the free-size-tree.
 */
static void resInsertIntoFreeSize(PHEAPANCHOR pha, PMEMBLOCKFREE pmbf)
{
    KLOGENTRY2("void","PHEAPANCHOR pha, PMEMBLOCKFREE pmbf", pha, pmbf);
    PMEMBLOCKFREE pmbfTmp;

    pmbfTmp = (PMEMBLOCKFREE)AVLULGet(&pha->pcoreFreeSize, pmbf->coreFree.Key);
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
        AVLULInsert(&pha->pcoreFreeSize, &pmbf->coreFree);
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
static void resInsertFree(PHEAPANCHOR pha, PMEMBLOCK pmb)
{
    KLOGENTRY2("void","PHEAPANCHOR pha, PMEMBLOCK pmb", pha, pmb);
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
    pmbfRight = (PMEMBLOCKFREE)AVLULGetWithParent((PPAVLULNODECORE)&pha->pmbFree,
                                                  (PPAVLULNODECORE)SSToDS(&pmbfRightParent),
                                                  (AVLULKEY)PNEXT_BLOCK(pmb));
    pmbfLeft = (PMEMBLOCKFREE)AVLULGetBestFit((PPAVLULNODECORE)&pha->pmbFree,
                                              (AVLULKEY)pmbf, FALSE);
    if (pmbfLeft != NULL && (PMEMBLOCKFREE)PNEXT_BLOCK((PMEMBLOCK)pmbfLeft) != pmbf)
        pmbfLeft = NULL;

    if (pmbfLeft == NULL && pmbfRight == NULL)
    {   /* 1 - insert no merge */
        pmbf->core.Key = (AVLULKEY)pmbf;
        AVLULInsert((PPAVLULNODECORE)&pha->pmbFree, (PAVLULNODECORE)pmbf);
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
        pmbf->core.Key = (AVLULKEY)pmbf;
        pmbf->coreFree.Key += CB_HDR + pmbfRight->coreFree.Key;
        pha->cbFree += CB_HDR;
        resInsertIntoFreeSize(pha, pmbf);
        if (pmbfRightParent != NULL)
        {
            if (pmbf < pmbfRightParent)
                pmbfRightParent->core.pLeft = (PAVLULNODECORE)pmbf;
            else
                pmbfRightParent->core.pRight = (PAVLULNODECORE)pmbf;
        }
        else
            pha->pmbFree = (PMEMBLOCK)pmbf;
    }
    else
    {   /* 4 -insert both left and right merge */
        if (AVLULRemove((PPAVLULNODECORE)&pha->pmbFree, (AVLULKEY)pmbfRight) != (PAVLULNODECORE)pmbfRight)
        {
            kprintf(("AVLULRemove on pmbfRight failed.\n"));
            KLOGEXITVOID();
            return;
        }
        resRemoveFromFreeSize(pha, pmbfLeft);
        resRemoveFromFreeSize(pha, pmbfRight);
        pmbfLeft->coreFree.Key += CB_HDR*2 + pmbf->coreFree.Key + pmbfRight->coreFree.Key;
        pha->cbFree += CB_HDR*2;
        resInsertIntoFreeSize(pha, pmbfLeft);
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
static PMEMBLOCK resGetFreeMemblock(PHEAPANCHOR *ppha, unsigned cbUserSize)
{
    KLOGENTRY2("PMEMBLOCK","PHEAPANCHOR * ppha, unsigned cbUserSize", ppha, cbUserSize);
    unsigned cbBlockSize;
    unsigned cbTotalSize = 0;

    #ifdef DEBUG
    cbUserSize = KMAX(KALIGNUP(ALIGNMENT, cbUserSize), sizeof(MEMBLOCKFREE) - CB_HDR);
    #else
    cbUserSize = KMAX(KALIGNUP(ALIGNMENT, cbUserSize + 16), sizeof(MEMBLOCKFREE) - CB_HDR); /* add 16 bytes for safety... */
    #endif

    *ppha = phaFirst;
    while (*ppha != NULL)
    {
        if ((*ppha)->cbFree >= cbUserSize + CB_HDR)
        {
            PMEMBLOCKFREE pmbf;

            pmbf = (PMEMBLOCKFREE)AVLULGetBestFit(&(*ppha)->pcoreFreeSize, cbUserSize, TRUE);
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
                    PMEMBLOCKFREE pmbfTmp = (PMEMBLOCKFREE)AVLULRemove((PPAVLULNODECORE)&(*ppha)->pmbFree, (AVLULKEY)pmbf);
                    if (pmbfTmp != pmbf)
                    {
                        kprintf(("Internal heap error - failed to Remove best fit block!\n"));
                        KLOGEXIT(NULL);
                        return NULL;
                    }
                }
                KLOGEXIT((PMEMBLOCK)pmbf);
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
    cbBlockSize = KALIGNUP(BLOCKSIZE, cbUserSize + CB_HDR * 2);
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

            KLOGEXIT(pmb);
            return pmb;
        }
        else
            kprintf(("Unable to allocate heap memory.\n"));
    }
    else
    {
        kprintf(("Allocation failed, limit reached. \n"
                 "    %d(=cbResHeapMax) < %d(=cbTotalSize) + %d(=cbBlockSize)\n",
                  cbResHeapMax, cbTotalSize, cbBlockSize));
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
static PMEMBLOCK resFindUsedBlock(PHEAPANCHOR *ppha, void *pvUser)
{
    KLOGENTRY2("PMEMBLOCK","PHEAPANCHOR * ppha, void * pvUser", ppha, pvUser);
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
                    kprintf(("Invalid heapanchor signature.\n"));
                    KLOGEXIT(NULL);
                    return NULL;
                }
            #endif
            pmb = (PMEMBLOCK)AVLULGet((PPAVLULNODECORE)&pha->pmbUsed, (AVLULKEY)((unsigned)pvUser - CB_HDR));
            #ifdef DEBUG_ALLOC
                if (pmb != NULL && pmb->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("Invalid memblock signature.\n"));
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
                kprintf(("Invalid heapanchor signature.\n"));
                KLOGEXIT(NULL);
                return NULL;
            }
        #endif


        pmb = (PMEMBLOCK)AVLULGetBestFit((PPAVLULNODECORE)&pha->pmbUsed,
                                         (AVLULKEY)pvUser, TRUE);
        if (pmb != NULL
            && (unsigned)pmb + pmb->cbSize + CB_HDR > (unsigned)pvUser
            && (unsigned)pmb + CB_HDR <= (unsigned)pvUser
            )
        {
            #ifdef DEBUG_ALLOC
                if (pmb->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("Invalid memblock signature.\n"));
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
int resHeapInit(unsigned cbSizeInit, unsigned cbSizeMax)
{
    KLOGENTRY2("int","unsigned cbSizeInit, unsigned cbSizeMax", cbSizeInit, cbSizeMax);
    unsigned  cbSize = KMAX(BLOCKSIZE, cbSizeInit);
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
        INT3();
        KLOGEXIT(-1);
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
            kprintf(("_res_heap_check failed!\n"));
            #ifdef DEBUG
            INT3();
            #endif
            KLOGEXIT(-2);
            return -2;
        }
    #endif
    #ifndef RING0
        fResInited = TRUE;
    #endif
    KLOGEXIT(0);
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
    KLOGENTRY1("void *","unsigned cbSize", cbSize);
    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_res_heap_check())
    {
        kprintf(("_res_heap_check failed!\n"));
        KLOGEXIT(NULL);
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
            KLOGEXIT(&pmb->achUserData[0]);
            return &pmb->achUserData[0];
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
void *rrealloc(void *pv, unsigned cbNew)
{
    KLOGENTRY2("void *","void * pv, unsigned cbNew", pv, cbNew);
    PMEMBLOCK pmb;
    PHEAPANCHOR pha;

    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_res_heap_check())
    {
        kprintf(("_res_heap_check failed!\n"));
        KLOGEXIT(NULL);
        return NULL;
    }
    #endif
    pmb = resFindUsedBlock(SSToDS(&pha), pv);
    if (pmb != NULL)
    {
        void *pvRet;

        cbNew = KMAX(KALIGNUP(ALIGNMENT, cbNew), sizeof(MEMBLOCKFREE) - CB_HDR);
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
                    kprintf(("_res_heap_check failed!\n"));
                    KLOGEXIT(NULL);
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
            PMEMBLOCKFREE pmbfRight = (PMEMBLOCKFREE)AVLULGetWithParent((PPAVLULNODECORE)&pha->pmbFree,
                                                                        (PPAVLULNODECORE)SSToDS(&pmbfRightParent),
                                                                        (AVLULKEY)PNEXT_BLOCK(pmb));
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
                    pmbfNew->core.Key = (AVLULKEY)pmbfNew;
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
                    if (AVLULRemove((PPAVLULNODECORE)&pha->pmbFree, (AVLULKEY)pmbfRight) != (PAVLULNODECORE)pmbfRight)
                    {
                        kprintf(("AVLULRemove failed for pmbfRight - hmm!\n"));
                        KLOGEXIT(NULL);
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
        KLOGEXIT(pvRet);
        return pvRet;
    }
    else
    {
        if (pv == NULL)
        {
            KLOGEXIT(rmalloc(cbNew));
            return rmalloc(cbNew);
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
void rfree(void *pv)
{
    KLOGENTRY1("void","void * pv", pv);
    #if defined(ALLWAYS_HEAPCHECK) || defined(SOMETIMES_HEAPCHECK)
    if (!_res_heap_check())
    {
        kprintf(("_res_heap_check failed!\n"));
        KLOGEXITVOID();
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
            PMEMBLOCK pmb = (PMEMBLOCK)AVLULRemove((PPAVLULNODECORE)&pha->pmbUsed,
                                                   (AVLULKEY)((unsigned)pv - CB_HDR));
            if (pmb != NULL)
            {
                pha->cbUsed -= pmb->cbSize;
                resInsertFree(pha, pmb);
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
unsigned _res_msize(void *pv)
{
    KLOGENTRY1("unsigned","void * pv", pv);
    PHEAPANCHOR pha;
    PMEMBLOCK   pmb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
        kprintf(("_res_heap_check failed!\n"));
    #endif

    pmb = resFindUsedBlock(SSToDS(&pha), pv);
    KLOGEXIT(pmb != NULL ? pmb->cbSize : 0);
    return pmb != NULL ? pmb->cbSize : 0;
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
    KLOGENTRY0("unsigned");
    PHEAPANCHOR pha = phaFirst;
    unsigned    cb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
        kprintf(("_res_heap_check failed!\n"));
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
unsigned    _res_memused(void)
{
    KLOGENTRY0("unsigned");
    PHEAPANCHOR pha = phaFirst;
    unsigned    cb;

    #ifdef ALLWAYS_HEAPCHECK
    if (!_res_heap_check())
        kprintf(("_res_heap_check failed!\n"));
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
 * @param       pState  Pointer to a RES_HEAPSTATE structure which is
 *                      filled upon successful return.
 */
int         _res_state(PHEAPSTATE pState)
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
        pmb = (PMEMBLOCK)AVLULBeginEnumTree((PPAVLULNODECORE)&pha->pmbFree, pEnumData, TRUE);
        while (pmb)
        {
            pState->cBlocksFree++;
            pmb = (PMEMBLOCK)AVLULGetNextNode(pEnumData);
        }

        /* count of used blocks */
        pmb = (PMEMBLOCK)AVLULBeginEnumTree((PPAVLULNODECORE)&pha->pmbUsed, pEnumData, TRUE);
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
int _res_heap_check(void)
{
    KLOGENTRY0("int");
#ifdef DEBUG_ALLOC
    PHEAPANCHOR pha = phaFirst;
    PHEAPANCHOR phaPrev = NULL;


    while (pha != NULL)
    {
        AVLULENUMDATA FreeEnumData;
        AVLULENUMDATA UsedEnumData;
        PMEMBLOCK pmbFree = (PMEMBLOCK)AVLULBeginEnumTree((PPAVLULNODECORE)&pha->pmbFree,
                                                        SSToDS(&FreeEnumData), TRUE);
        PMEMBLOCK pmbFreeNext = (PMEMBLOCK)AVLULGetNextNode(SSToDS(&FreeEnumData));
        PMEMBLOCK pmbUsed = (PMEMBLOCK)AVLULBeginEnumTree((PPAVLULNODECORE)&pha->pmbUsed,
                                                        SSToDS(&UsedEnumData), TRUE);
        PMEMBLOCK pmbUsedNext = (PMEMBLOCK)AVLULGetNextNode(SSToDS(&UsedEnumData));
        PMEMBLOCK pmbLast = NULL;
        unsigned  cbFree = 0;
        unsigned  cbUsed = 0;
        unsigned  cbSize = sizeof(*pha);

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

        if (pha->cbFree + pha->cbUsed >= pha->cbSize || pha->cbSize == 0)
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

        if ((unsigned)KMINNOTNULL(pmbFree, pmbUsed) != (unsigned)(pha+1))
        {
            kprintf(("The first free/used block don't start at start of memory\n"
                     "    block. pmbFree=0x%08x, pmbUsed=0x%08x, pha+1=0x%08x\n",
                     pmbFree, pmbUsed, pha+1));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }

        /*
         * Check the tree AVL-trees - !extra debug test!
         */
        if (resCheckAVLTree(pha->pmbUsed) != 0)
        {
            kprintf(("the Used tree is invalid.\n"));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }
        if (resCheckAVLTree(pha->pmbFree) != 0)
        {
            kprintf(("the Free tree is invalid.\n"));
            INT3();
            KLOGEXIT(FALSE);
            return FALSE;
        }
        if (resCheckAVLTreeFree(pha->pcoreFreeSize) != 0)
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
                kprintf(("internal error - memory hole!\n"));
                INT3();
                KLOGEXIT(FALSE);
                return FALSE;
                }

            /* check signature and advance to the next block */
            if (pmbUsed != NULL && (pmbFree == NULL || pmbUsed < pmbFree))
            {
                cbSize += CB_HDR + pmbUsed->cbSize;
                cbUsed += pmbUsed->cbSize;
                if (pmbUsed->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("invalid signature on used block, pmbUsed=0x%08x\n", pmbUsed));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }
                if (pmbUsed->cbSize < sizeof(MEMBLOCKFREE) - CB_HDR)
                {
                    kprintf(("internal error - cbSize is to small! (used), cbSize=%d\n", pmbUsed->cbSize));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }
                pmbLast = pmbUsed;
                pmbUsed = pmbUsedNext;
                pmbUsedNext = (PMEMBLOCK)AVLULGetNextNode(SSToDS(&UsedEnumData));
            }
            else
            {
                PMEMBLOCKFREE pmbf = (PMEMBLOCKFREE)AVLULGet(&pha->pcoreFreeSize, pmbFree->cbSize);
                if (pmbf != NULL)
                {
                    pmbf = MEMBLOCKFREE_FROM_FREESIZENODE(pmbf);
                    while (pmbf != NULL && pmbf != (PMEMBLOCKFREE)pmbFree)
                        pmbf = pmbf->pmbfNext;
                    if (pmbf == NULL)
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

                cbSize += CB_HDR + pmbFree->cbSize;
                cbFree += pmbFree->cbSize;
                if (pmbFree->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("invalid signature on free block, pmbUsed=0x%08x\n", pmbFree));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }
                if (pmbFree->cbSize < sizeof(MEMBLOCKFREE) - CB_HDR)
                {
                    kprintf(("internal error - cbSize is to small! (free), cbSize=%d\n", pmbFree->cbSize));
                    INT3();
                    KLOGEXIT(FALSE);
                    return FALSE;
                }
                pmbLast = pmbFree;
                pmbFree = pmbFreeNext;
                pmbFreeNext = (PMEMBLOCK)AVLULGetNextNode(SSToDS(&FreeEnumData));
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
        if (pmbLast == NULL || (unsigned)pha + pha->cbSize != (unsigned)PNEXT_BLOCK(pmbLast))
        {
            kprintf(("The last free/used block dont end at the end of memory block.\n"
                     "    pmbLast(end)=0x%08x, pha+pha->cbSize=0x%08x\n",
                     pmbLast != NULL ? PNEXT_BLOCK(pmbLast) : NULL, (unsigned)pha + pha->cbSize));
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
static int resCheckAVLTree(PMEMBLOCK pmb)
{
    KLOGENTRY1("int","PMEMBLOCK pmb", pmb);
    if (pmb == NULL)
    {
        KLOGEXIT(0);
        return 0;
    }
    if (pmb->core.Key != (AVLULKEY)pmb)
    {
        kprintf(("Invalid Key!\n"));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->core.Key % ALIGNMENT != 0)
    {
        kprintf(("Data is not correctly aligned, uData=0x%08x\n", pmb->core.Key));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->core.pLeft != NULL && (pmb->core.pLeft < (PAVLULNODECORE)0x10000 || pmb->core.Key <= (AVLULKEY)pmb->core.pLeft))
    {
        kprintf(("Invalid pLeft!\n"));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->core.pRight != NULL && (pmb->core.pRight < (PAVLULNODECORE)0x10000 || pmb->core.Key >= (AVLULKEY)pmb->core.pRight))
    {
        kprintf(("Invalid pRight!\n"));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->core.pLeft != NULL && resCheckAVLTree((PMEMBLOCK)pmb->core.pLeft) != 0)
    {
        KLOGEXIT(-1);
        return -1;
    }
    if (pmb->core.pRight != NULL && resCheckAVLTree((PMEMBLOCK)pmb->core.pRight) != 0)
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
static int resCheckAVLTreeFree(PAVLULNODECORE pNode)
{
    KLOGENTRY1("int","PAVLULNODECORE pNode", pNode);
    PMEMBLOCKFREE pmbf;
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
    pmbf = MEMBLOCKFREE_FROM_FREESIZENODE(pNode);
    if (pmbf->pmbfNext != NULL && pmbf->pmbfNext < (PMEMBLOCKFREE)0x10000)
    {
        kprintf(("invalid pmbfNext pointer, pmbfNext=0x%08x\n", pmbf->pmbfNext));
        INT3();
        KLOGEXIT(-1);
        return -1;
    }
    while (pmbf->pmbfNext != NULL)
    {
        if (pmbf->pmbfNext < (PMEMBLOCKFREE)0x10000)
        {
            kprintf(("invalid pmbfNext pointer, pmbfNext=0x%08x\n", pmbf->pmbfNext));
            INT3();
            KLOGEXIT(-1);
            return -1;
        }
        if (pmbf->coreFree.Key != pNode->Key)
        {
            kprintf(("invalid key, pmbf->coreFree.Key=%d, pNode->Key=%d\n", pmbf->coreFree.Key, pNode->Key));
            INT3();
            KLOGEXIT(-1);
            return -1;
        }
        /* next */
        pmbf = pmbf->pmbfNext;
    }

    if (pNode->pLeft != NULL && resCheckAVLTreeFree(pNode->pLeft) != 0)
    {
        KLOGEXIT(-1);
        return -1;
    }
    if (pNode->pRight != NULL && resCheckAVLTreeFree(pNode->pRight) != 0)
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
void _res_heapmin(void)
{
    KLOGENTRY0("void");
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
                kprintf(("DosFreeMem/D32Help_VMFree failed for pha=0x%08x, rc = %d\n",
                         pha, rc));
        }
        else
            pha = pha->pPrev;
    }
    KLOGEXITVOID();
}


/**
 * Dumps a summary of the subheaps (heapanchor chain).
 */
void _res_dump_subheaps(void)
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

        AVLULDoWithAll((PPAVLULNODECORE)&pha->pmbUsed, 1,
                       (PAVLULCALLBACK)_res_dump_subheaps_callback, SSToDS(&UsedParam));
        AVLULDoWithAll((PPAVLULNODECORE)&pha->pmbFree, 1,
                       (PAVLULCALLBACK)_res_dump_subheaps_callback, SSToDS(&FreeParam));

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
 * Callbackfunction used by _res_dump_subheaps to summarize a tree.
 */
static unsigned _res_dump_subheaps_callback(PMEMBLOCK pmb, PSUBHEAPS_CALLBACK_PARAM pParam)
{
    KLOGENTRY2("unsigned","PMEMBLOCK pmb, PSUBHEAPS_CALLBACK_PARAM pParam", pmb, pParam);
    pParam->cb += pmb->cbSize;
    pParam->c++;
    KLOGEXIT(0);
    return 0;
}


/**
 * Dumps all allocated memory.
 */
void _res_dump_allocated(unsigned cb)
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
        AVLULDoWithAll((PPAVLULNODECORE)&pha->pmbUsed, TRUE,
                       (PAVLULCALLBACK)_res_dump_allocated_callback, SSToDS(&Param));

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
static unsigned _res_dump_allocated_callback(PMEMBLOCK pmb, PALLOCATED_CALLBACK_PARAM pParam)
{
    KLOGENTRY2("unsigned","PMEMBLOCK pmb, PALLOCATED_CALLBACK_PARAM pParam", pmb, pParam);
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

    KLOGEXIT(0);
    return 0;
}

