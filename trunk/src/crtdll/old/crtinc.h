/* $Id: crtinc.h,v 1.1 2000-11-21 23:49:03 phaller Exp $ */

/* Definitions for the CRTDLL library (CRTDLL.DLL)
 *
 * Copyright 1999-2000 Jens Wiessner
 *
 */


#ifndef MAX_PATHNAME_LEN 
#define MAX_PATHNAME_LEN 260 
#endif 

//SvL: Internal heap allocation definitions for NTDLL
extern HANDLE CRTDLL_hHeap;
#define Heap_Alloc(a)	HeapAlloc(CRTDLL_hHeap, HEAP_ZERO_MEMORY, a)
#define Heap_Free(a)	HeapFree(CRTDLL_hHeap, 0, (PVOID)a)

//SvL: per process heap for CRTDLL
HANDLE CRTDLL_hHeap = 0;
int __fileno_alloc(HANDLE hFile, int mode);

// Defs
#define DOSFS_GetFullName(a,b,c) strcpy(c,a) 


#if (__IBMCPP__ > 300)
#define exception _exception
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

/* Definition for _cabs */
struct complex
{
	double	x;	/* Real part */
	double	y;	/* Imaginary part */
};

typedef VOID (*new_handler_type)(VOID);
typedef void (*_INITTERMFUN)();


CRTDLL_FILE CRTDLL_iob[3];

static CRTDLL_FILE * const CRTDLL_stdin  = &CRTDLL_iob[0];
static CRTDLL_FILE * const CRTDLL_stdout = &CRTDLL_iob[1];
static CRTDLL_FILE * const CRTDLL_stderr = &CRTDLL_iob[2];
static new_handler_type new_handler;


// CRTDLL Exports
double  *CRTDLL_HUGE_dll;        /* CRTDLL.20 */
UINT 	CRTDLL_argc_dll;         /* CRTDLL.23 */
LPSTR 	*CRTDLL_argv_dll;        /* CRTDLL.24 */
int     CRTDLL_mb_cur_max_dll;   /* CRTDLL.31 */
LPSTR  	CRTDLL_acmdln_dll;       /* CRTDLL.38 */
UINT 	CRTDLL_basemajor_dll;    /* CRTDLL.42 */
UINT 	CRTDLL_baseminor_dll;    /* CRTDLL.43 */
UINT 	CRTDLL_baseversion_dll;  /* CRTDLL.44 */
UINT 	CRTDLL_commode_dll;      /* CRTDLL.59 */
UCHAR   *CRTDLL_cpumode_dll;     /* CRTDLL.64 */
USHORT  *CRTDLL_ctype;	         /* CRTDLL.68 */
UINT 	CRTDLL_daylight_dll;	 /* CRTDLL.70 */
LPSTR	CRTDLL_environ_dll;      /* CRTDLL.75 */
LPSTR	CRTDLL_fileinfo_dll;     /* CRTDLL.95 */
UINT 	CRTDLL_fmode_dll;        /* CRTDLL.104 */
LPSTR	CRTDLL_mbctype;	         /* CRTDLL.193 */
UINT 	CRTDLL_osmajor_dll;      /* CRTDLL.239 */
UINT 	CRTDLL_osminor_dll;      /* CRTDLL.240 */
UINT 	CRTDLL_osmode_dll;       /* CRTDLL.241 */
UINT 	CRTDLL_osver_dll;        /* CRTDLL.242 */
UINT 	CRTDLL_osversion_dll;    /* CRTDLL.243 */
USHORT CRTDLL_pctype_dll[] = {0,0}; /* CRTDLL.245 */
LPSTR  	CRTDLL_pgmptr_dll;       /* CRTDLL.246 */
USHORT *CRTDLL_pwctype_dll;	 /* CRTDLL.253 */
UINT 	CRTDLL_timezone_dll;	 /* CRTDLL.304 */
LPSTR	CRTDLL_tzname;		 /* CRTDLL.307 */
UINT 	CRTDLL_winmajor_dll;     /* CRTDLL.326 */
UINT 	CRTDLL_winminor_dll;     /* CRTDLL.327 */
UINT 	CRTDLL_winver_dll;       /* CRTDLL.328 */


//
// Definitions for internal functions
//
void 		qsort1 (char*, char*, size_t,
                    int (* CDECL)(const void*, const void*));


