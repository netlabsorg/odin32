/*
 * msvcrt.dll drive/directory functions
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997,2000 Uwe Bonnes
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

#ifdef __WIN32OS2__
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <winbase.h>
#include "emxheader.h"
#else
#include "config.h"
#endif

#ifndef __MINIVCRT__

#include "wine/port.h"

#include <time.h>
#include "winternl.h"
#include "wine/unicode.h"
#include "msvcrt.h"
#include "msvcrt/errno.h"

#include "wine/unicode.h"
#include "msvcrt/direct.h"
#include "msvcrt/dos.h"
#include "msvcrt/io.h"
#include "msvcrt/stdlib.h"
#include "msvcrt/string.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msvcrt);

#else /* !__MINIVCRT__ */

#include "minivcrt.h"
#include "minivcrt_internal.h"

#include "winternl.h"
#include "wine/unicode.h"

#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#ifdef __EMX__
#include <direct.h>
#endif

#endif /* !__MINIVCRT__ */

#ifndef __MINIVCRT__

/* INTERNAL: Translate finddata_t to PWIN32_FIND_DATAA */
static void msvcrt_fttofd(LPWIN32_FIND_DATAA fd, struct _finddata_t* ft)
{
  DWORD dw;

  if (fd->dwFileAttributes == FILE_ATTRIBUTE_NORMAL)
    ft->attrib = 0;
  else
    ft->attrib = fd->dwFileAttributes;

  RtlTimeToSecondsSince1970( (LARGE_INTEGER *)&fd->ftCreationTime, &dw );
  ft->time_create = dw;
  RtlTimeToSecondsSince1970( (LARGE_INTEGER *)&fd->ftLastAccessTime, &dw );
  ft->time_access = dw;
  RtlTimeToSecondsSince1970( (LARGE_INTEGER *)&fd->ftLastWriteTime, &dw );
  ft->time_write = dw;
  ft->size = fd->nFileSizeLow;
  strcpy(ft->name, fd->cFileName);
}

#endif /* !__MINIVCRT__ */

