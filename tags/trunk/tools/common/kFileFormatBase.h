/* $Id: kFileFormatBase.h,v 1.6 2001-06-22 17:07:47 bird Exp $
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

    virtual ~kFileFormatBase();

    virtual BOOL  isDef() const { return FALSE;}
    virtual BOOL  isPe() const  { return FALSE;}
    virtual BOOL  isLx() const  { return FALSE;}
    virtual BOOL  dump(kFile *pOut);

};

#pragma pack()

#endif
