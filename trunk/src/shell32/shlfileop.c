
/*
 * SHFileOperation
 *
 * Copyright 2000 Juergen Schmied
 * Copyright 2002 Andriy Palamarchuk
 * Copyright 2002 Dietrich Teickner (from Odin)
 * Copyright 2002 Rolf Kalbermatter
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"
#include "wine/port.h"

#include <string.h>
#include <ctype.h>

#include "winreg.h"
#include "shellapi.h"
#include "shlobj.h"
#include "shresdef.h"
#include "shell32_main.h"
#include "undocshell.h"
#include "shlwapi.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(shell);

BOOL SHELL_ConfirmDialog (int nKindOfDialog, LPCSTR szDir)
{
	char szCaption[255], szText[255], szBuffer[MAX_PATH + 256];
	UINT caption_resource_id, text_resource_id;

  switch(nKindOfDialog) {

  case ASK_DELETE_FILE:
    caption_resource_id  = IDS_DELETEITEM_CAPTION;
    text_resource_id  = IDS_DELETEITEM_TEXT;
    break;
  case ASK_DELETE_FOLDER:
    caption_resource_id  = IDS_DELETEFOLDER_CAPTION;
    text_resource_id  = IDS_DELETEITEM_TEXT;
    break;
  case ASK_DELETE_MULTIPLE_ITEM:
    caption_resource_id  = IDS_DELETEITEM_CAPTION;
    text_resource_id  = IDS_DELETEMULTIPLE_TEXT;
    break;
  case ASK_OVERWRITE_FILE:
    caption_resource_id  = IDS_OVERWRITEFILE_CAPTION;
    text_resource_id  = IDS_OVERWRITEFILE_TEXT;
    break;
  default:
    FIXME(" Unhandled nKindOfDialog %d stub\n", nKindOfDialog);
    return FALSE;
  }

  LoadStringA(shell32_hInstance, caption_resource_id, szCaption, sizeof(szCaption));
  LoadStringA(shell32_hInstance, text_resource_id, szText, sizeof(szText));

  FormatMessageA(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
      szText, 0, 0, szBuffer, sizeof(szBuffer), (va_list*)&szDir);

  return (IDOK == MessageBoxA(GetActiveWindow(), szBuffer, szCaption, MB_OKCANCEL | MB_ICONEXCLAMATION));
}

/**************************************************************************
 *  SHELL_DeleteDirectoryA()
 *
 * like rm -r
 */

BOOL SHELL_DeleteDirectoryA(LPCSTR pszDir, BOOL bShowUI)
{
  BOOL    ret = TRUE;
  HANDLE  hFind;
  WIN32_FIND_DATAA wfd;
	char		szTemp[MAX_PATH];

	/* Make sure the directory exists before eventually prompting the user */
	PathCombineA(szTemp, pszDir, "*.*");
  hFind = FindFirstFileA(szTemp, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	  return FALSE;

	if (!bShowUI || SHELL_ConfirmDialog(ASK_DELETE_FOLDER, pszDir))
	{
	  do
	  {
	    if (lstrcmpA(wfd.cFileName, ".") && lstrcmpA(wfd.cFileName, ".."))
      {
	      PathCombineA(szTemp, pszDir, wfd.cFileName);
	      if (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)
	        ret = SHELL_DeleteDirectoryA(szTemp, FALSE);
	      else
	        ret = DeleteFileA(szTemp);
	    }
	  } while (ret && FindNextFileA(hFind, &wfd));
	}
	FindClose(hFind);
	if (ret)
	  ret = RemoveDirectoryA(pszDir);
	return ret;
}

/**************************************************************************
 *  SHELL_DeleteFileA()
 */

BOOL SHELL_DeleteFileA(LPCSTR pszFile, BOOL bShowUI)
{
	if (bShowUI && !SHELL_ConfirmDialog(ASK_DELETE_FILE, pszFile))
	  return FALSE;

	return DeleteFileA(pszFile);
}

/**************************************************************************
 * Win32CreateDirectory      (SHELL32_93)   [SHELL32.93]
 *
 * Creates a directory. Also triggers a change notify if one exists.
 */
static BOOL Win32CreateDirectoryA(LPCSTR path, LPSECURITY_ATTRIBUTES sec)
{
	BOOL ret;
	TRACE("(%%s, %p)\n", debugstr_a(path), sec);

	ret = CreateDirectoryA(path, sec);
	if (ret)
	{
	  SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHA, path, NULL);
	}
	return ret;
}

