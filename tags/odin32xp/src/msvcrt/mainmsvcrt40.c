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

#include <windows.h>
#include <win32type.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <initdll.h>
#include <exitlist.h>
#include <os2sel.h>

#include "msvcrt.h"
#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)
#include "msvcrt/locale.h"
#include "msvcrt/stdio.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

/* Index to TLS */
DWORD MSVCRT_tls_index;

static inline BOOL msvcrt_init_tls(void);
static inline BOOL msvcrt_free_tls(void);
const char* msvcrt_get_reason(DWORD reason) WINE_UNUSED;

typedef void* (*MSVCRT_malloc_func)(unsigned int);
typedef void (*MSVCRT_free_func)(void*);

static HMODULE dllHandle = 0;

BOOL WINAPI MSVCRT40_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);


unsigned long _DLL_InitTerm (unsigned long mod_handle,
                                unsigned long flag)
{
  switch (flag)
    {
       case 0:
         dllHandle = RegisterLxDll(mod_handle, MSVCRT40_Init, 0,0,0,0);
         return 1;
       case 1:

         if(dllHandle) {
             UnregisterLxDll(dllHandle);
         }

         return 1;
       default:
         return 0;
       }
     return 1;
   }

/*********************************************************************
 *                  Init
 */
BOOL WINAPI MSVCRT40_Init(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
 return TRUE;
}
 

