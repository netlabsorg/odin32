/*
 * SHFileOperation
 */
#ifdef __WIN32OS2__
#define ICOM_CINTERFACE 1
#include <odin.h>
#endif
#include <string.h>
#include "debugtools.h"
#include "shellapi.h"
#include "shlwapi.h"

#include "shlobj.h"
#include "shresdef.h"
#include "shell32_main.h"
#include "wine/undocshell.h"
#include "shlwapi.h"

DEFAULT_DEBUG_CHANNEL(shell);

BOOL SHELL_WarnItemDelete (int nKindOfDialog, LPCSTR szDir)
{
	char szCaption[255], szText[255], szBuffer[MAX_PATH + 256];

        if(nKindOfDialog == ASK_DELETE_FILE)
        {
	  LoadStringA(shell32_hInstance, IDS_DELETEITEM_TEXT, szText, 
		sizeof(szText));
	  LoadStringA(shell32_hInstance, IDS_DELETEITEM_CAPTION, 
		szCaption, sizeof(szCaption));
	}
        else if(nKindOfDialog == ASK_DELETE_FOLDER)
        {
	  LoadStringA(shell32_hInstance, IDS_DELETEITEM_TEXT, szText, 
		sizeof(szText));
	  LoadStringA(shell32_hInstance, IDS_DELETEFOLDER_CAPTION, 
		szCaption, sizeof(szCaption));
        }
        else if(nKindOfDialog == ASK_DELETE_MULTIPLE_ITEM)
        {
	  LoadStringA(shell32_hInstance, IDS_DELETEMULTIPLE_TEXT, szText, 
		sizeof(szText));
	  LoadStringA(shell32_hInstance, IDS_DELETEITEM_CAPTION, 
		szCaption, sizeof(szCaption));
        }
	else {
          FIXME("Called without a valid nKindOfDialog specified!");
	  LoadStringA(shell32_hInstance, IDS_DELETEITEM_TEXT, szText, 
		sizeof(szText));
	  LoadStringA(shell32_hInstance, IDS_DELETEITEM_CAPTION, 
		szCaption, sizeof(szCaption));
	}          

	FormatMessageA(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
	    szText, 0, 0, szBuffer, sizeof(szBuffer), (va_list*)&szDir);

	return (IDOK == MessageBoxA(GetActiveWindow(), szBuffer, szCaption, MB_OKCANCEL | MB_ICONEXCLAMATION));
}

/**************************************************************************
 *	SHELL_DeleteDirectoryA()
 *
 * like rm -r
 */

BOOL SHELL_DeleteDirectoryA(LPCSTR pszDir, BOOL bShowUI)
{
	BOOL		ret = FALSE;
	HANDLE		hFind;
	WIN32_FIND_DATAA wfd;
	char		szTemp[MAX_PATH];

	strcpy(szTemp, pszDir);
	PathAddBackslashA(szTemp);
	strcat(szTemp, "*.*");
	
	if (bShowUI && !SHELL_WarnItemDelete(ASK_DELETE_FOLDER, pszDir))
	  return FALSE;
	
	if(INVALID_HANDLE_VALUE != (hFind = FindFirstFileA(szTemp, &wfd)))
	{
	  do
	  {
	    if(strcasecmp(wfd.cFileName, ".") && strcasecmp(wfd.cFileName, ".."))
	    {
	      strcpy(szTemp, pszDir);
	      PathAddBackslashA(szTemp);
	      strcat(szTemp, wfd.cFileName);
	
	      if(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)
	        SHELL_DeleteDirectoryA(szTemp, FALSE);
	      else
	        DeleteFileA(szTemp);
	    }
	  } while(FindNextFileA(hFind, &wfd));

	  FindClose(hFind);
	  ret = RemoveDirectoryA(pszDir);
	}

	return ret;
}

/**************************************************************************
 *	SHELL_DeleteFileA()
 */

