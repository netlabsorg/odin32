/* $Id: info.c,v 1.1 2000-01-06 20:10:07 sandervl Exp $ */
/* 
 * Implementation of VERSION.DLL - Version Info access (Wine 991212)
 * 
 * Copyright 1996,1997 Marcus Meissner
 * Copyright 1997 David Cuthbert
 * Copyright 1999 Ulrich Weigand
 */

#include <stdlib.h>
#include <string.h>

#include "winreg.h"
#include "winver.h"
#include "wine/winestring.h"
#include "winerror.h"
#include "heap.h"
#include "crtdll.h"
#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(ver)


/******************************************************************************
 *
 *   This function will print via dprintf[_]ver to stddeb debug info regarding
 *   the file info structure vffi.
 *      15-Feb-1998 Dimitrie Paun (dimi@cs.toronto.edu)
 *      Added this function to clean up the code.
 *
 *****************************************************************************/
static void print_vffi_debug(VS_FIXEDFILEINFO *vffi)
{
	TRACE(" structversion=%u.%u, fileversion=%u.%u.%u.%u, productversion=%u.%u.%u.%u, flagmask=0x%lx, flags=%s%s%s%s%s%s\n",
		    HIWORD(vffi->dwStrucVersion),LOWORD(vffi->dwStrucVersion),
		    HIWORD(vffi->dwFileVersionMS),LOWORD(vffi->dwFileVersionMS),
		    HIWORD(vffi->dwFileVersionLS),LOWORD(vffi->dwFileVersionLS),
		    HIWORD(vffi->dwProductVersionMS),LOWORD(vffi->dwProductVersionMS),
		    HIWORD(vffi->dwProductVersionLS),LOWORD(vffi->dwProductVersionLS),
		    vffi->dwFileFlagsMask,
		    (vffi->dwFileFlags & VS_FF_DEBUG) ? "DEBUG," : "",
		    (vffi->dwFileFlags & VS_FF_PRERELEASE) ? "PRERELEASE," : "",
		    (vffi->dwFileFlags & VS_FF_PATCHED) ? "PATCHED," : "",
		    (vffi->dwFileFlags & VS_FF_PRIVATEBUILD) ? "PRIVATEBUILD," : "",
		    (vffi->dwFileFlags & VS_FF_INFOINFERRED) ? "INFOINFERRED," : "",
		    (vffi->dwFileFlags & VS_FF_SPECIALBUILD) ? "SPECIALBUILD," : ""
		    );

        TRACE("(");
	DPRINTF(" OS=0x%x.0x%x ",
		HIWORD(vffi->dwFileOS),
		LOWORD(vffi->dwFileOS)
	);
	switch (vffi->dwFileOS&0xFFFF0000) {
	case VOS_DOS:DPRINTF("DOS,");break;
	case VOS_OS216:DPRINTF("OS/2-16,");break;
	case VOS_OS232:DPRINTF("OS/2-32,");break;
	case VOS_NT:DPRINTF("NT,");break;
	case VOS_UNKNOWN:
	default:
		DPRINTF("UNKNOWN(0x%lx),",vffi->dwFileOS&0xFFFF0000);break;
	}
	switch (LOWORD(vffi->dwFileOS)) {
	case VOS__BASE:DPRINTF("BASE");break;
	case VOS__WINDOWS16:DPRINTF("WIN16");break;
	case VOS__WINDOWS32:DPRINTF("WIN32");break;
	case VOS__PM16:DPRINTF("PM16");break;
	case VOS__PM32:DPRINTF("PM32");break;
	default:DPRINTF("UNKNOWN(0x%x)",LOWORD(vffi->dwFileOS));break;
	}
	DPRINTF(")\n");

	switch (vffi->dwFileType) {
	default:
	case VFT_UNKNOWN:
		TRACE("filetype=Unknown(0x%lx)",vffi->dwFileType);
		break;
	case VFT_APP:TRACE("filetype=APP,");break;
	case VFT_DLL:TRACE("filetype=DLL,");break;
	case VFT_DRV:
		TRACE("filetype=DRV,");
		switch(vffi->dwFileSubtype) {
		default:
		case VFT2_UNKNOWN:
			DPRINTF("UNKNOWN(0x%lx)",vffi->dwFileSubtype);
			break;
		case VFT2_DRV_PRINTER:
			DPRINTF("PRINTER");
			break;
		case VFT2_DRV_KEYBOARD:
			DPRINTF("KEYBOARD");
			break;
		case VFT2_DRV_LANGUAGE:
			DPRINTF("LANGUAGE");
			break;
		case VFT2_DRV_DISPLAY:
			DPRINTF("DISPLAY");
			break;
		case VFT2_DRV_MOUSE:
			DPRINTF("MOUSE");
			break;
		case VFT2_DRV_NETWORK:
			DPRINTF("NETWORK");
			break;
		case VFT2_DRV_SYSTEM:
			DPRINTF("SYSTEM");
			break;
		case VFT2_DRV_INSTALLABLE:
			DPRINTF("INSTALLABLE");
			break;
		case VFT2_DRV_SOUND:
			DPRINTF("SOUND");
			break;
		case VFT2_DRV_COMM:
			DPRINTF("COMM");
			break;
		case VFT2_DRV_INPUTMETHOD:
			DPRINTF("INPUTMETHOD");
			break;
		}
		break;
	case VFT_FONT:
                TRACE("filetype=FONT.");
		switch (vffi->dwFileSubtype) {
		default:
			DPRINTF("UNKNOWN(0x%lx)",vffi->dwFileSubtype);
			break;
		case VFT2_FONT_RASTER:DPRINTF("RASTER");break;
		case VFT2_FONT_VECTOR:DPRINTF("VECTOR");break;
		case VFT2_FONT_TRUETYPE:DPRINTF("TRUETYPE");break;
		}
		break;
	case VFT_VXD:TRACE("filetype=VXD");break;
	case VFT_STATIC_LIB:TRACE("filetype=STATIC_LIB");break;
	}
        DPRINTF("\n");
	TRACE("  filedata=0x%lx.0x%lx\n",
		    vffi->dwFileDateMS,vffi->dwFileDateLS);
}


