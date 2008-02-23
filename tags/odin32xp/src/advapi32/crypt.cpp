/* $Id: crypt.cpp,v 1.3 2000-03-02 23:08:23 sandervl Exp $ */
/*
 * dlls/advapi32/crypt.c
 */

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>
#include "wincrypt.h"

ODINDEBUGCHANNEL(ADVAPI32-CRYPT)

/******************************************************************************
 * CryptAcquireContextA
 * Acquire a crypto provider context handle.
 * 
 * PARAMS
 * phProv: Pointer to HCRYPTPROV for the output.
 * pszContainer: FIXME (unknown)
 * pszProvider: FIXME (unknown)
 * dwProvType: Crypto provider type to get a handle.
 * dwFlags: flags for the operation
 *
 * RETURNS TRUE on success, FALSE on failure.
 */

BOOL WINAPI CryptAcquireContextA( HCRYPTPROV *phProv, LPCSTR pszContainer,
   	 	                  LPCSTR pszProvider, DWORD dwProvType, DWORD dwFlags)
{
    dprintf(("CryptAcquireContextA(%p, %s, %s, %ld, %08lx): FAKED.\n", phProv, pszContainer,
	  pszProvider, dwProvType, dwFlags));
    *phProv = 0x99000001;
    return TRUE;
//    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
//    return FALSE;
}

BOOL WINAPI CryptAcquireContextW(HCRYPTPROV *phProv, LPCWSTR pszContainer,
                                 LPCWSTR pszProvider, DWORD dwProvType,
                                 DWORD dwFlags)
{
    dprintf(("CryptAcquireContextW(%p, %ls, %s, %ld, %08lx): FAKED.\n", phProv, pszContainer,
	  pszProvider, dwProvType, dwFlags));
    *phProv = 0x99000001;
    return TRUE;
//    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
//    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptReleaseContext(HCRYPTPROV hProv, DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptReleaseContext %x %x", hProv, dwFlags));
    return TRUE;
//    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
//    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptGenKey(HCRYPTPROV hProv, ALG_ID Algid, DWORD dwFlags,
                        HCRYPTKEY *phKey)
{
    dprintf(("NOT IMPLEMENTED: CryptGenKey %x %x %x %x", hProv, Algid, dwFlags, phKey));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptDeriveKey(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTHASH hBaseData,
                           DWORD dwFlags, HCRYPTKEY *phKey)
{
    dprintf(("NOT IMPLEMENTED: CryptDeriveKey"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptDestroyKey(HCRYPTKEY hKey)
{
    dprintf(("NOT IMPLEMENTED: CryptDestroyKey"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptSetKeyParam(HCRYPTKEY hKey,DWORD dwParam,BYTE *pbData,DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptSetKeyParam(%lx, %lx, %p, %lx): not implemented.\n", hKey, dwParam, pbData, dwFlags));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptGetKeyParam(HCRYPTKEY hKey, DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen, DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptGetKeyParam"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptSetHashParam(HCRYPTHASH hHash, DWORD dwParam, BYTE *pbData, DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptSetHashParam"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptGetHashParam(HCRYPTHASH hHash, DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen,DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptGetHashParam"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptSetProvParam(HCRYPTPROV hProv, DWORD dwParam, BYTE *pbData,DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptSetProvParam"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptGetProvParam(HCRYPTPROV hProv, DWORD dwParam, BYTE *pbData,DWORD *pdwDataLen,
                              DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptGetProvParam"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptGenRandom(HCRYPTPROV hProv, DWORD dwLen, BYTE *pbBuffer)
{
    dprintf(("NOT IMPLEMENTED: CryptGenRandom"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptGetUserKey(HCRYPTPROV hProv, DWORD dwKeySpec, HCRYPTKEY *phUserKey)
{
    dprintf(("NOT IMPLEMENTED: CryptGetUserKey"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptExportKey(HCRYPTKEY hKey,HCRYPTKEY hExpKey, DWORD dwBlobType, DWORD dwFlags,
                           BYTE *pbData, DWORD *pdwDataLen)
{
    dprintf(("NOT IMPLEMENTED: CryptExportKey"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptImportKey(HCRYPTPROV hProv,CONST BYTE *pbData, DWORD dwDataLen,
                           HCRYPTKEY hPubKey, DWORD dwFlags,HCRYPTKEY *phKey)
{
    dprintf(("NOT IMPLEMENTED: CryptImportKey"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptEncrypt(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final,
                         DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen,
                         DWORD dwBufLen)
{
    dprintf(("NOT IMPLEMENTED: CryptEncrypt"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptDecrypt(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags,
                         BYTE *pbData, DWORD *pdwDataLen)
{
    dprintf(("NOT IMPLEMENTED: CryptDecrypt"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptCreateHash(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTKEY hKey, DWORD dwFlags,
                            HCRYPTHASH *phHash)
{
    dprintf(("NOT IMPLEMENTED: CryptCreateHash"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptHashData(HCRYPTHASH hHash, CONST BYTE *pbData, DWORD dwDataLen,
                          DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptHashData"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptHashSessionKey(HCRYPTHASH hHash, HCRYPTKEY hKey, DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptHashSessionKey"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptDestroyHash(HCRYPTHASH hHash)
{
    dprintf(("NOT IMPLEMENTED: CryptDestroyHash"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptSignHashA(HCRYPTHASH hHash, DWORD dwKeySpec, LPCSTR sDescription,
                           DWORD dwFlags, BYTE *pbSignature, DWORD *pdwSigLen)
{
    dprintf(("NOT IMPLEMENTED: CryptSignHashA"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptSignHashW(HCRYPTHASH hHash, DWORD dwKeySpec, LPCWSTR sDescription,
                           DWORD dwFlags, BYTE *pbSignature, DWORD *pdwSigLen)
{
    dprintf(("NOT IMPLEMENTED: CryptSignHashW"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptVerifySignatureA(HCRYPTHASH hHash, CONST BYTE *pbSignature, DWORD dwSigLen,
                                  HCRYPTKEY hPubKey, LPCSTR sDescription, DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptVerifySignatureA"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptVerifySignatureW(HCRYPTHASH hHash, CONST BYTE *pbSignature, DWORD dwSigLen,
                                  HCRYPTKEY hPubKey, LPCWSTR sDescription, DWORD dwFlags)
{
    dprintf(("NOT IMPLEMENTED: CryptVerifySignatureW"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptSetProviderA(LPCSTR pszProvName, DWORD dwProvType)
{
    dprintf(("NOT IMPLEMENTED: CryptSetProviderA"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CryptSetProviderW(LPCWSTR pszProvName, DWORD dwProvType)
{
    dprintf(("NOT IMPLEMENTED: CryptSetProviderW"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
//******************************************************************************
//******************************************************************************
