/* $Id: pe2lx.h,v 1.4 1999-10-14 01:17:55 bird Exp $
 *
 * Pe2Lx class declarations. Ring 0 and Ring 3
 *
 * Copyright (c) 1998-1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * Copyright (c) 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright (c) 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _PE2LX_H_
#define _PE2LX_H_


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * RING3 wrappings for RING0 constructs.
 */
#ifdef RING3
    typedef HFILE SFN;      /* System File Number. (OS2Krnl.h) */
    typedef PVOID PMTE;     /* Pointer to Module Table Entry. (OS2Krnl.h) */
#endif

/*
 * Misc
 */
#define PAGESIZE                    0x1000

/*
 * BufferedRVAReader config
 */
#define BUFFEREDRVAREADER_BUFFERSIZE PAGESIZE   /* reader code assumes this size... */


/*
 * Error definitions (used in addition to them in bseerr.h)
 */
#define ERROR_FAILED_TO_ADD_OBJECT          0x42000000UL
#define ERROR_INITMETHOD_NOT_INITTIME       0x42000001UL
#define ERROR_INTERNAL_PROCESSING_ERROR     0x42000002UL


/*
 * Output macros.
 * Macros:          option   infolevel
 *      printIPE    -W1      Error
 *      printErr    -W1      Error
 *      printWar    -W2      Warning
 *      printInf    -W3      Info
 *      printInfA   -W4      InfoAll
 */
#define printIPE(a) (Pe2Lx::ulInfoLevel >= Pe2Lx::Error ? \
                     Pe2Lx::printf("\nerror(%d:"__FUNCTION__"): !Internal Processing Error!\n\t", __LINE__), \
                     Pe2Lx::printf a,  \
                     Pe2Lx::printf("\n")  \
                     : (void)0,(void)0,(void)0 )
#define printErr(a) (Pe2Lx::ulInfoLevel >= Pe2Lx::Error ? \
                     Pe2Lx::printf("error(%d:"__FUNCTION__"): ", __LINE__), \
                     Pe2Lx::printf a  \
                     : (void)0,(void)0 )
#define printWar(a) (Pe2Lx::ulInfoLevel >= Pe2Lx::Warning ? \
                     Pe2Lx::printf("warning("__FUNCTION__"): "), \
                     Pe2Lx::printf a  \
                     : (void)0,(void)0 )
#define printInf(a) (Pe2Lx::ulInfoLevel >= Pe2Lx::Info ? \
                     Pe2Lx::printf a : (void)0 )
#define printInfA(a)(Pe2Lx::ulInfoLevel >= Pe2Lx::InfoAll ? \
                     Pe2Lx::printf a : (void)0 )



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/**
 * LXObject is use to store objects in the virtual LX file.
 */
typedef struct LXObject
{
    ULONG ulRVA;                /* Object relative virtual address  */
    ULONG cbPhysical;           /* Physical size        */
    ULONG cbVirtual;            /* Virtual size         */
    ULONG flFlags;              /* LX flags!            */
    struct
    {
        ULONG offTIBFix : 30;   /* TIBfix offset (from the object start) */
        ULONG fTIBFixObject : 1;/* Set: The object contains the tibfix. Clear: not tib fix object. */
        ULONG fStackObject : 1; /* Set: The object is the stack object. Clear: not the stack object. */
    } Misc;
    ULONG offPEFile;            /* section offset into real PE file. */
    ULONG offLXFile;            /* object offset into virtual LX file. */
} LXOBJECT, *PLXOBJECT;
typedef const LXOBJECT *PCLXOBJECT;



/**
 * Pe2Lx class. Reads a PE executable image and creates a virtual LX file which
 * can be read from and (RING3) dumped to file.
 *
 * @author      knut st. osmundsen
 * @approval    knut st. osmundsen
 */
class Pe2Lx
{
public:
    /** @cat Constructor/Destructor */
    Pe2Lx(SFN hFile);
    ~Pe2Lx();

    /** @cat Public Main methods */
    ULONG  init(PCSZ pszFilename);
    ULONG  read(ULONG offLXFile, PVOID pvBuffer, ULONG cbToRead, ULONG flFlags, PMTE pMTE);
    #ifndef RING0
    ULONG  writeLxFile(PCSZ pszLXFilename);
    #endif

    /** @cat public Helper methods */
    BOOL   queryIsModuleName(PCSZ pszFilename);
    ULONG  querySizeOfLxFile();
    VOID   dumpVirtualLxFile();


private:
    /** @cat conversion methods */
    ULONG       makeObjectTable();
    ULONG       makeObjectPageTable();
    ULONG       makeFixups();
    ULONG       makeExports();

    /** @cat conversion helper(s) */
    ULONG       loadNtHeaders();
    VOID        releaseNtHeaders();