/***********************************************************************
 * Version Info Structure
 */

typedef struct
{
    WORD  wLength;
    WORD  wValueLength;
    CHAR  szKey[1];
#if 0   /* variable length structure */
    /* DWORD aligned */
    BYTE  Value[];
    /* DWORD aligned */
    VS_VERSION_INFO_STRUCT16 Children[];
#endif
} VS_VERSION_INFO_STRUCT16;

typedef struct
{
    WORD  wLength;
    WORD  wValueLength;
    WORD  bText;
    WCHAR szKey[1];
#if 0   /* variable length structure */
    /* DWORD aligned */
    BYTE  Value[];
    /* DWORD aligned */
    VS_VERSION_INFO_STRUCT32 Children[];
#endif
} VS_VERSION_INFO_STRUCT32;

#define VersionInfoIs16( ver ) \
    ( ((VS_VERSION_INFO_STRUCT16 *)ver)->szKey[0] >= ' ' )

#define DWORD_ALIGN( ptr ) ((LPBYTE)( (((DWORD)(ptr)) + 3) & ~3 ))

#define VersionInfo16_Value( ver )  \
    DWORD_ALIGN( (ver)->szKey + lstrlenA((ver)->szKey) + 1 )
#define VersionInfo32_Value( ver )  \
    DWORD_ALIGN( (ver)->szKey + lstrlenW((ver)->szKey) + 1 )

#define VersionInfo16_Children( ver )  \
    (VS_VERSION_INFO_STRUCT16 *)( VersionInfo16_Value( ver ) + \
                           ( ( (ver)->wValueLength + 3 ) & ~3 ) )
#define VersionInfo32_Children( ver )  \
    (VS_VERSION_INFO_STRUCT32 *)( VersionInfo32_Value( ver ) + \
                           ( ( (ver)->wValueLength * \
                               ((ver)->bText? 2 : 1) + 3 ) & ~3 ) )

#define VersionInfo16_Next( ver ) \
    (VS_VERSION_INFO_STRUCT16 *)( (LPBYTE)ver + (((ver)->wLength + 3) & ~3) )
#define VersionInfo32_Next( ver ) \
    (VS_VERSION_INFO_STRUCT32 *)( (LPBYTE)ver + (((ver)->wLength + 3) & ~3) )

#ifndef __WIN32OS2__
/***********************************************************************
 *           ConvertVersionInfo32To16        [internal]
 */
