/* $Id: kFileFormatBase.h,v 1.7 2002-02-24 02:47:25 bird Exp $
 *
 * kFileFormatBase - Base class for kFile<format> classes.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _kFileFormat_h_
#define _kFileFormat_h_

/******************************************************************************
*   Defined Constants                                                         *
******************************************************************************/
#define ORD_START_INTERNAL_FUNCTIONS 1200


/******************************************************************************
*   Structures and Typedefs                                                   *
******************************************************************************/
#pragma pack(4)

class kFile;


/**
 * Base class for file formats.
 * @author      knut st. osmundsen
 */
class kFileFormatBase
{
public:
    virtual KBOOL isDef() const { return FALSE;}
    virtual KBOOL isPe() const  { return FALSE;}
    virtual KBOOL isLx() const  { return FALSE;}
    virtual KBOOL dump(kFile *pOut);

    kFileFormatBase(kFile *pFile) throw (kError);
    virtual ~kFileFormatBase() throw (kError);

protected:
    kFile *pFile;
};

#pragma pack()

#endif
