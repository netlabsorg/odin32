/* $Id: crtinc.h,v 1.8 1999-11-28 17:22:27 sandervl Exp $ */

/* Definitions for the CRTDLL library (CRTDLL.DLL)
 *
 * Copyright 1999 Jens Wiessner
 */

#define MB_LEN_MAX	2
#ifndef MAX_PATHNAME_LEN 
#define MAX_PATHNAME_LEN 260 
#endif 

// Errno Defs
#define EPERM		1	/* Operation not permitted */
#define	ENOFILE		2	/* No such file or directory */
#define	ENOENT		2
#define	ESRCH		3	/* No such process */
#define	EINTR		4	/* Interrupted function call */
#define	EIO		5	/* Input/output error */
#define	ENXIO		6	/* No such device or address */
#define	E2BIG		7	/* Arg list too long */
#define	ENOEXEC		8	/* Exec format error */
#define	EBADF		9	/* Bad file descriptor */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Resource temporarily unavailable */
#define	ENOMEM		12	/* Not enough space */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
/* 15 - Unknown Error */
#define	EBUSY		16	/* strerror reports "Resource device" */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Improper link (cross-device link?) */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* Too many open files in system */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Inappropriate I/O control operation */
/* 26 - Unknown Error */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Invalid seek (seek on a pipe?) */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Domain error (math functions) */
#define	ERANGE		34	/* Result too large (possibly too small) */
/* 35 - Unknown Error */
#define	EDEADLOCK	36	/* Resource deadlock avoided (non-Cyg) */
#define	EDEADLK		36
/* 37 - Unknown Error */
#define	ENAMETOOLONG	38	/* Filename too long (91 in Cyg?) */
#define	ENOLCK		39	/* No locks available (46 in Cyg?) */
#define	ENOSYS		40	/* Function not implemented (88 in Cyg?) */
#define	ENOTEMPTY	41	/* Directory not empty (90 in Cyg?) */
#define	EILSEQ		42	/* Illegal byte sequence */


// MBC Defs
#define _MBC_SINGLE	 0	
#define _MBC_LEAD	 1	
#define _MBC_TRAIL	 2		
#define _MBC_ILLEGAL	-1		

#define _MB_CP_SBCS      0
#define _MB_CP_OEM      -2
#define _MB_CP_ANSI     -3
#define _MB_CP_LOCALE   -4

#define _KNJ_M  ((char)0x01)    /* Non-punctuation of Kana-set */
#define _KNJ_P  ((char)0x02)    /* Punctuation of Kana-set */
#define _KNJ_1  ((char)0x04)    /* Legal 1st byte of double byte stream */
#define _KNJ_2  ((char)0x08)    /* Legal 2nd btye of double byte stream */

#define ___     0
#define _1_     _KNJ_1 /* Legal 1st byte of double byte code */
#define __2     _KNJ_2 /* Legal 2nd byte of double byte code */
#define _M_     _KNJ_M /* Non-puntuation in Kana-set */
#define _P_     _KNJ_P /* Punctuation of Kana-set */
#define _12     (_1_|__2)
#define _M2     (_M_|__2)
#define _P2     (_P_|__2)

#define CASE_DIFF (0x8281 - 0x8260)


// fpclass Defs
#define FP_SNAN       0x0001  //    signaling NaN
#define	FP_QNAN       0x0002  //    quiet NaN
#define	FP_NINF       0x0004  //    negative infinity
#define FP_PINF       0x0200  //    positive infinity
#define FP_NDENORM    0x0008  //    negative denormalized non-zero
#define FP_PDENORM    0x0010  //    positive denormalized non-zero
#define FP_NZERO      0x0020  //    negative zero
#define FP_PZERO      0x0040  //    positive zero
#define FP_NNORM      0x0080  //    negative normalized non-zero
#define FP_PNORM      0x0100  //    positive normalized non-zero


// Defs
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

