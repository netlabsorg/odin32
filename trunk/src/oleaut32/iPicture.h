/* $Id: iPicture.h,v 1.1 1999-11-14 01:26:36 davidr Exp $ */
/* 
 * OLE Picture functions private header
 *
 * 12/11/99 - New Code
 * 
 * Copyright 1999 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#if !defined(IPICTURE_INCLUDED)
#define IPICTURE_INCLUDED

#define PICTYPE_UNINITIALIZED (-1)
#define PICTYPE_NONE            0
#define PICTYPE_BITMAP          1
#define PICTYPE_METAFILE        2
#define PICTYPE_ICON            3
#define PICTYPE_ENHMETAFILE     4

typedef struct tagPICTDESC
{
    UINT cbSizeofstruct;
    UINT picType;
    union
    {
        struct
        {
            HBITMAP   hbitmap;        // Bitmap
            HPALETTE  hpal;           // Accompanying palette
        } bmp;

        struct
        {
            HMETAFILE hmeta;          // Metafile
            int       xExt;
            int       yExt;           // Extent
        } wmf;

        struct
        {
            HICON hicon;              // Icon
        } icon;

        struct
        {
            HENHMETAFILE hemf;        // Enhanced Metafile
        } emf;
    } u;

} PICTDESC, * LPPICTDESC;

#endif /* IPICTURE_INCLUDED */
