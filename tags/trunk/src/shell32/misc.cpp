/* $Id: misc.cpp,v 1.4 1999-09-18 15:57:51 sandervl Exp $ */

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
#include <wchar.h>
#include <wcstr.h>
#include <winnls.h>
#include "shell32.h"
#include <misc.h>
#include <string.h>
#include "winbase.h"
#include <heapstring.h>

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
 * Name      : HICON WIN32API ExtractAssociatedIconExA
 * Purpose   : Return icon for given file (either from file itself or from associated
 *             executable) and patch parameters if needed.
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL.150
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 20:00]
 *****************************************************************************/

HICON WIN32API ExtractAssociatedIconExA(HINSTANCE hInst,
                                      LPSTR     lpIconPath,
                                      LPWORD    lpiIcon)
{
  dprintf(("SHELL32: undoc ExtractAssociatedIconExA\n"));

  return ExtractAssociatedIconA(hInst,lpIconPath,lpiIcon);
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

/*****************************************************************************
 * Name      : HICON WIN32API ExtractAssociatedIconExW
 * Purpose   : Return icon for given file (either from file itself or from associated
 *             executable) and patch parameters if needed.
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL.186
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 20:00]
 *****************************************************************************/

HICON WIN32API ExtractAssociatedIconExW(HINSTANCE hInst,
                                      LPWSTR     lpIconPath,
                                      LPWORD    lpiIcon)
{
  dprintf(("SHELL32: undoc ExtractAssociatedIconExW\n"));

  return ExtractAssociatedIconW(hInst,lpIconPath,lpiIcon);
}

/*****************************************************************************
 * Name      : ParseField
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.58
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API ParseFieldA(LPCSTR src,
                           DWORD  field,
                           LPSTR  dst,
                           DWORD  len)
{
  dprintf(("SHELL32: ParseFieldA(%s,%08xh,%08xh,%08xh) not correctly implemented.\n",
           src,
           field,
           dst,
           len));

  if (!src || !src[0] || !dst || !len)
    return 0;

  if (field >1)
  {
    field--;
    while (field)
    {
      if (*src==0x0) return FALSE;
      if (*src==',') field--;
      src++;
    }
  }

  while (*src!=0x00 && *src!=',' && len>0)
  {
    *dst=*src; dst++, src++; len--;
  }
  *dst=0x0;
  return TRUE;
}


/*****************************************************************************
 * Name      : PickIconDlg
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.62
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API PickIconDlg(DWORD x,DWORD y,DWORD z,DWORD a)
{
  dprintf(("SHELL32: PickIconDlg(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x,
           y,
           z,
           a));

  return 0xffffffff;
}


/*****************************************************************************
 * Name      : GetFileNameFromBrowse
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.63
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API GetFileNameFromBrowse(HWND   howner,
                                     LPSTR  targetbuf,
                                     DWORD  len,
                                     DWORD  x,
                                     LPCSTR suffix,
                                     LPCSTR y,
                                     LPCSTR cmd)
{
  dprintf(("SHELL32: GetFileNameFromBrowse(%08xh,  %08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           howner,
           targetbuf,
           len,
           x,
           suffix,
           y,
           cmd));

  /* puts up a Open Dialog and requests input into targetbuf */
  /* OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST|OFN_unknown */
  strcpy(targetbuf,
         "x:\\dummy.exe");

  return 1;
}


