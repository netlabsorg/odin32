/* $Id: iPictureEmf.cpp,v 1.1 2001-08-10 19:24:41 sandervl Exp $ */
/* 
 * OLE Picture Enhanced Metafile Strategy object
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
// Enhanced Metafile Picture
// ----------------------------------------------------------------------

IPictureEmf::IPictureEmf(IPictureImpl * a_pPicture, HENHMETAFILE hemf)
    : IPictureStrat(a_pPicture)
{
    pPicture->u.emf.hemf = hemf;
}

IPictureEmf::~IPictureEmf()
{
    if (pPicture->fOwn)
    {
	// Free metafile...
	DeleteEnhMetaFile(pPicture->u.emf.hemf);
    }
}

HRESULT IPictureEmf::get_Handle(OLE_HANDLE * pHandle)
{
    *pHandle = pPicture->u.emf.hemf;
    return S_OK;
}

HRESULT IPictureEmf::get_hPal(OLE_HANDLE * phPal)
{
    *phPal = 0;
    return S_OK;
}

HRESULT IPictureEmf::get_Type(SHORT * pType)
{
    *pType = PICTYPE_ENHMETAFILE;
    return S_OK;
}

HRESULT IPictureEmf::get_Width(OLE_XSIZE_HIMETRIC * pWidth)
{
    *pWidth = 0;
    return E_FAIL;
}

HRESULT IPictureEmf::get_Height(OLE_YSIZE_HIMETRIC * pHeight)
{
    *pHeight = 0;
    return E_FAIL;
}

HRESULT IPictureEmf::Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				LPCRECT pRcWBounds)
{
    return E_FAIL;
}

HRESULT IPictureEmf::set_hPal(OLE_HANDLE hPal)
{
    return E_FAIL;
}

HRESULT IPictureEmf::SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize)
{
    return E_FAIL;
}

HRESULT IPictureEmf::get_Attributes(DWORD * pDwAttr)
{
    *pDwAttr = PICTURE_SCALEABLE | PICTURE_TRANSPARENT;
    return S_OK;
}

