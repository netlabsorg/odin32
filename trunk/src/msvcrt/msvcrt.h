/* $Id: msvcrt.h,v 1.1 1999-11-30 14:17:28 sandervl Exp $ */

/* Definitions for the MS Visual C-Runtime library (MSVCRT.DLL)
 *
 * Copyright 1999 Jens Wiessner
 */

typedef struct
{
    HANDLE handle;
    int      pad[7];
} CRTDLL_FILE, *PCRTDLL_FILE;

wchar_t ** __wargv;
wchar_t * _wpgmptr;
wchar_t ** _wenviron;

int __app_type;
int __usermatherr;

typedef VOID (*new_handler_type)(VOID);
static new_handler_type new_handler;

// var Defs
UINT 	MSVCRT___argc;      
LPSTR 	*MSVCRT___argv;     
int     MSVCRT___mb_cur_max;
LPSTR  	MSVCRT__acmdln;     
UINT 	MSVCRT__basemajor;  
UINT 	MSVCRT__baseminor;  
UINT 	MSVCRT__baseversion;
UINT 	MSVCRT__commode;    
UINT 	MSVCRT__daylight;   
LPSTR	MSVCRT__environ;    
LPSTR	MSVCRT__fileinfo;   
UINT 	MSVCRT__fmode;      
double  *MSVCRT__HUGE;      
UINT 	MSVCRT__osmajor;    
UINT 	MSVCRT__osminor;    
UINT 	MSVCRT__osmode;     
UINT 	MSVCRT__osver;      
UINT 	MSVCRT__osversion;  
USHORT  MSVCRT__pctype[] = {0,0};
LPSTR  	MSVCRT__pgmptr;      
USHORT *MSVCRT__pwctype;     
UINT 	MSVCRT__timezone;    
LPSTR	MSVCRT__tzname;
UINT 	MSVCRT__winmajor;    
UINT 	MSVCRT__winminor;    
UINT 	MSVCRT__winver;      


// syserr / sysnerr Defs
const char *MSVCRT_sys_errlist[] = {
__syserr00, __syserr01, __syserr02, __syserr03, __syserr04,
__syserr05, __syserr06, __syserr07, __syserr08, __syserr09,
__syserr10, __syserr11, __syserr12, __syserr13, __syserr14,
__syserr15, __syserr16, __syserr17, __syserr18, __syserr19,
__syserr20, __syserr21, __syserr22, __syserr23, __syserr24,
__syserr25, __syserr26, __syserr27, __syserr28, __syserr29,
__syserr30, __syserr31, __syserr32, __syserr33, __syserr34,
__syserr35, __syserr36, __syserr37, __syserr38
};
int __sys_nerr = sizeof(MSVCRT_sys_errlist) / sizeof(MSVCRT_sys_errlist[0]);
int*	MSVCRT__sys_nerr = &__sys_nerr;

// extern Defs
extern CRTDLL_FILE _iob;