/* $Id: winimagepeldr.cpp,v 1.100 2002-07-24 11:29:23 sandervl Exp $ */

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
 * TODO: Loading of forwarder dlls before handling imports might not be correct
 *       (circular dependencies; have to check what NT does)
 * TODO: Two LoadLibrary calls in two threads at the same time won't be handled properly (rare but possible)
 *
 * NOTE: FLAG_PELDR_LOADASDATAFILE is a special flag to only load the resource directory
 *       of a PE image. Processing imports, sections etc is not done.
 *       This is useful for GetVersionSize/Resource in case it wants to
 *       get version info of an image that is not loaded.
 *       So an instance of this type can't be used for anything but resource lookup!
 *
 * NOTE: File pointer operations relative to the start of the file must add
 *       ulPEOffset (in case PE image start != file start)
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
#include <heapcode.h>
#include "winimagebase.h"
#include "winimagepeldr.h"
#include "windllpeldr.h"
#include "windlllx.h"
#include "winexebase.h"
#include <pefile.h>
#include <unicode.h>
#include "oslibmisc.h"
#include "initterm.h"
#include <win\virtual.h>
#include "oslibdos.h"
#include "oslibmem.h"
#include "mmap.h"
#include <wprocess.h>

//Define COMMIT_ALL to let the pe loader commit all sections of the image
//This is very useful during debugging as you'll get lots of exceptions
//otherwise.
//#ifdef DEBUG
#define COMMIT_ALL
//#endif

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

