/* $Id: misc.cpp,v 1.1 1999-06-23 22:17:52 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 * 1998/06/15 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Copyright 1997 Marcus Meissner
 * Copyright 1988 Patrick Haller (adapted for win32os2)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <shellapi.h>
#include "shell32.h"
#include <misc.h>
#include <string.h>


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/


/*****************************************************************************
 * Name      : LPWSTR* WIN32API CommandLineToArgvW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

LPWSTR * WIN32API CommandLineToArgvW(LPCWSTR lpCmdLine,
                                     int     *pNumArgs)
{
  LPWSTR *argv, s, t;
  int     i;

  dprintf(("SHELL32: CommandLineToArgvW(%s,%08xh)\n",
           lpCmdLine,
           pNumArgs));

  s = (LPWSTR)lpCmdLine;
  i = 0;
  while (*s) {
    /* space */
    if (*s==0x0020) {
        i++;
        s++;
        while (*s && *s==0x0020)
            s++;
        continue;
    }
    s++;
  }
  argv = (LPWSTR *)LocalAlloc(LPTR, sizeof(LPWSTR)*(i+1));
  s = t = (LPWSTR)lpCmdLine;
  i = 0;
  while(*s) {
    if (*s==0x0020) {
        *s=0;
        argv[i++] = t;
        *s=0x0020;
        while (*s && *s==0x0020)
            s++;
        if(*s)
            t=s+1;
        else    t=s;
        continue;
    }
    s++;
  }
  if(*t)
    argv[i++] = t;

  argv[i]=NULL;
  *pNumArgs = i;
  return argv;
}


/*****************************************************************************
 * Name      : HICON WIN32API ExtractIconA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

HICON WIN32API ExtractIconA(HINSTANCE hInst,
                            LPCSTR    lpszExeFileName,
                            UINT      nIconIndex)
{
  dprintf(("SHELL32: ExtractIconA(%08xh,%s,%08xh) not implemented.\n",
           hInst,
           lpszExeFileName,
           nIconIndex));

  return(NULL);
}


/*****************************************************************************
 * Name      : HICON WIN32API ExtractIconW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

HICON WIN32API ExtractIconW(HINSTANCE hInst,
                               LPCWSTR   lpszExeFileName,
                               UINT      nIconIndex)
{
  HICON hicon = NULL;
  char *astring = UnicodeToAsciiString((LPWSTR)lpszExeFileName);

  dprintf(("SHELL32: ExtractIconW(%08xh,%s,%08xh) not implemented.\n",
           hInst,
           lpszExeFileName,
           nIconIndex));


  FreeAsciiString(astring);
  return(hicon);
}


/*****************************************************************************
 * Name      : HICON WIN32API ExtractIconExA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

DWORD WIN32API ExtractIconExA(LPCSTR lpszFile,
                              int    nIconIndex,
                              HICON  *phiconLarge,
                              HICON  *phiconSmall,
                              UINT   nIcons)
{
  dprintf(("SHELL32: ExtractIconExA(%s,%08xh,%08xh,%08xh,%u) not implemented.\n",
           lpszFile,
           nIconIndex,
           phiconLarge,
           phiconSmall,
           nIcons));

  return (0);
}


/*****************************************************************************
 * Name      : HICON WIN32API ExtractIconExW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

HICON WIN32API ExtractIconExW(LPCWSTR lpszFile,
                              int    nIconIndex,
                              HICON  *phiconLarge,
                              HICON  *phiconSmall,
                              UINT   nIcons)
{
  dprintf(("SHELL32: ExtractIconExW(%s,%08xh,%08xh,%08xh,%u) not implemented.\n",
           lpszFile,
           nIconIndex,
           phiconLarge,
           phiconSmall,
           nIcons));

  return (0);
}


/*****************************************************************************
 * Name      : HINSTANCE WIN32API FindExecutableA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

HINSTANCE WIN32API FindExecutableA(LPCSTR lpszFile,
                                   LPCSTR lpszDir,
                                   LPTSTR lpszResult)
{
  dprintf(("SHELL32: FindExecutableA (%s,%s,%s) not implemented.\n",
           lpszFile,
           lpszDir,
           lpszResult));

  return(NULL);
}


/*****************************************************************************
 * Name      : HINSTANCE WIN32API FindExecutableW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

HINSTANCE WIN32API FindExecutableW(LPCWSTR lpszFile,
                                   LPCWSTR lpszDir,
                                   LPWSTR  lpszResult)
{
  dprintf(("SHELL32: FindExecutableW (%s,%s,%s) not implemented.\n",
           lpszFile,
           lpszDir,
           lpszResult));

  return(NULL);
}


/*****************************************************************************
 * Name      : HICON WIN32API ExtractAssociatedIconA
 * Purpose   : Return icon for given file (either from file itself or from associated
 *             executable) and patch parameters if needed.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL.36
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

HICON WIN32API ExtractAssociatedIconA(HINSTANCE hInst,
                                      LPSTR     lpIconPath,
                                      LPWORD    lpiIcon)
{
  HICON hIcon;

  dprintf(("SHELL32: ExtractAssociatedIconA(%08xh,%s,%08xh)\n",
           hInst,
           lpIconPath,
           lpiIcon));

  hIcon = ExtractIconA(hInst,
                       lpIconPath,
                       *lpiIcon);

  if( hIcon < 2 )
  {
    if( hIcon == 1 ) /* no icons found in given file */
    {
      char tempPath[0x80];
      UINT uRet = FindExecutableA(lpIconPath,
                                  NULL,
                                  tempPath);

      if(uRet > 32 && tempPath[0])
      {
        strcpy(lpIconPath,tempPath);
        hIcon = ExtractIconA(hInst,
                             lpIconPath,
                             *lpiIcon);
        if( hIcon > 2 )
          return hIcon;
      }
      else
        hIcon = 0;
     }

     if( hIcon == 1 )
       *lpiIcon = 2;   /* MSDOS icon - we found .exe but no icons in it */
     else
       *lpiIcon = 6;   /* generic icon - found nothing */

     /* @@@PH - how is this supposed to work ?
     GetModuleFileNameA(hInst,
                        lpIconPath,
                        0x80);
     hIcon = LoadIconA(hInst,
                       *lpiIcon);
      */
  }
  return hIcon;
}


/*****************************************************************************
 * Name      : HICON WIN32API ExtractAssociatedIconW
 * Purpose   : Return icon for given file (either from file itself or from associated
 *             executable) and patch parameters if needed.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL.36
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

HICON WIN32API ExtractAssociatedIconW(HINSTANCE hInst,
                                      LPWSTR    lpIconPath,
                                      LPWORD    lpiIcon)
{
  LPSTR lpIconPathAscii = UnicodeToAsciiString(lpIconPath);
  HICON hIcon;

  dprintf(("SHELL32: ExtractAssociatedIconW(%08xh,%s,%08xh)\n",
           hInst,
           lpIconPathAscii,
           lpiIcon));

  hIcon = ExtractAssociatedIconA(hInst,
                                 lpIconPathAscii,
                                 lpiIcon);

  FreeAsciiString(lpIconPathAscii);

  return (hIcon);
}

