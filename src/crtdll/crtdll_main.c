/* $Id: crtdll_main.c,v 1.2 2001-09-05 12:14:24 bird Exp $ */
/*
 * The C RunTime DLL
 *
 * Implements C run-time functionality as known from UNIX.
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997,2000 Uwe Bonnes
 * Copyright 2000 Jon Griffiths
 */

/*
Unresolved issues Uwe Bonnes 970904:
- tested with ftp://ftp.remcomp.com/pub/remcomp/lcc-win32.zip, a C-Compiler
        for Win32, based on lcc, from Jacob Navia
UB 000416:
- probably not thread safe
*/

/* NOTE: This file also implements the wcs* functions. They _ARE_ in
 * the newer Linux libcs, but use 4 byte wide characters, so are unusable,
 * since we need 2 byte wide characters. - Marcus Meissner, 981031
 */

#include <string.h>
#include <errno.h>

#include <float.h>
#include <libc\locale.h>
#include <setjmp.h>
#include "signal.h"


#include "crtdll.h"
#include <ctype.h>
#define __USE_ISOC9X 1 /* for isfinite */
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include "ntddk.h"
#include "wingdi.h"
#include "winuser.h"


DEFAULT_DEBUG_CHANNEL(crtdll);


UINT CRTDLL_argc_dll;         /* CRTDLL.23 */
LPSTR *CRTDLL_argv_dll;       /* CRTDLL.24 */
LPSTR  CRTDLL_acmdln_dll;     /* CRTDLL.38 */
UINT CRTDLL_basemajor_dll;    /* CRTDLL.42 */
UINT CRTDLL_baseminor_dll;    /* CRTDLL.43 */
UINT CRTDLL_baseversion_dll;  /* CRTDLL.44 */
UINT CRTDLL_commode_dll;      /* CRTDLL.59 */
LPSTR  CRTDLL_environ_dll;    /* CRTDLL.75 */
UINT CRTDLL_fmode_dll;        /* CRTDLL.104 */
UINT CRTDLL_osmajor_dll;      /* CRTDLL.241 */
UINT CRTDLL_osminor_dll;      /* CRTDLL.242 */
UINT CRTDLL_osmode_dll;       /* CRTDLL.243 */
UINT CRTDLL_osver_dll;        /* CRTDLL.244 */
UINT CRTDLL_osversion_dll;    /* CRTDLL.245 */
UINT CRTDLL_winmajor_dll;     /* CRTDLL.329 */
UINT CRTDLL_winminor_dll;     /* CRTDLL.330 */
UINT CRTDLL_winver_dll;       /* CRTDLL.331 */
INT  CRTDLL_doserrno = 0;
INT  CRTDLL_errno = 0;
const INT  CRTDLL__sys_nerr = 43;




#define FS_OS2   unsigned short sel = RestoreOS2FS();
#define FS_WIN32 SetFS(sel);


/*********************************************************************
 *                  CRTDLL_MainInit  (CRTDLL.init)
 */

BOOL WINAPI CRTDLL_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  FS_OS2

    // call i/o initializer in file.c
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
      __CRTDLL__init_io();
    }

  /*
    PH 2000/11/21 this code doesn't look very useful

    if (fdwReason == DLL_PROCESS_ATTACH) {
      _fdopen(0,"r");
      _fdopen(1,"w");
      _fdopen(2,"w");
      CRTDLL_hHeap = HeapCreate(0, 0x10000, 0);
    }
    else
      if (fdwReason == DLL_PROCESS_DETACH) {
        HeapDestroy(CRTDLL_hHeap);
        CRTDLL_hHeap = 0;
      }
  */

  FS_WIN32
  return TRUE;
}