// syserr / sysnerr Defs
const char *CRTDLL_sys_errlist[] = {
  /*  0 EZERO          */ "Error 0",
  /*  1 EPERM          */ "Operation not permitted",
  /*  2 ENOENT         */ "No such file or directory",
  /*  3 ESRCH          */ "No such process",
  /*  4 EINTR          */ "Interrupted system call",
  /*  5 EIO            */ "I/O error",
  /*  6 ENXIO          */ "No such device or address",
  /*  7 E2BIG          */ "Arguments or environment too big",
  /*  8 ENOEXEC        */ "Invalid executable file format",
  /*  9 EBADF          */ "Bad file number",
  /* 10 ECHILD         */ "No children",
  /* 11 EAGAIN         */ "Resource temporarily unavailable",
  /* 12 ENOMEM         */ "Not enough memory",
  /* 13 EACCES         */ "Permission denied",
  /* 14 EFAULT         */ "Bad address",
  /* 15 ENOLCK         */ "No locks available",
  /* 16 EBUSY          */ "Resource busy",
  /* 17 EEXIST         */ "File exists",
  /* 18 EXDEV          */ "Cross-device link",
  /* 19 ENODEV         */ "No such device",
  /* 20 ENOTDIR        */ "Not a directory",
  /* 21 EISDIR         */ "Is a directory",
  /* 22 EINVAL         */ "Invalid argument",
  /* 23 ENFILE         */ "Too many open files in system",
  /* 24 EMFILE         */ "Too many open files",
  /* 25 ENOTTY         */ "Inappropriate ioctl",
  /* 26 EDEADLK        */ "Resource deadlock avoided",
  /* 27 EFBIG          */ "File too large",
  /* 28 ENOSPC         */ "Disk full",
  /* 29 ESPIPE         */ "Invalid seek",
  /* 30 EROFS          */ "Read-only file system",
  /* 31 EMLINK         */ "Too many links",
  /* 32 EPIPE          */ "Broken pipe",
  /* 33 EDOM           */ "Domain error",
  /* 34 ERANGE         */ "Result too large",
  /* 35 ENOTEMPTY      */ "Directory not empty",
  /* 36 EINPROGRESS    */ "Operation now in progress",
  /* 37 ENOSYS         */ "Function not implemented",
  /* 38 ENAMETOOLONG   */ "File name too long",
  /* 39 EDESTADDRREQ   */ "Destination address required",
  /* 40 EMSGSIZE       */ "Message too long",
  /* 41 EPROTOTYPE     */ "Protocol wrong type for socket",
  /* 42 ENOPROTOOPT    */ "Option not supported by protocol",
  /* 43 EPROTONOSUPPORT */ "Protocol not supported",
  /* 44 ESOCKTNOSUPPORT */ "Socket type not supported",
  /* 45 EOPNOTSUPP     */ "Operation not supported on socket",
  /* 46 EPFNOSUPPORT   */ "Protocol family not supported",
  /* 47 EAFNOSUPPORT   */ "Address family not supported by protocol family",
  /* 48 EADDRINUSE     */ "Address already in use",
  /* 49 EADDRNOTAVAIL  */ "Can't assign requested address",
  /* 50 ENETDOWN       */ "Network is down",
  /* 51 ENETUNREACH    */ "Network is unreachable",
  /* 52 ENETRESET      */ "Network dropped connection on reset",
  /* 53 ECONNABORTED   */ "Software caused connection abort",
  /* 54 ECONNRESET     */ "Connection reset by peer",
  /* 55 ENOBUFS        */ "No buffer space available",
  /* 56 EISCONN        */ "Socket is already connected",
  /* 57 ENOTCONN       */ "Socket is not connected",
  /* 58 ESHUTDOWN      */ "Can't send after socket shutdown",
  /* 59 ETOOMANYREFS   */ "Too many references: can't splice",
  /* 60 ETIMEDOUT      */ "Connection timed out",
  /* 61 ECONNREFUSED   */ "Connection refused",
  /* 62 ELOOP          */ "Too many levels of symbolic links",
  /* 63 ENOTSOCK       */ "Socket operation on non-socket",
  /* 64 EHOSTDOWN      */ "Host is down",
  /* 65 EHOSTUNREACH   */ "No route to host",
  /* 66 EALREADY       */ "Operation already in progress"
};

int __sys_nerr = sizeof(CRTDLL_sys_errlist) / sizeof(CRTDLL_sys_errlist[0]);
int*	CRTDLL_sys_nerr_dll = &__sys_nerr;


// atexit
void (*_atexit_v[64])(void);
int _atexit_n = 0;
