/* $Id: pefile.cpp,v 1.10 2000-10-06 11:04:01 sandervl Exp $ */

/*
 * PE2LX PE utility functions
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_BASE
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <win32type.h>
#include <pefile.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_pefile
#include "dbglocal.h"

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
LPVOID ImageDirectoryOffset(LPVOID lpFile, DWORD dwIMAGE_DIRECTORY)
{
    PIMAGE_OPTIONAL_HEADER   poh = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF (lpFile);
    IMAGE_SECTION_HEADER     sh;

    if (dwIMAGE_DIRECTORY >= poh->NumberOfRvaAndSizes)
	return NULL;

    if(GetSectionHdrByRVA(lpFile, &sh, poh->DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress) == FALSE)
    {
	return NULL;
    }

    return (LPVOID)((ULONG)lpFile + poh->DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress); 
}
//******************************************************************************
//******************************************************************************
DWORD ImageDirectorySize(LPVOID lpFile, DWORD dwIMAGE_DIRECTORY)
{
    PIMAGE_OPTIONAL_HEADER   poh = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF (lpFile);
    IMAGE_SECTION_HEADER     sh;

    if (dwIMAGE_DIRECTORY >= poh->NumberOfRvaAndSizes)
	return 0;

    if(GetSectionHdrByRVA(lpFile, &sh, poh->DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress) == FALSE)
    {
	return 0;
    }

    return poh->DataDirectory[dwIMAGE_DIRECTORY].Size;
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
BOOL IsSectionType(LPVOID lpFile, PIMAGE_SECTION_HEADER psh, DWORD dwIMAGE_DIRECTORY)
{
  PIMAGE_OPTIONAL_HEADER poh = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF (lpFile);
  int     	         i = 0;
  DWORD                  ImageDirVA;

    ImageDirVA = poh->DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress;

    if(psh->VirtualAddress <= ImageDirVA &&
       psh->VirtualAddress + max(psh->Misc.VirtualSize,psh->SizeOfRawData) > ImageDirVA)
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
