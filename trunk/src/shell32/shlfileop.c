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


#ifdef __WIN32OS2__
BOOL SHELL_ConfirmDialog (int nKindOfDialog, LPCSTR szDir)
{
	char szCaption[255], szText[255], szBuffer[MAX_PATH + 256];
        UINT caption_resource_id, text_resource_id;

	switch(nKindOfDialog) {

	case ASK_DELETE_FILE:
		caption_resource_id	= IDS_DELETEITEM_CAPTION;
		text_resource_id	= IDS_DELETEITEM_TEXT;
		break;
	case ASK_DELETE_FOLDER:
		caption_resource_id	= IDS_DELETEFOLDER_CAPTION;
		text_resource_id	= IDS_DELETEITEM_TEXT;
		break;
	case ASK_DELETE_MULTIPLE_ITEM:
		caption_resource_id	= IDS_DELETEITEM_CAPTION;
		text_resource_id	= IDS_DELETEMULTIPLE_TEXT;
		break;
	case ASK_OVERWRITE_FILE:
		caption_resource_id	= IDS_OVERWRITEFILE_CAPTION;
		text_resource_id	= IDS_OVERWRITEFILE_TEXT;
		break;
	default:
		FIXME(__FUNCTION__" Unhandled nKindOfDialog %d stub\n", nKindOfDialog);
		return FALSE;
	}

	LoadStringA(shell32_hInstance, caption_resource_id, szCaption, sizeof(szCaption));
	LoadStringA(shell32_hInstance, text_resource_id, szText, sizeof(szText));
#else
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
#endif
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

#ifdef __WIN32OS2__
/************************************************************************
 *	Creation from dir for SHFileOperation (FO_COPY)
 *
 */ 

LPSTR DirForFileOperationA (LPSTR pTo, DWORD *dwAttr, LPSHFILEOPSTRUCTA lpFileOp, BOOL toSingle)
{

	LPSTR pTempTo = NULL;
	long lenTo;
	BOOL withFileName = FALSE;
	if(pTo[0]) {
		lenTo = strlen(pTo);
		pTempTo = HeapAlloc(GetProcessHeap(), 0, lenTo + MAX_PATH);

	}
        *dwAttr = -1;
	if (pTempTo) {
		strcpy(pTempTo,pTo);
		PathRemoveBackslashA(pTempTo);
                *dwAttr = GetFileAttributesA(pTempTo);
                if (*(dwAttr) == -1 || !(*(dwAttr) & FILE_ATTRIBUTE_DIRECTORY)) {
                        withFileName = ((lenTo == strlen(pTempTo)) || !toSingle);
			if (withFileName) {
// Target must not be an directory
				PathRemoveFileSpecA(pTempTo);
				*(dwAttr) = GetFileAttributesA(pTempTo);
                        }
                }
                if ((*(dwAttr) == -1) || 
                  ( !(*(dwAttr) & FILE_ATTRIBUTE_DIRECTORY) || (!withFileName && !toSingle) )) {
// !toSingle == !(&pTo[strlen(pTo)+1]=="\0") redundant ?
                        if (!withFileName && !toSingle) {
				FIXME(__FUNCTION__" FO_COPY tagetdir and FOF_MULTIDESTFILES not implemented ? ,stub\n");
                        };
// never Create directory at this time
                                HeapFree(GetProcessHeap(), 0, pTempTo);
                                pTempTo = NULL;
//                      }
                }
        }
        if (!pTempTo) lpFileOp->fAnyOperationsAborted=TRUE;
        return pTempTo;
}
#endif

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
#ifdef __WIN32OS2__
	DWORD ToAttr;
	DWORD FromAttr;
	LPSTR pToFile;
	LPSTR pTempTo = NULL;
	LPSTR pTempFrom;

	FILEOP_FLAGS OFl = ((FILEOP_FLAGS)lpFileOp->fFlags & 0x7ff);
	BOOL Multi = (lpFileOp->fFlags & FOF_MULTIDESTFILES);
	BOOL withFileName = FALSE;
	BOOL not_overwrite;
	BOOL copyOk;
	long lenTo;
/* default no error
*/
	lpFileOp->fAnyOperationsAborted=FALSE;
#else
	LPSTR pTempTo;
#endif
	
	switch(lpFileOp->wFunc) {
	case FO_COPY:
		TRACE("File Copy:\n");
#ifdef __WIN32OS2__
/* 
 * FOF_MULTIDESTFILES, FOF_NOCONFIRMATION				are	implemented
 * FOF_CONFIRMMOUSE, FOF_SILENT, FOF_NOCONFIRMMKDIR, FOF_SIMPLEPROGRESS are not implemented and ignored
 * if any other flag set, an error occurs
 */  
		OFl = (OFl & (-1 - (FOF_MULTIDESTFILES)));
		OFl = (OFl ^ (FOF_SILENT | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMMKDIR));
		if (OFl) {
			if (OFl & (-1 - (FOF_CONFIRMMOUSE | FOF_SILENT | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMMKDIR))) {
				FIXME(__FUNCTION__" FO_COPY with this fFlags not implemented:%2x ,stub\n",lpFileOp->fFlags);
				lpFileOp->fAnyOperationsAborted=TRUE;
			} else {
// not FOF_SILENT, not FOF_SIMPLEPROGRESS, not FOF_NOCONFIRMMKDIR
				FIXME(__FUNCTION__" FO_COPY with this fFlags not full implemented:%2x ,stub\n",lpFileOp->fFlags);
			} /* endif */              
		} /* endif */

		not_overwrite = (!(lpFileOp->fFlags & FOF_NOCONFIRMATION));
		while(!Multi && !lpFileOp->fAnyOperationsAborted) {
// single targetdir
			pTempTo = DirForFileOperationA(pTo, &ToAttr, lpFileOp, TRUE);

			if (!pTempTo) break;

                        lenTo = strlen(pTempTo);
                        withFileName = (((lenTo + 1) <  strlen(pTo)) || (PathIsRootA(pTo) && lenTo < strlen(pTo)));
			PathAddBackslashA(pTempTo);
			break;
		}
		if (!lpFileOp->fAnyOperationsAborted) {
			TRACE("File Copy:\n");
#ifdef __WIN32OS2__
		  while(pFrom[0]) {
#endif
				if(!pTo[0]) break;

				TRACE("   From='%s' To='%s'\n", pFrom, pTo);

				if (Multi) {
// multi target, each one for one source. ? last
					pTempTo = DirForFileOperationA(pTo, &ToAttr, lpFileOp, FALSE);
                                        if (pTempTo) {

                                                lenTo = strlen(pTempTo);
                                                withFileName = (((lenTo + 1) <  strlen(pTo)) || (PathIsRootA(pTo) && lenTo < strlen(pTo)));
						PathAddBackslashA(pTempTo);
					}
				}
				if (pTempTo) {
					lenTo = strlen(pTempTo);
					pToFile = &pTempTo[lenTo];
// target name in target or from source
                                        if (withFileName) {
                                           if ((pFrom[strlen(pFrom)+1]=='\0') ||
                                               (Multi && !(pTo[strlen(pTo)+1]=='\0'))) {
                                                pTempFrom = pTo;
                                           } else {
// more then one source for one targetfile
                                                lpFileOp->fAnyOperationsAborted=TRUE;
                                                break;
                                           } /* endif */
                                        } else {
                                                pTempFrom = pFrom;
                                        } /* endif */
// isolate target filename
					strcpy(pToFile,pTempFrom);
					PathRemoveFileSpecA(pToFile);
					PathAddBackslashA(pToFile);
					lenTo = strlen(pToFile);

					strcpy(pToFile,&pTempFrom[lenTo]);
                                        FromAttr = GetFileAttributesA(pFrom);
                                        if (FromAttr & FILE_ATTRIBUTE_DIRECTORY) {
                                           if (!(FromAttr == -1)) {
                				FIXME(__FUNCTION__" FO_COPY only with sourcedir not implemented ,stub\n");
                                           };
                                           lpFileOp->fAnyOperationsAborted=TRUE;
                                           break;
                                        }
                                        ToAttr = GetFileAttributesA(pTempTo);
                                        if (!(ToAttr == -1) && (ToAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                                           lpFileOp->fAnyOperationsAborted=TRUE;
                                           break;
                                        }
// first try to copy
					copyOk = CopyFileA(pFrom, pTempTo, not_overwrite);

					if (!copyOk && not_overwrite) {
						if (SHELL_ConfirmDialog (ASK_OVERWRITE_FILE, pTempTo))
// second try to copy after confirm
							copyOk = CopyFileA(pFrom, pTempTo, FALSE);
					} /* endif */
					if (!copyOk) lpFileOp->fAnyOperationsAborted=TRUE;
// fix for mor then one source for one target 
					pToFile[0] = '\0';

                                        if (!withFileName && Multi && (pTo[strlen(pTo)+1]=='\0')) {
// Win Bug ?
                                                Multi = FALSE;
                                        } /* endif */
					if (Multi) HeapFree(GetProcessHeap(), 0, pTempTo);
				}				

				pFrom += strlen(pFrom) + 1;
                                
                        	if (Multi) pTo += strlen(pTo) + 1;

			}
		} /* endif */
		if (!Multi && pTempTo) HeapFree(GetProcessHeap(), 0, pTempTo);

		if (lpFileOp->fAnyOperationsAborted==TRUE) {
			TRACE(__FUNCTION__" Setting AnyOpsAborted=TRUE\n");
			return 0x75l;
		} else {
		      TRACE(__FUNCTION__" Setting AnyOpsAborted=FALSE\n");
                } /* endif */
		return 0;
#else
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
#endif

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
