/* $Id: malloc_old.c,v 1.2 2000-01-24 18:19:00 bird Exp $
 *
 * Heap.
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
/******************************************************************************
*  Defined macros and constants
******************************************************************************/
#ifdef DEBUG
    #define DEBUG_ALLOC
#endif

#define SIGNATURE 0xBEEFFEEB
/*#define CB_HDR (sizeof(MEMBLOCK) - 1) /* size of MEMBLOCK header (in bytes) */
#define CB_HDR (int)&(((PMEMBLOCK)0)->achUserData[0])
#define PNEXT_BLOCK(a) ((PMEMBLOCK)((unsigned)(a) + CB_HDR + (a)->cbSize))

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
#endif
#include "asmutils.h"
#include "log.h"
#include "malloc.h"
#include <memory.h>


/******************************************************************************
*  Structs and Typedefs
******************************************************************************/
#pragma pack(1)
typedef struct _MEMBLOCK /* MB */
{
#ifdef DEBUG_ALLOC
   unsigned long     ulSignature;   /* should be SIGNATURE (0xBEEFFEEB) */
#endif
   unsigned          cbSize;        /* size of user space (achBlock)*/
   struct _MEMBLOCK *pNext;
   unsigned char     achUserData[1];
} MEMBLOCK, *PMEMBLOCK;
#pragma pack()

/******************************************************************************
*  Global data
******************************************************************************/
/*#pragma info(nogen, nouni, noext)*/
static PMEMBLOCK   pUsed;         /* pointer to the used memblock chain. */
static PMEMBLOCK   pFree;         /* pointer to the free memblock chain. */
static unsigned    cbFree;        /* bytes of free user memory in the heap.*/
unsigned           _uHeapMinPtr;  /* heap pointers are greater or equal to this.*/
unsigned           _uHeapMaxPtr;  /* heap pointers are less than this. */
#ifndef RING0
    char           fInited;       /* init flag */
#endif

/******************************************************************************
*  Internal functions
******************************************************************************/
static void         insertUsed(PMEMBLOCK pMemblock);
static void         insertFree(PMEMBLOCK pMemblock);
static PMEMBLOCK    getFreeMemblock(unsigned cbUserSize);
static PMEMBLOCK    findBlock(PMEMBLOCK pMemblock, void *pvUser, int fWithin);


/**
 * Inserts a memblock into the used chain
 * @param    pMemblock  Pointer to memblock which is to inserted.
 * @remark   Sorts on address.
 */
static void insertUsed(PMEMBLOCK pMemblock)
{
    if (pUsed == NULL || pUsed > pMemblock)
    {
        pMemblock->pNext = pUsed;
        pUsed = pMemblock;
    }
    else
    {
        PMEMBLOCK pMb = pUsed;
        while (pMb->pNext != NULL && pMb->pNext < pMemblock)
            pMb = pMb->pNext;

        pMemblock->pNext = pMb->pNext;
        pMb->pNext= pMemblock;
    }
}


/**
 * Inserts a memblock into the free chain.
 * Merges blocks adjecent blocks.
 * @param    pMemblock  Pointer to memblock to insert into the free list.
 * @remark   Sorts on address.
 */
static void insertFree(PMEMBLOCK pMemblock)
{
    cbFree += pMemblock->cbSize;
    if (pMemblock < pFree || pFree == NULL)
    {
        /* test for merge with 2nd block */
        if (pFree != NULL && PNEXT_BLOCK(pMemblock) == pFree)
        {
            cbFree += pMemblock->cbSize + CB_HDR;
            pFree->cbSize += CB_HDR;
            #ifdef DEBUG_ALLOC
                pMemblock->ulSignature = 0xF0EEEE0F;
            #endif
        }
        else
        {
           pMemblock->pNext = pFree;
           pFree = pMemblock;

        }
    }
    else
    {
        PMEMBLOCK pMb = pFree;
        while (pMb->pNext != NULL && pMb->pNext < pMemblock)
            pMb = pMb->pNext;

        /* test for merge with left block */
        if (PNEXT_BLOCK(pMb) == pMemblock)
        {
            pMb->cbSize += CB_HDR + pMemblock->cbSize;
            cbFree += CB_HDR;
            #ifdef DEBUG_ALLOC
                pMemblock->ulSignature = 0xF0EEEE0F;
            #endif
            pMemblock = pMb;
        }
        else
        {
            pMemblock->pNext = pMb->pNext;
            pMb->pNext = pMemblock;
        }

        /* test for merge with right block */
        if (PNEXT_BLOCK(pMemblock) == pMemblock->pNext && pMemblock->pNext != NULL)
        {
            pMemblock->cbSize += CB_HDR + pMemblock->pNext->cbSize;
            cbFree += CB_HDR;
            #ifdef DEBUG_ALLOC
                pMemblock->pNext->ulSignature = 0xF0EEEE0F;
            #endif
            pMemblock->pNext = pMemblock->pNext->pNext;
        }
    }
}