    /** @cat init() helper methods - may only be called at init time! */
    ULONG       addObject(ULONG ulRVA, ULONG cbPhysical, ULONG cbVirtual, ULONG flFlags, ULONG offPEFile);
    ULONG       addTIBFixObject();
    ULONG       addStackObject(ULONG cbStack);

    /** @cat PE helper methods */

    /** @cat Fixups Helpers - calls allowed from makeFixups only! */
    ULONG       initFixups();
    ULONG       addPageFixupEntry(BOOL fLast = FALSE);
    ULONG       add32OffsetFixup(WORD offSource, ULONG ulTarget);
    ULONG       add32OrdImportFixup(WORD offSource, ULONG ulModuleOrdinal, ULONG ulFunctionOrdinal);
    ULONG       add32NameImportFixup(WORD offSource, ULONG ulModuleOrdinal, PCSZ pszFnName);
    ULONG       addModule(PCSZ pszModuleName, PULONG pulModuleOrdinal);
    ULONG       addImportFunctionName(PCSZ pszFnName, PULONG poffFnName);
    VOID        finalizeFixups();
    VOID        finalizeImportNames();

    /** @cat Entry table / Resident nametable helpers */
    ULONG       initEntry();
    ULONG       addResName(ULONG ulOrdinal, PCSZ pszName, ULONG cchName);
    ULONG       addEntry(ULONG ulOrdinal, ULONG ulRVA);
    ULONG       addForwarderEntry(ULONG ulOrdinal, PCSZ pszDllName, PCSZ pszFnNameOrOrd);
    ULONG       addLastEntry();
    VOID        finalizeExports();

    /** @cat Misc helpers */
    ULONG       getCountOfPages();
    ULONG       queryObjectAndOffset(ULONG ulRVA, PULONG pulObject, PULONG poffObject);

    /** @cat static helpers */
    static PCSZ queryOdin32ModuleName(PCSZ pszWin32ModuleName);

    /** @cat static dump methods */
    static VOID dumpNtHeaders(PIMAGE_NT_HEADERS pNtHdrs);
    static VOID dumpSectionHeader(PIMAGE_SECTION_HEADER pSection);

    /** @cat static print method */
public:
    static VOID printf(PCSZ pszFormat, ...);

private:
    /** @cat private data members - allways present.  */
    #ifdef DEBUG
    BOOL        fInitTime;              /* init time indicator (debug) */
    #endif
    SFN         hFile;                  /* filehandle */
    PSZ         pszFilename;            /* fullpath */
    PSZ         pszModuleName;          /* filename without extention. */
    BOOL        fAllInOneObject;        /* The All-in-object fix will be or is applied. */
    PLXOBJECT   paObjects;              /* Pointer to object array. */
    USHORT      cObjects;               /* Count of elements in the object array. */
    USHORT      cObjectsAllocated;      /* Size of the object array.  */

    /**
     * @cat LX structures
     */
    struct e32_exe     LXHdr;           /* Lxheader */

    struct o32_obj    *paObjTab;        /* Pointer to object table - if null check cObjects > 0 and generate it using makeObjectTable */
    struct o32_map    *paObjPageTab;    /* Pointer to object page table - if null check cObjects > 0 and generate it using makeObjectPageTable */

    PCHAR              pachResNameTable; /* Pointer to resident name table. */
    ULONG              offCurResName;    /* Offset of the next entry in the resident name table. */
    ULONG              cchRNTAllocated;  /* Count of char allocated for the resident name table. */

    struct b32_bundle *pEntryBundles;       /* Pointer to entry bundles. (exports) */
    ULONG              offCurEntryBundle;   /* Offset of the next bundle. */
    ULONG              offLastEntryBundle;  /* Offset of the last entry bundle. */
    ULONG              ulLastOrdinal;       /* Ordinal number of last entry which was added. */
    ULONG              cbEBAllocated;       /* Count of bytes allocate for entry bundles. */
    BOOL               fForwarders;         /* Set if forwarders are present. */

    PULONG             paulFixupPageTable;  /* Pointer to fixup pagetable. If null generate it using makeFixups. */
    ULONG              cFixupPTEntries;     /* Number of entries in the fixup page table. */
    ULONG              cFPTEAllocated;      /* Number of page table entries allocated. */

    PVOID              pFixupRecords;       /* Pointer to fixup records. If null generate it using makeFixups. */
    ULONG              offCurFixupRec;      /* Offset of next fixup. */
    ULONG              cbFRAllocated;       /* Count of bytes allocated for Fixup records. */

    PVOID              pvCrossPageFixup;    /* Pointer to cross page fixup. */
    ULONG              cbCrossPageFixup;    /* Fixup size in bytes. */

