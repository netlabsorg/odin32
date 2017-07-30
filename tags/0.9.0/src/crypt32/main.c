/*
 * Copyright 2002 Mike McCormack for CodeWeavers
 * Copyright 2005 Juan Lang
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

#include "config.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "windef.h"
#include "winbase.h"
#include "wincrypt.h"
#include "winreg.h"
#include "winuser.h"
#include "i_cryptasn1tls.h"
#include "crypt32_private.h"
#include "wine/debug.h"

#define DBG_LOCALLOG	DBG_crypt32
#include "dbglocal.h"


ODINDEBUGCHANNEL(CRYPT32-CRYPT32)

static HCRYPTPROV hDefProv;
HINSTANCE hInstance;

BOOL WINAPI Crypt32DllMain(HINSTANCE hInst, DWORD fdwReason, PVOID pvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            hInstance = hInst;
            DisableThreadLibraryCalls(hInst);
            crypt_oid_init();
            break;
        case DLL_PROCESS_DETACH:
            crypt_oid_free();
            crypt_sip_free();
            root_store_free();
            default_chain_engine_free();
            /* Don't release the default provider on process shutdown, there's
             * no guarantee the provider dll hasn't already been unloaded.
             */
            if (hDefProv && !pvReserved) CryptReleaseContext(hDefProv, 0);
            break;
    }
    return TRUE;
}

HCRYPTPROV CRYPT_GetDefaultProvider(void)
{
    if (!hDefProv)
    {
        HCRYPTPROV prov;

        CryptAcquireContextW(&prov, NULL, MS_ENHANCED_PROV_W, PROV_RSA_FULL,
         CRYPT_VERIFYCONTEXT);
        InterlockedCompareExchangePointer((PVOID *)&hDefProv, (PVOID)prov,
         NULL);
        if (hDefProv != prov)
            CryptReleaseContext(prov, 0);
    }
    return hDefProv;
}

typedef void * HLRUCACHE;

/* this function is called by Internet Explorer when it is about to verify a
 * downloaded component.  The first parameter appears to be a pointer to an
 * unknown type, native fails unless it points to a buffer of at least 20 bytes.
 * The second parameter appears to be an out parameter, whatever it's set to is
 * passed (by cryptnet.dll) to I_CryptFlushLruCache.
 */
BOOL WINAPI I_CryptCreateLruCache(void *unknown, HLRUCACHE *out)
{
    FIXME("(%p, %p): stub!\n", unknown, out);
    *out = (void *)0xbaadf00d;
    return TRUE;
}

BOOL WINAPI I_CryptFindLruEntry(DWORD unk0, DWORD unk1)
{
    FIXME("(%08x, %08x): stub!\n", unk0, unk1);
    return FALSE;
}

BOOL WINAPI I_CryptFindLruEntryData(DWORD unk0, DWORD unk1, DWORD unk2)
{
    FIXME("(%08x, %08x, %08x): stub!\n", unk0, unk1, unk2);
    return FALSE;
}

BOOL WINAPI I_CryptCreateLruEntry(HLRUCACHE h, DWORD unk0, DWORD unk1)
{
    FIXME("(%p, %08x, %08x): stub!\n", h, unk0, unk1);
    return FALSE;
}

DWORD WINAPI I_CryptFlushLruCache(HLRUCACHE h, DWORD unk0, DWORD unk1)
{
    FIXME("(%p, %08x, %08x): stub!\n", h, unk0, unk1);
    return 0;
}

HLRUCACHE WINAPI I_CryptFreeLruCache(HLRUCACHE h, DWORD unk0, DWORD unk1)
{
    FIXME("(%p, %08x, %08x): stub!\n", h, unk0, unk1);
    return h;
}

LPVOID WINAPI CryptMemAlloc(ULONG cbSize)
{
    return HeapAlloc(GetProcessHeap(), 0, cbSize);
}

LPVOID WINAPI CryptMemRealloc(LPVOID pv, ULONG cbSize)
{
    return HeapReAlloc(GetProcessHeap(), 0, pv, cbSize);
}

VOID WINAPI CryptMemFree(LPVOID pv)
{
    HeapFree(GetProcessHeap(), 0, pv);
}

DWORD WINAPI I_CryptAllocTls(void)
{
    return TlsAlloc();
}

LPVOID WINAPI I_CryptDetachTls(DWORD dwTlsIndex)
{
    LPVOID ret;

    ret = TlsGetValue(dwTlsIndex);
    TlsSetValue(dwTlsIndex, NULL);
    return ret;
}