ULONG WIN32API MissingApiOrd(char *parentimage, char *dllname, int ordinal);
ULONG WIN32API MissingApiName(char *parentimage, char *dllname, char *functionname);
ULONG WIN32API MissingApi(char *message);

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
Win32PeLdrImage::Win32PeLdrImage(char *pszFileName, BOOL isExe) :
    Win32ImageBase(-1),
    nrsections(0), imageSize(0), dwFlags(0), section(NULL),
    imageVirtBase(-1), realBaseAddress(0), imageVirtEnd(0),
    nrNameExports(0), nrOrdExports(0), nameexports(NULL), ordexports(NULL),
    memmap(NULL), pFixups(NULL), dwFixupSize(0), curnameexport(NULL), curordexport(NULL),
    nrOrdExportsRegistered(0)
{
    HFILE  dllfile;

    fIsPEImage = TRUE;

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
        else    OSLibDosClose(dllfile);
    }
    else {
        findDll(szFileName, szModule, sizeof(szModule));
        strcpy(szFileName, szModule);
    }
    strcpy(szModule, OSLibStripPath(szFileName));
    strupr(szModule);
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
        OSLibDosFreeMem((PVOID)realBaseAddress);

    if(nameexports)
        free(nameexports);

    if(ordexports)
        free(ordexports);

    if(section)
        free(section);
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::init(ULONG reservedMem, ULONG ulPEOffset)
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

    //offset in executable image where real PE file starts (default 0)
    this->ulPEOffset = ulPEOffset;

    hFile = OSLibDosOpen(szFileName, OSLIB_ACCESS_READONLY|OSLIB_ACCESS_SHAREDENYNONE);
  
    dprintf((LOG, "KERNEL32-PELDR: Opening PE-image (%s) returned handle %08xh.\n",
             szFileName,
             hFile));

    //change to PE image start if necessary
    if(ulPEOffset) {
        if(OSLibDosSetFilePtr(hFile, ulPEOffset, OSLIB_SETPTR_FILE_BEGIN) == -1) {
            goto failure;
        }
    }
  
    //default error:
    strcpy(szErrorModule, OSLibStripPath(szFileName));
    if(hFile == NULL) {
        goto failure;
    }
    //read dos header
    if(DosRead(hFile, (LPVOID)&doshdr, sizeof(doshdr), &ulRead)) {
        goto failure;
    }
    if(OSLibDosSetFilePtr(hFile, ulPEOffset+doshdr.e_lfanew, OSLIB_SETPTR_FILE_BEGIN) == -1) {
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
    if(memmap == NULL || !memmap->Init()) {
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
    section = (Section *)malloc(nSections*sizeof(Section));
    if(section == NULL) {
        DebugInt3();
        goto failure;
    }
    memset(section, 0, nSections*sizeof(Section));

    imageSize = 0;
    if ((psh = (PIMAGE_SECTION_HEADER)SECTIONHDROFF (win32file)) != NULL)
    {
        dprintf((LOG, "*************************PE SECTIONS START**************************" ));
        for (i=0; i<nSections; i++)
        {
            dprintf((LOG, "Section:              %-8s", psh[i].Name ));
            dprintf((LOG, "Raw data size:        %x", psh[i].SizeOfRawData ));
            dprintf((LOG, "Virtual Address:      %x", psh[i].VirtualAddress ));
            dprintf((LOG, "Virtual Address Start:%x", psh[i].VirtualAddress+oh.ImageBase ));
            dprintf((LOG, "Virtual Address End:  %x", psh[i].VirtualAddress+oh.ImageBase+psh[i].Misc.VirtualSize ));
            dprintf((LOG, "Virtual Size:         %x", psh[i].Misc.VirtualSize ));
            dprintf((LOG, "Pointer to raw data:  %x", psh[i].PointerToRawData ));
            dprintf((LOG, "Section flags:        %x\n\n", psh[i].Characteristics ));

            if(IsSectionType(win32file, &psh[i], IMAGE_DIRECTORY_ENTRY_BASERELOC))
            {
                dprintf((LOG, ".reloc" ));
                addSection(SECTION_RELOC, psh[i].PointerToRawData,
                           psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                           psh[i].Misc.VirtualSize, psh[i].Characteristics);
                continue;
            }
            if(IsSectionType(win32file, &psh[i], IMAGE_DIRECTORY_ENTRY_EXPORT))
            {
                //SvL: Angus.exe has empty export section that's really an
                //     uninitialized data section
                if(psh[i].SizeOfRawData) {
                     dprintf((LOG, ".edata" ));
                     addSection(SECTION_EXPORT, psh[i].PointerToRawData,
                                psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                                 psh[i].Misc.VirtualSize, psh[i].Characteristics);
                     continue;
                }
            }
            if(IsSectionType(win32file, &psh[i], IMAGE_DIRECTORY_ENTRY_RESOURCE))
            {
                dprintf((LOG, ".rsrc" ));
                addSection(SECTION_RESOURCE, psh[i].PointerToRawData,
                           psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                           psh[i].Misc.VirtualSize, psh[i].Characteristics);
                continue;
            }
            if(IsSectionType(win32file, &psh[i], IMAGE_DIRECTORY_ENTRY_TLS))
            {
                dprintf((LOG, "TLS section"));
                tlsDir = (IMAGE_TLS_DIRECTORY *)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_TLS);
                if(tlsDir) {
                    addSection(SECTION_TLS, psh[i].PointerToRawData,
                               psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                               psh[i].Misc.VirtualSize, psh[i].Characteristics);
                }
                continue;
            }
            if(IsSectionType(win32file, &psh[i], IMAGE_DIRECTORY_ENTRY_DEBUG))
            {
                dprintf((LOG, ".rdebug" ));
                addSection(SECTION_DEBUG,  psh[i].PointerToRawData,
                           psh[i].SizeOfRawData, psh[i].VirtualAddress + oh.ImageBase,
                           psh[i].Misc.VirtualSize, psh[i].Characteristics);
                continue;
            }
            if(IsSectionType(win32file, &psh[i], IMAGE_DIRECTORY_ENTRY_IMPORT))
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
            if(psh[i].Characteristics & IMAGE_SCN_CNT_CODE ||
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
    dprintf((LOG, "*************************PE SECTIONS END **************************" ));

    imageSize += imageVirtBase - oh.ImageBase;
    dprintf((LOG, "Total size of Image %x", imageSize ));
    dprintf((LOG, "imageVirtBase       %x", imageVirtBase ));
    dprintf((LOG, "imageVirtEnd        %x", imageVirtEnd ));

    //In case there are any gaps between sections, adjust size
    if(imageSize != imageVirtEnd - oh.ImageBase)
    {
        dprintf((LOG, "imageSize != imageVirtEnd - oh.ImageBase!" ));
        imageSize = imageVirtEnd - oh.ImageBase;
    }
    if(imageSize < oh.SizeOfImage) {
        imageSize = oh.SizeOfImage;
    }

    dprintf((LOG, "OS/2 base address   %x", realBaseAddress ));
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

    if(realBaseAddress != oh.ImageBase && !(dwFlags & FLAG_PELDR_LOADASDATAFILE)) {
        pFixups     = (PIMAGE_BASE_RELOCATION)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_BASERELOC);
        dwFixupSize = ImageDirectorySize(win32file, IMAGE_DIRECTORY_ENTRY_BASERELOC);
        commitPage((ULONG)pFixups, FALSE);
    }

    if(!(dwFlags & FLAG_PELDR_LOADASDATAFILE))
    {
        if(tlsDir = (IMAGE_TLS_DIRECTORY *)ImageDirectoryOffset(win32file, IMAGE_DIRECTORY_ENTRY_TLS))
        {
            Section *sect;
            BOOL     fTLSFixups = FALSE;

            sect = findSectionByAddr(tlsDir->StartAddressOfRawData);
            //There might be fixups for the TLS structure, so search the sections
            //by the OS/2 virtual address too
            if(sect == NULL) {
                sect = findSectionByOS2Addr(tlsDir->StartAddressOfRawData);
                fTLSFixups = TRUE;
            }

            dprintf((LOG, "TLS Directory" ));
            dprintf((LOG, "TLS Address of Index     %x", tlsDir->AddressOfIndex ));
            dprintf((LOG, "TLS Address of Callbacks %x", tlsDir->AddressOfCallBacks ));
            dprintf((LOG, "TLS SizeOfZeroFill       %x", tlsDir->SizeOfZeroFill ));
            dprintf((LOG, "TLS Characteristics      %x", tlsDir->Characteristics ));
            if(sect == NULL) {
                dprintf((LOG, "Couldn't find TLS section!!" ));
                goto failure;
            }
            setTLSAddress((char *)sect->realvirtaddr);
            setTLSInitSize(tlsDir->EndAddressOfRawData - tlsDir->StartAddressOfRawData);
            setTLSTotalSize(tlsDir->EndAddressOfRawData - tlsDir->StartAddressOfRawData + tlsDir->SizeOfZeroFill);

            fTLSFixups = FALSE;
            sect = findSectionByAddr((ULONG)tlsDir->AddressOfIndex);
            //There might be fixups for the TLS structure, so search the sections
            //by the OS/2 virtual address too
            if(sect == NULL) {
                sect = findSectionByOS2Addr((ULONG)tlsDir->AddressOfIndex);
                fTLSFixups = TRUE;
            }
            if(sect == NULL) {
                dprintf((LOG, "Couldn't find TLS AddressOfIndex section!!" ));
                goto failure;
            }
            if(fTLSFixups) {
                setTLSIndexAddr((LPDWORD)tlsDir->AddressOfIndex);  //no fixup required
            }
            else {//need to add a manual fixup
                setTLSIndexAddr((LPDWORD)(sect->realvirtaddr + ((ULONG)tlsDir->AddressOfIndex - sect->virtaddr)));
            }

            if((ULONG)tlsDir->AddressOfCallBacks != 0)
            {
                fTLSFixups = FALSE;

                sect = findSectionByAddr((ULONG)tlsDir->AddressOfCallBacks);
                //There might be fixups for the TLS structure, so search the sections
                //by the OS/2 virtual address too
                if(sect == NULL) {
                    sect = findSectionByOS2Addr((ULONG)tlsDir->AddressOfIndex);
                    fTLSFixups = TRUE;
                }
                if(sect == NULL) {
                    dprintf((LOG, "Couldn't find TLS AddressOfCallBacks section!!" ));
                    goto failure;
                }
                if(fTLSFixups) {
                    setTLSCallBackAddr((PIMAGE_TLS_CALLBACK *)tlsDir->AddressOfCallBacks); //no fixup required
                }
                else {//need to add a manual fixup
                    setTLSCallBackAddr((PIMAGE_TLS_CALLBACK *)(sect->realvirtaddr + ((ULONG)tlsDir->AddressOfCallBacks - sect->virtaddr)));
                }
                //modify tls callback pointers for new image base address
                int i = 0;
                while(tlsCallBackAddr[i])
                {
                    fTLSFixups = FALSE;

                    sect = findSectionByAddr((ULONG)tlsCallBackAddr[i]);
                    //There might be fixups for the TLS structure, so search the sections
                    //by the OS/2 virtual address too
                    if(sect == NULL) {
                        sect = findSectionByOS2Addr((ULONG)tlsCallBackAddr[i]);
                        fTLSFixups = TRUE;
                    }
                    if(sect == NULL) {
                        dprintf((LOG, "Couldn't find TLS callback section!!" ));
                        goto failure;
                    }
                    if(fTLSFixups) {
                            tlsCallBackAddr[i] = tlsCallBackAddr[i];
                    }
                    else    tlsCallBackAddr[i] = (PIMAGE_TLS_CALLBACK)(realBaseAddress + ((ULONG)tlsCallBackAddr[i] - oh.ImageBase));
                    i++;
                }
            }
        }

#ifdef DEBUG
        dprintf((LOG, "Image directories: "));
        for (i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
        {
            char *pszName;

            if(oh.DataDirectory[i].VirtualAddress && oh.DataDirectory[i].Size) {
                switch (i)
                {
                case IMAGE_DIRECTORY_ENTRY_EXPORT:      pszName = "Export Directory (IMAGE_DIRECTORY_ENTRY_EXPORT)"; break;
                case IMAGE_DIRECTORY_ENTRY_IMPORT:      pszName = "Import Directory (IMAGE_DIRECTORY_ENTRY_IMPORT)"; break;
                case IMAGE_DIRECTORY_ENTRY_RESOURCE:    pszName = "Resource Directory (IMAGE_DIRECTORY_ENTRY_RESOURCE)"; break;
                case IMAGE_DIRECTORY_ENTRY_EXCEPTION:   pszName = "Exception Directory (IMAGE_DIRECTORY_ENTRY_EXCEPTION)"; break;
                case IMAGE_DIRECTORY_ENTRY_SECURITY:    pszName = "Security Directory (IMAGE_DIRECTORY_ENTRY_SECURITY)"; break;
                case IMAGE_DIRECTORY_ENTRY_BASERELOC:   pszName = "Base Relocation Table (IMAGE_DIRECTORY_ENTRY_BASERELOC)"; break;
                case IMAGE_DIRECTORY_ENTRY_DEBUG:       pszName = "Debug Directory (IMAGE_DIRECTORY_ENTRY_DEBUG)"; break;
                case IMAGE_DIRECTORY_ENTRY_COPYRIGHT:   pszName = "Description String (IMAGE_DIRECTORY_ENTRY_COPYRIGHT)"; break;
                case IMAGE_DIRECTORY_ENTRY_GLOBALPTR:   pszName = "Machine Value (MIPS GP) (IMAGE_DIRECTORY_ENTRY_GLOBALPTR)"; break;
                case IMAGE_DIRECTORY_ENTRY_TLS:         pszName = "TLS Directory (IMAGE_DIRECTORY_ENTRY_TLS)"; break;
                case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG: pszName = "Load Configuration Directory (IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG)"; break;
                case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT:pszName = "Bound Import Directory in headers (IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT)"; break;
                case IMAGE_DIRECTORY_ENTRY_IAT:         pszName = "Import Address Table (IMAGE_DIRECTORY_ENTRY_IAT)"; break;
                default:
                    pszName = "unknown";
                }
                dprintf((LOG, "directory %s", pszName));
                dprintf((LOG, "          Address    0x%08x", oh.DataDirectory[i].VirtualAddress));
                dprintf((LOG, "          Size       0x%08x", oh.DataDirectory[i].Size));
            }
        }
        dprintf((LOG, "\n\n"));
#endif

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

#ifndef COMMIT_ALL
    if(entryPoint) {
        //commit code at entrypoint, since we going to call it anyway
        commitPage((ULONG)entryPoint, FALSE);
    }
#endif

    //SvL: Use pointer to image header as module handle now. Some apps needs this
    hinstance = (HINSTANCE)realBaseAddress;

    //SvL: Set instance handle in process database structure
    SetPDBInstance(hinstance);

    //PH: get pResRootDir pointer correct first, since processImports may
    //    implicitly call functions depending on it.
    if(oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress && oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size)
    {
        //get offset in resource object of directory entry
        pResRootDir = (PIMAGE_RESOURCE_DIRECTORY)(oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress + realBaseAddress);
        ulRVAResourceSection = oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
    }

    //Allocate TLS index for this module
    //Must do this before dlls are loaded for this module. Some apps assume
    //they get TLS index 0 for their main executable
    {
      USHORT sel = SetWin32TIB(TIB_SWITCH_FORCE_WIN32);
      tlsAlloc();
      tlsAttachThread();  //setup TLS (main thread)
      SetFS(sel);
    }

    if(!(dwFlags & (FLAG_PELDR_LOADASDATAFILE | FLAG_PELDR_SKIPIMPORTS)))
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
//******************************************************************************
#define DOSREAD_IDEAL_SIZE 61440
static inline APIRET _Optlink fastDosRead(HFILE hFile,
                                          PVOID pAddress,
                                          ULONG ulSize,
                                          PULONG pulBytesRead)
{
    /* we better break the DosRead into multiple calls */
    PBYTE  p = (PBYTE)pAddress;
    ULONG  ulReadBytes;
    APIRET rc;

    *pulBytesRead = ulSize;

    do
    {
        rc = DosRead(hFile,
                     p,
                     min(DOSREAD_IDEAL_SIZE, ulSize),
                     &ulReadBytes);
        if (rc != NO_ERROR)
        {
            /* in case of errors bail out */
            *pulBytesRead = 0;
            return rc;
        }
      
      // PH 2001-11-15
      // For corrupt or misinterpreted PE headers,
      // the preceeding DosSetFilePtr may have moved the
      // file pointer at or behind the file's end.
      // DosRead will then return rc=0 and ulReadBytes=0.
      // This must NOT lead to an infinite loop.
      if (ulReadBytes == 0)
      {
        return ERROR_INTERNAL;
      }

        ulSize -= ulReadBytes;
        p += ulReadBytes;
    }
    while (ulSize > 0);

    return NO_ERROR;
}

//******************************************************************************
//  commitPage:
//      commits image page(s) when an access violation exception is received
//      (usually called from exception.cpp; also from other methods in this file)
//
//  Parameters:
//      virtAddress     - address of exception (rounded down to page boundary)
//      fWriteAccess    - type of access violation (read or write)
//      fPageCmd        - SINGLE_PAGE       -> commit single page
//                        SECTION_PAGES     -> commit default nr of pages
//                        COMPLETE_SECTION  -> commit entire section
//
//  Remarks:
//      DosEnterCritSec/DosExitCritSec is used to make sure the other threads in
//      the application can't touch the pages before they are loaded from disk and
//      fixups are applied.
//
//  TODO:
//      SECTION_PAGES:  - don't load pages starting at access violation address, but
//                        a region surrounding it (e.g. -32k -> + 32k)
//                        this will prevent many pagefaults when the app uses
//                        pages with a lower addr.
//
//******************************************************************************
BOOL Win32PeLdrImage::commitPage(ULONG virtAddress, BOOL fWriteAccess, int fPageCmd)
{
 Section *section;
 ULONG    offset, size, sectionsize, protflags, fileoffset, range, attr;
 ULONG    ulNewPos, ulRead, orgVirtAddress = virtAddress;
 APIRET   rc;

    dprintf((LOG, "Win32PeLdrImage::commitPage %x %d %d", virtAddress, fWriteAccess, fPageCmd));
    if(virtAddress == 0) {
        return FALSE;
    }

    //Round down to nearest page boundary
    virtAddress = virtAddress & ~0xFFF;

    section = findSectionByOS2Addr(virtAddress);
    if(section == NULL) {
        section = findSectionByOS2Addr(orgVirtAddress);
        if(section) {
            virtAddress = orgVirtAddress;
        }
    }
    if(section == NULL) {
        size        = 4096;
        sectionsize = 4096;
        //Header page must be readonly (same as in NT)
        protflags   = PAG_READ;
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
    if(fPageCmd == COMPLETE_SECTION && (section && section->type == SECTION_DEBUG)) {//ignore
        return TRUE;
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
    //else complete section

    size = min(size, range);
    sectionsize = min(sectionsize, range);

    if(size && fileoffset != -1) {
        rc = DosEnterCritSec();
        if(rc) {
            dprintf((LOG, "DosEnterCritSec failed with rc %d", rc));
            goto fail;
        }
        rc = DosSetMem((PVOID)virtAddress, sectionsize, PAG_READ|PAG_WRITE|PAG_COMMIT);
        if(rc) {
            DosExitCritSec();
            dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
            goto fail;
        }

        if(DosSetFilePtr(hFile, ulPEOffset+fileoffset, FILE_BEGIN, &ulNewPos) == -1) {
            DosExitCritSec();
            dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetFilePtr failed for 0x%x!", fileoffset));
            goto fail;
        }
#if 1
        // 2001-05-31 PH
        // ensure DosRead() does not have to read more
        // than 65535 bytes, otherwise split into two requests!
        rc = fastDosRead(hFile, (PVOID)virtAddress, size, &ulRead);
#else
        rc = DosRead(hFile, (PVOID)virtAddress, size, &ulRead);
#endif
        if(rc) {
            DosExitCritSec();
            dprintf((LOG, "Win32PeLdrImage::commitPage: DosRead failed for 0x%x %x %x %x (rc=%d)!", virtAddress, size, ulRead, fileoffset, rc));
            goto fail;
        }
        if(ulRead != size) {
            DosExitCritSec();
            dprintf((LOG, "Win32PeLdrImage::commitPage: DosRead failed to read %x (%x) bytes at %x for 0x%x!", size, ulRead, fileoffset, virtAddress));
            goto fail;
        }
        setFixups(virtAddress, sectionsize);

        rc = DosSetMem((PVOID)virtAddress, sectionsize, protflags);
        DosExitCritSec();
        if(rc) {
            dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
            goto fail;
        }
    }
    else {
        rc = DosEnterCritSec();
        if(rc) {
            dprintf((LOG, "DosEnterCritSec failed with rc %d", rc));
            goto fail;
        }

        rc = DosSetMem((PVOID)virtAddress, sectionsize, PAG_READ|PAG_WRITE|PAG_COMMIT);
        if(rc) {
            DosExitCritSec();
            dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
            goto fail;
        }
        setFixups(virtAddress, sectionsize);

        rc = DosSetMem((PVOID)virtAddress, sectionsize, protflags);
        DosExitCritSec();
        if(rc) {
            dprintf((LOG, "Win32PeLdrImage::commitPage: DosSetMem failed (%d)!", rc));
            goto fail;
        }
    }
    return TRUE;

fail:
    return FALSE;
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

    //Allocated in by pe.exe
    if(reservedMem && reservedMem == oh.ImageBase) {
        realBaseAddress = oh.ImageBase;
        return TRUE;
    }

    //SvL: We don't care where the image is loaded for resource lookup
    if(fh.Characteristics & IMAGE_FILE_RELOCS_STRIPPED && !(dwFlags & FLAG_PELDR_LOADASDATAFILE)) {
        return allocFixedMem(reservedMem);
    }
    rc = OSLibDosAllocMem((PPVOID)&baseAddress, imageSize, PAG_READ | PAG_WRITE);
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
 LONG  index = -1;

    for(int i=0;i<nrsections;i++) {
        if(section[i].realvirtaddr <= addr) {
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
 BOOL   fLowMemory = FALSE;

    //Reserve enough space to store 4096 pointers to 1MB memory chunks
    memallocs = (ULONG *)malloc(4096*sizeof(ULONG *));
    if(memallocs == NULL) {
        dprintf((LOG, "allocFixedMem: MALLOC FAILED for memallocs" ));
        return FALSE;
    }

    if(oh.ImageBase < 512*1024*1024) {
        fLowMemory = TRUE;
    }
    while(TRUE) {
        rc = OSLibDosAllocMem((PPVOID)&address, FALLOC_SIZE, PAG_READ, fLowMemory);
        if(rc) break;

        dprintf((LOG, "DosAllocMem returned %x", address ));
        if(address + FALLOC_SIZE >= oh.ImageBase) {
            if(address > oh.ImageBase) {//we've passed it!
                OSLibDosFreeMem((PVOID)address);
                break;
            }
            //found the right address
            OSLibDosFreeMem((PVOID)address);

            diff = oh.ImageBase - address;
            if(diff) {
                rc = OSLibDosAllocMem((PPVOID)&address, diff, PAG_READ, fLowMemory);
                if(rc) break;
            }
            rc = OSLibDosAllocMem((PPVOID)&baseAddress, imageSize, PAG_READ | PAG_WRITE, fLowMemory);
            if(rc) break;

            if(diff) OSLibDosFreeMem((PVOID)address);

            realBaseAddress = baseAddress;
            break;
        }
        memallocs[alloccnt++] = address;
    }
    for(i=0;i<alloccnt;i++) {
        OSLibDosFreeMem((PVOID)memallocs[i]);
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
        case SECTION_IMPORT:
        case SECTION_TLS:
            section[i].pageflags = PAG_WRITE | PAG_READ;
            break;

        case SECTION_RESOURCE:
            //TODO: GDI32 changes some bitmap structures to avoid problems in Open32
            //      -> causes crashes if resource section is readonly
            //      -> make it readonly again when gdi32 has been rewritten
            section[i].pageflags = PAG_WRITE | PAG_READ;
            break;

        case SECTION_READONLYDATA:
        case SECTION_EXPORT:
        default:
            section[i].pageflags = PAG_READ;
            break;
        }
        if(section[i].flags & (IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_CNT_UNINITIALIZED_DATA)) {
            //SvL: sometimes i.e. import/export sections also contain data
            //     must make them read/write
            section[i].pageflags = PAG_WRITE;
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

                    //SvL: Enable write access (TODO: may need to prevent other threads from being active)
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
                    //SvL: Restore original page protection flags (TODO: may need to prevent other threads from being active)
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
#define MISSINGOFFSET_PUSHNAME        1
#define MISSINGOFFSET_PUSHORDINAL     1
#define MISSINGOFFSET_PUSHDLLNAME     6
#define MISSINGOFFSET_PUSHIMPORTIMAGE 11
#define MISSINGOFFSET_FUNCTION        16

char missingapicode[23] = {
//push  ordinal/name
        0x68, 0x00, 0x00, 0x00, 0x00,
//push  dllname
        0x68, 0x00, 0x00, 0x00, 0x00,
//push  image loading api
        0x68, 0x00, 0x00, 0x00, 0x00,
//mov   ecx, MissingApiOrd/Name
        0xB9, 0x99, 0x99, 0x99, 0x99,
//call  ecx
        0xFF, 0xD1,
//ret
        0xC3};

//******************************************************************************
void Win32PeLdrImage::StoreImportByOrd(Win32ImageBase *WinImage, ULONG ordinal, ULONG impaddr)
{
 ULONG *import;
 ULONG  apiaddr;

    import  = (ULONG *)impaddr;
    apiaddr = WinImage->getApi(ordinal);
    if(apiaddr == 0)
    {
        dprintf((LOG, "KERNEL32:Win32PeLdrImage - %s.%u not found\n",
                 WinImage->getModuleName(),
                 ordinal));

        dprintf((LOG, "--->>> NOT FOUND!" ));
        char *code = (char *)_cmalloc(sizeof(missingapicode));

        memcpy(code, missingapicode, sizeof(missingapicode));
        *(DWORD *)&code[MISSINGOFFSET_PUSHIMPORTIMAGE] = (DWORD)getModuleName();
        *(DWORD *)&code[MISSINGOFFSET_PUSHDLLNAME] = (DWORD)WinImage->getModuleName();
        *(DWORD *)&code[MISSINGOFFSET_PUSHORDINAL] = ordinal;
        *(DWORD *)&code[MISSINGOFFSET_FUNCTION]    = (DWORD)MissingApiOrd;
        *import = (ULONG)code;
    }
    else *import = apiaddr;
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::StoreImportByName(Win32ImageBase *WinImage, char *impname, ULONG impaddr)
{
 ULONG *import;
 ULONG  apiaddr;

    import = (ULONG *)impaddr;
    apiaddr = WinImage->getApi(impname);
    if(apiaddr == 0)
    {
        dprintf((LOG, "KERNEL32:Win32PeLdrImage - %s.%s not found\n",
                WinImage->getModuleName(),
                impname));

        dprintf((LOG, "--->>> NOT FOUND!" ));

        char *code = (char *)_cmalloc(sizeof(missingapicode));

        memcpy(code, missingapicode, sizeof(missingapicode));
        *(DWORD *)&code[MISSINGOFFSET_PUSHIMPORTIMAGE] = (DWORD)getModuleName();
        *(DWORD *)&code[MISSINGOFFSET_PUSHDLLNAME] = (DWORD)WinImage->getModuleName();
        *(DWORD *)&code[MISSINGOFFSET_PUSHNAME]    = (DWORD)impname;
        *(DWORD *)&code[MISSINGOFFSET_FUNCTION]    = (DWORD)MissingApiName;
        *import = (ULONG)code;
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
 BOOL fForwarder;
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
    for(i=0;i<ped->NumberOfNames;i++)
    {
    fForwarder = FALSE;
        ord        = ptrOrd[i] + ped->Base;
        name       = (char *)((ULONG)ptrNames[i] + (ULONG)win32file);
        RVAExport  = ptrAddress[ptrOrd[i]];

        /* forwarder? ulRVA within export directory. */
        if(RVAExport > oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress &&
           RVAExport < oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
                       + oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
        {
            fForwarder = AddForwarder(oh.ImageBase + RVAExport, name, ord);
        }
    if(!fForwarder) {
            //points to code (virtual address relative to oh.ImageBase
            AddNameExport(oh.ImageBase + RVAExport, name, ord);
            dprintf((LOG, "address 0x%x %s @%d (0x%08x)", RVAExport, name, ord, realBaseAddress + RVAExport));
        }
    }
    for(i=0;i<max(ped->NumberOfNames,ped->NumberOfFunctions);i++)
    {
    fForwarder = FALSE;
        ord = ped->Base + i;  //Correct??
        RVAExport = ptrAddress[i];
        /* forwarder? ulRVA within export directory. */
        if(RVAExport > oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress &&
           RVAExport < oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
                       + oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
        {
            fForwarder = AddForwarder(oh.ImageBase + RVAExport, NULL, ord);
        }
    if(!fForwarder && RVAExport) {
            //points to code (virtual address relative to oh.ImageBase
            dprintf((LOG, "ord %d at 0x%08x (0x%08x)", ord, RVAExport, realBaseAddress + RVAExport));
            AddOrdExport(oh.ImageBase + RVAExport, ord);
        }
    }
  }

  return(TRUE);
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::AddNameExport(ULONG virtaddr, char *apiname, ULONG ordinal, BOOL fAbsoluteAddress)
{
    ULONG nsize;
    int iApiNameLength = strlen(apiname);

    if(nameexports == NULL) {
        // think of a maximum of bytes per export name,
        // verify if this is true for MFC-DLLs, etc.
        nameExportSize = nrNameExports * (sizeof(NameExport) + 32);

        nameexports   = (NameExport *)malloc(nameExportSize);
        curnameexport = nameexports;
    }
    nsize = (ULONG)curnameexport - (ULONG)nameexports;
    if(nsize + sizeof(NameExport) + iApiNameLength > nameExportSize) {
        nameExportSize += 4096;
        char *tmp = (char *)nameexports;
        nameexports = (NameExport *)malloc(nameExportSize);
        memcpy(nameexports, tmp, nsize);
        curnameexport = (NameExport *)((ULONG)nameexports + nsize);
        free(tmp);
    }
    if(fAbsoluteAddress) {//forwarders use absolute address
        curnameexport->virtaddr = virtaddr;
    }
    else curnameexport->virtaddr = realBaseAddress + (virtaddr - oh.ImageBase);
    curnameexport->ordinal  = ordinal;
    *(ULONG *)curnameexport->name = 0;

    curnameexport->nlength = iApiNameLength + 1;
    memcpy(curnameexport->name, apiname,  curnameexport->nlength);

    if(curnameexport->nlength < sizeof(curnameexport->name))
        curnameexport->nlength = sizeof(curnameexport->name);

    curnameexport = (NameExport *)((ULONG)curnameexport->name + curnameexport->nlength);
}
//******************************************************************************
//******************************************************************************
void Win32PeLdrImage::AddOrdExport(ULONG virtaddr, ULONG ordinal, BOOL fAbsoluteAddress)
{
    if(ordexports == NULL) {
        ordexports   = (OrdExport *)malloc(nrOrdExports * sizeof(OrdExport));
        curordexport = ordexports;
    }
    if(fAbsoluteAddress) {//forwarders use absolute address
        curordexport->virtaddr = virtaddr;
    }
    else curordexport->virtaddr = realBaseAddress + (virtaddr - oh.ImageBase);

    curordexport->ordinal  = ordinal;
    curordexport++;
    nrOrdExportsRegistered++;
}
//******************************************************************************
//******************************************************************************
BOOL Win32PeLdrImage::AddForwarder(ULONG virtaddr, char *apiname, ULONG ordinal)
{
    char         *forward = (char *)(realBaseAddress + (virtaddr - oh.ImageBase));
    char         *forwarddll, *forwardapi;
    Win32DllBase *WinDll;
    DWORD         exportaddr;
    int           forwardord;
    int           iForwardDllLength = strlen(forward);
    int           iForwardApiLength;

    if(iForwardDllLength == 0)
        return FALSE;

    forwarddll = (char*)alloca(iForwardDllLength);
    if(forwarddll == NULL) {
        DebugInt3();
        return FALSE;
    }
    memcpy(forwarddll, forward, iForwardDllLength + 1);

    forwardapi = strchr(forwarddll, '.');
    if(forwardapi == NULL) {
        return FALSE;
    }
    *forwardapi++ = 0;
    iForwardApiLength = strlen(forwardapi);
    if(iForwardApiLength == 0) {
        return FALSE;
    }
    WinDll = Win32DllBase::findModule(forwarddll);
    if(WinDll == NULL) {
        WinDll = loadDll(forwarddll);
        if(WinDll == NULL) {
            dprintf((LOG, "ERROR: couldn't find forwarder %s.%s", forwarddll, forwardapi));
            return FALSE;
        }
    }
    //check if name or ordinal forwarder
    forwardord = 0;
    if(*forwardapi >= '0' && *forwardapi <= '9') {
        forwardord = atoi(forwardapi);
    }
    if(forwardord != 0 || (iForwardApiLength == 1 && *forwardapi == '0')) {
         exportaddr = WinDll->getApi(forwardord);
    }
    else exportaddr = WinDll->getApi(forwardapi);

    if(apiname) {
        dprintf((LOG, "address 0x%x %s @%d (0x%08x) forwarder %s.%s", virtaddr - oh.ImageBase, apiname, ordinal, virtaddr, forwarddll, forwardapi));
        AddNameExport(exportaddr, apiname, ordinal, TRUE);
    }
    else {
        dprintf((LOG, "address 0x%x @%d (0x%08x) forwarder %s.%s", virtaddr - oh.ImageBase, ordinal, virtaddr, forwarddll, forwardapi));
        AddOrdExport(exportaddr, ordinal, TRUE);
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
Win32DllBase *Win32PeLdrImage::loadDll(char *pszCurModule)
{
 Win32DllBase *WinDll = NULL;
 char modname[CCHMAXPATH];

    strcpy(modname, pszCurModule);

    //rename dll if necessary (i.e. OLE32 -> OLE32OS2)
    Win32DllBase::renameDll(modname);

    char szModName2[CCHMAXPATH];
    strcpy(szModName2, modname);
    if (!Win32ImageBase::findDll(szModName2, modname, sizeof(modname)))
    {
            dprintf((LOG, "Module %s not found!", modname));
            sprintf(szErrorModule, "%s", modname);
            errorState = 2;
            return NULL;
    }

    if(isPEImage(modname, NULL, NULL) != ERROR_SUCCESS_W)
    {//LX image, so let OS/2 do all the work for us
        APIRET rc;
        char   szModuleFailure[CCHMAXPATH] = "";
        ULONG  hInstanceNewDll;
        Win32LxDll *lxdll;

        char *dot = strchr(modname, '.');
        if(dot == NULL) {
            strcat(modname, DLL_EXTENSION);
        }
        rc = DosLoadModule(szModuleFailure, sizeof(szModuleFailure), modname, (HMODULE *)&hInstanceNewDll);
        if(rc) {
            dprintf((LOG, "DosLoadModule returned %X for %s", rc, szModuleFailure));
            sprintf(szErrorModule, "%s", szModuleFailure);
            errorState = rc;
            return NULL;
        }
        lxdll = Win32LxDll::findModuleByOS2Handle(hInstanceNewDll);
        if(lxdll == NULL) {//shouldn't happen!
            dprintf((LOG, "Just loaded the dll, but can't find it anywhere?!!?"));
            errorState = ERROR_INTERNAL;
            return NULL;
        }
        lxdll->setDllHandleOS2(hInstanceNewDll);
        if(lxdll->AddRef() == -1) {//-1 -> load failed (attachProcess)
            dprintf((LOG, "Dll %s refused to be loaded; aborting", modname));
            delete lxdll;
            errorState = ERROR_INTERNAL;
            return NULL;
        }
        WinDll = (Win32DllBase*)lxdll;
    }
    else {
         Win32PeLdrDll *pedll;

            pedll = new Win32PeLdrDll(modname, this);
            if(pedll == NULL) {
                dprintf((LOG, "pedll: Error allocating memory" ));
                WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szMemErrorMsg, szErrorTitle, 0, MB_OK | MB_ERROR | MB_MOVEABLE);
                errorState = ERROR_INTERNAL;
                return NULL;
            }
            dprintf((LOG, "**********************************************************************" ));
            dprintf((LOG, "**********************     Loading Module        *********************" ));
            dprintf((LOG, "**********************************************************************" ));
            if(pedll->init(0) == FALSE) {
                dprintf((LOG, "Internal WinDll error ", pedll->getError() ));
                delete pedll;
                return NULL;
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
                return NULL;
            }
        WinDll = (Win32DllBase*)pedll;
    }

    dprintf((LOG, "**********************************************************************" ));
    dprintf((LOG, "**********************  Finished Loading Module %s ", modname ));
    dprintf((LOG, "**********************************************************************" ));

    return WinDll;
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
    Win32DllBase    *WinDll;
    Win32ImageBase  *WinImage = NULL;
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

    pszModules = (char*)alloca(Size);
    if(pszModules == NULL) {
        DebugInt3();
        return FALSE;
    }
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
                    return FALSE;
                }
            }
            pszTmp = (char*)(pID[i].Name + (ULONG)win32file);
        }

        int iTmpLength = strlen(pszTmp) + 1;
        memcpy(pszModules+j, pszTmp, iTmpLength);
        j += iTmpLength;
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
        if(pID[i].ForwarderChain) {
            dprintf((LOG, "ForwarderChain: %x", pID[i].ForwarderChain));
        }
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
        else pulImport = (ULONG*)pID[i].u.OriginalFirstThunk;

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

        //Load dll if needed
        dprintf((LOG, "**********************************************************************" ));
        dprintf((LOG, "************** Import Module %s ", pszCurModule ));
        dprintf((LOG, "**********************************************************************" ));
        WinDll = Win32DllBase::findModule(pszCurModule);

        if(WinDll == NULL)
        {  //not found, so load it
            if (WinExe != NULL && WinExe->matchModName(pszCurModule)) {
                WinImage = (Win32ImageBase *)WinExe;
            }
            else {
                WinDll = loadDll(pszCurModule);
                if(WinDll == NULL) {
                    return FALSE;
                }
            }
        }
        else {
            WinDll->AddRef();
            dprintf((LOG, "Already found ", pszCurModule));
        }
        if(WinDll != NULL) {
            //add the dll we just loaded to dependency list for this image
            addDependency(WinDll);

            //Make sure the dependency list is correct (already done
            //in the ctor of Win32DllBase, but for LX dlls the parent is
            //then set to NULL; so change it here again
            WinDll->setUnloadOrder(this);
            WinImage = (Win32ImageBase *)WinDll;
        }
        else
        if(WinImage == NULL) {
            dprintf((LOG, "Unable to load dll %s", pszCurModule ));
            return FALSE;
        }

        pulImport  = (PULONG)((ULONG)pulImport + (ULONG)win32file);
        j          = 0;
        ulCurFixup = (ULONG)pID[i].FirstThunk + (ULONG)win32file;

        section    = findSectionByOS2Addr(ulCurFixup);
        if(section == NULL) {
            dprintf((LOG, "Unable to find section for %x", ulCurFixup ));
            return FALSE;
        }
        //Read page from disk
        commitPage(ulCurFixup & ~0xfff, FALSE, SINGLE_PAGE);
        //Enable write access
        DosSetMem((PVOID)(ulCurFixup & ~0xfff), PAGE_SIZE, PAG_READ|PAG_WRITE);
        nrPages    = 1;

        while (pulImport[j] != 0) {
            if (pulImport[j] & IMAGE_ORDINAL_FLAG) { //ordinal
                dprintf((LOG, "0x%08x Imported function %s @%d", ulCurFixup , pszCurModule, (pulImport[j] & ~IMAGE_ORDINAL_FLAG) ));
                StoreImportByOrd(WinImage, pulImport[j] & ~IMAGE_ORDINAL_FLAG, ulCurFixup);
            }
            else {  //name
                //check
                if (pulImport[j] < shExtra.VirtualAddress || pulImport[j] >= shExtra.VirtualAddress + max(shExtra.Misc.VirtualSize, shExtra.SizeOfRawData))
                {
                    if (!GetSectionHdrByRVA(win32file, &shExtra, pulImport[j]))
                    {
                        dprintf((LOG, "warning: could not find section for Import Name RVA ", pulImport[j] ));
                        break;
                    }
                }
                //KSO - Aug 6 1998 1:15am:this eases comparing...
                char *pszFunctionName = (char*)(pulImport[j] + (ULONG)win32file + 2);
                dprintf((LOG, "0x%08x Imported function %s (0x%08x)", ulCurFixup,  pszFunctionName, WinImage->getApi(pszFunctionName)));
                StoreImportByName(WinImage, pszFunctionName, ulCurFixup);
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
    else  apiname = name;

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

  /* accelerated resolving of ordinal exports
   * is based on the assumption the ordinal export
   * table is always sorted ascending.
   *
   * When the step size is too small, we continue
   * with the linear search.
   */

  // start in the middle of the tree
  i = nrOrdExportsRegistered >> 1;
  int iStep = i;

  for(;;)
  {
    int iThisExport = curexport[i].ordinal;

    iStep >>= 1;                    // next step will be narrower

    if (iThisExport < ordinal)
      i += min(iStep, (ordinal-iThisExport)); // move farther down the list
    else
      if (iThisExport == ordinal)   // found the export?
        return curexport[i].virtaddr;
      else
        i -= min(iStep, (iThisExport-ordinal));                 // move farther up the list

    // if we're in the direct neighbourhood search linearly
    if (iStep <= 1)
    {
      // decide if we're to search backward or forward
      if (ordinal > curexport[i].ordinal)
      {
        // As a certain number of exports are 0 at the end
        // of the array, this case will hit fairly often.
        // the last comparison will send the loop off into the
        // wrong direction!
#ifdef DEBUG
        if (curexport[i].ordinal == 0)
        {
            DebugInt3();
        }
#endif

        for (;i<nrOrdExports;i++) // scan forward
        {
          iThisExport = curexport[i].ordinal;
          if(iThisExport == ordinal)
            return(curexport[i].virtaddr);
          else
            if (iThisExport > ordinal)
            {
              // Oops, cannot find the ordinal in the sorted list
              break;
            }
        }
      }
      else
      {
        for (;i>=0;i--) // scan backward
        {
          iThisExport = curexport[i].ordinal;
          if(curexport[i].ordinal == ordinal)
            return(curexport[i].virtaddr);
          else
            if (iThisExport < ordinal)
              // Oops, cannot find the ordinal in the sorted list
              break;
        }
      }

      // not found yet.
      break;
    }
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
ULONG WIN32API MissingApiOrd(char *parentimage, char *dllname, int ordinal)
{
   char message[256];

   sprintf(message, "The application has called the non-existing api %s->%d (loaded by %s)", dllname, ordinal, parentimage);
   return MissingApi(message);
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API MissingApiName(char *parentimage, char *dllname, char *functionname)
{
   char message[256];

   sprintf(message, "The application has called the non-existing api %s->%s (loaded by %s)", dllname, functionname, parentimage);
   return MissingApi(message);
}
//******************************************************************************
//******************************************************************************
ULONG WIN32API MissingApi(char *message)
{
 static BOOL fIgnore = FALSE;
 int r;

    dprintf((LOG, "Missing api called!\n"));
    if(fIgnore)
        return(0);

    do {
        r = WinMessageBox(HWND_DESKTOP, NULLHANDLE, message,
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
