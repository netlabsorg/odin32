/* $Id: iPictureMeta.cpp,v 1.1 2001-08-10 19:24:43 sandervl Exp $ */
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
// Std Metafile Picture
// ----------------------------------------------------------------------

IPictureMeta::IPictureMeta(IPictureImpl * a_pPicture, HMETAFILE hmeta, int xExt, int yExt)
    : IPictureStrat(a_pPicture)
{
    pPicture->u.wmf.hmeta = hmeta;
    pPicture->u.wmf.xExt = xExt;
    pPicture->u.wmf.yExt = yExt;
}

IPictureMeta::~IPictureMeta()
{
    if (pPicture->fOwn)
    {
	// Free metafile...
	DeleteMetaFile(pPicture->u.wmf.hmeta);
    }
}

HRESULT IPictureMeta::get_Handle(OLE_HANDLE * pHandle)
{
    *pHandle = pPicture->u.wmf.hmeta;
    return S_OK;
}

HRESULT IPictureMeta::get_hPal(OLE_HANDLE * phPal)
{
    *phPal = 0;
    return E_FAIL;
}

HRESULT IPictureMeta::get_Type(SHORT * pType)
{
    *pType = PICTYPE_METAFILE;
    return S_OK;
}

HRESULT IPictureMeta::get_Width(OLE_XSIZE_HIMETRIC * pWidth)
{
    *pWidth = pPicture->u.wmf.xExt;
    return S_OK;
}

HRESULT IPictureMeta::get_Height(OLE_YSIZE_HIMETRIC * pHeight)
{
    *pHeight = pPicture->u.wmf.yExt;
    return S_OK;
}

HRESULT IPictureMeta::Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				LPCRECT pRcWBounds)
{
    return E_FAIL;
}

HRESULT IPictureMeta::set_hPal(OLE_HANDLE hPal)
{
    return E_FAIL;
}

HRESULT IPictureMeta::SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize)
{
    return E_FAIL;
}

HRESULT IPictureMeta::get_Attributes(DWORD * pDwAttr)
{
    *pDwAttr = PICTURE_SCALEABLE | PICTURE_TRANSPARENT;
    return S_OK;
}

