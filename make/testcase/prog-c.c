/* $Id: prog-c.c,v 1.1 2002-05-16 11:37:07 bird Exp $
 *
 * Test program.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#ifdef DEBUG
#define _STR "\"DEBUG\" "
#else
#define _STR ""
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <math.h>


/**
 * Testprogram.
 * @returns 0 on success.
 */
int main(void)
{
    long double lrd = asin(cos(0));
    #ifdef __cplusplus
    puts("Hello \"C++\" "_STR"World!\n");
    #else
    puts("Hello \"C\" "_STR"World!\n");
    #endif

/*
 * Define assertions.
 */
#ifdef __32BIT__
# ifdef __16BIT__
    puts("error: __16BIT__ && __32BIT__ is defined!\n");
# else
    puts("__32BIT__\n");
# endif
#else
# ifdef __16BIT__
    puts("__16BIT__\n");
# else
    puts("error: __16BIT__ nor __32BIT__ is defined!\n");
# endif

#endif

#if !defined(OS2) && !defined(WIN32) && !defined(LINUX)
    puts("error: OS is not defined!\n");
#endif

    return 0;
}
