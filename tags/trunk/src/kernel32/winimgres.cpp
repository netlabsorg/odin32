/* $Id: winimgres.cpp,v 1.51 2001-04-04 10:12:05 sandervl Exp $ */

/*
 * Win32 PE Image class (resource methods)
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Language order based on Wine Code (loader\pe_resource.c)
 * Copyright 1995 Thomas Sandford
 * Copyright 1996 Martin von Loewis
 * 
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Check created resource objects before loading the resource!
 * TODO: Support for 'DIB' resource type (VPBuddy)
 *
 */
#include <os2win.h>
#include <winnls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <winimagebase.h>
#include <unicode.h>
#include <heapstring.h>
#include "pefile.h"
#include "oslibmisc.h"

#define DBG_LOCALLOG	DBG_winimgres
#include "dbglocal.h"

#define MAX_RES 17
typedef struct {
  char *typename;
  int   namelen;
} STD_RESTYPE;

STD_RESTYPE ResType[MAX_RES] =
      { {NULL,          0}, 
        {"CURSOR",      6}, 
        {"BITMAP",      6},
        {"ICON",        4},
        {"MENU",        4},
        {"DIALOG",      6},
        {"STRING",      6},
        {"FONTDIR",     7},
        {"FONT",        4},
        {"ACCELERATOR", 11},
        {"RCDATA",      6},
        {"MESSAGETABLE",12},
        {"GROUP_CURSOR",12},
        {NULL,          0},
        {"GROUP_ICON",  10},
        {NULL,          0},
        {"VERSION",     7}
      };

//SvL: VPBuddy bugfix, seems to load bitmaps with type name 'DIB'
#define BITMAP_TYPENAME2    "DIB"

