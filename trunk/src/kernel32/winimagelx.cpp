/* $Id: winimagelx.cpp,v 1.13 2001-10-16 11:39:39 sandervl Exp $ */

/*
 * Win32 LX Image base class
 *
 * Copyright 1999-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * TODO: headers not complete
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#define INCL_BASE
#include <os2wrap.h>             //Odin32 OS/2 api wrappers

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>
#include <misc.h>
#include <win32type.h>
#include <winimagebase.h>
#include <winimagelx.h>
#include <windllbase.h>
#include <winexebase.h>
#include <winexelx.h>
#include <windlllx.h>
#include <pefile.h>
#include <unicode.h>
#include "oslibmisc.h"
#include "initterm.h"
#include <win\virtual.h>

#define DBG_LOCALLOG	DBG_winimagelx
#include "dbglocal.h"

static BYTE dosHeader[] = {
 0x4D, 0x5A, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x0B, 0x00,
 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
 0x6A, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00,
 0xB4, 0x30, 0xCD, 0x21, 0x86, 0xC4, 0x3D, 0x0A, 0x14, 0x72, 0x42, 0xBE, 0x80, 0x00, 0x8A, 0x1C,
 0x32, 0xFF, 0x46, 0x88, 0x38, 0x2E, 0x8C, 0x1E, 0x6E, 0x00, 0x8E, 0x06, 0x2C, 0x00, 0x33, 0xC0,
 0x8B, 0xF8, 0xB9, 0x00, 0x80, 0xFC, 0xF2, 0xAE, 0x75, 0x23, 0x49, 0x78, 0x20, 0xAE, 0x75, 0xF6,
 0x47, 0x47, 0x0E, 0x1F, 0x2E, 0x89, 0x3E, 0x68, 0x00, 0x2E, 0x8C, 0x06, 0x6A, 0x00, 0xBE, 0x5C,
 0x00, 0xB9, 0x6C, 0x63, 0xBB, 0x25, 0x00, 0xB4, 0x64, 0xCD, 0x21, 0x73, 0x0B, 0xBA, 0x7C, 0x00,
 0x0E, 0x1F, 0xB4, 0x09, 0xCD, 0x21, 0xB0, 0x01, 0xB4, 0x4C, 0xCD, 0x21, 0x20, 0x00, 0x01, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x54, 0x68, 0x69, 0x73,
 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x20, 0x6D, 0x75, 0x73, 0x74, 0x20, 0x62, 0x65,
 0x20, 0x72, 0x75, 0x6E, 0x20, 0x75, 0x6E, 0x64, 0x65, 0x72, 0x20, 0x57, 0x69, 0x6E, 0x33, 0x32,
 0x2E, 0x0D, 0x0A, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//******************************************************************************
//******************************************************************************
Win32LxImage::Win32LxImage(HINSTANCE hInstance, PVOID pResData)
               : Win32ImageBase(hInstance), header(0)
{
 APIRET rc;
 char  *name;

  szFileName[0] = 0;

  if(lpszCustomDllName) {
       name = lpszCustomDllName;
       this->dwOrdinalBase = ::dwOrdinalBase;
  }
  else {
       name = OSLibGetDllName(hinstance);
       this->dwOrdinalBase = 0;
  }

  strcpy(szFileName, name);
  strupr(szFileName);

  setFullPath(szFileName);

  //Pointer to PE resource tree generates by wrc (or NULL for system dlls)
  pResRootDir = (PIMAGE_RESOURCE_DIRECTORY)pResData;

  //ulRVAResourceSection contains the virtual address of the imagebase in the PE header
  //for the resource section (images loaded by the pe.exe)
  //For LX images, this is 0 as OffsetToData contains a relative offset
  ulRVAResourceSection = 0;
}
//******************************************************************************
//******************************************************************************
Win32LxImage::~Win32LxImage()
{
  if(header) {
	DosFreeMem(header);
  }
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxImage::getApi(char *name)
{
  APIRET      rc;
  ULONG       apiaddr;

  rc = DosQueryProcAddr(hinstanceOS2, 0, name, (PFN *)&apiaddr);
  if(rc)
  {
	dprintf(("Win32LxImage::getApi %x %s -> rc = %d", hinstanceOS2, name, rc));
	return(0);
  }
  return(apiaddr);
}
//******************************************************************************
//******************************************************************************
ULONG Win32LxImage::getApi(int ordinal)
{
 APIRET      rc;
 ULONG       apiaddr;

  rc = DosQueryProcAddr(hinstanceOS2, dwOrdinalBase+ordinal, NULL, (PFN *)&apiaddr);
  if(rc) {
	dprintf(("Win32LxImage::getApi %x %d -> rc = %d", hinstanceOS2, ordinal, rc));
	return(0);
  }
  return(apiaddr);
}
//******************************************************************************
//******************************************************************************
LPVOID Win32LxImage::buildHeader(DWORD MajorImageVersion, DWORD MinorImageVersion,
                                 DWORD Subsystem) 
{
 APIRET rc;
 IMAGE_DOS_HEADER *pdosheader;
 PIMAGE_OPTIONAL_HEADER poh;
 PIMAGE_FILE_HEADER     pfh;
 DWORD *ntsig;

  rc = DosAllocMem(&header, 4096, PAG_READ | PAG_WRITE | PAG_COMMIT);
  if(rc) {
	dprintf(("ERROR: buildHeader DosAllocMem failed!! (rc=%x)", rc));
        DebugInt3();
	return NULL;
  }
  memcpy(header, dosHeader, sizeof(dosHeader));
  ntsig  = (DWORD *)((LPBYTE)header + sizeof(dosHeader));
  *ntsig = IMAGE_NT_SIGNATURE;
  pfh    = (PIMAGE_FILE_HEADER)(ntsig+1);
  pfh->Machine              = IMAGE_FILE_MACHINE_I386;
  pfh->NumberOfSections     = 0;
  pfh->TimeDateStamp        = 0x3794f60f;
  pfh->PointerToSymbolTable = 0;
  pfh->NumberOfSymbols      = 0;
  pfh->SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER);
  pfh->Characteristics      = IMAGE_FILE_DLL | IMAGE_FILE_32BIT_MACHINE | 
                              IMAGE_FILE_DEBUG_STRIPPED | IMAGE_FILE_EXECUTABLE_IMAGE | 
                              IMAGE_FILE_RELOCS_STRIPPED;
  poh    = (PIMAGE_OPTIONAL_HEADER)(pfh+1);
  poh->Magic                       = IMAGE_NT_OPTIONAL_HDR_MAGIC;
  poh->MajorLinkerVersion          = 0x3;
  poh->MinorLinkerVersion          = 0xA;
  poh->SizeOfCode                  = 0;
  poh->SizeOfInitializedData       = 0;
  poh->SizeOfUninitializedData     = 0;
  poh->AddressOfEntryPoint         = 0;
  poh->BaseOfCode                  = 0;
  poh->BaseOfData                  = 0;
  poh->ImageBase                   = 0;
  poh->SectionAlignment            = 4096;
  poh->FileAlignment               = 512;
  poh->MajorOperatingSystemVersion = MajorImageVersion;
  poh->MinorOperatingSystemVersion = MinorImageVersion;
  poh->MajorImageVersion           = MajorImageVersion;
  poh->MinorImageVersion           = MinorImageVersion;
  poh->MajorSubsystemVersion       = ODINNT_MAJOR_VERSION;
  poh->MinorSubsystemVersion       = ODINNT_MINOR_VERSION;
  poh->Reserved1                   = 0;
  poh->SizeOfImage                 = 0;
  poh->SizeOfHeaders               = 1024;
  poh->CheckSum                    = 0;
  poh->Subsystem                   = Subsystem;
  poh->DllCharacteristics          = 0;
  poh->SizeOfStackReserve          = 1*1024*1024;
  poh->SizeOfStackCommit           = 4096;
  poh->SizeOfHeapReserve           = 1*1024*1024;
  poh->SizeOfHeapCommit            = 4096;
  poh->LoaderFlags                 = 0;
  poh->NumberOfRvaAndSizes         = 0;
//  poh->DataDirectory[0]

  return header;
}
//******************************************************************************
//******************************************************************************
