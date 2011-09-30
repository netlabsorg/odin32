/* $Id: kFilePE.h,v 1.5 2002-02-24 02:47:27 bird Exp $
 *
 * kFilePE - PE files.
 *
 * Copyright (c) 1999-2001 knut st. osmundsen
 *
 */

#ifndef _kFilePE_h_
#define _kFilePE_h_

#ifndef _peexe_h_
#define PIMAGE_DOS_HEADER               void *
#define PIMAGE_FILE_HEADER              void *
#define PIMAGE_OPTIONAL_HEADER          void *
#define PIMAGE_DATA_DIRECTORY           void *
#define PIMAGE_SECTION_HEADER           void *
#define PIMAGE_EXPORT_DIRECTORY         void *
#define PIMAGE_IMPORT_DESCRIPTOR        void *
#define PIMAGE_RESOURCE_DIRECTORY       void *
#define PIMAGE_BASE_RELOCATION          void *
#define PIMAGE_DEBUG_DIRECTORY          void *
#define PIMAGE_TLS_DIRECTORY            void *
#define PIMAGE_LOAD_CONFIG_DIRECTORY    void *
#define PIMAGE_IMPORT_DESCRIPTOR        void *
#define PIMAGE_THUNK_DATA               void *
#define PIMAGE_IMPORT_DESCRIPTOR        void *
#endif


/**
 * PE (portable executable) files.
 * @author      knut st. osmundsen
 */
class kFilePE : public kFileFormatBase, public kExecutableI
{
    private:
        void *                          pvBase;         /* The file mapping. */

        /* headers - pointers within the filemapping. */
        PIMAGE_DOS_HEADER               pDosHdr;
        PIMAGE_FILE_HEADER              pFileHdr;
        PIMAGE_OPTIONAL_HEADER          pOptHdr;
        PIMAGE_DATA_DIRECTORY           paDataDir;
        PIMAGE_SECTION_HEADER           paSectionHdr;

        /** @cat
         * Directory pointers.
         */
        PIMAGE_EXPORT_DIRECTORY         pExportDir;     /* 0 */
        PIMAGE_IMPORT_DESCRIPTOR        pImportDir;     /* 1 */
        PIMAGE_RESOURCE_DIRECTORY       pRsrcDir;       /* 2 */
        void *                          pExcpDir;       /* 3 */
        void *                          pSecDir;        /* 4 */
        PIMAGE_BASE_RELOCATION          pBRelocDir;     /* 5 */
        PIMAGE_DEBUG_DIRECTORY          pDebugDir;      /* 6 */
        char *                          pCopyright;     /* 7 */
        unsigned long *                 pulGlobalPtr;   /* 8 */  //is this the correct pointer type?
        PIMAGE_TLS_DIRECTORY            pTLSDir;        /* 9 */
        PIMAGE_LOAD_CONFIG_DIRECTORY    pLoadConfigDir; /* 10 */
        PIMAGE_IMPORT_DESCRIPTOR        pBoundImportDir;/* 11 */ //is this the correct pointer type?
        PIMAGE_THUNK_DATA               pIATDir;        /* 12 */ //is this the correct pointer type?
        PIMAGE_IMPORT_DESCRIPTOR        pDelayImportDir;/* 13 */
        void *                          pComDir;        /* 14 */
        void *                          pv15;           /* 15 */

    public:
        kFilePE(kFile *pFile) throw (kError);
        virtual ~kFilePE();

        /** @cat Module information methods. */
        KBOOL       moduleGetName(char *pszBuffer, int cchSize = 260);

        /** @cat Export enumeration methods. */
        KBOOL       exportFindFirst(kExportEntry *pExport);
        KBOOL       exportFindNext(kExportEntry *pExport);
        void        exportFindClose(kExportEntry *pExport);

        /** @cat Export Lookup methods */
        KBOOL       exportLookup(unsigned long ulOrdinal, kExportEntry *pExport);
        KBOOL       exportLookup(const char *  pszName, kExportEntry *pExport);

        KBOOL       isPe() const    { return TRUE;}

        KBOOL       dump(kFile *pOut);
};

#endif
