/* $Id: DosAllocMemEx.c,v 1.2 2001-02-19 05:58:18 bird Exp $
 *
 * Testcases for DosAllocMemEx.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define TESTCASES 10

#define INCL_BASE


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <stdio.h>
#include <string.h>

#include "win32k.h"





int main(argc, argv)
int argc;
char **argv;
{
    struct Tests
    {
        APIRET  rc;
        ULONG   cb;
        PVOID   pv;
        ULONG   fl;
    }   aTests[] =
    {
        {   0, 0x00001000, 0x00000000,  PAG_READ | PAG_WRITE | PAG_COMMIT},
        {   0, 0x00001000, 0x00000000,  PAG_READ | PAG_COMMIT},
        {   0, 0x00001000, 0x00230000,  PAG_READ | PAG_COMMIT | OBJ_ALIGN64K | OBJ_LOCATION},
        {   0, 0x00001000, 0x03f30000,  PAG_READ | PAG_COMMIT | OBJ_ALIGN64K | OBJ_LOCATION},
        {   0, 0x00001000, 0x04030000,  PAG_READ | PAG_COMMIT | OBJ_ALIGN64K | OBJ_LOCATION},
        {   0, 0x00001000, 0x00000000,  PAG_READ | PAG_COMMIT | OBJ_ANY | OBJ_ALIGNPAGE},
        {   0, 0x00001000, 0x00000000,  PAG_READ | PAG_COMMIT | OBJ_ANY},
        {   0, 0x00001000, 0x00000000,  PAG_READ | PAG_COMMIT | OBJ_ANY | OBJ_ALIGN64K},
        {   0, 0x00001000, 0x00000000,  PAG_READ | PAG_COMMIT | OBJ_ANY | OBJ_ALIGN64K},
        {   0, 0x00001000, 0x00000000,  PAG_READ | PAG_COMMIT | OBJ_ANY | OBJ_ALIGN64K},
        {   0, 0x00001000, 0x00000000,  PAG_READ | PAG_COMMIT | OBJ_ANY | OBJ_ALIGN64K},
        {   0, 0x00001000, 0x31230000,  PAG_READ | PAG_COMMIT | OBJ_ANY | OBJ_ALIGN64K | OBJ_LOCATION},
        {   0, 0x00001000, 0x23030000,  PAG_READ | PAG_COMMIT | OBJ_ANY | OBJ_ALIGN64K | OBJ_LOCATION},
    };

    int     i;
    int     cErrors;
    APIRET  rc;
    PVOID   pv;

    /*
     * Init win32k.sys library.
     */
    rc = libWin32kInit();
    if (rc != NO_ERROR)
    {
        printf("failed to init win32k.sys. rc=%d\n", rc);
        return rc;
    }


    /*
     * Execute the tests.
     *      xx: 0x00000000 0x00000000 0x00000000 00000  0000000 0x00000000
     */
    printf("no. Size       pv(in)     flags      rc(in) rc(out) pv(out)\n"
           "-----------------------------------------------------------\n");
    for (i = 0, cErrors = 0; i < sizeof(aTests) / sizeof(aTests[0]); i++)
    {
        printf("%2d: 0x%08x 0x%08x 0x%08x %5d  ",
               i, aTests[i].cb, aTests[i].pv, aTests[i].fl, aTests[i].rc);
        pv = aTests[i].pv;
        rc = DosAllocMemEx(&pv, aTests[i].cb, aTests[i].fl);
        printf("%7d 0x%08x\n", rc, pv);

        /* evaluate result */
        cErrors += (rc != aTests[i].rc || (pv != aTests[i].pv && aTests[i].pv != NULL));
    }

    printf("Done %d tests, %d failed\n", i, cErrors);
    libWin32kTerm();

    return cErrors;
}

