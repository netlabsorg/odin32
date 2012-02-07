/** @file
 *
 * INITDLL library implementation.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define INCL_DOS
#define INCL_ERRORS
#include <os2.h>

#include <initdll.h>

static bool cleanupDone = false;
static ULONG moduleHandle = NULLHANDLE;

static APIENTRY void cleanup(ULONG ulReason)
{
    cleanupDone = true;
    DLL_Term(moduleHandle);
    DosExitList(EXLST_EXIT, cleanup);
}

unsigned long _System _DLL_InitTerm(unsigned long hModule,
                                    unsigned long ulFlag)
{
    APIRET arc;

    if (ulFlag == 0)
    {
        ULONG order = DLL_Init(hModule);
        if (order == -1) // failure?
        {
            // OS/2 will call us back with ulFlag = 1 on failure;
            // uninit now on our own to retain the LIFO order
            cleanupDone = true;
            DLL_Term(hModule);
            return 0;
        }
        arc = DosExitList (EXLST_ADD | (order & 0x0000FF00), cleanup);
        if (arc != NO_ERROR)
        {
            // see above
            cleanupDone = true;
            DLL_Term(hModule);
            return 0;
        }
        moduleHandle = hModule;
        return 1;
    }

    if (!cleanupDone)
    {
        cleanupDone = true;
        DLL_Term(hModule);
        DosExitList(EXLST_REMOVE, cleanup);
    }
    return 1;
}

