/* $Id: msvcrt.cpp,v 1.4 1999-10-28 15:25:35 sandervl Exp $ */

/*
 * The Visual C RunTime DLL
 * 
 * Implements Visual C run-time functionality
 *
 * Copyright 1999 Jens Wiessner
 */


#include <os2win.h>
#include <wchar.h>
#include <except.h>
#include "debugtools.h"
#include <debugdefs.h>


DEFAULT_DEBUG_CHANNEL(msvcrt)

int __app_type;
int __usermatherr;

typedef struct
{
    HANDLE handle;
    int      pad[7];
} CRTDLL_FILE, *PCRTDLL_FILE;

extern LPSTR  	_acmdln_dll;     
extern UINT 	_commode_dll;    
extern UINT 	_fmode_dll;      
extern INT	__mb_cur_max_dll;
extern USHORT	_pctype_dll;
extern CRTDLL_FILE _iob;




/*********************************************************************
 *                  ??1type_info@@UAE@XZ    (MSVCRT.14)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP14(DWORD ret)
{
  dprintf(("MSVCRT: ??1type_info@@UAE@XZ not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??2@YAPAXI@Z    (MSVCRT.15)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP15(DWORD ret)
{
  dprintf(("MSVCRT: ??2@YAPAXI@Z not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.16)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP16(DWORD ret)
{
  dprintf(("MSVCRT: ??3@YAXPAX@Z not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _CxxThrowException    (MSVCRT.66)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT__CxxThrowException(DWORD ret)
{
  dprintf(("MSVCRT: _CxxThrowException not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _EH_prolog    (MSVCRT.67)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT__EH_prolog(DWORD ret)
{
  dprintf(("MSVCRT: _EH_prolog not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  __CxxFrameHandler    (MSVCRT.74)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT___CxxFrameHandler(DWORD ret)
{
  dprintf(("MSVCRT: __CxxFrameHandler not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  __p__initenv  (MSVCRT.101)
 */
char ** CDECL MSVCRT___p__initenv()
{
  dprintf(("MSVCRT: __p__initenv not implemented\n"));
	return &_acmdln_dll;
}


/*********************************************************************
 *                  __p___mb_cur_max  (MSVCRT.102)
 */
int * CDECL MSVCRT___p___mb_cur_max()
{
  dprintf(("MSVCRT: __p___mb_cur_max\n"));
	return &__mb_cur_max_dll;
}


/*********************************************************************
 *                  __p__acmdln  (MSVCRT.105)
 */
char ** CDECL MSVCRT___p__acmdln()
{
  dprintf(("MSVCRT: __p__acmdln\n"));
	return &_acmdln_dll;
}

/*********************************************************************
 *                  __p__fmode  (MSVCRT.107)
 */
int * CDECL MSVCRT___p__fmode()
{
  dprintf(("MSVCRT: __p__fmode\n"));
	return (int*)&_fmode_dll;
}


/*********************************************************************
 *                  __p__commode  (MSVCRT.112)
 */
int * CDECL MSVCRT___p__commode()
{
  dprintf(("MSVCRT: __p__commode\n"));
	return (int*)&_commode_dll;
}


/*********************************************************************
 *                  __p__iob  (MSVCRT.113)
 */
CRTDLL_FILE * CDECL MSVCRT___p__iob()
{
  dprintf(("MSVCRT: __p__iob\n"));
	return &_iob;
}


/*********************************************************************
 *                  __p__pctype  (MSVCRT.117)
 */
USHORT * CDECL MSVCRT___p__pctype()
{
  dprintf(("MSVCRT: __p__pctype\n"));
	return &_pctype_dll;
}


/*********************************************************************
 *                  __set_app_type    (MSVCRT.130)
 */
int CDECL MSVCRT___set_app_type(int app_type)
{
  dprintf(("MSVCRT: __set_app_type\n"));
	return __app_type = app_type;
}	


/*********************************************************************
 *                  __setusermatherr    (MSVCRT.132)
 */
int CDECL MSVCRT___setusermatherr(int matherr)
{
  dprintf(("MSVCRT: __setusermatherr\n"));
	return __usermatherr = matherr;
}	


