/* $Id: shell32_main.cpp,v 1.10 2000-03-27 15:09:21 cbratschi Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *             Shell basics
 *
 *  1998 Marcus Meissner
 *  1998 Juergen Schmied (jsch)  *  <juergen.schmied@metronet.de>
 *
 * Corel WINE 20000324 level
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <stdlib.h>
#include <string.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "wine/winuser16.h"
#include "winerror.h"
#include "heap.h"
#include "resource.h"
#include "dlgs.h"
//#include "ldt.h"
#include "sysmetrics.h"
#include "debugtools.h"
#include "winreg.h"
#include "authors.h"
#include "winversion.h"

#include "shellapi.h"
#include "pidl.h"

#include "shlobj.h"
#include "shell32_main.h"
#include "shlguid.h"
#include "wine/undocshell.h"
#include "shpolicy.h"

#include <heapstring.h>
#include <misc.h>


/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHELL32-MAIN)


#define MORE_DEBUG 1

BOOL VERSION_OsIsUnicode(VOID)
{
  return FALSE;
}

/*************************************************************************
 * CommandLineToArgvW                          [SHELL32.7]
 */

ODINFUNCTION2(LPWSTR*, CommandLineToArgvW, LPWSTR,  cmdline,
                                           LPDWORD, numargs)
{  LPWSTR  *argv,s,t;
   int   i;

   /* to get writeable copy */
   cmdline = HEAP_strdupW( GetProcessHeap(), 0, cmdline);
   s=cmdline;i=0;
   while (*s)
   { /* space */
     if (*s==0x0020)
     { i++;
       s++;
       while (*s && *s==0x0020)
         s++;
       continue;
     }
     s++;
   }
   argv=(LPWSTR*)HeapAlloc( GetProcessHeap(), 0, sizeof(LPWSTR)*(i+1) );
   s=t=cmdline;
   i=0;
   while (*s)
   { if (*s==0x0020)
     { *s=0;
       argv[i++]=HEAP_strdupW( GetProcessHeap(), 0, t );
       *s=0x0020;
       while (*s && *s==0x0020)
         s++;
       if (*s)
         t=s+1;
       else
         t=s;
       continue;
     }
     s++;
   }
   if (*t)
     argv[i++]=(LPWSTR)HEAP_strdupW( GetProcessHeap(), 0, t );

   HeapFree( GetProcessHeap(), 0, cmdline );
   argv[i]=NULL;
   *numargs=i;
   return argv;
}

/*************************************************************************
 * Control_RunDLL                           [SHELL32.12]
 *
 * Wild speculation in the following!
 *
 * http://premium.microsoft.com/msdn/library/techart/msdn193.htm
 */

ODINPROCEDURE4(Control_RunDLL,HWND,   hwnd,
                              LPCVOID,code,
                              LPCSTR, cmd,
                              DWORD,  arg4)
{
  dprintf(("SHELL32:Shell32_Main:Control_RunDLL not implemented.\n"));
}

/*************************************************************************
 * SHGetFileInfoA                           [SHELL32.254]
 */

