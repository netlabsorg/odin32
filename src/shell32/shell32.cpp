/* $Id: shell32.cpp,v 1.1 1999-05-24 20:19:57 ktk Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * 1998/06/15 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) shell32.c       1.0.0   1998/06/15 PH Merge WINE/SHELLORD.C
 *
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1988 Patrick Haller (adapted for win32os2)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <shellapi.h>
#include <winreg.h>
#include "shell32.h"

#include <stdarg.h>
//#include <builtin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <nameid.h>
#include <unicode.h>


//static const char*    lpstrMsgWndCreated = "OTHERWINDOWCREATED";
//static const char*    lpstrMsgWndDestroyed = "OTHERWINDOWDESTROYED";
//static const char*    lpstrMsgShellActivate = "ACTIVATESHELLWINDOW";

//static HWND   SHELL_hWnd = 0;
//static HHOOK  SHELL_hHook = 0;
//static USHORT uMsgWndCreated = 0;
//static USHORT uMsgWndDestroyed = 0;
//static USHORT uMsgShellActivate = 0;



/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

//******************************************************************************
//******************************************************************************
VOID WIN32API DragAcceptFiles(HWND hwnd,
                                 BOOL fAccept)
{
  dprintf(("SHELL32: DragAcceptFiles (%0uxh, %08xh) not implemented.\n",
           hwnd,
           fAccept));
}
//******************************************************************************
//******************************************************************************
VOID WIN32API DragFinish(HDROP hDrop)
{
  dprintf(("SHELL32: DragFinish(%08xh) not implemented.\n",
           hDrop));
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DragQueryFileA(HDROP  hDrop,
                             UINT   iFile,
                             LPTSTR lpszFile,
                             UINT   cch)
{
  dprintf(("SHELL32: DragQueryFileA(%08xh,%08xh,%s,%08xh) not implemented.\n",
           hDrop,
           iFile,
           lpszFile,
           cch));

  return(0);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DragQueryFileAorW(HDROP  hDrop,
                                UINT   iFile,
                                LPTSTR lpszFile,
                                UINT   cch)
{
  dprintf(("SHELL32: DragQueryFileAorW(%08xh,%08xh,%s,%08xh) not implemented.\n",
           hDrop,
           iFile,
           lpszFile,
           cch));

  return(0);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DragQueryFileW(HDROP  hDrop,
                                UINT   iFile,
                                LPWSTR lpszFile,
                                UINT   cch)
{
  dprintf(("SHELL32: DragQueryFileW(%08xh,%08xh,%s,%08xh) not implemented.\n",
           hDrop,
           iFile,
           lpszFile,
           cch));

  return(0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DragQueryPoint(HDROP   hDrop,
                                LPPOINT lppt)
{
  dprintf(("SHELL32: DragQueryPoint (%08xh,%08xh) not implemented.\n",
           hDrop,
           lppt));

  return(FALSE);
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
HINSTANCE WIN32API ShellExecuteA(HWND    hwnd,
                                 LPCTSTR lpOperation,
                                 LPCTSTR lpFile,
                                 LPCTSTR lpParameters,
                                 LPCTSTR lpDirectory,
                                 INT     nShowCmd)
{
  dprintf (("SHELL32: ShellExecuteA(%08xh,%s,%s,%s,%s,%08xh) not implemented.\n",
            hwnd,
            lpOperation,
            lpFile,
            lpParameters,
            lpDirectory,
            nShowCmd));

  return(0); //out of memory
}
//******************************************************************************
//TODO: Make nice dialog window
//******************************************************************************
int WIN32API ShellAboutA(HWND    hwnd,
                         LPCTSTR szApp,
                         LPCTSTR szOtherStuff,
                         HICON   hIcon)
{
  dprintf(("SHELL32: ShellAboutA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hwnd,
           szApp,
           szOtherStuff,
           hIcon));

  /* @@@PH 98/06/07 */
  if (szOtherStuff == NULL) szOtherStuff = "";
  if (szApp        == NULL) szApp        = "";

  MessageBoxA(NULL,
              szOtherStuff,
              szApp,
              MB_OK);                             /*PLF Sun  97-11-23 22:58:49*/

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
int WIN32API ShellAboutW(HWND   hwnd,
                         LPCWSTR szApp,
                         LPCWSTR szOtherStuff,
                         HICON  hIcon)
{
  dprintf(("SHELL32: ShellAboutW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hwnd,
           szApp,
           szOtherStuff,
           hIcon));

  return(TRUE);
}
//******************************************************************************
//Borrowed from Wine
//******************************************************************************
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
//******************************************************************************
//******************************************************************************
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
//******************************************************************************
//******************************************************************************
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
 * Name      : UINT WIN32API ExtractIconExA
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
 * Name      : UINT WIN32API ExtractIconExW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sat, 1998/07/11 11:55]
 *****************************************************************************/

