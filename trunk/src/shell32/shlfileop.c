/*
 * SHFileOperation
 *
 * Copyright 2000 Juergen Schmied
 * Copyright 2002 Andriy Palamarchuk
 * Copyright 2002 Dietrich Teickner (from Odin)
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
#include <string.h>
#include "winreg.h"
#include "shellapi.h"
#include "shlobj.h"
#include "shresdef.h"
#include "shell32_main.h"
#include "undocshell.h"
#include "shlwapi.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(shell);

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
          FIXME("Called without a valid nKindOfDialog specified!\n");
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
/*************************************************************************
 *
 * SHFileTyp (internal)
 *
 */
DWORD SHFileTyp(LPSTR pFromTo, long *len, LPSTR pTemp, long *lenTemp, LPSTR *pFile, DWORD *PathAttr, DWORD *Attr) {
#define TypisBad -1
#define TypisUnkn 0
#define TypisNewPath 1
#define TypisRoot 2
#define TypisDirS 3
#define TypisDir  4
#define TypisFile 5
#define TypisName 6
#define TypisNameS 7
#define TypisMask 8
	DWORD Typ = 										TypisUnkn;
	DWORD i_Attr;
	DWORD i_PathAttr;
	BOOL  i_Slash = FALSE;
	LPSTR i_pFile;
	LPSTR i_pLastSlash;
	long i_lenTemp;
	long i_len = *len = strlen(pFromTo);
	if (i_len == 0) return				  TypisUnkn;
	strcpy(pTemp,pFromTo);
	*pFile = i_pFile = &pTemp[i_len];
	pTemp[i_len+1] = '\0';
	if (i_len == 0) return	/* ?? */	  TypisBad;
	if (PathIsRootA(pFromTo))		Typ = TypisRoot;
	i_pLastSlash = strrchr(pTemp,'\\');
	if (i_pLastSlash == NULL) return	  TypisBad /* ??? */;
	i_Slash = (i_pLastSlash[1] == '\0');
	*lenTemp = i_lenTemp = strlen(pTemp) - i_Slash;
	*pFile   = i_pFile   = &pTemp[i_lenTemp];
	if (Typ != TypisRoot) i_pFile[0] = '\0';
	*PathAttr = *Attr = i_PathAttr = i_Attr = GetFileAttributesA(pTemp);
	if (i_Attr == -1) {
		if (Typ == TypisRoot)		Typ = TypisBad;
	} else { 
		if (i_Attr & FILE_ATTRIBUTE_DIRECTORY) {
			if (Typ == TypisUnkn) {
									Typ = TypisDir;
				if (i_Slash)		Typ = TypisDirS;
			}
		} else {
			if (Typ == TypisUnkn && !i_Slash) {
									Typ = TypisFile;
			} else					Typ = TypisBad;
		}
	}
// is the directory exists with \*.* ?
	i_pFile = strrchr(pTemp,'\\');
	if (NULL == i_pFile)			Typ = TypisBad; 
	if (Typ == TypisUnkn || Typ == TypisFile) {
		PathRemoveFileSpecA(pTemp);
// mask in Path ?
		if (NULL != strpbrk(pTemp,"*?\0")) {
									Typ = TypisBad;
		} else {
			while (-1 == (i_PathAttr = GetFileAttributesA(pTemp)) && !PathIsRootA(pTemp)) {
				PathRemoveFileSpecA(pTemp);
				Typ = TypisNewPath;
			}
			i_lenTemp = strlen(pTemp);
			if (pTemp[i_lenTemp - 1] == '\\') i_lenTemp--;
			*lenTemp = i_lenTemp;
			*pFile   = i_pFile = &pTemp[i_lenTemp];
			*PathAttr = i_PathAttr;
			if (-1 == i_PathAttr || !(i_PathAttr & FILE_ATTRIBUTE_DIRECTORY)) Typ = TypisBad;
		}
		strcpy(pTemp,pFromTo);
		if (Typ == TypisUnkn && i_PathAttr != -1 && (i_PathAttr & FILE_ATTRIBUTE_DIRECTORY)) {
 			if (NULL == strpbrk(i_pFile,"*?\0")) {
				if (!i_Slash) {
					*lenTemp = i_lenTemp = strlen(pTemp);
					*pFile   = &pTemp[i_lenTemp];
									Typ = TypisName;
				} else				Typ = TypisNameS;
			} else 					Typ = TypisMask;
		}
	}
	i_pLastSlash[0] = '\\';
	return Typ;
}
#else
/**************************************************************************
 *	SHELL_FileNamesMatch()
 *
 * Accepts two \0 delimited lists of the file names. Checks whether number of
 * files in the both lists is the same.
 */