ODINFUNCTION5(DWORD, SHGetFileInfoA, LPCSTR,       path,
                                     DWORD,        dwFileAttributes,
                                     SHFILEINFOA*, psfi,
                                     UINT,         sizeofpsfi,
                                     UINT,         flags )
{
   char szLoaction[MAX_PATH];
   int iIndex;
   DWORD ret = TRUE, dwAttributes = 0;
   IShellFolder * psfParent = NULL;
   IExtractIcon * pei = NULL;
   LPITEMIDLIST   pidlLast = NULL, pidl = NULL;
   HRESULT hr = S_OK;

   dprintf(("SHELL32:Shell32_main:SHGetFileInfoA (%s,0x%lx,%p,0x%x,0x%x)\n",
     (flags & SHGFI_PIDL)? "pidl" : path, dwFileAttributes, psfi, sizeofpsfi, flags));

#ifdef MORE_DEBUG
   ZeroMemory(psfi, sizeof(SHFILEINFOA));
#endif
   if ((flags & SHGFI_USEFILEATTRIBUTES) && (flags & (SHGFI_ATTRIBUTES|SHGFI_EXETYPE|SHGFI_PIDL)))
     return FALSE;

   /* translate the path into a pidl only when SHGFI_USEFILEATTRIBUTES in not specified
      the pidl functions fail on not existing file names */
   if (flags & SHGFI_PIDL)
   {
     pidl = (LPCITEMIDLIST) path;
     if (!pidl )
     {
       dprintf(("pidl is null!\n"));
       return FALSE;
     }
   }
   else if (!(flags & SHGFI_USEFILEATTRIBUTES))
   {
     hr = SHILCreateFromPathA ( path, &pidl, &dwAttributes);
     /* note: the attributes in ISF::ParseDisplayName are not implemented */
   }

   /* get the parent shellfolder */
   if (pidl)
   {
     hr = SHBindToParent( pidl, &IID_IShellFolder, (LPVOID*)&psfParent, &pidlLast);
   }

   /* get the attributes of the child */
   if (SUCCEEDED(hr) && (flags & SHGFI_ATTRIBUTES))
   {
     if (!(flags & SHGFI_ATTR_SPECIFIED))
     {
       psfi->dwAttributes = 0xffffffff;
     }
     IShellFolder_GetAttributesOf(psfParent, 1 , &pidlLast, &(psfi->dwAttributes));
   }

   /* get the displayname */
   if (SUCCEEDED(hr) && (flags & SHGFI_DISPLAYNAME))
   {
     if (flags & SHGFI_USEFILEATTRIBUTES)
     {
       strcpy (psfi->szDisplayName, PathFindFilenameA(path));
     }
     else
     {
       STRRET str;
       hr = IShellFolder_GetDisplayNameOf(psfParent, pidlLast, SHGDN_INFOLDER, &str);
       StrRetToStrNA (psfi->szDisplayName, MAX_PATH, &str, pidlLast);
     }
   }

   /* get the type name */
   if (SUCCEEDED(hr) && (flags & SHGFI_TYPENAME))
   {
     _ILGetFileType(pidlLast, psfi->szTypeName, 80);
   }

   /* ### icons ###*/
   if (flags & SHGFI_LINKOVERLAY)
   dprintf(("SHELL32:Shell32_main:SHGetFileInfoA set icon to link, stub\n"));

   if (flags & SHGFI_OPENICON)
   dprintf(("SHELL32:Shell32_main:SHGetFileInfoA set to open icon, stub\n"));

   if (flags & SHGFI_SELECTED)
   dprintf(("SHELL32:Shell32_main:SHGetFileInfoA set icon to selected, stub\n"));

   if (flags & SHGFI_SHELLICONSIZE)
   dprintf(("SHELL32:Shell32_main:SHGetFileInfoA set icon to shell size, stub\n"));

   /* get the iconlocation */
   if (SUCCEEDED(hr) && (flags & SHGFI_ICONLOCATION ))
   {
     UINT uDummy,uFlags;
     hr = IShellFolder_GetUIObjectOf(psfParent, 0, 1, &pidlLast, &IID_IExtractIconA, &uDummy, (LPVOID*)&pei);

     if (SUCCEEDED(hr))
     {
       hr = IExtractIconA_GetIconLocation(pei, 0, szLoaction, MAX_PATH, &iIndex, &uFlags);
       /* fixme what to do with the index? */

       if(uFlags != GIL_NOTFILENAME)
         strcpy (psfi->szDisplayName, szLoaction);
       else
         ret = FALSE;

       IExtractIconA_Release(pei);
     }
   }

   /* get icon index (or load icon)*/
   if (SUCCEEDED(hr) && (flags & (SHGFI_ICON | SHGFI_SYSICONINDEX)))
   {
     if (flags & SHGFI_USEFILEATTRIBUTES)
     {
       char sTemp [MAX_PATH];
       char * szExt;
       DWORD dwNr=0;

       lstrcpynA(sTemp, path, MAX_PATH);
       szExt = (LPSTR) PathFindExtensionA(sTemp);
       if( szExt && HCR_MapTypeToValue(szExt, sTemp, MAX_PATH, TRUE)
              && HCR_GetDefaultIcon(sTemp, sTemp, MAX_PATH, &dwNr))
            {
              if (!strcmp("%1",sTemp))            /* icon is in the file */
              {
                strcpy(sTemp, path);
              }
         /* FIXME: if sTemp contains a valid filename, get the icon
            from there, index is in dwNr
         */
              psfi->iIcon = 2;
            }
            else                                  /* default icon */
            {
              psfi->iIcon = 0;
            }
     }
     else
     {
       if (!(PidlToSicIndex(psfParent, pidlLast, (flags && SHGFI_LARGEICON), (PUINT)&(psfi->iIcon))))
       {
         ret = FALSE;
       }
     }
     if (ret)
     {
       ret = (DWORD) ((flags && SHGFI_LARGEICON) ? ShellBigIconList : ShellSmallIconList);
     }
   }

   /* icon handle */
   if (SUCCEEDED(hr) && (flags & SHGFI_ICON))
     psfi->hIcon = pImageList_GetIcon((flags && SHGFI_LARGEICON) ? ShellBigIconList:ShellSmallIconList, psfi->iIcon, ILD_NORMAL);


   if (flags & SHGFI_EXETYPE)
     dprintf(("SHELL32:Shell32_main:SHGetFileInfoA type of executable, stub\n"));

   if (flags & (SHGFI_UNKNOWN1 | SHGFI_UNKNOWN2 | SHGFI_UNKNOWN3))
     dprintf(("SHELL32:Shell32_main:SHGetFileInfoA unknown attribute!\n"));

   if (psfParent)
     IShellFolder_Release(psfParent);

   if (hr != S_OK)
     ret = FALSE;

#ifdef MORE_DEBUG
   TRACE_(shell) ("icon=0x%08x index=0x%08x attr=0x%08lx name=%s type=%s\n",
      psfi->hIcon, psfi->iIcon, psfi->dwAttributes, psfi->szDisplayName, psfi->szTypeName);
#endif
   return ret;
}

/*************************************************************************
 * SHGetFileInfoW                           [SHELL32.255]
 */

ODINFUNCTION5(DWORD, SHGetFileInfoW, LPCWSTR,      path,
                                     DWORD,        dwFileAttributes,
                                     SHFILEINFOW*, psfi,
                                     UINT,         sizeofpsfi,
                                     UINT,        flags)
{
  dprintf(("SHELL32:Shell32_main:SHGetFileInfoW not implemented\n"));
  return 0;
}


