/* $Id: winimgres.cpp,v 1.25 1999-10-26 11:15:33 sandervl Exp $ */

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <winimagebase.h>
#include <winimagepe2lx.h>
#include <winimagepeldr.h>
#include <winimagelx.h>
#include <winres.h>
#include <unicode.h>
#include <heapstring.h>
#include "pefile.h"
#include "oslibmisc.h"

//SvL: VPBuddy bugfix, seems to load bitmaps with type name 'DIB'
#define BITMAP_TYPENAME2    "DIB"

//******************************************************************************
//Assuming names are case insensitive
//PE spec says names & ids are sorted; keep on searching just to be sure
//******************************************************************************
PIMAGE_RESOURCE_DATA_ENTRY
 Win32ImageBase::getPEResourceEntry(ULONG id, ULONG type, ULONG lang)
{
 PIMAGE_RESOURCE_DIRECTORY       prdType;
 PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
 PIMAGE_RESOURCE_DIR_STRING_U    pstring;
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;
 ULONG  nodeData[3], i, j, nameOffset;
 BOOL  fFound = FALSE, fNumType;

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
    Win32ImageBase::ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType,
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
ULONG Win32ImageBase::getPEResourceSize(ULONG id, ULONG type, ULONG lang)
{
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;

    pData = getPEResourceEntry(id, type, lang);
    if(pData == NULL) {
        dprintf(("Win32ImageBase::getPEResourceSize: couldn't find resource %d (type %d, lang %d)", id, type, lang));
        return 0;
    }
    return pData->Size;
}
//******************************************************************************
//******************************************************************************
HRSRC Win32ImageBase::findResourceA(LPCSTR lpszName, LPSTR lpszType, ULONG lang)
{
 PIMAGE_RESOURCE_DATA_ENTRY      pData = NULL;
 Win32Resource                  *res;
 BOOL   fNumType;
 char  *winres = NULL;
 ULONG  id, type;
 int    i, stringid = -1, j;

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

    //String format: tables of 16 strings stored as one resource
    //upper 12 bits of resource id passed by user determines block (res id)
    //lower 4 bits are an index into the string table
    if(fNumType) {
        if(type == NTRT_STRING) {
            stringid = (ULONG)lpszName & 0xF;
            id       = (((ULONG)lpszName) >> 4)+1;
        }
        else id = (ULONG)lpszName;
    }
    else {
        if(stricmp((char *)type, ResTypes[NTRT_STRING]) == 0) {
            stringid = (ULONG)lpszName & 0xF;
            id       = (((ULONG)lpszName) >> 4)+1;
        }
        else id = (ULONG)lpszName;
    }

    pData = getPEResourceEntry(id, type, lang);
    if(pData == NULL) {
        if(HIWORD(id)) {
                dprintf(("Win32ImageBase::getPEResource %s: couldn't find resource %s (type %d, lang %d)", szModule, id, type, lang));
        }
        else    dprintf(("Win32ImageBase::getPEResource %s: couldn't find resource %d (type %d, lang %d)", szModule, id, type, lang));
        return 0;
    }
    //pResourceSectionStart contains the virtual address of the imagebase in the PE header
    //for the resource section (images loaded by the pe.exe)
    //For LX images, this is 0 as OffsetToData contains a relative offset
    char *resdata = (char *)((char *)pResDir + pData->OffsetToData - pResourceSectionStart);
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
    else
    {
       	res = new Win32Resource(this, id, type, pData->Size, resdata);
    }

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

    hres = (HRSRC) findResourceA(astring1, astring2);

    if(HIWORD(astring1)) FreeAsciiString(astring1);
    if(HIWORD(astring2)) FreeAsciiString(astring2);

    return(hres);
}
//******************************************************************************
//TODO:
//******************************************************************************
#if 0
ULONG Win32Pe2LxImage::getResourceSizeA(LPCSTR lpszName, LPSTR lpszType, ULONG lang)
{
    DebugInt3();
    return 0;
}
#endif
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
#if 0
ULONG Win32Pe2LxImage::getVersionSize()
{
    if(getVersionId() == -1) {
        dprintf(("GetVersionSize: %s has no version resource!\n", szModule));
        return(0);
    }
    return OSLibGetResourceSize(hinstance, getVersionId());
}
//******************************************************************************
//******************************************************************************
BOOL Win32Pe2LxImage::getVersionStruct(char *verstruct, ULONG bufLength)
{
    if(getVersionId() == -1) {
        dprintf(("GetVersionStruct: %s has no version resource!\n", szModule));
        return(FALSE);
    }
    return OSLibGetResource(hinstance, getVersionId(), verstruct, bufLength);
}
#endif
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

    if(verstruct == NULL || bufLength == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    pData = getPEResourceEntry(ID_GETFIRST, NTRT_VERSION);
    if(pData == NULL) {
        dprintf(("Win32PeLdrImage::getVersionStruct: couldn't find version resource!"));
        return 0;
    }
    char *resdata = (char *)((char *)pResDir + pData->OffsetToData - pResourceSectionStart);
    memcpy(verstruct, resdata, min(bufLength, pData->Size));
    return TRUE;
}
//******************************************************************************
//******************************************************************************
