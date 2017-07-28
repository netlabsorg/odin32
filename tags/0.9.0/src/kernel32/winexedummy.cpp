/* $Id: winexedummy.cpp,v 1.6 2004-01-15 10:39:09 sandervl Exp $ */

/*
 * Win32 Dummy Exe class
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <misc.h>
#include <win32type.h>
#include "winexedummy.h"
#include <winconst.h>
#include <win32api.h>
#include <wprocess.h>
#include "initterm.h"

static BOOL fIsDummyExe = FALSE;

//******************************************************************************
//Create Dummy Exe object
//******************************************************************************
BOOL WIN32API RegisterDummyExe(LPSTR pszExeName)
{
    return RegisterDummyExeEx (pszExeName, NULL);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RegisterDummyExeEx(LPSTR pszExeName, PVOID pResData)
{
    if(WinExe != NULL)
    	return TRUE;

    Win32DummyExe *winexe;

    winexe = new Win32DummyExe(pszExeName, pResData);

    if(winexe) {
        InitCommandLine(FALSE);
        winexe->start();
        fIsDummyExe = TRUE;
    }
    else {
      	eprintf(("Win32DummyExe creation failed!\n"));
      	DebugInt3();
       	return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDummyExeLoaded()
{
    return fIsDummyExe;
}
//******************************************************************************
//******************************************************************************
Win32DummyExe::Win32DummyExe(LPSTR pszExeName, PVOID pResData)
                 : Win32ImageBase(-1),
		   Win32ExeBase(-1), header(0)
{
    dprintf(("Win32DummyExe ctor: %s", pszExeName));
    hinstance = (HINSTANCE)buildHeader(1, 0, IMAGE_SUBSYSTEM_WINDOWS_GUI);
    strcpy(szModule, pszExeName);
    strcpy(szFileName, pszExeName);
    setFullPath(pszExeName);

    //Pointer to PE resource tree generates by wrc (or NULL for system dlls)
    pResRootDir = (PIMAGE_RESOURCE_DIRECTORY)pResData;
}
//******************************************************************************
//******************************************************************************
Win32DummyExe::~Win32DummyExe()
{
    if(header) {
    	DosFreeMem(header);
    }
}
//******************************************************************************
//******************************************************************************
ULONG Win32DummyExe::start()
{
    return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32DummyExe::getApi(char *name)
{
    return 0;
}
//******************************************************************************
//******************************************************************************
ULONG Win32DummyExe::getApi(int ordinal)
{
    return 0;
}
//******************************************************************************
//******************************************************************************
LPVOID Win32DummyExe::buildHeader(DWORD MajorImageVersion, DWORD MinorImageVersion,
                                  DWORD Subsystem) 
{
    APIRET rc;
    IMAGE_DOS_HEADER *pdosheader;
    PIMAGE_OPTIONAL_HEADER poh;
    PIMAGE_FILE_HEADER     pfh;
    DWORD *ntsig;

    // AH TODO: we are wasting 60kb address space here (unless using high memory)!!!
    rc = DosAllocMem(&header, 4096, PAG_READ | PAG_WRITE | PAG_COMMIT | flAllocMem);
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
    poh->Win32VersionValue           = 0;
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
