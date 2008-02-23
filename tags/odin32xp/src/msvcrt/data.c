/*
 * msvcrt.dll dll data items
 *
 * Copyright 2000 Jon Griffiths
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __WIN32OS2__
#include "config.h"
#else
#include <emxheader.h>
#include <math.h>
#include <string.h>
#endif

#include "wine/port.h"

#include "msvcrt.h"

#include "msvcrt/stdlib.h"
#include "msvcrt/string.h"

#ifndef __WIN32OS2__
#include "wine/library.h"
#endif
#include "wine/unicode.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

unsigned int MSVCRT___argc;
unsigned int MSVCRT_basemajor;/* FIXME: */
unsigned int MSVCRT_baseminor;/* FIXME: */
unsigned int MSVCRT_baseversion; /* FIXME: */
unsigned int MSVCRT__commode;
unsigned int MSVCRT__fmode;
unsigned int MSVCRT_osmajor;/* FIXME: */
unsigned int MSVCRT_osminor;/* FIXME: */
unsigned int MSVCRT_osmode;/* FIXME: */
unsigned int MSVCRT__osver;
unsigned int MSVCRT_osversion; /* FIXME: */
unsigned int MSVCRT__winmajor;
unsigned int MSVCRT__winminor;
unsigned int MSVCRT__winver;
unsigned int MSVCRT__sys_nerr; /* FIXME: not accessible from Winelib apps */
char**       MSVCRT__sys_errlist; /* FIXME: not accessible from Winelib apps */
unsigned int MSVCRT___setlc_active;
unsigned int MSVCRT___unguarded_readlc_active;
double MSVCRT__HUGE;
char **MSVCRT___argv;
MSVCRT_wchar_t **MSVCRT___wargv;
char *MSVCRT__acmdln;
MSVCRT_wchar_t *MSVCRT__wcmdln;
char **MSVCRT__environ = 0;
MSVCRT_wchar_t **MSVCRT__wenviron = 0;
char **MSVCRT___initenv = 0;
MSVCRT_wchar_t **MSVCRT___winitenv = 0;
int MSVCRT_timezone;
int MSVCRT_app_type;
char MSVCRT_pgm[MAX_PATH];
char* MSVCRT__pgmptr = 0;

/* Get a snapshot of the current environment
 * and construct the __p__environ array
 *
 * The pointer returned from GetEnvironmentStrings may get invalid when
 * some other module cause a reallocation of the env-variable block
 *
 * blk is an array of pointers to environment strings, ending with a NULL
 * and after that the actual copy of the environment strings, ending in a \0
 */
char ** msvcrt_SnapshotOfEnvironmentA(char **blk)
{
  char* environ_strings = GetEnvironmentStringsA();
  int count = 1, len = 1, i = 0; /* keep space for the trailing NULLS */
  char *ptr;

  for (ptr = environ_strings; *ptr; ptr += strlen(ptr) + 1)
  {
    count++;
    len += strlen(ptr) + 1;
  }
  if (blk)
      blk = HeapReAlloc( GetProcessHeap(), 0, blk, count* sizeof(char*) + len );
  else
    blk = HeapAlloc(GetProcessHeap(), 0, count* sizeof(char*) + len );

  if (blk)
    {
      if (count)
	{
	  memcpy(&blk[count],environ_strings,len);
	  for (ptr = (char*) &blk[count]; *ptr; ptr += strlen(ptr) + 1)
	    {
	      blk[i++] = ptr;
	    }
	}
      blk[i] = NULL;
    }
  FreeEnvironmentStringsA(environ_strings);
  return blk;
}

