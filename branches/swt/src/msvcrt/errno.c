/*
 * msvcrt.dll errno functions
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

#ifndef __MINIVCRT__

#include "msvcrt.h"
#include "msvcrt/errno.h"

#include <stdio.h>
#include <string.h>

#include "msvcrt/conio.h"
#include "msvcrt/stdlib.h"
#include "msvcrt/string.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

#else /* !__MINIVCRT__ */

#include <winbase.h>
#include <winerror.h>

#include "minivcrt.h"
#include "minivcrt_internal.h"

#include <errno.h>

#endif /* !__MINIVCRT__ */

/* INTERNAL: Set the crt and dos errno's from the OS error given. */
void MSVCRT__set_errno(int err)
{
#ifndef __MINIVCRT__

  int *__errno = MSVCRT__errno();
  unsigned long *doserrno = MSVCRT_doserrno();

  *doserrno = err;

#define ERR_CASE(oserr) case oserr:
#define ERR_MAPS(oserr,crterr) case oserr:*__errno = MSVCRT_##crterr;break;

#else /* !__MINIVCRT__ */

  int *__errno = _errno();

#define ERR_CASE(oserr) case oserr:
#define ERR_MAPS(oserr,crterr) case oserr:*__errno = crterr;break;

#endif /* !__MINIVCRT__ */

  switch(err)
  {
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
    ERR_MAPS(ERROR_IO_DEVICE,            EIO);
    ERR_MAPS(ERROR_BAD_FORMAT,           ENOEXEC);
    ERR_MAPS(ERROR_INVALID_HANDLE,       EBADF);
    ERR_CASE(ERROR_OUTOFMEMORY)
    ERR_CASE(ERROR_INVALID_BLOCK)
    ERR_CASE(ERROR_NOT_ENOUGH_QUOTA);
    ERR_MAPS(ERROR_ARENA_TRASHED,        ENOMEM);
    ERR_MAPS(ERROR_BUSY,                 EBUSY);
    ERR_CASE(ERROR_ALREADY_EXISTS)
    ERR_MAPS(ERROR_FILE_EXISTS,          EEXIST);
    ERR_MAPS(ERROR_BAD_DEVICE,           ENODEV);
    ERR_MAPS(ERROR_TOO_MANY_OPEN_FILES,  EMFILE);
    ERR_MAPS(ERROR_DISK_FULL,            ENOSPC);
    ERR_MAPS(ERROR_BROKEN_PIPE,          EPIPE);
    ERR_MAPS(ERROR_POSSIBLE_DEADLOCK,    EDEADLK);
    ERR_MAPS(ERROR_DIR_NOT_EMPTY,        ENOTEMPTY);
    ERR_MAPS(ERROR_BAD_ENVIRONMENT,      E2BIG);
    ERR_CASE(ERROR_WAIT_NO_CHILDREN)
    ERR_MAPS(ERROR_CHILD_NOT_COMPLETE,   ECHILD);
    ERR_CASE(ERROR_NO_PROC_SLOTS)
    ERR_CASE(ERROR_MAX_THRDS_REACHED)
    ERR_MAPS(ERROR_NESTING_NOT_ALLOWED,  EAGAIN);
  default:
    /*  Remaining cases map to EINVAL */
    /* FIXME: may be missing some errors above */
    *__errno = MSVCRT(EINVAL);
  }
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_errno (MSVCRT.@)
 */
int* MSVCRT__errno(void)
{
    dprintf(("MSVCRT: __errno %d",msvcrt_get_thread_data()->msv_errno));
    return &msvcrt_get_thread_data()->msv_errno;
}

/*********************************************************************
 *		MSVCRT_doserrno (MSVCRT.@)
 */
unsigned long* MSVCRT_doserrno(void)
{
    dprintf(("MSVCRT: _doserrno %d",msvcrt_get_thread_data()->doserrno));
    return &msvcrt_get_thread_data()->doserrno;
}

/*********************************************************************
 *		strerror (MSVCRT.@)
 */
char* MSVCRT_strerror(int err)
{
  return strerror(err); /* FIXME */
}

/**********************************************************************
 *		_strerror	(MSVCRT.@)
 */
char* MSVCRT__strerror(const char* err)
{
  static char strerrbuff[256]; /* FIXME: Per thread, nprintf */
  sprintf(strerrbuff,"%s: %s\n",err,MSVCRT_strerror(msvcrt_get_thread_data()->msv_errno));
  return strerrbuff;
}

/*********************************************************************
 *		perror (MSVCRT.@)
 */
void MSVCRT_perror(const char* str)
{
  MSVCRT__cprintf("%s: %s\n",str,MSVCRT_strerror(msvcrt_get_thread_data()->msv_errno));
}

#endif /* !__MINIVCRT__ */
