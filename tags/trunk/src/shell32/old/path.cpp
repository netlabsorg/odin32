/* $Id: path.cpp,v 1.1 1999-10-09 11:11:31 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
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
#include <winreg.h>

#include <stdarg.h>
//#include <builtin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wcstr.h>

#define HAVE_WCTYPE_H
#include <wctype.h>

#include <misc.h>
#include <unicode.h>
#include <winnls.h>
#include <ctype.h>

#include "shell32.h"


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

BOOL VERSION_OsIsUnicode()
{
  return FALSE; // ODIN is ASCII by default
                // however: what will the applications do ?
}

// quick'n'dirty port macros
#define debugstr_w(a) (a)
#define debugstr_a(a) (a)
#define lstrcpyAtoW(a,b) AsciiToUnicode(b,a)
#define lstrcpyWtoA(a,b) UnicodeToAscii(b,a)


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/


/*************************************************************************
 * PathIsRoot [SHELL32.29]
 */
BOOL WIN32API PathIsRootA(LPCSTR x)
{
  dprintf(("SHELL32: %s\n",x));

   if (*(x+1)==':' && *(x+2)=='\\')         /* "X:\" */
     return 1;
   if (*x=='\\')                         /* "\" */
     return 0;
   if (x[0]=='\\' && x[1]=='\\')            /* UNC "\\<xx>\" */
   { int foundbackslash = 0;
     x=x+2;
     while (*x)
     { if (*x++=='\\')
         foundbackslash++;
     }
     if (foundbackslash<=1)              /* max 1 \ more ... */
       return 1;
   }
   return 0;
}

BOOL WIN32API PathIsRootW(LPCWSTR x)
{  dprintf(("SHELL32: %s\n",debugstr_w(x)));
   if (*(x+1)==':' && *(x+2)=='\\')         /* "X:\" */
     return 1;
   if (*x == (WCHAR) '\\')                  /* "\" */
     return 0;
   if (x[0]==(WCHAR)'\\' && x[1]==(WCHAR)'\\') /* UNC "\\<xx>\" */
   { int foundbackslash = 0;
     x=x+2;
     while (*x)
     { if (*x++==(WCHAR)'\\')
         foundbackslash++;
     }
     if (foundbackslash<=1)              /* max 1 \ more ... */
       return 1;
   }
   return 0;
}

BOOL WIN32API PathIsRootAW(LPCVOID x)
{
  if (VERSION_OsIsUnicode())
    return PathIsRootW( (LPWSTR)x );
  else
    return PathIsRootA( (LPTSTR)x );
}

/*************************************************************************
 * PathBuildRoot [SHELL32.30]
 */
LPSTR WIN32API PathBuildRootA(LPSTR root,BYTE drive)
{
  dprintf(("SHELL32: %p %i\n",root, drive));
   strcpy(root,"A:\\");
   root[0]+=drive;
   return root;
}

LPWSTR WIN32API PathBuildRootW(LPWSTR root,BYTE drive)
{
   dprintf(("SHELL32: %p %i\n",root, drive));

   wcscpy((wchar_t*)root,L"A:\\");
   root[0]+=drive;
   return root;
}

LPVOID WIN32API PathBuildRootAW(LPCVOID x, BYTE y)
{
  if (VERSION_OsIsUnicode())
    return (LPVOID)PathBuildRootW( (LPWSTR)x,y);
  else
    return (LPVOID)PathBuildRootA( (LPTSTR)x,y);
}

/*************************************************************************
 * PathFindExtension [SHELL32.31]
 *
 * NOTES
 *     returns pointer to last . in last pathcomponent or at \0.
 */
