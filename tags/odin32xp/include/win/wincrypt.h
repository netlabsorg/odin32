/* $Id: wincrypt.h,v 1.2 2000-03-02 23:07:56 sandervl Exp $ */

#ifndef __WINE_WINCRYPT_H
#define __WINE_WINCRYPT_H

/* some typedefs for function parameters */
#define GET_ALG_CLASS(x)                (x & (7 << 13))
#define GET_ALG_TYPE(x)                 (x & (15 << 9))
#define GET_ALG_SID(x)                  (x & (511))

#define ALG_CLASS_ANY                   (0)
#define ALG_CLASS_SIGNATURE             (1 << 13)
#define ALG_CLASS_MSG_ENCRYPT           (2 << 13)
#define ALG_CLASS_DATA_ENCRYPT          (3 << 13)
#define ALG_CLASS_HASH                  (4 << 13)
#define ALG_CLASS_KEY_EXCHANGE          (5 << 13)

#define ALG_TYPE_ANY                    (0)
#define ALG_TYPE_DSS                    (1 << 9)
#define ALG_TYPE_RSA                    (2 << 9)
#define ALG_TYPE_BLOCK                  (3 << 9)
#define ALG_TYPE_STREAM                 (4 << 9)

#define ALG_SID_ANY                     (0)

#define ALG_SID_RSA_ANY                 0
#define ALG_SID_RSA_PKCS                1
#define ALG_SID_RSA_MSATWORK            2
#define ALG_SID_RSA_ENTRUST             3
#define ALG_SID_RSA_PGP                 4

#define ALG_SID_DSS_ANY                 0
#define ALG_SID_DSS_PKCS                1
#define ALG_SID_DSS_DMS                 2

#define ALG_SID_DES                     1
#define ALG_SID_3DES			3
#define ALG_SID_DESX			4
#define ALG_SID_IDEA			5
#define ALG_SID_CAST			6
#define ALG_SID_SAFERSK64		7
#define ALD_SID_SAFERSK128		8

#define CRYPT_MODE_CBCI			6
#define CRYPT_MODE_CFBP			7
#define CRYPT_MODE_OFBP			8
#define CRYPT_MODE_CBCOFM		9
#define CRYPT_MODE_CBCOFMI		10

#define ALG_SID_RC2                     2

#define ALG_SID_RC4                     1
#define ALG_SID_SEAL                    2

#define ALG_SID_MD2                     1
#define ALG_SID_MD4                     2
#define ALG_SID_MD5                     3
#define ALG_SID_SHA                     4
#define ALG_SID_MAC                     5
#define ALG_SID_RIPEMD			6
#define ALG_SID_RIPEMD160		7
#define ALG_SID_SSL3SHAMD5		8
#define ALG_SID_EXAMPLE                 80

#undef ALGIDDEF
#define ALGIDDEF
typedef unsigned int ALG_ID;

#define CALG_MD2        (ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_MD2)
#define CALG_MD4        (ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_MD4)
#define CALG_MD5        (ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_MD5)
#define CALG_SHA        (ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_SHA)
#define CALG_MAC        (ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_MAC)
#define CALG_RSA_SIGN   (ALG_CLASS_SIGNATURE | ALG_TYPE_RSA | ALG_SID_RSA_ANY)
#define CALG_DSS_SIGN   (ALG_CLASS_SIGNATURE | ALG_TYPE_DSS | ALG_SID_DSS_ANY)
#define CALG_RSA_KEYX   (ALG_CLASS_KEY_EXCHANGE|ALG_TYPE_RSA|ALG_SID_RSA_ANY)
#define CALG_DES        (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_BLOCK|ALG_SID_DES)
#define CALG_RC2        (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_BLOCK|ALG_SID_RC2)
#define CALG_RC4        (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_STREAM|ALG_SID_RC4)
#define CALG_SEAL       (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_STREAM|ALG_SID_SEAL)

#define CRYPT_VERIFYCONTEXT     0xF0000000
#define CRYPT_NEWKEYSET         0x8
#define CRYPT_DELETEKEYSET      0x10

#define CRYPT_EXPORTABLE        0x00000001
#define CRYPT_USER_PROTECTED    0x00000002
#define CRYPT_CREATE_SALT       0x00000004
#define CRYPT_UPDATE_KEY        0x00000008

