/* $Id: crtdll.cpp,v 1.1 1999-09-07 20:41:44 sandervl Exp $ */

/*
 * The C RunTime DLL
 * 
 * Implements C run-time functionality as known from UNIX.
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997 Uwe Bonnes
 * Copyright 1999 Jens Wiessner
 */


#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>
#include <unicode.h>
#include <heapstring.h>
#include <ctype.h>
#include <setjmp.h>
#include <except.h>
#include <debugtools.h>

#include <wchar.h>
#include <wctype.h>

#include <crtdll.h>
#include "crtinc.h"


// DEFAULT_DEBUG_CHANNEL(crtdll)

INT CDECL CRTDLL_vfprintf( CRTDLL_FILE *file, LPSTR format, va_list args );


/*********************************************************************
 *                  CRTDLL_MainInit  (CRTDLL.init)
 */
BOOL WINAPI CRTDLL_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		CRTDLL__fdopen(0,"r");
		CRTDLL__fdopen(1,"w");
		CRTDLL__fdopen(2,"w");
	}
	return TRUE;
}

/*********************************************************************
 *                  _XcptFilter    (CRTDLL.21)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__XcptFilter(DWORD ret)
{
        dprintf(("CRTDLL: XcptFilter\n"));

	return 0;
}

/*********************************************************************
 *                  _GetMainArgs  (CRTDLL.22)
 */
DWORD CDECL CRTDLL__GetMainArgs(LPDWORD argc,LPSTR **argv,
                                LPSTR *environ,DWORD flag)
{
        char *cmdline;
        char  **xargv;
	int	xargc,i,afterlastspace;
	DWORD	version;

	dprintf(("CRTDLL: GetMainArgs\n"));

	CRTDLL_acmdln_dll = cmdline = HEAP_strdupA( GetProcessHeap(), 0,
                                                    GetCommandLineA() );

	version	= GetVersion();
	CRTDLL_osver_dll       = version >> 16;
	CRTDLL_winminor_dll    = version & 0xFF;
	CRTDLL_winmajor_dll    = (version>>8) & 0xFF;
	CRTDLL_baseversion_dll = version >> 16;
	CRTDLL_winver_dll      = ((version >> 8) & 0xFF) + ((version & 0xFF) << 8);
	CRTDLL_baseminor_dll   = (version >> 16) & 0xFF;
	CRTDLL_basemajor_dll   = (version >> 24) & 0xFF;
	CRTDLL_osversion_dll   = version & 0xFFFF;
	CRTDLL_osminor_dll     = version & 0xFF;
	CRTDLL_osmajor_dll     = (version>>8) & 0xFF;

	/* missing threading init */
/*
	i=0;xargv=NULL;xargc=0;afterlastspace=0;
	dprintf(("CRTDLL: GetMainArgs i loop\n"));
	while (cmdline[i]) {
		if (cmdline[i]==' ') {
	dprintf(("CRTDLL: GetMainArgs *1\n"));
			xargv=(char**)HeapReAlloc( GetProcessHeap(), 0, xargv,
                                                   sizeof(char*)*(++xargc));
			cmdline[i]='\0';
			xargv[xargc-1] = HEAP_strdupA( GetProcessHeap(), 0,
                                                       cmdline+afterlastspace);
			i++;
	dprintf(("CRTDLL: GetMainArgs *2\n"));
			while (cmdline[i]==' ')
				i++;
			if (cmdline[i])
				afterlastspace=i;
	dprintf(("CRTDLL: GetMainArgs *3\n"));
		} else
			i++;

	}
	xargv=(char**)HeapReAlloc( GetProcessHeap(), 0, xargv,
                                   sizeof(char*)*(++xargc));
	dprintf(("CRTDLL: GetMainArgs *4\n"));
	cmdline[i]='\0';
	dprintf(("CRTDLL: GetMainArgs *5\n"));
	xargv[xargc-1] = HEAP_strdupA( GetProcessHeap(), 0,
                                       cmdline+afterlastspace);
	dprintf(("CRTDLL: GetMainArgs *6\n"));
	dprintf(("CRTDLL: GetMainArgs *7\n"));
	CRTDLL_argc_dll	= xargc;
	dprintf(("CRTDLL: GetMainArgs *8\n"));
	*argc		= xargc;
	dprintf(("CRTDLL: GetMainArgs *9\n"));
	CRTDLL_argv_dll	= xargv;
	dprintf(("CRTDLL: GetMainArgs *11\n"));
	*argv		= xargv;
*/
	dprintf(("CRTDLL: GetMainArgs end\n"));
	CRTDLL_environ_dll = *environ = GetEnvironmentStringsA();
	return 0;
}