/* INTERNAL: Set the crt and dos errno's from the OS error given. */
void __CRTDLL__set_errno(ULONG err)
{
  /* FIXME: not MT safe */
  CRTDLL_doserrno = err;

  switch(err)
  {
#define ERR_CASE(oserr) case oserr:
#define ERR_MAPS(oserr,crterr) case oserr:CRTDLL_errno = crterr;break;
    ERR_CASE(ERROR_ACCESS_DENIED)
    ERR_CASE(ERROR_NETWORK_ACCESS_DENIED)
    ERR_CASE(ERROR_CANNOT_MAKE)
    ERR_CASE(ERROR_SEEK_ON_DEVICE)
    ERR_CASE(ERROR_LOCK_FAILED)
    ERR_CASE(ERROR_FAIL_I24)
    ERR_CASE(ERROR_CURRENT_DIRECTORY)
    ERR_CASE(ERROR_DRIVE_LOCKED)
    ERR_CASE(ERROR_NOT_LOCKED)
    ERR_CASE(ERROR_INVALID_ACCESS)
    ERR_MAPS(ERROR_LOCK_VIOLATION,       EACCES);
    ERR_CASE(ERROR_FILE_NOT_FOUND)
    ERR_CASE(ERROR_NO_MORE_FILES)
    ERR_CASE(ERROR_BAD_PATHNAME)
    ERR_CASE(ERROR_BAD_NETPATH)
    ERR_CASE(ERROR_INVALID_DRIVE)
    ERR_CASE(ERROR_BAD_NET_NAME)
    ERR_CASE(ERROR_FILENAME_EXCED_RANGE)
    ERR_MAPS(ERROR_PATH_NOT_FOUND,       ENOENT);
/* @@@PH     ERR_MAPS(ERROR_IO_DEVICE,            EIO); */
    ERR_MAPS(ERROR_BAD_FORMAT,           ENOEXEC);
    ERR_MAPS(ERROR_INVALID_HANDLE,       EBADF);
    ERR_CASE(ERROR_OUTOFMEMORY)
    ERR_CASE(ERROR_INVALID_BLOCK)
    ERR_CASE(ERROR_NOT_ENOUGH_QUOTA);
    ERR_MAPS(ERROR_ARENA_TRASHED,        ENOMEM);
/* @@@PH    ERR_MAPS(ERROR_BUSY,                 EBUSY); */
    ERR_CASE(ERROR_ALREADY_EXISTS)
    ERR_MAPS(ERROR_FILE_EXISTS,          EEXIST);
/* @@@PH    ERR_MAPS(ERROR_BAD_DEVICE,           ENODEV); */
    ERR_MAPS(ERROR_TOO_MANY_OPEN_FILES,  EMFILE);
    ERR_MAPS(ERROR_DISK_FULL,            ENOSPC);
/* @@@PH   ERR_MAPS(ERROR_BROKEN_PIPE,          EPIPE); */
/* @@@PH   ERR_MAPS(ERROR_POSSIBLE_DEADLOCK,    EDEADLK); */
/* @@@PH   ERR_MAPS(ERROR_DIR_NOT_EMPTY,        ENOTEMPTY); */
    ERR_MAPS(ERROR_BAD_ENVIRONMENT,      E2BIG);
    ERR_CASE(ERROR_WAIT_NO_CHILDREN)
    ERR_MAPS(ERROR_CHILD_NOT_COMPLETE,   ECHILD);
    ERR_CASE(ERROR_NO_PROC_SLOTS)
    ERR_CASE(ERROR_MAX_THRDS_REACHED)
    ERR_MAPS(ERROR_NESTING_NOT_ALLOWED,  EAGAIN);
  default:
    /*  Remaining cases map to EINVAL */
    /* FIXME: may be missing some errors above */
    CRTDLL_errno = EINVAL;
  }
}


/*********************************************************************
 *                  _GetMainArgs  (CRTDLL.022)
 */