/*****************************************************************************
 * Name      : OleStrToStrN
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.78
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API OleStrToStrN (LPSTR   lpMulti,
                            INT     nMulti,
                            LPCWSTR lpWide,
                            INT     nWide)
{
  dprintf(("SHELL32: OleStrToStrN(%08xh,%08xh,%08xh,%08xh)\n",
           lpMulti,
           nMulti,
           lpWide,
           nWide));

  return WideCharToMultiByte (0,
                              0,
                              lpWide,
                              nWide,
                              lpMulti,
                              nMulti,
                              NULL,
                              NULL);
}


/*****************************************************************************
 * Name      : StrToOleStrN
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.79
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API StrToOleStrN (LPWSTR lpWide,
                            INT    nWide,
                            LPCSTR lpMulti,
                            INT    nMulti)
{
  dprintf(("SHELL32: StrToOleStrN(%08xh,%08xh,%08xh,%08xh)\n",
           lpWide,
           nWide,
           lpMulti,
           nMulti));

  return MultiByteToWideChar (0,
                              0,
                              lpMulti,
                              nMulti,
                              lpWide,
                              nWide);
}


/*****************************************************************************
 * Name      : RegisterShellHook
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.181
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *
 * PARAMS
 *      hwnd [I]  window handle
 *      y    [I]  flag ????
 *
 * NOTES
 *     exported by ordinal
 */

void WIN32API RegisterShellHook(HWND  hwnd,
                                DWORD y)
{
  dprintf(("SHELL32: RegisterShellHook(%08xh, %08xh) not implemented.\n",
           hwnd,
           y));
}


