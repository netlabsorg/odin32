/* $Id: kFileLX.h,v 1.2 2000-08-31 03:00:13 bird Exp $
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




class kFileLX : public kFileFormatBase
{
protected:
    PVOID               pvBase;
    ULONG               offLXHdr;
    struct e32_exe *    pe32;
    struct o32_obj *    paObject;

    BOOL                queryExportName(int iOrdinal, char *pszBuffer);

public:
    kFileLX(const char *pszFilename);
    ~kFileLX();

    virtual BOOL        queryModuleName(char *pszBuffer);
    virtual BOOL        findFirstExport(PEXPORTENTRY pExport);
    virtual BOOL        findNextExport(PEXPORTENTRY pExport);
    virtual BOOL        isLx() const {return TRUE;};

    struct o32_obj *    getObject(int iObject);
    int                 getObjectCount();


};
#endif
