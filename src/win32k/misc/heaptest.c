/* $Id: heaptest.c,v 1.2 2000-01-23 03:20:52 bird Exp $
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
#define RANDOMTEST_ITERATIONS   65536*2
#define Int3()          __interrupt(3)


/*******************************************************************************
*   Internal Functions
*******************************************************************************/
#include "malloc.h"
#include "rmalloc.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <builtin.h>




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

    /*
     * Initiate the heap.
     */
    if (ResHeapInit(128*1024, 1024*1024*64) != 0)
    {
        printf("Failed to initiate the resident heap.\n");
        return 1;
    }

/*
 *
 * resident heap tests
 * resident heap tests
 *
 */
#if 1
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
        if (apv[i] == NULL)
        {
            printf("rmalloc failed: acb[%d]=%d\n", i, acb[i]);
            if (acb[i] > 1000)
                break;
        }
        memset(apv[i], 0xA, MIN(acb[i],16));
        cb += acb[i];
    }

    printf("Finished allocating memory: %d allocations  %d bytes\n", i, cb);


    printf("_res_dump_subheaps:\n");
    _res_dump_subheaps();

    for (i = 0; i < NUMBER_OF_POINTERS; i++)
    {
        int cb = _res_msize(apv[i]);
        if (cb != ((acb[i] + 3) & ~3) && (cb < ((acb[i] + 3) & ~3) || cb > 52 + ((acb[i] + 3) & ~3)) )
            printf("size of avp[%d] (%d) != acb[%] (%d)\n", i, cb, i, acb[i]);
        rfree(apv[i]);
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
#endif


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
            (i < RANDOMTEST_ITERATIONS*1/4 ? (j % 8) > 2 :
             i < RANDOMTEST_ITERATIONS*2/4 ? (j % 8) > 3 :
             i < RANDOMTEST_ITERATIONS*3/4 ? (j % 8) > 4 :
             i < RANDOMTEST_ITERATIONS     ? (j % 8) > 5 : 0
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
                if (apv[j] == NULL)
                {
                    printf("rmalloc failed, acb[%d] = %d\n", j, acb[j]);
                    if (acb[j] > 10000)
                        continue;
                    break;
                }
                memset(apv[j], 0xA, MIN(acb[j],16));
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
                    if (pv == NULL)
                    {
                        printf("realloc(apv[%d](0x%08), %d) failed\n", j, apv[j], cb);
                        continue;
                    }
                    apv[j] = pv;
                    acb[j] = cb;
                }
                else
                {   /* free */
                    rfree(apv[j]);
                    apv[j] = NULL;
                    cAllocations--;
                    crfree++;
                }
            }
        }
        /*_res_heap_check();*/
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



/*
 *
 * swappable heap tests
 * swappable heap tests
 *
 */




    /* unreferenced parameters */
    argc = argc;
    argv = argv;

    return 0;
}



