/* $Id: winimagepeldr.cpp,v 1.14 1999-11-22 20:35:52 sandervl Exp $ */

/*
 * Win32 PE loader Image base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 * NOTE: RSRC_LOAD is a special flag to only load the resource directory
 *       of a PE image. Processing imports, sections etc is not done.
 *       Nor is it put into the linked list of dlls (if it's a dll).
 *       This is useful for GetVersionSize/Resource in case it wants to
 *       get version info of an image that is not loaded.
 *       So an instance of this type can't be used for anything but resource lookup!
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
#include <winimagepeldr.h>
#include <windllpeldr.h>
#include <pefile.h>
#include <unicode.h>
#include <winres.h>
#include "oslibmisc.h"
#include "initterm.h"
#include <win\virtual.h>
#include "oslibdos.h"
#include "mmap.h"

char szErrorTitle[]     = "Odin";
char szMemErrorMsg[]    = "Memory allocation failure";
char szFileErrorMsg[]   = "File IO error";
char szPEErrorMsg[]     = "Not a valid win32 exe. (perhaps 16 bits windows)";
char szCPUErrorMsg[]    = "Executable doesn't run on x86 machines";
char szExeErrorMsg[]    = "File isn't an executable";
char szInteralErrorMsg[]= "Internal Error";
char szErrorModule[128] = "";

#ifndef max
#define max(a, b)  ((a>b) ? a : b)
#endif

BOOL foutInit = FALSE;
ofstream fout;

ULONG MissingApi();
char *hex(ULONG num);

extern ULONG flAllocMem;    /*Tue 03.03.1998: knut */

