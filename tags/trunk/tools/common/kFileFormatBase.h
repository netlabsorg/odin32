/*
 * kFileFormatBase - Base class for kFile<format> classes.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _kFileFormat_h_
#define _kFileFormat_h_

/******************************************************************************
*   Defined Constants                                                         *
******************************************************************************/
#define MAXEXPORTNAME 256
#define ORD_START_INTERNAL_FUNCTIONS 1200


/******************************************************************************
*   Structures and Typedefs                                                   *
******************************************************************************/
#pragma pack(4)

/**
 * ExportEntry used by the findFirstExport/findNextExport functions
 */
typedef struct _ExportEntry
{
    unsigned long  ulOrdinal;
    char           achName[MAXEXPORTNAME];
    char           achIntName[MAXEXPORTNAME]; /* not used by PEFile */
    /* internal - do not use! */
    void          *pv;
} EXPORTENTRY, *PEXPORTENTRY;


/**
 * Base class for file formats.
 * @author      knut st. osmundsen
 */
class kFileFormatBase
{
    public:
        virtual BOOL  queryModuleName(char *pszBuffer) = 0;
        virtual BOOL  findFirstExport(PEXPORTENTRY pExport) = 0;
        virtual BOOL  findNextExport(PEXPORTENTRY pExport)  = 0;
        virtual BOOL  isDef() const { return FALSE;}
        virtual BOOL  isPe() const  { return FALSE;}
};

#pragma pack()

#endif
