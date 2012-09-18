/* $Id: codepage.c,v 1.3 2003-07-31 15:59:49 sandervl Exp $
 *
 * Code page functions
 *
 * Based on Wine code (memory\codepage.c)
 *
 * Copyright 2000 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winbase.h"
#include "winerror.h"
#include "winnls.h"
#include "wine/unicode.h"
#include "debugtools.h"

#ifdef __WIN32OS2__
#include <options.h>
#include "codepage.h"

#define DBG_LOCALLOG    DBG_codepage
#include "dbglocal.h"

BOOL fIsDBCSEnv = FALSE;

#endif

DEFAULT_DEBUG_CHANNEL(string);

/* current code pages */
static const union cptable *ansi_cptable;
static const union cptable *oem_cptable;
static const union cptable *mac_cptable;

/* retrieve a code page table from the locale info */
static const union cptable *get_locale_cp( LCID lcid, LCTYPE type )
{
    const union cptable *table = NULL;
    char buf[32];

    if (GetLocaleInfoA( lcid, type, buf, sizeof(buf) )) table = cp_get_table( atoi(buf) );
    return table;
}

/* setup default codepage info before we can get at the locale stuff */
static void init_codepages(void)
{
    ansi_cptable = cp_get_table( 1252 );
    oem_cptable  = cp_get_table( 437 );
    mac_cptable  = cp_get_table( 10000 );
    assert( ansi_cptable );
    assert( oem_cptable );
    assert( mac_cptable );
}

/* find the table for a given codepage, handling CP_ACP etc. pseudo-codepages */
static const union cptable *get_codepage_table( unsigned int codepage )
{
    const union cptable *ret = NULL;

    if (!ansi_cptable) init_codepages();

    switch(codepage)
    {
    case CP_ACP:        return ansi_cptable;
    case CP_OEMCP:      return oem_cptable;
    case CP_MACCP:      return mac_cptable;
    case CP_THREAD_ACP: return get_locale_cp( GetThreadLocale(), LOCALE_IDEFAULTANSICODEPAGE );
    case CP_UTF7:
    case CP_UTF8:
        break;
    default:
        if (codepage == ansi_cptable->info.codepage) return ansi_cptable;
        if (codepage == oem_cptable->info.codepage) return oem_cptable;
        if (codepage == mac_cptable->info.codepage) return mac_cptable;
        ret = cp_get_table( codepage );
        break;
    }
    return ret;
}

/* initialize default code pages from locale info */
/* FIXME: should be done in init_codepages, but it can't right now */
/* since it needs KERNEL32 to be loaded for the locale info. */
void CODEPAGE_Init(void)
{
    const union cptable *table;
    LCID lcid = GetUserDefaultLCID();

    if (!ansi_cptable) init_codepages();  /* just in case */

    if ((table = get_locale_cp( lcid, LOCALE_IDEFAULTANSICODEPAGE )) != NULL) ansi_cptable = table;
    if ((table = get_locale_cp( lcid, LOCALE_IDEFAULTMACCODEPAGE )) != NULL) mac_cptable = table;
    if ((table = get_locale_cp( lcid, LOCALE_IDEFAULTCODEPAGE )) != NULL) oem_cptable = table;

    TRACE( "ansi=%03d oem=%03d mac=%03d\n", ansi_cptable->info.codepage,
           oem_cptable->info.codepage, mac_cptable->info.codepage );
#ifdef __WIN32OS2__
    dprintf(("Language %s", getenv("LANG")));

    if(ansi_cptable->info.char_size == 2) {
        fIsDBCSEnv = TRUE;
    }
#endif
}

#ifdef __WIN32OS2__
ULONG GetDisplayCodepage()
{
    if (!ansi_cptable) CODEPAGE_Init();

    return ansi_cptable->info.codepage;
}

ULONG GetWindowsCodepage()
{
    if (!ansi_cptable) CODEPAGE_Init();

    return ansi_cptable->info.codepage;
}