    PCHAR              pachImpModuleNames;  /* Pointer to list of module names. */
    ULONG              offCurImpModuleName; /* Offset of next modulename. */
    ULONG              cchIMNAllocated;     /* Count of chars allocated for pachImpModuleNames. */

    PCHAR              pachImpFunctionNames;  /* Pointer to list of function names. */
    ULONG              offCurImpFunctionName; /* Offset of next functionname. */
    ULONG              cchIFNAllocated;       /* Count of chars allocated for pachImpFunctionNames. */

    /**
     * @cat PE structues
     */
    ULONG                   offNtHeaders;   /* Fileoffset of the PE\0\0 signature. */
    PIMAGE_NT_HEADERS       pNtHdrs;        /* Pointer to NT-Headers. If null load it using loadNtHeaders. */
    ULONG                   ulImageBase;    /* Image base address. */

    /**
     * @cat static data.
     */
    static struct LieListEntry              /* Dll Name lie table. */
    {
        PCSZ pszWin32Name;                  /* Win32 dll name. */
        PCSZ pszOdin32Name;                 /* Odin32 dll name. */
    } paLieList[];

    static struct PeCharacteristicsToLxFlags/* section characteristics to object flags */
    {
        unsigned int Characteristics;       /* set of section characteristics */
        ULONG flFlags;                      /* equivalent object flags */
    } paSecChars2Flags[];

public:
    static ULONG ulInfoLevel;                    /* Current output message detail level. */
    enum {Quiet, Error, Warning, Info, InfoAll}; /* Output message detail levels. */
};


/**
 * BufferedRVARead - read at RVA with buffering.
 * @author      knut st. osmundsen
 * @approval    knut st. osmundsen
 */
class BufferedRVARead
{
public:
    BufferedRVARead(SFN hFile, ULONG cObjects, PCLXOBJECT paObjects);
#if 0
    ULONG readAtRVA(ULONG ulRVA, PVOID pvBuffer, ULONG cbBuffer)
#else
    /**
     * Reads a chunk of data at the spcified RVA.
     * @returns   NO_ERROR on success.
     *            ERROR_INVALID_PARAMETER
     *            <Whatever rc ReadAt returns>
     * @param     ulRVA     RVA to read from. Within the filesize.
     * @param     pvBuffer  Pointer to output buffer. pvBuffer > 64KB
     * @param     cbBuffer  Number of bytes to read. 0 < cbBuffer > 256MB
     * @status    completely
     * @author    knut st. osmundsen
     */
    inline ULONG readAtRVA(ULONG ulRVA, PVOID pvBuffer, ULONG cbBuffer)
    {
        /*
         * five cases:
         *  1) entire area is within the buffer.
         *  2) start of area is within the buffer.
         *  3) end of area is within the buffer.
         *  4) the area is larger than the buffer, covering it.
         *  5) the area is outside the buffer.
         *
         * these are optimal: 1, 2, and 5.
         * The request is allways process from start to end. This will make case 3 and 4 less effecient.
         */
        #ifdef DEBUG
        if (ulRVA == ~0UL || (ULONG)pvBuffer < 0x10000UL || cbBuffer == 0UL || cbBuffer >= 0x10000000UL)
            return ERROR_INVALID_PARAMETER;
        #endif

        do
        {
            if (ulRVA >= this->ulRVA && ulRVA < this->ulRVA + sizeof(achBuffer))
            {   /* in buffer */
                register ULONG cbRead = sizeof(achBuffer) - (ulRVA - this->ulRVA);
                cbRead = min(cbRead, cbBuffer);
                memcpy(pvBuffer, &achBuffer[ulRVA - this->ulRVA], (size_t)cbRead);
                if (cbBuffer == cbRead)
                    return NO_ERROR;
                cbBuffer -= cbRead;
                pvBuffer = (PVOID)((ULONG)pvBuffer + cbRead);
                ulRVA += cbRead;
            }
            else
            {   /* not in buffer, then read it into the buffer! */
                APIRET rc = readToBuffer(ulRVA);
                if (rc != NO_ERROR)
                    return rc;
            }
        } while (cbBuffer != 0UL);

        return NO_ERROR;
    }
#endif

    ULONG dupString(ULONG ulRVA, PSZ *ppsz);
    BufferedRVARead & operator =(BufferedRVARead &SrcObj);

private:
    ULONG readToBuffer(ULONG ulRVA);

private:
    SFN   hFile;                /* Filehandle. */
    ULONG cObjects;             /* Count of objects */
    PCLXOBJECT paObjects;       /* Pointer to a readonly array of objects. */
    ULONG ulRVA;                /* RVA for the buffer start */
    CHAR  achBuffer[BUFFEREDRVAREADER_BUFFERSIZE];  /* Buffer. NOTE! Code assumes that it is a page. */
};

#endif
