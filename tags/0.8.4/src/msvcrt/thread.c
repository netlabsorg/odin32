/*
 * msvcrt.dll thread functions
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
#include <string.h>
#include "msvcrt/malloc.h"
#include "msvcrt/process.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

/********************************************************************/

typedef struct {
  _beginthread_start_routine_t start_address;
  void *arglist;
} _beginthread_trampoline_t;

/*********************************************************************
 *		msvcrt_get_thread_data
 *
 * Return the thread local storage structure.
 */
MSVCRT_thread_data *msvcrt_get_thread_data(void)
{
    MSVCRT_thread_data *ptr;
    DWORD err = GetLastError();  /* need to preserve last error */

    if (!(ptr = TlsGetValue( MSVCRT_tls_index )))
    {
        if (!(ptr = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*ptr) )))
            MSVCRT__amsg_exit(16);
        if (!TlsSetValue( MSVCRT_tls_index, ptr )) MSVCRT__amsg_exit(16);
    }
    SetLastError( err );
    return ptr;
}


/*********************************************************************
 *		_beginthread_trampoline
 */
static DWORD CALLBACK _beginthread_trampoline(LPVOID arg)
{
    _beginthread_trampoline_t local_trampoline;

    /* Maybe it's just being paranoid, but freeing arg right
     * away seems safer.
     */
    memcpy(&local_trampoline,arg,sizeof(local_trampoline));
    MSVCRT_free(arg);

    local_trampoline.start_address(local_trampoline.arglist);
    return 0;
}

/*********************************************************************
 *		_beginthread (MSVCRT.@)
 */
unsigned long MSVCRT__beginthread(
  _beginthread_start_routine_t start_address, /* [in] Start address of routine that begins execution of new thread */
  unsigned int stack_size, /* [in] Stack size for new thread or 0 */
  void *arglist)           /* [in] Argument list to be passed to new thread or NULL */
{
  _beginthread_trampoline_t* trampoline;

  TRACE("(%p, %d, %p)\n", start_address, stack_size, arglist);

  /* Allocate the trampoline here so that it is still valid when the thread
   * starts... typically after this function has returned.
   * _beginthread_trampoline is responsible for freeing the trampoline
   */
  trampoline=MSVCRT_malloc(sizeof(*trampoline));
  trampoline->start_address = start_address;
  trampoline->arglist = arglist;

  /* FIXME */
  return CreateThread(NULL, stack_size, _beginthread_trampoline, trampoline, 0, NULL);
}

/*********************************************************************
 *		_beginthreadex (MSVCRT.@)
 */
unsigned long _beginthreadex(
  void *security,          /* [in] Security descriptor for new thread; must be NULL for Windows 9x applications */
  unsigned int stack_size, /* [in] Stack size for new thread or 0 */
  _beginthreadex_start_routine_t start_address, /* [in] Start address of routine that begins execution of new thread */
  void *arglist,           /* [in] Argument list to be passed to new thread or NULL */
  unsigned int initflag,   /* [in] Initial state of new thread (0 for running or CREATE_SUSPEND for suspended) */
  unsigned int *thrdaddr)  /* [out] Points to a 32-bit variable that receives the thread identifier */
{
  TRACE("(%p, %d, %p, %p, %d, %p)\n", security, stack_size, start_address, arglist, initflag, thrdaddr);

  /* FIXME */
  return CreateThread(security, stack_size, (LPTHREAD_START_ROUTINE) start_address,
                      arglist, initflag, (LPDWORD) thrdaddr);
}

/*********************************************************************
 *		MSVCRT__endthread (MSVCRT.@)
 */
void MSVCRT__endthread(void)
{
  TRACE("MSVCRT: _endthread(void)\n");

  /* FIXME */
  ExitThread(0);
}

/*********************************************************************
 *		_endthreadex (MSVCRT.@)
 */
void _endthreadex(
  unsigned int retval) /* [in] Thread exit code */
{
  TRACE("MSVCRT: _endthreadex (%d)\n", retval);

  /* FIXME */
  ExitThread(retval);
}