/*********************************************************************
 *                  _adjust_fdiv    (MSVCRT.158)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT__adjust_fdiv(DWORD ret)
{
  dprintf(("MSVCRT: _adjust_fdiv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _except_handler3  (MSVCRT.203)
 */
INT CDECL MSVCRT__except_handler3 ( PEXCEPTION_RECORD rec,
	PEXCEPTION_FRAME frame, PCONTEXT context, 
	PEXCEPTION_FRAME  *dispatcher)
{
        dprintf(("MSVCRT: _except_handler3\n"));
	return 1;
}


/*********************************************************************
 *                  _get_sbh_threshold    (MSVCRT.247)
 */
size_t CDECL MSVCRT__get_sbh_threshold( void )
{
  dprintf(("MSVCRT: _get_sbh_threshold not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getmbcp    (MSVCRT.257)
 */
int CDECL MSVCRT__getmbcp( void )
{
  dprintf(("MSVCRT: _getmbcp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getws    (MSVCRT.261)
 */
wchar_t * CDECL MSVCRT__getws( wchar_t *s )
{
  dprintf(("MSVCRT: _getws not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _inp    (MSVCRT.273)
 */
int CDECL MSVCRT__inp( unsigned short port )
{
  dprintf(("MSVCRT: _inp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _inpw    (MSVCRT.274)
 */
unsigned short CDECL MSVCRT__inpw( unsigned short port )
{
  dprintf(("MSVCRT: _inpw not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _inpd    (MSVCRT.275)
 */
unsigned long CDECL MSVCRT__inpd( unsigned short port )
{
  dprintf(("MSVCRT: _inpd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkprint    (MSVCRT.284)
 */
int CDECL MSVCRT__ismbbkprint( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbbkprint not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcalnum    (MSVCRT.290)
 */
int CDECL MSVCRT__ismbcalnum( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcalnum not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcgraph    (MSVCRT.293)
 */
int CDECL MSVCRT__ismbcgraph( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcgraph not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcpunct    (MSVCRT.302)
 */
int CDECL MSVCRT__ismbcpunct( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcpunct not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _itow    (MSVCRT.310)
 */
wchar_t * CDECL MSVCRT__itow( int value, wchar_t *string, int radix )
{
  dprintf(("MSVCRT: _itow not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ltow    (MSVCRT.328)
 */
wchar_t * CDECL MSVCRT__ltow( long value, wchar_t *string, int radix )
{
  dprintf(("MSVCRT: _ltow not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _outp    (MSVCRT.395)
 */
int CDECL MSVCRT__outp( unsigned short port, int databyte )
{
  dprintf(("MSVCRT: _outp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _outpw    (MSVCRT.396)
 */
unsigned short CDECL MSVCRT__outpw( unsigned short port, unsigned short dataword )
{
  dprintf(("MSVCRT: _outpw not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _outpd    (MSVCRT.397)
 */
unsigned long CDECL MSVCRT__outpd( unsigned short port, unsigned long dataword )
{
  dprintf(("MSVCRT: _outpd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _putws    (MSVCRT.407)
 */
int CDECL MSVCRT__putws( const wchar_t *s )
{
  dprintf(("MSVCRT: _putws not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _set_error_mode    (MSVCRT.421)
 */
int CDECL MSVCRT__set_error_mode( int modeval )
{
  dprintf(("MSVCRT: _set_error_mode not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _set_sbh_threshold    (MSVCRT.422)
 */
int CDECL MSVCRT__set_sbh_threshold( size_t size )
{
  dprintf(("MSVCRT: _set_sbh_threshold not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _strncoll    (MSVCRT.453)
 */
int CDECL MSVCRT__strncoll( const char *s1, const char *s2, size_t n )
{
  dprintf(("MSVCRT: _strncoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _strnicoll    (MSVCRT.455)
 */
int CDECL MSVCRT__strnicoll( const char *s1, const char *s2, size_t n )
{
  dprintf(("MSVCRT: _strnicoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ultow    (MSVCRT.475)
 */
wchar_t * CDECL MSVCRT__ultow( unsigned long value, wchar_t *string, int radix )
{
  dprintf(("MSVCRT: _ultow not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _umask    (MSVCRT.476)
 */
int CDECL MSVCRT__umask( int pmode )
{
  dprintf(("MSVCRT: _umask not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _waccess    (MSVCRT.484)
 */
int CDECL MSVCRT__waccess( const wchar_t *path, int mode )
{
  dprintf(("MSVCRT: _waccess not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wasctime    (MSVCRT.485)
 */
wchar_t * CDECL MSVCRT__wasctime( const struct tm *timeptr )
{
  dprintf(("MSVCRT: _wasctime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wchdir    (MSVCRT.486)
 */
int CDECL MSVCRT__wchdir( const wchar_t *dirname )
{
  dprintf(("MSVCRT: _wchdir not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wchmod    (MSVCRT.487)
 */
int CDECL MSVCRT__wchmod( const wchar_t *filename, int pmode )
{
  dprintf(("MSVCRT: _wchmod not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wcreat    (MSVCRT.489)
 */
int CDECL MSVCRT__wcreat( const wchar_t *filename, int pmode )
{
  dprintf(("MSVCRT: _wcreat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wcsncoll    (MSVCRT.494)
 */
int CDECL MSVCRT__wcsncoll( const wchar_t *string1, const wchar_t *string2, size_t count )
{
  dprintf(("MSVCRT: _wcsncoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wcsnicoll    (MSVCRT.496)
 */
int CDECL MSVCRT__wcsnicoll( const wchar_t *string1, const wchar_t *string2 , size_t count )
{
  dprintf(("MSVCRT: _wcsnicoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wctime       (MSVCRT.501)
 */
wchar_t * CDECL MSVCRT__wctime( const time_t *timer )
{
  dprintf(("MSVCRT: _wctime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecl       (MSVCRT.503)
 */
int CDECL MSVCRT__wexecl( const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wexecl not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecle       (MSVCRT.504)
 */
int CDECL MSVCRT__wexecle( const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wexecle not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexeclp       (MSVCRT.505)
 */
int CDECL MSVCRT__wexeclp( const wchar_t *file, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wexeclp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexeclpe      (MSVCRT.506)
 */
int CDECL MSVCRT__wexeclpe( const wchar_t *file, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wexeclpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecv      (MSVCRT.507)
 */
int CDECL MSVCRT__wexecv( const wchar_t *path, const wchar_t *const argv[] )
{
  dprintf(("MSVCRT: _wexecv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecve     (MSVCRT.508)
 */
int CDECL MSVCRT__wexecve( const wchar_t *path, const wchar_t *const argv[], 
				  const wchar_t *const envp[] )
{
  dprintf(("MSVCRT: _wexecve not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecvp     (MSVCRT.509)
 */
int CDECL MSVCRT__wexecvp( const wchar_t *file, const wchar_t *const argv[] )
{
  dprintf(("MSVCRT: _wexecvp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecvpe     (MSVCRT.510)
 */
int CDECL MSVCRT__wexecvpe( const wchar_t *file, const wchar_t *const argv[], 
				   const wchar_t *const envp[] )
{
  dprintf(("MSVCRT: _wexecvpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfdopen     (MSVCRT.511)
 */
FILE * CDECL MSVCRT__wfdopen( int, const wchar_t *s )
{
  dprintf(("MSVCRT: _wfdopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfindfirst     (MSVCRT.512)
 */
long CDECL MSVCRT__wfindfirst( const wchar_t *filespec, struct _wfinddata_t *fileinfo )
{
  dprintf(("MSVCRT: _wfindfirst not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfindnext     (MSVCRT.514)
 */
int CDECL MSVCRT__wfindnext( long handle, struct _wfinddata_t *fileinfo )
{
  dprintf(("MSVCRT: _wfindnext not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfopen        (MSVCRT.516)
 */
FILE * CDECL MSVCRT__wfopen( const wchar_t *s1, const wchar_t *s2 )
{
  dprintf(("MSVCRT: _wfopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfreopen        (MSVCRT.517)
 */
FILE * CDECL MSVCRT__wfreopen( const wchar_t *s1, const wchar_t *s2, FILE * f)
{
  dprintf(("MSVCRT: _wfreopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfsopen        (MSVCRT.518)
 */
FILE * CDECL MSVCRT__wfsopen( const wchar_t *filename, const wchar_t *mode,  int shflag )
{
  dprintf(("MSVCRT: _wfsopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfullpath        (MSVCRT.519)
 */
wchar_t * CDECL MSVCRT__wfullpath( wchar_t *s1, const wchar_t *s2, size_t n )
{
  dprintf(("MSVCRT: _wfullpath not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wgetcwd          (MSVCRT.520)
 */
wchar_t * CDECL MSVCRT__wgetcwd( wchar_t *buf, size_t size )
{
  dprintf(("MSVCRT: _wgetcwd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wgetdcwd          (MSVCRT.521)
 */
wchar_t * CDECL MSVCRT__wgetdcwd( int drive, wchar_t *buffer, size_t maxlen )
{
  dprintf(("MSVCRT: _wgetdcwd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wgetenv          (MSVCRT.522)
 */
wchar_t * CDECL MSVCRT__wgetenv( const wchar_t *name )
{
  dprintf(("MSVCRT: _wgetenv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wmakepath          (MSVCRT.526)
 */
void CDECL MSVCRT__wmakepath( wchar_t *path, const wchar_t *drive,
	    const wchar_t *dir, const wchar_t *fname, const wchar_t *ext )
{
  dprintf(("MSVCRT: _wmakepath not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _wmkdir           (MSVCRT.527)
 */
int CDECL MSVCRT__wmkdir( const wchar_t *path )
{
  dprintf(("MSVCRT: _wmkdir not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wmktemp           (MSVCRT.528)
 */
wchar_t * CDECL MSVCRT__wmktemp( wchar_t*  )
{
  dprintf(("MSVCRT: _wmktemp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wopen           (MSVCRT.529)
 */
int CDECL MSVCRT__wopen( const wchar_t *s, int n, ... )
{
  dprintf(("MSVCRT: _wopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wperror          (MSVCRT.530)
 */
void CDECL MSVCRT__wperror( const wchar_t *s )
{
  dprintf(("MSVCRT: _wperror not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _wpopen           (MSVCRT.532)
 */
FILE * CDECL MSVCRT__wpopen( const wchar_t *command, const wchar_t *mode )
{
  dprintf(("MSVCRT: _wpopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wputenv          (MSVCRT.533)
 */
int CDECL MSVCRT__wputenv( const wchar_t *env_string )
{
  dprintf(("MSVCRT: _wputenv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wremove          (MSVCRT.534)
 */
int CDECL MSVCRT__wremove( const wchar_t *s )
{
  dprintf(("MSVCRT: _wremove not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wrename          (MSVCRT.535)
 */
int CDECL MSVCRT__wrename( const wchar_t *s1, const wchar_t *s2 )
{
  dprintf(("MSVCRT: _wrename not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wrmdir          (MSVCRT.537)
 */
int CDECL MSVCRT__wrmdir( const wchar_t *path )
{
  dprintf(("MSVCRT: _wrmdir not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wsearchenv          (MSVCRT.538)
 */
void CDECL MSVCRT__wsearchenv( const wchar_t *name, const wchar_t *env_var, wchar_t *buf )
{
  dprintf(("MSVCRT: _wsearchenv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _wsetlocale          (MSVCRT.539)
 */
wchar_t * CDECL MSVCRT__wsetlocale(int category,const wchar_t *locale)
{
  dprintf(("MSVCRT: _wsetlocale not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wsopen          (MSVCRT.540)
 */
int CDECL MSVCRT__wsopen( const wchar_t *s, int n1, int n2, ... )
{
  dprintf(("MSVCRT: _wsopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnl         (MSVCRT.541)
 */
int CDECL MSVCRT__wspawnl( int mode, const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wspawnl not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnle          (MSVCRT.542)
 */
int CDECL MSVCRT__wspawnle( int mode, const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wspawnle not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnlp          (MSVCRT.543)
 */
int CDECL MSVCRT__wspawnlp( int mode, const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wspawnlp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnlpe          (MSVCRT.544)
 */
int CDECL MSVCRT__wspawnlpe( int mode, const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wspawnlpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnv          (MSVCRT.545)
 */
int CDECL MSVCRT__wspawnv( int mode, const wchar_t *path, const wchar_t * const *argv )
{
  dprintf(("MSVCRT: _wspawnv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnve          (MSVCRT.546)
 */
int CDECL MSVCRT__wspawnve( int mode, const wchar_t *path, 
	   const wchar_t * const *argv, const wchar_t * const *envp )
{
  dprintf(("MSVCRT: _wspawnve not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnvp          (MSVCRT.547)
 */
int CDECL MSVCRT__wspawnvp( int mode, const wchar_t *path, const wchar_t * const *argv )
{
  dprintf(("MSVCRT: _wspawnvp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnvpe          (MSVCRT.548)
 */
int CDECL MSVCRT__wspawnvpe( int mode, const wchar_t *path, 
	   const wchar_t * const *argv, const wchar_t * const *envp )
{
  dprintf(("MSVCRT: _wspawnvpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wsplitpath         (MSVCRT.549)
 */
void CDECL MSVCRT__wsplitpath( const wchar_t *path, wchar_t *drive, 
		 wchar_t *dir, wchar_t *fname, wchar_t *ext )
{
  dprintf(("MSVCRT: _wsplitpath not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _wstat          (MSVCRT.550)
 */
int CDECL MSVCRT__wstat( const wchar_t *s, struct _wstat *w )
{
  dprintf(("MSVCRT: _wstat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wstrdate          (MSVCRT.552)
 */
wchar_t * CDECL MSVCRT__wstrdate( wchar_t *buf )
{
  dprintf(("MSVCRT: _wstrdate not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wstrtime          (MSVCRT.553)
 */
wchar_t * CDECL MSVCRT__wstrtime( wchar_t *buf )
{
  dprintf(("MSVCRT: _wstrtime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wsystem           (MSVCRT.554)
 */
int CDECL MSVCRT__wsystem( const wchar_t *cmd )
{
  dprintf(("MSVCRT: _wsystem not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wtempnam          (MSVCRT.555)
 */
wchar_t * CDECL MSVCRT__wtempnam( wchar_t *dir, wchar_t *prefix )
{
  dprintf(("MSVCRT: _wtempnam not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wtmpnam          (MSVCRT.555)
 */
wchar_t * CDECL MSVCRT__wtmpnam( wchar_t *tn )
{
  dprintf(("MSVCRT: _wtmpnam not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wunlink     (MSVCRT.560)
 */
int CDECL MSVCRT__wunlink(const wchar_t *path)
{
  dprintf(("MSVCRT: _wunlink not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wutime     (MSVCRT.561)
 */
int CDECL MSVCRT__wutime( const wchar_t *path, const struct utimbuf * times )
{
  dprintf(("MSVCRT: _wutime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  fgetws    (MSVCRT.597)
 */
wchar_t * CDECL MSVCRT_fgetws( wchar_t *s, int n, FILE *strm )
{
  dprintf(("MSVCRT: fgetws\n"));
  return (fgetws(s, n, strm));
}


/*********************************************************************
 *                  fputws    (MSVCRT.605)
 */
int CDECL MSVCRT_fputws( const wchar_t *s, FILE *strm )
{
  dprintf(("MSVCRT: fputws\n"));
  return (fputws(s, strm));
}


/*********************************************************************
 *                  getwc    (MSVCRT.621)
 */
wint_t CDECL MSVCRT_getwc( FILE * strm)
{
  dprintf(("MSVCRT: getwc\n"));
  return (getwc(strm));
}


/*********************************************************************
 *                  getwchar    (MSVCRT.622)
 */
wint_t CDECL MSVCRT_getwchar( void )
{
  dprintf(("MSVCRT: getwchar\n"));
  return (getwchar());
}


/*********************************************************************
 *                  putwc    (MSVCRT.675)
 */
wint_t CDECL MSVCRT_putwc( wint_t t, FILE * strm)
{
  dprintf(("MSVCRT: putwc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  putwchar    (MSVCRT.676)
 */
wint_t CDECL MSVCRT_putwchar( wint_t t)
{
  dprintf(("MSVCRT: putwchar not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
