/* $Id: longexecutablename.drv.c,v 1.1 2000-12-11 06:43:14 bird Exp $
 *
 * Long dll name test - .drv extention.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#include <os2.h>
#include <stdio.h>

void hello(argc, argv)
int argc;
char **argv;
{
    printf("Hello drv - %s\n", argv[0]);
}

