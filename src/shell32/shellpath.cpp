/* $Id: shellpath.cpp,v 1.6 2000-03-22 16:55:51 cbratschi Exp $ */

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
#define HAVE_WCTYPE_H
#include <odin.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"
#include "crtdll.h"

#include "shlobj.h"
#include "shell32_main.h"

#include <heapstring.h>
#include <misc.h>


ODINDEBUGCHANNEL(SHELL32-SHELLPATH)


/*************************************************************************
 * PathIsRoot [SHELL32.29]
 */
ODINFUNCTION1(BOOL, PathIsRootA,
              LPCSTR, x)
{  TRACE("%s\n",x);
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
ODINFUNCTION1(BOOL, PathIsRootW,
              LPCWSTR, x)
{  TRACE("%s\n",debugstr_w(x));
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
 * PathFindFilename [SHELL32.34]
 *
 * NOTES
 *     basename(char *fn);
 */
ODINFUNCTION1(LPCSTR, PathFindFilenameA,
              LPCSTR, aptr)
{  LPCSTR aslash;
   aslash = aptr;

   TRACE("%s\n",aslash);
   while (aptr[0])
   { if (((aptr[0]=='\\') || (aptr[0]==':')) && aptr[1] && aptr[1]!='\\')
         aslash = aptr+1;
     aptr++;
   }
   return aslash;

}
ODINFUNCTION1(LPCWSTR, PathFindFilenameW,
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
ODINFUNCTION1(LPCVOID, PathFindFilenameAW,
              LPCVOID, fn)
{
   if(VERSION_OsIsUnicode())
     return PathFindFilenameW((LPWSTR)fn);
   return PathFindFilenameA((LPSTR)fn);
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
  while (*name && *mask && *mask!=';') {
    if (*mask=='*') {
      do {
   if (PathMatchSingleMaskW(name,mask+1)) return 1;  /* try substrings */
      } while (*name++);
      return 0;
    }
    if (towupper(*mask)!=towupper(*name) && *mask!='?') return 0;
    name++;
    mask++;
  }
  if (!*name) {
    while (*mask=='*') mask++;
    if (!*mask || *mask==';') return 1;
  }
  return 0;
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

  if (!lstrcmpA( mask, "*.*" )) return 1;   /* we don't require a period */

  while (*mask) {
    if (PathMatchSingleMaskA(name,mask)) return 1;    /* helper function */
    while (*mask && *mask!=';') mask++;
    if (*mask==';') {
      mask++;
      while (*mask==' ') mask++;      /*  masks may be separated by "; " */
     }
   }
  return 0;
}
ODINFUNCTION2(BOOL, PathMatchSpecW,
              LPCWSTR, name,
              LPCWSTR, mask)
{  WCHAR stemp[4];
  TRACE("%ls %ls\n",name,mask);
   lstrcpyAtoW(stemp,"*.*");
  if (!lstrcmpW( mask, stemp )) return 1;   /* we don't require a period */

  while (*mask) {
    if (PathMatchSingleMaskW(name,mask)) return 1;    /* helper function */
    while (*mask && *mask!=';') mask++;
    if (*mask==';') {
      mask++;
      while (*mask==' ') mask++;       /* masks may be separated by "; " */
     }
   }
  return 0;
}
ODINFUNCTION2(BOOL, PathMatchSpecAW,
              LPVOID, name,
              LPVOID, mask)
{  if (VERSION_OsIsUnicode())
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
{
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
 * SheGetDirW [SHELL32.281]
 *
 */
ODINFUNCTION2(HRESULT, SheGetDirW,
              LPWSTR, u,
              LPWSTR, v)
{  FIXME("%p %p stub\n",u,v);
   return 0;
}

/*************************************************************************
 * SheChangeDirW [SHELL32.274]
 *
 */
ODINFUNCTION1(HRESULT, SheChangeDirW,
              LPWSTR, u)
{  FIXME("(%s),stub\n",debugstr_w(u));
   return 0;
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

/*************************************************************************
 * SHGetSpecialFolderPath [SHELL32.175]
 *
 * converts csidl to path
 *
 */

static char * szSHFolders = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";

ODINFUNCTION4(BOOL, SHGetSpecialFolderPathA,
              HWND, hwndOwner,
              LPSTR, szPath,
              DWORD, csidl,
              BOOL, bCreate)
{
   CHAR  szValueName[MAX_PATH], szDefaultPath[MAX_PATH];
   HKEY  hRootKey, hKey;
   BOOL  bRelative = TRUE;
   DWORD dwType, dwDisp, dwPathLen = MAX_PATH;

   TRACE("0x%04x,%p,csidl=%lu,0x%04x\n", hwndOwner,szPath,csidl,bCreate);

   /* build default values */
   switch(csidl)
   {
     case CSIDL_APPDATA:
       hRootKey = HKEY_CURRENT_USER;
       strcpy (szValueName, "AppData");
       strcpy (szDefaultPath, "AppData");
       break;

     case CSIDL_COOKIES:
       hRootKey = HKEY_CURRENT_USER;
       strcpy (szValueName, "Cookies");
       strcpy(szDefaultPath, "Cookies");
       break;

     case CSIDL_DESKTOPDIRECTORY:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Desktop");
       strcpy(szDefaultPath, "Desktop");
       break;

     case CSIDL_COMMON_DESKTOPDIRECTORY:
       hRootKey = HKEY_LOCAL_MACHINE;
       strcpy(szValueName, "Common Desktop");
       strcpy(szDefaultPath, "Desktop");
       break;

     case CSIDL_FAVORITES:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Favorites");
       strcpy(szDefaultPath, "Favorites");
       break;

     case CSIDL_FONTS:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Fonts");
       strcpy(szDefaultPath, "Fonts");
       break;

     case CSIDL_HISTORY:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "History");
       strcpy(szDefaultPath, "History");
       break;

     case CSIDL_NETHOOD:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "NetHood");
       strcpy(szDefaultPath, "NetHood");
       break;

     case CSIDL_INTERNET_CACHE:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Cache");
       strcpy(szDefaultPath, "Temporary Internet Files");
       break;

     case CSIDL_PERSONAL:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Personal");
       strcpy(szDefaultPath, "My Own Files");
       bRelative = FALSE;
       break;

     case CSIDL_PRINTHOOD:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "PrintHood");
       strcpy(szDefaultPath, "PrintHood");
       break;

     case CSIDL_PROGRAMS:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Programs");
       strcpy(szDefaultPath, "StatrMenu\\Programs");
       break;

     case CSIDL_COMMON_PROGRAMS:
       hRootKey = HKEY_LOCAL_MACHINE;
       strcpy(szValueName, "Common Programs");
       strcpy(szDefaultPath, "");
       break;

     case CSIDL_RECENT:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Recent");
       strcpy(szDefaultPath, "Recent");
       break;

     case CSIDL_SENDTO:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "SendTo");
       strcpy(szDefaultPath, "SendTo");
       break;

     case CSIDL_STARTMENU:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "StartMenu");
       strcpy(szDefaultPath, "StartMenu");
       break;

     case CSIDL_COMMON_STARTMENU:
       hRootKey = HKEY_LOCAL_MACHINE;
       strcpy(szValueName, "Common StartMenu");
       strcpy(szDefaultPath, "StartMenu");
       break;

     case CSIDL_STARTUP:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Startup");
       strcpy(szDefaultPath, "StartMenu\\Programs\\Startup");
       break;

     case CSIDL_COMMON_STARTUP:
       hRootKey = HKEY_LOCAL_MACHINE;
       strcpy(szValueName, "Common Startup");
       strcpy(szDefaultPath, "StartMenu\\Programs\\Startup");
       break;

     case CSIDL_TEMPLATES:
       hRootKey = HKEY_CURRENT_USER;
       strcpy(szValueName, "Templates");
       strcpy(szDefaultPath, "ShellNew");
       break;

     default:
       ERR("folder unknown or not allowed\n");
       return FALSE;
   }

   if (RegCreateKeyExA(hRootKey,szSHFolders,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hKey,&dwDisp))
   {
     return FALSE;
   }

   if (RegQueryValueExA(hKey,szValueName,NULL,&dwType,(LPBYTE)szPath,&dwPathLen))
   {
     /* value not existing */
     if (bRelative)
     {
       GetWindowsDirectoryA(szPath, MAX_PATH);
       PathAddBackslashA(szPath);
       strcat(szPath, szDefaultPath);
     }
     else
     {
       strcpy(szPath, szDefaultPath);
     }
     if (bCreate)
     {
       CreateDirectoryA(szPath,NULL);
     }
     RegSetValueExA(hKey,szValueName,0,REG_SZ,(LPBYTE)szPath,strlen(szPath)+1);
   }
   RegCloseKey(hKey);

   return TRUE;
}
ODINFUNCTION4(BOOL, SHGetSpecialFolderPathW,
              HWND, hwndOwner,
              LPWSTR, szPath,
              DWORD, csidl,
              BOOL, bCreate)
{
   char szTemp[MAX_PATH];

   if (SHGetSpecialFolderPathA(hwndOwner, szTemp, csidl, bCreate))
   {
     lstrcpynAtoW(szPath, szTemp, MAX_PATH);
   }

   TRACE("0x%04x,%p,csidl=%lu,0x%04x\n", hwndOwner,szPath,csidl,bCreate);

   return TRUE;
}
ODINFUNCTION4(BOOL, SHGetSpecialFolderPathAW,
              HWND, hwndOwner,
              LPVOID, szPath,
              DWORD, csidl,
              BOOL, bCreate)
{
   if (VERSION_OsIsUnicode())
     return SHGetSpecialFolderPathW (hwndOwner, (LPWSTR)szPath, csidl, bCreate);
   return SHGetSpecialFolderPathA (hwndOwner, (LPSTR)szPath, csidl, bCreate);
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

