/* $Id: winimage.cpp,v 1.7 1999-08-11 22:27:56 phaller Exp $ */

/*
 * Win32 PE Image class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 *
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

#include <odincrt.h>

//#include <iostream.h>
//#include <fstream.h>
#include <assert.h>
#include "misc.h"
#include "nameid.h"
#include "win32type.h"
#include "winimage.h"
#include "windll.h"
#include "winexe.h"
#include "pefile.h"
#include "unicode.h"
#include "winres.h"

char szErrorTitle[]     = "Win32 for OS/2";
char szMemErrorMsg[]    = "Memory allocation failure";
char szFileErrorMsg[]   = "File IO error";
char szPEErrorMsg[]     = "Not a valid win32 exe. (perhaps 16 bits windows)";
char szCPUErrorMsg[]    = "Executable doesn't run on x86 machines";
char szExeErrorMsg[]    = "File isn't an executable";
char szInteralErrorMsg[]= "Internal Error";

#ifndef max
#define max(a, b)  ((a>b) ? a : b)
#endif

BOOL foutInit = FALSE;
ofstream fout;

ULONG MissingApi();
char *hex(ULONG num);

//******************************************************************************
//******************************************************************************
Win32Image::Win32Image(char *szFileName) :
    errorState(NO_ERROR), entryPoint(0), nrsections(0), imageSize(0),
    imageVirtBase(-1), baseAddress(0), realBaseAddress(0), imageVirtEnd(0),
    nrNameExports(0), nrOrdExports(0), nameexports(NULL), ordexports(NULL),
    szFileName(NULL), NameTable(NULL), Win32Table(NULL), fullpath(NULL),
    tlsAddress(0), tlsIndexAddr(0), tlsInitSize(0), tlsTotalSize(0), tlsCallBackAddr(0), tlsIndex(-1),
    pResSection(NULL), pResDir(NULL), winres(NULL), VersionId(-1)
{
  //native win32 exe/dll, converted dll or native OS/2 dll
  //if it's either of the latter two, this flag will be reset when
  //RegisterDll is called
  fNativePEImage = TRUE;

  if(foutInit == FALSE) {
    fout.open("pe.log", ios::out | ios::trunc);
    foutInit = TRUE;
  }
  hinstance = (HINSTANCE)this;
  this->szFileName = szFileName;
#ifdef DEBUG
  magic = MAGIC_WINIMAGE;
#endif
}
//******************************************************************************
//******************************************************************************
Win32Image::Win32Image(HINSTANCE hinstance, int NameTableId, int Win32TableId) :
    errorState(NO_ERROR), entryPoint(0), nrsections(0), imageSize(0),
    imageVirtBase(-1), baseAddress(0), realBaseAddress(0), imageVirtEnd(0),
    nrNameExports(0), nrOrdExports(0), nameexports(NULL), ordexports(NULL),
    szFileName(NULL), NameTable(NULL), Win32Table(NULL), fullpath(NULL),
    tlsAddress(0), tlsIndexAddr(0), tlsInitSize(0), tlsTotalSize(0), tlsCallBackAddr(0), tlsIndex(-1),
    pResSection(NULL), pResDir(NULL), winres(NULL)
{
#ifdef DEBUG
  magic = MAGIC_WINIMAGE;
#endif
  OS2ImageInit(hinstance, NameTableId, Win32TableId);
}
//******************************************************************************
//******************************************************************************
void Win32Image::OS2ImageInit(HINSTANCE hinstance, int NameTableId, int Win32TableId)
{
 APIRET rc;

  this->hinstance    = hinstance;
  this->NameTableId  = NameTableId;
  this->Win32TableId = Win32TableId;

  //converted win32 exe/dll or OS/2 system dll
  fNativePEImage     = FALSE;

  if(NameTableId != NO_NAMETABLE) {
    //Load name table resource
    rc = DosGetResource(hinstance, RT_RCDATA, NameTableId, (PPVOID)&NameTable);
    if(rc) {
        eprintf(("Can't find converted name resource (rc %d)!!\n", rc));
        return;
    }
  }
  else  this->NameTableId = 0;

  //Load win32 id table resource
  if((Win32TableId & 0xFFFFFF) != NO_LOOKUPTABLE) {
    rc = DosGetResource(hinstance, RT_RCDATA, Win32TableId, (PPVOID)&Win32Table);
    if(rc) {
        eprintf(("Can't find win32 id resource (rc %d)!!\n", rc));
        return;
    }
  }
  else  this->Win32TableId = 0;
}
//******************************************************************************
//******************************************************************************
Win32Image::~Win32Image()
{
 Win32Resource *res;

  if(NameTable)
    DosFreeResource((PVOID)NameTable);

  if(Win32Table)
    DosFreeResource((PVOID)Win32Table);

  while(winres)
  {
    res    = winres->next;
    ODIN_delete(winres);
    winres = res;
  }
  if(realBaseAddress)
    DosFreeMem((PVOID)realBaseAddress);

  if(nameexports)
    free(nameexports);

  if(ordexports)
    free(ordexports);
  if(fullpath)
    free(fullpath);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Image::init()
{
 HFILE  win32handle;
 ULONG  ulAction       = 0;      /* Action taken by DosOpen */
 ULONG  ulLocal        = 0;      /* File pointer position after DosSetFilePtr */
 APIRET rc             = NO_ERROR;            /* Return code */
 char   szErrorMsg[64];
 LPVOID win32file     = NULL;
 ULONG  filesize, ulRead;
 PIMAGE_SECTION_HEADER psh;
 int    nSections, i;

  rc = DosOpen(szFileName,                     /* File path name */
           &win32handle,                   /* File handle */
               &ulAction,                      /* Action taken */
               0L,                             /* File primary allocation */
               0L,                     /* File attribute */
               OPEN_ACTION_FAIL_IF_NEW |
               OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
               OPEN_FLAGS_NOINHERIT |
               OPEN_SHARE_DENYNONE  |
               OPEN_ACCESS_READONLY,           /* Open mode of the file */
               0L);                            /* No extended attribute */

  if (rc != NO_ERROR) {
    sprintf(szErrorMsg, "Unable to open %32s\n", szFileName);
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    errorState = ERROR_INTERNAL;
        return(FALSE);
  }

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);
  DosSetFilePtr(win32handle, 0L, FILE_END, &filesize);
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  win32file = malloc(filesize);
  if(win32file == NULL) {
    fout << "Error allocating memory" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szMemErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    DosClose(win32handle);                /* Close the file */
    errorState = ERROR_INTERNAL;
    return(FALSE);
  }
  rc = DosRead(win32handle, win32file, filesize, &ulRead);
  if(rc != NO_ERROR) {
    fout << "DosRead returned " << rc << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szFileErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    DosClose(win32handle);                /* Close the file */
    errorState = ERROR_INTERNAL;
    return(FALSE);
  }

  if(GetPEFileHeader (win32file, &fh) == FALSE) {
    fout << "Not a valid PE file (probably a 16 bits windows exe/dll)!" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szPEErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    DosClose(win32handle);                /* Close the file */
    errorState = ERROR_INTERNAL;
    return(FALSE);
  }

  if(!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)) {//not valid
    fout << "Not a valid PE file!" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szPEErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    DosClose(win32handle);                /* Close the file */
    errorState = ERROR_INTERNAL;
    return(FALSE);
  }
  if(fh.Machine != IMAGE_FILE_MACHINE_I386) {
    fout << "You need a REAL CPU to run this code" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szCPUErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    DosClose(win32handle);                /* Close the file */
    errorState = ERROR_INTERNAL;
    return(FALSE);
  }
  //IMAGE_FILE_SYSTEM == only drivers (device/file system/video etc)?
  if(fh.Characteristics & IMAGE_FILE_SYSTEM) {
    fout << "Can't convert system files" << endl;
        WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szExeErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
    DosClose(win32handle);                /* Close the file */
    errorState = ERROR_INTERNAL;
    return(FALSE);
  }

  if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
    fout << "No fixups, might not run!" << endl;
  }

  GetPEOptionalHeader (win32file, &oh);
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

  nSections = NR_SECTIONS(win32file);

  if ((psh = (PIMAGE_SECTION_HEADER)SECTIONHDROFF (win32file)) != NULL) {
    fout << endl << "*************************PE SECTIONS START**************************" << endl;
    for (i=0; i<nSections; i++) {
        fout << "Raw data size:       " << psh[i].SizeOfRawData << endl;
        fout << "Virtual Address:     " << psh[i].VirtualAddress << endl;
        fout << "Virtual Size:        " << psh[i].Misc.VirtualSize << endl;
        fout << "Pointer to raw data: " << psh[i].PointerToRawData << endl;
        fout.setf(ios::hex, ios::basefield);
        fout << "Section flags:       " << psh[i].Characteristics << endl << endl;
        fout.setf(ios::dec, ios::basefield);
        if(strcmp(psh[i].Name, ".reloc") == 0) {
            fout << ".reloc" << endl << endl;
            addSection(SECTION_RELOC, (char *)win32file+psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(strcmp(psh[i].Name, ".edata") == 0) {
            fout << ".edata" << endl << endl;
            addSection(SECTION_EXPORT, (char *)win32file+psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(strcmp(psh[i].Name, ".rsrc") == 0) {
            fout << ".rsrc" << endl << endl;
            addSection(SECTION_RESOURCE, (char *)win32file+psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(strcmp(psh[i].Name, ".debug") == 0) {
            fout << ".rdebug" << endl << endl;
            addSection(SECTION_DEBUG, (char *)win32file+psh[i].PointerToRawData,
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
            addSection(type, (char *)win32file+psh[i].PointerToRawData,
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
            addSection(SECTION_CODE, (char *)win32file+psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(!(psh[i].Characteristics & IMAGE_SCN_MEM_WRITE)) { //read only data section
            fout << "Read Only Data Section" << endl << endl;
            addSection(SECTION_READONLYDATA, (char *)win32file+psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(psh[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
            fout << "Uninitialized Data Section" << endl << endl;
            addSection(SECTION_UNINITDATA, (char *)win32file+psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(psh[i].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
            fout << "Initialized Data Section" << endl << endl;
            addSection(SECTION_INITDATA, (char *)win32file+psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        if(psh[i].Characteristics & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ)) {
            fout << "Other Section, stored as read/write uninit data" << endl << endl;
            addSection(SECTION_UNINITDATA, (char *)win32file+psh[i].PointerToRawData,
                   psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                   psh[i].Misc.VirtualSize);
            continue;
        }
        fout << "Unknown section" << endl;
        errorState = ERROR_INTERNAL;
        return(FALSE);
        }
  }
  fout << "*************************PE SECTIONS END **************************" << endl;
  imageSize += (imageVirtBase - oh.ImageBase);
  fout << "Total size of Image " << imageSize << endl;
  fout << "imageVirtBase       " << imageVirtBase << endl;
  fout << "imageVirtEnd        " << imageVirtEnd << endl;

  if(imageSize != imageVirtEnd - oh.ImageBase) {
    fout << "imageSize != imageVirtEnd - oh.ImageBase!" << endl;
    imageSize = imageVirtEnd - oh.ImageBase;
  }
  if(allocSections() == FALSE) {
    fout << "Failed to allocate image memory, rc " << errorState << endl;
    return(FALSE);
  }
  fout << "OS/2 base address " << baseAddress << endl;
  if(storeSections() == FALSE) {
    fout << "Failed to store sections, rc " << errorState << endl;
    return(FALSE);
  }
  entryPoint = baseAddress + oh.AddressOfEntryPoint;

  if(setFixups((PIMAGE_BASE_RELOCATION)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_BASERELOC)) == FALSE) {
    fout << "Failed to set fixups" << endl;
    return(FALSE);
  }
  if(processImports((char *)win32file) == FALSE) {
    fout << "Failed to process imports!" << endl;
    return(FALSE);
  }

  if(fh.Characteristics & IMAGE_FILE_DLL) {
    if(processExports((char *)win32file) == FALSE) {
        fout << "Failed to process exported apis" << endl;
        return(FALSE);
    }
  }
  IMAGE_SECTION_HEADER sh;
  if(GetSectionHdrByName (win32file, &sh, ".rsrc")) {
    //get offset in resource object of directory entry
    pResDir = (PIMAGE_RESOURCE_DIRECTORY)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_RESOURCE);
  }
  //set final memory protection flags (storeSections sets them to read/write)
  if(setMemFlags() == FALSE) {
    fout << "Failed to set memory protection" << endl;
    return(FALSE);
  }

  //Now it's safe to free win32file
  free(win32file);
  DosClose(win32handle);                /* Close the file */
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
void Win32Image::addSection(ULONG type, char *rawdata, ULONG rawsize, ULONG virtaddress, ULONG virtsize)
{
  virtsize = max(rawsize, virtsize);

  section[nrsections].type           = type;
  section[nrsections].rawdata        = rawdata;
  section[nrsections].rawsize        = rawsize;
  section[nrsections].virtaddr       = virtaddress;

  if(type == SECTION_RESOURCE) {
    pResSection = &section[nrsections];
  }
  virtsize   = ((virtsize - 1) & ~0xFFF) + PAGE_SIZE;
  imageSize += virtsize;
  section[nrsections].virtualsize    = virtsize;

  if(virtaddress < imageVirtBase)
    imageVirtBase = virtaddress;
  if(virtaddress + virtsize > imageVirtEnd)
    imageVirtEnd = virtaddress + virtsize;

  nrsections++;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Image::allocSections()
{
 APIRET rc;

  if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
    fout << "No fixups, might not run!" << endl;
    return allocFixedMem();
  }
  rc = DosAllocMem((PPVOID)&baseAddress, imageSize, PAG_READ);
  if(rc) {
    errorState = rc;
    return(FALSE);
  }
  realBaseAddress = baseAddress;
  return(TRUE);
}
//******************************************************************************
#define FALLOC_SIZE (1024*1024)
//******************************************************************************
BOOL Win32Image::allocFixedMem()
{
 ULONG  address = 0;
 ULONG  lastaddress = 0;
 ULONG  firstaddress = 0;
 ULONG  diff, i;
 APIRET rc;

  baseAddress = realBaseAddress = 0;

  while(TRUE) {
    rc = DosAllocMem((PPVOID)&address, FALLOC_SIZE, PAG_READ);
    if(rc) break;

    if(firstaddress == 0)
        firstaddress = address;

    fout << "DosAllocMem returned " << address << endl;
    if(address + FALLOC_SIZE >= oh.ImageBase) {
        if(address > oh.ImageBase) {//we've passed it!
            DosFreeMem((PVOID)address);
            return(FALSE);
        }
        //found the right address
        DosFreeMem((PVOID)address);
        //align at 64 kb boundary
        realBaseAddress = oh.ImageBase & 0xFFFF0000;
        diff = realBaseAddress - address;
        if(diff) {
            rc = DosAllocMem((PPVOID)&address, diff, PAG_READ);
            if(rc) break;
        }
        rc = DosAllocMem((PPVOID)&baseAddress, imageSize, PAG_READ);
        if(rc) break;

        if(baseAddress != realBaseAddress) {
            fout << "baseAddress != realBaseAddress!!" << endl;
            break;
        }
        if(diff) DosFreeMem((PVOID)address);

        address = realBaseAddress;
        realBaseAddress = baseAddress;
        baseAddress = oh.ImageBase;
        break;
    }
    lastaddress = address;
  }
  while(firstaddress <= lastaddress) {
    DosFreeMem((PVOID)firstaddress);
    firstaddress += FALLOC_SIZE;
  }
  if(baseAddress == 0) //Let me guess.. MS Office app?
    return(FALSE);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Image::storeSections()
{
 int i;
 APIRET rc;
 ULONG  pagFlags = PAG_COMMIT;

  for(i=0;i<nrsections;i++) {
    section[i].realvirtaddr = baseAddress + (section[i].virtaddr - oh.ImageBase);
  }
  for(i=0;i<nrsections;i++) {
    pagFlags = PAG_COMMIT;
    switch(section[i].type) {
        case SECTION_CODE:
        case (SECTION_CODE | SECTION_IMPORT):
        case SECTION_INITDATA:
        case SECTION_UNINITDATA:
        case SECTION_IMPORT:
        case SECTION_READONLYDATA:
        case SECTION_RESOURCE:
            pagFlags |= PAG_WRITE | PAG_READ;
            break;
        case SECTION_EXPORT:
        case SECTION_DEBUG:
        case SECTION_RELOC:
            pagFlags = 0;   //don't commit
            break;
    }
    if(pagFlags == 0)   continue;   //debug or export section

    rc = DosSetMem((PVOID)section[i].realvirtaddr, section[i].virtualsize, pagFlags);
    if(rc) {
        errorState = rc;
        return(FALSE);
    }
    if(section[i].type != SECTION_UNINITDATA) {
        assert(section[i].rawdata);
        memcpy((char *)section[i].realvirtaddr, section[i].rawdata, section[i].rawsize);
    }
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Image::setMemFlags()
{
 int i;
 APIRET rc;
 ULONG  pagFlags = 0;

  for(i=0;i<nrsections;i++) {
    pagFlags = 0;
    switch(section[i].type) {
        case SECTION_CODE:
        case (SECTION_CODE | SECTION_IMPORT):
            pagFlags |= PAG_EXECUTE | PAG_READ;
            break;
        case SECTION_INITDATA:
        case SECTION_UNINITDATA:
        case SECTION_IMPORT: //TODO: read only?
            pagFlags |= PAG_WRITE | PAG_READ;
            break;
        case SECTION_READONLYDATA:
        case SECTION_RESOURCE:
            pagFlags |= PAG_READ;
            break;
        default:
            continue;
    }
    rc = DosSetMem((PVOID)section[i].realvirtaddr, section[i].virtualsize, pagFlags);
    if(rc) {
        errorState = rc;
        return(FALSE);
    }
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL Win32Image::setFixups(PIMAGE_BASE_RELOCATION prel)
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
                AddOff32Fixup(oh.ImageBase +
                          prel->VirtualAddress + offset);
                break;
            case IMAGE_REL_BASED_HIGH:
		AddOff16Fixup(oh.ImageBase + prel->VirtualAddress + offset, TRUE);
		break;
            case IMAGE_REL_BASED_LOW:
		AddOff16Fixup(oh.ImageBase + prel->VirtualAddress + offset, FALSE);
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
void Win32Image::AddOff32Fixup(ULONG fixupaddr)
{
 ULONG orgaddr;
 ULONG *fixup;

  fixup   = (ULONG *)(fixupaddr - oh.ImageBase + baseAddress);
  orgaddr = *fixup;
  *fixup  = baseAddress + (*fixup - oh.ImageBase);
}
//******************************************************************************
//******************************************************************************
void Win32Image::AddOff16Fixup(ULONG fixupaddr, BOOL fHighFixup)
{
 ULONG   orgaddr;
 USHORT *fixup;

  fixup   = (USHORT *)(fixupaddr - oh.ImageBase + baseAddress);
  orgaddr = *fixup;
  if(fHighFixup) {
  	*fixup  += (USHORT)((baseAddress - oh.ImageBase) >> 16);
  }
  else {
  	*fixup  += (USHORT)((baseAddress - oh.ImageBase) & 0xFFFF);
  }
}
//******************************************************************************
//******************************************************************************
void Win32Image::StoreImportByOrd(Win32Dll *WinDll, ULONG ordinal, ULONG impaddr)
{
 ULONG *import;
 ULONG  apiaddr;

  import  = (ULONG *)(impaddr - oh.ImageBase + baseAddress);
  apiaddr = WinDll->getApi(ordinal);
  if(apiaddr == 0) {
    fout << "--->>> NOT FOUND!";
    *import = (ULONG)MissingApi;
  }
  else  *import = apiaddr;
}
//******************************************************************************
//******************************************************************************
void Win32Image::StoreImportByName(Win32Dll *WinDll, char *impname, ULONG impaddr)
{
 ULONG *import;
 ULONG  apiaddr;

  import = (ULONG *)(impaddr - oh.ImageBase + baseAddress);
  apiaddr = WinDll->getApi(impname);
  if(apiaddr == 0) {
    fout << "--->>> NOT FOUND!";
    *import = (ULONG)MissingApi;
  }
  else  *import = apiaddr;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Image::processExports(char *win32file)
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
    ptrOrd     = (USHORT *)((ULONG)ped->AddressOfNameOrdinals -
                    (ULONG)sh.VirtualAddress +
                            (ULONG)sh.PointerToRawData + (ULONG)win32file);
    ptrNames   = (ULONG *)((ULONG)ped->AddressOfNames -
                  (ULONG)sh.VirtualAddress +
                          (ULONG)sh.PointerToRawData + (ULONG)win32file);
    ptrAddress = (ULONG *)((ULONG)ped->AddressOfFunctions -
                  (ULONG)sh.VirtualAddress +
                          (ULONG)sh.PointerToRawData + (ULONG)win32file);
    nrOrdExports  = ped->NumberOfFunctions;
    nrNameExports = ped->NumberOfNames;

    int   ord, RVAExport;
    char *name;
    for(i=0;i<ped->NumberOfNames;i++) {
        ord       = ptrOrd[i] + ped->Base;
        name      = (char *)((ULONG)ptrNames[i] - (ULONG)sh.VirtualAddress +
                             (ULONG)sh.PointerToRawData + (ULONG)win32file);
        RVAExport = ptrAddress[ptrOrd[i]];
#ifdef FORWARDERS
        if(RVAExport < sh.VirtualAddress || RVAExport > sh.VirtualAddress + sh.SizeOfRawData) {
#endif
            //points to code (virtual address relative to oh.ImageBase
                fout << "address 0x";
                fout.setf(ios::hex, ios::basefield);
            fout << RVAExport;
                    fout.setf(ios::dec, ios::basefield);
                fout << " " << name << "@" << ord << endl;
            AddNameExport(oh.ImageBase + RVAExport, name, ord);
#ifdef FORWARDERS

        }
        else {//forwarder
            char *forward = (char *)((ULONG)RVAExport -
                         (ULONG)sh.VirtualAddress +
                                     (ULONG)sh.PointerToRawData +
                             (ULONG)win32file);
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
            char *forward = (char *)((ULONG)RVAExport -
                         (ULONG)sh.VirtualAddress +
                                     (ULONG)sh.PointerToRawData +
                             (ULONG)win32file);
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
void Win32Image::AddNameExport(ULONG virtaddr, char *apiname, ULONG ordinal)
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
  curnameexport->virtaddr = baseAddress + (virtaddr - oh.ImageBase);
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
void Win32Image::AddOrdExport(ULONG virtaddr, ULONG ordinal)
{
  if(ordexports == NULL) {
    ordexports   = (OrdExport *)malloc(nrOrdExports * sizeof(OrdExport));
    curordexport = ordexports;
  }
  curordexport->virtaddr = baseAddress + (virtaddr - oh.ImageBase);
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
BOOL Win32Image::processImports(char *win32file)
{
 PIMAGE_IMPORT_DESCRIPTOR pID;
 IMAGE_SECTION_HEADER     shID;
 IMAGE_SECTION_HEADER     shExtra = {0};
 PIMAGE_OPTIONAL_HEADER   pOH;
 int    i,j;
 BOOL   fBorland = 0;
 int    cModules;
 char  *pszModules;
 char  *pszCurModule;
 char  *pszTmp;
 ULONG *pulImport;
 ULONG  ulCurFixup;
 int    Size;
 Win32Dll *WinDll;

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
        pszTmp = (char*)(pID[i].Name- shID.VirtualAddress + shID.PointerToRawData + (ULONG)win32file);
    }
    else {
        //is the "Extra"-section already found or do we have to find it?
        if (pID[i].Name < shExtra.VirtualAddress || pID[i].Name >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData)) {
            if (!GetSectionHdrByRVA(win32file, &shExtra, pID[i].Name))
                 return FALSE;
        }
        pszTmp = (char*)(pID[i].Name- shExtra.VirtualAddress + shExtra.PointerToRawData + (ULONG)win32file);
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
        pszTmp = (char*)(pID[i].Name- shID.VirtualAddress + shID.PointerToRawData + (ULONG)win32file);
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
        pszTmp = (char*)(pID[i].Name- shExtra.VirtualAddress + shExtra.PointerToRawData + (ULONG)win32file);
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
    if (pID[i].u.OriginalFirstThunk == 0 || fBorland)
        pulImport = (ULONG*)pID[i].FirstThunk;
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
    WinDll = Win32Dll::findModule(pszCurModule);
    if(WinDll == NULL)
     {  //not found, so load it
        ODIN_FS_BEGIN
        WinDll = new Win32Dll(pszCurModule);
        ODIN_FS_END

        if(WinDll == NULL) {
            fout << "WinDll: Error allocating memory" << endl;
                WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szMemErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
            errorState = ERROR_INTERNAL;
            return(FALSE);
        }
        fout << "**********************************************************************" << endl;
        fout << "**********************     Loading Module        *********************" << endl;
        fout << "**********************************************************************" << endl;
        if(WinDll->init() == FALSE) {
            fout << "Internal WinDll error " << WinDll->getError() << endl;
            return(FALSE);
        }
        if(WinDll->attachProcess() == FALSE) {
            fout << "attachProcess failed!" << endl;
            errorState = ERROR_INTERNAL;
            return(FALSE);
        }
        fout << "**********************************************************************" << endl;
        fout << "**********************  Finished Loading Module  *********************" << endl;
        fout << "**********************************************************************" << endl;
    }
    else    fout << "Already found " << pszCurModule << endl;

    WinDll->AddRef();

    pulImport  = (PULONG)((ULONG)pulImport - shExtra.VirtualAddress + (ULONG)win32file + shExtra.PointerToRawData);
    j          = 0;
    ulCurFixup = (ULONG)pID[i].FirstThunk + pOH->ImageBase;
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
            char *pszFunctionName = (char*)(pulImport[j] + (ULONG)win32file + shExtra.PointerToRawData - shExtra.VirtualAddress + 2);
            fout.setf(ios::hex, ios::basefield);
            fout << "0x" << ulCurFixup << " Imported function " << pszFunctionName << endl;
            fout.setf(ios::dec, ios::basefield);
            StoreImportByName(WinDll, pszFunctionName, ulCurFixup);
        }
            ulCurFixup += sizeof(IMAGE_THUNK_DATA);
        j++;
    }
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
BOOL Win32Image::isPEImage(char *szFileName)
{
 IMAGE_FILE_HEADER     fh;
 HFILE  win32handle;
 ULONG  ulAction       = 0;      /* Action taken by DosOpen */
 ULONG  ulLocal        = 0;      /* File pointer position after DosSetFilePtr */
 APIRET rc             = NO_ERROR;            /* Return code */
 LPVOID win32file      = NULL;
 ULONG  ulRead;
 int    nSections, i;

  rc = DosOpen(szFileName,                     /* File path name */
           &win32handle,                   /* File handle */
               &ulAction,                      /* Action taken */
               0L,                             /* File primary allocation */
               0L,                     /* File attribute */
               OPEN_ACTION_FAIL_IF_NEW |
               OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
               OPEN_FLAGS_NOINHERIT |
               OPEN_SHARE_DENYNONE  |
               OPEN_ACCESS_READONLY,           /* Open mode of the file */
               0L);                            /* No extended attribute */

  if (rc != NO_ERROR)
  {
    dprintf(("KERNEL32:Win32Image::isPEImage(%s) failed with %u\n",
             szFileName,
             rc));
    return(FALSE);
  }

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  IMAGE_DOS_HEADER *pdoshdr = (IMAGE_DOS_HEADER *)malloc(sizeof(IMAGE_DOS_HEADER));
  if(pdoshdr == NULL)   {
    DosClose(win32handle);                /* Close the file */
    return(FALSE);
  }
  rc = DosRead(win32handle, pdoshdr, sizeof(IMAGE_DOS_HEADER), &ulRead);
  if(rc != NO_ERROR) {
    DosClose(win32handle);                /* Close the file */
    return(FALSE);
  }
  ULONG hdrsize = pdoshdr->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER);
  free(pdoshdr);

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  win32file = malloc(hdrsize);
  if(win32file == NULL) {
    DosClose(win32handle);                /* Close the file */
    return(FALSE);
  }
  rc = DosRead(win32handle, win32file, hdrsize, &ulRead);
  if(rc != NO_ERROR) {
    goto failure;
  }

  if(GetPEFileHeader (win32file, &fh) == FALSE) {
    goto failure;
  }

  if(!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)) {//not valid
    goto failure;
  }
  if(fh.Machine != IMAGE_FILE_MACHINE_I386) {
    goto failure;
  }
  //IMAGE_FILE_SYSTEM == only drivers (device/file system/video etc)?
  if(fh.Characteristics & IMAGE_FILE_SYSTEM) {
    goto failure;
  }
  DosClose(win32handle);
  return(TRUE);

failure:
  free(win32file);
  DosClose(win32handle);
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
void Win32Image::setFullPath(char *name)
{
  dassert(name, ("setFullPath, name == NULL"));
  fullpath = (char *)malloc(strlen(name)+1);
  dassert(fullpath, ("setFullPath, fullpath == NULL"));
  strcpy(fullpath, name);
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
                  "Internal Error", 0, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_MOVEABLE);
  }
  while(r == MBID_RETRY); //giggle

  if( r != MBID_IGNORE )
     exit(987);

  fIgnore = TRUE;
  return(0);
}
//******************************************************************************
//******************************************************************************
/*heximal(decimal) KSO Sun 24.05.1998*/
char szHexBuffer[30];

char *hex(ULONG num)
{
    sprintf(szHexBuffer, "0x%+08x (%lu)",num,num);
    return szHexBuffer;
}
//******************************************************************************
//******************************************************************************
