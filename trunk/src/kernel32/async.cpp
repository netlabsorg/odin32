/* $Id: async.cpp,v 1.10 2001-03-13 18:45:32 sandervl Exp $ */

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

