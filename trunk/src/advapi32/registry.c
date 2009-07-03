/*
 * Registry management
 *
 * Copyright (C) 1999 Alexandre Julliard
 *
 * Based on misc/registry.c code
 * Copyright (C) 1996 Marcus Meissner
 * Copyright (C) 1998 Matthew Becker
 * Copyright (C) 1999 Sylvain St-Germain
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
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "windef.h"
#include "winbase.h"
#include "winreg.h"
#include "winerror.h"
#include "winternl.h"
#include "winuser.h"
#include "thread.h"

#include "wine/unicode.h"
#include "debugtools.h"

//WINE_DEFAULT_DEBUG_CHANNEL(reg);

#define NtCurrentTeb GetThreadTEB
TEB  *WIN32API GetThreadTEB();

#define HKEY_SPECIAL_ROOT_FIRST   HKEY_CLASSES_ROOT
#define HKEY_SPECIAL_ROOT_LAST    HKEY_DYN_DATA
#define NB_SPECIAL_ROOT_KEYS      ((UINT)HKEY_SPECIAL_ROOT_LAST - (UINT)HKEY_SPECIAL_ROOT_FIRST + 1)

static HKEY special_root_keys[NB_SPECIAL_ROOT_KEYS];
static BOOL hkcu_cache_disabled;

static const WCHAR name_CLASSES_ROOT[] =
    {'M','a','c','h','i','n','e','\\',
     'S','o','f','t','w','a','r','e','\\',
     'C','l','a','s','s','e','s',0};
static const WCHAR name_LOCAL_MACHINE[] =
    {'M','a','c','h','i','n','e',0};
static const WCHAR name_USERS[] =
    {'U','s','e','r',0};
static const WCHAR name_PERFORMANCE_DATA[] =
    {'P','e','r','f','D','a','t','a',0};
static const WCHAR name_CURRENT_CONFIG[] =
    {'M','a','c','h','i','n','e','\\',
     'S','y','s','t','e','m','\\',
     'C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\',
     'H','a','r','d','w','a','r','e',' ','P','r','o','f','i','l','e','s','\\',
     'C','u','r','r','e','n','t',0};
static const WCHAR name_DYN_DATA[] =
    {'D','y','n','D','a','t','a',0};

static const WCHAR * const root_key_names[NB_SPECIAL_ROOT_KEYS] =
{
    name_CLASSES_ROOT,
    NULL,         /* HKEY_CURRENT_USER is determined dynamically */
    name_LOCAL_MACHINE,
    name_USERS,
    name_PERFORMANCE_DATA,
    name_CURRENT_CONFIG,
    name_DYN_DATA
};


/* check if value type needs string conversion (Ansi<->Unicode) */
static inline int is_string( DWORD type )
{
    return (type == REG_SZ) || (type == REG_EXPAND_SZ) || (type == REG_MULTI_SZ);
}

/* check if current version is NT or Win95 */
static inline int is_version_nt(void)
{
    return !(GetVersion() & 0x80000000);
}

/* create one of the HKEY_* special root keys */
static HKEY create_special_root_hkey( HANDLE hkey, DWORD access )
{
    HKEY ret = 0;
    int idx = (UINT_PTR)hkey - (UINT_PTR)HKEY_SPECIAL_ROOT_FIRST;

    if (hkey == HKEY_CURRENT_USER)
    {
        if (RtlOpenCurrentUser( access, &hkey )) return 0;
        TRACE( "HKEY_CURRENT_USER -> %p\n", hkey );

        /* don't cache the key in the table if caching is disabled */
        if (hkcu_cache_disabled)
            return hkey;
    }
    else
    {
        OBJECT_ATTRIBUTES attr;
        UNICODE_STRING name;

        attr.Length = sizeof(attr);
        attr.RootDirectory = 0;
        attr.ObjectName = &name;
        attr.Attributes = 0;
        attr.SecurityDescriptor = NULL;
        attr.SecurityQualityOfService = NULL;
        RtlInitUnicodeString( &name, root_key_names[idx] );
        if (NtCreateKey( &hkey, access, &attr, 0, NULL, 0, NULL )) return 0;
        TRACE( "%s -> %p\n", debugstr_w(attr.ObjectName->Buffer), hkey );
    }

    if (!(ret = InterlockedCompareExchangePointer( (void **)&special_root_keys[idx], hkey, 0 )))
        ret = hkey;
    else
        NtClose( hkey );  /* somebody beat us to it */
    return ret;
}