LPVOID WINAPI I_CryptGetTls(DWORD dwTlsIndex)
{
    return TlsGetValue(dwTlsIndex);
}

BOOL WINAPI I_CryptSetTls(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
    return TlsSetValue(dwTlsIndex, lpTlsValue);
}

BOOL WINAPI I_CryptFreeTls(DWORD dwTlsIndex, DWORD unknown)
{
    TRACE("(%d, %d)\n", dwTlsIndex, unknown);
    return TlsFree(dwTlsIndex);
}

BOOL WINAPI I_CryptGetOssGlobal(DWORD x)
{
    FIXME("%08x\n", x);
    return FALSE;
}

HCRYPTPROV WINAPI I_CryptGetDefaultCryptProv(DWORD reserved)
{
    HCRYPTPROV ret;

    TRACE("(%08x)\n", reserved);

    if (reserved)
    {
        SetLastError(E_INVALIDARG);
        return (HCRYPTPROV)0;
    }
    ret = CRYPT_GetDefaultProvider();
    CryptContextAddRef(ret, NULL, 0);
    return ret;
}

BOOL WINAPI I_CryptReadTrustedPublisherDWORDValueFromRegistry(LPCWSTR name,
 DWORD *value)
{
    static const WCHAR safer[] = { 
     'S','o','f','t','w','a','r','e','\\','P','o','l','i','c','i','e','s','\\',
     'M','i','c','r','o','s','o','f','t','\\','S','y','s','t','e','m',
     'C','e','r','t','i','f','i','c','a','t','e','s','\\',
     'T','r','u','s','t','e','d','P','u','b','l','i','s','h','e','r','\\',
     'S','a','f','e','r',0 };
    HKEY key;
    LONG rc;
    BOOL ret = FALSE;

    TRACE("(%s, %p)\n", debugstr_w(name), value);

    *value = 0;
    rc = RegCreateKeyW(HKEY_LOCAL_MACHINE, safer, &key);
    if (rc == ERROR_SUCCESS)
    {
        DWORD size = sizeof(DWORD);

        if (!RegQueryValueExW(key, name, NULL, NULL, (LPBYTE)value, &size))
            ret = TRUE;
        RegCloseKey(key);
    }
    return ret;
}

DWORD WINAPI I_CryptInstallOssGlobal(DWORD x, DWORD y, DWORD z)
{
    static int ret = 8;
    ret++;
    FIXME("%08x %08x %08x, return value %d\n", x, y, z,ret);
    return ret;
}

BOOL WINAPI I_CryptInstallAsn1Module(ASN1module_t x, DWORD y, void* z)
{
    FIXME("(%p %08x %p): stub\n", x, y, z);
    return TRUE;
}

BOOL WINAPI I_CryptUninstallAsn1Module(HCRYPTASN1MODULE x)
{
    FIXME("(%08x): stub\n", x);
    return TRUE;
}

ASN1decoding_t WINAPI I_CryptGetAsn1Decoder(HCRYPTASN1MODULE x)
{
    FIXME("(%08x): stub\n", x);
    return NULL;
}

ASN1encoding_t WINAPI I_CryptGetAsn1Encoder(HCRYPTASN1MODULE x)
{
    FIXME("(%08x): stub\n", x);
    return NULL;
}

void ulong2string (unsigned long number, char *string, int n, int base)
{
    static char *digits = "0123456789ABCDEF";
    
    unsigned long tmp = number;
    char *s = string;
    int len = 0;
    int l = 0;
    int i;
    
    if (n <= 0)
    {
        return;
    }
    
    if (tmp == 0)
    {
        s[l++] = digits[0];
    }
    
    while (tmp != 0)
    {
        if (l >= n)
        {
            break;
        }
        s[l++] = digits[tmp%base];
        len++;
        tmp /= base;
    }
    if (l < n)
    {
        s[l++] = '\0';
    }
    
    s = string;
    
    for (i = 0; i < len/2; i++)
    {
        tmp = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = tmp;
    }

    return;
}

void long2string (long number, char *string, int n, int base)
{
    if (n <= 0)
    {
        return;
    }
    
    if (number < 0)
    {
        *string++ = '-';
        number = -number;
        n--;
    }
    
    ulong2string (number, string, n, base);
}
 
