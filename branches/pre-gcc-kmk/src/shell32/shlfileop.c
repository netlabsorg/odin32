/*
 * SHFileOperation
 *
 * Copyright 2000 Juergen Schmied
 * Copyright 2002 Andriy Palamarchuk
 * Copyright 2002-2003 Dietrich Teickner (from Odin)
 * Copyright 2002-2003 Rolf Kalbermatter
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
 * #ifdef __WIN32OS2__ .. lines
 */

#include "config.h"
#include "wine/port.h"

#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "windef.h"
#include "winbase.h"
#include "winreg.h"
#include "shellapi.h"
#include "wingdi.h"
#include "winuser.h"
#include "shlobj.h"
#include "shresdef.h"
#define NO_SHLWAPI_STREAM
#include "shlwapi.h"
#include "shell32_main.h"
#include "undocshell.h"
#include "wine/unicode.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(shell);

#define IsAttribFile(x) (!((x) & FILE_ATTRIBUTE_DIRECTORY))
#define IsAttrib(x,y)   ((INVALID_FILE_ATTRIBUTES != (x)) && ((x) & (y)))
#define IsAttribDir(x)  IsAttrib(x,FILE_ATTRIBUTE_DIRECTORY)

#define IsDotDir(x)     ((x[0] == '.') && ((x[1] == 0) || ((x[1] == '.') && (x[2] == 0))))

#define FO_MASK         0xFF
#define FO_LevelShift   8
#define HIGH_ADR (LPWSTR)0xffffffff

#define FOI_NeverOverwrite 2

CHAR aWildcardFile[] = {'*',0};
WCHAR wWildcardChars[] = {'?','*',0};
#define wWildcardFile &wWildcardChars[1]
WCHAR wBackslash[] = {'\\',0};
enum { none = 0, w95, w95b, nt351, nt40, w98, w98se, wMe, w2k, wXp};
static int WOsVers = none; /* for versionsdepended conditions */
static int retCodeIsInvalid = 0x75; /* w95?,w98se, nt <= 4? */
static LPCSTR cFO_String [] = {"FO_????","FO_MOVE","FO_COPY","FO_DELETE","FO_RENAME"};

static DWORD SHNotifyCreateDirectoryA(LPCSTR path, LPSECURITY_ATTRIBUTES sec);
static DWORD SHNotifyCreateDirectoryW(LPCWSTR path, LPSECURITY_ATTRIBUTES sec);
static DWORD SHNotifyRemoveDirectoryA(LPCSTR path);
static DWORD SHNotifyRemoveDirectoryW(LPCWSTR path);
static DWORD SHNotifyDeleteFileA(LPCSTR path);
static DWORD SHNotifyDeleteFileW(LPCWSTR path);
static DWORD SHNotifyMoveFileW(LPCWSTR src, LPCWSTR dest);
static DWORD SHNotifyCopyFileW(LPCWSTR, LPCWSTR, BOOL);

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
	      ret = (SHNotifyDeleteFileA(szTemp) == ERROR_SUCCESS);
	  } while (ret && FindNextFileA(hFind, &wfd));
	}
	FindClose(hFind);
	if (ret)
	  ret = (SHNotifyRemoveDirectoryA(pszDir) == ERROR_SUCCESS);
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
	      ret = (SHNotifyDeleteFileW(szTemp) == ERROR_SUCCESS);
	  } while (ret && FindNextFileW(hFind, &wfd));
	}
	FindClose(hFind);
	if (ret)
	  ret = (SHNotifyRemoveDirectoryW(pszDir) == ERROR_SUCCESS);
	return ret;
}

/**************************************************************************
 *  SHELL_DeleteFileA()      [internal]
 */
BOOL SHELL_DeleteFileA(LPCSTR pszFile, BOOL bShowUI)
{
	if (bShowUI && !SHELL_ConfirmDialog(ASK_DELETE_FILE, pszFile))
	  return FALSE;

	return (SHNotifyDeleteFileA(pszFile) == ERROR_SUCCESS);
}

BOOL SHELL_DeleteFileW(LPCWSTR pszFile, BOOL bShowUI)
{
	if (bShowUI && !SHELL_ConfirmDialogW(ASK_DELETE_FILE, pszFile))
	  return FALSE;

	return (SHNotifyDeleteFileW(pszFile) == ERROR_SUCCESS);
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
static DWORD SHNotifyCreateDirectoryA(LPCSTR path, LPSECURITY_ATTRIBUTES sec)
{
	WCHAR wPath[MAX_PATH];
	TRACE("(%s, %p)\n", debugstr_a(path), sec);

	MultiByteToWideChar(CP_ACP, 0, path, -1, wPath, MAX_PATH);
	return SHNotifyCreateDirectoryW(wPath, sec);
}

static DWORD SHNotifyCreateDirectoryW(LPCWSTR path, LPSECURITY_ATTRIBUTES sec)
{
	TRACE("(%s, %p)\n", debugstr_w(path), sec);

	if (StrPBrkW(path, wWildcardChars))
	{
	/* FIXME: This test is necessary since our CreateDirectory implementation
	   does create directories with wildcard chars without objection. Once this
	   is fixed, this can go away. */
	  SetLastError(ERROR_INVALID_NAME);
	  if (w98se >= WOsVers) /* wMe ? */
	    return ERROR_FILE_NOT_FOUND; /* w98se */
	  return ERROR_INVALID_NAME; /* w2k */
	}

	if (CreateDirectoryW(path, sec))
	{
	  SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, path, NULL);
	  return ERROR_SUCCESS;
	}
	return GetLastError();
}

