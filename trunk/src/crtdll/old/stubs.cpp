/* $Id: stubs.cpp,v 1.1 2000-11-21 23:49:04 phaller Exp $ */

/*
 * The C RunTime DLL
 * 
 * Implements C run-time functionality as known from UNIX.
 *
 * Partialy based on Wine
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997 Uwe Bonnes
 * Copyright 1999-2000 Jens Wiessner
 *
 * CRTDLL - Stubs
 *
 */

#include <odin.h>
#include <os2win.h>
#include <conio.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>


/*********************************************************************
 *	__dllonexit           			(CRTDLL.25)
 */
VOID CDECL CRTDLL___dllonexit ()
{	
  dprintf(("__dllonexit not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  __fpecode            (CRTDLL.27)
 */
int * CDECL CRTDLL___fpecode ( void )
{	
  dprintf(("CRTDLL: __fpecode not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	__pxcptinfoptrs		   		(CRTDLL.32)
 */
void ** CDECL CRTDLL___pxcptinfoptrs (void)
{
  dprintf(("CRTDLL: __pxcptinfoptrs not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}


/*********************************************************************
 *	_abnormal_termination			(CRTDLL.36)
 */
int CDECL CRTDLL__abnormal_termination(void)
{
  dprintf(("CRTDLL: _abnormal_termination  not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _chgsign	 (CRTDLL.53)
 */
double CDECL CRTDLL__chgsign(double x)
{
  dprintf(("CRTDLL: _chgsign not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _commit    (CRTDLL.58)
 */
int CDECL CRTDLL__commit( int fd )
{
  dprintf(("CRTDLL: _commit not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _copysign    (CRTDLL.62)
 */
double CDECL CRTDLL__copysign( double d, double s )
{
  dprintf(("CRTDLL: _copysign not implemented\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_expand   				(CRTDLL.88)
 */
void * CDECL CRTDLL__expand( void *ptr, size_t size )
{
  dprintf(("CRTDLL: _expand not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_filbuf					(CRTDLL.94)
 */
int CDECL CRTDLL__filbuf(FILE * f)
{
  dprintf(("CRTDLL: _filbuf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *	_findfirst				(CRTDLL.099)
 */
DWORD CDECL CRTDLL__findfirst(const char *_name, struct _finddata_t *result)
{
  dprintf(("CRTDLL: _findfirst not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_findnext				(CRTDLL.100)
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
 *	_flsbuf					(CRTDLL.102)
 */
INT CDECL CRTDLL__flsbuf(int i, FILE * f)
{
  dprintf(("CRTDLL: _flsbuf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fpclass     (CRTDLL.105)
 */
INT CDECL CRTDLL__fpclass( double d )
{
  dprintf(("CRTDLL: _fpclass not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_fpieee_flt				(CRTDLL.106)
 */
INT CDECL CRTDLL__fpieee_flt( unsigned long exc_code, struct _EXCEPTION_POINTERS *exc_info, int handler)
{
  dprintf(("CRTDLL: _fpieee_flt not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_fputwchar				(CRTDLL.109)
 */
wint_t CDECL CRTDLL__fputwchar( wint_t )
{
  dprintf(("CRTDLL: _fputwchar not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_fsopen					(CRTDLL.110)
 */
FILE * CDECL CRTDLL__fsopen( const char *file, const char *mode, int shflag )
{
  dprintf(("CRTDLL: _fsopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_futime					(CRTDLL.115)
 */
int CDECL CRTDLL__futime( int handle, struct _utimbuf *filetime )
{
  dprintf(("CRTDLL: _futime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _get_osfhandle     (CRTDLL.117)
 */
void* CDECL CRTDLL__get_osfhandle( int fileno )
{
  dprintf(("CRTDLL: _get_osfhandle not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _getdiskfree     (CRTDLL.122)
 */
unsigned int CDECL CRTDLL__getdiskfree( unsigned int drive, struct _diskfree_t *diskspace)
{
  dprintf(("CRTDLL: _getdiskfree not implemented\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _getdllprocaddr     (CRTDLL.123)
 */
FARPROC CDECL  CRTDLL__getdllprocaddr(HMODULE hModule,char * lpProcName, int iOrdinal)
{
  dprintf(("CRTDLL: _getdllprocaddr not implemented.\n"));   
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_heapwalk				(CRTDLL.133)
 */
int CDECL CRTDLL__heapwalk( struct _heapinfo *entry )
{
  dprintf(("CRTDLL: _heapwalk not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _isatty       (CRTDLL.137)
 */
BOOL CDECL CRTDLL__isatty(DWORD x)
{
   dprintf(("CRTDLL: _isatty(%ld) not implemented.\n",x));
   SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
   return TRUE;
}


/*********************************************************************
 *                  _ismbbalnum     (CRTDLL.139)
 */
int CDECL CRTDLL__ismbbalnum( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbalnum\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbbalpha     (CRTDLL.140)
 */
int CDECL CRTDLL__ismbbalpha( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbalpha\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbbgraph     (CRTDLL.141)
 */
int CDECL CRTDLL__ismbbgraph( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbgraph\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbbkalnum     (CRTDLL.142)
 */
int CDECL CRTDLL__ismbbkalnum( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbkalnum\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbbkana     (CRTDLL.143)
 */
int CDECL CRTDLL__ismbbkana( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbkana\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbbkpunct     (CRTDLL.144)
 */
int CDECL CRTDLL__ismbbkpunct( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbkpunct\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbblead     (CRTDLL.145)
 */
int CDECL CRTDLL__ismbblead( unsigned int c )
{
  dprintf(("CRTDLL: _ismbblead\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbbprint     (CRTDLL.146)
 */
int CDECL CRTDLL__ismbbprint( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbprint\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbbpunct     (CRTDLL.147)
 */
int CDECL CRTDLL__ismbbpunct( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbpunct\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbbtrail     (CRTDLL.148)
 */
int CDECL CRTDLL__ismbbtrail( unsigned int c )
{
  dprintf(("CRTDLL: _ismbbtrail\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbcalpha     (CRTDLL.149)
 */
int CDECL CRTDLL__ismbcalpha( unsigned int c )
{
  dprintf(("CRTDLL: _ismbcalpha\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbcdigit     (CRTDLL.150)
 */
int CDECL CRTDLL__ismbcdigit( unsigned int c )
{
  dprintf(("CRTDLL: _ismbcdigit\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbchira     (CRTDLL.151)
 */
int CDECL CRTDLL__ismbchira( unsigned int c )
{
  dprintf(("CRTDLL: _ismbchira\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbckata     (CRTDLL.152)
 */
int CDECL CRTDLL__ismbckata( unsigned int c )
{
  dprintf(("CRTDLL: _ismbckata\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_ismbcl0				(CRTDLL.153)
 */
int CDECL CRTDLL__ismbcl0( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl0 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_ismbcl1				(CRTDLL.154)
 */
int CDECL CRTDLL__ismbcl1( unsigned int ch )
{
  dprintf(("CRTDLL: _ismbcl1 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_ismbcl2				(CRTDLL.155)
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
  dprintf(("CRTDLL: _ismbclegal\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbclower     (CRTDLL.157)
 */
int CDECL CRTDLL__ismbclower( unsigned int c )
{
  dprintf(("CRTDLL: _ismbclower\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbcprint     (CRTDLL.158)
 */
int CDECL CRTDLL__ismbcprint( unsigned int c )
{
  dprintf(("CRTDLL: _ismbcprint\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_ismbcspace				(CRTDLL.159)
 */
int CDECL CRTDLL__ismbcspace( unsigned int c )
{
  dprintf(("CRTDLL: _ismbcspace not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_ismbcsymbol				(CRTDLL.160)
 */
int CDECL CRTDLL__ismbcsymbol( unsigned int c )
{
  dprintf(("CRTDLL: _ismbcsymbol not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbcupper     (CRTDLL.161)
 */
int CDECL CRTDLL__ismbcupper( unsigned int c )
{
  dprintf(("CRTDLL: _ismbcupper\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbslead     (CRTDLL.162)
 */
int CDECL CRTDLL__ismbslead(const unsigned char *str, const unsigned char *t)
{
  dprintf(("CRTDLL: _ismbslead\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *                  _ismbstrail     (CRTDLL.163)
 */
int CDECL CRTDLL__ismbstrail(const unsigned char *str, const unsigned char *t)
{
  dprintf(("CRTDLL: _ismbstrail\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
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
 *                  _lfind     (CRTDLL.170)
 */
void * CDECL CRTDLL__lfind(const void *key, const void *base, size_t *nelp,
         size_t width, int (*compar)(const void *, const void *))
{
  dprintf(("CRTDLL: _lfind not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _locking        (CRTDLL.173)
 */
int CDECL CRTDLL__locking( int fd, int mode, long nbytes )
{
  dprintf(("CRTDLL: _locking not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_logb					(CRTDLL.174)
 */
double CDECL CRTDLL__logb( double x )
{
  dprintf(("CRTDLL: _logb not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_lsearch				(CRTDLL.177)
 */
void * CDECL CRTDLL__lsearch(const void *key, void *base, size_t *nelp, size_t width,
         int (*compar)(const void *, const void *))
{
  dprintf(("CRTDLL: _lsearch not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbbtombc        (CRTDLL.182)
 */
unsigned int CDECL CRTDLL__mbbtombc( unsigned int c )
{
  dprintf(("CRTDLL: _mbbtombc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbbtype        (CRTDLL.183)
 */
int CDECL CRTDLL__mbbtype( unsigned char c, int type )
{
  dprintf(("CRTDLL: _mbbtype not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbccpy        (CRTDLL.184)
 */
void CDECL CRTDLL__mbccpy( unsigned char *dst, const unsigned char *src )
{
  dprintf(("CRTDLL: _mbccpy not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _mbcjistojms     (CRTDLL.185)
 */
int CDECL CRTDLL__mbcjistojms( unsigned int c )
{
  dprintf(("CRTDLL: _mbcjistojms not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbcjmstojis     (CRTDLL.186)
 */
int CDECL CRTDLL__mbcjmstojis( unsigned int c )
{
  dprintf(("CRTDLL: _mbcjmstojis not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbclen    (CRTDLL.187)
 */
size_t CDECL CRTDLL__mbclen( const unsigned char *s )
{
  dprintf(("CRTDLL: _mbclen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctohira     (CRTDLL.188)
 */
int CDECL CRTDLL__mbctohira( unsigned int c )
{
  dprintf(("CRTDLL: _mbctohira not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctokata     (CRTDLL.189)
 */
int CDECL CRTDLL__mbctokata( unsigned int c )
{
  dprintf(("CRTDLL: _mbctokata not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctolower     (CRTDLL.190)
 */
unsigned int CDECL CRTDLL__mbctolower( unsigned int c )
{
  dprintf(("CRTDLL: _mbctolower not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctombb        (CRTDLL.191)
 */
unsigned int CDECL CRTDLL__mbctombb( unsigned int c )
{
  dprintf(("CRTDLL: _mbctombb not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbctoupper     (CRTDLL.192)
 */
unsigned int CDECL CRTDLL__mbctoupper( unsigned int c )
{
  dprintf(("CRTDLL: _mbctoupper not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbsbtype     (CRTDLL.194)
 */
int CDECL CRTDLL__mbsbtype( const unsigned char *str, int n )
{
  dprintf(("CRTDLL: _mbsbtype not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _mbscspn        (CRTDLL.199)
 */
size_t CDECL CRTDLL__mbscspn( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbscspn not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsdec    (CRTDLL.200)
 */
unsigned char * CDECL CRTDLL__mbsdec( const unsigned char *str, const unsigned char *cur )
{
  dprintf(("CRTDLL: _mbsdec not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbslwr    (CRTDLL.205)
 */
unsigned char * CDECL CRTDLL__mbslwr( unsigned char *x )
{
  dprintf(("CRTDLL: _mbslwr not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnbcat  (CRTDLL.206)
 */
unsigned char * CDECL CRTDLL__mbsnbcat( unsigned char *dst, const unsigned char *src, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnbcmp  (CRTDLL.207)
 */
int CDECL CRTDLL__mbsnbcmp( const unsigned char *str1, const unsigned char *str2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnbcnt  (CRTDLL.208)
 */
size_t CDECL CRTDLL__mbsnbcnt( const unsigned char *str, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcnt not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnbcpy  (CRTDLL.209)
 */
unsigned char * CDECL CRTDLL__mbsnbcpy( unsigned char *str1, const unsigned char *str2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbcpy not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnbicmp (CRTDLL.210)
 */
int CDECL CRTDLL__mbsnbicmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnbicmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnbset (CRTDLL.211)
 */
unsigned char * CDECL CRTDLL__mbsnbset( unsigned char *src, unsigned int val, size_t count )
{
  dprintf(("CRTDLL: _mbsnbset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsncat (CRTDLL.212)
 */
unsigned char * CDECL CRTDLL__mbsncat( unsigned char *dst, const unsigned char *src, size_t n )
{
  dprintf(("CRTDLL: _mbsncat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnccnt (CRTDLL.213)
 */
size_t CDECL CRTDLL__mbsnccnt( const unsigned char *str, size_t n )
{
  dprintf(("CRTDLL: _mbsnccnt not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsncmp (CRTDLL.214)
 */
int CDECL CRTDLL__mbsncmp( const unsigned char *str1, const unsigned char *str2, size_t n )
{
  dprintf(("CRTDLL: _mbsncmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsncpy (CRTDLL.215)
 */
unsigned char * CDECL CRTDLL__mbsncpy( unsigned char *str1, const unsigned char *str2, size_t n )
{
  dprintf(("CRTDLL: _mbsncpy not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnextc (CRTDLL.216)
 */
unsigned int CDECL CRTDLL__mbsnextc( const unsigned char *src )
{
  dprintf(("CRTDLL: _mbsnextc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnicmp (CRTDLL.217)
 */
int CDECL CRTDLL__mbsnicmp( const unsigned char *s1, const unsigned char *s2, size_t n )
{
  dprintf(("CRTDLL: _mbsnicmp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsninc (CRTDLL.218)
 */
unsigned char * CDECL CRTDLL__mbsninc( const unsigned char *str, size_t n )
{
  dprintf(("CRTDLL: _mbsninc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsnset (CRTDLL.219)
 */
unsigned char * CDECL CRTDLL__mbsnset( unsigned char *src, unsigned int val, size_t count )
{
  dprintf(("CRTDLL: _mbsnset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbspbrk   (CRTDLL.220)
 */
unsigned char * CDECL CRTDLL__mbspbrk( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbspbrk not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsrev    (CRTDLL.222)
 */
unsigned char * CDECL CRTDLL__mbsrev( unsigned char *s )
{
  dprintf(("CRTDLL: _mbsrev not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsset    (CRTDLL.223)
 */
unsigned char * CDECL CRTDLL__mbsset( unsigned char *src, unsigned int c )
{
  dprintf(("CRTDLL: _mbsset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsspn   (CRTDLL.224)
 */
size_t CDECL CRTDLL__mbsspn( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbsspn not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsspnp   (CRTDLL.225)
 */
unsigned char * CDECL CRTDLL__mbsspnp( const unsigned char *s1, const unsigned char *s2 )
{
  dprintf(("CRTDLL: _mbsspnp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbstok    (CRTDLL.227)
 */
unsigned char * CDECL CRTDLL__mbstok( unsigned char *s, const unsigned char *delim )
{
  dprintf(("CRTDLL: _mbstok not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbstrlen   (CRTDLL.228)
 */
size_t CDECL CRTDLL__mbstrlen(const char *string)
{
  dprintf(("CRTDLL: _mbstrlen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _mbsupr    (CRTDLL.229)
 */
unsigned char * CDECL CRTDLL__mbsupr( unsigned char *x )
{
  dprintf(("CRTDLL: _mbsupr not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
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
 *	_pclose					(CRTDLL.244)
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
INT CDECL CRTDLL__pipe(int _fildes[2], unsigned int size, int mode )
{
  dprintf(("CRTDLL: _pipe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _popen    (CRTDLL.248)
 */
FILE * CDECL CRTDLL__popen(const char *cm, const char *md)
{
  dprintf(("CRTDLL: _popen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _scalb 	 (CRTDLL.259)
 */
double CDECL CRTDLL__scalb( double x, long e )
{
  dprintf(("CRTDLL: _scalb not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	_snprintf				(CRTDLL.266)
 */
int  CDECL CRTDLL__snprintf( char *buf, size_t bufsize, const char *fmt, ... )
{
  dprintf(("CRTDLL: _snprintf(%08xh, %08xh, %08xh) not implemented\n",
           buf,
           bufsize,
           fmt));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_snwprintf				(CRTDLL.267)
 */
int  CDECL CRTDLL__snwprintf( wchar_t *buf, size_t bufsize, const wchar_t *fmt, ... )
{
  dprintf(("CRTDLL: _snwprintf(%08xh, %08xh, %08xh) not implemented\n",
           buf,
           bufsize,
           fmt));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/*********************************************************************
 *	_tzset					(CRTDLL.308)
 */
void CDECL CRTDLL__tzset( void )
{
  dprintf(("CRTDLL: _tzset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *	_vsnwprintf				(CRTDLL.316)
 */
int CDECL CRTDLL__vsnwprintf( wchar_t *s1, size_t n, const wchar_t *s2, va_list arg )
{
  dprintf(("CRTDLL: _vsnwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	fwprintf				(CRTDLL.382)
 */
int CDECL CRTDLL_fwprintf( FILE *iop, const wchar_t *fmt, ... )
{
  dprintf(("CRTDLL: fwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	fwscanf					(CRTDLL.384)
 */
int CDECL CRTDLL_fwscanf( FILE *strm, const wchar_t *format, ... )
{
  dprintf(("CRTDLL: fwscanf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  is_wctype    (CRTDLL.390)
 */
INT CDECL CRTDLL_is_wctype(wint_t wc, wctype_t wctype)
{
  dprintf(("CRTDLL: is_wctype not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	isleadbyte				(CRTDLL.396)
 */
int CDECL CRTDLL_isleadbyte(int i)
{
  dprintf(("CRTDLL: isleadbyte(%08xh) not implemented.\n", i));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	vfwprintf				(CRTDLL.495)
 */
int CDECL CRTDLL_vfwprintf( FILE *F, const wchar_t *s, va_list arg )
{
  dprintf(("CRTDLL: vfwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	vwprintf				(CRTDLL.499)
 */
int CDECL CRTDLL_vwprintf( const wchar_t *s, va_list arg)
{
  dprintf(("CRTDLL: vwprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	wprintf					(CRTDLL.522)
 */
int CDECL CRTDLL_wprintf( const wchar_t *s, ... )
{
  dprintf(("CRTDLL: wprintf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *	wscanf					(CRTDLL.523)
 */
int CDECL CRTDLL_wscanf( const wchar_t *s, ... )
{
  dprintf(("CRTDLL: wscanf not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

