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
 *
 * !!! shlfileop.c is shared source between wine and odin, do not remove
 * #ifdef __win32os2__ .. lines
 */

#include "config.h"
#include "wine/port.h"

#include <string.h>
#include <ctype.h>

#include "winreg.h"
#include "shellapi.h"
#include "shlobj.h"
#include "shresdef.h"
#define NO_SHLWAPI_STREAM
#include "shlwapi.h"
#include "shell32_main.h"
#include "undocshell.h"
#include "wine/unicode.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(shell);

#define W98_FO_FUNCTION /* makes shlfileop W98-like */ 
#undef W98_FO_FUNCTION /* makes shlfileop W2K-like */
#define W98_FO_FUNCTION /* makes shlfileop W98-like */ 

#define IsAttribFile(x) (!(x & FILE_ATTRIBUTE_DIRECTORY))
#define IsAttribDir(x)  (!(x == -1) && (x & FILE_ATTRIBUTE_DIRECTORY))

#define IsDotDir(x)     ((x[0] == '.') && ((x[1] == 0) || ((x[1] == '.') && (x[2] == 0))))

#define FO_MASK         0xFF
#define FO_LevelShift   8
#define HIGH_ADR (LPWSTR)0xffffffff

CHAR aWildcardFile[] = {'*','.','*',0};
WCHAR wWildcardChars[] = {'?','*',0};
#define wWildcardFile &wWildcardChars[1]
WCHAR wBackslash[] = {'\\',0};

static BOOL SHNotifyCreateDirectoryA(LPCSTR path, LPSECURITY_ATTRIBUTES sec);
static BOOL SHNotifyCreateDirectoryW(LPCWSTR path, LPSECURITY_ATTRIBUTES sec);
static BOOL SHNotifyRemoveDirectoryA(LPCSTR path);
static BOOL SHNotifyRemoveDirectoryW(LPCWSTR path);
static BOOL SHNotifyDeleteFileA(LPCSTR path);
static BOOL SHNotifyDeleteFileW(LPCWSTR path);
static BOOL SHNotifyMoveFileW(LPCWSTR src, LPCWSTR dest);
static BOOL SHNotifyCopyFileW(LPCWSTR src, LPCWSTR dest, BOOL bRenameIfExists);

typedef struct
{
	UINT caption_resource_id, text_resource_id;
} SHELL_ConfirmIDstruc;

static BOOL SHELL_ConfirmIDs(int nKindOfDialog, SHELL_ConfirmIDstruc *ids)
{
	switch (nKindOfDialog) {
	  case ASK_DELETE_FILE:
	    ids->caption_resource_id  = IDS_DELETEITEM_CAPTION;
	    ids->text_resource_id  = IDS_DELETEITEM_TEXT;
	    return TRUE;
	  case ASK_DELETE_FOLDER:
	    ids->caption_resource_id  = IDS_DELETEFOLDER_CAPTION;
	    ids->text_resource_id  = IDS_DELETEITEM_TEXT;
	    return TRUE;
	  case ASK_DELETE_MULTIPLE_ITEM:
	    ids->caption_resource_id  = IDS_DELETEITEM_CAPTION;
	    ids->text_resource_id  = IDS_DELETEMULTIPLE_TEXT;
	    return TRUE;
	  case ASK_OVERWRITE_FILE:
	    ids->caption_resource_id  = IDS_OVERWRITEFILE_CAPTION;
	    ids->text_resource_id  = IDS_OVERWRITEFILE_TEXT;
	    return TRUE;
	  default:
	    FIXME(" Unhandled nKindOfDialog %d stub\n", nKindOfDialog);
	}
	return FALSE;
}

BOOL SHELL_ConfirmDialog(int nKindOfDialog, LPCSTR szDir)
{
	CHAR szCaption[255], szText[255], szBuffer[MAX_PATH + 256];
	SHELL_ConfirmIDstruc ids;

	if (!SHELL_ConfirmIDs(nKindOfDialog, &ids))
	  return FALSE;

	LoadStringA(shell32_hInstance, ids.caption_resource_id, szCaption, sizeof(szCaption));
	LoadStringA(shell32_hInstance, ids.text_resource_id, szText, sizeof(szText));

	FormatMessageA(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
	               szText, 0, 0, szBuffer, sizeof(szBuffer), (va_list*)&szDir);

	return (IDOK == MessageBoxA(GetActiveWindow(), szBuffer, szCaption, MB_OKCANCEL | MB_ICONEXCLAMATION));
}

BOOL SHELL_ConfirmDialogW(int nKindOfDialog, LPCWSTR szDir)
{
	WCHAR szCaption[255], szText[255], szBuffer[MAX_PATH + 256];
	SHELL_ConfirmIDstruc ids;

	if (!SHELL_ConfirmIDs(nKindOfDialog, &ids))
	  return FALSE;

	LoadStringW(shell32_hInstance, ids.caption_resource_id, szCaption, sizeof(szCaption));
	LoadStringW(shell32_hInstance, ids.text_resource_id, szText, sizeof(szText));

	FormatMessageW(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
	               szText, 0, 0, szBuffer, sizeof(szBuffer), (va_list*)&szDir);

	return (IDOK == MessageBoxW(GetActiveWindow(), szBuffer, szCaption, MB_OKCANCEL | MB_ICONEXCLAMATION));
}

/**************************************************************************
 * SHELL_DeleteDirectoryA()  [internal]
 *
 * like rm -r
 */