BOOL SHELL_DeleteFileA(LPCSTR pszFile, BOOL bShowUI)
{
	if (bShowUI && !SHELL_WarnItemDelete(ASK_DELETE_FILE, pszFile))
		return FALSE;
 
        return DeleteFileA(pszFile);
}

/*************************************************************************
 * SHCreateDirectory				[SHELL32.165]
 *
 * NOTES
 *  exported by ordinal
 *  not sure about LPSECURITY_ATTRIBUTES
 */
DWORD WINAPI SHCreateDirectory(LPSECURITY_ATTRIBUTES sec,LPCSTR path)
{
	DWORD ret;
	TRACE("(%p,%s)\n",sec,path);
	if ((ret = CreateDirectoryA(path,sec)))
	{
	  SHChangeNotifyA(SHCNE_MKDIR, SHCNF_PATHA, path, NULL);
	}
	return ret;
}

/************************************************************************
 *      Win32DeleteFile                         [SHELL32.164]  
 *
 * Deletes a file.  Also triggers a change notify if one exists.
 *
 * FIXME:
 * Verified on Win98 / IE 5 (SHELL32 4.72, March 1999 build) to be
 * ANSI.  Is this Unicode on NT?
 *
 */ 

BOOL WINAPI Win32DeleteFile(LPSTR fName)
{
	TRACE("%p(%s)\n", fName, fName);

	DeleteFileA(fName);
	SHChangeNotifyA(SHCNE_DELETE, SHCNF_PATHA, fName, NULL);
	return TRUE;
}

/*************************************************************************
 * SHFileOperationA				[SHELL32.243]
 *
 * NOTES
 *     exported by name
 */
DWORD WINAPI SHFileOperationA (LPSHFILEOPSTRUCTA lpFileOp)   
{
	LPSTR pFrom = (LPSTR)lpFileOp->pFrom;
	LPSTR pTo = (LPSTR)lpFileOp->pTo;
	LPSTR pTempTo;
	
	switch(lpFileOp->wFunc) {
	case FO_COPY:
		TRACE("File Copy:\n");
		while(1) {
			if(!pFrom[0]) break;
			if(!pTo[0]) break;
			TRACE("   From='%s' To='%s'\n", pFrom, pTo);

                        pTempTo = HeapAlloc(GetProcessHeap(), 0, strlen(pTo)+1);
                        if (pTempTo)
                        {
                            strcpy( pTempTo, pTo );
                            PathRemoveFileSpecA(pTempTo);
                            TRACE("   Creating Directory '%s'\n", pTempTo);
                            SHCreateDirectory(NULL,pTempTo);
                            HeapFree(GetProcessHeap(), 0, pTempTo);
                        }
                        CopyFileA(pFrom, pTo, FALSE);

			pFrom += strlen(pFrom) + 1;
			pTo += strlen(pTo) + 1;
		}
		TRACE("Setting AnyOpsAborted=FALSE\n");
		lpFileOp->fAnyOperationsAborted=FALSE;
		return 0;

	case FO_DELETE:
		TRACE("File Delete:\n");
		while(1) {
			if(!pFrom[0]) break;
			TRACE("   File='%s'\n", pFrom);
			DeleteFileA(pFrom);
			pFrom += strlen(pFrom) + 1;
		}
		TRACE("Setting AnyOpsAborted=FALSE\n");
		lpFileOp->fAnyOperationsAborted=FALSE;
		return 0;

	default:
		FIXME("Unhandled shell file operation %d\n", lpFileOp->wFunc);
	}

	return 1;
}

/*************************************************************************
 * SHFileOperationW				[SHELL32.244]
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
 * SHFileOperation				[SHELL32.242]
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
{	FIXME("%p %p stub\n",u,v);
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
 * IsNetDrive			[SHELL32.66]
 */
BOOL WINAPI IsNetDrive(DWORD drive)
{
	char root[4];
	strcpy(root, "A:\\");
	root[0] += drive;
	return (GetDriveTypeA(root) == DRIVE_REMOTE);
}