/* Definition for _cabs */
struct _complex
{
	double	x;	/* Real part */
	double	y;	/* Imaginary part */
};


#ifndef _DISKFREE_T_DEFINED
#define _DISKFREE_T_DEFINED
#define _DISKFREE_T_DEFINED_
struct _diskfree_t {
        unsigned total_clusters;
        unsigned avail_clusters;
        unsigned sectors_per_cluster;
        unsigned bytes_per_sector;
};
#define diskfree_t _diskfree_t
#endif


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
// UINT 	CRTDLL_sys_errlist;      /* CRTDLL.300 */
// UINT 	CRTDLL_sys_nerr_dll;     /* CRTDLL.301 */
UINT 	CRTDLL_timezone_dll;	 /* CRTDLL.304 */
LPSTR	CRTDLL_tzname;		 /* CRTDLL.307 */
UINT 	CRTDLL_winmajor_dll;     /* CRTDLL.326 */
UINT 	CRTDLL_winminor_dll;     /* CRTDLL.327 */
UINT 	CRTDLL_winver_dll;       /* CRTDLL.328 */


// CRTDLL Functions
CRTDLL_FILE * 	CDECL CRTDLL__fdopen(INT handle, LPCSTR mode);
INT 		CDECL NTDLL__wcsicmp( LPCWSTR str1, LPCWSTR str2 );
INT 		CDECL CRTDLL_vfprintf( CRTDLL_FILE *file, LPSTR format, va_list args );
VOID *		CDECL CRTDLL_malloc(DWORD size);
long 		CDECL CRTDLL__lseek(int handle,long offset,int origin);
long 		CDECL CRTDLL__filelength( int i );
VOID 		CDECL CRTDLL__exit(DWORD ret);
INT		CDECL CRTDLL_isalnum(int i);
int 		CDECL CRTDLL_isgraph(int i);
int 		CDECL CRTDLL__access(const char *path,int mode);
int 		CDECL CRTDLL__getch(void);
size_t 		CDECL CRTDLL_fread( void *ptr, size_t size, size_t n, FILE *fp );
int 		CDECL CRTDLL__mbbtype( unsigned char c, int type );
unsigned char * CDECL CRTDLL__mbscpy( unsigned char *s1, const unsigned char *s2 );
LPSTR 		CDECL CRTDLL__mbsinc( LPCSTR str );
INT 		CDECL CRTDLL__mbslen( LPCSTR str );
int 		CDECL CRTDLL__ismbbkalnum( unsigned int c );
int 		CDECL CRTDLL__ismbbkana( unsigned int c );
int 		CDECL CRTDLL__ismbbalpha( unsigned int c );
int 		CDECL CRTDLL__ismbbtrail( unsigned int c );
int 		CDECL CRTDLL__ismbblead( unsigned int c );
char * 		CDECL CRTDLL_getenv( const char *name );
int 		CDECL CRTDLL__isnan( double __x );


//
// Definitions for internal functions
//
int 		__set_errno (int error);
unsigned int 	_mbbtoupper(unsigned int c);
unsigned int 	_mbbtolower(unsigned int c);
size_t 		_mbclen2(const unsigned int s);
int 		_isinf(double __x);


