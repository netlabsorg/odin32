/* $Id: winimage.h,v 1.4 1999-08-09 22:46:46 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 PE Image class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __WINIMAGE_H__
#define __WINIMAGE_H__

#include <peexe.h>

#define MAX_RES 17
extern char *ResTypes[MAX_RES];


#ifndef __PE2LX__

#ifdef DEBUG
#define MAGIC_WINIMAGE          0x11223344
#endif

#pragma pack(1)
typedef struct {
  int    id;
  char   name[1];
} NameId;
#pragma pack()

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

#define PAGE_SIZE               4096

#define MAX_SECTION             64  /*PLF Mon  98-02-09 23:47:16*/

#define LANG_GETFIRST           0x80000000

#define NO_NAMETABLE            0x77777777
#define NO_LOOKUPTABLE          0x888888
#define GET_CONSOLE(a)          (a >> 24)
#define SET_CONSOLE(a)          (a << 24)

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

class Win32Dll;
class Win32Resource;

class Win32Image
{
#ifdef DEBUG
protected:
        DWORD magic;
public:
        void checkObject()
        {
        #ifdef DEBUG
                if(magic != MAGIC_WINIMAGE) {
                        eprintf(("Corrupt this pointer %X %X!!", this, magic));
                        DebugInt3();
                }
        #endif
        };
#endif

public:
        // Constructors and destructors
        Win32Image(HINSTANCE hinstance, int NameTableId, int Win32TableId);
        Win32Image(char *szFileName);
virtual ~Win32Image();

        //called to reset object to native OS/2 or converted win32 dll
        void OS2ImageInit(HINSTANCE hinstance, int NameTableId, int Win32TableId);

virtual BOOL  init();

        ULONG getError()              { return errorState; };
        HINSTANCE getInstanceHandle() { return hinstance; };

virtual void setFullPath(char *name);
        char *getFullPath()           { return fullpath; };

        HRSRC   findResourceA(LPCSTR lpszName, LPSTR lpszType);
        HRSRC   findResourceW(LPWSTR lpszName, LPWSTR lpszType);

        int   getWin32ResourceId(int id);
        int   convertNameId(char *lpszName);

static  BOOL  isPEImage(char *szFileName);

        void  setVersionId(int id) { VersionId = id;   };
        int   getVersionId()       { return VersionId; };

	void  setEntryPoint(ULONG startAddress) { entryPoint = startAddress; };

        void  setTLSAddress(LPVOID dwTlsAddress)     	{ tlsAddress = dwTlsAddress; };
        void  setTLSIndexAddr(LPDWORD dwTlsIndexAddr)	{ tlsIndexAddr = dwTlsIndexAddr; };
        void  setTLSInitSize(ULONG dwTlsSize)		{ tlsInitSize = dwTlsSize; };
        void  setTLSTotalSize(ULONG dwTlsSize)		{ tlsTotalSize = dwTlsSize; };
        void  setTLSCallBackAddr(PIMAGE_TLS_CALLBACK *dwTlsCallBackAddr)	
	{
	   tlsCallBackAddr = dwTlsCallBackAddr;
	};

	void  tlsAttachThread();	//setup TLS structures for new thread
	void  tlsDetachThread();	//destroy TLS structures

protected:
	void tlsAlloc();		//Allocate TLS index for this module
	void tlsDelete();		//Destroy TLS index for this module

        void StoreImportByOrd(Win32Dll *WinDll, ULONG ordinal, ULONG impaddr);
        void StoreImportByName(Win32Dll *WinDll, char *impname, ULONG impaddr);

        void  addSection(ULONG type, char *rawdata, ULONG rawsize, ULONG virtaddress, ULONG virtsize);
        BOOL  allocSections();
        BOOL  allocFixedMem();

        BOOL  storeSections();
        BOOL  setMemFlags();
        BOOL  setFixups(PIMAGE_BASE_RELOCATION prel);
        void  AddOff32Fixup(ULONG fixupaddr);
        void  AddOff16Fixup(ULONG fixupaddr, BOOL fHighFixup);

        BOOL  processImports(char *win32file);

        BOOL  processExports(char *win32file);
        void  AddNameExport(ULONG virtaddr, char *apiname, ULONG ordinal);
        void  AddOrdExport(ULONG virtaddr, ULONG ordinal);

        Win32Resource *getPEResource(ULONG id, ULONG type, ULONG lang = LANG_GETFIRST);
        PIMAGE_RESOURCE_DATA_ENTRY ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType,
                                                    ULONG *nodeData);
        PIMAGE_RESOURCE_DIRECTORY pResDir;
        Section                  *pResSection;
        Win32Resource            *winres;

        IMAGE_OPTIONAL_HEADER oh;
        IMAGE_FILE_HEADER     fh;

        ULONG                 errorState, entryPoint;
        ULONG                 nrNameExports, nameExportSize;
        ULONG                 nrOrdExports;
        NameExport           *nameexports, *curnameexport;
        OrdExport            *ordexports, *curordexport;

        ULONG                 nrsections, imageSize, imageVirtBase, imageVirtEnd;
        //OS/2 virtual base address
        ULONG                 baseAddress, realBaseAddress;
        Section               section[MAX_SECTION];

        char                 *szFileName, *fullpath;

        HINSTANCE             hinstance;

        int                   NameTableId;
        int                   Win32TableId;
        int                   VersionId;

        ULONG                *Win32Table;
        NameId               *NameTable;

        BOOL                  fNativePEImage;

  	LPVOID 			tlsAddress;		//address of TLS data
  	LPDWORD			tlsIndexAddr;		//address of DWORD that receives the TLS index
  	ULONG 			tlsInitSize;		//size of initialized TLS memory block
  	ULONG 			tlsTotalSize;		//size of TLS memory block
  	PIMAGE_TLS_CALLBACK    *tlsCallBackAddr;	//ptr to TLS callback array
	ULONG                   tlsIndex;		//module TLS index

private:

        friend class Win32Resource;
        friend ULONG SYSTEM GetVersionSize(char *modname);
};

#include <iostream.h>
#include <fstream.h>
extern ofstream fout;

#endif //__PE2LX__

#endif