BOOL SHELL_FileNamesMatch(LPCSTR pszFiles1, LPCSTR pszFiles2)
{
    while ((pszFiles1[strlen(pszFiles1) + 1] != '\0') &&
           (pszFiles2[strlen(pszFiles2) + 1] != '\0'))
    {
        pszFiles1 += strlen(pszFiles1) + 1;
        pszFiles2 += strlen(pszFiles2) + 1;
    }

    return
        ((pszFiles1[strlen(pszFiles1) + 1] == '\0') &&
         (pszFiles2[strlen(pszFiles2) + 1] == '\0')) ||
        ((pszFiles1[strlen(pszFiles1) + 1] != '\0') &&
         (pszFiles2[strlen(pszFiles2) + 1] != '\0'));
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
	LPSTR pTempTo;
#ifdef __WIN32OS2__
	DWORD FromAttr;
	DWORD ToAttr;
	DWORD FromPathAttr;
	DWORD ToPathAttr;
	DWORD FromTyp;
	DWORD ToTyp = TypisBad;
	DWORD zTyp;
	LPSTR pFromFile;
	LPSTR pToFile;
	LPSTR pTempFrom = NULL;
	LPSTR pToSlash;
	LPSTR pToFuncTXT = "FO_COPY";
	FILEOP_FLAGS OFl = ((FILEOP_FLAGS)lpFileOp->fFlags & 0x7ff);
	BOOL Multi = ((lpFileOp->fFlags & FOF_MULTIDESTFILES) != 0);
	BOOL MakeDir = FALSE;
	BOOL not_overwrite;
	BOOL TargetisDir;
	BOOL ask_overwrite;
	BOOL ToSingle;
	BOOL recurseinto;
	long lenFrom = -1;
	long lenTo   = -1;
	long lenTempFrom;
	long lenTempTo;
	long retCode = 0x75;
	long TempretCode = 0;
	long where       = 0;
	long FuncSwitch = (lpFileOp->wFunc & 15);
	SHFILEOPSTRUCTA nlpFileOp = *(lpFileOp);
	long level= nlpFileOp.wFunc>>4;
	HANDLE hFind;
	WIN32_FIND_DATAA wfd;

/* default no error
*/
	lpFileOp->fAnyOperationsAborted=FALSE;
	nlpFileOp.fAnyOperationsAborted=FALSE;
	level++;
	nlpFileOp.wFunc =  (level<<4) + FuncSwitch;
	if (level == 1)
#endif
        TRACE("flags (0x%04x) : %s%s%s%s%s%s%s%s%s%s%s%s \n", lpFileOp->fFlags, 
                lpFileOp->fFlags & FOF_MULTIDESTFILES ? "FOF_MULTIDESTFILES " : "",
                lpFileOp->fFlags & FOF_CONFIRMMOUSE ? "FOF_CONFIRMMOUSE " : "",
                lpFileOp->fFlags & FOF_SILENT ? "FOF_SILENT " : "",
                lpFileOp->fFlags & FOF_RENAMEONCOLLISION ? "FOF_RENAMEONCOLLISION " : "",
                lpFileOp->fFlags & FOF_NOCONFIRMATION ? "FOF_NOCONFIRMATION " : "",
                lpFileOp->fFlags & FOF_WANTMAPPINGHANDLE ? "FOF_WANTMAPPINGHANDLE " : "",
                lpFileOp->fFlags & FOF_ALLOWUNDO ? "FOF_ALLOWUNDO " : "",
                lpFileOp->fFlags & FOF_FILESONLY ? "FOF_FILESONLY " : "",
                lpFileOp->fFlags & FOF_SIMPLEPROGRESS ? "FOF_SIMPLEPROGRESS " : "",
                lpFileOp->fFlags & FOF_NOCONFIRMMKDIR ? "FOF_NOCONFIRMMKDIR " : "",
                lpFileOp->fFlags & FOF_NOERRORUI ? "FOF_NOERRORUI " : "",
                lpFileOp->fFlags & 0xf800 ? "MORE-UNKNOWN-Flags" : "");
#ifdef __WIN32OS2__
	switch(FuncSwitch) {
	case FO_MOVE:
		retCode = 0xb7;
		pToFuncTXT = "FO_MOVE";
#else
	switch(lpFileOp->wFunc) {
	case FO_MOVE:
#endif
	case FO_COPY:
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
#ifdef __WIN32OS2__
			nlpFileOp.pFrom = pTempFrom = HeapAlloc(GetProcessHeap(), 0, 3 * MAX_PATH+6);
			nlpFileOp.pTo   = pTempTo   = &pTempFrom[MAX_PATH+4];
/* 
 * FOF_MULTIDESTFILES, FOF_NOCONFIRMATION, FOF_FILESONLY					are		implemented
 * FOF_CONFIRMMOUSE, FOF_SILENT, FOF_NOCONFIRMMKDIR, FOF_SIMPLEPROGRESS		are not implemented and ignored
 * FOF_RENAMEONCOLLISION							   						are		implemented partially and breaks if file exist
 * FOF_ALLOWUNDO, FOF_WANTMAPPINGHANDLE										are not implemented and breaks
 * if any other flag set, an error occurs
 */  
			TRACE(__FUNCTION__" %s level=%d lpFileOp->fFlags=0x%x\n", pToFuncTXT, level, lpFileOp->fFlags);
//			OFl = (OFl & (-1 - (FOF_MULTIDESTFILES | FOF_FILESONLY)));
//			OFl = (OFl ^ (FOF_SILENT | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMMKDIR));
			OFl = (OFl & ( ~ (FOF_MULTIDESTFILES | FOF_NOCONFIRMATION | FOF_FILESONLY)));	// implemented
			OFl = (OFl ^ (FOF_SILENT | FOF_NOCONFIRMMKDIR));								// ignored, if one
			OFl = (OFl & ( ~ FOF_SIMPLEPROGRESS));											// ignored, only with FOF_SILENT
			if (OFl) {
				if (OFl & ( ~ (FOF_CONFIRMMOUSE | FOF_SILENT | FOF_RENAMEONCOLLISION | FOF_NOCONFIRMMKDIR))) {
					TRACE(__FUNCTION__" %s level=%d lpFileOp->fFlags=0x%x not implemented, Aborted=TRUE, stub\n", pToFuncTXT, level, OFl);
					nlpFileOp.fAnyOperationsAborted=TRUE;
				} else {
					TRACE(__FUNCTION__" %s level=%d lpFileOp->fFlags=0x%x not full implemented ,stub\n", pToFuncTXT, level, OFl);
				} /* endif */
			} /* endif */

			ask_overwrite = (!(lpFileOp->fFlags & FOF_NOCONFIRMATION) && !(lpFileOp->fFlags & FOF_RENAMEONCOLLISION));
			not_overwrite = (!(lpFileOp->fFlags & FOF_NOCONFIRMATION) ||  (lpFileOp->fFlags & FOF_RENAMEONCOLLISION));

// need break at error before change sourcepointer
			while(!nlpFileOp.fAnyOperationsAborted && (pFrom+=lenFrom+1)[0]) {

				if (Multi) pTo += lenTo + 1;
				if(!pTo[0]) {
						nlpFileOp.fAnyOperationsAborted=TRUE;
						where = 200;
						break;
				} /* endif */

				TRACE(__FUNCTION__" %s level=%d From='%s' To='%s'\n", pToFuncTXT, level, pFrom, pTo);

                FromTyp = SHFileTyp(pFrom, &lenFrom,
									pTempFrom, &lenTempFrom,
									&pFromFile,
									&FromPathAttr, &FromAttr);

// Check Source
				if (FromTyp != TypisDir
				 && FromTyp != TypisFile
				 && FromTyp != TypisMask) {        		
						nlpFileOp.fAnyOperationsAborted=TRUE;
						where = 201;
						retCode=0x402;
						break;
				} /* endif */
				zTyp  = ToTyp;
                ToTyp = SHFileTyp(pTo, &lenTo,
									pTempTo, &lenTempTo,
									&pToFile,
									&ToPathAttr, &ToAttr);

// fix for more then one source for one target 
				if (ToTyp == TypisUnkn
				 && zTyp  <  TypisFile
				 && zTyp  >  TypisUnkn
				 && lenTo == 0) {
					pToFile[0] = '\0';
					ToTyp = zTyp;
					lenTo = strlen(pTempTo);
				} /* endif */
// recursiv creating from directorys are not valid for FO_MOVE.
				if (FuncSwitch == FO_MOVE && ToTyp == TypisNewPath) ToTyp = TypisUnkn;
// Check Target
				if (ToTyp == TypisMask) {
					nlpFileOp.fAnyOperationsAborted=TRUE;
					where = 202;
       				if (FromTyp != TypisMask) {
						retCode=0x10003;
					} else {
						if ((lpFileOp->wFunc & 0xf) == FO_MOVE) TempretCode = 0x2;
					}	
					break;
				} /* endif */
				if (ToTyp == TypisBad) {
					nlpFileOp.fAnyOperationsAborted=TRUE;
					where = 203;
					retCode=0x402;
					break;
				} /* endif */
				TargetisDir = (ToTyp == TypisDir || ToTyp == TypisDirS || ToTyp == TypisRoot);
				ToSingle = (pTo[lenTo+1]=='\0');
				if (Multi && ToSingle) {
					Multi = (!(pFrom[lenFrom+1]!='\0'));
				}

				switch(FromTyp) {
				case TypisFile: { /* Source is File */
						if (ToTyp  == TypisName) {
							if (ToSingle && pFrom[lenFrom+1] == '\0') break;
							if (Multi) break;
						}
						if (ToTyp  == TypisFile) {
							if (0 == strcmp(pTempFrom,pTempTo)) { /* target is the same as source ? */
								nlpFileOp.fAnyOperationsAborted=TRUE;
								retCode = 0x71;
								where = 221;
								break;
							} /* endif */
							if (ask_overwrite && SHELL_ConfirmDialog (ASK_OVERWRITE_FILE, pTempTo)) break;
							if (!not_overwrite)  break;
							if (FuncSwitch == FO_MOVE && (!Multi)) TempretCode = 0x75;
						}
						if (!Multi && TargetisDir) {
							strcpy(pToFile,pFromFile);
							pTempTo[strlen(pTempTo)+1] = '\0';
							ToAttr = GetFileAttributesA(pTempTo);
							if (ToAttr == -1)  break;
							if (!(ToAttr & FILE_ATTRIBUTE_DIRECTORY)) break;
						} 
						if (FuncSwitch == FO_MOVE && (ToTyp == TypisUnkn || ToTyp == TypisName || ToTyp == TypisNameS)) TempretCode = 0x75;
						nlpFileOp.fAnyOperationsAborted=TRUE;
						where = 210 + TypisFile;
						break;
					}
				case TypisMask: { /* Source is Mask */
						if (TargetisDir)  break;
						nlpFileOp.fAnyOperationsAborted=TRUE;
						where = 210 + TypisMask;
						break;
					}
				case TypisRoot:  /* Source is Root, only small test with this */
				case TypisDir: { /* Source is Dir  */
						if (TargetisDir) {
// From Root to Root not full tested, also no Comparsion in W98
							MakeDir = (!(lpFileOp->fFlags & FOF_MULTIDESTFILES) && ToTyp != TypisRoot);
							break;
						}
						MakeDir = TRUE;
						if ((ToTyp  == TypisName || ToTyp  == TypisNameS || ToTyp  == TypisNewPath)) break;
						nlpFileOp.fAnyOperationsAborted=TRUE;
						where = 210 + TypisDir;
						break;
					}
				default: {
						nlpFileOp.fAnyOperationsAborted=TRUE;
						where = 210 + FromTyp;
//						retCode=0x750;
						break;
					}
				}
				if (nlpFileOp.fAnyOperationsAborted) {
					if (FuncSwitch == FO_MOVE && (ToTyp == TypisUnkn || ToTyp == TypisName || ToTyp == TypisNameS)) TempretCode = 0x75;
					break;
				}

				recurseinto = FALSE;

				if (FuncSwitch == FO_MOVE && ToTyp == TypisName  && (FromTyp == TypisDir || FromTyp == TypisFile)) {
					if (':' == pTempFrom[1] && ':' == pTempTo[1] && pTempFrom[0] == pTempTo[0]) {
						if (MoveFileA(pTempFrom, pTempTo)) continue;
					}
				}
				if (FromTyp == TypisDir && TargetisDir && MakeDir) {
					PathRemoveFileSpecA(pTempFrom);
					lenTempTo = strlen(pTempFrom);
					strcpy(pTempFrom,pFrom);
					if (pTempFrom[lenTempTo-1] == '\\') lenTempTo--;
					strcpy(pToFile,&pFrom[lenTempTo]);
					lenTempTo = strlen(pTempTo);
					pTempTo[lenTempTo+1] = '\0';
				}

				if (MakeDir) {
					pToSlash = NULL;
					if ((ToTyp  == TypisNameS || ToTyp  == TypisNewPath)) {
						pToSlash = strchr(&pToFile[1],'\\');
						if (NULL != pToSlash) pToSlash[0] = '\0';
					}						
					TRACE(__FUNCTION__" %s level=%d Creating Directory '%s'\n", pToFuncTXT, level, pTempTo);
					SHCreateDirectory(NULL,pTempTo);
					ToPathAttr = GetFileAttributesA(pTempTo);
					if (NULL != pToSlash) pToSlash[0] = '\\';
					if (ToPathAttr == -1) {
						nlpFileOp.fAnyOperationsAborted=TRUE;
						retCode=0x10004;
						where = 220;
						break;
					}
					recurseinto = TRUE;
				}
				if (ToTyp  != TypisNewPath) {
					if (FromTyp == TypisDir || FromTyp == TypisRoot) {
						strcpy(pFromFile,"\\*.*");
						pTempFrom[strlen(pTempFrom)+1] = '\0';
						recurseinto = TRUE;
					} else {
						if (FromTyp == TypisFile) {
							if (TargetisDir) { 
								recurseinto = TRUE;
							} else {
								if (CopyFileA(pTempFrom, pTempTo, FALSE)) {
									if (FuncSwitch == FO_COPY) continue;
									if (DeleteFileA(pTempFrom)) continue;
								}
								nlpFileOp.fAnyOperationsAborted=TRUE;
								where = 222;
								break;
							}
						} /* endif */
					}
				}
				if (recurseinto) {
					TRACE(__FUNCTION__" %s level=%d Entering into Directory '%s'\n", pToFuncTXT, level, pTempTo);
					TempretCode = SHFileOperationA (&nlpFileOp);
					if (nlpFileOp.fAnyOperationsAborted) {where = 223;break;}
					if (FuncSwitch == FO_MOVE && FromTyp == TypisDir) {
						if (!(RemoveDirectoryA(pFrom))) {
							nlpFileOp.fAnyOperationsAborted=TRUE;
							where = 100;
							break;
						}
					}
					continue; /* next pTo/pFrom */
				}
       			if (FromTyp == TypisMask) {
					hFind = FindFirstFileA(pTempFrom, &wfd);
					if (INVALID_HANDLE_VALUE == hFind) {
						nlpFileOp.fAnyOperationsAborted=TRUE;
						retCode=0x79;
						where = 224;
						break;
					}

					TRACE(__FUNCTION__" %s level=%d between Subdir %s -> %s'\n", pToFuncTXT, level, nlpFileOp.pFrom, nlpFileOp.pTo);
					nlpFileOp.fFlags = (nlpFileOp.fFlags & (-1 - (FOF_MULTIDESTFILES)));

					do {
						TRACE(__FUNCTION__" %s level=%d find '%s'\n", pToFuncTXT, level, wfd.cFileName);
						if (0==strcmp(wfd.cFileName,".")) continue;
						if (0==strcmp(wfd.cFileName,"..")) continue;
						if ((nlpFileOp.fFlags & FOF_FILESONLY) && (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)) {
							continue; /* next name in pFrom(dir) */
						} /* endif */

						strcpy(&pFromFile[1],wfd.cFileName);
						pTempFrom[strlen(pTempFrom)+1]='\0';

						TempretCode = SHFileOperationA (&nlpFileOp);

						if (nlpFileOp.fAnyOperationsAborted) {where = 230;break;}

					} while(FindNextFileA(hFind, &wfd));
				}
			}
			break;
#else
                int multifrom = pFrom[strlen(pFrom) + 1] != '\0';
                int destisdir = PathIsDirectoryA( pTo );
                int todir = 0;

                if (lpFileOp->wFunc == FO_COPY)
                    TRACE("File Copy:\n");
                else
                    TRACE("File Move:\n");

                if( destisdir ) {
                    if ( !((lpFileOp->fFlags & FOF_MULTIDESTFILES) && !multifrom))
                        todir = 1;
                } else {
                    if ( !(lpFileOp->fFlags & FOF_MULTIDESTFILES) && multifrom)
                        todir = 1;
                }

                if ((pTo[strlen(pTo) + 1] != '\0') &&
                    !(lpFileOp->fFlags & FOF_MULTIDESTFILES))
                {
                    WARN("Attempt to use multiple file names as a destination "
                         "without specifying FOF_MULTIDESTFILES\n");
                    return 1;
                }

                if ((lpFileOp->fFlags & FOF_MULTIDESTFILES) &&
                    !SHELL_FileNamesMatch(pTo, pFrom))
                {
                    WARN("Attempt to use multiple file names as a destination "
                         "with mismatching number of files in the source and "
                         "destination lists\n");
                    return 1;
                }

                if ( todir ) {
                    char szTempFrom[MAX_PATH];
                    char *fromfile;
                    int lenPTo;
                    if ( ! destisdir) {
                        TRACE("   creating directory %s\n",pTo);
                        SHCreateDirectory(NULL,pTo);
                    }
                    lenPTo = strlen(pTo);
                    while(1) {
		        HANDLE hFind;
		        WIN32_FIND_DATAA wfd;

                        if(!pFrom[0]) break;
                        TRACE("   From Pattern='%s'\n", pFrom);
			if(INVALID_HANDLE_VALUE != (hFind = FindFirstFileA(pFrom, &wfd)))
			{
			  do
			  {
			    if(strcasecmp(wfd.cFileName, ".") && strcasecmp(wfd.cFileName, ".."))
			    {
			      strcpy(szTempFrom, pFrom);

                              pTempTo = HeapAlloc(GetProcessHeap(), 0,
                                                  lenPTo + strlen(wfd.cFileName) + 5);
                              if (pTempTo) {
                                  strcpy(pTempTo,pTo);
                                  PathAddBackslashA(pTempTo);
                                  strcat(pTempTo,wfd.cFileName);

                                  fromfile = PathFindFileNameA(szTempFrom);
                                  fromfile[0] = '\0';
                                  PathAddBackslashA(szTempFrom);
                                  strcat(szTempFrom, wfd.cFileName);
                                  TRACE("   From='%s' To='%s'\n", szTempFrom, pTempTo);
                                  if(lpFileOp->wFunc == FO_COPY)
                                  {
                                      if(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)
                                      {
                                          /* copy recursively */
                                          if(!(lpFileOp->fFlags & FOF_FILESONLY))
                                          {
                                              SHFILEOPSTRUCTA shfo;

                                              SHCreateDirectory(NULL,pTempTo);
                                              PathAddBackslashA(szTempFrom);
                                              strcat(szTempFrom, "*.*");
                                              szTempFrom[strlen(szTempFrom) + 1] = '\0';
                                              pTempTo[strlen(pTempTo) + 1] = '\0';
                                              memcpy(&shfo, lpFileOp, sizeof(shfo));
                                              shfo.pFrom = szTempFrom;
                                              shfo.pTo = pTempTo;
                                              SHFileOperationA(&shfo);

                                              szTempFrom[strlen(szTempFrom) - 4] = '\0';
                                          }
                                      }
                                      else
                                          CopyFileA(szTempFrom, pTempTo, FALSE);
                                  }
                                  else
                                  {
                                      /* move file/directory */
                                      MoveFileA(szTempFrom, pTempTo);
                                  }
                                  HeapFree(GetProcessHeap(), 0, pTempTo);
                              }
			    }
			  } while(FindNextFileA(hFind, &wfd));
			  FindClose(hFind);
                        }
                        else
                        {
                            /* can't find file with specified name */
                            break;
                        }
                        pFrom += strlen(pFrom) + 1;
                    }
                } else {
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
                            if (lpFileOp->wFunc == FO_COPY)
                        CopyFileA(pFrom, pTo, FALSE);
                            else
                                MoveFileA(pFrom, pTo);

			pFrom += strlen(pFrom) + 1;
			pTo += strlen(pTo) + 1;
		}
                }
		TRACE("Setting AnyOpsAborted=FALSE\n");
		lpFileOp->fAnyOperationsAborted=FALSE;
		return 0;
#endif
        }

	case FO_DELETE:
#ifdef __WIN32OS2__
		TRACE(__FUNCTION__" FO_DELETE level=%d\n",level);
// need break at error before change sourcepointer
		while(!nlpFileOp.fAnyOperationsAborted && (pFrom+=lenFrom+1)[0]) {
				lenFrom=strlen(pFrom);
				FromAttr = GetFileAttributesA(pFrom);
				if (!(FromAttr & FILE_ATTRIBUTE_DIRECTORY)) {
					TRACE(__FUNCTION__" FO_DELETE level=%d File='%s'\n",level , pFrom);
					if (DeleteFileA(pFrom)) continue;
					nlpFileOp.fAnyOperationsAborted=TRUE;
//					retCode = 0x71;
					where = 301;
					break;
				}
				if (!(pTempFrom)) pTempFrom = HeapAlloc(GetProcessHeap(), 0, MAX_PATH+2);
				strcpy(pTempFrom,pFrom);
				PathRemoveBackslashA(pTempFrom);
				FromAttr = GetFileAttributesA(pTempFrom);
				if (!(FromAttr & FILE_ATTRIBUTE_DIRECTORY) ) {
					nlpFileOp.fAnyOperationsAborted=TRUE;
//					retCode = 0x71;
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

					TRACE(__FUNCTION__" FO_DELETE level=%d Entering Directory '%s'\n",level , nlpFileOp.pFrom);
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

				nlpFileOp.fFlags = (nlpFileOp.fFlags & (-1 - (FOF_MULTIDESTFILES)));

				TRACE(__FUNCTION__" FO_DELETE level=%d Delete in Subdir %s'\n",level , nlpFileOp.pFrom);

				do {
					TRACE(__FUNCTION__" FO_DELETE level=%d find '%s'\n",level , wfd.cFileName);
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
	case 0:
			break;
#else
	{
		HANDLE		hFind;
		WIN32_FIND_DATAA wfd;
		char		szTemp[MAX_PATH];
		char		*file_name;

		TRACE("File Delete:\n");
		while(1) {
			if(!pFrom[0]) break;
			TRACE("   Pattern='%s'\n", pFrom);
			if(INVALID_HANDLE_VALUE != (hFind = FindFirstFileA(pFrom, &wfd)))
			{
			  do
			  {
			    if(strcasecmp(wfd.cFileName, ".") && strcasecmp(wfd.cFileName, ".."))
			    {
			      strcpy(szTemp, pFrom);
			      file_name = PathFindFileNameA(szTemp);
			      file_name[0] = '\0';
			      PathAddBackslashA(szTemp);
			      strcat(szTemp, wfd.cFileName);

			      TRACE("   File='%s'\n", szTemp);
			      if(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)
			      {
			        if(!(lpFileOp->fFlags & FOF_FILESONLY))
			            SHELL_DeleteDirectoryA(szTemp, FALSE);
			      }
			      else
			        DeleteFileA(szTemp);
			    }
			  } while(FindNextFileA(hFind, &wfd));

			  FindClose(hFind);
			}
			pFrom += strlen(pFrom) + 1;
		}
		TRACE("Setting AnyOpsAborted=FALSE\n");
		lpFileOp->fAnyOperationsAborted=FALSE;
		return 0;
	}
#endif
        case FO_RENAME:
            TRACE("File Rename:\n");
            if (pFrom[strlen(pFrom) + 1] != '\0')
            {
                WARN("Attempt to rename more than one file\n");
                return 1;
            }
            lpFileOp->fAnyOperationsAborted = FALSE;
            TRACE("From %s, To %s\n", pFrom, pTo);
            return !MoveFileA(pFrom, pTo);

	default:
#ifdef __WIN32OS2__
		TRACE(__FUNCTION__" Unhandled shell file operation %d at level=%d stub\n",(lpFileOp->wFunc & 15), level );
		lpFileOp->fAnyOperationsAborted=TRUE;
#else
		FIXME("Unhandled shell file operation %d\n", lpFileOp->wFunc);
	}
#endif
	   return 1;

#ifdef __WIN32OS2__
	}
	if (pTempFrom) HeapFree(GetProcessHeap(), 0, pTempFrom);

	if (nlpFileOp.fAnyOperationsAborted) {
		lpFileOp->fAnyOperationsAborted=TRUE;
		if (TempretCode > 0 /* retCode */) {
			retCode = TempretCode;
		} /* endif */
	}
	if (lpFileOp->fAnyOperationsAborted==TRUE) {
		if (FO_DELETE == (lpFileOp->wFunc & 15)) {
			TRACE(__FUNCTION__" Setting AnyOpsAborted=TRUE level=%d ret=0x%x, at=%i with %s\n",level, retCode,where,pFrom);
		} else {
			TRACE(__FUNCTION__" Setting AnyOpsAborted=TRUE level=%d ret=0x%x, at=%i with %s -> %s\n",level, retCode,where,pFrom,pTo);
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


