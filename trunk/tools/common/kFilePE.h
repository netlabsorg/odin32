/*
 * kFilePE - PE files.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

#ifndef _kFilePE_h_
#define _kFilePE_h_

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/

#if !defined(__WINE_PEEXE_H) && !defined(_WINNT_)
#if 0
    #define PIMAGE_DOS_HEADER           void*
    #define PIMAGE_FILE_HEADER          void*
    #define PIMAGE_OPTIONAL_HEADER      void*
    #define PIMAGE_DATA_DIRECTORY       void*
    #define PIMAGE_SECTION_HEADER       void*
    #define PIMAGE_EXPORT_DIRECTORY          void*
    #define PIMAGE_IMPORT_DESCRIPTOR         void*
    #define PIMAGE_RESOURCE_DIRECTORY        void*
    #define PIMAGE_BASE_RELOCATION           void*
    #define PIMAGE_DEBUG_DIRECTORY           void*
    #define PSZ                              void*
    #define PULONG                           void*
    #define PIMAGE_TLS_DIRECTORY             void*
    #define PIMAGE_LOAD_CONFIG_DIRECTORY     void*
    #define PIMAGE_IMPORT_DESCRIPTOR         void*
    #define PIMAGE_THUNK_DATA                void*
    #define PIMAGE_IMPORT_DESCRIPTOR         void*
#else
#include <peexe.h>
#endif
#endif


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/

/**
 * PE (portable executable) files.
 * TODO: error handling.
 * @author      knut st. osmundsen
 */
class kFilePE : public kFileFormatBase
{
    private:
        void                   *pvBase;

        /* headers */
        PIMAGE_DOS_HEADER       pDosHdr;
        PIMAGE_FILE_HEADER      pFileHdr;
        PIMAGE_OPTIONAL_HEADER  pOptHdr;
        PIMAGE_DATA_DIRECTORY   paDataDir;
        PIMAGE_SECTION_HEADER   paSectionHdr;

        /** @cat
         * Directory pointers.
         */
        PIMAGE_EXPORT_DIRECTORY         pExportDir;     /* 0 */
        PIMAGE_IMPORT_DESCRIPTOR        pImportDir;     /* 1 */
        PIMAGE_RESOURCE_DIRECTORY       pRsrcDir;       /* 2 */
        PVOID                           pExcpDir;       /* 3 */
        PVOID                           pSecDir;        /* 4 */
        PIMAGE_BASE_RELOCATION          pBRelocDir;     /* 5 */
        PIMAGE_DEBUG_DIRECTORY          pDebugDir;      /* 6 */
        PSZ                             pCopyright;     /* 7 */
        PULONG                          pulGlobalPtr;   /* 8 */  //is this the correct pointer type?
        PIMAGE_TLS_DIRECTORY            pTLSDir;        /* 9 */
        PIMAGE_LOAD_CONFIG_DIRECTORY    pLoadConfigDir; /* 10 */
        PIMAGE_IMPORT_DESCRIPTOR        pBoundImportDir;/* 11 */ //is this the correct pointer type?
        PIMAGE_THUNK_DATA               pIATDir;        /* 12 */ //is this the correct pointer type?
        PIMAGE_IMPORT_DESCRIPTOR        pDelayImportDir;/* 13 */
        PVOID                           pComDir;        /* 14 */
        PVOID                           pv15;           /* 15 */

    public:
        kFilePE(kFile *pFile) throw(int);
        virtual ~kFilePE();

        /* operations */
        BOOL  queryModuleName(char *pszBuffer);
        BOOL  findFirstExport(PEXPORTENTRY pExport);
        BOOL  findNextExport(PEXPORTENTRY pExport);
        BOOL  isPe() const   { return TRUE;}

        BOOL  dump(kFile *pOut);
};

#endif
