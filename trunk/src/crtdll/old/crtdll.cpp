/* $Id: crtdll.cpp,v 1.1 2000-11-21 23:48:55 phaller Exp $ */

/*
 * The C RunTime DLL
 * 
 * Implements C run-time functionality as known from UNIX.
 *
 * TODO: 
 *   - Check setjmp(3)
 *   - fix *ALL* functions for the FS: wrapper problem
 *
 * Partialy based on Wine
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997 Uwe Bonnes
 * Copyright 1999-2000 Jens Wiessner
 * Copyright 2000 Przemyslaw Dobrowolski
 */


#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>



#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <unicode.h>
#include <heapstring.h>
#include <ctype.h>
#include <setjmp.h>
#include <ntddk.h>
#include <debugtools.h>

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
#include <sys\timeb.h>
#include <sys\stat.h>
#include "signal.h"

#include <crtdll.h>
// #include <ieee.h>
#include <asmhlp.h>
#include "crtinc.h"


ODINDEBUGCHANNEL(CRTDLL)



#define FS_OS2   unsigned short sel = RestoreOS2FS();
#define FS_WIN32 SetFS(sel);

#define dprintf2 dprintf


/*********************************************************************
 *                  CRTDLL_MainInit  (CRTDLL.init)
 */
BOOL WINAPI CRTDLL_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  FS_OS2
    
    if (fdwReason == DLL_PROCESS_ATTACH) {
      _fdopen(0,"r");
      _fdopen(1,"w");
      _fdopen(2,"w");
      CRTDLL_hHeap = HeapCreate(0, 0x10000, 0);
    }
    else
      if (fdwReason == DLL_PROCESS_DETACH) {
        HeapDestroy(CRTDLL_hHeap);
        CRTDLL_hHeap = 0;
      }
  
  FS_WIN32
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
double CDECL CRTDLL__CIacos()
{
  double x;
  dprintf2(("CRTDLL: _CIacos\n"));
  POP_FPU(x);
  return acos(x);
}


/*********************************************************************
 *                  _CIasin    (CRTDLL.005)
 */
double CDECL CRTDLL__CIasin()
{
  double x;
  dprintf2(("CRTDLL: _CIasin\n"));
  POP_FPU(x);
  return asin(x);
}


/*********************************************************************
 *                  _CIatan    (CRTDLL.006)
 */
double CDECL CRTDLL__CIatan()
{
  double x;
  dprintf2(("CRTDLL: _CIatan\n"));
  POP_FPU(x);
  return atan(x);
}


/*********************************************************************
 *                  _CIatan2    (CRTDLL.007)
 */
double CDECL CRTDLL__CIatan2()
{
  double x, y;
  dprintf2(("CRTDLL: _CIatan2\n"));
  POP_FPU(y);
  POP_FPU(x);
  return atan2(x,y);
}


/*********************************************************************
 *                  _CIcos    (CRTDLL.008)
 */
double CDECL CRTDLL__CIcos()
{
  double x;
  dprintf2(("CRTDLL: _CIcos\n"));
  POP_FPU(x);
  return cos(x);
}


/*********************************************************************
 *                  _CIcosh    (CRTDLL.009)
 */
double CDECL CRTDLL__CIcosh()
{
  double x;
  dprintf2(("CRTDLL: _CIcosh\n"));
  POP_FPU(x);
  return cosh(x);
}


/*********************************************************************
 *                  _CIexp    (CRTDLL.010)
 */
double CDECL CRTDLL__CIexp()
{
  double x;
  dprintf2(("CRTDLL: _CIexp\n"));
  POP_FPU(x);
  return exp(x);
}


/*********************************************************************
 *                  _CIfmod     (CRTDLL.011)
 */
double CDECL CRTDLL__CIfmod( )
{
  double x, y;
  dprintf2(("CRTDLL: _CIfmod\n"));
  POP_FPU(y);
  POP_FPU(x);
  return fmod(x,y);
}


/*********************************************************************
 *                  _CIlog    (CRTDLL.012)
 */
double CDECL CRTDLL__CIlog()
{
  double x;
  dprintf2(("CRTDLL: _CIlog\n"));
  POP_FPU(x);
  return log(x);
}


/*********************************************************************
 *                  _CIlog10    (CRTDLL.013)
 */
