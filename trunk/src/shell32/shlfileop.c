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
        LPSTR pTempFrom = NULL;
        LPSTR pTempTo;
        LPSTR pFromFile;
        LPSTR pToFile;

        FILEOP_FLAGS OFl = ((FILEOP_FLAGS)lpFileOp->fFlags & 0x7ff);
        BOOL Multi = TRUE;
        BOOL withFileName;
        BOOL not_overwrite;
        BOOL ToSingle;
        BOOL BothDir;
        BOOL ToWithoutBackSlash;
        long lenFrom = -1;
        long lenTo   = -1;
        long lenTempFrom;
        long lenTempTo;
        long retCode = 0x75;
        long TempretCode = 0;
        long where       = 0;
        SHFILEOPSTRUCTA nlpFileOp = *(lpFileOp);
        HANDLE hFind;
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
            pTempFrom = HeapAlloc(GetProcessHeap(), 0, 3 * MAX_PATH+6);
            pTempTo = &pTempFrom[MAX_PATH+4];
/* 
 * FOF_MULTIDESTFILES, FOF_NOCONFIRMATION, FOF_FILESONLY				are	implemented
 * FOF_CONFIRMMOUSE, FOF_SILENT, FOF_NOCONFIRMMKDIR, FOF_SIMPLEPROGRESS are not implemented and ignored
 * if any other flag set, an error occurs
 */  
            OFl = (OFl & (-1 - (FOF_MULTIDESTFILES | FOF_FILESONLY)));
            OFl = (OFl ^ (FOF_SILENT | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMMKDIR));
            if (OFl) {
                if (OFl & (-1 - (FOF_CONFIRMMOUSE | FOF_SILENT | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMMKDIR))) {
                    FIXME(__FUNCTION__" FO_COPY with this fFlags not implemented:%x ,stub\n",lpFileOp->fFlags);
                    lpFileOp->fAnyOperationsAborted=TRUE;
                } else {
// not FOF_SILENT, not FOF_SIMPLEPROGRESS, not FOF_NOCONFIRMMKDIR
                    FIXME(__FUNCTION__" FO_COPY with this lpFileOp->fFlags not full implemented:0x%x ,stub\n",lpFileOp->fFlags);
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
                if(!pTo[0]) {
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        where = 213;
                        break;
                }

                TRACE("   From='%s' To='%s'\n", pFrom, pTo);

// fix for more then one source for one target 
                pToFile[0] = '\0';
                nlpFileOp.pTo = pTo;

                lenFrom=strlen(pFrom);
                strcpy(pTempFrom,pFrom);
                FromAttr = GetFileAttributesA(pTempFrom);

                if (Multi) {
                    lenTo = strlen(pTo);
//  single targetdir !Multi
                    Multi = (Multi && (lpFileOp->fFlags & FOF_MULTIDESTFILES));
//  multi target, each one for one source. ? last target + more than one source (all source files an one dir as target)

                    ToSingle = ((pTo[lenTo+1]=='\0') || !Multi);

                    strcpy(pTempTo,pTo);
                    PathRemoveBackslashA(pTempTo);
                    ToWithoutBackSlash = (strlen(pTempTo)==lenTo); 
                    ToAttr = GetFileAttributesA(pTempTo);

                    BothDir = (Multi                       &&
                               ToWithoutBackSlash          &&
                               (-1 != (FromAttr | ToAttr)) && 
                               (ToAttr & FromAttr & FILE_ATTRIBUTE_DIRECTORY));

                    withFileName = (!BothDir                      &&
                                (ToWithoutBackSlash || !ToSingle) &&
                                (ToAttr == -1 || !(ToAttr & FILE_ATTRIBUTE_DIRECTORY)));

                    if (withFileName) {
// Target must not be an directory
                        PathRemoveFileSpecA(pTempTo);
                        ToAttr = GetFileAttributesA(pTempTo);
                    }
                    if ((ToAttr == -1) ||
                        !(ToAttr & FILE_ATTRIBUTE_DIRECTORY) ||
                        (!withFileName && !ToSingle) ) {
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        where = 201;
                        break;
                    } 
                    lenTempTo = strlen(pTempTo);
                    withFileName = (((lenTempTo + 1) <  lenTo) || (PathIsRootA(pTo) && lenTempTo < lenTo));
                    PathAddBackslashA(pTempTo);
                }

                if (FromAttr == -1 || BothDir) {
// is Source an existing directory\*.* ?
                    if (FromAttr == -1) {
                        PathRemoveFileSpecA(pTempFrom);
                        FromAttr = GetFileAttributesA(pTempFrom);
                    }

                    PathAddBackslashA(pTempFrom);
                    lenTempFrom = strlen(pTempFrom);
                    pFromFile=&pTempFrom[lenTempFrom];

                    if (FromAttr == -1 ||
                       ((lenTempFrom==lenFrom) && !PathIsRootA(pFrom)) ||
                      !(FromAttr & FILE_ATTRIBUTE_DIRECTORY) ||
                      !((0==strcmp(&pFrom[lenTempFrom],"*.*")) || BothDir)) {
                        retCode=0x402;
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        where = 202;
                        break;
                    }

                    strcpy(pFromFile, "*.*");
                    hFind = FindFirstFileA(pTempFrom, &wfd);
                    if (INVALID_HANDLE_VALUE == hFind) {
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        retCode=0x79;
                        where = 203;
                        break;
                    }

                    nlpFileOp.pFrom  = pTempFrom;
// single copy never with FOF_MULTIDESTFILES, I can use lpFileOp->pTo as nlpFileOp.pTo,
// I need no different targetarea for the name 
                    nlpFileOp.fFlags = (nlpFileOp.fFlags & (-1 - (FOF_MULTIDESTFILES)));

                    TRACE(__FUNCTION__"   Copy between Subdir %s -> %s'\n", nlpFileOp.pFrom, nlpFileOp.pTo);

                    do {
                        TRACE(__FUNCTION__"   find '%s'\n", wfd.cFileName);
                        if (0==strcmp(wfd.cFileName,".")) continue;
                        if (0==strcmp(wfd.cFileName,"..")) continue;
                        if ((nlpFileOp.fFlags & FOF_FILESONLY) && (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)) {
                            continue;
                        } /* endif */

                        strcpy(pFromFile,wfd.cFileName);
                        pTempFrom[strlen(pTempFrom)+1]='\0';

                        TempretCode = SHFileOperationA (&nlpFileOp);

                        if (nlpFileOp.fAnyOperationsAborted) {where = 204;break;}

                    } while(FindNextFileA(hFind, &wfd));

                    FindClose(hFind);
                    if (nlpFileOp.fAnyOperationsAborted) {where = 205;break;}
                    continue;
                }

                lenTempTo = strlen(pTempTo);
                pToFile = &pTempTo[lenTempTo];
// Check Source
                strcpy(pToFile,pTempFrom);
                PathRemoveBackslashA(pToFile);
                if (strlen(pToFile)<lenFrom) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    retCode=0x402;
                    where = 206;
                    break;
                } /* endif */

// target name in target or from source
                pFromFile = NULL;
                if (withFileName) {
                    if ((pFrom[lenFrom+1]=='\0') || (Multi && !(pTo[lenTo+1]=='\0'))) {
                        pFromFile = pTo;
                    } /* endif */
                } else {
// Multi Target 
                    if (!Multi || !(pFrom[lenFrom+1]=='\0') ||
// only target+\, target without \ has 0x402
                        (Multi && (FromAttr & ToAttr & FILE_ATTRIBUTE_DIRECTORY))) {
                        pFromFile = pTempFrom;
                    }
                } /* endif */

                if (!pFromFile) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    where = 207;
                    break;
                } /* endif */

