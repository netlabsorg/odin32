/* $Id: kFileLX.h,v 1.6 2002-02-24 02:47:26 bird Exp $
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

struct e32_exe;
struct o32_obj;
struct o32_map;


class kFileLX : public kFileFormatBase, public kExecutableI, public kPageI, public kRelocI
{
public:
    kFileLX(const char *pszFilename) throw (kError);
    kFileLX(kFile *pFile) throw (kError);
    ~kFileLX();

    /** @cat Module information methods. */
    KBOOL               moduleGetName(char *pszBuffer, int cchSize = 260);

    /** @cat Misc */
    virtual KBOOL       isLx() const {return TRUE;};

    struct o32_obj *    getObject(int iObject);
    int                 getObjectCount();

    /** @cat Export enumeration methods. */
    KBOOL               exportFindFirst(kExportEntry *pExport);
    KBOOL               exportFindNext(kExportEntry *pExport);
    void                exportFindClose(kExportEntry *pExport);

    /** @cat Export Lookup methods */
    KBOOL               exportLookup(unsigned long ulOrdinal, kExportEntry *pExport);
    KBOOL               exportLookup(const char *  pszName, kExportEntry *pExport);

    /** @cat Get and put page methods. */
    int                 pageGet(char *pachPage, unsigned long ulAddress) const;
    int                 pageGet(char *pachPage, int iObject, int offObject) const;
    int                 pagePut(const char *pachPage, unsigned long ulAddress);
    int                 pagePut(const char *pachPage, int iObject, int offObject);
    int                 pageGetPageSize() const;

    /** @cat Compression and expansion methods compatible with exepack:1 and exepack:2. */
    static int          expandPage1(char *pachPage, int cchPage, const char * pachSrcPage, int cchSrcPage);
    static int          expandPage2(char *pachPage, int cchPage, const char * pachSrcPage, int cchSrcPage);
    static int          compressPage1(char *pachPage, const char * pachSrcPage, int cchSrcPage);
    static int          compressPage2(char *pachPage, const char * pachSrcPage, int cchSrcPage);

    /** @cat Relocation methods */
    KBOOL               relocFindFirst(unsigned long iSegment, unsigned long offObject, kRelocEntry *preloc);
    KBOOL               relocFindFirst(unsigned long ulAddress, kRelocEntry *preloc);
    KBOOL               relocFindNext(kRelocEntry *preloc);
    void                relocFindClose(kRelocEntry *preloc);


protected:
    /** @cat Internal data */
    void *              pvBase;         /* Pointer to filemapping. */
    unsigned long       cbFile;         /* Size of filemapping. */
    unsigned long       offLXHdr;       /* Offset of the LX header. */
    struct e32_exe *    pe32;           /* Pointer to the exe header within the filemapping. */
    struct o32_obj *    paObject;       /* Pointer to the objecttable. */
    struct o32_map *    paMap;          /* Pointer to page map table. */
    unsigned long   *   paulFixupPageTable; /* Pointer to the fixup page table. */
    char *              pchFixupRecs;   /* Pointer to the fixup record table. */

    /** @cat Export and Module information worker. */
    KBOOL               queryExportName(int iOrdinal, char *pszBuffer);

    /** @cat Get and put page workers. */
    /*
    int                 pageGetLX(char *pachPage, int iObject, int iPage) const;
    int                 pageGetLX(char *pachPage, int iPage) const;
    int                 pagePutLX(const char *pachPage, int iObject, int iPage);
    int                 pagePutLX(const char *pachPage, int iPage);
    */

    /** @cat Address mapping helpers. */
    unsigned long       lxAddressToObjectOffset(unsigned long ulAddress, unsigned long * pulObject) const;
    unsigned long       lxSpecialSelectorToObjectOffset(unsigned long offObject, unsigned long * pulObject) const;
    KBOOL               lxValidateObjectOffset(unsigned long ulObject, unsigned long offOffset) const;
    KBOOL               lxValidateAddress(unsigned long ulAddress) const;

    /** @cat Import Helpers */
    char *              lxGetImportModuleName(unsigned long ordModule) const;
    char *              lxGetImportProcName(unsigned long offProc) const;
};

#endif