//******************************************************************************
//******************************************************************************
Win32PeLdrImage::Win32PeLdrImage(char *pszFileName, int loadtype) :
    Win32ImageBase(-1),
    nrsections(0), imageSize(0),
    imageVirtBase(-1), realBaseAddress(0), imageVirtEnd(0),
    nrNameExports(0), nrOrdExports(0), nameexports(NULL), ordexports(NULL),
    memmap(NULL), pFixups(NULL)
{
 HFILE  dllfile;

  loadType = loadtype;

  strcpy(szFileName, pszFileName);
  strupr(szFileName);
  if(!strchr(szFileName, (int)'.')) {
	strcat(szFileName,".DLL");
  }
  dllfile = OSLibDosOpen(szFileName, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
  if(dllfile == NULL) {//search in libpath for dll
	strcpy(szModule, kernel32Path);
	strcat(szModule, szFileName);
	strcpy(szFileName, szModule);
  }
  else	OSLibDosClose(dllfile);

  strcpy(szModule, OSLibStripPath(szFileName));
  strupr(szModule);
  char *dot = strstr(szModule, ".");
  while(dot) {
	char *newdot = strstr(dot+1, ".");
	if(newdot == NULL)	break;
	dot = newdot;
  }
  if(dot)
	*dot = 0;

  if(foutInit == FALSE) {
    char logname[32];
	sprintf(logname, "pe_%d.log", loadNr);
    	fout.open(logname, ios::out | ios::trunc);
	if(fout.good() == FALSE) {
		sprintf(logname, "%spe_%d.log", kernel32Path, loadNr);
	    	fout.open(logname, ios::out | ios::trunc);
	}
	dprintf(("PE LOGFILE for %s: %s", szModule, logname));
    	foutInit = TRUE;
  }
}
//******************************************************************************
//******************************************************************************
Win32PeLdrImage::~Win32PeLdrImage()
{
  if(memmap)
	delete memmap;

  if(hFile) {
  	OSLibDosClose(hFile);
	hFile = 0;
  }

  if(realBaseAddress)
    	DosFreeMem((PVOID)realBaseAddress);

  if(nameexports)
    	free(nameexports);

  if(ordexports)
    	free(ordexports);
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::init(ULONG reservedMem)
{
 LPVOID win32file = NULL;
 ULONG  filesize, ulRead, ulNewPos;
 PIMAGE_SECTION_HEADER psh;
 IMAGE_SECTION_HEADER sh;
 IMAGE_TLS_DIRECTORY *tlsDir = NULL;
 int    nSections, i;
 char   szFullPath[CCHMAXPATH] = "";
 IMAGE_DOS_HEADER doshdr;
 ULONG  signature;

  hFile = OSLibDosOpen(szFileName, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);

  //default error:
  strcpy(szErrorModule, OSLibStripPath(szFileName));
  if(hFile == NULL) {
    	goto failure;
  }
  //read dos header
  if(DosRead(hFile, (LPVOID)&doshdr, sizeof(doshdr), &ulRead)) {
    	goto failure;
  }
  if(OSLibDosSetFilePtr(hFile, doshdr.e_lfanew, OSLIB_SETPTR_FILE_BEGIN) == -1) {
    	goto failure;
  }
  //read signature dword
  if(DosRead(hFile, (LPVOID)&signature, sizeof(signature), &ulRead)) {
    	goto failure;
  }
  //read pe header
  if(DosRead(hFile, (LPVOID)&fh, sizeof(fh), &ulRead)) {
    	goto failure;
  }
  //read optional header
  if(DosRead(hFile, (LPVOID)&oh, sizeof(oh), &ulRead)) {
    	goto failure;
  }
  if(doshdr.e_magic != IMAGE_DOS_SIGNATURE || signature != IMAGE_NT_SIGNATURE) {
    	fout << "Not a valid PE file (probably a 16 bits windows exe/dll)!" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szPEErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    	goto failure;
  }

  if(oh.SizeOfImage == 0) {//just in case
	oh.SizeOfImage = OSLibDosGetFileSize(hFile);
  }

  imageSize = oh.SizeOfImage;
  //Allocate memory told hold the entire image
  if(allocSections(reservedMem) == FALSE) {
    	fout << "Failed to allocate image memory, rc " << errorState << endl;
    	goto failure;
  }

  memmap = new Win32MemMap(this, realBaseAddress, imageSize);
  if(memmap == NULL || !memmap->Init(0)) {
        strcpy(szErrorModule, OSLibStripPath(szFileName));
    	goto failure;
  }
  win32file = memmap->mapViewOfFile(0, 0, 2);

  if(DosQueryPathInfo(szFileName, FIL_QUERYFULLNAME, szFullPath, sizeof(szFullPath)) == 0) {
	setFullPath(szFullPath);
  }

  if(!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)) {//not valid
    	fout << "Not a valid PE file!" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szPEErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    	goto failure;
  }
  if(fh.Machine != IMAGE_FILE_MACHINE_I386) {
    	fout << "You need a REAL CPU to run this code" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szCPUErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    	goto failure;
  }
  //IMAGE_FILE_SYSTEM == only drivers (device/file system/video etc)?
  if(fh.Characteristics & IMAGE_FILE_SYSTEM) {
    	fout << "Can't convert system files" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szExeErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    	goto failure;
  }

  if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
    	fout << "No fixups, might not run!" << endl;
  }

  fout << "PE file           : " << szFileName << endl;
  fout << "PE Optional header: " << endl;
  fout << "Preferred address : " << oh.ImageBase << endl;
  fout << "Base Of Code      : " << oh.BaseOfCode << endl;
  fout << "CodeSize          : " << oh.SizeOfCode << endl;
  fout << "Base Of Data      : " << oh.BaseOfData << endl;
  fout << "Data Size (uninit): " << oh.SizeOfUninitializedData << endl;
  fout << "Data Size (init)  : " << oh.SizeOfInitializedData << endl;
  fout << "Entry Point       : " << oh.AddressOfEntryPoint << endl;
  fout << "Section Alignment : " << oh.SectionAlignment << endl;
  fout << "Stack Reserve size: " << oh.SizeOfStackReserve << endl;
  fout << "Stack Commit size : " << oh.SizeOfStackCommit << endl;
  fout << "SizeOfHeapReserve : " << oh.SizeOfHeapReserve << endl;
  fout << "SizeOfHeapCommit  : " << oh.SizeOfHeapCommit << endl;
  fout << "FileAlignment     : " << oh.FileAlignment << endl;
  fout << "Subsystem         : " << oh.Subsystem << endl;
  fout << "Image Size        : " << oh.SizeOfImage << endl;
  fout << "Header Size       : " << oh.SizeOfHeaders << endl;

  //get header page
  commitPage(realBaseAddress, FALSE);

  nSections = NR_SECTIONS(win32file);

  if(loadType == REAL_LOAD) 
  {
   imageSize = 0;
   if ((psh = (PIMAGE_SECTION_HEADER)SECTIONHDROFF (win32file)) != NULL) {
    fout << endl << "*************************PE SECTIONS START**************************" << endl;
    for (i=0; i<nSections; i++) {
        fout << "Raw data size:       " << hex(psh[i].SizeOfRawData) << endl;
        fout << "Virtual Address:     " << hex(psh[i].VirtualAddress) << endl;
        fout << "Virtual Size:        " << hex(psh[i].Misc.VirtualSize) << endl;
        fout << "Pointer to raw data: " << hex(psh[i].PointerToRawData) << endl;
        fout << "Section flags:       " << hex(psh[i].Characteristics) << endl << endl;
        if(strcmp(psh[i].Name, ".reloc") == 0) {
            fout << ".reloc" << endl << endl;
            addSection(SECTION_RELOC, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(strcmp(psh[i].Name, ".edata") == 0) {
            fout << ".edata" << endl << endl;
            addSection(SECTION_EXPORT, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(strcmp(psh[i].Name, ".rsrc") == 0) {
            fout << ".rsrc" << endl << endl;
            addSection(SECTION_RESOURCE, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
 	if(strcmp(psh[i].Name, ".tls") == 0)
	{
		tlsDir = (IMAGE_TLS_DIRECTORY *)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_TLS);
		if(tlsDir) {
			fout << "TLS Directory" << endl;
			fout << "TLS Address of Index     " << hex((ULONG)tlsDir->AddressOfIndex) << endl;
			fout << "TLS Address of Callbacks " << hex((ULONG)tlsDir->AddressOfCallBacks) << endl;
			fout << "TLS SizeOfZeroFill       " << hex(tlsDir->SizeOfZeroFill) << endl;
			fout << "TLS Characteristics      " << hex(tlsDir->Characteristics) << endl;
		        addSection(SECTION_TLS, psh[i].PointerToRawData,
                		   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   		   psh[i].Misc.VirtualSize);
		}
		continue;
	}

        if(strcmp(psh[i].Name, ".debug") == 0) {
            fout << ".rdebug" << endl << endl;
            addSection(SECTION_DEBUG,  psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
	if(IsImportSection(win32file, &psh[i]))
    	{
          int type = SECTION_IMPORT;
            fout << "Import Data Section" << endl << endl;
            if(psh[i].Characteristics & IMAGE_SCN_CNT_CODE) {
                fout << "Also Code Section" << endl << endl;
                type |= SECTION_CODE;
            }
            addSection(type, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }

            //KSO Sun 1998-08-09: Borland does not alway set the CODE flag for its "CODE" section
        if(  psh[i].Characteristics & IMAGE_SCN_CNT_CODE ||
            (psh[i].Characteristics & IMAGE_SCN_MEM_EXECUTE &&
               !(psh[i].Characteristics & (IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_CNT_INITIALIZED_DATA))) //KSO: make sure its not marked as a datasection
            )
        {
            fout << "Code Section" << endl << endl;
            addSection(SECTION_CODE, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(!(psh[i].Characteristics & IMAGE_SCN_MEM_WRITE)) { //read only data section
            fout << "Read Only Data Section" << endl << endl;
            addSection(SECTION_READONLYDATA, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(psh[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
            fout << "Uninitialized Data Section" << endl << endl;
            addSection(SECTION_UNINITDATA, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(psh[i].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
            fout << "Initialized Data Section" << endl << endl;
            addSection(SECTION_INITDATA, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(psh[i].Characteristics & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ)) {
            fout << "Other Section, stored as read/write uninit data" << endl << endl;
            addSection(SECTION_UNINITDATA, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        fout << "Unknown section" << endl;
    	goto failure;
     }
   }
  }
  else {
  	if(GetSectionHdrByName (win32file, &sh, ".rsrc")) 
        {
            addSection(SECTION_RESOURCE, sh.PointerToRawData,
                       sh.SizeOfRawData, sh.VirtualAddress + oh.ImageBase,
                       sh.Misc.VirtualSize);
  	}
  }
  fout << "*************************PE SECTIONS END **************************" << endl;
  imageSize += imageVirtBase - oh.ImageBase;
  fout << "Total size of Image " << imageSize << endl;
  fout << "imageVirtBase       " << imageVirtBase << endl;
  fout << "imageVirtEnd        " << imageVirtEnd << endl;

  //In case there are any gaps between sections, adjust size
  if(imageSize != imageVirtEnd - oh.ImageBase) {
    	fout << "imageSize != imageVirtEnd - oh.ImageBase!" << endl;
    	imageSize = imageVirtEnd - oh.ImageBase;
  }
  if(imageSize < oh.SizeOfImage) { 
	imageSize = oh.SizeOfImage;
  }

  fout << "OS/2 base address " << realBaseAddress << endl;
  if(oh.AddressOfEntryPoint) {
  	entryPoint = realBaseAddress + oh.AddressOfEntryPoint;
  }
  else {
	fout << "EntryPoint == NULL" << endl; 
	entryPoint = NULL;
  }

  //set memory protection flags
  if(setMemFlags() == FALSE) {
    	fout << "Failed to set memory protection" << endl;
    	goto failure;
  }

  if(loadType == REAL_LOAD) 
  {
   if(tlsDir != NULL) {
    Section *sect = findSection(SECTION_TLS);

	if(sect == NULL) {
		fout << "Couldn't find TLS section!!" << endl;
	    	goto failure;
	}
  	setTLSAddress((char *)sect->realvirtaddr);
  	setTLSInitSize(tlsDir->EndAddressOfRawData - tlsDir->StartAddressOfRawData);
  	setTLSTotalSize(tlsDir->EndAddressOfRawData - tlsDir->StartAddressOfRawData + tlsDir->SizeOfZeroFill);

	sect = findSectionByAddr((ULONG)tlsDir->AddressOfIndex);
	if(sect == NULL) {
		fout << "Couldn't find TLS AddressOfIndex section!!" << endl;
	    	goto failure;
	}
  	setTLSIndexAddr((LPDWORD)(sect->realvirtaddr + ((ULONG)tlsDir->AddressOfIndex - sect->virtaddr)));

	sect = findSectionByAddr((ULONG)tlsDir->AddressOfCallBacks);
	if(sect == NULL) {
		fout << "Couldn't find TLS AddressOfCallBacks section!!" << endl;
	    	goto failure;
	}
  	setTLSCallBackAddr((PIMAGE_TLS_CALLBACK *)(sect->realvirtaddr + ((ULONG)tlsDir->AddressOfCallBacks - sect->virtaddr)));
   }

   if(realBaseAddress != oh.ImageBase) {
	pFixups = (PIMAGE_BASE_RELOCATION)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	commitPage((ULONG)pFixups, FALSE);
   }
   if(fh.Characteristics & IMAGE_FILE_DLL) {
    	if(processExports((char *)win32file) == FALSE) {
        	fout << "Failed to process exported apis" << endl;
	    	goto failure;
    	}
   }
  }

  for (i=0; i<nSections; i++) {
	commitPage((ULONG)section[i].realvirtaddr, FALSE, COMPLETE_SECTION);
  }

  //SvL: Use pointer to image header as module handle now. Some apps needs this
  hinstance = (HINSTANCE)realBaseAddress;

  //PH: get pResDir pointer correct first, since processImports may
  //    implicitly call functions depending on it.
  if(GetSectionHdrByName (win32file, &sh, ".rsrc")) {
    	//get offset in resource object of directory entry
	pResDir = (PIMAGE_RESOURCE_DIRECTORY)(sh.VirtualAddress + realBaseAddress);
        pResourceSectionStart = sh.VirtualAddress;
  }

  if (loadType == REAL_LOAD) 
  {
   if(processImports((char *)win32file) == FALSE) {
    	fout << "Failed to process imports!" << endl;
     	goto failure;
   }
  }
 
  return(TRUE);
failure:
  if(memmap) {
	delete memmap;
	memmap = NULL;
  }
  if(hFile) {
  	OSLibDosClose(hFile);
	hFile = 0;
  }
  errorState = ERROR_INTERNAL;
  return FALSE;
}
//******************************************************************************
//commits image page(s) when an access violation exception is dispatched
//virtAddress = address of exception (rounded down to page boundary)
//******************************************************************************
BOOL Win32PeLdrImage::commitPage(ULONG virtAddress, BOOL fWriteAccess, int fPageCmd)
{
 Section *section;
 ULONG    offset, size, sectionsize, protflags, fileoffset, range, attr;
 ULONG    ulNewPos, ulRead;
 APIRET   rc;

  rc = DosQueryMem((PVOID)virtAddress, &range, &attr);
  if(rc) {
	dprintf(("Win32PeLdrImage::commitPage: DosQueryMem returned %d", rc));
	return FALSE;
  }
  if(attr & PAG_COMMIT) {
	dprintf(("Win32PeLdrImage::commitPage: Memory at 0x%x already committed!", virtAddress));
	return FALSE;
  }

  section = findSectionByOS2Addr(virtAddress);
  if(section == NULL) {
	size        = 4096;
	sectionsize = 4096;
	protflags   = PAG_READ|PAG_WRITE; //readonly?
	section = findPreviousSectionByOS2Addr(virtAddress);
  	if(section == NULL) {//access to header
		fileoffset = virtAddress - realBaseAddress;
	}
	else {
		offset = virtAddress - (section->realvirtaddr + section->virtualsize);
		fileoffset = section->rawoffset + section->rawsize + offset;
	}
  }  
  else {
  	protflags   = section->pageflags;
  	offset      = virtAddress - section->realvirtaddr;
  	sectionsize = section->virtualsize - offset;
  	if(offset > section->rawsize) {
		//unintialized data (set to 0)
		size = 0;
		fileoffset = -1;
	}  
  	else {
		size = section->rawsize-offset;
  		fileoffset = section->rawoffset + offset;
	}
  	if(fWriteAccess & !(section->pageflags & PAG_WRITE)) {
		dprintf(("Win32PeLdrImage::commitPage: No write access to 0%x!", virtAddress));
		return FALSE;
	}
  }
  if(fPageCmd == SINGLE_PAGE) {
	size = min(size, PAGE_SIZE);
	sectionsize = min(sectionsize, PAGE_SIZE);
  }
  else
  if(fPageCmd == SECTION_PAGES) {
	size = min(size, DEFAULT_NR_PAGES*PAGE_SIZE);
	sectionsize = min(sectionsize, DEFAULT_NR_PAGES*PAGE_SIZE);
  }
  size = min(size, range);
  sectionsize = min(sectionsize, range);

  if(fileoffset != -1) {
	rc = DosSetMem((PVOID)virtAddress, sectionsize, PAG_READ|PAG_WRITE|PAG_COMMIT);
	if(rc) {
		dprintf(("Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
    		return FALSE;
	}

  	if(DosSetFilePtr(hFile, fileoffset, FILE_BEGIN, &ulNewPos) == -1) {
		dprintf(("Win32PeLdrImage::commitPage: DosSetFilePtr failed for 0x%x!", fileoffset));
    		return FALSE;
  	}
  	if(DosRead(hFile, (PVOID)virtAddress, size, &ulRead)) {
		dprintf(("Win32PeLdrImage::commitPage: DosRead failed for 0x%x!", virtAddress));
    		return FALSE;
  	}
	if(ulRead != size) {
		dprintf(("Win32PeLdrImage::commitPage: DosRead failed to read %x (%x) bytes for 0x%x!", size, ulRead, virtAddress));
    		return FALSE;
	}
   	if(realBaseAddress != oh.ImageBase) {
		setFixups(virtAddress, sectionsize);
	}

  	rc = DosSetMem((PVOID)virtAddress, sectionsize, protflags);
  	if(rc) {
		dprintf(("Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
	  	return FALSE;
	}
  }
  else {
	rc = DosSetMem((PVOID)virtAddress, sectionsize, PAG_READ|PAG_WRITE|PAG_COMMIT);
	if(rc) {
		dprintf(("Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
    		return FALSE;
	}
   	if(realBaseAddress != oh.ImageBase) {
		setFixups(virtAddress, sectionsize);
	}
  	rc = DosSetMem((PVOID)virtAddress, sectionsize, protflags);
  	if(rc) {
		dprintf(("Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
	  	return FALSE;
	}
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::addSection(ULONG type, ULONG rawoffset, ULONG rawsize, ULONG virtaddress, ULONG virtsize)
{
  virtsize = max(rawsize, virtsize);

  section[nrsections].rawoffset      = rawoffset;
  section[nrsections].type           = type;
  section[nrsections].rawsize        = rawsize;
  section[nrsections].virtaddr       = virtaddress;

  virtsize   = ((virtsize - 1) & ~0xFFF) + PAGE_SIZE;
  imageSize += virtsize;
  section[nrsections].virtualsize = virtsize;

  if(virtaddress < imageVirtBase)
    	imageVirtBase = virtaddress;
  if(virtaddress + virtsize > imageVirtEnd)
    	imageVirtEnd  = virtaddress + virtsize;

  nrsections++;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::allocSections(ULONG reservedMem)
{
 APIRET rc;
 ULONG  baseAddress;

  if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
    	return allocFixedMem(reservedMem);
  }
  rc = DosAllocMem((PPVOID)&baseAddress, imageSize, PAG_READ | PAG_WRITE | flAllocMem);
  if(rc) {
	dprintf(("Win32PeLdrImage::allocSections, DosAllocMem returned %d", rc));
    	errorState = rc;
    	return(FALSE);
  }
  realBaseAddress = baseAddress;
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
Section *Win32PeLdrImage::findSection(ULONG type)
{
  for(int i=0;i<nrsections;i++) {
	if(section[i].type == type) {
		return &section[i];
	}
  }
  return NULL;
}
//******************************************************************************
//******************************************************************************
Section *Win32PeLdrImage::findSectionByAddr(ULONG addr)
{
  for(int i=0;i<nrsections;i++) {
	if(section[i].virtaddr <= addr && section[i].virtaddr + section[i].virtualsize > addr) {
		return &section[i];
	}
  }
  return NULL;
}
//******************************************************************************
//******************************************************************************
Section *Win32PeLdrImage::findSectionByOS2Addr(ULONG addr)
{
  for(int i=0;i<nrsections;i++) {
	if(section[i].realvirtaddr <= addr && section[i].realvirtaddr + section[i].virtualsize > addr) {
		return &section[i];
	}
  }
  return NULL;
}
//******************************************************************************
//******************************************************************************
Section *Win32PeLdrImage::findPreviousSectionByOS2Addr(ULONG addr)
{
 ULONG lowestAddr = 0xffffffff;
 ULONG index = -1;

  for(int i=0;i<nrsections;i++) {
	if(section[i].realvirtaddr > addr) {
		if(section[i].realvirtaddr < lowestAddr) {
			lowestAddr = section[i].realvirtaddr;
			index = i;
		}
	}
  }
  if(index == -1) 
  	return NULL;

  return &section[index];
}
//******************************************************************************
#define FALLOC_SIZE (1024*1024)
//NOTE: Needs testing (while loop)
//TODO: Free unused (parts of) reservedMem
//******************************************************************************
BOOL Win32PeLdrImage::allocFixedMem(ULONG reservedMem)
{
 ULONG  address = 0;
 ULONG  *memallocs;
 ULONG  alloccnt = 0;
 ULONG  diff, i, baseAddress;
 APIRET rc;
 BOOL   allocFlags = flAllocMem;

  realBaseAddress = 0;

  if(reservedMem && reservedMem <= oh.ImageBase &&
     ((oh.ImageBase - reservedMem) + imageSize < PELDR_RESERVEDMEMSIZE))
  {
	//ok, it fits perfectly
       	realBaseAddress = oh.ImageBase;
	return TRUE;
  }

  //Reserve enough space to store 4096 pointers to 1MB memory chunks
  memallocs = (ULONG *)malloc(4096*sizeof(ULONG *));
  if(memallocs == NULL) {
	fout << "allocFixedMem: MALLOC FAILED for memallocs" << endl;
	return FALSE;
  }

  if(oh.ImageBase < 512*1024*124) {
	allocFlags = 0;
  }
  while(TRUE) {
    	rc = DosAllocMem((PPVOID)&address, FALLOC_SIZE, PAG_READ | allocFlags);
    	if(rc) break;

    	fout << "DosAllocMem returned " << address << endl;
    	if(address + FALLOC_SIZE >= oh.ImageBase) {
        	if(address > oh.ImageBase) {//we've passed it!
            		DosFreeMem((PVOID)address);
            		break;
        	}
        	//found the right address
        	DosFreeMem((PVOID)address);

        	diff = oh.ImageBase - address;
        	if(diff) {
            		rc = DosAllocMem((PPVOID)&address, diff, PAG_READ | allocFlags);
            		if(rc) break;
        	}
        	rc = DosAllocMem((PPVOID)&baseAddress, imageSize, PAG_READ | PAG_WRITE | allocFlags);
        	if(rc) break;

        	if(diff) DosFreeMem((PVOID)address);

        	realBaseAddress = baseAddress;
        	break;
    	}
	memallocs[alloccnt++] = address;
  }
  for(i=0;i<alloccnt;i++) {
    	DosFreeMem((PVOID)memallocs[i]);
  }
  free(memallocs);

  if(realBaseAddress == 0) //Let me guess.. MS Office app?
    	return(FALSE);

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::setMemFlags()
{
 int i;
 WINIMAGE_LOOKUP *imgLookup;

  imgLookup = WINIMAGE_LOOKUPADDR(realBaseAddress);
  imgLookup->magic1 = MAGIC_WINIMAGE;
  imgLookup->image  = this;
  imgLookup->magic2 = MAGIC_WINIMAGE;

  // Process all the image sections
  for(i=0;i<nrsections;i++) {
    	section[i].realvirtaddr = realBaseAddress + (section[i].virtaddr - oh.ImageBase);
  }

  for(i=0;i<nrsections;i++) {
    	switch(section[i].type) 
	{
        case SECTION_CODE:
        case (SECTION_CODE | SECTION_IMPORT):
            	section[i].pageflags = PAG_EXECUTE | PAG_READ;
            	break;
        case SECTION_INITDATA:
        case SECTION_UNINITDATA:
        case SECTION_IMPORT: //TODO: read only?
            	section[i].pageflags = PAG_WRITE | PAG_READ;
            	break;
        case SECTION_READONLYDATA:
        case SECTION_RESOURCE:
	case SECTION_TLS:
        default:
            	section[i].pageflags = PAG_READ;
            	break;
    	}
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::setFixups(ULONG virtAddress, ULONG size)
{
 int   i, j;
 char *page;
 ULONG count, newpage;
 Section *section;
 PIMAGE_BASE_RELOCATION prel = pFixups;

  if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
    	return(TRUE);
  }

  virtAddress -= realBaseAddress;

  if(prel) {
 	j = 1;
	while(prel->VirtualAddress && prel->VirtualAddress < virtAddress) {
      		prel = (PIMAGE_BASE_RELOCATION)((char*)prel + prel->SizeOfBlock);
	}
    	while(prel->VirtualAddress && prel->VirtualAddress < virtAddress + size) {
      		page = (char *)((char *)prel + (ULONG)prel->VirtualAddress);
      		count  = (prel->SizeOfBlock - 8)/2;
      		j++;
      		for(i=0;i<count;i++) {
		        int type   = prel->TypeOffset[i] >> 12;
		        int offset = prel->TypeOffset[i] & 0xFFF;
			int fixupsize = 0;

		        switch(type) 
			{
		        case IMAGE_REL_BASED_HIGHLOW:
				fixupsize = 4;
				break;
            		case IMAGE_REL_BASED_HIGH:
            		case IMAGE_REL_BASED_LOW:
				fixupsize = 2;
				break;
			}			
			//If the fixup crosses the final page boundary, 
			//then we have to load another page 
			if(prel->VirtualAddress + offset + fixupsize > virtAddress + size) 
			{
				newpage  = realBaseAddress + prel->VirtualAddress + offset + fixupsize;
				newpage &= ~0xFFF;

    				section  = findSectionByOS2Addr(newpage);
    				if(section == NULL) {
					//should never happen
					dprintf(("::setFixups -> section == NULL!!"));
					return FALSE;
    				}		
				//SvL: Read page from disk
				commitPage(newpage, TRUE, SINGLE_PAGE);

    				//SvL: Enable write access
    				DosSetMem((PVOID)newpage, PAGE_SIZE, PAG_READ|PAG_WRITE);
			}

		        switch(type) 
			{
		        case IMAGE_REL_BASED_ABSOLUTE:
		        	break;  //skip
		        case IMAGE_REL_BASED_HIGHLOW:
		                AddOff32Fixup(prel->VirtualAddress + offset);
		                break;
            		case IMAGE_REL_BASED_HIGH:
				AddOff16Fixup(prel->VirtualAddress + offset, TRUE);
				break;
            		case IMAGE_REL_BASED_LOW:
				AddOff16Fixup(prel->VirtualAddress + offset, FALSE);
				break;
            		case IMAGE_REL_BASED_HIGHADJ:
            		case IMAGE_REL_BASED_MIPS_JMPADDR:
       			default:
                		break;
        		}
			if(prel->VirtualAddress + offset + fixupsize > virtAddress + size) 
			{
    				//SvL: Restore original page protection flags
    				DosSetMem((PVOID)newpage, PAGE_SIZE, section->pageflags);
			}

      		}
      		prel = (PIMAGE_BASE_RELOCATION)((char*)prel + prel->SizeOfBlock);
    	}//while
  }
  else {
	dprintf(("Win32PeLdrImage::setFixups, no fixups at %x, %d", virtAddress, size));
    	return(FALSE);
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::setFixups(PIMAGE_BASE_RELOCATION prel)
{
 int   i, j;
 char *page;
 ULONG count;

  if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
    	return(TRUE);
  }

  if(prel) {
        j = 1;
    while(prel->VirtualAddress) {
      page = (char *)((char *)prel + (ULONG)prel->VirtualAddress);
      count  = (prel->SizeOfBlock - 8)/2;
      fout.setf(ios::hex, ios::basefield);
      fout << "Page " << j << " Address " << (ULONG)prel->VirtualAddress << " Count " << count << endl;
      fout.setf(ios::dec, ios::basefield);
      j++;
      for(i=0;i<count;i++) {
        int type   = prel->TypeOffset[i] >> 12;
        int offset = prel->TypeOffset[i] & 0xFFF;
        switch(type) {
            case IMAGE_REL_BASED_ABSOLUTE:
////                fout << "absolute fixup; unused" << endl;
                break;  //skip
            case IMAGE_REL_BASED_HIGHLOW:
////                fout << "address " << offset << " type " << type << endl;
                AddOff32Fixup(prel->VirtualAddress + offset);
                break;
            case IMAGE_REL_BASED_HIGH:
		AddOff16Fixup(prel->VirtualAddress + offset, TRUE);
		break;
            case IMAGE_REL_BASED_LOW:
		AddOff16Fixup(prel->VirtualAddress + offset, FALSE);
		break;
            case IMAGE_REL_BASED_HIGHADJ:
            case IMAGE_REL_BASED_MIPS_JMPADDR:
            default:
                fout << "Unknown/unsupported fixup type!" << endl;
                break;
        }
      }
      prel = (PIMAGE_BASE_RELOCATION)((char*)prel + prel->SizeOfBlock);
    }//while
  }
  else {
    fout << "No internal fixups found!\n" << endl;
    errorState = ERROR_INTERNAL;
    return(FALSE);
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::AddOff32Fixup(ULONG fixupaddr)
{
 ULONG orgaddr;
 ULONG *fixup;

  fixup   = (ULONG *)(fixupaddr + realBaseAddress);
  orgaddr = *fixup;
  *fixup  = realBaseAddress + (*fixup - oh.ImageBase);
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::AddOff16Fixup(ULONG fixupaddr, BOOL fHighFixup)
{
 ULONG   orgaddr;
 USHORT *fixup;

  fixup   = (USHORT *)(fixupaddr + realBaseAddress);
  orgaddr = *fixup;
  if(fHighFixup) {
  	*fixup  += (USHORT)((realBaseAddress - oh.ImageBase) >> 16);
  }
  else {
  	*fixup  += (USHORT)((realBaseAddress - oh.ImageBase) & 0xFFFF);
  }
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::StoreImportByOrd(Win32DllBase *WinDll, ULONG ordinal, ULONG impaddr)
{
 ULONG *import;
 ULONG  apiaddr;

  import  = (ULONG *)impaddr;
  apiaddr = WinDll->getApi(ordinal);
  if(apiaddr == 0)
  {
    	dprintf(("KERNEL32:Win32PeLdrImage - %s.%u not found\n",
                 WinDll->getName(),
                 ordinal));

    	fout << "--->>> NOT FOUND!" << endl;
    	*import = (ULONG)MissingApi;
  }
  else  *import = apiaddr;
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::StoreImportByName(Win32DllBase *WinDll, char *impname, ULONG impaddr)
{
 ULONG *import;
 ULONG  apiaddr;

  import = (ULONG *)impaddr;
  apiaddr = WinDll->getApi(impname);
  if(apiaddr == 0)
  {
    	dprintf(("KERNEL32:Win32PeLdrImage - %s.%s not found\n",
                WinDll->getName(),
                impname));

    	fout << "--->>> NOT FOUND!" << endl;
    	*import = (ULONG)MissingApi;
  }
  else  *import = apiaddr;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::processExports(char *win32file)
{
 IMAGE_SECTION_HEADER    sh;
 PIMAGE_EXPORT_DIRECTORY ped;
 ULONG *ptrNames, *ptrAddress;
 USHORT *ptrOrd;
 int i;

  /* get section header and pointer to data directory for .edata section */
  if((ped = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryOffset
     (win32file, IMAGE_DIRECTORY_ENTRY_EXPORT)) != NULL &&
     GetSectionHdrByImageDir(win32file, IMAGE_DIRECTORY_ENTRY_EXPORT, &sh) ) {

        fout << "Exported Functions: " << endl;
    ptrOrd     = (USHORT *)((ULONG)ped->AddressOfNameOrdinals +
                            (ULONG)win32file);
    ptrNames   = (ULONG *)((ULONG)ped->AddressOfNames +
                            (ULONG)win32file);
    ptrAddress = (ULONG *)((ULONG)ped->AddressOfFunctions +
                            (ULONG)win32file);
    nrOrdExports  = ped->NumberOfFunctions;
    nrNameExports = ped->NumberOfNames;

    int   ord, RVAExport;
    char *name;
    for(i=0;i<ped->NumberOfNames;i++) {
        ord       = ptrOrd[i] + ped->Base;
        name      = (char *)((ULONG)ptrNames[i] + (ULONG)win32file);
        RVAExport = ptrAddress[ptrOrd[i]];
#ifdef FORWARDERS
        if(RVAExport < sh.VirtualAddress || RVAExport > sh.VirtualAddress + sh.SizeOfRawData) {
#endif
            //points to code (virtual address relative to oh.ImageBase
            AddNameExport(oh.ImageBase + RVAExport, name, ord);
            fout << "address 0x";
            fout.setf(ios::hex, ios::basefield);
            fout << RVAExport;
            fout.setf(ios::dec, ios::basefield);
            fout << " " << name << "@" << ord << endl;
#ifdef FORWARDERS

        }
        else {//forwarder
            char *forward = (char *)((ULONG)RVAExport + (ULONG)win32file);
            fout << RVAExport << " " << name << " @" << ord << " is forwarder to " << (int)forward << endl;
        }
#endif
    }
    for(i=0;i<max(ped->NumberOfNames,ped->NumberOfFunctions);i++) {
        ord = ped->Base + i;  //Correct??
        RVAExport = ptrAddress[i];
#ifdef FORWARDERS
        if(RVAExport < sh.VirtualAddress || RVAExport > sh.VirtualAddress + sh.SizeOfRawData) {
#endif
            if(RVAExport) {
              //points to code (virtual address relative to oh.ImageBase
              fout << "ord " << ord << " at 0x";
              fout.setf(ios::hex, ios::basefield);
              fout << RVAExport  << endl;
              fout.setf(ios::dec, ios::basefield);
              AddOrdExport(oh.ImageBase + RVAExport, ord);
            }
#ifdef FORWARDERS
        }
        else {//forwarder or empty
            char *forward = (char *)((ULONG)RVAExport + (ULONG)win32file);
            fout << "ord " << ord << " at 0x";
            fout.setf(ios::hex, ios::basefield);
            fout << RVAExport << " is forwarder to 0x" << (int)forward << endl;
            fout.setf(ios::dec, ios::basefield);
        }
#endif
    }
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::AddNameExport(ULONG virtaddr, char *apiname, ULONG ordinal)
{
 ULONG nsize;

  if(nameexports == NULL) {
    	nameExportSize= 4096;
    	nameexports   = (NameExport *)malloc(nameExportSize);
    	curnameexport = nameexports;
  }
  nsize = (ULONG)curnameexport - (ULONG)nameexports;
  if(nsize + sizeof(NameExport) + strlen(apiname) > nameExportSize) {
    	nameExportSize += 4096;
    	char *tmp = (char *)nameexports;
    	nameexports = (NameExport *)malloc(nameExportSize);
    	memcpy(nameexports, tmp, nsize);
    	curnameexport = (NameExport *)((ULONG)nameexports + nsize);
    	free(tmp);
  }
  curnameexport->virtaddr = realBaseAddress + (virtaddr - oh.ImageBase);
  curnameexport->ordinal  = ordinal;
  *(ULONG *)curnameexport->name = 0;
  strcpy(curnameexport->name, apiname);

  curnameexport->nlength = strlen(apiname) + 1;
  if(curnameexport->nlength < sizeof(curnameexport->name))
    	curnameexport->nlength = sizeof(curnameexport->name);

  curnameexport = (NameExport *)((ULONG)curnameexport->name + curnameexport->nlength);
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::AddOrdExport(ULONG virtaddr, ULONG ordinal)
{
  if(ordexports == NULL) {
    	ordexports   = (OrdExport *)malloc(nrOrdExports * sizeof(OrdExport));
    	curordexport = ordexports;
  }
  curordexport->virtaddr = realBaseAddress + (virtaddr - oh.ImageBase);
  curordexport->ordinal  = ordinal;
  curordexport++;
}
//******************************************************************************
/** All initial processing of imports is done here
 *  Should now detect most Borland styled files including the GifCon32.exe and
 *  loader32 from SoftIce. (Stupid Borland!!!)
 *
 *  knut [Jul 22 1998 2:44am]
 **/
//******************************************************************************
BOOL Win32PeLdrImage::processImports(char *win32file)
{
 PIMAGE_IMPORT_DESCRIPTOR pID;
 IMAGE_SECTION_HEADER     shID;
 IMAGE_SECTION_HEADER     shExtra = {0};
 PIMAGE_OPTIONAL_HEADER   pOH;
 int    i,j, nrPages;
 BOOL   fBorland = 0;
 int    cModules;
 char  *pszModules;
 char  *pszCurModule;
 char  *pszTmp;
 ULONG *pulImport;
 ULONG  ulCurFixup;
 int    Size;
 Win32PeLdrDll *WinDll;
 Section *section;

/* "algorithm:"
 *      1) get module names and store them
 *          a) check dwRVAModuleName is within .idata seg - if not find section
 *      2) iterate thru functions of each module
 *          a) check OriginalFirstThunk is not 0 and that it points to a RVA.
 *          b) if not a) borland-styled PE-file - ARG!!!
 *              check FirstThunk
 *          c) check OriginalFirstThunk/FirstThunk ok RVAs and find right section
 *          d) store ordinal/name import
 *      3) finished
 */

   /* 1) get module names */
   pID = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_IMPORT);
   if (pID == NULL)
        return TRUE;
   if (!GetSectionHdrByImageDir(win32file, IMAGE_DIRECTORY_ENTRY_IMPORT, &shID))
        return TRUE;

   //calc size of module list
   i = Size = cModules = 0;
   while (pID[i].Name != 0)
   {
    //test RVA inside ID-Section
    if (pID[i].Name >= shID.VirtualAddress && pID[i].Name < shID.VirtualAddress + max(shID.Misc.VirtualSize, shID.SizeOfRawData)) {
        pszTmp = (char*)(pID[i].Name + (ULONG)win32file);
    }
    else {
        //is the "Extra"-section already found or do we have to find it?
        if (pID[i].Name < shExtra.VirtualAddress || pID[i].Name >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData)) {
            if (!GetSectionHdrByRVA(win32file, &shExtra, pID[i].Name))
                 return FALSE;
        }
        pszTmp = (char*)(pID[i].Name + (ULONG)win32file);
    }
    Size += strlen(pszTmp) + 1;
    i++;
    cModules++;
  }

  pszModules = (char*)malloc(Size);
  assert(pszModules != NULL);
  j = 0;
  for (i = 0; i < cModules; i++)
  {
    //test RVA inside ID-Section
    if (pID[i].Name >= shID.VirtualAddress && pID[i].Name < shID.VirtualAddress + max(shID.Misc.VirtualSize, shID.SizeOfRawData)) {
        pszTmp = (char*)(pID[i].Name + (ULONG)win32file);
    }
    else {
        fBorland = TRUE;
        //is the "Extra"-section already found or do we have to find it?
        if (pID[i].Name < shExtra.VirtualAddress || pID[i].Name >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData))
        {
            if (GetSectionHdrByRVA(win32file, &shExtra, pID[i].Name)) {
                free(pszModules);
                return FALSE;
            }
        }
        pszTmp = (char*)(pID[i].Name + (ULONG)win32file);
    }

    strcpy(pszModules+j, pszTmp);
    j += strlen(pszTmp) + 1;
  }
  fout << endl;
  if (fBorland)
    fout << "Borland-styled PE-File." << endl;
  //Store modules
  fout << cModules << " imported Modules: " << endl;

  /* 2) functions */
  pszCurModule = pszModules;
  pOH = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF(win32file);
  for (i = 0; i < cModules; i++)
  {
    fout << "Module " << pszCurModule << endl;
    //  a) check that OriginalFirstThunk not is 0 and look for Borland-styled PE
    if (i == 0)
    {
        //heavy borland-style test - assume array of thunks is within that style does not change
        if((ULONG)pID[i].u.OriginalFirstThunk == 0 ||
           (ULONG)pID[i].u.OriginalFirstThunk < shID.VirtualAddress ||
           (ULONG)pID[i].u.OriginalFirstThunk >= shID.VirtualAddress + max(shID.Misc.VirtualSize, shID.SizeOfRawData) ||
           (ULONG)pID[i].u.OriginalFirstThunk >= pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress &&
           (ULONG)pID[i].u.OriginalFirstThunk < sizeof(*pID)*cModules + pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)
        {
            fBorland = TRUE;
        }
    }
    //light borland-style test
    if (pID[i].u.OriginalFirstThunk == 0 || fBorland) {
            pulImport = (ULONG*)pID[i].FirstThunk;
    }
    else    pulImport = (ULONG*)pID[i].u.OriginalFirstThunk;

    //  b) check if RVA ok
    if (!(pulImport > 0 && (ULONG)pulImport < pOH->SizeOfImage)) {
        fout << "Invalid RVA " << hex((ULONG)pulImport) << endl;
        break;
    }
    // check section
    if ((ULONG)pulImport < shExtra.VirtualAddress || (ULONG)pulImport >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData))
    {
        if (!GetSectionHdrByRVA(win32file, &shExtra, (ULONG)pulImport))
        {
            fout << "warning: could not find section for Thunk RVA " << hex((ULONG)pulImport) << endl;
            break;
        }
    }

    //SvL: Load dll if needed
    fout << "**********************************************************************" << endl;
    fout << "************** Import Module " << pszCurModule << endl;
    fout << "**********************************************************************" << endl;
    WinDll = (Win32PeLdrDll *)Win32DllBase::findModule(pszCurModule);

    if(WinDll == NULL)
    {  //not found, so load it
	char modname[CCHMAXPATH];
	
	strcpy(modname, pszCurModule);
  	//rename dll if necessary (i.e. OLE32 -> OLE32OS2)
  	Win32DllBase::renameDll(modname);

	if(isPEImage(modname) == FALSE)
	{//LX image, so let OS/2 do all the work for us
		APIRET rc;
  		char   szModuleFailure[CCHMAXPATH] = "";
		ULONG  hInstanceNewDll;

		char *dot = strchr(modname, '.');
		if(dot) {
			*dot = 0;
		}
		strcat(modname, ".DLL");
  		rc = DosLoadModule(szModuleFailure, sizeof(szModuleFailure), modname, (HMODULE *)&hInstanceNewDll);
  		if(rc) {
			dprintf(("DosLoadModule returned %X for %s\n", rc, szModuleFailure));
		    	sprintf(szErrorModule, "%s.DLL", szModuleFailure);
			errorState = rc;
			return(FALSE);
  		}
		WinDll = (Win32PeLdrDll *)Win32DllBase::findModule(hInstanceNewDll);
		if(WinDll == NULL) {//shouldn't happen!
			dprintf(("Just loaded the dll, but can't find it anywhere?!!?"));
			errorState = ERROR_INTERNAL;
			return(FALSE);
		}
	}
	else {
        	WinDll = new Win32PeLdrDll(modname, this);

        	if(WinDll == NULL) {
	            fout << "WinDll: Error allocating memory" << endl;
	            WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szMemErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
	            errorState = ERROR_INTERNAL;
	            return(FALSE);
	        }
	        fout << "**********************************************************************" << endl;
	        fout << "**********************     Loading Module        *********************" << endl;
	        fout << "**********************************************************************" << endl;
	        if(WinDll->init(0) == FALSE) {
	            fout << "Internal WinDll error " << WinDll->getError() << endl;
		    strcpy(szErrorModule, OSLibStripPath(modname));
	            return(FALSE);
	        }
	        if(WinDll->attachProcess() == FALSE) {
        	    fout << "attachProcess failed!" << endl;
	            errorState = ERROR_INTERNAL;
	            return(FALSE);
	        }
	    	WinDll->AddRef();
	}
	fout << "**********************************************************************" << endl;
	fout << "**********************  Finished Loading Module  *********************" << endl;
        fout << "**********************************************************************" << endl;
    }
    else    fout << "Already found " << pszCurModule << endl;

    WinDll->AddRef();

    pulImport  = (PULONG)((ULONG)pulImport + (ULONG)win32file);
    j          = 0;
    ulCurFixup = (ULONG)pID[i].FirstThunk + (ULONG)win32file;

    section    = findSectionByOS2Addr(ulCurFixup);
    if(section == NULL) {
        fout.setf(ios::hex, ios::basefield);
	fout << "Unable to find section for " << ulCurFixup << endl;
	return FALSE;
    }
    //SvL: Read page from disk
    commitPage(ulCurFixup & ~0xfff, FALSE, SINGLE_PAGE);
    //SvL: Enable write access
    DosSetMem((PVOID)(ulCurFixup & ~0xfff), PAGE_SIZE, PAG_READ|PAG_WRITE);
    nrPages    = 1;

    while (pulImport[j] != 0) {
        if (pulImport[j] & IMAGE_ORDINAL_FLAG) { //ordinal
            fout.setf(ios::hex, ios::basefield);
            fout << "0x" << ulCurFixup << " Imported function " << pszCurModule << "@" << (pulImport[j] & ~IMAGE_ORDINAL_FLAG) << endl;
            fout.setf(ios::dec, ios::basefield);
            StoreImportByOrd(WinDll, pulImport[j] & ~IMAGE_ORDINAL_FLAG, ulCurFixup);
        }
        else {  //name
            //check
            if (pulImport[j] < shExtra.VirtualAddress || pulImport[j] >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData)) {
                if (!GetSectionHdrByRVA(win32file, &shExtra, pulImport[j]))
                {
                    fout << "warning: could not find section for Import Name RVA " << hex(pulImport[j]) << endl;
                    break;
                }
            }
            //KSO - Aug 6 1998 1:15am:this eases comparing...
            char *pszFunctionName = (char*)(pulImport[j] + (ULONG)win32file + 2);
            fout.setf(ios::hex, ios::basefield);
            fout << "0x" << ulCurFixup << " Imported function " << pszFunctionName << endl;
            fout.setf(ios::dec, ios::basefield);
            StoreImportByName(WinDll, pszFunctionName, ulCurFixup);
        }
        ulCurFixup += sizeof(IMAGE_THUNK_DATA);
        j++;
	if((ulCurFixup & 0xfff) == 0) {
    		commitPage(ulCurFixup & ~0xfff, TRUE, SINGLE_PAGE);
    		DosSetMem((PVOID)(ulCurFixup & ~0xfff), PAGE_SIZE, PAG_READ|PAG_WRITE);
    		nrPages++;
	}
    }
    //SvL: And restore original protection flags
    ulCurFixup = (ULONG)pID[i].FirstThunk + pOH->ImageBase;
    DosSetMem((PVOID)(ulCurFixup & ~0xfff), PAGE_SIZE*nrPages, section->pageflags);

    fout << "**********************************************************************" << endl;
    fout << "************** End Import Module " << pszCurModule << endl;
    fout << "**********************************************************************" << endl;

    pszCurModule += strlen(pszCurModule) + 1;
    fout << endl;
  }//for (i = 0; i < cModules; i++)

  free(pszModules);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
ULONG MissingApi()
{
 static BOOL fIgnore = FALSE;
 int r;

  dprintf(("Missing api called!\n"));
  if(fIgnore)
    return(0);

  do {
    r = WinMessageBox(HWND_DESKTOP, NULLHANDLE, "The application has called a non-existing api\n",
                  "Internal Odin Error", 0, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_MOVEABLE);
  }
  while(r == MBID_RETRY); //giggle

  if( r != MBID_IGNORE )
     exit(987);

  fIgnore = TRUE;
  return(0);
}
/******************************************************************************/
/******************************************************************************/
/*heximal(decimal) KSO Sun 24.05.1998*/
char szHexBuffer[30];

char *hex(ULONG num)
{
    sprintf(szHexBuffer, "0x%+08x (%lu)",num,num);
    return szHexBuffer;
}
//******************************************************************************
//******************************************************************************