#endif

/******************************************************************************
 *              GetACP   (KERNEL32)
 *
 * RETURNS
 *    Current ANSI code-page identifier, default if no current defined
 */
UINT WINAPI GetACP(void)
{
    if (!ansi_cptable) init_codepages();
#ifdef __WIN32OS2__
    dprintf(("GetACP %d", ansi_cptable->info.codepage));
#endif
    return ansi_cptable->info.codepage;
}


/***********************************************************************
 *              GetOEMCP   (KERNEL32)
 */
UINT WINAPI GetOEMCP(void)
{
    if (!oem_cptable) init_codepages();
#ifdef __WIN32OS2__
    dprintf(("GetOEMCP %d", oem_cptable->info.codepage));
#endif
    return oem_cptable->info.codepage;
}


/***********************************************************************
 *           IsValidCodePage   (KERNEL32)
 */
BOOL WINAPI IsValidCodePage( UINT codepage )
{
#ifdef __WIN32OS2__
    dprintf(("IsValidCodePage %d", codepage));
#endif
    return cp_get_table( codepage ) != NULL;
}


/***********************************************************************
 *           IsDBCSLeadByteEx   (KERNEL32)
 */
BOOL WINAPI IsDBCSLeadByteEx( UINT codepage, BYTE testchar )
{
    const union cptable *table = get_codepage_table( codepage );
#ifdef __WIN32OS2__
    dprintf2(("IsDBCSLeadByteEx %d %x", codepage, testchar));
#endif
    return table && is_dbcs_leadbyte( table, testchar );
}


/***********************************************************************
 *           IsDBCSLeadByte   (KERNEL32)
 */
BOOL WINAPI IsDBCSLeadByte( BYTE testchar )
{
    if (!ansi_cptable) init_codepages();
    return is_dbcs_leadbyte( ansi_cptable, testchar );
}


/***********************************************************************
 *           GetCPInfo   (KERNEL32.@)
 *
 * Get information about a code page.
 *
 * PARAMS
 *  codepage [I] Code page number
 *  cpinfo   [O] Destination for code page information
 *
 * RETURNS
 *  Success: TRUE. cpinfo is updated with the information about codepage.
 *  Failure: FALSE, if codepage is invalid or cpinfo is NULL.
 */
