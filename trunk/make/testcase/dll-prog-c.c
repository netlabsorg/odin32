/* $Id: dll-prog-c.c,v 1.1 2002-05-16 11:37:06 bird Exp $
 *
 * DLL Test program.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#ifdef OS2
#define INCL_DOS
#include <os2.h>
#endif


/**
 * Testprogram.
 * @returns 0 on success.
 */
int main(int argc, char **argv)
{
#ifdef OS2
    PFN         pfnFoo42;
    PFN         pfnFooName;
    int         rc;
    HMODULE     hmod;
#endif

    if (argc != 2)
    {
        printf("Syntax error!\nsyntax: %s <dllname>\n", argv[0]);
        return 16;
    }


#ifdef OS2
    rc = DosLoadModule(NULL, 0, argv[1], &hmod);
    if (rc)
    {
        printf("Failed to load module '%s'. rc=%d\n", argv[1], rc);
        return 16;
    }

    #ifdef __16BIT__
    rc = DosGetProcAddr(hmod, "#42", &pfnFoo42);
    #else
    rc = DosQueryProcAddr(hmod, 42, NULL, &pfnFoo42);
    #endif
    if (rc)
    {
        printf("Failed to get function at ordinal 42. rc=%d\n", rc);
        return 16;
    }

    #ifdef __16BIT__
    rc = DosGetProcAddr(hmod, "FOONAME", &pfnFooName);
    #else
    rc = DosQueryProcAddr(hmod, 0, "FOONAME", &pfnFooName);
    #endif
    if (rc)
    {
        printf("Failed to get function at ordinal 42. rc=%d\n", rc);
        return 16;
    }

#endif

    /*
     * Call the functions.
     */
    (*pfnFoo42)();
    (*pfnFooName)();

    return 0;
}


