/* $Id: path.cpp,v 1.2 2000-05-16 20:17:30 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Path Functions
 *
 * Many of this functions are in SHLWAPI.DLL also
 *
 * Corel WINE 20000324 level (without CRTDLL_* calls)
 */

/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <wcstr.h>
#define HAVE_WCTYPE_H

#include "debugtools.h"

#include <winreg.h>

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>
#include <winversion.h>
#include <winuser.h>

#include "shlwapi.h"


ODINDEBUGCHANNEL(SHLWAPI-SHELLPATH)


/* Supported protocols for PathIsURL */
LPSTR SupportedProtocol[] = {"http","https","ftp","gopher","file","mailto",""};

/*************************************************************************
 * PathIsRoot [SHELL32.29]
 */
ODINFUNCTION1(BOOL, PathIsRootA,
              LPCSTR, x)
{  TRACE("%s\n",x);
   if (*(x+1)==':' && *(x+2)=='\\')         /* "X:\" */
     return 1;
   if (*x=='\\' || *x=='/')                         /* "\" */
     return 1;
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
ODINFUNCTION1(BOOL, PathIsRootW,
              LPCWSTR, x)
{  TRACE("%s\n",debugstr_w(x));
   if (*(x+1)==':' && *(x+2)=='\\')         /* "X:\" */
     return 1;
   if (*x == (WCHAR) '\\' || *x == (WCHAR) '/')                  /* "\" */
     return 1;
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
ODINFUNCTION1(BOOL, PathIsRootAW,
              LPCVOID, x)
{  if (VERSION_OsIsUnicode())
     return PathIsRootW((LPWSTR)x);
   return PathIsRootA((LPSTR)x);

}
/*************************************************************************
 * PathBuildRoot [SHELL32.30]
 */
LPSTR WINAPI PathBuildRootA(LPSTR root, BYTE drive)
{
  TRACE("%p %i\n",root, drive);
   strcpy(root,"A:\\");
   root[0]+=drive;
   return root;
}

/*************************************************************************
 */
LPWSTR WINAPI PathBuildRootW(LPWSTR root, BYTE drive)
{
   lstrcpyW(root, (LPWSTR)L"A:\\");
   root[0]+=drive;
   return root;
}

/*************************************************************************
 * PathFindExtension [SHELL32.31]
 *
 * NOTES
 *     returns pointer to last . in last pathcomponent or at \0.
 */
ODINFUNCTION1(LPCSTR, PathFindExtensionA,
              LPCSTR, path)
{  LPCSTR   lastpoint = NULL;
   TRACE("%p %s\n",path,path);
   while (*path)
   { if (*path=='\\'||*path==' ')
       lastpoint=NULL;
     if (*path=='.')
       lastpoint=path;
     path++;
   }
   return lastpoint?lastpoint:path;
}
ODINFUNCTION1(LPCWSTR, PathFindExtensionW,
              LPCWSTR, path)
{  LPCWSTR   lastpoint = NULL;
   TRACE("%p L%s\n",path,debugstr_w(path));
   while (*path)
   { if (*path==(WCHAR)'\\'||*path==(WCHAR)' ')
       lastpoint=NULL;
     if (*path==(WCHAR)'.')
       lastpoint=path;
     path++;
   }
   return lastpoint?lastpoint:path;
}
ODINFUNCTION1(LPCVOID, PathFindExtensionAW,
              LPCVOID, path)
{  if (VERSION_OsIsUnicode())
     return PathFindExtensionW((LPWSTR)path);
   return PathFindExtensionA((LPSTR)path);

}

/*************************************************************************
 * PathAddBackslash [SHELL32.32]
 *
 * NOTES
 *     append \ if there is none
 */
ODINFUNCTION1(LPSTR, PathAddBackslashA,
              LPSTR, path)
{  int len;
   TRACE("%p->%s\n",path,path);

   len = strlen(path);
   if (len && path[len-1]!='\\')
   { path[len]  = '\\';
     path[len+1]= 0x00;
     return path+len+1;
   }
   return path+len;
}
ODINFUNCTION1(LPWSTR, PathAddBackslashW,
              LPWSTR, path)
{  int len;
   TRACE("%p->%s\n",path,debugstr_w(path));

   len = lstrlenW(path);
   if (len && path[len-1]!=(WCHAR)'\\')
   { path[len]  = (WCHAR)'\\';
     path[len+1]= 0x00;
     return path+len+1;
   }
   return path+len;
}
ODINFUNCTION1(LPVOID, PathAddBackslashAW,
              LPVOID, path)
{  if(VERSION_OsIsUnicode())
     return PathAddBackslashW((LPWSTR)path);
   return PathAddBackslashA((LPSTR)path);
}

/*************************************************************************
 * PathRemoveBlanks [SHELL32.33]
 *
 * NOTES
 *     remove spaces from beginning and end of passed string
 */
ODINFUNCTION1(LPSTR, PathRemoveBlanksA,
              LPSTR, str)
{  LPSTR x = str;
   TRACE("%s\n",str);
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
ODINFUNCTION1(LPWSTR, PathRemoveBlanksW,
              LPWSTR, str)
{  LPWSTR x = str;
   TRACE("%s\n",debugstr_w(str));
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
ODINFUNCTION1(LPVOID, PathRemoveBlanksAW,
              LPVOID, str)
{  if(VERSION_OsIsUnicode())
     return PathRemoveBlanksW((LPWSTR)str);
   return PathRemoveBlanksA((LPSTR)str);
}



/*************************************************************************
 * PathFindFileName [SHELL32.34]
 *
 * NOTES
 *     basename(char *fn);
 */
ODINFUNCTION1(LPCSTR, PathFindFileNameA,
              LPCSTR, aptr)
{
  int iLength = strlen(aptr);
  LPSTR strSlash = (LPSTR)aptr + iLength - 1;
  
  while (iLength--)
  {
    if ( (*strSlash == '/' ) ||
         (*strSlash == '\\') )
      return strSlash + 1;
  }
  
  // no slash found, look for ":"
  if (aptr[1] == ':')
    return aptr+2;
  
  return aptr; // no slash and no drive found
}

ODINFUNCTION1(LPCWSTR, PathFindFileNameW,
              LPCWSTR, wptr)
{  LPCWSTR wslash;
   wslash = wptr;

   TRACE("L%s\n",debugstr_w(wslash));
   while (wptr[0])
   { if (((wptr[0]=='\\') || (wptr[0]==':')) && wptr[1] && wptr[1]!='\\')
       wslash = wptr+1;
     wptr++;
   }
   return wslash;
}
ODINFUNCTION1(LPCVOID, PathFindFileNameAW,
              LPCVOID, fn)
{
   if(VERSION_OsIsUnicode())
     return PathFindFileNameW((LPWSTR)fn);
   return PathFindFileNameA((LPSTR)fn);
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
ODINFUNCTION1(DWORD, PathRemoveFileSpecA,
              LPSTR, fn)
{
   LPSTR x,cutplace;
  TRACE("%s\n",fn);
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
LPSTR WINAPI PathAppendA(LPSTR x1, LPSTR x2)
{
  TRACE("%s %s\n",x1,x2);
  while (x2[0]=='\\') x2++;
  return PathCombineA(x1,x1,x2);
}

/*************************************************************************
 * PathAppend [SHELL32.36]
 *
 * NOTES
 *     concat_paths(char*target,const char*add);
 *     concats "target\\add" and writes them to target
 */
LPWSTR WINAPI PathAppendW(LPWSTR x1, LPWSTR x2)
{
  while (x2[0] == (WCHAR)'\\') x2++;
  return PathCombineW(x1,x1,x2);
}

/*************************************************************************
 * PathCombine [SHELL32.37]
 *
 * NOTES
 *  if lpszFile='.' skip it
 *  szDest can be equal to lpszFile. Thats why we use sTemp
 */
ODINFUNCTION3(LPSTR, PathCombineA,
              LPSTR, szDest,
              LPCSTR, lpszDir,
              LPCSTR, lpszFile)
{  char sTemp[MAX_PATH];
   TRACE("%p %p->%s %p->%s\n",szDest, lpszDir, lpszDir, lpszFile, lpszFile);


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
ODINFUNCTION3(LPWSTR, PathCombineW,
              LPWSTR, szDest,
              LPCWSTR, lpszDir,
              LPCWSTR, lpszFile)
{  WCHAR sTemp[MAX_PATH];
   TRACE("%p %p->%s %p->%s\n",szDest, lpszDir, debugstr_w(lpszDir),
          lpszFile, debugstr_w(lpszFile));


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
ODINFUNCTION3(LPVOID, PathCombineAW,
              LPVOID, szDest,
              LPCVOID, lpszDir,
              LPCVOID, lpszFile)
{  if (VERSION_OsIsUnicode())
     return PathCombineW( (LPWSTR)szDest, (LPWSTR)lpszDir, (LPWSTR)lpszFile );
   return PathCombineA( (LPSTR)szDest, (LPSTR)lpszDir, (LPSTR)lpszFile );
}

/*************************************************************************
 * PathIsUNC [SHELL32.39]
 *
 * NOTES
 *     PathIsUNC(char*path);
 */
ODINFUNCTION1(BOOL, PathIsUNCA,
              LPCSTR, path)
{  TRACE("%s\n",path);

   if ((path[0]=='\\') && (path[1]=='\\'))
     return TRUE;
   return FALSE;
}
ODINFUNCTION1(BOOL, PathIsUNCW,
              LPCWSTR, path)
{  TRACE("%s\n",debugstr_w(path));

   if ((path[0]=='\\') && (path[1]=='\\'))
     return TRUE;
   return FALSE;
}
ODINFUNCTION1(BOOL, PathIsUNCAW,
              LPCVOID, path)
{  if (VERSION_OsIsUnicode())
     return PathIsUNCW( (LPWSTR)path );
   return PathIsUNCA( (LPSTR)path );
}
/*************************************************************************
 *  PathIsRelativ [SHELL32.40]
 *
 */
ODINFUNCTION1(BOOL, PathIsRelativeA,
              LPCSTR, path)
{  TRACE("path=%s\n",path);

   if (path && (path[0]!='\\' && path[1]==':'))
     return TRUE;
   return FALSE;
}
ODINFUNCTION1(BOOL, PathIsRelativeW,
              LPCWSTR, path)
{  TRACE("path=%s\n",debugstr_w(path));

   if (path && (path[0]!='\\' && path[1]==':'))
     return TRUE;
   return FALSE;
}
ODINFUNCTION1(BOOL, PathIsRelativeAW,
              LPCVOID, path)
{  if (VERSION_OsIsUnicode())
     return PathIsRelativeW( (LPWSTR)path );
   return PathIsRelativeA( (LPSTR)path );
}
/*************************************************************************
 *  PathIsExe [SHELL32.43]
 *
 */
ODINFUNCTION1(BOOL, PathIsExeA,
              LPCSTR, path)
{  FIXME("path=%s\n",path);
   return FALSE;
}
ODINFUNCTION1(BOOL, PathIsExeW,
              LPCWSTR, path)
{  FIXME("path=%s\n",debugstr_w(path));
   return FALSE;
}
ODINFUNCTION1(BOOL, PathIsExeAW,
              LPCVOID, path)
{  if (VERSION_OsIsUnicode())
     return PathIsExeW ((LPWSTR)path);
   return PathIsExeA((LPSTR)path);
}

/*************************************************************************
 * PathFileExists [SHELL32.45]
 *
 * NOTES
 *     file_exists(char *fn);
 */
ODINFUNCTION1(BOOL, PathFileExistsA,
              LPSTR, fn)
{
  TRACE("%s\n",fn);
   if (GetFileAttributesA(fn)==-1)
      return FALSE;
    else
      return TRUE;
}
/*************************************************************************
 * PathFileExists [SHELL32.45]
 *
 * NOTES
 *     file_exists(char *fn);
 */
ODINFUNCTION1(BOOL, PathFileExistsW,
              LPWSTR, fn)
{
   if (GetFileAttributesW(fn)==-1)
      return FALSE;
    else
      return TRUE;
}
/*************************************************************************
 * PathMatchSingleMask
 *
 * NOTES
 *     internal (used by PathMatchSpec)
 */
static BOOL PathMatchSingleMaskA(LPCSTR name, LPCSTR mask)
{
  while (*name && *mask && *mask!=';') {
    if (*mask=='*') {
      do {
   if (PathMatchSingleMaskA(name,mask+1)) return 1;  /* try substrings */
      } while (*name++);
      return 0;
    }
    if (toupper(*mask)!=toupper(*name) && *mask!='?') return 0;
    name++;
    mask++;
  }
  if (!*name) {
    while (*mask=='*') mask++;
    if (!*mask || *mask==';') return 1;
  }
  return 0;
}

static BOOL PathMatchSingleMaskW(LPCWSTR name, LPCWSTR mask)
{
  while (*name && *mask && (*mask != ';'))
  {
    if (*mask == '*')
    {
      do
      {
        if (PathMatchSingleMaskW(name,mask+1)) return 1;  /* try substrings */
      } while (*name++);
      return FALSE;
    }
    if ((towupper(*mask) != towupper(*name)) && (*mask != '?')) return 0;
    name++;
    mask++;
  }
  if (!*name)
  {
    while (*mask == '*') mask++;
    if (!*mask || (*mask == ';')) return TRUE;
  }

  return FALSE;
}
/*************************************************************************
 * PathMatchSpec [SHELL32.46]
 *
 * NOTES
 *     used from COMDLG32
 */
ODINFUNCTION2(BOOL, PathMatchSpecA,
              LPCSTR, name,
              LPCSTR, mask)
{
  TRACE("%s %s\n",name,mask);

  if (!lstrcmpA( mask, "*.*" )) return TRUE;   /* we don't require a period */

  while (*mask)
  {
    if (PathMatchSingleMaskA(name,mask)) return TRUE;    /* helper function */
    while (*mask && *mask!=';') mask++;
    if (*mask==';')
    {
      mask++;
      while (*mask==' ') mask++;      /*  masks may be separated by "; " */
    }
  }

  return FALSE;
}

ODINFUNCTION2(BOOL, PathMatchSpecW,
              LPCWSTR, name,
              LPCWSTR, mask)
{
  TRACE("%ls %ls\n",name,mask);

  if (!lstrcmpW( mask, (WCHAR*)L"*.*" )) return TRUE;   /* we don't require a period */

  while (*mask)
  {
    if (PathMatchSingleMaskW(name,mask)) return TRUE;    /* helper function */
    while (*mask && (*mask != ';')) mask++;
    if (*mask == ';')
    {
      mask++;
      while (*mask==' ') mask++;       /* masks may be separated by "; " */
    }
  }

  return FALSE;
}
ODINFUNCTION2(BOOL, PathMatchSpecAW,
              LPVOID, name,
              LPVOID, mask)
{
   if (VERSION_OsIsUnicode())
     return PathMatchSpecW( (LPWSTR)name, (LPWSTR)mask );
   return PathMatchSpecA( (LPSTR)name, (LPSTR)mask );
}
/*************************************************************************
 * PathSetDlgItemPathAW [SHELL32.48]
 * NOTES
 *  use PathCompactPath to make sure, the path fits into the control
 */

ODINFUNCTION3(BOOL, PathSetDlgItemPathA,
              HWND, hDlg,
              int, id,
              LPCSTR, pszPath)
{  TRACE("%x %x %s\n",hDlg, id, pszPath);
   return SetDlgItemTextA(hDlg, id, pszPath);
}
ODINFUNCTION3(BOOL, PathSetDlgItemPathW,
              HWND, hDlg,
              int, id,
              LPCWSTR, pszPath)
{  TRACE("%x %x %s\n",hDlg, id, debugstr_w(pszPath));
   return SetDlgItemTextW(hDlg, id, pszPath);
}
ODINFUNCTION3(BOOL, PathSetDlgItemPathAW,
              HWND, hDlg,
              int, id,
              LPCVOID, pszPath)
{  if (VERSION_OsIsUnicode())
     return PathSetDlgItemPathW(hDlg, id, (LPWSTR)pszPath);
   return PathSetDlgItemPathA(hDlg, id, (LPSTR)pszPath);
}

/*************************************************************************
 * PathQualifyAW [SHELL32.49]
 */

ODINFUNCTION1(BOOL, PathQualifyA,
              LPCSTR, pszPath)
{  FIXME("%s\n",pszPath);
   return 0;
}
ODINFUNCTION1(BOOL, PathQualifyW,
              LPCWSTR, pszPath)
{  FIXME("%s\n",debugstr_w(pszPath));
   return 0;
}
ODINFUNCTION1(BOOL, PathQualifyAW,
              LPCVOID, pszPath)
{  if (VERSION_OsIsUnicode())
     return PathQualifyW((LPWSTR)pszPath);
   return PathQualifyA((LPSTR)pszPath);
}

/*************************************************************************
 * PathResolve [SHELL32.51]
 */
ODINFUNCTION3(DWORD, PathResolve,
              LPCSTR, s,
              DWORD, x2,
              DWORD, x3)
{
   FIXME("(%s,0x%08lx,0x%08lx),stub!\n",s,x2,x3);
   return 0;
}

/*************************************************************************
 * PathGetArgs [SHELL32.52]
 *
 * NOTES
 *     look for next arg in string. handle "quoted" strings
 *     returns pointer to argument *AFTER* the space. Or to the \0.
 */
ODINFUNCTION1(LPCSTR, PathGetArgsA,
              LPCSTR, cmdline)
{  BOOL  qflag = FALSE;

   TRACE("%s\n",cmdline);

   while (*cmdline)
   { if ((*cmdline==' ') && !qflag)
       return cmdline+1;
     if (*cmdline=='"')
       qflag=!qflag;
     cmdline++;
   }
   return cmdline;

}
ODINFUNCTION1(LPCWSTR, PathGetArgsW,
              LPCWSTR, cmdline)
{  BOOL  qflag = FALSE;

   TRACE("%sL\n",debugstr_w(cmdline));

   while (*cmdline)
   { if ((*cmdline==' ') && !qflag)
       return cmdline+1;
     if (*cmdline=='"')
       qflag=!qflag;
     cmdline++;
   }
   return cmdline;
}
ODINFUNCTION1(LPCVOID, PathGetArgsAW,
              LPVOID, cmdline)
{  if (VERSION_OsIsUnicode())
     return PathGetArgsW((LPWSTR)cmdline);
   return PathGetArgsA((LPSTR)cmdline);
}
/*************************************************************************
 * PathQuoteSpaces [SHELL32.55]
 *
 * NOTES
 *     basename(char *fn);
 */
ODINFUNCTION1(LPSTR, PathQuoteSpacesA,
              LPCSTR, aptr)
{  FIXME("%s\n",aptr);
   return 0;

}
ODINFUNCTION1(LPWSTR, PathQuoteSpacesW,
              LPCWSTR, wptr)
{  FIXME("L%s\n",debugstr_w(wptr));
   return 0;
}
ODINFUNCTION1(LPVOID, PathQuoteSpacesAW,
              LPCVOID, fn)
{  if(VERSION_OsIsUnicode())
     return PathQuoteSpacesW((LPWSTR)fn);
   return PathQuoteSpacesA((LPSTR)fn);
}


/*************************************************************************
 * PathUnquoteSpaces [SHELL32.56]
 *
 * NOTES
 *     unquote string (remove ")
 */
ODINPROCEDURE1(PathUnquoteSpacesA,
               LPSTR, str)
{  DWORD      len = lstrlenA(str);
   TRACE("%s\n",str);
   if (*str!='"')
     return;
   if (str[len-1]!='"')
     return;
   str[len-1]='\0';
   lstrcpyA(str,str+1);
   return;
}
ODINPROCEDURE1(PathUnquoteSpacesW,
               LPWSTR, str)
{  DWORD len = lstrlenW(str);

   TRACE("%s\n",debugstr_w(str));

   if (*str!='"')
     return;
   if (str[len-1]!='"')
     return;
   str[len-1]='\0';
   lstrcpyW(str,str+1);
   return;
}
ODINPROCEDURE1(PathUnquoteSpacesAW,
               LPVOID, str)
{  if(VERSION_OsIsUnicode())
     PathUnquoteSpacesW((LPWSTR)str);
   PathUnquoteSpacesA((LPSTR)str);
}


/*************************************************************************
 * PathGetDriveNumber32 [SHELL32.57]
 *
 */
ODINFUNCTION1(HRESULT, PathGetDriveNumberAW,
              LPSTR, u)
{  FIXME("%s stub\n",debugstr_a(u));
   return 0;
}

/*************************************************************************
 * PathGetDriveNumber32 [SHELL32.57]
 *
 */
ODINFUNCTION1(HRESULT, PathGetDriveNumberA,
              LPSTR, u)
{  FIXME("%s stub\n",debugstr_a(u));
   return 0;
}

/*************************************************************************
 * PathGetDriveNumber32 [SHELL32.57]
 *
 */
ODINFUNCTION1(HRESULT, PathGetDriveNumberW,
              LPWSTR, u)
{  FIXME("%s stub\n",debugstr_a(u));
   return 0;
}

/*************************************************************************
 * PathYetAnotherMakeUniqueName [SHELL32.75]
 *
 * NOTES
 *     exported by ordinal
 */
ODINFUNCTION2(BOOL, PathYetAnotherMakeUniqueNameA,
              LPDWORD, x,
              LPDWORD, y)
{
    FIXME("(%p,%p):stub.\n",x,y);
    return TRUE;
}

/*************************************************************************
 * IsLFNDrive [SHELL32.119]
 *
 * NOTES
 *     exported by ordinal Name
 */
ODINFUNCTION1(BOOL, IsLFNDriveA,
              LPCSTR, path)
{
    DWORD   fnlen;

    if (!GetVolumeInformationA(path,NULL,0,NULL,&fnlen,NULL,NULL,0))
   return FALSE;
    return fnlen>12;
}
/*************************************************************************
 * PathFindOnPath [SHELL32.145]
 */
ODINFUNCTION2(BOOL, PathFindOnPathA,
              LPSTR, sFile,
              LPCSTR, sOtherDirs)
{  FIXME("%s %s\n",sFile, sOtherDirs);
   return FALSE;
}
ODINFUNCTION2(BOOL, PathFindOnPathW,
              LPWSTR, sFile,
              LPCWSTR, sOtherDirs)
{  FIXME("%s %s\n",debugstr_w(sFile), debugstr_w(sOtherDirs));
   return FALSE;
}
ODINFUNCTION2(BOOL, PathFindOnPathAW,
              LPVOID, sFile,
              LPCVOID, sOtherDirs)
{  if (VERSION_OsIsUnicode())
     return PathFindOnPathW((LPWSTR)sFile, (LPWSTR)sOtherDirs);
   return PathFindOnPathA((LPSTR)sFile, (LPSTR)sOtherDirs);
}

/*************************************************************************
 * PathGetExtension [SHELL32.158]
 *
 * NOTES
 *     exported by ordinal
 */
ODINFUNCTION3(LPCSTR, PathGetExtensionA,
              LPCSTR, path,
              DWORD, y,
              DWORD, z)
{  TRACE("(%s,%08lx,%08lx)\n",path,y,z);
   path = PathFindExtensionA(path);
   return *path?(path+1):path;
}
ODINFUNCTION3(LPCWSTR, PathGetExtensionW,
              LPCWSTR, path,
              DWORD, y,
              DWORD, z)
{  TRACE("(L%s,%08lx,%08lx)\n",debugstr_w(path),y,z);
   path = PathFindExtensionW(path);
   return *path?(path+1):path;
}
ODINFUNCTION3(LPCVOID, PathGetExtensionAW,
              LPCVOID, path,
              DWORD, y,
              DWORD, z)
{  if (VERSION_OsIsUnicode())
     return PathGetExtensionW((LPWSTR)path,y,z);
   return PathGetExtensionA((LPSTR)path,y,z);
}

/*************************************************************************
 * PathCleanupSpec                                [SHELL32.171]
 *
 */
ODINFUNCTION2(DWORD, PathCleanupSpecA,
              LPSTR, x,
              LPSTR, y)
{
   FIXME("%p(%s) %p(%s) stub\n",x,x,y,y);
   return TRUE;
}

ODINFUNCTION2(DWORD, PathCleanupSpecW,
              LPWSTR, x,
              LPWSTR, y)
{
   FIXME("%p(%s) %p(%s) stub\n",x,debugstr_w(x),y,debugstr_w(y));
   return TRUE;
}

ODINFUNCTION2(DWORD, PathCleanupSpecAW,
              LPVOID, x,
              LPVOID, y)
{
   if (VERSION_OsIsUnicode())
     return PathCleanupSpecW((LPWSTR)x,(LPWSTR)y);
   return PathCleanupSpecA((LPSTR)x,(LPSTR)y);
}


/*************************************************************************
*  PathProcessCommand                    [SHELL32.653]
*/
ODINFUNCTION4(HRESULT, PathProcessCommandA,
              LPSTR, lpCommand,
              LPSTR, v,
              DWORD, w,
              DWORD, x)
{
   FIXME("%p(%s) %p 0x%04lx 0x%04lx stub\n",
   lpCommand, lpCommand, v, w,x );
   lstrcpyA(v,lpCommand);
   return 0;
}

ODINFUNCTION4(HRESULT, PathProcessCommandW,
              LPWSTR, lpCommand,
              LPSTR, v,
              DWORD, w,
              DWORD, x)
{
   FIXME("%p(%s) %p 0x%04lx 0x%04lx stub\n",
   lpCommand, debugstr_w(lpCommand), v, w,x );
   return 0;
}

ODINFUNCTION4(HRESULT, PathProcessCommandAW,
              LPVOID, lpCommand,
              LPSTR, v,
              DWORD, w,
              DWORD, x)
{
   if (VERSION_OsIsUnicode())
     return PathProcessCommandW((LPWSTR)lpCommand, v, w, x);
   return PathProcessCommandA((LPSTR)lpCommand, v, w, x);
}


/* PathRemoveBackslash
 *
 * If the path ends in a backslash it is replaced by a NULL
 * and the address of the NULL is returned
 * Otherwise
 * the address of the last character is returned.
 *
 */
LPSTR WINAPI PathRemoveBackslashA(LPSTR lpPath)
{
        LPSTR temp = lpPath;
        LPSTR prev = lpPath;

        while (*temp)
        {
                prev = temp++;
        }
        if ( *prev == (CHAR)'\\')
        {
                *prev = (CHAR)'\0';
        }

        return prev;
}

LPWSTR WINAPI PathRemoveBackslashW(LPWSTR lpPath)
{
        LPWSTR temp = lpPath;
        LPWSTR prev = lpPath;

        while (*temp)
        {
                prev = temp++;
        }
        if ( *prev == (WCHAR)'\\')
        {
                *prev = (WCHAR)'\0';
        }

        return prev;
}

/*
   shlwapi functions that have found their way in because most of
   shlwapi is unimplemented and doesn't have a home.

   FIXME: move to a more appropriate file( when one exists )
*/

 /* SHGetValue: Gets a value from the registry */


BOOL WINAPI PathIsURLA(LPCSTR lpstrPath)
{
  LPSTR lpstrRes;
  char lpstrFileType[10] = "";
  int iSize;
  int i = 0;
  /* sanity check */
  if(!lpstrPath)
    return FALSE;

  /* get protocol        */
  /* protocol://location */
  if(!(lpstrRes = strchr(lpstrPath,':')))
  {
    return FALSE;
  }
  iSize = lpstrRes - lpstrPath;
  if(iSize > sizeof(lpstrFileType))
    return FALSE;

  strncpy(lpstrFileType,lpstrPath,iSize);

  while(strlen(SupportedProtocol[i]))
  {
    if(!stricmp(lpstrFileType,SupportedProtocol[i++]))
      return TRUE;
  }

  return FALSE;
}


/*****************************************************************************
 * Name      : LPSTR PathGetShortPathA
 * Purpose   : unknown
 * Parameters: LPSTR lpszPath
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.??
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/05/14 01:02]
 *****************************************************************************/

ODINFUNCTION1(LPSTR,   PathGetShortPathA,
              LPSTR,   lpszPath)
{
  dprintf(("stub: lpszPath=[%s]", lpszPath));
  return NULL;
}


/*****************************************************************************
 * Name      : LPWSTR PathGetShortPathW
 * Purpose   : unknown
 * Parameters: LPWSTR lpszPath
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.??
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/05/14 01:02]
 *****************************************************************************/

ODINFUNCTION1(LPWSTR,   PathGetShortPathW,
              LPWSTR,   lpszPath)
{
  dprintf(("stub: lpszPath=[%08xh]", lpszPath));
  return NULL;
}


/*****************************************************************************
 * Name      : LPVOID PathGetShortPathAW
 * Purpose   : unknown
 * Parameters: LPVOID lpszPath
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.92
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/05/14 01:02]
 *****************************************************************************/

ODINFUNCTION1(LPVOID,   PathGetShortPathAW,
              LPVOID,   lpszPath)
{
  if (VERSION_OsIsUnicode())
    return PathGetShortPathW((LPWSTR)lpszPath);
  else
    return PathGetShortPathA((LPSTR)lpszPath);
}


/*****************************************************************************
 * Name      : PathStripToRootA
 * Purpose   : return root (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(LPSTR,PathStripToRootA,LPSTR,pszPath)
{
  dprintf(("not implemented"));

  return NULL;
}

/*****************************************************************************
 * Name      : PathStripToRootW
 * Purpose   : return root (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION1(LPSTR,PathStripToRootW,LPSTR,pszPath)
{
  dprintf(("not implemented"));

  return NULL;
}

/*****************************************************************************
 * Name      : LPSTR PathSkipRootA
 * Purpose   : Parses a path, ignoring the drive letter or UNC server/share path parts.
 * Parameters: LPCSTR pszPath
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.PathSkipRootA
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 2000/01/31 23:02]
 *****************************************************************************/

ODINFUNCTION1(LPSTR, PathSkipRootA, LPCSTR, pszPath)
{
  // check if "driveletter:\"
  if (pszPath[1] == ':')
    return (LPSTR)(pszPath + 2);

  // check if UNC-style path
  if ( (pszPath[0] == '\\') &&
      (pszPath[1] == '\\') )
  {
    LPSTR pszTemp = strchr(pszPath + 2, '\\');
    if (NULL != pszTemp)
      // return root part, skip server/share
      return (LPSTR)pszTemp++;
    else
      // UNC syntax validation, return pszPath
      return (LPSTR)pszTemp;
  }

  // else ...
  return (LPSTR)pszPath;
}


/*****************************************************************************
 * Name      : LPWSTR PathSkipRootW
 * Purpose   : Parses a path, ignoring the drive letter or UNC server/share path parts.
 * Parameters: LPCWSTR pszPath
 * Variables :
 * Result    : unknown
 * Remark    : SHLWAPI.PathSkipRootW
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Mon, 2000/01/31 23:02]
 *****************************************************************************/

ODINFUNCTION1(LPWSTR, PathSkipRootW, LPCWSTR, pszPath)
{
  dprintf(("not implemented"));

  return (LPWSTR)pszPath;
}