#define SIMPLEBLOB              0x1
#define PUBLICKEYBLOB           0x6
#define PRIVATEKEYBLOB          0x7

#define AT_KEYEXCHANGE          1
#define AT_SIGNATURE            2

#define CRYPT_USERDATA          1

#define KP_IV                   1
#define KP_SALT                 2
#define KP_PADDING              3
#define KP_MODE                 4
#define KP_MODE_BITS            5
#define KP_PERMISSIONS          6
#define KP_ALGID                7
#define KP_BLOCKLEN             8

#define PKCS5_PADDING           1

#define CRYPT_MODE_CBC          1
#define CRYPT_MODE_ECB          2
#define CRYPT_MODE_OFB          3
#define CRYPT_MODE_CFB          4
#define CRYPT_MODE_CTS          5

#define CRYPT_ENCRYPT           0x0001
#define CRYPT_DECRYPT           0x0002
#define CRYPT_EXPORT            0x0004
#define CRYPT_READ              0x0008
#define CRYPT_WRITE             0x0010
#define CRYPT_MAC               0x0020

#define HP_ALGID                0x0001
#define HP_HASHVAL              0x0002
#define HP_HASHSIZE             0x0004


#define CRYPT_FAILED            FALSE
#define CRYPT_SUCCEED           TRUE

#define RCRYPT_SUCCEEDED(rt)     ((rt) == CRYPT_SUCCEED)
#define RCRYPT_FAILED(rt)        ((rt) == CRYPT_FAILED)

#define PP_ENUMALGS             1
#define PP_ENUMCONTAINERS       2
#define PP_IMPTYPE              3
#define PP_NAME                 4
#define PP_VERSION              5
#define PP_CONTAINER            6

#define CRYPT_FIRST             1
#define CRYPT_NEXT              2

#define CRYPT_IMPL_HARDWARE     1
#define CRYPT_IMPL_SOFTWARE     2
#define CRYPT_IMPL_MIXED        3
#define CRYPT_IMPL_UNKNOWN      4

#define PP_CLIENT_HWND          1

#define PROV_RSA_FULL           1
#define PROV_RSA_SIG            2
#define PROV_DSS                3
#define PROV_FORTEZZA           4
#define PROV_MS_EXCHANGE        5
#define PROV_SSL                6

#define PROV_STT_MER            7
#define PROV_STT_ACQ            8
#define PROV_STT_BRND           9
#define PROV_STT_ROOT           10
#define PROV_STT_ISS            11

#define MAXUIDLEN               64

#define CUR_BLOB_VERSION        2

typedef struct _VTableProvStruc {
    DWORD   Version;
    FARPROC FuncVerifyImage;
    FARPROC FuncReturnhWnd;
} VTableProvStruc, *PVTableProvStruc;

typedef unsigned long HCRYPTPROV;
typedef unsigned long HCRYPTKEY;
typedef unsigned long HCRYPTHASH;

typedef struct _PROV_ENUMALGS {
    ALG_ID    aiAlgid;
    DWORD     dwBitLen;
    DWORD     dwNameLen;
    CHAR      szName[20];
} PROV_ENUMALGS;

typedef struct _PUBLICKEYSTRUC {
        BYTE    bType;
        BYTE    bVersion;
        WORD    reserved;
        ALG_ID  aiKeyAlg;
} PUBLICKEYSTRUC;

typedef struct _RSAPUBKEY {
        DWORD   magic;
        DWORD   bitlen;
        DWORD   pubexp;
} RSAPUBKEY;

/* function declarations */

BOOL WINAPI CryptAcquireContextA(HCRYPTPROV *phProv, LPCSTR pszContainer,
                                 LPCSTR pszProvider, DWORD dwProvType,
                                 DWORD dwFlags);
BOOL WINAPI CryptAcquireContextW(HCRYPTPROV *phProv, LPCWSTR pszContainer,
                                 LPCWSTR pszProvider, DWORD dwProvType,
                                 DWORD dwFlags);
#define     CryptAcquireContext WINELIB_NAME_AW(CryptAcquireContext)