void ConvertVersionInfo32To16( VS_VERSION_INFO_STRUCT32 *info32, 
                               VS_VERSION_INFO_STRUCT16 *info16 )
{
    /* Copy data onto local stack to prevent overwrites */
    WORD wLength = info32->wLength;
    WORD wValueLength = info32->wValueLength;
    WORD bText = info32->bText;
    LPBYTE lpValue = VersionInfo32_Value( info32 );
    VS_VERSION_INFO_STRUCT32 *child32 = VersionInfo32_Children( info32 );
    VS_VERSION_INFO_STRUCT16 *child16;

    TRACE("Converting %p to %p\n", info32, info16 );
    TRACE("wLength %d, wValueLength %d, bText %d, value %p, child %p\n",
                wLength, wValueLength, bText, lpValue, child32 );

    /* Convert key */
    lstrcpyWtoA( info16->szKey, info32->szKey );

    TRACE("Copied key from %p to %p: %s\n", info32->szKey, info16->szKey,
                debugstr_a(info16->szKey) );

    /* Convert value */
    if ( wValueLength == 0 )
    {
        info16->wValueLength = 0;
        TRACE("No value present\n" );
    }
    else if ( bText )
    {
        info16->wValueLength = lstrlenW( (LPCWSTR)lpValue ) + 1;
        lstrcpyWtoA( VersionInfo16_Value( info16 ), (LPCWSTR)lpValue );

        TRACE("Copied value from %p to %p: %s\n", lpValue, 
                    VersionInfo16_Value( info16 ), 
                    debugstr_a(VersionInfo16_Value( info16 )) );
    }
    else
    {
        info16->wValueLength = wValueLength;
        memmove( VersionInfo16_Value( info16 ), lpValue, wValueLength );

        TRACE("Copied value from %p to %p: %d bytes\n", lpValue, 
                     VersionInfo16_Value( info16 ), wValueLength );
    }

    /* Convert children */
    child16 = VersionInfo16_Children( info16 );
    while ( (DWORD)child32 < (DWORD)info32 + wLength )
    {
        VS_VERSION_INFO_STRUCT32 *nextChild = VersionInfo32_Next( child32 );

        ConvertVersionInfo32To16( child32, child16 );

        child16 = VersionInfo16_Next( child16 );
        child32 = nextChild;
    }

    /* Fixup length */
    info16->wLength = (DWORD)child16 - (DWORD)info16;

    TRACE("Finished, length is %d (%p - %p)\n", 
                info16->wLength, info16, child16 );
}


/***********************************************************************
 *           GetFileVersionInfoSize32A         [VERSION.2]
 */
DWORD WINAPI GetFileVersionInfoSizeA( LPCSTR filename, LPDWORD handle )
{
    VS_FIXEDFILEINFO *vffi;
    DWORD len, ret, offset;
    BYTE buf[144];

    TRACE("(%s,%p)\n", debugstr_a(filename), handle );

    len = GetFileResourceSize( filename,
                                 MAKEINTRESOURCEA(VS_FILE_INFO),
                                 MAKEINTRESOURCEA(VS_VERSION_INFO),
                                 &offset );
    if (!len) return 0;

    ret = GetFileResource( filename,
                             MAKEINTRESOURCEA(VS_FILE_INFO),
                             MAKEINTRESOURCEA(VS_VERSION_INFO),
                             offset, sizeof( buf ), buf );
    if (!ret) return 0;

    if ( handle ) *handle = offset;
    
    if ( VersionInfoIs16( buf ) )
        vffi = (VS_FIXEDFILEINFO *)VersionInfo16_Value( (VS_VERSION_INFO_STRUCT16 *)buf );
    else
        vffi = (VS_FIXEDFILEINFO *)VersionInfo32_Value( (VS_VERSION_INFO_STRUCT32 *)buf );

    if ( vffi->dwSignature != VS_FFI_SIGNATURE )
    {
        WARN("vffi->dwSignature is 0x%08lx, but not 0x%08lx!\n",
                   vffi->dwSignature, VS_FFI_SIGNATURE );
        return 0;
    }

    if ( ((VS_VERSION_INFO_STRUCT16 *)buf)->wLength < len )
        len = ((VS_VERSION_INFO_STRUCT16 *)buf)->wLength;

    if ( TRACE_ON(ver) )
        print_vffi_debug( vffi );

    return len;
}

