/* $Id: crtinc.h,v 1.4 1999-10-04 09:55:56 sandervl Exp $ */

/* Definitions for the CRTDLL library (CRTDLL.DLL)
 *
 * Copyright 1999 Jens Wiessner
 */


#define MB_LEN_MAX	2
#ifndef MAX_PATHNAME_LEN 
#define MAX_PATHNAME_LEN 260 
#endif 

#define DOSFS_GetFullName(a,b,c) strcpy(c,a) 

#if defined(__GNUC__) && defined(__i386__)
#define USING_REAL_FPU
#define DO_FPU(x,y) __asm__ __volatile__( x " %0;fwait" : "=m" (y) : )
#define POP_FPU(x) DO_FPU("fstpl",x)
#endif

typedef struct
{
    HANDLE handle;
    int      pad[7];
} CRTDLL_FILE, *PCRTDLL_FILE;


/* Definition of a full DOS file name */
typedef struct
{
    char  long_name[MAX_PATHNAME_LEN];  /* Long pathname in Unix format */
    char  short_name[MAX_PATHNAME_LEN]; /* Short pathname in DOS 8.3 format */
    int   drive;
} DOS_FULL_NAME;


typedef VOID (*new_handler_type)(VOID);
typedef void (*_INITTERMFUN)();


CRTDLL_FILE CRTDLL_iob[3];

static CRTDLL_FILE * const CRTDLL_stdin  = &CRTDLL_iob[0];
static CRTDLL_FILE * const CRTDLL_stdout = &CRTDLL_iob[1];
static CRTDLL_FILE * const CRTDLL_stderr = &CRTDLL_iob[2];
static new_handler_type new_handler;

CRTDLL_FILE * CDECL CRTDLL__fdopen(INT handle, LPCSTR mode);



UINT 	CRTDLL_argc_dll;         /* CRTDLL.23 */
LPSTR 	*CRTDLL_argv_dll;        /* CRTDLL.24 */
LPSTR  	CRTDLL_acmdln_dll;       /* CRTDLL.38 */
UINT 	CRTDLL_basemajor_dll;    /* CRTDLL.42 */
UINT 	CRTDLL_baseminor_dll;    /* CRTDLL.43 */
UINT 	CRTDLL_baseversion_dll;  /* CRTDLL.44 */
UINT 	CRTDLL_commode_dll;      /* CRTDLL.59 */
UINT 	CRTDLL_daylight_dll;	 /* CRTDLL.70 */
LPSTR	CRTDLL_environ_dll;      /* CRTDLL.75 */
LPSTR	CRTDLL_fileinfo_dll;     /* CRTDLL.95 */
UINT 	CRTDLL_fmode_dll;        /* CRTDLL.104 */
UINT 	CRTDLL_osmajor_dll;      /* CRTDLL.239 */
UINT 	CRTDLL_osminor_dll;      /* CRTDLL.240 */
UINT 	CRTDLL_osmode_dll;       /* CRTDLL.241 */
UINT 	CRTDLL_osver_dll;        /* CRTDLL.242 */
UINT 	CRTDLL_osversion_dll;    /* CRTDLL.243 */
UINT 	CRTDLL_sys_errlist;      /* CRTDLL.300 */
UINT 	CRTDLL_sys_nerr_dll;     /* CRTDLL.301 */
UINT 	CRTDLL_timezone_dll;	 /* CRTDLL.304 */
LPSTR	CRTDLL_tzname;		 /* CRTDLL.307 */
UINT 	CRTDLL_winmajor_dll;     /* CRTDLL.326 */
UINT 	CRTDLL_winminor_dll;     /* CRTDLL.327 */
UINT 	CRTDLL_winver_dll;       /* CRTDLL.328 */

INT CDECL NTDLL__wcsicmp( LPCWSTR str1, LPCWSTR str2 );
INT CDECL CRTDLL_vfprintf( CRTDLL_FILE *file, LPSTR format, va_list args );
VOID* CDECL CRTDLL_malloc(DWORD size);

