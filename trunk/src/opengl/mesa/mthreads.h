/* $Id: mthreads.h,v 1.1 2000-02-29 00:48:34 sandervl Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.1
 *
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */




/*
 * mthreads.h -- platform dependent thread support for Mesa
 *
 * Initial version by John Stone (j.stone@acm.org) (johns@cs.umr.edu)
 *                and Christoph Poliwoda (poliwoda@volumegraphics.com)
 *
 * Revised by Keith Whitwell
 */



/*
 * If this file is accidentally included by a non-threaded build,
 * it should not cause the build to fail, or otherwise cause problems.
 * In general, it should only be included when needed however.
 */
#ifdef THREADS
/*
 * It is an error not to select a specific threads API when compiling.
 */
#if !defined PTHREADS && !defined SOLARIS_THREADS && !defined WIN32 && !defined(__WIN32OS2__)
#error One of PTHREADS, SOLARIS_THREADS, WIN32 or __WIN32OS2__ must be defined.
#endif


/*
 * Error messages which should be printed when our Mesa thread APIs fail
 * for one reason or another.
 */
#define MESA_INIT_TSD_ERROR "Mesa: thread failed to allocate key for thread specific data"
#define MESA_GET_TSD_ERROR "Mesa: thread failed to get thread specific data"
#define MESA_SET_TSD_ERROR "Mesa: thread failed to set thread specific data"


/*
 * magic number for win32 and solaris threads equivalents of pthread_once
 * This could probably be done better, but we haven't figured out how yet.
 */
#define INITFUNC_CALLED_MAGIC 0xff8adc98




/*
 * POSIX threads. This should be your choice in the Unix world
 * whenever possible.  When building with POSIX threads, be sure
 * to any able any compiler flags which will cause the MT-safe
 * libc (if one exists) to be used when linking, as well as any
 * header macros for MT-safe errno, etc.  For Solaris, this is the -mt
 * compiler flag.  On Solaris with gcc, use -D_REENTRANT to enable
 * proper compiling for MT-safe libc etc.
 */
#ifdef PTHREADS
#include <pthread.h> /* POSIX threads headers */

typedef struct {
  pthread_key_t  key;
  pthread_once_t once;
} MesaTSD;

typedef pthread_mutex_t MesaMutex;
typedef pthread_t MesaThread;

#endif /* PTHREADS */




/*
 * Solaris threads. Use only up to Solaris 2.4.
 * Solaris 2.5 and higher provide POSIX threads.
 * Be sure to compile with -mt on the Solaris compilers, or
 * use -D_REENTRANT if using gcc.
 */
#ifdef SOLARIS_THREADS
#include <thread.h>

typedef struct {
  thread_key_t key;
  mutex_t      keylock;
  int          initfuncCalled;
} MesaTSD;

typedef mutex_t MesaMutex;
typedef thread_t MesaThread;

#endif /* SOLARIS_THREADS */




/*
 * Windows threads. Should work with Windows NT and 95.
 * IMPORTANT: Link with multithreaded runtime library when THREADS are
 * used!
 */

#if defined(WIN32) || defined(__WIN32OS2__)

#ifdef __WIN32OS2__
#include <os2win.h>
#else
#include <windows.h>
#endif

typedef struct {
  DWORD key;
  int   initfuncCalled;
} MesaTSD;

typedef CRITICAL_SECTION MesaMutex;
typedef HANDLE MesaThread;

#endif /* WIN32 */




/*
 * Platform independent thread specific data API.
 */
void  MesaInitTSD(MesaTSD *);
void* MesaGetTSD (MesaTSD *);
void  MesaSetTSD (MesaTSD *, void *, void (*initfunc)(void));


/*
 * The following APIs are preliminary.  They may be needed in
 * future versions of Mesa.
 */
#if 0

/*
 * Platform independent mutual exclusion lock API.
 */
void  MesaInitMutex    (MesaMutex *);
void  MesaDestroyMutex (MesaMutex *);
void  MesaLockMutex    (MesaMutex *);
void  MesaUnlockMutex  (MesaMutex *);

/*
 * Platform independent thread management API.
 */
MesaThread  MesaCreateThread ((void*)(*)(void*), void*);
void        MesaJoinThread   (MesaThread);

#endif

#endif /* THREADS */