/***********************************************************************
 *           GetFileVersionInfoSize32W         [VERSION.3]
 */
DWORD WINAPI GetFileVersionInfoSizeW( LPCWSTR filename, LPDWORD handle )
{
    LPSTR fn = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    DWORD ret = GetFileVersionInfoSizeA( fn, handle );
    HeapFree( GetProcessHeap(), 0, fn );
    return ret;
}

/***********************************************************************
 *           GetFileVersionInfo32A             [VERSION.1]
 */
DWORD WINAPI GetFileVersionInfoA( LPCSTR filename, DWORD handle,
                                    DWORD datasize, LPVOID data )
{
    TRACE("(%s,%ld,size=%ld,data=%p)\n",
                debugstr_a(filename), handle, datasize, data );

    if ( !GetFileResource( filename, MAKEINTRESOURCEA(VS_FILE_INFO),
                                       MAKEINTRESOURCEA(VS_VERSION_INFO),
                                       handle, datasize, data ) )
        return FALSE;

    if (    datasize >= sizeof(VS_VERSION_INFO_STRUCT16)
         && datasize >= ((VS_VERSION_INFO_STRUCT16 *)data)->wLength  
         && !VersionInfoIs16( data ) )
    {
        /* convert resource from PE format to NE format */
        ConvertVersionInfo32To16( (VS_VERSION_INFO_STRUCT32 *)data, 
                                  (VS_VERSION_INFO_STRUCT16 *)data );
    }

    return TRUE;
}

/***********************************************************************
 *           GetFileVersionInfo32W             [VERSION.4]
 */
DWORD WINAPI GetFileVersionInfoW( LPCWSTR filename, DWORD handle,
                                    DWORD datasize, LPVOID data )
{
    LPSTR fn = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    DWORD retv = TRUE;

    TRACE("(%s,%ld,size=%ld,data=%p)\n",
                debugstr_a(fn), handle, datasize, data );

    if ( !GetFileResource( fn, MAKEINTRESOURCEA(VS_FILE_INFO),
                                 MAKEINTRESOURCEA(VS_VERSION_INFO),
                                 handle, datasize, data ) )
        retv = FALSE;

    else if (    datasize >= sizeof(VS_VERSION_INFO_STRUCT16)
              && datasize >= ((VS_VERSION_INFO_STRUCT16 *)data)->wLength 
              && VersionInfoIs16( data ) )
    {
        ERR("Cannot access NE resource in %s\n", debugstr_a(fn) );
        retv =  FALSE;
    }

    HeapFree( GetProcessHeap(), 0, fn );
    return retv;
}
#endif //!__WIN32OS2__

/***********************************************************************
 *           VersionInfo16_FindChild             [internal]
 */
VS_VERSION_INFO_STRUCT16 *VersionInfo16_FindChild( VS_VERSION_INFO_STRUCT16 *info, 
                                            LPCSTR szKey, UINT cbKey )
{
    VS_VERSION_INFO_STRUCT16 *child = VersionInfo16_Children( info );

    while ( (DWORD)child < (DWORD)info + info->wLength )
    {
        if ( !lstrncmpiA( child->szKey, szKey, cbKey ) )
            return child;

	if (!(child->wLength)) return NULL;
        child = VersionInfo16_Next( child );
    }

    return NULL;
}

/***********************************************************************
 *           VersionInfo32_FindChild             [internal]
 */
VS_VERSION_INFO_STRUCT32 *VersionInfo32_FindChild( VS_VERSION_INFO_STRUCT32 *info, 
                                            LPCWSTR szKey, UINT cbKey )
{
    VS_VERSION_INFO_STRUCT32 *child = VersionInfo32_Children( info );

    while ( (DWORD)child < (DWORD)info + info->wLength )
    {
        if ( !CRTDLL__wcsnicmp( child->szKey, szKey, cbKey ) )
            return child;

        child = VersionInfo32_Next( child );
    }

    return NULL;
}

/***********************************************************************
 *           VerQueryValue32W              [VERSION.13]
 */
