/* $Id: iPictureIcon.cpp,v 1.1 2001-08-10 19:24:42 sandervl Exp $ */
/* 
 * OLE Picture Icon Strategy object
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
// Icon Picture
// ----------------------------------------------------------------------
IPictureIcon::IPictureIcon(IPictureImpl * a_pPicture, HICON hicon)
    : IPictureStrat(a_pPicture)
{
    pPicture->u.icon.hicon = hicon;
}

IPictureIcon::~IPictureIcon()
{
    if (pPicture->fOwn)
    {
	// Free icon...
	DestroyIcon(pPicture->u.icon.hicon);
    }
}

HRESULT IPictureIcon::get_Handle(OLE_HANDLE * pHandle)
{
    *pHandle = pPicture->u.icon.hicon;
    return S_OK;
}

HRESULT IPictureIcon::get_hPal(OLE_HANDLE * phPal)
{
    *phPal = 0;
    return S_OK;
}

HRESULT IPictureIcon::get_Type(SHORT * pType)
{
    *pType = PICTYPE_ICON;
    return S_OK;
}

HRESULT IPictureIcon::get_Width(OLE_XSIZE_HIMETRIC * pWidth)
{
    // FIXME - Read from system constants...
    *pWidth = 32;

    return S_OK;
}

HRESULT IPictureIcon::get_Height(OLE_YSIZE_HIMETRIC * pHeight)
{
    // FIXME - Read from system constants...
    *pHeight = 32;

    return S_OK;
}

HRESULT IPictureIcon::Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				LPCRECT pRcWBounds)
{
    return E_FAIL;
}

HRESULT IPictureIcon::set_hPal(OLE_HANDLE hPal)
{
    return E_FAIL;
}

HRESULT IPictureIcon::SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize)
{
    return E_FAIL;
}

HRESULT IPictureIcon::get_Attributes(DWORD * pDwAttr)
{
    *pDwAttr = PICTURE_TRANSPARENT;
    return S_OK;
}