// move isolated target filename
                strcpy(pToFile,pFromFile);
                PathRemoveFileSpecA(pToFile);
                PathAddBackslashA(pToFile);

                strcpy(pToFile,&pFromFile[strlen(pToFile)]);
                ToAttr = GetFileAttributesA(pTempTo);

                if (FromAttr == -1) {
                    FIXME(__FUNCTION__" FO_COPY with Source %s not implementiert ,stub\n",pTempFrom);
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    where = 208;
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
                            where = 209;
                            break;
                        }

                        lenTempTo = strlen(pTempTo);

                        PathAddBackslashA(pTempFrom);
                        strcat(pTempFrom, "*.*");
                        pTempFrom[strlen(pTempFrom)+1]='\0';
                        nlpFileOp.pFrom = pTempFrom;

                        pTempTo[lenTempTo+1]='\0';
                        nlpFileOp.pTo = pTempTo;

                        TRACE(__FUNCTION__"   Entering Directory '%s'\n", nlpFileOp.pTo);
                        TempretCode = SHFileOperationA (&nlpFileOp);

                        if (nlpFileOp.fAnyOperationsAborted) {break;}
                        continue;

                    } else {
                        FIXME(__FUNCTION__" FO_COPY unexpected with %s -> %s ? ,stub\n",pTempFrom,pTo);
                        nlpFileOp.fAnyOperationsAborted=TRUE;
                        where = 210;
                        retCode=0x77;
                        break;

                    }

                }

                if (!(ToAttr == -1) && (ToAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    where = 211;
                    break;
                }
                if (0==strcmp(pTempFrom, pTempTo)) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    retCode = 0x71;
                    where = 212;
                    break;
                }
