/* $Id: kFileLX.h,v 1.5 2001-04-17 00:26:11 bird Exp $
 *
 * kFileLX - Linear Executable file reader.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _kFileLX_h_
#define _kFileLX_h_




class kFileLX : public kFileFormatBase, public kExecutableI /*, public kPageI*/
{
protected:
    PVOID               pvBase;         /* Pointer to filemapping. */
    ULONG               cbFile;         /* Size of filemapping. */
    ULONG               offLXHdr;       /* Offset of the LX header. */
    struct e32_exe *    pe32;           /* Pointer to the exe header within the filemapping. */
    struct o32_obj *    paObject;       /* Pointer to the objecttable. */
    struct o32_map *    paMap;          /* Pointer to page map table. */

    BOOL                queryExportName(int iOrdinal, char *pszBuffer);

public:
    kFileLX(const char *pszFilename) throw (int);
    kFileLX(kFile *pFile) throw (int);
    ~kFileLX();

    /** @cat Module information methods. */
    BOOL        moduleGetName(char *pszBuffer, int cchSize = 260);

    /** @cat Export enumeration methods. */
    BOOL        exportFindFirst(kExportEntry *pExport);
    BOOL        exportFindNext(kExportEntry *pExport);
    void        exportFindClose(kExportEntry *pExport);

    /** @cat Export Lookup methods */
    BOOL        exportLookup(unsigned long ulOrdinal, kExportEntry *pExport);
    BOOL        exportLookup(const char *  pszName, kExportEntry *pExport);

    virtual BOOL        isLx() const {return TRUE;};

    struct o32_obj *    getObject(int iObject);
    int                 getObjectCount();

#if 0
    /** @cat Get and put page methods. (the first ones are generic) */
    BOOL                getPage(char *pachPage, ULONG ulAddress);
    BOOL                getPage(char *pachPage, int iObject, int offObject);
    BOOL                putPage(const char *pachPage, ULONG ulAddress);
    BOOL                putPage(const char *pachPage, int iObject, int offObject);

    BOOL                getPageLX(char *pachPage, int iObject, int iPage);
    BOOL                getPageLX(char *pachPage, int iPage);
    BOOL                putPageLX(const char *pachPage, int iObject, int iPage);
    BOOL                putPageLX(const char *pachPage, int iPage);

    /** @cat Compression and expansion methods compatible with exepack:1 and exepack:2. */
    static BOOL         expandPage1(char *pachPage, const char * pachSrcPage, int cchSrcPage);
    static BOOL         expandPage2(char *pachPage, const char * pachSrcPage, int cchSrcPage);
    static int          compressPage1(char *pachPage, const char * pachSrcPage);
    static int          compressPage2(char *pachPage, const char * pachSrcPage);
#endif
};
#endif