LPSTR * CDECL CRTDLL__GetMainArgs(LPDWORD argc,LPSTR **argv,
                                LPSTR *environ,DWORD flag)
{
        char *cmdline;
        char  **xargv;
    int xargc,end,last_arg,afterlastspace;
    DWORD   version;

    TRACE("(%p,%p,%p,%ld).\n",
        argc,argv,environ,flag
    );

    if (CRTDLL_acmdln_dll != NULL)
        HeapFree(GetProcessHeap(), 0, CRTDLL_acmdln_dll);

    CRTDLL_acmdln_dll = cmdline = CRTDLL__strdup( GetCommandLineA() );
    TRACE("got '%s'\n", cmdline);

    version = GetVersion();
    CRTDLL_osver_dll       = version >> 16;
    CRTDLL_winminor_dll    = version & 0xFF;
    CRTDLL_winmajor_dll    = (version>>8) & 0xFF;
    CRTDLL_baseversion_dll = version >> 16;
    CRTDLL_winver_dll      = ((version >> 8) & 0xFF) + ((version & 0xFF) << 8);
    CRTDLL_baseminor_dll   = (version >> 16) & 0xFF;
    CRTDLL_basemajor_dll   = (version >> 24) & 0xFF;
    CRTDLL_osversion_dll   = version & 0xFFFF;
    CRTDLL_osminor_dll     = version & 0xFF;
    CRTDLL_osmajor_dll     = (version>>8) & 0xFF;

    /* missing threading init */

    end=0;last_arg=0;xargv=NULL;xargc=0;afterlastspace=0;
    while (1)
    {
        if ((cmdline[end]==' ') || (cmdline[end]=='\0'))
        {
        if (cmdline[end]=='\0')
            last_arg=1;
        else
            cmdline[end]='\0';
        /* alloc xargc + NULL entry */
            xargv=(char**)HeapReAlloc( GetProcessHeap(), 0, xargv,
                                     sizeof(char*)*(xargc+1));
        if (strlen(cmdline+afterlastspace))
        {
            xargv[xargc] = CRTDLL__strdup(cmdline+afterlastspace);
            xargc++;
                    if (!last_arg) /* need to seek to the next arg ? */
            {
            end++;
            while (cmdline[end]==' ')
                end++;
    }
            afterlastspace=end;
        }
        else
        {
            xargv[xargc] = NULL; /* the last entry is NULL */
            break;
        }
        }
        else
        end++;
    }
    CRTDLL_argc_dll = xargc;
    *argc       = xargc;
    CRTDLL_argv_dll = xargv;
    *argv       = xargv;

    TRACE("found %d arguments\n",
        CRTDLL_argc_dll);
    CRTDLL_environ_dll = *environ = GetEnvironmentStringsA();
    return environ;
}


/*********************************************************************
 *                  _initterm     (CRTDLL.135)
 */
DWORD CDECL CRTDLL__initterm(_INITTERMFUN *start,_INITTERMFUN *end)
{
    _INITTERMFUN    *current;

    TRACE("(%p,%p)\n",start,end);
    current=start;
    while (current<end) {
        if (*current) (*current)();
        current++;
    }
    return 0;
}


/*******************************************************************
 *         _global_unwind2  (CRTDLL.129)
 */
void CDECL CRTDLL__global_unwind2( PEXCEPTION_FRAME frame )
{
    RtlUnwind( frame, 0, NULL, 0 );
}


/*******************************************************************
 *         _local_unwind2  (CRTDLL.173)
 */
void CDECL CRTDLL__local_unwind2( PEXCEPTION_FRAME endframe, DWORD nr )
{
    dprintf2(("CRTDLL: _local_unwind2\n"));
    TRACE("(%p,%ld)\n",endframe,nr);
}


/*********************************************************************
 *                  _beep          (CRTDLL.045)
 *
 * Output a tone using the PC speaker.
 *
 * PARAMS
 * freq [in]     Frequency of the tone
 *
 * duration [in] Length of time the tone should sound
 *
 * RETURNS
 * None.
 */
void CDECL CRTDLL__beep( UINT freq, UINT duration)
{
    TRACE(":Freq %d, Duration %d\n",freq,duration);
    Beep(freq, duration);
}


/*********************************************************************
 *                  rand          (CRTDLL.446)
 */
INT CDECL CRTDLL_rand()
{
    return (rand() & CRTDLL_RAND_MAX);
}


/*********************************************************************
 *                  _rotl          (CRTDLL.259)
 */
UINT CDECL CRTDLL__rotl(UINT x,INT shift)
{
  // return (_rotl(value, shift));

    shift &= 31;
    return (x << shift) | (x >> (32-shift));
}


/*********************************************************************
 *                  _lrotl          (CRTDLL.175)
 */
DWORD CDECL CRTDLL__lrotl(DWORD x,INT shift)
{
  // return (_lrotl(value, shift));

    shift &= 31;
    return (x << shift) | (x >> (32-shift));
}


/*********************************************************************
 *                  _lrotr          (CRTDLL.176)
 */
DWORD CDECL CRTDLL__lrotr(DWORD x,INT shift)
{
  // return (_lrotr(value, shift));

    shift &= 0x1f;
    return (x >> shift) | (x << (32-shift));
}


/*********************************************************************
 *                  _rotr          (CRTDLL.258)
 */
DWORD CDECL CRTDLL__rotr(UINT x,INT shift)
{
  // return (_rotr(value, shift));

    shift &= 0x1f;
    return (x >> shift) | (x << (32-shift));
}


/*********************************************************************
 *                  vswprintf      (CRTDLL.501)
 */
