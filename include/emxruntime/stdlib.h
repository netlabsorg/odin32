/* stdlib.h (emx+gcc) */

#ifndef _EMXSTDLIB_H
#define _EMXSTDLIB_H

#ifndef CRTWRAP
#define CRTWRAP(a)   emx_##a
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#if !defined (_SIZE_T)
#define _SIZE_T
typedef unsigned long size_t;
#endif

#if !defined (_WCHAR_T) && !defined (__cplusplus)
#define _WCHAR_T
typedef unsigned short wchar_t;
#endif

#if !defined (NULL)
#if defined (__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#if !defined (_DIV_T)
#define _DIV_T
typedef struct _div_t
{
  int quot;
  int rem;
} div_t;
typedef struct _ldiv_t
{
  long quot;
  long rem;
} ldiv_t;
#endif

#if !defined (RAND_MAX)
#define RAND_MAX 0x7fff
#endif

#if !defined (EXIT_SUCCESS)
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

#if !defined (MB_CUR_MAX)
extern int _mb_cur_max;
#define MB_CUR_MAX _mb_cur_max
#endif

#if !defined (_MAX_PATH)
#define _MAX_PATH   260
#define _MAX_DRIVE    3
#define _MAX_DIR    256
#define _MAX_FNAME  256
#define _MAX_EXT    256
#endif

#define abort CRTWRAP(abort)
#define atexit CRTWRAP(atexit)
#define atof CRTWRAP(atof)
#define atoi CRTWRAP(atoi)
#define atol CRTWRAP(atol)
#define bsearch CRTWRAP(bsearch)
#define div CRTWRAP(div)
#define exit CRTWRAP(exit)
#define getenv CRTWRAP(getenv)
#define ldiv CRTWRAP(ldiv)
#define mblen CRTWRAP(mblen)
#define mbstowcs CRTWRAP(mbstowcs)
#define mbtowc CRTWRAP(mbtowc)
#define qsort CRTWRAP(qsort)
#define rand CRTWRAP(rand)
#define srand CRTWRAP(srand)
#define strtod CRTWRAP(strtod)
#define strtol CRTWRAP(strtol)
#define strtoul CRTWRAP(strtoul)
#define system CRTWRAP(system)
#define wcstombs CRTWRAP(wcstombs)
#define wctomb CRTWRAP(wctomb)

void abort (void) __attribute__ ((__noreturn__));
int atexit (void (*)(void));
double atof (__const__ char *);
int atoi (__const__ char *);
long atol (__const__ char *);
void *bsearch (__const__ void *, __const__ void *, size_t, size_t,
    int (*)(__const__ void *, __const__ void *));
div_t div (int, int);
void exit (int) __attribute__ ((__noreturn__));
char *getenv (__const__ char *);
ldiv_t ldiv (long, long);
int mblen (__const__ char *, size_t);
size_t mbstowcs (wchar_t *, __const__ char *, size_t);
int mbtowc (wchar_t *, __const__ char *, size_t);
void qsort (void *, size_t, size_t,
    int (*)(__const__ void *, __const__ void *));
int rand (void);
void srand (unsigned);
double strtod (__const__ char *, char **);
long strtol (__const__ char *, char **, int);
unsigned long strtoul (__const__ char *, char **, int);
int system (__const__ char *);
size_t wcstombs (char *, __const__ wchar_t *, size_t);
int wctomb (char *, wchar_t);

#if !defined (_ABS)                                      /* see also math.h */
#define _ABS
#if !defined (__GNUC__) || __GNUC__ >= 2

#define abs CRTWRAP(abs)
#define labs CRTWRAP(labs)

extern int abs (int);
extern long labs (long);
#else
extern __inline__ int abs (int _n) { return (_n < 0 ? -_n : _n); }
extern __inline__ long labs (long _n) { return (_n < 0 ? -_n : _n); }
#endif
#endif

#if !defined (__NO_C9X)

#define strtof CRTWRAP(strtof)
#define strtold CRTWRAP(strtold)

float strtof (__const__ char *, char **);
long double strtold (__const__ char *, char **);

#endif


#if !defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)

#if !defined (OS2_MODE)
#define DOS_MODE 0
#define OS2_MODE 1
#endif

#if !defined (_ERRNO)
#define _ERRNO
#if defined (__MT__) || defined (__ST_MT_ERRNO__)
extern int *__errno (void);
#define errno (*emx__errno ())
#else
#define errno CRTWRAP(errno)
extern int errno;
#endif
#endif

#if !defined (_ULDIV_T)
#define _ULDIV_T
typedef struct
{
  unsigned long quot;
  unsigned long rem;
} _uldiv_t;
typedef struct
{
  long long quot;
  long long rem;
} _lldiv_t;
typedef struct
{
  unsigned long long quot;
  unsigned long long rem;
} _ulldiv_t;
#endif

