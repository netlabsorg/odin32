/* $Id: crtdll.cpp,v 1.4 1999-09-18 15:58:34 sandervl Exp $ */

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
#include <io.h>
#include <assert.h>
#include <process.h>
#include <float.h>
#include <conio.h>
#include <direct.h>
#include <malloc.h>
#include <sys\utime.h>
#include <sys\stat.h>

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
 *           CRTDLL___isascii   (CRTDLL.28)
 */
int CDECL CRTDLL___isascii(int i)
{
  dprintf(("CRTDLL: __isascii\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL___iscsym   (CRTDLL.29)
 */
int CDECL CRTDLL___iscsym(int i)
{
  dprintf(("CRTDLL: __iscsym\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL___iscsymf   (CRTDLL.30)
 */
int CDECL CRTDLL___iscsymf(int i)
{
  dprintf(("CRTDLL: __iscsymf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *           CRTDLL___threadhandle   (CRTDLL.32)
 */
unsigned long CDECL CRTDLL___threadhandle( void )
{
  dprintf(("CRTDLL: __threadhandle\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL___threadid   (CRTDLL.33)
 */
int * CDECL CRTDLL___threadid(void)
{
  dprintf(("CRTDLL: __threadid\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__abnormal_termination   (CRTDLL.36)
 */
int CDECL CRTDLL__abnormal_termination(void)
{
  dprintf(("CRTDLL: _abnormal_termination\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__access   (CRTDLL.37)
 */
int CDECL CRTDLL__access(const char *path,int mode)
{
  dprintf(("CRTDLL: _access\n"));
  return (_access(path, mode));
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
 *           CRTDLL__assert   (CRTDLL.41)
 */
void CDECL CRTDLL__assert( char *s1, char *s2, int i)
{
  dprintf(("CRTDLL: _assert\n"));
  _assert(s1, s2, i);
}


/*********************************************************************
 *           CRTDLL__beginthread   (CRTDLL.46)
 */
unsigned long CDECL CRTDLL__beginthread(  register void (*start_address)(void *),
		unsigned stack_size, void *arglist )
{
  dprintf(("CRTDLL: _beginthread\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *           CRTDLL__close   (CRTDLL.57)
 */
int CDECL CRTDLL__close(int handle)
{
  dprintf(("CRTDLL: _close\n"));
  return (_close(handle));
}


/*********************************************************************
 *           CRTDLL__control87   (CRTDLL.60)
 */
unsigned CDECL CRTDLL__control87(unsigned i1,unsigned i2)
{
  dprintf(("CRTDLL: _control87\n"));
  return (_control87(i1, i2));
}


/*********************************************************************
 *           CRTDLL__cwait   (CRTDLL.69)
 */
int CDECL CRTDLL__cwait( int *status, int process_id, int action_code )
{
  dprintf(("CRTDLL: _cwait\n"));
  return (_cwait(status, process_id, action_code));
}


/*********************************************************************
 *           CRTDLL__dup   (CRTDLL.71)
 */
int CDECL CRTDLL__dup(int handle)
{
  dprintf(("CRTDLL: _dup\n"));
  return (_dup(handle));
}


/*********************************************************************
 *           CRTDLL__dup2  (CRTDLL.72)
 */
int CDECL CRTDLL__dup2(int handle1,int handle2)
{
  dprintf(("CRTDLL: _dup2\n"));
  return (_dup2(handle1, handle2));
}


/*********************************************************************
 *           CRTDLL__ecvt  (CRTDLL.73)
 */
char * CDECL CRTDLL__ecvt( double val, int ndig, int *dec, int *sign )
{
  dprintf(("CRTDLL: _ecvt\n"));
  return (_ecvt(val, ndig, dec, sign));
}


/*********************************************************************
 *           CRTDLL__endthread  (CRTDLL.74)
 */
void CDECL CRTDLL__endthread(void)
{
  dprintf(("CRTDLL: _endthread\n"));
  _endthread ();
}


/*********************************************************************
 *           CRTDLL__errno  (CRTDLL.77)
 */
int * CDECL CRTDLL__errno(void)
{
  dprintf(("CRTDLL: _errno\n"));
  return (_errno());
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
 *           CRTDLL__expand   (CRTDLL.88)
 */
void * CDECL CRTDLL__expand( void *ptr, size_t size )
{
  dprintf(("CRTDLL: _expand\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__fcvt  (CRTDLL.90)
 */
char * CDECL CRTDLL__fcvt( double val, int ndig, int *dec, int *sign )
{
  dprintf(("CRTDLL: _fcvt\n"));
  return (_fcvt(val, ndig, dec, sign));
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
 *           CRTDLL__fgetchar  (CRTDLL.92)
 */
int CDECL CRTDLL__fgetchar( void )
{
  dprintf(("CRTDLL: _fgetchar\n"));
  return (_fgetchar());
}


/*********************************************************************
 *           CRTDLL__fgetwchar  (CRTDLL.93)
 */
wint_t CDECL CRTDLL__fgetwchar( void *i )
{
  dprintf(("CRTDLL: _fgetwchar\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


 /*********************************************************************
 *                  _findclose    (CRTDLL.098)
 */
int CDECL CRTDLL__findclose( long handle )
{
  dprintf(("CRTDLL: _findclose\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


 /*********************************************************************
 *                  _findfirst    (CRTDLL.099)
 */
DWORD CDECL CRTDLL__findfirst(LPCSTR fname,  struct find_t * x2)
{
  dprintf(("CRTDLL: _findfirst\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _findnext     (CRTDLL.100)
 */
INT CDECL CRTDLL__findnext(DWORD hand, struct find_t * x2)
{
  dprintf(("CRTDLL: _findnext\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _finite     (CRTDLL.101)
 */
INT CDECL CRTDLL__finite(double x)
{
  dprintf(("CRTDLL: _finite\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fpreset     (CRTDLL.107)
 */
INT CDECL CRTDLL__fpreset(void)
{
  dprintf(("CRTDLL: _fpreset\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fputchar     (CRTDLL.108)
 */
INT CDECL CRTDLL__fputchar( int c )
{
  dprintf(("CRTDLL: _fputchar\n"));
  return(_fputchar(c));
}


/*********************************************************************
 *                  _fputwchar     (CRTDLL.109)
 */
wint_t CDECL CRTDLL__fputwchar( wint_t )
{
  dprintf(("CRTDLL: _fputwchar\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fsopen     (CRTDLL.110)
 */
FILE * CDECL CRTDLL__fsopen( const char *filename, const char *mode, int shflag )
{
  dprintf(("CRTDLL: _fsopen\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fstat        (CRTDLL.111)
 */
int CDECL CRTDLL__fstat(int file, struct stat* buf)
{
  dprintf(("CRTDLL: _fstat\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ftime        (CRTDLL.112)
 */
int CDECL CRTDLL__ftime( struct timeb *timeptr )
{
  dprintf(("CRTDLL: _ftime\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fullpath     (CRTDLL.114)
 */
char * CDECL CRTDLL__fullpath( char *buf, char *path, size_t size )
{
  dprintf(("CRTDLL: _fullpath\n"));
  return (_fullpath(buf, path, size));
}


/*********************************************************************
 *                  _gcvt     (CRTDLL.116)
 */
char * CDECL CRTDLL__gcvt( double val, int ndig, char *buf )
{
  dprintf(("CRTDLL: _gcvt\n"));
  return (_gcvt(val, ndig, buf));
}


/*********************************************************************
 *                  _get_osfhandle     (CRTDLL.117)
 */
long CDECL CRTDLL__get_osfhandle( int posixhandle )
{
  dprintf(("CRTDLL: _gcvt\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getch     (CRTDLL.118)
 */
int CDECL CRTDLL__getch(void)
{
  dprintf(("CRTDLL: _getch\n"));
  return (_getch());
}


/*********************************************************************
 *                  _getche     (CRTDLL.119)
 */
int CDECL CRTDLL__getche(void)
{
  dprintf(("CRTDLL: _getche\n"));
  return (_getche());
}


/*********************************************************************
 *                  _getcwd     (CRTDLL.120)
 */
char * CDECL CRTDLL__getcwd( char *buf, size_t size )
{
  dprintf(("CRTDLL: _getcwd\n"));
  return (_getcwd(buf, size));
}


/*********************************************************************
 *                  _getdcwd     (CRTDLL.121)
 */
char * CDECL CRTDLL__getdcwd( int drive, char *buffer, size_t maxlen )
{
  dprintf(("CRTDLL: _getdcwd\n"));
  return (_getdcwd(drive, buffer, maxlen));
}


/*********************************************************************
 *                  _getdiskfree     (CRTDLL.122)
 */
unsigned CDECL CRTDLL__getdiskfree( unsigned drive, struct _diskfree_t *diskspace)
{
  dprintf(("CRTDLL: _getdiskfree\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getdrive    (CRTDLL.124)
 */
unsigned CDECL CRTDLL__getdrive( void )
{
  dprintf(("CRTDLL: _getdrive\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getw     (CRTDLL.128)
 */
int CDECL CRTDLL__getw( FILE *fp )
{
  dprintf(("CRTDLL: _getw\n"));
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
 *                  _heapchk    (CRTDLL.130)
 */
int CDECL CRTDLL__heapchk( void )
{
  dprintf(("CRTDLL: _heapchk\n"));
  return (_heapchk());
}


/*********************************************************************
 *                  _heapmin    (CRTDLL.131)
 */
int CDECL CRTDLL__heapmin( void )
{
  dprintf(("CRTDLL: _heapmin\n"));
  return (_heapmin());
}


/*********************************************************************
 *                  _heapset    (CRTDLL.132)
 */
int CDECL CRTDLL__heapset( unsigned int fill )
{
  dprintf(("CRTDLL: _heapset\n"));
  return (_heapset(fill));
}


/*********************************************************************
 *                  _heapwalk     (CRTDLL.133)
 */
int CDECL CRTDLL__heapwalk( struct _heapinfo *entry )
{
  dprintf(("CRTDLL: _heapwalk\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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


/*********************************************************************
 *                  _ismbbalnum     (CRTDLL.139)
 */
int CDECL CRTDLL__ismbbalnum( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbalnum\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbalpha     (CRTDLL.140)
 */
int CDECL CRTDLL__ismbbalpha( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbalpha\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbgraph     (CRTDLL.141)
 */
int CDECL CRTDLL__ismbbgraph( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbgraph\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkalnum     (CRTDLL.142)
 */
int CDECL CRTDLL__ismbbkalnum( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbkalnum\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkana     (CRTDLL.143)
 */
int CDECL CRTDLL__ismbbkana( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbkana\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkpunct     (CRTDLL.144)
 */
int CDECL CRTDLL__ismbbkpunct( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbkpunct\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbblead     (CRTDLL.145)
 */
int CDECL CRTDLL__ismbblead( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbblead\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbprint     (CRTDLL.146)
 */
int CDECL CRTDLL__ismbbprint( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbprint\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbpunct     (CRTDLL.147)
 */
int CDECL CRTDLL__ismbbpunct( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbpunct\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbtrail     (CRTDLL.148)
 */
int CDECL CRTDLL__ismbbtrail( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbtrail\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcalpha     (CRTDLL.149)
 */
int CDECL CRTDLL__ismbcalpha( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcalpha\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcdigit     (CRTDLL.150)
 */
int CDECL CRTDLL__ismbcdigit( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcdigit\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbchira     (CRTDLL.151)
 */
int CDECL CRTDLL__ismbchira( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbchira\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbckata     (CRTDLL.152)
 */
int CDECL CRTDLL__ismbckata( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbckata\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  _ismbcl0     (CRTDLL.153)
 */
int CDECL CRTDLL__ismbcl0( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl0\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcl1     (CRTDLL.154)
 */
int CDECL CRTDLL__ismbcl1( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl1\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcl2     (CRTDLL.155)
 */
int CDECL CRTDLL__ismbcl2( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl2\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbclegal     (CRTDLL.156)
 */
int CDECL CRTDLL__ismbclegal( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbclegal\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbclower     (CRTDLL.157)
 */
int CDECL CRTDLL__ismbclower( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbclower\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcprint     (CRTDLL.158)
 */
int CDECL CRTDLL__ismbcprint( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcprint\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcspace     (CRTDLL.159)
 */
int CDECL CRTDLL__ismbcspace( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcspace\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcsymbol     (CRTDLL.160)
 */
int CDECL CRTDLL__ismbcsymbol( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcsymbol\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcupper     (CRTDLL.161)
 */
int CDECL CRTDLL__ismbcupper( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcupper\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
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
 *                  _locking        (CRTDLL.173)
 */
int CDECL CRTDLL__locking(int handle,int mode,unsigned long nbyte)
{
  dprintf(("CRTDLL: _locking\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _lrotl	(CRTDLL.175)
 */
unsigned long CDECL CRTDLL__lrotl( unsigned long value, unsigned int shift )
{
  dprintf(("CRTDLL: _lrotl\n"));
  return (_lrotl(value, shift));
}


/*********************************************************************
 *                  _lrotr	(CRTDLL.176)
 */
unsigned long CDECL CRTDLL__lrotr( unsigned long value, unsigned int shift )
{
  dprintf(("CRTDLL: _lrotr\n"));
  return (_lrotr(value, shift));
}


/*********************************************************************
 *                  _lseek	(CRTDLL.178)
 */
long CDECL CRTDLL__lseek(int handle,long offset,int origin)
{
  dprintf(("CRTDLL: _lssek\n"));
  return (_lseek(handle, offset, origin));
}


/*********************************************************************
 *                  _makepath	(CRTDLL.180)
 */
void CDECL CRTDLL__makepath( char *path, char *drive,
                    char *dir, char *fname, char *ext )
{
  dprintf(("CRTDLL: _makepath\n"));
  _makepath(path, drive, dir, fname, ext);
}


/*********************************************************************
 *                  _matherr	(CRTDLL.181)
 */
double CDECL CRTDLL__matherr( struct exception * excep )
{
  dprintf(("CRTDLL: _matherr\n"));
  return (_matherr(excep));
}

// unsigned int	_mbbtombc( unsigned int __ch );
// unsigned int	_mbctombb( unsigned int __ch );


/*********************************************************************
 *                  _mbscspn        (CRTDLL.199)
 */
size_t CDECL CRTDLL__mbscspn( const unsigned char *s, const unsigned char *charset )
{
  dprintf(("CRTDLL: _mbscspn\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *           CRTDLL__mbsdec    (CRTDLL.200)
 */
unsigned char * CDECL CRTDLL__mbsdec( const unsigned char *s, const unsigned char *ch )
{
  dprintf(("CRTDLL: _mbsdec\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsdec    (CRTDLL.201)
 */
unsigned char * CDECL CRTDLL__mbsdup( unsigned char *src )
{
  dprintf(("CRTDLL: _mbsdup\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsicmp   (CRTDLL.202)
 */
int CDECL CRTDLL__mbsicmp( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbsicmp\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbslwr    (CRTDLL.205)
 */
unsigned char * CDECL CRTDLL__mbslwr( unsigned char *s )
{
  dprintf(("CRTDLL: _mbslwr\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbcat  (CRTDLL.206)
 */
unsigned char * CDECL CRTDLL__mbsnbcat( unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcat\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbcmp  (CRTDLL.207)
 */
int CDECL CRTDLL__mbsnbcmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcmp\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbcnt  (CRTDLL.208)
 */
size_t CDECL CRTDLL__mbsnbcnt( const unsigned char *s, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcnt\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbcpy  (CRTDLL.209)
 */
unsigned char * CDECL CRTDLL__mbsnbcpy( unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcpy\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbicmp (CRTDLL.210)
 */
int CDECL CRTDLL__mbsnbicmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbicmp\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbset (CRTDLL.211)
 */
unsigned char * CDECL CRTDLL__mbsnbset( unsigned char *s, unsigned int ch, size_t n )
{
  dprintf(("CRTDLL: _mbsnbset\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsncat (CRTDLL.212)
 */
unsigned char * CDECL CRTDLL__mbsncat( unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsncat\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnccnt (CRTDLL.213)
 */
size_t CDECL CRTDLL__mbsnccnt( const unsigned char *s, size_t n )
{
  dprintf(("CRTDLL: _mbsnccnt\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsncmp (CRTDLL.214)
 */
int CDECL CRTDLL__mbsncmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsncmp\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsncpy (CRTDLL.215)
 */
unsigned char * CDECL CRTDLL__mbsncpy( unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsncpy\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnextc (CRTDLL.216)
 */
unsigned int CDECL CRTDLL__mbsnextc( const unsigned char *s )
{
  dprintf(("CRTDLL: _mbsnextc\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnicmp (CRTDLL.217)
 */
int CDECL CRTDLL__mbsnicmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnicmp\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsninc (CRTDLL.218)
 */
unsigned char * CDECL CRTDLL__mbsninc( const unsigned char *s, size_t count )
{
  dprintf(("CRTDLL: _mbsninc\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnset (CRTDLL.219)
 */
unsigned char * CDECL CRTDLL__mbsnset( unsigned char *s, unsigned int ch, size_t n )
{
  dprintf(("CRTDLL: _mbsnset\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbspbrk   (CRTDLL.220)
 */
unsigned char * CDECL CRTDLL__mbspbrk( const unsigned char *s, const unsigned char *charset )
{
  dprintf(("CRTDLL: _mbspbrk\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsrchr   (CRTDLL.221)
 */
unsigned char * CDECL CRTDLL__mbsrchr( const unsigned char *s, unsigned int ch )
{
  dprintf(("CRTDLL: _mbsrchr\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsrev    (CRTDLL.222)
 */
unsigned char * CDECL CRTDLL__mbsrev( unsigned char *s )
{
  dprintf(("CRTDLL: _mbsrev\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsset    (CRTDLL.223)
 */
unsigned char * CDECL CRTDLL__mbsset( unsigned char *s, unsigned int ch )
{
  dprintf(("CRTDLL: _mbsset\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsspn   (CRTDLL.224)
 */
size_t CDECL CRTDLL__mbsspn( const unsigned char *s, const unsigned char *charset )
{
  dprintf(("CRTDLL: _mbsspn\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsspnp   (CRTDLL.225)
 */
unsigned char * CDECL CRTDLL__mbsspnp( const unsigned char *s, const unsigned char *charset )
{
  dprintf(("CRTDLL: _mbsspnp\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsstr    (CRTDLL.226)
 */
unsigned char * CDECL CRTDLL__mbsstr( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbsstr\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbstok    (CRTDLL.227)
 */
unsigned char * CDECL CRTDLL__mbstok( unsigned char *s, const unsigned char *delim )
{
  dprintf(("CRTDLL: _mbstok\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsupr    (CRTDLL.229)
 */
unsigned char * CDECL CRTDLL__mbsupr( unsigned char *s )
{
  dprintf(("CRTDLL: _mbsupr\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *                  _mktemp        (CRTDLL.233)
 */
char * CDECL CRTDLL__mktemp( char * templt )
{
  dprintf(("CRTDLL: _mktemp\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _msize        (CRTDLL.234)
 */
size_t CDECL CRTDLL__msize( void *ptr )
{
  dprintf(("CRTDLL: _msize\n"));
  return (_msize(ptr));
}


/*********************************************************************
 *                  _open        (CRTDLL.237)
 */
INT CDECL CRTDLL__open(const char *path,int oflag,...)
{
  dprintf(("CRTDLL: _open\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _open_osfhandle  (CRTDLL.238)
 */
INT CDECL CRTDLL__open_osfhandle( long osfhandle, int flags )
{
  dprintf(("CRTDLL: _open_osfhandle\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _pclose     (CRTDLL.244)
 */
INT CDECL CRTDLL__pclose( FILE *fp )
{
  dprintf(("CRTDLL: _pclose\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *                  _pipe     (CRTDLL.247)
 */
INT CDECL CRTDLL__pipe( int *phandles, unsigned psize, int textmode )
{
  dprintf(("CRTDLL: _pipe\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _popen    (CRTDLL.248)
 */
FILE * CDECL CRTDLL__popen( const char *command, const char *mode )
{
  dprintf(("CRTDLL: _popen\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *                  _putw     (CRTDLL.252)
 */
INT CDECL CRTDLL__putw( int binint, FILE *fp )
{
  dprintf(("CRTDLL: _putw\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__rotl 	 (CRTDLL.257)
 */
unsigned int CDECL CRTDLL__rotl( unsigned int value, unsigned int shift )
{
  dprintf(("CRTDLL: _rotl\n"));
  return (_rotl(value, shift));
}


/*********************************************************************
 *           CRTDLL__rotr 	 (CRTDLL.258)
 */
unsigned int CDECL CRTDLL__rotr( unsigned int value, unsigned int shift )
{
  dprintf(("CRTDLL: _rotr\n"));
  return (_rotr(value, shift));
}
 

/*********************************************************************
 *           CRTDLL__searchenv 	 (CRTDLL.260)
 */
void CDECL CRTDLL__searchenv( const char *name, const char *env_var, char *buf )
{
  dprintf(("CRTDLL: _searchenv\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *           CRTDLL__setjmp 	 (CRTDLL.262)
 */
int CDECL CRTDLL__setjmp( jmp_buf env )
{
  dprintf(("CRTDLL: _setjmp -> setjmp\n"));
  return(setjmp( env));
}


/*********************************************************************
 *           CRTDLL__stat   	 (CRTDLL.278)
 */
int CDECL CRTDLL__stat( const char *s1, struct stat * n )
{
  dprintf(("CRTDLL: _stat\n"));
  return(_stat(s1, n));
}


/*********************************************************************
 *           CRTDLL__strdate   	 (CRTDLL.281)
 */
char * CDECL CRTDLL__strdate( char *buf )
{
  dprintf(("CRTDLL: _strdate\n"));
  return(_strdate(buf));
}


/*********************************************************************
 *           CRTDLL__strdec    	 (CRTDLL.282)
 */
char * CDECL CRTDLL__strdec( const char *, const char *p ) 
{
  dprintf(("CRTDLL: _strdec\n"));
  return( (char *)(p-1) );
}


/*********************************************************************
 *           CRTDLL__strdup    	 (CRTDLL.283)
 */
char * CDECL CRTDLL__strdup( const char *string )
{
  dprintf(("CRTDLL: _strdup\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__stricoll  	 (CRTDLL.286)
 */
int CDECL CRTDLL__stricoll( const char *s1, const char *s2 )
{
  dprintf(("CRTDLL: _stricoll\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__strinc  	 (CRTDLL.287)
 */
char * CDECL CRTDLL__strinc( const char *p )
{
    dprintf(("CRTDLL: _strinc\n"));
    return( (char *)(p+1) );
}


/*********************************************************************
 *           CRTDLL__strncnt	 (CRTDLL.289)
 */
size_t CDECL CRTDLL__strncnt( const char *p, size_t l ) 
{
    dprintf(("CRTDLL: _strncnt\n"));
    size_t i;
    i = strlen(p);
    return( (i>l) ? l : i );
}

/*********************************************************************
 *           CRTDLL__strnextc	 (CRTDLL.290)
 */
unsigned int CDECL CRTDLL__strnextc( const char *p )
{
    dprintf(("CRTDLL: _strnextc\n"));
    return( (unsigned int)*p );
}


/*********************************************************************
 *           CRTDLL__strninc	 (CRTDLL.292)
 */
char * CDECL CRTDLL__strninc( const char *p, size_t l )
{
    dprintf(("CRTDLL: _strninc\n"));
    return( (char *)(p+l) );
}


/*********************************************************************
 *           CRTDLL__strnset	 (CRTDLL.293)
 */
char * CDECL CRTDLL__strnset( char *string, int c, size_t len )
{
  dprintf(("CRTDLL: _strnset\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__strrev	 (CRTDLL.294)
 */
char * CDECL CRTDLL__strrev( char *string )
{
  dprintf(("CRTDLL: _strrev\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__strset	 (CRTDLL.295)
 */
char * CDECL CRTDLL__strset( char *string, int c )
{
  dprintf(("CRTDLL: _strset\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__strspnp	 (CRTDLL.296)
 */
char * CDECL CRTDLL__strspnp( const char *p1, const char *p2 ) 
{
    dprintf(("CRTDLL: _strspnp\n"));
    return( (*(p1 += strspn(p1,p2))!='\0') ? (char*)p1 : NULL );
}


/*********************************************************************
 *           CRTDLL__strtime	 (CRTDLL.297)
 */
char * CDECL CRTDLL__strtime( char *buf )
{
  dprintf(("CRTDLL: _strtime\n"));
  return (_strtime(buf));
}


/*********************************************************************
 *           CRTDLL__tempnam	 (CRTDLL.303)
 */
char * CDECL CRTDLL__tempnam( char *dir, char *prefix )
{
  dprintf(("CRTDLL: _tempnam\n"));
  return (_tempnam(dir, prefix));
}
	

/*********************************************************************
 *           CRTDLL__tolower	 (CRTDLL.305)
 */
int CDECL CRTDLL__tolower(int n)
{
  dprintf(("CRTDLL: _tolower\n"));
  return (_tolower(n));
}


/*********************************************************************
 *           CRTDLL__toupper	 (CRTDLL.306)
 */
int CDECL CRTDLL__toupper(int n)
{
  dprintf(("CRTDLL: _toupper\n"));
  return (_toupper(n));
}


/*********************************************************************
 *           CRTDLL__utime	 (CRTDLL.314)
 */
int CDECL CRTDLL__utime( char *path, struct utimbuf * times )
{
  dprintf(("CRTDLL: _utime\n"));
  return (_utime(path, times));
}


/*********************************************************************
 *           CRTDLL__vsnwprintf	 (CRTDLL.316)
 */
int CDECL CRTDLL__vsnwprintf( wchar_t *s1, size_t n, const wchar_t *s2, va_list arg )
{
  dprintf(("CRTDLL: _vsnwprintf\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__wcsdup	 (CRTDLL.317)
 */
wchar_t * CDECL CRTDLL__wcsdup( const wchar_t *s1 )
{
  dprintf(("CRTDLL: _wcsdup\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__wcsicoll	 (CRTDLL.319)
 */
int CDECL CRTDLL__wcsicoll( const wchar_t *s1, const wchar_t *s2 )
{
  dprintf(("CRTDLL: _wcsicoll\n"));
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
 *           CRTDLL__wcsrev	 (CRTDLL.323)
 */
wchar_t * CDECL CRTDLL__wcsrev( wchar_t *s1 )
{
  dprintf(("CRTDLL: _wcsrev\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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