LPCSTR WIN32API PathFindExtensionA(LPCSTR path)
{  LPCSTR   lastpoint = NULL;
   dprintf(("SHELL32: %p %s\n",path,path));
   while (*path)
   { if (*path=='\\'||*path==' ')
       lastpoint=NULL;
     if (*path=='.')
       lastpoint=path;
     path++;
   }
   return lastpoint?lastpoint:path;
}
LPCWSTR WIN32API PathFindExtensionW(LPCWSTR path)
{  LPCWSTR   lastpoint = NULL;
   dprintf(("SHELL32: %p L%s\n",path,debugstr_w(path)));
   while (*path)
   { if (*path==(WCHAR)'\\'||*path==(WCHAR)' ')
       lastpoint=NULL;
     if (*path==(WCHAR)'.')
       lastpoint=path;
     path++;
   }
   return lastpoint?lastpoint:path;
}
LPCVOID WIN32API PathFindExtensionAW(LPCVOID path)
{
 if (VERSION_OsIsUnicode())
   return PathFindExtensionW( (LPCWSTR)path );
 else
   return PathFindExtensionA( (LPCTSTR)path );
}

/*************************************************************************
 * PathAddBackslash [SHELL32.32]
 *
 * NOTES
 *     append \ if there is none
 */
LPSTR WIN32API PathAddBackslashA(LPSTR path)
{  int len;
   dprintf(("SHELL32: %p->%s\n",path,path));

   len = strlen(path);
   if (len && path[len-1]!='\\')
   { path[len]  = '\\';
     path[len+1]= 0x00;
     return path+len+1;
   }
   return path+len;
}
LPWSTR WIN32API PathAddBackslashW(LPWSTR path)
{  int len;
   dprintf(("SHELL32: %p->%s\n",path,debugstr_w(path)));

   len = lstrlenW(path);
   if (len && path[len-1]!=(WCHAR)'\\')
   { path[len]  = (WCHAR)'\\';
     path[len+1]= 0x00;
     return path+len+1;
   }
   return path+len;
}
LPVOID WIN32API PathAddBackslashAW(LPVOID path)
{
  if(VERSION_OsIsUnicode())
    return PathAddBackslashW( (LPWSTR)path );
  else
    return PathAddBackslashA( (LPTSTR)path );
}


/*************************************************************************
 * PathRemoveBlanks [SHELL32.33]
 *
 * NOTES
 *     remove spaces from beginning and end of passed string
 */
LPSTR WIN32API PathRemoveBlanksA(LPSTR str)
{  LPSTR x = str;
   dprintf(("SHELL32: %s\n",str));
   while (*x==' ') x++;
   if (x!=str)
     strcpy(str,x);
   if (!*str)
     return str;
   x=str+strlen(str)-1;
   while (*x==' ')
     x--;
   if (*x==' ')
     *x='\0';
   return x;
}
LPWSTR WIN32API PathRemoveBlanksW(LPWSTR str)
{  LPWSTR x = str;
   dprintf(("SHELL32: %s\n",debugstr_w(str)));
   while (*x==' ') x++;
   if (x!=str)
     lstrcpyW(str,x);
   if (!*str)
     return str;
   x=str+lstrlenW(str)-1;
   while (*x==' ')
     x--;
   if (*x==' ')
     *x='\0';
   return x;
}

LPVOID WIN32API PathRemoveBlanksAW(LPVOID str)
{
  if(VERSION_OsIsUnicode())
    return PathRemoveBlanksW( (LPWSTR)str );
  else
    return PathRemoveBlanksA( (LPTSTR)str );
}



/*************************************************************************
 * PathFindFilename [SHELL32.34]
 *
 * NOTES
 *     basename(char *fn);
 */
LPCSTR WIN32API PathFindFilenameA(LPCSTR aptr)
{  LPCSTR aslash;
   aslash = aptr;

   dprintf(("SHELL32: %s\n",aslash));
   while (aptr[0])
   { if (((aptr[0]=='\\') || (aptr[0]==':')) && aptr[1] && aptr[1]!='\\')
         aslash = aptr+1;
     aptr++;
   }
   return aslash;

}
LPCWSTR WIN32API PathFindFilenameW(LPCWSTR wptr)
{  LPCWSTR wslash;
   wslash = wptr;

   dprintf(("SHELL32: L%s\n",debugstr_w(wslash)));
   while (wptr[0])
   { if (((wptr[0]=='\\') || (wptr[0]==':')) && wptr[1] && wptr[1]!='\\')
       wslash = wptr+1;
     wptr++;
   }
   return wslash;
}
LPCVOID WIN32API PathFindFilenameAW(LPCVOID fn)
{
  if(VERSION_OsIsUnicode())
    return PathFindFilenameW( (LPCWSTR)fn );
  else
    return PathFindFilenameA( (LPCTSTR)fn );
}