/*****************************************************************************
 * Name      : RunFileDlg
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.61
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API RunFileDlg (HWND  hwndOwner,
                           DWORD dwParam1,
                           DWORD dwParam2,
                           LPSTR lpszTitle,
                           LPSTR lpszPrompt,
                           UINT  uFlags)
{
  dprintf(("SHELL32: RunFileDlg(%08xh,%08xh,%08xh,%s,%s,%08xh) not implemented.\n",
           hwndOwner,
           dwParam1,
           dwParam2,
           lpszTitle,
           lpszPrompt,
           uFlags));

  return 0;
}


/*****************************************************************************
 * Name      : ExitWindowsDialog
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.60
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

void WIN32API ExitWindowsDialog (HWND hWndOwner)
{
  dprintf(("SHELL32: ExitWindowsDialog(%08xh)\n",
           hWndOwner));

   if (MessageBoxA(hWndOwner,
                   "Do you want to exit ODIN?",
                   "Shutdown",
                   MB_YESNO|MB_ICONQUESTION)
       == IDOK)
   {
     SendMessageA (hWndOwner,
                   WM_QUIT,
                   0,
                   0);
   }
}


/*****************************************************************************
 * Name      : ArrangeWindows
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.184
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API ArrangeWindows (DWORD dwParam1,
                               DWORD dwParam2,
                               DWORD dwParam3,
                               DWORD dwParam4,
                               DWORD dwParam5)
{
  dprintf(("SHELL32: ArrangeWindows(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           dwParam1,
           dwParam2,
           dwParam3,
           dwParam4,
           dwParam5));

  return 0;
}


/*****************************************************************************
 * Name      : SignalFileOpen
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.103
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SignalFileOpen (DWORD dwParam1)
{
  dprintf(("SHELL32: SignalFileOpen(%08xh) not implemented.\n",
           dwParam1));

  return 0;
}


/*****************************************************************************
 * Name      : DAD_ShowDrawImage
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.137
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API DAD_ShowDragImage (DWORD u)
{
  dprintf(("SHELL32: DAD_ShowDragImage(%08xh) not implemented.\n",
           u));

  return 0;
}


/*****************************************************************************
 * Name      : ReadCabinetState
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.651
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API ReadCabinetState(DWORD u,
                                  DWORD v)
{
  dprintf(("SHELL32: ReadCabinetState(%08xh, %08xh) not implemented.\n",
           u,
           v));

  return 0;
}


/*****************************************************************************
 * Name      : WriteCabinetState
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.652
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API WriteCabinetState(DWORD u)
{
  dprintf(("SHELL32: WriteCabinetState(%08xh) not implemented.\n",
           u));

  return 0;
}


/*****************************************************************************
 * Name      : FileIconInit
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.660
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API FileIconInit(BOOL bFullInit)
{
  dprintf(("SHELL32: FileIconInit(%08xh) not implemented.\n",
           bFullInit));

  return 0;
}


/*****************************************************************************
 * Name      : IsUserAdmin
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.680
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API IsUserAdmin(void)
{
  dprintf(("SHELL32: IsUserAdmin() not implmented. Yes, user is admin ;-)\n"));

  return TRUE;
}


/*****************************************************************************
 * Name      : StrRetToStrN
 * Purpose   : converts a STRRET to a normal string
 * Parameters: the pidl is for STRRET OFFSET
 * Variables :
 * Result    :
 * Remark    : SHELL32.96
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API StrRetToStrN (LPVOID       dest,
                               DWORD        len,
                               LPSTRRET     src,
                               LPITEMIDLIST pidl)
{
  dprintf(("SHELL32: StrRetToStrN(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           dest,
           len,
           src,
           pidl));

  switch (src->uType)
  {
    case STRRET_WSTR:
      WideCharToMultiByte(CP_ACP,
                          0,
                          src->u.pOleStr,
                          -1,
                          (LPSTR)dest,
                          len,
                          NULL,
                          NULL);
      SHFree(src->u.pOleStr);
      break;

    case STRRET_CSTRA:
      //if (VERSION_OsIsUnicode())
      //  lstrcpynAtoW((LPWSTR)dest, src->u.cStr, len);
      //else
        strncpy((LPSTR)dest,
                src->u.cStr,
                len);
      break;

    case STRRET_OFFSETA:
      if (pidl)
      {
        //if(VERSION_OsIsUnicode())
        //  lstrcpynAtoW((LPWSTR)dest, ((LPCSTR)&pidl->mkid)+src->u.uOffset, len);
        //else
          strncpy((LPSTR)dest,
                  ((LPCSTR)&pidl->mkid)+src->u.uOffset,
                  len);
        break;
      }

    default:
      dprintf(("SHELL32: StrRetToStrN: unknown type!\n"));

      if (len)
        *(LPSTR)dest = '\0';

      return(FALSE);
  }
  return(TRUE);
}

/*****************************************************************************
 * Name      : StrChrW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.651
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

LPWSTR WIN32API StrChrW (LPWSTR str,
                         WCHAR  x)
{
  dprintf (("SHELL32: StrChrW(%08xh, %08xh)\n",
            str,
            x));

  return ((LPWSTR)wcschr((const wchar_t*)str,
                         x));
}


/*****************************************************************************
 * Name      : StrChrNIW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.?
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

INT WIN32API StrCmpNIW (LPWSTR wstr1,
                        LPWSTR wstr2,
                        INT    len)
{
  dprintf(("SHELL32: StrCmpNIW(%08xh,%08xh,%08xh) not correctly implemented.\n",
           wstr1,
           wstr2,
           len));

  return (wcsncmp((const wchar_t*)wstr1,
                  (const wchar_t*)wstr2,
                  len));
}


/*****************************************************************************
 * Name      : DriveType
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.64
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API DriveType(DWORD u)
{
  dprintf(("SHELL32: DriveType(%08xh) not implemented.\n",
           u));

  return 0;
}


/*****************************************************************************
 * Name      : StrRChrW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.320, wcsrchr does not work?
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

LPWSTR WIN32API StrRChrW(LPWSTR lpStart,
                         LPWSTR lpEnd,
                         DWORD  wMatch)
{
  LPWSTR wptr=NULL;

  dprintf(("SHELL32: StrRChrW(%08xh,%08xh,%08xh)\n",
           lpStart,
           lpEnd,
           wMatch));

  /* if the end not given, search*/
  if (!lpEnd)
  {
    lpEnd=lpStart;
    while (*lpEnd)
      lpEnd++;
  }

  do
  {
    if (*lpStart==(WCHAR)wMatch)
      wptr = lpStart;
    lpStart++;
  }
  while ( lpStart<=lpEnd );
  return wptr;
}


