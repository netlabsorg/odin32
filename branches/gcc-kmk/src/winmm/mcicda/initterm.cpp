/*
 * MCICDA DLL entry point
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <os2wrap.h>    //Odin32 OS/2 api wrappers

#include <odin.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>

// Win32 resource table (produced by wrc)
extern DWORD mcicda_PEResTab;

static HMODULE dllHandle = 0;

ULONG SYSTEM DLL_InitMciCda(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
    dllHandle = RegisterLxDll(hModule, NULL, (PVOID)&mcicda_PEResTab);
    if (dllHandle == 0)
        return -1;

    return 0;
}

void SYSTEM DLL_TermMciCda(ULONG hModule)
{
    if(dllHandle)
        UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitMciCda(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermMciCda(hModule);
    DLL_TermDefault(hModule);
}