BOOL WINAPI GetCPInfo( UINT codepage, LPCPINFO cpinfo )
{
    const union cptable *table;

#ifdef __WIN32OS2__
    dprintf(("GetCPInfo %d %x", codepage, cpinfo));
#endif

    if (!cpinfo)
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if (!(table = get_codepage_table( codepage )))
    {
        switch(codepage)
        {
            case CP_UTF7:
            case CP_UTF8:
                cpinfo->DefaultChar[0] = 0x3f;
                cpinfo->DefaultChar[1] = 0;
                cpinfo->LeadByte[0] = cpinfo->LeadByte[1] = 0;
                cpinfo->MaxCharSize = (codepage == CP_UTF7) ? 5 : 4;
                return TRUE;
        }

        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    if (table->info.def_char & 0xff00)
    {
        cpinfo->DefaultChar[0] = (table->info.def_char & 0xff00) >> 8;
        cpinfo->DefaultChar[1] = table->info.def_char & 0x00ff;
    }
    else
    {
        cpinfo->DefaultChar[0] = table->info.def_char & 0xff;
        cpinfo->DefaultChar[1] = 0;
    }
    if ((cpinfo->MaxCharSize = table->info.char_size) == 2)
        memcpy( cpinfo->LeadByte, table->dbcs.lead_bytes, sizeof(cpinfo->LeadByte) );
    else
        cpinfo->LeadByte[0] = cpinfo->LeadByte[1] = 0;

    return TRUE;
}

/***********************************************************************
 *           GetCPInfoExA   (KERNEL32.@)
 *
 * Get extended information about a code page.
 *
 * PARAMS
 *  codepage [I] Code page number
 *  dwFlags  [I] Reserved, must to 0.
 *  cpinfo   [O] Destination for code page information
 *
 * RETURNS
 *  Success: TRUE. cpinfo is updated with the information about codepage.
 *  Failure: FALSE, if codepage is invalid or cpinfo is NULL.
 */
BOOL WINAPI GetCPInfoExA( UINT codepage, DWORD dwFlags, LPCPINFOEXA cpinfo )
{
    CPINFOEXW cpinfoW;

    if (!GetCPInfoExW( codepage, dwFlags, &cpinfoW ))
      return FALSE;

    /* the layout is the same except for CodePageName */
    memcpy(cpinfo, &cpinfoW, sizeof(CPINFOEXA));
    WideCharToMultiByte(CP_ACP, 0, cpinfoW.CodePageName, -1, cpinfo->CodePageName, sizeof(cpinfo->CodePageName), NULL, NULL);
    return TRUE;
}

/***********************************************************************
 *           GetCPInfoExW   (KERNEL32.@)
 *
 * Unicode version of GetCPInfoExA.
 */
BOOL WINAPI GetCPInfoExW( UINT codepage, DWORD dwFlags, LPCPINFOEXW cpinfo )
{
    if (!GetCPInfo( codepage, (LPCPINFO)cpinfo ))
      return FALSE;

    switch(codepage)
    {
        case CP_UTF7:
        {
            static const WCHAR utf7[] = {'U','n','i','c','o','d','e',' ','(','U','T','F','-','7',')',0};

            cpinfo->CodePage = CP_UTF7;
            cpinfo->UnicodeDefaultChar = 0x3f;
            strcpyW(cpinfo->CodePageName, utf7);
            break;
        }

        case CP_UTF8:
        {
            static const WCHAR utf8[] = {'U','n','i','c','o','d','e',' ','(','U','T','F','-','8',')',0};

            cpinfo->CodePage = CP_UTF8;
            cpinfo->UnicodeDefaultChar = 0x3f;
            strcpyW(cpinfo->CodePageName, utf8);
            break;
        }

        default:
        {
            const union cptable *table = get_codepage_table( codepage );

            cpinfo->CodePage = table->info.codepage;
            cpinfo->UnicodeDefaultChar = table->info.def_unicode_char;
            MultiByteToWideChar( CP_ACP, 0, table->info.name, -1, cpinfo->CodePageName,
                                 sizeof(cpinfo->CodePageName)/sizeof(WCHAR));
            break;
        }
    }
    return TRUE;
}


/***********************************************************************
 *              EnumSystemCodePagesA   (KERNEL32)
 */
BOOL WINAPI EnumSystemCodePagesA( CODEPAGE_ENUMPROCA lpfnCodePageEnum, DWORD flags )
{
    const union cptable *table;
    char buffer[10];
    int index = 0;

#ifdef __WIN32OS2__
    dprintf(("EnumSystemCodePagesA %x %x", lpfnCodePageEnum, flags));
#endif

    for (;;)
    {
        if (!(table = cp_enum_table( index++ ))) break;
        sprintf( buffer, "%d", table->info.codepage );
        if (!lpfnCodePageEnum( buffer )) break;
    }
    return TRUE;
}


/***********************************************************************
 *              EnumSystemCodePagesW   (KERNEL32)
 */
BOOL WINAPI EnumSystemCodePagesW( CODEPAGE_ENUMPROCW lpfnCodePageEnum, DWORD flags )
{
    const union cptable *table;
    WCHAR buffer[10], *p;
    int page, index = 0;

#ifdef __WIN32OS2__
    dprintf(("EnumSystemCodePagesW %x %x", lpfnCodePageEnum, flags));
#endif

    for (;;)
    {
        if (!(table = cp_enum_table( index++ ))) break;
        p = buffer + sizeof(buffer)/sizeof(WCHAR);
        *--p = 0;
        page = table->info.codepage;
        do
        {
            *--p = '0' + (page % 10);
            page /= 10;
        } while( page );
        if (!lpfnCodePageEnum( p )) break;
    }
    return TRUE;
}


/***********************************************************************
 *              MultiByteToWideChar   (KERNEL32)
 *
 * PARAMS
 *   page [in]    Codepage character set to convert from
 *   flags [in]   Character mapping flags
 *   src [in]     Source string buffer
 *   srclen [in]  Length of source string buffer
 *   dst [in]     Destination buffer
 *   dstlen [in]  Length of destination buffer
 *
 * NOTES
 *   The returned length includes the null terminator character.
 *
 * RETURNS
 *   Success: If dstlen > 0, number of characters written to destination
 *            buffer.  If dstlen == 0, number of characters needed to do
 *            conversion.
 *   Failure: 0. Occurs if not enough space is available.
 *
 * ERRORS
 *   ERROR_INSUFFICIENT_BUFFER
 *   ERROR_INVALID_PARAMETER
 *   ERROR_NO_UNICODE_TRANSLATION
 *
 */
INT WINAPI MultiByteToWideChar( UINT page, DWORD flags, LPCSTR src, INT srclen,
                                LPWSTR dst, INT dstlen )
{
    const union cptable *table;
    int ret;

#ifdef __WIN32OS2__
    dprintf2(("MultiByteToWideChar %d %x %x %d %x %d", page, flags, src, srclen, dst, dstlen));
#endif

    //Docs say source ptr can't be the same as destination (Windows ME, NT4-SP6)
    if (!src || (!dst && dstlen) || ((void *)src == (void *)dst))
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return 0;
    }

    //Even though the docs claim this only works for -1, testing shows it
    //is done for any negative value (Windows ME, NT4-SP6)
    if (srclen <= -1) srclen = strlen(src) + 1;

    if (flags & MB_USEGLYPHCHARS) FIXME("MB_USEGLYPHCHARS not supported\n");

    switch(page)
    {
    case CP_UTF7:
        FIXME("UTF not supported\n");
        SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
        return 0;
    case CP_UTF8:
        ret = utf8_mbstowcs( flags, src, srclen, dst, dstlen );
        break;
    default:
        if (!(table = get_codepage_table( page )))
        {
            SetLastError( ERROR_INVALID_PARAMETER );
            return 0;
        }
        ret = cp_mbstowcs( table, flags, src, srclen, dst, dstlen );
        break;
    }

    if (ret < 0)
    {
        switch(ret)
        {
        case -1: SetLastError( ERROR_INSUFFICIENT_BUFFER ); break;
        case -2: SetLastError( ERROR_NO_UNICODE_TRANSLATION ); break;
        }
        ret = 0;
    }
    return ret;
}