/* INTERNAL: Translate wfinddata_t to PWIN32_FIND_DATAA */
static void msvcrt_wfttofd(LPWIN32_FIND_DATAW fd, struct _wfinddata_t* ft)
{
  DWORD dw;

  if (fd->dwFileAttributes == FILE_ATTRIBUTE_NORMAL)
    ft->attrib = 0;
  else
    ft->attrib = fd->dwFileAttributes;

  RtlTimeToSecondsSince1970( (LARGE_INTEGER *)&fd->ftCreationTime, &dw );
  ft->time_create = dw;
  RtlTimeToSecondsSince1970( (LARGE_INTEGER *)&fd->ftLastAccessTime, &dw );
  ft->time_access = dw;
  RtlTimeToSecondsSince1970( (LARGE_INTEGER *)&fd->ftLastWriteTime, &dw );
  ft->time_write = dw;
  ft->size = fd->nFileSizeLow;
  strcpyW(ft->name, fd->cFileName);
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_chdir (MSVCRT.@)
 */
int MSVCRT__chdir(const char * newdir)
{
  if (!SetCurrentDirectoryA(newdir))
  {
    MSVCRT__set_errno(newdir?GetLastError():0);
    return -1;
  }
  return 0;
}

#endif /* !__MINIVCRT__ */

/*********************************************************************
 *		_wchdir (MSVCRT.@)
 */
int _wchdir(const MSVCRT_wchar_t * newdir)
{
  if (!SetCurrentDirectoryW(newdir))
  {
    MSVCRT__set_errno(newdir?GetLastError():0);
    return -1;
  }
  return 0;
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_chdrive (MSVCRT.@)
 */
int MSVCRT__chdrive(int newdrive)
{
  char buffer[3] = "A:";
  buffer[0] += newdrive - 1;
  if (!SetCurrentDirectoryA( buffer ))
  {
    MSVCRT__set_errno(GetLastError());
    if (newdrive <= 0)
      *MSVCRT__errno() = MSVCRT_EACCES;
    return -1;
  }
  return 0;
}

/*********************************************************************
 *		_findclose (MSVCRT.@)
 */
int _findclose(long hand)
{
  TRACE(":handle %ld\n",hand);
  if (!FindClose((HANDLE)hand))
  {
    MSVCRT__set_errno(GetLastError());
    return -1;
  }
  return 0;
}

/*********************************************************************
 *		_findfirst (MSVCRT.@)
 */
long _findfirst(const char * fspec, struct _finddata_t* ft)
{
  WIN32_FIND_DATAA find_data;
  HANDLE hfind;

  hfind  = FindFirstFileA(fspec, &find_data);
  if (hfind == INVALID_HANDLE_VALUE)
  {
    MSVCRT__set_errno(GetLastError());
    return -1;
  }
  msvcrt_fttofd(&find_data,ft);
  TRACE(":got handle %p\n",hfind);
  return (long)hfind;
}

#endif /* !__MINIVCRT__ */

/*********************************************************************
 *		_wfindfirst (MSVCRT.@)
 */
long _wfindfirst(const MSVCRT_wchar_t * fspec, struct _wfinddata_t* ft)
{
  WIN32_FIND_DATAW find_data;
  HANDLE hfind;

  hfind  = FindFirstFileW(fspec, &find_data);
  if (hfind == INVALID_HANDLE_VALUE)
  {
    MSVCRT__set_errno(GetLastError());
    return -1;
  }
  msvcrt_wfttofd(&find_data,ft);
  TRACE(":got handle %p\n",hfind);
  return (long)hfind;
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_findnext (MSVCRT.@)
 */
int _findnext(long hand, struct _finddata_t * ft)
{
  WIN32_FIND_DATAA find_data;

  if (!FindNextFileA((HANDLE)hand, &find_data))
  {
    *MSVCRT__errno() = MSVCRT(ENOENT);
    return -1;
  }

  msvcrt_fttofd(&find_data,ft);
  return 0;
}

#endif /* !__MINIVCRT__ */

/*********************************************************************
 *		_wfindnext (MSVCRT.@)
 */
int _wfindnext(long hand, struct _wfinddata_t * ft)
{
  WIN32_FIND_DATAW find_data;

  if (!FindNextFileW((HANDLE)hand, &find_data))
  {
    *MSVCRT__errno() = MSVCRT(ENOENT);
    return -1;
  }

  msvcrt_wfttofd(&find_data,ft);
  return 0;
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_getcwd (MSVCRT.@)
 */
char* MSVCRT__getcwd(char * buf, int size)
{
  char dir[MAX_PATH];
  int dir_len = GetCurrentDirectoryA(MAX_PATH,dir);

  if (dir_len < 1)
    return NULL; /* FIXME: Real return value untested */

  if (!buf)
  {
    if (size < 0)
      return MSVCRT__strdup(dir);
    return msvcrt_strndup(dir,size);
  }
  if (dir_len >= size)
  {
    *MSVCRT__errno() = MSVCRT_ERANGE;
    return NULL; /* buf too small */
  }
  strcpy(buf,dir);
  return buf;
}

#endif /* !__MINIVCRT__ */

/*********************************************************************
 *		_wgetcwd (MSVCRT.@)
 */
MSVCRT_wchar_t* _wgetcwd(MSVCRT_wchar_t * buf, int size)
{
  MSVCRT_wchar_t dir[MAX_PATH];
  int dir_len = GetCurrentDirectoryW(MAX_PATH,dir);

  if (dir_len < 1)
    return NULL; /* FIXME: Real return value untested */

  if (!buf)
  {
    if (size < 0)
      return _wcsdup(dir);
    return msvcrt_wstrndup(dir,size);
  }
  if (dir_len >= size)
  {
    *MSVCRT__errno() = MSVCRT(ERANGE);
    return NULL; /* buf too small */
  }
  strcpyW(buf,dir);
  return buf;
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_getdrive (MSVCRT.@)
 */
int MSVCRT__getdrive(void)
{
    char buffer[MAX_PATH];
    if (!GetCurrentDirectoryA( sizeof(buffer), buffer )) return 0;
    if (buffer[1] != ':') return 0;
    return toupper(buffer[0]) - 'A' + 1;
}

/*********************************************************************
 *		_getdcwd (MSVCRT.@)
 */
char* MSVCRT__getdcwd(int drive, char * buf, int size)
{
  static char* dummy;

  TRACE(":drive %d(%c), size %d\n",drive, drive + 'A' - 1, size);

  if (!drive || drive == MSVCRT__getdrive())
    return MSVCRT__getcwd(buf,size); /* current */
  else
  {
    char dir[MAX_PATH];
    char drivespec[4] = {'A', ':', '\\', 0};
    int dir_len;

    drivespec[0] += drive - 1;
    if (GetDriveTypeA(drivespec) < DRIVE_REMOVABLE)
    {
      *MSVCRT__errno() = MSVCRT_EACCES;
      return NULL;
    }

    dir_len = GetFullPathNameA(drivespec,MAX_PATH,dir,&dummy);
    if (dir_len >= size || dir_len < 1)
    {
      *MSVCRT__errno() = MSVCRT_ERANGE;
      return NULL; /* buf too small */
    }

    TRACE(":returning '%s'\n", dir);
    if (!buf)
      return MSVCRT__strdup(dir); /* allocate */

    strcpy(buf,dir);
  }
  return buf;
}

#endif /* !__MINIVCRT__ */

/*********************************************************************
 *		_wgetdcwd (MSVCRT.@)
 */
MSVCRT_wchar_t* _wgetdcwd(int drive, MSVCRT_wchar_t * buf, int size)
{
  static MSVCRT_wchar_t* dummy;

  TRACE(":drive %d(%c), size %d\n",drive, drive + 'A' - 1, size);

  if (!drive || drive == MSVCRT__getdrive())
    return _wgetcwd(buf,size); /* current */
  else
  {
    MSVCRT_wchar_t dir[MAX_PATH];
    MSVCRT_wchar_t drivespec[4] = {'A', ':', '\\', 0};
    int dir_len;

    drivespec[0] += drive - 1;
    if (GetDriveTypeW(drivespec) < DRIVE_REMOVABLE)
    {
      *MSVCRT__errno() = MSVCRT(EACCES);
      return NULL;
    }

    dir_len = GetFullPathNameW(drivespec,MAX_PATH,dir,&dummy);
    if (dir_len >= size || dir_len < 1)
    {
      *MSVCRT__errno() = MSVCRT(ERANGE);
      return NULL; /* buf too small */
    }

    TRACE(":returning %s\n", debugstr_w(dir));
    if (!buf)
      return _wcsdup(dir); /* allocate */
    strcpyW(buf,dir);
  }
  return buf;
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_getdiskfree (MSVCRT.@)
 */
unsigned int _getdiskfree(unsigned int disk, struct _diskfree_t* d)
{
  char drivespec[4] = {'@', ':', '\\', 0};
  DWORD ret[4];
  unsigned int err;

  if (disk > 26)
    return ERROR_INVALID_PARAMETER; /* MSVCRT doesn't set errno here */

  drivespec[0] += disk; /* make a drive letter */

  if (GetDiskFreeSpaceA(disk==0?NULL:drivespec,ret,ret+1,ret+2,ret+3))
  {
    d->sectors_per_cluster = (unsigned)ret[0];
    d->bytes_per_sector = (unsigned)ret[1];
    d->avail_clusters = (unsigned)ret[2];
    d->total_clusters = (unsigned)ret[3];
    return 0;
  }
  err = GetLastError();
  MSVCRT__set_errno(err);
  return err;
}

/*********************************************************************
 *		_mkdir (MSVCRT.@)
 */
int MSVCRT__mkdir(const char * newdir)
{
  if (CreateDirectoryA(newdir,NULL))
    return 0;
  MSVCRT__set_errno(GetLastError());
  return -1;
}

#endif /* !__MINIVCRT__ */

/*********************************************************************
 *		_wmkdir (MSVCRT.@)
 */
int _wmkdir(const MSVCRT_wchar_t* newdir)
{
  if (CreateDirectoryW(newdir,NULL))
    return 0;
  MSVCRT__set_errno(GetLastError());
  return -1;
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_rmdir (MSVCRT.@)
 */
int MSVCRT__rmdir(const char * dir)
{
  if (RemoveDirectoryA(dir))
    return 0;
  MSVCRT__set_errno(GetLastError());
  return -1;
}

#endif /* !__MINIVCRT__ */

/*********************************************************************
 *		_wrmdir (MSVCRT.@)
 */
int _wrmdir(const MSVCRT_wchar_t * dir)
{
  if (RemoveDirectoryW(dir))
    return 0;
  MSVCRT__set_errno(GetLastError());
  return -1;
}

/*********************************************************************
 *		_wsplitpath (MSVCRT.@)
 */
void _wsplitpath(const MSVCRT_wchar_t *inpath, MSVCRT_wchar_t *drv, MSVCRT_wchar_t *dir,
                 MSVCRT_wchar_t *fname, MSVCRT_wchar_t *ext )
{
  /* Modified PD code from 'snippets' collection. */
  MSVCRT_wchar_t ch, *ptr, *p;
  MSVCRT_wchar_t pathbuff[MAX_PATH],*path=pathbuff;

  TRACE("MSVCRT: _wsplitpath %s\n",debugstr_w(path));
  /* FIXME: Should be an strncpyW or something */
  strcpyW(pathbuff, inpath);

  /* convert slashes to backslashes for searching */
  for (ptr = (MSVCRT_wchar_t*)path; *ptr; ++ptr)
    if (*ptr == '/')
      *ptr = '\\';

  /* look for drive spec */
  if ((ptr = strchrW(path, ':')) != 0)
  {
    ++ptr;
    if (drv)
    {
      strncpyW(drv, path, ptr - path);
      drv[ptr - path] = 0;
    }
    path = ptr;
  }
  else if (drv)
    *drv = 0;

  /* find rightmost backslash or leftmost colon */
  if ((ptr = strrchrW(path, '\\')) == NULL)
    ptr = (strchrW(path, ':'));

  if (!ptr)
  {
    ptr = (MSVCRT_wchar_t *)path; /* no path */
    if (dir)
      *dir = 0;
  }
  else
  {
    ++ptr; /* skip the delimiter */
    if (dir)
    {
      ch = *ptr;
      *ptr = 0;
      strcpyW(dir, path);
      *ptr = ch;
    }
  }

  if ((p = strrchrW(ptr, '.')) == NULL)
  {
    if (fname)
      strcpyW(fname, ptr);
    if (ext)
      *ext = 0;
  }
  else
  {
    *p = 0;
    if (fname)
      strcpyW(fname, ptr);
    *p = '.';
    if (ext)
      strcpyW(ext, p);
  }

  /* Fix pathological case - Win returns ':' as part of the
   * directory when no drive letter is given.
   */
  if (drv && drv[0] == ':')
  {
    *drv = 0;
    if (dir)
    {
      pathbuff[0] = ':';
      pathbuff[1] = 0;
      strcatW(pathbuff,dir);
      strcpyW(dir, pathbuff);
    }
  }
}

static char *msvcrt_clean_path(char *path)
{
    char *cur, *sep, *next;

    /* convert "/" => "\" */
    cur = path;
    while (*cur)
    {
        if (*cur == '/')
            *cur = '\\';
        ++cur;
    }

    cur = path;
    while (*cur)
    {
        sep = strchr(cur, '\\');
        if (!sep)
            sep = cur + strlen(cur);

        next = sep;
        while (*next == '\\')
            ++next;

        if (sep - cur == 1 && *cur == '.')
        {
            /* eat "." */
            strcpy(cur, next);
        }
        else if (sep - cur == 2 && cur[0] == '.' && cur[1] == '.')
        {
            /* go one level up if there is any */
            if (cur - path > 1 && cur[-2] != ':')
            {
                cur -= 2;
                while (cur > path && *cur != '\\')
                    --cur;
                if (*cur == '\\')
                    ++cur;
            }
            strcpy(cur, next);
        }
        else if (next - sep > 1)
        {
            /* eat multiple "\\" */
            cur = sep + 1;
            strcpy(cur, next);
        }
        else
        {
            cur = next;
        }
    }

    /* strip trailing '\' unless it indicates the root path */
    if (cur - path > 1 && cur[-1] == '\\' && cur[-2] != ':')
        *--cur = '\0';

    return path;
}

#ifdef __EMX__

/*
 * NOTE: _fullpath() is broken in kLIBC and changes CWD (see
 * http://svn.netlabs.org/odin32/ticket/63 for details). Enable the
 * alternative implementation.
 */

/*********************************************************************
 *		_fullpath (MSVCRT.@)
 */
char *MSVCRT__fullpath(char * absPath, const char* relPath, unsigned int size)
{
  char drive[5],dir[MAX_PATH],file[MAX_PATH],ext[MAX_PATH];
  char res[MAX_PATH];
  size_t len;

  res[0] = '\0';

  if (!relPath || !*relPath)
    return msvcrt_clean_path(MSVCRT(_getcwd)(absPath, size));

  if (size < 4)
  {
    *MSVCRT__errno() = MSVCRT(ERANGE);
    return NULL;
  }

  TRACE("MSVCRT: _fullpath '%s'\n",relPath);

  _splitpath(relPath, drive, dir, file, ext);

  /* Get Directory and drive into 'res' */
  if (!dir[0] || (dir[0] != '/' && dir[0] != '\\'))
  {
    /* Relative or no directory given */
    _getdcwd(drive[0] ? toupper(drive[0]) - 'A' + 1 :  0, res, MAX_PATH);
    strcat(res,"\\");
    if (dir[0])
      strcat(res,dir);
    if (drive[0])
      res[0] = drive[0]; /* If given a drive, preserve the letter case */
  }
  else
  {
    if (!drive[0])
    {
      drive[0] = _getdrive();
      drive[1] = ':';
      drive[2] = '\0';
    }
    strcpy(res,drive);
    strcat(res,dir);
  }

  strcat(res,"\\");
  strcat(res, file);
  strcat(res, ext);
  msvcrt_clean_path(res);

  len = strlen(res);
  if (len >= MAX_PATH || len >= (size_t)size)
    return NULL; /* FIXME: errno? */

  if (!absPath)
    return MSVCRT(_strdup(res));
  strcpy(absPath,res);
  return absPath;
}

#endif /* __EMX__ */

#ifndef __MINIVCRT__

/*********************************************************************
 *		_makepath (MSVCRT.@)
 */
VOID MSVCRT__makepath(char * path, const char * drive,
                              const char *directory, const char * filename,
                              const char * extension )
{
    char ch;
    TRACE("got %s %s %s %s\n", drive, directory,
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
            if ( extension[0] != '.' )
                strcat(path,".");
            strcat(path,extension);
        }
    }

    TRACE("returning %s\n",path);
}

#endif /* !__MINIVCRT__ */

/*********************************************************************
 *		_wmakepath (MSVCRT.@)
 */
VOID _wmakepath(MSVCRT_wchar_t *path, const MSVCRT_wchar_t *drive, const MSVCRT_wchar_t *directory,
		const MSVCRT_wchar_t *filename, const MSVCRT_wchar_t *extension)
{
    MSVCRT_wchar_t ch;
    TRACE("%s %s %s %s\n", debugstr_w(drive), debugstr_w(directory),
	  debugstr_w(filename), debugstr_w(extension));

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
        strcatW(path, directory);
        ch = path[strlenW(path) - 1];
        if (ch != '/' && ch != '\\')
	{
	    static const MSVCRT_wchar_t backslashW[] = {'\\',0};
            strcatW(path, backslashW);
	}
    }
    if (filename && filename[0])
    {
        strcatW(path, filename);
        if (extension && extension[0])
        {
            if ( extension[0] != '.' )
	    {
		static const MSVCRT_wchar_t dotW[] = {'.',0};
                strcatW(path, dotW);
	    }
            strcatW(path, extension);
        }
    }

    TRACE("returning %s\n", debugstr_w(path));
}

#ifndef __MINIVCRT__

/*********************************************************************
 *		_searchenv (MSVCRT.@)
 */
void MSVCRT__searchenv(const char* file, const char* env, char *buf)
{
  char*envVal, *penv;
  char curPath[MAX_PATH];

  *buf = '\0';

  /* Try CWD first */
  if (GetFileAttributesA( file ) != 0xFFFFFFFF)
  {
    GetFullPathNameA( file, MAX_PATH, buf, NULL );
    /* Sigh. This error is *always* set, regardless of success */
    MSVCRT__set_errno(ERROR_FILE_NOT_FOUND);
    return;
  }

  /* Search given environment variable */
  envVal = MSVCRT_getenv(env);
  if (!envVal)
  {
    MSVCRT__set_errno(ERROR_FILE_NOT_FOUND);
    return;
  }

  penv = envVal;
  TRACE(":searching for %s in paths %s\n", file, envVal);

  do
  {
    char *end = penv;

    while(*end && *end != ';') end++; /* Find end of next path */
    if (penv == end || !*penv)
    {
      MSVCRT__set_errno(ERROR_FILE_NOT_FOUND);
      return;
    }
    strncpy(curPath, penv, end - penv);
    if (curPath[end - penv] != '/' || curPath[end - penv] != '\\')
    {
      curPath[end - penv] = '\\';
      curPath[end - penv + 1] = '\0';
    }
    else
      curPath[end - penv] = '\0';

    strcat(curPath, file);
    TRACE("Checking for file %s\n", curPath);
    if (GetFileAttributesA( curPath ) != 0xFFFFFFFF)
    {
      strcpy(buf, curPath);
      MSVCRT__set_errno(ERROR_FILE_NOT_FOUND);
      return; /* Found */
    }
    penv = *end ? end + 1 : end;
  } while(1);
}

#endif /* !__MINIVCRT__ */

MSVCRT_wchar_t* _wfullpath(MSVCRT_wchar_t* absPath,const MSVCRT_wchar_t* relPath,MSVCRT(size_t) size)
{
   char asciiabsPath[280], asciirelPath[280];

   TRACE("MSVCRT: _wfullpath %s %d\n", debugstr_w(relPath),size);

   WideCharToMultiByte(CP_ACP, 0, relPath, -1, (LPSTR)asciirelPath, 280, NULL, NULL);

   MSVCRT__fullpath(asciiabsPath, asciirelPath, size);

   MultiByteToWideChar(CP_ACP, 0, asciiabsPath, -1, absPath, size);

   return absPath;
}
