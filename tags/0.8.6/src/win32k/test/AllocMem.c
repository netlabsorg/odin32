/* $Id: AllocMem.c,v 1.1 2001-02-11 15:25:51 bird Exp $
 *
 * Program which allocates memory.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_BASE


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#ifndef OBJ_ANY
#define OBJ_ANY 0x400UL
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>




int main(argc, argv)
int argc;
char **argv;
{
    int     argi;
    PVOID   pv;
    APIRET  rc;
    ULONG   cbMem = 1024*1024*20;
    ULONG   flFlags = PAG_READ | PAG_WRITE | PAG_COMMIT;

    for (argi = 1; argi < argc; argi++ )
    {
        if (argv[argi][0] == '-')
            flFlags |= OBJ_ANY;
        else
        {
            char *psz;
            cbMem = strtol(argv[argi], &psz,
                           argv[argi][0] == '0' && (argv[argi][1] == 'x' || argv[argi][1] == 'X')
                            ? 16 : 10);
        }
    }


    rc = DosAllocMem(&pv, cbMem, flFlags);
    printf("DosAllocMem(, 0x%x, 0x%x) -> rc=%d, pv=0x%x\n", cbMem, flFlags, rc, pv);
    if (!rc)
    {
        char *pch;
        char *pchEnd = (char*)pv + cbMem;
        printf("Press a key to touch all pages.\n");
        getc(stdin);
        for (pch = (char*)pv; pch < pchEnd; pch += 0x1000)
            *pch = '1';
        printf("Press a key to exit.\n");
        getc(stdin);
    }

    return 0;
}