/* map the hkey from special root to normal key if necessary */
static inline HKEY get_special_root_hkey( HKEY hkey )
{
    HKEY ret = hkey;

    if ((hkey >= HKEY_SPECIAL_ROOT_FIRST) && (hkey <= HKEY_SPECIAL_ROOT_LAST))
    {
        if (!(ret = special_root_keys[(UINT_PTR)hkey - (UINT_PTR)HKEY_SPECIAL_ROOT_FIRST]))
            ret = create_special_root_hkey( hkey, KEY_ALL_ACCESS );
    }
    return ret;
}


/******************************************************************************
 * RegDeleteTreeW [ADVAPI32.@]
 *
 */
LSTATUS WINAPI RegDeleteTreeW(HKEY hKey, LPCWSTR lpszSubKey)
{
    LONG ret;
    DWORD dwMaxSubkeyLen, dwMaxValueLen;
    DWORD dwMaxLen, dwSize;
    WCHAR szNameBuf[MAX_PATH], *lpszName = szNameBuf;
    HKEY hSubKey = hKey;

    TRACE("(hkey=%p,%p %s)\n", hKey, lpszSubKey, debugstr_w(lpszSubKey));

    if(lpszSubKey)
    {
        ret = RegOpenKeyExW(hKey, lpszSubKey, 0, KEY_READ, &hSubKey);
        if (ret) return ret;
    }

    /* Get highest length for keys, values */
    ret = RegQueryInfoKeyW(hSubKey, NULL, NULL, NULL, NULL,
            &dwMaxSubkeyLen, NULL, NULL, &dwMaxValueLen, NULL, NULL, NULL);
    if (ret) goto cleanup;

    dwMaxSubkeyLen++;
    dwMaxValueLen++;
    dwMaxLen = max(dwMaxSubkeyLen, dwMaxValueLen);
    if (dwMaxLen > sizeof(szNameBuf)/sizeof(WCHAR))
    {
        /* Name too big: alloc a buffer for it */
        if (!(lpszName = HeapAlloc( GetProcessHeap(), 0, dwMaxLen*sizeof(WCHAR))))
        {
            ret = ERROR_NOT_ENOUGH_MEMORY;
            goto cleanup;
        }
    }


    /* Recursively delete all the subkeys */
    while (TRUE)
    {
        dwSize = dwMaxLen;
        if (RegEnumKeyExW(hSubKey, 0, lpszName, &dwSize, NULL,
                          NULL, NULL, NULL)) break;

        ret = RegDeleteTreeW(hSubKey, lpszName);
        if (ret) goto cleanup;
    }

    if (lpszSubKey)
        ret = RegDeleteKeyW(hKey, lpszSubKey);
    else
        while (TRUE)
        {
            dwSize = dwMaxLen;
            if (RegEnumValueW(hKey, 0, lpszName, &dwSize,
                  NULL, NULL, NULL, NULL)) break;

            ret = RegDeleteValueW(hKey, lpszName);
            if (ret) goto cleanup;
        }

cleanup:
    /* Free buffer if allocated */
    if (lpszName != szNameBuf)
        HeapFree( GetProcessHeap(), 0, lpszName);
    if(lpszSubKey)
        RegCloseKey(hSubKey);
    return ret;
}

/******************************************************************************
 * RegDeleteTreeA [ADVAPI32.@]
 *
 */
LSTATUS WINAPI RegDeleteTreeA(HKEY hKey, LPCSTR lpszSubKey)
{
    LONG ret;
    UNICODE_STRING lpszSubKeyW;

    if (lpszSubKey) RtlCreateUnicodeStringFromAsciiz( &lpszSubKeyW, lpszSubKey);
    else lpszSubKeyW.Buffer = NULL;
    ret = RegDeleteTreeW( hKey, lpszSubKeyW.Buffer);
    RtlFreeUnicodeString( &lpszSubKeyW );
    return ret;
}