BOOL SHELL_DeleteDirectoryA(LPCSTR pszDir, BOOL bShowUI)
{
	BOOL    ret = TRUE;
	HANDLE  hFind;
	WIN32_FIND_DATAA wfd;
	char    szTemp[MAX_PATH];

	/* Make sure the directory exists before eventually prompting the user */
	PathCombineA(szTemp, pszDir, aWildcardFile);
	hFind = FindFirstFileA(szTemp, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	  return FALSE;

	if (!bShowUI || SHELL_ConfirmDialog(ASK_DELETE_FOLDER, pszDir))
	{
	  do
	  {
	    LPSTR lp = wfd.cAlternateFileName;
	    if (!lp[0])
	      lp = wfd.cFileName;
	    if (IsDotDir(lp))
	      continue;
	    PathCombineA(szTemp, pszDir, lp);
	    if (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)
	      ret = SHELL_DeleteDirectoryA(szTemp, FALSE);
	    else
	      ret = SHNotifyDeleteFileA(szTemp);
	  } while (ret && FindNextFileA(hFind, &wfd));
	}
	FindClose(hFind);
	if (ret)
	  ret = SHNotifyRemoveDirectoryA(pszDir);
	return ret;
}

BOOL SHELL_DeleteDirectoryW(LPCWSTR pszDir, BOOL bShowUI)
{
	BOOL    ret = TRUE;
	HANDLE  hFind;
	WIN32_FIND_DATAW wfd;
	WCHAR   szTemp[MAX_PATH];

	/* Make sure the directory exists before eventually prompting the user */
	PathCombineW(szTemp, pszDir, wWildcardFile);
	hFind = FindFirstFileW(szTemp, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	  return FALSE;

	if (!bShowUI || SHELL_ConfirmDialogW(ASK_DELETE_FOLDER, pszDir))
	{
	  do
	  {
	    LPWSTR lp = wfd.cAlternateFileName;
	    if (!lp[0])
	      lp = wfd.cFileName;
	    if (IsDotDir(lp))
	      continue;
	    PathCombineW(szTemp, pszDir, lp);
	    if (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)
	      ret = SHELL_DeleteDirectoryW(szTemp, FALSE);
	    else
	      ret = SHNotifyDeleteFileW(szTemp);
	  } while (ret && FindNextFileW(hFind, &wfd));
	}
	FindClose(hFind);
	if (ret)
	  ret = SHNotifyRemoveDirectoryW(pszDir);
	return ret;
}

/**************************************************************************
 *  SHELL_DeleteFileA()      [internal]
 */
BOOL SHELL_DeleteFileA(LPCSTR pszFile, BOOL bShowUI)
{
	if (bShowUI && !SHELL_ConfirmDialog(ASK_DELETE_FILE, pszFile))
	  return FALSE;

	return SHNotifyDeleteFileA(pszFile);
}

BOOL SHELL_DeleteFileW(LPCWSTR pszFile, BOOL bShowUI)
{
	if (bShowUI && !SHELL_ConfirmDialogW(ASK_DELETE_FILE, pszFile))
	  return FALSE;

	return SHNotifyDeleteFileW(pszFile);
}

/**************************************************************************
 * Win32CreateDirectory      [SHELL32.93]
 *
 * Creates a directory. Also triggers a change notify if one exists.
 *
 * PARAMS
 *  path       [I]   path to directory to create
 *
 * RETURNS
 *  TRUE if successful, FALSE otherwise
 *
 * NOTES:
 *  Verified on Win98 / IE 5 (SHELL32 4.72, March 1999 build) to be ANSI.
 *  This is Unicode on NT/2000
 */
static BOOL SHNotifyCreateDirectoryA(LPCSTR path, LPSECURITY_ATTRIBUTES sec)
{
	BOOL ret;
	TRACE("(%s, %p)\n", debugstr_a(path), sec);

	ret = CreateDirectoryA(path, sec);
	if (ret)
	{
	  SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHA, path, NULL);
	}
	return ret;
}

static BOOL SHNotifyCreateDirectoryW(LPCWSTR path, LPSECURITY_ATTRIBUTES sec)
{
	BOOL ret;
	TRACE("(%s, %p)\n", debugstr_w(path), sec);

	ret = CreateDirectoryW(path, sec);
	if (ret)
	{
	  SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, path, NULL);
	}
	return ret;
}

BOOL WINAPI Win32CreateDirectoryAW(LPCVOID path, LPSECURITY_ATTRIBUTES sec)
{
	if (SHELL_OsIsUnicode())
	  return SHNotifyCreateDirectoryW(path, sec);
	return SHNotifyCreateDirectoryA(path, sec);
}

/************************************************************************
 * Win32RemoveDirectory      [SHELL32.94]
 *
 * Deletes a directory. Also triggers a change notify if one exists.
 *
 * PARAMS
 *  path       [I]   path to directory to delete
 *
 * RETURNS
 *  TRUE if successful, FALSE otherwise
 *
 * NOTES:
 *  Verified on Win98 / IE 5 (SHELL32 4.72, March 1999 build) to be ANSI.
 *  This is Unicode on NT/2000
 */
static BOOL SHNotifyRemoveDirectoryA(LPCSTR path)
{
	BOOL ret;
	TRACE("(%s)\n", debugstr_a(path));

	ret = RemoveDirectoryA(path);
	if (!ret)
	{
	  /* Directory may be write protected */
	  DWORD dwAttr = GetFileAttributesA(path);
	  if (dwAttr != -1 && dwAttr & FILE_ATTRIBUTE_READONLY)
	    if (SetFileAttributesA(path, dwAttr & ~FILE_ATTRIBUTE_READONLY))
	      ret = RemoveDirectoryA(path);
	}
	if (ret)
	  SHChangeNotify(SHCNE_RMDIR, SHCNF_PATHA, path, NULL);
	return ret;
}

static BOOL SHNotifyRemoveDirectoryW(LPCWSTR path)
{
	BOOL ret;
	TRACE("(%s)\n", debugstr_w(path));

	ret = RemoveDirectoryW(path);
	if (!ret)
	{
	  /* Directory may be write protected */
	  DWORD dwAttr = GetFileAttributesW(path);
	  if (dwAttr != -1 && dwAttr & FILE_ATTRIBUTE_READONLY)
	    if (SetFileAttributesW(path, dwAttr & ~FILE_ATTRIBUTE_READONLY))
	      ret = RemoveDirectoryW(path);
	}
	if (ret)
	  SHChangeNotify(SHCNE_RMDIR, SHCNF_PATHW, path, NULL);
	return ret;
}

BOOL WINAPI Win32RemoveDirectoryAW(LPCVOID path)
{
	if (SHELL_OsIsUnicode())
	  return SHNotifyRemoveDirectoryW(path);
	return SHNotifyRemoveDirectoryA(path);
}

/************************************************************************
 * Win32DeleteFile           [SHELL32.164]
 *
 * Deletes a file. Also triggers a change notify if one exists.
 *
 * PARAMS
 *  path       [I]   path to file to delete
 *
 * RETURNS
 *  TRUE if successful, FALSE otherwise
 *
 * NOTES:
 *  Verified on Win98 / IE 5 (SHELL32 4.72, March 1999 build) to be ANSI.
 *  This is Unicode on NT/2000
 */