/**
 * Finds a free block at the requested size.
 * @returns  Pointer to block (not in free list any longer).
 * @param    cbUserSize  Bytes the user have requested.
 * @sketch   cbUserSize is aligned to nearest 4 bytes.
 *
 *
 */
static PMEMBLOCK getFreeMemblock(unsigned cbUserSize)
{
    PMEMBLOCK pBestFit     = NULL;
    PMEMBLOCK pBestFitPrev = NULL;
    PMEMBLOCK pCur  = pFree;
    PMEMBLOCK pPrev = NULL;

    cbUserSize = (cbUserSize + 3) & ~3;

    /* search for block */
    while (pCur != NULL)
    {
        /* check for perfect match first */
        if (pCur->cbSize == cbUserSize)
            break;
            /* TODO: The following test may need to be adjusted later. */
        else if (pCur->cbSize >= cbUserSize
                && (pBestFit == NULL || pCur->cbSize < pBestFit->cbSize)
                )
        {
            pBestFit = pCur;
            pBestFitPrev = pPrev;
        }

        /* next */
        pPrev = pCur;
        pCur = pCur->pNext;
    }

    /* link out block */
    if (pCur != NULL)
    {   /* prefect match */
        if (pPrev != NULL)
            pPrev->pNext = pCur->pNext;
        else
            pFree = pCur->pNext;

        cbFree -= cbUserSize;
    }
    else if (pBestFit != NULL)
    {   /* best fit */
        /* two cases 1) split block. 2) block is too small to be splitted. */
        if (pBestFit->cbSize > cbUserSize + CB_HDR)
        {
            pCur = (PMEMBLOCK)((unsigned)pBestFit + pBestFit->cbSize - cbUserSize);
            #ifdef DEBUG_ALLOC
                pCur->ulSignature = SIGNATURE;
            #endif
            pCur->cbSize = cbUserSize;
            pBestFit->cbSize -= cbUserSize + CB_HDR;

            cbFree -= cbUserSize + CB_HDR;
        }
        else
        {
            if (pBestFitPrev != NULL)
                pBestFitPrev->pNext = pBestFit->pNext;
            else
                pFree = pBestFit->pNext;
            pCur = pBestFit;

            cbFree -= pCur->cbSize;
        }
    }

    return pCur;
}


/**
 * Finds a memory block starting the search at pMemblock.
 * @returns   Pointer to memblock if found.
 * @param     pMemblock  Start node.
 * @param     pvUser     User pointer to find the block to.
 * @param     fWithin    When this flag is set, the pointer may point anywhere within the block.
 */
static PMEMBLOCK    findBlock(PMEMBLOCK pMemblock, void *pvUser, int fWithin)
{
    if (pvUser != NULL && pMemblock != NULL)
    {
        if (fWithin)
            while (pMemblock != NULL &&
                   !(pvUser >= (void*)pMemblock && pvUser < (void*)PNEXT_BLOCK(pMemblock))
                   )
                pMemblock = pMemblock->pNext;
        else
        {
            pvUser = (void*)((unsigned)pvUser - CB_HDR);
            while (pMemblock != NULL && pvUser != (void*)pMemblock)
                pMemblock = pMemblock->pNext;
        }
    }
    else
        pMemblock = NULL;

    return pMemblock;
}


/**
 * Initiate the heap "subsystem".
 * @returns   0 on success, not 0 on error.
 * @param     cbSize  Heapsize in bytes.
 */