/*********************************************************************
 *                  __mb_cur_max_dll    (CRTDLL.31)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL___mb_cur_max_dll(DWORD ret)
{
        dprintf(("CRTDLL: __mb_cur_max_dll\n"));

	return 0;
}

/*********************************************************************
 *                  _chdir           (CRTDLL.51)
 */
INT CDECL CRTDLL__chdir(LPCSTR newdir)
{
	dprintf(("CRTDLL: chdir\n"));
	if (!SetCurrentDirectoryA(newdir))
		return 1;
	return 0;
}

/*********************************************************************
 *                  _exit          (CRTDLL.87)
 */
VOID CDECL CRTDLL__exit(DWORD ret)
{
	dprintf(("CRTDLL: exit\n"));
	ExitProcess(ret);
}

/*********************************************************************
 *                  _fdopen     (CRTDLL.91)
 */
CRTDLL_FILE * CDECL CRTDLL__fdopen(INT handle, LPCSTR mode)
{
    dprintf(("CRTDLL: fdopen\n"));
    CRTDLL_FILE *file;

    switch (handle) 
    {
    case 0:
        file = CRTDLL_stdin;
        if (!file->handle) file->handle = GetStdHandle( STD_INPUT_HANDLE );
        break;
    case 1:
        file = CRTDLL_stdout;
        if (!file->handle) file->handle = GetStdHandle( STD_OUTPUT_HANDLE );
        break;
    case 2:
        file=CRTDLL_stderr;
        if (!file->handle) file->handle = GetStdHandle( STD_ERROR_HANDLE );
        break;
    default:
        file = (PCRTDLL_FILE)HeapAlloc( GetProcessHeap(), 0, sizeof(*file) );
        file->handle = handle;
        break;
    }
  return file;
}

 /*********************************************************************
 *                  _findfirst    (CRTDLL.099)
 * 
 * BUGS
 *   Unimplemented
 */
DWORD CDECL CRTDLL__findfirst(LPCSTR fname,  struct find_t * x2)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  _findnext     (CRTDLL.100)
 * 
 * BUGS
 *   Unimplemented
 */
INT CDECL CRTDLL__findnext(DWORD hand, struct find_t * x2)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  _fstat        (CRTDLL.111)
 * 
 * BUGS
 *   Unimplemented
 */
int CDECL CRTDLL__fstat(int file, struct stat* buf)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*******************************************************************
 *         _global_unwind2  (CRTDLL.129)
 */
void CDECL CRTDLL__global_unwind2( PEXCEPTION_FRAME frame )
{
    dprintf(("CRTDLL: global_undwind2\n"));
    RtlUnwind( frame, 0, NULL, 0 );
}

/*********************************************************************
 *                  _initterm     (CRTDLL.135)
 */
DWORD CDECL CRTDLL__initterm(_INITTERMFUN *start,_INITTERMFUN *end)
{
	dprintf(("CRTDLL: initterm\n"));
	_INITTERMFUN	*current;

	current=start;
	while (current<end) {
		if (*current) (*current)();
		current++;
	}
	return 0;
}

/*********************************************************************
 *                  _isctype           (CRTDLL.138)
 */
BOOL CDECL CRTDLL__isctype(CHAR x,CHAR type)
{
	dprintf(("CRTDLL: isctype\n"));
	if ((type & CRTDLL_SPACE) && isspace(x))
		return TRUE;
	if ((type & CRTDLL_PUNCT) && ispunct(x))
		return TRUE;
	if ((type & CRTDLL_LOWER) && islower(x))
		return TRUE;
	if ((type & CRTDLL_UPPER) && isupper(x))
		return TRUE;
	if ((type & CRTDLL_ALPHA) && isalpha(x))
		return TRUE;
	if ((type & CRTDLL_DIGIT) && isdigit(x))
		return TRUE;
	if ((type & CRTDLL_CONTROL) && iscntrl(x))
		return TRUE;
	/* check CRTDLL_LEADBYTE */
	return FALSE;
}

