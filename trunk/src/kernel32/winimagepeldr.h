/* $Id: winimagepeldr.h,v 1.21 2004-01-15 10:39:15 sandervl Exp $ */

/*
 * Win32 PE loader Image base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINIMAGEPELDR_H__
#define __WINIMAGEPELDR_H__

#include "winimagebase.h"

#define SINGLE_PAGE 		0  //commit single page
#define COMPLETE_SECTION 	1  //commit entire section
#define SECTION_PAGES		2  //commit default nr of pages

#define DEFAULT_NR_PAGES        16 //default nr of pages to commit during exception

//SvL: To load a dll/exe for i.e. getting a single resource (GetVersionSize/Resource)
#define FLAG_PELDR_LOADASDATAFILE	1  //also implies FLAG_PELDR_SKIPIMPORTS
#define FLAG_PELDR_SKIPIMPORTS		2

//SvL: Amount of memory the peldr dll reserves for win32 exes without fixups
//(most of them need to be loaded at 4 MB; except MS Office apps of course)
#define PELDR_RESERVEDMEMSIZE	32*1024*1024

#define ERROR_INTERNAL          1

#define SECTION_CODE            1
#define SECTION_INITDATA        2
#define SECTION_UNINITDATA      4
#define SECTION_READONLYDATA    8
#define SECTION_IMPORT          16
#define SECTION_RESOURCE        32
#define SECTION_RELOC           64
#define SECTION_EXPORT          128
#define SECTION_DEBUG           256
#define SECTION_TLS             512

#define PAGE_SIZE               4096

typedef struct {
  ULONG  rawoffset;
  ULONG  rawsize;
  ULONG  virtaddr;
  ULONG  realvirtaddr;  //as allocated in OS/2
  ULONG  virtualsize;
  ULONG  type;
  ULONG  pageflags;
  ULONG  flags;         //psh[i].Characteristics
} Section;

typedef struct {
  ULONG  virtaddr;
  ULONG  ordinal;
  ULONG  nlength;
  char   name[4];
} NameExport;

typedef struct {
  ULONG  virtaddr;
  ULONG  ordinal;
} OrdExport;

class Win32DllBase;
class Win32MemMap;

class Win32PeLdrImage : public virtual Win32ImageBase
{
public:
         Win32PeLdrImage(char *szFileName, BOOL isExe);
virtual ~Win32PeLdrImage();

    //reservedMem: address of memory reserved in peldr.dll (allocated before
    //             any dlls are loaded, so that exes without fixups can be 
    //             loaded at a low address)
    //ulPEOffset:  offset in file where real PE image starts
    virtual DWORD init(ULONG reservedMem, ULONG ulPEOffset = 0);

    virtual BOOL  insideModule(ULONG address);
    virtual BOOL  insideModuleCode(ULONG address);

    virtual ULONG getImageSize();

    //Returns required OS version for this image
    virtual ULONG getVersion();

        //tell loader to only process resources and ignore the rest
        void  setLoadAsDataFile()     { dwFlags |= FLAG_PELDR_LOADASDATAFILE; };
        void  disableImportHandling() { dwFlags |= FLAG_PELDR_SKIPIMPORTS; };

        //commits image page(s) when an access violation exception is dispatched
	BOOL  commitPage(ULONG virtAddress, BOOL fWriteAccess, int fPageCmd = SECTION_PAGES);

protected:
        void  StoreImportByOrd(Win32ImageBase *WinImage, ULONG ordinal, ULONG impaddr);
        void  StoreImportByName(Win32ImageBase *WinImage, char *impname, ULONG impaddr);

        void  addSection(ULONG type, ULONG rawoffset, ULONG rawsize, ULONG virtaddress, ULONG virtsize, ULONG flags);
        BOOL  allocSections(ULONG reservedMem);
        BOOL  allocFixedMem(ULONG reservedMem);
     Section *findSection(ULONG type);
     Section *findSectionByAddr(ULONG addr);
     Section *findSectionByOS2Addr(ULONG addr);
     Section *findPreviousSectionByOS2Addr(ULONG addr);

        BOOL  setMemFlags();
        BOOL  setFixups(PIMAGE_BASE_RELOCATION prel);
        BOOL  setFixups(ULONG virtAddress, ULONG size);
        void  AddOff32Fixup(ULONG fixupaddr);
        void  AddOff16Fixup(ULONG fixupaddr, BOOL fHighFixup);

        BOOL  processImports();
        BOOL  processExports();

        BOOL  loadForwarder(ULONG virtaddr, char *apiname, ULONG ordinal);

Win32DllBase *loadDll(char *pszCurModule);

        ULONG                 nrsections, imageSize, imageVirtBase, imageVirtEnd;
        //OS/2 virtual base address
        ULONG                 realBaseAddress, originalBaseAddress;
        Section              *section;
        ULONG                 Characteristics;

        //offset in executable image where real PE file starts (default 0)
        ULONG                 ulPEOffset;

        //internal flags (see FLAGS_PELDR_*)
	DWORD                 dwFlags;

	HFILE                 hFile;

        PIMAGE_BASE_RELOCATION pFixups;
        DWORD                  dwFixupSize;

        Win32MemMap          *memmap;
        LPVOID                peview;
private:
};

#endif //__WINIMAGEPELDR_H__