BOOL WINAPI CryptReleaseContext(HCRYPTPROV hProv, DWORD dwFlags);
BOOL WINAPI CryptGenKey(HCRYPTPROV hProv, ALG_ID Algid, DWORD dwFlags,
                        HCRYPTKEY *phKey);
BOOL WINAPI CryptDeriveKey(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTHASH hBaseData,
                           DWORD dwFlags, HCRYPTKEY *phKey);
BOOL WINAPI CryptDestroyKey(HCRYPTKEY hKey);
BOOL WINAPI CryptSetKeyParam(HCRYPTKEY hKey,DWORD dwParam,BYTE *pbData,DWORD dwFlags);
BOOL WINAPI CryptGetKeyParam(HCRYPTKEY hKey, DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen, DWORD dwFlags);
BOOL WINAPI CryptSetHashParam(HCRYPTHASH hHash, DWORD dwParam, BYTE *pbData, DWORD dwFlags);
BOOL WINAPI CryptGetHashParam(HCRYPTHASH hHash, DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen,DWORD dwFlags);
BOOL WINAPI CryptSetProvParam(HCRYPTPROV hProv, DWORD dwParam, BYTE *pbData,DWORD dwFlags);
BOOL WINAPI CryptGetProvParam(HCRYPTPROV hProv, DWORD dwParam, BYTE *pbData,DWORD *pdwDataLen,
                              DWORD dwFlags);
BOOL WINAPI CryptGenRandom(HCRYPTPROV hProv, DWORD dwLen, BYTE *pbBuffer);
BOOL WINAPI CryptGetUserKey(HCRYPTPROV hProv, DWORD dwKeySpec, HCRYPTKEY *phUserKey);
BOOL WINAPI CryptExportKey(HCRYPTKEY hKey,HCRYPTKEY hExpKey, DWORD dwBlobType, DWORD dwFlags,
                           BYTE *pbData, DWORD *pdwDataLen);
BOOL WINAPI CryptImportKey(HCRYPTPROV hProv,CONST BYTE *pbData, DWORD dwDataLen,
                           HCRYPTKEY hPubKey, DWORD dwFlags,HCRYPTKEY *phKey);
BOOL WINAPI CryptEncrypt(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final,
                         DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen,
                         DWORD dwBufLen);
BOOL WINAPI CryptDecrypt(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags,
                         BYTE *pbData, DWORD *pdwDataLen);
BOOL WINAPI CryptCreateHash(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTKEY hKey, DWORD dwFlags,
                            HCRYPTHASH *phHash);
BOOL WINAPI CryptHashData(HCRYPTHASH hHash, CONST BYTE *pbData, DWORD dwDataLen,
                          DWORD dwFlags);
BOOL WINAPI CryptHashSessionKey(HCRYPTHASH hHash, HCRYPTKEY hKey, DWORD dwFlags);
BOOL WINAPI CryptDestroyHash(HCRYPTHASH hHash);
BOOL WINAPI CryptSignHashA(HCRYPTHASH hHash, DWORD dwKeySpec, LPCSTR sDescription,
                           DWORD dwFlags, BYTE *pbSignature, DWORD *pdwSigLen);
BOOL WINAPI CryptSignHashW(HCRYPTHASH hHash, DWORD dwKeySpec, LPCWSTR sDescription,
                           DWORD dwFlags, BYTE *pbSignature, DWORD *pdwSigLen);
#define     CryptSignHash WINELIB_NAME_AW(CryptSignHash)

BOOL WINAPI CryptVerifySignatureA(HCRYPTHASH hHash, CONST BYTE *pbSignature, DWORD dwSigLen,
                                  HCRYPTKEY hPubKey, LPCSTR sDescription, DWORD dwFlags);
BOOL WINAPI CryptVerifySignatureW(HCRYPTHASH hHash, CONST BYTE *pbSignature, DWORD dwSigLen,
                                  HCRYPTKEY hPubKey, LPCWSTR sDescription, DWORD dwFlags);
#define     CryptVerifySignature WINELIB_NAME_AW(CryptVerifySignature)

BOOL WINAPI CryptSetProviderA(LPCSTR pszProvName, DWORD dwProvType);
BOOL WINAPI CryptSetProviderW(LPCWSTR pszProvName, DWORD dwProvType);
#define     CryptSetProvider WINELIB_NAME_AW(CryptSetProvider)

#endif