/*****************************************************************************
 * Name      : Control_FillCache_RunDLL
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.8
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API Control_FillCache_RunDLL(HWND   hWnd,
                                          HANDLE hModule,
                                          DWORD  w,
                                          DWORD  x)
{
  dprintf(("SHELL32: Control_FillCache_RunDLL(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hWnd,
           hModule,
           w,
           x));

  return 0;
}


/*****************************************************************************
 * Name      : RunDLL_CallEntry16
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.122 - the name is propably wrong
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API RunDLL_CallEntry16(DWORD v,
                                    DWORD w,
                                    DWORD x,
                                    DWORD y,
                                    DWORD z)
{
  dprintf(("SHELL32: RunDLL_CallEntry16(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           v,
           w,
           x,
           y,
           z));

  return 0;
}


/*****************************************************************************
 * Name      : SHELL32_654
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.654
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API SHELL32_654 (DWORD x,
                              DWORD y)
{
  dprintf(("SHELL32: SHELL32_654(%08xh,%08xh) not implemented.\n",
           x,
           y));
  return 0;
}


/*****************************************************************************
 * Name      : RLBuildListOfPaths
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.146 - builds a DPA
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API RLBuildListOfPaths ()
{
  dprintf(("SHELL32: RLBuildListOfPaths() not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      : StrToOleStr
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.163
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

int WIN32API StrToOleStr (LPWSTR lpWideCharStr,
                          LPCSTR lpMultiByteString)
{
  dprintf(("SHELL32: StrToOleStr(%08xh,%08xh)\n",
           lpWideCharStr,
           lpMultiByteString));

  return MultiByteToWideChar(0,
                             0,
                             lpMultiByteString,
                             -1,
                             lpWideCharStr,
                             MAX_PATH);
}


/*****************************************************************************
 * Name      : SetAppStartingCursor
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.99
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

HRESULT WIN32API SetAppStartingCursor(HWND  u,
                                      DWORD v)
{
  dprintf(("SHELL32: SetAppStartingCursor (SHELL32.99): (%08xh,%08xh) not implemented.\n",
           u,
           v));

  return 0;
}

/*****************************************************************************
 * Name      : StrCpyNW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.310 - used by winfile.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

LPWSTR WIN32API StrCpyNW(LPWSTR lpString1,LPWSTR lpString2,int iMaxLength)
{
  dprintf(("SHELL32: undoc StrCpyNW\n"));

  return lstrcpynW(lpString1,lpString2,iMaxLength);
}

/*****************************************************************************
 * Name      : StrNCpyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.316 - used by winfile.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

LPWSTR WIN32API StrNCpyW(LPWSTR lpString1,LPWSTR lpString2)
{
  dprintf(("SHELL32: undoc StrNCpyW\n"));

  return lstrcpyW(lpString1,lpString2);
}

/*****************************************************************************
 * Name      : FreeIconList
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.220 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API FreeIconList(DWORD dw)
{
  dprintf(("SHELL32: undoc FreeIconList %08xh\n",dw));
}

/*****************************************************************************
 * Name      : InternalExtractIconListA
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.221 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API InternalExtractIconListA(VOID)
{
  dprintf(("SHELL32: undoc InternalExtractIconListA \n"));
}

/*****************************************************************************
 * Name      : InternalExtractIconListW
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.222 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API InternalExtractIconListW(VOID)
{
  dprintf(("SHELL32: undoc InternalExtractIconListW\n"));
}

/*****************************************************************************
 * Name      : ExtractIconResInfoA
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.192 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API ExtractIconResInfoA(VOID)
{
  dprintf(("SHELL32: undoc ExtractIconResInfoA\n"));
}

/*****************************************************************************
 * Name      : ExtractIconResInfoW
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.193 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API ExtractIconResInfoW(VOID)
{
  dprintf(("SHELL32: undoc ExtractIconResInfoW\n"));
}

/*************************************************************************
 *				FindEnvironmentString	[SHELL.38]
 *
 * Returns a pointer into the DOS environment... Ugh.
 */