static BOOL Win32CreateDirectoryW(LPCWSTR path, LPSECURITY_ATTRIBUTES sec)
{
	BOOL ret;
	TRACE("(%%s, %p)\n", debugstr_w(path), sec);

	ret = CreateDirectoryW(path, sec);
	if (ret)
	{
	  SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, path, NULL);
	}
	return ret;
}

BOOL WINAPI Win32CreateirectoryAW(LPCVOID path, LPSECURITY_ATTRIBUTES sec)
{
	if (SHELL_OsIsUnicode())
	  return Win32CreateDirectoryW(path, sec);
	return Win32CreateDirectoryA(path, sec);
}

/************************************************************************
 * Win32RemoveDirectory      (SHELL32_94)   [SHELL32.94]
 *
 * Deletes a directory. Also triggers a change notify if one exists.
 */
static BOOL Win32RemoveDirectoryA(LPCSTR path)
{
	DWORD attr;
	BOOL ret;
	TRACE("(%%s)\n", debugstr_a(path));
	
	ret = RemoveDirectoryA(path);
	if (!ret)
	{
	  attr = GetFileAttributesA(path);
	  if (attr != -1 && attr & FILE_ATTRIBUTE_READONLY)
	  {
	    SetFileAttributesA(path, attr & ~FILE_ATTRIBUTE_READONLY);
	    ret = RemoveDirectoryA(path);
	  }
	}
	if (ret)
	{
	  SHChangeNotify(SHCNE_RMDIR, SHCNF_PATHA, path, NULL);
	}
	return ret;
}

static BOOL Win32RemoveDirectoryW(LPCWSTR path)
{
	DWORD attr;
	BOOL ret;
	TRACE("(%%s)\n", debugstr_w(path));
	
	ret = RemoveDirectoryW(path);
	if (!ret)
	{
	  attr = GetFileAttributesW(path);
	  if (attr != -1 && attr & FILE_ATTRIBUTE_READONLY)
	  {
	    SetFileAttributesW(path, attr & ~FILE_ATTRIBUTE_READONLY);
	    ret = RemoveDirectoryW(path);
	  }
	}
	if (ret)
	{
	  SHChangeNotify(SHCNE_RMDIR, SHCNF_PATHW, path, NULL);
	}
	return ret;
}

BOOL WINAPI Win32RemoveDirectoryAW(LPCVOID path)
{
	if (SHELL_OsIsUnicode())
	  return Win32RemoveDirectoryW(path);
	return Win32RemoveDirectoryA(path);
}

/************************************************************************
 * Win32DeleteFile           (SHELL32_164)  [SHELL32.164]
 *
 * Deletes a file. Also triggers a change notify if one exists.
 *
 * NOTES:
 *  Verified on Win98 / IE 5 (SHELL32 4.72, March 1999 build) to be ANSI.
 *  This is Unicode on NT/2000
 */