MSVCRT_wchar_t ** msvcrt_SnapshotOfEnvironmentW(MSVCRT_wchar_t **wblk)
{
  MSVCRT_wchar_t* wenviron_strings = GetEnvironmentStringsW();
  int count = 1, len = 1, i = 0; /* keep space for the trailing NULLS */
  MSVCRT_wchar_t *wptr;

  for (wptr = wenviron_strings; *wptr; wptr += strlenW(wptr) + 1)
  {
    count++;
    len += strlenW(wptr) + 1;
  }
  if (wblk)
      wblk = HeapReAlloc( GetProcessHeap(), 0, wblk, count* sizeof(MSVCRT_wchar_t*) + len * sizeof(MSVCRT_wchar_t));
  else
    wblk = HeapAlloc(GetProcessHeap(), 0, count* sizeof(MSVCRT_wchar_t*) + len * sizeof(MSVCRT_wchar_t));
  if (wblk)
    {
      if (count)
	{
	  memcpy(&wblk[count],wenviron_strings,len * sizeof(MSVCRT_wchar_t));
	  for (wptr = (MSVCRT_wchar_t*)&wblk[count]; *wptr; wptr += strlenW(wptr) + 1)
	    {
	      wblk[i++] = wptr;
	    }
	}
      wblk[i] = NULL;
    }
  FreeEnvironmentStringsW(wenviron_strings);
  return wblk;
}

typedef void (*_INITTERMFUN)(void);

/***********************************************************************
 *		__p___argc (MSVCRT.@)
 */
int* __p___argc(void) { dprintf(("MSVCRT: Query of p__argc")); return &MSVCRT___argc; }

/***********************************************************************
 *		__p__commode (MSVCRT.@)
 */
unsigned int* __p__commode(void) { dprintf(("MSVCRT: Query of p__commode"));  return &MSVCRT__commode; }

/***********************************************************************
 *              __p__pgmptr (MSVCRT.@)
 */
char** __p__pgmptr(void) { return &MSVCRT__pgmptr; }

/***********************************************************************
 *		__p__fmode (MSVCRT.@)
 */
unsigned int* __p__fmode(void) {  dprintf(("MSVCRT: Query of p__fmode"));  return &MSVCRT__fmode; }

/***********************************************************************
 *		__p__osver (MSVCRT.@)
 */
unsigned int* __p__osver(void) {  dprintf(("MSVCRT: Query of p__osver")); return &MSVCRT__osver; }

/***********************************************************************
 *		__p__winmajor (MSVCRT.@)
 */
unsigned int* __p__winmajor(void) {  dprintf(("MSVCRT: Query of p__winmajor")); return &MSVCRT__winmajor; }

/***********************************************************************
 *		__p__winminor (MSVCRT.@)
 */
unsigned int* __p__winminor(void) {  dprintf(("MSVCRT: Query of p__winminor")); return &MSVCRT__winminor; }

/***********************************************************************
 *		__p__winver (MSVCRT.@)
 */
unsigned int* __p__winver(void) {  dprintf(("MSVCRT: Query of p__winver")); return &MSVCRT__winver; }

/*********************************************************************
 *		__p__acmdln (MSVCRT.@)
 */
char** __p__acmdln(void) {  dprintf(("MSVCRT: Query of p__acmdln")); return &MSVCRT__acmdln; }

/*********************************************************************
 *		__p__wcmdln (MSVCRT.@)
 */
MSVCRT_wchar_t** __p__wcmdln(void) {  dprintf(("MSVCRT: Query of p__wcmdln")); return &MSVCRT__wcmdln; }

/*********************************************************************
 *		__p___argv (MSVCRT.@)
 */
char*** __p___argv(void) {  dprintf(("MSVCRT: Query of p__argv"));  return &MSVCRT___argv; }

/*********************************************************************
 *		__p___wargv (MSVCRT.@)
 */
MSVCRT_wchar_t*** __p___wargv(void) {  dprintf(("MSVCRT: Query of p__wargv"));  return &MSVCRT___wargv; }

/*********************************************************************
 *		__p__environ (MSVCRT.@)
 */
char*** __p__environ(void)
{
  dprintf(("MSVCRT: Query of p__environ")); 
  if (!MSVCRT__environ)
    MSVCRT__environ = msvcrt_SnapshotOfEnvironmentA(NULL);
  return &MSVCRT__environ;
}

