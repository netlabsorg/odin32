/* $Id: char.cpp,v 1.1 2002-02-11 15:22:39 achimha Exp $ */

#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <winnls.h>

#include "unicode.h"
#include <heapstring.h>
#include "handlemanager.h"

/***********************************************************************
 *           CharToOemA   (USER32.@)
 */
BOOL WINAPI CharToOemA( LPCSTR s, LPSTR d )
{
    dprintf2(("CharToOemA %x %x", s, d));
    if ( !s || !d ) return TRUE;
    return CharToOemBuffA( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           CharToOemBuffA   (USER32.@)
 */
BOOL WINAPI CharToOemBuffA( LPCSTR s, LPSTR d, DWORD len )
{
    WCHAR *bufW;

    dprintf2(("CharToOemBuffA %x %x %d", s, d, len));
    bufW = (WCHAR *)HeapAlloc( GetProcessHeap(), 0, len * sizeof(WCHAR) );
    if( bufW )
    {
	MultiByteToWideChar( CP_ACP, 0, s, len, bufW, len );
	WideCharToMultiByte( CP_OEMCP, 0, bufW, len, d, len, NULL, NULL );
	HeapFree( GetProcessHeap(), 0, bufW );
    }
    return TRUE;
}



/***********************************************************************
 *           OemToCharA   (USER32.@)
 */
BOOL WINAPI OemToCharA( LPCSTR s, LPSTR d )
{
    return OemToCharBuffA( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           OemToCharBuffA   (USER32.@)
 */
BOOL WINAPI OemToCharBuffA( LPCSTR s, LPSTR d, DWORD len )
{
    WCHAR *bufW;

    dprintf2(("OemToCharBuffA %x %x %d", s, d, len));

    bufW = (WCHAR *)HeapAlloc( GetProcessHeap(), 0, len * sizeof(WCHAR) );
    if( bufW )
    {
	MultiByteToWideChar( CP_OEMCP, 0, s, len, bufW, len );
	WideCharToMultiByte( CP_ACP, 0, bufW, len, d, len, NULL, NULL );
	HeapFree( GetProcessHeap(), 0, bufW );
    }
    return TRUE;
}

