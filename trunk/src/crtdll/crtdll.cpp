/* $Id: crtdll.cpp,v 1.20 2000-01-08 12:07:45 sandervl Exp $ */

/*
 * The C RunTime DLL
 * 
 * Implements C run-time functionality as known from UNIX.
 *
 * TODO: Check setjmp(3) 
 *
 * Partialy based on Wine and ReactOS
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
#include <ntddk.h>
#include <debugtools.h>

#include <wchar.h>
#include <wctype.h>
#include <math.h>
#include <libc\locale.h>
#include <signal.h>
#include <io.h>
#include <assert.h>
#include <process.h>
#include <float.h>
#include <conio.h>
#include <direct.h>
#include <malloc.h>
#include <drive.h>
#include <fcntl.h>
#include <search.h>
#include <heap.h>
#include <errno.h>
#include <sys\utime.h>
#include <sys\stat.h>

#include <crtdll.h>
#include "crtinc.h"
#include "ieee.h"

DEFAULT_DEBUG_CHANNEL(crtdll)

//SvL: per process heap for CRTDLL
HANDLE CRTDLL_hHeap = 0;


/*********************************************************************
 *                  CRTDLL_MainInit  (CRTDLL.init)
 */
BOOL WINAPI CRTDLL_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		CRTDLL__fdopen(0,"r");
		CRTDLL__fdopen(1,"w");
		CRTDLL__fdopen(2,"w");
	     	CRTDLL_hHeap = HeapCreate(0, 0x10000, 0);
	}
	else 
	if (fdwReason == DLL_PROCESS_DETACH) {
        	HeapDestroy(CRTDLL_hHeap);
        	CRTDLL_hHeap = 0;
	}	
	return TRUE;
}


/*********************************************************************
 *                  new           (CRTDLL.001)
 */
VOID* CDECL CRTDLL_new(DWORD size)
{
    dprintf2(("CRTDLL: ??2@YAPAXI@Z\n"));
    VOID* result;
    if(!(result = Heap_Alloc(size)) && new_handler)
	(*new_handler)();
    return result;
}


/*********************************************************************
 *                  delete       (CRTDLL.002)
 */
VOID CDECL CRTDLL_delete(VOID* ptr)
{
    dprintf2(("CRTDLL: ??3@YAXPAX@Z\n"));
    Heap_Free(ptr);
}


/*********************************************************************
 *                  set_new_handler(CRTDLL.003)
 */
new_handler_type CDECL CRTDLL_set_new_handler(new_handler_type func)
{
    dprintf2(("CRTDLL: ?_set_new_handler@@YAP6AHI@ZP6AHI@Z@Z\n"));
    new_handler_type old_handler = new_handler;
    new_handler = func;
    return old_handler;
}


/*********************************************************************
 *                  _CIacos    (CRTDLL.004)
 */
double CDECL CRTDLL__CIacos(double x)
{
  dprintf2(("CRTDLL: _CIacos\n"));
  dprintf2(("should be register function\n"));
  return acos(x);
}


/*********************************************************************
 *                  _CIasin    (CRTDLL.005)
 */
double CDECL CRTDLL__CIasin( double x )
{
  dprintf2(("CRTDLL: _CIasin\n"));
  dprintf2(("should be register function\n"));
  return asin(x);
}


/*********************************************************************
 *                  _CIatan    (CRTDLL.006)
 */
double CDECL CRTDLL__CIatan( double x )
{
  dprintf2(("CRTDLL: _CIatan\n"));
  dprintf2(("should be register function\n"));
  return atan(x);
}


/*********************************************************************
 *                  _CIatan2    (CRTDLL.007)
 */
double CDECL CRTDLL__CIatan2( double x, double y )
{
  dprintf2(("CRTDLL: _CIatan2\n"));
  dprintf2(("should be register function\n"));
  return atan2(x,y);
}


/*********************************************************************
 *                  _CIcos    (CRTDLL.008)
 */
double CDECL CRTDLL__CIcos( double x )
{
  dprintf2(("CRTDLL: _CIcos\n"));
  dprintf2(("should be register function\n"));
  return cos(x);
}


/*********************************************************************
 *                  _CIcosh    (CRTDLL.009)
 */
double CDECL CRTDLL__CIcosh( double x )
{
  dprintf2(("CRTDLL: _CIcosh\n"));
  dprintf2(("should be register function\n"));
  return cosh(x);
}


/*********************************************************************
 *                  _CIexp    (CRTDLL.010)
 */
double CDECL CRTDLL__CIexp( double x )
{
  dprintf2(("CRTDLL: _CIexp\n"));
  dprintf2(("should be register function\n"));
  return exp(x);
}


/*********************************************************************
 *                  _CIfmod     (CRTDLL.011)
 */
double CDECL CRTDLL__CIfmod( double x, double y )
{
  dprintf2(("CRTDLL: _CIfmod\n"));
  dprintf2(("should be register function\n"));
  return fmod(x,y);
}


/*********************************************************************
 *                  _CIlog    (CRTDLL.012)
 */
double CDECL CRTDLL__CIlog( double x )
{
  dprintf2(("CRTDLL: _CIlog\n"));
  dprintf2(("should be register function\n"));
  return log(x);
}


/*********************************************************************
 *                  _CIlog10    (CRTDLL.013)
 */
double CDECL CRTDLL__CIlog10( double x )
{
  dprintf2(("CRTDLL: _CIlog10\n"));
  dprintf2(("should be register function\n"));
  return log10(x);
}


/*********************************************************************
 *                  _CIsin     (CRTDLL.015)
 */
double CDECL CRTDLL__CIsin( double x )
{
  dprintf2(("CRTDLL: _CIsin\n"));
  dprintf2(("should be register function\n"));
  return sin(x);
}


/*********************************************************************
 *                  _CIsinh    (CRTDLL.016)
 */
double CDECL CRTDLL__CIsinh( double x )
{
  dprintf2(("CRTDLL: _CIsinh\n"));
  dprintf2(("should be register function\n"));
  return sinh(x);
}


/*********************************************************************
 *                  _CIsqrt    (CRTDLL.017)
 */
double CDECL CRTDLL__CIsqrt( double x )
{
  dprintf2(("CRTDLL: _CIsqrt\n"));
  dprintf2(("should be register function\n"));
  return acos(x);
}


/*********************************************************************
 *                  _CItan    (CRTDLL.018)
 */
double CDECL CRTDLL__CItan( double x )
{
  dprintf2(("CRTDLL: _CItan\n"));
  dprintf2(("should be register function\n"));
  return tan(x);
}


/*********************************************************************
 *                  _CItanh    (CRTDLL.019)
 */
double CDECL CRTDLL__CItanh( double x )
{
  dprintf2(("CRTDLL: _CItanh\n"));
  dprintf2(("should be register function\n"));
  return tanh(x);
}


/*********************************************************************
 *                  _XcptFilter    (CRTDLL.21)
 */
INT CDECL CRTDLL__XcptFilter(DWORD ret, struct _EXCEPTION_POINTERS *  ExceptionInfo )
{
  dprintf2(("CRTDLL: _XcptFilter\n"));
  return UnhandledExceptionFilter(ExceptionInfo);
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

	dprintf2(("CRTDLL: GetMainArgs\n"));

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

	i=0;xargv=NULL;xargc=0;afterlastspace=0;
/*
	while (cmdline[i]) {
		if (cmdline[i]==' ') {
			xargv=(char**)HeapReAlloc( GetProcessHeap(), 0, xargv,
                                                   sizeof(char*)*(++xargc));
			cmdline[i]='\0';
			xargv[xargc-1] = HEAP_strdupA( GetProcessHeap(), 0,
                                                       cmdline+afterlastspace);
			i++;
			while (cmdline[i]==' ')
				i++;
			if (cmdline[i])
				afterlastspace=i;

		} else
			i++;

	}

	xargv=(char**)HeapReAlloc( GetProcessHeap(), 0, xargv,
                                   sizeof(char*)*(++xargc));
	cmdline[i]='\0';
	xargv[xargc-1] = HEAP_strdupA( GetProcessHeap(), 0,
                                       cmdline+afterlastspace);
*/
	CRTDLL_argc_dll	= xargc;
	*argc		= xargc;
	CRTDLL_argv_dll	= xargv;
	*argv		= xargv;
	CRTDLL_environ_dll = *environ = GetEnvironmentStringsA();
	dprintf2(("CRTDLL: GetMainArgs end\n"));
	return 0;
}


/*********************************************************************
 *                  __dllonexit           (CRTDLL.25)
 */
VOID CDECL CRTDLL___dllonexit ()
{	
	dprintf(("__dllonexit not implemented.\n"));
}


/*********************************************************************
 *                  __doserrno            (CRTDLL.26)
 */
int * CDECL CRTDLL___doserrno()
{	
	dprintf2(("__doserrno\n"));
	_doserrno = GetLastError();
	return &_doserrno;
}


/*********************************************************************
 *                  __fpecode            (CRTDLL.27)
 */
int fpecode = 0;

int * CDECL CRTDLL___fpecode ( void )
{	
	dprintf2(("__fpecode\n"));
        return &fpecode;
}


/*********************************************************************
 *           CRTDLL___isascii   (CRTDLL.28)
 */
int CDECL CRTDLL___isascii(int i)
{
  dprintf2(("CRTDLL: __isascii\n"));
  return  (!((i)&(~0x7f)));
}


/*********************************************************************
 *           CRTDLL___iscsym   (CRTDLL.29)
 */
int CDECL CRTDLL___iscsym(int c)
{
  dprintf2(("CRTDLL: __iscsym\n"));
  return (CRTDLL_isalnum(c) || ( c == '_' )) ;
}


/*********************************************************************
 *           CRTDLL___iscsymf   (CRTDLL.30)
 */
int CDECL CRTDLL___iscsymf(int c)
{
  dprintf2(("CRTDLL: __iscsymf\n"));
  return (isalpha(c) || ( c == '_' )) ;
}


/*********************************************************************
 *           CRTDLL___pxcptinfoptrs   (CRTDLL.32)
 */