//
// MBC Includes
//
static unsigned short han_to_zen_ascii_table[0x5f] = {
  0x8140, 0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166,
  0x8169, 0x816a, 0x8196, 0x817b, 0x8143, 0x817c, 0x8144, 0x815e,
  0x824f, 0x8250, 0x8251, 0x8252, 0x8253, 0x8254, 0x8255, 0x8256,
  0x8257, 0x8258, 0x8146, 0x8147, 0x8183, 0x8181, 0x8184, 0x8148,
  0x8197, 0x8260, 0x8261, 0x8262, 0x8263, 0x8264, 0x8265, 0x8266,
  0x8267, 0x8268, 0x8269, 0x826a, 0x826b, 0x826c, 0x826d, 0x826e,
  0x826f, 0x8270, 0x8271, 0x8272, 0x8273, 0x8274, 0x8275, 0x8276,
  0x8277, 0x8278, 0x8279, 0x816d, 0x818f, 0x816e, 0x814f, 0x8151,
  0x8165, 0x8281, 0x8282, 0x8283, 0x8284, 0x8285, 0x8286, 0x8287,
  0x8288, 0x8289, 0x828a, 0x828b, 0x828c, 0x828d, 0x828e, 0x828f,
  0x8290, 0x8291, 0x8292, 0x8293, 0x8294, 0x8295, 0x8296, 0x8297,
  0x8298, 0x8299, 0x829a, 0x816f, 0x8162, 0x8170, 0x8150
};
static unsigned short han_to_zen_kana_table[0x40] = {
  0x8140, 0x8142, 0x8175, 0x8176, 0x8141, 0x8145, 0x8392, 0x8340,
  0x8342, 0x8344, 0x8346, 0x8348, 0x8383, 0x8385, 0x8387, 0x8362,
  0x815b, 0x8341, 0x8343, 0x8345, 0x8347, 0x8349, 0x834a, 0x834c,
  0x834e, 0x8350, 0x8352, 0x8354, 0x8356, 0x8358, 0x835a, 0x835c,
  0x835e, 0x8360, 0x8363, 0x8365, 0x8367, 0x8369, 0x836a, 0x836b,
  0x836c, 0x836d, 0x836e, 0x8371, 0x8374, 0x8377, 0x837a, 0x837d,
  0x837e, 0x8380, 0x8381, 0x8382, 0x8384, 0x8386, 0x8388, 0x8389,
  0x838a, 0x838b, 0x838c, 0x838d, 0x838f, 0x8393, 0x814a, 0x814b
};
static unsigned char zen_to_han_kana_table[0x8396-0x8340+1] = {
  0xa7, 0xb1, 0xa8, 0xb2, 0xa9, 0xb3, 0xaa, 0xb4,
  0xab, 0xb5, 0xb6, 0xb6, 0xb7, 0xb7, 0xb8, 0xb8,
  0xb9, 0xb9, 0xba, 0xba, 0xbb, 0xbb, 0xbc, 0xbc,
  0xbd, 0xbd, 0xbe, 0xbe, 0xbf, 0xbf, 0xc0, 0xc0,
  0xc1, 0xc1, 0xaf, 0xc2, 0xc2, 0xc3, 0xc3, 0xc4,
  0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xca,
  0xca, 0xcb, 0xcb, 0xcb, 0xcc, 0xcc, 0xcc, 0xcd,
  0xcd, 0xcd, 0xce, 0xce, 0xce, 0xcf, 0xd0, 0,
  0xd1, 0xd2, 0xd3, 0xac, 0xd4, 0xad, 0xd5, 0xae,
  0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdc,
  0xb2, 0xb4, 0xa6, 0xdd, 0xb3, 0xb6, 0xb9
};
#define ZTOH_SYMBOLS 9
static unsigned short zen_to_han_symbol_table_1[ZTOH_SYMBOLS] = {
  0x8142, 0x8175, 0x8176, 0x8141, 0x8145, 0x815b, 0x814a, 0x814b
};
static unsigned char zen_to_han_symbol_table_2[ZTOH_SYMBOLS] = {
  0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xb0, 0xde, 0xdf
};
#define ISKANA(c) ((c) >= 0xa1 && (c) <= 0xdf)
#define JISHIRA(c) ((c) >= 0x829f && (c) <= 0x82f1)
#define JISKANA(c) ((c) >= 0x8340 && (c) <= 0x8396 && (c) != 0x837f)
#define JTOKANA(c) ((c) <= 0x82dd ? (c) + 0xa1 : (c) + 0xa2)