extern char **environ;

extern __const__ char * __const__ sys_errlist[];
extern __const__ int sys_nerr;

extern __const__ unsigned int _emx_vcmp;
extern __const__ unsigned int _emx_env;
extern __const__ unsigned int _emx_rev;
extern __const__ unsigned char _osminor;
extern __const__ unsigned char _osmajor;
extern __const__ unsigned char _osmode;
extern __const__ char _emx_vprt[5];


#define alarm CRTWRAP(alarm)
#define brk CRTWRAP(brk)
#define chdir CRTWRAP(chdir)
#define gcvt CRTWRAP(gcvt)
#define getcwd CRTWRAP(getcwd)
#define getpagesize CRTWRAP(getpagesize)
#define getwd CRTWRAP(getwd)
#define mkdir CRTWRAP(mkdir)
#define perror CRTWRAP(perror)
#define putenv CRTWRAP(putenv)
#define rmdir CRTWRAP(rmdir)
#define sbrk CRTWRAP(sbrk)
#define sleep CRTWRAP(sleep)
#define swab CRTWRAP(swab)
#define ulimit CRTWRAP(ulimit)

unsigned alarm (unsigned);
void *brk (void *);
int chdir (__const__ char *);
char *gcvt (double, int, char *);
char *getcwd (char *, size_t);
int getpagesize (void);
char *getwd (char *);
int mkdir (__const__ char *, long);
void perror (__const__ char *);
int putenv (__const__ char *);
int rmdir (__const__ char *);
void *sbrk (int);
unsigned sleep (unsigned);
void swab (__const__ void *, void *, size_t);
long ulimit (int, ...);

#define heapsort CRTWRAP(heapsort)
#define initstate CRTWRAP(initstate)
#define random CRTWRAP(random)
#define setstate CRTWRAP(setstate)
#define srandom CRTWRAP(srandom)

int heapsort (void *, size_t, size_t,
    int (*)(__const__ void *, __const__ void *));       /* BSD */
char *initstate (unsigned, char *, int);                /* BSD */
long random (void);                                     /* BSD */
char *setstate (char *);                                /* BSD */
void srandom (unsigned);                                /* BSD */

#endif


#if (!defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)) \
    || defined (_WITH_UNDERSCORE)

extern char **_environ;
extern __const__ char * __const__ _sys_errlist[];
extern __const__ int _sys_nerr;

#define _alarm CRTWRAP(_alarm)
#define _brk CRTWRAP(_brk)
#define _chdir CRTWRAP(_chdir)
#define _gcvt CRTWRAP(_gcvt)
#define _getcwd CRTWRAP(_getcwd)
#define _getpagesize CRTWRAP(_getpagesize)
#define _getwd CRTWRAP(_getwd)
#define _mkdir CRTWRAP(_mkdir)
#define _perror CRTWRAP(_perror)
#define _putenv CRTWRAP(_putenv)
#define _rmdir CRTWRAP(_rmdir)
#define _sbrk CRTWRAP(_sbrk)
#define _sleep CRTWRAP(_sleep)
#define _swab CRTWRAP(_swab)
#define _ulimit CRTWRAP(_ulimit)

unsigned _alarm (unsigned);
void *_brk (void *);
int _chdir (__const__ char *);
char *_gcvt (double, int, char *);
char *_getcwd (char *, size_t);
int _getpagesize (void);
char *_getwd (char *);
int _mkdir (__const__ char *, long);
int _putenv (__const__ char *);
int _rmdir (__const__ char *);
void *_sbrk (int);
unsigned _sleep (unsigned);
void _swab (__const__ void *, void *, size_t);
long _ulimit (int, ...);