double CDECL CRTDLL__CIlog10()
{
  double x;
  dprintf2(("CRTDLL: _CIlog10\n"));
  POP_FPU(x);
  return log10(x);
}


/*********************************************************************
 *                  _CIpow     (CRTDLL.014)
 */
LONG CDECL CRTDLL__CIpow()
{
  double x,y;
  dprintf2(("CRTDLL: _CIpow\n"));
  POP_FPU(y);
  POP_FPU(x);
  return pow(x,y);
}




/*********************************************************************
 *                  _CIsin     (CRTDLL.015)
 */
double CDECL CRTDLL__CIsin()
{
  double x;
  dprintf2(("CRTDLL: _CIsin\n"));
  POP_FPU(x);
  return sin(x);
}


/*********************************************************************
 *                  _CIsinh    (CRTDLL.016)
 */
double CDECL CRTDLL__CIsinh()
{
  double x;
  dprintf2(("CRTDLL: _CIsinh\n"));
  POP_FPU(x);
  return sinh(x);
}


/*********************************************************************
 *                  _CIsqrt    (CRTDLL.017)
 */
double CDECL CRTDLL__CIsqrt()
{
  double x;
  dprintf2(("CRTDLL: _CIsqrt\n"));
  POP_FPU(x);
  return sqrt(x);
}


/*********************************************************************
 *                  _CItan    (CRTDLL.018)
 */
double CDECL CRTDLL__CItan()
{
  double x;
  dprintf2(("CRTDLL: _CItan\n"));
  POP_FPU(x);
  return tan(x);
}


/*********************************************************************
 *                  _CItanh    (CRTDLL.019)
 */
