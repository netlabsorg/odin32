/* $Id: os2main.cpp,v 1.1 1999-11-28 23:10:10 bird Exp $
 *
 * OS/2 startup code.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
//#include <os2.h>
#include <winbase.h>
#include <win32type.h>
#include <odinlx.h>
#include "kernel32test.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/* Win32 resource directory (produced by wrc) */
extern "C" DWORD _Resource_PEResTab;



int main(int argc, char *argv[])
{
    RegisterLxExe(Kernel32TestMain, (PVOID)&_Resource_PEResTab);
}

