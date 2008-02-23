/* $Id: iPictureNone.cpp,v 1.1 2001-08-10 19:24:44 sandervl Exp $ */
/* 
 * OLE Picture Metafile Strategy object
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

// ----------------------------------------------------------------------
// Uninitialised Picture
// ----------------------------------------------------------------------
HRESULT IPictureNone::get_Handle(OLE_HANDLE * pHandle)
{
    *pHandle = 0;
    return E_FAIL;
}

HRESULT IPictureNone::get_hPal(OLE_HANDLE * phPal)
{
    *phPal = 0;
    return E_FAIL;
}

HRESULT IPictureNone::get_Type(SHORT * pType)
{
    *pType = PICTYPE_UNINITIALIZED;
    return S_OK;
}

HRESULT IPictureNone::get_Width(OLE_XSIZE_HIMETRIC * pWidth)
{
    *pWidth = 0;
    return E_FAIL;
}

HRESULT IPictureNone::get_Height(OLE_YSIZE_HIMETRIC * pHeight)
{
    *pHeight = 0;
    return E_FAIL;
}

HRESULT IPictureNone::Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				LPCRECT pRcWBounds)
{
    return E_FAIL;
}

HRESULT IPictureNone::set_hPal(OLE_HANDLE hPal)
{
    return E_FAIL;
}

HRESULT IPictureNone::SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize)
{
    return E_FAIL;
}

HRESULT IPictureNone::get_Attributes(DWORD * pDwAttr)
{
    *pDwAttr = 0;
    return S_OK;
}

IPictureBmp::IPictureBmp(IPictureImpl * a_pPicture, HBITMAP hbitmap, HPALETTE hpal)
    : IPictureStrat(a_pPicture)
{
    pPicture->u.bmp.hbitmap = hbitmap;
    pPicture->u.bmp.hpal = hpal;
}

