/* $Id: kFileLX.h,v 1.1 2000-03-27 10:18:41 bird Exp $
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

public:
    kFileLX(const char *pszFilename);
    ~kFileLX();

    virtual BOOL queryModuleName(char *pszBuffer);
    virtual BOOL findFirstExport(PEXPORTENTRY pExport);
    virtual BOOL findNextExport(PEXPORTENTRY pExport);
    virtual BOOL isLx() const {return TRUE;};

    struct o32_obj * getObject(int iObject);


};
#endif