/***********************************************************************
 *              WideCharToMultiByte   (KERNEL32)
 *
 * PARAMS
 *   page [in]    Codepage character set to convert to
 *   flags [in]   Character mapping flags
 *   src [in]     Source string buffer
 *   srclen [in]  Length of source string buffer
 *   dst [in]     Destination buffer
 *   dstlen [in]  Length of destination buffer
 *   defchar [in] Default character to use for conversion if no exact
 *          conversion can be made
 *   used [out]   Set if default character was used in the conversion
 *
 * NOTES
 *   The returned length includes the null terminator character.
 *
 * RETURNS
 *   Success: If dstlen > 0, number of characters written to destination
 *            buffer.  If dstlen == 0, number of characters needed to do
 *            conversion.
 *   Failure: 0. Occurs if not enough space is available.
 *
 * ERRORS
 *   ERROR_INSUFFICIENT_BUFFER
 *   ERROR_INVALID_PARAMETER
 */
INT WINAPI WideCharToMultiByte( UINT page, DWORD flags, LPCWSTR src, INT srclen,
                                LPSTR dst, INT dstlen, LPCSTR defchar, BOOL *used )
{
    const union cptable *table;
    int ret, used_tmp;

#ifdef __WIN32OS2__
    dprintf2(("WideCharToMultiByte %d %x %x %d %x %d", page, flags, src, srclen, dst, dstlen));
#endif

    //Docs say source ptr can't be the same as destination (Windows ME, NT4-SP6)
    if (!src || (!dst && dstlen) || ((void *)src == (void *)dst))
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return 0;
    }

    //Even though the docs claim this only works for -1, testing shows it
    //is done for any negative value (Windows ME, NT4 - SP6)
    if (srclen <= -1) srclen = strlenW(src) + 1;

    switch(page)
    {
    case CP_UTF7:
        FIXME("UTF-7 not supported\n");
        SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
        return 0;
    case CP_UTF8:
        ret = utf8_wcstombs( src, srclen, dst, dstlen );
        break;
    default:
        if (!(table = get_codepage_table( page )))
        {
            SetLastError( ERROR_INVALID_PARAMETER );
            return 0;
        }
        ret = cp_wcstombs( table, flags, src, srclen, dst, dstlen,
                           defchar, used ? &used_tmp : NULL );
        if (used) *used = used_tmp;
        break;
    }

    if (ret == -1)
    {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        ret = 0;
    }
    return ret;
}


