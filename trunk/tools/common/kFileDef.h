/*
 * kFileDef - Definition files.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _kFileDef_h_
#define _kFileDef_h_

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define ORD_START_INTERNAL_FUNCTIONS 1200


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/

/**
 * Segment list entry.
 */
typedef struct _DefSegment
{
    char               *psz;
    char               *pszName;
    char               *pszClass;
    char               *pszAttr;
    struct _DefSegment *pNext;
} DEFSEGMENT, *PDEFSEGMENT;



/**
 * Import list entry.
 */
typedef struct _DefImport
{
    unsigned long       ulOrdinal;
    char               *pszName;
    char               *pszDll;
    char               *pszIntName;
    struct _DefImport  *pNext;
} DEFIMPORT, *PDEFIMPORT;


/**
 * Export list entry.
 */
typedef struct _DefExport
{
    unsigned long       ulOrdinal;
    char               *pszName;
    char               *pszIntName;
    KBOOL               fResident;
    unsigned long       cParam;
    struct _DefExport  *pNext;
} DEFEXPORT, *PDEFEXPORT;


/**
 * Definition files.
 * TODO: error handling.
 * @author      knut st. osmundsen
 */
class kFileDef : public kExportI, public kFileFormatBase, public kModuleI
{
    private:
        /**@cat pointers to different sections */
        char *pszType;
        KBOOL fProgram;
        KBOOL fLibrary;
        KBOOL fPhysicalDevice;
        KBOOL fVirtualDevice;
        KBOOL fInitInstance;
        KBOOL fTermInstance;
        KBOOL fInitGlobal;
        KBOOL fTermGlobal;
        char *pszModName;
        char  chAppType;

        char *pszBase;
        char *pszCode;
        char *pszData;
        char *pszDescription;
        char *pszExeType;
        char *pszHeapSize;
        char *pszOld;
        char *pszProtmode;
        char *pszStackSize;
        char *pszStub;

        /**@cat Lists to multistatement sections */
        PDEFSEGMENT pSegments;
        PDEFIMPORT  pImports;
        PDEFEXPORT  pExports;

        /**@cat internal functions */
        void  read(kFile *pFile) throw (kError);
        char *readln(kFile *pFile, char *pszBuffer, int cbBuffer) throw (kError);
        KBOOL isKeyword(const char *psz);
        KBOOL setModuleName(void);

    public:
        /**@cat Constructor/Destructor */
        kFileDef(kFile *pFile) throw(kError);
        virtual ~kFileDef();

        /** @cat Module information methods. */
        KBOOL       moduleGetName(char *pszBuffer, int cchSize = 260);

        /** @cat Export enumeration methods. */
        KBOOL       exportFindFirst(kExportEntry *pExport);
        KBOOL       exportFindNext(kExportEntry *pExport);
        void        exportFindClose(kExportEntry *pExport);

        /** @cat Export Lookup methods */
        KBOOL       exportLookup(unsigned long ulOrdinal, kExportEntry *pExport);
        KBOOL       exportLookup(const char *  pszName, kExportEntry *pExport);

        /**@cat queries... */
        KBOOL       isDef() const                { return TRUE;}
        char const *queryModuleName(void) const  { return pszModName;     }
        char const *queryType(void) const        { return pszType;        }
        char const *queryBase(void) const        { return pszBase;        }
        char const *queryCode(void) const        { return pszCode;        }
        char const *queryData(void) const        { return pszData;        }
        char const *queryDescription(void) const { return pszDescription; }
        char const *queryExeType(void) const     { return pszExeType;     }
        char const *queryHeapSize(void) const    { return pszHeapSize;    }
        char const *queryOld(void) const         { return pszOld;         }
        char const *queryProtmode(void) const    { return pszProtmode;    }
        char const *queryStackSize(void) const   { return pszStackSize;   }
        char const *queryStub(void) const        { return pszStub;        }

        /**@cat Operations */
        KBOOL       makeWatcomLinkFileAddtion(kFile *pFile, int enmOS) throw(kError);

        enum {fullscreen = 0, pmvio = 2, pm = 3, unknown = 255};
        enum {os2, os2v1, dos, win32, win16, nlm, qnx, elf};
};

#endif
