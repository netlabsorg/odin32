/*
 * kFilePE - PE files.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

#ifndef _kFilePE_h_
#define  _kFilePE_h_

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#if !defined(__WINE_PEEXE_H) && !defined(_WINNT_)
    #define PIMAGE_DOS_HEADER           void*
    #define PIMAGE_FILE_HEADER          void*
    #define PIMAGE_OPTIONAL_HEADER      void*
    #define PIMAGE_DATA_DIRECTORY       void*
    #define PIMAGE_SECTION_HEADER       void*
    #define PIMAGE_EXPORT_DIRECTORY     void*
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

        /* directories */
        PIMAGE_EXPORT_DIRECTORY pExportDir;

    public:
        kFilePE(FILE *phFile) throw(int);
        virtual ~kFilePE();

        /* operations */
        BOOL  queryModuleName(char *pszBuffer);
        BOOL  findFirstExport(PEXPORTENTRY pExport);
        BOOL  findNextExport(PEXPORTENTRY pExport);
        BOOL  isPe() const   { return TRUE;}
};

#endif