/******************************************************************************
 *              GetStringTypeW   (KERNEL32)
 *
 */
BOOL WINAPI GetStringTypeW( DWORD type, LPCWSTR src, INT count, LPWORD chartype )
{
#ifdef __WIN32OS2__
    dprintf(("GetStringTypeW %x %x %d %x", type, src, count, chartype));
#endif

    if (count == -1) count = strlenW(src) + 1;
    switch(type)
    {
    case CT_CTYPE1:
        while (count--) *chartype++ = get_char_typeW( *src++ ) & 0xfff;
        break;
    case CT_CTYPE2:
        while (count--) *chartype++ = get_char_typeW( *src++ ) >> 12;
        break;
    case CT_CTYPE3:
        FIXME("CT_CTYPE3 not supported.\n");
    default:
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    return TRUE;
}


/******************************************************************************
 *              GetStringTypeExW   (KERNEL32)
 */
BOOL WINAPI GetStringTypeExW( LCID locale, DWORD type, LPCWSTR src, INT count, LPWORD chartype )
{
    /* locale is ignored for Unicode */
    return GetStringTypeW( type, src, count, chartype );
}

/******************************************************************************
 *		GetStringTypeA	[KERNEL32.@]
 */
BOOL WINAPI GetStringTypeA(LCID locale, DWORD type, LPCSTR src, INT count, LPWORD chartype)
{
    char buf[20];
    UINT cp;
    INT countW;
    LPWSTR srcW;
    BOOL ret = FALSE;

    if(count == -1) count = strlen(src) + 1;

    if(!GetLocaleInfoA(locale, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_NOUSEROVERRIDE,
		   buf, sizeof(buf)))
    {
	FIXME("For locale %04lx using current ANSI code page\n", locale);
	cp = GetACP();
    }
    else
	cp = atoi(buf);

    countW = MultiByteToWideChar(cp, 0, src, count, NULL, 0);
    if((srcW = HeapAlloc(GetProcessHeap(), 0, countW * sizeof(WCHAR))) != NULL)
    {
	MultiByteToWideChar(cp, 0, src, count, srcW, countW);
	ret = GetStringTypeW(type, srcW, count, chartype);
	HeapFree(GetProcessHeap(), 0, srcW);
    }
    return ret;
}

/******************************************************************************
 *		GetStringTypeExA	[KERNEL32.@]
 */
BOOL WINAPI GetStringTypeExA(LCID locale, DWORD type, LPCSTR src, INT count, LPWORD chartype)
{
    return GetStringTypeA(locale, type, src, count, chartype);
}