/*************************************************************************
 * ExtractIconA                                [SHELL32.133]
 */

ODINFUNCTION3(HICON, ExtractIconA, HINSTANCE, hInstance,
                                   LPCSTR,    lpszExeFileName,
                                   UINT,      nIconIndex )
{
  HGLOBAL handle = InternalExtractIcon(hInstance,lpszExeFileName,nIconIndex, 1);
    TRACE_(shell)("\n");
    if( handle )
    {
   HICON* ptr = (HICON*)GlobalLock(handle);
   HICON  hIcon = *ptr;

   GlobalFree(handle);
   return hIcon;
    }
   return 0;
}

/*************************************************************************
 * ExtractIconW                                [SHELL32.180]
 */

ODINFUNCTION3(HICON, ExtractIconW, HINSTANCE, hInstance,
                                   LPCWSTR,   lpszExeFileName,
                                   UINT,      nIconIndex )
{ LPSTR  exefn;
  HICON  ret;

  exefn = HEAP_strdupWtoA(GetProcessHeap(),0,lpszExeFileName);
  ret = ExtractIconA(hInstance,exefn,nIconIndex);

  HeapFree(GetProcessHeap(),0,exefn);
  return ret;
}

/*************************************************************************
 * FindExecutableA                             [SHELL32.184]
 */

ODINFUNCTION3(HINSTANCE, FindExecutableA, LPCSTR, lpFile,
                                          LPCSTR, lpDirectory,
                                          LPSTR,  lpResult )
{ HINSTANCE retval=31;    /* default - 'No association was found' */
    char old_dir[1024];

  dprintf(("File %s, Dir %s\n",
       (lpFile != NULL?lpFile:"-"),
       (lpDirectory != NULL?lpDirectory:"-")));

    lpResult[0]='\0'; /* Start off with an empty return string */

    /* trap NULL parameters on entry */
    if (( lpFile == NULL ) || ( lpResult == NULL ))
  { /* FIXME - should throw a warning, perhaps! */
   return 2; /* File not found. Close enough, I guess. */
    }

    if (lpDirectory)
  { GetCurrentDirectoryA( sizeof(old_dir), old_dir );
        SetCurrentDirectoryA( lpDirectory );
    }

    retval = SHELL_FindExecutable( lpFile, "open", lpResult );

  dprintf(("returning %s\n", lpResult));
  if (lpDirectory)
    SetCurrentDirectoryA( old_dir );
    return retval;
}

/*************************************************************************
 * FindExecutableW                             [SHELL32.219]
 */
ODINFUNCTION3(HINSTANCE, FindExecutableW, LPCWSTR, lpFile,
                                          LPCWSTR, lpDirectory,
                                          LPWSTR,  lpResult )
{
  dprintf(("FindExecutableW not implemented.\n"));
  return 31;    /* default - 'No association was found' */
}

typedef struct
{ LPCSTR  szApp;
    LPCSTR  szOtherStuff;
    HICON hIcon;
} ABOUT_INFO;

#define     IDC_STATIC_TEXT                 100
#define     IDC_LISTBOX                     99
#define     IDC_ODIN_TEXT                   98

#define     DROP_FIELD_TOP                  (-15)
#define     DROP_FIELD_HEIGHT            15

#if 0 //CB: not used
extern HICON hIconTitleFont;
#endif

static BOOL __get_dropline( HWND hWnd, LPRECT lprect )
{ HWND hWndCtl = GetDlgItem(hWnd, IDC_ODIN_TEXT);
    if( hWndCtl )
  { GetWindowRect( hWndCtl, lprect );
   MapWindowPoints( 0, hWnd, (LPPOINT)lprect, 2 );
   lprect->bottom = (lprect->top += DROP_FIELD_TOP);
   return TRUE;
    }
    return FALSE;
}

/*************************************************************************
 * SHAppBarMessage32                           [SHELL32.207]
 */

ODINFUNCTION2(UINT, SHAppBarMessage, DWORD,       msg,
                                     PAPPBARDATA, data)
{
  int width=data->rc.right - data->rc.left;
  int height=data->rc.bottom - data->rc.top;
  RECT rec=data->rc;

  dprintf(("SHELL32: SHAppBarMessage.\n"));

        switch (msg)
        { case ABM_GETSTATE:
               return ABS_ALWAYSONTOP | ABS_AUTOHIDE;
          case ABM_GETTASKBARPOS:
               GetWindowRect(data->hWnd, &rec);
               data->rc=rec;
               return TRUE;
          case ABM_ACTIVATE:
               SetActiveWindow(data->hWnd);
               return TRUE;
          case ABM_GETAUTOHIDEBAR:
               data->hWnd=GetActiveWindow();
               return TRUE;
          case ABM_NEW:
               SetWindowPos(data->hWnd,HWND_TOP,rec.left,rec.top,
                                        width,height,SWP_SHOWWINDOW);
               return TRUE;
          case ABM_QUERYPOS:
               GetWindowRect(data->hWnd, &(data->rc));
               return TRUE;
          case ABM_REMOVE:
               CloseHandle(data->hWnd);
               return TRUE;
          case ABM_SETAUTOHIDEBAR:
               SetWindowPos(data->hWnd,HWND_TOP,rec.left+1000,rec.top,
                                       width,height,SWP_SHOWWINDOW);
               return TRUE;
          case ABM_SETPOS:
               data->uEdge=(ABE_RIGHT | ABE_LEFT);
               SetWindowPos(data->hWnd,HWND_TOP,data->rc.left,data->rc.top,
                                  width,height,SWP_SHOWWINDOW);
               return TRUE;
          case ABM_WINDOWPOSCHANGED:
               SetWindowPos(data->hWnd,HWND_TOP,rec.left,rec.top,
                                        width,height,SWP_SHOWWINDOW);
               return TRUE;
          }
      return FALSE;
}

