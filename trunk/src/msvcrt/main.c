/*
 * msvcrt.dll initialisation functions
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
#include "msvcrt.h"
#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)
#include "msvcrt/locale.h"
#include "msvcrt/stdio.h"
#include <string.h>
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

/* Index to TLS */
DWORD MSVCRT_tls_index;

static inline BOOL msvcrt_init_tls(void);
static inline BOOL msvcrt_free_tls(void);
const char* msvcrt_get_reason(DWORD reason) WINE_UNUSED;

typedef void* (*MSVCRT_malloc_func)(unsigned int);
typedef void (*MSVCRT_free_func)(void*);

int _CRT_init (void);
void _CRT_term (void);
void __ctordtorInit (void);
void __ctordtorTerm (void);

static HMODULE dllHandle = 0;

BOOL WINAPI MSVCRT_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);


/*********************************************************************
 *                  Init
 */
BOOL WINAPI MSVCRT_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  MSVCRT_thread_data *tls;

  TRACE("(0x%08x, %s, %p) pid(%ld), tid(%ld), tls(%ld)\n",
        hinstDLL, msvcrt_get_reason(fdwReason), lpvReserved,
        (long)GetCurrentProcessId(), (long)GetCurrentThreadId(),
        (long)MSVCRT_tls_index);

  switch (fdwReason)
  {
  case DLL_PROCESS_ATTACH:
   if (!msvcrt_init_tls())
      return FALSE;
    msvcrt_init_mt_locks();
    msvcrt_init_vtables();
    msvcrt_init_io();
    msvcrt_init_console();
    msvcrt_init_args();
    MSVCRT_setlocale(0, "C");
    TRACE("finished process init\n");
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_PROCESS_DETACH:
    msvcrt_free_mt_locks();
    msvcrt_free_io();
    msvcrt_free_console();
    msvcrt_free_args();
    if (!msvcrt_free_tls())
      return FALSE;
    TRACE("finished process free\n");
    break;
  case DLL_THREAD_DETACH:
    /* Free TLS */
    tls = TlsGetValue(MSVCRT_tls_index);
    if (tls) HeapFree(GetProcessHeap(), 0, tls);
    TRACE("finished thread free\n");
    break;
  }
  return TRUE;
}

static inline BOOL msvcrt_init_tls(void)
{
  MSVCRT_tls_index = TlsAlloc();

  if (MSVCRT_tls_index == TLS_OUT_OF_INDEXES)
  {
    ERR("TlsAlloc() failed!\n");
    return FALSE;
  }
  return TRUE;
}

static inline BOOL msvcrt_free_tls(void)
{
  if (!TlsFree(MSVCRT_tls_index))
  {
    ERR("TlsFree() failed!\n");
    return FALSE;
  }
  return TRUE;
}

const char* msvcrt_get_reason(DWORD reason)
{
  switch (reason)
  {
  case DLL_PROCESS_ATTACH: return "DLL_PROCESS_ATTACH";
  case DLL_PROCESS_DETACH: return "DLL_PROCESS_DETACH";
  case DLL_THREAD_ATTACH:  return "DLL_THREAD_ATTACH";
  case DLL_THREAD_DETACH:  return "DLL_THREAD_DETACH";
  }
  return "UNKNOWN";
}


/*********************************************************************
 *		$I10_OUTPUT (MSVCRT.@)
 * Function not really understood but needed to make the DLL work
 */
void MSVCRT_I10_OUTPUT(void)
{
  /* FIXME: This is probably data, not a function */
}


/*********************************************************************
 *		__unDName (MSVCRT.@)
 * Function not really understood but needed to make the DLL work
 */
char* MSVCRT___unDName(int unknown, const char* mangled,
                       MSVCRT_malloc_func memget,
                       MSVCRT_free_func memfree,
                       unsigned int flags)
{
  char* ret;

  FIXME("(%d,%s,%p,%p,%x) stub!\n", unknown, mangled, memget, memfree, flags);

  /* Experimentation reveals the following flag meanings when set:
   * 0x0001 - Dont show __ in calling convention
   * 0x0002 - Dont show calling convention at all
   * 0x0004 - Dont show function/method return value
   * 0x0010 - Same as 0x1
   * 0x0080 - Dont show access specifier (public/protected/private)
   * 0x0200 - Dont show static specifier
   * 0x1000 - Only report the variable/class name
   */
  /* Duplicate the mangled name; for comparisons it doesn't matter anyway */
  ret = memget(strlen(mangled) + 1);
  strcpy(ret, mangled);
  return ret;
}


/*********************************************************************
 *		__unDNameEx (MSVCRT.@)
 * Function not really understood but needed to make the DLL work
 */
char* MSVCRT___unDNameEx(void)
{
   return NULL;
}