DWORD VerQueryValue32W( LPVOID pBlock, LPCWSTR lpSubBlock,
                        LPVOID *lplpBuffer, UINT *puLen, BOOL *bText)
{
    VS_VERSION_INFO_STRUCT32 *info = (VS_VERSION_INFO_STRUCT32 *)pBlock;
    if ( VersionInfoIs16( info ) )
    {
        ERR("called on NE resource!\n" );
        return FALSE;
    }

#if defined(__WIN32OS2__)
    dprintf(("VerQueryValueW %x %x %x %x", pBlock, lpSubBlock, lplpBuffer, puLen));
#else
    TRACE("(%p,%s,%p,%p)\n",
                pBlock, debugstr_w(lpSubBlock), lplpBuffer, puLen );
#endif

    while ( *lpSubBlock )
    {
        /* Find next path component */
        LPCWSTR lpNextSlash;
        for ( lpNextSlash = lpSubBlock; *lpNextSlash; lpNextSlash++ )
            if ( *lpNextSlash == '\\' )
                break;

        /* Skip empty components */
        if ( lpNextSlash == lpSubBlock )
        {
            lpSubBlock++;
            continue;
        }

        /* We have a non-empty component: search info for key */
        info = VersionInfo32_FindChild( info, lpSubBlock, lpNextSlash-lpSubBlock );
        if ( !info ) return FALSE;

        /* Skip path component */
        lpSubBlock = lpNextSlash;
    }

    /* Return value */
    *lplpBuffer = VersionInfo32_Value( info );
    *puLen = info->wValueLength;
    *bText = info->bText;

#if defined(__WIN32OS2__)
    dprintf(("VerQueryValueW successful"));
#endif
    return TRUE;
}


#if defined(__WIN32OS2__)
DWORD VerQueryValueW( LPVOID pBlock, LPCWSTR lpSubBlock,
                      LPVOID *lplpBuffer, UINT *puLen)
{
 BOOL bText;

    return VerQueryValue32W(pBlock, lpSubBlock, lplpBuffer, puLen, &bText);
}
#endif

/***********************************************************************
 *           VerQueryValue32A              [VERSION.12]
 */
DWORD WINAPI VerQueryValueA( LPVOID pBlock, LPCSTR lpSubBlock,
                               LPVOID *lplpBuffer, UINT *puLen )
{
    VS_VERSION_INFO_STRUCT16 *info = (VS_VERSION_INFO_STRUCT16 *)pBlock;
    if ( !VersionInfoIs16( info ) )
    {
#ifdef __WIN32OS2__
        // this is a quick hack, not much tested
        WCHAR *ustring = (WCHAR *)malloc(strlen((char *)lpSubBlock)*2+2);
        LPVOID ubuffer;
        char *abuffer;
        UINT len = *puLen * 2;
        BOOL rc, bText;

        dprintf(("VERSION: called on PE unicode resource!\n" ));

        AsciiToUnicode((char *)lpSubBlock, ustring);
        rc = VerQueryValue32W( pBlock, (LPWSTR)ustring, &ubuffer, &len, &bText);
	if(rc == 0) {
	        free(ustring);
		return 0;
	}
        if(!bText) {
          *lplpBuffer = ubuffer;
        }
        else
        {
          *lplpBuffer = malloc(len+1); // FIXME: no free, memory leak!!
          UnicodeToAsciiN((WCHAR *)ubuffer, (char *)*lplpBuffer, len);
        }
        *puLen = len;
        free(ustring);
        return rc;
#else
        ERR("called on PE resource!\n" );
        return FALSE;
#endif //__WIN32OS2__
    }

    TRACE("(%p,%s,%p,%p)\n",
                pBlock, debugstr_a(lpSubBlock), lplpBuffer, puLen );

    while ( *lpSubBlock )
    {
        /* Find next path component */
        LPCSTR lpNextSlash;
        for ( lpNextSlash = lpSubBlock; *lpNextSlash; lpNextSlash++ )
            if ( *lpNextSlash == '\\' )
                break;

        /* Skip empty components */
        if ( lpNextSlash == lpSubBlock )
        {
            lpSubBlock++;
            continue;
        }

        /* We have a non-empty component: search info for key */
        info = VersionInfo16_FindChild( info, lpSubBlock, lpNextSlash-lpSubBlock );
        if ( !info ) return FALSE;

        /* Skip path component */
        lpSubBlock = lpNextSlash;
    }

    /* Return value */
    *lplpBuffer = VersionInfo16_Value( info );
    *puLen = info->wValueLength;

    return TRUE;
}