INT CDECL CRTDLL_vswprintf( LPWSTR buffer, LPCWSTR spec, va_list args )
{
  // return (vswprintf(s, t, format, arg));

    return wvsprintfW( buffer, spec, args );
}


/*********************************************************************
 *                  longjmp        (CRTDLL.426)
 */
VOID CDECL CRTDLL_longjmp(jmp_buf env, int val)
{
    FIXME("CRTDLL_longjmp semistub, expect crash\n");
    longjmp(env, val);
}


/*********************************************************************
 *                  setlocale           (CRTDLL.453)
 */
LPSTR CDECL CRTDLL_setlocale(INT category,LPCSTR locale)
{
  return (setlocale(category, locale));

  /*
    LPSTR categorystr;

    switch (category) {
    case CRTDLL_LC_ALL: categorystr="LC_ALL";break;
    case CRTDLL_LC_COLLATE: categorystr="LC_COLLATE";break;
    case CRTDLL_LC_CTYPE: categorystr="LC_CTYPE";break;
    case CRTDLL_LC_MONETARY: categorystr="LC_MONETARY";break;
    case CRTDLL_LC_NUMERIC: categorystr="LC_NUMERIC";break;
    case CRTDLL_LC_TIME: categorystr="LC_TIME";break;
    default: categorystr = "UNKNOWN?";break;
    }
    FIXME("(%s,%s),stub!\n",categorystr,locale);
    return "C";
  */
}


/*********************************************************************
 *                  _isctype           (CRTDLL.138)
 */
BOOL CDECL CRTDLL__isctype(CHAR x,CHAR type)
{
    if ((type & CRTDLL_SPACE) && isspace(x))
        return TRUE;
    if ((type & CRTDLL_PUNCT) && ispunct(x))
        return TRUE;
    if ((type & CRTDLL_LOWER) && islower(x))
        return TRUE;
    if ((type & CRTDLL_UPPER) && isupper(x))
        return TRUE;
    if ((type & CRTDLL_ALPHA) && isalpha(x))
        return TRUE;
    if ((type & CRTDLL_DIGIT) && isdigit(x))
        return TRUE;
    if ((type & CRTDLL_CONTROL) && iscntrl(x))
        return TRUE;
    /* check CRTDLL_LEADBYTE */
    return FALSE;
}


/*********************************************************************
 *                  _fullpath           (CRTDLL.114)
 */
LPSTR CDECL CRTDLL__fullpath(LPSTR buf, LPCSTR name, INT size)
{
  // return (_fullpath(buf, path, size));

  if (!buf)
  {
      size = 256;
      if(!(buf = CRTDLL_malloc(size))) return NULL;
  }
  if (!GetFullPathNameA( name, size, buf, NULL )) return NULL;
  TRACE("CRTDLL_fullpath got %s\n",buf);
  return buf;
}


/*********************************************************************
 *                  _splitpath           (CRTDLL.279)
 */
VOID CDECL CRTDLL__splitpath(LPCSTR path, LPSTR drive, LPSTR directory, LPSTR filename, LPSTR extension )
{
  // _splitpath( path, drive, dir, fname, ext);

  /* drive includes :
     directory includes leading and trailing (forward and backward slashes)
     filename without dot and slashes
     extension with leading dot
     */
  char * drivechar,*dirchar,*namechar;

  TRACE("CRTDLL__splitpath got %s\n",path);

  drivechar  = strchr(path,':');
  dirchar    = strrchr(path,'/');
  namechar   = strrchr(path,'\\');
  dirchar = max(dirchar,namechar);
  if (dirchar)
    namechar   = strrchr(dirchar,'.');
  else
    namechar   = strrchr(path,'.');

  if (drive)
    {
      *drive = 0x00;
      if (drivechar)
      {
          strncat(drive,path,drivechar-path+1);
          path = drivechar+1;
      }
    }
  if (directory)
    {
      *directory = 0x00;
      if (dirchar)
      {
          strncat(directory,path,dirchar-path+1);
          path = dirchar+1;
      }
    }
  if (filename)
    {
      *filename = 0x00;
      if (namechar)
      {
          strncat(filename,path,namechar-path);
          if (extension)
          {
              *extension = 0x00;
              strcat(extension,namechar);
          }
      }
    }

  TRACE("CRTDLL__splitpath found %s %s %s %s\n",drive,directory,filename,extension);
}


/*********************************************************************
 *                  _makepath           (CRTDLL.182)
 */

