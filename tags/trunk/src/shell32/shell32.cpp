/* $Id: shell32.cpp,v 1.4 1999-06-09 15:22:27 phaller Exp $ */

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


/*****************************************************************************
 * Name      : DWORD ShellMessageBoxW
 * Purpose   : display a messagebox, retrieve message text from resources
 * Parameters: HMODULE hmod
 *             HWND    hwnd
 *             DWORD   idText
 *             DWORD   idTitle
 *             DWORD   uType
 *             LPCVOID arglist
 * Variables :
 * Result    :
 * Remark    : SHELL32.182
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API ShellMessageBoxW(HMODULE hmod,
                                HWND    hwnd,
                                DWORD   idText,
                                DWORD   idTitle,
                                DWORD   uType,
                                LPCVOID arglist)
{
  WCHAR   szText[100],
         szTitle[100],
         szTemp[256];
  LPWSTR pszText = &szText[0],
         pszTitle = &szTitle[0];
  LPVOID args = &arglist;

  dprintf(("SHELL32: ShellMessageBoxW(%08lx,%08lx,%08lx,%08lx,%08lx,%p)\n",
           hmod,
           hwnd,
           idText,
           idTitle,
           uType,
           arglist));

  if (!HIWORD (idTitle))
    LoadStringW(hmod,
                idTitle,
                pszTitle,
                100);
  else
    pszTitle = (LPWSTR)idTitle;

  if (! HIWORD (idText))
    LoadStringW(hmod,idText,pszText,100);
  else
    pszText = (LPWSTR)idText;

  FormatMessageW(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                 szText,
                 0,
                 0,
                 szTemp,
                 256,
                 (LPDWORD)args);

  return MessageBoxW(hwnd,
                     szTemp,
                     szTitle,
                     uType);
}

/*****************************************************************************
 * Name      : DWORD ShellMessageBoxA
 * Purpose   : display a messagebox, retrieve message text from resources
 * Parameters: HMODULE hmod
 *             HWND    hwnd
 *             DWORD   idText
 *             DWORD   idTitle
 *             DWORD   uType
 *             LPCVOID arglist
 * Variables :
 * Result    :
 * Remark    : SHELL32.183
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

DWORD WIN32API ShellMessageBoxA(HMODULE hmod,
                                HWND    hwnd,
                                DWORD   idText,
                                DWORD   idTitle,
                                DWORD   uType,
                                LPCVOID arglist)
{
  char   szText[100],
         szTitle[100],
         szTemp[256];
  LPSTR  pszText = &szText[0],
         pszTitle = &szTitle[0];
  LPVOID args = &arglist;

  dprintf(("SHELL32: ShellMessageBoxA (%08lx,%08lx,%08lx,%08lx,%08lx,%p)\n",
           hmod,
           hwnd,
           idText,
           idTitle,
           uType,
           arglist));

  if (!HIWORD (idTitle))
    LoadStringA(hmod,idTitle,pszTitle,100);
  else
    pszTitle = (LPSTR)idTitle;

  if (! HIWORD (idText))
    LoadStringA(hmod,idText,pszText,100);
  else
    pszText = (LPSTR)idText;

  FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                 pszText,
                 0,
                 0,
                 szTemp,
                 256,
                 (LPDWORD)args);

  return MessageBoxA(hwnd,
                     szTemp,
                     pszTitle,
                     uType);
}


