/* $Id: lfile.cpp,v 1.8 2003-05-12 15:29:09 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 compatibility file functions for OS/2
 *
 * 1998/06/12 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) LFILE.C         1.0.0 1998/06/12 PH  added HandleManager support
 *                      1.1.0 1998/08/29 KSO corrected error codes and forwarded
 *                                           to Win32 file api as NT/95 does.
 *                      1.1.1 1999/06/09 PH  NOTE: only forard to KERNEL32:FILEIO
 *                                           calls, never do anything else !
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>

#define DBG_LOCALLOG	DBG_lfile
#include "dbglocal.h"


/*****************************************************************************
 * Name      : HFILE WIN32API _lclose
 * Purpose   : forward call to CloseHandle
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Depends on CloseHandle working.
 * Status    : Tested
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

HFILE WIN32API _lclose(HFILE arg1)
{
   dprintf(("KERNEL32: _lclose(%08xh)\n",
            arg1));

   if (CloseHandle(arg1))
      return 0;
   else
      return -1;
}


/*****************************************************************************
 * Name      : HFILE WIN32API _lcreat
 * Purpose   : forward call to CreateFileA
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Depends on CreateFile working.
 * Status    : Tested
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

HFILE WIN32API _lcreat(LPCSTR arg1,
                       int    arg2)
{
   HANDLE hFile;

   dprintf(("KERNEL32: _lcreat(%s, %08xh)\n",
            arg1,
            arg2));

   //if (arg2 & ~(1 | 2 | 4))
   //   dprintf(("KERNEL32: Warning: _lcreat has unknown flag(s) set - fails.\n"));

   //SetLastError(0); - CreateFileA sets error
   hFile = CreateFileA(arg1,                              /* filename */
                       GENERIC_READ | GENERIC_WRITE,      /* readwrite access */
                       FILE_SHARE_READ | FILE_SHARE_WRITE,/* share all */
                       NULL,                              /* ignore scurity */
                       CREATE_ALWAYS,                     /* create (trunkate don't work) */
                       arg2 & 0x3fb7,                     /* so M$ does (that is more attributes than I could find in the headers and in the docs!) */
                       NULL);                             /* no template */

   dprintf(("KERNEL32: _lcreat returns %08xh.\n",
            hFile));
   return hFile;
}


/*****************************************************************************
 * Name      : HFILE WIN32API _lopen
 * Purpose   : forward call to CreateFileA
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : Tested
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

HFILE WIN32API _lopen(LPCSTR pszFileName,
                      int    mode)
{
   ULONG  ulAccess = 0;
   ULONG  ulShare;
   HANDLE hFile;

   dprintf(("KERNEL32: _lopen(%s, %08xh)\n",
            pszFileName,
            mode));

   if (mode & ~(OF_READ|OF_READWRITE|OF_WRITE|OF_SHARE_COMPAT|OF_SHARE_DENY_NONE|OF_SHARE_DENY_READ|OF_SHARE_DENY_WRITE|OF_SHARE_EXCLUSIVE))
      dprintf(("KERNEL32: (warn) _lopen has unknown flag(s) set.\n"));

   /* Access */
   switch(mode & 0x03)
   {
   case OF_READ:      ulAccess = GENERIC_READ; break;
   case OF_WRITE:     ulAccess = GENERIC_WRITE; break;
   case OF_READWRITE: ulAccess = GENERIC_READ | GENERIC_WRITE; break;
   default:           ulAccess = 0; break;
   }

   /* Share */
   ulShare = mode & (OF_SHARE_COMPAT | OF_SHARE_DENY_NONE | OF_SHARE_DENY_READ | OF_SHARE_DENY_WRITE | OF_SHARE_EXCLUSIVE);
   if (ulShare == OF_SHARE_DENY_READ)
      ulShare = FILE_SHARE_WRITE;
   else if (ulShare == OF_SHARE_DENY_WRITE)
      ulShare = FILE_SHARE_READ;
   else if (ulShare == OF_SHARE_DENY_NONE)
      ulShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
   else if (ulShare == OF_SHARE_EXCLUSIVE)
      ulShare = 0; //no share
   else if (ulShare == OF_SHARE_COMPAT)
      ulShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
   else
   {
      dprintf(("KERNEL32: _lopen - warning incorrect value for arg2 (or incorrect implementation...)\n"));
      ulShare = 0;
   }

   hFile = CreateFileA(pszFileName,                 /* filename */
                       ulAccess,               /*  */
                       ulShare,                /*  */
                       NULL,                   /* ignore scurity */
                       OPEN_EXISTING,          /* open existing file, fail if new */
                       FILE_ATTRIBUTE_NORMAL,  /* normal attribs - no flags */ //m$ sets this to 0
                       NULL);                  /* no template */

   dprintf(("KERNEL32: _lopen returns %08xh.\n",
            hFile));

   // map correct return code for _lopen
   if (hFile != INVALID_HANDLE_VALUE)
     return hFile;
   else
     return HFILE_ERROR;
}


/*****************************************************************************
 * Name      : HFILE WIN32API _lread
 * Purpose   : forward call to ReadFile
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Depend on that ReadFile works properly.
 * Status    : Tested
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

UINT WIN32API _lread(HFILE arg1,
                     PVOID arg2,
                     UINT  arg3)
{
   ULONG rc;

//   dprintf(("KERNEL32: _lread(%08xh, %08xh, %08xh)\n",
//            arg1,
//            arg2,
//            arg3));

   if (!ReadFile(arg1,
                 arg2,
                 arg3,
                 &rc,
                 NULL))
      rc = -1;

//   dprintf(("KERNEL32: _lread returns %08xh.",
//            rc));

   return rc;
}


/*****************************************************************************
 * Name      : HFILE WIN32API _llseek
 * Purpose   : forward call to SetFilePointer
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Depends on SetFilePointer working
 * Status    : Tested.
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

LONG WIN32API _llseek(HFILE arg1,
                      LONG  arg2,
                      int   arg3)
{
   ULONG rc;

   dprintf(("KERNEL32: _llseek(%08xh, %08xh, %08xh)\n",
            arg1,
            arg2,
            arg3));
   if (!(arg3 == FILE_BEGIN   ||
         arg3 == FILE_CURRENT ||
         arg3 == FILE_END))
   {
      dprintf(("KERNEL32: _llseek - incorrect origin - fails.\n" ));
      return -1;
   }
  
   rc = SetFilePointer(arg1,
                       arg2,
                       NULL,
                       arg3);   
  //returns -1 on error (-1 == 0xffffffff)

   dprintf(("KERNEL32: _llseek returns %08xh", rc));

   return rc;
}


/*****************************************************************************
 * Name      : HFILE WIN32API _lwrite
 * Purpose   : forward call to WriteFile
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Depends on WriteFile working
 * Status    : Tested
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

UINT WIN32API _lwrite(HFILE hFile,
                      LPCSTR lpBuffer,
                      UINT  cbWrite)
{
   ULONG rc;
   dprintf(("KERNEL32: _lwrite(%08xh, %08xh, %08xh)\n", hFile, lpBuffer, cbWrite));

   if (!cbWrite)
       rc = SetEndOfFile(hFile) ? 0 : HFILE_ERROR;
   else
   {
       if (!WriteFile(hFile,
                      (PVOID)lpBuffer,
                      cbWrite,
                  &rc,
                  NULL))
          rc = HFILE_ERROR;
   }

   dprintf(("KERNEL32: _lwrite returns %08xh.\n", rc));
   return rc;
}