#define _abspath CRTWRAP(_abspath)
#define _atoll CRTWRAP(_atoll)
#define _atofl CRTWRAP(_atofl)
#define _chdir2 CRTWRAP(_chdir2)
#define _chdrive CRTWRAP(_chdrive)
#define _core CRTWRAP(_core)
#define _defext CRTWRAP(_defext)
#define _envargs CRTWRAP(_envargs)
#define _execname CRTWRAP(_execname)
#define _exit CRTWRAP(_exit)
#define _filesys CRTWRAP(_filesys)
#define _fncmp CRTWRAP(_fncmp)
#define _fnexplode CRTWRAP(_fnexplode)
#define _fnexplodefree CRTWRAP(_fnexplodefree)
#define _fngetdrive CRTWRAP(_fngetdrive)
#define _fnisabs CRTWRAP(_fnisabs)
#define _fnisrel CRTWRAP(_fnisrel)
#define _fnlwr CRTWRAP(_fnlwr)
#define _fnlwr2 CRTWRAP(_fnlwr2)
#define _fnslashify CRTWRAP(_fnslashify)
#define _fullpath CRTWRAP(_fullpath)
#define _getcwd1 CRTWRAP(_getcwd1)
#define _getcwd2 CRTWRAP(_getcwd2)
#define _getdrive CRTWRAP(_getdrive)
#define _getext CRTWRAP(_getext)
#define _getext2 CRTWRAP(_getext2)
#define _getname CRTWRAP(_getname)
#define _getsockhandle CRTWRAP(_getsockhandle)
#define _gettid CRTWRAP(_gettid)
#define _getvol CRTWRAP(_getvol)
#define _itoa CRTWRAP(_itoa)
#define _lldiv CRTWRAP(_lldiv)
#define _lltoa CRTWRAP(_lltoa)
#define _ltoa CRTWRAP(_ltoa)
#define _makepath CRTWRAP(_makepath)
#define _path CRTWRAP(_path)
#define _read_kbd CRTWRAP(_read_kbd)
#define _remext CRTWRAP(_remext)
#define _rfnlwr CRTWRAP(_rfnlwr)
#define _response CRTWRAP(_response)
#define _scrsize CRTWRAP(_scrsize)
#define _searchenv CRTWRAP(_searchenv)
#define _setsyserrno CRTWRAP(_setsyserrno)
#define _sfnlwr CRTWRAP(_sfnlwr)
#define _sleep2 CRTWRAP(_sleep2)
#define _splitargs CRTWRAP(_splitargs)
#define _splitpath CRTWRAP(_splitpath)
#define _strtof CRTWRAP(_strtof)
#define _strtold CRTWRAP(_strtold)
#define _strtoll CRTWRAP(_strtoll)
#define _strtoull CRTWRAP(_strtoull)
#define _swchar CRTWRAP(_swchar)
#define _syserrno CRTWRAP(_syserrno)
#define _uldiv CRTWRAP(_uldiv)
#define _ulldiv CRTWRAP(_ulldiv)
#define _ulltoa CRTWRAP(_ulltoa)
#define _ultoa CRTWRAP(_ultoa)
#define _wildcard CRTWRAP(_wildcard)

int _abspath (char *, __const__ char *, int);
long long _atoll (__const__ char *);
long double _atofl (__const__ char *);
int _chdir2 (__const__ char *);
int _chdrive (char);
int _core (int);
void _defext (char *, __const__ char *);
void _envargs (int *, char ***, __const__ char *);
int _execname (char *, size_t);
void _exit (int) __attribute__ ((__noreturn__));
int _filesys (__const__ char *, char *, size_t);
int _fncmp (__const__ unsigned char *, __const__ unsigned char *);
char **_fnexplode (__const__ char *);
void _fnexplodefree (char **);
char _fngetdrive (__const__ char *);
int _fnisabs (__const__ char *);
int _fnisrel (__const__ char *);
void _fnlwr (char *);
void _fnlwr2 (char *, __const__ char *);
char *_fnslashify (char *);
int _fullpath (char *, __const__ char *, int);
int _getcwd1 (char *, char);
char *_getcwd2 (char *, int);
char _getdrive (void);
char *_getext (__const__ char *);
char *_getext2 (__const__ char *);
char *_getname (__const__ char *);
int _getsockhandle (int);
int _gettid (void);
char *_getvol (char);
char *_itoa (int, char *, int);
_lldiv_t _lldiv (long long, long long);
char *_lltoa (long long, char *, int);
char *_ltoa (long, char *, int);
void _makepath (char *, __const__ char *, __const__ char *,
    __const__ char *, __const__ char *);
int _path (char *, __const__ char *);
int _read_kbd (int, int, int);
void _remext (char *);
void _rfnlwr (void);
void _response (int *, char ***);
void _scrsize (int *);
void _searchenv (__const__ char *, __const__ char *, char *);
int _setsyserrno (int);
void _sfnlwr (__const__ char *);
unsigned _sleep2 (unsigned);
char ** _splitargs (char *, int *);
void _splitpath (__const__ char *, char *, char *, char *, char *);
float _strtof (__const__ char *, char **);
long double _strtold (__const__ char *, char **);
long long _strtoll (__const__ char *, char **, int);
unsigned long long _strtoull (__const__ char *, char **, int);
char _swchar (void);
int _syserrno (void);
_uldiv_t _uldiv (unsigned long, unsigned long);
_ulldiv_t _ulldiv (unsigned long long, unsigned long long);
char *_ulltoa (unsigned long long, char *, int);
char *_ultoa (unsigned long, char *, int);
void _wildcard (int *, char ***);

#if defined (__MT__)
/* Prefix it to prevent using ;) */
int _emx_beginthread (void (*)(void *), void *, unsigned, void *);
void _emx_endthread (void);
void **_emx_threadstore (void);
#endif

#endif


#if defined (__cplusplus)
}
#endif

#include <malloc.h>

#endif /* not _STDLIB_H */
