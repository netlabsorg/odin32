/*
 * Copyright 2001 Jon Griffiths
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

#ifndef __WINE_MSVCRT_H
#define __WINE_MSVCRT_H

#include <stdarg.h>

#include "winbase.h"
#include "winnls.h"
#include "winerror.h"
#include "msvcrt/eh.h"

/* TLS data */
extern DWORD MSVCRT_tls_index;

typedef struct __MSVCRT_thread_data
{
    int                      msv_errno;
    unsigned long            doserrno;
    char                    *mbstok_next;        /* next ptr for mbstok() */
    char                    *efcvt_buffer;       /* buffer for ecvt/fcvt */
    terminate_function       terminate_handler;
    unexpected_function      unexpected_handler;
    _se_translator_function  se_translator;
    EXCEPTION_RECORD        *exc_record;
} MSVCRT_thread_data;

extern MSVCRT_thread_data *msvcrt_get_thread_data(void);

extern int MSVCRT_current_lc_all_cp;

void _purecall(void);
void   MSVCRT__set_errno(int);
char*  msvcrt_strndup(const char*,unsigned int);
LPWSTR msvcrt_wstrndup(LPCWSTR, unsigned int);

void MSVCRT__amsg_exit(int errnum);

extern char **MSVCRT__environ;
extern WCHAR **MSVCRT__wenviron;

extern char ** msvcrt_SnapshotOfEnvironmentA(char **);
extern WCHAR ** msvcrt_SnapshotOfEnvironmentW(WCHAR **);

/* FIXME: This should be declared in new.h but it's not an extern "C" so
 * it would not be much use anyway. Even for Winelib applications.
 */
int    MSVCRT__set_new_mode(int mode);

/* Setup and teardown multi threaded locks */
extern void msvcrt_init_mt_locks(void);
extern void msvcrt_free_mt_locks(void);

extern void msvcrt_init_io(void);
extern void msvcrt_free_io(void);
extern void msvcrt_init_console(void);
extern void msvcrt_free_console(void);
extern void msvcrt_init_args(void);
extern void msvcrt_free_args(void);
extern void msvcrt_init_vtables(void);

/* run-time error codes */
#define _RT_STACK       0
#define _RT_NULLPTR     1
#define _RT_FLOAT       2
#define _RT_INTDIV      3
#define _RT_EXECMEM     5
#define _RT_EXECFORM    6
#define _RT_EXECENV     7
#define _RT_SPACEARG    8
#define _RT_SPACEENV    9
#define _RT_ABORT       10
#define _RT_NPTR        12
#define _RT_FPTR        13
#define _RT_BREAK       14
#define _RT_INT         15
#define _RT_THREAD      16
#define _RT_LOCK        17
#define _RT_HEAP        18
#define _RT_OPENCON     19
#define _RT_QWIN        20
#define _RT_NOMAIN      21
#define _RT_NONCONT     22
#define _RT_INVALDISP   23
#define _RT_ONEXIT      24
#define _RT_PUREVIRT    25
#define _RT_STDIOINIT   26
#define _RT_LOWIOINIT   27
#define _RT_HEAPINIT    28
#define _RT_DOMAIN      120
#define _RT_SING        121
#define _RT_TLOSS       122
#define _RT_CRNL        252
#define _RT_BANNER      255

/* Signal types */

#define SIGINT_W          2       /* interrupt */
#define SIGILL_W          4       /* illegal instruction - invalid function image */
#define SIGFPE_W          8       /* floating point exception */
#define SIGSEGV_W         11      /* segment violation */
#define SIGTERM_W         15      /* Software termination signal from kill */
#define SIGBREAK_W        21      /* Ctrl-Break sequence */
#define SIGABRT_W         22      /* abnormal termination triggered by abort call */

#define SIG_DFL_W (void (__cdecl *)(int))0           /* default signal action */
#define SIG_IGN_W (void (__cdecl *)(int))1           /* ignore signal */
#define SIG_SGE_W (void (__cdecl *)(int))3           /* signal gets error */
#define SIG_ACK_W (void (__cdecl *)(int))4           /* acknowledge */

/* signal error value (returned by signal call on error) */

#define SIG_ERR (void (__cdecl *)(int))-1          /* signal error value */


#endif /* __WINE_MSVCRT_H */