int heapInit(unsigned cbSize)
{
    pUsed = NULL;

    #ifdef RING0
        pFree = D32Hlp_VMAlloc(VMDHA_SWAP, cbSize, ~0UL);
    #else
        if (DosAllocMem((void*)&pFree, cbSize, PAG_COMMIT | PAG_READ | PAG_WRITE) != 0)
            pFree = NULL;
    #endif
    if (pFree == NULL)
    {
        kprintf(("unable to allocate heap memory.\n"));
        Int3();
        return -1;
    }

    #ifdef DEBUG_ALLOC
        pFree->ulSignature = SIGNATURE;
    #endif
    pFree->cbSize = cbSize - CB_HDR;
    pFree->pNext = NULL;
    cbFree = pFree->cbSize;

    _uHeapMinPtr = (unsigned)pFree + CB_HDR;
    _uHeapMaxPtr = (unsigned)pFree + cbSize;

    #ifdef DEBUG_ALLOC
        if (!_heap_check())
        {
            /* error! */
            kprintf(("%s: _heap_check failed!\n", "heapInit"));
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
void * malloc(unsigned cbSize)
{
    void *pvRet = NULL;

    #ifdef DEBUG_ALLOC
        if (!_heap_check())
        {
            kprintf(("%s: _heap_check failed!\n", "malloc"));
            return NULL;
        }
    #endif

    if (cbSize != 0)
    {
        PMEMBLOCK pMemblock = getFreeMemblock(cbSize);
        if (pMemblock != NULL)
        {
            insertUsed(pMemblock);
            pvRet = &pMemblock->achUserData[0];
        }
    }
    else
    {
        /* error! */
        kprintf(("%s: error cbSize = 0\n", "malloc"));
    }

    return pvRet;
}


/**
 * Reallocate a heapblock.
 * @returns   Pointer to new heapblock.
 * @param     pv     Pointer to the block to realloc.
 * @param     cbNew  The new block size.
 */
void *realloc(void *pv, unsigned cbNew)
{
    PMEMBLOCK pMemblock;
    pMemblock = findBlock(pUsed, pv, FALSE);
    if (pMemblock != NULL)
    {
        void *pvRet;

        cbNew = (cbNew + 3) & ~3;
        if (cbNew <= pMemblock->cbSize)
        {   /* shrink block */
            pvRet = pv;
            if (cbNew + CB_HDR < pMemblock->cbSize)
            {   /* split block */
                PMEMBLOCK pNewBlock = (PMEMBLOCK)((unsigned)pMemblock + CB_HDR + cbNew);
                #ifdef DEBUG_ALLOC
                    pNewBlock->ulSignature = SIGNATURE;
                #endif
                pNewBlock->cbSize = pMemblock->cbSize - cbNew - CB_HDR;
                pNewBlock->pNext = NULL;
                pMemblock->cbSize = cbNew;
                insertFree(pNewBlock);
            }
        }
        else
        {   /* expand block */
            pvRet = malloc(cbNew);
            if (pvRet != NULL)
            {
                memcpy(pvRet, pv, pMemblock->cbSize);
                free(pv);
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
void free(void *pv)
{
    #ifdef DEBUG_ALLOC
        if (!_heap_check())
        {
            kprintf(("free: _heap_check failed!\n"));
            return;
        }
    #endif

    if (pv != NULL)
    {
        PMEMBLOCK pCur  = pUsed;
        PMEMBLOCK pPrev = NULL;
        pv = (void*)((int)pv - CB_HDR);

        while (pCur != NULL &&
        #ifdef DEBUG_ALLOC /* pointer within block */
               !(pv >= (void*)pCur && (void*)((unsigned)pv + CB_HDR) < (void*)PNEXT_BLOCK(pCur))
        #else
               pv != (void*)pCur
        #endif
               )
        {
            pPrev = pCur;
            pCur = pCur->pNext;
        }

        if (pCur != NULL)
        {
            if (pv == pCur)
            {
                if (pPrev != NULL)
                    pPrev->pNext = pCur->pNext;
                else
                    pUsed = pCur->pNext;

                insertFree(pCur);

                #ifdef DEBUG_ALLOC
                    if (!_heap_check())
                        kprintf(("%s: _heap_check failed 3!\n", "free"));
                #endif
            }
            else
                kprintf(("free: pv is not pointing to start of block.\n"));
        }
        else
            kprintf(("free: heap block not found!\n"));
    }
    else
        kprintf(("free: Free received a NULL pointer!\n"));
}


/**
 * Gets the size of a block.
 * @returns  Bytes in a block.
 */
unsigned _msize(void *pv)
{
    PMEMBLOCK pBlock;
    #ifdef DEBUG_ALLOC
        if (!_heap_check())
            kprintf(("_msize: _heap_check failed!\n"));
    #endif
    pBlock = findBlock(pUsed, pv, FALSE);
    return pBlock != NULL ? pBlock->cbSize : 0;
}


/**
 * Checks if pv is a valid heappointer.
 * @returns   1 if valid. 0 if invalid.
 * @param     pv  User data pointer.
 */
int _validptr(void *pv)
{
    PMEMBLOCK pBlock;

    #ifdef DEBUG_ALLOC
        if (!_heap_check())
            kprintf(("_validptr: _heap_check failed!\n"));
    #endif

    pBlock = findBlock(pUsed, pv, TRUE);
    return pBlock != NULL;
}


/**
 * Checks that the dataaera made up by pv and cbSize valid with in the heap.
 * @returns   1 if valid. 0 if invalid.
 * @param     pv      User data pointer.
 * @param     cbSize  Size of data which has to be valid.
 */
int _validptr2(void *pv, unsigned cbSize)
{
    PMEMBLOCK pBlock;

    #ifdef DEBUG_ALLOC
        if (!_heap_check())
            kprintf(("_validptr: _heap_check failed!\n"));
    #endif

    pBlock = findBlock(pUsed, pv, TRUE);
    return pBlock != NULL ? (pBlock->cbSize - ((unsigned)pv - (unsigned)pBlock - CB_HDR)) >= cbSize : FALSE;
}


/**
 * Get amount of free memory (in bytes)
 * @returns  Amount of free memory (in bytes).
 * @remark   Note that this amount is of all free memory blocks and
 *           that these blocks are fragmented.
 *           You'll probably not be able to allocate a single block
 *           of the returned size.
 */
unsigned _memfree(void)
{
    #ifdef DEBUG_ALLOC
        if (!_heap_check())
            kprintf(("_memfree: _heap_check failed!\n"));
    #endif
    return cbFree;
}


/**
 * Checks heap integrety.
 * @returns  TRUE  when ok.
 *           FALSE on error.
 *           NULL if out of memory. (Or memory to fragmented.)
 */
int _heap_check(void)
{
    #ifdef DEBUG_ALLOC
        PMEMBLOCK pCurFree = pFree;
        PMEMBLOCK pCurUsed = pUsed;

        while (pCurFree != NULL || pCurUsed != NULL)
        {
            /** @sketch:
             * check signatures and for lost memory.
             *
             * three cases:
             *  1) pCurUsed adjecent to pCurUsed->pNext
             *  2) pCurUsed adjecent to pCurFree
             *  3) pCurFree adjecent to pCurFree->pNext
             *  4) pCurFree adjecent to pCurUsed
             *  5) pCurUsed is the last block
             *  6) pCurFree is the last block
             */
            #if 0
            if (pCurUsed != NULL && PNEXT_BLOCK(pCurUsed) == pCurUsed->pNext)       /* 1.*/
                ;
            else if (pCurUsed != NULL && PNEXT_BLOCK(pCurUsed) == pCurFree)         /* 2.*/
                ;
            else if (pCurFree != NULL && PNEXT_BLOCK(pCurFree) == pCurFree->pNext)  /* 3.*/
                ;
            else if (pCurFree != NULL && PNEXT_BLOCK(pCurFree) == pCurUsed)         /* 4.*/
                ;
            else if (pCurUsed != NULL && pCurFree == NULL && pCurUsed->pNext == NULL)   /* 5.*/
                ;
            else if (pCurFree != NULL && pCurUsed == NULL && pCurFree->pNext == NULL)   /* 6.*/
                ;
            else
            #else
            if (!( (pCurUsed != NULL && PNEXT_BLOCK(pCurUsed) == pCurUsed->pNext)    /* 1.*/
                || (pCurUsed != NULL && PNEXT_BLOCK(pCurUsed) == pCurFree)           /* 2.*/
                || (pCurFree != NULL && PNEXT_BLOCK(pCurFree) == pCurFree->pNext)    /* 3.*/
                || (pCurFree != NULL && PNEXT_BLOCK(pCurFree) == pCurUsed)           /* 4.*/
                || (pCurUsed != NULL && pCurFree == NULL && pCurUsed->pNext == NULL) /* 5.*/
                || (pCurFree != NULL && pCurUsed == NULL && pCurFree->pNext == NULL) /* 6.*/
                 )
               )
            #endif
                {
                /* error hole */
                kprintf(("_heap_check: internal error - memory hole!\n"));
                return FALSE;
                }

            /* check signature and advance to the next block */
            if (pCurUsed != NULL && (pCurFree == NULL || pCurUsed < pCurFree))
            {
                if (pCurUsed->ulSignature != SIGNATURE)
                    return FALSE;
                pCurUsed = pCurUsed->pNext;
            }
            else
            {
                if (pCurFree->ulSignature != SIGNATURE)
                    return FALSE;
                pCurFree = pCurFree->pNext;
            }
        }
    #endif
    return TRUE;
}


#if !defined(RING0) && defined(__IBMC__)

/**
 * Initialize Memory Functions
 * Called from _exeentry.
 */
int _rmem_init(void)
{
    int rc = heapInit(0x100000);
    return rc;
}

/**
 * Initialize Memory Functions
 * Called from _exeentry.
 */
int _rmem_term(void)
{
    return 0;
}

#endif
