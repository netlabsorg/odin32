/**
 * File:   lx.h
 * Desc:   Decleration of the classs LXHeaderSuper, LXHeader and LXFile.
 * Author: Sander van Leeuwen, reorganized by knut st. osmundsen
 */


#ifndef __LX_H__
    #define __LX_H__
    /* * * * * * */
    /* includes  */
    /* * * * * * */
    /* types used in exe386 which is not usual elsewhere in OS/2 */
    #ifndef __EXE386__
        #define DWORD    ULONG
        #define LPVOID   VOID *
        #define WORD     USHORT
        #define UNALIGNED
        #define PDWORD   DWORD *
        #define PWORD    WORD *
        #define NTAPI
        #define BOOLEAN  ULONG
        #define WCHAR    USHORT
        #define HANDLE   ULONG
        #include <exe386.h>
    #endif


    /* * * * * * * * * * * * */
    /* some "usefull" MACROS */
    /* * * * * * * * * * * * */
    #define ZERO(a)      (memset(&a, 0, sizeof(a)))
    #define ZEROstac(a)  (memset(__StackToFlat(&a), 0, sizeof(a)))

    #ifndef max
    #define max(a,b)  (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef min
    #define min(a,b)  (((a) < (b)) ? (a) : (b))
    #endif

    #ifndef minnotnull
    #define minnotnull(a , b) ((a)==0 ? (b) : ((b)==0 ? (a) : min(a,b)))
    #endif



    /* * * * * */
    /* Defines */
    /* * * * * */
    /* Some constants */
    #define LXHEADER_OFFSET           0x80
    #define PAGE_SIZE                 4096
    #define PAGE_SHIFT                  12

    /* System type: gui or text - SetModuleType */
    #define SYSTEM_CHARACTER             0
    #define SYSTEM_GUI                   1

    /* Sectiontypes  */
    #define SECTION_CODE            0x0001
    #define SECTION_INITDATA        0x0002
    #define SECTION_UNINITDATA      0x0004
    #define SECTION_READONLYDATA    0x0008
    #define SECTION_IMPORT          0x0010
    #define SECTION_STACK           0x0020
    #define SECTION_RESOURCE_ORG    0x0040
    #define SECTION_RESOURCE        0x0080
    #define SECTION_COMBINEDDATA    0x0100
    #define SECTION_TIBFIX          0x0200

    /* limits for arrays */
    #define MAX_SECTION                 64  /*PLF Mon  98-02-09 23:47:16*/
    #define MAX_IMPORT                4096  //this should do for most bloated apps
    #define MAX_RESOURCE              4096

    /* Ordinals for exported calls from kernel32 which are used in TIBEXE/DLL code */
    #define ORD_REGISTERRESOURCEINFO  1203
    #define ORD_KERNEL32DLLEXITLIST   1208
    #define ORD_REGISTERDLL           1209


    /* * * * * * * * * * * */
    /* TIB Section defines */
    /* * * * * * * * * * * */
    #define EXE_OFF_ENTRYFIX    (1+19+2+10)
    #define EXE_OFF_TIBCALL     (EXE_OFF_ENTRYFIX-1)
    #define EXE_OFF_NAMETABLE   (3+2+10)
    //SvL: 18-7-'98: offsets for internal pe2lx version & version resource id
    #define EXE_OFF_PE2LXVER    (5)
    #define EXE_OFF_VERRESID    (10)
    #define EXE_TIBSIZE         sizeof(achEXETIBFix)
    #define EXE_OFF_KERIMP      (EXE_OFF_NAMETABLE+10)

    #define DLL_OFF_ENTRYFIX    (15+28+10)
    //#define DLL_OFF_ENTRYFIX   (16+28+10)
    #define DLL_OFF_TIBCALL     (DLL_OFF_ENTRYFIX-1)
    //SvL: 18-7-'98: offsets for internal pe2lx version & version resource id
    #define DLL_OFF_PE2LXVER    (12)
    #define DLL_OFF_VERRESID    (17)
    #define DLL_OFF_NAMETABLE   (12+10)
    //#define DLL_OFF_NAMETABLE   12+1+10
    #define DLL_TIBSIZE         sizeof(achDLLTIBFix)
    #define DLL_OFF_KERIMP      (DLL_OFF_NAMETABLE+10)
    #define DLL_OFF_KERIMP2     (sizeof(achDLLTIBFix)-5)

    #define SIZE_TIBCALL         5

    #define EXTRA_FIXUPS         2  //extra kernel32 imports

    /* Size of code arrays - icc didn't like to have extern arrays without a fixed */
    #define DOSHEADER_SIZE 128
    #define EXETIBFIX_SIZE  37
    #define DLLTIBFIX_SIZE  68
    extern unsigned char achDosHeader[DOSHEADER_SIZE];
    extern unsigned char achEXETIBFix[EXETIBFIX_SIZE];
    extern unsigned char achDLLTIBFix[DLLTIBFIX_SIZE];

    /* header file need for RING0 */
    #ifdef RING0
    #include <OS2Krnl.h>
    #endif