//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getResourceSizeA(LPSTR lpszName, LPSTR lpszType, ULONG lang)
{
 PIMAGE_RESOURCE_DATA_ENTRY pData = NULL;
   
    pData = (PIMAGE_RESOURCE_DATA_ENTRY)findResourceA(lpszName, lpszType, lang);
    if(pData == NULL) {
        dprintf(("Win32ImageBase::getPEResourceSizeA: couldn't find resource %x (type %x, lang %x)", lpszName, lpszType, lang));
        return 0;
    }
    return pData->Size;
}
//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getResourceSizeW(LPWSTR lpszName, LPWSTR lpszType, ULONG lang)
{
 PIMAGE_RESOURCE_DATA_ENTRY pData = NULL;
   
    pData = (PIMAGE_RESOURCE_DATA_ENTRY)findResourceW(lpszName, lpszType, lang);
    if(pData == NULL) {
        dprintf(("Win32ImageBase::getResourceSizeW: couldn't find resource %x (type %x, lang %x)", lpszName, lpszType, lang));
        return 0;
    }
    return pData->Size;
}
//******************************************************************************
//Returns pointer to data of resource handle
//******************************************************************************
char *Win32ImageBase::getResourceAddr(HRSRC hResource)
{
 PIMAGE_RESOURCE_DATA_ENTRY pData = (PIMAGE_RESOURCE_DATA_ENTRY)hResource;

  if(pData == NULL) {
	DebugInt3();
	return NULL;
  }
  //ulRVAResourceSection contains the relative virtual address (relative to the start of the image)
  //for the resource section (images loaded by the pe.exe and pe2lx/win32k)
  //For LX images, this is 0 as OffsetToData contains a relative offset
  return (char *)((char *)pResRootDir + (pData->OffsetToData - ulRVAResourceSection));
}
//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getResourceSize(HRSRC hResource)
{
 PIMAGE_RESOURCE_DATA_ENTRY pData = (PIMAGE_RESOURCE_DATA_ENTRY)hResource;

  if(pData == NULL) {
	DebugInt3();
	return NULL;
  }
  return pData->Size;
}
//******************************************************************************
//findResource returns the pointer of the resource's IMAGE_RESOURCE_DATA_ENTRY structure
//******************************************************************************
HRSRC Win32ImageBase::findResourceA(LPCSTR lpszName, LPSTR lpszType, ULONG lang)
{
 PIMAGE_RESOURCE_DIRECTORY pResDirRet;
 int                       typelen;
 HRSRC                     hRes;

    if(HIWORD(lpszType) != 0) 
    {
	typelen = strlen(lpszType);

        for(int i=0;i<MAX_RES;i++) {
            if(ResType[i].namelen && 
               ResType[i].namelen == typelen &&
               stricmp(lpszType, ResType[i].typename) == 0)
                    break;
        }
        if(i != MAX_RES) {//standard res type
             lpszType = (LPSTR)i;
        }
    }
    pResDirRet = getResSubDirA(pResRootDir, lpszType);
    if(!pResDirRet) {
        dprintf2(("FindResourceA %s: resource %x (type %x, lang %x) TYPE NOT FOUND", szModule, lpszName, lpszType, lang));
	SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
	return NULL;
    }
    pResDirRet = getResSubDirA(pResDirRet, lpszName);
    if(!pResDirRet) {
        dprintf2(("FindResourceA %s: resource %x (type %x, lang %x) NAME NOT FOUND", szModule, lpszName, lpszType, lang));
	SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
	return NULL;
    }

    /* Here is the real difference between FindResource and FindResourceEx */
    if(lang == MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) ||
       lang == MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) ||
       lang == MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT) ||
       lang == MAKELANGID(LANG_NEUTRAL, 3)) /* FIXME: real name? */
    {
      	 hRes = getResourceLang(pResDirRet);
    }
    else hRes = getResourceLangEx(pResDirRet, lang);

    if((ULONG)lpszName != ID_GETFIRST && HIWORD(lpszName)) {
            dprintf(("FindResourceA %s: resource %s (type %x, lang %x)", szModule, lpszName, lpszType, lang));
    }
    else    dprintf(("FindResourceA %s: resource %x (type %x, lang %x)", szModule, lpszName, lpszType, lang));

    SetLastError(ERROR_SUCCESS);
    return hRes;
}
//******************************************************************************
//******************************************************************************
HRSRC Win32ImageBase::findResourceW(LPWSTR lpszName, LPWSTR lpszType, ULONG lang)
{
 PIMAGE_RESOURCE_DIRECTORY pResDirRet;
 char                     *astring1 = NULL;
 int                       typelen;
 HRSRC                     hRes;

    if(HIWORD(lpszType) != 0 && lpszType[0] != (WCHAR)'#') 
    {
        astring1 = UnicodeToAsciiString(lpszType);
	typelen = strlen(astring1);

        for(int i=0;i<MAX_RES;i++) {
            if(ResType[i].namelen && 
               ResType[i].namelen == typelen &&
               stricmp(astring1, ResType[i].typename) == 0)
                    break;
        }
        if(i != MAX_RES) {//standard res type
             lpszType = (LPWSTR)i;
        }
	FreeAsciiString(astring1);
    }
    pResDirRet = getResSubDirW(pResRootDir, lpszType);
    if(!pResDirRet) {
        dprintf2(("FindResourceW %s: resource %x (type %x, lang %x) TYPE NOT FOUND", szModule, lpszName, lpszType, lang));
	SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
	return NULL;
    }
    pResDirRet = getResSubDirW(pResDirRet, lpszName);
    if(!pResDirRet) {
        dprintf2(("FindResourceW %s: resource %x (type %x, lang %x) NAME NOT FOUND", szModule, lpszName, lpszType, lang));
	SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
	return NULL;
    }

    /* Here is the real difference between FindResource and FindResourceEx */
    if(lang == MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) ||
       lang == MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) ||
       lang == MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT) ||
       lang == MAKELANGID(LANG_NEUTRAL, 3)) /* FIXME: real name? */
    {
      	 hRes = getResourceLang(pResDirRet);
    }
    else hRes = getResourceLangEx(pResDirRet, lang);

    if((ULONG)lpszName != ID_GETFIRST && HIWORD(lpszName)) {
            dprintf(("FindResourceW %s: resource %ls (type %x, lang %x)", szModule, lpszName, lpszType, lang));
    }
    else    dprintf(("FindResourceW %s: resource %x (type %x, lang %x)", szModule, lpszName, lpszType, lang));

    SetLastError(ERROR_SUCCESS);
    return hRes;
}
//******************************************************************************
//According to Wine:
/* FindResourceA/W does search in the following order:
 * 1. Neutral language with neutral sublanguage
 * 2. Neutral language with default sublanguage
 * 3. Current locale lang id
 * 4. Current locale lang id with neutral sublanguage
 * 5. (!) LANG_ENGLISH, SUBLANG_DEFAULT
 * 6. Return first in the list
 */