DWORD WIN32API ExtractIconExW(LPCWSTR lpszFile,
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


//******************************************************************************
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
//******************************************************************************
//******************************************************************************
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
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SHGetMalloc(LPMALLOC *ppMalloc)
{
  dprintf(("SHELL32: SHGetMalloc(%08xh) not implemented.\n",
           ppMalloc));

  return(0);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SHGetFileInfoA(LPCSTR          pszPath,
                                 DWORD           dwFileAttributes,
                                 SHFILEINFOA     *psfi,
                                 UINT            cbFileInfo,
                                 UINT            uFlags)
{
  dprintf(("SHELL32: SHGetFileInfoA (%s,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pszPath,
           dwFileAttributes,
           psfi,
           cbFileInfo,
           uFlags));

  return(0);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SHGetFileInfoW(LPCWSTR         pszPath,
                                 DWORD           dwFileAttributes,
                                 SHFILEINFOW     *psfi,
                                 UINT            cbFileInfo,
                                 UINT            uFlags)
{
  char *astring = UnicodeToAsciiString((LPWSTR)pszPath);

  dprintf(("SHELL32: SHGetFileInfoW (%s,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           astring,
           dwFileAttributes,
           psfi,
           cbFileInfo,
           uFlags));

  FreeAsciiString(astring);
  return(0);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SHGetDesktopFolder(LPSHELLFOLDER *ppshf)
{
  dprintf(("SHELL32: SHGetDesktopFolder(%08xh) not implemented.\n",
           ppshf));

  return(1);
}
//******************************************************************************
//******************************************************************************
void WIN32API SHAddToRecentDocs(UINT uFlags, LPCVOID pv)
{
  dprintf(("SHELL32: SHAddToRecentDocs(%08xh,%08xh) not implemented.\n",
           uFlags,
           pv));
}
//******************************************************************************
//******************************************************************************
void WIN32API SHChangeNotify(LONG    wEventId,
                                UINT    uFlags,
                                LPCVOID dwItem1,
                                LPCVOID dwItem2)
{
  dprintf(("SHELL32: SHChangeNotify(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           wEventId,
           uFlags,
           dwItem1,
           dwItem2));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API SHFileOperationA(LPSHFILEOPSTRUCTA lpFileOp)
{
  dprintf(("SHELL32: SHFileOperationA(%08xh) not implemented.\n",
           lpFileOp));

  return(0);
}
//******************************************************************************

//******************************************************************************
DWORD WIN32API SHFileOperationW(LPSHFILEOPSTRUCTW lpFileOp)
{
  dprintf(("SHELL32: SHFileOperationW(%08xh) not implemented.\n",
           lpFileOp));

  return(0);
}
//******************************************************************************

//******************************************************************************
BOOL WIN32API SHGetPathFromIDListA(LPCITEMIDLIST pidl,
                                      LPSTR         pszPath)
{
  dprintf(("SHELL32: SHGetPathFromIDListA(%08xh,%s) not implemented.\n",
           pidl,
           pszPath));

  return(FALSE);
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API SHGetSpecialFolderLocation(HWND         hwndOwner,
                                               int          nFolder,
                                               LPITEMIDLIST * ppidl)
{
  dprintf(("SHELL32: SHGetSpecialFolderLocation(%08xh,%08xh,%08xh) not implemented.\n",
           hwndOwner,
           nFolder,
           ppidl));

  return(1);
}
//******************************************************************************
//******************************************************************************
LPITEMIDLIST WIN32API SHBrowseForFolder(LPBROWSEINFOA lpbi)
{
  dprintf(("SHELL32: SHBrowseForFolder(%08xh) not implemented.\n",
           lpbi));

  return(NULL);
}
//******************************************************************************


/*************************************************************************
 *              Shell_NotifyIcon    [SHELL32.240]
 *  FIXME
 *  This function is supposed to deal with the systray.
 *  Any ideas on how this is to be implimented?
 */
BOOL WIN32API Shell_NotifyIconA(DWORD dwMessage,
	                        PNOTIFYICONDATAA pnid )
{
  dprintf(("SHELL32: Shell_NotifyIconA(%08xh,%08xh) not implemented.\n",
           dwMessage,
           pnid));

  return FALSE;
}


/*************************************************************************
 *              Shell_NotifyIcon    [SHELL32.240]
 *  FIXME
 *  This function is supposed to deal with the systray.
 *  Any ideas on how this is to be implimented?
 */
BOOL WIN32API Shell_NotifyIconW(DWORD dwMessage,
                           	PNOTIFYICONDATAW pnid )
{
  dprintf(("SHELL32: Shell_NotifyIconW(%08xh,%08xh) not implemented.\n",
           dwMessage,
           pnid));

  return FALSE;
}


/*****************************************************************************
 *
 * Code merged from WINE
 *
 * Patrick Haller 98/06/15
 *
 *****************************************************************************/


/*************************************************************************
 *           SHELL32_2              [SHELL32.2]
 */
DWORD WIN32API SHELL32_2(HWND hwnd,
                            DWORD x2,
                            DWORD x3,
                            DWORD x4,
                            DWORD x5,
                            DWORD x6)
{
  dprintf(("SHELL32: .2 (0x%04x,0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx) not implemented.\n",
           hwnd,
           x2,
           x3,
           x4,
           x5,
           x6));
  return 0;
}


/*************************************************************************
 *           SHELL32_16             [SHELL32.16]
 * find_lastitem_in_itemidlist()
 */
LPSHITEMID WIN32API SHELL32_16(LPITEMIDLIST iil)
{
  LPSHITEMID lastsii,
             sii;

  dprintf(("SHELL32: .16 (%08xh)\n",
           iil));

  if (!iil)
    return NULL;

  sii = &(iil->mkid);
  lastsii = sii;

  while (sii->cb)
  {
    lastsii = sii;
    sii = (LPSHITEMID)(((char*)sii)+sii->cb);
  }

  return lastsii;
}


/*************************************************************************
 *           SHELL32_29             [SHELL32.29]
 * is_rootdir(const char*path)
 */
BOOL WIN32API SHELL32_29(LPCSTR x)
{
  dprintf(("SHELL32: .29 (%s)\n",
           x));


  if (!lstrcmpA(x+1,":\\"))      /* "X:\" */
    return 1;

  if (!lstrcmpA(x,"\\"))     /* "\" */
    return 1;

  if (x[0]=='\\' && x[1]=='\\')
  {     /* UNC "\\<xx>\" */
    int foundbackslash = 0;

    x=x+2;
    while (*x)
    {
      if (*x++=='\\')
        foundbackslash++;
    }

    if (foundbackslash<=1)  /* max 1 \ more ... */
      return 1;
  }
  return 0;
}


/*************************************************************************
 *           SHELL32_30             [SHELL32.30]
 * get_rootdir(char*path,int drive)
 */
LPSTR WIN32API SHELL32_30(LPSTR root,
                             BYTE drive)
{
  dprintf(("SHELL32: .30 (%s, %u)\n",
           root,
           drive));


  strcpy(root,
         "A:\\");
  root[0]+=drive;

  return root;
}


/*************************************************************************
 *                  SHELL32_31      [SHELL32.31]
 * returns pointer to last . in last pathcomponent or at \0.
 */
LPSTR WIN32API SHELL32_31(LPSTR path)
{
  LPSTR   lastpoint = NULL;

  dprintf(("SHELL32: .31 (%s)\n",
           path));


  while (*path)
  {
    if (*path=='\\'||*path==' ')
      lastpoint=NULL;

    if (*path=='.')
      lastpoint=path;

    path++;
  }

  return lastpoint ? lastpoint : path;
}


/*************************************************************************
 *              SHELL32_32  [SHELL32.32]
 * append \ if there is none
 */
LPSTR WIN32API SHELL32_32(LPSTR path)
{
  int len;

  dprintf(("SHELL32: .31 (%s)\n",
           path));


  len = lstrlenA(path);

  if (len && path[len-1]!='\\')
  {
    path[len+0]='\\';
    path[len+1]='\0';
    return path+len+1;
  }
  else
    return path+len;
}


/*************************************************************************
 *              SHELL32_33      [SHELL32.33]
 * remove spaces from beginning and end of passed string
 */
LPSTR WIN32API SHELL32_33(LPSTR str)
{
  LPSTR x = str;

  dprintf(("SHELL32: .33 (%s)\n",
           str));


  while (*x==' ')
    x++;

  if (x!=str)
    lstrcpyA(str,
               x);

  if (!*str)
    return str;

  x=str+strlen(str)-1;

  while (*x==' ')
    x--;

  if (*x==' ')
    *x='\0';

  return x;
}


/*************************************************************************
 *              SHELL32_34  [SHELL32.34]
 * basename(char *fn);
 */
LPSTR WIN32API SHELL32_34(LPSTR fn)
{
  LPSTR basefn;

  dprintf(("SHELL32: .34 (%s)\n",
           fn));


  basefn = fn;
  while (fn[0])
  {
    if (( (fn[0]=='\\') ||
          (fn[0]==':')) && fn[1] && fn[1]!='\\')
      basefn = fn+1;

    fn++;
  }

  return basefn;
}

/*************************************************************************
 *           SHELL32_35             [SHELL32.35]
 * bool getpath(char *pathname); truncates passed argument to a valid path
 * returns if the string was modified or not.
 * "\foo\xx\foo"-> "\foo\xx"
 * "\"      -> "\"
 * "a:\foo" -> "a:\"
 */
DWORD WIN32API SHELL32_35(LPSTR fn)
{
  LPSTR x,cutplace;

  dprintf(("SHELL32: .35 (%s)\n",
           fn));


    if (!fn[0])
        return 0;
    x=fn;
    cutplace = fn;
    while (*x) {
        if (*x=='\\') {
            cutplace=x++;
            continue;
        }
        if (*x==':') {
            x++;
            if (*x=='\\')
                cutplace=++x;
            continue; /* already x++ed */
        }
        x++;
    }
    if (!*cutplace)
        return 0;
    if (cutplace==fn) {
        if (fn[0]=='\\') {
            if (!fn[1])
                return 0;
            fn[0]='\0';
            return 1;
        }
    }
    *cutplace='\0';
    return 1;
}

/*************************************************************************
 *              SHELL32_37  [SHELL32.37]
 * concat_paths(char*target,const char*add);
 * concats "target\\add" and writes them to target
 */
LPSTR WIN32API SHELL32_37(LPSTR target,
                             LPSTR x1,
                             LPSTR x2)
{
  char  buf[260];

  dprintf(("SHELL32: .37 (%s, %s, %s)\n",
           target,
           x1,
           x2));


  if (!x2 || !x2[0])
  {
    lstrcpyA(target,x1);

    return target;
  }

  lstrcpyA(buf,x1);
  SHELL32_32(buf); /* append \ if not there */
  lstrcatA(buf,x2);
  lstrcpyA(target,buf);

  return target;
}

/*************************************************************************
 *              SHELL32_36  [SHELL32.36]
 * concat_paths(char*target,const char*add);
 * concats "target\\add" and writes them to target
 */
LPSTR WIN32API SHELL32_36(LPSTR x1,
                             LPSTR x2)
{
  dprintf(("SHELL32: .36 (%s, %s)\n",
           x1,
           x2));


  while (x2[0]=='\\')
    x2++;

  return SHELL32_37(x1,
                       x1,
                       x2);
}

/*************************************************************************
 *              SHELL32_39  [SHELL32.39]
 * isUNC(const char*path);
 */
BOOL WIN32API SHELL32_39(LPCSTR path)
{
  dprintf (("SHELL32: .39 (%s)\n",
            path));


  if ((path[0]=='\\') &&
      (path[1]=='\\'))
    return TRUE;

  return FALSE;
}


/*************************************************************************
 *              SHELL32_45  [SHELL32.45]
 * file_exists(char *fn);
 */
BOOL WIN32API SHELL32_45(LPSTR fn)
{
  dprintf(("SHELL32: .45 (%s)\n",
           fn));


  if (GetFileAttributesA(fn)==-1)
    return FALSE;
  else
    return TRUE;
}

/*************************************************************************
 *              SHELL32_52  [SHELL32.52]
 * look for next arg in string. handle "quoted" strings
 * returns pointer to argument *AFTER* the space. Or to the \0.
 */
LPSTR WIN32API SHELL32_52(LPSTR cmdline)
{
  BOOL  qflag = FALSE;

  dprintf (("SHELL32: .52 (%s)\n",
            cmdline));


  while (*cmdline)
  {
    if ((*cmdline==' ') && !qflag)
      return cmdline+1;

    if (*cmdline=='"')
      qflag=!qflag;

    cmdline++;
  }
  return cmdline;
}


/*************************************************************************
 *              SHELL32_56      [SHELL32.56]
 * unquote string (remove ")
 */
VOID WIN32API SHELL32_56(LPSTR str)
{
  DWORD      len = lstrlenA(str);

  dprintf (("SHELL32: .56 (%s)\n",
            str));


  if (*str!='"')
    return;

  if (str[len-1]!='"')
    return;

  str[len-1]='\0';
  lstrcpyA(str,
             str+1);
  return;
}


/*************************************************************************
 *           SHELL32_58             [SHELL32.58]
 */
DWORD WIN32API SHELL32_58(LPCSTR src,
                             DWORD x2,
                             LPSTR target,
                             DWORD pathlen)
{
  dprintf (("SHELL32: .58 (%s, %08xh, %08xh, %08xh) not implemented\n",
            src,
            x2,
            target,
            pathlen));

  return 0;
}


/*************************************************************************
 *           SHELL32_62             [SHELL32.62]
 */
DWORD WIN32API SHELL32_62(DWORD x,
                             DWORD y,
                             DWORD z,
                             DWORD a)
{
  dprintf(("SHELL32: .62 (%08xh, %08xh, %08xh, %08xh) not implemented.\n",
           x,
           y,
           z,
           a));

  return 0xffffffff;
}


/*************************************************************************
 *                      SHELL32_63                     [SHELL32.63]
 */
DWORD WIN32API SHELL32_63(HWND   howner,
                             LPSTR  targetbuf,
                             DWORD  len,
                             DWORD  x,
                             LPCSTR suffix,
                             LPCSTR y,
                             LPCSTR cmd)
{
  dprintf (("SHELL32: .63 (%08xh, %08x, %08xh, %08xh, %08xh, %08xh, %s) not implemented.\n",
            howner,
            targetbuf,
            len,
            x,
            suffix,
            y,
            cmd));

  /* puts up a Open Dialog and requests input into targetbuf */
  /* OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST|OFN_unknown */
  lstrcpyA(targetbuf,"x:\\s3.exe");
  return 1;
}


/*************************************************************************
 *                      SHELL32_68                     [SHELL32.68]
 */
DWORD WIN32API SHELL32_68(DWORD x,
                             DWORD y,
                             DWORD z)
{
  dprintf(("SHELL32: .68 (%08xh, %08xh, %08xh) not implemented.\n",
           x,
           y,
           z));

  return 0;
}


/*************************************************************************
 *           SHELL32_71             [SHELL32.71]
 * returns internal shell values in the passed pointers
 */
BOOL WIN32API SHELL32_71(LPDWORD x,
                            LPDWORD y)
{
  dprintf(("SHELL32: .71 (%08xh, %08xh) not implemented.\n",
           x,
           y));

  return TRUE;
}


/*************************************************************************
 *           SHELL32_72             [SHELL32.72]
 * dunno. something with icons
 */
void WIN32API SHELL32_72(LPSTR x,
                            DWORD y,
                            DWORD z)
{
  dprintf (("SHELL32: .27 (%08xh, %08xh, %08xh) not implemented.\n",
            x,
            y,
            z));
}


/*************************************************************************
 *           SHELL32_89             [SHELL32.89]
 */
DWORD WIN32API SHELL32_89(DWORD x1,
                             DWORD x2,
                             DWORD x3)
{
  dprintf(("SHELL32: .89 (%08xh, %08xh, %08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return 0;
}


/*************************************************************************
 *              SHELL32_119 [SHELL32.119]
 * unknown
 */
void WIN32API SHELL32_119(LPVOID x)
{
  dprintf(("SHELL32: .119 (%08xh) not implemented.\n",
           x));
}


/*************************************************************************
 *              SHELL32_175 [SHELL32.175]
 * unknown
 */
void WIN32API SHELL32_175(DWORD x1,
                             DWORD x2,
                             DWORD x3,
                             DWORD x4)
{
  dprintf (("SHELL32: .175 (%08x, %08xh, %08xh, %08xh not implemented.\n",
            x1,
            x2,
            x3,
            x4));
}


/*************************************************************************
 *              SHELL32_181 [SHELL32.181]
 * unknown
 */
void WIN32API SHELL32_181(DWORD x,
                             DWORD y)
{
  dprintf(("SHELL32: .181 (%08xh, %08xh) not implemented.\n",
           x,
           y));
}


/*************************************************************************
 *              SHELL32_75  [SHELL32.75]
 * unknown
 */
BOOL WIN32API SHELL32_75(LPDWORD x,
                            LPDWORD y)
{
  dprintf(("SHELL32: .75 (%08xh, %08xh) not implemented.\n",
           x,
           y));

  return TRUE;
}


/*************************************************************************
 *           SHELL32_77             [SHELL32.77]
 */
DWORD WIN32API SHELL32_77(DWORD x,
                             DWORD y,
                             DWORD z)
{
  dprintf(("SHELL32: .77 (%08xh, %08xh, %08xh) not implemented.\n",
           x,
           y,
           z));

  return 0;
}


/*************************************************************************
 *           SHELL32_79             [SHELL32.79]
 * create_directory_and_notify(...)
 */
DWORD WIN32API SHELL32_79(LPCSTR dir,
                             LPVOID xvoid)
{
  dprintf(("SHELL32: .79 (%s, %08xh)\n",
           dir,
           xvoid));

  if (!CreateDirectoryA(dir,
                          (PSECURITY_ATTRIBUTES)xvoid))
    return FALSE;

    /* @@@PH: SHChangeNotify(8,1,dir,0); */
  return TRUE;
}


/*************************************************************************
 *           SHELL32_183            [SHELL32.183]
 * Format and output errormessage.
 */
//void __cdecl SHELL32_183(HMODULE hmod,
// CDECL ? really ?
void WIN32API SHELL32_183(HMODULE hmod,
                             HWND    hwnd,
                             DWORD   id,
                             DWORD   x,
                             DWORD   type,
                             LPVOID  arglist)
{
  char   buf[100],
         buf2[100],
         *buf3;
  LPVOID args = &arglist;

  dprintf(("SHELL32: .183 (%08xh, %08xh, %08xh, %08xh, %08xh, %08xh) not implemented.\n",
           hmod,
           hwnd,
           id,
           x,
           type,
           arglist));

  if (!LoadStringA(hmod,
                     x,
                     buf,
                     100))
    strcpy(buf,
           "Desktop");

  LoadStringA(hmod,
                id,
                buf2,
                100);

  /* FIXME: the varargs handling doesn't. */
  FormatMessageA(0x500,
                   buf2,
                   0,
                   0,
                   (LPSTR)&buf3,
                   256,
                   (LPDWORD)&args);

  MessageBoxA(hwnd,
                buf3,
                buf,
                id|0x10000);
}


/*************************************************************************
 *           SHELL32_100            [SHELL32.100]
 * walks through policy table, queries <app> key, <type> value, returns
 * queried (DWORD) value.
 * {0x00001,Explorer,NoRun}
 * {0x00002,Explorer,NoClose}
 * {0x00004,Explorer,NoSaveSettings}
 * {0x00008,Explorer,NoFileMenu}
 * {0x00010,Explorer,NoSetFolders}
 * {0x00020,Explorer,NoSetTaskbar}
 * {0x00040,Explorer,NoDesktop}
 * {0x00080,Explorer,NoFind}
 * {0x00100,Explorer,NoDrives}
 * {0x00200,Explorer,NoDriveAutoRun}
 * {0x00400,Explorer,NoDriveTypeAutoRun}
 * {0x00800,Explorer,NoNetHood}
 * {0x01000,Explorer,NoStartBanner}
 * {0x02000,Explorer,RestrictRun}
 * {0x04000,Explorer,NoPrinterTabs}
 * {0x08000,Explorer,NoDeletePrinter}
 * {0x10000,Explorer,NoAddPrinter}
 * {0x20000,Explorer,NoStartMenuSubFolders}
 * {0x40000,Explorer,MyDocsOnNet}
 * {0x80000,WinOldApp,NoRealMode}
 */
DWORD WIN32API SHELL32_100(DWORD pol)
{
 HKEY xhkey;

  dprintf (("SHELL32: .100 (%08xh) not implemented.\n",
            pol));

  if (RegOpenKeyA((HKEY)HKEY_CURRENT_USER,
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Policies",
                    &xhkey))
    return 0;

  /* FIXME: do nothing for now, just return 0 (== "allowed") */
  RegCloseKey(xhkey);

  return 0;
}


/*************************************************************************
 *           SHELL32_152            [SHELL32.152]
 * itemlist_length
 */
DWORD WIN32API SHELL32_152(LPITEMIDLIST iil)
{
  LPSHITEMID    si;
  DWORD     len;

  dprintf(("SHELL32: .152 (%08xh)\n",
           iil));


  si = &(iil->mkid);
  len = 2;

  while (si->cb)
  {
    len += si->cb;
    si   = (LPSHITEMID)(((char*)si)+si->cb);
  }

  return len;
}


/*************************************************************************
 *                      SHELL32_158                    [SHELL32.158]
 */
LPSTR WIN32API SHELL32_158(LPSTR path,
                              DWORD y,
                              DWORD z)
{
  dprintf(("SHELL32: .158 (%s, %08xh, %08xh)\n",
           path,
           y,
           z));

  path = SHELL32_31(path);

  return *path ? (path+1) : path;
}


/*************************************************************************
 *           SHELL32_165            [SHELL32.165]
 * create_path_and_notify(...)
 */
DWORD WIN32API SHELL32_165(DWORD x,
                              LPCSTR path)
{
  dprintf (("SHELL32 .165 (%08x, %08xh) not implemented.\n",
            x,
            path));

  if (SHELL32_79(path,
                    (LPVOID)x))
    return 0;

  return 0;
}


/*************************************************************************
 *           SHELL32_195            [SHELL32.195]
 * free_ptr() - frees memory using IMalloc
 */
DWORD WIN32API SHELL32_195(LPVOID x)
{
  dprintf (("SHELL32: .195 (%08xh) (LocalFree)\n",
            x));

  return (DWORD)LocalFree((HLOCAL)x);
}


/*************************************************************************
 *           SHELL32_196            [SHELL32.196]
 * void *task_alloc(DWORD len), uses SHMalloc allocator
 */
LPVOID WIN32API SHELL32_196(DWORD len)
{
  dprintf(("SHELL32: .196 (%08xh) (LocalAlloc)\n",
           len));

  return (LPVOID)LocalAlloc(len,
                              LMEM_ZEROINIT); /* FIXME */
}


/*************************************************************************
 *           SHELL32_18             [SHELL32.18]
 * copy_itemidlist()
 */
LPITEMIDLIST WIN32API SHELL32_18(LPITEMIDLIST iil)
{
  DWORD     len;
  LPITEMIDLIST  newiil;

  dprintf(("SHELL32: .18 (%08xh)\n",
           iil));


  len = SHELL32_152(iil);
  newiil = (LPITEMIDLIST)SHELL32_196(len);
  memcpy(newiil,
         iil,
         len);

  return newiil;
}


/*************************************************************************
 *           SHELL32_25             [SHELL32.25]
 * merge_itemidlist()
 */
LPITEMIDLIST WIN32API SHELL32_25(LPITEMIDLIST iil1,
                                    LPITEMIDLIST iil2)
{
  DWORD     len1,len2;
  LPITEMIDLIST  newiil;

  dprintf(("SHELL32: .25 (%08xh, %08xh)\n",
           iil1,
           iil2));


  len1   = SHELL32_152(iil1)-2;
  len2   = SHELL32_152(iil2);
  newiil = (LPITEMIDLIST)SHELL32_196(len1+len2);

  memcpy(newiil,
         iil1,
         len1);

  memcpy(((char*)newiil)+len1,
         iil2,
         len2);

  return newiil;
}


/*************************************************************************
 *           SHELL32_155            [SHELL32.155]
 * free_check_ptr - frees memory (if not NULL) allocated by SHMalloc allocator
 */
DWORD WIN32API SHELL32_155(LPVOID x)
{
  dprintf(("SHELL32: .155 (%08xh)\n",
           x));

  if (!x)
    return 0;

  return SHELL32_195(x);
}


/*************************************************************************
 * SHELL32_85 [SHELL32.85]
 * unknown
 */
DWORD WIN32API SHELL32_85(DWORD x1,
                             DWORD x2,
                             DWORD x3,
                             DWORD x4)
{
  dprintf(("SHELL32: .85 (%08x, %08xh, %08xh, %08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


/*****************************************************************************
 * Name      : .4
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHELL32_4(DWORD x1)
{
  dprintf(("SHELL32: .4 (%08xh) not implemented.\n",
           x1));

  return (0);
}


/*****************************************************************************
 * Name      : .59
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHELL32_59(void)
{
  dprintf(("SHELL32: .59 () not implemented.\n"));

  return (0);
}


/*****************************************************************************
 * Name      : .162
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHELL32_162(DWORD x1)
{
  dprintf(("SHELL32: .162 (%08xh) not implemented.\n",
           x1));

  return (0);
}


/*****************************************************************************
 * Name      : .145
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHELL32_145(DWORD x1,
                            DWORD x2)
{
  dprintf(("SHELL32: .145 (%08xh, %08xh) not implemented.\n",
           x1,
           x2));

  return (0);
}


/*****************************************************************************
 * Name      : .43
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHELL32_43(DWORD x1)
{
  dprintf(("SHELL32: .43 (%08xh) not implemented.\n",
           x1));

  return (0);
}


/*****************************************************************************
 * Name      : .51
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHELL32_51(DWORD x1,
                           DWORD x2,
                           DWORD x3)
{
  dprintf(("SHELL32: .51 (%08xh, %08xh, %08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return (0);
}


/*****************************************************************************
 * Name      : .55
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHELL32_55(DWORD x1)
{
  dprintf(("SHELL32: .55 (%08xh) not implemented.\n",
           x1));

  return (0);
}


/*****************************************************************************
 * Name      : .102
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHELL32_102(DWORD x1,
                            DWORD x2,
                            DWORD x3,
                            DWORD x4,
                            DWORD x5)
{
  dprintf(("SHELL32: .102 (%08xh, %08xh, %08xh, %08xh, %08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL ShellExecuteEx
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API ShellExecuteEx(LPSHELLEXECUTEINFOA lpExecInfo)
{
  dprintf(("SHELL32: ShellExecuteEx (%08xh) not implemented.\n",
           lpExecInfo));

  return (0);
}



/*****************************************************************************
 * Name      : BOOL ShellExecuteExA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API ShellExecuteExA(LPSHELLEXECUTEINFOA lpExecInfo)
{
  dprintf(("SHELL32: ShellExecuteExA (%08xh) not implemented.\n",
           lpExecInfo));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL ShellExecuteExW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

BOOL WIN32API ShellExecuteExW(LPSHELLEXECUTEINFOW lpExecInfo)
{
  dprintf(("SHELL32: ShellExecuteExW (%08xh) not implemented.\n",
           lpExecInfo));

  return (0);
}






#if 0
typedef DWORD (* WIN32API GetClassPtr)(REFCLSID,REFIID,LPVOID);

static GetClassPtr SH_find_moduleproc(LPSTR   dllname,
                                      HMODULE *xhmod,
                                      LPSTR   name)
{
  HMODULE hmod;
  FARPROC dllunload,
          nameproc;

  if (xhmod)
    *xhmod = 0;

  if (!lstrcmpi32A(SHELL32_34(dllname),
                   "shell32.dll"))
    return (GetClassPtr)SHELL32_DllGetClassObject;

  hmod = LoadLibraryEx32A(dllname,
                          0,
                          LOAD_WITH_ALTERED_SEARCH_PATH);
  if (!hmod)
    return NULL;

  dllunload = GetProcAddress32(hmod,
                               "DllCanUnloadNow");
  if (!dllunload)
    if (xhmod)
      *xhmod = hmod;

  nameproc = GetProcAddress32(hmod,
                              name);
  if (!nameproc)
  {
    FreeLibrary32(hmod);
    return NULL;
  }

  /* register unloadable dll with unloadproc ... */
  return (GetClassPtr)nameproc;
}


static DWORD SH_get_instance(REFCLSID clsid,
                             LPSTR    dllname,
                             LPVOID   unknownouter,
                             REFIID   refiid,
                             LPVOID inst)
{
  GetClassPtr    dllgetclassob;
  DWORD      hres;
  LPCLASSFACTORY classfac;

  dllgetclassob = SH_find_moduleproc(dllname,
                                     NULL,
                                     "DllGetClassObject");
  if (!dllgetclassob)
          return 0x80070000|GetLastError();

/* FIXME */
/*
  hres = (*dllgetclassob)(clsid,(REFIID)&IID_IClassFactory,inst);
  if (hres<0)
          return hres;

*/
  hres = (*dllgetclassob)(clsid,
                          (REFIID)&IID_IClassFactory,
                          &classfac);
  if (hres<0)
    return hres;

  classfac->lpvtbl->fnCreateInstance(classfac,
                                     unknownouter,
                                     refiid,
                                     inst);
  classfac->lpvtbl->fnRelease(classfac);

  return 0;
}


/*************************************************************************
 *              SHELL32_102 [SHELL32.102]
 * unknown
 */
LRESULT WIN32API SHELL32_102(LPSTR     aclsid,
                                CLSID     *clsid,
                                LPUNKNOWN unknownouter,
                                REFIID    refiid,
                                LPVOID    inst)
{
  char  buffer[256],
        xclsid[48],
        xiid[48],
        path[260],
        tmodel[100];
  HKEY  inprockey;
  DWORD pathlen,
        type,
        tmodellen;
  DWORD hres;


  WINE_StringFromCLSID(refiid,
                       xiid);

  if (clsid)
    WINE_StringFromCLSID(clsid,
                         xclsid);
  else
  {
    if (!aclsid)
      return 0x80040154;

    strcpy(xclsid,
           aclsid);
  }

  dprintf(("SHELL32: .102 (%s. %08xh, %08xh, %08x, %08xh)\n",
           aclsid,
           clsid,
           unknownouter,
           refiid,
           inst));


  sprintf(buffer,
          "CLSID\\%s\\InProcServer32",
          xclsid);

  if (RegOpenKeyEx32A(HKEY_CLASSES_ROOT,
                      buffer,
                      0,
                      0x02000000,
                      &inprockey))
    return SH_get_instance(clsid,
                           "shell32.dll",
                           unknownouter,
                           refiid,
                           inst);

  pathlen=sizeof(path);
  if (RegQueryValue32A(inprockey,
                       NULL,
                       path,
                       &pathlen))
  {
    RegCloseKey(inprockey);
    return SH_get_instance(clsid,
                           "shell32.dll",
                           unknownouter,
                           refiid,
                           inst);
  }

  dprintf (("SHELL32: .102 Server dll is %s\n",
            path));

  tmodellen=sizeof(tmodel);
  type=REG_SZ;

  if (RegQueryValueEx32A(inprockey,
                         "ThreadingModel",
                         NULL,
                         &type,
                         tmodel,
                         &tmodellen))
  {
    RegCloseKey(inprockey);
    return SH_get_instance(clsid,
                           "shell32.dll",
                           unknownouter,
                           refiid,
                           inst);
  }


  dprintf (("SHELL32: .102 Threading model is %s\n",
            tmodel));

  hres=SH_get_instance(clsid,
                       path,
                       unknownouter,
                       refiid,
                       inst);
  if (hres<0)
    hres=SH_get_instance(clsid,
                         "shell32.dll",
                         unknownouter,
                         refiid,
                         inst);

  RegCloseKey(inprockey);

  return hres;
}


/*************************************************************************
 *              SHELL_FindExecutable
 * Utility for code sharing between FindExecutable and ShellExecute
 */
static HINSTANCE32 SHELL_FindExecutable( LPCSTR lpFile,
                                         LPCSTR lpOperation,
                                         LPSTR lpResult)
{
    char *extension = NULL; /* pointer to file extension */
    char tmpext[5];         /* local copy to mung as we please */
    char filetype[256];     /* registry name for this filetype */
    LONG filetypelen=256;   /* length of above */
    char command[256];      /* command from registry */
    LONG commandlen=256;    /* This is the most DOS can handle :) */
    char buffer[256];       /* Used to GetProfileString */
    HINSTANCE32 retval=31;  /* default - 'No association was found' */
    char *tok;              /* token pointer */
    int i;                  /* random counter */
    char xlpFile[256];      /* result of SearchPath */

    TRACE(exec, "%s\n",
                 (lpFile != NULL?lpFile:"-") );
    lpResult[0]='\0'; /* Start off with an empty return string */

    /* trap NULL parameters on entry */
    if (( lpFile == NULL ) || ( lpResult == NULL ) || ( lpOperation == NULL ))
    {
    /* FIXME - should throw a warning, perhaps! */
    return 2; /* File not found. Close enough, I guess. */
    }

    if (SearchPath32A( NULL, lpFile,".exe",sizeof(xlpFile),xlpFile,NULL))
        lpFile = xlpFile;

    /* First thing we need is the file's extension */
    extension = strrchr( xlpFile, '.' ); /* Assume last "." is the one; */
                    /* File->Run in progman uses */
                    /* .\FILE.EXE :( */
    if ((extension == NULL) || (extension == &xlpFile[strlen(xlpFile)]))
    {
    return 31; /* no association */
    }

    /* Make local copy & lowercase it for reg & 'programs=' lookup */
    lstrcpyn32A( tmpext, extension, 5 );
    CharLower32A( tmpext );
    TRACE(exec, "%s file\n", tmpext);

    /* Three places to check: */
    /* 1. win.ini, [windows], programs (NB no leading '.') */
    /* 2. Registry, HKEY_CLASS_ROOT\<filetype>\shell\open\command */
    /* 3. win.ini, [extensions], extension (NB no leading '.' */
    /* All I know of the order is that registry is checked before */
    /* extensions; however, it'd make sense to check the programs */
    /* section first, so that's what happens here. */

    /* See if it's a program - if GetProfileString fails, we skip this
     * section. Actually, if GetProfileString fails, we've probably
     * got a lot more to worry about than running a program... */
    if ( GetProfileString32A("windows", "programs", "exe pif bat com",
                          buffer, sizeof(buffer)) > 0 )
      {
        for (i=0;i<strlen(buffer); i++) buffer[i]=tolower(buffer[i]);

        tok = strtok(buffer, " \t"); /* ? */
        while( tok!= NULL)
          {
            if (strcmp(tok, &tmpext[1])==0) /* have to skip the leading "." */
              {
                strcpy(lpResult, xlpFile);
                /* Need to perhaps check that the file has a path
                 * attached */
                TRACE(exec, "found %s\n",
                             lpResult);
                                return 33;

        /* Greater than 32 to indicate success FIXME According to the
         * docs, I should be returning a handle for the
         * executable. Does this mean I'm supposed to open the
         * executable file or something? More RTFM, I guess... */
              }
            tok=strtok(NULL, " \t");
          }
      }

    /* Check registry */
    if (RegQueryValue16( (HKEY)HKEY_CLASSES_ROOT, tmpext, filetype,
                         &filetypelen ) == SHELL_ERROR_SUCCESS )
    {
    filetype[filetypelen]='\0';
    TRACE(exec, "File type: %s\n",
             filetype);

    /* Looking for ...buffer\shell\lpOperation\command */
    strcat( filetype, "\\shell\\" );
    strcat( filetype, lpOperation );
    strcat( filetype, "\\command" );

    if (RegQueryValue16( (HKEY)HKEY_CLASSES_ROOT, filetype, command,
                             &commandlen ) == SHELL_ERROR_SUCCESS )
    {
        /* Is there a replace() function anywhere? */
        command[commandlen]='\0';
        strcpy( lpResult, command );
        tok=strstr( lpResult, "%1" );
        if (tok != NULL)
        {
        tok[0]='\0'; /* truncate string at the percent */
        strcat( lpResult, xlpFile ); /* what if no dir in xlpFile? */
        tok=strstr( command, "%1" );
        if ((tok!=NULL) && (strlen(tok)>2))
        {
            strcat( lpResult, &tok[2] );
        }
        }
        retval=33; /* FIXME see above */
    }
    }
    else /* Check win.ini */
    {
    /* Toss the leading dot */
    extension++;
    if ( GetProfileString32A( "extensions", extension, "", command,
                                  sizeof(command)) > 0)
      {
        if (strlen(command)!=0)
          {
            strcpy( lpResult, command );
            tok=strstr( lpResult, "^" ); /* should be ^.extension? */
            if (tok != NULL)
              {
                tok[0]='\0';
                strcat( lpResult, xlpFile ); /* what if no dir in xlpFile? */
                tok=strstr( command, "^" ); /* see above */
                if ((tok != NULL) && (strlen(tok)>5))
                  {
                    strcat( lpResult, &tok[5]);
                  }
              }
            retval=33; /* FIXME - see above */
          }
      }
    }

    TRACE(exec, "returning %s\n", lpResult);
    return retval;
}

/*************************************************************************
 *              ShellExecute16      [SHELL.20]
 */
HINSTANCE16 WIN32API ShellExecute16( HWND16 hWnd, LPCSTR lpOperation,
                                   LPCSTR lpFile, LPCSTR lpParameters,
                                   LPCSTR lpDirectory, INT16 iShowCmd )
{
    HINSTANCE16 retval=31;
    char old_dir[1024];
    char cmd[256];

    TRACE(exec, "(%04x,'%s','%s','%s','%s',%x)\n",
        hWnd, lpOperation ? lpOperation:"<null>", lpFile ? lpFile:"<null>",
        lpParameters ? lpParameters : "<null>",
        lpDirectory ? lpDirectory : "<null>", iShowCmd);

    if (lpFile==NULL) return 0; /* should not happen */
    if (lpOperation==NULL) /* default is open */
      lpOperation="open";

    if (lpDirectory)
    {
        GetCurrentDirectory32A( sizeof(old_dir), old_dir );
        SetCurrentDirectory32A( lpDirectory );
    }

    retval = SHELL_FindExecutable( lpFile, lpOperation, cmd );

    if (retval > 32)  /* Found */
    {
        if (lpParameters)
        {
            strcat(cmd," ");
            strcat(cmd,lpParameters);
        }

        TRACE(exec,"starting %s\n",cmd);
        retval = WinExec32( cmd, iShowCmd );
    }
    if (lpDirectory) SetCurrentDirectory32A( old_dir );
    return retval;
}


/*************************************************************************
 *             ShellExecute32A   (SHELL32.245)
 */
HINSTANCE32 WIN32API ShellExecute32A( HWND hWnd, LPCSTR lpOperation,
                                    LPCSTR lpFile, LPCSTR lpParameters,
                                    LPCSTR lpDirectory, INT32 iShowCmd )
{
    return ShellExecute16( hWnd, lpOperation, lpFile, lpParameters,
                           lpDirectory, iShowCmd );
}


/*************************************************************************
 *             FindExecutable32A   (SHELL32.184)
 */
HINSTANCE32 WIN32API FindExecutable32A( LPCSTR lpFile, LPCSTR lpDirectory,
                                      LPSTR lpResult )
{
    HINSTANCE32 retval=31;    /* default - 'No association was found' */
    char old_dir[1024];

    TRACE(exec, "File %s, Dir %s\n",
         (lpFile != NULL?lpFile:"-"),
         (lpDirectory != NULL?lpDirectory:"-"));

    lpResult[0]='\0'; /* Start off with an empty return string */

    /* trap NULL parameters on entry */
    if (( lpFile == NULL ) || ( lpResult == NULL ))
    {
    /* FIXME - should throw a warning, perhaps! */
    return 2; /* File not found. Close enough, I guess. */
    }

    if (lpDirectory)
    {
        GetCurrentDirectory32A( sizeof(old_dir), old_dir );
        SetCurrentDirectory32A( lpDirectory );
    }

    retval = SHELL_FindExecutable( lpFile, "open", lpResult );

    TRACE(exec, "returning %s\n", lpResult);
    if (lpDirectory) SetCurrentDirectory32A( old_dir );
    return retval;
}





/*************************************************************************
 *             ExtractIcon32A   (SHELL32.133)
 */
HICON32 WIN32API ExtractIcon32A( HINSTANCE32 hInstance, LPCSTR lpszExeFileName,
    UINT32 nIconIndex )
{
    HGLOBAL16 handle = InternalExtractIcon(hInstance,lpszExeFileName,nIconIndex, 1);

    if( handle )
    {
    HICON16* ptr = (HICON16*)GlobalLock16(handle);
    HICON16  hIcon = *ptr;

    GlobalFree16(handle);
    return hIcon;
    }
    return 0;
}

/*************************************************************************
 *             ExtractIcon32W   (SHELL32.180)
 */
HICON32 WIN32API ExtractIcon32W( HINSTANCE32 hInstance, LPCWSTR lpszExeFileName,
    UINT32 nIconIndex )
{
    LPSTR   exefn = HEAP_strdupWtoA(GetProcessHeap(),0,lpszExeFileName);
    HICON32 ret = ExtractIcon32A(hInstance,exefn,nIconIndex);

    HeapFree(GetProcessHeap(),0,exefn);
    return ret;
}


/*************************************************************************
 *              ExtractAssociatedIcon   [SHELL.36]
 *
 * Return icon for given file (either from file itself or from associated
 * executable) and patch parameters if needed.
 */
HICON32 WIN32API ExtractAssociatedIcon32A(HINSTANCE32 hInst,LPSTR lpIconPath,
    LPWORD lpiIcon)
{
    return ExtractAssociatedIcon16(hInst,lpIconPath,lpiIcon);
}

HICON16 WIN32API ExtractAssociatedIcon16(HINSTANCE16 hInst,LPSTR lpIconPath,
    LPWORD lpiIcon)
{
    HICON16 hIcon = ExtractIcon16(hInst, lpIconPath, *lpiIcon);

    if( hIcon < 2 )
    {

    if( hIcon == 1 ) /* no icons found in given file */
    {
        char  tempPath[0x80];
        USHORT  uRet = FindExecutable16(lpIconPath,NULL,tempPath);

        if( uRet > 32 && tempPath[0] )
        {
        strcpy(lpIconPath,tempPath);
        hIcon = ExtractIcon16(hInst, lpIconPath, *lpiIcon);

        if( hIcon > 2 ) return hIcon;
        }
        else hIcon = 0;
    }

    if( hIcon == 1 )
        *lpiIcon = 2;   /* MSDOS icon - we found .exe but no icons in it */
    else
        *lpiIcon = 6;   /* generic icon - found nothing */

    GetModuleFileName16(hInst, lpIconPath, 0x80);
    hIcon = LoadIcon16( hInst, MAKEINTRESOURCE16(*lpiIcon));
    }

    return hIcon;
}

/*************************************************************************
 *              FindEnvironmentString   [SHELL.38]
 *
 * Returns a pointer into the DOS environment... Ugh.
 */
LPSTR SHELL_FindString(LPSTR lpEnv, LPCSTR entry)
{
    USHORT  l = strlen(entry);
    for( ; *lpEnv ; lpEnv+=strlen(lpEnv)+1 )
    {
    if( lstrncmpi32A(lpEnv, entry, l) ) continue;

    if( !*(lpEnv+l) )
        return (lpEnv + l);         /* empty entry */
    else if ( *(lpEnv+l)== '=' )
        return (lpEnv + l + 1);
    }
    return NULL;
}

SEGPTR WIN32API FindEnvironmentString(LPSTR str)
{
    SEGPTR  spEnv = GetDOSEnvironment();
    LPSTR  lpEnv = (LPSTR)PTR_SEG_TO_LIN(spEnv);

    LPSTR  lpString = (spEnv)?SHELL_FindString(lpEnv, str):NULL;

    if( lpString )      /*  offset should be small enough */
    return spEnv + (lpString - lpEnv);

    return (SEGPTR)NULL;
}

/*************************************************************************
 *                      DoEnvironmentSubst      [SHELL.37]
 *
 * Replace %KEYWORD% in the str with the value of variable KEYWORD
 * from "DOS" environment.
 */
DWORD WIN32API DoEnvironmentSubst(LPSTR str,WORD length)
{
  LPSTR   lpEnv = (LPSTR)PTR_SEG_TO_LIN(GetDOSEnvironment());
  LPSTR   lpBuffer = (LPSTR)HeapAlloc( GetProcessHeap(), 0, length);
  LPSTR   lpstr = str;
  LPSTR   lpbstr = lpBuffer;

  CharToOem32A(str,str);

  TRACE(reg,"accept %s\n", str);

  while( *lpstr && lpbstr - lpBuffer < length )
   {
     LPSTR lpend = lpstr;

     if( *lpstr == '%' )
       {
      do { lpend++; } while( *lpend && *lpend != '%' );
      if( *lpend == '%' && lpend - lpstr > 1 )  /* found key */
        {
           LPSTR lpKey;
          *lpend = '\0';
           lpKey = SHELL_FindString(lpEnv, lpstr+1);
           if( lpKey )              /* found key value */
         {
           int l = strlen(lpKey);

           if( l > length - (lpbstr - lpBuffer) - 1 )
             {
               WARN(reg,"Env subst aborted - string too short\n");
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
      else break;                   /* back off and whine */

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

  TRACE(reg,"    return %s\n", str);

  OemToChar32A(str,str);
  HeapFree( GetProcessHeap(), 0, lpBuffer);

  /*  Return str length in the LOWORD
   *  and 1 in HIWORD if subst was successful.
   */
 return (DWORD)MAKELONG(strlen(str), length);
}

/*************************************************************************
 *              ShellHookProc       [SHELL.103]
 * System-wide WH_SHELL hook.
 */
LRESULT WIN32API ShellHookProc(INT16 code, WPARAM16 wParam, LPARAM lParam)
{
    TRACE(reg,"%i, %04x, %08x\n", code, wParam,
                              (unsigned)lParam );
    if( SHELL_hHook && SHELL_hWnd )
    {
    USHORT  uMsg = 0;
        switch( code )
        {
        case HSHELL_WINDOWCREATED:      uMsg = uMsgWndCreated;   break;
        case HSHELL_WINDOWDESTROYED:    uMsg = uMsgWndDestroyed; break;
        case HSHELL_ACTIVATESHELLWINDOW:    uMsg = uMsgShellActivate;
        }
    PostMessage16( SHELL_hWnd, uMsg, wParam, 0 );
    }
    return CallNextHookEx16( WH_SHELL, code, wParam, lParam );
}

/*************************************************************************
 *              RegisterShellHook   [SHELL.102]
 */
BOOL WIN32API RegisterShellHook(HWND16 hWnd, USHORT uAction)
{
    TRACE(reg,"%04x [%u]\n", hWnd, uAction );

    switch( uAction )
    {
    case 2: /* register hWnd as a shell window */

         if( !SHELL_hHook )
         {
        HMODULE16 hShell = GetModuleHandle16( "SHELL" );

        SHELL_hHook = SetWindowsHookEx16( WH_SHELL, ShellHookProc,
                                                  hShell, 0 );
        if( SHELL_hHook )
        {
            uMsgWndCreated = RegisterWindowMessage32A( lpstrMsgWndCreated );
            uMsgWndDestroyed = RegisterWindowMessage32A( lpstrMsgWndDestroyed );
            uMsgShellActivate = RegisterWindowMessage32A( lpstrMsgShellActivate );
        }
        else WARN(reg, "unable to install ShellHookProc()!\n");
         }

         if( SHELL_hHook ) return ((SHELL_hWnd = hWnd) != 0);
         break;

    default:

         WARN(reg, "unknown code %i\n", uAction );

         /* just in case */

         SHELL_hWnd = 0;
    }
    return FALSE;
}


/*************************************************************************
 *              SHAppBarMessage32   [SHELL32.207]
 */
UINT32 WIN32API SHAppBarMessage32(DWORD msg, PAPPBARDATA data)
{
    FIXME(shell,"(0x%08lx,%p): stub\n", msg, data);
#if 0
    switch (msg) {
        case ABM_ACTIVATE:
        case ABM_GETAUTOHIDEBAR:
        case ABM_GETSTATE:
        case ABM_GETTASKBARPOS:
        case ABM_NEW:
        case ABM_QUERYPOS:
        case ABM_REMOVE:
        case ABM_SETAUTOHIDEBAR:
        case ABM_SETPOS:
        case ABM_WINDOWPOSCHANGED:
        ;
    }
#endif
    return 0;
}


/*************************************************************************
 *              Control_RunDLL      [SHELL32.12]
 *
 * Wild speculation in the following!
 *
 * http://premium.microsoft.com/msdn/library/techart/msdn193.htm
 */

void WIN32API Control_RunDLL (HWND hwnd, LPCVOID code, LPCSTR cmd, DWORD arg4)
{
  TRACE(exec, "(%08x, %p, \"%s\", %08lx)\n",
    hwnd, code ? code : "(null)", cmd ? cmd : "(null)", arg4);
}

/*************************************************************************
 */

void WIN32API FreeIconList( DWORD dw )
{
    FIXME(reg, "empty stub\n" );
}

/*************************************************************************
 *           SHELL32_DllGetClassObject   [SHELL32.14]
 *
 * http://premium.microsoft.com/msdn/library/sdkdoc/api2_48fo.htm
 */
DWORD WIN32API SHELL32_DllGetClassObject(REFCLSID rclsid,REFIID iid,LPVOID *ppv)
{
    char    xclsid[50],xiid[50];
    HRESULT hres = E_OUTOFMEMORY;


    WINE_StringFromCLSID((LPCLSID)rclsid,xclsid);
    WINE_StringFromCLSID((LPCLSID)iid,xiid);
    TRACE(shell,"(%s,%s,%p)\n",xclsid,xiid,ppv);

    *ppv = NULL;
/* SDK example code looks like this:
 *
    HRESULT hres = E_OUTOFMEMORY;

    *ppv = NULL;
    CClassFactory *pClassFactory = new CClassFactory(rclsid);

    if (pClassFactory) {
        hRes = pClassFactory->QueryInterface(riid,ppv);
    pClassFactory->Release();
    }
    return hRes;
 *
 * The magic of the whole stuff is still unclear to me, so just hack together
 * something.
 */

    if (!memcmp(rclsid,&CLSID_ShellDesktop,sizeof(CLSID_ShellDesktop))) {
        TRACE(shell,"   requested CLSID_ShellDesktop, creating it.\n");
    *ppv = IShellFolder_Constructor();
    FIXME(shell,"Initialize this folder to be the shell desktop folder\n");
    return 0;
    }

    FIXME(shell, "   -> clsid not found. returning E_OUTOFMEMORY.\n");
    return hres;
}

#endif




#if 0

WINSHELLAPI HICON     APIENTRY DuplicateIcon(HINSTANCE hInst, HICON hIcon);
WINSHELLAPI HICON     APIENTRY ExtractAssociatedIconA(HINSTANCE hInst, LPSTR lpIconPath, LPWORD lpiIcon);
WINSHELLAPI HICON     APIENTRY ExtractAssociatedIconW(HINSTANCE hInst, LPWSTR lpIconPath, LPWORD lpiIcon);
WINSHELLAPI UINT APIENTRY SHAppBarMessage(DWORD dwMessage, PAPPBARDATA pData);
WINSHELLAPI DWORD   APIENTRY DoEnvironmentSubstA(LPSTR szString, UINT cbString);
WINSHELLAPI DWORD   APIENTRY DoEnvironmentSubstW(LPWSTR szString, UINT cbString);
WINSHELLAPI LPSTR APIENTRY FindEnvironmentStringA(LPSTR szEnvVar);
WINSHELLAPI LPWSTR APIENTRY FindEnvironmentStringW(LPWSTR szEnvVar);
WINSHELLAPI void WINAPI SHFreeNameMappings(HANDLE hNameMappings);
WINSHELLAPI void WINAPI WinExecErrorA(HWND hwnd, int error, LPCSTR lpstrFileName, LPCSTR lpstrTitle);
WINSHELLAPI void WINAPI WinExecErrorW(HWND hwnd, int error, LPCWSTR lpstrFileName, LPCWSTR lpstrTitle);
WINSHELLAPI BOOL WINAPI SHGetNewLinkInfoA(LPCSTR pszLinkTo, LPCSTR pszDir, LPSTR pszName,
                             BOOL FAR * pfMustCopy, UINT uFlags);
WINSHELLAPI BOOL WINAPI SHGetNewLinkInfoW(LPCWSTR pszLinkTo, LPCWSTR pszDir, LPWSTR pszName,
                             BOOL FAR * pfMustCopy, UINT uFlags);

#endif

