/* $Id: winimagepeldr.h,v 1.1 1999-09-15 23:29:37 sandervl Exp $ */

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

#include <winimagebase.h>

//SvL: Amount of memory the peldr dll reserves for win32 exes without fixups
//(most of them need to be loaded at 4 MB; except MS Office apps of course)
#define PELDR_RESERVEDMEMSIZE	16*1024*1024

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

#define MAX_SECTION             64  /*PLF Mon  98-02-09 23:47:16*/

typedef struct {
  char  *rawdata;
  ULONG  rawsize;
  ULONG  virtaddr;
  ULONG  realvirtaddr;  //as allocated in OS/2
  ULONG  virtualsize;
  ULONG  type;
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

class Win32PeLdrImage : public virtual Win32ImageBase
{
public:
         Win32PeLdrImage(char *szFileName);
virtual ~Win32PeLdrImage();

        //reservedMem is address of memory reserved in peldr.dll (allocated before
        //any dlls are loaded, so that exes without fixups can be loaded at a low
        //address)
virtual BOOL  init(ULONG reservedMem);

virtual HRSRC findResourceA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = LANG_GETFIRST);
virtual ULONG getResourceSizeA(LPCSTR lpszName, LPSTR lpszType, ULONG lang = LANG_GETFIRST);

virtual ULONG getVersionSize();
virtual BOOL  getVersionStruct(char *verstruct, ULONG bufLength);

protected:
        void StoreImportByOrd(Win32DllBase *WinDll, ULONG ordinal, ULONG impaddr);
        void StoreImportByName(Win32DllBase *WinDll, char *impname, ULONG impaddr);

        void  addSection(ULONG type, char *rawdata, ULONG rawsize, ULONG virtaddress, ULONG virtsize);
        BOOL  allocSections(ULONG reservedMem);
        BOOL  allocFixedMem(ULONG reservedMem);
     Section *findSection(ULONG type);
     Section *findSectionByAddr(ULONG addr);

        BOOL  storeSections(char *win32file);
        BOOL  setMemFlags();
        BOOL  setFixups(PIMAGE_BASE_RELOCATION prel);
        void  AddOff32Fixup(ULONG fixupaddr);
        void  AddOff16Fixup(ULONG fixupaddr, BOOL fHighFixup);

        BOOL  processImports(char *win32file);

        BOOL  processExports(char *win32file);
        void  AddNameExport(ULONG virtaddr, char *apiname, ULONG ordinal);
        void  AddOrdExport(ULONG virtaddr, ULONG ordinal);


        ULONG getPEResourceSize(ULONG id, ULONG type, ULONG lang = LANG_GETFIRST);

        PIMAGE_RESOURCE_DATA_ENTRY getPEResourceEntry(ULONG id, ULONG type, ULONG lang = LANG_GETFIRST);
        PIMAGE_RESOURCE_DATA_ENTRY ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType,
                                                    ULONG *nodeData, int level);
        PIMAGE_RESOURCE_DIRECTORY pResDir;
        Section                  *pResSection;

 	IMAGE_OPTIONAL_HEADER oh;
	IMAGE_FILE_HEADER     fh;

        ULONG                 nrNameExports, nameExportSize;
        ULONG                 nrOrdExports;
        NameExport           *nameexports, *curnameexport;
        OrdExport            *ordexports, *curordexport;

        ULONG                 nrsections, imageSize, imageVirtBase, imageVirtEnd;
        //OS/2 virtual base address
        ULONG                 realBaseAddress;
        Section               section[MAX_SECTION];

private:
};

#include <iostream.h>
#include <fstream.h>
extern ofstream fout;

#endif //__WINIMAGEPELDR_H__

