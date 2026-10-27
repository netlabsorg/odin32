/*
 * bcrypt - CNG (Cryptography API: Next Generation) stub
 * Odin32 compatibility DLL: provides the exports that current Windows programs
 * import so that they can be loaded; see the individual functions for what is
 * implemented and what merely reports "not supported".
 */
#include <os2win.h>

#define VC_STATUS_NOT_SUPPORTED     ((LONG)0xC00000BB)

/* No cryptographic provider is available: every call reports
   STATUS_NOT_SUPPORTED and callers (e.g. file hashing in editors) show an error. */
extern "C" {

LONG WINAPI BCryptOpenAlgorithmProvider(LPVOID *phAlgorithm, LPCWSTR pszAlgId,
                                        LPCWSTR pszImplementation, ULONG dwFlags)
{
    if(phAlgorithm) *phAlgorithm = NULL;
    return VC_STATUS_NOT_SUPPORTED;
}

LONG WINAPI BCryptCloseAlgorithmProvider(LPVOID hAlgorithm, ULONG dwFlags)
{
    return 0;
}

LONG WINAPI BCryptGetProperty(LPVOID hObject, LPCWSTR pszProperty, PUCHAR pbOutput,
                              ULONG cbOutput, ULONG *pcbResult, ULONG dwFlags)
{
    if(pcbResult) *pcbResult = 0;
    return VC_STATUS_NOT_SUPPORTED;
}

LONG WINAPI BCryptCreateHash(LPVOID hAlgorithm, LPVOID *phHash, PUCHAR pbHashObject,
                             ULONG cbHashObject, PUCHAR pbSecret, ULONG cbSecret, ULONG dwFlags)
{
    if(phHash) *phHash = NULL;
    return VC_STATUS_NOT_SUPPORTED;
}

LONG WINAPI BCryptHashData(LPVOID hHash, PUCHAR pbInput, ULONG cbInput, ULONG dwFlags)
{
    return VC_STATUS_NOT_SUPPORTED;
}

LONG WINAPI BCryptFinishHash(LPVOID hHash, PUCHAR pbOutput, ULONG cbOutput, ULONG dwFlags)
{
    return VC_STATUS_NOT_SUPPORTED;
}

LONG WINAPI BCryptDestroyHash(LPVOID hHash)
{
    return 0;
}

} // extern "C"