int string2ulong (const char *string, char **pstring2, unsigned long *pvalue, int base)
{
    unsigned long value = 0;
    int sign = 1;
    
    const char *p = string;
    
    if (p[0] == '-')
    {
        sign = -1;
        p++;
    }
    
    if (base == 0)
    {
        if (p[0] == 0 && (p[1] == 'x' || p[1] == 'X'))
        {
            base = 16;
            p += 2;
        }
        else if (p[0] == 0)
        {
            base = 8;
            p += 1;
        }
        else
        {
            base = 10;
        }
    }
            
    while (*p)
    {
        int digit = 0;
        
        if ('0' <= *p && *p <= '9')
        {
            digit = *p - '0';
        }
        else if ('a' <= *p && *p <= 'f')
        {
            digit = *p - 'a' + 0xa;
        }
        else if ('A' <= *p && *p <= 'F')
        {
            digit = *p - 'A' + 0xa;
        }
        else
        {
            break;
        }
        
        if (digit >= base)
        {
            break;
        }
        
        value = value*base + digit;
        
        p++;
    }
    
    if (pstring2)
    {
        *pstring2 = (char *)p;
    }
    
    *pvalue = sign*value;
    
    return 1;
}

int crypt32_vsnprintf (char *buf, int n, const char *fmt, va_list args)
{
    int count = 0;
    char *s = (char *)fmt;
    char *d = buf;
    
    if (n <= 0)
    {
        return 0;
    }
    
    n--;
    
    while (*s && count < n)
    {
        char tmpstr[16];
     
        char *str = NULL;
        
        int width = 0;
        int precision = 0;
            
        if (*s == '%')
        {
            s++;
            
            if ('0' <= *s && *s <= '9' || *s == '-')
            {
                char setprec = (*s == '0');
                string2ulong (s, &s, (unsigned long *)&width, 10);
                if (setprec)
                {
                    precision = width;
                }
            }

            if (*s == '.')
            {
                s++;
                string2ulong (s, &s, (unsigned long *)&precision, 10);
            }
            
            if (*s == 's')
            {
                str = va_arg(args, char *);
                s++;
                precision = 0;
            }
            else if (*s == 'c')
            {
                tmpstr[0] = va_arg(args, int);
                tmpstr[1] = 0;
                str = &tmpstr[0];
                s++;
                precision = 0;
            }
            else if (*s == 'p' || *s == 'P')
            {
                int num = va_arg(args, int);
            
                ulong2string (num, tmpstr, sizeof (tmpstr), 16);
                
                str = &tmpstr[0];
                s++;
                width = 8;
                precision = 8;
            }
            else
            {
                if (*s == 'l')
                {
                    s++;
                }
                
                if (*s == 'd' || *s == 'i')
                {
                    int num = va_arg(args, int);
                
                    long2string (num, tmpstr, sizeof (tmpstr), 10);
                
                    str = &tmpstr[0];
                    s++;
                }
                else if (*s == 'u')
                {
                    int num = va_arg(args, int);
                
                    ulong2string (num, tmpstr, sizeof (tmpstr), 10);
                
                    str = &tmpstr[0];
                    s++;
                }
                else if (*s == 'x' || *s == 'X')
                {
                    int num = va_arg(args, int);
                
                    ulong2string (num, tmpstr, sizeof (tmpstr), 16);
                
                    str = &tmpstr[0];
                    s++;
                }
            }
        }
        
        if (str != NULL)
        {
            int i;
            char numstr[16];
            int len = strlen (str);
            int leftalign = 0;
            
            if (width < 0)
            {
                width = -width;
                leftalign = 1;
            }
            
            if (precision)
            {
                i = 0;
                while (precision > len)
                {
                    numstr[i++] = '0';
                    precision--;
                }
                
                memcpy (&numstr[i], str, len);
                
                str = &numstr[0];
                len += i;
            }
            
            if (len < width && !leftalign)
            {
                while (len < width && count < n)
                {
                    *d++ = ' ';
                    width--;
                    count++;
                }
                
                if (count >= n)
                {
                    break;
                }
            }
            
            i = 0;
            while (i < len && count < n)
            {
                *d++ = str[i++];
                count++;
            }
                
            if (count >= n)
            {
                break;
            }
            
            if (len < width && leftalign)
            {
                while (len < width && count < n)
                {
                    *d++ = ' ';
                    width--;
                    count++;
                }
                
                if (count >= n)
                {
                    break;
                }
            }
        }
        else
        {
            *d++ = *s++;
            count++;
        }
    }
    
    *d = '\0';
    
    return count + 1;
}

int crypt32_snprintf (char *buf, int n, const char *fmt, ...)
{
    va_list args;
    
    int rc = 0;

    va_start (args, fmt);
    
    rc = crypt32_vsnprintf (buf, n, fmt, args);

    va_end (args);
    
    return rc;
}
