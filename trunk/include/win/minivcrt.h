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

#ifdef __cplusplus
extern "C" {
#endif

#define MSVCRT(x) x

#define FILENAME_MAX      260

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

#ifdef __cplusplus
}
#endif

#endif /* _minivcrt_h_ */
