/* $Id: rmalloc.c,v 1.4 2000-09-02 21:08:14 bird Exp $
 *
 * Resident Heap.
 *
 * Note: This heap does very little checking on input.
 *       Use with care! We're running at Ring-0!
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define static
/******************************************************************************
*  Defined macros and constants
******************************************************************************/
#ifdef DEBUG
    #define DEBUG_ALLOC
    #undef ALLWAYS_HEAPCHECK
#endif

#define HEAPANCHOR_SIGNATURE    0xBEEFFEEB
#define MEMBLOCK_SIGNATURE      0xFEEBBEEF


/*#define CB_HDR (sizeof(MEMBLOCK) - 1) /* size of MEMBLOCK header (in bytes) */
#define CB_HDR              ((int)&(((PMEMBLOCK)0)->achUserData[0]))
#define PNEXT_BLOCK(a)      ((PMEMBLOCK)((unsigned)(a) + CB_HDR + (a)->cbSize))
#define BLOCKSIZE (1024*256)            /* 256KB */


#define MAX(a, b)           (((a) > (b))? (a) : (b))
#define ALIGN(a, alignment) (((a) + (alignment - 1UL)) & ~(alignment - 1UL))
                                        /* aligns something, a,  up to nearest alignment boundrary-
                                         * Note: Aligment must be a 2**n number. */


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
    #include "asmutils.h"
#else
    #include <builtin.h>
    #define Int3() __interrupt(3)
#endif
#include "log.h"
#include "rmalloc.h"
#include <memory.h>
#include "dev32.h"
#include "macros.h"


/******************************************************************************
*  Structs and Typedefs
******************************************************************************/
typedef struct _MEMBLOCK /* MB */
{
    #ifdef DEBUG_ALLOC
    unsigned long       ulSignature;    /* Contains MEMBLOCK_SIGNATURE (0xFEEBBEEF) */
    #endif
    unsigned            cbSize;         /* Size of user space (achBlock)*/
    struct _MEMBLOCK *  pNext;          /* Pointer to the next memblock. */
    unsigned char       achUserData[1]; /* User data */
} MEMBLOCK, *PMEMBLOCK;


typedef struct _HeapAnchor /* HA */
{
    #ifdef DEBUG_ALLOC
    unsigned long       ulSignature;    /* Contains HEAPANCHOR_SIGNATURE */
    #endif
    unsigned            cbSize;         /* Total amount of memory in this block. */
    struct _HeapAnchor *pNext;          /* Pointer to the next anchor block. */
    struct _HeapAnchor *pPrev;          /* Pointer to the previous anchor block. */
    PMEMBLOCK           pmbFree;        /* Pointer to the used-memblock chain. */
    unsigned            cbFree;         /* Amount of free memory. */
    PMEMBLOCK           pmbUsed;        /* Pointer to the used-memblock chain. */
    unsigned            cbUsed;         /* Amount of used memory. */

} HEAPANCHOR, *PHEAPANCHOR;




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
static void         resInsertFree(PHEAPANCHOR pha, PMEMBLOCK pmb);
static PMEMBLOCK    resGetFreeMemblock(PHEAPANCHOR *ppha, unsigned cbUserSize);
static PMEMBLOCK    resFindUsedBlock(PHEAPANCHOR *ppha, void *pvUser, int fWithin);


/**
 * Inserts a memblock into the used chain.
 * @param    pha  Pointer to the heap anchor which the block is to be inserted into.
 * @param    pmb  Pointer to memblock to insert into the free list.
 * @remark   Sorts on address.
 */
