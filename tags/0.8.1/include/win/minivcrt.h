/*
 * Header file for the stripped down version of MSVCRT that only
 * contains functions specific to the MS VC Runtime.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _minivcrt_h_
#define _minivcrt_h_

#include <winnt.h>

#include <time.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MSVCRT(x) x

#define FILENAME_MAX      260

#ifndef max
#define max(a,b) ( (a>b) ? a : b )
#endif
#ifndef min
#define min(a,b) ( (a<b) ? a : b )
#endif

typedef unsigned long _fsize_t;

struct _wfinddata_t {
  unsigned attrib;
  MSVCRT(time_t) time_create;
  MSVCRT(time_t) time_access;
  MSVCRT(time_t) time_write;
  _fsize_t       size;
  WCHAR          name[MSVCRT(FILENAME_MAX)];
};

int         _wchdir(const WCHAR*);
WCHAR*      _wgetcwd(WCHAR*,int);
WCHAR*      _wgetdcwd(int,WCHAR*,int);
int         _wmkdir(const WCHAR*);
int         _wrmdir(const WCHAR*);

WCHAR*      _itow(int,WCHAR*,int); /* NTDLL */
WCHAR*      _i64tow(__int64,WCHAR*,int); /* NTDLL */
WCHAR*      _ltow(long,WCHAR*,int); /* NTDLL */
WCHAR*      _ui64tow(unsigned __int64,WCHAR*,int); /* NTDLL */
WCHAR*      _ultow(unsigned long,WCHAR*,int); /* NTDLL */

WCHAR*      _wfullpath(WCHAR*,const WCHAR*,MSVCRT(size_t));
WCHAR*      _wgetenv(const WCHAR*);
void        _wmakepath(WCHAR*,const WCHAR*,const WCHAR*,const WCHAR*,const WCHAR*);
void        _wperror(const WCHAR*); /* NTDLL */
int         _wputenv(const WCHAR*);
void        _wsearchenv(const WCHAR*,const WCHAR*,WCHAR*); /* NTDLL */
void        _wsplitpath(const WCHAR*,WCHAR*,WCHAR*,WCHAR*,WCHAR*);

int         _wsystem(const WCHAR*);
int         _wtoi(const WCHAR*); /* NTDLL */
__int64     _wtoi64(const WCHAR*); /* NTDLL */
long        _wtol(const WCHAR*); /* NTDLL */

#define _wcsicmp    NTDLL__wcsicmp
#define _wcslwr     NTDLL__wcslwr
#define _wcsnicmp   NTDLL__wcsnicmp
#define _wcsupr     NTDLL__wcsupr

WCHAR*      _wcsdup(const WCHAR*);
int         _wcsicmp(const WCHAR*,const WCHAR*);
int         _wcsicoll(const WCHAR*,const WCHAR*);
WCHAR*      _wcslwr(WCHAR*);
int         _wcsnicmp(const WCHAR*,const WCHAR*,MSVCRT(size_t));
WCHAR*      _wcsnset(WCHAR*,WCHAR,MSVCRT(size_t));
WCHAR*      _wcsrev(WCHAR*);
WCHAR*      _wcsset(WCHAR*,WCHAR);
WCHAR*      _wcsupr(WCHAR*);

int         _waccess(const WCHAR*,int);
int         _wchmod(const WCHAR*,int);
int         _wcreat(const WCHAR*,int);
long        _wfindfirst(const WCHAR*,struct _wfinddata_t*);
int         _wfindnext(long,struct _wfinddata_t*);
WCHAR*      _wmktemp(WCHAR*);
int         _wrename(const WCHAR*,const WCHAR*);
int         _wunlink(const WCHAR*);

#define wcstok      NTDLL_wcstok

WCHAR*      wcstok(WCHAR*,WCHAR*);

#define _stat stat
struct _stat;
int         _wstat(const WCHAR*,struct _stat*);

int         _wremove(const WCHAR*);
WCHAR*      _wtempnam(const WCHAR*,const WCHAR*);

char *MSVCRT__fullpath(char * absPath, const char* relPath, unsigned int size);
#undef _fullpath
#define _fullpath MSVCRT__fullpath

#define swprintf    NTDLL_swprintf
#define snwprintf   NTDLL_snwprintf
#define swscanf     NTDLL_swscanf

int         swprintf(WCHAR*,const WCHAR*,...);
int         snwprintf(WCHAR*,unsigned int,const WCHAR*,...);
int         swscanf(const WCHAR*,const WCHAR*,...);

int         _vsnwprintf(WCHAR*,unsigned int,const WCHAR *,va_list);
int         vswprintf(WCHAR*,const WCHAR*,va_list);
int         vfwprintf(FILE*, const WCHAR*,va_list);
int         vwprintf(const WCHAR*,va_list);
int         fwprintf(FILE*, const WCHAR*, ...);

#define wcsdup      _wcsdup
#define wcsicmp     _wcsicmp
#define wcsnicmp    _wcsnicmp
#define wcsnset     _wcsnset
#define wcsrev      _wcsrev
#define wcsset      _wcsset
#define wcslwr      _wcslwr
#define wcsupr      _wcsupr
#define wcsicoll    _wcsicoll

#ifdef __cplusplus
}
#endif

#endif /* _minivcrt_h_ */
