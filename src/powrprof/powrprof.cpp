/*
 * powrprof - power management API stub.  The JVM asks for processor power
 * information through CallNtPowerInformation; OS/2 has none, so it fails.
 */
#include <os2win.h>

#define PP_STATUS_NOT_IMPLEMENTED   ((LONG)0xC0000002)

extern "C" {

LONG WINAPI CallNtPowerInformation(DWORD level, PVOID in, ULONG inlen, PVOID out, ULONG outlen)
{
    return PP_STATUS_NOT_IMPLEMENTED;
}

} // extern "C"
