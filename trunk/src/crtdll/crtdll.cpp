/* $Id: crtdll.cpp,v 1.2 1999-09-13 09:05:13 sandervl Exp $ */

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
#include <math.h>
#include <locale.h>
#include <signal.h>

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
 *                  _aexit_rtn_dll    (CRTDLL.39)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__aexit_rtn_dll(DWORD ret)
{
        dprintf(("CRTDLL: _aexit_rtn_dll\n"));

	return 0;
}

/*********************************************************************
 *                  _amsg_exit    (CRTDLL.40)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__amsg_exit(DWORD ret)
{
        dprintf(("CRTDLL: _amsg_exit\n"));

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
 *                  _except_handler2    (CRTDLL.78)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__except_handler2(DWORD ret)
{
        dprintf(("CRTDLL: _except_handler2\n"));

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
 *                  isleadbyte	(CRTDLL.335)
 */
void CDECL CRTDLL_abort( void )
{
  dprintf(("CRTDLL: abort\n"));
  abort();
}

/*********************************************************************
 *                  acos	(CRTDLL.336)
 */
double CDECL CRTDLL_acos( double x )
{
  dprintf(("CRTDLL: acos\n"));
  return (acos(x));
}

/*********************************************************************
 *                  asctime	(CRTDLL.338)
 */
char * CDECL CRTDLL_asctime( const struct tm *timeptr )
{
  dprintf(("CRTDLL: asctime\n"));
  return (asctime(timeptr));
}

/*********************************************************************
 *                  asin	(CRTDLL.339)
 */
double CDECL CRTDLL_asin( double x )
{
  dprintf(("CRTDLL: asin\n"));
  return (asin(x));
}

/*********************************************************************
 *                  atan2	(CRTDLL.341)
 */
double CDECL CRTDLL_atan2( double y, double x )
{
  dprintf(("CRTDLL: atan2\n"));
  return (atan2(y, x));
}

/*********************************************************************
 *                  atexit	(CRTDLL.342)
 */