/*******************************************************************
 *         _local_unwind2  (CRTDLL.172)
 */
void CDECL CRTDLL__local_unwind2( PEXCEPTION_FRAME endframe, DWORD nr )
{
	dprintf(("CRTDLL: local_undwind2\n"));
}

/*********************************************************************
 *           CRTDLL__mbsinc    (CRTDLL.203)
 */
LPSTR CDECL CRTDLL__mbsinc( LPCSTR str )
{
    int len = mblen( str, MB_LEN_MAX );
    if (len < 1) len = 1;
    return (LPSTR)(str + len);
}


/*********************************************************************
 *           CRTDLL__mbslen    (CRTDLL.204)
 */
INT CDECL CRTDLL__mbslen( LPCSTR str )
{
    INT len, total = 0;
    while ((len = mblen( str, MB_LEN_MAX )) > 0)
    {
        str += len;
        total++;
    }
    return total;
}

/*********************************************************************
 *                  _mkdir           (CRTDLL.232)
 */
INT CDECL CRTDLL__mkdir(LPCSTR newdir)
{
	dprintf(("CRTDLL: mkdir\n"));
	if (!CreateDirectoryA(newdir,NULL))
		return -1;
	return 0;
}

/*********************************************************************
 *                  _memicmp           (CRTDLL.233)
 * A stringcompare, without \0 check
 * RETURNS
 *	-1:if first string is alphabetically before second string
 *	1:if second ''    ''      ''          ''   first   ''
 *      0:if both are equal.
 */
INT CDECL CRTDLL__memicmp(
	LPCSTR s1,	/* [in] first string */
	LPCSTR s2,	/* [in] second string */
	DWORD len	/* [in] length to compare */
) { 
	dprintf(("CRTDLL: memicmp\n"));
	int	i;

	for (i=0;i<len;i++) {
		if (tolower(s1[i])<tolower(s2[i]))
			return -1;
		if (tolower(s1[i])>tolower(s2[i]))
			return  1;
	}
	return 0;
}

/*********************************************************************
 *                  _pctype_dll    (CRTDLL.245)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__pctype_dll(DWORD ret)
{
        dprintf(("CRTDLL: _pctype_dll\n"));

	return 0;
}

/*********************************************************************
 *                  _read     (CRTDLL.254)
 * 
 * BUGS
 *   Unimplemented
 */
