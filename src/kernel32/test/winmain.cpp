/* $Id: winmain.cpp,v 1.1 1999-11-28 23:10:13 bird Exp $
 *
 * Windows startup code.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <windows.h>
#include "kernel32test.h"



int main(int argc, char *argv[])
{
    argc=argc;
    argv=argv;
    return Kernel32TestMain(0, 0, 0, 0);
}

