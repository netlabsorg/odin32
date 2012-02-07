/** @file
 *
 * INITDLL library implementation.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <os2wrap.h>

#define MAYBE_WEAK __attribute__((weak))
#include <initdll.h>

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    return DLL_InitDefault(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermDefault(hModule);
}

ULONG SYSTEM DLL_InitDefault(ULONG hModule)
{
    if (_CRT_init() != 0) // failure?
        return -1;
    __ctordtorInit();

    return 0;
}

void SYSTEM DLL_TermDefault(ULONG hModule)
{
    __ctordtorTerm();
    _CRT_term();
}