INT CDECL CRTDLL__read(INT fd, LPVOID buf, UINT count)
{
  dprintf(("CRTDLL: _read\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  _strnicmp   (CRTDLL.291)
 */
INT CDECL CRTDLL__strnicmp( LPCSTR s1, LPCSTR s2, INT n )
{
    dprintf(("CRTDLL: strnicmp\n"));
    return lstrncmpiA( s1, s2, n );
}

/*********************************************************************
 *           CRTDLL__wcsnset    (CRTDLL.322)
 */
LPWSTR CDECL CRTDLL__wcsnset( LPWSTR str, WCHAR c, INT n )
{
    LPWSTR ret = str;
    while ((n-- > 0) && *str) *str++ = c;
    return ret;
}

/*********************************************************************
 *           CRTDLL__wcsset    (CRTDLL.324)
 */
LPWSTR CDECL CRTDLL__wcsset( LPWSTR str, WCHAR c )
{
    LPWSTR ret = str;
    while (*str) *str++ = c;
    return ret;
}

/*********************************************************************
 *                  exit          (CRTDLL.356)
 */
void CDECL CRTDLL_exit(DWORD ret)
{
	dprintf(("CRTDLL: exit\n"));
	ExitProcess(ret);
}

/*********************************************************************
 *                  fprintf       (CRTDLL.370)
 */
INT CDECL CRTDLL_fprintf( CRTDLL_FILE *file, LPSTR format, ... )
{
    dprintf(("CRTDLL: fprintf\n"));
    va_list valist;
    INT res;

    va_start( valist, format );
    res = CRTDLL_vfprintf( file, format, valist );
    va_end( valist );
    return res;
}

/*********************************************************************
 *                  free          (CRTDLL.375)
 */
VOID CDECL CRTDLL_free(LPVOID ptr)
{
    dprintf(("CRTDLL: free\n"));
    HeapFree(GetProcessHeap(),0,ptr);
}

/*********************************************************************
 *                  fwrite     (CRTDLL.383)
 */
DWORD CDECL CRTDLL_fwrite( LPVOID ptr, INT size, INT nmemb, CRTDLL_FILE *file )
{
    DWORD ret;

    dprintf(("CRTDLL: fwrite\n"));
    WriteFile( file->handle, ptr, size * nmemb, &ret, NULL );
    return ret / size;
}

/*********************************************************************
 *                  isalnum    (CRTDLL.391)
 */
int CDECL CRTDLL_isalnum(int i)
{
  dprintf(("CRTDLL: isalnum(%08xh)\n", i));

  return (isalnum(i));
}

/*********************************************************************
 *                  iscntrl    (CRTDLL.393)
 */
int CDECL CRTDLL_iscntrl(int i)
{
  dprintf(("CRTDLL: iscntrl(%08xh)\n", i));

  return (iscntrl(i));
}

/*********************************************************************
 *                  isgraph    (CRTDLL.395)
 */
int CDECL CRTDLL_isgraph(int i)
{
  dprintf(("CRTDLL: isgraph(%08xh)\n", i));

  return (isgraph(i));
}

/*********************************************************************
 *                  isleadbyte	(CRTDLL.396)
 */
int CDECL CRTDLL_isleadbyte(int i)
{
  dprintf(("CRTDLL: isleadbyte(%08xh)\n", i));

  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  ispunct    (CRTDLL.399)
 */
int CDECL CRTDLL_ispunct(int i)
{
  dprintf(("CRTDLL: ispunct(%08xh)\n", i));

  return (ispunct(i));
}

/*********************************************************************
 *                  iswalnum    (CRTDLL.402)
 */
int CDECL CRTDLL_iswalnum(wint_t i)
{
  dprintf(("CRTDLL: iswalnum(%08xh)\n", i));

  return (iswalnum(i));
}

/*********************************************************************
 *                  iswalpha    (CRTDLL.403)
 */
int CDECL CRTDLL_iswalpha(wint_t i)
{
  dprintf(("CRTDLL: iswalpha(%08xh)\n", i));

  return (iswalpha(i));
}

/*********************************************************************
 *                  iswascii    (CRTDLL.404)
 */
int CDECL CRTDLL_iswascii(wint_t i)
{
  dprintf(("CRTDLL: iswascii(%08xh)\n", i));

  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  iswcntrl    (CRTDLL.405)
 */
int CDECL CRTDLL_iswcntrl(wint_t i)
{
  dprintf(("CRTDLL: iswcntrl(%08xh)\n", i));

  return (iswcntrl(i));
}

/*********************************************************************
 *                  iswctype    (CRTDLL.406)
 */
int CDECL CRTDLL_iswctype(wint_t i, wctype_t wct)
{
  dprintf(("CRTDLL: iswctype\n"));

  return (iswctype(i, wct));
}

/*********************************************************************
 *                  iswdigit    (CRTDLL.407)
 */
int CDECL CRTDLL_iswdigit(wint_t i)
{
  dprintf(("CRTDLL: iswdigit(%08xh)\n", i));

  return (iswdigit(i));
}

/*********************************************************************
 *                  iswgraph    (CRTDLL.408)
 */
int CDECL CRTDLL_iswgraph(wint_t i)
{
  dprintf(("CRTDLL: iswgraph(%08xh)\n", i));

  return (iswgraph(i));
}

/*********************************************************************
 *                  iswlower    (CRTDLL.409)
 */
int CDECL CRTDLL_iswlower(wint_t i)
{
  dprintf(("CRTDLL: iswlower(%08xh)\n", i));

  return (iswlower(i));
}

/*********************************************************************
 *                  iswprint    (CRTDLL.410)
 */
int CDECL CRTDLL_iswprint(wint_t i)
{
  dprintf(("CRTDLL: iswprint(%08xh)\n", i));

  return (iswprint(i));
}

/*********************************************************************
 *                  iswpunct    (CRTDLL.411)
 */
int CDECL CRTDLL_iswpunct(wint_t i)
{
  dprintf(("CRTDLL: iswpunct(%08xh)\n", i));

  return (iswpunct(i));
}

/*********************************************************************
 *                  iswspace    (CRTDLL.412)
 */
int CDECL CRTDLL_iswspace(wint_t i)
{
  dprintf(("CRTDLL: iswspace(%08xh)\n", i));

  return (iswspace(i));
}

/*********************************************************************
 *                  iswupper    (CRTDLL.413)
 */
int CDECL CRTDLL_iswupper(wint_t i)
{
  dprintf(("CRTDLL: iswupper(%08xh)\n", i));

  return (iswupper(i));
}

/*********************************************************************
 *                  iswxdigit    (CRTDLL.414)
 */
int CDECL CRTDLL_iswxdigit(wint_t i)
{
  dprintf(("CRTDLL: iswxdigit(%08xh)\n", i));

  return (iswxdigit(i));
}

/*********************************************************************
 *                  isxdigit    (CRTDLL.415)
 */
int CDECL CRTDLL_isxdigit(int i)
{
  dprintf(("CRTDLL: isxdigit(%08xh)\n", i));

  return (isxdigit(i));
}

/*********************************************************************
 *                  longjmp        (CRTDLL.423)
 */
VOID CDECL CRTDLL_longjmp(jmp_buf env, int val)
{
    dprintf(("CRTDLL: longjmp\n"));
    longjmp(env, val);
}

/*********************************************************************
 *                  malloc        (CRTDLL.424)
 */
VOID* CDECL CRTDLL_malloc(DWORD size)
{
      dprintf(("CRTDLL: malloc\n"));
      return HeapAlloc(GetProcessHeap(),0,size);
}

/*********************************************************************
 *           CRTDLL_mbtowc    (CRTDLL.427)
 */
INT CDECL CRTDLL_mbtowc( WCHAR *dst, LPCSTR str, INT n )
{
    wchar_t res;
    int ret = mbtowc( &res, str, n );
    if (dst) *dst = (WCHAR)res;
    return ret;
}

/*********************************************************************
 *                  memcmp   (CRTDLL.429)
 */
int CDECL CRTDLL_memcmp( const void * c1, const void * c2, size_t n )
{
    dprintf(("CRTDLL: memcmp\n"));
    return memcmp( c1, c2, n );
}

/*********************************************************************
 *                  memcpy   (CRTDLL.430)
 */
void * CDECL CRTDLL_memcpy( void *s1, const void *s2, size_t n )
{
    dprintf(("CRTDLL: memcpy\n"));
    return memcpy( s1, s2, n );
}

/*********************************************************************
 *                  memset   (CRTDLL.432)
 */
void * CDECL CRTDLL_memset( void *s, int i, size_t n )
{
    dprintf(("CRTDLL: memset\n"));
    return memset( s, i, n );
}

/*********************************************************************
 *                  remove           (CRTDLL.445)
 */
INT CDECL CRTDLL_remove(LPCSTR file)
{
        dprintf(("CRTDLL: remove\n"));
	if (!DeleteFileA(file))
		return -1;
	return 0;
}

/*********************************************************************
 *                  vfprintf       (CRTDLL.494)
 */
INT CDECL CRTDLL_vfprintf( CRTDLL_FILE *file, LPSTR format, va_list args )
{
    dprintf(("CRTDLL: vprintf\n"));
    char buffer[2048];  /* FIXME... */

    vsprintf( buffer, format, args );
    return CRTDLL_fwrite( buffer, 1, strlen(buffer), file );
}

/*********************************************************************
 *                  vfwprintf     (CRTDLL.495)
 */
int CDECL CRTDLL_vfwprintf( FILE *F, const wchar_t *s, va_list arg )
{
  dprintf(("CRTDLL: vfwprintf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  vprintf       (CRTDLL.496)
 */
int CDECL CRTDLL_vprintf( const char *format, __va_list arg )
{
  dprintf(("CRTDLL: vprintf\n"));

  return (vprintf(format, arg));
}

/*********************************************************************
 *                  vsprintf       (CRTDLL.497)
 */
int CDECL CRTDLL_vsprintf( char *s, const char *format, va_list arg )
{
  dprintf(("CRTDLL: vsprintf\n"));

  return (vsprintf(s, format, arg));
}

/*********************************************************************
 *                  vswprintf       (CRTDLL.498)
 */
int CDECL CRTDLL_vswprintf( wchar_t *s , size_t t, const wchar_t *format, va_list arg )
{
  dprintf(("CRTDLL: vswprintf\n"));

  return (vswprintf(s, t, format, arg));
}

/*********************************************************************
 *                  vwprintf     (CRTDLL.499)
 */
int CDECL CRTDLL_vwprintf( const wchar_t *s, va_list arg)
{
  dprintf(("CRTDLL: vwprintf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  wcscoll    (CRTDLL.503)
 */
int CDECL CRTDLL_wcscoll(const wchar_t *s1, const wchar_t *s2)
{
  dprintf(("CRTDLL: wcscoll\n"));

  return (wcscoll(s1, s2));
}

/*********************************************************************
 *                  wcsftime   (CRTDLL.506)
 */
size_t CDECL CRTDLL_wcsftime( wchar_t *s, size_t maxsize, 
		const wchar_t *format, const struct tm *timeptr )
{
  dprintf(("CRTDLL: wcsftime\n"));

  return (wcsftime(s, maxsize, format, timeptr));
}

/*********************************************************************
 *                  wcstod   (CRTDLL.515)
 */
double CDECL CRTDLL_wcstod( const wchar_t *nptr, wchar_t **endptr )
{
  dprintf(("CRTDLL: wcstod\n"));
  return (wcstod(nptr, endptr));
}

/*********************************************************************
 *                  wcstok   (CRTDLL.516)
 */
wchar_t *CRTDLL_wcstok( wchar_t *s1, const wchar_t *s2, wchar_t **ptr )
{
  dprintf(("CRTDLL: wcstod\n"));
  return (wcstok(s1, s2, ptr));
}
/*********************************************************************
 *                  wcstol   (CRTDLL.517)
 */
long int CDECL CRTDLL_wcstol( const wchar_t *s1, wchar_t **s2, int i )
{
  dprintf(("CRTDLL: wcstol\n"));

  return (wcstol(s1, s2, i));
}

/*********************************************************************
 *                  wcstombs   (CRTDLL.518)
 */
size_t CDECL CRTDLL_wcstombs( char *s, const wchar_t *pwcs, size_t n )
{
  dprintf(("CRTDLL: wcstombs\n"));

  return (wcstombs(s, pwcs, n));
}

/*********************************************************************
 *                  wcstoul   (CRTDLL.519)
 */
unsigned long int CDECL CRTDLL_wcstoul( const wchar_t *s1, wchar_t **s2, int i )
{
  dprintf(("CRTDLL: wcstoul\n"));

  return (wcstoul(s1, s2, i));
}

/*********************************************************************
 *                  wcsxfrm   (CRTDLL.520)
 */
size_t CDECL CRTDLL_wcsxfrm( wchar_t *s1, const wchar_t *s2, size_t n )
{
  dprintf(("CRTDLL: wcsxfrm\n"));

  return (wcsxfrm(s1, s2, n));
}

/*********************************************************************
 *                  wcstomb   (CRTDLL.521)
 */
int CDECL CRTDLL_wctomb( char *s, wchar_t wchar )
{
  dprintf(("CRTDLL: wctomb\n"));

  return (wctomb(s,wchar));
}

/*********************************************************************
 *                  fwprintf   (CRTDLL.522)
 */
int CDECL CRTDLL_fwprintf( const wchar_t *s, ... )
{
  dprintf(("CRTDLL: fwprintf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  wscanf    (CRTDLL.523)
 */
int CDECL CRTDLL_wscanf( const wchar_t *s, ... )
{
  dprintf(("CRTDLL: wscanf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


INT WINAPI lstrncmpiA( LPCSTR str1, LPCSTR str2, INT n )
{
  dprintf(("CRTDLL: lstrncmpiA\n"));
  //CB: implement!
  return 0;
}
