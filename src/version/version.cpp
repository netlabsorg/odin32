/* $Id: version.cpp,v 1.4 1999-07-23 07:30:49 sandervl Exp $ */

/*
 * Win32 Version resource APIs for OS/2
 *
 * Implementation of VERSION.DLL - File Installer routines
 * Copyright 1996,1997 Marcus Meissner
 * Copyright 1997      David Cuthbert
 * Copyright 1998      Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */


#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <unicode.h>
#include "version.h"

#include "lzexpand.h"


/******************************************************************************/
/******************************************************************************/
BOOL WIN32API VERSION_GetFileVersionInfoA(LPTSTR lpszFile,
                                          DWORD  dwHandle,
                                          DWORD  cbBuf,
                                          LPVOID lpvData)
{
   dprintf(("VERSION: GetFileVersionInfoA %s\n",
            lpszFile));

   return GetVersionStruct(lpszFile,
                           (char *)lpvData,
                           cbBuf);
}
/******************************************************************************/
/******************************************************************************/
BOOL WIN32API VERSION_GetFileVersionInfoW(LPWSTR lpszFile,
                                          DWORD  dwHandle,
                                          DWORD  cbBuf,
                                          LPVOID lpvData)
{
  BOOL  rc;
  char *astring = UnicodeToAsciiString(lpszFile);

  dprintf(("VERSION: GetFileVersionInfoW (%s,%08xh,%08xh,%08xh)\n",
           lpszFile,
           dwHandle,
           cbBuf,
           lpvData));

  rc = GetVersionStruct(astring, (char *)lpvData, cbBuf);
  FreeAsciiString(astring);
  return(rc);
}
/******************************************************************************/
/******************************************************************************/
DWORD WIN32API VERSION_GetFileVersionInfoSizeA(LPTSTR  lpszFile,
                                               LPDWORD lpdwHandle)
{
  dprintf(("VERSION: GetFileVersionInfoSizeA(%s,%08xh)\n",
           lpszFile,
           lpdwHandle));

  if(lpdwHandle)
    lpdwHandle = 0;

  return GetVersionSize(lpszFile);
}
/******************************************************************************/
/******************************************************************************/
DWORD WIN32API VERSION_GetFileVersionInfoSizeW(LPWSTR  lpszFile,
                                               LPDWORD lpdwHandle)
{
 char  *astring = UnicodeToAsciiString(lpszFile);
 DWORD rc;

 dprintf(("VERSION: GetFileVersionInfoSizeW(%08xh,%08xh)\n",
          lpszFile,
          lpdwHandle));

 if(lpdwHandle)
   lpdwHandle = 0;

 rc = GetVersionSize(astring);
 FreeAsciiString(astring);
 return(rc);
}
/******************************************************************************/
/******************************************************************************/
INT WIN32API lstrncmpiW( LPCWSTR str1, LPCWSTR str2, INT n )
{
    INT res;

    if (!n) return 0;
    while ((--n > 0) && *str1)
    {
        if ((res = towupper(*str1) - towupper(*str2)) != 0) return res;
        str1++;
        str2++;
    }
    return towupper(*str1) - towupper(*str2);
}
/***********************************************************************
 *           VersionInfo16_FindChild             [internal]
 */
VS_VERSION_INFO_STRUCT16 *VersionInfo16_FindChild( VS_VERSION_INFO_STRUCT16 *info, 
                                            LPCSTR szKey, UINT cbKey )
{
    VS_VERSION_INFO_STRUCT16 *child = VersionInfo16_Children( info );

    while ( (DWORD)child < (DWORD)info + info->wLength )
    {
        if ( !strnicmp( child->szKey, szKey, cbKey ) )
            return child;

	if (!(child->wLength)) return NULL;
        child = VersionInfo16_Next( child );
    }

    return NULL;
}
/***********************************************************************
 *           VersionInfo32_FindChild             [internal]
 */
