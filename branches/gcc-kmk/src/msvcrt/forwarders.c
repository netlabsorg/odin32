/*
 * msvcrt.dll stdcall to cdecl forwarders
 *
 */

#include <winbase.h>
#include "wine/port.h"

#include "winternl.h"
#include "wine/exception.h"
#include "thread.h"
#include "msvcrt.h"

HANDLE MSVCRT_GetCurrentThread(void)
{
 return GetCurrentThread();
}

DWORD MSVCRT_GetCurrentThreadId(void)
{
 return GetCurrentThreadId();
}

DWORD MSVCRT_GetCurrentProcessId(void)
{
 return GetCurrentProcessId();
}

DWORD MSVCRT_GetLogicalDrives(void)
{
 return GetLogicalDrives();
}
