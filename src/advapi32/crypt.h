/*
 * Driver routines
 *
 * Copyright 2001 - Travis Michielsen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_CRYPT_H
#define __WINE_CRYPT_H

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "wincrypt.h"

/*RLW - created typedefs for each function to enable pointers
  to be cast to the appropriate type when loaded using GetProcAddress()
*/
typedef BOOL (WINAPI fnCPAcquireContext)(HCRYPTPROV *phProv, LPSTR pszContainer, DWORD dwFlags, PVTableProvStruc pVTable);
typedef BOOL (WINAPI fnCPCreateHash)(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTKEY hKey, DWORD dwFlags, HCRYPTHASH *phHash);
typedef BOOL (WINAPI fnCPDecrypt)(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
typedef BOOL (WINAPI fnCPDeriveKey)(HCRYPTPROV hProv, ALG_ID     Algid, HCRYPTHASH hBaseData, DWORD dwFlags, HCRYPTKEY *phKey);
typedef BOOL (WINAPI fnCPDestroyHash)(HCRYPTPROV hProv, HCRYPTHASH hHash);
typedef BOOL (WINAPI fnCPDestroyKey)(HCRYPTPROV hProv, HCRYPTKEY hKey);
typedef BOOL (WINAPI fnCPDuplicateHash)(HCRYPTPROV hUID, HCRYPTHASH hHash, DWORD *pdwReserved, DWORD dwFlags, HCRYPTHASH *phHash);
typedef BOOL (WINAPI fnCPDuplicateKey)(HCRYPTPROV hUID, HCRYPTKEY hKey, DWORD *pdwReserved, DWORD dwFlags, HCRYPTKEY *phKey);
typedef BOOL (WINAPI fnCPEncrypt)(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen);
typedef BOOL (WINAPI fnCPExportKey)(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTKEY hPubKey, DWORD dwBlobType, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
typedef BOOL (WINAPI fnCPGenKey)(HCRYPTPROV hProv, ALG_ID Algid, DWORD dwFlags, HCRYPTKEY *phKey);
typedef BOOL (WINAPI fnCPGenRandom)(HCRYPTPROV hProv, DWORD dwLen, BYTE *pbBuffer);
typedef BOOL (WINAPI fnCPGetHashParam)(HCRYPTPROV hProv, HCRYPTHASH hHash, DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen, DWORD dwFlags);
typedef BOOL (WINAPI fnCPGetKeyParam)(HCRYPTPROV hProv, HCRYPTKEY hKey, DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen, DWORD dwFlags);
typedef BOOL (WINAPI fnCPGetProvParam)(HCRYPTPROV hProv, DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen, DWORD dwFlags);
typedef BOOL (WINAPI fnCPGetUserKey)(HCRYPTPROV hProv, DWORD dwKeySpec, HCRYPTKEY *phUserKey);
typedef BOOL (WINAPI fnCPHashData)(HCRYPTPROV hProv, HCRYPTHASH hHash, CONST BYTE *pbData, DWORD dwDataLen, DWORD dwFlags);
typedef BOOL (WINAPI fnCPHashSessionKey)(HCRYPTPROV hProv, HCRYPTHASH hHash, HCRYPTKEY hKey, DWORD dwFlags);
typedef BOOL (WINAPI fnCPImportKey)(HCRYPTPROV hProv, CONST BYTE *pbData, DWORD dwDataLen, HCRYPTKEY hPubKey, DWORD dwFlags, HCRYPTKEY *phKey);
typedef BOOL (WINAPI fnCPReleaseContext)(HCRYPTPROV hProv, DWORD dwFlags);
typedef BOOL (WINAPI fnCPSetHashParam)(HCRYPTPROV hProv, HCRYPTHASH hHash, DWORD dwParam, CONST BYTE *pbData, DWORD dwFlags);
typedef BOOL (WINAPI fnCPSetKeyParam)(HCRYPTPROV hProv, HCRYPTKEY hKey, DWORD dwParam, CONST BYTE *pbData, DWORD dwFlags);
typedef BOOL (WINAPI fnCPSetProvParam)(HCRYPTPROV hProv, DWORD dwParam, CONST BYTE *pbData, DWORD dwFlags);
typedef BOOL (WINAPI fnCPSignHash)(HCRYPTPROV hProv, HCRYPTHASH hHash, DWORD dwKeySpec, LPCWSTR sDescription, DWORD dwFlags, BYTE *pbSignature, DWORD *pdwSigLen);
typedef BOOL (WINAPI fnCPVerifySignature)(HCRYPTPROV hProv, HCRYPTHASH hHash, CONST BYTE *pbSignature, DWORD dwSigLen, HCRYPTKEY hPubKey, LPCWSTR sDescription, DWORD dwFlags);

typedef struct tagPROVFUNCS
{
    fnCPAcquireContext*  pCPAcquireContext;
    fnCPCreateHash*      pCPCreateHash;
    fnCPDecrypt*         pCPDecrypt;
    fnCPDeriveKey*       pCPDeriveKey;
    fnCPDestroyHash*     pCPDestroyHash;
    fnCPDestroyKey*      pCPDestroyKey;
    fnCPDuplicateHash*   pCPDuplicateHash;
    fnCPDuplicateKey*    pCPDuplicateKey;
    fnCPEncrypt*         pCPEncrypt;
    fnCPExportKey*       pCPExportKey;
    fnCPGenKey*          pCPGenKey;
    fnCPGenRandom*       pCPGenRandom;
    fnCPGetHashParam*    pCPGetHashParam;
    fnCPGetKeyParam*     pCPGetKeyParam;
    fnCPGetProvParam*    pCPGetProvParam;
    fnCPGetUserKey*      pCPGetUserKey;
    fnCPHashData*        pCPHashData;
    fnCPHashSessionKey*  pCPHashSessionKey;
    fnCPImportKey*       pCPImportKey;
    fnCPReleaseContext*  pCPReleaseContext;
    fnCPSetHashParam*    pCPSetHashParam;
    fnCPSetKeyParam*     pCPSetKeyParam;
    fnCPSetProvParam*    pCPSetProvParam;
    fnCPSignHash*        pCPSignHash;
    fnCPVerifySignature* pCPVerifySignature;
} PROVFUNCS, *PPROVFUNCS;

#define MAGIC_CRYPTPROV 0xA39E741F

typedef struct tagCRYPTPROV
{
	DWORD dwMagic;
	UINT refcount;
	HMODULE hModule;
	PPROVFUNCS pFuncs;
        HCRYPTPROV hPrivate;  /*CSP's handle - Should not be given to application under any circumstances!*/
	PVTableProvStruc pVTable;
} CRYPTPROV, *PCRYPTPROV;

typedef struct tagCRYPTKEY
{
	PCRYPTPROV pProvider;
        HCRYPTKEY hPrivate;    /*CSP's handle - Should not be given to application under any circumstances!*/
} CRYPTKEY, *PCRYPTKEY;

typedef struct tagCRYPTHASH
{
	PCRYPTPROV pProvider;
        HCRYPTHASH hPrivate;    /*CSP's handle - Should not be given to application under any circumstances!*/
} CRYPTHASH, *PCRYPTHASH;

#define MAXPROVTYPES 999

extern unsigned char *CRYPT_DESkey8to7( unsigned char *dst, const unsigned char *key );
extern unsigned char *CRYPT_DEShash( unsigned char *dst, const unsigned char *key,
                                     const unsigned char *src );
extern unsigned char *CRYPT_DESunhash( unsigned char *dst, const unsigned char *key,
                                       const unsigned char *src );

struct ustring {
    DWORD Length;
    DWORD MaximumLength;
    unsigned char *Buffer;
};

NTSTATUS WINAPI SystemFunction032(struct ustring *data, struct ustring *key);

#endif /* __WINE_CRYPT_H_ */
