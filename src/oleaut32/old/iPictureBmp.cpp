/* $Id: iPictureBmp.cpp,v 1.1 2001-08-10 19:24:40 sandervl Exp $ */
/* 
 * OLE Picture Bitmap Strategy object
 *
 * 20/11/99 - New Code
 * 
 * Copyright 1999 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
 
#include "oleaut32.h"
#include "olectl.h"
#include "ipicture.h"

// ======================================================================
// Local Methods
// ======================================================================

// ----------------------------------------------------------------------
// Bitmap Picture
// ----------------------------------------------------------------------
IPictureBmp::~IPictureBmp()
{
    if (pPicture->fOwn)
    {
	// Free bitmap...
	DeleteObject(pPicture->u.bmp.hbitmap);
	DeleteObject(pPicture->u.bmp.hpal);
    }
}

// ----------------------------------------------------------------------
// IPictureBmp::get_Handle
// ----------------------------------------------------------------------
HRESULT IPictureBmp::get_Handle(OLE_HANDLE * pHandle)
{
    *pHandle = pPicture->u.bmp.hbitmap;
    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureBmp::get_hPal
// ----------------------------------------------------------------------
HRESULT IPictureBmp::get_hPal(OLE_HANDLE * phPal)
{
    *phPal = pPicture->u.bmp.hpal;
    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureBmp::get_Type
// ----------------------------------------------------------------------
HRESULT IPictureBmp::get_Type(SHORT * pType)
{
    *pType = PICTYPE_BITMAP;
    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureBmp::get_Width
// ----------------------------------------------------------------------
HRESULT IPictureBmp::get_Width(OLE_XSIZE_HIMETRIC * pWidth)
{
    SIZE	size;

    if (GetBitmapDimensionEx( pPicture->u.bmp.hbitmap, &size) == 0)
    {
	*pWidth = 0;
	return E_FAIL;
    }

    *pWidth = size.cx;

    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureBmp::get_Height
// ----------------------------------------------------------------------
HRESULT IPictureBmp::get_Height(OLE_YSIZE_HIMETRIC * pHeight)
{
    SIZE	size;

    if (GetBitmapDimensionEx( pPicture->u.bmp.hbitmap, &size) == 0)
    {
	*pHeight = 0;
	return E_FAIL;
    }

    *pHeight = size.cy;

    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureBmp::Render
// ----------------------------------------------------------------------
HRESULT IPictureBmp::Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				LPCRECT pRcWBounds)
{
    return E_FAIL;
}

// ----------------------------------------------------------------------
// IPictureBmp::set_hPal
// ----------------------------------------------------------------------
HRESULT IPictureBmp::set_hPal(OLE_HANDLE hPal)
{
    pPicture->u.bmp.hpal = hPal;
    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureBmp::SaveAsFile
// ----------------------------------------------------------------------
HRESULT IPictureBmp::SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize)
{
    return E_FAIL;
}

// ----------------------------------------------------------------------
//  IPictureBmp::get_Attributes
// ----------------------------------------------------------------------
HRESULT IPictureBmp::get_Attributes(DWORD * pDwAttr)
{
    // Although bitmaps may be scaled, it is really stretching.
    *pDwAttr = 0;
    return S_OK;
}

