/* $Id: winimgres.cpp,v 1.37 2000-05-22 19:08:00 sandervl Exp $ */

/*
 * Win32 PE Image class (resource methods)
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Check created resource objects before loading the resource!
 * TODO: Once the resource handling in PE2LX/win32k is changed,
 *       getVersionStruct/Size can be moved into the Win32ImageBase class
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
#include <winres.h>
#include <unicode.h>
#include <heapstring.h>
#include "pefile.h"
#include "oslibmisc.h"

#define DBG_LOCALLOG	DBG_winimgres
#include "dbglocal.h"

//SvL: VPBuddy bugfix, seems to load bitmaps with type name 'DIB'
#define BITMAP_TYPENAME2    "DIB"

#define RESERR_SUCCESS          0
#define RESERR_IDNOTFOUND	1
#define RESERR_TYPENOTFOUND     2
#define RESERR_LANGNOTFOUND     3

//******************************************************************************
//Assuming names are case insensitive
//PE spec says names & ids are sorted; keep on searching just to be sure
//******************************************************************************
PIMAGE_RESOURCE_DATA_ENTRY
 Win32ImageBase::getPEResourceEntry(ULONG id, ULONG type, ULONG lang, int *error)
{
 PIMAGE_RESOURCE_DIRECTORY       prdType;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
 PIMAGE_RESOURCE_DIR_STRING_U    pstring;
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;
 ULONG  nodeData[3], i, j, nameOffset;
 BOOL  fFound = FALSE, fNumType;

  *error = RESERR_TYPENOTFOUND;

  //PH: our system LX DLLs might not have a resource segment
  if (pResDir == NULL)
    return NULL;

  /* set pointer to first resource type entry */
  prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDir + sizeof(IMAGE_RESOURCE_DIRECTORY));

  /* loop through all resource directory entry types */
  //1st level -> types
  //2nd level -> names
  //3rd level -> language
  nodeData[0] = id;
  nodeData[1] = lang;
  nodeData[2] = 0xFFFFFFFF;

  fNumType = TRUE;    //assume numeric
  if(HIWORD(type) != 0) {//string id?
    for(i=0;i<MAX_RES;i++) {
        if(stricmp((char *)type, ResTypes[i]) == 0)
                break;
    }
//TODO:
#if 0
    if(stricmp((char *)type, BITMAP_TYPENAME2) == 0) {
        i = (int)RT_BITMAPA;
    }
#endif

    if(i == MAX_RES) {//custom resource type
         fNumType = FALSE;
    }
    else type   = i;
  }

  for (i=0; i<pResDir->NumberOfNamedEntries+pResDir->NumberOfIdEntries; i++) {
    /* locate directory or each resource type */
    prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)pResDir + (int)prde->u2.OffsetToData);

    if(i < pResDir->NumberOfNamedEntries)
    {//name or id entry?
        //SvL: 30-10-'97, high bit is set, so clear to get real offset
        nameOffset = prde->u1.Name & ~0x80000000;

        pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((ULONG)pResDir + nameOffset);
        char *typename = (char *)malloc(pstring->Length+1);
        lstrcpynWtoA(typename, pstring->NameString, pstring->Length+1);
        typename[pstring->Length] = 0;

        if(!fNumType) {
            if(stricmp(typename, (char *)type) == 0) {
                fFound = TRUE;
            }
        }
        else {
            for(j=0;j<MAX_RES;j++) {
                if(stricmp(typename, ResTypes[j]) == 0)
                    break;
            }
            if(j == type) {
                fFound = TRUE;
            }
        }
        free(typename);
    }
    else {
        if(prde->u1.Id == type) {
            fFound = TRUE;
        }
    }
    if(fFound) {
        if((ULONG)prdType & 0x80000000) {//subdirectory?
            pData = ProcessResSubDir(prdType, &nodeData[0], 2, error);
        }
        else {
            pData = (PIMAGE_RESOURCE_DATA_ENTRY)prdType;
            dprintf(("getResource: not a subdir!!\n"));
        }
        break;
    }
    /* increment to next entry */
    prde++;
  }
  if(pData) {
	*error = RESERR_SUCCESS; //found it
  }
  return pData;
}
//******************************************************************************
//level: 2 ids
//       3 languages
//******************************************************************************
PIMAGE_RESOURCE_DATA_ENTRY
    Win32ImageBase::ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType,
                                     ULONG *nodeData, int level, int *error)
{
 PIMAGE_RESOURCE_DIRECTORY       prdType2;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
 PIMAGE_RESOURCE_DIR_STRING_U    pstring;
 PIMAGE_RESOURCE_DATA_ENTRY      pData;
 BOOL  fFound = FALSE, fNumId;
 ULONG nrres, nameOffset;
 char *resname;
 int   i;

  if(*nodeData == 0xFFFFFFFF) {//shouldn't happen!
        dprintf(("ProcessResSubDir: *nodeData == 0xFFFFFFFF!\n"));
	DebugInt3();
        return(NULL);
  }
  prdType = (PIMAGE_RESOURCE_DIRECTORY)((ULONG)prdType & ~0x80000000);
  prde    = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdType + sizeof(IMAGE_RESOURCE_DIRECTORY));

  if(level == 2) {
	*error = RESERR_IDNOTFOUND;
  }
  else {
	*error = RESERR_LANGNOTFOUND;
  }
  //level 2 (id)   -> get first id?
  //level 3 (lang) -> get first language?
  if(*nodeData == IDLANG_GETFIRST) {
    	nrres  = prdType->NumberOfNamedEntries + prdType->NumberOfIdEntries;
    	fNumId = (prdType->NumberOfNamedEntries == 0);
  }
  else {
    	fNumId = HIWORD(*nodeData) == 0;

    	if(fNumId) {//numeric or string id?
        	nrres = prdType->NumberOfIdEntries;
	        prde += prdType->NumberOfNamedEntries;  //skip name entries
    	}
    	else    nrres = prdType->NumberOfNamedEntries;
  }

  for(i=0;i<nrres;i++) {
        /* locate directory or each resource type */
        prdType2 = (PIMAGE_RESOURCE_DIRECTORY)((ULONG)pResDir + (ULONG)prde->u2.OffsetToData);

        if(*nodeData == IDLANG_GETFIRST) {
        	fFound = TRUE; //always take the first one
    	}
    	else
        if(!fNumId) {//name or id entry?
            nameOffset = prde->u1.Name;
            if(prde->u1.s.NameIsString) //unicode directory string /*PLF Sat  97-06-21 22:30:35*/
                    nameOffset &= ~0x80000000;

            pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((ULONG)pResDir + nameOffset);

            resname = (char *)malloc(pstring->Length+1);
            lstrcpynWtoA(resname, pstring->NameString, pstring->Length+1);
            resname[pstring->Length] = 0;
            if(stricmp(resname, (char *)*nodeData) == 0) {
                    fFound = TRUE;
            }
            free(resname);
        }
        else {
            if(*nodeData == prde->u1.Id)
                    fFound = TRUE;
        }

        if(fFound) {
            if((ULONG)prdType2 & 0x80000000) {//subdirectory?
                    return ProcessResSubDir(prdType2, nodeData+1, 3, error);
            }
            else {
                    pData = (PIMAGE_RESOURCE_DATA_ENTRY)prdType2;
                    if(pData->Size) {//winamp17 winzip archive has resource with size 0
                            return(pData);
                    }
                    else    return(NULL);
            }
        }
        prde++;
  }
  return(NULL);
}
//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getPEResourceSize(ULONG id, ULONG type, ULONG lang)
{
 PIMAGE_RESOURCE_DATA_ENTRY pData = NULL;
 int                        error;

    switch(lang) {
    case LOCALE_SYSTEM_DEFAULT:
	lang = GetSystemDefaultLangID();
	break;    
    case LOCALE_USER_DEFAULT:
	lang = GetUserDefaultLangID();
	break;    
    case LOCALE_NEUTRAL:
	//TODO: Not correct; should take language associated with current thread
	lang = IDLANG_GETFIRST;
	break;    
    }

    pData = getPEResourceEntry(id, type, lang, &error);
    if(pData == NULL) {
        dprintf(("Win32ImageBase::getPEResourceSize: couldn't find resource %d (type %d, lang %x)", id, type, lang));
        return 0;
    }
    return pData->Size;
}
//******************************************************************************
//******************************************************************************
HRSRC Win32ImageBase::findResourceA(LPCSTR lpszName, LPSTR lpszType, ULONG langid)
{
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;
 Win32Resource                  *res;
 BOOL   fNumType;
 char  *winres = NULL;
 ULONG  id, type, lang;
 int    i, j, error;

    fNumType = TRUE;    //assume numeric
    if(HIWORD(lpszType) != 0) {//string id?
        for(i=0;i<MAX_RES;i++) {
            if(stricmp(lpszType, ResTypes[i]) == 0)
                    break;
        }
//TODO:
#if 0
        if(stricmp((char *)lpszType, BITMAP_TYPENAME2) == 0) {
            i = (int)RT_BITMAPA;
        }
#endif
        if(i == MAX_RES) {//custom resource type
            fNumType = FALSE;
            type = (ULONG)lpszType;
        }
        else type = i;
    }
    else  type = (ULONG)lpszType;

    switch(langid) {
    case LOCALE_SYSTEM_DEFAULT:
	lang = GetSystemDefaultLangID();
	break;    
    case LOCALE_USER_DEFAULT:
	lang = GetUserDefaultLangID();
	break;    
    case LOCALE_NEUTRAL:
	//TODO: Not correct; should take language associated with current thread
	lang = IDLANG_GETFIRST;
	break; 
    case IDLANG_GETFIRST:
        lang = GetUserDefaultLangID();
        break;   
    }
    id = (ULONG)lpszName;

    pData = getPEResourceEntry(id, type, lang, &error);
    if(pData == NULL) {
        //TODO: Optimize this; check if language wasn't found
        //try system default language
	if(error == RESERR_LANGNOTFOUND) {
	    	pData = getPEResourceEntry(id, type, GetSystemDefaultLangID(), &error);
	}
	if(pData == NULL) {
        	//finally try first available language
		if(error == RESERR_LANGNOTFOUND) {
    			pData = getPEResourceEntry(id, type, IDLANG_GETFIRST, &error);
		}
		if(pData == NULL) {
		        if(HIWORD(id)) {
		                dprintf(("Win32ImageBase::getPEResource %s: couldn't find resource %s (type %d, lang %x)", szModule, id, type, lang));
		        }
		        else    dprintf(("Win32ImageBase::getPEResource %s: couldn't find resource %d (type %d, lang %x)", szModule, id, type, lang));
		       	return 0;
		}
	}
    }
    if(HIWORD(id)) {
            dprintf(("FindResource %s: resource %s (type %d, lang %x)", szModule, id, type, lang));
    }
    else    dprintf(("FindResource %s: resource %d (type %d, lang %x)", szModule, id, type, lang));
    //ulRVAResourceSection contains the relative virtual address (relative to the start of the image)
    //for the resource section (images loaded by the pe.exe and pe2lx/win32k)
    //For LX images, this is 0 as OffsetToData contains a relative offset
    char *resdata = (char *)((char *)pResDir + (pData->OffsetToData - ulRVAResourceSection));
    res = new Win32Resource(this, id, type, pData->Size, resdata);

    return (HRSRC) res;
}
//******************************************************************************
//******************************************************************************
HRSRC Win32ImageBase::findResourceW(LPWSTR lpszName, LPWSTR lpszType, ULONG lang)
{
 HRSRC hres;
 char *astring1 = NULL, *astring2 = NULL;

    if(HIWORD(lpszName) != 0) {
            astring1 = UnicodeToAsciiString(lpszName);
    }
    else    astring1 = (char *)lpszName;

    if(HIWORD(lpszType) != 0) {
            astring2 = UnicodeToAsciiString(lpszType);
    }
    else    astring2 = (char *)lpszType;

    hres = (HRSRC) findResourceA(astring1, astring2, lang);

    if(HIWORD(astring1)) FreeAsciiString(astring1);
    if(HIWORD(astring2)) FreeAsciiString(astring2);

    return(hres);
}
//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getResourceSizeA(LPCSTR lpszName, LPSTR lpszType, ULONG lang)
{
    return getPEResourceSize((ULONG)lpszName, (ULONG)lpszType, lang);
}
//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getResourceSizeW(LPCWSTR lpszName, LPWSTR lpszType, ULONG lang)
{
 char *astring1 = NULL, *astring2 = NULL;
 ULONG ressize;

    if(HIWORD(lpszName) != 0) {
            astring1 = UnicodeToAsciiString((LPWSTR)lpszName);
    }
    else    astring1 = (char *)lpszName;

    if(HIWORD(lpszType) != 0) {
            astring2 = UnicodeToAsciiString(lpszType);
    }
    else    astring2 = (char *)lpszType;

    ressize =  getResourceSizeA(astring2, astring1, lang);

    if(HIWORD(astring1)) FreeAsciiString(astring1);
    if(HIWORD(astring2)) FreeAsciiString(astring2);

    return(ressize);
}
//******************************************************************************
//******************************************************************************
ULONG Win32ImageBase::getVersionSize()
{
    return getResourceSizeA((LPCSTR)1, (LPSTR)NTRT_VERSION);
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::getVersionStruct(char *verstruct, ULONG bufLength)
{
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;
 int                             error;

    if(verstruct == NULL || bufLength == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    pData = getPEResourceEntry(ID_GETFIRST, NTRT_VERSION, IDLANG_GETFIRST, &error);
    if(pData == NULL) {
        dprintf(("Win32PeLdrImage::getVersionStruct: couldn't find version resource!"));
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return 0;
    }
    char *resdata = (char *)((char *)pResDir + pData->OffsetToData - ulRVAResourceSection);
    memcpy(verstruct, resdata, min(bufLength, pData->Size));
    return TRUE;
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

    if (pResDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    /* validate parameters - FIXME... Exception handler??? */
    if ((unsigned)lpszType >= 0xc0000000) //....
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if ((unsigned)lpEnumFunc < 0x10000 || (unsigned)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }

    //reminder:
    //1st level -> types
    //2nd level -> names
    //3rd level -> language

    pResDirOurType = getResSubDirA(pResDir, lpszType);
    if (pResDirOurType != NULL)
    {
        char     *pszASCII = NULL;
        unsigned  cch = 0;
        unsigned  cResEntries;
        unsigned  i;

        fRet = TRUE;
        cResEntries = pResDirOurType->NumberOfNamedEntries + pResDirOurType->NumberOfIdEntries;
        paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((unsigned)pResDirOurType + sizeof(*pResDirOurType));
        for (i = 0; i < cResEntries && fRet; i++)
        {
            LPSTR lpszName;

            if (paResDirEntries[i].u1.s.NameIsString)
            {
                PIMAGE_RESOURCE_DIR_STRING_U pResDirString =
                    (PIMAGE_RESOURCE_DIR_STRING_U)(paResDirEntries[i].u1.s.NameOffset + (unsigned)pResDir);

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

    if (pResDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    /* validate parameters - FIXME... Exception handler??? */
    if ((unsigned)lpszType >= 0xc0000000) //....
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if ((unsigned)lpEnumFunc < 0x10000 || (unsigned)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }


    //reminder:
    //1st level -> types
    //2nd level -> names
    //3rd level -> language

    pResDirOurType = getResSubDirW(pResDir, lpszType);
    if (pResDirOurType != NULL)
    {
        unsigned  cResEntries;
        unsigned  i;

        fRet = TRUE;
        cResEntries = pResDirOurType->NumberOfNamedEntries + pResDirOurType->NumberOfIdEntries;
        paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((unsigned)pResDirOurType + sizeof(*pResDirOurType));
        for (i = 0; i < cResEntries && fRet; i++)
        {
            LPWSTR lpszName;

            if (paResDirEntries[i].u1.s.NameIsString)
                lpszName = (LPWSTR)(paResDirEntries[i].u1.s.NameOffset + (unsigned)pResDir + 2);
            else
                lpszName = (LPWSTR)paResDirEntries[i].u1.Id;

            fRet = lpEnumFunc(hmod, lpszType, lpszName, lParam);
        }
    }
    else
    {
        SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
        fRet = FALSE;
    }

    return fRet > 0;
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
PIMAGE_RESOURCE_DIRECTORY Win32ImageBase::getResSubDirW(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch, LPCWSTR lpszName)
{
    PIMAGE_RESOURCE_DIRECTORY_ENTRY paResDirEntries;
    int     i;
    int     idName = -1;

    /* lpszName */
    if (HIWORD(lpszName) != 0)
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

    paResDirEntries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((unsigned)pResDirToSearch + sizeof(*pResDirToSearch));
    if (idName != -1)
    {   /* idName */
        paResDirEntries += pResDirToSearch->NumberOfNamedEntries;

        for (i = 0; i < pResDirToSearch->NumberOfIdEntries; i++)
            if (paResDirEntries[i].u1.Id == (WORD)idName)
                return paResDirEntries[i].u2.s.DataIsDirectory ?
                    (PIMAGE_RESOURCE_DIRECTORY) (paResDirEntries[i].u2.s.OffsetToDirectory + (unsigned)pResDir /*?*/
                                                 /*- ulRVAResourceSection*/)
                    : NULL;
    }
    else
    {   /* string name */
        int cusName = lstrlenW(lpszName);

        for (i = 0; i < pResDirToSearch->NumberOfNamedEntries; i++)
        {
            PIMAGE_RESOURCE_DIR_STRING_U pResDirStr =
                (PIMAGE_RESOURCE_DIR_STRING_U)(paResDirEntries[i].u1.s.NameOffset + (unsigned)pResDir /*?*/);

            if (pResDirStr->Length == cusName
                && UniStrncmp(pResDirStr->NameString, lpszName, cusName) == 0)
            {
                return paResDirEntries[i].u2.s.DataIsDirectory ?
                    (PIMAGE_RESOURCE_DIRECTORY) (paResDirEntries[i].u2.s.OffsetToDirectory + (unsigned)pResDir
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
PIMAGE_RESOURCE_DIRECTORY Win32ImageBase::getResSubDirA(PIMAGE_RESOURCE_DIRECTORY pResDirToSearch, LPCTSTR lpszName)
{
    PIMAGE_RESOURCE_DIRECTORY   pResDirRet;
    LPCWSTR                     lpszwName;

    /* lpszName */
    if (HIWORD(lpszName) != 0)
    {
        lpszwName = AsciiToUnicodeString((char*)lpszName);
        if (lpszwName == NULL)
            return NULL;
    }
    else
        lpszwName = (LPWSTR)lpszName;

    pResDirRet = getResSubDirW(pResDirToSearch, lpszwName);

    if (HIWORD(lpszwName) != 0)
        free((void*)lpszwName);

    return pResDirRet;
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

    if (pResDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    if ((unsigned)lpEnumFunc < 0x10000 || (unsigned)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }

    //reminder:
    //1st level -> types
    //2nd level -> names
    //3rd level -> language

    /* set pointer to first resource type entry */
    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDir + sizeof(IMAGE_RESOURCE_DIRECTORY));

    for (i=0; i<pResDir->NumberOfNamedEntries+pResDir->NumberOfIdEntries && fRet; i++) 
    {
      	/* locate directory or each resource type */
    	prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)pResDir + (int)prde->u2.OffsetToData);

        if (prde->u1.s.NameIsString)
    	{//name or id entry?
	        //SvL: 30-10-'97, high bit is set, so clear to get real offset
	        nameOffset = prde->u1.Name & ~0x80000000;
	
	        pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((ULONG)pResDir + nameOffset);
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

    if (pResDir == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    if ((unsigned)lpEnumFunc < 0x10000 || (unsigned)lpEnumFunc >= 0xc0000000)
    {
        SetLastError(ERROR_NOACCESS);
        return FALSE;
    }

    //reminder:
    //1st level -> types
    //2nd level -> names
    //3rd level -> language

    /* set pointer to first resource type entry */
    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)pResDir + sizeof(IMAGE_RESOURCE_DIRECTORY));

    for (i=0; i<pResDir->NumberOfNamedEntries+pResDir->NumberOfIdEntries && fRet; i++) 
    {
      	/* locate directory or each resource type */
    	prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)pResDir + (int)prde->u2.OffsetToData);

        if (prde->u1.s.NameIsString)
    	{//name or id entry?
	        //SvL: 30-10-'97, high bit is set, so clear to get real offset
	        nameOffset = prde->u1.Name & ~0x80000000;
	
	        pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((ULONG)pResDir + nameOffset);
		fRet = lpEnumFunc(hmod, pstring->NameString, lParam);
    	}
    	else 	fRet = lpEnumFunc(hmod, (LPWSTR)prde->u1.Id, lParam);

        /* increment to next entry */
        prde++;
    }
    return fRet > 0 ? TRUE : FALSE;
}
//******************************************************************************
//******************************************************************************
