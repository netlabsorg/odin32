/* $Id: crtdll.cpp,v 1.7 1999-10-04 09:55:54 sandervl Exp $ */

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
#include <sys\utime.h>
#include <sys\stat.h>

#include <crtdll.h>
#include "crtinc.h"


DEFAULT_DEBUG_CHANNEL(crtdll)


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
 *                  new           (CRTDLL.001)
 */
VOID* CDECL CRTDLL_new(DWORD size)
{
    dprintf(("CRTDLL: ??2@YAPAXI@Z\n"));
    VOID* result;
    if(!(result = HeapAlloc(GetProcessHeap(),0,size)) && new_handler)
	(*new_handler)();
    return result;
}


/*********************************************************************
 *                  delete       (CRTDLL.002)
 */
VOID CDECL CRTDLL_delete(VOID* ptr)
{
    dprintf(("CRTDLL: ??3@YAXPAX@Z\n"));
    HeapFree(GetProcessHeap(),0,ptr);
}


/*********************************************************************
 *                  set_new_handler(CRTDLL.003)
 */
new_handler_type CDECL CRTDLL_set_new_handler(new_handler_type func)
{
    dprintf(("CRTDLL: ?_set_new_handler@@YAP6AHI@ZP6AHI@Z@Z\n"));
    new_handler_type old_handler = new_handler;
    new_handler = func;
    return old_handler;
}


/*********************************************************************
 *                  _CIacos    (CRTDLL.004)
 */
double CDECL CRTDLL__CIacos( double x )
{
  dprintf(("NTDLL: _CIacos\n"));
  dprintf(("should be register function\n"));
  return acos(x);
}


/*********************************************************************
 *                  _CIasin    (CRTDLL.005)
 */
double CDECL CRTDLL__CIasin( double x )
{
  dprintf(("NTDLL: _CIasin\n"));
  dprintf(("should be register function\n"));
  return asin(x);
}


/*********************************************************************
 *                  _CIatan    (CRTDLL.006)
 */
double CDECL CRTDLL__CIatan( double x )
{
  dprintf(("NTDLL: _CIatan\n"));
  dprintf(("should be register function\n"));
  return atan(x);
}


/*********************************************************************
 *                  _CIatan2    (CRTDLL.007)
 */
double CDECL CRTDLL__CIatan2( double x, double y )
{
  dprintf(("NTDLL: _CIatan2\n"));
  dprintf(("should be register function\n"));
  return atan2(x,y);
}


/*********************************************************************
 *                  _CIcos    (CRTDLL.008)
 */
double CDECL CRTDLL__CIcos( double x )
{
  dprintf(("NTDLL: _CIcos\n"));
  dprintf(("should be register function\n"));
  return cos(x);
}


/*********************************************************************
 *                  _CIcosh    (CRTDLL.009)
 */
double CDECL CRTDLL__CIcosh( double x )
{
  dprintf(("NTDLL: _CIcosh\n"));
  dprintf(("should be register function\n"));
  return cosh(x);
}


/*********************************************************************
 *                  _CIexp    (CRTDLL.010)
 */
double CDECL CRTDLL__CIexp( double x )
{
  dprintf(("NTDLL: _CIexp\n"));
  dprintf(("should be register function\n"));
  return exp(x);
}


/*********************************************************************
 *                  _CIfmod     (CRTDLL.011)
 */
double CDECL CRTDLL__CIfmod( double x, double y )
{
  dprintf(("NTDLL: _CIfmod\n"));
  dprintf(("should be register function\n"));
  return fmod(x,y);
}


/*********************************************************************
 *                  _CIlog    (CRTDLL.012)
 */
double CDECL CRTDLL__CIlog( double x )
{
  dprintf(("NTDLL: _CIlog\n"));
  dprintf(("should be register function\n"));
  return log(x);
}


/*********************************************************************
 *                  _CIlog10    (CRTDLL.013)
 */
double CDECL CRTDLL__CIlog10( double x )
{
  dprintf(("NTDLL: _CIlog10\n"));
  dprintf(("should be register function\n"));
  return log10(x);
}


/*********************************************************************
 *                  _CIsin     (CRTDLL.015)
 */
double CDECL CRTDLL__CIsin( double x )
{
  dprintf(("NTDLL: _CIsin\n"));
  dprintf(("should be register function\n"));
  return sin(x);
}


/*********************************************************************
 *                  _CIsinh    (CRTDLL.016)
 */
double CDECL CRTDLL__CIsinh( double x )
{
  dprintf(("NTDLL: _CIsinh\n"));
  dprintf(("should be register function\n"));
  return sinh(x);
}


/*********************************************************************
 *                  _CIsqrt    (CRTDLL.017)
 */
double CDECL CRTDLL__CIsqrt( double x )
{
  dprintf(("NTDLL: _CIsqrt\n"));
  dprintf(("should be register function\n"));
  return acos(x);
}


/*********************************************************************
 *                  _CItan    (CRTDLL.018)
 */
double CDECL CRTDLL__CItan( double x )
{
  dprintf(("NTDLL: _CItan\n"));
  dprintf(("should be register function\n"));
  return tan(x);
}


/*********************************************************************
 *                  _CItanh    (CRTDLL.019)
 */
double CDECL CRTDLL__CItanh( double x )
{
  dprintf(("NTDLL: _CItanh\n"));
  dprintf(("should be register function\n"));
  return tanh(x);
}


