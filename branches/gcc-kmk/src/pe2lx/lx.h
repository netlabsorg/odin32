/* $Id: LX.H,v 1.4 1999-07-07 08:11:11 sandervl Exp $ */

/*
 * PE2LX LX
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __LX_H__
#define __LX_H__

#include <win32type.h>
#include <exe386.h>

#define LXHEADER_OFFSET     0x80
#define PAGE_SIZE       4096
#define PAGE_SHIFT      12

#define SYSTEM_CHARACTER    0
#define SYSTEM_GUI      1

#define SECTION_CODE        1
#define SECTION_INITDATA        2
#define SECTION_UNINITDATA  4
#define SECTION_READONLYDATA    8
#define SECTION_IMPORT      16
#define SECTION_STACK           32
#define SECTION_RESOURCE_ORG    64
#define SECTION_RESOURCE        128
#define SECTION_COMBINEDDATA    256
#define MAX_SECTION             64  /*PLF Mon  98-02-09 23:47:16*/

#define MAX_IMPORT      8192    //this should do for most bloated apps
#define MAX_RESOURCE        8192

typedef struct {
  char  *rawdata;
  int    rawsize;
  int    virtualsize;
  int    curoff;
  int    type;
  int    address;
  int    endaddress;
  int    nrpages;
  int    nrinvalidpages;
  BOOL   fProcessed;
  BOOL   fInvalid;
} Section;

typedef struct {
  int    size;
  int    flags;
} DataPage;

#pragma pack(1)

//also defined in USER32\NAMEID.H!
#define RESID_CONVERTEDNAMES    63*1024
typedef struct {
  int    id;
  char   name[1];
} NameId;

typedef struct {
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
    unsigned short      proc;           /* Procedure name offset */
    unsigned short      srcpage;
} namefixup;

typedef struct {
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
    unsigned short      proc;           /* Procedure name offset */
} realnamefixup;

typedef struct {
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
    unsigned short      ord;            /* Ordinal */
    unsigned short      srcpage;
} ordfixup;

typedef struct {
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
    unsigned short      ord;            /* Ordinal */
} realordfixup;

typedef struct {
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      targetobj;
    unsigned long       targetaddr;
    unsigned short      srcobj;
    unsigned short      srcpage;
} intfixup;

typedef struct {
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      targetobj;
    unsigned long       targetaddr;
} realintfixup;

typedef struct
{
    unsigned char       b32_cnt;        /* Number of entries in this bundle */
    unsigned char       b32_type;       /* Bundle type */
    unsigned short      b32_obj;        /* Object number */

    unsigned char       e32_flags;      /* Entry point flags */
    unsigned long       e32_offset;     /* 16-bit/32-bit offset entry */
} exportbundle;

typedef struct
{
    unsigned char       b32_cnt;        /* Number of entries in this bundle */
    unsigned char       b32_type;       /* Bundle type */
    unsigned short      b32_obj;        /* Object number */

    unsigned char       e32_flags;      /* Entry point flags */
    unsigned short      modord;         /* Module ordinal number */
    unsigned long       value;          /* Proc name offset or ordinal */
} forwardbundle;

#pragma pack()

class LXHeader {
public:
        LXHeader();
               ~LXHeader();

       void SetEntryAddress(int address);	//relative

       void SetEntryPoint(int address) { EntryAddress = address; }; //absolute virtual address

       void SetExeType(BOOL IsEXE);
       void SetNoFixups();
       void SetStackSize(int size);
       void SetResourceSize(int size);
       void SetNrResources(int cnt);
       void StoreSection(char *rawdata, int rawsize, int virtualsize, int address, int type);
       void AlignSections();
       BOOL StoreImportByName(char *modname, int idxmod, char *name, int offset);
       BOOL StoreImportByOrd(int idxmod, int ordinal, int offset);
       void StoreImportModules(char *modules, int nrmod);
           void SetNoNameImports();
       BOOL SaveNewExeFile(char *filename);
       void SetModuleType(int type);
       void AddOff32Fixup(int address, BOOL fLookatStartupCode = FALSE);
       void SetNrOff32Fixups(int nr);
       void AddNameExport(int address, char *name, int ordinal);
       void AddOrdExport(int address, int ordinal);
       void SetNrExtFixups(int nr);
       void AddForwarder(char *name, int ordinal, char *forward);

       int  GetAddressPage(int address);
       void SetModuleName(char *filename);
       //resource procedures
       void StoreResource(int id, int type, int size, char *resourcedata);
       void StoreWin32Resource(int id, int type, int size, char *resourcedata);
       int  ConvertNametoId(char *nameid);
       void SaveConvertedNames();
       void StoreResourceId(int id);
       int  GetUniqueId();

       //SvL: 18-7-'98: Set version resource id
       void SetVersionResourceId(int id) { VersionResourceId = id; };

       void SetTLSAddress(ULONG dwTlsAddress)     	{ tlsAddress = dwTlsAddress; };
       void SetTLSIndexAddress(ULONG dwTlsIndexAddr)	{ tlsIndexAddr = dwTlsIndexAddr; };
       void SetTLSInitSize(ULONG dwTlsSize)		{ tlsInitSize = dwTlsSize; };
       void SetTLSTotalSize(ULONG dwTlsSize)		{ tlsTotalSize = dwTlsSize; };
       void SetTLSCallBackAddr(ULONG dwTlsCallBackAddr)	{ tlsCallBackAddr = dwTlsCallBackAddr; };

       void AddExtraFixups();

private:
      BOOL  IsSystemModule(char *mod, int size);
       int  FindName(char *table, int index);
      char *StripPath(char *path);
      char *StripExtension(char *fname);

       void StoreAndSortImport(namefixup *newfrec);

       int  GetNrPages();
       int  GetNrObjects();
      void  UpCase(char *mixedcase);
       int  GetSection(int type);
       int  GetSectionByAddress(int address);
       int  GetRealSectionByAddress(int address);
       int  GetSectionLEAddress(int address);

  BOOL      IsEXE, fConsole;
  ULONG     EntryAddress;
  Section   PESection[MAX_SECTION];
  Section   ResSection;
  DataPage *datapage;

  char  *impmodules, *impnames, *impnameoff;
  int    impmodulesize, impnamesize;
  int    fFlags;
  int    StackSize;
  int    nrsections;

  namefixup *impfixuprec;
  int        nrimpfixups;

  intfixup  *intfixuprec;
  int        nrintfixups;

  rsrc32    *os2resource;
  int        nrresources;
  int        curresource;
  NameId    *cvtname, *curcvtname;
  int        nrcvtnames;
  int       *resids;
  int        nrids;
  int        cvtnametableid;
  int        orgrestableid;
  ULONG     *orgrestable;
  int        nrorgres;

  char      *exports, *curexport;
  int        nrexports;
  exportbundle *expbundle, *curexpb;

  char       modulename[128];

  struct e32_exe LXHdr;

  static int uniqueId;

  int            kernel32Object;

  //SvL: 18-7-'98: Version resource id
  ULONG          VersionResourceId;

  ULONG 	tlsAddress;		//address of TLS data
  ULONG 	tlsIndexAddr;		//address of DWORD that receives the TLS index
  ULONG 	tlsInitSize;		//size of initialized TLS memory block
  ULONG 	tlsTotalSize;		//size of TLS memory block
  ULONG 	tlsCallBackAddr;	//ptr to TLS callback array
};

extern LXHeader OS2Exe;

#endif