/*************************************************************************
 * PathRemoveFileSpec [SHELL32.35]
 *
 * NOTES
 *     bool getpath(char *pathname); truncates passed argument to a valid path
 *     returns if the string was modified or not.
 *     "\foo\xx\foo"-> "\foo\xx"
 *     "\" -> "\"
 *     "a:\foo"   -> "a:\"
 */
DWORD WIN32API PathRemoveFileSpecA(LPSTR fn)
{
   LPSTR x,cutplace;
  dprintf(("SHELL32: %s\n",fn));
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
 * PathAppend [SHELL32.36]
 *
 * NOTES
 *     concat_paths(char*target,const char*add);
 *     concats "target\\add" and writes them to target
 */
LPSTR WIN32API PathAppendA(LPSTR x1,LPSTR x2)
{
  dprintf(("SHELL32: %s %s\n",x1,x2));
  while (x2[0]=='\\') x2++;
  return PathCombineA(x1,x1,x2);
}

/*************************************************************************
 * PathCombine [SHELL32.37]
 *
 * NOTES
 *  if lpszFile='.' skip it
 *  szDest can be equal to lpszFile. Thats why we use sTemp
 */
LPSTR WIN32API PathCombineA(LPSTR szDest, LPCSTR lpszDir, LPCSTR lpszFile)
{  char sTemp[MAX_PATH];
   dprintf(("SHELL32: %p %p->%s %p->%s\n",szDest, lpszDir, lpszDir, lpszFile, lpszFile));


   if (!lpszFile || !lpszFile[0] || (lpszFile[0]=='.' && !lpszFile[1]) )
   { strcpy(szDest,lpszDir);
     return szDest;
   }

   /*  if lpszFile is a complete path don't care about lpszDir */
   if (PathIsRootA(lpszFile))
   { strcpy(szDest,lpszFile);
   }
   else
   { strcpy(sTemp,lpszDir);
     PathAddBackslashA(sTemp);
     strcat(sTemp,lpszFile);
     strcpy(szDest,sTemp);
   }
   return szDest;
}
LPWSTR WIN32API PathCombineW(LPWSTR szDest, LPCWSTR lpszDir, LPCWSTR lpszFile)
{  WCHAR sTemp[MAX_PATH];
   dprintf(("SHELL32: %p %p->%s %p->%s\n",szDest, lpszDir, debugstr_w(lpszDir),
          lpszFile, debugstr_w(lpszFile)));


   if (!lpszFile || !lpszFile[0] || (lpszFile[0]==(WCHAR)'.' && !lpszFile[1]) )
   { lstrcpyW(szDest,lpszDir);
     return szDest;
   }

   /*  if lpszFile is a complete path don't care about lpszDir */
   if (PathIsRootW(lpszFile))
   { lstrcpyW(szDest,lpszFile);
   }
   else
   { lstrcpyW(sTemp,lpszDir);
     PathAddBackslashW(sTemp);
     lstrcatW(sTemp,lpszFile);
     lstrcpyW(szDest,sTemp);
   }
   return szDest;
}
LPVOID WIN32API PathCombineAW(LPVOID szDest, LPCVOID lpszDir, LPCVOID lpszFile)
{
 if (VERSION_OsIsUnicode())
   return PathCombineW( (LPWSTR)szDest,
                        (LPWSTR)lpszDir,
                        (LPWSTR)lpszFile );
 else
   return PathCombineA( (LPTSTR)szDest,
                        (LPTSTR)lpszDir,
                        (LPTSTR)lpszFile );
}

/*************************************************************************
 * PathIsUNC [SHELL32.39]
 *
 * NOTES
 *     PathIsUNC(char*path);
 */
BOOL WIN32API PathIsUNCA(LPCSTR path)
{  dprintf(("SHELL32: %s\n",path));

   if ((path[0]=='\\') && (path[1]=='\\'))
     return TRUE;
   return FALSE;
}
BOOL WIN32API PathIsUNCW(LPCWSTR path)
{  dprintf(("SHELL32: %s\n",debugstr_w(path)));

   if ((path[0]=='\\') && (path[1]=='\\'))
     return TRUE;
   return FALSE;
}
BOOL WIN32API PathIsUNCAW (LPCVOID path)
{
  if (VERSION_OsIsUnicode())
    return PathIsUNCW( (LPWSTR)path );
  else
    return PathIsUNCA( (LPTSTR)path );
}
/*************************************************************************
 *  PathIsRelativ [SHELL32.40]
 *
 */
BOOL WIN32API PathIsRelativeA (LPCSTR path)
{
  dprintf(("SHELL32: path=%s\n",path));

  if (path && (path[0]!='\\' && path[1]==':'))
    return TRUE;
  else
    return FALSE;
}

BOOL WIN32API PathIsRelativeW (LPCWSTR path)
{
  dprintf(("SHELL32: path=%s\n",debugstr_w(path)));

  if (path && (path[0]!='\\' && path[1]==':'))
    return TRUE;
  else
    return FALSE;
}
BOOL WIN32API PathIsRelativeAW (LPCVOID path)
{
  if (VERSION_OsIsUnicode())
    return PathIsRelativeW( (LPWSTR)path );
  else
    return PathIsRelativeA( (LPTSTR)path );
}

/*************************************************************************
 *  PathIsExe [SHELL32.43]
 *
 */
BOOL WIN32API PathIsExeA (LPCSTR path)
{  dprintf(("SHELL32: path=%s\n",path));
   return FALSE;
}
BOOL WIN32API PathIsExeW (LPCWSTR path)
{  dprintf(("SHELL32: path=%s\n",debugstr_w(path)));
   return FALSE;
}

BOOL WIN32API PathIsExeAW (LPCVOID path)
{
  if (VERSION_OsIsUnicode())
    return PathIsExeW ((LPWSTR)path);
  else
    return PathIsExeA ((LPTSTR)path);
}

/*************************************************************************
 * PathFileExists [SHELL32.45]
 *
 * NOTES
 *     file_exists(char *fn);
 */
BOOL WIN32API PathFileExistsA(LPSTR fn)
{
  dprintf(("SHELL32: %s\n",fn));

  if (GetFileAttributesA(fn)==-1)
    return FALSE;
  else
    return TRUE;
}

BOOL WIN32API PathFileExistsW(LPWSTR fn)
{
  dprintf(("SHELL32: %08xh\n",fn));

  if (GetFileAttributesW(fn)==-1)
    return FALSE;
  else
    return TRUE;
}

BOOL WIN32API PathFileExistsAW (LPCVOID fn)
{
  if (VERSION_OsIsUnicode())
    return PathFileExistsW ((LPWSTR)fn);
  else
    return PathFileExistsA ((LPTSTR)fn);
}


/*************************************************************************
 * PathMatchSpec [SHELL32.46]
 *
 * NOTES
 *     used from COMDLG32
 */

BOOL WIN32API PathMatchSpecA(LPCSTR name, LPCSTR mask)
{  LPCSTR _name;

   dprintf(("SHELL32: %s %s stub\n",name,mask));

   _name = name;
   while (*_name && *mask)
   { if (*mask ==';')
     { mask++;
       _name = name;
     }
     else if (*mask == '*')
     { mask++;
       while (*mask == '*') mask++;         /* Skip consecutive '*' */
       if (!*mask || *mask==';') return TRUE;  /* '*' matches everything */
       while (*_name && (toupper(*_name) != toupper(*mask))) _name++;
       if (!*_name)
       { while ( *mask && *mask != ';') mask++;
         _name = name;
       }
     }
     else if ( (*mask == '?') || (toupper(*mask) == toupper(*_name)) )
     { mask++;
       _name++;
     }
     else
     { while ( *mask && *mask != ';') mask++;
     }
   }
   return (!*_name && (!*mask || *mask==';'));
}
BOOL WIN32API PathMatchSpecW(LPCWSTR name, LPCWSTR mask)
{  WCHAR stemp[4];
   LPCWSTR _name;

   dprintf(("SHELL32: %s %s stub\n",debugstr_w(name),debugstr_w(mask)));

   lstrcpyAtoW(stemp,"*.*");
   if (!lstrcmpW( mask, stemp )) return 1;

   _name = name;
   while (*_name && *mask)
   { if (*mask ==';')
     { mask++;
       _name = name;
     }
     else if (*mask == '*')
     { mask++;
       while (*mask == '*') mask++;         /* Skip consecutive '*' */
       if (!*mask || *mask==';') return TRUE;  /* '*' matches everything */
       while (*_name && (towupper(*_name) != towupper(*mask))) _name++;
       if (!*_name)
       { while ( *mask && *mask != ';') mask++;
         _name = name;
       }
     }
     else if ( (*mask == '?') || (towupper(*mask) == towupper(*_name)) )
     { mask++;
       _name++;
     }
     else
     { while ( *mask && *mask != ';') mask++;
     }
   }
   return (!*_name && (!*mask || *mask==';'));
}
BOOL WIN32API PathMatchSpecAW(LPVOID name, LPVOID mask)
{
  if (VERSION_OsIsUnicode())
    return PathMatchSpecW( (LPCWSTR)name,
                           (LPCWSTR)mask );
  else
    return PathMatchSpecA( (LPCTSTR)name,
                           (LPCTSTR)mask );
}
/*************************************************************************
 * PathSetDlgItemPathAW [SHELL32.48]
 * NOTES
 *  use PathCompactPath to make sure, the path fits into the control
 */

BOOL WIN32API PathSetDlgItemPathA(HWND hDlg, int id, LPCSTR pszPath)
{  dprintf(("SHELL32: %x %x %s\n",hDlg, id, pszPath));
   return SetDlgItemTextA(hDlg, id, pszPath);
}
BOOL WIN32API PathSetDlgItemPathW(HWND hDlg, int id, LPCWSTR pszPath)
{  dprintf(("SHELL32: %x %x %s\n",hDlg, id, debugstr_w(pszPath)));
   return SetDlgItemTextW(hDlg, id, pszPath);
}
BOOL WIN32API PathSetDlgItemPathAW(HWND hDlg, int id, LPCVOID pszPath)
{
  if (VERSION_OsIsUnicode())
    return PathSetDlgItemPathW(hDlg,
                               id,
                               (LPCWSTR)pszPath);
  else
    return PathSetDlgItemPathA(hDlg,
                               id,
                               (LPCTSTR)pszPath);
}

/*************************************************************************
 * PathQualifyAW [SHELL32.49]
 */

BOOL WIN32API PathQualifyA(LPCSTR pszPath)
{  dprintf(("SHELL32: %s\n",pszPath));
   return 0;
}
BOOL WIN32API PathQualifyW(LPCWSTR pszPath)
{  dprintf(("SHELL32: %s\n",debugstr_w(pszPath)));
   return 0;
}
BOOL WIN32API PathQualifyAW(LPCVOID pszPath)
{
 if (VERSION_OsIsUnicode())
   return PathQualifyW((LPWSTR)pszPath);
 else
   return PathQualifyA((LPTSTR)pszPath);
}

/*************************************************************************
 * PathResolve [SHELL32.51]
 */
DWORD WIN32API PathResolve(LPCSTR s,DWORD x2,DWORD x3) {
   dprintf(("SHELL32: (%s,0x%08lx,0x%08lx),stub!\n",s,x2,x3));
   return 0;
}

/*************************************************************************
 * PathGetArgs [SHELL32.52]
 *
 * NOTES
 *     look for next arg in string. handle "quoted" strings
 *     returns pointer to argument *AFTER* the space. Or to the \0.
 */
LPCSTR WIN32API PathGetArgsA(LPCSTR cmdline)
{  BOOL  qflag = FALSE;

   dprintf(("SHELL32: %s\n",cmdline));

   while (*cmdline)
   { if ((*cmdline==' ') && !qflag)
       return cmdline+1;
     if (*cmdline=='"')
       qflag=!qflag;
     cmdline++;
   }
   return cmdline;

}
LPCWSTR WIN32API PathGetArgsW(LPCWSTR cmdline)
{  BOOL  qflag = FALSE;

   dprintf(("SHELL32: %sL\n",debugstr_w(cmdline)));

   while (*cmdline)
   { if ((*cmdline==' ') && !qflag)
       return cmdline+1;
     if (*cmdline=='"')
       qflag=!qflag;
     cmdline++;
   }
   return cmdline;
}
LPCVOID WIN32API PathGetArgsAW(LPVOID cmdline)
{
 if (VERSION_OsIsUnicode())
   return PathGetArgsW((LPCWSTR)cmdline);
 else
   return PathGetArgsA((LPCTSTR)cmdline);
}

/*************************************************************************
 * PathQuoteSpaces [SHELL32.55]
 *
 * NOTES
 *     basename(char *fn);
 */
LPSTR WIN32API PathQuoteSpacesA(LPCSTR aptr)
{  dprintf(("SHELL32: %s\n",aptr));
   return 0;

}
LPWSTR WIN32API PathQuoteSpacesW(LPCWSTR wptr)
{  dprintf(("SHELL32: L%s\n",debugstr_w(wptr)));
   return 0;
}
LPVOID WIN32API PathQuoteSpacesAW (LPCVOID fn)
{
  if(VERSION_OsIsUnicode())
    return PathQuoteSpacesW((LPCWSTR)fn);
  else
    return PathQuoteSpacesA((LPCTSTR)fn);
}


/*************************************************************************
 * PathUnquoteSpaces [SHELL32.56]
 *
 * NOTES
 *     unquote string (remove ")
 */
VOID WIN32API PathUnquoteSpacesA(LPSTR str)
{  DWORD      len = lstrlenA(str);
   dprintf(("SHELL32: %s\n",str));
   if (*str!='"')
     return;
   if (str[len-1]!='"')
     return;
   str[len-1]='\0';
   lstrcpyA(str,str+1);
   return;
}
VOID WIN32API PathUnquoteSpacesW(LPWSTR str)
{  DWORD len = lstrlenW(str);

   dprintf(("SHELL32: %s\n",debugstr_w(str)));

   if (*str!='"')
     return;
   if (str[len-1]!='"')
     return;
   str[len-1]='\0';
   lstrcpyW(str,str+1);
   return;
}
VOID WIN32API PathUnquoteSpacesAW(LPVOID str)
{
  if(VERSION_OsIsUnicode())
    PathUnquoteSpacesW((LPWSTR)str);
  else
    PathUnquoteSpacesA((LPTSTR)str);
}


/*************************************************************************
 * PathGetDriveNumber32 [SHELL32.57]
 *
 */
HRESULT WIN32API PathGetDriveNumber(LPSTR u)
{  dprintf(("SHELL32: %s stub\n",debugstr_a(u)));
   return 0;
}

/*************************************************************************
 * PathYetAnotherMakeUniqueName [SHELL32.75]
 *
 * NOTES
 *     exported by ordinal
 */
BOOL WIN32API PathYetAnotherMakeUniqueNameA(LPDWORD x,LPDWORD y) {
    dprintf(("SHELL32: (%p,%p):stub.\n",x,y));
    return TRUE;
}

/*************************************************************************
 * IsLFNDrive [SHELL32.119]
 *
 * NOTES
 *     exported by ordinal Name
 */
BOOL WIN32API IsLFNDriveA(LPCSTR path) {
    DWORD   fnlen;

    if (!GetVolumeInformationA(path,NULL,0,NULL,&fnlen,NULL,NULL,0))
   return FALSE;
    return fnlen>12;
}
/*************************************************************************
 * PathFindOnPath [SHELL32.145]
 */
BOOL WIN32API PathFindOnPathA(LPSTR sFile, LPCSTR sOtherDirs)
{  dprintf(("SHELL32: %s %s\n",sFile, sOtherDirs));
   return FALSE;
}
BOOL WIN32API PathFindOnPathW(LPWSTR sFile, LPCWSTR sOtherDirs)
{  dprintf(("SHELL32: %s %s\n",debugstr_w(sFile), debugstr_w(sOtherDirs)));
   return FALSE;
}
BOOL WIN32API PathFindOnPathAW(LPVOID sFile, LPCVOID sOtherDirs)
{
  if (VERSION_OsIsUnicode())
    return PathFindOnPathW((LPWSTR)sFile,
                           (LPWSTR)sOtherDirs);
  else
    return PathFindOnPathA((LPTSTR)sFile,
                           (LPTSTR)sOtherDirs);
}

/*************************************************************************
 * PathGetExtension [SHELL32.158]
 *
 * NOTES
 *     exported by ordinal
 */
LPCSTR WIN32API PathGetExtensionA(LPCSTR path,DWORD y,DWORD z)
{  dprintf(("SHELL32: (%s,%08lx,%08lx)\n",path,y,z));
   path = PathFindExtensionA(path);
   return *path?(path+1):path;
}
LPCWSTR WIN32API PathGetExtensionW(LPCWSTR path,DWORD y,DWORD z)
{  dprintf(("SHELL32: (L%s,%08lx,%08lx)\n",debugstr_w(path),y,z));
   path = PathFindExtensionW(path);
   return *path?(path+1):path;
}
LPCVOID WIN32API PathGetExtensionAW(LPCVOID path,DWORD y,DWORD z)
{
  if (VERSION_OsIsUnicode())
    return PathGetExtensionW((LPCWSTR)path,y,z);
  else
    return PathGetExtensionA((LPCTSTR)path,y,z);
}

/*************************************************************************
 * SheGetDirW [SHELL32.281]
 *
 */
HRESULT WIN32API SheGetDirW(LPWSTR u, LPWSTR v)
{  dprintf(("SHELL32: %p %p stub\n",u,v));
   return 0;
}

/*************************************************************************
 * SheChangeDirW [SHELL32.274]
 *
 */
HRESULT WIN32API SheChangeDirW(LPWSTR u)
{  dprintf(("SHELL32: (%s),stub\n",debugstr_w(u)));
   return 0;
}

/*************************************************************************
*  PathProcessCommand                    [SHELL32.653]
*/
HRESULT WIN32API PathProcessCommand (DWORD u, DWORD v, DWORD w, DWORD x)
{  dprintf(("SHELL32: 0x%04lx 0x%04lx 0x%04lx 0x%04lx stub\n",u,v,w,x));
   return 0;
}

/*************************************************************************
 * SHGetSpecialFolderPath [SHELL32.175]
 *
 * converts csidl to path
 *
 */
BOOL WIN32API SHGetSpecialFolderPathA (DWORD x1,LPSTR szPath,DWORD csidl,DWORD x4)
{  LPITEMIDLIST pidl;

   dprintf(("SHELL32: (0x%04lx,%p,csidl=%lu,0x%04lx) semi-stub\n", x1,szPath,csidl,x4));

   SHGetSpecialFolderLocation(0, csidl, &pidl);
   SHGetPathFromIDListA (pidl, szPath);
   SHFree (pidl);
   return TRUE;
}
BOOL WIN32API SHGetSpecialFolderPathW (DWORD x1,LPWSTR szPath, DWORD csidl,DWORD x4)
{  LPITEMIDLIST pidl;

   dprintf(("SHELL32: (0x%04lx,%p,csidl=%lu,0x%04lx) semi-stub\n", x1,szPath,csidl,x4));

   SHGetSpecialFolderLocation(0, csidl, &pidl);
   SHGetPathFromIDListW (pidl, szPath);
   SHFree (pidl);
   return TRUE;
}
BOOL WIN32API SHGetSpecialFolderPath (DWORD x1,LPVOID szPath,DWORD csidl,DWORD x4)
{
  if (VERSION_OsIsUnicode())
    return SHGetSpecialFolderPathW ( x1, (LPWSTR)szPath, csidl, x4);
  else
    return SHGetSpecialFolderPathA ( x1, (LPTSTR)szPath, csidl, x4);
}