/*********************************************************************
 *		__p__wenviron (MSVCRT.@)
 */
MSVCRT_wchar_t*** __p__wenviron(void)
{
  dprintf(("MSVCRT: Query of p__wenviron")); 
  if (!MSVCRT__wenviron)
    MSVCRT__wenviron = msvcrt_SnapshotOfEnvironmentW(NULL);
  return &MSVCRT__wenviron;
}

/*********************************************************************
 *		__p___initenv (MSVCRT.@)
 */
char*** __p___initenv(void) {  dprintf(("MSVCRT: Query of p__initenv")); return &MSVCRT___initenv; }

/*********************************************************************
 *		__p___winitenv (MSVCRT.@)
 */
MSVCRT_wchar_t*** __p___winitenv(void) {  dprintf(("MSVCRT: Query of p__winitenv")); return &MSVCRT___winitenv; }

/*********************************************************************
 *		__p__timezone (MSVCRT.@)
 */
int* __p__timezone(void) {  dprintf(("MSVCRT: Query of p__timezone")); return &MSVCRT_timezone; }

/* INTERNAL: Create a wide string from an ascii string */
static MSVCRT_wchar_t *wstrdupa(const char *str)
{
  const size_t len = strlen(str) + 1 ;
  dprintf(("MSVCRT: wstrdupa %s",str)); 
  MSVCRT_wchar_t *wstr = MSVCRT_malloc(len* sizeof (MSVCRT_wchar_t));
  if (!wstr)
    return NULL;
   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,str,len,wstr,len);
  return wstr;
}


#ifdef __WIN32OS2__

static int __wine_main_argc = 0;
static char **__wine_main_argv = NULL;
static WCHAR **__wine_main_wargv = NULL;
char *argv[255];

static void set_library_argv( char **incargv)
{
    int argc = 0;
    WCHAR *p;
    WCHAR **wargv;
    char *token2 = NULL;
    DWORD total = 0;

    char argvbuf[255];
    
    strcpy(argvbuf,incargv[0]);

    if (argvbuf[0] == '\"')   
     token2 = strtok(argvbuf, "\"");
    else
     token2 = strtok(argvbuf, " ");

    if (token2)
    do
    {
      argv[argc] = HeapAlloc( GetProcessHeap(), 0, strlen(token2)+1);
      strcpy(argv[argc++],token2);
    }
    while (token2 = strtok(NULL, " "));

    argv[argc] = NULL;
    for (argc = 0; argv[argc]; argc++)
        total += MultiByteToWideChar( CP_ACP, 0, argv[argc], -1, NULL, 0 );

    wargv = HeapAlloc( GetProcessHeap(), 0,
                       total * sizeof(WCHAR) + (argc + 1) * sizeof(*wargv) );
    p = (WCHAR *)(wargv + argc + 1);
    for (argc = 0; argv[argc]; argc++)
    {
        DWORD len = MultiByteToWideChar( CP_ACP, 0, argv[argc], -1, p, total );
        wargv[argc] = p;
        p += len;
        total -= len;
    }
    wargv[argc] = NULL;
   
    dprintf(("Num of args is %d",argc));    

    __wine_main_argc  = argc;
    __wine_main_argv  = argv;
    __wine_main_wargv = wargv;
}
#endif

/* INTERNAL: Since we can't rely on Winelib startup code calling w/getmainargs,
 * we initialise data values during DLL loading. When called by a native
 * program we simply return the data we've already initialised. This also means
 * you can call multiple times without leaking
 */