//******************************************************************************
HRSRC Win32ImageBase::getResourceLang(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch)
{
 HRSRC  hRes;
 DWORD lang;

    /* 1. Neutral language with neutral sublanguage */
    lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
    hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    if(!hRes) {
	/* 2. Neutral language with default sublanguage */
	lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
        hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    }
    if(!hRes) {
	/* 3. Current locale lang id */
	lang = LANGIDFROMLCID(GetUserDefaultLCID());
        hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    }
    if(!hRes) {
	/* 4. Current locale lang id with neutral sublanguage */
	lang = MAKELANGID(PRIMARYLANGID(lang), SUBLANG_NEUTRAL);
        hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    }
    if(!hRes) {
	/* 5. (!) LANG_ENGLISH, SUBLANG_DEFAULT */
	lang = MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);
        hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    }
    if(!hRes) {
	/* 6. Return first in the list */
        hRes = (HRSRC)getResDataLang(pResDirToSearch, lang, TRUE);
    }
    return hRes;
}
//******************************************************************************
//According to Wine:
/* FindResourceExA/W does search in the following order:
 * 1. Exact specified language
 * 2. Language with neutral sublanguage
 * 3. Neutral language with neutral sublanguage
 * 4. Neutral language with default sublanguage
 */
//******************************************************************************
HRSRC Win32ImageBase::getResourceLangEx(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch,
                                        DWORD lang)
{
 HRSRC  hRes;

    /* 1. Exact specified language */
    hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    if(!hRes) {
	/* 2. Language with neutral sublanguage */
        lang = MAKELANGID(PRIMARYLANGID(lang), SUBLANG_NEUTRAL);
        hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    }
    if(!hRes) {
	/* 3. Neutral language with neutral sublanguage */
        lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
        hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    }
    if(!hRes) {
	/* 4. Current locale lang id with neutral sublanguage */
	lang = MAKELANGID(PRIMARYLANGID(lang), SUBLANG_NEUTRAL);
        hRes = (HRSRC)getResDataLang(pResDirToSearch, lang);
    }
    return hRes;
}
//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getVersionSize()
{
    return getResourceSizeW((LPWSTR)1, (LPWSTR)NTRT_VERSION);
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::getVersionStruct(char *verstruct, ULONG bufLength)
{
 HRSRC hRes;

    if(verstruct == NULL || bufLength == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    hRes = findResourceW((LPWSTR)ID_GETFIRST, (LPWSTR)NTRT_VERSION, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    if(hRes == NULL) {
	//last error already set by findResourceW
        dprintf(("Win32PeLdrImage::getVersionStruct: couldn't find version resource!"));
        return 0;
    }
    memcpy(verstruct, getResourceAddr(hRes), min(bufLength, getResourceSize(hRes)));
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}

/**
 * This function finds a resource (sub)directory within a given resource directory.
 * @returns   Pointer to resource directory. NULL if not found or not a directory.
 * @param     pResDirToSearch  Pointer to resource directory to search. (any level)
 * @param     lpszName         Resource ID string.
 * @sketch
 * @status    completely implemented
 * @author    knut st. osmundsen
 */
PIMAGE_RESOURCE_DIRECTORY Win32ImageBase::getResSubDirW(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch, 
                                                        LPCWSTR lpszName)
{
    PIMAGE_RESOURCE_DIRECTORY_ENTRY paResDirEntries;
    int     i;
    int     idName = -1;

    if(pResDirToSearch == NULL) {
	return NULL;
    }

    if((ULONG)lpszName == -1) {//check for nonsense values
	return NULL;
    }

    /* lpszName */
    if ((ULONG)lpszName != ID_GETFIRST && HIWORD(lpszName) != 0)
    {
        if (lpszName[0] == '#')
        {
            char szBuf[10];
            lstrcpynWtoA(szBuf, (WCHAR*)(lpszName + 1), sizeof(szBuf));
            idName = atoi(szBuf);
        }
    }
    else
        idName = (int)lpszName;

    paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDirToSearch + sizeof(*pResDirToSearch));
    if(idName == ID_GETFIRST) {
        return paResDirEntries[0].u2.s.DataIsDirectory ?
               (PIMAGE_RESOURCE_DIRECTORY) (paResDirEntries[0].u2.s.OffsetToDirectory + (ULONG)pResRootDir)
               : NULL;
    }

    if (idName != -1)
    {   /* idName */
        paResDirEntries += pResDirToSearch->NumberOfNamedEntries;

        for (i = 0; i < pResDirToSearch->NumberOfIdEntries; i++)
            if (paResDirEntries[i].u1.Id == (WORD)idName)
                return paResDirEntries[i].u2.s.DataIsDirectory ?
                    (PIMAGE_RESOURCE_DIRECTORY) (paResDirEntries[i].u2.s.OffsetToDirectory + (ULONG)pResRootDir /*?*/
                                                 /*- ulRVAResourceSection*/)
                    : NULL;
    }
    else
    {   /* string name */
        int cusName = lstrlenW(lpszName);

        for (i = 0; i < pResDirToSearch->NumberOfNamedEntries; i++)
        {
            PIMAGE_RESOURCE_DIR_STRING_U pResDirStr =
                (PIMAGE_RESOURCE_DIR_STRING_U)(paResDirEntries[i].u1.s.NameOffset + (ULONG)pResRootDir /*?*/);

	    //SvL: Must do case insensitive string compare here
            if (pResDirStr->Length == cusName
                && lstrncmpiW(pResDirStr->NameString, lpszName, cusName) == 0)
            {
                return paResDirEntries[i].u2.s.DataIsDirectory ?
                    (PIMAGE_RESOURCE_DIRECTORY) (paResDirEntries[i].u2.s.OffsetToDirectory + (ULONG)pResRootDir
                                                 /*- ulRVAResourceSection*/)
                    : NULL;
            }
        }
    }

    return NULL;
}

/**
 * This function finds a resource (sub)directory within a given resource directory.
 * @returns   Pointer to resource directory. NULL if not found or not a directory.
 * @param     pResDirToSearch  Pointer to resource directory to search. (any level)
 * @param     lpszName         Resource ID string.
 * @sketch
 * @status    completely implemented
 * @author    knut st. osmundsen
 */
PIMAGE_RESOURCE_DIRECTORY Win32ImageBase::getResSubDirA(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch, 
                                                        LPCTSTR lpszName)
{
    PIMAGE_RESOURCE_DIRECTORY   pResDirRet;
    LPCWSTR                     lpszwName;

    if(pResDirToSearch == NULL) {
	return NULL;
    }

    if((ULONG)lpszName == -1) {//check for nonsense values
	return NULL;
    }

    /* lpszName */
    if ((ULONG)lpszName != ID_GETFIRST && HIWORD(lpszName) != 0)
    {
        lpszwName = AsciiToUnicodeString((char*)lpszName);
        if (lpszwName == NULL)
            return NULL;
    }
    else
        lpszwName = (LPWSTR)lpszName;

    pResDirRet = getResSubDirW(pResDirToSearch, lpszwName);

    if ((ULONG)lpszName != ID_GETFIRST && HIWORD(lpszwName) != 0)
        FreeAsciiString((void*)lpszwName);

    return pResDirRet;
}
//******************************************************************************
/**
 * This function finds a resource data entry within a given resource directory.
 * @returns   Pointer to resource data entry. NULL if not found
 * @param     pResDirToSearch  Pointer to resource directory to search. (lang level)
 * @param     language         Resource language id
 * @param     fGetDefault      TRUE -> get first available resource if not found
 * @sketch
 * @status    completely implemented
 * @author    Sander van Leeuwen
 */
//******************************************************************************
PIMAGE_RESOURCE_DATA_ENTRY 
     Win32ImageBase::getResDataLang(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch,
                                    ULONG language, BOOL fGetDefault)
{
    PIMAGE_RESOURCE_DIRECTORY_ENTRY paResDirEntries;
    int     i;

    if(pResDirToSearch == NULL) {
	return NULL;
    }

    paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDirToSearch + sizeof(*pResDirToSearch));

    if(pResDirToSearch->NumberOfNamedEntries) {
	DebugInt3();
    }
    paResDirEntries += pResDirToSearch->NumberOfNamedEntries;

    for (i = 0; i < pResDirToSearch->NumberOfIdEntries; i++) {
    	if (paResDirEntries[i].u1.Id == language) 
        {
               	return (PIMAGE_RESOURCE_DATA_ENTRY) (paResDirEntries[i].u2.s.OffsetToDirectory + (ULONG)pResRootDir);
	}
    }

    // if nothing found and fGetDefault is true, return first entry
    if(fGetDefault)
    {
    	return (PIMAGE_RESOURCE_DATA_ENTRY) (paResDirEntries[0].u2.s.OffsetToDirectory + (ULONG)pResRootDir);
    }
    return NULL;
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::enumResourceTypesA(HMODULE hmod, ENUMRESTYPEPROCA lpEnumFunc, 
                                        LONG lParam)
{
 PIMAGE_RESOURCE_DIRECTORY       prdType;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
 PIMAGE_RESOURCE_DIR_STRING_U    pstring;
 ULONG  i, nameOffset;
 BOOL   fRet;

    if (pResRootDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    if ((ULONG)lpEnumFunc < 0x10000 || (ULONG)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }

    //reminder:
    //1st level -> types
    //2nd level -> names
    //3rd level -> language

    /* set pointer to first resource type entry */
    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResRootDir + sizeof(IMAGE_RESOURCE_DIRECTORY));

    for (i=0; i<pResRootDir->NumberOfNamedEntries+pResRootDir->NumberOfIdEntries && fRet; i++) 
    {
      	/* locate directory or each resource type */
    	prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)pResRootDir + (int)prde->u2.OffsetToData);

        if (prde->u1.s.NameIsString)
    	{//name or id entry?
	        //SvL: 30-10-'97, high bit is set, so clear to get real offset
	        nameOffset = prde->u1.Name & ~0x80000000;
	
	        pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((ULONG)pResRootDir + nameOffset);
	        char *typename = (char *)malloc(pstring->Length+1);
	        lstrcpynWtoA(typename, pstring->NameString, pstring->Length+1);
	        typename[pstring->Length] = 0;

		fRet = lpEnumFunc(hmod, typename, lParam);
        	free(typename);
    	}
    	else {
		fRet = lpEnumFunc(hmod, (LPSTR)prde->u1.Id, lParam);
	}

        /* increment to next entry */
        prde++;
    }
    return fRet > 0 ? TRUE : FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::enumResourceTypesW(HMODULE hmod, ENUMRESTYPEPROCW lpEnumFunc, 
                                        LONG lParam)
{
 PIMAGE_RESOURCE_DIRECTORY       prdType;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
 PIMAGE_RESOURCE_DIR_STRING_U    pstring;
 ULONG  i, nameOffset;
 BOOL   fRet;
 LPWSTR lpszType;

    if (pResRootDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    if ((ULONG)lpEnumFunc < 0x10000 || (ULONG)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }

    //reminder:
    //1st level -> types
    //2nd level -> names
    //3rd level -> language

    /* set pointer to first resource type entry */
    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResRootDir + sizeof(IMAGE_RESOURCE_DIRECTORY));

    for (i=0; i<pResRootDir->NumberOfNamedEntries+pResRootDir->NumberOfIdEntries && fRet; i++) 
    {
      	/* locate directory or each resource type */
    	prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)pResRootDir + (int)prde->u2.OffsetToData);

        if (prde->u1.s.NameIsString)
    	{//name or id entry?
	        //SvL: 30-10-'97, high bit is set, so clear to get real offset
	        nameOffset = prde->u1.Name & ~0x80000000;
	
	        pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((ULONG)pResRootDir + nameOffset);

		lpszType = (LPWSTR)malloc((pstring->Length+1) * sizeof(WCHAR));
                memcpy(lpszType, pstring->NameString, pstring->Length * sizeof(WCHAR));
                lpszType[pstring->Length] = 0;

		fRet = lpEnumFunc(hmod, pstring->NameString, lParam);

		free(lpszType);
    	}
    	else 	fRet = lpEnumFunc(hmod, (LPWSTR)prde->u1.Id, lParam);

        /* increment to next entry */
        prde++;
    }
    return fRet > 0 ? TRUE : FALSE;
}
/**
 * The EnumResourceNames function searches a module for each
 * resource of the specified type and passes the name of each
 * resource it locates to an application-defined callback function
 *
 * @returns   If the function succeeds, the return value is nonzero.
 *            If the function fails, the return value is zero
 * @param     hmod          The specified module handle.
 * @param     lpszType      pointer to resource type
 * @param     lpEnumFunc    pointer to callback function
 * @param     lParam        application-defined parameter
 * @sketch    IF not resources in module THEN return fail.
 *            Validate parameters.
 *            Get the subdirectory for the specified type.
 *            IF found THEN
 *            BEGIN
 *                Find the number of directory entries.
 *                Find directory entries.
 *                LOOP thru all entries while the callback function returns true
 *                BEGIN
 *                    Name = pointer to ASCII name string or Id.
 *                    call callback function.
 *                END
 *            END
 *            ELSE
 *                fail.
 *            return
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 * @remark    The EnumResourceNames function continues to enumerate resource
 *            names until the callback function returns FALSE or all resource
 *            names have been enumerated
 */