// first try to copy
                if (CopyFileA(pTempFrom, pTempTo, not_overwrite)) continue;

                if (not_overwrite) {
                    if (SHELL_ConfirmDialog (ASK_OVERWRITE_FILE, pTempTo))
// second try to copy after confirm
                        if (CopyFileA(pTempFrom, pTempTo, FALSE)) continue;
                } /* endif */

                nlpFileOp.fAnyOperationsAborted=TRUE;
                where = 215;
            }
            break;
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
#ifdef __WIN32OS2__
            while((pFrom+=lenFrom+1)[0] && !nlpFileOp.fAnyOperationsAborted) {
                lenFrom=strlen(pFrom);
                FromAttr = GetFileAttributesA(pFrom);
                if (!(FromAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                    TRACE("   File='%s'\n", pFrom);
                    if (DeleteFileA(pFrom)) continue;
                    nlpFileOp.fAnyOperationsAborted=TRUE;
//                  retCode = 0x71;
                    where = 301;
                    break;
                }
                if (!(pTempFrom)) pTempFrom = HeapAlloc(GetProcessHeap(), 0, MAX_PATH+2);
                strcpy(pTempFrom,pFrom);
                PathRemoveBackslashA(pTempFrom);
                FromAttr = GetFileAttributesA(pTempFrom);
                if (!(FromAttr & FILE_ATTRIBUTE_DIRECTORY) ) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
//                  retCode = 0x71;
                    where = 302;
                    break;
                }
// is Source an existing directory\*.* ?
                if (FromAttr == -1) {
                    PathRemoveFileSpecA(pTempFrom);
                    FromAttr = GetFileAttributesA(pTempFrom);
                }

                PathAddBackslashA(pTempFrom);
                lenTempFrom = strlen(pTempFrom);
                pFromFile=&pTempFrom[lenTempFrom];

                if (FromAttr == -1 ||
                   ((lenTempFrom==lenFrom) && !PathIsRootA(pFrom)) ||
                  !(FromAttr & FILE_ATTRIBUTE_DIRECTORY) ||
                  !(('\0'==pFrom[lenTempFrom]) || (0==strcmp(&pFrom[lenTempFrom],"*.*"))) ) {
                    retCode=0x402;
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    where = 303;
                    break;
                }
                strcpy(pFromFile, "*.*");
                lenTempFrom = strlen(pTempFrom);
                if (lenFrom < lenTempFrom) {
// Source is without \*.*
                    pTempFrom[lenTempFrom+1]='\0';
                    nlpFileOp.pFrom = pTempFrom;

                    TRACE(__FUNCTION__"   Entering Directory '%s'\n", nlpFileOp.pFrom);
                    TempretCode = SHFileOperationA (&nlpFileOp);

                    if (nlpFileOp.fAnyOperationsAborted) {break;}
// Call SHELL_DeleteDirectoryA ?
                    if (RemoveDirectoryA(pFrom)) continue;
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    where = 304;
                    break;
                }
                hFind = FindFirstFileA(pTempFrom, &wfd);
                if (INVALID_HANDLE_VALUE == hFind) {
                    nlpFileOp.fAnyOperationsAborted=TRUE;
                    retCode=0x79;
                    where = 303;
                    break;
                }

                nlpFileOp.pFrom  = pTempFrom;
// single copy never with FOF_MULTIDESTFILES, I can use lpFileOp->pTo as nlpFileOp.pTo,
// I need no different targetarea for the name 
                nlpFileOp.fFlags = (nlpFileOp.fFlags & (-1 - (FOF_MULTIDESTFILES)));

                TRACE(__FUNCTION__"   Delete in Subdir %s'\n", nlpFileOp.pFrom);

                do {
                    TRACE(__FUNCTION__"   find '%s'\n", wfd.cFileName);
                    if (0==strcmp(wfd.cFileName,".")) continue;
                    if (0==strcmp(wfd.cFileName,"..")) continue;
                    if ((nlpFileOp.fFlags & FOF_FILESONLY) && (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)) {
                        continue;
                        } /* endif */

                    strcpy(pFromFile,wfd.cFileName);
                    pTempFrom[strlen(pTempFrom)+1]='\0';

                    TempretCode = SHFileOperationA (&nlpFileOp);

                    if (nlpFileOp.fAnyOperationsAborted) {where = 304;break;}

                } while(FindNextFileA(hFind, &wfd));

                FindClose(hFind);
                if (nlpFileOp.fAnyOperationsAborted) {where = 305;break;}
                continue;
            }
            break;
	case FO_MOVE:
		TRACE("File\\Tree Move: simply (Copy/Delete)\n");
        nlpFileOp.wFunc = FO_COPY;