double CDECL CRTDLL__CItanh()
{
  double x;
  dprintf2(("CRTDLL: _CItanh\n"));
  POP_FPU(x);
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
 *                  __doserrno            (CRTDLL.26)
 */
int * CDECL CRTDLL___doserrno()
{	
	dprintf2(("CRTDLL: __doserrno\n"));
	return (__doserrno());
}


/*********************************************************************
 *           CRTDLL___isascii   (CRTDLL.28)
 */
int CDECL CRTDLL___isascii(int i)
{
  //TODO: Check if really ok.
  dprintf(("CRTDLL: __isascii -> _isascii\n"));
  return (_isascii(i));
}


/*********************************************************************
 *           CRTDLL___iscsym   (CRTDLL.29)
 */
int CDECL CRTDLL___iscsym(int c)
{
  //TODO: Check if really ok.
  dprintf(("CRTDLL: __iscsym -> _iscsym\n"));
  return (_iscsym(c));
}


/*********************************************************************
 *           CRTDLL___iscsymf   (CRTDLL.30)
 */
int CDECL CRTDLL___iscsymf(int c)
{
  //TODO: Check if really ok.
  dprintf(("CRTDLL: __iscsymf -> _iscsymf\n"));
  return (_iscsymf(c));
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
  //TODO: Check if really ok.
  dprintf(("CRTDLL: __toascii -> _toascii\n"));
  return (_toascii(c));
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
 *           _beginthread   (CRTDLL.46)
 */
int CDECL CRTDLL__beginthread (void (*start)(void *arg), void *stack, unsigned stack_size,
                  void *arg_list)
{
  dprintf(("CRTDLL: _beginthread\n"));
/*
  ULONG rc;
  TID tid;
  struct _thread *tp;
  tp = __alloc_thread ();
  if (tp == NULL)
    {
      errno = ENOMEM;
      return -1;
    }
  tp->_th_start = start;
  tp->_th_arg = arg_list;
  rc = DosCreateThread (&tid, (PFNTHREAD)start_thread, (ULONG)tp,
                        3, stack_size);
  if (rc != 0)
    {
      if (rc == ERROR_NOT_ENOUGH_MEMORY)
        errno = ENOMEM;
      else if (rc == ERROR_MAX_THRDS_REACHED)
        errno = EAGAIN;
      else
        errno = EINVAL;
      DosFreeMem (tp);
      return -1;
    }
  if (tid > MAX_THREADS)
    {
      DosKillThread (tid);
      errno = EAGAIN;
      DosFreeMem (tp);
      return -1;
    }
  if (__newthread (tid) != 0)
    {
      DosKillThread (tid);
      DosFreeMem (tp);
      return -1;
    }
  _thread_tab[tid] = tp;
  rc = DosResumeThread (tid);
  if (rc != 0)
    {
      errno = ESRCH;
      DosFreeMem (tp);
      return -1;
    }
  return tid;
*/
  return 0;
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
 *           _cabs   (CRTDLL.48)
 */
double CDECL CRTDLL__cabs(struct complex z)
{
  dprintf2(("CRTDLL: _cabs\n"));
  return (_cabs(z));
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
	dprintf2(("CRTDLL: _chdir\n"));
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
  
  return CloseHandle(handle);
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
 *                  _cprintf    (CRTDLL.63)
 */
INT CDECL CRTDLL__cprintf( char *fmt, va_list arg )
{
  dprintf2(("CRTDLL: _cprintf.\n"));
  return (_cprintf(fmt, arg));
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
 *	_cscanf	    				(CRTDLL.67)
 */
INT CDECL CRTDLL__cscanf( char *s, va_list arg )
{
  dprintf(("CRTDLL: _cscanf\n"));
  return (_cscanf(s, arg));
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
 *           _eof   (CRTDLL.76)
 */
int CDECL CRTDLL__eof( int _fd )
{
  dprintf2(("CRTDLL: _eof\n"));
  return (__eof(_fd));
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
	dprintf (("exception %lx flags=%lx at %p handler=%p %p %p stub\n",
	rec->ExceptionCode, rec->ExceptionFlags, rec->ExceptionAddress,
	frame->Handler, context, dispatcher));
	return ExceptionContinueSearch;
}


/*********************************************************************
 *           _execl   (CRTDLL.79)
 */
int CDECL CRTDLL__execl(char* szPath, char* szArgv0, va_list arg)
{
  dprintf2(("CRTDLL: _execl\n"));
  return (_execl(szPath, szArgv0, arg));
}


/*********************************************************************
 *           _execle   (CRTDLL.80)
 */
int CDECL CRTDLL__execle(char *path, char *szArgv0, va_list arg)
{
  dprintf2(("CRTDLL: _execle\n"));
  return (_execle(path, szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__execlp   (CRTDLL.81)
 */
int CDECL CRTDLL__execlp( char *szPath, char *szArgv0, va_list arg)
{
  dprintf2(("CRTDLL: _execlp\n"));
  return (_execlp(szPath, szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__execlpe   (CRTDLL.82)
 */
int CDECL CRTDLL__execlpe( char *path, char *szArgv0, va_list arg)
{
  dprintf2(("CRTDLL: _execlpe\n"));
  return (_execlpe(path, szArgv0, arg));
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
FILE * CDECL CRTDLL__fdopen(INT handle, LPCSTR mode)
{
    dprintf2(("CRTDLL: _fdopen\n"));
    return (_fdopen(handle, mode));
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
 *                  _flushall     (CRTDLL.103)
 */
INT CDECL CRTDLL__flushall(void)
{
  dprintf2(("CRTDLL: _flushall\n"));
  return (_flushall());
}


/*********************************************************************
 *	_fpreset				(CRTDLL.107)
 */
void CDECL CRTDLL__fpreset(void)
{
  dprintf(("CRTDLL: _fpreset\n"));
  _fpreset();
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
 *	_fstat					(CRTDLL.111)
 */
int CDECL CRTDLL__fstat(int file, struct stat* buf)
{
  dprintf(("CRTDLL: _fstat\n"));
  return (_fstat(file, buf));
}


/*********************************************************************
 *	_ftime					(CRTDLL.112)
 */
void CDECL CRTDLL__ftime( struct timeb *timebuf )
{
  dprintf(("CRTDLL: _ftime\n"));
  _ftime(timebuf);
}


/*********************************************************************
 *                  _ftol         (CRTDLL.113)
 */
LONG CDECL CRTDLL__ftol(void)
{
	/* don't just do DO_FPU("fistp",retval), because the rounding
	 * mode must also be set to "round towards zero"... */
	double fl;
	POP_FPU(fl);
	return (LONG)fl;
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
 *                  _gcvt     (CRTDLL.116)
 */
char * CDECL CRTDLL__gcvt( double val, int ndig, char *buf )
{
  dprintf2(("CRTDLL: _gcvt\n"));
  return (_gcvt(val, ndig, buf));
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
  SYSTEMTIME  systemtime;

  GetLocalTime(&systemtime);

  tp->tm_isdst = -1; // FIXME: I don't know is there a correct value
  tp->tm_sec   = systemtime.wSecond;
  tp->tm_min   = systemtime.wMinute;
  tp->tm_hour  = systemtime.wHour;
  tp->tm_mday  = systemtime.wDay;
  tp->tm_mon   = systemtime.wMonth - 1;
  // struct tm has time from 1900  -> 2000 = 100
  tp->tm_year  = systemtime.wYear - 1900;
  tp->tm_wday  = systemtime.wDayOfWeek;

  mktime(tp);

  return (0); // FIXME: What Can we return??
}


/*********************************************************************
 *                  _getw     (CRTDLL.128)
 */
int CDECL CRTDLL__getw( FILE *stream )
{
  dprintf2(("CRTDLL: _getw\n"));
  int x;
  return (fread (&x, sizeof (x), 1, stream) == 1 ? x : EOF);
}


/*******************************************************************
 *         _global_unwind2  (CRTDLL.129)
 */
void CDECL CRTDLL__global_unwind2( PEXCEPTION_FRAME frame )
{
    dprintf2(("CRTDLL: _global_unwind2\n"));
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
 *                  _itoa      (CRTDLL.165)
 */
char * CDECL CRTDLL__itoa(int i, char *s, int r)
{
  dprintf2(("CRTDLL: _itoa(%08xh, %08xh, %08xh)\n",
           i,
           s,
           r));

  return (itoa(i,s,r));
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
	dprintf2(("CRTDLL: _local_unwind2\n"));
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
 *                  _lseek	(CRTDLL.178)
 */
long CDECL CRTDLL__lseek(int handle,long offset,int origin)
{
  dprintf2(("CRTDLL: _lssek\n"));
  return (_lseek(handle, offset, origin));
}


/*********************************************************************
 *                  _ltoa 	(CRTDLL.179)
 */
LPSTR  CDECL CRTDLL__ltoa(long x,LPSTR buf,INT radix)
{
    return ltoa(x,buf,radix);
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
 *                  _mkdir           (CRTDLL.232)
 */
INT CDECL CRTDLL__mkdir(LPCSTR newdir)
{
	dprintf2(("CRTDLL: _mkdir\n"));
	if (!CreateDirectoryA(newdir,NULL))
		return -1;
	return 0;
}


/*********************************************************************
 *                  _mktemp        (CRTDLL.233)
 */
char * CDECL CRTDLL__mktemp( char *string )
{
  dprintf2(("CRTDLL: _mktemp\n"));
  int pid, n, saved_errno;
  char *s;

  pid = _getpid ();
  s = strchr (string, 0);
  n = 0;
  while (s != string && s[-1] == 'X')
    {
      --s; ++n;
      *s = (char)(pid % 10) + '0';
      pid /= 10;
    }
  if (n < 2)
    return NULL;
  *s = 'a'; saved_errno = errno;
  for (;;)
    {
      errno = 0;
      if (_access (string, 0) != 0 && errno == ENOENT)
        {
          errno = saved_errno;
          return string;
        }
      if (*s == 'z')
        {
          errno = saved_errno;
          return NULL;
        }
      ++*s;
    }
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
INT CDECL CRTDLL__putw( int x, FILE *stream )
{
  dprintf2(("CRTDLL: _putw\n"));
  return (fwrite (&x, sizeof (x), 1, stream) == 1 ? x : EOF);
}


/*********************************************************************
 *                  _read     (CRTDLL.254)
 */
INT CDECL CRTDLL__read(int handle, void *buf, size_t nbyte)
{
  dprintf(("CRTDLL: _read\n"));
/*
  int n, *pflags, *pla;
  size_t j, k;
  char *dst, c;

  if ((pflags = _fd_flags (handle)) == NULL
      || (pla = _fd_lookahead (handle)) == NULL)
    {
      errno = EBADF;
      return -1;
    }

  *pflags &= ~F_CRLF;
  if (nbyte > 0 && (*pflags & F_EOF))
    return 0;
  dst = buf;
  n = read_lookahead (handle, dst, nbyte, pla);
  if (n == -1)
    return -1;
  if ((*pflags & O_TEXT) && !(*pflags & F_TERMIO) && n > 0)
    {
      if (!(*pflags & (F_PIPE|F_SOCKET|F_DEV)) && dst[n-1] == 0x1a &&
          _eof (handle))
        {
          --n;
          *pflags |= F_EOF;
          if (n == 0)
            return 0;
        }
      if (n == 1 && dst[0] == '\r')
        {
          int saved_errno = errno;
          j = read_lookahead (handle, &c, 1, pla);
          if (j == -1 && errno == EAGAIN)
            {
              *pla = dst[0];
              return -1;
            }
          errno = saved_errno;                
          if (j == 1 && c == '\n')            
            {
              dst[0] = '\n';
              *pflags |= F_CRLF;
            }
          else
            *pla = c;
        }
      else
        {

          if (_crlf (dst, n, &k))
            {

              *pla = '\r';
              --n;
            }
          if (k != n)
            *pflags |= F_CRLF;
          n = k;
        }
    }
  return n;
*/
  return 0;
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
 *           CRTDLL__searchenv 	 (CRTDLL.260)
 */
void CDECL CRTDLL__searchenv(char *file, char *var,char *path )
{
  dprintf2(("CRTDLL: _searchenv\n"));
  _searchenv(file, var, path);
}


/*********************************************************************
 *           CRTDLL__seterrormode 	 (CRTDLL.261)
 */
void CDECL CRTDLL__seterrormode(int uMode)
{
  dprintf2(("CRTDLL: _seterrormode\n"));
  SetErrorMode(uMode);
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
  SYSTEMTIME  systemtime;

  mktime(tp);

  systemtime.wMilliseconds = ms;
  systemtime.wSecond       = tp->tm_sec;
  systemtime.wMinute       = tp->tm_min;
  systemtime.wHour         = tp->tm_hour;
  systemtime.wDay          = tp->tm_mday;
  systemtime.wMonth        = tp->tm_mon + 1;
  // struct tm has time from 1900 -> 2000 = 100
  systemtime.wYear         = tp->tm_year + 1900;

  if (SetLocalTime(&systemtime) != 0) return GetLastError();

  return (0);
}


/*********************************************************************
 *                  _sleep           (CRTDLL.265)
 */
VOID CDECL CRTDLL__sleep(unsigned long timeout) 
{
  dprintf2(("_sleep for %ld milliseconds\n",timeout));
  Sleep((timeout)?timeout:1);
}


/*********************************************************************
 *	_sopen					(CRTDLL.268)
 */
int CDECL CRTDLL__sopen( const char *s, int i1, int i2, va_list arg )
{
  dprintf(("CRTDLL: _sopen\n"));
  return (_sopen(s, i1, i2, arg));
}


/*********************************************************************
 *           CRTDLL__spawnl 	 (CRTDLL.269)
 */
int CDECL CRTDLL__spawnl(int nMode, char* szPath, char* szArgv0, va_list arg)
{
  dprintf2(("CRTDLL: _spawnl\n"));
  return (_spawnl(nMode, szPath, szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__spawnle 	 (CRTDLL.270)
 */
int CDECL CRTDLL__spawnle( int mode, char *path, char **szArgv0, va_list arg )
{
  dprintf2(("CRTDLL: _spawnle\n"));
  return (_spawnle(mode, path, (char*)szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__spawnlp 	 (CRTDLL.271)
 */
int CDECL CRTDLL__spawnlp(int nMode, char* szPath, char* szArgv0, va_list arg)
{
  dprintf2(("CRTDLL: _spawnlp\n"));
  return (_spawnlp(nMode, szPath, szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__spawnlpe 	 (CRTDLL.272)
 */
int CDECL CRTDLL__spawnlpe( int mode, char *path, char *szArgv0, va_list arg )
{
  dprintf2(("CRTDLL: _spawnlpe\n"));
  return (_spawnlpe(mode, path, szArgv0, arg));
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
 *           _splitpath     	 (CRTDLL.277)
 */
void CDECL CRTDLL__splitpath( char *path, char *drive, char *dir, char *fname, char *ext )
{
  dprintf2(("CRTDLL: _splitpath"));
  _splitpath( path, drive, dir, fname, ext);
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
 *           _ultoa		 (CRTDLL.309)
 */
LPSTR  CDECL CRTDLL__ultoa(long x,LPSTR buf,INT radix)
{
  dprintf2(("CRTDLL: _ultoa\n"));
  return _ultoa(x,buf,radix);
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
 *           _unloaddll	 (CRTDLL.313)
 */
int CDECL CRTDLL__unloaddll(void *handle)
{
  dprintf2(("CRTDLL: _unloaddll\n"));
  return FreeLibrary((HMODULE)handle);
}


/*********************************************************************
 *           _utime		 (CRTDLL.314)
 */
int CDECL CRTDLL__utime( char *path, struct utimbuf * times )
{
  dprintf2(("CRTDLL: _utime\n"));
  return (_utime(path, times));
}


/*********************************************************************
 *	_vsnprintf			 	(CRTDLL.315)
 */
int CDECL CRTDLL__vsnprintf( char *s, size_t bufsize, const char *format, va_list arg )
{
  dprintf2(("CRTDLL: _vsnprintf(%08xh, %08xh, %08xh)\n",
           s,
           bufsize,
           format));

  return wvsnprintfA(s, bufsize, format, arg);
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
 *                  abort     	(CRTDLL.335)
 */
void CDECL CRTDLL_abort( void )
{
  dprintf2(("CRTDLL: abort\n"));
  abort();
}


/*********************************************************************
 *                  abs		(CRTDLL.336)
 */
double CDECL CRTDLL_abs(double d)
{
  dprintf2(("CRTDLL: abs(%f)\n",
           d));

  return (abs(d));
}


/*********************************************************************
 *                  acos	(CRTDLL.337)
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
 *                  atan 	(CRTDLL.340)
 */
double CDECL CRTDLL_atan(double d)
{
  dprintf2(("CRTDLL: atan(%f)\n",
           d));

  return (atan(d));
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
int CDECL CRTDLL_atexit(void (*func)(void))
{
  dprintf(("CRTDLL: atexit\n"));
  if (_atexit_n >= sizeof (_atexit_v) / sizeof (_atexit_v[0]))
    return -1;
  _atexit_v[_atexit_n++] = func;
  return 0;
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
 *                  atoi 	(CRTDLL.344)
 */
int CDECL CRTDLL_atoi(LPSTR str)
{
  dprintf2(("CRTDLL: atoi(%s)\n",
           str));

  return (atoi(str));
}


/*********************************************************************
 *                  atol 	(CRTDLL.345)
 */
long CDECL CRTDLL_atol(LPSTR str)
{
  dprintf2(("CRTDLL: atol(%s)\n",
           str));

  return (atol(str));
}


/*********************************************************************
 *                  bsearch	(CRTDLL.346)
 */
void *CDECL CRTDLL_bsearch (const void *key, const void *base, size_t num, size_t width,
                     int (* CDECL compare)(const void *key, const void *element))
{
  int left, right, median, sign;
  const void *element;
  
  if (width == 0)
    return 0;
  left = 1; right = num;
  while (left <= right)
    {
      median = (left + right) / 2;
      element = (void *)((char *)base + (median-1)*width);
      sign = compare (key, element);
      if (sign == 0)
        return (void *)element;
      if (sign > 0)
        left = median + 1;
      else
        right = median - 1;
    }
  return 0;
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
 *                  ceil  	(CRTDLL.348)
 */
double CDECL CRTDLL_ceil(double d)
{
  dprintf2(("CRTDLL: ceil(%f)\n",
           d));
  return (ceil(d));
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
 *                  cos 	(CRTDLL.351)
 */
double CDECL CRTDLL_cos(double d)
{
  dprintf2(("CRTDLL: cos(%f)\n",
           d));

  return (cos(d));
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
ULONG CDECL CRTDLL_div( int number, int denom )
{
 div_t divt;

  dprintf2(("CRTDLL: div\n"));
  divt = (div(number, denom));
  SetEDX(divt.rem); //NOTE: make sure the compiler doesn't overwrite edx!
  return divt.quot;
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
 *                  fabs  	(CRTDLL.358)
 */
double CDECL CRTDLL_fabs(double d)
{
  dprintf2(("CRTDLL: fabs(%f)\n",
           d));

  return (fabs(d));
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
 *                  floor		(CRTDLL.367)
 */
double CDECL CRTDLL_floor(double d)
{
  dprintf2(("CRTDLL: floor(%f)\n",
           d));

  return (floor(d));
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
INT CDECL CRTDLL_fprintf( FILE *file, LPSTR format, va_list arg )
{
    dprintf2(("CRTDLL: fprintf\n"));
    return (fprintf(file, format, arg));
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
int CDECL CRTDLL_fscanf( FILE*fp, const char *format, va_list arg )
{
  dprintf2(("CRTDLL: fscanf\n"));
  return (fscanf(fp, format, arg));
}


/*********************************************************************
 *                  fseek 	  (CRTDLL.379)
 */
int CDECL CRTDLL_fseek( FILE *file, long int offset, int whence )
{
  dprintf2(("CRTDLL: fseek\n"));
  return (fseek(file, offset, whence));
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
 *                  fwrite     (CRTDLL.383)
 */
DWORD CDECL CRTDLL_fwrite( LPVOID ptr, INT size, INT nmemb, FILE *file )
{
    dprintf2(("CRTDLL: fwrite\n"));
    return (fwrite( ptr, size, nmemb, file));
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
 *                  isalnum    (CRTDLL.391)
 */
int CDECL CRTDLL_isalnum(int i)
{
  dprintf2(("CRTDLL: isalnum(%08xh)\n", i));
  return (isalnum(i));
}


/*********************************************************************
 *                  isalpha    (CRTDLL.392)
 */
int CDECL CRTDLL_isalpha(int i)
{
  dprintf2(("CRTDLL: isalpha(%08xh)\n",
           i));

  return (isalpha(i));
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
 *                  isdigit    (CRTDLL.394)
 */
int CDECL CRTDLL_isdigit(int i)
{
  dprintf2(("CRTDLL: isdigit(%08xh)\n",
           i));

  return (isdigit(i));
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
 *                  islower    (CRTDLL.397)
 */
int CDECL CRTDLL_islower(int i)
{
  dprintf2(("CRTDLL: islower(%08xh)\n",
           i));

  return (islower(i));
}


/*********************************************************************
 *                  isprint    (CRTDLL.398)
 */
int CDECL CRTDLL_isprint(int i)
{
  dprintf2(("CRTDLL: isprint(%08xh)\n",
           i));

  return (isprint(i));
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
 *                  isspace    (CRTDLL.400)
 */
int CDECL CRTDLL_isspace(int i)
{
  dprintf2(("CRTDLL: isspace(%08xh)\n",
           i));

  return (isspace(i));
}


/*********************************************************************
 *                  isupper       (CRTDLL.401)
 */
int CDECL CRTDLL_isupper(int i)
{
  dprintf2(("CRTDLL: isupper(%08xh)\n",
           i));

  return (isupper(i));
}


/*********************************************************************
 *                  isxdigit    (CRTDLL.415)
 */
int CDECL CRTDLL_isxdigit(int i)
{
  dprintf2(("CRTDLL: isxdigit(%08xh)\n", i));
  return (isxdigit(i));
}


/*********************************************************************
 *                  labs	(CRTDLL.416)
 */
long int CDECL CRTDLL_labs( long int j )
{
  dprintf2(("CRTDLL: labs(%08xh)\n", j));
  return (labs(j));
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
 *                  log  	(CRTDLL.421)
 */
double CDECL CRTDLL_log( double x )
{
  dprintf2(("CRTDLL: log(%08xh)\n", x));
  return (log(x));
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
 *                  pow      (CRTDLL.436)
 */
double CDECL CRTDLL_pow( double x, double y )	
{
    dprintf2(("CRTDLL: pow(%08xh, %08xh)\n",x, y));
    return pow( x, y );
}


/*********************************************************************
 *                  printf   (CRTDLL.437)
 */
int CDECL CRTDLL_printf( const char *format, va_list arg )
{
  dprintf2(("CRTDLL: printf\n"));
  return (printf(format, arg));
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
 *                  qsort        (CRTDLL.441)
 */
void CDECL CRTDLL_qsort (void *base, size_t num, size_t width,
                         int (*CDECL compare)(const void *x1, const void *x2))
{
  dprintf2(("CRTDLL: qsort\n"));
  if (width > 0 && num > 1 && base != 0)
    qsort1 ((char *)base, (char *)base+(num-1)*width, width, compare);
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
INT CDECL CRTDLL_remove(const char* file)
{
  dprintf2(("CRTDLL: remove\n"));
  return (remove(file));
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
 *	scanf					(CRTDLL.448)
 */
int CDECL CRTDLL_scanf( const char *format, va_list arg )
{
  dprintf(("CRTDLL: scanf\n"));
  return (scanf(format, arg));
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
_SigFunc CDECL CRTDLL_signal(int sig, _SigFunc func)
{
  dprintf(("CRTDLL: signal\n"));
  return (signal(sig, func));
}


/*********************************************************************
 *                  sin          (CRTDLL.453)
 */
double CDECL CRTDLL_sin( double x )
{
  dprintf2(("CRTDLL: sin(%08xh)\n", x));
  return (sin(x));
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
 *                  sprintf      (CRTDLL.455)
 */
LPSTR CDECL CRTDLL_sprintf(LPSTR lpstrBuffer,
                       LPSTR lpstrFormat,
                       ...)
{
  va_list argptr;                          /* -> variable argument list */

  dprintf2(("CRTDLL: sprintf(%08xh,%s)\n",
           lpstrBuffer,
           lpstrFormat));

  va_start(argptr,
           lpstrFormat);                   /* get pointer to argument list */
  vsprintf(lpstrBuffer,
           lpstrFormat,
           argptr);
  va_end(argptr);                          /* done with variable arguments */

  return (lpstrBuffer);
}


/*********************************************************************
 *                  sqrt         (CRTDLL.456)
 */
double CDECL CRTDLL_sqrt( double x )
{
  dprintf2(("CRTDLL: sqrt(%08xh)\n", x));
  return (sqrt(x));
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
 *	sscanf					(CRTDLL.458)
 */
int CDECL CRTDLL_sscanf( const char *s, const char *format, va_list arg )
{
  dprintf(("CRTDLL: sscanf\n"));
  return (sscanf(s, format, arg));
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
 *                  tan           (CRTDLL.483)
 */
double CDECL CRTDLL_tan(double d)
{
  dprintf2(("CRTDLL: tan(%f)\n",
           d));

  return (tan(d));
}


/*********************************************************************
 *                  tanh           (CRTDLL.484)
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
 *                  tolower       (CRTDLL.488)
 */
int CDECL CRTDLL_tolower(int c)
{
  dprintf2(("CRTDLL: tolower(%c)\n",
           c));

  return (tolower(c));
}


/*********************************************************************
 *                  toupper       (CRTDLL.489)
 */
int CDECL CRTDLL_toupper(int c)
{
  dprintf2(("CRTDLL: toupper(%c)\n",
           c));

  return (toupper(c));
}


/*********************************************************************
 *	ungetc					(CRTDLL.492)
 */
INT CDECL CRTDLL_ungetc(int c, FILE *f)
{
  dprintf(("CRTDLL: ungetc\n"));
  return (ungetc(c, f));
}


/*********************************************************************
 *                  vfprintf       (CRTDLL.494)
 */
INT CDECL CRTDLL_vfprintf( FILE *file, LPSTR format, va_list args )
{
    dprintf2(("CRTDLL: vfprintf\n"));
    return (vfprintf(file, format, args));
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
 *                  vsprintf       (CRTDLL.497)
 */
int CDECL CRTDLL_vsprintf( char *s, const char *format, va_list arg )
{
  dprintf2(("CRTDLL: vsprintf(%08xh, %08xh)\n", s, format));
  return (vsprintf(s, format, arg));
}


/*********************************************************************
 *           CRTDLL__itow    	 (CRTDLL.600)
 */
char * CDECL CRTDLL__itow(int i, char *s, int r)
{
  dprintf(("CRTDLL: _itow(%08xh, %08xh, %08xh) no unicode support !\n",
           i,
           s,
           r));

  return (itoa(i,s,r));
}


/*********************************************************************
 *           CRTDLL__setjmp3 	 (CRTDLL.600)
 */
int CDECL CRTDLL__setjmp3( jmp_buf env )
{
  //TODO:
  dprintf2(("CRTDLL: _setjmp3 -> setjmp (NOT IDENTICAL!!!)\n"));
  return(setjmp( env));
}
