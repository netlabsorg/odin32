/* $Id: kernel32test.cpp,v 1.1 1999-11-28 23:10:06 bird Exp $
 *
 * Kernel32 test program.
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
// #include <winbase.h>
#include "testlib.h"
#include "kernel32test.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static TESTENTRY aMainTests[] =
{ /* pfnTest                    pszName                (cError)  (cWarnings) */
    {TestResources,             "Resources",            0,        0},
    {NULL,                      NULL,                   0,        0} /* terminator entry */
};

static TESTTABLE MainTestTable =
{
    "Kernel32Tests",
    0,
    0,
    &aMainTests[0],
    -1,
    -1,
    -1,
};



int WIN32API Kernel32TestMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hInstance = hInstance; hPrevInstance = hPrevInstance; lpCmdLine = lpCmdLine; nCmdShow = nCmdShow;

    TstProcessTestTable(&MainTestTable);
    return MainTestTable.cErrors;
}