BOOL Win32ImageBase::enumResourceNamesA(HMODULE hmod,
                                        LPCTSTR  lpszType,
                                        ENUMRESNAMEPROCA lpEnumFunc,
                                        LONG lParam)
{
 BOOL                            fRet;
 PIMAGE_RESOURCE_DIRECTORY       pResDirOurType;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY paResDirEntries;

    if (pResRootDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    /* validate parameters - FIXME... Exception handler??? */
    if ((ULONG)lpszType >= 0xc0000000) //....
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if ((ULONG)lpEnumFunc < 0x10000 || (ULONG)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }

    //reminder:
    //1st level -> types
    //2nd level -> names
    //3rd level -> language

    pResDirOurType = getResSubDirA(pResRootDir, lpszType);
    if (pResDirOurType != NULL)
    {
        char     *pszASCII = NULL;
        unsigned  cch = 0;
        unsigned  cResEntries;
        unsigned  i;

        fRet = TRUE;
        cResEntries = pResDirOurType->NumberOfNamedEntries + pResDirOurType->NumberOfIdEntries;
        paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDirOurType + sizeof(*pResDirOurType));
        for (i = 0; i < cResEntries && fRet; i++)
        {
            LPSTR lpszName;

            if (paResDirEntries[i].u1.s.NameIsString)
            {
                PIMAGE_RESOURCE_DIR_STRING_U pResDirString =
                    (PIMAGE_RESOURCE_DIR_STRING_U)(paResDirEntries[i].u1.s.NameOffset + (ULONG)pResRootDir);

                /* ASCII buffer allocation/adjustment? */
                if (cch <= pResDirString->Length)
                {
                    void *pszTmp;
                    cch = max(pResDirString->Length + 1, 32);
                    pszTmp = pszASCII != NULL ? realloc(pszASCII, cch) : malloc(cch);
                    if (pszTmp == NULL)
                    {
                        fRet = FALSE;
                        break;
                    }
                    pszASCII = (char*)pszTmp;
                }
		lstrcpynWtoA(pszASCII, pResDirString->NameString, pResDirString->Length+1);
		pszASCII[pResDirString->Length] = 0;
                lpszName = pszASCII;
            }
            else
                lpszName = (LPSTR)paResDirEntries[i].u1.Id;

            fRet = lpEnumFunc(hmod, lpszType, lpszName, lParam);
        }

        if (pszASCII != NULL)
            free(pszASCII);
    }
    else
    {
        SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
        fRet = FALSE;
    }

    return fRet > 0 ? TRUE : FALSE;
}


