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
        DWORD FromAttr;
        DWORD ToAttr;
        LPSTR pTempTo = NULL;
        LPSTR pTempFrom;
        LPSTR pNextFrom;
        LPSTR pNextTo;
        LPSTR pToFile;

        FILEOP_FLAGS OFl = ((FILEOP_FLAGS)lpFileOp->fFlags & 0x7ff);
        BOOL Multi = TRUE;
        BOOL withFileName = FALSE;
        BOOL not_overwrite;
        BOOL toSingle;
        BOOL StarStar;
        BOOL copyOk;
        long lenFrom = -1;
        long lenTo   = -1;
        long lenTempTo;
        long retCode = 0x75;
        long TempretCode = 0;
        long lenNextFrom;
        long lenNextTo;
        SHFILEOPSTRUCTA nlpFileOp = *(lpFileOp);
        HANDLE 	hFind;
        WIN32_FIND_DATAA wfd;

/* default no error
*/
	lpFileOp->fAnyOperationsAborted=FALSE;
	nlpFileOp.fAnyOperationsAborted=FALSE;
#else
	LPSTR pTempTo;
#endif
	
	switch(lpFileOp->wFunc) {
	case FO_COPY:
		TRACE("File Copy:\n");
#ifdef __WIN32OS2__
            pTempTo = HeapAlloc(GetProcessHeap(), 0, 4 * MAX_PATH+4);
            pNextFrom = &pTempTo[2*MAX_PATH+2];
            pNextTo = &pNextFrom[MAX_PATH+1];
/* 
 * FOF_MULTIDESTFILES, FOF_NOCONFIRMATION, FOF_FILESONLY				are	implemented
 * FOF_CONFIRMMOUSE, FOF_SILENT, FOF_NOCONFIRMMKDIR, FOF_SIMPLEPROGRESS are not implemented and ignored
 * if any other flag set, an error occurs
 */  
            OFl = (OFl & (-1 - (FOF_MULTIDESTFILES | FOF_FILESONLY)));
            OFl = (OFl ^ (FOF_SILENT | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMMKDIR));
            if (OFl) {
                if (OFl & (-1 - (FOF_CONFIRMMOUSE | FOF_SILENT | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMMKDIR))) {
                    FIXME(__FUNCTION__" FO_COPY with this fFlags not implemented:%2x ,stub\n",lpFileOp->fFlags);
                    lpFileOp->fAnyOperationsAborted=TRUE;
                } else {
// not FOF_SILENT, not FOF_SIMPLEPROGRESS, not FOF_NOCONFIRMMKDIR
                    FIXME(__FUNCTION__" FO_COPY with this lpFileOp->fFlags not full implemented:0x%2x ,stub\n",lpFileOp->fFlags);
                } /* endif */              
            } /* endif */

            not_overwrite = (!(lpFileOp->fFlags & FOF_NOCONFIRMATION));

// fix for more then one source for one target 
            pToFile = pTempTo;

            while((pFrom+=lenFrom+1)[0] && !nlpFileOp.fAnyOperationsAborted) {

                if (!withFileName && Multi && (pTo[lenTo+1]=='\0')) {
// Win Bug ?
                    Multi = FALSE;
                } /* endif */

                if (Multi) pTo += lenTo + 1;
                if(!pTo[0]) break;

                TRACE("   From='%s' To='%s'\n", pFrom, pTo);

// fix for more then one source for one target 
                pToFile[0] = '\0';

                lenFrom=strlen(pFrom);
                strcpy(pNextFrom,pFrom);
                FromAttr = GetFileAttributesA(pNextFrom);
                StarStar = FALSE;

                if (Multi) {
                    lenTo = strlen(pTo);
//  single targetdir !Multi
                    Multi = (Multi && (lpFileOp->fFlags & FOF_MULTIDESTFILES));
//  multi target, each one for one source. ? last target + more than one source (all source files an one dir as target)

                    toSingle = ((pTo[lenTo+1]=='\0') || !Multi);
                    withFileName = FALSE;

                    strcpy(pTempTo,pTo);
                    PathRemoveBackslashA(pTempTo);
                    ToAttr = GetFileAttributesA(pTempTo);
                    if (ToAttr == -1 || !(ToAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                        withFileName = ((lenTo == strlen(pTempTo)) || !toSingle);
                        if (withFileName) {
// Target must not be an directory
                            PathRemoveFileSpecA(pTempTo);
                            ToAttr = GetFileAttributesA(pTempTo);
                        }
                    }
                    if (ToAttr == -1) {
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        break;
                    } 
                    if  (!(ToAttr & FILE_ATTRIBUTE_DIRECTORY) || (!withFileName && !toSingle) ) {
// never Create directory at this time
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        /* retCode=0x279; */
                        break;
                    }
                    lenTempTo = strlen(pTempTo);
                    withFileName = (((lenTempTo + 1) <  lenTo) || (PathIsRootA(pTo) && lenTempTo < lenTo));
                    PathAddBackslashA(pTempTo);
                    StarStar = (Multi && !withFileName &&
                        (ToAttr & FILE_ATTRIBUTE_DIRECTORY) &&
                        (FromAttr != -1) &&
                        (FromAttr & FILE_ATTRIBUTE_DIRECTORY) &&
                        (0!=strcmp(pTempTo,pTo)));
                }

                if (FromAttr == -1 || StarStar) {
// is Source an existing directory\*.* ?
                    if (FromAttr == -1) {
                        PathRemoveFileSpecA(pNextFrom);
                        FromAttr = GetFileAttributesA(pNextFrom);
                    }
                    PathAddBackslashA(pNextFrom);
                    lenNextFrom = strlen(pNextFrom);
                    pToFile=&pNextFrom[lenNextFrom];
                        
                    if (FromAttr == -1 ||
                       ((0==strcmp(pNextFrom,pFrom)) && !PathIsRootA(pFrom)) ||
                      !(FromAttr & FILE_ATTRIBUTE_DIRECTORY) ||
                      !((0==strcmp(&pFrom[lenNextFrom],"*.*")) || StarStar)) {
                        retCode=0x402;
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        break;
                    }

                    strcpy(pToFile, "*.*");
                    hFind = FindFirstFileA(pNextFrom, &wfd);
                    if (INVALID_HANDLE_VALUE == hFind) {
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        retCode=0x79;
                        break;
                    }
                    nlpFileOp.pFrom = pNextFrom;
                    strcpy(pNextTo,pTo);
                    pNextTo[strlen(pNextTo)+1]='\0';
                    nlpFileOp.fFlags = (nlpFileOp.fFlags & (-1 - (FOF_MULTIDESTFILES)));

                    TRACE(__FUNCTION__"   Copy between Subdir %s -> %s'\n", nlpFileOp.pFrom, nlpFileOp.pTo);

                    do {
                        TRACE(__FUNCTION__"   find '%s'\n", wfd.cFileName);
                        if (0==strcmp(wfd.cFileName,".")) continue;
                        if (0==strcmp(wfd.cFileName,"..")) continue;
                        if ((nlpFileOp.fFlags & FOF_FILESONLY) && (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)) {
                            continue;
                        } /* endif */

                        strcpy(pToFile,wfd.cFileName);
                        pNextFrom[strlen(pNextFrom)+1]='\0';

                        TempretCode = SHFileOperationA (&nlpFileOp);

                        if (nlpFileOp.fAnyOperationsAborted) {/*retCode=0x179;*/break;}

                    } while(FindNextFileA(hFind, &wfd));

                    FindClose(hFind);
                    if (nlpFileOp.fAnyOperationsAborted) break;
                    continue;
                }


                lenTempTo = strlen(pTempTo);
                pToFile = &pTempTo[lenTempTo];
// Check Source
                strcpy(pToFile,pNextFrom);
                PathRemoveBackslashA(pToFile);
                if (strlen(pToFile)<lenFrom) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    retCode=0x402;
                    break;
                } /* endif */

// target name in target or from source
                pTempFrom = NULL;
                if (withFileName) {
                    if ((pFrom[lenFrom+1]=='\0') || (Multi && !(pTo[lenTo+1]=='\0'))) {
                        pTempFrom = pTo;
                    } /* endif */
                } else {
// Multi Target 
                    if (!Multi || !(pFrom[lenFrom+1]=='\0') ||
// only target+\, target without \ has 0x402
                        (Multi && (FromAttr & FILE_ATTRIBUTE_DIRECTORY) && (ToAttr & FILE_ATTRIBUTE_DIRECTORY))) {
                        pTempFrom = pNextFrom;
                    }
                } /* endif */

                if (!pTempFrom) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    break;
                } /* endif */

// move isolated target filename
                strcpy(pToFile,pTempFrom);
                PathRemoveFileSpecA(pToFile);
                PathAddBackslashA(pToFile);

                strcpy(pToFile,&pTempFrom[strlen(pToFile)]);
                ToAttr = GetFileAttributesA(pTempTo);


                if (FromAttr == -1) {
                    FIXME(__FUNCTION__" FO_COPY with Source %s not implementiert ,stub\n",pNextFrom);
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    retCode=0x76;
                    break;
                }
                if (FromAttr & FILE_ATTRIBUTE_DIRECTORY) {
                    if (ToAttr == -1) {
// Try to create an new Directory and enter in it
                        TRACE("   Creating Directory '%s'\n", pTempTo);
                        SHCreateDirectory(NULL,pTempTo);
                        ToAttr = GetFileAttributesA(pTempTo);
                        if (ToAttr == -1) {
                            nlpFileOp.fAnyOperationsAborted=TRUE;
                            retCode=0x10003;
                            break;
                        }

                        lenTempTo = strlen(pTempTo);

                        PathAddBackslashA(pNextFrom);
                        strcat(pNextFrom, "*.*");
                        pNextFrom[strlen(pNextFrom)+1]='\0';
                        nlpFileOp.pFrom = pNextFrom;

                        pTempTo[lenTempTo+1]='\0';
                        nlpFileOp.pTo = pTempTo;

                        TRACE(__FUNCTION__"   Entering Directory '%s'\n", nlpFileOp.pTo);
                        TempretCode = SHFileOperationA (&nlpFileOp);

                        if (nlpFileOp.fAnyOperationsAborted) {break;}
                        continue;

                    } else {
                        if (!(ToAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                            FIXME(__FUNCTION__" FO_COPY only with %s -> %t ? ,stub\n",pNextFrom,pTo);
                            nlpFileOp.fAnyOperationsAborted=TRUE;
                            retCode=0x77;
                            break;
                        }
                        if (strlen(pToFile)==0) {
                            nlpFileOp.fAnyOperationsAborted=TRUE;
                            retCode=0x78;
                            break;
                        } else {
                            nlpFileOp.fAnyOperationsAborted=TRUE;
                            retCode=0x80;
                            break;
                        } /* endif */

                    }

                }

                if (!(ToAttr == -1) && (ToAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    break;
                }
// first try to copy
                if (CopyFileA(pNextFrom, pTempTo, not_overwrite)) continue;

                if (not_overwrite) {
                    if (SHELL_ConfirmDialog (ASK_OVERWRITE_FILE, pTempTo))
// second try to copy after confirm
                        if (CopyFileA(pNextFrom, pTempTo, FALSE)) continue;
                } /* endif */

                nlpFileOp.fAnyOperationsAborted=TRUE;
            }

            if (pTempTo) HeapFree(GetProcessHeap(), 0, pTempTo);

            if (nlpFileOp.fAnyOperationsAborted) {
                lpFileOp->fAnyOperationsAborted=TRUE;
                if (TempretCode > retCode) {
                    retCode = TempretCode;
                } /* endif */
            }
            if (lpFileOp->fAnyOperationsAborted==TRUE) {
                TRACE(__FUNCTION__" Setting AnyOpsAborted=TRUE\n");
                return retCode;
            } /* endif */

            TRACE(__FUNCTION__" Setting AnyOpsAborted=FALSE\n");
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