void msvcrt_init_args(void)
{
  DWORD version;

  MSVCRT__acmdln = MSVCRT__strdup( GetCommandLineA() );
  MSVCRT__wcmdln = wstrdupa(MSVCRT__acmdln);
#ifdef __WIN32OS2__
  set_library_argv(&MSVCRT__acmdln);
#endif
  MSVCRT___argc = __wine_main_argc;
  MSVCRT___argv = __wine_main_argv;
  MSVCRT___wargv = __wine_main_wargv;

  TRACE("got '%s', wide = %s argc=%d\n", MSVCRT__acmdln,
        debugstr_w(MSVCRT__wcmdln),MSVCRT___argc);

  version = GetVersion();
  MSVCRT__osver       = version >> 16;
  MSVCRT__winminor    = version & 0xFF;
  MSVCRT__winmajor    = (version>>8) & 0xFF;
  MSVCRT_baseversion = version >> 16;
  MSVCRT__winver     = ((version >> 8) & 0xFF) + ((version & 0xFF) << 8);
  MSVCRT_baseminor   = (version >> 16) & 0xFF;
  MSVCRT_basemajor   = (version >> 24) & 0xFF;
  MSVCRT_osversion   = version & 0xFFFF;
  MSVCRT_osminor     = version & 0xFF;
  MSVCRT_osmajor     = (version>>8) & 0xFF;
  MSVCRT__sys_nerr   = 43;
  MSVCRT__HUGE = HUGE_VAL;
  MSVCRT___setlc_active = 0;
  MSVCRT___unguarded_readlc_active = 0;
  MSVCRT_timezone = 0;

  MSVCRT___initenv= msvcrt_SnapshotOfEnvironmentA(NULL);
  MSVCRT___winitenv= msvcrt_SnapshotOfEnvironmentW(NULL);

  MSVCRT_pgm[0] = '\0';
  GetModuleFileNameA(0, MSVCRT_pgm, sizeof(MSVCRT_pgm)/sizeof(MSVCRT_pgm[0]));
  MSVCRT__pgmptr = MSVCRT_pgm;
}


/* INTERNAL: free memory used by args */
void msvcrt_free_args(void)
{
  /* FIXME: more things to free */
  HeapFree(GetProcessHeap(), 0,MSVCRT___initenv);
  HeapFree(GetProcessHeap(), 0,MSVCRT___winitenv);
  HeapFree(GetProcessHeap(), 0,MSVCRT__environ);
  HeapFree(GetProcessHeap(), 0,MSVCRT__wenviron);
}

/*********************************************************************
 *		__getmainargs (MSVCRT.@)
 */
void __getmainargs(int *argc, char** *argv, char** *envp,
                                  int expand_wildcards, int *new_mode)
{
  TRACE("(%p,%p,%p,%d,%p).\n", argc, argv, envp, expand_wildcards, new_mode);
  *argc = MSVCRT___argc;
  *argv = MSVCRT___argv;
  *envp = MSVCRT___initenv;
  if (new_mode)
    MSVCRT__set_new_mode( *new_mode );
}

/*********************************************************************
 *		__wgetmainargs (MSVCRT.@)
 */
void __wgetmainargs(int *argc, MSVCRT_wchar_t** *wargv, MSVCRT_wchar_t** *wenvp,
                    int expand_wildcards, int *new_mode)
{
  TRACE("(%p,%p,%p,%d,%p).\n", argc, wargv, wenvp, expand_wildcards, new_mode);
  *argc = MSVCRT___argc;
  *wargv = MSVCRT___wargv;
  *wenvp = MSVCRT___winitenv;
  if (new_mode)
    MSVCRT__set_new_mode( *new_mode );
}

/*********************************************************************
 *		_initterm (MSVCRT.@)
 */
unsigned int _initterm(_INITTERMFUN *start,_INITTERMFUN *end)
{
  _INITTERMFUN* current = start;

  TRACE("(%p,%p)\n",start,end);
  while (current<end)
  {
    if (*current)
    {
      TRACE("Call init function %p\n",*current);
      (**current)();
      TRACE("returned\n");
    }
    current++;
  }
  return 0;
}

/*********************************************************************
 *		__set_app_type (MSVCRT.@)
 */
void MSVCRT___set_app_type(int app_type)
{
  TRACE("(%d) %s application\n", app_type, app_type == 2 ? "Gui" : "Console");
  MSVCRT_app_type = app_type;
}