VOID CDECL CRTDLL__makepath(LPSTR path, LPCSTR drive,
                              LPCSTR directory, LPCSTR filename,
                              LPCSTR extension )
{
  // _makepath(path, drive, dir, fname, ext);

    char ch;
    TRACE("CRTDLL__makepath got %s %s %s %s\n", drive, directory,
          filename, extension);

    if ( !path )
        return;

    path[0] = 0;
    if (drive && drive[0])
    {
        path[0] = drive[0];
        path[1] = ':';
        path[2] = 0;
    }
    if (directory && directory[0])
    {
        strcat(path, directory);
        ch = path[strlen(path)-1];
        if (ch != '/' && ch != '\\')
            strcat(path,"\\");
    }
    if (filename && filename[0])
    {
        strcat(path, filename);
        if (extension && extension[0])
        {
            if ( extension[0] != '.' ) {
                strcat(path,".");
            }
            strcat(path,extension);
        }
    }

    TRACE("CRTDLL__makepath returns %s\n",path);
}


/*********************************************************************
 *                  _errno           (CRTDLL.52)
 * Return the address of the CRT errno (Not the libc errno).
 * @@@PH Note: veeeery strange ...
 * BUGS
 * Not MT safe.
 */
LPINT CDECL CRTDLL__errno( VOID )
{
  // return (_errno());

  return &CRTDLL_errno;
}


/*********************************************************************
 *                  __doserrno       (CRTDLL.26)
 *
 * Return the address of the DOS errno (holding the last OS error).
 * @@@PH Note: veeeery strange ...
 * BUGS
 * Not MT safe.
 */
LPINT CDECL CRTDLL___doserrno( VOID )
{
  // return (__doserrno());

  return &CRTDLL_doserrno;
}


/**********************************************************************
 *                  _strerror       (CRTDLL.284)
 *
 * Return a formatted system error message.
 *
 * NOTES
 * The caller does not own the string returned.
 */
extern int sprintf(char *str, const char *format, ...);

LPSTR CDECL CRTDLL__strerror (LPCSTR err)
{
  // return (_strerror((char*)s));

  static char strerrbuff[256];
  sprintf(strerrbuff,"%s: %s\n",err,CRTDLL_strerror(CRTDLL_errno));
  return strerrbuff;
}


/*********************************************************************
 *                  perror       (CRTDLL.435)
 *
 * Print a formatted system error message to stderr.
 */
VOID CDECL CRTDLL_perror (LPCSTR err)
{
  // perror( s );

  char *err_str = CRTDLL_strerror(CRTDLL_errno);
  CRTDLL_fprintf(CRTDLL_stderr,"%s: %s\n",err,err_str);
  CRTDLL_free(err_str);
}


/*********************************************************************
 *                  strerror       (CRTDLL.465)
 *
 * Return the text of an error.
 *
 * NOTES
 * The caller does not own the string returned.
 */
extern char *strerror(int errnum);

LPSTR CDECL CRTDLL_strerror (INT err)
{
  return strerror(err);
}


/*********************************************************************
 *                  signal           (CRTDLL.455)
 */
LPVOID CDECL CRTDLL_signal(INT sig, sig_handler_type ptr)
{
  //return (signal(sig, ptr));

  FIXME("(%d %p):stub.\n", sig, ptr);
  return (void*)-1;
}


/*********************************************************************
 *                  _sleep           (CRTDLL.267)
 */
VOID CDECL CRTDLL__sleep(ULONG timeout)
{
  TRACE("CRTDLL__sleep for %ld milliseconds\n",timeout);
  Sleep((timeout)?timeout:1);
}


/*********************************************************************
 *                  getenv           (CRTDLL.437)
 */
LPSTR CDECL CRTDLL_getenv(LPCSTR name)
{
  // return (getenv(name));

     LPSTR environ = GetEnvironmentStringsA();
     LPSTR pp,pos = NULL;
     unsigned int length;

     for (pp = environ; (*pp); pp = pp + strlen(pp) +1)
       {
     pos =strchr(pp,'=');
     if (pos)
       length = pos -pp;
     else
       length = strlen(pp);
     if (!strncmp(pp,name,length)) break;
       }
     if ((pp)&& (pos))
       {
     pp = pos+1;
     TRACE("got %s\n",pp);
       }
     FreeEnvironmentStringsA( environ );
     return pp;
}


/*********************************************************************
 *                  _except_handler2  (CRTDLL.78)
 */