/**
 * The EnumResourceNames function searches a module for each
 * resource of the specified type and passes the name of each
 * resource it locates to an application-defined callback function
 *
 * @returns   If the function succeeds, the return value is nonzero.
 *            If the function fails, the return value is zero
 * @param     hmod          The specified module handle.
 * @param     lpszType      pointer to resource type
 * @param     lpEnumFunc    pointer to callback function
 * @param     lParam        application-defined parameter
 * @sketch    IF not resources in module THEN return fail.
 *            Validate parameters.
 *            Get the subdirectory for the specified type.
 *            IF found THEN
 *            BEGIN
 *                Find the number of directory entries.
 *                Find directory entries.
 *                LOOP thru all entries while the callback function returns true
 *                BEGIN
 *                    Name = pointer to ASCII name string or Id.
 *                    call callback function.
 *                END
 *            END
 *            ELSE
 *                fail.
 *            return
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 * @remark    The EnumResourceNames function continues to enumerate resource
 *            names until the callback function returns FALSE or all resource
 *            names have been enumerated
 */
BOOL Win32ImageBase::enumResourceNamesW(HMODULE hmod,
                                        LPCWSTR  lpszType,
                                        ENUMRESNAMEPROCW lpEnumFunc,
                                        LONG lParam)
{
 BOOL                            fRet;
 PIMAGE_RESOURCE_DIRECTORY       pResDirOurType;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY paResDirEntries;

    if (pResRootDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    /* validate parameters - FIXME... Exception handler??? */
    if ((ULONG)lpszType >= 0xc0000000) //....
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if ((ULONG)lpEnumFunc < 0x10000 || (ULONG)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }


    //reminder:
    //1st level -> types
    //2nd level -> names
    //3rd level -> language

    pResDirOurType = getResSubDirW(pResRootDir, lpszType);
    if (pResDirOurType != NULL)
    {
        unsigned  cResEntries;
        unsigned  i, length;

        fRet = TRUE;
        cResEntries = pResDirOurType->NumberOfNamedEntries + pResDirOurType->NumberOfIdEntries;
        paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDirOurType + sizeof(*pResDirOurType));
        for (i = 0; i < cResEntries && fRet; i++)
        {
            LPWSTR lpszName, lpszResName;

            if(paResDirEntries[i].u1.s.NameIsString) 
            {
                lpszName = (LPWSTR)(paResDirEntries[i].u1.s.NameOffset + (ULONG)pResRootDir + 2);
                length   = (int)*(LPWSTR)(paResDirEntries[i].u1.s.NameOffset + (ULONG)pResRootDir);

       		lpszResName = (LPWSTR)malloc((length+1) * sizeof(WCHAR));
                memcpy(lpszResName, lpszName, length * sizeof(WCHAR));
                lpszResName[length] = 0;

                fRet = lpEnumFunc(hmod, lpszType, lpszResName, lParam);
		free(lpszResName);
            }
            else {
                lpszName = (LPWSTR)paResDirEntries[i].u1.Id;
                fRet = lpEnumFunc(hmod, lpszType, lpszName, lParam);
            }
        }
    }
    else
    {
        SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
        fRet = FALSE;
    }

    return fRet > 0;
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::enumResourceLanguagesA(HMODULE hmod, LPCSTR lpszType, 
                                            LPCSTR lpszName, 
                                            ENUMRESLANGPROCA lpEnumFunc, 
                                            LONG lParam)
{
 PIMAGE_RESOURCE_DIRECTORY pResDirRet;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY paResDirEntries;
 BOOL   fRet;

    if (pResRootDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    if ((ULONG)lpEnumFunc < 0x10000 || (ULONG)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }

    pResDirRet = getResSubDirA(pResRootDir, lpszType);
    if(!pResDirRet) {
	SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
	return FALSE;
    }
    pResDirRet = getResSubDirA(pResDirRet, lpszName);
    if(!pResDirRet) {
	SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
	return FALSE;
    }

    paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDirRet + sizeof(*pResDirRet));
    if(pResDirRet->NumberOfNamedEntries) {
	DebugInt3();
    }
    paResDirEntries += pResDirRet->NumberOfNamedEntries;

    for(int i = 0;i < pResDirRet->NumberOfIdEntries;i++) 
    {
	fRet = lpEnumFunc(hmod, lpszType, lpszName, paResDirEntries[i].u1.Id, lParam);
	if(!fRet)
		break;
    }
    return fRet;
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::enumResourceLanguagesW(HMODULE hmod, LPCWSTR lpszType, 
                                            LPCWSTR lpszName, 
                                            ENUMRESLANGPROCW lpEnumFunc, 
                                            LONG lParam)
{
 PIMAGE_RESOURCE_DIRECTORY pResDirRet;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY paResDirEntries;
 BOOL   fRet;

    if (pResRootDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    if ((ULONG)lpEnumFunc < 0x10000 || (ULONG)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }

    pResDirRet = getResSubDirW(pResRootDir, lpszType);
    if(!pResDirRet) {
	SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
	return FALSE;
    }
    pResDirRet = getResSubDirW(pResDirRet, lpszName);
    if(!pResDirRet) {
	SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
	return FALSE;
    }

    paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDirRet + sizeof(*pResDirRet));
    if(pResDirRet->NumberOfNamedEntries) {
	DebugInt3();
    }
    paResDirEntries += pResDirRet->NumberOfNamedEntries;

    for(int i = 0;i < pResDirRet->NumberOfIdEntries;i++) 
    {
	fRet = lpEnumFunc(hmod, lpszType, lpszName, paResDirEntries[i].u1.Id, lParam);
	if(!fRet)
		break;
    }
    return fRet;
}
//******************************************************************************
//******************************************************************************
