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
    BOOL                fResident;
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
        BOOL  fProgram;
        BOOL  fLibrary;
        BOOL  fPhysicalDevice;
        BOOL  fVirtualDevice;
        BOOL  fInitInstance;
        BOOL  fTermInstance;
        BOOL  fInitGlobal;
        BOOL  fTermGlobal;
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
        void  read(kFile *pFile) throw (int);
        char *readln(kFile *pFile, char *pszBuffer, int cbBuffer) throw (int);
        BOOL  isKeyword(const char *psz);
        BOOL  setModuleName(void);

    public:
        /**@cat Constructor/Destructor */
        kFileDef(kFile *pFile) throw(int);
        virtual ~kFileDef();

        /** @cat Module information methods. */
        BOOL        moduleGetName(char *pszBuffer, int cchSize = 260);

        /** @cat Export enumeration methods. */
        BOOL        exportFindFirst(kExportEntry *pExport);
        BOOL        exportFindNext(kExportEntry *pExport);
        void        exportFindClose(kExportEntry *pExport);

        /** @cat Export Lookup methods */
        BOOL        exportLookup(unsigned long ulOrdinal, kExportEntry *pExport);
        BOOL        exportLookup(const char *  pszName, kExportEntry *pExport);

        /**@cat queries... */
        BOOL        isDef() const                { return TRUE;}
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
        BOOL        makeWatcomLinkFileAddtion(kFile *pFile) throw(int);

        enum {fullscreen = 0, pmvio = 2, pm = 3, unknown = 255};
};

#endif
