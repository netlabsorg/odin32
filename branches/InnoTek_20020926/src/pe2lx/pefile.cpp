/* $Id: pefile.cpp,v 1.3 1999-06-10 17:08:55 phaller Exp $ */

/*
 * PE2LX PE utility functions
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __WATCOMC__
#include <mem.h>
#endif
#include <win32type.h>
#include <pefile.h>
#include <misc.h>
#include <winimage.h>

//******************************************************************************
//******************************************************************************
char *UnicodeToFixedAsciiString(int length, WCHAR *NameString)
{
static char asciistring[256];
int i;

  if(length >= 255)	length = 255;
  for(i=0;i<length;i++) {
	asciistring[i] = NameString[i] & 0xFF;
  }
  asciistring[length] = 0;
  return(asciistring);
}
//******************************************************************************
//******************************************************************************
BOOL GetPEFileHeader (LPVOID lpFile, PIMAGE_FILE_HEADER pHeader)
{
    if(*(USHORT *)lpFile == IMAGE_DOS_SIGNATURE &&
       *(DWORD *)PE_HEADER (lpFile) == IMAGE_NT_SIGNATURE) 
    {
	 memcpy ((LPVOID)pHeader, PEHEADEROFF (lpFile), sizeof (IMAGE_FILE_HEADER));
	 return TRUE;
    }
    else return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL GetPEOptionalHeader (LPVOID lpFile, PIMAGE_OPTIONAL_HEADER pHeader)
{
    if(*(USHORT *)lpFile == IMAGE_DOS_SIGNATURE &&
       *(DWORD *)PE_HEADER (lpFile) == IMAGE_NT_SIGNATURE) 
    {
	 memcpy ((LPVOID)pHeader, OPTHEADEROFF (lpFile), sizeof (IMAGE_OPTIONAL_HEADER));
	 return TRUE;
    }
    else return FALSE;
}
//******************************************************************************
//******************************************************************************
LPVOID ImageDirectoryOffset (LPVOID lpFile, DWORD dwIMAGE_DIRECTORY)
{
    PIMAGE_OPTIONAL_HEADER   poh = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF (lpFile);
    IMAGE_SECTION_HEADER     sh;

    if (dwIMAGE_DIRECTORY >= poh->NumberOfRvaAndSizes)
	return NULL;

    if(GetSectionHdrByRVA(lpFile, &sh, poh->DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress) == FALSE)
    {
	return NULL;
    }

    return (LPVOID)(((ULONG)lpFile + (ULONG)(poh->DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress - 
                    sh.VirtualAddress) + (ULONG)sh.PointerToRawData));
}
//******************************************************************************
//******************************************************************************
BOOL GetSectionHdrByImageDir(LPVOID lpFile, DWORD dwIMAGE_DIRECTORY, PIMAGE_SECTION_HEADER pSect)
{
  PIMAGE_OPTIONAL_HEADER poh = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF (lpFile);

    if (dwIMAGE_DIRECTORY >= poh->NumberOfRvaAndSizes)
	return FALSE;

    return GetSectionHdrByRVA(lpFile, pSect, poh->DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress);
} 
//******************************************************************************
//******************************************************************************
BOOL IsImportSection(LPVOID lpFile, PIMAGE_SECTION_HEADER psh) 
{
  PIMAGE_OPTIONAL_HEADER poh = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF (lpFile);
  int     	         i = 0;
  DWORD                  ImageDirVA;

    ImageDirVA = poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    if(psh->VirtualAddress <= ImageDirVA &&
       psh->VirtualAddress + max(psh->Misc.VirtualSize,psh->SizeOfRawData) > ImageDirVA &&
       strcmp(psh->Name, ".idata") == 0)
    {
	return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL GetSectionHdrByName (LPVOID lpFile, IMAGE_SECTION_HEADER *sh, char *szSection)
{
 PIMAGE_SECTION_HEADER    psh;
 int nSections = NR_SECTIONS (lpFile);
 int i;

    if((psh = (PIMAGE_SECTION_HEADER)SECTIONHDROFF (lpFile)) != NULL) 
    {
	for(i=0; i<nSections; i++) 
        {
	    if(strcmp (psh->Name, szSection) == 0) 
            {
		 memcpy ((LPVOID)sh, (LPVOID)psh, sizeof (IMAGE_SECTION_HEADER));
		 return TRUE;
  	    }
	    else psh++;
        }
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL GetSectionHdrByType (LPVOID lpFile, IMAGE_SECTION_HEADER *sh, int type)
{
 PIMAGE_SECTION_HEADER    psh;
 int nSections = NR_SECTIONS (lpFile);
 int i;

    if((psh = (PIMAGE_SECTION_HEADER)SECTIONHDROFF (lpFile)) != NULL) 
    {
	for(i=0; i<nSections; i++) 
        {
	    if(psh->Characteristics & type) 
            {
		 memcpy ((LPVOID)sh, (LPVOID)psh, sizeof (IMAGE_SECTION_HEADER));
		 return TRUE;
  	    }
	    else psh++;
        }
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
int GetNumberOfResources(LPVOID lpFile)
{
  PIMAGE_RESOURCE_DIRECTORY	     prdRoot, prdType;
  PIMAGE_RESOURCE_DIRECTORY_ENTRY    prde;
  int 			             nCnt=0, i, j, id;
  char                              *resname;

    if ((prdRoot = (PIMAGE_RESOURCE_DIRECTORY)ImageDirectoryOffset
		    (lpFile, IMAGE_DIRECTORY_ENTRY_RESOURCE)) == NULL)
	return 0;

    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdRoot + sizeof (IMAGE_RESOURCE_DIRECTORY));

    for (i=0; i<prdRoot->NumberOfNamedEntries+prdRoot->NumberOfIdEntries; i++) 
    {
	prdType = (PIMAGE_RESOURCE_DIRECTORY)((ULONG)prdRoot + (ULONG)prde->u2.OffsetToData);

	if(i<prdRoot->NumberOfNamedEntries) {
		//SvL: 30-10-'97, high bit is set, so clear to get real offset
	 	prde->u1.Name &= ~0x80000000;
		for(j=0;j<MAX_RES;j++) {
			resname = UnicodeToFixedAsciiString(*(WCHAR *)((ULONG)prdRoot + (ULONG)prde->u1.Name), (WCHAR *)((ULONG)prdRoot + (ULONG)prde->u1.Name + sizeof(WCHAR)));  // first word = string length
			if(strcmp(resname, ResTypes[j]) == 0)
				break;
		}
		if(j == MAX_RES) {
			//SvL: 30-10-'97, not found = custom resource type (correct?)
			id = NTRT_RCDATA;
		}
		else    id = j;
	}
	else    id = prde->u1.Id;

	prdType = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)prdType ^ 0x80000000);

  	if(id == NTRT_STRING) {
		//String tables can contain up to 16 individual resources!
		nCnt += prdType->NumberOfNamedEntries*16 + prdType->NumberOfIdEntries*16;
	}
	else {
		//Only icon groups are stored as resources in the LX file
		//Icon groups contain one or more icons
		if(id != NTRT_ICON)
			nCnt += prdType->NumberOfNamedEntries + prdType->NumberOfIdEntries;
	}	
	prde++;
    }

    return nCnt;
}
/** Get Section Header for the given RVA - returns boolean according to the result
 *
 *  knut [Jul 22 1998 2:53am]
 */
BOOL GetSectionHdrByRVA (LPVOID lpFile, IMAGE_SECTION_HEADER *sh, ULONG rva)
{
	PIMAGE_SECTION_HEADER    psh;
	int nSections = NR_SECTIONS (lpFile);
	int i;

	if ((psh = (PIMAGE_SECTION_HEADER)SECTIONHDROFF (lpFile)) != NULL)
        {
		for (i=0; i<nSections; i++)
                {
			if (rva >= psh->VirtualAddress && rva < psh->VirtualAddress + max(psh->Misc.VirtualSize,psh->SizeOfRawData))
			{
				memcpy (sh, psh, sizeof(IMAGE_SECTION_HEADER));
				return TRUE;
			}
			else 	psh++;
		}
	}
	return FALSE;
}
//******************************************************************************
//******************************************************************************