static BOOL Win32DeleteFileA(LPCSTR path)
{
	BOOL ret;

	TRACE("(%s)\n", debugstr_a(path));

	ret = DeleteFileA(path);
	if (!ret)
	{
	  /* File may be write protected or a system file */
	  DWORD dwAttr = GetFileAttributesA(path);
	  if ((dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM))
	    if (SetFileAttributesA(path, dwAttr & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
	      ret = DeleteFileA(path);
	}
	if (ret)
	  SHChangeNotify(SHCNE_DELETE, SHCNF_PATHA, path, NULL);
	return ret;
}

static BOOL Win32DeleteFileW(LPCWSTR path)
{
	BOOL ret;
	
	TRACE("(%s)\n", debugstr_w(path));

	ret = DeleteFileW(path);
	if (!ret)
	{
	  /* File may be write protected or a system file */
	  DWORD dwAttr = GetFileAttributesW(path);
	  if ((dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM))
	    if (SetFileAttributesW(path, dwAttr & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
	      ret = DeleteFileW(path);
	}
	if (ret)
	  SHChangeNotify(SHCNE_DELETE, SHCNF_PATHW, path, NULL);
	return ret;
}

DWORD WINAPI Win32DeleteFileAW(LPCVOID path)
{
	if (SHELL_OsIsUnicode())
	  return Win32DeleteFileW(path);
	return Win32DeleteFileA(path);
}

/*************************************************************************
 * SHCreateDirectory                       [SHELL32.165]
 *
 * NOTES
 *  exported by ordinal
 *  WinNT/2000 exports Unicode
 */
DWORD WINAPI SHCreateDirectory(HWND hWnd, LPCVOID path)
{
	if (SHELL_OsIsUnicode())
	  return SHCreateDirectoryExW(hWnd, path, NULL);
	return SHCreateDirectoryExA(hWnd, path, NULL);
}

/*************************************************************************
 * SHCreateDirectoryExA                     [SHELL32.@]
 */
DWORD WINAPI SHCreateDirectoryExA(HWND hWnd, LPCSTR path, LPSECURITY_ATTRIBUTES sec)
{
	WCHAR wPath[MAX_PATH];
	TRACE("(%p, %s, %p)\n",hWnd, debugstr_a(path), sec);

	MultiByteToWideChar(CP_ACP, 0, path, -1, wPath, MAX_PATH);
	return SHCreateDirectoryExW(hWnd, wPath, sec);
}

/*************************************************************************
 * SHCreateDirectoryExW                     [SHELL32.@]
 */
DWORD WINAPI SHCreateDirectoryExW(HWND hWnd, LPCWSTR path, LPSECURITY_ATTRIBUTES sec)
{
	DWORD ret = ERROR_SUCCESS;
	TRACE("(%p, %s, %p)\n",hWnd, debugstr_w(path), sec);

	if (PathIsRelativeW(path))
	{
	  ret = ERROR_BAD_PATHNAME;
	  SetLastError(ERROR_BAD_PATHNAME);
	}
	else
	{
	  if (!Win32CreateDirectoryW(path, sec))
	  {
	    ret = GetLastError();
	    if (ret != ERROR_FILE_EXISTS &&
	        ret != ERROR_ALREADY_EXISTS &&
	        ret != ERROR_FILENAME_EXCED_RANGE)
	    {
	    /*lstrcpynW(pathName, path, MAX_PATH);
	      lpStr = PathAddBackslashW(pathName);*/
	      FIXME("Semi-stub, non zero hWnd should be used somehow?");
	    }
	  }
	}
	return ret;
}

/*************************************************************************
 *
 * SHFileStrICmp HelperFunction for SHFileOperationA
 *
 */
BOOL SHFileStrICmpA(LPSTR p1,LPSTR p2, LPSTR p1End, LPSTR p2End)
{
  CHAR C1 = '\0';
  CHAR C2 = '\0';
  int i_Temp = -1;
  int i_len1 = lstrlenA(p1);
  int i_len2 = lstrlenA(p2);

  if (p1End && (&p1[i_len1] >= p1End) && ('\\' == p1End[0]))
  {
     C1 = p1End[0];
    p1End[0] = '\0';
    i_len1 = lstrlenA(p1);
  }
  if (p2End)
  {
    if ((&p2[i_len2] >= p2End) && ('\\' == p2End[0]))
    {
      C2 = p2End[0];
      if (C2)
        p2End[0] = '\0';
    }
  }
  else
  {
    if ((i_len1 <= i_len2) && ('\\' == p2[i_len1]))
    {
      C2 = p2[i_len1];
      if (C2)
        p2[i_len1] = '\0';
    }
  }
  i_len2 = lstrlenA(p2);
  if (i_len1 == i_len2)
    i_Temp = lstrcmpiA(p1,p2);
  if (C1)
    p1[i_len1] = C1;
  if (C2)
    p2[i_len2] = C2;
  return !(i_Temp);
}
/*************************************************************************
 *
 * SHFileStrCpyCat HelperFunction for SHFileOperationA/W
 *
 */
#define I_SHFileStrCpyCatA
#include "shlfileop.h"
#define I_SHFileStrCpyCatW
#include "shlfileop.h"

/*************************************************************************
 * SHFileOperationA        [SHELL32.@]
 *
 * NOTES
 *     exported by name
 */
DWORD WINAPI SHFileOperationA (LPSHFILEOPSTRUCTA lpFileOp)
{
  SHFILEOPSTRUCTA nlpFileOp = *(lpFileOp);

  LPCSTR pNextFrom = nlpFileOp.pFrom;
  LPCSTR pNextTo = nlpFileOp.pTo;
  LPCSTR pFrom = pNextFrom;
  LPCSTR pTo = NULL;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATAA wfd;
  LPSTR pTempFrom = NULL;
  LPSTR pTempTo = NULL;
  LPSTR pFromFile;
  LPSTR pToFile;
  long retCode = 0;
  DWORD ToAttr;
  DWORD ToPathAttr;
  FILEOP_FLAGS OFl = ((FILEOP_FLAGS)lpFileOp->fFlags & 0x7ff);

  BOOL b_Multi = (nlpFileOp.fFlags & FOF_MULTIDESTFILES);

  BOOL b_MultiTo = (FO_DELETE != (lpFileOp->wFunc & 0xf));
  BOOL b_MultiFrom = FALSE;
  BOOL not_overwrite;
  BOOL ask_overwrite;
  BOOL b_SameRoot;
  BOOL b_SameTailName;
  BOOL b_ToInvalidTail;
  BOOL b_ToValid; /* for W98-Bug for FO_MOVE with source and taget in same rootdrive */
  BOOL b_Mask; /* wird als Schalter benutzt, vieleicht finde ich die richtige bitposition */
  BOOL b_ToTailSlash;

  LPCSTR cFO_Name [4] = { "FO_MOVE", "FO_COPY", "FO_DELETE", "FO_RENAME" };
  LPCSTR pToFuncTXT = "FO_????";

  long FuncSwitch = (nlpFileOp.wFunc & 0xf);
  long level= nlpFileOp.wFunc>>4;

/*  default no error */
  nlpFileOp.fAnyOperationsAborted=FALSE;

  if (!(FuncSwitch) || (FO_RENAME < FuncSwitch))
    goto shfileop_normal;

  pToFuncTXT = cFO_Name [FuncSwitch-1];
  if (level == 0)
    TRACE("%s: flags (0x%04x) : %s%s%s%s%s%s%s%s%s%s%s%s \n",pToFuncTXT, nlpFileOp.fFlags,
                nlpFileOp.fFlags & FOF_MULTIDESTFILES ? "FOF_MULTIDESTFILES " : "",
                nlpFileOp.fFlags & FOF_CONFIRMMOUSE ? "FOF_CONFIRMMOUSE " : "",
                nlpFileOp.fFlags & FOF_SILENT ? "FOF_SILENT " : "",
                nlpFileOp.fFlags & FOF_RENAMEONCOLLISION ? "FOF_RENAMEONCOLLISION " : "",
                nlpFileOp.fFlags & FOF_NOCONFIRMATION ? "FOF_NOCONFIRMATION " : "",
                nlpFileOp.fFlags & FOF_WANTMAPPINGHANDLE ? "FOF_WANTMAPPINGHANDLE " : "",
                nlpFileOp.fFlags & FOF_ALLOWUNDO ? "FOF_ALLOWUNDO " : "",
                nlpFileOp.fFlags & FOF_FILESONLY ? "FOF_FILESONLY " : "",
                nlpFileOp.fFlags & FOF_SIMPLEPROGRESS ? "FOF_SIMPLEPROGRESS " : "",
                nlpFileOp.fFlags & FOF_NOCONFIRMMKDIR ? "FOF_NOCONFIRMMKDIR " : "",
                nlpFileOp.fFlags & FOF_NOERRORUI ? "FOF_NOERRORUI " : "",
                nlpFileOp.fFlags & 0xf800 ? "MORE-UNKNOWN-Flags" : "");
  ;
  {
                /* establish when pTo is interpreted as the name of the destination file
                 * or the directory where the Fromfile should be copied to.
                 * This depends on:
                 * (1) pTo points to the name of an existing directory;
                 * (2) the flag FOF_MULTIDESTFILES is present;
                 * (3) whether pFrom point to multiple filenames.
                 *
                 * Some experiments:
                 *
                 * destisdir               1 1 1 1 0 0 0 0
                 * FOF_MULTIDESTFILES      1 1 0 0 1 1 0 0
                 * multiple from filenames 1 0 1 0 1 0 1 0
                 *                         ---------------
                 * copy files to dir       1 0 1 1 0 0 1 0
                 * create dir              0 0 0 0 0 0 1 0
                 */
/*
 * FOF_MULTIDESTFILES, FOF_NOCONFIRMATION, FOF_FILESONLY          are    implemented
 * FOF_CONFIRMMOUSE, FOF_SILENT, FOF_NOCONFIRMMKDIR, FOF_SIMPLEPROGRESS    are not implemented and ignored
 * FOF_RENAMEONCOLLISION                             are    implemented partially and breaks if file exist
 * FOF_ALLOWUNDO, FOF_WANTMAPPINGHANDLE                    are not implemented and breaks
 * if any other flag set, an error occurs
 */
    TRACE(__FUNCTION__" %s level=%d nlpFileOp.fFlags=0x%x\n", pToFuncTXT, level, lpFileOp->fFlags);

/*    OFl &= (-1 - (FOF_MULTIDESTFILES | FOF_FILESONLY));
/*    OFl ^= (FOF_SILENT | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMMKDIR); */
    OFl &= (~(FOF_MULTIDESTFILES | FOF_NOCONFIRMATION | FOF_FILESONLY));  /* implemented */
    OFl ^= (FOF_SILENT | FOF_NOCONFIRMMKDIR);                /* ignored, if one */
    OFl &= (~FOF_SIMPLEPROGRESS);                      /* ignored, only wit FOF_SILEN */
    if (OFl)
    {
      if (OFl & ( ~ (FOF_CONFIRMMOUSE | FOF_SILENT | FOF_RENAMEONCOLLISION | FOF_NOCONFIRMMKDIR)))
      {
        TRACE("%s level=%d lpFileOp->fFlags=0x%x not implemented, Aborted=TRUE, stub\n", pToFuncTXT, level, OFl);
        retCode = 0x403; /* 1027, we need a extension of shlfileop */
        goto shfileop_error;
      }
      else
      {
        TRACE("%s level=%d lpFileOp->fFlags=0x%x not full implemented ,stub\n", pToFuncTXT, level, OFl);
      } /* endif */
    } /* endif */

    if ((pNextFrom) && (!(b_MultiTo) || (pNextTo)))
    {
      nlpFileOp.pFrom = pTempFrom = HeapAlloc(GetProcessHeap(), 0, (1+2*(b_MultiTo)) * MAX_PATH+6);
      if (b_MultiTo) pTempTo = &pTempFrom[MAX_PATH+4];
      nlpFileOp.pTo   = pTempTo;
      ask_overwrite = (!(nlpFileOp.fFlags & FOF_NOCONFIRMATION) && !(nlpFileOp.fFlags & FOF_RENAMEONCOLLISION));
      not_overwrite = (!(nlpFileOp.fFlags & FOF_NOCONFIRMATION) ||  (nlpFileOp.fFlags & FOF_RENAMEONCOLLISION));
    }
    else
    {
      retCode = 0x402;      /* 1026 */
      goto shfileop_error;
    }
/* need break at error before change sourcepointer */
    while(!nlpFileOp.fAnyOperationsAborted && (pNextFrom[0]))
    {
      nlpFileOp.wFunc =  ((level+1)<<4) + FuncSwitch;
      nlpFileOp.fFlags = lpFileOp->fFlags;

      if (b_MultiTo)
      {
        pTo = pNextTo;
        pNextTo = &pNextTo[lstrlenA(pTo)+1];
        b_MultiTo = (b_Multi && pNextTo[0]);
      }

      pFrom = pNextFrom;
      pNextFrom = &pNextFrom[lstrlenA(pNextFrom)+1];
      if (!b_MultiFrom && !b_MultiTo)
        b_MultiFrom = (pNextFrom[0]);

      pFromFile = SHFileStrCpyCatA(pTempFrom, pFrom, NULL);

      if (pTo)
      {
        pToFile = SHFileStrCpyCatA(pTempTo, pTo, NULL);
      }
      if (!(pFromFile) || !(pFromFile[1]) || ((pTo) && !(pToFile)))
      {
         retCode = 0x402;      /* 1026 */
         goto shfileop_error;
      }
      if (pTo)
      {
        b_ToTailSlash = (!pToFile[1]);
        if (b_ToTailSlash)
        {
          pToFile[0] = '\0';
          if (strchr(pTempTo,'\\'))
          {
            pToFile = SHFileStrCpyCatA(pTempTo, NULL, NULL);
          }
        }
        b_ToInvalidTail = (NULL != StrPBrkA(&pToFile[1],"*?"));
      }

      b_Mask = (NULL != StrPBrkA(&pFromFile[1],"*?"));
      if (FO_RENAME == FuncSwitch)
      {
/* temporr only for FO_RENAME */
//        b_Mask = (NULL != strpbrk(pFrom,"*?"));
        if (b_MultiTo || b_MultiFrom || (b_Mask && !b_ToInvalidTail))
        {
/* no work, only RC=0 */
// /* ???? */    nlpFileOp.fAnyOperationsAborted=TRUE;
          goto shfileop_normal;
        }
      }

      hFind = FindFirstFileA(pFrom, &wfd);
      if (INVALID_HANDLE_VALUE == hFind)
      {
/* root (without mask) is also not allowed as source, tested in W98 */
        retCode = 0x402;   /* 1026 */
        goto shfileop_error;
      } /* endif */

/* for all */
#define HIGH_ADR (LPSTR)0xffffffff
//      b_Mask = (!SHFileStrICmpA(&pFromFile[1], &wfd.cFileName[0], HIGH_ADR, HIGH_ADR));

      if (!pTo) /* FO_DELETE */
      {
        do
        {
          if (wfd.cFileName[0] == '.')
          {
          if (wfd.cFileName[1] == '\0')
            continue;
          if (wfd.cFileName[1] == '.' && wfd.cFileName[2] == '\0')
            continue;
          }
          SHFileStrCpyCatA(&pFromFile[1], wfd.cFileName, NULL);
          if (IsAttribFile(wfd.dwFileAttributes))
          {
            nlpFileOp.fAnyOperationsAborted = (!Win32DeleteFileA(pTempFrom));
            retCode = 0x78; /* value unknown */
          }
          else
          {
            nlpFileOp.fAnyOperationsAborted = (!SHELL_DeleteDirectoryA(pTempFrom,(!(nlpFileOp.fFlags & FOF_NOCONFIRMATION))));
            retCode = 0x79; /* value unknown */
          }
        } while(!nlpFileOp.fAnyOperationsAborted && FindNextFileA(hFind, &wfd));
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
        if (nlpFileOp.fAnyOperationsAborted)
        {
          goto shfileop_error;
        }
        continue;
      } /* FO_DELETE ends, pTo must be every valid from here */

      b_SameRoot = (toupper(pTempFrom[0]) == toupper(pTempTo[0]));
      b_SameTailName = SHFileStrICmpA(pToFile, pFromFile, NULL, NULL);

      ToPathAttr = ToAttr = GetFileAttributesA(pTempTo);
      if (!b_Mask && (ToAttr -1))
      {
        if (pToFile)
        {
          pToFile[0] = '\0';
          ToPathAttr = GetFileAttributesA(pTempTo);
          pToFile[0] = '\\';
        }
      }

      if (FO_RENAME == FuncSwitch)
      {
        if (!b_SameRoot || b_Mask /* FO_RENAME works not with Mask */
         || !SHFileStrICmpA(pTempFrom, pTempTo, pFromFile, NULL)
         || (SHFileStrICmpA(pTempFrom, pTempTo, pFromFile, HIGH_ADR) && !b_ToTailSlash))
        {
          retCode = 0x73;
          goto shfileop_error;
        }
        if (b_ToInvalidTail)
        {
          retCode=0x2;
          goto shfileop_error;
        }
        if (-1 == ToPathAttr)
        {
          retCode = 0x75;
          goto shfileop_error;
        }
        if (IsAttribDir(wfd.dwFileAttributes) && IsAttribDir(ToAttr))
        {
          retCode = (b_ToTailSlash) ? 0xb7 : 0x7b;
          goto shfileop_error;
        } /* endif */
        if (!MoveFileA(pTempFrom, pTempTo))
        {
/* we need still the value for the returncode, we use the mostly assumed */
          retCode = 0xb7;
          goto shfileop_error;
        }
        goto shfileop_normal;
      }

      /* W98 Bug with FO_MOVE(/RENAME ?) different to FO_COPY, better the same as FO_COPY */

      b_ToValid = ((b_SameTailName &&  b_SameRoot && (FO_COPY == FuncSwitch)) ||
                (b_SameTailName && !b_SameRoot) || (b_ToInvalidTail));

/* handle mask in source */
      if (b_Mask)
      {
        if (!IsAttribDir(ToAttr))
        {
          retCode = (b_ToInvalidTail &&/* b_SameTailName &&*/ (FO_MOVE == FuncSwitch)) \
            ? 0x2 : 0x75;
          goto shfileop_error;
        }
        pToFile = SHFileStrCpyCatA(pTempTo,NULL, "\\");
        nlpFileOp.fFlags = (nlpFileOp.fFlags | FOF_MULTIDESTFILES);
        do
        {
          if (wfd.cFileName[0] == '.')
          {
            if (wfd.cFileName[1] == '\0')
              continue;
            if (wfd.cFileName[1] == '.' && wfd.cFileName[2] == '\0') continue;
          }
          if (IsAttribDir(wfd.dwFileAttributes) && (nlpFileOp.fFlags & FOF_FILESONLY))
          {
            continue; /* next name in pTempFrom(dir) */
          }
          SHFileStrCpyCatA(&pToFile[1], wfd.cFileName, NULL);
          SHFileStrCpyCatA(&pFromFile[1], wfd.cFileName, NULL);
          retCode = SHFileOperationA (&nlpFileOp);
        } while(!nlpFileOp.fAnyOperationsAborted && FindNextFileA(hFind, &wfd));
      }
      FindClose(hFind);
      hFind = INVALID_HANDLE_VALUE;
      /* only FO_COPY/FO_MOVE without mask, FO_DELETE and FO_RENAME are solved */
      if (b_Mask)
        continue;

      if (IsAttribDir(wfd.dwFileAttributes) && (ToAttr -1))
      {
        if (pToFile)
        {
          pToFile[0] = '\0';
          ToPathAttr = GetFileAttributesA(pTempTo);
          if ((ToPathAttr == -1) && b_ToValid)
          {
/* create dir must be here, sample target D:\y\ *.* create with RC=10003 */
            if(SHCreateDirectoryExA(NULL,pTempTo, NULL))
            {
              retCode = 0x73;/* value unknown */
              goto shfileop_error;
            }
            ToPathAttr = GetFileAttributesA(pTempTo);
          }
          pToFile[0] = '\\';
          if (b_ToInvalidTail)
          {
            retCode=0x10003;
            goto shfileop_error;
          }
        }
      }

/* tailling BackSlash is ever removed and pToFile points to BackSlash before */
      if (!b_MultiTo && (b_MultiFrom || (!(b_Multi) && IsAttribDir(ToAttr))))
      {
        if ((FO_MOVE == FuncSwitch) && IsAttribDir(ToAttr) && IsAttribDir(wfd.dwFileAttributes))
        {
          if (b_Multi)
          {
            retCode = 0x73; /* !b_Multi = 0x8 ?? */
            goto shfileop_error;
          }
        }
        pToFile = SHFileStrCpyCatA(pTempTo, NULL, wfd.cFileName);
        ToAttr = GetFileAttributesA(pTempTo);
      }

      if (IsAttribDir(ToAttr))
      {
        if (IsAttribFile(wfd.dwFileAttributes))
        {
          retCode = (FO_COPY == FuncSwitch) ? 0x75 : 0xb7;
          goto shfileop_error;
        }
      }
      else
      {
        pToFile[0] = '\0';
        ToPathAttr = GetFileAttributesA(pTempTo);
        pToFile[0] = '\\';
        if (IsAttribFile(ToPathAttr))
        {
/* error, is this tested ? */
          retCode = 0x777402;
          goto shfileop_error;
        } /* endif */
      }

/* singlesource + no mask */
      if (-1 == (ToAttr & ToPathAttr))
      {
/* Target-dir does not exist, and can not created */
        retCode=0x75;
        goto shfileop_error;
      }

      switch(FuncSwitch)
      {
      case FO_MOVE:
        pToFile = NULL;
        if ((ToAttr == -1) && SHFileStrICmpA(pTempFrom, pTempTo, pFromFile, NULL))
        {
          nlpFileOp.wFunc =  ((level+1)<<4) + FO_RENAME;
        }
        else
        {
          if (b_SameRoot && IsAttribDir(ToAttr) && IsAttribDir(wfd.dwFileAttributes))
          {
            pToFile = SHFileStrCpyCatA(pTempFrom, NULL, "*.*");
            retCode = SHFileOperationA(&nlpFileOp);
          }
          else
          {
            nlpFileOp.wFunc =  ((level+1)<<4) + FO_COPY;
          } /* endif */
        }
        retCode = SHFileOperationA(&nlpFileOp);
        if (pToFile)
          ((WORD*)pToFile)[0] = '\0';
        if (!nlpFileOp.fAnyOperationsAborted && (FO_RENAME != (nlpFileOp.wFunc & 0xf)))
        {
          nlpFileOp.wFunc =  ((level+1)<<4) + FO_DELETE;
          retCode = SHFileOperationA(&nlpFileOp);
        } /* endif */
        continue;
      case FO_COPY:
        if (SHFileStrICmpA(pTempFrom, pTempTo, NULL, NULL))
        { /* target is the same as source ? */
/* we need still the value for the returncode, we assume 0x71 */
          retCode = 0x71;
          goto shfileop_error;
        } /* endif */
        if (IsAttribDir((ToAttr & wfd.dwFileAttributes)))
        {
          if (IsAttribDir(ToAttr) || !SHCreateDirectoryExA(NULL,pTempTo, NULL))
          {
/*  ??? nlpFileOp.fFlags = (nlpFileOp.fFlags | FOF_MULTIDESTFILES); */
            SHFileStrCpyCatA(pTempFrom, NULL, "*.*");
            retCode = SHFileOperationA(&nlpFileOp);
          }
          else
          {
            retCode = 0x750;/* value unknown */
            goto shfileop_error;
          }
        }
        else
        {
          if (!(ask_overwrite && SHELL_ConfirmDialog (ASK_OVERWRITE_FILE, pTempTo))
          && (not_overwrite))
          {
            /* we need still the value for the returncode, we use the mostly assumed */
            retCode = 0x73;
            goto shfileop_error;
          }
          if (!(CopyFileA(pTempFrom, pTempTo, FALSE)))
          {
            retCode = 0x77; /* value unknown */
            goto shfileop_error;
          }
        }
      } /* end-switch */
    } /* end-while */
  }
shfileop_normal:
  if (!(nlpFileOp.fAnyOperationsAborted))
    retCode = 0;
shfileop_error:
  if (retCode)
    nlpFileOp.fAnyOperationsAborted = TRUE;
  if (hFind != INVALID_HANDLE_VALUE)
    FindClose(hFind);
  hFind = INVALID_HANDLE_VALUE;
  if (pTempFrom)
    HeapFree(GetProcessHeap(), 0, pTempFrom);

  TRACE("%s level=%d AnyOpsAborted=%s ret=0x%x, with %s%s%s\n",
        pToFuncTXT, level, nlpFileOp.fAnyOperationsAborted ? "TRUE":"FALSE",
        retCode, debugstr_a(pFrom), pTo ? " -> ":"", debugstr_a(pTo));

  lpFileOp->fAnyOperationsAborted = nlpFileOp.fAnyOperationsAborted;
  return retCode;
}

/*************************************************************************
 * SHFileOperationW        [SHELL32.@]
 *
 * NOTES
 *     exported by name
 */
DWORD WINAPI SHFileOperationW (LPSHFILEOPSTRUCTW lpFileOp)
{
  FIXME("(%p):stub.\n", lpFileOp);
  return 1;
}

/*************************************************************************
 * SHFileOperation        [SHELL32.@]
 *
 */
DWORD WINAPI SHFileOperationAW(LPVOID lpFileOp)
{
  if (SHELL_OsIsUnicode())
    return SHFileOperationW(lpFileOp);
  return SHFileOperationA(lpFileOp);
}

/*************************************************************************
 * SheGetDirW [SHELL32.281]
 *
 */
HRESULT WINAPI SheGetDirW(LPWSTR u, LPWSTR v)
{  FIXME("%p %p stub\n",u,v);
  return 0;
}

/*************************************************************************
 * SheChangeDirW [SHELL32.274]
 *
 */
HRESULT WINAPI SheChangeDirW(LPWSTR u)
{	FIXME("(%s),stub\n",debugstr_w(u));
	return 0;
}

/*************************************************************************
 * IsNetDrive      [SHELL32.66]
 */
BOOL WINAPI IsNetDrive(DWORD drive)
{
  char root[4];
  strcpy(root, "A:\\");
  root[0] += (char)drive;
  return (GetDriveTypeA(root) == DRIVE_REMOTE);
}