void ** CDECL CRTDLL___pxcptinfoptrs (void)
{
  dprintf(("CRTDLL: __pxcptinfoptrs not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}


/*********************************************************************
 *           CRTDLL___threadhandle   (CRTDLL.33)
 */
unsigned long CDECL CRTDLL___threadhandle( void )
{
  dprintf2(("CRTDLL: __threadhandle\n"));
  return GetCurrentThread();
}


/*********************************************************************
 *           CRTDLL___threadid   (CRTDLL.34)
 */
unsigned long CDECL CRTDLL___threadid(void)
{
  dprintf2(("CRTDLL: __threadid\n"));
  return GetCurrentThreadId();
}


/*********************************************************************
 *           CRTDLL__abnormal_termination   (CRTDLL.36)
 */
int CDECL CRTDLL__abnormal_termination(void)
{
  dprintf(("CRTDLL: _abnormal_termination  not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__access   (CRTDLL.37)
 */
int CDECL CRTDLL__access(const char *path,int mode)
{
  dprintf2(("CRTDLL: _access\n"));
  return (_access(path, mode));
}


/*********************************************************************
 *           CRTDLL___toascii   (CRTDLL.38)
 */
int CDECL CRTDLL___toascii(int c)
{
  dprintf2(("CRTDLL: __toascii\n"));
  return ((unsigned)(c) & 0x7F );
}


/*********************************************************************
 *                  _aexit_rtn_dll    (CRTDLL.39)
 */
VOID CDECL CRTDLL__aexit_rtn_dll(int exitcode)
{
  dprintf2(("CRTDLL: _aexit_rtn_dll\n"));
  ExitProcess(exitcode);
}


/*********************************************************************
 *                  _amsg_exit    (CRTDLL.40)
 */
VOID CDECL CRTDLL__amsg_exit(int errnum)
{
  dprintf2(("CRTDLL: _amsg_exit\n"));
  fprintf(stderr,strerror(errnum));
  ExitProcess(-1);
}


/*********************************************************************
 *           CRTDLL__assert   (CRTDLL.41)
 */
void CDECL CRTDLL__assert( char *s1, char *s2, int i)
{
  dprintf2(("CRTDLL: _assert\n"));
  _assert(s1, s2, i);
}


/*********************************************************************
 *                  CRTDLL__beep	            (CRTDLL.45)
 */
void CDECL CRTDLL__beep(unsigned nFreq, unsigned nDur)
{	
	dprintf2(("_beep\n"));
	Beep(nFreq,nDur);
}


/*********************************************************************
 *           CRTDLL__beginthread   (CRTDLL.46)
 */
unsigned long CDECL CRTDLL__beginthread(void (*pfuncStart)(void *),
		unsigned unStackSize, void* pArgList)
{
	DWORD  ThreadId;
 	HANDLE hThread;
	if (  pfuncStart == NULL )
		__set_errno(EINVAL);

	hThread = CreateThread( NULL,unStackSize,(LPTHREAD_START_ROUTINE)pfuncStart,pArgList,0, &ThreadId);
	if (hThread == NULL ) {
		__set_errno(EAGAIN);
		return -1;
	}
	return (unsigned long)hThread;
}


/*********************************************************************
 *                  _c_exit          (CRTDLL.47)
 *
 */
void CDECL CRTDLL__c_exit(INT ret)
{
        dprintf2(("_c_exit(%d)\n",ret));
	ExitProcess(ret);
}


/*********************************************************************
 *           CRTDLL__cabs   (CRTDLL.48)
 */
double CDECL CRTDLL__cabs(struct _complex z)
{
  dprintf2(("CRTDLL: _cabs\n"));
  return sqrt( z.x*z.x + z.y*z.y );
}


/*********************************************************************
 *                  _cexit          (CRTDLL.49)
 */
void CDECL CRTDLL__cexit(INT ret)
{
        dprintf2(("_cexit(%d)\n",ret));
	ExitProcess(ret);
}


/*********************************************************************
 *           CRTDLL__cgets  (CRTDLL.50)
 */
char * CDECL CRTDLL__cgets( char *s )
{
  dprintf2(("CRTDLL: _cgets\n"));
  return (_cgets(s));
}


/*********************************************************************
 *                  _chdir           (CRTDLL.51)
 */
INT CDECL CRTDLL__chdir(LPCSTR newdir)
{
	dprintf2(("CRTDLL: chdir\n"));
	if (!SetCurrentDirectoryA(newdir))
		return 1;
	return 0;
}


/*********************************************************************
 *                  _chdrive           (CRTDLL.52)
 *
 *  newdir      [I] drive to change to, A=1
 *
 */
BOOL CDECL CRTDLL__chdrive(INT newdrive)
{
	/* FIXME: generates errnos */
	dprintf2(("CRTDLL: _chdrive\n"));
	return DRIVE_SetCurrentDrive(newdrive-1);
}


/*********************************************************************
 *           CRTDLL__chgsign	 (CRTDLL.53)
 */
double CDECL CRTDLL__chgsign(double __x)
{
  dprintf2(("CRTDLL: _chgsign\n"));
  double_t *x = (double_t *)&x;
  if ( x->sign == 1 )
	x->sign = 0;
  else 
	x->sign = 1;

  return __x;
}


/*********************************************************************
 *           CRTDLL__chmod   (CRTDLL.54)
 */
int CDECL CRTDLL__chmod( const char *s, int i)
{
  dprintf2(("CRTDLL: _chmod\n"));
  return (_chmod(s, i));
}


/*********************************************************************
 *           CRTDLL__chsize   (CRTDLL.55)
 */
int CDECL CRTDLL__chsize( int i, long l )
{
  dprintf2(("CRTDLL: _chsize\n"));
  return (_chsize(i, l));
}


/*********************************************************************
 *           CRTDLL__clearfp  (CRTDLL.56)
 */
unsigned int CDECL CRTDLL__clearfp( void )
{
  dprintf2(("CRTDLL: _clearfp\n"));
  return (_clear87());
}


/*********************************************************************
 *           CRTDLL__close   (CRTDLL.57)
 */
int CDECL CRTDLL__close(int handle)
{
  dprintf2(("CRTDLL: _close\n"));
  return (_close(handle));
}


/*********************************************************************
 *                  CRTDLL__commit    (CRTDLL.58)
 */
int CDECL CRTDLL__commit( int _fd )
{
  dprintf2(("CRTDLL: _commit\n"));
  if (! FlushFileBuffers((HFILE)CRTDLL__get_osfhandle(_fd)) ) {
	__set_errno(EBADF);
	return -1;
  }
  return 0;
}


/*********************************************************************
 *           CRTDLL__control87   (CRTDLL.60)
 */
unsigned CDECL CRTDLL__control87(unsigned i1,unsigned i2)
{
  dprintf2(("CRTDLL: _control87\n"));
  return (_control87(i1, i2));
}


/*********************************************************************
 *                  CRTDLL__controlfp    (CRTDLL.61)
 */
unsigned CDECL CRTDLL__controlfp(unsigned i1,unsigned i2)
{
  dprintf2(("CRTDLL: _controlfp\n"));
  return (_control87(i1, i2));
}


/*********************************************************************
 *                  CRTDLL__copysign    (CRTDLL.62)
 */
double CDECL CRTDLL__copysign( double __d, double __s )
{
  dprintf2(("CRTDLL: _copysign\n"));
  double_t *d = (double_t *)&__d;
  double_t *s = (double_t *)&__s;

  d->sign = s->sign;

  return __d;
}


/*********************************************************************
 *                  _cprintf    (CRTDLL.63)
 */
INT CDECL CRTDLL__cprintf( char *fmt, ... )
{
  dprintf2(("CRTDLL: _cprintf.\n"));

  int     cnt;
  char    buf[ 2048 ];		/* this is buggy, because buffer might be too small. */
  va_list ap;
  
  va_start(ap, fmt);
  cnt = vsprintf(buf, fmt, ap);
  va_end(ap);
  
  _cputs(buf);
  return cnt;
}


/*********************************************************************
 *                  CRTDLL__cputs      (CRTDLL.65)
 */
INT CDECL CRTDLL__cputs( char * s )
{
  dprintf2(("CRTDLL: _cputs\n"));
  return (_cputs(s));
}


/*********************************************************************
 *                  CRTDLL__creat      (CRTDLL.66)
 */
INT CDECL CRTDLL__creat( const char *s, int i )
{
  dprintf2(("CRTDLL: _creat\n"));
  return (_creat(s, i));
}


/*********************************************************************
 *                  CRTDLL__cscanf    (CRTDLL.67)
 */
INT CDECL CRTDLL__cscanf( char *s, ... )
{
  dprintf(("CRTDLL: _cscanf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__cwait   (CRTDLL.69)
 */
int CDECL CRTDLL__cwait( int *status, int process_id, int action_code )
{
  dprintf2(("CRTDLL: _cwait\n"));
  return (_cwait(status, process_id, action_code));
}


/*********************************************************************
 *           CRTDLL__dup   (CRTDLL.71)
 */
int CDECL CRTDLL__dup(int handle)
{
  dprintf2(("CRTDLL: _dup\n"));
  return (_dup(handle));
}


/*********************************************************************
 *           CRTDLL__dup2  (CRTDLL.72)
 */
int CDECL CRTDLL__dup2(int handle1,int handle2)
{
  dprintf2(("CRTDLL: _dup2\n"));
  return (_dup2(handle1, handle2));
}


/*********************************************************************
 *           CRTDLL__ecvt  (CRTDLL.73)
 */
char * CDECL CRTDLL__ecvt( double val, int ndig, int *dec, int *sign )
{
  dprintf2(("CRTDLL: _ecvt\n"));
  return (_ecvt(val, ndig, dec, sign));
}


/*********************************************************************
 *           CRTDLL__endthread  (CRTDLL.74)
 */
void CDECL CRTDLL__endthread(void)
{
  dprintf2(("CRTDLL: _endthread\n"));
  _endthread ();
}


/*********************************************************************
 *           CRTDLL___eof   (CRTDLL.76)
 */
int CDECL CRTDLL__eof( int _fd )
{
  dprintf2(("CRTDLL: _eof\n"));
  int cur_pos = CRTDLL__lseek(_fd, 0, SEEK_CUR);
  int end_pos = CRTDLL__filelength( _fd );
  if ( cur_pos == -1 || end_pos == -1)
	return -1;

  if ( cur_pos == end_pos )
	return 1;

  return 0;
}


/*********************************************************************
 *           CRTDLL__errno  (CRTDLL.77)
 */
int * CDECL CRTDLL__errno(void)
{
  dprintf2(("CRTDLL: _errno\n"));
  return (_errno());
}


/*********************************************************************
 *                  _except_handler2  (CRTDLL.78)
 */
INT CDECL CRTDLL__except_handler2 ( PEXCEPTION_RECORD rec,
	PEXCEPTION_FRAME frame, PCONTEXT context, 
	PEXCEPTION_FRAME  *dispatcher)
{
        dprintf2(("CRTDLL: _except_handler2\n"));
	return 1;
}


/*********************************************************************
 *           _execl   (CRTDLL.79)
 */
int CDECL CRTDLL__execl(const char* szPath, const char* szArgv0, ...)
{
  dprintf2(("CRTDLL: _execl\n"));

  char *szArg[100];
  const char *a;
  int i = 0;
  va_list l = 0;
  va_start(l,szArgv0);
  do {
  	a = va_arg(l,const char *);
	szArg[i++] = (char *)a;
  } while ( a != NULL && i < 100 );

  return _spawnve(P_OVERLAY, (char*)szPath, szArg, _environ);
}


/*********************************************************************
 *           CRTDLL__execle   (CRTDLL.80)
 */
int CDECL CRTDLL__execle(char *path, char *szArgv0, ...)
{
  dprintf2(("CRTDLL: _execle not correct implemented.\n"));
  char *szArg[100];
  const char *a;
  char *ptr;
  int i = 0;
  va_list l = 0;
  va_start(l,szArgv0);
  do {
  	a = (const char *)va_arg(l,const char *);
	szArg[i++] = (char *)a;
  } while ( a != NULL && i < 100 );


// szArg0 is passed and not environment if there is only one parameter;

  if ( i >=2 ) {
  	ptr = szArg[i-2];
  	szArg[i-2] = NULL;
  }
  else
	ptr = NULL;

  return _spawnve(P_OVERLAY, path, szArg, (char**)ptr);
}


/*********************************************************************
 *           CRTDLL__execlp   (CRTDLL.81)
 */
int CDECL CRTDLL__execlp( char *szPath, char *szArgv0, ...)
{
  dprintf2(("CRTDLL: _execlp\n"));
  char *szArg[100];
  const char *a;
  int i = 0;
  va_list l = 0;
  va_start(l,szArgv0);
  do {
  	a = (const char *)va_arg(l,const char *);
	szArg[i++] = (char *)a;
  } while ( a != NULL && i < 100 );
  return _spawnvpe(P_OVERLAY, szPath,szArg, _environ);
}


/*********************************************************************
 *           CRTDLL__execlpe   (CRTDLL.82)
 */
int CDECL CRTDLL__execlpe( char *path, char *szArgv0, ...)
{
  dprintf2(("CRTDLL: _execlpe not correct implemented.\n"));
  char *szArg[100];
  const char *a;
  char *ptr;
  int i = 0;
  va_list l = 0;
  va_start(l,szArgv0);
  do {
  	a = (const char *)va_arg(l,const char *);
	szArg[i++] = (char *)a;
  } while ( a != NULL && i < 100 );


// szArg0 is passed and not environment if there is only one parameter;

  if ( i >=2 ) {
  	ptr = szArg[i-2];
  	szArg[i-2] = NULL;
  }
  else
	ptr = NULL;
  return spawnvpe(P_OVERLAY, path, szArg, (char**)ptr);
}


/*********************************************************************
 *           CRTDLL__execv   (CRTDLL.83)
 */
int CDECL CRTDLL__execv( char *s1, char **s2)
{
  dprintf2(("CRTDLL: _execv\n"));
  return (_execv(s1, s2));
}


/*********************************************************************
 *           CRTDLL__execve   (CRTDLL.84)
 */
int CDECL CRTDLL__execve( char *s1, char **s2, char **s3)
{
  dprintf2(("CRTDLL: _execve\n"));
  return (_execve(s1, s2, s3));
}


/*********************************************************************
 *           CRTDLL__execvp   (CRTDLL.85)
 */
int CDECL CRTDLL__execvp( char *s1, char **s2)
{
  dprintf2(("CRTDLL: _execvp\n"));
  return (_execvp(s1, s2));
}


/*********************************************************************
 *           CRTDLL__execvpe   (CRTDLL.86)
 */
int CDECL CRTDLL__execvpe( char *s1, char **s2, char **s3)
{
  dprintf2(("CRTDLL: _execvpe\n"));
  return (_execvpe(s1, s2, s3));
}


/*********************************************************************
 *                  _exit          (CRTDLL.87)
 */
VOID CDECL CRTDLL__exit(DWORD ret)
{
	dprintf2(("CRTDLL: _exit\n"));
	ExitProcess(ret);
}


/*********************************************************************
 *           _expand   (CRTDLL.88)
 */
void * CDECL CRTDLL__expand( void *ptr, size_t size )
{
  dprintf(("CRTDLL: _expand not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _fcloseall   (CRTDLL.89)
 */
int CDECL CRTDLL__fcloseall( void )
{
  dprintf2(("CRTDLL: _fcloseall\n"));
  return (_fcloseall());
}


/*********************************************************************
 *           _fcvt  (CRTDLL.90)
 */
char * CDECL CRTDLL__fcvt( double val, int ndig, int *dec, int *sign )
{
  dprintf2(("CRTDLL: _fcvt\n"));
  return (_fcvt(val, ndig, dec, sign));
}


/*********************************************************************
 *                  _fdopen     (CRTDLL.91)
 */
CRTDLL_FILE * CDECL CRTDLL__fdopen(INT handle, LPCSTR mode)
{
    dprintf2(("CRTDLL: fdopen\n"));
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
        file = (PCRTDLL_FILE)Heap_Alloc(sizeof(*file) );
        file->handle = handle;
        break;
    }
  return file;
}


/*********************************************************************
 *           _fgetchar  (CRTDLL.92)
 */
int CDECL CRTDLL__fgetchar( void )
{
  dprintf2(("CRTDLL: _fgetchar\n"));
  return (_fgetchar());
}


/*********************************************************************
 *           _fgetwchar  (CRTDLL.93)
 */
wint_t CDECL CRTDLL__fgetwchar( void *i )
{
  dprintf2(("CRTDLL: _fgetwchar\n"));
  return CRTDLL__getch();
}


/*********************************************************************
 *           _filbuf     (CRTDLL.94)
 */
int CDECL CRTDLL__filbuf(FILE * f)
{
  dprintf(("CRTDLL: _filbuf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__filelength     (CRTDLL.96)
 */
long CDECL CRTDLL__filelength( int i )
{
  dprintf2(("CRTDLL: _filelength\n"));
  return (_filelength(i));
}


/*********************************************************************
 *           _fileno     (CRTDLL.97)
 */
int CDECL CRTDLL__fileno(FILE * f)
{
  dprintf2(("CRTDLL: _fileno\n"));
  return (_fileno(f));
}


/*********************************************************************
*                  _findclose    (CRTDLL.098)
*/
int CDECL CRTDLL__findclose( long handle )
{
  dprintf2(("CRTDLL: _findclose\n"));
  // check no wildcards or invalid handle
  if ( handle == 0 || handle == -1)
	return 0;
  return FindClose(handle);
}


 /*********************************************************************
 *                  _findfirst    (CRTDLL.099)
 */
DWORD CDECL CRTDLL__findfirst(LPCSTR fname,  struct find_t * x2)
{
  dprintf(("CRTDLL: _findfirst not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _findnext     (CRTDLL.100)
 */
INT CDECL CRTDLL__findnext(DWORD hand, struct find_t * x2)
{
  dprintf(("CRTDLL: _findnext not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _finite     (CRTDLL.101)
 */
INT CDECL CRTDLL__finite(double x)
{
  dprintf2(("CRTDLL: _finite\n"));
  return !_isinf(x);
}


/*********************************************************************
 *                  _flsbuf     (CRTDLL.102)
 */
INT CDECL CRTDLL__flsbuf(int i, FILE * f)
{
  dprintf(("CRTDLL: _flsbuf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _flushall     (CRTDLL.103)
 */
INT CDECL CRTDLL__flushall(void)
{
  dprintf2(("CRTDLL: _flushall\n"));
  return (_flushall());
}


/*********************************************************************
 *                  _fpclass     (CRTDLL.105)
 */
INT CDECL CRTDLL__fpclass( double __d )
{
  dprintf2(("CRTDLL: _fpclass\n"));
	double_t *d = (double_t *)&__d;

	if ( d->exponent == 0 ) {
		if ( d->mantissah == 0 &&  d->mantissal == 0 ) {
			if ( d->sign ==0 )
				return FP_NZERO;
			else
				return FP_PZERO;
		} else {
			if ( d->sign ==0 )
				return FP_NDENORM;
			else
				return FP_PDENORM;
		}
	}
	if (d->exponent == 0x7ff ) {
		if ( d->mantissah == 0 &&  d->mantissal == 0 ) {
			if ( d->sign ==0 )
				return FP_NINF;
			else
				return FP_PINF;
		} 
		else if ( d->mantissah == 0 &&  d->mantissal != 0 ) {
			return FP_QNAN;
		}
		else if ( d->mantissah == 0 &&  d->mantissal != 0 ) {
			return FP_SNAN;
		}
	
	}

	return 0;
}


/*********************************************************************
 *                  _fpieee_flt     (CRTDLL.106)
 */
INT CDECL CRTDLL__fpieee_flt( unsigned long exc_code, struct _EXCEPTION_POINTERS *exc_info, int handler)
{
  dprintf(("CRTDLL: _fpieee_flt not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}



/*********************************************************************
 *                  _fpreset     (CRTDLL.107)
 */
void CDECL CRTDLL__fpreset(void)
{
  dprintf(("CRTDLL: _fpreset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _fputchar     (CRTDLL.108)
 */
INT CDECL CRTDLL__fputchar( int c )
{
  dprintf2(("CRTDLL: _fputchar\n"));
  return(_fputchar(c));
}


/*********************************************************************
 *                  _fputwchar     (CRTDLL.109)
 */
wint_t CDECL CRTDLL__fputwchar( wint_t )
{
  dprintf(("CRTDLL: _fputwchar not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fsopen     (CRTDLL.110)
 */
FILE * CDECL CRTDLL__fsopen( const char *file, const char *mode, int shflag )
{
  dprintf(("CRTDLL: _fsopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fstat        (CRTDLL.111)
 */
int CDECL CRTDLL__fstat(int file, struct stat* buf)
{
  dprintf(("CRTDLL: _fstat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ftime        (CRTDLL.112)
 */
int CDECL CRTDLL__ftime( struct timeb *timebuf )
{
  dprintf(("CRTDLL: _ftime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fullpath     (CRTDLL.114)
 */
char * CDECL CRTDLL__fullpath( char *buf, char *path, size_t size )
{
  dprintf2(("CRTDLL: _fullpath\n"));
  return (_fullpath(buf, path, size));
}


/*********************************************************************
 *                  _futime        (CRTDLL.115)
 */
int CDECL CRTDLL__futime( int handle, struct _utimbuf *filetime )
{
  dprintf(("CRTDLL: _futime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _gcvt     (CRTDLL.116)
 */
char * CDECL CRTDLL__gcvt( double val, int ndig, char *buf )
{
  dprintf2(("CRTDLL: _gcvt\n"));
  return (_gcvt(val, ndig, buf));
}


/*********************************************************************
 *                  _get_osfhandle     (CRTDLL.117)
 */
void* CDECL CRTDLL__get_osfhandle( int fileno )
{
  dprintf2(("CRTDLL: _get_osfhandle\n"));
  return filehnd(fileno);
}


/*********************************************************************
 *                  _getch     (CRTDLL.118)
 */
int CDECL CRTDLL__getch(void)
{
  dprintf2(("CRTDLL: _getch\n"));
  return (_getch());
}


/*********************************************************************
 *                  _getche     (CRTDLL.119)
 */
int CDECL CRTDLL__getche(void)
{
  dprintf2(("CRTDLL: _getche\n"));
  return (_getche());
}


/*********************************************************************
 *                  _getcwd     (CRTDLL.120)
 */
char * CDECL CRTDLL__getcwd( char *buf, size_t size )
{
  dprintf2(("CRTDLL: _getcwd\n"));
  return (_getcwd(buf, size));
}


/*********************************************************************
 *                  _getdcwd     (CRTDLL.121)
 */
char * CDECL CRTDLL__getdcwd( int drive, char *buffer, size_t maxlen )
{
  dprintf2(("CRTDLL: _getdcwd\n"));
  return (_getdcwd(drive, buffer, maxlen));
}


/*********************************************************************
 *                  _getdiskfree     (CRTDLL.122)
 */
unsigned int CDECL CRTDLL__getdiskfree( unsigned int drive, struct _diskfree_t *diskspace)
{
  dprintf2(("CRTDLL: _getdiskfree\n"));
  char RootPathName[10];
  RootPathName[0] = toupper(drive +'@');
  RootPathName[1] = ':';
  RootPathName[2] = '\\';
  RootPathName[3] = 0;
  if ( diskspace == NULL )
	return 0;

  if ( !GetDiskFreeSpaceA(RootPathName,(LPDWORD)&diskspace->sectors_per_cluster,(LPDWORD)&diskspace->bytes_per_sector,
		(LPDWORD )&diskspace->avail_clusters,(LPDWORD )&diskspace->total_clusters ) )
	return 0;
  return diskspace->avail_clusters;
}


/*********************************************************************
 *                  _getdllprocaddr     (CRTDLL.123)
 */
FARPROC CDECL  CRTDLL__getdllprocaddr(HMODULE hModule,char * lpProcName, int iOrdinal)
{
  dprintf2(("CRTDLL: _getdllprocaddr\n"));   
  if ( lpProcName != NULL ) 
	return GetProcAddress(hModule, lpProcName);
  else
	return GetProcAddress(hModule, (LPSTR)iOrdinal);
  return (NULL);
}


/*********************************************************************
 *                  _getdrive    (CRTDLL.124)
 */
unsigned CDECL CRTDLL__getdrive( void )
{
  dprintf2(("CRTDLL: _getdrive\n"));
  return DRIVE_GetCurrentDrive() + 1;
}


/*********************************************************************
 *                  _getdrives    (CRTDLL.125)
 */
unsigned long CDECL CRTDLL__getdrives(void)
{
  dprintf2(("CRTDLL: _getdrives\n"));
  return GetLogicalDrives();
}


/*********************************************************************
 *                  _getpid    (CRTDLL.126)
 */
int CDECL CRTDLL__getpid( void )
{
  dprintf2(("CRTDLL: _getpid\n"));
  return (_getpid());
}


/*********************************************************************
 *                  _getsystime    (CRTDLL.127)
 */
unsigned int CDECL CRTDLL__getsystime(struct tm *tp)
{
  dprintf(("CRTDLL: _getsystime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getw     (CRTDLL.128)
 */
int CDECL CRTDLL__getw( FILE *stream )
{
  dprintf2(("CRTDLL: _getw\n"));
  int w;

  /* Is there a better way?  */
  if (CRTDLL_fread( &w, sizeof(w), 1, stream) != 1)
    return(EOF);
  return(w);
}


/*******************************************************************
 *         _global_unwind2  (CRTDLL.129)
 */
void CDECL CRTDLL__global_unwind2( PEXCEPTION_FRAME frame )
{
    dprintf2(("CRTDLL: global_undwind2\n"));
    RtlUnwind( frame, 0, NULL, 0 );
}


/*********************************************************************
 *                  _heapchk    (CRTDLL.130)
 */
int CDECL CRTDLL__heapchk( void )
{
  dprintf2(("CRTDLL: _heapchk\n"));
  return (_heapchk());
}


/*********************************************************************
 *                  _heapmin    (CRTDLL.131)
 */
int CDECL CRTDLL__heapmin( void )
{
  dprintf2(("CRTDLL: _heapmin\n"));
  return (_heapmin());
}


/*********************************************************************
 *                  _heapset    (CRTDLL.132)
 */
int CDECL CRTDLL__heapset( unsigned int fill )
{
  dprintf2(("CRTDLL: _heapset\n"));
  return (_heapset(fill));
}


/*********************************************************************
 *                  _heapwalk     (CRTDLL.133)
 */
int CDECL CRTDLL__heapwalk( struct _heapinfo *entry )
{
  dprintf(("CRTDLL: _heapwalk not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _hypot     (CRTDLL.134)
 */
double CDECL CRTDLL__hypot(double x1, double x2)
{
  dprintf2(("CRTDLL: _hypot\n"));
  return (_hypot(x1, x2));
}


/*********************************************************************
 *                  _initterm     (CRTDLL.135)
 */
DWORD CDECL CRTDLL__initterm(_INITTERMFUN *start,_INITTERMFUN *end)
{
	dprintf2(("CRTDLL: initterm\n"));
	_INITTERMFUN	*current;

	current=start;
	while (current<end) {
		if (*current) (*current)();
		current++;
	}
	return 0;
}


/*********************************************************************
 *                  _isatty       (CRTDLL.137)
 */
BOOL CDECL CRTDLL__isatty(DWORD x)
{
   dprintf2(("(%ld)\n",x));
   return TRUE;
}


/*********************************************************************
 *                  _isctype           (CRTDLL.138)
 */
BOOL CDECL CRTDLL__isctype(CHAR x,CHAR type)
{
	dprintf2(("CRTDLL: isctype\n"));
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
int CDECL CRTDLL__ismbbalnum( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbalnum\n"));
  return (CRTDLL_isalnum(c) || CRTDLL__ismbbkalnum(c));
}


/*********************************************************************
 *                  _ismbbalpha     (CRTDLL.140)
 */
int CDECL CRTDLL__ismbbalpha( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbalpha\n"));
  return (isalpha(c) || CRTDLL__ismbbkalnum(c));
}


/*********************************************************************
 *                  _ismbbgraph     (CRTDLL.141)
 */
int CDECL CRTDLL__ismbbgraph( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbgraph\n"));
  return (CRTDLL_isgraph(c) || CRTDLL__ismbbkana(c));
}


/*********************************************************************
 *                  _ismbbkalnum     (CRTDLL.142)
 */
int CDECL CRTDLL__ismbbkalnum( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbkalnum\n"));
  return  ((_jctype+1)[(unsigned char)(c)] & (_KNJ_P));
}


/*********************************************************************
 *                  _ismbbkana     (CRTDLL.143)
 */
int CDECL CRTDLL__ismbbkana( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbkana\n"));
  return ((_jctype+1)[(unsigned char)(c)] & (_KNJ_M|_KNJ_P));
}


/*********************************************************************
 *                  _ismbbkpunct     (CRTDLL.144)
 */
int CDECL CRTDLL__ismbbkpunct( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbkpunct\n"));
  return  ((_jctype+1)[(unsigned char)(c)] & (_KNJ_P));
}


/*********************************************************************
 *                  _ismbblead     (CRTDLL.145)
 */
int CDECL CRTDLL__ismbblead( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbblead\n"));
  return ((_jctype+1)[(unsigned char)(c)] & _KNJ_1);
}


/*********************************************************************
 *                  _ismbbprint     (CRTDLL.146)
 */
int CDECL CRTDLL__ismbbprint( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbprint\n"));
  return (isprint(c) || CRTDLL__ismbbkana(c));
}


/*********************************************************************
 *                  _ismbbpunct     (CRTDLL.147)
 */
int CDECL CRTDLL__ismbbpunct( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbpunct\n"));
  return (ispunct(c) ||  CRTDLL__ismbbkana(c));
}


/*********************************************************************
 *                  _ismbbtrail     (CRTDLL.148)
 */
int CDECL CRTDLL__ismbbtrail( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbbtrail\n"));
  return ((_jctype+1)[(unsigned char)(c)] & _KNJ_2);
}


/*********************************************************************
 *                  _ismbcalpha     (CRTDLL.149)
 */
int CDECL CRTDLL__ismbcalpha( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbcalpha\n"));
  if ((c & 0xFF00) != 0) {
	// true multibyte character
	return 0;
  }
  else
	return CRTDLL__ismbbalpha(c);

  return 0;
}


/*********************************************************************
 *                  _ismbcdigit     (CRTDLL.150)
 */
int CDECL CRTDLL__ismbcdigit( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbcdigit\n"));
  if ((c & 0xFF00) != 0) {
	// true multibyte character
	return 0;
  }
  else
	return 0;
//		return _ismbbdigit(c);

	return 0;
}


/*********************************************************************
 *                  _ismbchira     (CRTDLL.151)
 */
int CDECL CRTDLL__ismbchira( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbchira\n"));
  return ((c>=0x829F) && (c<=0x82F1));
}


/*********************************************************************
 *                  _ismbckata     (CRTDLL.152)
 */
int CDECL CRTDLL__ismbckata( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbckata\n"));
  return ((c>=0x8340) && (c<=0x8396));
}

/*********************************************************************
 *                  _ismbcl0     (CRTDLL.153)
 */
int CDECL CRTDLL__ismbcl0( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl0 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbcl1     (CRTDLL.154)
 */
int CDECL CRTDLL__ismbcl1( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl1 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbcl2     (CRTDLL.155)
 */
int CDECL CRTDLL__ismbcl2( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl2 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbclegal     (CRTDLL.156)
 */
int CDECL CRTDLL__ismbclegal( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbclegal\n"));
  if ((c & 0xFF00) != 0) {
	return CRTDLL__ismbblead(c>>8) && CRTDLL__ismbbtrail(c&0xFF);
  }
  else
	return CRTDLL__ismbbtrail(c&0xFF);

  return 0;
}


/*********************************************************************
 *                  _ismbclower     (CRTDLL.157)
 */
int CDECL CRTDLL__ismbclower( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbclower\n"));
  if ((c & 0xFF00) != 0) {
	if ( c >= 0x829A && c<= 0x829A )
		return 1;
  }
  else
    return isupper(c);
}


/*********************************************************************
 *                  _ismbcprint     (CRTDLL.158)
 */
int CDECL CRTDLL__ismbcprint( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbcprint\n"));
  if ((c & 0xFF00) != 0) {
	// true multibyte character
	return 0;
  }
  else
	return 0;
//	return _ismbbdigit(c);

  return 0;
}


/*********************************************************************
 *                  _ismbcspace     (CRTDLL.159)
 */
int CDECL CRTDLL__ismbcspace( unsigned int c )
{
  dprintf(("CRTDLL: _ismbcspace not implemented.\n"));
  if ((c & 0xFF00) != 0) {
	// true multibyte character
	return 0;
  }
  else
	return 0;
//	return _ismbbdigit(c);

  return 0;
}


/*********************************************************************
 *                  _ismbcsymbol     (CRTDLL.160)
 */
int CDECL CRTDLL__ismbcsymbol( unsigned int c )
{
  dprintf(("CRTDLL: _ismbcsymbol not implemented.\n"));
  if ((c & 0xFF00) != 0) {
	// true multibyte character
	return 0;
  }
  else
	return 0;
//	return _ismbbdigit(c);

  return 0;
}


/*********************************************************************
 *                  _ismbcupper     (CRTDLL.161)
 */
int CDECL CRTDLL__ismbcupper( unsigned int c )
{
  dprintf2(("CRTDLL: _ismbcupper\n"));
  if ((c & 0xFF00) != 0) {
	if ( c >= 0x8260 && c<= 0x8279 )
		return 1;
  }
  else
  return isupper(c);
}


/*********************************************************************
 *                  _ismbslead     (CRTDLL.162)
 */
int CDECL CRTDLL__ismbslead(const unsigned char *str, const unsigned char *t)
{
  dprintf2(("CRTDLL: _ismbslead\n"));
  unsigned char *s = (unsigned char *)str;
  while(*s != 0 && s != t) 
  {  
	s+= _mbclen2(*s);
  }		
  return CRTDLL__ismbblead( *s);
}


/*********************************************************************
 *                  _ismbstrail     (CRTDLL.163)
 */
int CDECL CRTDLL__ismbstrail(const unsigned char *str, const unsigned char *t)
{
  dprintf2(("CRTDLL: _ismbstrail\n"));
  unsigned char *s = (unsigned char *)str;
  while(*s != 0 && s != t) 
  {
		
	s+= _mbclen2(*s);
  }
		
  return CRTDLL__ismbbtrail(*s);
}


/*********************************************************************
 *                  _isnan     (CRTDLL.164)
 */
int CDECL CRTDLL__isnan( double __x )
{
  dprintf2(("CRTDLL: _isnan\n"));
  double_t * x = (double_t *)&__x;
  return ( x->exponent == 0x7ff  && ( x->mantissah != 0 || x->mantissal != 0 ));
}


/*********************************************************************
 *                  _j0     (CRTDLL.166)
 */
double CDECL CRTDLL__j0(double x)
{
  dprintf2(("CRTDLL: _j0\n"));
  return (_j0(x));
}


/*********************************************************************
 *                  _j1     (CRTDLL.167)
 */
double CDECL CRTDLL__j1(double x)
{
  dprintf2(("CRTDLL: _j1\n"));
  return (_j1(x));}


/*********************************************************************
 *                  _jn     (CRTDLL.168)
 */
double CDECL CRTDLL__jn(int i, double x)
{
  dprintf2(("CRTDLL: _jn\n"));
  return (_jn(i, x));
}


/*********************************************************************
 *                  _kbhit     (CRTDLL.169)
 */
int CDECL CRTDLL__kbhit( void )
{
  dprintf2(("CRTDLL: _kbhit\n"));
  return (_kbhit());
}


/*********************************************************************
 *                  _lfind     (CRTDLL.170)
 */
void * CDECL CRTDLL__lfind(const void *key, const void *base, size_t *nelp,
         size_t width, int (*compar)(const void *, const void *))
{
  dprintf2(("CRTDLL: _lfind\n"));
  char *char_base = (char *)base;
  int i;
  for(i=0;i<*nelp;i++) {
 	if ( compar(key,char_base) == 0)
		return char_base;
	char_base += width;
  }
  return NULL;
}


/*********************************************************************
 *                  _loaddll    (CRTDLL.171)
 */
void * CDECL CRTDLL__loaddll (char *name)
{
  dprintf2(("CRTDLL: _loaddll\n"));
  return (void*)LoadLibraryA(name);
}


/*******************************************************************
 *         _local_unwind2  (CRTDLL.172)
 */
void CDECL CRTDLL__local_unwind2( PEXCEPTION_FRAME endframe, DWORD nr )
{
	dprintf2(("CRTDLL: local_undwind2\n"));
}


/*********************************************************************
 *                  _locking        (CRTDLL.173)
 */
int CDECL CRTDLL__locking(int handle,int mode,unsigned long nbyte)
{
  dprintf(("CRTDLL: _locking not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _logb     (CRTDLL.174)
 */
double CDECL CRTDLL__logb( double x )
{
  dprintf(("CRTDLL: _logb not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _lrotl	(CRTDLL.175)
 */
unsigned long CDECL CRTDLL__lrotl( unsigned long value, unsigned int shift )
{
  dprintf2(("CRTDLL: _lrotl\n"));
  return (_lrotl(value, shift));
}


/*********************************************************************
 *                  _lrotr	(CRTDLL.176)
 */
unsigned long CDECL CRTDLL__lrotr( unsigned long value, unsigned int shift )
{
  dprintf2(("CRTDLL: _lrotr\n"));
  return (_lrotr(value, shift));
}


/*********************************************************************
 *                  _lsearch   (CRTDLL.177)
 */
void * CDECL CRTDLL__lsearch(const void *key, void *base, size_t *nelp, size_t width,
         int (*compar)(const void *, const void *))
{
  dprintf(("CRTDLL: _lsearch not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _lseek	(CRTDLL.178)
 */
long CDECL CRTDLL__lseek(int handle,long offset,int origin)
{
  dprintf2(("CRTDLL: _lssek\n"));
  return (_lseek(handle, offset, origin));
}


/*********************************************************************
 *                  _makepath	(CRTDLL.180)
 */
void CDECL CRTDLL__makepath( char *path, char *drive,
                    char *dir, char *fname, char *ext )
{
  dprintf2(("CRTDLL: _makepath\n"));
  _makepath(path, drive, dir, fname, ext);
}


/*********************************************************************
 *                  _matherr	(CRTDLL.181)
 */
double CDECL CRTDLL__matherr( struct exception * excep )
{
  dprintf2(("CRTDLL: _matherr\n"));
  return (_matherr(excep));
}


/*********************************************************************
 *                  _mbbtombc        (CRTDLL.182)
 */
unsigned int CDECL CRTDLL__mbbtombc( unsigned int c )
{
  dprintf2(("CRTDLL: _mbbtombc\n"));
  if (c >= 0x20 && c <= 0x7e) {
    return han_to_zen_ascii_table[c - 0x20];
  } else if (ISKANA(c)) {
    return han_to_zen_kana_table[c - 0xa0];
  }
  return c;
}


/*********************************************************************
 *                  _mbbtype        (CRTDLL.183)
 */
int CDECL CRTDLL__mbbtype( unsigned char c, int type )
{
  dprintf2(("CRTDLL: _mbbtype\n"));
	if ( type == 1 ) {
		if ((c >= 0x40 && c <= 0x7e ) || (c >= 0x80 && c <= 0xfc ) )
		{
			return _MBC_TRAIL;
		}
		else if (( c >= 0x20 && c >= 0x7E ) || ( c >= 0xA1 && c <= 0xDF ) || 
			 ( c >= 0x81 && c <= 0x9F ) || ( c >= 0xE0 && c <= 0xFC ) )
			 return _MBC_ILLEGAL;
		else
			return 0;
		
	}
	else  {
		if (( c >= 0x20 && c <= 0x7E ) || ( c >= 0xA1  && c <= 0xDF )) {
			return _MBC_SINGLE;
		}
		else if ( (c >= 0x81 && c <= 0x9F ) || ( c >= 0xE0 && c <= 0xFC) )
			return _MBC_LEAD;
		else if (( c >= 0x20 && c >= 0x7E ) || ( c >= 0xA1 && c <= 0xDF ) || 
			 ( c >= 0x81 && c <= 0x9F ) || ( c >= 0xE0 && c <= 0xFC ) )
			 return _MBC_ILLEGAL;
		else
			return 0;
		
	}
	
	
	return 0;	
}


/*********************************************************************
 *                  _mbccpy        (CRTDLL.184)
 */
void CDECL CRTDLL__mbccpy( unsigned char *dst, const unsigned char *src )
{
  dprintf2(("CRTDLL: _mbccpy\n"));

  if (!CRTDLL__ismbblead(*src) )
	return;
		
  memcpy(dst,src,_mbclen2(*src));
}


/*********************************************************************
 *                  _mbcjistojms     (CRTDLL.185)
 */
int CDECL CRTDLL__mbcjistojms( unsigned int c )
{
  dprintf2(("CRTDLL: _mbcjistojms\n"));
  int c1, c2;

  c2 = (unsigned char)c;
  c1 = c >> 8;
  if (c1 >= 0x21 && c1 <= 0x7e && c2 >= 0x21 && c2 <= 0x7e) {
    if (c1 & 0x01) {
      c2 += 0x1f;
      if (c2 >= 0x7f)
        c2 ++;
    } else {
      c2 += 0x7e;
    }
    c1 += 0xe1;
    c1 >>= 1;
    if (c1 >= 0xa0)
      c1 += 0x40;
    return ((c1 << 8) | c2);
  }
  return 0;
}


/*********************************************************************
 *                  _mbcjmstojis     (CRTDLL.186)
 */
int CDECL CRTDLL__mbcjmstojis( unsigned int c )
{
  dprintf2(("CRTDLL: _mbcjmstojis\n"));
  int c1, c2;

  c2 = (unsigned char)c;
  c1 = c >> 8;
  if (c1 < 0xf0 && CRTDLL__ismbblead(c1) && CRTDLL__ismbbtrail(c2)) {
    if (c1 >= 0xe0)
      c1 -= 0x40;
    c1 -= 0x70;
    c1 <<= 1;
    if (c2 < 0x9f) {
      c1 --;
      c2 -= 0x1f;
      if (c2 >= (0x80-0x1f))
        c2 --;
    } else {
      c2 -= 0x7e;
    }
    return ((c1 << 8) | c2);
  }
  return 0;
}


/*********************************************************************
 *                  _mbclen    (CRTDLL.187)
 */
size_t CDECL CRTDLL__mbclen( const unsigned char *s )
{
  dprintf2(("CRTDLL: _mbclen\n"));
  return (CRTDLL__ismbblead(*s>>8) && CRTDLL__ismbbtrail(*s&0x00FF)) ? 2 : 1;
}


/*********************************************************************
 *                  _mbctohira     (CRTDLL.188)
 */
int CDECL CRTDLL__mbctohira( unsigned int c )
{
  dprintf2(("CRTDLL: _mbctohira\n"));
  return c;
}


/*********************************************************************
 *                  _mbctokata     (CRTDLL.189)
 */
int CDECL CRTDLL__mbctokata( unsigned int c )
{
  dprintf2(("CRTDLL: _mbctokata\n"));
  return c;
}


/*********************************************************************
 *                  _mbctolower     (CRTDLL.190)
 */
unsigned int CDECL CRTDLL__mbctolower( unsigned int c )
{
  dprintf2(("CRTDLL: _mbctolower\n"));
        if ((c & 0xFF00) != 0) {
// true multibyte case conversion needed
		if ( CRTDLL__ismbclower(c) )
			return c + CASE_DIFF;

        } else
		return _mbbtolower(c);

	return 0;
}


/*********************************************************************
 *                  _mbctombb        (CRTDLL.191)
 */
unsigned int CDECL CRTDLL__mbctombb( unsigned int c )
{
  dprintf2(("CRTDLL: _mbctombb\n"));
  int i;
  unsigned short *p;

  if (JISKANA(c)) {
    return zen_to_han_kana_table[c - 0x8340];
  } else if (JISHIRA(c)) {
    c = JTOKANA(c);
    return zen_to_han_kana_table[c - 0x8340];
  } else if (c <= 0x8396) {
    for (i = 0x20, p = han_to_zen_ascii_table; i <= 0x7e; i++, p++) {
      if (*p == c) {
        return i;
      }
    }
    for (i = 0; i < ZTOH_SYMBOLS; i++) {
      if (zen_to_han_symbol_table_1[i] == c) {
        return zen_to_han_symbol_table_2[i];
      }
    }
  }
  return c;
}


/*********************************************************************
 *                  _mbctoupper     (CRTDLL.192)
 */
unsigned int CDECL CRTDLL__mbctoupper( unsigned int c )
{
  dprintf2(("CRTDLL: _mbctoupper\n"));
  if ((c & 0xFF00) != 0) {
// true multibyte case conversion needed
  if ( CRTDLL__ismbcupper(c) )
	return c + CASE_DIFF;

  } else
	return _mbbtoupper(c);

  return 0;
}


/*********************************************************************
 *                  _mbsbtype     (CRTDLL.194)
 */
int CDECL CRTDLL__mbsbtype( const unsigned char *str, int n )
{
  dprintf2(("CRTDLL: _mbsbtype\n"));
  if ( str == NULL )
	return -1;
  return CRTDLL__mbbtype(*(str+n),1);
}


/*********************************************************************
 *                  _mbscat       (CRTDLL.195)
 */
unsigned char * CDECL CRTDLL__mbscat( unsigned char *dst, const unsigned char *src )
{
  dprintf2(("CRTDLL: _mbscat\n"));
  return (unsigned char*)strcat((char*)dst,(char*)src);
}


/*********************************************************************
 *                  _mbschr       (CRTDLL.196)
 */
unsigned char * CDECL CRTDLL__mbschr( const unsigned char *str, unsigned int c )
{
  dprintf2(("CRTDLL: _mbschr\n"));
  return (unsigned char*)strchr((char*)str,c);
}


/*********************************************************************
 *                  _mbscmp       (CRTDLL.197)
 */
int CDECL CRTDLL__mbscmp( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbscmp\n"));
  return strcmp((char*)s1,(char*)s2);
}


/*********************************************************************
 *                  _mbscpy       (CRTDLL.198)
 */
unsigned char * CDECL CRTDLL__mbscpy( unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbscpy\n"));
  return (unsigned char*)strcpy((char*)s1,(char*)s2);
}


/*********************************************************************
 *                  _mbscspn        (CRTDLL.199)
 */
size_t CDECL CRTDLL__mbscspn( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbscspn\n"));
  const char *p, *spanp;
  char c, sc;

  for (p = (const char*)s1;;)
  {
    c = *p++;
    spanp = (const char*)s2;
    do {
      if ((sc = *spanp++) == c)
	return (size_t)(p - 1) - (size_t)s1;
    } while (sc != 0);
  }
  /* NOTREACHED */
}


/*********************************************************************
 *           _mbsdec    (CRTDLL.200)
 */
unsigned char * CDECL CRTDLL__mbsdec( const unsigned char *str, const unsigned char *cur )
{
  dprintf2(("CRTDLL: _mbsdec\n"));
  unsigned char *s = (unsigned char *)cur;
  if ( str >= cur )
	return NULL;
  s--;
  if (CRTDLL__ismbblead(*(s-1)) )
 	s--;

  return s; 
}


/*********************************************************************
 *           _mbsdup    (CRTDLL.201)
 */
unsigned char * CDECL CRTDLL__mbsdup( unsigned char *_s )
{
  dprintf2(("CRTDLL: _mbsdup\n"));
  char *rv;
  if (_s == 0)
	return 0;
  rv = (char *)malloc(CRTDLL__mbslen((LPCSTR)_s) + 1);
  if (rv == 0)
 	return 0;
  CRTDLL__mbscpy((unsigned char*)rv, _s);
  return (unsigned char*)rv;
}


/*********************************************************************
 *           CRTDLL__mbsicmp   (CRTDLL.202)
 */
int CDECL CRTDLL__mbsicmp( const unsigned char *x, const unsigned char *y )
{
  dprintf2(("CRTDLL: _mbsicmp\n"));
    do {
	if (!*x)
	    return !!*y;
	if (!*y)
	    return !!*x;
	/* FIXME: MBCS handling... */
	if (*x!=*y)
	    return 1;
        x++;
        y++;
    } while (1);
}


/*********************************************************************
 *           CRTDLL__mbsinc    (CRTDLL.203)
 */
LPSTR CDECL CRTDLL__mbsinc( LPCSTR str )
{
    dprintf2(("CRTDLL: _mbsinc\n"));
    int len = mblen( str, MB_LEN_MAX );
    if (len < 1) len = 1;
    return (LPSTR)(str + len);
}


/*********************************************************************
 *           CRTDLL__mbslen    (CRTDLL.204)
 */
INT CDECL CRTDLL__mbslen( LPCSTR str )
{
    dprintf2(("CRTDLL: _mbslen\n"));
    INT len, total = 0;
    while ((len = mblen( str, MB_LEN_MAX )) > 0)
    {
        str += len;
        total++;
    }
    return total;
}


/*********************************************************************
 *           _mbslwr    (CRTDLL.205)
 */
unsigned char * CDECL CRTDLL__mbslwr( unsigned char *x )
{
  dprintf2(("CRTDLL: _mbslwr\n"));
  unsigned char  *y=x;

  while (*y) {
	if (!CRTDLL__ismbblead(*y) )
		*y = tolower(*y);
	else {
               	*y=CRTDLL__mbctolower(*(unsigned short *)y);
               	y++;
	}
  }
  return x;
}


/*********************************************************************
 *           _mbsnbcat  (CRTDLL.206)
 */
unsigned char * CDECL CRTDLL__mbsnbcat( unsigned char *dst, const unsigned char *src, size_t n )
{
  dprintf2(("CRTDLL: _mbsnbcat\n"));
	char *d; 
    	char *s = (char *)src;	
    	if (n != 0) {
		d = (char*)dst + strlen((char*)dst); // get the end of string 
		d += _mbclen2(*d); // move 1 or 2 up

		do {
			if ((*d++ = *s++) == 0)
			{
				while (--n != 0)
					*d++ = 0;
				break;
      			}
			if ( !(n==1 && CRTDLL__ismbblead(*s)) )
				n--;
    		} while (n > 0);
  	}
  	return dst;
}


/*********************************************************************
 *           _mbsnbcmp  (CRTDLL.207)
 */
int CDECL CRTDLL__mbsnbcmp( const unsigned char *str1, const unsigned char *str2, size_t n )
{
  dprintf2(("CRTDLL: _mbsnbcmp\n"));
	unsigned char *s1 = (unsigned char *)str1;
	unsigned char *s2 = (unsigned char *)str2;

	unsigned short *short_s1, *short_s2;

	int l1, l2;

	if (n == 0)
		return 0;
	do {
		
		if (*s1 == 0)
			break;	

		l1 = CRTDLL__ismbblead(*s1);
		l2 = CRTDLL__ismbblead(*s2);
		if ( !l1 &&  !l2  ) {

			if (*s1 != *s2)
				return *s1 - *s2;
			else {
				s1 += 1;
				s2 += 1;
				n--;
			}
		}
		else if ( l1 && l2 ){
			short_s1 = (unsigned short *)s1;
			short_s2 = (unsigned short *)s2;
			if ( *short_s1 != *short_s2 )
				return *short_s1 - *short_s2;
			else {
				s1 += 2;
				s2 += 2;
				n-=2;

			}
		}
		else
			return *s1 - *s2;
	} while (n > 0);
	return 0;
}


/*********************************************************************
 *           _mbsnbcnt  (CRTDLL.208)
 */
size_t CDECL CRTDLL__mbsnbcnt( const unsigned char *str, size_t n )
{
  dprintf2(("CRTDLL: _mbsnbcnt\n"));
	unsigned char *s = (unsigned char *)str;
	while(*s != 0 && n > 0) {
		if (!CRTDLL__ismbblead(*s) )
			n--;
		s++;
	}
	
	return (size_t)(s - str);
}


/*********************************************************************
 *           _mbsnbcpy  (CRTDLL.209)
 */
unsigned char * CDECL CRTDLL__mbsnbcpy( unsigned char *str1, const unsigned char *str2, size_t n )
{
  dprintf2(("CRTDLL: _mbsnbcpy\n"));
	unsigned char *s1 = (unsigned char *)str1;
	unsigned char *s2 = (unsigned char *)str2;

	unsigned short *short_s1, *short_s2;

	if (n == 0)
		return 0;
	do {
		
		if (*s2 == 0)
			break;	

		if (  !CRTDLL__ismbblead(*s2) ) {

			*s1 = *s2;
			s1 += 1;
			s2 += 1;
			n--;
		}
		else {
			short_s1 = (unsigned short *)s1;
			short_s2 = (unsigned short *)s2;
			*short_s1 = *short_s2;
			s1 += 2;
			s2 += 2;
			n-=2;
		}
	} while (n > 0);
	return str1;
}


/*********************************************************************
 *           _mbsnbicmp (CRTDLL.210)
 */
int CDECL CRTDLL__mbsnbicmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf2(("CRTDLL: _mbsnbicmp\n"));
  if (n == 0)
    return 0;
  do {
    if (_mbbtoupper(*s1) != _mbbtoupper(*s2))
      return _mbbtoupper(*(unsigned const char *)s1) - _mbbtoupper(*(unsigned const char *)s2);
    s1 += _mbclen2(*s1);
    s2 += _mbclen2(*s2);


    if (*s1 == 0)
      break;
    n--;
  } while (n > 0);
  return 0;
}


/*********************************************************************
 *           _mbsnbset (CRTDLL.211)
 */
unsigned char * CDECL CRTDLL__mbsnbset( unsigned char *src, unsigned int val, size_t count )
{
  dprintf2(("CRTDLL: _mbsnbset\n"));
	unsigned char *char_src = (unsigned char *)src;
	unsigned short *short_src = (unsigned short *)src;
       
	if ( _mbclen2(val) == 1 ) {
	
		while(count > 0) {
                	*char_src = val;
                	char_src++;
                	count--;
        	}
        	*char_src = 0;
        }
        else {
		while(count > 0) {
                	*short_src = val;
                	short_src++;
                	count-=2;
        	}    	
        	*short_src = 0;
        }
        
        return src;
}


/*********************************************************************
 *           _mbsncat (CRTDLL.212)
 */
unsigned char * CDECL CRTDLL__mbsncat( unsigned char *dst, const unsigned char *src, size_t n )
{
  dprintf2(("CRTDLL: _mbsncat\n"));
	char *d = (char *)dst; 
    	char *s = (char *)src;	
    	if (n != 0) {
		d = (char*)dst + strlen((char*)dst); // get the end of string 
		d += _mbclen2(*d); // move 1 or 2 up

		do {
			if ((*d++ = *s++) == 0)
			{
				while (--n != 0)
					*d++ = 0;
				break;
      			}
			if (!CRTDLL__ismbblead(*s) )
				n--;
    		} while (n > 0);
  	}
  	return dst;
}


/*********************************************************************
 *           _mbsnccnt (CRTDLL.213)
 */
size_t CDECL CRTDLL__mbsnccnt( const unsigned char *str, size_t n )
{
  dprintf2(("CRTDLL: _mbsnccnt\n"));
	unsigned char *s = (unsigned char *)str;
	size_t cnt = 0;
	while(*s != 0 && n > 0) {
		if (CRTDLL__ismbblead(*s) ) 
			s++;
		else
			n--;
		s++;
		cnt++;
	}
	
	return cnt;
}


/*********************************************************************
 *           _mbsncmp (CRTDLL.214)
 */
int CDECL CRTDLL__mbsncmp( const unsigned char *str1, const unsigned char *str2, size_t n )
{
  dprintf2(("CRTDLL: _mbsncmp\n"));
	unsigned char *s1 = (unsigned char *)str1;
	unsigned char *s2 = (unsigned char *)str2;

	unsigned short *short_s1, *short_s2;

	int l1, l2;

	if (n == 0)
		return 0;
	do {
		
		if (*s1 == 0)
			break;	

		l1 = CRTDLL__ismbblead(*s1);
		l2 = CRTDLL__ismbblead(*s2);
		if ( !l1 &&  !l2  ) {

			if (*s1 != *s2)
				return *s1 - *s2;
			else {
				s1 += 1;
				s2 += 1;
				n--;
			}
		}
		else if ( l1 && l2 ){
			short_s1 = (unsigned short *)s1;
			short_s2 = (unsigned short *)s2;
			if ( *short_s1 != *short_s2 )
				return *short_s1 - *short_s2;
			else {
				s1 += 2;
				s2 += 2;
				n--;

			}
		}
		else
			return *s1 - *s2;
	} while (n > 0);
	return 0;
}


/*********************************************************************
 *           _mbsncpy (CRTDLL.215)
 */
unsigned char * CDECL CRTDLL__mbsncpy( unsigned char *str1, const unsigned char *str2, size_t n )
{
  dprintf2(("CRTDLL: _mbsncpy\n"));
	unsigned char *s1 = (unsigned char *)str1;
	unsigned char *s2 = (unsigned char *)str2;

	unsigned short *short_s1, *short_s2;

	if (n == 0)
		return 0;
	do {
		
		if (*s2 == 0)
			break;	

		if (  !CRTDLL__ismbblead(*s2) ) {

			*s1 = *s2;
			s1 += 1;
			s2 += 1;
			n--;
		}
		else {
			short_s1 = (unsigned short *)s1;
			short_s2 = (unsigned short *)s2;
			*short_s1 = *short_s2;
			s1 += 2;
			s2 += 2;
			n--;
		}
	} while (n > 0);
	return str1;
}


/*********************************************************************
 *           _mbsnextc (CRTDLL.216)
 */
unsigned int CDECL CRTDLL__mbsnextc( const unsigned char *src )
{
  dprintf2(("CRTDLL: _mbsnextc\n"));
	unsigned char *char_src = (unsigned char *)src;
	unsigned short *short_src = (unsigned short *)src;

	if ( src == NULL )
		return 0;

	if ( !CRTDLL__ismbblead(*src) )
		return *char_src;
	else
		return *short_src;
	return 0;

}


/*********************************************************************
 *           _mbsnicmp (CRTDLL.217)
 */
int CDECL CRTDLL__mbsnicmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf2(("CRTDLL: _mbsnicmp\n"));
  if (n == 0)
    return 0;
  do {
    if (_mbbtoupper(*s1) != _mbbtoupper(*s2))
      return _mbbtoupper(*(unsigned const char *)s1) - _mbbtoupper(*(unsigned const char *)s2);

// Next 2 lines won't compile
//    *s1 += _mbclen2(*s1);
//    *s2 += _mbclen2(*s2);


    if (*s1 == 0)
      break;
    if (!CRTDLL__ismbblead(*s1) )
	n--;
  } while (n > 0);
  return 0;
}


/*********************************************************************
 *           _mbsninc (CRTDLL.218)
 */
unsigned char * CDECL CRTDLL__mbsninc( const unsigned char *str, size_t n )
{
  dprintf2(("CRTDLL: _mbsninc\n"));
	unsigned char *s = (unsigned char *)str;
	while(*s != 0 && n > 0) {
		if (!CRTDLL__ismbblead(*s) )
			n--;
		s++;
	}
	
	return s;
}


/*********************************************************************
 *           _mbsnset (CRTDLL.219)
 */
unsigned char * CDECL CRTDLL__mbsnset( unsigned char *src, unsigned int val, size_t count )
{
  dprintf2(("CRTDLL: _mbsnset\n"));
	unsigned char *char_src = (unsigned char *)src;
	unsigned short *short_src = (unsigned short *)src;
       
	if ( _mbclen2(val) == 1 ) {
	
		while(count > 0) {
                	*char_src = val;
                	char_src++;
                	count--;
        	}
        	*char_src = 0;
        }
        else {
		while(count > 0) {
                	*short_src = val;
                	short_src++;
                	count-=2;
        	}    	
        	*short_src = 0;
        }
        
        return src;

}


/*********************************************************************
 *           _mbspbrk   (CRTDLL.220)
 */
unsigned char * CDECL CRTDLL__mbspbrk( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbspbrk\n"));
  const char *scanp;
  int c, sc;

  while ((c = *s1++) != 0)
  {
    for (scanp = (char*)s2; (sc = *scanp++) != 0;)
      if (sc == c)
	return (unsigned char *)((char *)s1 - (char *)1);
  }
  return 0;
}


/*********************************************************************
 *           CRTDLL__mbsrchr   (CRTDLL.221)
 */
LPSTR CDECL CRTDLL__mbsrchr(LPSTR s,CHAR x)
{
  dprintf2(("CRTDLL: _mbsrchr\n"));
	/* FIXME: handle multibyte strings */
	return strrchr(s,x);
}


/*********************************************************************
 *           _mbsrev    (CRTDLL.222)
 */
unsigned char * CDECL CRTDLL__mbsrev( unsigned char *s )
{
  dprintf2(("CRTDLL: _mbsrev\n"));
	unsigned char  *e;
	unsigned char  a;
	e=s;
	while (*e) {
		if ( CRTDLL__ismbblead(*e) ) {
			a = *e;
			*e = *++e;
			if ( *e == 0 )
				break;
			*e = a;
		} 
		e++;
	}
	while (s<e) {
		a=*s;
		*s=*e;
		*e=a;
		s++;
		e--;
	}
	

	return s;
}


/*********************************************************************
 *           _mbsset    (CRTDLL.223)
 */
unsigned char * CDECL CRTDLL__mbsset( unsigned char *src, unsigned int c )
{
  dprintf2(("CRTDLL: _mbsset\n"));
	unsigned char *char_src = src;
	unsigned short *short_src = (unsigned short*)src;
       
	if ( _mbclen2(c) == 1 ) {
	
		while(*char_src != 0) {
                	*char_src = c;
                	char_src++;
        	}
        	*char_src = 0;
        }
        else {
		while(*short_src != 0) {
                	*short_src = c;
                	short_src++;
        	}    	
        	*short_src = 0;
        }
        
        return src;
}


/*********************************************************************
 *           _mbsspn   (CRTDLL.224)
 */
size_t CDECL CRTDLL__mbsspn( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbsspn\n"));
  const char *p = (char*)s1, *spanp;
  char c, sc;

 cont:
  c = *p++;
  for (spanp = (char*)s2; (sc = *spanp++) != 0;)
    if (sc == c)
      goto cont;
  return (size_t)(p - 1) - (size_t)s1;
}


/*********************************************************************
 *           _mbsspnp   (CRTDLL.225)
 */
unsigned char * CDECL CRTDLL__mbsspnp( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbsspnp\n"));
  const char *p = (char*)s1, *spanp;
  char c, sc;

 cont:
  c = *p++;
  for (spanp = (char*)s2; (sc = *spanp++) != 0;)
    if (sc == c)
      goto cont;
  return (unsigned char*)p;
}


/*********************************************************************
 *           _mbsstr    (CRTDLL.226)
 */
unsigned char * CDECL CRTDLL__mbsstr( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf2(("CRTDLL: _mbsstr\n"));
  return (unsigned char*)strstr((const char*)s1,(const char*)s2);
}


/*********************************************************************
 *           _mbstok    (CRTDLL.227)
 */
unsigned char * CDECL CRTDLL__mbstok( unsigned char *s, const unsigned char *delim )
{
  dprintf2(("CRTDLL: _mbstok\n"));
  const char *spanp;
  int c, sc;
  char *tok;
  static char *last;


  if (s == NULL && (s = (unsigned char*)last) == NULL)
    return (NULL);

  /*
   * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
   */
 cont:
  c = *s;
  s = (unsigned char*)CRTDLL__mbsinc((LPCSTR)s);

  for (spanp = (const char*)delim; (sc = *spanp) != 0; spanp = CRTDLL__mbsinc(spanp)) {
    if (c == sc)
      goto cont;
  }

  if (c == 0) {			/* no non-delimiter characters */
    last = NULL;
    return (NULL);
  }
  tok = (char*)s - 1;

  /*
   * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
   * Note that delim must have one NUL; we stop if we see that, too.
   */
  for (;;) {
    c = *s;
    s = (unsigned char*)CRTDLL__mbsinc((LPCSTR)s);
    spanp = (const char*)delim;
    do {
      if ((sc = *spanp) == c) {
	if (c == 0)
	  s = NULL;
	else
	  s[-1] = 0;
	last = (char*)s;
	return ((unsigned char*)tok);
      }
      spanp = CRTDLL__mbsinc(spanp);
    } while (sc != 0);
  }
  /* NOTREACHED */
}


/*********************************************************************
 *           _mbstrlen   (CRTDLL.228)
 */
size_t CDECL CRTDLL__mbstrlen(const char *string)
{
  dprintf2(("CRTDLL: _mbstrlen\n"));
	char *s = (char *)string;
	size_t i;
	while ( *s != 0 ) {
		if ( CRTDLL__ismbblead(*s) )
			s++;
		s++;
		i++;
	}
	return i;
}


/*********************************************************************
 *           _mbsupr    (CRTDLL.229)
 */
unsigned char * CDECL CRTDLL__mbsupr( unsigned char *x )
{
  dprintf2(("CRTDLL: _mbsupr\n"));
	unsigned char  *y=x;
        while (*y) {
		if (!CRTDLL__ismbblead(*y) )
			*y = toupper(*y);
		else {
                	*y=CRTDLL__mbctoupper(*(unsigned short *)y);
                	y++;
		}
        }
        return x;
}


/*********************************************************************
 *           CRTDLL__memccpy   (CRTDLL.230)
 */
void * CDECL CRTDLL__memccpy(void *to, const void *from,int c,size_t count)
{
  dprintf2(("CRTDLL: _memccpy\n"));
  memcpy(to,from,count);
  return memchr(to,c,count);
}


/*********************************************************************
 *                  _mkdir           (CRTDLL.232)
 */
INT CDECL CRTDLL__mkdir(LPCSTR newdir)
{
	dprintf2(("CRTDLL: mkdir\n"));
	if (!CreateDirectoryA(newdir,NULL))
		return -1;
	return 0;
}


/*********************************************************************
 *                  _mktemp        (CRTDLL.233)
 */
char * CDECL CRTDLL__mktemp( char * _template )
{
  dprintf2(("CRTDLL: _mktemp\n"));
  static int count = 0;
  char *cp, *dp;
  int i, len, xcount, loopcnt;

 

  len = strlen (_template);
  cp = _template + len;

  xcount = 0;
  while (xcount < 6 && cp > _template && cp[-1] == 'X')
    xcount++, cp--;

  if (xcount) {
    dp = cp;
    while (dp > _template && dp[-1] != '/' && dp[-1] != '\\' && dp[-1] != ':')
      dp--;

    /* Keep the first characters of the template, but turn the rest into
       Xs.  */
    while (cp > dp + 8 - xcount) {
      *--cp = 'X';
      xcount = (xcount >= 6) ? 6 : 1 + xcount;
    }

    /* If dots occur too early -- squash them.  */
    while (dp < cp) {
      if (*dp == '.') *dp = 'a';
      dp++;
    }

    /* Try to add ".tmp" to the filename.  Truncate unused Xs.  */
    if (cp + xcount + 3 < _template + len)
      strcpy (cp + xcount, ".tmp");
    else
      cp[xcount] = 0;

    for (loopcnt = 0; loopcnt < (1 << (5 * xcount)); loopcnt++) {
      int c = count++;
      for (i = 0; i < xcount; i++, c >>= 5)
	cp[i] = "abcdefghijklmnopqrstuvwxyz012345"[c & 0x1f];
      if (CRTDLL__access(_template,0) == -1)
	return _template;
    }
  }
  /* Failure:  truncate the template and return NULL.  */
  *_template = 0;
  return 0;
}


/*********************************************************************
 *                  _msize        (CRTDLL.234)
 */
size_t CDECL CRTDLL__msize( void *ptr )
{
  dprintf2(("CRTDLL: _msize\n"));
  return (_msize(ptr));
}


/*********************************************************************
 *                  _nextafter        (CRTDLL.235)
 */
double CDECL CRTDLL__nextafter( double x, double y )
{
  dprintf2(("CRTDLL: _nextafter\n"));
  if ( x == y)
	return x;
  if ( CRTDLL__isnan(x) || CRTDLL__isnan(y) )
	return x;

  return x;
}


/*********************************************************************
 *                  _onexit        (CRTDLL.236)
 */
onexit_t CDECL CRTDLL__onexit(onexit_t t)
{
  dprintf2(("CRTDLL: _onexit\n"));
  return (_onexit(t));
}


/*********************************************************************
 *                  _open        (CRTDLL.237)
 */
HFILE CDECL CRTDLL__open(LPCSTR path,INT flags)
{
    dprintf2(("CRTDLL: _open\n"));
    DWORD access = 0, creation = 0;
    HFILE ret;
    
    switch(flags & 3)
    {
    case O_RDONLY: access |= GENERIC_READ; break;
    case O_WRONLY: access |= GENERIC_WRITE; break;
    case O_RDWR:   access |= GENERIC_WRITE | GENERIC_READ; break;
    }

    if (flags & 0x0100) /* O_CREAT */
    {
        if (flags & 0x0400) /* O_EXCL */
            creation = CREATE_NEW;
        else if (flags & 0x0200) /* O_TRUNC */
            creation = CREATE_ALWAYS;
        else
            creation = OPEN_ALWAYS;
    }
    else  /* no O_CREAT */
    {
        if (flags & 0x0200) /* O_TRUNC */
            creation = TRUNCATE_EXISTING;
        else
            creation = OPEN_EXISTING;
    }
    if (flags & 0x0008) /* O_APPEND */
        dprintf2(("O_APPEND not supported\n" ));
    if (flags & 0xf0f4) 
      dprintf2(("CRTDLL_open file unsupported flags 0x%04x\n",flags));
    /* End Fixme */

    ret = CreateFileA( path, access, FILE_SHARE_READ | FILE_SHARE_WRITE,
                         NULL, creation, FILE_ATTRIBUTE_NORMAL, -1 );
    dprintf2(("CRTDLL_open file %s mode 0x%04x got handle %d\n", path,flags,ret));
    return ret;
}


/*********************************************************************
 *                  _open_osfhandle  (CRTDLL.238)
 */
INT CDECL CRTDLL__open_osfhandle( long osfhandle, int flags )
{
  dprintf2(("CRTDLL: _open_osfhandle\n"));
HFILE handle;
 
	switch (osfhandle) {
	case STD_INPUT_HANDLE :
	case 0 :
	  handle=0;
	  break;
 	case STD_OUTPUT_HANDLE:
 	case 1:
	  handle=1;
	  break;
	case STD_ERROR_HANDLE:
	case 2:
	  handle=2;
	  break;
	default:
	  return (-1);
	}
	dprintf2(("(handle %08lx,flags %d) return %d\n",
		     osfhandle,flags,handle));
	return handle;
}


/*********************************************************************
 *                  _pclose     (CRTDLL.244)
 */
INT CDECL CRTDLL__pclose( FILE *fp )
{
  dprintf(("CRTDLL: _pclose not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _pipe     (CRTDLL.247)
 */
INT CDECL CRTDLL__pipe( int *phandles, unsigned psize, int textmode )
{
  dprintf(("CRTDLL: _pipe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _popen    (CRTDLL.248)
 */
FILE * CDECL CRTDLL__popen( const char *command, const char *mode )
{
  dprintf(("CRTDLL: _popen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _purecall     (CRTDLL.249)
 */
void CDECL CRTDLL__purecall(void)
{
  dprintf2(("CRTDLL: _purecall\n"));
}


/*********************************************************************
 *                  _putch     (CRTDLL.250)
 */
INT CDECL CRTDLL__putch( int i )
{
  dprintf2(("CRTDLL: _putch\n"));
  return (_putch(i));
}


/*********************************************************************
 *                  _putenv     (CRTDLL.251)
 */
INT CDECL CRTDLL__putenv(const char *s)
{
  dprintf2(("CRTDLL: _putenv\n"));
  return (_putenv(s));
}


/*********************************************************************
 *                  _putw     (CRTDLL.252)
 */
INT CDECL CRTDLL__putw( int w, FILE *stream )
{
  dprintf2(("CRTDLL: _putw\n"));
  if (fwrite( &w, sizeof(w), 1, stream) < 1)
    return(EOF);
  return(0);
}


/*********************************************************************
 *                  _read     (CRTDLL.254)
 */
INT CDECL CRTDLL__read(INT fd, LPVOID buf, UINT count)
{
  dprintf(("CRTDLL: _read not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _rmdir     (CRTDLL.255)
 */
INT CDECL CRTDLL__rmdir(const char *path)
{
  dprintf2(("CRTDLL: _rmdir\n"));
  if (!RemoveDirectoryA(path))
	return -1;
  return 0;
}


/*********************************************************************
 *                  _rmtmp     (CRTDLL.256)
 */
INT CDECL CRTDLL__rmtmp(void)
{
  dprintf2(("CRTDLL: _rmtmp\n"));
  return(_rmtmp());
}


/*********************************************************************
 *           CRTDLL__rotl 	 (CRTDLL.257)
 */
unsigned int CDECL CRTDLL__rotl( unsigned int value, unsigned int shift )
{
  dprintf2(("CRTDLL: _rotl\n"));
  return (_rotl(value, shift));
}


/*********************************************************************
 *           CRTDLL__rotr 	 (CRTDLL.258)
 */
unsigned int CDECL CRTDLL__rotr( unsigned int value, unsigned int shift )
{
  dprintf2(("CRTDLL: _rotr\n"));
  return (_rotr(value, shift));
}


/*********************************************************************
 *           _scalb 	 (CRTDLL.259)
 */
double CDECL CRTDLL__scalb( double __x, long e )
{
  dprintf2(("CRTDLL: _scalb\n"));
  double_t *x = (double_t *)&__x;
	
  x->exponent += e;

  return __x;
}


/*********************************************************************
 *           CRTDLL__searchenv 	 (CRTDLL.260)
 */
void CDECL CRTDLL__searchenv(const char *file,const char *var,char *path )
{
  dprintf2(("CRTDLL: _searchenv\n"));
  char *env = CRTDLL_getenv(var);

  char *x;
  char *y;
  char *FilePart;
  x = strchr(env,'=');
  if ( x != NULL ) {
	*x = 0;
	x++;
  }
  y = strchr(env,';');
  while ( y != NULL ) {
	*y = 0;
	if ( SearchPathA(x,file,NULL,MAX_PATH,path,&FilePart) > 0 ) {
			return;
	}
	x = y+1;
	y = strchr(env,';');
  }
  return;
}


/*********************************************************************
 *           CRTDLL__seterrormode 	 (CRTDLL.261)
 */
void CDECL CRTDLL__seterrormode(int i)
{
  dprintf2(("CRTDLL: _seterrormode\n"));
  SetErrorMode(i);
  return;
}


/*********************************************************************
 *           CRTDLL__setjmp 	 (CRTDLL.262)
 */
int CDECL CRTDLL__setjmp( jmp_buf env )
{
  //TODO:
  dprintf2(("CRTDLL: _setjmp -> setjmp (NOT IDENTICAL!!!)\n"));
  return(setjmp( env));
}

/*********************************************************************
 *           CRTDLL__setjmp3 	 (CRTDLL.262)
 */
int CDECL CRTDLL__setjmp3( jmp_buf env )
{
  //TODO:
  dprintf2(("CRTDLL: _setjmp3 -> setjmp (NOT IDENTICAL!!!)\n"));
  return(setjmp( env));
}


/*********************************************************************
 *                  _setmode           (CRTDLL.263)
 */
INT CDECL CRTDLL__setmode( INT fh,INT mode)
{
	dprintf2(("CRTDLL: _setmode\n"));
	return (_setmode(fh, mode));
}


/*********************************************************************
 *                  _setsystime    (CRTDLL.264)
 */
unsigned int CDECL CRTDLL__setsystime(struct tm *tp, unsigned int ms)
{
  dprintf(("CRTDLL: _setsystime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _sleep           (CRTDLL.265)
 */
VOID CDECL CRTDLL__sleep(unsigned long timeout) 
{
  dprintf2(("CRTDLL__sleep for %ld milliseconds\n",timeout));
  Sleep((timeout)?timeout:1);
}


/*********************************************************************
 *           _sopen 	 (CRTDLL.268)
 */
int CDECL CRTDLL__sopen( const char *s, int i1, int i2, ... )
{
  dprintf(("CRTDLL: _sopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__spawnl 	 (CRTDLL.269)
 */
int CDECL CRTDLL__spawnl(int nMode, const char* szPath, const char* szArgv0,...)
{
  dprintf2(("CRTDLL: _spawnl\n"));
  char *szArg[100];
  const char *a;
  int i = 0;
  va_list l = 0;
  va_start(l,szArgv0);
  do {
  	a = va_arg(l,const char *);
	szArg[i++] = (char *)a;
  } while ( a != NULL && i < 100 );
  
  return _spawnve(nMode, (char*)szPath, szArg, _environ);
}


/*********************************************************************
 *           CRTDLL__spawnle 	 (CRTDLL.270)
 */
int CDECL CRTDLL__spawnle( int mode, char *path, char **szArgv0, ... )
{
  dprintf2(("CRTDLL: _spawnle not correct implemented.\n"));
  char *szArg[100];
  char *a;
  char *ptr;
  int i = 0;
  va_list l = 0;
  va_start(l,szArgv0);
  do {
  	a = (char*)va_arg(l,const char *);
	szArg[i++] = (char *)a;
  } while ( a != NULL && i < 100 );


// szArg0 is passed and not environment if there is only one parameter;

  if ( i >=2 ) {
  	ptr = szArg[i-2];
  	szArg[i-2] = NULL;
  }
  else
	ptr = NULL;

  return _spawnve(mode, path, szArg, (char**)ptr);
}


/*********************************************************************
 *           CRTDLL__spawnlp 	 (CRTDLL.271)
 */
int CDECL CRTDLL__spawnlp(int nMode, const char* szPath, const char* szArgv0, ...)
{
  dprintf2(("CRTDLL: _spawnlp\n"));
  char *szArg[100];
  const char *a;
  int i = 0;
  va_list l = 0;
  va_start(l,szArgv0);
  do {
  	a = (const char *)va_arg(l,const char *);
	szArg[i++] = (char *)a;
  } while ( a != NULL && i < 100 );
  return _spawnvpe(nMode, (char*)szPath,szArg, _environ);
}


/*********************************************************************
 *           CRTDLL__spawnlpe 	 (CRTDLL.272)
 */
int CDECL CRTDLL__spawnlpe( int mode, char *path, char *szArgv0, ... )
{
  dprintf2(("CRTDLL: _spawnlpe not correct implemented.\n"));
  char *szArg[100];
  const char *a;
  char *ptr;
  int i = 0;
  va_list l = 0;
  va_start(l,szArgv0);
  do {
  	a = (char *)va_arg(l,const char *);
	szArg[i++] = (char *)a;
  } while ( a != NULL && i < 100 );


// szArg0 is passed and not environment if there is only one parameter;

  if ( i >=2 ) {
  	ptr = szArg[i-2];
  	szArg[i-2] = NULL;
  }
  else
	ptr = NULL;

  return _spawnvpe(mode, path, szArg, (char**)ptr);
}


/*********************************************************************
 *           CRTDLL__spawnv 	 (CRTDLL.273)
 */
int CDECL CRTDLL__spawnv( int i, char *s1, char ** s2 )
{
  dprintf2(("CRTDLL: _spawnv\n"));
  return (_spawnv(i, s1, s2));
}


/*********************************************************************
 *           CRTDLL__spawnve 	 (CRTDLL.274)
 */
int CDECL CRTDLL__spawnve( int i, char *s1, char ** s2, char ** s3 )
{
  dprintf2(("CRTDLL: _spawnve\n"));
  return (_spawnve(i, s1, s2, s3));
}


/*********************************************************************
 *           CRTDLL__spawnvp 	 (CRTDLL.275)
 */
int CDECL CRTDLL__spawnvp( int i, char *s1, char ** s2 )
{
  dprintf2(("CRTDLL: _spawnvp\n"));
  return (_spawnvp(i, s1, s2));
}

/*********************************************************************
 *           CRTDLL__spawnv 	 (CRTDLL.276)
 */
int CDECL CRTDLL__spawnvpe( int i, char *s1, char ** s2, char ** s3 )
{
  dprintf2(("CRTDLL: _spawnvpe\n"));
  return (_spawnvpe(i, s1, s2, s3));
}


/*********************************************************************
 *           CRTDLL__stat   	 (CRTDLL.278)
 */
int CDECL CRTDLL__stat( const char *s1, struct stat * n )
{
  dprintf2(("CRTDLL: _stat\n"));
  return(_stat(s1, n));
}


/*********************************************************************
 *           CRTDLL__statusfp	 (CRTDLL.279)
 */
unsigned int CDECL CRTDLL__statusfp( void )
{
  dprintf2(("CRTDLL: _statusfp\n"));
  return (_status87());
}


/*********************************************************************
 *           CRTDLL__strdate   	 (CRTDLL.281)
 */
char * CDECL CRTDLL__strdate( char *buf )
{
  dprintf2(("CRTDLL: _strdate\n"));
  return(_strdate(buf));
}


/*********************************************************************
 *           CRTDLL__strdec    	 (CRTDLL.282)
 */
char * CDECL CRTDLL__strdec( const char *, const char *p ) 
{
  dprintf2(("CRTDLL: _strdec\n"));
  return( (char *)(p-1) );
}


/*********************************************************************
 *           CRTDLL__strdup    	 (CRTDLL.283)
 */
LPSTR CDECL CRTDLL__strdup(LPCSTR ptr)
{
  dprintf2(("CRTDLL: _strdup\n"));
  return HEAP_strdupA(GetProcessHeap(),0,ptr);
}


/*********************************************************************
 *           _strerror  	 (CRTDLL.284)
 */
char * CDECL CRTDLL__strerror(const char *s)
{
  dprintf(("CRTDLL: _strerror not implemented\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
//  return (_strerror(s));
}


/*********************************************************************
 *           CRTDLL__stricoll  	 (CRTDLL.286)
 */
int CDECL CRTDLL__stricoll( const char *s1, const char *s2 )
{
  dprintf2(("CRTDLL: _stricoll\n"));
  return stricmp(s1,s2);
}


/*********************************************************************
 *           CRTDLL__strinc  	 (CRTDLL.287)
 */
char * CDECL CRTDLL__strinc( const char *p )
{
    dprintf2(("CRTDLL: _strinc\n"));
    return( (char *)(p+1) );
}


/*********************************************************************
 *           CRTDLL__strncnt	 (CRTDLL.289)
 */
size_t CDECL CRTDLL__strncnt( const char *p, size_t l ) 
{
    dprintf2(("CRTDLL: _strncnt\n"));
    size_t i;
    i = strlen(p);
    return( (i>l) ? l : i );
}

/*********************************************************************
 *           CRTDLL__strnextc	 (CRTDLL.290)
 */
unsigned int CDECL CRTDLL__strnextc( const char *p )
{
    dprintf2(("CRTDLL: _strnextc\n"));
    return( (unsigned int)*p );
}


/*********************************************************************
 *           CRTDLL__strninc	 (CRTDLL.292)
 */
char * CDECL CRTDLL__strninc( const char *p, size_t l )
{
    dprintf2(("CRTDLL: _strninc\n"));
    return( (char *)(p+l) );
}


/*********************************************************************
 *           CRTDLL__strnset	 (CRTDLL.293)
 */
char * CDECL CRTDLL__strnset(char* szToFill, int szFill, size_t sizeMaxFill)
{
  dprintf2(("CRTDLL: _strnset\n"));
  char *t = szToFill;
  int i = 0;
  while( *szToFill != 0 && i < sizeMaxFill)
  {
 	*szToFill = szFill;
	szToFill++;
	i++;
  }
  return t;
}


/*********************************************************************
 *           CRTDLL__strrev	 (CRTDLL.294)
 */
char * CDECL CRTDLL__strrev( char *s )
{
  dprintf2(("CRTDLL: _strrev\n"));
  char  *e;
  char   a;
  e=s;
  while (*e)
	e++;
  while (s<e) {
 	a=*s;
	*s=*e;
	*e=a;
	s++;
	e--;
  }
  return s;
}


/*********************************************************************
 *           CRTDLL__strset	 (CRTDLL.295)
 */
char * CDECL CRTDLL__strset(char* szToFill, int szFill)
{
  dprintf2(("CRTDLL: _strset\n"));
  char *t = szToFill;
  while( *szToFill != 0 )
  {
 	*szToFill = szFill;
	szToFill++;
  }
  return t;
}


/*********************************************************************
 *           CRTDLL__strspnp	 (CRTDLL.296)
 */
char * CDECL CRTDLL__strspnp( const char *p1, const char *p2 ) 
{
    dprintf2(("CRTDLL: _strspnp\n"));
    return( (*(p1 += strspn(p1,p2))!='\0') ? (char*)p1 : NULL );
}


/*********************************************************************
 *           CRTDLL__strtime	 (CRTDLL.297)
 */
char * CDECL CRTDLL__strtime( char *buf )
{
  dprintf2(("CRTDLL: _strtime\n"));
  return (_strtime(buf));
}


/*********************************************************************
 *           CRTDLL__swab	 (CRTDLL.299)
 */
void CDECL CRTDLL__swab(char *s1, char *s2, int i)
{
  dprintf2(("CRTDLL: _swab\n"));
  _swab(s1, s2, i);
}


/*********************************************************************
 *           CRTDLL__tell	 (CRTDLL.302)
 */
long CDECL CRTDLL__tell( int i )
{
  dprintf2(("CRTDLL: _tell\n"));
  return (_tell(i));
}


/*********************************************************************
 *           CRTDLL__tempnam	 (CRTDLL.303)
 */
char * CDECL CRTDLL__tempnam( char *dir, char *prefix )
{
  dprintf2(("CRTDLL: _tempnam\n"));
  return (_tempnam(dir, prefix));
}
	

/*********************************************************************
 *           CRTDLL__tolower	 (CRTDLL.305)
 */
int CDECL CRTDLL__tolower(int n)
{
  dprintf2(("CRTDLL: _tolower\n"));
  return (_tolower(n));
}


/*********************************************************************
 *           CRTDLL__toupper	 (CRTDLL.306)
 */
int CDECL CRTDLL__toupper(int n)
{
  dprintf2(("CRTDLL: _toupper\n"));
  return (_toupper(n));
}


/*********************************************************************
 *           _tzset 	 (CRTDLL.308)
 */
void CDECL CRTDLL__tzset( void )
{
  dprintf(("CRTDLL: _tzset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *           CRTDLL__umask	 (CRTDLL.310)
 */
int CDECL CRTDLL__umask( int i )
{
  dprintf2(("CRTDLL: _umask\n"));
  return (_umask(i));
}


/*********************************************************************
 *           CRTDLL__ungetch	 (CRTDLL.311)
 */
int CDECL CRTDLL__ungetch( int i )
{
  dprintf2(("CRTDLL: _ungetch\n"));
  return (_ungetch(i));
}


/*********************************************************************
 *                  _unlink           (CRTDLL.312)
 */
INT CDECL CRTDLL__unlink(LPCSTR pathname)
{
    dprintf2(("CRTDLL: _unlink\n"));
    int ret=0;
    DOS_FULL_NAME full_name;

    if (!DOSFS_GetFullName( pathname, FALSE, (CHAR*)&full_name )) {
      dprintf2(("CRTDLL_unlink file %s bad name\n",pathname));
      return EOF;
    }
  
    ret=unlink(full_name.long_name);
    dprintf2(("(%s unix %s)\n",
		   pathname,full_name.long_name));
    if(ret)
      dprintf2((" Failed!\n"));

    return ret;
}


/*********************************************************************
 *           CRTDLL__unloaddll	 (CRTDLL.313)
 */
int CDECL CRTDLL__unloaddll(void *handle)
{
  dprintf2(("CRTDLL: _unloaddll\n"));
  return FreeLibrary((HMODULE)handle);
}


/*********************************************************************
 *           CRTDLL__utime	 (CRTDLL.314)
 */
int CDECL CRTDLL__utime( char *path, struct utimbuf * times )
{
  dprintf2(("CRTDLL: _utime\n"));
  return (_utime(path, times));
}


/*********************************************************************
 *           CRTDLL__vsnwprintf	 (CRTDLL.316)
 */
int CDECL CRTDLL__vsnwprintf( wchar_t *s1, size_t n, const wchar_t *s2, va_list arg )
{
  dprintf(("CRTDLL: _vsnwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__wcsdup	 (CRTDLL.317)
 */
LPWSTR CDECL CRTDLL__wcsdup( LPCWSTR str )
{
  dprintf2(("CRTDLL: _wcsdup\n"));
  LPWSTR ret = NULL;
  if (str)
  {
      int size = (wcslen((const wchar_t*)str) + 1) * sizeof(WCHAR);
// FIXME      ret = CRTDLL_malloc( size );
      if (ret) memcpy( ret, str, size );
  }
  return ret;
}


/*********************************************************************
 *           CRTDLL__wcsicoll	 (CRTDLL.319)
 */
int CDECL CRTDLL__wcsicoll( LPCWSTR str1, LPCWSTR str2 )
{
  dprintf2(("CRTDLL: _wcsicoll\n"));
  return CRTDLL__wcsicmp( str1, str2 );
}


/*********************************************************************
 *           CRTDLL__wcsnset    (CRTDLL.322)
 */
LPWSTR CDECL CRTDLL__wcsnset( LPWSTR str, WCHAR c, INT n )
{
    dprintf2(("CRTDLL: _wcsnset\n"));
    LPWSTR ret = str;
    while ((n-- > 0) && *str) *str++ = c;
    return ret;
}


/*********************************************************************
 *           CRTDLL__wcsrev	 (CRTDLL.323)
 */
LPWSTR CDECL CRTDLL__wcsrev( LPWSTR str )
{
  dprintf2(("CRTDLL: _wcsrev\n"));
  LPWSTR ret = str;
  LPWSTR end = str + wcslen((const wchar_t*)str) - 1;
  while (end > str)
  {
      WCHAR t = *end;
      *end--  = *str;
      *str++  = t;
  }
  return ret;
}


/*********************************************************************
 *           CRTDLL__wcsset    (CRTDLL.324)
 */
LPWSTR CDECL CRTDLL__wcsset( LPWSTR str, WCHAR c )
{
    dprintf2(("CRTDLL: _wcsset\n"));
    LPWSTR ret = str;
    while (*str) *str++ = c;
    return ret;
}


/*********************************************************************
 *                  _write        (CRTDLL.329)
 */
INT CDECL CRTDLL__write(INT fd,LPCVOID buf,UINT count)
{
        dprintf2(("CRTDLL: _write\n"));
        INT len=0;

	if (fd == -1)
	  len = -1;
	else if (fd<=2)
	  len = (UINT)write(fd,buf,(LONG)count);
	else
	  len = _lwrite(fd,(LPCSTR)buf,count);
	dprintf2(("%d/%d byte to dfh %d from %p,\n",
		       len,count,fd,buf));
	return len;
}


/*********************************************************************
 *                  _y0     (CRTDLL.332)
 */
double CDECL CRTDLL__y0(double x)
{
  dprintf2(("CRTDLL: _y0\n"));
  return (_y0(x));
}


/*********************************************************************
 *                  _y1     (CRTDLL.333)
 */
double CDECL CRTDLL__y1(double x)
{
  dprintf2(("CRTDLL: _y1\n"));
  return (_y1(x));
}


/*********************************************************************
 *                  _yn     (CRTDLL.334)
 */
double CDECL CRTDLL__yn(int i, double x)
{
  dprintf2(("CRTDLL: _yn\n"));
  return (_yn(i, x));
}


/*********************************************************************
 *                  isleadbyte	(CRTDLL.335)
 */
void CDECL CRTDLL_abort( void )
{
  dprintf2(("CRTDLL: abort\n"));
  abort();
}


/*********************************************************************
 *                  acos	(CRTDLL.336)
 */
double CDECL CRTDLL_acos( double x )
{
  dprintf2(("CRTDLL: acos\n"));
  return (acos(x));
}


/*********************************************************************
 *                  asctime	(CRTDLL.338)
 */
char * CDECL CRTDLL_asctime( const struct tm *timeptr )
{
  dprintf2(("CRTDLL: asctime\n"));
  return (asctime(timeptr));
}


/*********************************************************************
 *                  asin	(CRTDLL.339)
 */
double CDECL CRTDLL_asin( double x )
{
  dprintf2(("CRTDLL: asin\n"));
  return (asin(x));
}


/*********************************************************************
 *                  atan2	(CRTDLL.341)
 */
double CDECL CRTDLL_atan2( double y, double x )
{
  dprintf2(("CRTDLL: atan2\n"));
  return (atan2(y, x));
}


/*********************************************************************
 *                  atexit	(CRTDLL.342)
 */
int CDECL CRTDLL_atexit( register void ( *func )( void ) )
{
  dprintf(("CRTDLL: atexit not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  atof 	(CRTDLL.343)
 */
double CDECL CRTDLL_atof( const char *nptr )
{
  dprintf2(("CRTDLL: atof\n"));
  return (atof(nptr));
}

/*********************************************************************
 *                  calloc	(CRTDLL.347)
 */
void * CDECL CRTDLL_calloc( size_t n, size_t size )
{
//  dprintf2(("CRTDLL: calloc\n"));
  return Heap_Alloc(size*n);
}


/*********************************************************************
 *                  clearerr	(CRTDLL.349)
 */
void CDECL CRTDLL_clearerr( FILE *fp )
{
  dprintf2(("CRTDLL: clearerr\n"));
  clearerr(fp);
}


/*********************************************************************
 *                  clock	(CRTDLL.350)
 */
clock_t CDECL CRTDLL_clock( void )
{
  dprintf2(("CRTDLL: clock\n"));
  return (clock());
}


/*********************************************************************
 *                  cosh	(CRTDLL.352)
 */
double CDECL CRTDLL_cosh( double x )
{
  dprintf2(("CRTDLL: cosh\n"));
  return (cosh(x));
}


/*********************************************************************
 *                  ctime	(CRTDLL.353)
 */
char * CDECL CRTDLL_ctime( const time_t *timer )
{
  dprintf2(("CRTDLL: ctime\n"));
  return (ctime(timer));
}


/*********************************************************************
 *                  difftime	(CRTDLL.354)
 */
double CDECL CRTDLL_difftime( time_t t1, time_t t0 )
{
  dprintf2(("CRTDLL: difftime\n"));
  return (difftime(t1, t0));
}


/*********************************************************************
 *                  div		(CRTDLL.355)
 */
div_t CDECL CRTDLL_div( int numer, int denom )
{
  dprintf2(("CRTDLL: div\n"));
  return (div(numer, denom));
}


/*********************************************************************
 *                  exit          (CRTDLL.356)
 */
void CDECL CRTDLL_exit(DWORD ret)
{
	dprintf2(("CRTDLL: exit\n"));
	ExitProcess(ret);
}


/*********************************************************************
 *                  exp		(CRTDLL.357)
 */
double CDECL CRTDLL_exp( double x )
{
  dprintf2(("CRTDLL: exp\n"));
  return (exp(x));
}


/*********************************************************************
 *                  fclose	(CRTDLL.359)
 */
int CDECL CRTDLL_fclose( FILE *fp )
{
  dprintf2(("CRTDLL: fclose\n"));
  return (fclose(fp));
}


/*********************************************************************
 *                  feof	(CRTDLL.360)
 */
int CDECL CRTDLL_feof( FILE *fp )
{
  dprintf2(("CRTDLL: feof\n"));
  return (feof(fp));
}


/*********************************************************************
 *                  ferror	(CRTDLL.361)
 */
int CDECL CRTDLL_ferror( FILE *fp )
{
  dprintf2(("CRTDLL: ferror\n"));
  return (ferror(fp));
}


/*********************************************************************
 *                  fflush	(CRTDLL.362)
 */
int CDECL CRTDLL_fflush( FILE *fp )
{
  dprintf2(("CRTDLL: fflush\n"));
  return (fflush(fp));
}


/*********************************************************************
 *                  fgetc 	(CRTDLL.363)
 */
int CDECL CRTDLL_fgetc( FILE *fp )
{
  dprintf2(("CRTDLL: fgetc\n"));
  return (fgetc(fp));
}


/*********************************************************************
 *                  fgetpos	(CRTDLL.364)
 */
int CDECL CRTDLL_fgetpos( FILE *fp, fpos_t *pos )
{
  dprintf2(("CRTDLL: fgetpos\n"));
  return (fgetpos(fp, pos));
}


/*********************************************************************
 *                  fgets	(CRTDLL.365)
 */
char * CDECL CRTDLL_fgets( char *s, int n, FILE *fp )
{
  dprintf2(("CRTDLL: fgets\n"));
  return (fgets(s, n, fp));
}


/*********************************************************************
 *                  fgetwc	(CRTDLL.366)
 */
wint_t CDECL CRTDLL_fgetwc( FILE *f )
{
  dprintf2(("CRTDLL: fgetwc\n"));
  return (fgetwc(f));
}


/*********************************************************************
 *                  fmod	(CRTDLL.368)
 */
double CDECL CRTDLL_fmod(double x, double y )
{
  dprintf2(("CRTDLL: fmod\n"));
  return (fmod(x,y));
}


/*********************************************************************
 *                  fopen	(CRTDLL.369)
 */
FILE * CDECL CRTDLL_fopen( const char *filename, const char *mode )
{
  dprintf2(("CRTDLL: fopen\n"));
  return (fopen( filename, mode));
}


/*********************************************************************
 *                  fprintf       (CRTDLL.370)
 */
INT CDECL CRTDLL_fprintf( CRTDLL_FILE *file, LPSTR format, ... )
{
    dprintf2(("CRTDLL: fprintf\n"));
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
  dprintf2(("CRTDLL: fputc\n"));
  return (fputc(c, fp));
}


/*********************************************************************
 *                  fputs   (CRTDLL.372)
 */
int CDECL CRTDLL_fputs( const char *s, FILE *fp )
{
  dprintf2(("CRTDLL: fputs\n"));
  return (fputs(s, fp));
}


/*********************************************************************
 *                  fputwc  (CRTDLL.373)
 */
wint_t CDECL CRTDLL_fputwc( wint_t wc, FILE *strm )
{
  dprintf2(("CRTDLL: fputwc\n"));
  return (fputwc(wc, strm));
}


/*********************************************************************
 *                  fread  (CRTDLL.374)
 */
size_t CDECL CRTDLL_fread( void *ptr, size_t size, size_t n, FILE *fp )
{
//  dprintf2(("CRTDLL: fread\n"));
  return (fread(ptr, size, n, fp));
}

 
/*********************************************************************
 *                  free          (CRTDLL.375)
 */
VOID CDECL CRTDLL_free(LPVOID ptr)
{
//    dprintf2(("CRTDLL: free\n"));
    Heap_Free(ptr);
}


/*********************************************************************
 *                  freopen	  (CRTDLL.376)
 */
FILE * CDECL CRTDLL_freopen( const char *filename, const char *mode, FILE *fp )
{
  dprintf2(("CRTDLL: freopen\n"));
  return (freopen(filename, mode, fp));
}


/*********************************************************************
 *                  frexp	  (CRTDLL.377)
 */
double CDECL CRTDLL_frexp( double value, int *exp )
{
  dprintf2(("CRTDLL: frexp\n"));
  return (frexp(value, exp));
}


/*********************************************************************
 *                  fscanf	  (CRTDLL.378)
 */
int CDECL CRTDLL_fscanf( FILE*fp, const char *format, ... )
{
  dprintf2(("CRTDLL: fscanf\n"));
#if 0
    va_list valist;
    INT res;

    va_start( valist, format );
#ifdef HAVE_VFSCANF
    res = vfscanf( xlat_file_ptr(stream), format, valist );
#endif
    va_end( valist );
    return res;
#endif
    dprintf2(("broken\n"));
    return 0;
}


/*********************************************************************
 *                  fseek 	  (CRTDLL.379)
 */
int CDECL CRTDLL_fseek( FILE *file, long int offset, int whence )
{
  dprintf2(("CRTDLL: fseek\n"));
  dprintf2(("file %p to 0x%08lx pos %s\n",
        file,offset,(whence==SEEK_SET)?"SEEK_SET":
        (whence==SEEK_CUR)?"SEEK_CUR":
        (whence==SEEK_END)?"SEEK_END":"UNKNOWN"));
// FIXME if (SetFilePointer( file->handle, offset, NULL, whence ) != 0xffffffff)
// FIXME     return 0;
  dprintf2((" failed!\n"));
  return -1;
}


/*********************************************************************
 *                  fsetpos	  (CRTDLL.380)
 */
int CDECL CRTDLL_fsetpos( FILE *fp, const fpos_t *pos )
{
  dprintf2(("CRTDLL: fsetpos\n"));
  return (fsetpos(fp, pos));
}


/*********************************************************************
 *                  ftell  	  (CRTDLL.381)
 */
long int CDECL CRTDLL_ftell( FILE *fp )
{
  dprintf2(("CRTDLL: ftell\n"));
  return (ftell(fp));
}


/*********************************************************************
 *                  fwprintf 	  (CRTDLL.382)
 */
int CDECL CRTDLL_fwprintf( FILE *iop, const wchar_t *fmt, ... )
{
  dprintf(("CRTDLL: fwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  fwrite     (CRTDLL.383)
 */
DWORD CDECL CRTDLL_fwrite( LPVOID ptr, INT size, INT nmemb, CRTDLL_FILE *file )
{
    DWORD ret;

    dprintf2(("CRTDLL: fwrite\n"));
    if (!WriteFile( file->handle, ptr, size * nmemb, &ret, NULL ))
        dprintf2((" failed!\n"));

    return ret / size;
}


/*********************************************************************
 *                  fwscanf 	  (CRTDLL.384)
 */
int CDECL CRTDLL_fwscanf( FILE *strm, const wchar_t *format, ... )
{
  dprintf(("CRTDLL: fwscanf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  getc    (CRTDLL.385)
 */
int CDECL CRTDLL_getc( FILE *fp )
{
  dprintf2(("CRTDLL: getc\n"));
  return (getc(fp));
}


/*********************************************************************
 *                  getchar    (CRTDLL.386)
 */
int CDECL CRTDLL_getchar( void )
{
  dprintf2(("CRTDLL: getchar\n"));
  return (getchar());
}


/*********************************************************************
 *                  getenv    (CRTDLL.387)
 */
char * CDECL CRTDLL_getenv( const char *name )
{
  dprintf2(("CRTDLL: getenv\n"));
  return (getenv(name));
}


/*********************************************************************
 *                  gets    (CRTDLL.388)
 */
char * CDECL CRTDLL_gets( char *s )
{
  dprintf2(("CRTDLL: gets\n"));
  return (gets(s));
}


/*********************************************************************
 *                  gmtime    (CRTDLL.389)
 */
struct tm * CDECL CRTDLL_gmtime( const time_t *timer )
{
  dprintf2(("CRTDLL: gmtime\n"));
  return (gmtime(timer));
}


/*********************************************************************
 *                  is_wctype    (CRTDLL.390)
 */
INT CDECL CRTDLL_is_wctype(wint_t wc, wctype_t wctypeFlags)
{
        dprintf2(("CRTDLL: is_wctype\n"));
	return ((CRTDLL_pwctype_dll[(unsigned char)(wc & 0xFF)]&wctypeFlags) == wctypeFlags );
}


/*********************************************************************
 *                  isalnum    (CRTDLL.391)
 */
int CDECL CRTDLL_isalnum(int i)
{
  dprintf2(("CRTDLL: isalnum(%08xh)\n", i));
  return (isalnum(i));
}


/*********************************************************************
 *                  iscntrl    (CRTDLL.393)
 */
int CDECL CRTDLL_iscntrl(int i)
{
  dprintf2(("CRTDLL: iscntrl(%08xh)\n", i));
  return (iscntrl(i));
}


/*********************************************************************
 *                  isgraph    (CRTDLL.395)
 */
int CDECL CRTDLL_isgraph(int i)
{
  dprintf2(("CRTDLL: isgraph(%08xh)\n", i));
  return (isgraph(i));
}


/*********************************************************************
 *                  isleadbyte	(CRTDLL.396)
 */
int CDECL CRTDLL_isleadbyte(int i)
{
  dprintf(("CRTDLL: isleadbyte(%08xh) not implemented.\n", i));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ispunct    (CRTDLL.399)
 */
int CDECL CRTDLL_ispunct(int i)
{
  dprintf2(("CRTDLL: ispunct(%08xh)\n", i));
  return (ispunct(i));
}


/*********************************************************************
 *                  iswalnum    (CRTDLL.402)
 */
int CDECL CRTDLL_iswalnum(wint_t i)
{
  dprintf2(("CRTDLL: iswalnum(%08xh)\n", i));
  return (iswalnum(i));
}


/*********************************************************************
 *                  iswascii    (CRTDLL.404)
 */
int CDECL CRTDLL_iswascii(wint_t c)
{
  dprintf2(("CRTDLL: iswascii\n", c));
  return  (!((c)&(~0x7f)));
}


/*********************************************************************
 *                  iswcntrl    (CRTDLL.405)
 */
int CDECL CRTDLL_iswcntrl(wint_t i)
{
  dprintf2(("CRTDLL: iswcntrl(%08xh)\n", i));
  return (iswcntrl(i));
}


/*********************************************************************
 *                  iswdigit    (CRTDLL.407)
 */
int CDECL CRTDLL_iswdigit(wint_t i)
{
  dprintf2(("CRTDLL: iswdigit(%08xh)\n", i));
  return (iswdigit(i));
}


/*********************************************************************
 *                  iswgraph    (CRTDLL.408)
 */
int CDECL CRTDLL_iswgraph(wint_t i)
{
  dprintf2(("CRTDLL: iswgraph(%08xh)\n", i));
  return (iswgraph(i));
}


/*********************************************************************
 *                  iswlower    (CRTDLL.409)
 */
int CDECL CRTDLL_iswlower(wint_t i)
{
  dprintf2(("CRTDLL: iswlower(%08xh)\n", i));
  return (iswlower(i));
}


/*********************************************************************
 *                  iswprint    (CRTDLL.410)
 */
int CDECL CRTDLL_iswprint(wint_t i)
{
  dprintf2(("CRTDLL: iswprint(%08xh)\n", i));
  return (iswprint(i));
}


/*********************************************************************
 *                  iswpunct    (CRTDLL.411)
 */
int CDECL CRTDLL_iswpunct(wint_t i)
{
  dprintf2(("CRTDLL: iswpunct(%08xh)\n", i));
  return (iswpunct(i));
}


/*********************************************************************
 *                  iswspace    (CRTDLL.412)
 */
int CDECL CRTDLL_iswspace(wint_t i)
{
  dprintf2(("CRTDLL: iswspace(%08xh)\n", i));
  return (iswspace(i));
}


/*********************************************************************
 *                  iswupper    (CRTDLL.413)
 */
int CDECL CRTDLL_iswupper(wint_t i)
{
  dprintf2(("CRTDLL: iswupper(%08xh)\n", i));
  return (iswupper(i));
}


/*********************************************************************
 *                  iswxdigit    (CRTDLL.414)
 */
int CDECL CRTDLL_iswxdigit(wint_t i)
{
  dprintf2(("CRTDLL: iswxdigit(%08xh)\n", i));
  return (iswxdigit(i));
}


/*********************************************************************
 *                  ldexp	(CRTDLL.417)
 */
double CDECL CRTDLL_ldexp( double x, int exp ) 
{
  dprintf2(("CRTDLL: ldexp\n"));
  return (ldexp(x, exp));
}


/*********************************************************************
 *                  ldiv	(CRTDLL.418)
 */
ldiv_t CDECL CRTDLL_ldiv( long int numer, long int denom ) 
{
  dprintf2(("CRTDLL: ldiv\n"));
  return (ldiv(numer, denom));
}


/*********************************************************************
 *                  localeconv	(CRTDLL.419)
 */
struct lconv * CDECL CRTDLL_localeconv(void)
{
  dprintf2(("CRTDLL: localeconv\n"));
  return (localeconv());
}


/*********************************************************************
 *                  localtime 	(CRTDLL.420)
 */
struct tm * CDECL CRTDLL_localtime( const time_t *timer )
{
  dprintf2(("CRTDLL: localtime\n"));
  return (localtime(timer));
}


/*********************************************************************
 *                  log10	(CRTDLL.422)
 */
double CDECL CRTDLL_log10( double x )
{
  dprintf2(("CRTDLL: log10\n"));
  return (log10(x));
}


/*********************************************************************
 *                  longjmp        (CRTDLL.423)
 */
VOID CDECL CRTDLL_longjmp(jmp_buf env, int val)
{
    dprintf2(("CRTDLL: longjmp\n"));
    longjmp(env, val);
}


/*********************************************************************
 *                  malloc        (CRTDLL.424)
 */
VOID* CDECL CRTDLL_malloc(DWORD size)
{
//      dprintf2(("CRTDLL: malloc\n"));
      return Heap_Alloc(size);
}


/*********************************************************************
 *                  mblen        (CRTDLL.425)
 */
INT CDECL CRTDLL_mblen( const char *s, size_t n )
{
      dprintf2(("CRTDLL: mblen\n"));
      return (mblen(s, n));
}


/*********************************************************************
 *           CRTDLL_mbtowc    (CRTDLL.427)
 */
INT CDECL CRTDLL_mbtowc( WCHAR *dst, LPCSTR str, INT n )
{
    dprintf2(("CRTDLL: _mbtowc\n"));
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
    dprintf2(("CRTDLL: mktime\n"));
    return mktime( timeptr );
}


/*********************************************************************
 *                  modf   (CRTDLL.434)
 */
double CDECL CRTDLL_modf( double value, double *iptr )
{
    dprintf2(("CRTDLL: modf\n"));
    return modf( value, iptr );
}


/*********************************************************************
 *                  perror   (CRTDLL.435)
 */
void CDECL CRTDLL_perror( const char *s )
{
    dprintf2(("CRTDLL: perror\n"));
    perror( s );
}


/*********************************************************************
 *                  printf   (CRTDLL.437)
 */
int CDECL CRTDLL_printf( const char *format, ... )
{
  dprintf2(("CRTDLL: printf\n"));
  va_list arg;
  int done;

  va_start (arg, format);
  done = vprintf (format, arg);
  va_end (arg);
  return done;
}


/*********************************************************************
 *                  putc      (CRTDLL.438)
 */
int CDECL CRTDLL_putc( int c, FILE *fp )
{
    dprintf2(("CRTDLL: putc\n"));
    return putc( c, fp );
}


/*********************************************************************
 *                  putchar      (CRTDLL.439)
 */
int CDECL CRTDLL_putchar( int c )
{
    dprintf2(("CRTDLL: putchar\n"));
    return putchar( c );
}


/*********************************************************************
 *                  puts         (CRTDLL.440)
 */
int CDECL CRTDLL_puts( const char *s )
{
    dprintf2(("CRTDLL: puts\n"));
    return puts( s );
}


/*********************************************************************
 *                  raise        (CRTDLL.442)
 */
int CDECL CRTDLL_raise( int sig )
{
    dprintf2(("CRTDLL: raise\n"));
    return raise( sig );
}


/*********************************************************************
 *                  rand   (CRTDLL.443)
 */
int CDECL CRTDLL_rand( void )
{
//    dprintf2(("CRTDLL: rand\n"));
    return (rand());
}


/*********************************************************************
 *                  realloc   (CRTDLL.444)
 */
void * CDECL CRTDLL_realloc( void *ptr, size_t size )
{
    dprintf2(("CRTDLL: realloc\n"));
    return HeapReAlloc( GetProcessHeap(), 0, ptr, size );
}


/*********************************************************************
 *                  remove           (CRTDLL.445)
 */
INT CDECL CRTDLL_remove(LPCSTR file)
{
        dprintf2(("CRTDLL: remove\n"));
	if (!DeleteFileA(file))
		return -1;
	return 0;
}


/*********************************************************************
 *                  rename      (CRTDLL.446)
 */
int CDECL CRTDLL_rename (const char *old, const char *new2)
{
  dprintf2(("CRTDLL: rename\n"));
  return (rename(old, new2));
}


/*********************************************************************
 *                  rewind      (CRTDLL.447)
 */
void CDECL CRTDLL_rewind( FILE *fp )
{
  dprintf2(("CRTDLL: rewind\n"));
  rewind(fp);
}


/*********************************************************************
 *                  scanf       (CRTDLL.448)
 */
int CDECL CRTDLL_scanf( const char *format, ... )
{
  dprintf(("CRTDLL: scanf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

	
/*********************************************************************
 *                  setbuf      (CRTDLL.449)
 */
void CDECL CRTDLL_setbuf( FILE *fp, char *buf )	
{
  dprintf2(("CRTDLL: setbuf\n"));
  setbuf(fp, buf);
}


/*********************************************************************
 *                  setlocale      (CRTDLL.450)
 */
char * CDECL CRTDLL_setlocale(int category,const char *locale)
{
  dprintf2(("CRTDLL: setlocale\n"));
  return (setlocale(category, locale));
}


/*********************************************************************
 *                  setvbuf      (CRTDLL.451)
 */
int CDECL CRTDLL_setvbuf( FILE *fp, char *buf, int mode, size_t size )
{
  dprintf2(("CRTDLL: setvbuf\n"));
  return (setvbuf(fp, buf, mode, size));
}


/*********************************************************************
 *                  signal       (CRTDLL.452)
 */
void CDECL CRTDLL_signal( int sig, void (*func)(int)) 
{
    dprintf(("CRTDLL: signal not implemented.\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  sinh         (CRTDLL.454)
 */
double CDECL CRTDLL_sinh( double x )
{
  dprintf2(("CRTDLL: sinh\n"));
  return (sinh(x));
}


/*********************************************************************
 *                  srand        (CRTDLL.457)
 */
void CDECL CRTDLL_srand( unsigned int seed )
{
  dprintf2(("CRTDLL: srand\n"));
  srand(seed);
}


/*********************************************************************
 *                  strcoll        (CRTDLL.462)
 */
int CDECL CRTDLL_strcoll( const char *s1, const char *s2 )
{
  dprintf2(("CRTDLL: strcoll\n"));
  return strcoll(s1, s2);
}


/*********************************************************************
 *                  strerror        (CRTDLL.465)
 */
char * CDECL CRTDLL_strerror( int errnum )
{
  dprintf2(("CRTDLL: strerror\n"));
  return strerror(errnum);
}


/*********************************************************************
 *                  strftime        (CRTDLL.466)
 */
size_t CDECL CRTDLL_strftime( char *s, size_t maxsiz, const char *fmt, const struct tm *tp )
{
  dprintf2(("CRTDLL: strftime\n"));
  return strftime(s, maxsiz, fmt, tp);
}
 

/*********************************************************************
 *                  strtod        (CRTDLL.475)
 */
double CDECL CRTDLL_strtod( const char *nptr, char **endptr )
{
  dprintf2(("CRTDLL: strtod\n"));
  return strtod(nptr, endptr);
}


/*********************************************************************
 *                  strtok        (CRTDLL.476)
 */
char * CDECL CRTDLL_strtok( char *s1, const char *s2 )
{
  dprintf2(("CRTDLL: strtok\n"));
  return strtok(s1, s2);
}


/*********************************************************************
 *                  strtol        (CRTDLL.477)
 */
long int CDECL CRTDLL_strtol( const char *nptr, char **endptr, int base )
{
  dprintf2(("CRTDLL: strtol\n"));
  return strtol(nptr, endptr, base);
}


/*********************************************************************
 *                  strtoul        (CRTDLL.478)
 */
unsigned long CDECL CRTDLL_strtoul( const char *nptr, char **endptr, int base )
{
  dprintf2(("CRTDLL: strtoul\n"));
  return strtoul(nptr, endptr, base);
}


/*********************************************************************
 *                  strxfrm        (CRTDLL.479)
 */
size_t CDECL CRTDLL_strxfrm( char *s1, const char *s2, size_t n )
{
  dprintf2(("CRTDLL: strxfrm\n"));
  return strxfrm(s1, s2, n);
}


/*********************************************************************
 *                  swscanf           (CRTDLL.481)
 */
int CDECL CRTDLL_swscanf( const wchar_t *s1, const wchar_t *s2, ... )
{
  dprintf(("CRTDLL: swscanf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  system         (CRTDLL.482)
 */
int CDECL CRTDLL_system( const char *string )
{
  dprintf2(("CRTDLL: system\n"));
  return system(string);
}


/*********************************************************************
 *                  tanh           (CRTDLL.485)
 */
double CDECL CRTDLL_tanh( double x )
{
  dprintf2(("CRTDLL: tanh\n"));
  return tanh(x);
}


/*********************************************************************
 *                  time           (CRTDLL.485)
 */
time_t CDECL CRTDLL_time( time_t *timer )
{
  dprintf2(("CRTDLL: time\n"));

  return time(timer);
}


/*********************************************************************
 *                  tmpfile           (CRTDLL.486)
 */
FILE * CDECL CRTDLL_tmpfile( void )
{
  dprintf2(("CRTDLL: tmpfile\n"));
  return (tmpfile());
}

	
/*********************************************************************
 *                  tmpnam           (CRTDLL.487)
 */
char * CDECL CRTDLL_tmpnam( char *s )
{
  dprintf2(("CRTDLL: tmpnam\n"));
  return (tmpnam(s));
}


/*********************************************************************
 *                  ungetc           (CRTDLL.492)
 */
INT CDECL CRTDLL_ungetc(int c, FILE *f)
{
  dprintf(("CRTDLL: ungetc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ungetwc           (CRTDLL.493)
 */
wint_t CDECL CRTDLL_ungetwc( wint_t wc, FILE *strm )
{
  dprintf2(("CRTDLL: ungetwc\n"));
  return (ungetwc(wc, strm));
}

/*********************************************************************
 *                  vfprintf       (CRTDLL.494)
 */
INT CDECL CRTDLL_vfprintf( CRTDLL_FILE *file, LPSTR format, va_list args )
{
    dprintf2(("CRTDLL: vprintf\n"));
    char buffer[2048];  /* FIXME... */
    vsprintf( buffer, format, args );
    return CRTDLL_fwrite( buffer, 1, strlen(buffer), file );
}


/*********************************************************************
 *                  vfwprintf     (CRTDLL.495)
 */
int CDECL CRTDLL_vfwprintf( FILE *F, const wchar_t *s, va_list arg )
{
  dprintf(("CRTDLL: vfwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  vprintf       (CRTDLL.496)
 */
int CDECL CRTDLL_vprintf( const char *format, __va_list arg )
{
  dprintf2(("CRTDLL: vprintf\n"));
  return (vprintf(format, arg));
}


/*********************************************************************
 *                  vswprintf       (CRTDLL.498)
 */
int CDECL CRTDLL_vswprintf( wchar_t *s , size_t t, const wchar_t *format, va_list arg )
{
  dprintf2(("CRTDLL: vswprintf\n"));
  return (vswprintf(s, t, format, arg));
}


/*********************************************************************
 *                  vwprintf     (CRTDLL.499)
 */
int CDECL CRTDLL_vwprintf( const wchar_t *s, va_list arg)
{
  dprintf(("CRTDLL: vwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  wcscoll    (CRTDLL.503)
 */
DWORD CDECL CRTDLL_wcscoll(LPCWSTR str1, LPCWSTR str2)
{
  dprintf2(("CRTDLL: wcscoll\n"));
  return (wcscoll((const wchar_t*)str1, (const wchar_t*)str2));
}


/*********************************************************************
 *                  wcsftime   (CRTDLL.506)
 */
size_t CDECL CRTDLL_wcsftime( wchar_t *s, size_t maxsize, 
		const wchar_t *format, const struct tm *timeptr )
{
  dprintf2(("CRTDLL: wcsftime\n"));
  return (wcsftime(s, maxsize, format, timeptr));
}


/*********************************************************************
 *                  wcstod   (CRTDLL.515)
 */
double CDECL CRTDLL_wcstod( const wchar_t *nptr, wchar_t **endptr )
{
  dprintf2(("CRTDLL: wcstod\n"));
  return (wcstod(nptr, endptr));
}


/*********************************************************************
 *                  wcsxfrm   (CRTDLL.520)
 */
size_t CDECL CRTDLL_wcsxfrm( wchar_t *s1, const wchar_t *s2, size_t n )
{
  dprintf2(("CRTDLL: wcsxfrm\n"));
  return (wcsxfrm(s1, s2, n));
}


/*********************************************************************
 *                  wctomb   (CRTDLL.521)
 */
int CDECL CRTDLL_wctomb( LPSTR dst, WCHAR ch )
{
  dprintf2(("CRTDLL: wctomb\n"));
  return (wctomb((char*)dst,ch));
}


/*********************************************************************
 *                  wprintf   (CRTDLL.522)
 */
int CDECL CRTDLL_wprintf( const wchar_t *s, ... )
{
  dprintf(("CRTDLL: wprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  wscanf    (CRTDLL.523)
 */
int CDECL CRTDLL_wscanf( const wchar_t *s, ... )
{
  dprintf(("CRTDLL: wscanf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}



/*********************************************************************
 *                  __set_errno    (INTERNAL-#1)
 */
int __set_errno (int error)
{
	errno = error;
	return error;
}


/*********************************************************************
 *                  _mbbtoupper    (INTERNAL-#2)
 */
unsigned int _mbbtoupper(unsigned int c)
{
	if (!CRTDLL__ismbblead(c) )
		return toupper(c);
	
	return c;
}


/*********************************************************************
 *                  _mbbtolower    (INTERNAL-#3)
 */
unsigned int _mbbtolower(unsigned int c)
{
	if (!CRTDLL__ismbblead(c) )
		return tolower(c);
	return c;
}


/*********************************************************************
 *                  _mbclen2    (INTERNAL-#4)
 */
size_t _mbclen2(const unsigned int s)
{
	return (CRTDLL__ismbblead(s>>8) && CRTDLL__ismbbtrail(s&0x00FF)) ? 2 : 1;
}


/*********************************************************************
 *                  _isinf    (INTERNAL-#5)
 */
int _isinf(double __x)
{
	double_t * x = (double_t *)&__x;
	return ( x->exponent == 0x7ff  && ( x->mantissah == 0 && x->mantissal == 0 ));	
}


/*********************************************************************
 *                  _filehnd    (INTERNAL-#6)
 */
void* filehnd(int fileno)
{
	if ( fileno < 0 )
		return (void *)-1;
#define STD_AUX_HANDLE 3
#define STD_PRINTER_HANDLE 4

	switch(fileno)
	{
	case 0:
		return (void*)GetStdHandle(STD_INPUT_HANDLE);
	case 1:
		return (void*)GetStdHandle(STD_OUTPUT_HANDLE);
	case 2:
		return (void*)GetStdHandle(STD_ERROR_HANDLE);
	case 3:
		return (void*)GetStdHandle(STD_AUX_HANDLE);
	case 4:
		return (void*)GetStdHandle(STD_PRINTER_HANDLE);
	default:
		break;
	}
		
	if ( fileno >= maxfno )
		return (void *)-1;

	if ( fileno_modes[fileno].fd == -1 )
		return (void *)-1;
	return (void*)fileno_modes[fileno].hFile;
}