VS_VERSION_INFO_STRUCT32 *VersionInfo32_FindChild( VS_VERSION_INFO_STRUCT32 *info, 
                                            LPCWSTR szKey, UINT cbKey )
{
    VS_VERSION_INFO_STRUCT32 *child = VersionInfo32_Children( info );

    while ( (DWORD)child < (DWORD)info + info->wLength )
    {
        if ( !lstrncmpiW( child->szKey, szKey, cbKey ) )
            return child;

        child = VersionInfo32_Next( child );
    }

    return NULL;
}
/******************************************************************************/
/******************************************************************************
 *           VerQueryValue32W              [VERSION.13]
 */
BOOL WIN32API VERSION_VerQueryValueW( LPVOID pBlock, LPCWSTR lpSubBlock,
                                      LPVOID *lplpBuffer, UINT *puLen )
{
    VS_VERSION_INFO_STRUCT32 *info = (VS_VERSION_INFO_STRUCT32 *)pBlock;
    if ( VersionInfoIs16( info ) )
    {
        dprintf(("VERSION: called on NE resource!\n"));
        return FALSE;
    }

    dprintf(("VERSION: (%p,%s,%p,%p)\n",
             pBlock, lpSubBlock, lplpBuffer, puLen ));

    while ( *lpSubBlock )
    {
        /* Find next path component */
        LPCWSTR lpNextSlash;
        for ( lpNextSlash = lpSubBlock; *lpNextSlash; lpNextSlash++ )
            if ( *lpNextSlash == '\\' )
                break;

        /* Skip empty components */
        if ( lpNextSlash == lpSubBlock )
        {
            lpSubBlock++;
            continue;
        }

        /* We have a non-empty component: search info for key */
        info = VersionInfo32_FindChild( info, lpSubBlock, lpNextSlash-lpSubBlock );
        if ( !info ) return FALSE;

        /* Skip path component */
        lpSubBlock = lpNextSlash;
    }

    /* Return value */
    *lplpBuffer = VersionInfo32_Value( info );
    *puLen = info->wValueLength;

    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
/***********************************************************************
 *           VerQueryValue32A              [VERSION.12]
 */
BOOL WIN32API VERSION_VerQueryValueA( LPVOID pBlock, LPCSTR lpSubBlock,
                                      LPVOID *lplpBuffer, UINT *puLen )
{
    VS_VERSION_INFO_STRUCT16 *info = (VS_VERSION_INFO_STRUCT16 *)pBlock;
    if ( !VersionInfoIs16( info ) )
    {
        // this is a quick hack, not much tested 
        WCHAR *ustring = (WCHAR *)malloc(strlen((char *)lpSubBlock)*2+1);
        LPVOID ubuffer;
        char *abuffer;
        UINT len = *puLen * 2;
        BOOL rc;

        dprintf(("VERSION: called on PE unicode resource!\n" ));

        AsciiToUnicode((char *)lpSubBlock, ustring);
        rc = VERSION_VerQueryValueW( pBlock, (LPCWSTR)ustring, &ubuffer, &len);
        if(lpSubBlock[0] == '\\' && lpSubBlock[1] == 0)
          *lplpBuffer = ubuffer;
        else
        {
          *lplpBuffer = malloc(len+1); // no free, memory leak
          UnicodeToAsciiN((WCHAR *)ubuffer, (char *)*lplpBuffer, len);
        }
        *puLen = len;
        free(ustring);
        return rc;
    }

    dprintf(("VERSION: (%p,%s,%p,%p)\n",
             pBlock, lpSubBlock, lplpBuffer, puLen ));

    while ( *lpSubBlock )
    {
        /* Find next path component */
        LPCSTR lpNextSlash;
        for ( lpNextSlash = lpSubBlock; *lpNextSlash; lpNextSlash++ )
            if ( *lpNextSlash == '\\' )
                break;

        /* Skip empty components */
        if ( lpNextSlash == lpSubBlock )
        {
            lpSubBlock++;
            continue;
        }

        /* We have a non-empty component: search info for key */
        info = VersionInfo16_FindChild( info, lpSubBlock, lpNextSlash-lpSubBlock );
        if ( !info ) return FALSE;

        /* Skip path component */
        lpSubBlock = lpNextSlash;
    }

    /* Return value */
    *lplpBuffer = VersionInfo16_Value( info );
    *puLen = info->wValueLength;

    return TRUE;
}
/******************************************************************************/


/******************************************************************************
 *
 *   void  ver_dstring(
 *      char const * prologue,
 *      char const * teststring,
 *      char const * epilogue )
 *
 *   This function will print via dprintf[_]ver to stddeb the prologue string,
 *   followed by the address of teststring and the string it contains if
 *   teststring is non-null or "(null)" otherwise, and then the epilogue
 *   string followed by a new line.
 *
 *   Revision history
 *      30-May-1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *         Original implementation as dprintf[_]ver_string
 *      05-Jul-1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *         Fixed problem that caused bug with tools/make_debug -- renaming
 *         this function should fix the problem.
 *      15-Feb-1998 Dimitrie Paun (dimi@cs.toronto.edu)
 *         Modified it to make it print the message using only one
 *         dprintf[_]ver call.
 *
 *****************************************************************************/

static void  ver_dstring(char const * prologue,
                         char const * teststring,
                         char const * epilogue )
{
  dprintf(("VERSION: ver_dstring(%s, %s, %s)\n",
           prologue,
           teststring,
           epilogue));
}


/******************************************************************************
 *
 *   int  testFileExistence(
 *      char const * path,
 *      char const * file )
 *
 *   Tests whether a given path/file combination exists.  If the file does
 *   not exist, the return value is zero.  If it does exist, the return
 *   value is non-zero.
 *
 *   Revision history
 *      30-May-1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *         Original implementation
 *
 *****************************************************************************/

static int  testFileExistence(char const * path,
                              char const * file )
{
  char      filename[1024];
  int       filenamelen;
  OFSTRUCT  fileinfo;
  int       retval;

  fileinfo.cBytes = sizeof(OFSTRUCT);

  strcpy(filename, path);
  filenamelen = strlen(filename);

  /* Add a trailing \ if necessary */
  if(filenamelen)
  {
    if(filename[filenamelen - 1] != '\\')
      strcat(filename, "\\");
  }
  else /* specify the current directory */
    strcpy(filename, ".\\");

  /* Create the full pathname */
  strcat(filename, file);

  if(OpenFile(filename, &fileinfo, OF_EXIST) == HFILE_ERROR)
    retval = 0;
  else
    retval = 1;

  return  retval;
}


/******************************************************************************
 *
 *   int  testFileExclusiveExistence(
 *      char const * path,
 *      char const * file )
 *
 *   Tests whether a given path/file combination exists and ensures that no
 *   other programs have handles to the given file.  If the file does not
 *   exist or is open, the return value is zero.  If it does exist, the
 *   return value is non-zero.
 *
 *   Revision history
 *      30-May-1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *         Original implementation
 *
 *****************************************************************************/

static int  testFileExclusiveExistence(char const * path,
                                       char const * file )
{
  char      filename[1024];
  int       filenamelen;
  OFSTRUCT  fileinfo;
  int       retval;

  fileinfo.cBytes = sizeof(OFSTRUCT);

  strcpy(filename, path);
  filenamelen = strlen(filename);

  /* Add a trailing \ if necessary */
  if(filenamelen)
  {
    if(filename[filenamelen - 1] != '\\')
      strcat(filename, "\\");
  }
  else /* specify the current directory */
    strcpy(filename, ".\\");

  /* Create the full pathname */
  strcat(filename, file);

  if(OpenFile(filename,
              &fileinfo,
              OF_EXIST | OF_SHARE_EXCLUSIVE) == HFILE_ERROR)
    retval = 0;
  else
    retval = 1;

  return retval;
}


/*****************************************************************************
 *
 *   VerFindFile() [VER.8]
 *   Determines where to install a file based on whether it locates another
 *   version of the file in the system.  The values VerFindFile returns are
 *   used in a subsequent call to the VerInstallFile function.
 *
 *   Revision history:
 *      30-May-1997   Dave Cuthbert (dacut@ece.cmu.edu)
 *         Reimplementation of VerFindFile from original stub.
 *
 ****************************************************************************/

/* VerFindFile32A                                    [VERSION.5] */
DWORD WIN32API VERSION_VerFindFileA(UINT flags,
                                    LPCSTR lpszFilename,
                                    LPCSTR lpszWinDir,
                                    LPCSTR lpszAppDir,
                                    LPSTR lpszCurDir,
                                    UINT *lpuCurDirLen,
                                    LPSTR lpszDestDir,
                                    UINT *lpuDestDirLen )
{
  DWORD  retval;
  char  curDir[256];
  char  destDir[256];
  unsigned int  curDirSizeReq;
  unsigned int  destDirSizeReq;

  retval = 0;

  /* Print out debugging information */
  dprintf(("VERSION: VerFindFileA(%08xh,%s,%s,%s,%08xh,%08xh,%08xh,%08xh)\n",
           flags,
           lpszFilename,
           lpszWinDir,
           lpszAppDir,
           lpszCurDir,
           lpuCurDirLen,
           lpszDestDir,
           lpuDestDirLen));

  ver_dstring("\tlpszFilename = ", lpszFilename, "");
  ver_dstring("\tlpszWinDir = ", lpszWinDir, "");
  ver_dstring("\tlpszAppDir = ", lpszAppDir, "");

  /* Figure out where the file should go; shared files default to the
     system directory */

  strcpy(curDir, "");
  strcpy(destDir, "");

  if(flags & VFFF_ISSHAREDFILE)
  {
    GetSystemDirectoryA(destDir, 256);

    /* Were we given a filename?  If so, try to find the file. */
    if(lpszFilename)
    {
      if(testFileExistence(destDir, lpszFilename))
      {
        strcpy(curDir, destDir);

        if(!testFileExclusiveExistence(destDir, lpszFilename))
          retval |= VFF_FILEINUSE;
      }
      else
        if(lpszAppDir && testFileExistence(lpszAppDir,
                      lpszFilename))
        {
          strcpy(curDir, lpszAppDir);
          retval |= VFF_CURNEDEST;

          if(!testFileExclusiveExistence(lpszAppDir, lpszFilename))
            retval |= VFF_FILEINUSE;
        }
    }
  }
  else
    if(!(flags & VFFF_ISSHAREDFILE))
    { /* not a shared file */
      if(lpszAppDir)
      {
        char  systemDir[256];
        GetSystemDirectoryA(systemDir, 256);

        strcpy(destDir, lpszAppDir);

        if(lpszFilename)
        {
          if(testFileExistence(lpszAppDir, lpszFilename))
          {
            strcpy(curDir, lpszAppDir);

            if(!testFileExclusiveExistence(lpszAppDir, lpszFilename))
              retval |= VFF_FILEINUSE;
          }
          else
            if(testFileExistence(systemDir, lpszFilename))
            {
              strcpy(curDir, systemDir);
              retval |= VFF_CURNEDEST;

              if(!testFileExclusiveExistence(systemDir, lpszFilename))
                retval |= VFF_FILEINUSE;
            }
        }
      }
    }

  curDirSizeReq = strlen(curDir) + 1;
  destDirSizeReq = strlen(destDir) + 1;



  /* Make sure that the pointers to the size of the buffers are
     valid; if not, do NOTHING with that buffer.  If that pointer
     is valid, then make sure that the buffer pointer is valid, too! */

  if(lpuDestDirLen && lpszDestDir)
  {
    if(*lpuDestDirLen < destDirSizeReq)
    {
      retval |= VFF_BUFFTOOSMALL;
      if (*lpuDestDirLen)
      {
        strncpy(lpszDestDir, destDir, *lpuDestDirLen - 1);
        lpszDestDir[*lpuDestDirLen - 1] = '\0';
      }
    }
    else
      strcpy(lpszDestDir, destDir);

    *lpuDestDirLen = destDirSizeReq;
  }

  if(lpuCurDirLen && lpszCurDir)
  {
    if(*lpuCurDirLen < curDirSizeReq)
    {
      retval |= VFF_BUFFTOOSMALL;
      if (*lpuCurDirLen)
      {
        strncpy(lpszCurDir, curDir, *lpuCurDirLen - 1);
        lpszCurDir[*lpuCurDirLen - 1] = '\0';
      }
    }
    else
      strcpy(lpszCurDir, curDir);

    *lpuCurDirLen = curDirSizeReq;
  }

  dprintf(("VERSION: VerFindFileA ret = %lu (%s%s%s)\n", retval,
     (retval & VFF_CURNEDEST) ? "VFF_CURNEDEST " : "",
     (retval & VFF_FILEINUSE) ? "VFF_FILEINUSE " : "",
     (retval & VFF_BUFFTOOSMALL) ? "VFF_BUFFTOOSMALL " : ""));

  ver_dstring("\t(Exit) lpszCurDir = ", lpszCurDir, "");

  return retval;
}

/* VerFindFile32W                                    [VERSION.6] */
DWORD WIN32API VERSION_VerFindFileW(UINT   flags,
                                    LPWSTR filename,
                                    LPWSTR windir,
                                    LPWSTR appdir,
                                    LPWSTR curdir,
                                    UINT   *pcurdirlen,
                                    LPWSTR destdir,
                                    UINT   *pdestdirlen)
{
    UINT  curdirlen,
          destdirlen;
    LPSTR wfn,
          wwd,
          wad,
          wdd,
          wcd;
    DWORD ret;

    wfn = UnicodeToAsciiString(filename );
    wwd = UnicodeToAsciiString(windir );
    wad = UnicodeToAsciiString(appdir );
    wcd = (LPSTR)HeapAlloc( GetProcessHeap(), 0, *pcurdirlen );
    wdd = (LPSTR)HeapAlloc( GetProcessHeap(), 0, *pdestdirlen );

    ret = VERSION_VerFindFileA(flags,
                               wfn,
                               wwd,
                               wad,
                               wcd,
                               &curdirlen,
                               wdd,
                               &destdirlen);

    AsciiToUnicodeN(wcd,curdir,*pcurdirlen);
    AsciiToUnicodeN(wdd,destdir,*pdestdirlen);
    *pcurdirlen = strlen(wcd);
    *pdestdirlen = strlen(wdd);

    FreeAsciiString(wfn );
    FreeAsciiString(wwd );
    FreeAsciiString(wad );
    FreeAsciiString(wcd );
    FreeAsciiString(wdd );
    return ret;
}

static LPBYTE _fetch_versioninfo(LPSTR fn,VS_FIXEDFILEINFO **vffi)
{
  DWORD   alloclen;
  LPBYTE  buf;
  DWORD   ret;

  alloclen = 1000;
  buf = (LPBYTE)malloc(alloclen);

  while (1)
  {
    ret = VERSION_GetFileVersionInfoA(fn,
                                      0,
                                      alloclen,
                                      buf);
    if (!ret)
    {
      free(buf);
      return 0;
    }

    if (alloclen<*(WORD*)buf)
    {
      free(buf);
      alloclen = *(WORD*)buf;
      buf = (LPBYTE)malloc(alloclen);
    }
    else
    {
      *vffi = (VS_FIXEDFILEINFO*)(buf+0x14);

      if ((*vffi)->dwSignature == 0x004f0049) /* hack to detect unicode */
        *vffi = (VS_FIXEDFILEINFO*)(buf+0x28);

      if ((*vffi)->dwSignature != VS_FFI_SIGNATURE)
        dprintf(("VERSION: _fetch_versioninfo: Bad VS_FIXEDFILEINFO signature 0x%08lx\n",
                 (*vffi)->dwSignature));

      return buf;
   }
  }
}

static DWORD _error2vif(DWORD error)
{
  switch (error)
  {
    case ERROR_ACCESS_DENIED:
      return VIF_ACCESSVIOLATION;

    case ERROR_SHARING_VIOLATION:
      return VIF_SHARINGVIOLATION;

    default:
      return 0;
  }
}


/******************************************************************************
 * VerInstallFile32A [VERSION.7]
 */
DWORD WIN32API VERSION_VerInstallFileA(UINT   flags,
                                       LPCSTR srcfilename,
                                       LPCSTR destfilename,
                                       LPCSTR srcdir,
                                       LPCSTR destdir,
                                       LPCSTR curdir,
                                       LPSTR  tmpfile,
                                       UINT   *tmpfilelen )
{
    LPCSTR   pdest;
    char     destfn[260],
             tmpfn[260],
             srcfn[260];
    HFILE    hfsrc,
             hfdst;
    DWORD    attr,
             ret,
             xret,
             tmplast;
    LPBYTE   buf1,buf2;
    OFSTRUCT ofs;

    dprintf(("VERSION: VerInstallFileA(%x,%s,%s,%s,%s,%s,%p,%d)\n",
             flags,
             srcfilename,
             destfilename,
             srcdir,
             destdir,
             curdir,
             tmpfile,
             *tmpfilelen));

#if 1
  dprintf(("VERSION: VersInstallFileA not implemented\n"));

  return 0;
#else
    xret = 0;

    sprintf(srcfn,
            "%s\\%s",
            srcdir,
            srcfilename);

    if (!destdir || !*destdir)
      pdest = srcdir;
    else
      pdest = destdir;

    sprintf(destfn,
            "%s\\%s",
            pdest,
            destfilename);

    hfsrc=LZOpenFileA(srcfn,
                      &ofs,
                      OF_READ);

    if (hfsrc==HFILE_ERROR)
      return VIF_CANNOTREADSRC;

    sprintf(tmpfn,"%s\\%s",pdest,destfilename);
    tmplast=strlen(pdest)+1;
    attr = GetFileAttributesA(tmpfn);
    if (attr!=-1) {
   if (attr & FILE_ATTRIBUTE_READONLY) {
       LZClose(hfsrc);
       return VIF_WRITEPROT;
   }
   /* FIXME: check if file currently in use and return VIF_FILEINUSE */
    }
    attr = -1;
    if (flags & VIFF_FORCEINSTALL) {
      if (tmpfile[0]) {
       sprintf(tmpfn,"%s\\%s",pdest,tmpfile);
       tmplast = strlen(pdest)+1;
       attr = GetFileAttributesA(tmpfn);
       /* if it exists, it has been copied by the call before.
        * we jump over the copy part...
        */
   }
    }
    if (attr == -1) {
      char  *s;

   GetTempFileNameA(pdest,"ver",0,tmpfn); /* should not fail ... */
   s=strrchr(tmpfn,'\\');
   if (s)
       tmplast = s-tmpfn;
   else
       tmplast = 0;
   hfdst = OpenFile(tmpfn,&ofs,OF_CREATE);
   if (hfdst == HFILE_ERROR) {
       LZClose(hfsrc);
       return VIF_CANNOTCREATE; /* | translated dos error */
   }
   ret = LZCopy(hfsrc,hfdst);
   _lclose(hfdst);
   if (((long) ret) < 0) {
       /* translate LZ errors into VIF_xxx */
       switch (ret) {
       case LZERROR_BADINHANDLE:
       case LZERROR_READ:
       case LZERROR_BADVALUE:
       case LZERROR_UNKNOWNALG:
      ret = VIF_CANNOTREADSRC;
      break;
       case LZERROR_BADOUTHANDLE:
       case LZERROR_WRITE:
      ret = VIF_OUTOFMEMORY; /* FIXME: correct? */
      break;
       case LZERROR_GLOBALLOC:
       case LZERROR_GLOBLOCK:
      ret = VIF_OUTOFSPACE;
      break;
       default: /* unknown error, should not happen */
      ret = 0;
      break;
       }
       if (ret) {
      LZClose(hfsrc);
      return ret;
       }
   }
    }
    xret = 0;
    if (!(flags & VIFF_FORCEINSTALL)) {
   VS_FIXEDFILEINFO *destvffi,*tmpvffi;
      buf1 = _fetch_versioninfo(destfn,&destvffi);
   if (buf1) {
       buf2 = _fetch_versioninfo(tmpfn,&tmpvffi);
       if (buf2) {
         char  *tbuf1,*tbuf2;
      UINT  len1,len2;

      len1=len2=40;

      /* compare file versions */
      if ((destvffi->dwFileVersionMS > tmpvffi->dwFileVersionMS)||
          ((destvffi->dwFileVersionMS==tmpvffi->dwFileVersionMS)&&
           (destvffi->dwFileVersionLS > tmpvffi->dwFileVersionLS)
          )
      )
          xret |= VIF_MISMATCH|VIF_SRCOLD;
      /* compare filetypes and filesubtypes */
      if ((destvffi->dwFileType!=tmpvffi->dwFileType) ||
          (destvffi->dwFileSubtype!=tmpvffi->dwFileSubtype)
      )
          xret |= VIF_MISMATCH|VIF_DIFFTYPE;
      if (VerQueryValueA(buf1,"\\VarFileInfo\\Translation",(LPVOID*)&tbuf1,&len1) &&
          VerQueryValueA(buf2,"\\VarFileInfo\\Translation",(LPVOID*)&tbuf2,&len2)
      ) {
          /* irgendwas mit tbuf1 und tbuf2 machen
           * generiert DIFFLANG|MISMATCH
           */
      }
      free(buf2);
       } else
      xret=VIF_MISMATCH|VIF_SRCOLD;
       free(buf1);
   }
    }
    if (xret) {
   if (*tmpfilelen<strlen(tmpfn+tmplast)) {
       xret|=VIF_BUFFTOOSMALL;
       DeleteFileA(tmpfn);
   } else {
       strcpy(tmpfile,tmpfn+tmplast);
       *tmpfilelen = strlen(tmpfn+tmplast)+1;
       xret|=VIF_TEMPFILE;
   }
    } else {
      if (-1!=GetFileAttributesA(destfn))
       if (!DeleteFileA(destfn)) {
      xret|=_error2vif(GetLastError())|VIF_CANNOTDELETE;
      DeleteFileA(tmpfn);
      LZClose(hfsrc);
      return xret;
       }
   if ((!(flags & VIFF_DONTDELETEOLD))   &&
       curdir                               &&
       *curdir                              &&
       lstrcmpiA(curdir,pdest)
   ) {
       char curfn[260];

       sprintf(curfn,"%s\\%s",curdir,destfilename);
       if (-1!=GetFileAttributesA(curfn)) {
      /* FIXME: check if in use ... if it is, VIF_CANNOTDELETECUR */
      if (!DeleteFileA(curfn))
             xret|=_error2vif(GetLastError())|VIF_CANNOTDELETECUR;
       }
   }
   if (!MoveFileA(tmpfn,destfn)) {
       xret|=_error2vif(GetLastError())|VIF_CANNOTRENAME;
       DeleteFileA(tmpfn);
   }
    }
    LZClose(hfsrc);
    return xret;
#endif
}


/* VerInstallFile32W                              [VERSION.8] */
DWORD WIN32API VERSION_VerInstallFileW(UINT   flags,
                                       LPWSTR srcfilename,
                                       LPWSTR destfilename,
                                       LPWSTR srcdir,
                                       LPWSTR destdir,
                                       LPWSTR curdir,
                                       LPWSTR tmpfile,
                                       UINT   *tmpfilelen )
{
    LPSTR wsrcf,
          wsrcd,
          wdestf,
          wdestd,
          wtmpf,
          wcurd;
    DWORD ret;

    wsrcf  = UnicodeToAsciiString(srcfilename );
    wsrcd  = UnicodeToAsciiString(srcdir );
    wdestf = UnicodeToAsciiString(destfilename );
    wdestd = UnicodeToAsciiString(destdir );
    wtmpf  = UnicodeToAsciiString(tmpfile );
    wcurd  = UnicodeToAsciiString(curdir );

    ret = VERSION_VerInstallFileA(flags,
                                  wsrcf,
                                  wdestf,
                                  wsrcd,
                                  wdestd,
                                  wcurd,
                                  wtmpf,
                                  tmpfilelen);
    if (!ret)
      AsciiToUnicodeN(wtmpf,
                      tmpfile,
                      *tmpfilelen);

    FreeAsciiString(wsrcf);
    FreeAsciiString(wsrcd);
    FreeAsciiString(wdestf);
    FreeAsciiString(wdestd);
    FreeAsciiString(wtmpf);

    if (wcurd)
      FreeAsciiString(wcurd);

    return ret;
}


/***********************************************************************
 *           VerLanguageName32A              [VERSION.9]
 */
DWORD WIN32API VERSION_VerLanguageNameA(UINT  wLang,
                                        LPSTR szLang,
                                        UINT  nSize)
{
  char    buffer[80];
  LPCSTR  name;
  DWORD   result;

  dprintf(("VERSION: VerLanguageNameA(%08xh,%08xh,%08xh) not implemented.\n",
           wLang,
           szLang,
           nSize));

#if 0
  /*
   * First, check \System\CurrentControlSet\control\Nls\Locale\<langid>
   * from the registry.
   */

  sprintf( buffer,
           "\\System\\CurrentControlSet\\control\\Nls\\Locale\\%08x",
           wLang );

  result = RegQueryValueA(HKEY_LOCAL_MACHINE,
                          buffer,
                          szLang,
                          (LPDWORD)&nSize );
  if (result == ERROR_SUCCESS ||
      result == ERROR_MORE_DATA)
    return nSize;

  /*
   * If that fails, use the internal table
   * (actually, Windows stores the names in a string table resource ...)
   */

  name = WINE_GetLanguageName( wLang );
  lstrcpynA( szLang, name, nSize );
  return lstrlenA( name );
#endif

  return 0;
}

/***********************************************************************
 *           VerLanguageName32W              [VERSION.10]
 */
DWORD WIN32API VERSION_VerLanguageNameW(UINT   wLang,
                                        LPWSTR szLang,
                                        UINT   nSize )
{
  char    buffer[80];
  LPWSTR  keyname;
  LPCSTR  name;
  DWORD   result;

  dprintf(("VERSION: VerLanguageNameW(%08xh,%08xh,%08xh) not implemented.\n",
           wLang,
           szLang,
           nSize));

#if 0
  /*
   * First, check \System\CurrentControlSet\control\Nls\Locale\<langid>
   * from the registry.
   */

  sprintf( buffer,
           "\\System\\CurrentControlSet\\control\\Nls\\Locale\\%08x",
           wLang );

  keyname = HEAP_strdupAtoW( GetProcessHeap(), 0, buffer );
  result = RegQueryValueW( HKEY_LOCAL_MACHINE, keyname, szLang, (LPDWORD)&nSize );
  HeapFree( GetProcessHeap(), 0, keyname );

  if (result == ERROR_SUCCESS ||
      result == ERROR_MORE_DATA)
    return nSize;

  /*
   * If that fails, use the internal table
   * (actually, Windows stores the names in a string table resource ...)
   */

  name = WINE_GetLanguageName( wLang );
  lstrcpynAtoW( szLang, name, nSize );
  return lstrlenA( name );
#else
  return 0;
#endif
}


