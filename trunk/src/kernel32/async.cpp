/* $Id: async.cpp,v 1.7 2000-02-16 14:25:30 sandervl Exp $ */

/*
 * Win32 Asynchronous I/O Subsystem for OS/2
 *
 * 1998/04/10 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) Async.Cpp       1.0.0   1998/04/10 PH start
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _ASYNCIOSUBSYSTEM_H_
#define _ASYNCIOSUBSYSTEM_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

  ReadFileEx

 */



/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#define  INCL_WIN
#define  INCL_DOSMEMMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#define  INCL_DOSPROCESS
#define  INCL_DOSMODULEMGR
#define  INCL_VIO
#define  INCL_AVIO
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <builtin.h>

#include <stdlib.h>
#include <string.h>
#include "win32type.h"
#include "misc.h"
#include "unicode.h"

#include "conwin.h"          // Windows Header for console only
#include "handlemanager.h"

#define DBG_LOCALLOG	DBG_async
#include "dbglocal.h"


/*****************************************************************************
 * Defines & Macros                                                          *
 *****************************************************************************/


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct _IORequest
{
  struct _IORequest *pNext;                   /* pointer to next I/O request */

  APIRET rc;                                   /* result code of I/O request */

} IOREQUEST, *PIOREQUEST;


typedef struct _Globals
{
  HEV hevIOEvent;                      /* asynchronous I/O event completed ! */
  TID tidIO;                           /* I/O thread                         */
} GLOBALS, *PGLOBALS;

static GLOBALS Globals;


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

DWORD WIN32API SleepEx(DWORD dwTimeout,
                          BOOL  fAlertable)
{

  dprintf(("KERNEL32:  SleepEx(%u,%u)\n",
           dwTimeout,
           fAlertable));

  /* @@@PH could be implemented as a timed wait on a event semaphore */
  /*       for the WAIT_IO_COMPLETION flag                           */

  if (fAlertable == FALSE)
    DosSleep(dwTimeout);
  else {
    dprintf(("SleepEx: Wait for io completion not supported!"));
    DosSleep(1);
  }

  return (0);
}


/*****************************************************************************
 * Name      : BOOL ReadFileEx
 * Purpose   : The ReadFileEx function reads data from a file asynchronously.
 *             It is designed solely for asynchronous operation, unlike the
 *             ReadFile function, which is designed for both synchronous and
 *             asynchronous operation. ReadFileEx lets an application perform
 *             other processing during a file read operation.
 *             The ReadFileEx function reports its completion status asynchronously,
 *             calling a specified completion routine when reading is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to read
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to read
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

#define LPOVERLAPPED_COMPLETION_ROUTINE LPVOID

DWORD WIN32API ReadFileEx(HANDLE       hFile,
                             LPVOID       lpBuffer,
                             DWORD        nNumberOfBytesToRead,
                             LPOVERLAPPED lpOverlapped,
                             LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("Kernel32: ReadFileEx(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hFile,
           lpBuffer,
           nNumberOfBytesToRead,
           lpOverlapped,
           lpCompletionRoutine));


  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WriteFileEx
 * Purpose   : The WriteFileEx function writes data to a file. It is designed
 *             solely for asynchronous operation, unlike WriteFile, which is
 *             designed for both synchronous and asynchronous operation.
 *             WriteFileEx reports its completion status asynchronously,
 *             calling a specified completion routine when writing is completed
 *             and the calling thread is in an alertable wait state.
 * Parameters: HANDLE       hFile                handle of file to write
 *             LPVOID       lpBuffer             address of buffer
 *             DWORD        nNumberOfBytesToRead number of bytes to write
 *             LPOVERLAPPED lpOverlapped         address of offset
 *             LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine address of completion routine
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API WriteFileEx(HANDLE       hFile,
                              LPVOID       lpBuffer,
                              DWORD        nNumberOfBytesToWrite,
                              LPOVERLAPPED lpOverlapped,
                              LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
{
  dprintf(("Kernel32: WriteFileEx(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hFile,
           lpBuffer,
           nNumberOfBytesToWrite,
           lpOverlapped,
           lpCompletionRoutine));


  return (FALSE);
}


#endif /* _ASYNCIOSUBSYSTEM_H_ */
