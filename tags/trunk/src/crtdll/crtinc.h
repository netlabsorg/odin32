/* $Id: crtinc.h,v 1.1 1999-09-07 20:41:45 sandervl Exp $ */

/* Definitions for the CRTDLL library (CRTDLL.DLL)
 *
 * Copyright 1999 Jens Wiessner
 */


#define CRTDLL_RAND_MAX         0x7fff 
#define MB_LEN_MAX	2

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

typedef VOID (*new_handler_type)(VOID);
typedef void (*_INITTERMFUN)();


CRTDLL_FILE CRTDLL_iob[3];

static CRTDLL_FILE * const CRTDLL_stdin  = &CRTDLL_iob[0];
static CRTDLL_FILE * const CRTDLL_stdout = &CRTDLL_iob[1];
static CRTDLL_FILE * const CRTDLL_stderr = &CRTDLL_iob[2];
static new_handler_type new_handler;

CRTDLL_FILE * CDECL CRTDLL__fdopen(INT handle, LPCSTR mode);

UINT CRTDLL_argc_dll;         /* CRTDLL.23 */
LPSTR *CRTDLL_argv_dll;         /* CRTDLL.24 */
LPSTR  CRTDLL_acmdln_dll;       /* CRTDLL.38 */
UINT CRTDLL_basemajor_dll;    /* CRTDLL.42 */
UINT CRTDLL_baseminor_dll;    /* CRTDLL.43 */
UINT CRTDLL_baseversion_dll;  /* CRTDLL.44 */
UINT CRTDLL_commode_dll;      /* CRTDLL.59 */
LPSTR  CRTDLL_environ_dll;      /* CRTDLL.75 */
UINT CRTDLL_fmode_dll;        /* CRTDLL.104 */
UINT CRTDLL_osmajor_dll;      /* CRTDLL.241 */
UINT CRTDLL_osminor_dll;      /* CRTDLL.242 */
UINT CRTDLL_osmode_dll;       /* CRTDLL.243 */
UINT CRTDLL_osver_dll;        /* CRTDLL.244 */
UINT CRTDLL_osversion_dll;    /* CRTDLL.245 */
UINT CRTDLL_winmajor_dll;     /* CRTDLL.329 */
UINT CRTDLL_winminor_dll;     /* CRTDLL.330 */
UINT CRTDLL_winver_dll;       /* CRTDLL.331 */

INT         WINAPI lstrncmpiA(LPCSTR,LPCSTR,INT);

