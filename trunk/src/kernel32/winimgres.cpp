/* $Id: winimgres.cpp,v 1.11 1999-08-20 11:52:44 sandervl Exp $ */

/*
 * Win32 PE Image class (resource methods)
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Check created resource objects before loading the resource!
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <nameid.h>
#include <winimage.h>
#include <windll.h>
#include <winexe.h>
#include <winres.h>
#include <unicode.h>
#include <heapstring.h>
#include "pefile.h"

char *ResTypes[MAX_RES] =
      {"niks", "CURSOR", "BITMAP", "ICON", "MENU", "DIALOG", "STRING",
       "FONTDIR", "FONT", "ACCELERATOR", "RCDATA",  "MESSAGETABLE",
       "GROUP_CURSOR", "niks", "GROUP_ICON", "niks", "VERSION"};


//******************************************************************************
//Assuming names are case insensitive
//PE spec says names & ids are sorted; keep on searching just to be sure
//******************************************************************************
PIMAGE_RESOURCE_DATA_ENTRY 
 Win32Image::getPEResourceEntry(ULONG id, ULONG type, ULONG lang)
{
 PIMAGE_RESOURCE_DIRECTORY       prdType;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
 PIMAGE_RESOURCE_DIR_STRING_U    pstring;
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;
 ULONG  nodeData[3], i, j, nameOffset;
 BOOL  fFound = FALSE, fNumType;

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
       	lstrcpynWtoA(typename, pstring->NameString, pstring->Length);
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
            pData = ProcessResSubDir(prdType, &nodeData[0], 2);
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
  return pData;
}
//******************************************************************************
//level: 2 ids
//       3 languages
//******************************************************************************
PIMAGE_RESOURCE_DATA_ENTRY
    Win32Image::ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType,
                                 ULONG *nodeData, int level)
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
    	return(NULL);
  }
  prdType = (PIMAGE_RESOURCE_DIRECTORY)((ULONG)prdType & ~0x80000000);
  prde    = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdType + sizeof(IMAGE_RESOURCE_DIRECTORY));

  if(level == 3 && *nodeData == LANG_GETFIRST) {
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

    	if(!fNumId) {//name or id entry?
		nameOffset = prde->u1.Name;
        	if(prde->u1.s.NameIsString) //unicode directory string /*PLF Sat  97-06-21 22:30:35*/
               		nameOffset &= ~0x80000000;

           	pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((ULONG)pResDir + nameOffset);

		resname = (char *)malloc(pstring->Length+1);
         	lstrcpynWtoA(resname, pstring->NameString, pstring->Length);
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
      	if(*nodeData == LANG_GETFIRST)
         	fFound = TRUE;

   	if(fFound) {
         	if((ULONG)prdType2 & 0x80000000) {//subdirectory?
               		return ProcessResSubDir(prdType2, nodeData+1, 3);
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
ULONG Win32Image::getPEResourceSize(ULONG id, ULONG type, ULONG lang)
{
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;

  pData = getPEResourceEntry(id, type, lang);
  if(pData == NULL) {
	dprintf(("Win32Image::getPEResourceSize: couldn't find resource %d (type %d, lang %d)", id, type, lang));
	return 0;
  }
  return pData->Size;
}
//******************************************************************************
//******************************************************************************
Win32Resource *Win32Image::getPEResource(ULONG id, ULONG type, ULONG lang)
{
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;
 Win32Resource                  *res;
 BOOL   fNumType;
 char  *winres = NULL;
 int    i, stringid = -1, j;

  fNumType = TRUE;    //assume numeric
  if(HIWORD(type) != 0) {//string id?
    for(i=0;i<MAX_RES;i++) {
         if(stricmp((char *)type, ResTypes[i]) == 0)
            	break;
    }
    if(i == MAX_RES) {//custom resource type
       	 fNumType = FALSE;
    }
    else type   = i;
  }

  //String format: tables of 16 strings stored as one resource
  //upper 12 bits of resource id passed by user determines block (res id)
  //lower 4 bits are an index into the string table
  if(fNumType) {
    if(type == NTRT_STRING) {
        stringid = id & 0xF;
        id       = (id >> 4)+1;
    }
  }
  else {
    if(stricmp((char *)type, ResTypes[NTRT_STRING]) == 0) {
        stringid = id & 0xF;
        id       = (id >> 4)+1;
    }
  }

  pData = getPEResourceEntry(id, type, lang);
  if(pData == NULL) {
	dprintf(("Win32Image::getPEResource: couldn't find resource %d (type %d, lang %d)", id, type, lang));
	return 0;
  }

  char *resdata = (char *)((char *)pResDir + pData->OffsetToData - (pResSection->virtaddr - oh.ImageBase));
  if(stringid != -1) {//search for string in table
    	USHORT *unicodestr = (USHORT *)resdata;

    	for(i=0;i<stringid;i++) {
        	unicodestr += *unicodestr+1;
    	}
    	res = new Win32Resource(this, id, NTRT_STRING, (*unicodestr+1)*sizeof(WCHAR),
                                (char *)(unicodestr+1));
    	if(res == NULL) {
        	dprintf(("new Win32Resource failed!\n"));
        	return(NULL);
    	}
  }
  else  res = new Win32Resource(this, id, type, pData->Size, resdata);

  return res;
}
//******************************************************************************
//******************************************************************************
HRSRC Win32Image::findResourceA(LPCSTR lpszName, LPSTR lpszType)
{
 Win32Resource *res = NULL;
 HRSRC hres;
 int   i;
 LPSTR szType = (LPSTR)lpszType;

    if(fNativePEImage == TRUE) {
        return (HRSRC) getPEResource((ULONG)lpszName, (ULONG)lpszType);
    }
    //else converted win32 exe/dll

    if(HIWORD(lpszType) != 0) {//type name, translate to id
        for(i=0;i<MAX_RES;i++) {
            if(strcmp(lpszType, ResTypes[i]) == 0)
                break;
        }
        if(i == MAX_RES) {//custom resource type, stored as rcdata
                dprintf(("FindResourceA custom type %s\n", lpszType));
               lpszType = (LPSTR)NTRT_RCDATA;
        }
        else    lpszType = (LPSTR)i;

        szType = (LPSTR)lpszType;
    }
    switch((int)szType) {
    case NTRT_GROUP_ICON:
        szType = (LPSTR)NTRT_ICON;
        break;
    case NTRT_GROUP_CURSOR:
        szType = (LPSTR)NTRT_CURSOR;
        break;
    case NTRT_VERSION:
        szType = (LPSTR)NTRT_RCDATA;
        break;
    case NTRT_STRING:
    case NTRT_MENU:
    case NTRT_ICON:
    case NTRT_BITMAP:
    case NTRT_CURSOR:
    case NTRT_DIALOG:
    case NTRT_RCDATA:
    case NTRT_ACCELERATORS:
        szType = lpszType;
        break;
    default: //unknown are stored as rcdata
        szType = (LPSTR)NTRT_RCDATA;
        break;
    }
    dprintf(("FindResourceA from %X type %d (%X)\n", hinstance, szType, lpszType));

    if(HIWORD(lpszName) != 0) {//convert string name identifier to numeric id
        dprintf(("FindResource %s\n", lpszName));
      	if(lpszName[0] == '#') {// #344
            	lpszName = (LPCSTR)atoi(&lpszName[1]);
      	}
      	else  	lpszName = (LPCSTR)ConvertNameId(hinstance, (char *)lpszName);
    }
    else dprintf(("FindResource %d\n", (int)lpszName));

    hres = O32_FindResource(hinstance, lpszName, szType);
    if(hres)
    {
      	res = new Win32Resource(this, hres, (ULONG)lpszName, (ULONG)szType);
    }

    if(hres == NULL && HIWORD(lpszName) == 0 && (int)szType == NTRT_STRING) {
      	hres = O32_FindResource(hinstance, (LPCSTR)(((int)lpszName - 1)*16), (LPCSTR)NTRT_RCDATA);
      	if(hres)
      	{
         	res = new Win32Resource(this, hres, (ULONG)lpszName, (ULONG)szType);
      	}
      	else    dprintf(("FindResourceA can't find string %d\n", (int)lpszName));
    }
    dprintf(("FindResourceA returned %X (%X)\n", hres, GetLastError()));

    return (HRSRC)res;
}
//******************************************************************************
//******************************************************************************
HRSRC Win32Image::findResourceW(LPWSTR lpszName, LPWSTR lpszType)
{
 Win32Resource *res = NULL;
 HRSRC hres;
 LPSTR szType = (LPSTR)lpszType;
 int   i;
 char *astring1 = NULL, *astring2 = NULL;

    if(fNativePEImage == TRUE) {//load resources directly from res section
    	if(HIWORD(lpszType) != 0) {
         	char *resname = UnicodeToAsciiString(lpszType);
    	} 
    	else	astring1 = (char *)lpszType;

    	if(HIWORD(lpszName) != 0) {
         	astring2 = UnicodeToAsciiString(lpszName);
    	}
    	else	astring2 = (char *)lpszName;

    	hres = (HRSRC) getPEResource((ULONG)astring1, (ULONG)astring1);
    	if(astring1) FreeAsciiString(astring1);
    	if(astring2) FreeAsciiString(astring2);

    	return(hres);
    }
    //else converted win32 exe/dll
    if(HIWORD(lpszType) != 0) {//type name, translate to id
        char *resname = UnicodeToAsciiString(lpszType);
        for(i=0;i<MAX_RES;i++) {
            if(strcmp(resname, ResTypes[i]) == 0)
                break;
        }
        if(i == MAX_RES) {//custom resource type, stored as rcdata
                dprintf(("FindResourceW custom type %s\n", resname));
            i = NTRT_RCDATA;
        }
        FreeAsciiString(resname);
        lpszType = (LPWSTR)i;

        szType = (LPSTR)lpszType;
    }
    switch((int)szType) {
    case NTRT_GROUP_ICON:
        szType = (LPSTR)NTRT_ICON;
        break;
    case NTRT_GROUP_CURSOR:
        szType = (LPSTR)NTRT_CURSOR;
        break;
    case NTRT_VERSION:
        szType = (LPSTR)NTRT_RCDATA;
        break;
    case NTRT_STRING:
    case NTRT_MENU:
    case NTRT_ICON:
    case NTRT_BITMAP:
    case NTRT_CURSOR:
    case NTRT_DIALOG:
    case NTRT_RCDATA:
    case NTRT_ACCELERATORS:
        szType = (LPSTR)lpszType;
        break;
    default: //unknown are stored as rcdata
        szType = (LPSTR)NTRT_RCDATA;
        break;
    }
    dprintf(("FindResourceW type %d\n", szType));

    if(HIWORD(lpszName) != 0) {//convert string name identifier to numeric id
      	astring1 = UnicodeToAsciiString(lpszName);
        dprintf(("FindResourceW %X %s\n", hinstance, astring1));
      	if(astring1[0] == '#') {// #344
            	lpszName = (LPWSTR)atoi(&astring1[1]);
      	}
      	else  	lpszName = (LPWSTR)ConvertNameId(hinstance, (char *)astring1);
    }
    else dprintf(("FindResourceW %X %d\n", hinstance, (int)lpszName));

    hres = O32_FindResource(hinstance, (LPCSTR)lpszName, (LPCSTR)szType);
    if(hres)
    {
      	res = new Win32Resource(this, hres, (ULONG)lpszName, (ULONG)szType);
    }

    if(hres == NULL && HIWORD(lpszName) == 0 && (int)szType == NTRT_STRING) {
      	hres = O32_FindResource(hinstance, (LPCSTR)(((ULONG)lpszName - 1)*16), (LPCSTR)NTRT_RCDATA);
      	if(hres)
      	{
        	res = new Win32Resource(this, hres, (ULONG)lpszName, (ULONG)szType);
      	}
      	else    dprintf(("FindResourceW can't find string %d\n", (int)lpszName));
    }
    if(astring1)    FreeAsciiString(astring1);

    dprintf(("FindResourceW returned %X (%X)\n", hres, GetLastError()));

    return (HRSRC)res;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Image::getResourceSizeA(LPCSTR lpszName, LPSTR lpszType)
{
    if(fNativePEImage == TRUE) {
        return getPEResourceSize((ULONG)lpszName, (ULONG)lpszType);
    }
    DebugInt3();
    return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32Image::getResourceSizeW(LPCWSTR lpszName, LPWSTR lpszType)
{
 char *astring1 = NULL, *astring2 = NULL;
 ULONG ressize;

    if(fNativePEImage == TRUE) {//load resources directly from res section
    	if(HIWORD(lpszType) != 0) {
         	char *resname = UnicodeToAsciiString(lpszType);
    	} 
    	else	astring1 = (char *)lpszType;

    	if(HIWORD(lpszName) != 0) {
         	astring2 = UnicodeToAsciiString((LPWSTR)lpszName);
    	}
    	else	astring2 = (char *)lpszName;

    	ressize =  getPEResourceSize((ULONG)astring1, (ULONG)astring1);
    	if(astring1) FreeAsciiString(astring1);
    	if(astring2) FreeAsciiString(astring2);

    	return(ressize);
    }
    DebugInt3();
    return 0;
}
//******************************************************************************
//******************************************************************************