char _jctype[257] = {
/*-1*/  ___,
/*0x*/  ___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,
/*1x*/  ___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,
/*2x*/  ___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,
/*3x*/  ___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,
/*4x*/  __2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,
/*5x*/  __2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,
/*6x*/  __2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,
/*7x*/  __2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,__2,___,
/*8x*/  __2,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,
/*9x*/  _12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,
/*Ax*/  __2,_P2,_P2,_P2,_P2,_P2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,
/*Bx*/  _M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,
/*Cx*/  _M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,
/*Dx*/  _M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,_M2,
/*Ex*/  _12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,
/*Fx*/  _12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,_12,___,___,___
};


// syserr / sysnerr Defs
char __syserr00[] = "No Error";
char __syserr01[] = "Input to function out of range (EDOM)";
char __syserr02[] = "Output of function out of range (ERANGE)";
char __syserr03[] = "Argument list too long (E2BIG)";
char __syserr04[] = "Permission denied (EACCES)";
char __syserr05[] = "Resource temporarily unavailable (EAGAIN)";
char __syserr06[] = "Bad file descriptor (EBADF)";
char __syserr07[] = "Resource busy (EBUSY)";
char __syserr08[] = "No child processes (ECHILD)";
char __syserr09[] = "Resource deadlock avoided (EDEADLK)";
char __syserr10[] = "File exists (EEXIST)";
char __syserr11[] = "Bad address (EFAULT)";
char __syserr12[] = "File too large (EFBIG)";
char __syserr13[] = "Interrupted system call (EINTR)";
char __syserr14[] = "Invalid argument (EINVAL)";
char __syserr15[] = "Input or output error (EIO)";
char __syserr16[] = "Is a directory (EISDIR)";
char __syserr17[] = "Too many open files (EMFILE)";
char __syserr18[] = "Too many links (EMLINK)";
char __syserr19[] = "File name too long (ENAMETOOLONG)";
char __syserr20[] = "Too many open files in system (ENFILE)";
char __syserr21[] = "No such device (ENODEV)";
char __syserr22[] = "No such file or directory (ENOENT)";
char __syserr23[] = "Unable to execute file (ENOEXEC)";
char __syserr24[] = "No locks available (ENOLCK)";
char __syserr25[] = "Not enough memory (ENOMEM)";
char __syserr26[] = "No space left on drive (ENOSPC)";
char __syserr27[] = "Function not implemented (ENOSYS)";
char __syserr28[] = "Not a directory (ENOTDIR)";
char __syserr29[] = "Directory not empty (ENOTEMPTY)";
char __syserr30[] = "Inappropriate I/O control operation (ENOTTY)";
char __syserr31[] = "No such device or address (ENXIO)";
char __syserr32[] = "Operation not permitted (EPERM)";
char __syserr33[] = "Broken pipe (EPIPE)";
char __syserr34[] = "Read-only file system (EROFS)";
char __syserr35[] = "Invalid seek (ESPIPE)";
char __syserr36[] = "No such process (ESRCH)";
char __syserr37[] = "Improper link (EXDEV)";
char __syserr38[] = "No more files (ENMFILE)";

const char *CRTDLL_sys_errlist[] = {
__syserr00, __syserr01, __syserr02, __syserr03, __syserr04,
__syserr05, __syserr06, __syserr07, __syserr08, __syserr09,
__syserr10, __syserr11, __syserr12, __syserr13, __syserr14,
__syserr15, __syserr16, __syserr17, __syserr18, __syserr19,
__syserr20, __syserr21, __syserr22, __syserr23, __syserr24,
__syserr25, __syserr26, __syserr27, __syserr28, __syserr29,
__syserr30, __syserr31, __syserr32, __syserr33, __syserr34,
__syserr35, __syserr36, __syserr37, __syserr38
};

int __sys_nerr = sizeof(CRTDLL_sys_errlist) / sizeof(CRTDLL_sys_errlist[0]);

int*	CRTDLL_sys_nerr_dll = &__sys_nerr;