INT CDECL CRTDLL__except_handler2 (
    PEXCEPTION_RECORD rec,
    PEXCEPTION_FRAME frame,
    PCONTEXT context,
    PEXCEPTION_FRAME  *dispatcher)
{
    FIXME ("exception %lx flags=%lx at %p handler=%p %p %p stub\n",
    rec->ExceptionCode, rec->ExceptionFlags, rec->ExceptionAddress,
    frame->Handler, context, dispatcher);
    return ExceptionContinueSearch;
}


/*********************************************************************
 *                  __isascii           (CRTDLL.028)
 *
 */
INT CDECL CRTDLL___isascii(INT c)
{
  return isascii(c);
}


/*********************************************************************
 *                  __toascii           (CRTDLL.035)
 *
 */
INT CDECL CRTDLL___toascii(INT c)
{
  // return (_toascii(c));

  return c & 0x7f;
}


/*********************************************************************
 *                  iswascii           (CRTDLL.404)
 *
 */
INT CDECL CRTDLL_iswascii(LONG c)
{
  // return  (!((c)&(~0x7f)));
  return ((unsigned)c < 0x80);
}


/*********************************************************************
 *                  __iscsym           (CRTDLL.029)
 *
 * Is a character valid in a C identifier (a-Z,0-9,_).
 *
 * PARAMS
 *   c       [I]: Character to check
 *
 * RETURNS
 * Non zero if c is valid as t a C identifier.
 */
INT CDECL CRTDLL___iscsym(LONG c)
{
  // return (_iscsym(c));

  return (isalnum(c) || c == '_');
}


/*********************************************************************
 *                  __iscsymf           (CRTDLL.030)
 *
 * Is a character valid as the first letter in a C identifier (a-Z,_).
 *
 * PARAMS
 *   c [in]  Character to check
 *
 * RETURNS
 *   Non zero if c is valid as the first letter in a C identifier.
 */
INT CDECL CRTDLL___iscsymf(LONG c)
{
  // return (_iscsymf(c));

  return (isalpha(c) || c == '_');
}


/*********************************************************************
 *                  _loaddll        (CRTDLL.171)
 *
 * Get a handle to a DLL in memory. The DLL is loaded if it is not already.
 *
 * PARAMS
 * dll [in]  Name of DLL to load.
 *
 * RETURNS
 * Success: A handle to the loaded DLL.
 *
 * Failure: FIXME.
 */
INT CDECL CRTDLL__loaddll(LPSTR dllname)
{
  return LoadLibraryA(dllname);
}


/*********************************************************************
 *                  _unloaddll        (CRTDLL.313)
 *
 * Free reference to a DLL handle from loaddll().
 *
 * PARAMS
 *   dll [in] Handle to free.
 *
 * RETURNS
 * Success: 0.
 *
 * Failure: Error number.
 */
INT CDECL CRTDLL__unloaddll(HANDLE dll)
{
  INT err;
  if (FreeLibrary(dll))
    return 0;
  err = GetLastError();
  __CRTDLL__set_errno(err);
  return err;
}


/*********************************************************************
 *                  _lsearch        (CRTDLL.177)
 *
 * Linear search of an array of elements. Adds the item to the array if
 * not found.
 *
 * PARAMS
 *   match [in]      Pointer to element to match
 *   start [in]      Pointer to start of search memory
 *   array_size [in] Length of search array (element count)
 *   elem_size [in]  Size of each element in memory
 *   comp_func [in]  Pointer to comparason function (like qsort()).
 *
 * RETURNS
 *   Pointer to the location where element was found or added.
 */
LPVOID CDECL CRTDLL__lsearch(LPVOID match,LPVOID start, LPUINT array_size,
                               UINT elem_size, comp_func cf)
{
  LPBYTE pStart = (LPBYTE)start;
  UINT size = *array_size;
  if (size)
    do
    {
      if (cf((LPVOID*)match, (LPVOID*)pStart) == 0)
    return pStart; /* found */
      pStart += elem_size;
    } while (--size);

  /* not found, add to end */
  memcpy(pStart, match, elem_size);
  array_size[0]++;
  return pStart;
}


/*********************************************************************
 *                  _itow           (CRTDLL.164)
 *
 * Convert an integer to a wide char string.
 */
/* @@@PH extern LPSTR  CDECL _itoa( long , LPSTR , INT);*/ /* ntdll */