/*************************************************************************
 * SHHelpShortcuts_RunDLL                   [SHELL32.224]
 *
 */

ODINFUNCTION4(DWORD, SHHelpShortcuts_RunDLL, DWORD, dwArg1,
                                             DWORD, dwArg2,
                                             DWORD, dwArg3,
                                             DWORD, dwArg4)
{
  dprintf(("SHELL32:Shell32_Main:SHHelpShortcuts_RunDLL not implemented.\n"));
  return 0;
}

/*************************************************************************
 * SHLoadInProc                                [SHELL32.225]
 * Create an instance of specified object class from within the shell process
 */

ODINFUNCTION1(DWORD, SHLoadInProc, DWORD, dwArg1)
{
  CLSID *id;

  dprintf(("SHELL32: SHLoadInProc\n"));

  CLSIDFromString((LPCOLESTR) dwArg1, id);
  if (S_OK==SHCoCreateInstance( (LPSTR) dwArg1, id, NULL, &IID_IUnknown, NULL) )
        return NOERROR;
  return DISP_E_MEMBERNOTFOUND;
}



/*************************************************************************
 * ShellExecuteA                            [SHELL32.245]
 */

ODINFUNCTION6(HINSTANCE, ShellExecuteA, HWND,   hWnd,
                                        LPCSTR, lpOperation,
                                        LPCSTR, lpFile,
                                        LPCSTR, lpParameters,
                                        LPCSTR, lpDirectory,
                                        INT,    iShowCmd )
{   HINSTANCE retval=31;
    char old_dir[1024];
    char cmd[256];

    if (lpFile==NULL) return 0; /* should not happen */
    if (lpOperation==NULL) /* default is open */
      lpOperation="open";

    if (lpDirectory)
    { GetCurrentDirectoryA( sizeof(old_dir), old_dir );
        SetCurrentDirectoryA( lpDirectory );
    }

    cmd[0] = '\0';
    retval = SHELL_FindExecutable( lpFile, lpOperation, cmd );

    if (retval > 32)  /* Found */
    {
        if (lpParameters)
        {
            strcat(cmd," ");
            strcat(cmd,lpParameters);
        }

        dprintf(("starting %s\n",cmd));
        retval = WinExec( cmd, iShowCmd );
    }
    else if(PathIsURLA((LPSTR)lpFile))    /* File not found, check for URL */
    {
      char lpstrProtocol[256];
      LONG cmdlen = 512;
      LPSTR lpstrRes;
      INT iSize;

      lpstrRes = strchr(lpFile,':');
      iSize = lpstrRes - lpFile;

      /* Looking for ...protocol\shell\lpOperation\command */
      strncpy(lpstrProtocol,lpFile,iSize);
      lpstrProtocol[iSize]='\0';
      strcat( lpstrProtocol, "\\shell\\" );
      strcat( lpstrProtocol, lpOperation );
      strcat( lpstrProtocol, "\\command" );

      /* Remove File Protocol from lpFile */
      /* In the case file://path/file     */
      if(!strnicmp(lpFile,"file",iSize))
      {
        lpFile += iSize;
        while(*lpFile == ':') lpFile++;
      }


      /* Get the application for the protocol and execute it */
      if (RegQueryValueA( HKEY_CLASSES_ROOT, lpstrProtocol, cmd,
                           &cmdlen ) == ERROR_SUCCESS )
      {
          LPSTR tok;
          LPSTR tmp;
          char param[256] = "";
          LONG paramlen = 256;

          /* Get the parameters needed by the application
             from the associated ddeexec key */
          tmp = strstr(lpstrProtocol,"command");
          tmp[0] = '\0';
          strcat(lpstrProtocol,"ddeexec");

          if(RegQueryValueA( HKEY_CLASSES_ROOT, lpstrProtocol, param,&paramlen ) == ERROR_SUCCESS)
          {
            strcat(cmd," ");
            strcat(cmd,param);
            cmdlen += paramlen;
          }

          /* Is there a replace() function anywhere? */
          cmd[cmdlen]='\0';
          tok=strstr( cmd, "%1" );
          if (tok != NULL)
          {
            tok[0]='\0'; /* truncate string at the percent */
            strcat( cmd, lpFile ); /* what if no dir in xlpFile? */
            tok=strstr( cmd, "%1" );
            if ((tok!=NULL) && (strlen(tok)>2))
            {
              strcat( cmd, &tok[2] );
            }
          }

          retval = WinExec( cmd, iShowCmd );
      }
    }
    /* Check if file specified is in the form www.??????.*** */
    else if(!strnicmp(lpFile,"www",3))
    {
      /* if so, append lpFile http:// and call ShellExecute */
      char lpstrTmpFile[256] = "http://" ;
      strcat(lpstrTmpFile,lpFile);
      retval = ShellExecuteA(hWnd,lpOperation,lpstrTmpFile,NULL,NULL,0);
    }
    /* Nothing was done yet, try to execute the cmdline directly,
       maybe it's an OS/2 program */
    else
    {
      strcpy(cmd,lpFile);
      strcat(cmd,lpParameters ? lpParameters : "");
      retval = WinExec( cmd, iShowCmd );
    }

    if (lpDirectory)
      SetCurrentDirectoryA( old_dir );
    return retval;
}