/*********************************************************************
 *                  _XcptFilter    (CRTDLL.21)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__XcptFilter(DWORD ret)
{
  dprintf(("CRTDLL: XcptFilter not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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

	i=0;xargv=NULL;xargc=0;afterlastspace=0;
/*
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
*/
	CRTDLL_argc_dll	= xargc;
	*argc		= xargc;
	CRTDLL_argv_dll	= xargv;
	*argv		= xargv;
	dprintf(("CRTDLL: GetMainArgs end\n"));
	CRTDLL_environ_dll = *environ = GetEnvironmentStringsA();
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
long * CDECL CRTDLL___doserrno()
{	
	dprintf(("__doserrno not implemented.\n"));
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
//	return _doserrno();
}


/*********************************************************************
 *           CRTDLL___isascii   (CRTDLL.28)
 */
int CDECL CRTDLL___isascii(int i)
{
  dprintf(("CRTDLL: __isascii not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL___iscsym   (CRTDLL.29)
 */
int CDECL CRTDLL___iscsym(int i)
{
  dprintf(("CRTDLL: __iscsym not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL___iscsymf   (CRTDLL.30)
 */
int CDECL CRTDLL___iscsymf(int i)
{
  dprintf(("CRTDLL: __iscsymf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  __mb_cur_max_dll    (CRTDLL.31)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL___mb_cur_max_dll(DWORD ret)
{
  dprintf(("CRTDLL: __mb_cur_max_dll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL___threadhandle   (CRTDLL.32)
 */
unsigned long CDECL CRTDLL___threadhandle( void )
{
  dprintf(("CRTDLL: __threadhandle  not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL___threadid   (CRTDLL.33)
 */
int * CDECL CRTDLL___threadid(void)
{
  dprintf(("CRTDLL: __threadid  not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
  dprintf(("CRTDLL: _access\n"));
  return (_access(path, mode));
}


/*********************************************************************
 *                  _aexit_rtn_dll    (CRTDLL.39)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__aexit_rtn_dll(DWORD ret)
{
  dprintf(("CRTDLL: _aexit_rtn_dll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _amsg_exit    (CRTDLL.40)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__amsg_exit(DWORD ret)
{
        dprintf(("CRTDLL: _amsg_exit not implemented.\n"));
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
 *                  CRTDLL__beep	            (CRTDLL.45)
 */
void CDECL CRTDLL__beep(unsigned i1, unsigned i2)
{	
	dprintf(("_beep not implemented.\n"));
}


/*********************************************************************
 *           CRTDLL__beginthread   (CRTDLL.46)
 */
unsigned long CDECL CRTDLL__beginthread(  register void (*start_address)(void *),
		unsigned stack_size, void *arglist )
{
  dprintf(("CRTDLL: _beginthread not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _c_exit          (CRTDLL.47)
 *
 */
void CDECL CRTDLL__c_exit(INT ret)
{
        dprintf(("_c_exit(%d)\n",ret));
	ExitProcess(ret);
}


/*********************************************************************
 *           CRTDLL__cabs   (CRTDLL.48)
 */
double CDECL CRTDLL__cabs(struct complex * c)
{
  dprintf(("CRTDLL: _cabs not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
//  return (_cabs(c));
}


/*********************************************************************
 *                  _cexit          (CRTDLL.49)
 *
 */
void CDECL CRTDLL__cexit(INT ret)
{
        dprintf(("_cexit(%d)\n",ret));
	ExitProcess(ret);
}


/*********************************************************************
 *           CRTDLL__cgets  (CRTDLL.50)
 */
char * CDECL CRTDLL__cgets( char *s )
{
  dprintf(("CRTDLL: _cgets\n"));
  return (_cgets(s));
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
 *                  _chdrive           (CRTDLL.52)
 *
 *  newdir      [I] drive to change to, A=1
 *
 */
BOOL CDECL CRTDLL__chdrive(INT newdrive)
{
	/* FIXME: generates errnos */
	dprintf(("CRTDLL: _chdrive\n"));
	return DRIVE_SetCurrentDrive(newdrive-1);
}


/*********************************************************************
 *           CRTDLL__chgsign	 (CRTDLL.53)
 */
double CDECL CRTDLL__chgsign(double x)
{
  dprintf(("CRTDLL: _chgsign not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__chmod   (CRTDLL.54)
 */
int CDECL CRTDLL__chmod( const char *s, int i)
{
  dprintf(("CRTDLL: _chmod\n"));
  return (_chmod(s, i));
}


/*********************************************************************
 *           CRTDLL__chsize   (CRTDLL.55)
 */
int CDECL CRTDLL__chsize( int i, long l )
{
  dprintf(("CRTDLL: _chsize\n"));
  return (_chsize(i, l));
}


/*********************************************************************
 *           CRTDLL__clearfp  (CRTDLL.56)
 */
unsigned int CDECL CRTDLL__clearfp( void )
{
  dprintf(("CRTDLL: _clearfp\n"));
  return (_clear87());
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
 *                  CRTDLL__controlfp    (CRTDLL.61)
 */
unsigned CDECL CRTDLL__controlfp(unsigned i1,unsigned i2)
{
  dprintf(("CRTDLL: _controlfp\n"));
  return (_control87(i1, i2));
}


/*********************************************************************
 *                  CRTDLL__copysign    (CRTDLL.62)
 */
double CDECL CRTDLL__copysign( double x, double y )
{
  dprintf(("CRTDLL: _copysign not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  CRTDLL__cprintf    (CRTDLL.63)
 */
INT CDECL CRTDLL__cprintf( char *s, ... )
{
  dprintf(("CRTDLL: _cprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  CRTDLL__cputs      (CRTDLL.65)
 */
INT CDECL CRTDLL__cputs( char * s )
{
  dprintf(("CRTDLL: _cputs\n"));
  return (_cputs(s));
}


/*********************************************************************
 *                  CRTDLL__creat      (CRTDLL.66)
 */
INT CDECL CRTDLL__creat( const char *s, int i )
{
  dprintf(("CRTDLL: _creat\n"));
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
 *                  _except_handler2  (CRTDLL.78)
 */
INT CDECL CRTDLL__except_handler2 ( PEXCEPTION_RECORD rec,
	PEXCEPTION_FRAME frame, PCONTEXT context, 
	PEXCEPTION_FRAME  *dispatcher)
{
        dprintf(("CRTDLL: _except_handler2\n"));
	return 1;
}


/*********************************************************************
 *           CRTDLL__execl   (CRTDLL.79)
 */
int CDECL CRTDLL__execl( char *s1, char *s2, ...)
{
  dprintf(("CRTDLL: _execl not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__execle   (CRTDLL.80)
 */
int CDECL CRTDLL__execle( char *s1, char *s2, ...)
{
  dprintf(("CRTDLL: _execle not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__execlp   (CRTDLL.81)
 */
int CDECL CRTDLL__execlp( char *s1, char *s2, ...)
{
  dprintf(("CRTDLL: _execlp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__execlpe   (CRTDLL.82)
 */
int CDECL CRTDLL__execlpe( char *s1, char *s2, ...)
{
  dprintf(("CRTDLL: _execlpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__execv   (CRTDLL.83)
 */
int CDECL CRTDLL__execv( char *s1, char **s2)
{
  dprintf(("CRTDLL: _execv\n"));
  return (_execv(s1, s2));
}


/*********************************************************************
 *           CRTDLL__execve   (CRTDLL.84)
 */
int CDECL CRTDLL__execve( char *s1, char **s2, char **s3)
{
  dprintf(("CRTDLL: _execve\n"));
  return (_execve(s1, s2, s3));
}


/*********************************************************************
 *           CRTDLL__execvp   (CRTDLL.85)
 */
int CDECL CRTDLL__execvp( char *s1, char **s2)
{
  dprintf(("CRTDLL: _execvp\n"));
  return (_execvp(s1, s2));
}


/*********************************************************************
 *           CRTDLL__execvpe   (CRTDLL.86)
 */
int CDECL CRTDLL__execvpe( char *s1, char **s2, char **s3)
{
  dprintf(("CRTDLL: _execvpe\n"));
  return (_execvpe(s1, s2, s3));
}


/*********************************************************************
 *                  _exit          (CRTDLL.87)
 */
VOID CDECL CRTDLL__exit(DWORD ret)
{
	dprintf(("CRTDLL: _exit\n"));
	ExitProcess(ret);
}


/*********************************************************************
 *           CRTDLL__expand   (CRTDLL.88)
 */
void * CDECL CRTDLL__expand( void *ptr, size_t size )
{
  dprintf(("CRTDLL: _expand not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__fcloseall   (CRTDLL.89)
 */
int CDECL CRTDLL__fcloseall( void )
{
  dprintf(("CRTDLL: _fcloseall\n"));
  return (_fcloseall());
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
  dprintf(("CRTDLL: _fgetwchar not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__filbuf     (CRTDLL.94)
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
  dprintf(("CRTDLL: _filelength\n"));
  return (_filelength(i));
}


/*********************************************************************
 *           CRTDLL__fileno     (CRTDLL.97)
 */
int CDECL CRTDLL__fileno(FILE * f)
{
  dprintf(("CRTDLL: _fileno\n"));
  return (_fileno(f));
}


/*********************************************************************
*                  _findclose    (CRTDLL.098)
*/
int CDECL CRTDLL__findclose( long handle )
{
  dprintf(("CRTDLL: _findclose not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
  dprintf(("CRTDLL: _finite not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
  dprintf(("CRTDLL: _flushall\n"));
  return (_flushall());
}


/*********************************************************************
 *                  _fpclass     (CRTDLL.105)
 */
INT CDECL CRTDLL__fpclass( double x )
{
  dprintf(("CRTDLL: _fpclass not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fpieee_flt     (CRTDLL.106)
 */
INT CDECL CRTDLL__fpieee_flt( unsigned long exc_code, struct _EXCEPTION_POINTERS *exc_info, int handler)
{
  dprintf(("CRTDLL: _fpieee_flt not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}



/*********************************************************************
 *                  _fpreset     (CRTDLL.107)
 */
INT CDECL CRTDLL__fpreset(void)
{
  dprintf(("CRTDLL: _fpreset not implemented.\n"));
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
  dprintf(("CRTDLL: _fputwchar not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fsopen     (CRTDLL.110)
 */
FILE * CDECL CRTDLL__fsopen( const char *filename, const char *mode, int shflag )
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
int CDECL CRTDLL__ftime( struct timeb *timeptr )
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
  dprintf(("CRTDLL: _fullpath\n"));
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
  dprintf(("CRTDLL: _gcvt\n"));
  return (_gcvt(val, ndig, buf));
}


/*********************************************************************
 *                  _get_osfhandle     (CRTDLL.117)
 */
long CDECL CRTDLL__get_osfhandle( int posixhandle )
{
  dprintf(("CRTDLL: _gcvt not implemented.\n"));
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
  dprintf(("CRTDLL: _getdiskfree not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getdrive    (CRTDLL.124)
 */
unsigned CDECL CRTDLL__getdrive( void )
{
  dprintf(("CRTDLL: _getdrive\n"));
  return DRIVE_GetCurrentDrive() + 1;
}


/*********************************************************************
 *                  _getdrives    (CRTDLL.125)
 */
unsigned long CDECL CRTDLL__getdrives(void)
{
  dprintf(("CRTDLL: _getdrives not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getpid    (CRTDLL.126)
 */
int CDECL CRTDLL__getpid( void )
{
  dprintf(("CRTDLL: _getpid\n"));
  return (_getpid());
}


/*********************************************************************
 *                  _getw     (CRTDLL.128)
 */
int CDECL CRTDLL__getw( FILE *fp )
{
  dprintf(("CRTDLL: _getw not implemented.\n"));
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
  dprintf(("CRTDLL: _heapwalk not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _hypot     (CRTDLL.134)
 */
double CDECL CRTDLL__hypot(double x1, double x2)
{
  dprintf(("CRTDLL: _hypot not implemented.\n"));
  return (_hypot(x1, x2));
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
 *                  _isatty       (CRTDLL.137)
 */
BOOL CDECL CRTDLL__isatty(DWORD x)
{
   dprintf(("(%ld)\n",x));
   return TRUE;
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
  dprintf(("CRTDLL: _ismbbalnum not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbalpha     (CRTDLL.140)
 */
int CDECL CRTDLL__ismbbalpha( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbalpha not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbgraph     (CRTDLL.141)
 */
int CDECL CRTDLL__ismbbgraph( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbgraph not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkalnum     (CRTDLL.142)
 */
int CDECL CRTDLL__ismbbkalnum( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbkalnum not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkana     (CRTDLL.143)
 */
int CDECL CRTDLL__ismbbkana( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbkana not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkpunct     (CRTDLL.144)
 */
int CDECL CRTDLL__ismbbkpunct( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbkpunct not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbblead     (CRTDLL.145)
 */
int CDECL CRTDLL__ismbblead( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbblead not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbprint     (CRTDLL.146)
 */
int CDECL CRTDLL__ismbbprint( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbprint not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbpunct     (CRTDLL.147)
 */
int CDECL CRTDLL__ismbbpunct( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbpunct not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbtrail     (CRTDLL.148)
 */
int CDECL CRTDLL__ismbbtrail( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbbtrail not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcalpha     (CRTDLL.149)
 */
int CDECL CRTDLL__ismbcalpha( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcalpha not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcdigit     (CRTDLL.150)
 */
int CDECL CRTDLL__ismbcdigit( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcdigit not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbchira     (CRTDLL.151)
 */
int CDECL CRTDLL__ismbchira( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbchira not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbckata     (CRTDLL.152)
 */
int CDECL CRTDLL__ismbckata( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbckata not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/*********************************************************************
 *                  _ismbcl0     (CRTDLL.153)
 */
int CDECL CRTDLL__ismbcl0( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl0 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcl1     (CRTDLL.154)
 */
int CDECL CRTDLL__ismbcl1( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl1 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcl2     (CRTDLL.155)
 */
int CDECL CRTDLL__ismbcl2( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl2 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbclegal     (CRTDLL.156)
 */
int CDECL CRTDLL__ismbclegal( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbclegal not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbclower     (CRTDLL.157)
 */
int CDECL CRTDLL__ismbclower( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbclower not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcprint     (CRTDLL.158)
 */
int CDECL CRTDLL__ismbcprint( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcprint not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcspace     (CRTDLL.159)
 */
int CDECL CRTDLL__ismbcspace( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcspace not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcsymbol     (CRTDLL.160)
 */
int CDECL CRTDLL__ismbcsymbol( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcsymbol not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcupper     (CRTDLL.161)
 */
int CDECL CRTDLL__ismbcupper( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcupper not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbslead     (CRTDLL.162)
 */
int CDECL CRTDLL__ismbslead(const unsigned char *s1, const unsigned char *s2)
{
  dprintf(("CRTDLL: _ismbslead not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbstrail     (CRTDLL.163)
 */
int CDECL CRTDLL__ismbstrail(const unsigned char *s1, const unsigned char *s2)
{
  dprintf(("CRTDLL: _ismbstrail not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _isnan     (CRTDLL.164)
 */
int CDECL CRTDLL__isnan( double x )
{
  dprintf(("CRTDLL: _isnan not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _j0     (CRTDLL.166)
 */
double CDECL CRTDLL__j0(double x)
{
  dprintf(("CRTDLL: _j0\n"));
  return (_j0(x));
}


/*********************************************************************
 *                  _j1     (CRTDLL.167)
 */
double CDECL CRTDLL__j1(double x)
{
  dprintf(("CRTDLL: _j1\n"));
  return (_j1(x));}


/*********************************************************************
 *                  _jn     (CRTDLL.168)
 */
double CDECL CRTDLL__jn(int i, double x)
{
  dprintf(("CRTDLL: _jn\n"));
  return (_jn(i, x));
}


/*********************************************************************
 *                  _kbhit     (CRTDLL.169)
 */
int CDECL CRTDLL__kbhit( void )
{
  dprintf(("CRTDLL: _kbhit\n"));
  return (_kbhit());
}


/*********************************************************************
 *                  _lfind     (CRTDLL.170)
 */
void * CDECL CRTDLL__lfind(const void *v1, const void *v2, unsigned int *i1, unsigned int i2,
        int (CDECL *i3)(const void *v3, const void *v4))
{
  dprintf(("CRTDLL: _lfind not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
//  return (_lfind(v1,v2,i1,i2,i3(v3,v4)));
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
 *                  _lsearch   (CRTDLL.177)
 */
void * CDECL CRTDLL__lsearch(const void *v1, void  *v2, unsigned int *i1, unsigned int i2,
        int (CDECL *i3)(const void *v3, const void *v4))
{
  dprintf(("CRTDLL: _lsearch not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
//  return (_lsearch(v1,v2,i1,i2,i3(v3,v4)));
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


#if (__IBMCPP__ > 300)
#define exception _exception
#endif

/*********************************************************************
 *                  _matherr	(CRTDLL.181)
 */
double CDECL CRTDLL__matherr( struct exception * excep )
{
  dprintf(("CRTDLL: _matherr\n"));
  return (_matherr(excep));
}


/*********************************************************************
 *                  _mbbtombc        (CRTDLL.182)
 */
unsigned int CDECL CRTDLL__mbbtombc( unsigned int ch )
{
  dprintf(("CRTDLL: _mbbtombc\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbbtype        (CRTDLL.183)
 */
int CDECL CRTDLL__mbbtype( unsigned char s, int i )
{
  dprintf(("CRTDLL: _mbbtype not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbccpy        (CRTDLL.184)
 */
void CDECL CRTDLL__mbccpy( unsigned char *dest, const unsigned char *ch )
{
  dprintf(("CRTDLL: _mbccpy not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _mbcjistojms     (CRTDLL.185)
 */
int CDECL CRTDLL__mbcjistojms( unsigned int ch )
{
  dprintf(("CRTDLL: _mbcjistojms not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbcjmstojis     (CRTDLL.186)
 */
int CDECL CRTDLL__mbcjmstojis( unsigned int ch )
{
  dprintf(("CRTDLL: _mbcjmstojis not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbclen    (CRTDLL.187)
 */
size_t CDECL CRTDLL__mbclen( const unsigned char *ch )
{
  dprintf(("CRTDLL: _mbclen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctohira     (CRTDLL.188)
 */
int CDECL CRTDLL__mbctohira( unsigned int ch )
{
  dprintf(("CRTDLL: _mbctohira not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctokata     (CRTDLL.189)
 */
int CDECL CRTDLL__mbctokata( unsigned int ch )
{
  dprintf(("CRTDLL: _mbctokata not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctolower     (CRTDLL.190)
 */
unsigned int CDECL CRTDLL__mbctolower( unsigned int ch )
{
  dprintf(("CRTDLL: _mbctolower not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctombb        (CRTDLL.191)
 */
unsigned int CDECL CRTDLL__mbctombb( unsigned int ch )
{
  dprintf(("CRTDLL: _mbctombb not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctoupper     (CRTDLL.192)
 */
unsigned int CDECL CRTDLL__mbctoupper( unsigned int ch )
{
  dprintf(("CRTDLL: _mbctoupper not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbsbtype     (CRTDLL.194)
 */
int CDECL CRTDLL__mbsbtype( const unsigned char *s1, int ch )
{
  dprintf(("CRTDLL: _mbsbtype not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbscat       (CRTDLL.195)
 */
unsigned char * CDECL CRTDLL__mbscat( unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbscat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbschr       (CRTDLL.196)
 */
unsigned char * CDECL CRTDLL__mbschr( const unsigned char *s, unsigned int ch )
{
  dprintf(("CRTDLL: _mbschr not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbscmp       (CRTDLL.197)
 */
int CDECL CRTDLL__mbscmp( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbscmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbscpy       (CRTDLL.198)
 */
unsigned char * CDECL CRTDLL__mbscpy( unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbscpy not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbscspn        (CRTDLL.199)
 */
size_t CDECL CRTDLL__mbscspn( const unsigned char *s, const unsigned char *charset )
{
  dprintf(("CRTDLL: _mbscspn not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsdec    (CRTDLL.200)
 */
unsigned char * CDECL CRTDLL__mbsdec( const unsigned char *s, const unsigned char *ch )
{
  dprintf(("CRTDLL: _mbsdec not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsdec    (CRTDLL.201)
 */
unsigned char * CDECL CRTDLL__mbsdup( unsigned char *src )
{
  dprintf(("CRTDLL: _mbsdup not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsicmp   (CRTDLL.202)
 */
int CDECL CRTDLL__mbsicmp( const unsigned char *x, const unsigned char *y )
{
  dprintf(("CRTDLL: _mbsicmp\n"));
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
    dprintf(("CRTDLL: _mbsinc\n"));
    int len = mblen( str, MB_LEN_MAX );
    if (len < 1) len = 1;
    return (LPSTR)(str + len);
}


/*********************************************************************
 *           CRTDLL__mbslen    (CRTDLL.204)
 */
INT CDECL CRTDLL__mbslen( LPCSTR str )
{
    dprintf(("CRTDLL: _mbslen\n"));
    INT len, total = 0;
    while ((len = mblen( str, MB_LEN_MAX )) > 0)
    {
        str += len;
        total++;
    }
    return total;
}


/*********************************************************************
 *           CRTDLL__mbslwr    (CRTDLL.205)
 */
unsigned char * CDECL CRTDLL__mbslwr( unsigned char *s )
{
  dprintf(("CRTDLL: _mbslwr not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbcat  (CRTDLL.206)
 */
unsigned char * CDECL CRTDLL__mbsnbcat( unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbcmp  (CRTDLL.207)
 */
int CDECL CRTDLL__mbsnbcmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbcnt  (CRTDLL.208)
 */
size_t CDECL CRTDLL__mbsnbcnt( const unsigned char *s, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcnt not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbcpy  (CRTDLL.209)
 */
unsigned char * CDECL CRTDLL__mbsnbcpy( unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcpy not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbicmp (CRTDLL.210)
 */
int CDECL CRTDLL__mbsnbicmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbicmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnbset (CRTDLL.211)
 */
unsigned char * CDECL CRTDLL__mbsnbset( unsigned char *s, unsigned int ch, size_t n )
{
  dprintf(("CRTDLL: _mbsnbset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsncat (CRTDLL.212)
 */
unsigned char * CDECL CRTDLL__mbsncat( unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsncat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnccnt (CRTDLL.213)
 */
size_t CDECL CRTDLL__mbsnccnt( const unsigned char *s, size_t n )
{
  dprintf(("CRTDLL: _mbsnccnt not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsncmp (CRTDLL.214)
 */
int CDECL CRTDLL__mbsncmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsncmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsncpy (CRTDLL.215)
 */
unsigned char * CDECL CRTDLL__mbsncpy( unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsncpy not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnextc (CRTDLL.216)
 */
unsigned int CDECL CRTDLL__mbsnextc( const unsigned char *s )
{
  dprintf(("CRTDLL: _mbsnextc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnicmp (CRTDLL.217)
 */
int CDECL CRTDLL__mbsnicmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnicmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsninc (CRTDLL.218)
 */
unsigned char * CDECL CRTDLL__mbsninc( const unsigned char *s, size_t count )
{
  dprintf(("CRTDLL: _mbsninc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsnset (CRTDLL.219)
 */
unsigned char * CDECL CRTDLL__mbsnset( unsigned char *s, unsigned int ch, size_t n )
{
  dprintf(("CRTDLL: _mbsnset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbspbrk   (CRTDLL.220)
 */
unsigned char * CDECL CRTDLL__mbspbrk( const unsigned char *s, const unsigned char *charset )
{
  dprintf(("CRTDLL: _mbspbrk not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsrchr   (CRTDLL.221)
 */
LPSTR CDECL CRTDLL__mbsrchr(LPSTR s,CHAR x)
{
  dprintf(("CRTDLL: _mbsrchr\n"));
	/* FIXME: handle multibyte strings */
	return strrchr(s,x);
}


/*********************************************************************
 *           CRTDLL__mbsrev    (CRTDLL.222)
 */
unsigned char * CDECL CRTDLL__mbsrev( unsigned char *s )
{
  dprintf(("CRTDLL: _mbsrev not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsset    (CRTDLL.223)
 */
unsigned char * CDECL CRTDLL__mbsset( unsigned char *s, unsigned int ch )
{
  dprintf(("CRTDLL: _mbsset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsspn   (CRTDLL.224)
 */
size_t CDECL CRTDLL__mbsspn( const unsigned char *s, const unsigned char *charset )
{
  dprintf(("CRTDLL: _mbsspn not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsspnp   (CRTDLL.225)
 */
unsigned char * CDECL CRTDLL__mbsspnp( const unsigned char *s, const unsigned char *charset )
{
  dprintf(("CRTDLL: _mbsspnp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsstr    (CRTDLL.226)
 */
unsigned char * CDECL CRTDLL__mbsstr( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbsstr not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbstok    (CRTDLL.227)
 */
unsigned char * CDECL CRTDLL__mbstok( unsigned char *s, const unsigned char *delim )
{
  dprintf(("CRTDLL: _mbstok not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbstrlen   (CRTDLL.228)
 */
size_t CDECL CRTDLL__mbstrlen(const char *s)
{
  dprintf(("CRTDLL: _mbstrlen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__mbsupr    (CRTDLL.229)
 */
unsigned char * CDECL CRTDLL__mbsupr( unsigned char *s )
{
  dprintf(("CRTDLL: _mbsupr not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__memccpy   (CRTDLL.230)
 */
void * CDECL CRTDLL__memccpy(void *v1, const void *v2, int i, size_t s)
{
  dprintf(("CRTDLL: _memccpy not implemented.\n"));
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
  dprintf(("CRTDLL: _mktemp not implemented.\n"));
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
 *                  _nextafter        (CRTDLL.235)
 */
double CDECL CRTDLL__nextafter( double x, double y )
{
  dprintf(("CRTDLL: _nextafter not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _onexit        (CRTDLL.236)
 */
onexit_t CDECL CRTDLL__onexit(onexit_t t)
{
  dprintf(("CRTDLL: _onexit\n"));
  return (_onexit(t));
}


/*********************************************************************
 *                  _open        (CRTDLL.237)
 */
HFILE CDECL CRTDLL__open(LPCSTR path,INT flags)
{
    dprintf(("CRTDLL: _open\n"));
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
        dprintf(("O_APPEND not supported\n" ));
    if (flags & 0xf0f4) 
      dprintf(("CRTDLL_open file unsupported flags 0x%04x\n",flags));
    /* End Fixme */

    ret = CreateFileA( path, access, FILE_SHARE_READ | FILE_SHARE_WRITE,
                         NULL, creation, FILE_ATTRIBUTE_NORMAL, -1 );
    dprintf(("CRTDLL_open file %s mode 0x%04x got handle %d\n", path,flags,ret));
    return ret;
}


/*********************************************************************
 *                  _open_osfhandle  (CRTDLL.238)
 */
INT CDECL CRTDLL__open_osfhandle( long osfhandle, int flags )
{
  dprintf(("CRTDLL: _open_osfhandle\n"));
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
	dprintf(("(handle %08lx,flags %d) return %d\n",
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
 *                  _pctype_dll    (CRTDLL.245)
 *	FIXME - Could not find anything about it
 */
INT CDECL CRTDLL__pctype_dll(DWORD ret)
{
        dprintf(("CRTDLL: _pctype_dll not implemented.\n"));
	return 0;
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
 *                  _putch     (CRTDLL.250)
 */
INT CDECL CRTDLL__putch( int i )
{
  dprintf(("CRTDLL: _putch\n"));
  return (_putch(i));
}


/*********************************************************************
 *                  _putenv     (CRTDLL.251)
 */
INT CDECL CRTDLL__putenv(const char *s)
{
  dprintf(("CRTDLL: _putenv\n"));
  return (_putenv(s));
}


/*********************************************************************
 *                  _putw     (CRTDLL.252)
 */
INT CDECL CRTDLL__putw( int binint, FILE *fp )
{
  dprintf(("CRTDLL: _putw not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
INT CDECL CRTDLL__rmdir(const char *s1)
{
  dprintf(("CRTDLL: _rmdir\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
//  return (_rmdir(s1));
}


/*********************************************************************
 *                  _rmtmp     (CRTDLL.256)
 */
INT CDECL CRTDLL__rmtmp(void)
{
  dprintf(("CRTDLL: _rmtmp\n"));
  return(_rmtmp());
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
 *           CRTDLL__scalb 	 (CRTDLL.259)
 */
double CDECL CRTDLL__scalb( double x, long exp )
{
  dprintf(("CRTDLL: _scalb not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__searchenv 	 (CRTDLL.260)
 */
void CDECL CRTDLL__searchenv( const char *name, const char *env_var, char *buf )
{
  dprintf(("CRTDLL: _searchenv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *           CRTDLL__seterrormode 	 (CRTDLL.261)
 */
void CDECL CRTDLL__seterrormode(int i)
{
  dprintf(("CRTDLL: _seterrormode not implemented.\n"));
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
 *                  _setmode           (CRTDLL.263)
 */
INT CDECL CRTDLL__setmode( INT fh,INT mode)
{
	dprintf(("CRTDLL: _setmode\n"));
	return (_setmode(fh, mode));
}


/*********************************************************************
 *                  _sleep           (CRTDLL.265)
 */
VOID CDECL CRTDLL__sleep(unsigned long timeout) 
{
  dprintf(("CRTDLL__sleep for %ld milliseconds\n",timeout));
  Sleep((timeout)?timeout:1);
}


/*********************************************************************
 *           CRTDLL__sopen 	 (CRTDLL.268)
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
int CDECL CRTDLL__spawnl( int i, char *s1, char *s2, ... )
{
  dprintf(("CRTDLL: _spawnl not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__spawnle 	 (CRTDLL.270)
 */
int CDECL CRTDLL__spawnle( int i, char *s1, char *s2, ... )
{
  dprintf(("CRTDLL: _spawnle not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__spawnlp 	 (CRTDLL.271)
 */
int CDECL CRTDLL__spawnlp( int i, char *s1, char *s2, ... )
{
  dprintf(("CRTDLL: _spawnlp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__spawnlpe 	 (CRTDLL.272)
 */
int CDECL CRTDLL__spawnlpe( int i, char *s1, char *s2, ... )
{
  dprintf(("CRTDLL: _spawnlpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__spawnv 	 (CRTDLL.273)
 */
int CDECL CRTDLL__spawnv( int i, char *s1, char ** s2 )
{
  dprintf(("CRTDLL: _spawnv\n"));
  return (_spawnv(i, s1, s2));
}


/*********************************************************************
 *           CRTDLL__spawnve 	 (CRTDLL.274)
 */
int CDECL CRTDLL__spawnve( int i, char *s1, char ** s2, char ** s3 )
{
  dprintf(("CRTDLL: _spawnve\n"));
  return (_spawnve(i, s1, s2, s3));
}


/*********************************************************************
 *           CRTDLL__spawnvp 	 (CRTDLL.275)
 */
int CDECL CRTDLL__spawnvp( int i, char *s1, char ** s2 )
{
  dprintf(("CRTDLL: _spawnvp\n"));
  return (_spawnvp(i, s1, s2));
}

/*********************************************************************
 *           CRTDLL__spawnv 	 (CRTDLL.276)
 */
int CDECL CRTDLL__spawnvpe( int i, char *s1, char ** s2, char ** s3 )
{
  dprintf(("CRTDLL: _spawnvpe\n"));
  return (_spawnvpe(i, s1, s2, s3));
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
 *           CRTDLL__statusfp	 (CRTDLL.279)
 */
unsigned int CDECL CRTDLL__statusfp( void )
{
  dprintf(("CRTDLL: _statusfp\n"));
  return (_status87());
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
LPSTR CDECL CRTDLL__strdup(LPCSTR ptr)
{
  dprintf(("CRTDLL: _strdup\n"));
  return HEAP_strdupA(GetProcessHeap(),0,ptr);
}


/*********************************************************************
 *           CRTDLL__strerror  	 (CRTDLL.284)
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
  dprintf(("CRTDLL: _stricoll not implemented.\n"));
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
  dprintf(("CRTDLL: _strnset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__strrev	 (CRTDLL.294)
 */
char * CDECL CRTDLL__strrev( char *string )
{
  dprintf(("CRTDLL: _strrev not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__strset	 (CRTDLL.295)
 */
char * CDECL CRTDLL__strset( char *string, int c )
{
  dprintf(("CRTDLL: _strset not implemented.\n"));
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
 *           CRTDLL__swab	 (CRTDLL.299)
 */
void CDECL CRTDLL__swab(char *s1, char *s2, int i)
{
  dprintf(("CRTDLL: _swab\n"));
  _swab(s1, s2, i);
}


/*********************************************************************
 *           CRTDLL__tell	 (CRTDLL.302)
 */
long CDECL CRTDLL__tell( int i )
{
  dprintf(("CRTDLL: _tell\n"));
  return (_tell(i));
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
 *           CRTDLL__tzset 	 (CRTDLL.308)
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
  dprintf(("CRTDLL: _umask\n"));
  return (_umask(i));
}


/*********************************************************************
 *           CRTDLL__ungetch	 (CRTDLL.311)
 */
int CDECL CRTDLL__ungetch( int i )
{
  dprintf(("CRTDLL: _ungetch\n"));
  return (_ungetch(i));
}


/*********************************************************************
 *                  _unlink           (CRTDLL.312)
 */
INT CDECL CRTDLL__unlink(LPCSTR pathname)
{
    dprintf(("CRTDLL: _unlink\n"));
    int ret=0;
    DOS_FULL_NAME full_name;

    if (!DOSFS_GetFullName( pathname, FALSE, (CHAR*)&full_name )) {
      dprintf(("CRTDLL_unlink file %s bad name\n",pathname));
      return EOF;
    }
  
    ret=unlink(full_name.long_name);
    dprintf(("(%s unix %s)\n",
		   pathname,full_name.long_name));
    if(ret)
      dprintf((" Failed!\n"));

    return ret;
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
  dprintf(("CRTDLL: _vsnwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           CRTDLL__wcsdup	 (CRTDLL.317)
 */
LPWSTR CDECL CRTDLL__wcsdup( LPCWSTR str )
{
  dprintf(("CRTDLL: _wcsdup\n"));
  LPWSTR ret = NULL;
  if (str)
  {
      int size = (wcslen((const wchar_t*)str) + 1) * sizeof(WCHAR);
// FIXME     ret = CRTDLL_malloc( size );
      if (ret) memcpy( ret, str, size );
  }
  return ret;
}


/*********************************************************************
 *           CRTDLL__wcsicoll	 (CRTDLL.319)
 */
int CDECL CRTDLL__wcsicoll( LPCWSTR str1, LPCWSTR str2 )
{
  dprintf(("CRTDLL: _wcsicoll\n"));
  return NTDLL__wcsicmp( str1, str2 );
}


/*********************************************************************
 *           CRTDLL__wcsnset    (CRTDLL.322)
 */
LPWSTR CDECL CRTDLL__wcsnset( LPWSTR str, WCHAR c, INT n )
{
    dprintf(("CRTDLL: _wcsnset\n"));
    LPWSTR ret = str;
    while ((n-- > 0) && *str) *str++ = c;
    return ret;
}


/*********************************************************************
 *           CRTDLL__wcsrev	 (CRTDLL.323)
 */
LPWSTR CDECL CRTDLL__wcsrev( LPWSTR str )
{
  dprintf(("CRTDLL: _wcsrev\n"));
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
    dprintf(("CRTDLL: _wcsset\n"));
    LPWSTR ret = str;
    while (*str) *str++ = c;
    return ret;
}


/*********************************************************************
 *                  _write        (CRTDLL.329)
 */
INT CDECL CRTDLL__write(INT fd,LPCVOID buf,UINT count)
{
        dprintf(("CRTDLL: _write\n"));
        INT len=0;

	if (fd == -1)
	  len = -1;
	else if (fd<=2)
	  len = (UINT)write(fd,buf,(LONG)count);
	else
	  len = _lwrite(fd,(LPCSTR)buf,count);
	dprintf(("%d/%d byte to dfh %d from %p,\n",
		       len,count,fd,buf));
	return len;
}


/*********************************************************************
 *                  _y0     (CRTDLL.332)
 */
double CDECL CRTDLL__y0(double x)
{
  dprintf(("CRTDLL: _y0\n"));
  return (_y0(x));
}


/*********************************************************************
 *                  _y1     (CRTDLL.333)
 */
double CDECL CRTDLL__y1(double x)
{
  dprintf(("CRTDLL: _y1\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (_y1(x));
}


/*********************************************************************
 *                  _yn     (CRTDLL.334)
 */
double CDECL CRTDLL__yn(int i, double x)
{
  dprintf(("CRTDLL: _yn\n"));
  return (_yn(i, x));
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
  dprintf(("CRTDLL: atexit not implemented.\n"));
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
  dprintf(("CRTDLL: bsearch not implemented.\n"));
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
  dprintf(("CRTDLL: fscanf\n"));
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
    dprintf(("broken\n"));
    return 0;
}


/*********************************************************************
 *                  fseek 	  (CRTDLL.379)
 */
int CDECL CRTDLL_fseek( FILE *file, long int offset, int whence )
{
  dprintf(("CRTDLL: fseek\n"));
  dprintf(("file %p to 0x%08lx pos %s\n",
        file,offset,(whence==SEEK_SET)?"SEEK_SET":
        (whence==SEEK_CUR)?"SEEK_CUR":
        (whence==SEEK_END)?"SEEK_END":"UNKNOWN"));
// FIXME if (SetFilePointer( file->handle, offset, NULL, whence ) != 0xffffffff)
// FIXME     return 0;
  dprintf((" failed!\n"));
  return -1;
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

    dprintf(("CRTDLL: fwrite\n"));
    if (!WriteFile( file->handle, ptr, size * nmemb, &ret, NULL ))
        dprintf((" failed!\n"));

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
  dprintf(("CRTDLL: isleadbyte(%08xh) not implemented.\n", i));
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
  dprintf(("CRTDLL: iswascii(%08xh) not implemented.\n", i));
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
    dprintf(("CRTDLL: _mbtowc\n"));
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
    dprintf(("CRTDLL: printf not implemented.\n"));
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
    return HeapReAlloc( GetProcessHeap(), 0, ptr, size );
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
  dprintf(("CRTDLL: scanf not implemented.\n"));
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
    dprintf(("CRTDLL: signal not implemented.\n"));
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
  dprintf(("CRTDLL: swscanf not implemented.\n"));
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
 *                  tmpfile           (CRTDLL.486)
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
  dprintf(("CRTDLL: ungetc not implemented.\n"));
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
  dprintf(("CRTDLL: vfwprintf not implemented.\n"));
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
  dprintf(("CRTDLL: vwprintf not implemented.\n"));
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
