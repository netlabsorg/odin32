/* $Id: heaptest.c,v 1.1 2000-01-22 18:21:03 bird Exp $
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
#define NUMBER_OF_POINTERS      10240
#define RANDOMTEST_ITERATIONS   10240


/*******************************************************************************
*   Internal Functions
*******************************************************************************/
#include "malloc.h"
#include "rmalloc.h"
#include <stdio.h>
#include <stdlib.h>




int main(int argc, char *argv)
{
    static void * apv[NUMBER_OF_POINTERS];
    static int    acb[NUMBER_OF_POINTERS];
    unsigned      cAllocations;
    unsigned      cb = 0;
    unsigned      crmalloc;
    unsigned      crfree;

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
    /*
     * Simple allocation test.
     */
    for (i = 0; i < NUMBER_OF_POINTERS; i++)
    {
        do
        {
            acb[i] = rand();
        } while(acb[i] == 0 || acb[i] > 64);

        apv[i] = rmalloc(acb[i]);
        if (apv[i] == NULL)
        {
            printf("rmalloc failed: acb[%d]=%d\n", i, acb[i]);
            if (acb[i] > 1000)
                break;
        }
        cb += acb[i];
    }

    printf("Finished allocating memory: %d allocations  %d bytes\n", i, cb);


    printf("_res_dump_subheaps:\n");
    _res_dump_subheaps();

    for (i = 0; i < NUMBER_OF_POINTERS; i++)
    {
        int cb = _res_msize(apv[i]);
        if (cb != ((acb[i] + 3) & ~3) && (cb < ((acb[i] + 3) & ~3) || cb > 40 + ((acb[i] + 3) & ~3)) )
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
    while (i++ < RANDOMTEST_ITERATIONS || cAllocations > 0)
    {
        int j;
        j = rand();
        if (cAllocations + (NUMBER_OF_POINTERS/20) < NUMBER_OF_POINTERS &&
            (i < RANDOMTEST_ITERATIONS*1/4 ? (j % 8) > 1 :
             i < RANDOMTEST_ITERATIONS*2/4 ? (j % 8) > 2 :
             i < RANDOMTEST_ITERATIONS*3/4 ? (j % 8) > 3 :
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
                } while (acb[j] == 0 || acb[j] > 2048);
                apv[j] = rmalloc(acb[j]);
                if (apv[j] == NULL)
                {
                    printf("rmalloc failed, acb[%d] = %d\n", j, acb[j]);
                    if (acb[j] > 10000)
                        continue;
                }
                cAllocations++;
                cb += acb[j];
                crmalloc++;
            }
        }
        else
        { /* free */
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
                } while (k < NUMBER_OF_POINTERS/2 && (j >= NUMBER_OF_POINTERS || apv[j] == NULL));
                if (k >= NUMBER_OF_POINTERS/2)
                {
                    for (j = 0; j < NUMBER_OF_POINTERS && apv[j] == NULL; j++)
                        (void)0;
                }
            }

            if (j < NUMBER_OF_POINTERS && apv[j] != NULL)
            {
                int cb = _res_msize(apv[j]);
                if (cb != ((acb[j] + 3) & ~3) && (cb < ((acb[j] + 3) & ~3) || cb > 40 + ((acb[j] + 3) & ~3)) )
                    printf("size of avp[%d] (%d) != acb[%d] (%d)\n", j, cb, j, acb[j]);
                rfree(apv[j]);
                apv[j] = NULL;
                cAllocations--;
                crfree++;
            }
        }
        _res_heap_check();
        if (RANDOMTEST_ITERATIONS == i*2)
            _res_dump_subheaps();
    }

    printf("cb=%d crfree=%d crmalloc=%d\n", cb, crfree, crmalloc);

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