static void resInsertUsed(PHEAPANCHOR pha, PMEMBLOCK pmb)
{
    if (pha->pmbUsed == NULL || pha->pmbUsed > pmb)
    {
        pmb->pNext = pha->pmbUsed;
        pha->pmbUsed = pmb;
    }
    else
    {
        PMEMBLOCK pmbTmp = pha->pmbUsed;
        while (pmbTmp->pNext != NULL && pmbTmp->pNext < pmb)
            pmbTmp = pmbTmp->pNext;

        pmb->pNext = pmbTmp->pNext;
        pmbTmp->pNext= pmb;
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
    pha->cbFree += pmb->cbSize;
    if (pmb < pha->pmbFree || pha->pmbFree == NULL)
    {
        /* test for merge with first block */
        if (pha->pmbFree != NULL && PNEXT_BLOCK(pmb) == pha->pmbFree)
        {
            pha->cbFree += CB_HDR;
            pmb->cbSize += CB_HDR + pha->pmbFree->cbSize;
            pmb->pNext = pha->pmbFree->pNext;
            #ifdef DEBUG_ALLOC
                pha->pmbFree->ulSignature = 0xF0EEEE0F;
                pha->pmbFree->pNext = (void*)~0;
            #endif
        }
        else
           pmb->pNext = pha->pmbFree;
        pha->pmbFree = pmb;
    }
    else
    {
        PMEMBLOCK pmbTmp = pha->pmbFree;
        while (pmbTmp->pNext != NULL && pmbTmp->pNext < pmb)
            pmbTmp = pmbTmp->pNext;

        /* test for merge with left block */
        if (PNEXT_BLOCK(pmbTmp) == pmb)
        {
            pmbTmp->cbSize += CB_HDR + pmb->cbSize;
            pha->cbFree += CB_HDR;
            #ifdef DEBUG_ALLOC
                pmb->ulSignature = 0xF0EEEE0F;
                pmb->pNext = (void*)~0;
            #endif
            pmb = pmbTmp;
        }
        else
        {
            pmb->pNext = pmbTmp->pNext;
            pmbTmp->pNext = pmb;
        }

        /* test for merge with right block */
        if (pmb->pNext != NULL && PNEXT_BLOCK(pmb) == pmb->pNext)
        {
            pmb->cbSize += CB_HDR + pmb->pNext->cbSize;
            pha->cbFree += CB_HDR;
            #ifdef DEBUG_ALLOC
                pmb->pNext->ulSignature = 0xF0EEEE0F;
            #endif
            pmb->pNext = pmb->pNext->pNext;
        }
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

    cbUserSize = (cbUserSize + 3) & ~3;
    *ppha = phaFirst;
    while (*ppha != NULL)
    {
        if ((*ppha)->cbFree >= cbUserSize + CB_HDR)
        {
            PMEMBLOCK pmbBestFit     = NULL;
            PMEMBLOCK pmbBestFitPrev = NULL;
            PMEMBLOCK pmbCur  = (*ppha)->pmbFree;
            PMEMBLOCK pmbPrev = NULL;

            /* search for block */
            while (pmbCur != NULL)
            {
                /* check for perfect match first */
                if (pmbCur->cbSize == cbUserSize)
                    break;
                    /* TODO: The following test may need to be adjusted later. */
                else if (pmbCur->cbSize >= cbUserSize
                        && (pmbBestFit == NULL || pmbCur->cbSize < pmbBestFit->cbSize)
                        )
                {
                    pmbBestFit = pmbCur;
                    pmbBestFitPrev = pmbPrev;
                }

                /* next */
                pmbPrev = pmbCur;
                pmbCur = pmbCur->pNext;
            }

            /* link out block */
            if (pmbCur != NULL)
            {   /* prefect match */
                if (pmbPrev != NULL)
                    pmbPrev->pNext = pmbCur->pNext;
                else
                    (*ppha)->pmbFree = pmbCur->pNext;

                (*ppha)->cbFree -= cbUserSize;
                (*ppha)->cbUsed += cbUserSize;
            }
            else if (pmbBestFit != NULL)
            {   /* best fit */
                /* two cases 1) split block. 2) block is too small to be splitted. */
                if (pmbBestFit->cbSize > cbUserSize + CB_HDR)
                {
                    pmbCur = (PMEMBLOCK)((unsigned)pmbBestFit + pmbBestFit->cbSize - cbUserSize);
                    #ifdef DEBUG_ALLOC
                        pmbCur->ulSignature = MEMBLOCK_SIGNATURE;
                    #endif
                    pmbCur->cbSize = cbUserSize;
                    pmbBestFit->cbSize -= cbUserSize + CB_HDR;

                    (*ppha)->cbFree -= cbUserSize + CB_HDR;
                    (*ppha)->cbUsed += cbUserSize;
                }
                else
                {
                    if (pmbBestFitPrev != NULL)
                        pmbBestFitPrev->pNext = pmbBestFit->pNext;
                    else
                        (*ppha)->pmbFree = pmbBestFit->pNext;
                    pmbCur = pmbBestFit;

                    (*ppha)->cbFree -= pmbCur->cbSize;
                    (*ppha)->cbUsed += pmbCur->cbSize;
                }
            }

            if (pmbCur != NULL)
                return pmbCur;
        }

        cbTotalSize += (*ppha)->cbSize;

        /* next heap anchor */
        *ppha = (*ppha)->pNext;
    }

    /* add a new heap anchor? */
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
            pha->cbUsed = cbUserSize;

            /* free memblock */
            pha->pmbFree = (PMEMBLOCK)((unsigned)pha + sizeof(*pha));
            pha->cbFree = cbBlockSize - sizeof(*pha) - CB_HDR * 2 - cbUserSize;
            #ifdef DEBUG_ALLOC
                pha->pmbFree->ulSignature = MEMBLOCK_SIGNATURE;
            #endif
            pha->pmbFree->cbSize = pha->cbFree;
            pha->pmbFree->pNext = NULL;

            /* used memblock */
            pmb = (PMEMBLOCK)((unsigned)pha + cbBlockSize - cbUserSize - CB_HDR);
            #ifdef DEBUG_ALLOC
                pmb->ulSignature = MEMBLOCK_SIGNATURE;
            #endif
            pmb->cbSize = cbUserSize;
            pmb->pNext = NULL;

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
 * @param     pvUser     User pointer to find the block to.
 * @param     fWithin    When this flag is set, the pointer may point anywhere within the block.
 *                       When clear, it has to point exactly at the start of the user data area.
 */
static PMEMBLOCK resFindUsedBlock(PHEAPANCHOR *ppha, void *pvUser, int fWithin)
{
    PMEMBLOCK pmb;

    if (pvUser != NULL && ppha != NULL)
    {
        register PHEAPANCHOR pha = phaFirst;
        while (pha != NULL
               && !((unsigned)pvUser > (unsigned)pha
                    && (unsigned)pvUser < (unsigned)pha + pha->cbSize))
            pha = pha->pNext;

        if (pha != NULL)
        {
            #ifdef DEBUG_ALLOC
                if (pha->ulSignature != HEAPANCHOR_SIGNATURE)
                {
                    kprintf(("resFindUsedBlock: Invalid heapanchor signature.\n"));
                    return NULL;
                }
            #endif
            *ppha = pha;
            pmb = pha->pmbUsed;

            if (fWithin)
            {
                while (pmb != NULL && !(pvUser >= (void*)pmb && pvUser < (void*)PNEXT_BLOCK(pmb)))
                    pmb = pmb->pNext;
            }
            else
            {
                pvUser = (void*)((unsigned)pvUser - CB_HDR);
                while (pmb != NULL && pvUser != (void*)pmb)
                    pmb = pmb->pNext;
            }
            #ifdef DEBUG_ALLOC
                if (pmb != NULL && pmb->ulSignature != MEMBLOCK_SIGNATURE)
                {
                    kprintf(("resFindUsedBlock: Invalid memoryblock signature.\n"));
                    pmb = NULL;
                }
            #endif
        }
        else
            pmb = NULL;
    }
    else
        pmb = NULL;

    return pmb;
}


/**
 * Initiate the heap "subsystem".
 * @returns   0 on success, not 0 on error.
 * @param     cbSizeInit  The initial size of the heap in bytes.
 * @param     cbSizeMax   Maximum heapsize in bytes.
 */
int resHeapInit(unsigned cbSizeInit, unsigned cbSizeMax)
{
    unsigned cbSize = MAX(BLOCKSIZE, cbSizeInit);

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
    phaFirst->pmbUsed = NULL;
    phaFirst->cbUsed = 0UL;
    phaFirst->pmbFree = (PMEMBLOCK)((unsigned)phaFirst+sizeof(*phaFirst));
    phaFirst->cbFree = cbSize - sizeof(*phaFirst) - CB_HDR;

    #ifdef DEBUG_ALLOC
        phaFirst->pmbFree->ulSignature = MEMBLOCK_SIGNATURE;
    #endif
    phaFirst->pmbFree->cbSize = phaFirst->cbFree;
    phaFirst->pmbFree->pNext = NULL;
    phaLast = phaFirst;

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

    pmb = resFindUsedBlock(SSToDS(&pha), pv, FALSE);
    if (pmb != NULL)
    {
        void *pvRet;

        cbNew = ALIGN(cbNew, 4);
        if (cbNew <= pmb->cbSize)
        {   /* shrink block */
            pvRet = pv;
            if (cbNew + CB_HDR < pmb->cbSize)
            {   /* split block */
                PMEMBLOCK pmbNew = (PMEMBLOCK)((unsigned)pmb + CB_HDR + cbNew);
                #ifdef DEBUG_ALLOC
                    pmbNew->ulSignature = MEMBLOCK_SIGNATURE;
                #endif
                pmbNew->cbSize = pmb->cbSize - cbNew - CB_HDR;
                pmbNew->pNext = NULL;
                pha->cbUsed -= pmb->cbSize - cbNew;
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
            PMEMBLOCK pmbCur  = pha->pmbUsed;
            PMEMBLOCK pmbPrev = NULL;
            pv = (void*)((unsigned)pv - CB_HDR);

            while (pmbCur != NULL &&
            #ifdef DEBUG_ALLOC /* pointer within block */
                   !(pv >= (void*)pmbCur && (void*)((unsigned)pv + CB_HDR) < (void*)PNEXT_BLOCK(pmbCur))
            #else
                   pv != (void*)pmbCur
            #endif
                   )
            {
                pmbPrev = pmbCur;
                pmbCur = pmbCur->pNext;
            }

            if (pmbCur != NULL)
            {
                #ifdef DEBUG_ALLOC
                    if (pmbCur->ulSignature != MEMBLOCK_SIGNATURE)
                    {
                        kprintf(("rfree: Invalid memoryblock signature - pmbCur\n"));
                        return;
                    }
                    if (pmbPrev != NULL && pmbPrev->ulSignature != MEMBLOCK_SIGNATURE)
                    {
                        kprintf(("rfree: Invalid memoryblock signature - pmbPrev\n"));
                        return;
                    }
                #endif
                if (pv == pmbCur)
                {
                    if (pmbPrev != NULL)
                        pmbPrev->pNext = pmbCur->pNext;
                    else
                        pha->pmbUsed = pmbCur->pNext;
                    pha->cbUsed -= pmbCur->cbSize;

                    resInsertFree(pha, pmbCur);

                    #ifdef ALLWAYS_HEAPCHECK
                        if (!_res_heap_check())
                            kprintf(("rfree: _res_heap_check failed 3!\n"));
                    #endif
                }
                else
                    kprintf(("rfree: pv is not pointing to start of block.\n"));
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

    pmb = resFindUsedBlock(SSToDS(&pha), pv, FALSE);
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

    pmb = resFindUsedBlock(SSToDS(&pha), pv, TRUE);
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

    pmb = resFindUsedBlock(SSToDS(&pha), pv, TRUE);
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
        PMEMBLOCK pmbFree = pha->pmbFree;
        PMEMBLOCK pmbUsed = pha->pmbUsed;
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
            kprintf(("_res_heap_check: internal error - error in heap anchor chain.\n"));
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
            if (!( (pmbUsed != NULL && PNEXT_BLOCK(pmbUsed) == pmbUsed->pNext)    /* 1.*/
                || (pmbUsed != NULL && PNEXT_BLOCK(pmbUsed) == pmbFree)           /* 2.*/
                || (pmbFree != NULL && PNEXT_BLOCK(pmbFree) == pmbFree->pNext)    /* 3.*/
                || (pmbFree != NULL && PNEXT_BLOCK(pmbFree) == pmbUsed)           /* 4.*/
                || (pmbUsed != NULL && pmbFree == NULL && pmbUsed->pNext == NULL) /* 5.*/
                || (pmbFree != NULL && pmbUsed == NULL && pmbFree->pNext == NULL) /* 6.*/
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
                pmbUsed = pmbUsed->pNext;
            }
            else
            {
                cbSize += CB_HDR + pmbFree->cbSize;
                cbFree += pmbFree->cbSize;
                if (pmbFree->ulSignature != MEMBLOCK_SIGNATURE)
                    return FALSE;
                pmbLast = pmbFree;
                pmbFree = pmbFree->pNext;
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
        unsigned    cbFree = 0;
        unsigned    cFree = 0;
        unsigned    cbUsed = 0;
        unsigned    cUsed = 0;


        pmb = pha->pmbUsed;
        while (pmb != NULL)
        {
            cUsed++;
            cbUsed += pmb->cbSize;
            pmb = pmb->pNext;
        }

        pmb = pha->pmbFree;
        while (pmb != NULL)
        {
            cFree++;
            cbFree += pmb->cbSize;
            pmb = pmb->pNext;
        }

        kprintf(("HeapAnchor %2d addr=0x%08x cbSize=%6d cbFree=%6d cbUsed=%6d cUsed=%4d cFree=%d\n",
                 i, pha, pha->cbSize, pha->cbFree, pha->cbUsed, cUsed, cFree));

        cTotalFree += cFree;
        cbTotalFree += cbFree;
        cTotalUsed += cUsed;
        cbTotalUsed += cbUsed;

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
 * Dumps all allocated memory.
 */
void _res_dump_allocated(unsigned cb)
{
    PHEAPANCHOR pha;
    unsigned    cBlocks;
    unsigned    cbAllocated;

    kprintf(("----------------------------\n"
             "- Dumping allocated blocks -\n"
             "----------------------------\n"));

    cBlocks = 0;
    cbAllocated = 0;
    pha = phaFirst;
    while (pha != NULL)
    {
        PMEMBLOCK pmb = pha->pmbUsed;
        while (pmb != NULL)
        {
            int i;
            cBlocks++;
            cbAllocated += pmb->cbSize;

            kprintf(("pvUserData=0x%08x  cbSize=%6d\n",
                     &pmb->achUserData[0], pmb->cbSize
                     ));
            /* dump cb of the block */
            i = 0;
            while (i < cb)
            {
                int j;

                /* hex */
                j = 0;
                while (j < 16)
                {
                    if (j+i < cb && j+i < pmb->cbSize)
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
                while (j < 16 && j+i < pmb->cbSize && j+i < cb)
                {
                    kprintf(("%c", pmb->achUserData[j+i] < 0x20 ? '.' : pmb->achUserData[j+i]));
                    j++;
                }
                kprintf(("\n"));
                i += j;
            }

            /* next */
            pmb = pmb->pNext;
        }

        /* next */
        pha = pha->pNext;
    }

    kprintf((
             "---------------------------------------------\n"
             " #Blocks=%6d  Allocated=%9d (bytes)\n"
             "---------------------------------------------\n",
             cBlocks, cbAllocated
             ));

}