int CDECL CRTDLL_atexit( register void ( *func )( void ) )
{
  dprintf(("CRTDLL: atexit\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  atof 	(CRTDLL.343)
 */
double CDECL CRTDLL_atof( const char *nptr )
{
  dprintf(("CRTDLL: atof\n"));
  return (atof(nptr));
}

/*********************************************************************
 *                  bsearch	(CRTDLL.346)
 */
void * CDECL CRTDLL_bsearch( const void *key, const void *base,  size_t nmemb,
	 size_t size, int (*compar)(const void *pkey, const void *pbase) )
{
  dprintf(("CRTDLL: bsearch\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  calloc	(CRTDLL.347)
 */
void * CDECL CRTDLL_calloc( size_t n, size_t size )
{
  dprintf(("CRTDLL: calloc\n"));
  return (calloc(n, size));
}

/*********************************************************************
 *                  clearerr	(CRTDLL.349)
 */
void CDECL CRTDLL_clearerr( FILE *fp )
{
  dprintf(("CRTDLL: clearerr\n"));
  clearerr(fp);
}

/*********************************************************************
 *                  clock	(CRTDLL.350)
 */
clock_t CDECL CRTDLL_clock( void )
{
  dprintf(("CRTDLL: clock\n"));
  return (clock());
}

/*********************************************************************
 *                  cosh	(CRTDLL.352)
 */
double CDECL CRTDLL_cosh( double x )
{
  dprintf(("CRTDLL: cosh\n"));
  return (cosh(x));
}

/*********************************************************************
 *                  ctime	(CRTDLL.353)
 */
char * CDECL CRTDLL_ctime( const time_t *timer )
{
  dprintf(("CRTDLL: ctime\n"));
  return (ctime(timer));
}

/*********************************************************************
 *                  difftime	(CRTDLL.354)
 */
double CDECL CRTDLL_difftime( time_t t1, time_t t0 )
{
  dprintf(("CRTDLL: difftime\n"));
  return (difftime(t1, t0));
}

/*********************************************************************
 *                  div		(CRTDLL.355)
 */
div_t CDECL CRTDLL_div( int numer, int denom )
{
  dprintf(("CRTDLL: div\n"));
  return (div(numer, denom));
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
 *                  exp		(CRTDLL.357)
 */
double CDECL CRTDLL_exp( double x )
{
  dprintf(("CRTDLL: exp\n"));
  return (exp(x));
}

/*********************************************************************
 *                  fclose	(CRTDLL.359)
 */
int CDECL CRTDLL_fclose( FILE *fp )
{
  dprintf(("CRTDLL: fclose\n"));
  return (fclose(fp));
}

/*********************************************************************
 *                  feof	(CRTDLL.360)
 */
int CDECL CRTDLL_feof( FILE *fp )
{
  dprintf(("CRTDLL: feof\n"));
  return (feof(fp));
}

/*********************************************************************
 *                  ferror	(CRTDLL.361)
 */
int CDECL CRTDLL_ferror( FILE *fp )
{
  dprintf(("CRTDLL: ferror\n"));
  return (ferror(fp));
}

/*********************************************************************
 *                  fflush	(CRTDLL.362)
 */
int CDECL CRTDLL_fflush( FILE *fp )
{
  dprintf(("CRTDLL: fflush\n"));
  return (fflush(fp));
}

/*********************************************************************
 *                  fgetc 	(CRTDLL.363)
 */
int CDECL CRTDLL_fgetc( FILE *fp )
{
  dprintf(("CRTDLL: fgetc\n"));
  return (fgetc(fp));
}

/*********************************************************************
 *                  fgetpos	(CRTDLL.364)
 */
int CDECL CRTDLL_fgetpos( FILE *fp, fpos_t *pos )
{
  dprintf(("CRTDLL: fgetpos\n"));
  return (fgetpos(fp, pos));
}

/*********************************************************************
 *                  fgets	(CRTDLL.365)
 */
char * CDECL CRTDLL_fgets( char *s, int n, FILE *fp )
{
  dprintf(("CRTDLL: fgets\n"));
  return (fgets(s, n, fp));
}

/*********************************************************************
 *                  fgetwc	(CRTDLL.366)
 */
wint_t CDECL CRTDLL_fgetwc( FILE *f )
{
  dprintf(("CRTDLL: fgetwc\n"));
  return (fgetwc(f));
}

/*********************************************************************
 *                  fmod	(CRTDLL.368)
 */
double CDECL CRTDLL_fmod(double x, double y )
{
  dprintf(("CRTDLL: fmod\n"));
  return (fmod(x,y));
}

/*********************************************************************
 *                  fopen	(CRTDLL.369)
 */
FILE * CDECL CRTDLL_fopen( const char *filename, const char *mode )
{
  dprintf(("CRTDLL: fopen\n"));
  return (fopen( filename, mode));
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
 *                  fputc   (CRTDLL.371)
 */
int CDECL CRTDLL_fputc( int c, FILE *fp )
{
  dprintf(("CRTDLL: fputc\n"));
  return (fputc(c, fp));
}

/*********************************************************************
 *                  fputs   (CRTDLL.372)
 */
int CDECL CRTDLL_fputs( const char *s, FILE *fp )
{
  dprintf(("CRTDLL: fputs\n"));
  return (fputs(s, fp));
}

/*********************************************************************
 *                  fputwc  (CRTDLL.373)
 */
wint_t CDECL CRTDLL_fputwc( wint_t wc, FILE *strm )
{
  dprintf(("CRTDLL: fputwc\n"));
  return (fputwc(wc, strm));
}

/*********************************************************************
 *                  fread  (CRTDLL.374)
 */
size_t CDECL CRTDLL_fread( void *ptr, size_t size, size_t n, FILE *fp )
{
  dprintf(("CRTDLL: fread\n"));
  return (fread(ptr, size, n, fp));
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
 *                  freopen	  (CRTDLL.376)
 */
FILE * CDECL CRTDLL_freopen( const char *filename, const char *mode, FILE *fp )
{
  dprintf(("CRTDLL: freopen\n"));
  return (freopen(filename, mode, fp));
}

/*********************************************************************
 *                  frexp	  (CRTDLL.377)
 */
double CDECL CRTDLL_frexp( double value, int *exp )
{
  dprintf(("CRTDLL: frexp\n"));
  return (frexp(value, exp));
}

/*********************************************************************
 *                  fscanf	  (CRTDLL.378)
 */
int CDECL CRTDLL_fscanf( FILE*fp, const char *format, ... )
{
  dprintf(("CRTDLL: frexp\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  fseek 	  (CRTDLL.379)
 */
int CDECL CRTDLL_fseek( FILE *fp, long int offset, int whence )
{
  dprintf(("CRTDLL: fseek\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  fsetpos	  (CRTDLL.380)
 */
int CDECL CRTDLL_fsetpos( FILE *fp, const fpos_t *pos )
{
  dprintf(("CRTDLL: fsetpos\n"));
  return (fsetpos(fp, pos));
}

/*********************************************************************
 *                  ftell  	  (CRTDLL.381)
 */
long int CDECL CRTDLL_ftell( FILE *fp )
{
  dprintf(("CRTDLL: ftell\n"));
  return (ftell(fp));
}

/*********************************************************************
 *                  fwprintf 	  (CRTDLL.382)
 */
int CDECL CRTDLL_fwprintf( FILE *strm, const wchar_t *format, ... )
{
  dprintf(("CRTDLL: fwprintf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *                  fwscanf 	  (CRTDLL.384)
 */
int CDECL CRTDLL_fwscanf( FILE *strm, const wchar_t *format, ... )
{
  dprintf(("CRTDLL: fwscanf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  getc    (CRTDLL.385)
 */
int CDECL CRTDLL_getc( FILE *fp )
{
  dprintf(("CRTDLL: getc\n"));
  return (getc(fp));
}

/*********************************************************************
 *                  getchar    (CRTDLL.386)
 */
int CDECL CRTDLL_getchar( void )
{
  dprintf(("CRTDLL: getchar\n"));
  return (getchar());
}

/*********************************************************************
 *                  getenv    (CRTDLL.387)
 */
char * CDECL CRTDLL_getenv( const char *name )
{
  dprintf(("CRTDLL: getenv\n"));
  return (getenv(name));
}

/*********************************************************************
 *                  gets    (CRTDLL.388)
 */
char * CDECL CRTDLL_gets( char *s )
{
  dprintf(("CRTDLL: gets\n"));
  return (gets(s));
}

/*********************************************************************
 *                  gmtime    (CRTDLL.389)
 */
struct tm * CDECL CRTDLL_gmtime( const time_t *timer )
{
  dprintf(("CRTDLL: gmtime\n"));
  return (gmtime(timer));
}

/*********************************************************************
 *                  is_wctype    (CRTDLL.390)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL_is_wctype(DWORD ret)
{
        dprintf(("CRTDLL: is_wctype\n"));
	return 0;
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
 *                  ldexp	(CRTDLL.417)
 */
double CDECL CRTDLL_ldexp( double x, int exp ) 
{
  dprintf(("CRTDLL: ldexp\n"));

  return (ldexp(x, exp));
}

/*********************************************************************
 *                  ldiv	(CRTDLL.418)
 */
ldiv_t CDECL CRTDLL_ldiv( long int numer, long int denom ) 
{
  dprintf(("CRTDLL: ldiv\n"));

  return (ldiv(numer, denom));
}

/*********************************************************************
 *                  localeconv	(CRTDLL.419)
 */
struct lconv * CDECL CRTDLL_localeconv(void)
{
  dprintf(("CRTDLL: localeconv\n"));
  return (localeconv());
}

/*********************************************************************
 *                  localtime 	(CRTDLL.420)
 */
struct tm * CDECL CRTDLL_localtime( const time_t *timer )
{
  dprintf(("CRTDLL: localtime\n"));
  return (localtime(timer));
}

/*********************************************************************
 *                  log10	(CRTDLL.422)
 */
double CDECL CRTDLL_log10( double x )
{
  dprintf(("CRTDLL: log10\n"));
  return (log10(x));
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
 *                  mblen        (CRTDLL.425)
 */
INT CDECL CRTDLL_mblen( const char *s, size_t n )
{
      dprintf(("CRTDLL: mblen\n"));
      return (mblen(s, n));
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
 *                  mktime   (CRTDLL.433)
 */
time_t CDECL CRTDLL_mktime( struct tm *timeptr )
{
    dprintf(("CRTDLL: mktime\n"));
    return mktime( timeptr );
}

/*********************************************************************
 *                  modf   (CRTDLL.434)
 */
double CDECL CRTDLL_modf( double value, double *iptr )
{
    dprintf(("CRTDLL: modf\n"));
    return modf( value, iptr );
}

/*********************************************************************
 *                  perror   (CRTDLL.435)
 */
void CDECL CRTDLL_perror( const char *s )
{
    dprintf(("CRTDLL: perror\n"));
    perror( s );
}

/*********************************************************************
 *                  printf   (CRTDLL.437)
 */
int CDECL CRTDLL_printf( const char *format, ... )
{
    dprintf(("CRTDLL: printf\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

/*********************************************************************
 *                  putc      (CRTDLL.438)
 */
int CDECL CRTDLL_putc( int c, FILE *fp )
{
    dprintf(("CRTDLL: putc\n"));
    return putc( c, fp );
}

/*********************************************************************
 *                  putchar      (CRTDLL.439)
 */
int CDECL CRTDLL_putchar( int c )
{
    dprintf(("CRTDLL: putchar\n"));
    return putchar( c );
}

/*********************************************************************
 *                  puts         (CRTDLL.440)
 */
int CDECL CRTDLL_puts( const char *s )
{
    dprintf(("CRTDLL: puts\n"));
    return puts( s );
}

/*********************************************************************
 *                  raise        (CRTDLL.442)
 */
int CDECL CRTDLL_raise( int sig )
{
    dprintf(("CRTDLL: raise\n"));
    return raise( sig );
}

/*********************************************************************
 *                  rand   (CRTDLL.443)
 */
int CDECL CRTDLL_rand( void )
{
    dprintf(("CRTDLL: rand\n"));
    return (rand());
}

/*********************************************************************
 *                  realloc   (CRTDLL.444)
 */
void * CDECL CRTDLL_realloc( void *ptr, size_t size )
{
    dprintf(("CRTDLL: realloc\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
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
 *                  rename      (CRTDLL.446)
 */
int CDECL CRTDLL_rename (const char *old, const char *new2)
{
  dprintf(("CRTDLL: rename\n"));
  return (rename(old, new2));
}

/*********************************************************************
 *                  rewind      (CRTDLL.447)
 */
void CDECL CRTDLL_rewind( FILE *fp )
{
  dprintf(("CRTDLL: rewind\n"));
  rewind(fp);
}

/*********************************************************************
 *                  scanf       (CRTDLL.448)
 */
int CDECL CRTDLL_scanf( const char *format, ... )
{
  dprintf(("CRTDLL: scanf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
	
/*********************************************************************
 *                  setbuf      (CRTDLL.449)
 */
void CDECL CRTDLL_setbuf( FILE *fp, char *buf )	
{
  dprintf(("CRTDLL: setbuf\n"));
  setbuf(fp, buf);
}

/*********************************************************************
 *                  setlocale      (CRTDLL.450)
 */
char * CDECL CRTDLL_setlocale(int category,const char *locale)
{
  dprintf(("CRTDLL: setlocale\n"));
  return (setlocale(category, locale));
}

/*********************************************************************
 *                  setvbuf      (CRTDLL.451)
 */
int CDECL CRTDLL_setvbuf( FILE *fp, char *buf, int mode, size_t size )
{
  dprintf(("CRTDLL: setvbuf\n"));
  return (setvbuf(fp, buf, mode, size));
}

/*********************************************************************
 *                  signal       (CRTDLL.452)
 */
void CDECL CRTDLL_signal( int sig, void (*func)(int)) 
{
    dprintf(("CRTDLL: signal\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}

/*********************************************************************
 *                  sinh         (CRTDLL.454)
 */
double CDECL CRTDLL_sinh( double x )
{
  dprintf(("CRTDLL: sinh\n"));
  return (sinh(x));
}

/*********************************************************************
 *                  srand        (CRTDLL.457)
 */
void CDECL CRTDLL_srand( unsigned int seed )
{
  dprintf(("CRTDLL: srand\n"));
  srand(seed);
}

/*********************************************************************
 *                  strcoll        (CRTDLL.462)
 */
int CDECL CRTDLL_strcoll( const char *s1, const char *s2 )
{
  dprintf(("CRTDLL: strcoll\n"));
  return strcoll(s1, s2);
}

/*********************************************************************
 *                  strerror        (CRTDLL.465)
 */
char * CDECL CRTDLL_strerror( int errnum )
{
  dprintf(("CRTDLL: strerror\n"));
  return strerror(errnum);
}

/*********************************************************************
 *                  strftime        (CRTDLL.466)
 */
size_t CDECL CRTDLL_strftime( char *s, size_t maxsiz, const char *fmt, const struct tm *tp )
{
  dprintf(("CRTDLL: strftime\n"));
  return strftime(s, maxsiz, fmt, tp);
}
 

/*********************************************************************
 *                  strtod        (CRTDLL.475)
 */
double CDECL CRTDLL_strtod( const char *nptr, char **endptr )
{
  dprintf(("CRTDLL: strtod\n"));
  return strtod(nptr, endptr);
}

/*********************************************************************
 *                  strtok        (CRTDLL.476)
 */
char * CDECL CRTDLL_strtok( char *s1, const char *s2 )
{
  dprintf(("CRTDLL: strtok\n"));
  return strtok(s1, s2);
}

/*********************************************************************
 *                  strtol        (CRTDLL.477)
 */
long int CDECL CRTDLL_strtol( const char *nptr, char **endptr, int base )
{
  dprintf(("CRTDLL: strtol\n"));
  return strtol(nptr, endptr, base);
}

/*********************************************************************
 *                  strtoul        (CRTDLL.478)
 */
unsigned long CDECL CRTDLL_strtoul( const char *nptr, char **endptr, int base )
{
  dprintf(("CRTDLL: strtoul\n"));
  return strtoul(nptr, endptr, base);
}

/*********************************************************************
 *                  strxfrm        (CRTDLL.479)
 */
size_t CDECL CRTDLL_strxfrm( char *s1, const char *s2, size_t n )
{
  dprintf(("CRTDLL: strxfrm\n"));
  return strxfrm(s1, s2, n);
}

/*********************************************************************
 *                  swscanf           (CRTDLL.481)
 */
int CDECL CRTDLL_swscanf( const wchar_t *s1, const wchar_t *s2, ... )
{
  dprintf(("CRTDLL: swscanf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  system         (CRTDLL.482)
 */
int CDECL CRTDLL_system( const char *string )
{
  dprintf(("CRTDLL: system\n"));
  return system(string);
}

/*********************************************************************
 *                  tanh           (CRTDLL.485)
 */
double CDECL CRTDLL_tanh( double x )
{
  dprintf(("CRTDLL: tanh\n"));
  return tanh(x);
}

/*********************************************************************
 *                  time           (CRTDLL.485)
 */
time_t CDECL CRTDLL_time( time_t *timer )
{
  dprintf(("CRTDLL: time\n"));

  return time(timer);
}

/*********************************************************************
 *                  tmpnam           (CRTDLL.486)
 */
FILE * CDECL CRTDLL_tmpfile( void )
{
  dprintf(("CRTDLL: tmpfile\n"));
  return (tmpfile());
}
	
/*********************************************************************
 *                  tmpnam           (CRTDLL.487)
 */
char * CDECL CRTDLL_tmpnam( char *s )
{
  dprintf(("CRTDLL: tmpnam\n"));

  return (tmpnam(s));
}

/*********************************************************************
 *                  ungetc           (CRTDLL.492)
 */
INT CDECL CRTDLL_ungetc(int c)
{
  dprintf(("CRTDLL: ungetc\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  ungetwc           (CRTDLL.493)
 */
wint_t CDECL CRTDLL_ungetwc( wint_t wc, FILE *strm )
{
  dprintf(("CRTDLL: ungetwc\n"));

  return (ungetwc(wc, strm));
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
 *                  wprintf   (CRTDLL.522)
 */
int CDECL CRTDLL_wprintf( const wchar_t *s, ... )
{
  dprintf(("CRTDLL: wprintf\n"));
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