/*************************************************************************
 * ShellExecuteW                            [SHELL32.294]
 * from shellapi.h
 * WINSHELLAPI HINSTANCE APIENTRY ShellExecuteW(HWND hwnd, LPCWSTR lpOperation,
 * LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
 */

ODINFUNCTION6(HINSTANCE, ShellExecuteW, HWND,    hWnd,
                                        LPCWSTR, lpOperation,
                                        LPCWSTR, lpFile,
                                        LPCWSTR, lpParameters,
                                        LPCWSTR, lpDirectory,
                                        INT,     iShowCmd )
{
  dprintf(("SHELL32:Shell32_Main:ShellExecuteW not implemented\n"));
  return 0;
}


/*************************************************************************
 * AboutDlgProc32                           (internal)
 */
#define IDC_ODINLOGO 2001
#define IDB_ODINLOGO 5555

BOOL WINAPI AboutDlgProc( HWND hWnd, UINT msg, WPARAM wParam,
                              LPARAM lParam )
{
    HWND hWndCtl;
    char Template[512], AppTitle[512];

    switch(msg)
    {
    case WM_INITDIALOG:
    {
        ABOUT_INFO *info = (ABOUT_INFO *)lParam;
        if(info)
        {
          const char* const *pstr = SHELL_People;

            SendDlgItemMessageA(hWnd, stc1, STM_SETICON,info->hIcon, 0);
            GetWindowTextA( hWnd, Template, sizeof(Template) );
            sprintf( AppTitle, Template, info->szApp );
            SetWindowTextA( hWnd, AppTitle );
            SetWindowTextA( GetDlgItem(hWnd, IDC_STATIC_TEXT), info->szOtherStuff );

            HWND hwndOdinLogo = GetDlgItem(hWnd, IDC_ODINLOGO);
            if(hwndOdinLogo) {
                        HBITMAP hBitmap = LoadBitmapA(shell32_hInstance, MAKEINTRESOURCEA(IDB_ODINLOGO));
                        SendMessageA(hwndOdinLogo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
            }

            hWndCtl = GetDlgItem(hWnd, IDC_LISTBOX);
            SendMessageA( hWndCtl, WM_SETREDRAW, 0, 0 );
#if 0 //CB: not used (hIconTitleFont not valid!!!), default font is ok
            SendMessageA( hWndCtl, WM_SETFONT, hIconTitleFont, 0 );
#endif
            while (*pstr)
            {
                SendMessageA( hWndCtl, LB_ADDSTRING, (WPARAM)-1, (LPARAM)*pstr );
                pstr++;
            }
            SendMessageA( hWndCtl, WM_SETREDRAW, 1, 0 );
        }
        return 1;
    }

    case WM_PAINT:
    {
        RECT rect;
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint( hWnd, &ps );

        if( __get_dropline( hWnd, &rect ) ) {
            SelectObject( hDC, GetStockObject( BLACK_PEN ) );
            MoveToEx( hDC, rect.left, rect.top, NULL );
            LineTo( hDC, rect.right, rect.bottom );
        }
        EndPaint( hWnd, &ps );
        break;
   }

    case WM_LBTRACKPOINT:
   hWndCtl = GetDlgItem(hWnd, IDC_LISTBOX);
   if( (INT)GetKeyState( VK_CONTROL ) < 0 )
      { if( DragDetect( hWndCtl, *((LPPOINT)&lParam) ) )
        { INT idx = SendMessageA( hWndCtl, LB_GETCURSEL, 0, 0 );
      if( idx != -1 )
          { INT length = SendMessageA( hWndCtl, LB_GETTEXTLEN, (WPARAM)idx, 0 );
          HGLOBAL hMemObj = GlobalAlloc( GMEM_MOVEABLE, length + 1 );
          char* pstr = (char*)GlobalLock( hMemObj );

          if( pstr )
            { HCURSOR hCursor = LoadCursorA( 0, (LPCSTR)OCR_DRAGOBJECT );
         SendMessageA( hWndCtl, LB_GETTEXT, (WPARAM)idx, (LPARAM)pstr );
         SendMessageA( hWndCtl, LB_DELETESTRING, (WPARAM)idx, 0 );
         UpdateWindow( hWndCtl );

         if( !DragObject(hWnd, hWnd, DRAGOBJ_DATA, hMemObj, hCursor) )
             SendMessageA( hWndCtl, LB_ADDSTRING, (WPARAM)-1, (LPARAM)pstr );
          }
            if( hMemObj )
              GlobalFree( hMemObj );
      }
       }
   }
   break;

    case WM_QUERYDROPOBJECT:
   if( wParam == 0 )
      { LPDRAGINFO lpDragInfo = (LPDRAGINFO)lParam;
       if( lpDragInfo && lpDragInfo->wFlags == DRAGOBJ_DATA )
        { RECT rect;
      if( __get_dropline( hWnd, &rect ) )
          { POINT pt;
       pt.x=lpDragInfo->pt.x;
       pt.x=lpDragInfo->pt.y;
          rect.bottom += DROP_FIELD_HEIGHT;
          if( PtInRect( &rect, pt ) )
            { SetWindowLongA( hWnd, DWL_MSGRESULT, 1 );
         return TRUE;
          }
      }
       }
   }
   break;

    case WM_DROPOBJECT:
   if( wParam == hWnd )
      { LPDRAGINFO lpDragInfo = (LPDRAGINFO)lParam;
       if( lpDragInfo && lpDragInfo->wFlags == DRAGOBJ_DATA && lpDragInfo->hList )
        { char* pstr = (char*)GlobalLock( (HGLOBAL)(lpDragInfo->hList) );
      if( pstr )
          { static char __appendix_str[] = " with";

          hWndCtl = GetDlgItem( hWnd, IDC_ODIN_TEXT );
          SendMessageA( hWndCtl, WM_GETTEXT, 512, (LPARAM)Template );
          if( !strncmp( Template, "ODIN", 4 ) )
         SetWindowTextA( GetDlgItem(hWnd, IDC_STATIC_TEXT), Template );
          else
          { char* pch = Template + strlen(Template) - strlen(__appendix_str);
         *pch = '\0';
         SendMessageA( GetDlgItem(hWnd, IDC_LISTBOX), LB_ADDSTRING,
               (WPARAM)-1, (LPARAM)Template );
          }

          strcpy( Template, pstr );
          strcat( Template, __appendix_str );
          SetWindowTextA( hWndCtl, Template );
          SetWindowLongA( hWnd, DWL_MSGRESULT, 1 );
          return TRUE;
      }
       }
   }
   break;

    case WM_COMMAND:
        if (wParam == IDOK)
        {
            EndDialog(hWnd, TRUE);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hWnd, TRUE);
        break;
    }
    return 0;
}


/*************************************************************************
 * ShellAboutA                              [SHELL32.243]
 */

ODINFUNCTION4(INT,ShellAboutA, HWND,   hWnd,
                               LPCSTR, szApp,
                               LPCSTR, szOtherStuff,
                               HICON,  hIcon )
{   ABOUT_INFO info;
    HRSRC hRes;
    LPVOID dlgTemplate;

    if(!(hRes = FindResourceA(shell32_hInstance, "SHELL_ABOUT_MSGBOX", RT_DIALOGA)))
        return FALSE;
    if(!(dlgTemplate = (LPVOID)LoadResource(shell32_hInstance, hRes)))
        return FALSE;

    info.szApp        = szApp;
    info.szOtherStuff = szOtherStuff;
    info.hIcon        = hIcon;
    if (!hIcon) info.hIcon = LoadIconA( 0, MAKEINTRESOURCEA(OIC_ODINICON) );
    return DialogBoxIndirectParamA( GetWindowLongA( hWnd, GWL_HINSTANCE ),
                                    (DLGTEMPLATE*)dlgTemplate , hWnd, AboutDlgProc, (LPARAM)&info );
}


/*************************************************************************
 * ShellAboutW                              [SHELL32.244]
 */
ODINFUNCTION4(INT,ShellAboutW, HWND,    hWnd,
                               LPCWSTR, szApp,
                               LPCWSTR, szOtherStuff,
                               HICON,   hIcon )
{   INT ret;
    ABOUT_INFO info;
    HRSRC hRes;
    LPVOID dlgTemplate;

    if(!(hRes = FindResourceA(shell32_hInstance, "SHELL_ABOUT_MSGBOX", RT_DIALOGA)))
        return FALSE;
    if(!(dlgTemplate = (LPVOID)LoadResource(shell32_hInstance, hRes)))
        return FALSE;

    info.szApp        = HEAP_strdupWtoA( GetProcessHeap(), 0, szApp );
    info.szOtherStuff = HEAP_strdupWtoA( GetProcessHeap(), 0, szOtherStuff );
    info.hIcon        = hIcon;
    if (!hIcon) info.hIcon = LoadIconA( 0, MAKEINTRESOURCEA(OIC_ODINICON) );
    ret = DialogBoxIndirectParamA( GetWindowLongA( hWnd, GWL_HINSTANCE ),
                                   (DLGTEMPLATE*)dlgTemplate, hWnd, AboutDlgProc, (LPARAM)&info );
    HeapFree( GetProcessHeap(), 0, (LPSTR)info.szApp );
    HeapFree( GetProcessHeap(), 0, (LPSTR)info.szOtherStuff );
    return ret;
}

/*************************************************************************
 * FreeIconList
 */

ODINPROCEDURE1(FreeIconList,DWORD,dw)
{
  dprintf(("SHELL32:Shell32_Main:FreeIconList not implemented.\n"));
}

/***********************************************************************
 * DllGetVersion [COMCTL32.25]
 *
 * Retrieves version information of the 'SHELL32.DLL'
 *
 * PARAMS
 *     pdvi [O] pointer to version information structure.
 *
 * RETURNS
 *     Success: S_OK
 *     Failure: E_INVALIDARG
 *
 * NOTES
 *     Returns version of a shell32.dll from IE4.01 SP1.
 */

ODINFUNCTION1(HRESULT, SHELL32_DllGetVersion, DLLVERSIONINFO*, pdvi)
{
   if (pdvi->cbSize != sizeof(DLLVERSIONINFO))
   { dprintf(("wrong DLLVERSIONINFO size from app\n"));
     return E_INVALIDARG;
   }

   pdvi->dwMajorVersion = 4;
   pdvi->dwMinorVersion = 72;
   pdvi->dwBuildNumber = 3110;
   pdvi->dwPlatformID = 1;

   dprintf(("%lu.%lu.%lu.%lu\n",
      pdvi->dwMajorVersion, pdvi->dwMinorVersion,
      pdvi->dwBuildNumber, pdvi->dwPlatformID));

   return S_OK;
}
/*************************************************************************
 * global variables of the shell32.dll
 * all are once per process
 *
 */
void  (WINAPI* pDLLInitComctl)(LPVOID);
INT   (WINAPI* pImageList_AddIcon) (HIMAGELIST himl, HICON hIcon);
INT   (WINAPI* pImageList_ReplaceIcon) (HIMAGELIST, INT, HICON);
HIMAGELIST (WINAPI * pImageList_Create) (INT,INT,UINT,INT,INT);
BOOL  (WINAPI* pImageList_Draw) (HIMAGELIST himl, int i, HDC hdcDest, int x, int y, UINT fStyle);
HICON (WINAPI * pImageList_GetIcon) (HIMAGELIST, INT, UINT);
INT   (WINAPI* pImageList_GetImageCount)(HIMAGELIST);
COLORREF (WINAPI *pImageList_SetBkColor)(HIMAGELIST, COLORREF);

LPVOID   (WINAPI* pCOMCTL32_Alloc) (INT);
BOOL  (WINAPI* pCOMCTL32_Free) (LPVOID);

HDPA  (WINAPI* pDPA_Create) (INT);
INT   (WINAPI* pDPA_InsertPtr) (const HDPA, INT, LPVOID);
BOOL  (WINAPI* pDPA_Sort) (const HDPA, PFNDPACOMPARE, LPARAM);
LPVOID   (WINAPI* pDPA_GetPtr) (const HDPA, INT);
BOOL  (WINAPI* pDPA_Destroy) (const HDPA);
INT   (WINAPI *pDPA_Search) (const HDPA, LPVOID, INT, PFNDPACOMPARE, LPARAM, UINT);
LPVOID   (WINAPI *pDPA_DeletePtr) (const HDPA hdpa, INT i);

/* user32 */
HICON (WINAPI *pLookupIconIdFromDirectoryEx)(LPBYTE dir, BOOL bIcon, INT width, INT height, UINT cFlag);
HICON (WINAPI *pCreateIconFromResourceEx)(LPBYTE bits,UINT cbSize, BOOL bIcon, DWORD dwVersion, INT width, INT height,UINT cFlag);

/* ole2 */
HRESULT (WINAPI* pOleInitialize)(LPVOID reserved);
void (WINAPI* pOleUninitialize)(void);
HRESULT (WINAPI* pDoDragDrop)(IDataObject* pDataObject, IDropSource * pDropSource, DWORD dwOKEffect, DWORD *pdwEffect);
HRESULT (WINAPI* pRegisterDragDrop)(HWND hwnd, IDropTarget* pDropTarget);
HRESULT (WINAPI* pRevokeDragDrop)(HWND hwnd);

static HINSTANCE  hComctl32;
static HINSTANCE  hOle32;
static INT     shell32_RefCount = 0;

INT      shell32_ObjCount = 0;
HINSTANCE   shell32_hInstance = 0;
HIMAGELIST  ShellSmallIconList = 0;
HIMAGELIST  ShellBigIconList = 0;

/*************************************************************************
 * SHELL32 LibMain
 *
 * NOTES
 *  calling oleinitialize here breaks sone apps.
 */

static void Shell32ProcLoadHelper(LPVOID* pAddr,
                                  HANDLE hModule,
                                  LPCSTR lpstrName)
{
  *pAddr = (void*)GetProcAddress(hModule,lpstrName);

  if (!pAddr)
     dprintf(("Shell32: Shell32ProcLoadHelper(%08xh,%08xh,%s) failed!\n",
              pAddr,
              hModule,
              lpstrName));
}


ODINFUNCTION3(BOOL, Shell32LibMain, HINSTANCE, hinstDLL,
                                    DWORD,     fdwReason,
                                    LPVOID,    fImpLoad)
{
   HMODULE  hUser32;

   switch (fdwReason)
   {
     case DLL_PROCESS_ATTACH:
       shell32_RefCount++;
       if (shell32_hInstance)
       {
         dprintf(("shell32.dll instantiated twice in one address space!\n"));
       }
       else
       {
         /* we only want to call this the first time shell32 is instantiated */
         SHInitRestricted(NULL, NULL);
       }

       shell32_hInstance = hinstDLL;

       hComctl32 = LoadLibraryA("COMCTL32.DLL");
       hOle32 = LoadLibraryA("OLE32.DLL");
       hUser32 = GetModuleHandleA("USER32");

       if (!hComctl32 || !hUser32 || !hOle32)
       {
         dprintf(("P A N I C SHELL32 loading failed\n"));
         return FALSE;
       }

       /* comctl32 */
       Shell32ProcLoadHelper((LPVOID*)&pDLLInitComctl,hComctl32,"InitCommonControlsEx");
       Shell32ProcLoadHelper((LPVOID*)&pImageList_Create,hComctl32,"ImageList_Create");
       Shell32ProcLoadHelper((LPVOID*)&pImageList_AddIcon,hComctl32,"ImageList_AddIcon");
       Shell32ProcLoadHelper((LPVOID*)&pImageList_ReplaceIcon,hComctl32,"ImageList_ReplaceIcon");
       Shell32ProcLoadHelper((LPVOID*)&pImageList_GetIcon,hComctl32,"ImageList_GetIcon");
       Shell32ProcLoadHelper((LPVOID*)&pImageList_GetImageCount,hComctl32,"ImageList_GetImageCount");
       Shell32ProcLoadHelper((LPVOID*)&pImageList_Draw,hComctl32,"ImageList_Draw");
       Shell32ProcLoadHelper((LPVOID*)&pImageList_SetBkColor,hComctl32,"ImageList_SetBkColor");
       Shell32ProcLoadHelper((LPVOID*)&pCOMCTL32_Alloc,hComctl32, (LPCSTR)71L);
       Shell32ProcLoadHelper((LPVOID*)&pCOMCTL32_Free,hComctl32, (LPCSTR)73L);
       Shell32ProcLoadHelper((LPVOID*)&pDPA_Create,hComctl32, (LPCSTR)328L);
       Shell32ProcLoadHelper((LPVOID*)&pDPA_Destroy,hComctl32, (LPCSTR)329L);
       Shell32ProcLoadHelper((LPVOID*)&pDPA_GetPtr,hComctl32, (LPCSTR)332L);
       Shell32ProcLoadHelper((LPVOID*)&pDPA_InsertPtr,hComctl32, (LPCSTR)334L);
       Shell32ProcLoadHelper((LPVOID*)&pDPA_DeletePtr,hComctl32, (LPCSTR)336L);
       Shell32ProcLoadHelper((LPVOID*)&pDPA_Sort,hComctl32, (LPCSTR)338L);
       Shell32ProcLoadHelper((LPVOID*)&pDPA_Search,hComctl32, (LPCSTR)339L);
       /* user32 */
       Shell32ProcLoadHelper((LPVOID*)&pLookupIconIdFromDirectoryEx,hUser32,"LookupIconIdFromDirectoryEx");
       Shell32ProcLoadHelper((LPVOID*)&pCreateIconFromResourceEx,hUser32,"CreateIconFromResourceEx");
       /* ole2 */
       Shell32ProcLoadHelper((LPVOID*)&pOleInitialize,hOle32,"OleInitialize");
       Shell32ProcLoadHelper((LPVOID*)&pOleUninitialize,hOle32,"OleUninitialize");
       Shell32ProcLoadHelper((LPVOID*)&pDoDragDrop,hOle32,"DoDragDrop");
       Shell32ProcLoadHelper((LPVOID*)&pRegisterDragDrop,hOle32,"RegisterDragDrop");
       Shell32ProcLoadHelper((LPVOID*)&pRevokeDragDrop,hOle32,"RevokeDragDrop");

       /* initialize the common controls */
       if (pDLLInitComctl)
       {
         pDLLInitComctl(NULL);
       }

       SIC_Initialize();
       SYSTRAY_Init();

       break;

     case DLL_THREAD_ATTACH:
       shell32_RefCount++;
       break;

     case DLL_THREAD_DETACH:
       shell32_RefCount--;
       break;

     case DLL_PROCESS_DETACH:
       shell32_RefCount--;

       if ( !shell32_RefCount )
       {
         shell32_hInstance = 0;

         if (pdesktopfolder)
         {
           IShellFolder_Release(pdesktopfolder);
           pdesktopfolder = NULL;
         }

         SIC_Destroy();

         /* this one is here to check if AddRef/Release is balanced */
         if (shell32_ObjCount)
         {
           dprintf(("leaving with %u objects left (memory leak)\n", shell32_ObjCount));
         }
       }

       FreeLibrary(hOle32);
       FreeLibrary(hComctl32);

       dprintf(("refcount=%u objcount=%u \n", shell32_RefCount, shell32_ObjCount));
       break;
   }
   return TRUE;
}

/*************************************************************************
 * DllInstall         [SHELL32.202]
 *
 * PARAMETERS
 *
 *    BOOL bInstall - TRUE for install, FALSE for uninstall
 *    LPCWSTR pszCmdLine - command line (unused by shell32?)
 */

HRESULT WINAPI DllInstall(BOOL bInstall, LPCWSTR cmdline)
{
   FIXME("(%s, %s): stub!\n", bInstall ? "TRUE":"FALSE", debugstr_w(cmdline));

   return S_OK;         /* indicate success */
}