static BOOL SHNotifyDeleteFileA(LPCSTR path)
{
	BOOL ret;

	TRACE("(%s)\n", debugstr_a(path));

	ret = DeleteFileA(path);
	if (!ret)
	{
	  /* File may be write protected or a system file */
	  DWORD dwAttr = GetFileAttributesA(path);
	  if ((dwAttr != -1) && (dwAttr & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
	    if (SetFileAttributesA(path, dwAttr & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
	      ret = DeleteFileA(path);
	}
	if (ret)
	  SHChangeNotify(SHCNE_DELETE, SHCNF_PATHA, path, NULL);
	return ret;
}

static BOOL SHNotifyDeleteFileW(LPCWSTR path)
{
	BOOL ret;

	TRACE("(%s)\n", debugstr_w(path));

	ret = DeleteFileW(path);
	if (!ret)
	{
	  /* File may be write protected or a system file */
	  DWORD dwAttr = GetFileAttributesW(path);
	  if ((dwAttr != -1) && (dwAttr & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
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
	  return SHNotifyDeleteFileW(path);
	return SHNotifyDeleteFileA(path);
}

/************************************************************************
 * SHNotifyMoveFile          [internal]
 *
 * Moves a file. Also triggers a change notify if one exists.
 *
 * PARAMS
 *  src        [I]   path to source file to move
 *  dest       [I]   path to target file to move to
 *
 * RETURNS
 *  TRUE if successful, FALSE otherwise
 */
static BOOL SHNotifyMoveFileW(LPCWSTR src, LPCWSTR dest)
{
	BOOL ret;

	TRACE("(%s %s)\n", debugstr_w(src), debugstr_w(dest));

	ret = MoveFileW(src, dest);
	if (!ret)
	{
	  /* Source file may be write protected or a system file */
	  DWORD dwAttr = GetFileAttributesW(src);
	  if ((dwAttr != -1) && (dwAttr & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
	    if (SetFileAttributesW(src, dwAttr & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
	      ret = MoveFileW(src, dest);
	}
	if (ret)
	  SHChangeNotify(SHCNE_RENAMEITEM, SHCNF_PATHW, src, dest);
	return ret;
}

/************************************************************************
 * SHNotifyCopyFile          [internal]
 *
 * Copies a file. Also triggers a change notify if one exists.
 *
 * PARAMS
 *  src        [I]   path to source file to move
 *  dest       [I]   path to target file to move to
 *  bRename    [I]   if TRUE, the target file will be renamed if a
 *                   file with this name already exists
 *
 * RETURNS
 *  TRUE if successful, FALSE otherwise
 */
static BOOL SHNotifyCopyFileW(LPCWSTR src, LPCWSTR dest, BOOL bRename)
{
	BOOL ret;

	TRACE("(%s %s %s)\n", debugstr_w(src), debugstr_w(dest), bRename ? "renameIfExists" : "");

	ret = CopyFileW(src, dest, TRUE);
	if (!ret && bRename)
	{
	  /* Destination file probably exists */
	  DWORD dwAttr = GetFileAttributesW(dest);
	  if (dwAttr != -1)
	  {
	    FIXME("Rename on copy to existing file not implemented!");
	  }
	}
	if (ret)
	  SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW, dest, NULL);
	return ret;
}

/*************************************************************************
 * SHCreateDirectory         [SHELL32.165]
 *
 * Create a directory at the specified location
 *
 * PARAMS
 *  hWnd       [I]
 *  path       [I]   path of directory to create
 *
 * RETURNS
 *  ERRROR_SUCCESS or one of the following values:
 *  ERROR_BAD_PATHNAME if the path is relative
 *  ERROR_FILE_EXISTS when a file with that name exists
 *  ERROR_ALREADY_EXISTS when the directory already exists
 *  ERROR_FILENAME_EXCED_RANGE if the filename was to long to process
 *
 * NOTES
 *  exported by ordinal
 *  Win9x exports ANSI
 *  WinNT/2000 exports Unicode
 */
DWORD WINAPI SHCreateDirectory(HWND hWnd, LPCVOID path)
{
	if (SHELL_OsIsUnicode())
	  return SHCreateDirectoryExW(hWnd, path, NULL);
	return SHCreateDirectoryExA(hWnd, path, NULL);
}

/*************************************************************************
 * SHCreateDirectoryExA      [SHELL32.@]
 *
 * Create a directory at the specified location
 *
 * PARAMS
 *  hWnd       [I]
 *  path       [I]   path of directory to create
 *  sec        [I]   security attributes to use or NULL
 *
 * RETURNS
 *  ERRROR_SUCCESS or one of the following values:
 *  ERROR_BAD_PATHNAME if the path is relative
 *  ERROR_FILE_EXISTS when a file with that name exists
 *  ERROR_ALREADY_EXISTS when the directory already exists
 *  ERROR_FILENAME_EXCED_RANGE if the filename was to long to process
 */
DWORD WINAPI SHCreateDirectoryExA(HWND hWnd, LPCSTR path, LPSECURITY_ATTRIBUTES sec)
{
	WCHAR wPath[MAX_PATH];
	TRACE("(%p, %s, %p)\n",hWnd, debugstr_a(path), sec);

	MultiByteToWideChar(CP_ACP, 0, path, -1, wPath, MAX_PATH);
	return SHCreateDirectoryExW(hWnd, wPath, sec);
}

/*************************************************************************
 * SHTryFind      [intern]
 * Try for existing names
 */
DWORD SHTryFindW(LPCWSTR pName)
{
	WCHAR szName [MAX_PATH + 4];
	DWORD retCode = ERROR_FILE_NOT_FOUND; 
	WIN32_FIND_DATAW wfd;
	HANDLE  hFind;
	LPCWSTR pFileName = StrRChrW(pName,NULL,'\\');
	if ((!pFileName) || \ 
	  /* we must isolate the part befor '\' */ 
	  (lstrcpynW(&szName[0],pName,(pFileName-pName)),
	   NULL != StrPBrkW(&szName[0], wWildcardChars)))
	{
	  return ERROR_INVALID_NAME;
	}
	hFind = FindFirstFileW(pName, &wfd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
	  FindClose(hFind);
	  retCode = ERROR_ALREADY_EXISTS;
	}
	return retCode;
}
/*************************************************************************
 * SHCreateDirectoryExW      [SHELL32.@]
 */
DWORD WINAPI SHCreateDirectoryExW(HWND hWnd, LPCWSTR path, LPSECURITY_ATTRIBUTES sec)
{
	DWORD ret = SHTryFindW(path);
	TRACE("(%p, %s, %p)\n",hWnd, debugstr_w(path), sec);

	if (ERROR_FILE_NOT_FOUND == ret)
	{
	  ret = ERROR_SUCCESS;
	  if (PathIsRelativeW(path))
	  {
	    ret = ERROR_BAD_PATHNAME;
	  }
	}
	SetLastError(ret);
	if (ERROR_SUCCESS == ret)
	{
	  if (!SHNotifyCreateDirectoryW(path, sec))
	  {
	    ret = GetLastError();
	    if (ret != ERROR_FILE_EXISTS &&
	        ret != ERROR_ALREADY_EXISTS &&
	        ret != ERROR_FILENAME_EXCED_RANGE)
	    {
	    /* handling network file names?
	      lstrcpynW(pathName, path, MAX_PATH);
	      lpStr = PathAddBackslashW(pathName);*/
	      FIXME("Semi-stub, non zero hWnd should be used somehow?");
	    }
	  }
	}
	return ret;
}

/*************************************************************************
 *
 * SHFileStrICmp HelperFunction for SHFileOperationW
 *
 */
BOOL SHFileStrICmpW(LPWSTR p1, LPWSTR p2, LPWSTR p1End, LPWSTR p2End)
{
	WCHAR C1 = '\0';
	WCHAR C2 = '\0';
	int i_Temp = -1;
	int i_len1 = lstrlenW(p1);
	int i_len2 = lstrlenW(p2);

	if (p1End && (&p1[i_len1] >= p1End) && ('\\' == p1End[0]))
	{
	  C1 = p1End[0];
	  p1End[0] = '\0';
	  i_len1 = lstrlenW(p1);
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
	i_len2 = lstrlenW(p2);
	if (i_len1 == i_len2)
	  i_Temp = lstrcmpiW(p1,p2);
	if (C1)
	  p1[i_len1] = C1;
	if (C2)
	  p2[i_len2] = C2;
	return !(i_Temp);
}

/*************************************************************************
 *
 * SHFileStrCpyCat HelperFunction for SHFileOperationW
 *
 */
LPWSTR SHFileStrCpyCatW(LPWSTR pTo, LPCWSTR pFrom, LPCWSTR pCatStr)
{
	LPWSTR pToFile = NULL;
	int  i_len;
	if (pTo)
	{
	  if (pFrom)
	    lstrcpyW(pTo, pFrom);
	  if (pCatStr)
	  {
	    i_len = lstrlenW(pTo);
	    if ((i_len) && (pTo[--i_len] != '\\'))
	      i_len++;
	    pTo[i_len] = '\\';
	    if (pCatStr[0] == '\\')
	      pCatStr++; \
	    lstrcpyW(&pTo[i_len+1], pCatStr);
	  }
	  pToFile = StrRChrW(pTo,NULL,'\\');
/* !! termination of the new string-group */
	  pTo[(lstrlenW(pTo)) + 1] = '\0';
	}
	return pToFile;
}

/**************************************************************************
 *	SHELL_FileNamesMatch()
 *
 * Accepts two \0 delimited lists of the file names. Checks whether number of
 * files in both lists is the same, and checks also if source-name exists.
 */
BOOL SHELL_FileNamesMatch(LPCWSTR pszFiles1, LPCWSTR pszFiles2, BOOL bOnlySrc)
{
	LPWSTR pszTemp;
	while ((pszFiles1[0] != '\0') &&
	       (bOnlySrc || (pszFiles2[0] != '\0')))
	{
	  pszTemp = StrChrW(pszFiles1,'\\');
	  /* root (without mask/name) is also not allowed as source, tested in W98 */
	  if (!pszTemp || !pszTemp[1])
	    return FALSE;
	  pszTemp = StrPBrkW(pszFiles1, wWildcardChars);
	  if (pszTemp)
	  {
	    WCHAR szMask [MAX_PATH + 4];
	    strncpyW(&szMask[0],pszFiles1,(pszTemp - pszFiles1));
	    pszTemp = StrRChrW(&szMask[0],NULL,'\\');
	    if (!pszTemp)
	      return FALSE;
	    pszTemp[0] = '\0';
        /* we will check the root of the mask as valid dir */
	    if (!IsAttribDir(GetFileAttributesW(&szMask[0])))
	      return FALSE;
	  }
	  else
	  {
	    if (-1 == GetFileAttributesW(pszFiles1))
	      return FALSE;
	  }
	  pszFiles1 += lstrlenW(pszFiles1) + 1;
	  if (!bOnlySrc)
	    pszFiles2 += lstrlenW(pszFiles2) + 1;
	}
	return ((pszFiles1[0] == '\0') && (bOnlySrc || (pszFiles2[0] == '\0')));
}

/*************************************************************************
 *
 * SHName(s)Translate HelperFunction for SHFileOperationW
 *
 * Translates a list of 0 terminated ASCI strings into Unicode. If *wString
 * is NULL, only the necessary size of the string is determined and returned,
 * otherwise the ASCII strings are copied into it and the buffer is increased
 * to point to the location after the final 0 termination char.
 */
DWORD SHNameTranslate(LPWSTR* wString, LPCWSTR* pWToFrom, BOOL more)
{
	DWORD size = 0, aSize = 0;
	LPCSTR aString = (LPCSTR)*pWToFrom;

	if (aString)
	{
	  do
	  {
	    size = lstrlenA(aString) + 1;
	    aSize += size;
	    aString += size;
	  } while ((size != 1) && more);
	  /* The two sizes might be different in the case of multibyte chars */
	  size = MultiByteToWideChar(CP_ACP, 0, aString, aSize, *wString, 0);
	  if (*wString) /* only in the second loop */
	  {
	    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)*pWToFrom, aSize, *wString, size);
	    *pWToFrom = *wString;
	    *wString += size;
	  }
	}
	return size;
}
/*************************************************************************
 * SHFileOperationA          [SHELL32.@]
 *
 * Function to copy, move, delete and create one or more files with optional
 * user prompts.
 *
 * PARAMS
 *  lpFileOp   [I/O] pointer to a structure containing all the necessary information
 *
 * NOTES
 *  exported by name
 */
DWORD WINAPI SHFileOperationA(LPSHFILEOPSTRUCTA lpFileOp)
{
	SHFILEOPSTRUCTW nFileOp = *((LPSHFILEOPSTRUCTW)lpFileOp);
	DWORD retCode = 0, size;
	LPWSTR ForFree = NULL, /* we change wString in SHNameTranslate and can't use it for freeing */
	       wString = NULL; /* we change this in SHNameTranslate */

#ifdef __WIN32OS2__
	TRACE("SHFileOperationA");
#else
	TRACE("\n");
#endif
	if (FO_DELETE == (nFileOp.wFunc & FO_MASK))
	  nFileOp.pTo = NULL; /* we need a NULL or a valid pointer for translation */
	if (!(nFileOp.fFlags & FOF_SIMPLEPROGRESS))
	  nFileOp.lpszProgressTitle = NULL; /* we need a NULL or a valid pointer for translation */
	while (1) /* every loop calculate size, second translate also, if we have storage for this */
	{
	  size = SHNameTranslate(&wString, &nFileOp.lpszProgressTitle, FALSE); /* no loop */
	  size += SHNameTranslate(&wString, &nFileOp.pFrom, TRUE); /* internal loop */
	  size += SHNameTranslate(&wString, &nFileOp.pTo, TRUE); /* internal loop */

	  if (ForFree)
	  {
	    retCode = SHFileOperationW(&nFileOp);
	    HeapFree(GetProcessHeap(), 0, ForFree); /* we can not use wString, it was changed */
	    lpFileOp->hNameMappings         = nFileOp.hNameMappings;
	    lpFileOp->fAnyOperationsAborted = nFileOp.fAnyOperationsAborted;
	    return retCode;
	  }
	  else
	  {
	    wString = ForFree = HeapAlloc(GetProcessHeap(), 0, size * sizeof(WCHAR));
	    if (ForFree) continue;
	    retCode = ERROR_OUTOFMEMORY;
	    SetLastError(retCode);
	    return retCode;
	  }
	}
}
/*************************************************************************
 * SHFileOperationCheck
 */
DWORD SHFileOperationCheck(LPSHFILEOPSTRUCTW lpFileOp, LPCSTR* cFO_Name)
{
	FILEOP_FLAGS OFl = (FILEOP_FLAGS)lpFileOp->fFlags;
	LPCSTR cFO_Temp [] = {"FO_????","FO_MOVE","FO_COPY","FO_DELETE","FO_RENAME"};
	long retCode = NO_ERROR;
	long FuncSwitch = (lpFileOp->wFunc & FO_MASK);

	/*  default no error */

	if ((FuncSwitch < FO_MOVE) || (FuncSwitch > FO_RENAME))
	{
	  FuncSwitch = 0;
	  retCode = ERROR_INVALID_PARAMETER;
	}
	cFO_Name[0] = cFO_Temp [FuncSwitch];

	if (!(~FO_MASK & lpFileOp->wFunc)) /* ? Level == 0 */
	{ 
	  lpFileOp->fAnyOperationsAborted = FALSE;
	  TRACE("%s: flags (0x%04x) : %s%s%s%s%s%s%s%s%s%s%s%s%s \n",cFO_Name[0], OFl,
	              OFl & FOF_MULTIDESTFILES ? "FOF_MULTIDESTFILES " : "",
	              OFl & FOF_CONFIRMMOUSE ? "FOF_CONFIRMMOUSE " : "",
	              OFl & FOF_SILENT ? "FOF_SILENT " : "",
	              OFl & FOF_RENAMEONCOLLISION ? "FOF_RENAMEONCOLLISION " : "",
	              OFl & FOF_NOCONFIRMATION ? "FOF_NOCONFIRMATION " : "",
	              OFl & FOF_WANTMAPPINGHANDLE ? "FOF_WANTMAPPINGHANDLE " : "",
	              OFl & FOF_ALLOWUNDO ? "FOF_ALLOWUNDO " : "",
	              OFl & FOF_FILESONLY ? "FOF_FILESONLY " : "",
	              OFl & FOF_SIMPLEPROGRESS ? "FOF_SIMPLEPROGRESS " : "",
	              OFl & FOF_NOCONFIRMMKDIR ? "FOF_NOCONFIRMMKDIR " : "",
	              OFl & FOF_NOERRORUI ? "FOF_NOERRORUI " : "",
	              OFl & FOF_NOCOPYSECURITYATTRIBS ? "FOF_NOCOPYSECURITYATTRIBS" : "",
	              OFl & 0xf000 ? "MORE-UNKNOWN-Flags" : "");
	  OFl &= (~(FOF_MULTIDESTFILES | FOF_NOCONFIRMATION | FOF_FILESONLY));  /* implemented */
	  OFl ^= (FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_NOCOPYSECURITYATTRIBS); /* ignored, if one */
	  OFl &= (~FOF_SIMPLEPROGRESS);                      /* ignored, only with FOF_SILENT */
	  if (OFl)
	  {
	    if (OFl & (~(FOF_CONFIRMMOUSE | FOF_SILENT | FOF_RENAMEONCOLLISION |
	                 FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_NOCOPYSECURITYATTRIBS)))
	    {
	            TRACE("%s lpFileOp->fFlags=0x%x not implemented, Aborted=TRUE, stub\n", cFO_Name[0], OFl);
	      retCode = ERROR_INVALID_PARAMETER;
	    }
	    else
	    {
	            TRACE("%s lpFileOp->fFlags=0x%x not fully implemented, stub\n", cFO_Name[0], OFl);
	    } /* endif */
	  } /* endif */
	}
	return retCode;
}
/*************************************************************************
 *
 * SHFileOperationMove HelperFunction for SHFileOperationW
 *
 * Contains the common part of FO_MOVE/FO_RENAME.
 * It is not with recursive call solvable.
 * We have both tryed FO_RENAME contains not FO_MOVE and
 * also FO_RENAME does not contains FO_MOVE, only common Parts.
 */
DWORD SHFileOperationMove(LPSHFILEOPSTRUCTW lpFileOp)
{
	DWORD retCode;
	WCHAR szToName [MAX_PATH + 4];
	LPWSTR pToFile;
	if (NULL != StrPBrkW(lpFileOp->pTo, wWildcardChars))
	{
#ifdef W98_FO_FUNCTION /* W98 */
	  return ERROR_FILE_NOT_FOUND;
#else
	  return ERROR_INVALID_NAME;
#endif
	}
	retCode = SHTryFindW(lpFileOp->pTo);
	/* FOF_NOCONFIRMATION, FOF_RENAMEONCOLLISION ? */
	if (retCode != ERROR_FILE_NOT_FOUND)
	{
	  return retCode;
	}
	lstrcpyW(&szToName[0],lpFileOp->pTo);
	pToFile = StrRChrW(&szToName[0],NULL,'\\');
	if (!pToFile[1])
	{
	  pToFile[0] = '\0';
	}
	/* we use SHNotifyMoveFile() instead MoveFileW */
	if (!SHNotifyMoveFileW(lpFileOp->pFrom, &szToName[0]))
	{
	  /* we need still the value for the returncode */
	  return GetLastError();
	}
	return ERROR_SUCCESS;
}
/*************************************************************************
 * SHFileOperationW          [SHELL32.@]
 *
 * See SHFileOperationA
 */
DWORD WINAPI SHFileOperationW(LPSHFILEOPSTRUCTW lpFileOp)
{
	SHFILEOPSTRUCTW nFileOp = lpFileOp ? *(lpFileOp):nFileOp;

	LPCWSTR pNextFrom = nFileOp.pFrom;
	LPCWSTR pNextTo = nFileOp.pTo;
	LPCWSTR pFrom = pNextFrom;
	LPCWSTR pTo = NULL;
#ifdef __WIN32OS2__
#ifdef DEBUG
#define _SHFO_DEBUGSTR_
#ifdef _SHFO_DEBUGSTR_
	CHAR Debug_pFrom [MAX_PATH + 4];
	CHAR Debug_pTo [MAX_PATH + 4];
#endif
#endif
#endif
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW wfd;
	LPWSTR pTempFrom = NULL;
	LPWSTR pTempTo = NULL;
	LPWSTR pFromFile;
	LPWSTR pToFile = NULL;
	LPWSTR pToTailSlash; /* points also to ToMask, we know not what is this */
	LPWSTR lpFileName; /* temporary for pFromFile,pToFile,pToTailSlash */
	DWORD ToAttr;
	DWORD ToPathAttr;
	DWORD FromAttr;
	DWORD FromPathAttr;
	
	BOOL b_Multi = (nFileOp.fFlags & FOF_MULTIDESTFILES);

	BOOL b_MultiTo = (FO_DELETE != (lpFileOp->wFunc & FO_MASK));
	BOOL b_MultiPaired = FALSE;
	BOOL b_MultiFrom = FALSE;
	BOOL not_overwrite;
	BOOL ask_overwrite;
	BOOL b_SameRoot;
	BOOL b_SameRoot_MOVE;
	BOOL b_ToMask = FALSE;
	BOOL b_FromMask;

	long FuncSwitch = (nFileOp.wFunc & FO_MASK);
	long level= nFileOp.wFunc>>FO_LevelShift;

	LPCSTR cFO_Name;
	long retCode = SHFileOperationCheck(&nFileOp,&cFO_Name);

	if (retCode)
	{
#ifdef W98_FO_FUNCTION /* W98 returns NO_ERROR */
	  if ('?' == cFO_Name[3]) 
	    retCode = NO_ERROR;
#endif
	  goto shfileop_exit; /* no valid FunctionCode */
	}

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
 * FOF_MULTIDESTFILES, FOF_NOCONFIRMATION, FOF_FILESONLY  are implemented
 * FOF_CONFIRMMOUSE, FOF_SILENT, FOF_NOCONFIRMMKDIR,
 *       FOF_SIMPLEPROGRESS, FOF_NOCOPYSECURITYATTRIBS    are not implemented and ignored
 * FOF_RENAMEONCOLLISION                                  are implemented partially and breaks if file exist
 * FOF_ALLOWUNDO, FOF_WANTMAPPINGHANDLE                   are not implemented and breaks
 * if any other flag set, an error occurs
 */
	  TRACE(" %s level=%ld nFileOp.fFlags=0x%x\n", cFO_Name, level, nFileOp.fFlags);

	  if ((pNextFrom) && (!(b_MultiTo) || (pNextTo)))
	  {
	    nFileOp.pFrom = pTempFrom = HeapAlloc(GetProcessHeap(), 0, ((1 + 2 * (b_MultiTo)) * MAX_PATH + 6) * sizeof(WCHAR));
	    if (!pTempFrom)
	    {
	      retCode = ERROR_OUTOFMEMORY;
	      SetLastError(retCode);
	      goto shfileop_exit;
	    }
	    if (b_MultiTo)
	      pTempTo = &pTempFrom[MAX_PATH + 4];
	    nFileOp.pTo = pTempTo;
	    ask_overwrite = (!(nFileOp.fFlags & FOF_NOCONFIRMATION) && !(nFileOp.fFlags & FOF_RENAMEONCOLLISION));
	    not_overwrite = (!(nFileOp.fFlags & FOF_NOCONFIRMATION) ||  (nFileOp.fFlags & FOF_RENAMEONCOLLISION));
	  }
	  else
	  {
	    retCode = 0x402;      /* 1026 */
	    goto shfileop_exit;
	  }
	  /* need break at error before change sourcepointer */
	  while(!retCode && (pNextFrom[0]))
	  {
	    nFileOp.wFunc =  ((level + 1) << FO_LevelShift) + FuncSwitch;
	    nFileOp.fFlags = lpFileOp->fFlags;

	    if (b_MultiTo)
	    {
	      pTo = pNextTo;
	      pNextTo = &pNextTo[lstrlenW(pTo)+1];
	      b_MultiTo = (b_Multi && pNextTo[0]);
	    }

	    pFrom = pNextFrom;
#ifdef _SHFO_DEBUGSTR_
	    /* for seeing in debugger outside from TRACE */
	    strcpy(&Debug_pFrom[0],debugstr_w(pFrom));
	    strcpy(&Debug_pTo[0],debugstr_w(pTo));
	    TRACE("%s level=%ld with %s %s%s\n",
	      cFO_Name, level, &Debug_pFrom[0], pTo ? "-> ":"", &Debug_pTo[0]);
#endif
	    pNextFrom = &pNextFrom[lstrlenW(pNextFrom)+1];
	    if (!b_MultiFrom && !b_MultiTo)
	      b_MultiFrom = (pNextFrom[0]);

	    pFromFile = SHFileStrCpyCatW(pTempFrom, pFrom, NULL);
	    b_FromMask = (pFromFile && (NULL != StrPBrkW(&pFromFile[1], wWildcardChars)));

	    if (pTo)
	    {
	      pToFile = SHFileStrCpyCatW(pTempTo, pTo, NULL);
	      b_ToMask = (NULL != StrPBrkW(pTempTo, wWildcardChars));
	    }

	    if (FO_RENAME == FuncSwitch)
	    {
	      if (b_MultiTo || b_MultiFrom || (b_FromMask && !b_ToMask))
	      {
#define retCodeIsInvalid 0x075 /* W98 */
#ifndef W98_FO_FUNCTION /* is W2K */
#undef retCodeIsInvalid
#define retCodeIsInvalid 0x4c7 /* W2K */
	        retCode = 0x1f;    /* value 31 in W2K, W98 no work, only RC=0 */
#endif
	        goto shfileop_exit;
	      }
	    }

	    if (!b_MultiPaired)
	    {
	      b_MultiPaired =
	        SHELL_FileNamesMatch(lpFileOp->pFrom,
	              lpFileOp->pTo, ((FO_DELETE == FuncSwitch) || !b_Multi || b_MultiFrom));
	    } /* endif */
	    if (!(b_MultiPaired) || !(pFromFile) || !(pFromFile[1]) || ((pTo) && !(pToFile)))
	    {
	      retCode = 0x402;      /* 1026 */
	      goto shfileop_exit;
	    }

	    if (FO_DELETE == FuncSwitch)
	    {
	      hFind = FindFirstFileW(pFrom, &wfd);
	      if (INVALID_HANDLE_VALUE == hFind)
	        /* no problem for FO_DELETE */
	        continue;
	      do
	      {
	        lpFileName = wfd.cAlternateFileName;
	        if (!lpFileName[0])
	           lpFileName = wfd.cFileName;
	        if (IsDotDir(lpFileName) ||
	            ((b_FromMask) && IsAttribDir(wfd.dwFileAttributes) && (nFileOp.fFlags & FOF_FILESONLY)))
	          continue;
	        SHFileStrCpyCatW(&pFromFile[1], lpFileName, NULL);
	        /* TODO: Check the SHELL_DeleteFileOrDirectoryW() function in shell32.dll */
	        if (IsAttribFile(wfd.dwFileAttributes))
	        {
	          if (!SHNotifyDeleteFileW(pTempFrom))
	            retCode = 0x78; /* value unknown */
	        }
	        else
	        {
	          if (!SHELL_DeleteDirectoryW(pTempFrom, (!(nFileOp.fFlags & FOF_NOCONFIRMATION))))
	            retCode = 0x79; /* value unknown */
	        }
	      } while (!retCode && FindNextFileW(hFind, &wfd));
	      FindClose(hFind);
	      continue;
	    } /* FO_DELETE ends, pTo must be always valid from here */

	    pToTailSlash = NULL;
	    if ((pToFile) && !(pToFile[1]))
	    {
	      pToFile[0] = '\0';
	      lpFileName = StrRChrW(pTempTo,NULL,'\\');
	      if (lpFileName)
	      {
	        pToTailSlash = pToFile; 
	        pToFile = lpFileName;
	      }
	    }
	    ToPathAttr = ToAttr = GetFileAttributesW(pTempTo);
	    FromAttr = GetFileAttributesW(pTempFrom);
	    b_SameRoot = (toupperW(pTempFrom[0]) == toupperW(pTempTo[0]));
	    b_SameRoot_MOVE = (b_SameRoot && (FO_MOVE == FuncSwitch));
	    if ((pToFile) && !IsAttribDir(ToAttr))
	    {
	      pToFile[0] = '\0';
	      ToPathAttr = GetFileAttributesW(pTempTo);
	      pToFile[0] = '\\';
	    }
	    if (pToTailSlash)
	    {
	      pToTailSlash[0] = '\\';
	    }
	    if (!b_FromMask && b_SameRoot && \
	      SHFileStrICmpW(pTempFrom, pTempTo, NULL, NULL))
	    { /* target is the same as source ? W98 has 0x71, W2K also */
	      retCode = 0x71; /* is no error with FOF_RENAMEONCOLLISION */
	      goto shfileop_exit;
	    } /* endif */
	    /* FO_RENAME is not only a Filter for FO_MOVE */
	    if (FO_RENAME == FuncSwitch)
	    {
	      if (b_FromMask || !b_SameRoot || \
	        !SHFileStrICmpW(pTempFrom, pTempTo, pFromFile, NULL) || \
	         SHFileStrICmpW(pTempFrom, pTempTo, pFromFile, HIGH_ADR))
	      {
	        retCode = 0x73; /* must be here or before, not later */
	        goto shfileop_exit;
	      }
#ifdef W98_FO_FUNCTION /* not in W2K, later in SHFileOperationMove */
	      if (!pToTailSlash && IsAttribDir(ToAttr))
	      {
	        retCode = ERROR_INVALID_NAME; /* FOF_NOCONFIRMATION, FOF_RENAMEONCOLLISION ? */
	        goto shfileop_exit;
	      }
	      if (-1 != ToPathAttr || b_ToMask)
#else
	      if (-1 != ToPathAttr)
#endif
	      {
	        retCode = SHFileOperationMove(&nFileOp);
	        goto shfileop_exit;
	      }
	    }
	    else \
	    if (!b_FromMask)
	    {
	      if (IsAttribDir(ToPathAttr))
	      {
	    /* Analycing for moving SourceName to as TargetName */
	        if (!b_MultiTo && IsAttribDir(ToAttr) && (b_MultiFrom || !b_Multi))
	        {
	          SHFileStrCpyCatW(pTempTo, NULL, pFromFile);
	          /* without FOF_MULTIDESTFILES shlfileop will create dir's recursive */
	          nFileOp.fFlags |= FOF_MULTIDESTFILES;
	          retCode = SHFileOperationW(&nFileOp);
	          continue;
	        }
	      }
	      /* What can we do with one pair and FO_MOVE/FO_COPY ? */
#ifndef W98_FO_FUNCTION /* W2K */
	      if (!b_SameRoot_MOVE || IsAttribDir(ToPathAttr))
#endif
	      if (!b_MultiFrom)
//	      if ((!b_MultiFrom) && (!b_SameRoot_MOVE || IsAttribDir(ToPathAttr) || b_ToMask))

	      {
	        if (IsAttribFile(FromAttr))
	        {
	          if (b_SameRoot_MOVE \
	       /* IsAttribDir(ToPathAttr) && !pToTailSlash && (bug) */
	       /* windos-bug, MOVE for File also with pToTailSlash, COPY not for this */
//	              && IsAttribDir(ToPathAttr))
	              && (IsAttribDir(ToPathAttr) || b_ToMask))
	          {
	       /* At the same drive, we can move for FO_MOVE, dir to dir was solved later */
	            retCode = SHFileOperationMove(&nFileOp);
	            continue;
	          } /* endif */
	          if (IsAttribDir(ToPathAttr) && !pToTailSlash && !IsAttribDir(ToAttr))
	          {
	            if (IsAttribFile(ToAttr) && not_overwrite)
	            {
	              /* Rename on Collision ? */
	              if (!ask_overwrite)
	              {
	                /* the retcode for this case is unknown */
	                retCode = 0x10008;
	                goto shfileop_exit;
	              }
	              /* we must change the dialog in the future for return 3-4 cases,
	               * 'Yes','No','Yes for all','Never ?' */
	              if (!SHELL_ConfirmDialogW(ASK_OVERWRITE_FILE, pTempTo))
	              {
	                nFileOp.fAnyOperationsAborted = TRUE;
	                continue;
	              } 
	            }
	            if (!(SHNotifyCopyFileW(pTempFrom, pTempTo, nFileOp.fFlags & FOF_RENAMEONCOLLISION)))
	            {
	            /* the retcode for this case is unknown */
	              retCode = 0x10009;
	            }
	            if ((FO_COPY == FuncSwitch) || retCode)
	              continue;
	            nFileOp.wFunc =  ((level+1) << FO_LevelShift) + FO_DELETE;
	            retCode = SHFileOperationW(&nFileOp);
	            continue;
	          }
	        }
	        if (IsAttribDir(FromAttr))
	        {
#ifdef W98_FO_FUNCTION /* not in W2K, -> retCode = retCodeIsInvalid */
	          if ((-1 == ToAttr) && (!b_SameRoot_MOVE || IsAttribDir(ToPathAttr) || b_ToMask))
	          {
	            if (-1 == ToPathAttr)
	            {
	              pToFile[0] = '\0';
	              retCode = SHCreateDirectoryExW(NULL,pTempTo, NULL);
	              ToPathAttr = GetFileAttributesW(pTempTo);
	              pToFile[0] = '\\';
	              if (retCode)
	                goto shfileop_exit;
	            }
#else
	          if (-1 == ToAttr)
	          {
#endif
	            if (IsAttribDir(ToPathAttr))
	            {
	              if (pToTailSlash)
	   	            pToTailSlash[0] = '\0';
	              retCode = SHCreateDirectoryExW(NULL,pTempTo, NULL);
	              ToAttr = GetFileAttributesW(pTempTo); /* for continuing */
	              if (pToTailSlash)
	   	            pToTailSlash[0] = '\\';
#ifndef W98_FO_FUNCTION /* W2k */
	                if (retCode)
	                  goto shfileop_exit;
#endif
	            }
	            if (-1 == ToAttr)
	            {
	              retCode = 0x10003;
	              goto shfileop_exit;
	            }
	          }
	          if (IsAttribDir(ToAttr))
	          {
	            /* both are existing dirs, we (FO_)MOVE/COPY the contence */
	            pToFile = SHFileStrCpyCatW(pTempFrom, NULL, wWildcardFile);
	            nFileOp.fFlags &= ~FOF_MULTIDESTFILES;
	            retCode = SHFileOperationW(&nFileOp);
	            if ((FO_COPY == FuncSwitch) || retCode)
	              continue;
	            ((DWORD*)pToFile)[0] = '\0';
	            nFileOp.wFunc =  ((level+1) << FO_LevelShift) + FO_DELETE;
	            retCode = SHFileOperationW(&nFileOp);
	            continue;
	          }
	        }
	      } /* end-!b_MultiFrom */
	    }

#ifdef W98_FO_FUNCTION /* not in W2K, -> retCode = retCodeIsInvalid */
	    if (b_FromMask && \
	    /* we do not know, why (b_SameRoot_MOVE && b_ToMask && (-1 != ToPathAttr)) */
	        (IsAttribDir(ToAttr) || (b_SameRoot_MOVE && b_ToMask)))
#else
	    if (b_FromMask && (-1 != ToPathAttr) && \
	    /* we do not know, why (b_SameRoot_MOVE && b_ToMask && (-1 != ToPathAttr)) */
	        (IsAttribDir(ToAttr) || (b_SameRoot_MOVE && b_ToMask)))
#endif
	    {
	      /* FO_COPY/FO_MOVE with mask, FO_DELETE are solved lang before */
	      hFind = FindFirstFileW(pFrom, &wfd);
	      if (INVALID_HANDLE_VALUE == hFind)
	      {
	        /* the retcode for this case is unknown */
	        retCode = 0x10007;
	        goto shfileop_exit;
	      }
	      nFileOp.fFlags &= ~FOF_MULTIDESTFILES;
	      pToFile = SHFileStrCpyCatW(pTempTo, NULL, wBackslash);
	      do
	      {
	        lpFileName = wfd.cAlternateFileName;
	        if (!lpFileName[0])
	          lpFileName = wfd.cFileName;
	        if (IsDotDir(lpFileName) ||
	            (IsAttribDir(wfd.dwFileAttributes) && (nFileOp.fFlags & FOF_FILESONLY)))
	          continue; /* next name in pTempFrom(dir) */
	        SHFileStrCpyCatW(&pFromFile[1], lpFileName, NULL);
	        retCode = SHFileOperationW (&nFileOp);
	      } while(!retCode && FindNextFileW(hFind, &wfd));
	      FindClose(hFind);
	      continue;
	    }
	    /* W98 returns 0x75, W2K 0x4c7 */
	    retCode = retCodeIsInvalid;
	    break;

	  } /* end-while */

shfileop_exit:
	if (pTempFrom)
	  HeapFree(GetProcessHeap(), 0, pTempFrom);
	TRACE("%s level=%ld AnyOpsAborted=%s ret=0x%lx, with %s %s%s\n",
	      cFO_Name, level, nFileOp.fAnyOperationsAborted ? "TRUE":"FALSE",
	      retCode, debugstr_w(pFrom), pTo ? "-> ":"", debugstr_w(pTo));

	lpFileOp->hNameMappings         = nFileOp.hNameMappings;
	lpFileOp->fAnyOperationsAborted = nFileOp.fAnyOperationsAborted;
	return retCode;
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