BOOL WINAPI Win32CreateDirectoryAW(LPCVOID path, LPSECURITY_ATTRIBUTES sec)
{
	if (SHELL_OsIsUnicode())
	  return (SHNotifyCreateDirectoryW(path, sec) == ERROR_SUCCESS);
	return (SHNotifyCreateDirectoryA(path, sec) == ERROR_SUCCESS);
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
 *  ERROR_SUCCESS if successful
 *
 * NOTES:
 *  Verified on Win98 / IE 5 (SHELL32 4.72, March 1999 build) to be ANSI.
 *  This is Unicode on NT/2000
 */

static DWORD SHNotifyRemoveDirectoryA(LPCSTR path)
{
	WCHAR wPath[MAX_PATH];
	TRACE("(%s)\n", debugstr_a(path));

	MultiByteToWideChar(CP_ACP, 0, path, -1, wPath, MAX_PATH);
	return SHNotifyRemoveDirectoryW(wPath);
}

/***********************************************************************/

static DWORD SHNotifyRemoveDirectoryW(LPCWSTR path)
{
	BOOL ret;
	TRACE("(%s)\n", debugstr_w(path));

	ret = RemoveDirectoryW(path);
	if (!ret)
	{
	  /* Directory may be write protected */
	  DWORD dwAttr = GetFileAttributesW(path);
	  if (IsAttrib(dwAttr,FILE_ATTRIBUTE_READONLY))
	    if (SetFileAttributesW(path, dwAttr & ~FILE_ATTRIBUTE_READONLY))
	      ret = RemoveDirectoryW(path);
	}
	if (ret)
	{
	  SHChangeNotify(SHCNE_RMDIR, SHCNF_PATHW, path, NULL);
	  return ERROR_SUCCESS;
	}
	return GetLastError();
}

/***********************************************************************/

BOOL WINAPI Win32RemoveDirectoryAW(LPCVOID path)
{
	if (SHELL_OsIsUnicode())
	  return (SHNotifyRemoveDirectoryW(path) == ERROR_SUCCESS);
	return (SHNotifyRemoveDirectoryA(path) == ERROR_SUCCESS);
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

static DWORD SHNotifyDeleteFileA(LPCSTR path)
{
	WCHAR wPath[MAX_PATH];
	TRACE("(%s)\n", debugstr_a(path));

	MultiByteToWideChar(CP_ACP, 0, path, -1, wPath, MAX_PATH);
	return SHNotifyDeleteFileW(wPath);
}

/***********************************************************************/

static DWORD SHNotifyDeleteFileW(LPCWSTR path)
{
	BOOL ret;

	TRACE("(%s)\n", debugstr_w(path));

	ret = DeleteFileW(path);
	if (!ret)
	{
	  /* File may be write protected or a system file */
	  DWORD dwAttr = GetFileAttributesW(path);
	  if (IsAttrib(dwAttr,FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM))
	    if (SetFileAttributesW(path, dwAttr & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
	      ret = DeleteFileW(path);
	}
	if (ret)
	{
	  SHChangeNotify(SHCNE_DELETE, SHCNF_PATHW, path, NULL);
	  return ERROR_SUCCESS;
	}
	return GetLastError();
}

/***********************************************************************/

DWORD WINAPI Win32DeleteFileAW(LPCVOID path)
{
	if (SHELL_OsIsUnicode())
	  return (SHNotifyDeleteFileW(path) == ERROR_SUCCESS);
	return (SHNotifyDeleteFileA(path) == ERROR_SUCCESS);
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
 *  NO_ERROR if successful, or an error code otherwise
 */
static DWORD SHNotifyMoveFileW(LPCWSTR src, LPCWSTR dest)
{
	BOOL ret = FALSE;

	TRACE("(%s %s)\n", debugstr_w(src), debugstr_w(dest));

	if (StrPBrkW(dest, wWildcardChars))
	{
	  /* FIXME: This test is currently necessary since our MoveFile
	     implementation does create files with wildcard characters
	     without objection!! Once this is fixed, this here can go away. */
	  SetLastError(ERROR_INVALID_NAME);
	  if (w98se >= WOsVers) /* wMe ? */
	    return ERROR_FILE_NOT_FOUND; /* w98se */
	  return ERROR_INVALID_NAME; /* w2k,wXp */
	}

	ret = MoveFileW(src, dest);
	if (!ret)
	{
	  /* Source file may be write protected or a system file */
	  DWORD dwAttr = GetFileAttributesW(src);
	  if (IsAttrib(dwAttr,FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM))
	    if (SetFileAttributesW(src, dwAttr & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)))
	      ret = MoveFileW(src, dest);
	}
	if (ret)
	{
	  SHChangeNotify(SHCNE_RENAMEITEM, SHCNF_PATHW, src, dest);
	  return ERROR_SUCCESS;
	}
	return GetLastError();
}

/************************************************************************
 * SHNotifyCopyFile          [internal]
 *
 * Copies a file. Also triggers a change notify if one exists.
 *
 * PARAMS
 *  src        [I]   path to source file to move
 *  dest       [I]   path to target file to move to
 *  b_nOverWrt [I]   if TRUE, the target file will not be overwritten, if a
 *                   file with this name already exists
 *
 * RETURNS
 *  NO_ERROR if successful, or an error code otherwise
 */
static DWORD SHNotifyCopyFileW(LPCWSTR src, LPCWSTR dest, BOOL b_nOverWrt)
{
	BOOL ret;

	TRACE("(%s %s %s)\n", debugstr_w(src), debugstr_w(dest), b_nOverWrt ? "OverWriteIfExists" : "");

	ret = CopyFileW(src, dest, b_nOverWrt);
	if (ret)
	{
	  SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW, dest, NULL);
	  return ERROR_SUCCESS;
	}
	return GetLastError();
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
 *  ERROR_INVLID_NAME if the path contains invalid chars
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
 *  ERROR_BAD_PATHNAME or ERROR_PATH_NOT_FOUND if the path is relative
 *  ERROR_INVLID_NAME if the path contains invalid chars
 * ? ERROR_FILE_EXISTS when a file with that name exists
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
 * SHCreateDirectoryExW      [SHELL32.@]
 */
DWORD WINAPI SHCreateDirectoryExW(HWND hWnd, LPCWSTR path, LPSECURITY_ATTRIBUTES sec)
{
	DWORD ret = ERROR_BAD_PATHNAME;
	TRACE("(%p, %s, %p)\n",hWnd, debugstr_w(path), sec);

	if (PathIsRelativeW(path))
	{
	  SetLastError(ret);
	}
	else
	{
	  ret = SHNotifyCreateDirectoryW(path, sec);
	  if (ret && ret != ERROR_FILE_EXISTS &&
	      ret != ERROR_ALREADY_EXISTS &&
	      ret != ERROR_FILENAME_EXCED_RANGE)
	  {
	  /* handling network file names?
	    lstrcpynW(pathName, path, MAX_PATH);
	    lpStr = PathAddBackslashW(pathName);*/
	    FIXME("Semi-stub, creating path %s, failed with error %ld?\n", debugstr_w(path), ret);
	  }
	}
	return ret;
}

/*************************************************************************
 * SHFreeNameMappings      [shell32.246]
 *
 * Free the mapping handle returned by SHFileoperation if FOF_WANTSMAPPINGHANDLE
 * was specified.
 *
 * PARAMS
 *  hNameMapping [I] handle to the name mappings used during renaming of files
 *
 */
void WINAPI SHFreeNameMappings(HANDLE hNameMapping)
{
	if (hNameMapping)
	{
	  LPSHNAMEMAPPINGW lp;
	  UINT i = 0;

	  while (NULL != (lp = DSA_GetItemPtr((struct _DSA* const)hNameMapping, i++)))
	  {
	    SHFree(lp->pszOldPath);
	    SHFree(lp->pszNewPath);
	  }
	  DSA_Destroy((struct _DSA*)hNameMapping);
	}
}

static BOOL SetIfPointer(LPWSTR pToSlash, WCHAR x)
{
	if (pToSlash) *pToSlash = x;
	return (BOOL)pToSlash; 
}
/*************************************************************************
 * SHFindAttrW      [internal]
 *
 * Get the Attributes for a file or directory. The difference to GetAttributes()
 * is that this function will also work for paths containing wildcard characters
 * in its filename.

 * PARAMS
 *  path       [I]   path of directory or file to check
 *  fileOnly   [I]   TRUE if only files should be found
 *
 * RETURNS
 *  INVALID_FILE_ATTRIBUTES if the path does not exist, the actual attributes of
 *  the first file or directory found otherwise
 */
static DWORD SHFindAttrW(LPCWSTR pName, BOOL fileOnly)
{
	WIN32_FIND_DATAW wfd;
	BOOL b_FileMask = fileOnly && (NULL != StrPBrkW(pName, wWildcardChars));
	DWORD dwAttr = INVALID_FILE_ATTRIBUTES;
	HANDLE hFind = FindFirstFileW(pName, &wfd);

	TRACE("%s %d\n", debugstr_w(pName), fileOnly);
	if (INVALID_HANDLE_VALUE != hFind)
	{
	  do
	  {
	    if (b_FileMask && IsAttribDir(wfd.dwFileAttributes))
	       continue;
	    dwAttr = wfd.dwFileAttributes;
	    break;
	  }
	  while (FindNextFileW(hFind, &wfd));
	  FindClose(hFind);
	}
	return dwAttr;
}

/*************************************************************************
 *
 * SHFileStrICmp HelperFunction for SHFileOperationW
 *
 */
static BOOL SHFileStrICmpW(LPWSTR p1, LPWSTR p2, LPWSTR p1End, LPWSTR p2End)
{
	WCHAR C1 = '\0';
	WCHAR C2 = '\0';
	int i_Temp = 0 - (toupperW(p1[0]) == toupperW(p2[0]));
	int i_len1 = lstrlenW(p1);
	int i_len2 = lstrlenW(p2);

	if (!i_Temp) return FALSE; /* driveletters are different */
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
static LPWSTR SHFileStrCpyCatW(LPWSTR pTo, LPCWSTR pFrom, LPCWSTR pCatStr)
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
	    if ((i_len) && ('\\' != pTo[--i_len]))
	      i_len++;
	    pTo[i_len] = '\\';
	    if ('\\' == pCatStr[0])
	      pCatStr++; \
	    lstrcpyW(&pTo[i_len+1], pCatStr);
	  }
	  pToFile = StrRChrW(pTo,NULL,'\\');
	  /* termination of the new string-group */
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
static BOOL SHELL_FileNamesMatch(LPCWSTR pszFiles1, LPCWSTR pszFiles2, BOOL bOnlySrc)
{
	LPWSTR pszTemp;

	TRACE("%s %s %d\n", debugstr_w(pszFiles1), debugstr_w(pszFiles2), bOnlySrc);

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
	    WCHAR szMask [MAX_PATH];
	    pszTemp = StrRChrW(pszFiles1, pszTemp, '\\');
	    if (!pszTemp)
	      return FALSE;
	    lstrcpynW(szMask, pszFiles1, (pszTemp - pszFiles1) + 1);
	    /* we will check the root of the mask as valid dir */
	    if (!IsAttribDir(GetFileAttributesW(&szMask[0])))
	      return FALSE;
	  }
	  else
	  {
	    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(pszFiles1))
	      return FALSE;
	  }
	  pszFiles1 += lstrlenW(pszFiles1) + 1;
	  if (!bOnlySrc)
	    pszFiles2 += lstrlenW(pszFiles2) + 1;
	}
	return ((pszFiles1[0] == '\0') && (bOnlySrc || (pszFiles2[0] == '\0')));
}

/*************************************************************************
 * SHFileOperationCheck
 */
static DWORD SHFileOperationCheck(LPSHFILEOPSTRUCTW lpFileOp, LPCSTR* cFO_Name)
{
	FILEOP_FLAGS OFl = (FILEOP_FLAGS)lpFileOp->fFlags;
	long retCode = NO_ERROR;
	long FuncSwitch = (lpFileOp->wFunc & FO_MASK);

	/*  default no error */
	if (none == WOsVers) {
	  OSVERSIONINFOA info;
	  info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	  GetVersionExA(&info);
	  WOsVers = wMe; /* <= wMe, tested with w98se and partialy nt4,w95 */
	  /* this cases must be more different */
	  if ((VER_PLATFORM_WIN32_WINDOWS == info.dwPlatformId) &&
	      (4 == info.dwMajorVersion) && (10 == info.dwMinorVersion)) {
	    WOsVers = w98se;
	    retCodeIsInvalid = 0x75; /* <= W98se, what is with wMe? */
	  }
	  if (VER_PLATFORM_WIN32_NT == info.dwPlatformId)
	  {
	    if (4 > info.dwMajorVersion)
	    {
	      WOsVers = nt351;
	      retCodeIsInvalid =  0x75;
	    }
	    if (4 == info.dwMajorVersion)
	    {
	      WOsVers = nt40;
	      retCodeIsInvalid =  0x75;
	    }
	    if (5 == info.dwMajorVersion)
	    {
	      WOsVers = w2k;
	      if (0 < info.dwMinorVersion)
	        WOsVers = wXp; /* Longhorn also ? */
	      retCodeIsInvalid = 0x4c7; /* >= W2K */
	    }
	    if (5 < info.dwMajorVersion)
	    {
	      WOsVers = wXp; /* Longhorn ? */
	      retCodeIsInvalid = 0x4c7; /* >= W2K */
	    }
	  }
	  TRACE("WOsVers:%d Id: %ld Ver: %ld.%ld.%ld, %s\n\n", WOsVers, info.dwPlatformId,
	         info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber, info.szCSDVersion);
	}
	if ((FuncSwitch < FO_MOVE) || (FuncSwitch > FO_RENAME))
	{
	  FuncSwitch = 0;
	  retCode = ERROR_INVALID_PARAMETER;
	}
	cFO_Name[0] = cFO_String [FuncSwitch];

	if (!(~FO_MASK & lpFileOp->wFunc)) /* ? Level == 0 */
	{
	  lpFileOp->hNameMappings = 0;
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
	  OFl &= (~(FOF_FILESONLY | FOF_WANTMAPPINGHANDLE | FOF_NOCONFIRMATION |
	            FOF_RENAMEONCOLLISION | FOF_MULTIDESTFILES));  /* implemented */
	  OFl ^= (FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_NOCOPYSECURITYATTRIBS); /* ignored, if one */
	  OFl &= (~FOF_SIMPLEPROGRESS);                      /* ignored, only with FOF_SILENT */
	  if (OFl)
	  {
	    if (OFl & (~(FOF_CONFIRMMOUSE | FOF_SILENT |
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
 * SHCreateMappingElement internal HelperFunction for SHRenameOnCollision
 *
 * Old/New MappingNameElement
 */
static LPWSTR SHCreateMappingElement(PINT size, LPWSTR pName)
{
	LPWSTR pMapName = NULL;
	CHAR TempPath [MAX_PATH];
	int isize = lstrlenW(pName)+1;
	int oldsize = isize;
	if ((w98se >= WOsVers) && (nt40 != WOsVers) && (nt351 != WOsVers))
	{
	  isize = WideCharToMultiByte( CP_ACP, 0, pName, oldsize, TempPath, MAX_PATH, NULL, NULL );
	  if (0 < isize)
	  	pName = (LPWSTR)&TempPath[0];
	}
	else
	{
	  oldsize = oldsize*sizeof(WCHAR);
	}
	pMapName = SHAlloc(oldsize);
	memcpy(pMapName,pName,oldsize);
	*size = isize-1;
	return pMapName;
}

/*************************************************************************
 *
 * SHRenameOnCollision internal HelperFunction for SHFileOperationW
 *
 * Creates new Names and Checks for existance.
 *
 * w98 has problems with some remames on collision, if the original target-name,
 * or the root of the target-name is equal any former orginal target-name,
 * and if the root-dir of the target differs from the partiell root-dir of the source.
 * If we have different target-names or has all target the same root of hear source,
 * we have the problem not, root of target can be shorter as the full root of source. 
 * I think, that is a mapping-problem.
 * Move within the same or shorter root can made from fo_rename,
 * that works every in w98, only move/copy in other roots has this problem,
 * this must be done in fo_move/fo_copy, there we have the problem.
 * we ignore this problem, it is solved in w2k. 
 */
static WCHAR wStrFormat[] = {' ','(','%','d',')',' ',0};

static DWORD SHRenameOnCollision(LPWSTR pTempTo, LPWSTR pToFile, LPCWSTR pFromFile, LPSHFILEOPSTRUCTW lpFileOp)
{
/* todo ERROR_FILENAME_EXCED_RANGE ?? */
	static WCHAR wCopy_x_of[40] = {0};
	LPWSTR pszTemp = wStrFormat + 5;
	DWORD ToAttr;
	WCHAR szNumber[16];
	WCHAR szOldToName [MAX_PATH];
	int number = 0;

	while (ToAttr = SHFindAttrW(pTempTo, FALSE),
	       (INVALID_FILE_ATTRIBUTES != ToAttr) && (FOF_RENAMEONCOLLISION & lpFileOp->fFlags))
	{
	  if (!number)
	  {
	    if (!*wCopy_x_of && !LoadStringW(shell32_hInstance, IDS_COPY_X_OF_TEXT, wCopy_x_of, sizeof(wCopy_x_of)-1))
	      break; /* should never be */
	    lstrcpyW(szOldToName, pTempTo);
	  }
	  number++;
	  if (1 < number)
	  {
	    pszTemp = szNumber;
	    wsprintfW(szNumber, wStrFormat, number);
	  }
	  wsprintfW(pToFile + 1,wCopy_x_of, pszTemp, pFromFile + 1);
	  pToFile[lstrlenW(pToFile)+1] = 0;

	} /* endwhile */
	if (number)
	{
	  SHNAMEMAPPINGW shm;
	  shm.pszOldPath = SHCreateMappingElement(&shm.cchOldPath, szOldToName);
	  shm.pszNewPath = SHCreateMappingElement(&shm.cchNewPath, pTempTo);
	  if (!lpFileOp->hNameMappings)
	    lpFileOp->hNameMappings = DSA_Create(sizeof(SHNAMEMAPPINGW),1);
	  DSA_InsertItem (lpFileOp->hNameMappings, ((HDSA)lpFileOp->hNameMappings)->nItemCount, &shm);
	}
	return ToAttr;
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
static DWORD SHFileOperationMove(LPSHFILEOPSTRUCTW lpFileOp)
{
	WCHAR szToName [MAX_PATH + 4];
	LPCWSTR pTo = lpFileOp->pTo;
	int i_lenTo;
	if (NULL != StrPBrkW(pTo, wWildcardChars))
	{
	  if (w98se >= WOsVers) /* wMe? */
	    return ERROR_FILE_NOT_FOUND; /* w98se */
	  return ERROR_INVALID_NAME; /* w2k */
	}

	/* FOF_NOCONFIRMATION, FOF_RENAMEONCOLLISION ? */
	i_lenTo = lstrlenW(pTo);
	if (i_lenTo && ('\\' == pTo[--i_lenTo]))
	{
	  lstrcpynW(szToName, pTo, i_lenTo + 1);
	  pTo = &szToName[0];
	}
	if (INVALID_FILE_ATTRIBUTES != SHFindAttrW(pTo, FALSE))
	{
	  return ERROR_ALREADY_EXISTS;
	}
	/* we use SHNotifyMoveFile() instead of MoveFileW */
	return SHNotifyMoveFileW(lpFileOp->pFrom, pTo);
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
	
	long FuncSwitch = (nFileOp.wFunc & FO_MASK);
	long level= nFileOp.wFunc>>FO_LevelShift;

	BOOL b_Multi = (0 != (nFileOp.fFlags & FOF_MULTIDESTFILES));

	BOOL b_MultiTo = (FO_DELETE != FuncSwitch);
	BOOL b_MultiPaired = (0 < level) /* FALSE only at Level 0 */;
	BOOL b_MultiFrom = FALSE;
	BOOL b_RenamOnColl = (0 != (nFileOp.fFlags & FOF_RENAMEONCOLLISION));
	BOOL b_NoConfirm = (0 != (nFileOp.fFlags & FOF_NOCONFIRMATION));
	BOOL b_ask_overwrite = (!b_NoConfirm && !b_RenamOnColl);
	BOOL b_not_overwrite = (!b_NoConfirm ||  b_RenamOnColl);
	BOOL b_DELETE = (FO_DELETE == FuncSwitch);
	BOOL b_SameRoot;
	BOOL b_SameRoot_MOVE;
	BOOL b_ToMask = FALSE;
	BOOL b_FromMask;
	BOOL b_FileMask;

	LPCSTR cFO_Name;
	long retCode = SHFileOperationCheck(&nFileOp,&cFO_Name);

	if (retCode)
	{
	  if (('?' == cFO_Name[3]) && (/* wMe ?*/ w98se >= WOsVers))
	    retCode = NO_ERROR; /* nt4.0, w95, w98se returns no error, w2k, wXp not */
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
	    nFileOp.pFrom = pTempFrom = SHAlloc(((1 + 2 * (b_MultiTo)) * MAX_PATH + 6) * sizeof(WCHAR));
	    if (!pTempFrom)
	    {
	      retCode = ERROR_OUTOFMEMORY;
	      SetLastError(retCode);
	      goto shfileop_exit;
	    }
	    if (b_MultiTo)
	      pTempTo = &pTempFrom[MAX_PATH + 4];
	    nFileOp.pTo = pTempTo;
	    if (0 == level)
	    {
	      if ((0 != (nFileOp.fFlags & FOF_NOCONFIRMATION)) && b_RenamOnColl)
	        nFileOp.fAnyOperationsAborted |= FOI_NeverOverwrite;
	    }
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
	      b_MultiTo = (b_Multi && (0 != pNextTo[0]));
	    }

	    pFrom = pNextFrom;
	    pNextFrom = &pNextFrom[lstrlenW(pNextFrom) + 1];
	    if (!b_MultiFrom && !b_MultiTo)
	      b_MultiFrom = (0 != pNextFrom[0]);

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
	        if (/* wMe? */ w2k <= WOsVers)
	          retCode = ERROR_GEN_FAILURE;  /* W2K ERROR_GEN_FAILURE, w95,W98,NT40 returns no error */
	        goto shfileop_exit;
	      }
	    }

	    if (!b_MultiPaired)
	    {
	      b_MultiPaired =
	        SHELL_FileNamesMatch(lpFileOp->pFrom,
	              lpFileOp->pTo, (b_DELETE || !b_Multi || b_MultiFrom));
	    } /* endif */
	    if (!(b_MultiPaired) || !(pFromFile) || !(pFromFile[1]) || ((pTo) && !(pToFile)))
	    {
	      if (nt40 < WOsVers)
	        retCode = 0x402;      /* 1026, nt40 returns 0 */
	      goto shfileop_exit;
	    }

	    b_FileMask = b_FromMask && (FOF_FILESONLY & nFileOp.fFlags);
	    if (!b_DELETE)
	    {
	      b_SameRoot = (toupperW(pTempFrom[0]) == toupperW(pTempTo[0]));
	      b_SameRoot_MOVE = (b_SameRoot && (FO_MOVE == FuncSwitch));
	    }
	    FromAttr = SHFindAttrW(pTempFrom, b_FileMask);
	    if (INVALID_FILE_ATTRIBUTES == FromAttr) 
	    {
	      retCode = GetLastError();
	      if (ERROR_FILE_NOT_FOUND == retCode || ERROR_NO_MORE_FILES == retCode) /* only tested in wXp,w2k,w98 */
	      {
	        if (b_FromMask)
	        {
	          retCode = NO_ERROR;
	        }
	        else
	          if (!b_SameRoot_MOVE && !b_DELETE)
	            goto shfileop_IsInvalid;
	      }
	      continue;
	    } /* endif */

	    if (b_DELETE)
	    {
	      hFind = FindFirstFileW(pFrom, &wfd);
	      if (INVALID_HANDLE_VALUE == hFind)
	      {
	        goto shfileop_IsInvalid;
	      }
	      do
	      {
	        lpFileName = wfd.cAlternateFileName;
	        if (!lpFileName[0])
	           lpFileName = wfd.cFileName;
	        if (IsDotDir(lpFileName) ||
	            (b_FileMask && IsAttribDir(wfd.dwFileAttributes)))
	          continue;
	        SHFileStrCpyCatW(&pFromFile[1], lpFileName, NULL);
	        /* TODO: Check the SHELL_DeleteFileOrDirectoryW() function in shell32.dll */
	        if (IsAttribFile(wfd.dwFileAttributes))
	        {
	          if (SHNotifyDeleteFileW(pTempFrom))
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
	      lpFileName = StrRChrW(pTempTo, NULL, '\\');
	      if (lpFileName)
	      {
	        pToTailSlash = pToFile; 
	        pToFile = lpFileName;
	      }
	    }
	    ToPathAttr = ToAttr = GetFileAttributesW(pTempTo);
	    if (!IsAttribDir(ToAttr) && SetIfPointer(pToFile, '\0'))
	    {
	      ToPathAttr = GetFileAttributesW(pTempTo);
	      *pToFile = '\\';
	    }
	    SetIfPointer(pToTailSlash,'\\');

	    /* FO_RENAME is not only a Filter for FO_MOVE */
	    if (FO_RENAME == FuncSwitch)
	    {
	      if (!(b_FromMask) && (b_SameRoot) && SHFileStrICmpW(pTempFrom, pTempTo, NULL, NULL))
	      {
	        if (!b_RenamOnColl) /* neu */
	      /* target is the same as source ? W98 has 0x71, W2K also */
	          retCode = 0x71;
	        goto shfileop_exit;
	      } /* endif */
	      if ((nt40 == WOsVers) && (b_FromMask))
	      {
	        retCode = 0x72;  /* only found in nt40, not w98se or w2k */
	        goto shfileop_exit;
	      } /* endif */
	      if ((b_FromMask && !b_ToMask) || !b_SameRoot || \
	        !SHFileStrICmpW(pTempFrom, pTempTo, pFromFile, NULL) || \
	         SHFileStrICmpW(pTempFrom, pTempTo, pFromFile, HIGH_ADR))
	      {
	        retCode = 0x73; /* must be here or before, not later */
	        goto shfileop_exit;
	      }
	      if (!(b_ToMask) && !(pToTailSlash) && IsAttribDir(ToAttr))
	      {
	        if (IsAttribDir(FromAttr))
	          ToAttr = SHRenameOnCollision(pTempTo, pToFile, pFromFile, &nFileOp);
	        if ((w98se >= WOsVers) && IsAttribDir(ToAttr /* can be changed from SHRenameOnCollision */))
	        {
	          /* never in W2K */
	          retCode = ERROR_INVALID_NAME; /* FOF_NOCONFIRMATION ? */
	          goto shfileop_exit;
	        }
	      } /* endif */
	      if ((INVALID_FILE_ATTRIBUTES != ToPathAttr) || (b_ToMask && (w98se >= WOsVers)))
	      /* w2k only (INVALID_FILE_ATTRIBUTES != ToPathAttr) */
	      {
	        retCode = SHFileOperationMove(&nFileOp);
	      }
	      else
	        /* W98 returns 0x75, W2K 0x4c7 */
	        retCode = retCodeIsInvalid;
	      goto shfileop_exit;
	    } /* FO_RENAME ends, only FO_MOVE or FO_COPY valid from here */

	    if (b_FromMask)
	    {
	      /* FO_COPY/FO_MOVE with mask, FO_DELETE are solved long before */
	      hFind = FindFirstFileW(pFrom, &wfd);
	      if (INVALID_HANDLE_VALUE == hFind)
	      {
	        /* the retcode for this case is unknown */
	        goto shfileop_IsInvalid;
	      }
	      SetIfPointer(pToTailSlash, '\0');
	      ToAttr = SHFindAttrW(pTempTo, FALSE);
	      if (b_ToMask && !b_Multi)
	        nFileOp.fFlags &= ~FOF_RENAMEONCOLLISION;
	      nFileOp.fFlags &= ~FOF_MULTIDESTFILES;
	      pToFile = SHFileStrCpyCatW(pTempTo, NULL, wBackslash);
	      do
	      {
	        lpFileName = wfd.cAlternateFileName;
	        if (!lpFileName[0])
	          lpFileName = wfd.cFileName;
	        if (IsDotDir(lpFileName) ||
	            (b_FileMask && IsAttribDir(wfd.dwFileAttributes)))
	          continue; /* next name in pTempFrom(dir) */
	        if (INVALID_FILE_ATTRIBUTES == ToAttr)
	        {
	          if (b_MultiTo)
	          {
	            retCode = retCodeIsInvalid;
	            break; /* we need the FindClose */
	          }
	          if (b_ToMask)
	          {
	            if ((w2k <= WOsVers) && IsAttribDir(wfd.dwFileAttributes)) /* w2k,wXp is tested */
	            {
	              if (w2k == WOsVers)
	                retCode = ERROR_ALREADY_EXISTS; /* no root for target exist, and w2k send this, kind of mad */
	              else
	                retCode = ERROR_INVALID_NAME; /* wXp */
	              nFileOp.fAnyOperationsAborted |= TRUE;
	              break;
	            }
	          }
	          else
	          {
	            nFileOp.fFlags |= FOF_MULTIDESTFILES;
	            SHFileStrCpyCatW(&pToFile[1], lpFileName, NULL);
	          }
	        }
	        SHFileStrCpyCatW(&pFromFile[1], lpFileName, NULL);
	        retCode = SHFileOperationW (&nFileOp);
	      } while(!retCode && FindNextFileW(hFind, &wfd));
	      FindClose(hFind);
	      if (retCodeIsInvalid == retCode)
	        goto shfileop_IsInvalid;
	      continue;
	    }

	    if ((INVALID_FILE_ATTRIBUTES != (FromAttr | ToAttr)) && b_RenamOnColl && !(b_ToMask) && !(pToTailSlash))
	    {
	      if (b_SameRoot_MOVE && !(b_Multi) && IsAttribDir(FromAttr & ToAttr) && SHFileStrICmpW(pTempFrom, pTempTo, HIGH_ADR, HIGH_ADR))
	      {
	        retCode = ERROR_FILENAME_EXCED_RANGE; /* in w98/w2k recursive move, we does this not */
	        if (w98se >= WOsVers) retCode |= 0x10000; /* unexpected, seen in w98se and nt40sp6 */
	        goto shfileop_exit;
	      }
	      if ( /* i-means indifferent */ 
//	  /*bad*/   ( b_SameRoot_MOVE && !(b_Multi) && !(b_MultiFrom) && IsAttribDir(ToAttr) && !(SHFileStrICmpW(pTempFrom, pTempTo, NULL, NULL)))
//	  /*bad*/   ( b_SameRoot_MOVE && !(b_Multi) &&  (b_MultiFrom) && IsAttribDir(ToAttr) && !(SHFileStrICmpW(pTempFrom, pTempTo, NULL, NULL)))
	  /*ok*/    ( b_SameRoot_MOVE &&  (b_Multi) && !(b_MultiFrom) && IsAttribDir(ToAttr) && !(SHFileStrICmpW(pTempFrom, pTempTo, NULL, NULL)))
//	  /*bad*/|| ( b_SameRoot_MOVE &&  (b_Multi) &&  (b_MultiFrom) && IsAttribDir(ToAttr) && !(SHFileStrICmpW(pTempFrom, pTempTo, NULL, NULL)))
//
//	  /*bad*/|| (!b_SameRoot_MOVE && !(b_Multi) && !(b_MultiFrom) && IsAttribDir(ToAttr) && IsAttribDir(FromAttr))
//	  /*i*/  || (!b_SameRoot_MOVE && !(b_Multi) &&  (b_MultiFrom) && IsAttribDir(ToAttr) && IsAttribDir(FromAttr))
	  /*ok*/ || (!b_SameRoot_MOVE &&  (b_Multi) && !(b_MultiFrom) && IsAttribDir(ToAttr) && IsAttribDir(FromAttr))
//	  /*i*/  || (!b_SameRoot_MOVE &&  (b_Multi) &&  (b_MultiFrom) && IsAttribDir(ToAttr) && IsAttribDir(FromAttr))
//
	  /*ok*/ || ( b_SameRoot_MOVE && IsAttribFile(FromAttr | ToAttr) && !(SHFileStrICmpW(pTempFrom, pTempTo, NULL, NULL)))
//
	  /*ok*/ || (!b_SameRoot_MOVE && IsAttribFile(FromAttr | ToAttr))
	  /*ok*/ || ((FO_COPY == FuncSwitch) && IsAttribDir(FromAttr) && IsAttribFile(ToAttr))
	          )
	      {
	        ToAttr = SHRenameOnCollision(pTempTo, pToFile, pFromFile, &nFileOp);
	      } /* endif */
	    } /* endif */
	    if ((b_SameRoot) && !(b_MultiFrom) && (!b_RenamOnColl || b_SameRoot_MOVE)
	    && SHFileStrICmpW(pTempFrom, pTempTo, NULL, NULL))
	    {
	      if (!b_RenamOnColl && (IsAttribFile(FromAttr))) /* neu */
	        /* target is the same as source ? W98 has 0x71, W2K also */
	        retCode = 0x71;
	      goto shfileop_exit;
	    } /* endif */

	    /* Analycing for moving SourceName to as TargetName */
	    if ((b_MultiFrom || !b_Multi) && (
	        (IsAttribFile(FromAttr) && IsAttribDir(ToAttr))
	    ||  (!b_MultiTo && IsAttribDir(ToAttr))
	    ||  (!b_MultiTo && IsAttribDir(FromAttr) && b_MultiFrom && !b_SameRoot_MOVE)
	       ))
	    {
	        SHFileStrCpyCatW(pTempTo, NULL, pFromFile);
	        /* without FOF_MULTIDESTFILES shlfileop will create dir's recursive */
	        nFileOp.fFlags |= FOF_MULTIDESTFILES;
	        retCode = SHFileOperationW(&nFileOp);
	        continue;
	    }
	    /* What can we do with one pair and FO_MOVE/FO_COPY ? */
	    /* w98se, nt40 can create recursive dirs, W2K not! wMe ist not tested */
	    if (IsAttribDir(ToPathAttr) || !b_SameRoot_MOVE || (w98se >= WOsVers))
	    if (!b_MultiFrom) 
	    {
	      if (IsAttribFile(FromAttr))
	      {
	        if (b_SameRoot_MOVE &&
	          /* IsAttribDir(ToPathAttr) && !pToTailSlash && (bug) */
	          /* windows-bug, MOVE for File also with pToTailSlash, COPY not for this */
	           (!(IsAttribFile(ToAttr)) || (!(b_ask_overwrite) && b_not_overwrite)) &&
	           (IsAttribDir(ToPathAttr) || b_ToMask))
	        {
	          /* At the same drive, we can move for FO_MOVE, dir to dir is solved later */
	          retCode = SHFileOperationMove(&nFileOp);
	          if ((nt40 == WOsVers) && (ERROR_ALREADY_EXISTS == retCode))
	            retCode = 0x10005;
	          continue;
	        } /* endif */
	        if (IsAttribDir(ToPathAttr) && !pToTailSlash && !IsAttribDir(ToAttr))
	        {
	          if (!(FOI_NeverOverwrite & nFileOp.fAnyOperationsAborted))
	          {
	            if (!b_not_overwrite)
	              ToAttr = INVALID_FILE_ATTRIBUTES;
	            if (IsAttribFile(ToAttr) && b_not_overwrite)
	            {
	              if (b_ask_overwrite)
	              {
	                /* we must change the dialog in the future for return 3-4 cases,
	                 * 'Yes','No','Yes for all','Never ?' */
	                if ((INVALID_FILE_ATTRIBUTES != ToAttr) && !SHELL_ConfirmDialogW(ASK_OVERWRITE_FILE, pTempTo))
	                {
	                  retCode = 0x10008;
	                  nFileOp.fAnyOperationsAborted |= TRUE;
	                  continue;
	                } 
	                ToAttr = INVALID_FILE_ATTRIBUTES;
	              } /* endif */
	            } /* endif */
	          } /* endif */
	          if (INVALID_FILE_ATTRIBUTES == ToAttr)
	          {
	            if (SHNotifyCopyFileW(pTempFrom, pTempTo, nFileOp.fFlags & FOF_RENAMEONCOLLISION))
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
	      }
	      if (IsAttribDir(FromAttr))
	      {
	        if (INVALID_FILE_ATTRIBUTES == ToAttr)
	        {
	          SetIfPointer(pToTailSlash, '\0');
	          if (w2k <= WOsVers)  /* if w2k,wxp  not in w98se, nt40sp6 */
	          {
	            ToAttr = SHRenameOnCollision(pTempTo, pToFile, pFromFile, &nFileOp);
	            if (INVALID_FILE_ATTRIBUTES != ToAttr)
	            {
	              if (w2k == WOsVers)
	                retCode = ERROR_ALREADY_EXISTS; /* w2k */
	              else
	                retCode = ERROR_INVALID_NAME; /* wXp */
	              nFileOp.fAnyOperationsAborted |= TRUE;
	              goto shfileop_exit;
	            }
	            lpFileName = &pToFile[lstrlenW(pToFile)];
	            if (pToTailSlash)
	            {
	              pToTailSlash = lpFileName;
	              lpFileName++;
	            }
	            ((DWORD*)lpFileName)[0] = '\0';
	            retCode = SHCreateDirectoryExW(NULL,pTempTo, NULL);
	          }
	          else
	          { /* only for < w2k, tested with w98se,nt40sp6 */
	            if (IsAttribDir(ToPathAttr) || !b_SameRoot_MOVE || b_ToMask)
	            { /* tested with w98se, partially nt40sp6, w2k(wXp). only w98se,nt40sp6 can loop */
	              lpFileName = NULL;
	              while ((lpFileName = StrRChrW(pTempTo,lpFileName, '\\'),
	                     INVALID_FILE_ATTRIBUTES == ToAttr) &&
	                     (':' != lpFileName[-1])) /* not begin with root, or end with root */
	              {
	                SetIfPointer(lpFileName, '\0');
	                ToAttr = GetFileAttributesW(pTempTo); /* for continuing */
	                SetIfPointer(lpFileName--, '\\');
	              }
	              while (lpFileName && lpFileName[1] && !retCode)
	              {
				    SetIfPointer(lpFileName,'\\');
	                lpFileName = StrChrW(++lpFileName,'\\');
	                SetIfPointer(lpFileName,'\0');
	                retCode = SHCreateDirectoryExW(NULL,pTempTo, NULL);
	              }
	            }
	          }
	          if (retCode)
	          {
	            retCode = (ERROR_PATH_NOT_FOUND | 0x10000); /* nt40,w98se,w2k,wxp */
	            goto shfileop_exit;
	          }
	          ToAttr = GetFileAttributesW(pTempTo);
	          SetIfPointer(pToTailSlash,'\\');
	          nFileOp.fFlags &= ~FOF_RENAMEONCOLLISION;
	        }
	        if (IsAttribDir(ToAttr))
	        {
	          /* both are existing dirs, we (FO_)MOVE/COPY the content */
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

shfileop_IsInvalid:
	    if (!(b_SameRoot_MOVE) && (nt40 < WOsVers))
	    {
	      nFileOp.fAnyOperationsAborted |= TRUE;
	    } /* endif */
	    /* NT4.0,W98 returns 0x75, W2K,WXP 0x4c7 */
	    retCode = retCodeIsInvalid;
	    break;

	  } /* end-while */

shfileop_exit:
	if (pTempFrom)
	  SHFree(pTempFrom);
	TRACE("%s level=%ld AnyOpsAborted=%s ret=0x%lx, with %s %s%s\n",
	      cFO_Name, level, (TRUE & nFileOp.fAnyOperationsAborted) ? "TRUE":"FALSE",
	      retCode, debugstr_w(pFrom), pTo ? "-> ":"", debugstr_w(pTo));

	if (0 == level)
	{
	  nFileOp.fAnyOperationsAborted &= TRUE;
	  if (!(nFileOp.fFlags & FOF_WANTMAPPINGHANDLE))
	  {
	    SHFreeNameMappings((HANDLE)nFileOp.hNameMappings);
	    nFileOp.hNameMappings = 0;
	  }
	} 
	lpFileOp->hNameMappings         = nFileOp.hNameMappings;
	lpFileOp->fAnyOperationsAborted = nFileOp.fAnyOperationsAborted;
	return retCode;
}

/*************************************************************************
 *
 * SHNameTranslate HelperFunction for SHFileOperationA
 *
 * Translates a list of 0 terminated ASCII strings into Unicode. If *wString
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

	TRACE("\n");
	for (;;) /* every loop calculate size, second translate also, if we have storage for this */
	{
	  size = SHNameTranslate(&wString, &nFileOp.pFrom, TRUE); /* internal loop */
	  if (FO_DELETE != (nFileOp.wFunc & FO_MASK))
	    size += SHNameTranslate(&wString, &nFileOp.pTo, TRUE); /* internal loop */
	  if ((nFileOp.fFlags & FOF_SIMPLEPROGRESS))
	    size += SHNameTranslate(&wString, &nFileOp.lpszProgressTitle, FALSE); /* no loop */

	  if (ForFree)
	  {
	    retCode = SHFileOperationW(&nFileOp);
	    SHFree(ForFree); /* we can not use wString, it was changed */
	    lpFileOp->hNameMappings         = nFileOp.hNameMappings;
	    lpFileOp->fAnyOperationsAborted = nFileOp.fAnyOperationsAborted;
	  }
	  else
	  {
	    wString = ForFree = SHAlloc(size * sizeof(WCHAR));
	    if (ForFree) continue;
	    retCode = ERROR_OUTOFMEMORY;
	    SetLastError(retCode);
	  }
	  return retCode;
	}
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