LPSTR SHELL_FindStringA(LPSTR lpEnv, LPCSTR entry)
{ UINT16 l;

  //TRACE_(shell)("\n");

  l = strlen(entry);
  for( ; *lpEnv ; lpEnv+=strlen(lpEnv)+1 )
  { if( lstrncmpiA(lpEnv, entry, l) )
      continue;
	if( !*(lpEnv+l) )
	    return (lpEnv + l); 		/* empty entry */
	else if ( *(lpEnv+l)== '=' )
	    return (lpEnv + l + 1);
    }
    return NULL;
}

/*****************************************************************************
 * Name      : DoEnvironmentSubstA
 * Purpose   :
 * Parameters: ??? ported WINE 16 bit function
 * Variables :
 * Result    :
 * Remark    : SHELL32.44 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

DWORD WIN32API DoEnvironmentSubstA(LPSTR str,DWORD length)
{
  LPSTR   lpEnv = (LPSTR)GetEnvironmentStringsA();
  LPSTR   lpBuffer = (LPSTR)HeapAlloc( GetProcessHeap(), 0, length);
  LPSTR   lpstr = str;
  LPSTR   lpbstr = lpBuffer;

  dprintf(("SHELL32: undoc DoEnvironmentSubstA\n"));

  CharToOemA(str,str);

  while( *lpstr && lpbstr - lpBuffer < length )
   {
     LPSTR lpend = lpstr;

     if( *lpstr == '%' )
       {
	  do { lpend++; } while( *lpend && *lpend != '%' );
	  if( *lpend == '%' && lpend - lpstr > 1 )	/* found key */
	    {
	       LPSTR lpKey;
	      *lpend = '\0';
	       lpKey = SHELL_FindStringA(lpEnv, lpstr+1);
	       if( lpKey )				/* found key value */
		 {
		   int l = strlen(lpKey);

		   if( l > length - (lpbstr - lpBuffer) - 1 )
		     {
           //WARN_(shell)("-- Env subst aborted - string too short\n");
		      *lpend = '%';
		       break;
		     }
		   strcpy(lpbstr, lpKey);
		   lpbstr += l;
		 }
	       else break;
	      *lpend = '%';
	       lpstr = lpend + 1;
	    }
	  else break;					/* back off and whine */

	  continue;
       }

     *lpbstr++ = *lpstr++;
   }

 *lpbstr = '\0';
  if( lpstr - str == strlen(str) )
    {
      strncpy(str, lpBuffer, length);
      length = 1;
    }
  else
      length = 0;

  //TRACE_(shell)("-- return %s\n", str);

  OemToCharA(str,str);
  HeapFree( GetProcessHeap(), 0, lpBuffer);

  FreeEnvironmentStringsA(lpEnv);

  /*  Return str length in the LOWORD
   *  and 1 in HIWORD if subst was successful.
   */

 return (DWORD)MAKELONG(strlen(str), length);
}

/*****************************************************************************
 * Name      : DoEnvironmentSubstW
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.193 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

DWORD WIN32API DoEnvironmentSubstW(LPWSTR str,DWORD length)
{
  //CB: call DoEnvironmentSubstW
  dprintf(("SHELL32: undoc DoEnvironmentSubstW\n"));

  return 0;
}

/*****************************************************************************
 * Name      : CheckEscapesA
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.3 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API CheckEscapesA(DWORD x1,DWORD x2)
{
  dprintf(("SHELL32: undoc CheckEscapesA\n"));
}

/*****************************************************************************
 * Name      : CheckEscapesW
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.6 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API CheckEscapesW(DWORD x1,DWORD x2)
{
  dprintf(("SHELL32: undoc CheckEscapesW\n"));
}

/*****************************************************************************
 * Name      : DuplicateIcon
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.138 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

DWORD WIN32API DuplicateIcon(DWORD x1)
{
  dprintf(("SHELL32: undoc DuplicateIcon\n"));

  return 0;
}
