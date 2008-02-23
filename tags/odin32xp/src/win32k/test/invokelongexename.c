/* $Id: invokelongexename.c,v 1.1 2000-12-11 06:43:14 bird Exp $
 *
 * Long dll name test - exe code.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#include <os2.h>
#include <stdio.h>

void hello(int, char**);

void main(argc, argv)
int argc;
char **argv;
{
    printf("Hello Exe - %s\n", argv[0]);
    hello(argc, argv);
    DosSleep(5000);
}

