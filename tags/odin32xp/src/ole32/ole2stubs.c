/*
 * Temporary place for ole2 stubs.
 *
 * Copyright (C) 1999 Corel Corporation
 * Move these functions to dlls/ole32/ole2impl.c when you implement them.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "windef.h"
#include "winbase.h"
#include "winuser.h"
#include "ole2.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ole);

/******************************************************************************
 *               OleCreateLinkToFile        [OLE32.@]
 */
HRESULT WINAPI  OleCreateLinkToFile(LPCOLESTR lpszFileName, REFIID riid,
	  		DWORD renderopt, LPFORMATETC lpFormatEtc,
			LPOLECLIENTSITE pClientSite, LPSTORAGE pStg, LPVOID* ppvObj)
{
    FIXME("(%p,%p,%li,%p,%p,%p,%p), stub!\n",lpszFileName, riid, renderopt, lpFormatEtc, pClientSite, pStg, ppvObj);
    return E_NOTIMPL;
}


/******************************************************************************
 *              OleDuplicateData        [OLE32.@]
 */
HRESULT WINAPI OleDuplicateData(HANDLE hSrc, CLIPFORMAT cfFormat,
	                          UINT uiFlags)
{
    FIXME("(%x,%x,%x), stub!\n", hSrc, cfFormat, uiFlags);
    return E_NOTIMPL;
}


/***********************************************************************
 *               WriteFmtUserTypeStg (OLE32.160)
 */
HRESULT WINAPI WriteFmtUserTypeStg(
	  LPSTORAGE pstg, CLIPFORMAT cf, LPOLESTR lpszUserType)
{
    FIXME("(%p,%x,%s) stub!\n",pstg,cf,debugstr_w(lpszUserType));
    return E_NOTIMPL;
}


/******************************************************************************
 *              SetConvertStg        [OLE32.@]
 */
HRESULT WINAPI SetConvertStg(LPSTORAGE pStg, BOOL fConvert)
{
  FIXME("(%p,%x), stub!\n", pStg, fConvert);
  return E_NOTIMPL;
}

/******************************************************************************
 *              OleCreateLink        [OLE32.@]
 */
HRESULT WINAPI OleCreateLink(LPMONIKER pmkLinkSrc, REFIID riid, DWORD renderopt, LPFORMATETC lpFormatEtc,
                LPOLECLIENTSITE pClientSite, LPSTORAGE pStg, LPVOID* ppvObj)
{
  FIXME("(not shown), stub!\n");
  return E_NOTIMPL;
}

/******************************************************************************
 *              OleCreateFromFile        [OLE32.@]
 */
HRESULT WINAPI OleCreateFromFile(REFCLSID rclsid, LPCOLESTR lpszFileName, REFIID riid,
            DWORD renderopt, LPFORMATETC lpFormatEtc, LPOLECLIENTSITE pClientSite, LPSTORAGE pStg, LPVOID* ppvObj)
{
  FIXME("(not shown), stub!\n");
  return E_NOTIMPL;
}


/******************************************************************************
 *              OleGetIconOfClass        [OLE32.@]
 */
HGLOBAL WINAPI OleGetIconOfClass(REFCLSID rclsid, LPOLESTR lpszLabel, BOOL fUseTypeAsLabel)
{
  FIXME("(%p,%p,%x), stub!\n", rclsid, lpszLabel, fUseTypeAsLabel);
  return (HGLOBAL)NULL;
}

/******************************************************************************
 *              ReadFmtUserTypeStg        [OLE32.136]
 */
HRESULT WINAPI ReadFmtUserTypeStg (LPSTORAGE pstg, CLIPFORMAT* pcf, LPOLESTR* lplpszUserType)
{
  FIXME("(%p,%p,%p), stub!\n", pstg, pcf, lplpszUserType);
  return E_NOTIMPL;
}

/******************************************************************************
 *              OleCreateStaticFromData        [OLE32.@]
 */
HRESULT     WINAPI OleCreateStaticFromData(LPDATAOBJECT pSrcDataObj, REFIID iid,
                DWORD renderopt, LPFORMATETC pFormatEtc, LPOLECLIENTSITE pClientSite,
                LPSTORAGE pStg, LPVOID* ppvObj)
{
  FIXME("(not shown), stub!\n");
  return E_NOTIMPL;
}

/******************************************************************************
 *              OleCreateLinkFromData        [OLE32.@]
 */

HRESULT WINAPI  OleCreateLinkFromData(LPDATAOBJECT pSrcDataObj, REFIID riid,
                DWORD renderopt, LPFORMATETC pFormatEtc,
                LPOLECLIENTSITE pClientSite, LPSTORAGE pStg,
                LPVOID* ppvObj)
{
  FIXME("(not shown), stub!\n");
  return E_NOTIMPL;
}

/******************************************************************************
 *              OleIsRunning        [OLE32.@]
 */
BOOL WINAPI OleIsRunning(LPOLEOBJECT pObject)
{
  FIXME("(%p), stub!\n", pObject);
  return TRUE;
}

/***********************************************************************
 *           OleRegEnumVerbs    [OLE32.@]
 */
HRESULT WINAPI OleRegEnumVerbs (REFCLSID clsid, LPENUMOLEVERB* ppenum)
{
    FIXME("(%p,%p), stub!\n", clsid, ppenum);
    return OLEOBJ_E_NOVERBS;
}

/***********************************************************************
 *           OleRegEnumFormatEtc    [OLE32.@]
 */
HRESULT     WINAPI OleRegEnumFormatEtc (
  REFCLSID clsid,
  DWORD    dwDirection,
  LPENUMFORMATETC* ppenumFormatetc)
{
    FIXME("(%p, %ld, %p), stub!\n", clsid, dwDirection, ppenumFormatetc);

    return E_NOTIMPL;
}

/***********************************************************************
 *           CoIsOle1Class                              [OLE32.@]
 */
BOOL WINAPI CoIsOle1Class(REFCLSID clsid)
{
  FIXME("%s\n", debugstr_guid(clsid));
  return FALSE;
}