WCHAR* CDECL CRTDLL__itow(INT value,WCHAR* out,INT base)
{
  char buff[64]; /* FIXME: Whats the maximum buffer size for INT_MAX? */

  _itoa(value, buff, base);
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buff, -1, out, 64);
  return out;
}


/*********************************************************************
 *                  _ltow           (CRTDLL.??)
 *
 * Convert a long to a wide char string.
 */
/* @@@PH extern LPSTR  CDECL _ltoa( long , LPSTR , INT); */ /* ntdll */

WCHAR* CDECL CRTDLL__ltow(LONG value,WCHAR* out,INT base)
{
  char buff[64]; /* FIXME: Whats the maximum buffer size for LONG_MAX? */

  _ltoa(value, buff, base);
  MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, buff, -1, out, 64);
  return out;
}


/*********************************************************************
 *                  _ultow           (CRTDLL.??)
 *
 * Convert an unsigned long to a wide char string.
 */
/* @@@PH extern LPSTR  CDECL _ultoa( long , LPSTR , INT); */ /* ntdll */

WCHAR* CDECL CRTDLL__ultow(ULONG value,WCHAR* out,INT base)
{
  char buff[64]; /* FIXME: Whats the maximum buffer size for ULONG_MAX? */

  _ultoa(value, buff, base);
  MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, buff, -1, out, 64);
  return out;
}


/*********************************************************************
 *                  _toupper           (CRTDLL.489)
 */
CHAR CDECL CRTDLL__toupper(CHAR c)
{
  // return (_toupper(n));

  return toupper(c);
}


/*********************************************************************
 *                  _tolower           (CRTDLL.490)
 */
CHAR CDECL CRTDLL__tolower(CHAR c)
{
  // return (_tolower(n));

  return tolower(c);
}


/* FP functions */

/*********************************************************************
 *                  _cabs           (CRTDLL.048)
 *
 * Return the absolue value of a complex number.
 *
 * PARAMS
 *   c [in] Structure containing real and imaginary parts of complex number.
 *
 * RETURNS
 *   Absolute value of complex number (always a positive real number).
 */
double CDECL CRTDLL__cabs(struct __CRTDLL_complex c)
{
  // return (_cabs(c));

  return sqrt(c.real * c.real + c.imaginary * c.imaginary);
}


/*********************************************************************
 *                  _chgsign    (CRTDLL.053)
 *
 * Change the sign of an IEEE double.
 *
 * PARAMS
 *   d [in] Number to invert.
 *
 * RETURNS
 *   Number with sign inverted.
 */
double CDECL CRTDLL__chgsign(double d)
{
  /* FIXME: +-infinity,Nan not tested */
  return -d;
}


/*********************************************************************
 *                  _copysign           (CRTDLL.062)
 *
 * Return the number x with the sign of y.
 */
double CDECL CRTDLL__copysign(double x, double y)
{
  /* FIXME: Behaviour for Nan/Inf etc? */
  if (y < 0.0)
    return x < 0.0 ? x : -x;

  return x < 0.0 ? -x : x;
}


/*********************************************************************
 *                  _finite           (CRTDLL.101)
 *
 * Determine if an IEEE double is finite (i.e. not +/- Infinity).
 *
 * PARAMS
 *   d [in]  Number to check.
 *
 * RETURNS
 *   Non zero if number is finite.
 */
INT CDECL  CRTDLL__finite(double d)
{
  return 0;
  //@@@PH return (isfinite(d)?1:0); /* See comment for CRTDLL__isnan() */
}


/*********************************************************************
 *                  _fpreset           (CRTDLL.107)
 *
 * Reset the state of the floating point processor.
 *
 * PARAMS
 *   None.
 *
 * RETURNS
 *   None.
 *
 * BUGS
 * Unimplemented.
 */
VOID CDECL CRTDLL__fpreset(void)
{
  //FIXME(":stub!\n");
  _fpreset();
}


/*********************************************************************
 *                  _isnan           (CRTDLL.164)
 *
 * Determine if an IEEE double is unrepresentable (NaN).
 *
 * PARAMS
 *   d [in]  Number to check.
 *
 * RETURNS
 *   Non zero if number is NaN.
 */
INT CDECL  CRTDLL__isnan(double d)
{
  /* some implementations return -1 for true(glibc), crtdll returns 1.
   * Do the same, as the result may be used in calculations.
   */
   return 0;
  //@@@PH return isnan(d)?1:0;
}