/* KSO: Rawdata for use in Section is different between LXFile and LXHdr */
#ifndef RING0
    typedef char *RAWDATA;
#else
    typedef unsigned long int RAWDATA;
#endif


/**
 * Section struct used for storing PE section.
 */
typedef struct
{
    RAWDATA rawdata;
#ifdef RING0
    ULONG  lxoffset;  //file offset in the generated LX file    (w/ TIBFix)
#endif
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



/**
 * Used with section aligning.
 */
typedef struct
{
    int    size;
    int    flags;
} DataPage;



#pragma pack(1)
//also defined in USER32\NAMEID.H!
#define RESID_CONVERTEDNAMES    63*1024
typedef struct
{
    int    id;
    char   name[1];
} NameId;


/**
 * Name import fixup - intermediate
 */
typedef struct
{
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
    unsigned short      proc;           /* Procedure name offset */
    unsigned short      srcpage;
} namefixup;


/**
 * Name import fixup - really used in LX-files
 */
typedef struct
{
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
    unsigned short      proc;           /* Procedure name offset */
} realnamefixup;


/**
 * Ordinal Import Fixup - intermediate
 */
typedef struct
{
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
    unsigned short      ord;            /* Ordinal */
    unsigned short      srcpage;
} ordfixup;


/**
 * Ordinal Import Fixup - really used in LX-files
 */
typedef struct
{
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      r32_objmod;     /* Target object number or Module ordinal */
    unsigned short      ord;            /* Ordinal */
} realordfixup;


/**
 * Internal 32-bit Offset Fixup - intermediate
 */
typedef struct
{
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      targetobj;
    unsigned long       targetaddr;
    unsigned short      srcobj;
    unsigned short      srcpage;
} intfixup;


/**
 * Internal 32-bit Offset Fixup - really used in LX-files
 */
typedef struct
{
    unsigned char       nr_stype;       /* Source type - field shared with new_rlc */
    unsigned char       nr_flags;       /* Flag byte - field shared with new_rlc */
    short               r32_soff;       /* Source offset */
    unsigned short      targetobj;
    unsigned long       targetaddr;
} realintfixup;


/**
 * Export Bundle - format of exports
 */
typedef struct
{
    unsigned char       b32_cnt;        /* Number of entries in this bundle */
    unsigned char       b32_type;       /* Bundle type */
    unsigned short      b32_obj;        /* Object number */

    unsigned char       e32_flags;      /* Entry point flags */
    unsigned long       e32_offset;     /* 16-bit/32-bit offset entry */
} exportbundle;


/**
 * Forward Bundle - used to forward exports
 */
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




/**
 * @prop     Toplevel class for LX files.
 * @purpose  Generate LX-files.
 * @desc     ....
 * @author   Sander van Leeuwen, modified by knut st. osmundsen
 */
class LXHeaderSuper
{

 public:
    /** @cat Constructor/Destructor */
    LXHeaderSuper();
    ~LXHeaderSuper();

    /** @cat Public Methods */
    /** dummy text */
    BOOL SetEntryAddress(int vaEntryPoint);
    void SetEntryPoint(int address) { EntryAddress = address; }; //absolute virtual address
    void SetExeType(BOOL IsEXE);
    void SetNoFixups();
    void SetStackSize(int size);
    BOOL SetResourceSize(int size);
    BOOL SetNrResources(int cnt);
    void StoreSection(RAWDATA rawdata, int rawsize, int virtualsize, int address, int type);
    BOOL AlignSections(PIMAGE_OPTIONAL_HEADER pOH);
    BOOL StoreImportByName(char *modname, int idxmod, char *name, int offset);
    BOOL StoreImportByOrd(int idxmod, int ordinal, int offset);
    BOOL StoreImportModules(char *modules, int nrmod);
    void SetNoNameImports();

    void SetModuleType(int type);
    void AddOff32Fixup(int address);
    BOOL SetNrOff32Fixups(int nr);
    BOOL AddNameExport(int address, char *name, int ordinal);
    BOOL AddOrdExport(int address, int ordinal);
    BOOL SetNrExtFixups(int nr);
    BOOL AddForwarder(char *name, int ordinal, char *forward);

    int  GetPageNoByVA(int vaAddress);
    void SetModuleName(char *filename);

    BOOL SetNrSections(int cSections);
    BOOL CreateTIBSection(PIMAGE_OPTIONAL_HEADER pOH);


    /** @cat Resource Procedures */
    BOOL StoreResource(int id, int type, int size, char *resourcedata);
    BOOL StoreWin32Resource(int id, int type, int size, char *resourcedata);
    int  ConvertNametoId(char *nameid);
    BOOL SaveConvertedNames();
    BOOL StoreResourceId(int id);
    int  GetUniqueId();
    //SvL: 18-7-'98: Set version resource id
    void SetVersionResourceId(int id) { VersionResourceId = id; };

    void SetTLSAddress(ULONG dwTlsAddress)       { tlsAddress = dwTlsAddress; };
    void SetTLSIndexAddress(ULONG dwTlsIndexAddr)    { tlsIndexAddr = dwTlsIndexAddr; };
    void SetTLSInitSize(ULONG dwTlsSize)     { tlsInitSize = dwTlsSize; };
    void SetTLSTotalSize(ULONG dwTlsSize)        { tlsTotalSize = dwTlsSize; };
    void SetTLSCallBackAddr(ULONG dwTlsCallBackAddr) { tlsCallBackAddr = dwTlsCallBackAddr; };

    void AddExtraFixups();

protected:
    /** @cat Protected Procedures */
    BOOL IsSystemModule(char *mod, int size);
    int  FindName(char *table, int index);
    char *StripPath(char *path);
    char *StripExtension(char *fname);
    void UpCase(char *mixedcase);

    BOOL StoreAndSortImport(namefixup *newfrec);

    int  GetNrPages();
    int  GetNrObjects();
    int  GetNrObjects2();
    int  GetSection(int type);
    int  GetOriginalSectionIdxByVA(int vaAddress);
    int  GetSectionIdxByVA(int vaAddress);
    int  GetObjectNoByVA(int vaAddress);
    int  GetTIBSectionIdx();
    int  GetTIBObjectNo();

    /** UNUSED!*/
    int  GetSectionLEAddress(int address);

    /** @cat Protected Data */
    BOOL           IsEXE, fConsole;
    ULONG          EntryAddress;
    Section       *PESection;   //KSO: PESection is now dynamic
    int            cPESections; //KSO: PESection is now dynamic
    Section        ResSection;
    DataPage      *datapage;

    char          *impmodules;
    char          *impnames, *impnameoff;
    int            impmodulesize, impnamesize;
    int            fFlags;
    int            StackSize;
    int            nrsections;

    namefixup     *impfixuprec;
    int            nrimpfixups;
    int            nrofimpfixups; //available in impfixuprec

    intfixup      *intfixuprec;
    int            nrintfixups;
    int            nrofintfixups; //available in intfixuprec

    rsrc32        *os2resource;
    int            nrresources;
    int            curresource;
    NameId        *cvtname, *curcvtname;
    int            nrcvtnames;
    int           *resids;
    int            nrids;
    int            nrofids;
    int            cvtnametableid;
    int            orgrestableid;
    ULONG         *orgrestable;
    int            nrorgres;

    char           *exports, *curexport;
    int            exportsize;
    int            nrexports;
    exportbundle   *expbundle, *curexpb;

    char           modulename[132];

    struct e32_exe LXHdr;

    static int     uniqueId;

    int            kernel32Object;

    //SvL: 18-7-'98: Version resource id
    ULONG          VersionResourceId;

    ULONG     tlsAddress;         //address of TLS data
    ULONG     tlsIndexAddr;       //address of DWORD that receives the TLS index
    ULONG     tlsInitSize;        //size of initialized TLS memory block
    ULONG     tlsTotalSize;       //size of TLS memory block
    ULONG     tlsCallBackAddr;    //ptr to TLS callback array


    unsigned char  *szTIBFix;
    int            TIBSize;
    int            TIBOffEntry;
    int            TIBOffCall;
    int            TIBOffName;
    int            TIBOffKerImport;
    //SvL: 18-7-'98: Internal pe2lx version and version resource id offsets
    int            TIBOffPe2lxVer;
    int            TIBOffVerResId;


    /* prev statics */
    int lastord;
    BOOL fFirstIcon;
};


#ifndef RING0


class LXHeader : public LXHeaderSuper
{
    public:
    LXHeader();
    ~LXHeader();
    BOOL SaveNewExeFile(char *filename);
   void SetFileBase(char *p) { pBaseFile = p;}


   protected:
      char *pBaseFile;
};

extern LXHeader OS2Exe;


#else



/* signature and file header struct */
typedef struct
{
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
} IMAGE_SIG_FILE_HEADER, *PIMAGE_SIG_FILE_HEADER;




/**
 * @prop     Convert PE files to LX files.
 * @purpose  Convert PE files to LX files for use with win32k in RING0.
 * @desc     Initially designed to be used with win32k, but may be used in
 *           other programs. The init function analyses a given PE-file, builds
 *           a (virtual) LX-file from it and waits from read request of data from
 *           the virtual LX file.
 * @author   knut st. osmundsen
 */
class LXFile : public LXHeaderSuper
{
    public:
        LXFile();
        ~LXFile();

        /** @cat Public Methods */
        BOOL   init(SFN hFile, PIMAGE_SIG_FILE_HEADER pHdr, ULONG PEOffset);
        ULONG  read(ULONG ulOffset, PVOID pBuffer, ULONG ulBytesToRead, PULONG ulBytesRead, ULONG ulFlag);

        /** @cat Public Helper Functions */
        int    queryMemoryUsage();
        BOOL   queryIsModuleName(const char *szFilename);
        ULONG  queryFileSize();

        /** @cat Static Methods */
        static BOOL isPEFile(PIMAGE_SIG_FILE_HEADER pHdr);
        static void buildQAppTypeHeader(struct e32_exe *pHdr);

        /** @cat New/Delete Operators */
        #ifndef __DEBUG_ALLOC__
            void *operator new(/*size_t*/unsigned int a);
            void operator delete(void *p);
        #else
            void *operator new(size_t a, const char *file, unsigned int line);
            void operator delete(void *p, const char *file, unsigned int line);
        #endif


    protected:
        /** @cat Process Methods */
        BOOL ProcessFixups   (PIMAGE_SECTION_HEADER pSectionTable);
        BOOL ProcessExports  (PIMAGE_SECTION_HEADER pSectionTable);
        BOOL ProcessImports  (PIMAGE_SECTION_HEADER pSectionTable);
        BOOL ProcessResources(PIMAGE_SECTION_HEADER pSectionTable);
        BOOL PrepareLX();

        /** @cat Helper Functions */
        BOOL  PEIsImportDataSection(PIMAGE_SECTION_HEADER psh);
        DWORD PEImageDirectoryOffset (PIMAGE_SECTION_HEADER psh, DWORD dwIMAGE_DIRECTORY);
        PIMAGE_SECTION_HEADER PEGetSectionHdrByImageDir(PIMAGE_SECTION_HEADER psh, DWORD dwIMAGE_DIRECTORY);
        PIMAGE_SECTION_HEADER PEGetSectionHdrByName(IMAGE_SECTION_HEADER *psh, char *szSection);
        PIMAGE_SECTION_HEADER PEGetSectionHdrByRVA(IMAGE_SECTION_HEADER *psh, ULONG rva);
        PVOID PEReadSection(PIMAGE_SECTION_HEADER pSH);
        int  PEGetNumberOfResources(PIMAGE_RESOURCE_DIRECTORY prdRoot);
        BOOL StoreIdResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType, int level, PIMAGE_RESOURCE_DIRECTORY prdRoot, int VirtualAddress, int type, int id);
        BOOL ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType, int level, PIMAGE_RESOURCE_DIRECTORY prdRoot, int VirtualAddress, int type, int id);
        int  AddOff32Fixup(int address, char *pPage);
        int  AddOff32FinishCrossPageFixup(void);
        int  ReadPageByAddress(ULONG address, void *pPage);
        void AdjustTIB();


    private:
        /** @cat Privat Datamembers */
        int inited;
        IMAGE_NT_HEADERS PEHdrs;
        ULONG   ulPEOffset;                    /* file offset of PE signature */
        SFN     hFile;                         /* file handle */
        ULONG nSections;

        /* fixup stuff */
        intfixup  crossedpage;
        BOOL      fCrossed;
        int       crossedpageaddress;

        /* LX converted stuff */
        struct o32_obj *pObjectTable;
        struct o32_map *pObjectPageTable;
        //info: struct rsrc32 *pResourceTable = os2resource
        //info: if (Exe){
        //         pResidentNameTable = modulename; // (NB. two blanks)
        //         pEntryTable = '\0';
        //      }else{
        //         pResidentNameTable = exports;
        //         pEntryTable = expbundle;
        //      }
        int *pFixupTable;
        realintfixup  *pFixupRecordTable;  //size = LXHdr.e32_fixupsize - sizeof(FixupTable) //((nrpages+1)*sizeof(int))
        //info: pImportModuleNameTable = impmodules;  //size impmodulesize
        //info: pImportProcedureNameTable = impnames; //size impnamesize
        //info: rawdata begins here.
        ULONG TIBEntry;
        //ULONG ul
        ULONG ulResourceOffset;
        //info: resources after here.
};

#endif

#endif