// not delete at error from copy
        TempretCode = SHFileOperationA (&nlpFileOp);

        if (nlpFileOp.fAnyOperationsAborted) {
            if (TempretCode == 0x75) {
// not all, the most
                TempretCode = 0xD7;
                retCode = 0xD7;
            } /* endif */
            break;
        }

        nlpFileOp.wFunc = FO_DELETE;

        TempretCode = SHFileOperationA (&nlpFileOp);

	case 0:
            break;
#else
		while(1) {
			if(!pFrom[0]) break;
			TRACE("   File='%s'\n", pFrom);
			DeleteFileA(pFrom);
			pFrom += strlen(pFrom) + 1;
		}
		TRACE("Setting AnyOpsAborted=FALSE\n");
		lpFileOp->fAnyOperationsAborted=FALSE;
		return 0;
#endif

	default:
#ifdef __WIN32OS2__
		FIXME(__FUNCTION__" Unhandled shell file operation %d stub\n", lpFileOp->wFunc);
#else
		FIXME("Unhandled shell file operation %d\n", lpFileOp->wFunc);
#endif
	   return 1;
	}

#ifdef __WIN32OS2__
    if (pTempFrom) HeapFree(GetProcessHeap(), 0, pTempFrom);

    if (nlpFileOp.fAnyOperationsAborted) {
        lpFileOp->fAnyOperationsAborted=TRUE;
        if (TempretCode > retCode) {
            retCode = TempretCode;
        } /* endif */
    }
    if (lpFileOp->fAnyOperationsAborted==TRUE) {
        if (FO_DELETE == lpFileOp->wFunc) {
            TRACE(__FUNCTION__" Setting AnyOpsAborted=TRUE ret=0x%x, at=%i with %s\n",retCode,where,pFrom);
        } else {
            TRACE(__FUNCTION__" Setting AnyOpsAborted=TRUE ret=0x%x, at=%i with %s -> %s\n",retCode,where,pFrom,pTo);
        }
        return retCode;
    } /* endif */
    TRACE(__FUNCTION__" Setting AnyOpsAborted=FALSE\n");
    return 0;        

#endif
}

/*************************************************************************
 * SHFileOperationW				[SHELL32.244]
 *
 * NOTES
 *     exported by name
 */
DWORD WINAPI SHFileOperationW (LPSHFILEOPSTRUCTW lpFileOp)   
{
#ifdef __WIN32OS2__
	FIXME(__FUNCTION__"(%p) ,stub\n", lpFileOp);
#else
	FIXME("(%p):stub.\n", lpFileOp);
#endif
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
#ifdef __WIN32OS2__
{	FIXME(__FUNCTION__"(%p, %p) ,stub\n",u,v);
#else
{	FIXME("%p %p stub\n",u,v);
#endif
	return 0;
}

/*************************************************************************
 * SheChangeDirW [SHELL32.274]
 *
 */
HRESULT WINAPI SheChangeDirW(LPWSTR u)
#ifdef __WIN32OS2__
{	FIXME(__FUNCTION__"(%s),stub\n",debugstr_w(u));
#else
{	FIXME("(%s),stub\n",debugstr_w(u));
#endif
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

