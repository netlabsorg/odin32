/* $Id: heaptest.c,v 1.5 2000-01-24 18:18:59 bird Exp $
 *
 * Test of resident and swappable heaps.
 *
 *
 * Copyright (c) 2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/******************************************************************************
*   Defined Constants
*******************************************************************************/
#define NUMBER_OF_POINTERS      16384
#define RANDOMTEST_ITERATIONS   65536
#define EXTRA_HEAPCHECK

/*******************************************************************************
*   Internal Functions
*******************************************************************************/
/*#include "malloc.h"*/
#include "rmalloc.h"
#include "smalloc.h"
#include "macros.h"
#include "asmutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>




int main(int argc, char *argv)
{
    static void * apv[NUMBER_OF_POINTERS];
    static int    acb[NUMBER_OF_POINTERS];
    unsigned      cAllocations;
    unsigned      cb = 0;
    unsigned      crmalloc;
    unsigned      crfree;
    unsigned      crealloc;
    int           i;
    int           fResTests = 1;
    int           fResSimple = 1;
    int           fResRandom = 1;
    int           fSwpTests = 1;
    int           fSwpSimple = 1;
    int           fSwpRandom = 1;
    enum   {malloc,realloc, free, unknown} enmLast = unknown;

    /*
     * Initiate the heap.
     */
    if (resHeapInit(128*1024, 1024*1024*64) != 0)
    {
        printf("Failed to initiate the resident heap.\n");
        return 1;
    }

    if (swpHeapInit(128*1024, 1024*1024*64) != 0)
    {
        printf("Failed to initiate the swappable heap.\n");
        return 2;
    }



    if (fResTests)
    {
        /*
         *
         * resident heap tests
         * resident heap tests
         *
         */
        if (fResSimple)
        {
            /*
             * Simple allocation test.
             */
            for (i = 0; i < NUMBER_OF_POINTERS; i++)
            {
                do
                {
                    acb[i] = rand();
                } while(acb[i] == 0 || acb[i] > 127);

                if ((i % (NUMBER_OF_POINTERS/3)) == 1)
                    acb[i] += 1024*260;
                apv[i] = rmalloc(acb[i]);
                enmLast = malloc;
                if (apv[i] == NULL)
                {
                    printf("rmalloc failed: acb[%d]=%d\n", i, acb[i]);
                    if (acb[i] > 1000)
                        break;
                }
                memset(apv[i], 0xA, acb[i]);
                cb += acb[i];
            }

            printf("Finished allocating memory: %d allocations  %d bytes\n", i, cb);


            printf("_res_dump_subheaps:\n");
            _res_dump_subheaps();

            for (i = 0; i < NUMBER_OF_POINTERS; i++)
            {
                int cb = _res_msize(apv[i]);
                if (cb != ((acb[i] + 3) & ~3) && (cb < ((acb[i] + 3) & ~3) || cb > 52 + ((acb[i] + 3) & ~3)) )
                    printf("size of avp[%d] (%d) != acb[%d] (%d)\n", i, cb, i, acb[i]);
                memset(apv[i], 0xF, acb[i]);
                rfree(apv[i]);
                enmLast = free;
            }


            /*
             * test _res_heapmin
             */
            printf("_res_memfree - before heapmin: %d\n", _res_memfree());
            _res_heapmin();
            printf("_res_memfree - after heapmin : %d\n", _res_memfree());

            /*
             * Test _res_dump_subheaps
             */
            printf("\n_res_dump_subheaps:\n");
            _res_dump_subheaps();
        } /* fResSimple */


        if (fResRandom)
        {
            /*
             * Test 2 - random allocation and freeing of memory.
             */
            printf("\n"
                   "Random allocation and freeing test:\n");
            for (i = 0; i < NUMBER_OF_POINTERS; i++)
                apv[i] = NULL, acb[i] = 0;
            cAllocations = 0;
            i = 0;
            cb = 0;
            crfree = 0;
            crmalloc = 0;
            crealloc = 0;
            while (i++ < RANDOMTEST_ITERATIONS || cAllocations > 0)
            {
                int j;
                j = rand();
                if (cAllocations + (NUMBER_OF_POINTERS/20) < NUMBER_OF_POINTERS &&
                    (i < RANDOMTEST_ITERATIONS*1/4 ? (j % 8) > 4 :
                     i < RANDOMTEST_ITERATIONS*2/4 ? (j % 8) > 5 :
                     i < RANDOMTEST_ITERATIONS*3/4 ? (j % 8) > 6 :
                     i < RANDOMTEST_ITERATIONS     ? (j % 8) > 6 : 0
                     )
                    )
                {   /* malloc */
                    for (j = 0; j < NUMBER_OF_POINTERS && apv[j] != NULL; j++)
                        (void)0;
                    if (j < NUMBER_OF_POINTERS)
                    {
                        do
                        {
                            acb[j] = rand();
                        } while (acb[j] == 0 || (acb[j] > 2048 && (i % 11) != 10));
                        if ((i % (RANDOMTEST_ITERATIONS/20)) == 1)
                            acb[j] += 1024*256;
                        apv[j] = rmalloc(acb[j]);
                        enmLast = malloc;
                        if (apv[j] == NULL)
                        {
                            printf("rmalloc failed, acb[%d] = %d\n", j, acb[j]);
                            if (acb[j] > 10000)
                                continue;
                            break;
                        }
                        memset(apv[j], 0xA, acb[j]);
                        cAllocations++;
                        cb += acb[j];
                        crmalloc++;
                    }
                }
                else
                { /* free or realloc */
                    if (cAllocations == 0)
                        continue;

                    if (cAllocations < NUMBER_OF_POINTERS/10)
                    {
                        for (j = 0; j < NUMBER_OF_POINTERS && apv[j] == NULL; j++)
                            (void)0;
                    }
                    else
                    {
                        int k = 0;
                        do
                        {
                            j = rand();
                        } while (k++ < NUMBER_OF_POINTERS/2 && (j >= NUMBER_OF_POINTERS || apv[j] == NULL));
                        if (k >= NUMBER_OF_POINTERS/2)
                        {
                            for (j = 0; j < NUMBER_OF_POINTERS && apv[j] == NULL; j++)
                                (void)0;
                        }
                    }

                    if (j < NUMBER_OF_POINTERS && apv[j] != NULL)
                    {
                        int cb = _res_msize(apv[j]);
                        if (cb != ((acb[j] + 3) & ~3) && (cb < ((acb[j] + 3) & ~3) || cb > 52 + ((acb[j] + 3) & ~3)) )
                            printf("size of avp[%d] (%d) != acb[%d] (%d)\n", j, cb, j, acb[j]);
                        if (i < RANDOMTEST_ITERATIONS*3/4 && j % 3 == 0)
                        {   /* realloc */
                            int cb;
                            void *pv;
                            crealloc++;
                            do
                            {
                                cb = rand();
                            } while (cb == 0 || cb > 3072);
                            /*
                            if (i >= 0x1c14)
                                Int3();
                            */
                            pv = rrealloc(apv[j], cb);
                            enmLast = realloc;
                            if (pv == NULL)
                            {
                                printf("realloc(apv[%d](0x%08), %d) failed\n", j, apv[j], cb);
                                continue;
                            }
                            apv[j] = pv;
                            acb[j] = cb;
                            memset(apv[j], 0xB, acb[j]);
                        }
                        else
                        {   /* free */
                            memset(apv[j], 0xF, acb[j]);
                            rfree(apv[j]);
                            enmLast = free;
                            apv[j] = NULL;
                            cAllocations--;
                            crfree++;
                        }
                    }
                }
                #ifdef EXTRA_HEAPCHECK
                _res_heap_check();
                #endif
                if (RANDOMTEST_ITERATIONS/2 == i)
                    _res_dump_subheaps();
                if ((i % 2048) == 0)
                    printf("i=%d cAllocations=%d\n", i, cAllocations);
            }

            printf("cb=%d crfree=%d crmalloc=%d crealloc=%d\n", cb, crfree, crmalloc, crealloc);

            printf("_res_dump_subheaps:\n");
            _res_dump_subheaps();

            printf("_res_memfree - before heapmin: %d\n", _res_memfree());
            _res_heapmin();
            printf("_res_memfree - after heapmin : %d\n", _res_memfree());

            printf("_res_dump_subheaps:\n");
            _res_dump_subheaps();
        } /* fResRandom */
    } /* fResTests */


/*
 *
 * swappable heap tests
 * swappable heap tests
 *
 */
    if (fSwpTests)
    {
        printf("\nSwappable heap tests\nSwappable heap tests\n");
        if (fSwpSimple)
        {
            /*
             * Simple allocation test.
             */
            printf("\nSimple swappable heap tests\nSimple swappable heap tests\n");
            for (i = 0; i < NUMBER_OF_POINTERS; i++)
            {
                do
                {
                    acb[i] = rand();
                } while(acb[i] == 0 || acb[i] > 127);

                if ((i % (NUMBER_OF_POINTERS/3)) == 1)
                    acb[i] += 1024*260;
                apv[i] = smalloc(acb[i]);
                enmLast = malloc;
                if (apv[i] == NULL)
                {
                    printf("smalloc failed: acb[%d]=%d\n", i, acb[i]);
                    if (acb[i] > 1000)
                        break;
                }
                memset(apv[i], 0xA, acb[i]);
                cb += acb[i];
            }

            printf("Finished allocating memory: %d allocations  %d bytes\n", i, cb);


            printf("_swp_dump_subheaps:\n");
            _swp_dump_subheaps();

            for (i = 0; i < NUMBER_OF_POINTERS; i++)
            {
                int cb = _swp_msize(apv[i]);
                if (cb != ((acb[i] + 3) & ~3) && (cb < ((acb[i] + 3) & ~3) || cb > 52 + ((acb[i] + 3) & ~3)) )
                    printf("size of avp[%d] (%d) != acb[%d] (%d)\n", i, cb, i, acb[i]);
                memset(apv[i], 0xF, acb[i]);
                sfree(apv[i]);
                enmLast = free;
            }


            /*
             * test _swp_heapmin
             */
            printf("_swp_memfree - before heapmin: %d\n", _swp_memfree());
            _swp_heapmin();
            printf("_swp_memfree - after heapmin : %d\n", _swp_memfree());

            /*
             * Test _swp_dump_subheaps
             */
            printf("\n_swp_dump_subheaps:\n");
            _swp_dump_subheaps();
        } /* fSwpSimple */


        if (fSwpRandom)
        {
            /*
             * Test 2 - random allocation and freeing of memory.
             */
            printf("\n"
                   "Random allocation and freeing test (swappable)\n"
                   "Random allocation and freeing test (swappable)\n"
                   );
            for (i = 0; i < NUMBER_OF_POINTERS; i++)
                apv[i] = NULL, acb[i] = 0;
            cAllocations = 0;
            i = 0;
            cb = 0;
            crfree = 0;
            crmalloc = 0;
            crealloc = 0;
            while (i++ < RANDOMTEST_ITERATIONS || cAllocations > 0)
            {
                int j;
                j = rand();
                if (cAllocations + (NUMBER_OF_POINTERS/20) < NUMBER_OF_POINTERS &&
                    (i < RANDOMTEST_ITERATIONS*1/4 ? (j % 8) > 4 :
                     i < RANDOMTEST_ITERATIONS*2/4 ? (j % 8) > 5 :
                     i < RANDOMTEST_ITERATIONS*3/4 ? (j % 8) > 6 :
                     i < RANDOMTEST_ITERATIONS     ? (j % 8) > 6 : 0
                     )
                    )
                {   /* malloc */
                    for (j = 0; j < NUMBER_OF_POINTERS && apv[j] != NULL; j++)
                        (void)0;
                    if (j < NUMBER_OF_POINTERS)
                    {
                        do
                        {
                            acb[j] = rand();
                        } while (acb[j] == 0 || (acb[j] > 2048 && (i % 11) != 10));
                        if ((i % (RANDOMTEST_ITERATIONS/20)) == 1)
                            acb[j] += 1024*256;
                        apv[j] = smalloc(acb[j]);
                        enmLast = malloc;
                        if (apv[j] == NULL)
                        {
                            printf("rmalloc failed, acb[%d] = %d\n", j, acb[j]);
                            if (acb[j] > 10000)
                                continue;
                            break;
                        }
                        memset(apv[j], 0xA, acb[j]);
                        cAllocations++;
                        cb += acb[j];
                        crmalloc++;
                    }
                }
                else
                { /* free or realloc */
                    if (cAllocations == 0)
                        continue;

                    if (cAllocations < NUMBER_OF_POINTERS/10)
                    {
                        for (j = 0; j < NUMBER_OF_POINTERS && apv[j] == NULL; j++)
                            (void)0;
                    }
                    else
                    {
                        int k = 0;
                        do
                        {
                            j = rand();
                        } while (k++ < NUMBER_OF_POINTERS/2 && (j >= NUMBER_OF_POINTERS || apv[j] == NULL));
                        if (k >= NUMBER_OF_POINTERS/2)
                        {
                            for (j = 0; j < NUMBER_OF_POINTERS && apv[j] == NULL; j++)
                                (void)0;
                        }
                    }

                    if (j < NUMBER_OF_POINTERS && apv[j] != NULL)
                    {
                        int cb = _swp_msize(apv[j]);
                        if (cb != ((acb[j] + 3) & ~3) && (cb < ((acb[j] + 3) & ~3) || cb > 52 + ((acb[j] + 3) & ~3)) )
                            printf("size of avp[%d] (%d) != acb[%d] (%d)\n", j, cb, j, acb[j]);
                        if (i < RANDOMTEST_ITERATIONS*3/4 && j % 3 == 0)
                        {   /* realloc */
                            int cb;
                            void *pv;
                            crealloc++;
                            do
                            {
                                cb = rand();
                            } while (cb == 0 || cb > 3072);
                            /*
                            if (i >= 0x1c14)
                                Int3();
                            */
                            pv = srealloc(apv[j], cb);
                            enmLast = realloc;
                            if (pv == NULL)
                            {
                                printf("realloc(apv[%d](0x%08), %d) failed\n", j, apv[j], cb);
                                continue;
                            }
                            apv[j] = pv;
                            acb[j] = cb;
                            memset(apv[j], 0xB, acb[j]);
                        }
                        else
                        {   /* free */
                            memset(apv[j], 0xF, acb[j]);
                            sfree(apv[j]);
                            enmLast = free;
                            apv[j] = NULL;
                            cAllocations--;
                            crfree++;
                        }
                    }
                }
                #ifdef EXTRA_HEAPCHECK
                _swp_heap_check();
                #endif
                if (RANDOMTEST_ITERATIONS/2 == i)
                {
                    _swp_dump_subheaps();
                    _res_dump_subheaps();
                }
                if ((i % 2048) == 0)
                    printf("i=%d cAllocations=%d\n", i, cAllocations);
            }

            printf("cb=%d crfree=%d crmalloc=%d crealloc=%d\n", cb, crfree, crmalloc, crealloc);

            printf("_swp_dump_subheaps:\n");
            _swp_dump_subheaps();
            printf("_res_dump_subheaps:\n");
            _res_dump_subheaps();

            printf("_swp_memfree - before heapmin: %d\n", _swp_memfree());
            _swp_heapmin();
            printf("_swp_memfree - after heapmin : %d\n", _swp_memfree());

            printf("_swp_dump_subheaps:\n");
            _swp_dump_subheaps();
            printf("_res_dump_subheaps:\n");
            _res_dump_subheaps();
        } /* fSwpRandom */
    }



    /* unreferenced parameters */
    argc = argc;
    argv = argv;

    return 0;
}



