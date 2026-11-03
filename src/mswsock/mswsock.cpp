/*
 * mswsock - Winsock extension stub.  TransmitFile is not available; callers
 * (java.nio FileChannel.transferTo) fall back to a read/write loop.
 */
#include <os2win.h>

extern "C" {

BOOL WINAPI TransmitFile(DWORD sock, HANDLE file, DWORD bytes, DWORD perSend,
                         PVOID overlapped, PVOID buffers, DWORD flags)
{
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

} // extern "C"
