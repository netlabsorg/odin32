/* $Id: winimagepeldr.cpp,v 1.52 2000-08-11 10:56:19 sandervl Exp $ */

/*
 * Win32 PE loader Image base class
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Check psh[i].Characteristics for more than only the code section
 * TODO: Make resource section readonly when GDI32 is fixed
 *
 * NOTE: RSRC_LOAD is a special flag to only load the resource directory
 *       of a PE image. Processing imports, sections etc is not done.
 *       Nor is it put into the linked list of dlls (if it's a dll).
 *       This is useful for GetVersionSize/Resource in case it wants to
 *       get version info of an image that is not loaded.
 *       So an instance of this type can't be used for anything but resource lookup!
 *
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
//use a different logfile
#define PRIVATE_LOGGING
#include <misc.h>
#include <win32api.h>
#include "winimagebase.h"
#include "winimagepeldr.h"
#include "windllpeldr.h"
#include "windlllx.h"
#include <pefile.h>
#include <unicode.h>
#include "oslibmisc.h"
#include "initterm.h"
#include <win\virtual.h>
#include "oslibdos.h"
#include "mmap.h"
#include <wprocess.h>

//Define COMMIT_ALL to let the pe loader commit all sections of the image
//This is very useful during debugging as you'll get lots of exceptions
//otherwise.
#define COMMIT_ALL

char szErrorTitle[]     = "Odin";
char szMemErrorMsg[]    = "Memory allocation failure";
char szFileErrorMsg[]   = "File IO error";
char szPEErrorMsg[]     = "Not a valid win32 exe. (perhaps 16 bits windows)";
char szCPUErrorMsg[]    = "Executable doesn't run on x86 machines";
char szExeErrorMsg[]    = "File isn't an executable";
char szInteralErrorMsg[]= "Internal Error";
char szErrorModule[128] = "";

#ifdef DEBUG
static FILE *_privateLogFile = NULL;
#endif

ULONG MissingApi();

//******************************************************************************
//******************************************************************************
void OpenPrivateLogFilePE()
{
#ifdef DEBUG
 char logname[CCHMAXPATH];

	sprintf(logname, "pe_%d.log", loadNr);
    	_privateLogFile = fopen(logname, "w");
	if(_privateLogFile == NULL) {
		sprintf(logname, "%spe_%d.log", kernel32Path, loadNr);
    		_privateLogFile = fopen(logname, "w");
	}
	dprintfGlobal(("PE LOGFILE : %s", logname));
#endif
}
//******************************************************************************
//******************************************************************************
void ClosePrivateLogFilePE()
{
#ifdef DEBUG
  	if(_privateLogFile) {
		fclose(_privateLogFile);
		_privateLogFile = NULL;
	}
#endif
}
//******************************************************************************
//******************************************************************************
Win32PeLdrImage::Win32PeLdrImage(char *pszFileName, BOOL isExe, int loadtype) :
    Win32ImageBase(-1),
    nrsections(0), imageSize(0),
    imageVirtBase(-1), realBaseAddress(0), imageVirtEnd(0),
    nrNameExports(0), nrOrdExports(0), nameexports(NULL), ordexports(NULL),
    memmap(NULL), pFixups(NULL), dwFixupSize(0)
{
 HFILE  dllfile;

  loadType = loadtype;

  strcpy(szFileName, pszFileName);
  strupr(szFileName);
  if(isExe) {
	if(!strchr(szFileName, '.')) {
		strcat(szFileName,".EXE");
	}
  	dllfile = OSLibDosOpen(szFileName, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
  	if(dllfile == NULL) {
		if(!strstr(szFileName, ".EXE")) {
			strcat(szFileName,".EXE");
		}
  		dllfile = OSLibDosOpen(szFileName, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
  		if(dllfile == NULL) {
			OSLibDosSearchPath(OSLIB_SEARCHENV, "PATH", szFileName, szFileName, sizeof(szFileName));
		}
	}
  	else	OSLibDosClose(dllfile);
  }
  else {
  	findDll(szFileName, szModule, sizeof(szModule));
	strcpy(szFileName, szModule);
  }
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
    	dprintf((LOG, "Not a valid PE file (probably a 16 bits windows exe/dll)!"));
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szPEErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    	goto failure;
  }

  if(oh.SizeOfImage == 0) {//just in case
	oh.SizeOfImage = OSLibDosGetFileSize(hFile, NULL);
  }

  imageSize = oh.SizeOfImage;
  //Allocate memory to hold the entire image
  if(allocSections(reservedMem) == FALSE) {
    	dprintf((LOG, "Failed to allocate image memory for %s at %x, rc %d", szFileName, oh.ImageBase, errorState));;
    	goto failure;
  }

  memmap = new Win32MemMap(this, realBaseAddress, imageSize);
  if(memmap == NULL || !memmap->Init(0)) {
    	goto failure;
  }
  win32file = memmap->mapViewOfFile(0, 0, 2);

  if(DosQueryPathInfo(szFileName, FIL_QUERYFULLNAME, szFullPath, sizeof(szFullPath)) == 0) {
	setFullPath(szFullPath);
  }

  if(!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)) {//not valid
    	dprintf((LOG, "Not a valid PE file!"));
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szPEErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    	goto failure;
  }
  if(fh.Machine != IMAGE_FILE_MACHINE_I386) {
    	dprintf((LOG, "Doesn't run on x86 processors!"));
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szCPUErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    	goto failure;
  }
  //IMAGE_FILE_SYSTEM == only drivers (device/file system/video etc)?
  if(fh.Characteristics & IMAGE_FILE_SYSTEM) {
    	dprintf((LOG, "Can't convert system files"));
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szExeErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    	goto failure;
  }

  if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
    	dprintf((LOG, "No fixups, might not run!"));
  }

  dprintf((LOG, "PE file           : %s", szFileName));
  dprintf((LOG, "PE Optional header: "));
  dprintf((LOG, "Preferred address : %d", oh.ImageBase ));
  dprintf((LOG, "Base Of Code      : %d", oh.BaseOfCode ));
  dprintf((LOG, "CodeSize          : %d", oh.SizeOfCode ));
  dprintf((LOG, "Base Of Data      : %d", oh.BaseOfData ));
  dprintf((LOG, "Data Size (uninit): %d", oh.SizeOfUninitializedData ));
  dprintf((LOG, "Data Size (init)  : %d", oh.SizeOfInitializedData ));
  dprintf((LOG, "Entry Point       : %d", oh.AddressOfEntryPoint ));
  dprintf((LOG, "Section Alignment : %d", oh.SectionAlignment ));
  dprintf((LOG, "Stack Reserve size: %d", oh.SizeOfStackReserve ));
  dprintf((LOG, "Stack Commit size : %d", oh.SizeOfStackCommit ));
  dprintf((LOG, "SizeOfHeapReserve : %d", oh.SizeOfHeapReserve ));
  dprintf((LOG, "SizeOfHeapCommit  : %d", oh.SizeOfHeapCommit ));
  dprintf((LOG, "FileAlignment     : %d", oh.FileAlignment ));
  dprintf((LOG, "Subsystem         : %d", oh.Subsystem ));
  dprintf((LOG, "Image Size        : %d", oh.SizeOfImage ));
  dprintf((LOG, "Header Size       : %d", oh.SizeOfHeaders ));
  dprintf((LOG, "MajorImageVersion : %d", oh.MajorImageVersion ));
  dprintf((LOG, "MinorImageVersion : %d", oh.MinorImageVersion ));

  //get header page
  commitPage(realBaseAddress, FALSE);

  nSections = NR_SECTIONS(win32file);

  if(loadType == REAL_LOAD)
  {
   imageSize = 0;
   if ((psh = (PIMAGE_SECTION_HEADER)SECTIONHDROFF (win32file)) != NULL) {
    dprintf((LOG, "*************************PE SECTIONS START**************************" ));
    for (i=0; i<nSections; i++) { 
        dprintf((LOG, "Raw data size:        %x", psh[i].SizeOfRawData ));
        dprintf((LOG, "Virtual Address:      %x", psh[i].VirtualAddress ));
        dprintf((LOG, "Virtual Address Start:%x", psh[i].VirtualAddress+oh.ImageBase ));
        dprintf((LOG, "Virtual Address End:  %x", psh[i].VirtualAddress+oh.ImageBase+psh[i].Misc.VirtualSize ));
        dprintf((LOG, "Virtual Size:         %x", psh[i].Misc.VirtualSize ));
        dprintf((LOG, "Pointer to raw data:  %x", psh[i].PointerToRawData ));
        dprintf((LOG, "Section flags:        %x\n\n", psh[i].Characteristics ));
        if(strcmp(psh[i].Name, ".reloc") == 0) {
            dprintf((LOG, ".reloc" ));
            addSection(SECTION_RELOC, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
        if(strcmp(psh[i].Name, ".edata") == 0) {
            dprintf((LOG, ".edata" ));
            addSection(SECTION_EXPORT, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
        if(strcmp(psh[i].Name, ".rsrc") == 0) {
            dprintf((LOG, ".rsrc" ));
            addSection(SECTION_RESOURCE, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
 	if(strcmp(psh[i].Name, ".tls") == 0)
	{
		tlsDir = (IMAGE_TLS_DIRECTORY *)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_TLS);
		if(tlsDir) {
		        addSection(SECTION_TLS, psh[i].PointerToRawData,
                		   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   		   psh[i].Misc.VirtualSize, psh[i].Characteristics);
		}
		continue;
	}

        if(strcmp(psh[i].Name, ".debug") == 0) {
            dprintf((LOG, ".rdebug" ));
            addSection(SECTION_DEBUG,  psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
	if(IsImportSection(win32file, &psh[i]))
    	{
          int type = SECTION_IMPORT;
            dprintf((LOG, "Import Data Section" ));
            if(psh[i].Characteristics & IMAGE_SCN_CNT_CODE) {
                dprintf((LOG, "Also Code Section"));
                type |= SECTION_CODE;
            }
            addSection(type, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }

            //KSO Sun 1998-08-09: Borland does not alway set the CODE flag for its "CODE" section
        if(  psh[i].Characteristics & IMAGE_SCN_CNT_CODE ||
            (psh[i].Characteristics & IMAGE_SCN_MEM_EXECUTE &&
               !(psh[i].Characteristics & (IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_CNT_INITIALIZED_DATA))) //KSO: make sure its not marked as a datasection
            )
        {
            dprintf((LOG, "Code Section"));
            addSection(SECTION_CODE, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
        if(!(psh[i].Characteristics & IMAGE_SCN_MEM_WRITE)) { //read only data section
            dprintf((LOG, "Read Only Data Section" ));
            addSection(SECTION_READONLYDATA, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
        if(psh[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
            dprintf((LOG, "Uninitialized Data Section" ));
            addSection(SECTION_UNINITDATA, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
        if(psh[i].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
            dprintf((LOG, "Initialized Data Section" ));
            addSection(SECTION_INITDATA, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
        if(psh[i].Characteristics & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ)) {
            dprintf((LOG, "Other Section, stored as read/write uninit data" ));
            addSection(SECTION_UNINITDATA, psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize, psh[i].Characteristics);
            continue;
        }
        dprintf((LOG, "Unknown section" ));
    	goto failure;
     }
   }
  }
  else {
  	if(GetSectionHdrByName (win32file, &sh, ".rsrc"))
        {
            addSection(SECTION_RESOURCE, sh.PointerToRawData,
                       sh.SizeOfRawData, sh.VirtualAddress + oh.ImageBase,
                       sh.Misc.VirtualSize, sh.Characteristics);
  	}
  }
  dprintf((LOG, "*************************PE SECTIONS END **************************" ));
  imageSize += imageVirtBase - oh.ImageBase;
  dprintf((LOG, "Total size of Image %x", imageSize ));
  dprintf((LOG, "imageVirtBase       %x", imageVirtBase ));
  dprintf((LOG, "imageVirtEnd        %x", imageVirtEnd ));

  //In case there are any gaps between sections, adjust size
  if(imageSize != imageVirtEnd - oh.ImageBase) {
    	dprintf((LOG, "imageSize != imageVirtEnd - oh.ImageBase!" ));
    	imageSize = imageVirtEnd - oh.ImageBase;
  }
  if(imageSize < oh.SizeOfImage) {
	imageSize = oh.SizeOfImage;
  }

  dprintf((LOG, "OS/2 base address %x", realBaseAddress ));
  if(oh.AddressOfEntryPoint) {
  	entryPoint = realBaseAddress + oh.AddressOfEntryPoint;
  }
  else {
	dprintf((LOG, "EntryPoint == NULL" ));
	entryPoint = NULL;
  }

  //set memory protection flags
  if(setMemFlags() == FALSE) {
    	dprintf((LOG, "Failed to set memory protection" ));
    	goto failure;
  }

  if(loadType == REAL_LOAD)
  {
   if(tlsDir != NULL) {
    Section *sect = findSection(SECTION_TLS);

	if(sect == NULL) {
		dprintf((LOG, "Couldn't find TLS section!!" ));
	    	goto failure;
	}
	dprintf((LOG, "TLS Directory" ));
	dprintf((LOG, "TLS Address of Index     %x", tlsDir->AddressOfIndex ));
	dprintf((LOG, "TLS Address of Callbacks %x", tlsDir->AddressOfCallBacks ));
	dprintf((LOG, "TLS SizeOfZeroFill       %x", tlsDir->SizeOfZeroFill ));
	dprintf((LOG, "TLS Characteristics      %x", tlsDir->Characteristics ));
  	setTLSAddress((char *)sect->realvirtaddr);
  	setTLSInitSize(tlsDir->EndAddressOfRawData - tlsDir->StartAddressOfRawData);
  	setTLSTotalSize(tlsDir->EndAddressOfRawData - tlsDir->StartAddressOfRawData + tlsDir->SizeOfZeroFill);

	sect = findSectionByAddr((ULONG)tlsDir->AddressOfIndex);
	if(sect == NULL) {
		dprintf((LOG, "Couldn't find TLS AddressOfIndex section!!" ));
	    	goto failure;
	}
  	setTLSIndexAddr((LPDWORD)(sect->realvirtaddr + ((ULONG)tlsDir->AddressOfIndex - sect->virtaddr)));

	if((ULONG)tlsDir->AddressOfCallBacks != 0) {
		sect = findSectionByAddr((ULONG)tlsDir->AddressOfCallBacks);
		if(sect == NULL) {
			dprintf((LOG, "Couldn't find TLS AddressOfCallBacks section!!" ));
		    	goto failure;
		}
		setTLSCallBackAddr((PIMAGE_TLS_CALLBACK *)(sect->realvirtaddr + ((ULONG)tlsDir->AddressOfCallBacks - sect->virtaddr)));
	}
   }

   if(realBaseAddress != oh.ImageBase) {
	pFixups     = (PIMAGE_BASE_RELOCATION)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_BASERELOC);
        dwFixupSize = ImageDirectorySize(win32file, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	commitPage((ULONG)pFixups, FALSE);
   }
#ifdef COMMIT_ALL
   for (i=0; i<nSections; i++) {
	commitPage((ULONG)section[i].realvirtaddr, FALSE, COMPLETE_SECTION);
   }
#else
   for (i=0; i<nSections; i++) {
	switch(section[i].type)
	{
	case SECTION_IMPORT:
	case SECTION_RELOC:
	case SECTION_EXPORT:
		commitPage((ULONG)section[i].realvirtaddr, FALSE, COMPLETE_SECTION);
		break;
	}
   }
#endif
   if(processExports((char *)win32file) == FALSE) {
       	dprintf((LOG, "Failed to process exported apis" ));
    	goto failure;
   }
  }
#ifdef COMMIT_ALL
  else {
	commitPage((ULONG)section[0].realvirtaddr, FALSE, COMPLETE_SECTION);
  }
#endif

  //SvL: Use pointer to image header as module handle now. Some apps needs this
  hinstance = (HINSTANCE)realBaseAddress;

  //SvL: Set instance handle in process database structure
  SetPDBInstance(hinstance);

  //PH: get pResRootDir pointer correct first, since processImports may
  //    implicitly call functions depending on it.
  if(GetSectionHdrByName (win32file, &sh, ".rsrc")) {
    	//get offset in resource object of directory entry
	pResRootDir = (PIMAGE_RESOURCE_DIRECTORY)(sh.VirtualAddress + realBaseAddress);
        ulRVAResourceSection = sh.VirtualAddress;
  }

  if (loadType == REAL_LOAD)
  {
   if(processImports((char *)win32file) == FALSE) {
    	dprintf((LOG, "Failed to process imports!" ));
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

  //Round down to nearest page boundary
  virtAddress = virtAddress & ~0xFFF;
  
  section = findSectionByOS2Addr(virtAddress);
  if(section == NULL) {
	size        = 4096;
	sectionsize = 4096;
	protflags   = PAG_READ|PAG_WRITE; //readonly?
	section = findPreviousSectionByOS2Addr(virtAddress);
  	if(section == NULL) {//access to header
		offset     = 0;
		fileoffset = virtAddress - realBaseAddress;
	}
	else {
		offset     = virtAddress - (section->realvirtaddr + section->virtualsize);
		fileoffset = section->rawoffset + section->rawsize + offset;
	}
  }
  else {
  	protflags   = section->pageflags;
  	offset      = virtAddress - section->realvirtaddr;
  	sectionsize = section->virtualsize - offset;

  	if(offset > section->rawsize || section->type == SECTION_UNINITDATA) {
		//unintialized data (set to 0)
		size = 0;
		fileoffset = -1;
	}
  	else {
		size = section->rawsize-offset;
  		fileoffset = section->rawoffset + offset;
	}
  	if(fWriteAccess & !(section->pageflags & PAG_WRITE)) {
		dprintf((LOG, "Win32PeLdrImage::commitPage: No write access to 0%x!", virtAddress));
		return FALSE;
	}
  }
  //Check range of pages with the same attributes starting at virtAddress
  //(some pages might already have been loaded)
  range = sectionsize;
  rc = DosQueryMem((PVOID)virtAddress, &range, &attr);
  if(rc) {
	dprintf((LOG, "Win32PeLdrImage::commitPage: DosQueryMem for %x returned %d", virtAddress, rc));
	return FALSE;
  }
  if(attr & PAG_COMMIT) {
	dprintf((LOG, "Win32PeLdrImage::commitPage: Memory at 0x%x already committed!", virtAddress));
	return FALSE;
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
		dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
    		return FALSE;
	}

  	if(DosSetFilePtr(hFile, fileoffset, FILE_BEGIN, &ulNewPos) == -1) {
		dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetFilePtr failed for 0x%x!", fileoffset));
    		return FALSE;
  	}
  	if(DosRead(hFile, (PVOID)virtAddress, size, &ulRead)) {
		dprintf((LOG, "Win32PeLdrImage::commitPage: DosRead failed for 0x%x!", virtAddress));
    		return FALSE;
  	}
	if(ulRead != size) {
		dprintf((LOG, "Win32PeLdrImage::commitPage: DosRead failed to read %x (%x) bytes at %x for 0x%x!", size, ulRead, fileoffset, virtAddress));
    		return FALSE;
	}
	setFixups(virtAddress, sectionsize);

  	rc = DosSetMem((PVOID)virtAddress, sectionsize, protflags);
  	if(rc) {
		dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
	  	return FALSE;
	}
  }
  else {
	rc = DosSetMem((PVOID)virtAddress, sectionsize, PAG_READ|PAG_WRITE|PAG_COMMIT);
	if(rc) {
		dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
    		return FALSE;
	}
	setFixups(virtAddress, sectionsize);

  	rc = DosSetMem((PVOID)virtAddress, sectionsize, protflags);
  	if(rc) {
		dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
	  	return FALSE;
	}
  }
  return TRUE;
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::addSection(ULONG type, ULONG rawoffset, ULONG rawsize, ULONG virtaddress, ULONG virtsize, ULONG flags)
{
  virtsize = max(rawsize, virtsize);

  section[nrsections].rawoffset      = rawoffset;
  section[nrsections].type           = type;
  section[nrsections].rawsize        = rawsize;
  section[nrsections].virtaddr       = virtaddress;
  section[nrsections].flags          = flags;

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

  realBaseAddress = 0;

  //Allocated in peldr.dll
  if(reservedMem && reservedMem == oh.ImageBase) {
	realBaseAddress = oh.ImageBase;
	return TRUE;
  }

  //SvL: We don't care where the image is loaded for resource lookup
  if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED && loadType == REAL_LOAD) {
    	return allocFixedMem(reservedMem);
  }
  rc = DosAllocMem((PPVOID)&baseAddress, imageSize, PAG_READ | PAG_WRITE | flAllocMem);
  if(rc) {
	dprintf((LOG, "Win32PeLdrImage::allocSections, DosAllocMem returned %d", rc));
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

  //Reserve enough space to store 4096 pointers to 1MB memory chunks
  memallocs = (ULONG *)malloc(4096*sizeof(ULONG *));
  if(memallocs == NULL) {
	dprintf((LOG, "allocFixedMem: MALLOC FAILED for memallocs" ));
	return FALSE;
  }

  if(oh.ImageBase < 512*1024*1024) {
	allocFlags = 0;
  }
  while(TRUE) {
    	rc = DosAllocMem((PPVOID)&address, FALLOC_SIZE, PAG_READ | allocFlags);
    	if(rc) break;

    	dprintf((LOG, "DosAllocMem returned %x", address ));
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
		if(section[i].flags & IMAGE_SCN_MEM_WRITE) 
			section[i].pageflags |= PAG_WRITE;
            	break;
        case SECTION_INITDATA:
        case SECTION_UNINITDATA:
        case SECTION_IMPORT: //TODO: read only?
            	section[i].pageflags = PAG_WRITE | PAG_READ;
            	break;

        case SECTION_RESOURCE:
		//TODO: GDI32 changes some bitmap structures to avoid problems in Open32
                //      -> causes crashes if resource section is readonly
                //      -> make it readonly again when gdi32 has been rewritten
            	section[i].pageflags = PAG_WRITE | PAG_READ;
		break;

        case SECTION_READONLYDATA:
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

  if(realBaseAddress == oh.ImageBase || fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
    	return(TRUE);
  }

  virtAddress -= realBaseAddress;
  //round size to next page boundary
  size  = (size-1) & ~0xFFF;
  size += PAGE_SIZE;
 
  if(prel) {
 	j = 1;
	while(((ULONG)prel < (ULONG)pFixups+dwFixupSize) && 
              prel->VirtualAddress && prel->VirtualAddress < virtAddress) 
        {
      		prel = (PIMAGE_BASE_RELOCATION)((char*)prel + prel->SizeOfBlock);
	}
	while(((ULONG)prel < (ULONG)pFixups+dwFixupSize) && 
              prel->VirtualAddress && prel->VirtualAddress < virtAddress + size) 
        {
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
					dprintf((LOG, "::setFixups -> section == NULL!!"));
					return FALSE;
    				}
				//SvL: Read page from disk
				commitPage(newpage, FALSE, SINGLE_PAGE);

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
	dprintf((LOG, "Win32PeLdrImage::setFixups, no fixups at %x, %d", virtAddress, size));
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
      dprintf((LOG, "Page %d Address %x Count %d", j, prel->VirtualAddress, count ));
      j++;
      for(i=0;i<count;i++) {
        int type   = prel->TypeOffset[i] >> 12;
        int offset = prel->TypeOffset[i] & 0xFFF;
        switch(type) {
            case IMAGE_REL_BASED_ABSOLUTE:
////                dprintf((LOG, "absolute fixup; unused" ));
                break;  //skip
            case IMAGE_REL_BASED_HIGHLOW:
////                dprintf((LOG, "address ", offset << " type ", type ));
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
                dprintf((LOG, "Unknown/unsupported fixup type!" ));
                break;
        }
      }
      prel = (PIMAGE_BASE_RELOCATION)((char*)prel + prel->SizeOfBlock);
    }//while
  }
  else {
    dprintf((LOG, "No internal fixups found!" ));
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
//  dprintf((LOG, "AddOff32Fixup 0x%x org 0x%x -> new 0x%x", fixup, orgaddr, realBaseAddress + (*fixup - oh.ImageBase)));
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
//        dprintf((LOG, "AddOff16FixupH 0x%x org 0x%x -> new 0x%x", fixup, orgaddr, *fixup));
  }
  else {
  	*fixup  += (USHORT)((realBaseAddress - oh.ImageBase) & 0xFFFF);
//        dprintf((LOG, "AddOff16FixupL 0x%x org 0x%x -> new 0x%x", fixup, orgaddr, *fixup));
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
    	dprintf((LOG, "KERNEL32:Win32PeLdrImage - %s.%u not found\n",
                 WinDll->getName(),
                 ordinal));

    	dprintf((LOG, "--->>> NOT FOUND!" ));
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
    	dprintf((LOG, "KERNEL32:Win32PeLdrImage - %s.%s not found\n",
                WinDll->getName(),
                impname));

    	dprintf((LOG, "--->>> NOT FOUND!" ));
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

        dprintf((LOG, "Exported Functions: " ));
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
            dprintf((LOG, "address 0x%x %s @%d", RVAExport, name, ord));
#ifdef FORWARDERS

        }
        else {//forwarder
            char *forward = (char *)((ULONG)RVAExport + (ULONG)win32file);
            fout << RVAExport << " ", name << " @", ord << " is forwarder to ", (int)forward ));
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
              dprintf((LOG, "ord %d at 0x%x", ord, RVAExport));
              AddOrdExport(oh.ImageBase + RVAExport, ord);
            }
#ifdef FORWARDERS
        }
        else {//forwarder or empty
            char *forward = (char *)((ULONG)RVAExport + (ULONG)win32file);
            dprintf((LOG, "ord ", ord << " at 0x";
            fout << RVAExport << " is forwarder to 0x", (int)forward ));
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
 Win32DllBase *WinDll;
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
  if (fBorland)
    dprintf((LOG, "Borland-styled PE-File." ));
  //Store modules
  dprintf((LOG, "%d imported Modules: ", cModules ));

  /* 2) functions */
  pszCurModule = pszModules;
  pOH = (PIMAGE_OPTIONAL_HEADER)OPTHEADEROFF(win32file);
  for (i = 0; i < cModules; i++)
  {
    dprintf((LOG, "Module %s", pszCurModule ));
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
        dprintf((LOG, "Invalid RVA %x", pulImport ));
        break;
    }
    // check section
    if ((ULONG)pulImport < shExtra.VirtualAddress || (ULONG)pulImport >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData))
    {
        if (!GetSectionHdrByRVA(win32file, &shExtra, (ULONG)pulImport))
        {
            dprintf((LOG, "warning: could not find section for Thunk RVA %x", pulImport ));
            break;
        }
    }

    //SvL: Load dll if needed
    dprintf((LOG, "**********************************************************************" ));
    dprintf((LOG, "************** Import Module %s ", pszCurModule ));
    dprintf((LOG, "**********************************************************************" ));
    WinDll = Win32DllBase::findModule(pszCurModule);

    if(WinDll == NULL)
    {  //not found, so load it
	char modname[CCHMAXPATH];

	strcpy(modname, pszCurModule);
  	//rename dll if necessary (i.e. OLE32 -> OLE32OS2)
  	Win32DllBase::renameDll(modname);

	if(isPEImage(modname) != ERROR_SUCCESS_W)
	{//LX image, so let OS/2 do all the work for us
		APIRET rc;
  		char   szModuleFailure[CCHMAXPATH] = "";
		ULONG  hInstanceNewDll;
                Win32LxDll *lxdll;

		char *dot = strchr(modname, '.');
		if(dot) {
			*dot = 0;
		}
		strcat(modname, ".DLL");
  		rc = DosLoadModule(szModuleFailure, sizeof(szModuleFailure), modname, (HMODULE *)&hInstanceNewDll);
  		if(rc) {
			dprintf((LOG, "DosLoadModule returned %X for %s\n", rc, szModuleFailure));
			sprintf(szErrorModule, "%s.DLL", szModuleFailure);
			errorState = rc;
			return(FALSE);
  		}
		lxdll = Win32LxDll::findModuleByOS2Handle(hInstanceNewDll);
		if(lxdll == NULL) {//shouldn't happen!
			dprintf((LOG, "Just loaded the dll, but can't find it anywhere?!!?"));
			errorState = ERROR_INTERNAL;
			return(FALSE);
		}
		lxdll->setDllHandleOS2(hInstanceNewDll);
		lxdll->AddRef();
                WinDll = (Win32DllBase*)lxdll;
	}
	else {
         Win32PeLdrDll *pedll;

        	pedll = new Win32PeLdrDll(modname, this);
        	if(pedll == NULL) {
	            dprintf((LOG, "pedll: Error allocating memory" ));
	            WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szMemErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
	            errorState = ERROR_INTERNAL;
	            return(FALSE);
	        }
	        dprintf((LOG, "**********************************************************************" ));
	        dprintf((LOG, "**********************     Loading Module        *********************" ));
	        dprintf((LOG, "**********************************************************************" ));
	        if(pedll->init(0) == FALSE) {
	            dprintf((LOG, "Internal WinDll error ", pedll->getError() ));
	            return(FALSE);
	        }
#ifdef DEBUG
    		pedll->AddRef(getModuleName());
#else
    		pedll->AddRef();
#endif
	        if(pedll->attachProcess() == FALSE) {
        	    dprintf((LOG, "attachProcess failed!" ));
		    delete pedll;
	            errorState = ERROR_INTERNAL;
	            return(FALSE);
	        }
		WinDll = (Win32DllBase*)pedll;
	}

	dprintf((LOG, "**********************************************************************" ));
	dprintf((LOG, "**********************  Finished Loading Module  *********************" ));
        dprintf((LOG, "**********************************************************************" ));
    }
    else {
	WinDll->AddRef();
	
	dprintf((LOG, "Already found ", pszCurModule));
    }
    //add the dll we just loaded to dependency list for this image
    addDependency(WinDll);

    //Make sure the dependency list is correct (already done
    //in the ctor of Win32DllBase, but for LX dlls the parent is
    //then set to NULL; so change it here again
    WinDll->setUnloadOrder(this);

    pulImport  = (PULONG)((ULONG)pulImport + (ULONG)win32file);
    j          = 0;
    ulCurFixup = (ULONG)pID[i].FirstThunk + (ULONG)win32file;

    section    = findSectionByOS2Addr(ulCurFixup);
    if(section == NULL) {
	dprintf((LOG, "Unable to find section for %x", ulCurFixup ));
	return FALSE;
    }
    //SvL: Read page from disk
    commitPage(ulCurFixup & ~0xfff, FALSE, SINGLE_PAGE);
    //SvL: Enable write access
    DosSetMem((PVOID)(ulCurFixup & ~0xfff), PAGE_SIZE, PAG_READ|PAG_WRITE);
    nrPages    = 1;

    while (pulImport[j] != 0) {
        if (pulImport[j] & IMAGE_ORDINAL_FLAG) { //ordinal
            dprintf((LOG, "0x%08x Imported function %s @%d", ulCurFixup , pszCurModule, (pulImport[j] & ~IMAGE_ORDINAL_FLAG) ));
            StoreImportByOrd(WinDll, pulImport[j] & ~IMAGE_ORDINAL_FLAG, ulCurFixup);
        }
        else {  //name
            //check
            if (pulImport[j] < shExtra.VirtualAddress || pulImport[j] >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData)) {
                if (!GetSectionHdrByRVA(win32file, &shExtra, pulImport[j]))
                {
                    dprintf((LOG, "warning: could not find section for Import Name RVA ", pulImport[j] ));
                    break;
                }
            }
            //KSO - Aug 6 1998 1:15am:this eases comparing...
            char *pszFunctionName = (char*)(pulImport[j] + (ULONG)win32file + 2);
            dprintf((LOG, "0x%08x Imported function %s", ulCurFixup,  pszFunctionName ));
            StoreImportByName(WinDll, pszFunctionName, ulCurFixup);
        }
        ulCurFixup += sizeof(IMAGE_THUNK_DATA);
        j++;
	if((ulCurFixup & 0xfff) == 0) {
    		commitPage(ulCurFixup & ~0xfff, FALSE, SINGLE_PAGE);
    		DosSetMem((PVOID)(ulCurFixup & ~0xfff), PAGE_SIZE, PAG_READ|PAG_WRITE);
    		nrPages++;
	}
    }
    //SvL: And restore original protection flags
    ulCurFixup = (ULONG)pID[i].FirstThunk + pOH->ImageBase;
    DosSetMem((PVOID)(ulCurFixup & ~0xfff), PAGE_SIZE*nrPages, section->pageflags);

    dprintf((LOG, "**********************************************************************" ));
    dprintf((LOG, "************** End Import Module %s ", pszCurModule ));
    dprintf((LOG, "**********************************************************************" ));

    pszCurModule += strlen(pszCurModule) + 1;
  }//for (i = 0; i < cModules; i++)

  free(pszModules);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::insideModule(ULONG address)
{
  if((address >= realBaseAddress) && (address < realBaseAddress + imageSize)) {
  	return TRUE;
  }
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::insideModuleCode(ULONG address)
{
 Section *sect;

  sect = findSectionByOS2Addr(address);
  if(sect && (sect->pageflags & PAG_EXECUTE)) {
	return TRUE;
  }
  return FALSE;
}
//******************************************************************************
//******************************************************************************
ULONG Win32PeLdrImage::getImageSize()
{
  return imageSize;
}
//******************************************************************************
//******************************************************************************
ULONG Win32PeLdrImage::getApi(char *name)
{
  ULONG       apiaddr, i, apilen;
  char       *apiname;
  char        tmp[4];
  NameExport *curexport;
  ULONG       ulAPIOrdinal;                      /* api requested by ordinal */

  apilen = strlen(name) + 1;
  if(apilen < 4)
  {
	*(ULONG *)tmp = 0;
	strcpy(tmp, name);
	apiname = tmp;
	apilen  = 4;
  }
  else	apiname = name;

  curexport = nameexports;
  for(i=0; i<nrNameExports; i++)
  {
    if(apilen == curexport->nlength &&
       *(ULONG *)curexport->name == *(ULONG *)apiname)
    {
      	if(strcmp(curexport->name, apiname) == 0)
        	return(curexport->virtaddr);
    }
    curexport = (NameExport *)((ULONG)curexport->name + curexport->nlength);
  }
  return(0);
}
//******************************************************************************
//******************************************************************************
ULONG Win32PeLdrImage::getApi(int ordinal)
{
 ULONG       apiaddr, i;
 OrdExport  *curexport;
 NameExport *nexport;

  curexport = ordexports;
  for(i=0;i<nrOrdExports;i++) {
	if(curexport->ordinal == ordinal)
		return(curexport->virtaddr);
	curexport++;
  }
  //Name exports also contain an ordinal, so check this
  nexport = nameexports;
  for(i=0;i<nrNameExports;i++) {
	if(nexport->ordinal == ordinal)
		return(nexport->virtaddr);

	nexport = (NameExport *)((ULONG)nexport->name + nexport->nlength);
  }
  return(0);
}
//******************************************************************************
//Returns required OS version for this image
//******************************************************************************
ULONG Win32PeLdrImage::getVersion()
{
  return (oh.MajorOperatingSystemVersion << 16) | oh.MinorOperatingSystemVersion;
}
//******************************************************************************
//******************************************************************************
ULONG MissingApi()
{
 static BOOL fIgnore = FALSE;
 int r;

  dprintf((LOG, "Missing api called!\n"));
  if(fIgnore)
    return(0);

  do {
    r = WinMessageBox(HWND_DESKTOP, NULLHANDLE, "The application has called a non-existing api\n",
                  "Internal Odin Error", 0, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_MOVEABLE);
  }
  while(r == MBID_RETRY); //giggle

  if( r != MBID_IGNORE )
	ExitProcess(987);

  fIgnore = TRUE;
  return(0);
}
/******************************************************************************/
/******************************************************************************/
