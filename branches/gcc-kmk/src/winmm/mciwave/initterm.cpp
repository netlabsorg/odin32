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
extern DWORD mciwave_PEResTab;

static HMODULE dllHandle = 0;

ULONG SYSTEM DLL_InitMciWave(ULONG hModule)
{
    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
    dllHandle = RegisterLxDll(hModule, NULL, (PVOID)&mciwave_PEResTab);
    if (dllHandle == 0)
        return -1;

    return 0;
}

void SYSTEM DLL_TermMciWave(ULONG hModule)
{
    if(dllHandle)
        UnregisterLxDll(dllHandle);
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitMciWave(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermMciWave(hModule);
    DLL_TermDefault(hModule);
}
