/* $Id: crtdll.cpp,v 1.30 2001-09-05 12:14:23 bird Exp $ */
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
#include <time.h>

#include <crtdll.h>
// #include <ieee.h>
#include <asmhlp.h>
#include "crtinc.h"


ODINDEBUGCHANNEL(CRTDLL)

#undef dprintf2
#define dprintf2 dprintf


//
// Definitions for internal functions
//
void        qsort1 (char*, char*, size_t,
                    int (* CDECL)(const void*, const void*));



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
 *           CRTDLL__cgets  (CRTDLL.50)
 */
char * CDECL CRTDLL__cgets( char *s )
{
  dprintf2(("CRTDLL: _cgets\n"));
  return (_cgets(s));
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
 *  _cscanf                     (CRTDLL.67)
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
 *           _fcvt  (CRTDLL.90)
 */
char * CDECL CRTDLL__fcvt( double val, int ndig, int *dec, int *sign )
{
  dprintf2(("CRTDLL: _fcvt\n"));
  return (_fcvt(val, ndig, dec, sign));
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
 *  _ftime                  (CRTDLL.112)
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


/*********************************************************************
 *                  _hypot     (CRTDLL.134)
 */
double CDECL CRTDLL__hypot(double x1, double x2)
{
  dprintf2(("CRTDLL: _hypot\n"));
  return (_hypot(x1, x2));
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
 *                  _ltoa   (CRTDLL.179)
 */
LPSTR  CDECL CRTDLL__ltoa(long x,LPSTR buf,INT radix)
{
    return ltoa(x,buf,radix);
}


/*********************************************************************
 *                  _matherr    (CRTDLL.181)
 */
double CDECL CRTDLL__matherr( struct exception * excep )
{
  dprintf2(("CRTDLL: _matherr\n"));
  return (_matherr(excep));
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
 *                  _rmtmp     (CRTDLL.256)
 */
INT CDECL CRTDLL__rmtmp(void)
{
  dprintf2(("CRTDLL: _rmtmp\n"));
  return(_rmtmp());
}


/*********************************************************************
 *           CRTDLL__searchenv   (CRTDLL.260)
 */
void CDECL CRTDLL__searchenv(char *file, char *var,char *path )
{
  dprintf2(("CRTDLL: _searchenv\n"));
  _searchenv(file, var, path);
}


/*********************************************************************
 *           CRTDLL__seterrormode    (CRTDLL.261)
 */
void CDECL CRTDLL__seterrormode(int uMode)
{
  dprintf2(("CRTDLL: _seterrormode\n"));
  SetErrorMode(uMode);
  return;
}


/*********************************************************************
 *           CRTDLL__setjmp      (CRTDLL.262)
 */
int CDECL CRTDLL__setjmp( jmp_buf env )
{
  //TODO:
  dprintf2(("CRTDLL: _setjmp -> setjmp (NOT IDENTICAL!!!)\n"));
  return(setjmp( env));
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
 *  _sopen                  (CRTDLL.268)
 */
int CDECL CRTDLL__sopen( const char *s, int i1, int i2, va_list arg )
{
  dprintf(("CRTDLL: _sopen\n"));
  return (_sopen(s, i1, i2, arg));
}


/*********************************************************************
 *           CRTDLL__spawnl      (CRTDLL.269)
 */
int CDECL CRTDLL__spawnl(int nMode, char* szPath, char* szArgv0, va_list arg)
{
  dprintf2(("CRTDLL: _spawnl\n"));
  return (_spawnl(nMode, szPath, szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__spawnle     (CRTDLL.270)
 */
int CDECL CRTDLL__spawnle( int mode, char *path, char **szArgv0, va_list arg )
{
  dprintf2(("CRTDLL: _spawnle\n"));
  return (_spawnle(mode, path, (char*)szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__spawnlp     (CRTDLL.271)
 */
int CDECL CRTDLL__spawnlp(int nMode, char* szPath, char* szArgv0, va_list arg)
{
  dprintf2(("CRTDLL: _spawnlp\n"));
  return (_spawnlp(nMode, szPath, szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__spawnlpe    (CRTDLL.272)
 */
int CDECL CRTDLL__spawnlpe( int mode, char *path, char *szArgv0, va_list arg )
{
  dprintf2(("CRTDLL: _spawnlpe\n"));
  return (_spawnlpe(mode, path, szArgv0, arg));
}


/*********************************************************************
 *           CRTDLL__spawnv      (CRTDLL.273)
 */
int CDECL CRTDLL__spawnv( int i, char *s1, char ** s2 )
{
  dprintf2(("CRTDLL: _spawnv\n"));
  return (_spawnv(i, s1, s2));
}


/*********************************************************************
 *           CRTDLL__spawnvp     (CRTDLL.275)
 */
int CDECL CRTDLL__spawnvp( int i, char *s1, char ** s2 )
{
  dprintf2(("CRTDLL: _spawnvp\n"));
  return (_spawnvp(i, s1, s2));
}

/*********************************************************************
 *           CRTDLL__spawnv      (CRTDLL.276)
 */
int CDECL CRTDLL__spawnvpe( int i, char *s1, char ** s2, char ** s3 )
{
  dprintf2(("CRTDLL: _spawnvpe\n"));
  return (_spawnvpe(i, s1, s2, s3));
}


/*********************************************************************
 *           CRTDLL__statusfp    (CRTDLL.279)
 */
unsigned int CDECL CRTDLL__statusfp( void )
{
  dprintf2(("CRTDLL: _statusfp\n"));
  return (_status87());
}


/*********************************************************************
 *           _ultoa      (CRTDLL.309)
 */
LPSTR  CDECL CRTDLL__ultoa(long x,LPSTR buf,INT radix)
{
  dprintf2(("CRTDLL: _ultoa\n"));
  return _ultoa(x,buf,radix);
}


/*********************************************************************
 *           CRTDLL__ungetch     (CRTDLL.311)
 */
int CDECL CRTDLL__ungetch( int i )
{
  dprintf2(("CRTDLL: _ungetch\n"));
  return (_ungetch(i));
}


/*********************************************************************
 *           _utime      (CRTDLL.314)
 */
int CDECL CRTDLL__utime( char *path, struct utimbuf * times )
{
  dprintf2(("CRTDLL: _utime\n"));
  return (_utime(path, times));
}


/*********************************************************************
 *  _vsnprintf              (CRTDLL.315)
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
 *                  abs     (CRTDLL.336)
 */
double CDECL CRTDLL_abs(double d)
{
  dprintf2(("CRTDLL: abs(%f)\n",
           d));

  return (abs(d));
}


/*********************************************************************
 *                  acos    (CRTDLL.337)
 */
double CDECL CRTDLL_acos( double x )
{
  dprintf2(("CRTDLL: acos\n"));
  return (acos(x));
}


/*********************************************************************
 *                  asctime (CRTDLL.338)
 */
char * CDECL CRTDLL_asctime( const struct tm *timeptr )
{
  dprintf2(("CRTDLL: asctime\n"));
  return (asctime(timeptr));
}


/*********************************************************************
 *                  asin    (CRTDLL.339)
 */
double CDECL CRTDLL_asin( double x )
{
  dprintf2(("CRTDLL: asin\n"));
  return (asin(x));
}


/*********************************************************************
 *                  atan    (CRTDLL.340)
 */
double CDECL CRTDLL_atan(double d)
{
  dprintf2(("CRTDLL: atan(%f)\n",
           d));

  return (atan(d));
}


/*********************************************************************
 *                  atan2   (CRTDLL.341)
 */
double CDECL CRTDLL_atan2( double y, double x )
{
  dprintf2(("CRTDLL: atan2\n"));
  return (atan2(y, x));
}


/*********************************************************************
 *                  atof    (CRTDLL.343)
 */
double CDECL CRTDLL_atof( const char *nptr )
{
  dprintf2(("CRTDLL: atof\n"));
  return (atof(nptr));
}


/*********************************************************************
 *                  atoi    (CRTDLL.344)
 */
int CDECL CRTDLL_atoi(LPSTR str)
{
  dprintf2(("CRTDLL: atoi(%s)\n",
           str));

  return (atoi(str));
}


/*********************************************************************
 *                  atol    (CRTDLL.345)
 */
long CDECL CRTDLL_atol(LPSTR str)
{
  dprintf2(("CRTDLL: atol(%s)\n",
           str));

  return (atol(str));
}


/*********************************************************************
 *                  bsearch (CRTDLL.346)
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
 *                  ceil    (CRTDLL.348)
 */
double CDECL CRTDLL_ceil(double d)
{
  dprintf2(("CRTDLL: ceil(%f)\n",
           d));
  return (ceil(d));
}


/*********************************************************************
 *                  clock   (CRTDLL.350)
 */
clock_t CDECL CRTDLL_clock( void )
{
  dprintf2(("CRTDLL: clock\n"));
  return (clock());
}


/*********************************************************************
 *                  cos     (CRTDLL.351)
 */
double CDECL CRTDLL_cos(double d)
{
  dprintf2(("CRTDLL: cos(%f)\n",
           d));

  return (cos(d));
}


/*********************************************************************
 *                  cosh    (CRTDLL.352)
 */
double CDECL CRTDLL_cosh( double x )
{
  dprintf2(("CRTDLL: cosh\n"));
  return (cosh(x));
}


/*********************************************************************
 *                  ctime   (CRTDLL.353)
 */
char * CDECL CRTDLL_ctime( const time_t *timer )
{
  dprintf2(("CRTDLL: ctime\n"));
  return (ctime(timer));
}


/*********************************************************************
 *                  difftime    (CRTDLL.354)
 */
double CDECL CRTDLL_difftime( time_t t1, time_t t0 )
{
  dprintf2(("CRTDLL: difftime\n"));
  return (difftime(t1, t0));
}


/*********************************************************************
 *                  div     (CRTDLL.355)
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
 *                  exp     (CRTDLL.357)
 */
double CDECL CRTDLL_exp( double x )
{
  dprintf2(("CRTDLL: exp\n"));
  return (exp(x));
}


/*********************************************************************
 *                  fabs    (CRTDLL.358)
 */
double CDECL CRTDLL_fabs(double d)
{
  dprintf2(("CRTDLL: fabs(%f)\n",
           d));

  return (fabs(d));
}


/*********************************************************************
 *                  floor       (CRTDLL.367)
 */
double CDECL CRTDLL_floor(double d)
{
  dprintf2(("CRTDLL: floor(%f)\n",
           d));

  return (floor(d));
}


/*********************************************************************
 *                  fmod    (CRTDLL.368)
 */
double CDECL CRTDLL_fmod(double x, double y )
{
  dprintf2(("CRTDLL: fmod\n"));
  return (fmod(x,y));
}


/*********************************************************************
 *                  frexp     (CRTDLL.377)
 */
double CDECL CRTDLL_frexp( double value, int *exp )
{
  dprintf2(("CRTDLL: frexp\n"));
  return (frexp(value, exp));
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
 *                  labs    (CRTDLL.416)
 */
long int CDECL CRTDLL_labs( long int j )
{
  dprintf2(("CRTDLL: labs(%08xh)\n", j));
  return (labs(j));
}


/*********************************************************************
 *                  ldexp   (CRTDLL.417)
 */
double CDECL CRTDLL_ldexp( double x, int exp )
{
  dprintf2(("CRTDLL: ldexp\n"));
  return (ldexp(x, exp));
}


/*********************************************************************
 *                  ldiv    (CRTDLL.418)
 */
ldiv_t CDECL CRTDLL_ldiv( long int numer, long int denom )
{
  dprintf2(("CRTDLL: ldiv\n"));
  return (ldiv(numer, denom));
}


/*********************************************************************
 *                  localeconv  (CRTDLL.419)
 */
struct lconv * CDECL CRTDLL_localeconv(void)
{
  dprintf2(("CRTDLL: localeconv\n"));
  return (localeconv());
}


/*********************************************************************
 *                  localtime   (CRTDLL.420)
 */
struct tm * CDECL CRTDLL_localtime( const time_t *timer )
{
  dprintf2(("CRTDLL: localtime\n"));
  return (localtime(timer));
}


/*********************************************************************
 *                  log     (CRTDLL.421)
 */
double CDECL CRTDLL_log( double x )
{
  dprintf2(("CRTDLL: log(%08xh)\n", x));
  return (log(x));
}


/*********************************************************************
 *                  log10   (CRTDLL.422)
 */
double CDECL CRTDLL_log10( double x )
{
  dprintf2(("CRTDLL: log10\n"));
  return (log10(x));
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
 *  scanf                   (CRTDLL.448)
 */
int CDECL CRTDLL_scanf( const char *format, va_list arg )
{
  dprintf(("CRTDLL: scanf\n"));
  return (scanf(format, arg));
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
 *  sscanf                  (CRTDLL.458)
 */
int CDECL CRTDLL_sscanf( const char *s, const char *format, va_list arg )
{
  dprintf(("CRTDLL: sscanf\n"));
  return (sscanf(s, format, arg));
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
 *  ungetc                  (CRTDLL.492)
 */
INT CDECL CRTDLL_ungetc(int c, FILE *f)
{
  dprintf(("CRTDLL: ungetc\n"));
  return (ungetc(c, f));
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
 *           CRTDLL__setjmp3     (CRTDLL.600)
 */
int CDECL CRTDLL__setjmp3( jmp_buf env )
{
  //TODO:
  dprintf2(("CRTDLL: _setjmp3 -> setjmp (NOT IDENTICAL!!!)\n"));
  return(setjmp( env));
}
