/* $Id: iPicture.cpp,v 1.1 2001-08-10 19:24:37 sandervl Exp $ */
/* 
 * OLE Picture functions
 *
 * Defines the OLE "Picture" object
 * This object has the following ingoing interfaces:
 *  IUnknown
 *  IPicture		- Picture manipulation
 *  IDispatch		- for Automation
 *  IPictureDisp	- for Automation
 *  IPersistStream	- Persist to stream
 * 
 * 12/11/99 - New Code
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
// Prototypes.
// ======================================================================

// ----------------------------------------------------------------------
// IPictureImpl
// ----------------------------------------------------------------------
static IPictureImpl *   IPictureImpl_Constructor(LPPICTDESC description, BOOL fOwn);
static void             IPictureImpl_Destructor(IPictureImpl * obj);

static HRESULT WIN32API	IPictureImpl_QueryInterface(LPPICTURE iface,
				REFIID refiid, LPVOID * obj);
static ULONG   WIN32API	IPictureImpl_AddRef(LPPICTURE iface);
static ULONG   WIN32API	IPictureImpl_Release(LPPICTURE iface);
static HRESULT WIN32API	IPictureImpl_get_Handle(LPPICTURE iface,  
				OLE_HANDLE * pHandle);
static HRESULT WIN32API	IPictureImpl_get_hPal(LPPICTURE iface,
				OLE_HANDLE * phPal);
static HRESULT WIN32API	IPictureImpl_get_Type(LPPICTURE iface,
				SHORT * pType);
static HRESULT WIN32API	IPictureImpl_get_Width(LPPICTURE iface,
				OLE_XSIZE_HIMETRIC * pWidth);
static HRESULT WIN32API	IPictureImpl_get_Height(LPPICTURE iface,
				OLE_YSIZE_HIMETRIC * pHeight);
static HRESULT WIN32API	IPictureImpl_Render(LPPICTURE iface,
				HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				LPCRECT pRcWBounds);
static HRESULT WIN32API	IPictureImpl_set_hPal(LPPICTURE iface,
				OLE_HANDLE hPal);
static HRESULT WIN32API	IPictureImpl_get_CurDC(LPPICTURE iface,
				HDC * phDC);
static HRESULT WIN32API	IPictureImpl_SelectPicture(LPPICTURE iface,
				HDC hDCIn, HDC * phDCOut, OLE_HANDLE * phBmpOut);
static HRESULT WIN32API	IPictureImpl_get_KeepOriginalFormat(LPPICTURE iface,
				BOOL * pKeep);
static HRESULT WIN32API	IPictureImpl_put_KeepOriginalFormat(LPPICTURE iface,
				BOOL Keep);
static HRESULT WIN32API	IPictureImpl_PictureChanged(LPPICTURE iface);
static HRESULT WIN32API	IPictureImpl_SaveAsFile(LPPICTURE iface,
				LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize);
static HRESULT WIN32API	IPictureImpl_get_Attributes(LPPICTURE iface,
				DWORD * pDwAttr);

// ----------------------------------------------------------------------
// IDispatch
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_QueryInterface(LPDISPATCH iface,
				REFIID riid, LPVOID * obj);
static ULONG   WINAPI IPictureImpl_IDispatch_AddRef(LPDISPATCH iface);
static ULONG   WINAPI IPictureImpl_IDispatch_Release(LPDISPATCH iface);
static HRESULT WINAPI IPictureImpl_IDispatch_GetTypeInfoCount(LPDISPATCH iface, 
				unsigned int* pctinfo);
static HRESULT WINAPI IPictureImpl_IDispatch_GetTypeInfo(LPDISPATCH  iface,
				UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
static HRESULT WINAPI IPictureImpl_IDispatch_GetIDsOfNames(LPDISPATCH iface,
				REFIID riid, LPOLESTR* rgszNames, UINT cNames,
				LCID lcid, DISPID* rgDispId);
static HRESULT WINAPI IPictureImpl_IDispatch_Invoke(LPDISPATCH iface,
				DISPID dispIdMember, REFIID riid,
				LCID lcid, WORD wFlags,
				DISPPARAMS* pDispParams, VARIANT* pVarResult,
				EXCEPINFO* pExepInfo, UINT* puArgErr);

// ----------------------------------------------------------------------
// IPersistStream
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_QueryInterface(LPPERSISTSTREAM iface,
				REFIID riid, LPVOID * obj);
static ULONG   WINAPI IPictureImpl_IPersistStream_AddRef(LPPERSISTSTREAM iface);
static ULONG   WINAPI IPictureImpl_IPersistStream_Release(LPPERSISTSTREAM iface);
static HRESULT WINAPI IPictureImpl_IPersistStream_GetClassID(LPPERSISTSTREAM iface,
				CLSID * pClassID);
static HRESULT WINAPI IPictureImpl_IPersistStream_IsDirty(LPPERSISTSTREAM iface);
static HRESULT WINAPI IPictureImpl_IPersistStream_Load(LPPERSISTSTREAM iface,
				LPSTREAM pLoadStream);
static HRESULT WINAPI IPictureImpl_IPersistStream_Save(LPPERSISTSTREAM iface,
				LPSTREAM pOutStream, BOOL fClearDirty);
static HRESULT WINAPI IPictureImpl_IPersistStream_GetSizeMax(LPPERSISTSTREAM iface,
				ULARGE_INTEGER * pcbSize);

// ======================================================================
// Local Data
// ======================================================================
static ICOM_VTABLE(IPicture) IPictureImpl_VTable =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IPictureImpl_QueryInterface,
    IPictureImpl_AddRef,
    IPictureImpl_Release,
    IPictureImpl_get_Handle,
    IPictureImpl_get_hPal,
    IPictureImpl_get_Type,
    IPictureImpl_get_Width,
    IPictureImpl_get_Height,
    IPictureImpl_Render,
    IPictureImpl_set_hPal,
    IPictureImpl_get_CurDC,
    IPictureImpl_SelectPicture,
    IPictureImpl_get_KeepOriginalFormat,
    IPictureImpl_put_KeepOriginalFormat,
    IPictureImpl_PictureChanged,
    IPictureImpl_SaveAsFile,
    IPictureImpl_get_Attributes
};

static ICOM_VTABLE(IDispatch) IPictureImpl_IDispatch_VTable =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IPictureImpl_IDispatch_QueryInterface,
    IPictureImpl_IDispatch_AddRef,
    IPictureImpl_IDispatch_Release,
    IPictureImpl_IDispatch_GetTypeInfoCount,
    IPictureImpl_IDispatch_GetTypeInfo,
    IPictureImpl_IDispatch_GetIDsOfNames,
    IPictureImpl_IDispatch_Invoke
};

static ICOM_VTABLE(IPersistStream) IPictureImpl_IPersistStream_VTable =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IPictureImpl_IPersistStream_QueryInterface,
    IPictureImpl_IPersistStream_AddRef,
    IPictureImpl_IPersistStream_Release,
    IPictureImpl_IPersistStream_GetClassID,
    IPictureImpl_IPersistStream_IsDirty,
    IPictureImpl_IPersistStream_Load,
    IPictureImpl_IPersistStream_Save,
    IPictureImpl_IPersistStream_GetSizeMax
};

// ======================================================================
// Local Methods
// ======================================================================

// ----------------------------------------------------------------------
// IPictureImpl_QueryInterface
// ----------------------------------------------------------------------
static IPictureImpl *   IPictureImpl_Constructor(LPPICTDESC description, BOOL fOwn)
{
    IPictureImpl *	pNewObject;

    // Allocate a new object on the heap...
    if ((pNewObject = (IPictureImpl *)HeapAlloc(GetProcessHeap(), 0, sizeof(IPictureImpl))) == 0)
    	return 0;

    dprintf(("OLEAUT32: IPictureImpl(%p)->Contructor(fOwn = %d)\n", pNewObject, fOwn));

    // Initialise the vft's
    pNewObject->lpvtbl1 = &IPictureImpl_VTable;
    pNewObject->lpvtbl2 = &IPictureImpl_IDispatch_VTable;
    pNewObject->lpvtbl3 = &IPictureImpl_IPersistStream_VTable;

    // Initial reference count of 1...
    pNewObject->ref = 1;
    pNewObject->fOwn = fOwn;

    // If a description is not passed then leave the picture uninitialised...
    if (description == 0)
	pNewObject->pStrat = new IPictureNone(pNewObject);
    else
    {
	// Validate size...
	if (description->cbSizeofstruct < sizeof(PICTDESC))
	{
	    dprintf(("OLEAUT32: IPictureImpl(%p)->Constructor - Invalid cbSizeofstruct!", pNewObject));
	    return 0;
	}
	switch(description->picType)
	{
	    case PICTYPE_BITMAP:
		pNewObject->pStrat = new IPictureBmp(
					    pNewObject,
					    description->u.bmp.hbitmap,
					    description->u.bmp.hpal);
		break;

	    case PICTYPE_METAFILE:
		pNewObject->pStrat = new IPictureMeta(pNewObject,
					    description->u.wmf.hmeta,
					    description->u.wmf.xExt,
					    description->u.wmf.yExt);
		break;

	    case PICTYPE_ICON:
		pNewObject->pStrat = new IPictureIcon(pNewObject,
					    description->u.icon.hicon);
		break;

	    case PICTYPE_ENHMETAFILE:
		pNewObject->pStrat = new IPictureIcon(pNewObject,
					    description->u.emf.hemf);
		break;

	    default:
	    	// assume uninitialised...
		pNewObject->pStrat = new IPictureNone(pNewObject);
		break;
	}
    }

    return pNewObject;
}

// ----------------------------------------------------------------------
// IPictureImpl_Destructor
// ----------------------------------------------------------------------
static void             IPictureImpl_Destructor(IPictureImpl * This)
{
    dprintf(("OLEAUT32: IPictureImpl(%p)->Destructor()\n", This));

    delete This->pStrat;

    HeapFree(GetProcessHeap(), 0, This);
}

// ----------------------------------------------------------------------
// IPictureImpl_QueryInterface
// ----------------------------------------------------------------------
static HRESULT WIN32API IPictureImpl_QueryInterface(LPPICTURE iface, REFIID refiid, LPVOID *obj)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->QueryInterface()", This));

    // Sanity check...
    if ((This == 0) || (obj == 0))
	return E_INVALIDARG;

    // Initialise return jic...
    *obj = 0;

    // Go find the correct interface...
    if (IsEqualIID(&IID_IUnknown, refiid))
	*obj = &(This->lpvtbl1);
    else if (IsEqualIID(&IID_IPicture, refiid))
	*obj = &(This->lpvtbl1);
    else if (IsEqualIID(&IID_IDispatch, refiid))
	*obj = &(This->lpvtbl2);
    else if (IsEqualIID(&IID_IPictureDisp, refiid))
	*obj = &(This->lpvtbl2);
    else if (IsEqualIID(&IID_IPersistStream, refiid))
	*obj = &(This->lpvtbl3);
    else
	return OLE_E_ENUM_NOMORE; 

    // Query Interface always increases the reference count by one...
    IPictureImpl_AddRef((LPPICTURE)This);

    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureImpl_AddRef
// ----------------------------------------------------------------------
static ULONG WIN32API IPictureImpl_AddRef(LPPICTURE iface)
{ 
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->AddRef()", This));

    return ++(This->ref);
}

// ----------------------------------------------------------------------
// IPictureImpl_Release
// ----------------------------------------------------------------------
static ULONG WIN32API IPictureImpl_Release(LPPICTURE iface)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->Release()\n", This));

    if (--(This->ref) == 0)
    {
	IPictureImpl_Destructor(This);
	return 0;
    }
    return This->ref;
}

// ----------------------------------------------------------------------
// IPictureImpl_get_Handle
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_Handle(LPPICTURE iface,  
				OLE_HANDLE * pHandle)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_Handle()", This));

    // Sanity check...
    if (pHandle == 0)
    	return E_POINTER;

    // Return the handle...
    return This->pStrat->get_Handle(pHandle);
}

// ----------------------------------------------------------------------
// IPictureImpl_get_hPal
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_hPal(LPPICTURE iface,
				OLE_HANDLE * phPal)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_hPal()", This));

    // Sanity check...
    if (phPal == 0)
    	return E_POINTER;

    // Return the handle...
    return This->pStrat->get_hPal(phPal);
}

// ----------------------------------------------------------------------
// IPictureImpl_get_Type
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_Type(LPPICTURE iface,
				SHORT * pType)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_Type()", This));

    // Sanity check...
    if (pType == 0)
	return E_POINTER;

    // Return the type...
    return This->pStrat->get_Type(pType);
}

// ----------------------------------------------------------------------
// IPictureImpl_get_Width
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_Width(LPPICTURE iface,
				OLE_XSIZE_HIMETRIC * pWidth)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_Width()", This));

    // Sanity check...
    if (pWidth == 0)
	return E_POINTER;

    // Return the width...
    return This->pStrat->get_Width(pWidth);
}

// ----------------------------------------------------------------------
// IPictureImpl_get_Height
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_Height(LPPICTURE iface,
				OLE_YSIZE_HIMETRIC * pHeight)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_Height()", This));

    // Sanity check...
    if (pHeight == 0)
	return E_POINTER;

    // Return the height...
    return This->pStrat->get_Height(pHeight);
}

// ----------------------------------------------------------------------
// IPictureImpl_Render
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_Render(LPPICTURE iface,
				HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				LPCRECT pRcWBounds)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->Render()", This));

    // Delegate...
    return This->pStrat->Render(hdc, x, y, cx, cy, xSrc, ySrc, cxSrc, cySrc, pRcWBounds);
}

// ----------------------------------------------------------------------
// IPictureImpl_set_hPal
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_set_hPal(LPPICTURE iface,
				OLE_HANDLE hPal)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->set_hPal()", This));

    //Delegate...
    return This->pStrat->set_hPal(hPal);
}

// ----------------------------------------------------------------------
// IPictureImpl_get_CurDC
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_CurDC(LPPICTURE iface,
				HDC * phDC)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_CurDC()", This));

    // Sanity check
    if (phDC == 0)
	return E_POINTER;

    // Return current hDC
    *phDC = This->hCurDC;

    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureImpl_SelectPicture
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_SelectPicture(LPPICTURE iface,
				HDC hDCIn, HDC * phDCOut, OLE_HANDLE * phBmpOut)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->SelectPicture() - Stub", This));

    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// IPictureImpl_get_KeepOriginalFormat
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_KeepOriginalFormat(LPPICTURE iface,
				BOOL * pKeep)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_KeepOriginalFormat()", This));

    // Sanity check...
    if (pKeep == 0)
	return E_POINTER;

    // Return the current setting...
    *pKeep = This->fKeepOriginalFormat;

    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureImpl_put_KeepOriginalFormat
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_put_KeepOriginalFormat(LPPICTURE iface,
				BOOL Keep)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->put_KeepOriginalFormat()", This));

    // Save it...
    This->fKeepOriginalFormat = Keep;

    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureImpl_PictureChanged
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_PictureChanged(LPPICTURE iface)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->PictureChanged()", This));

    return S_OK;
}

// ----------------------------------------------------------------------
// IPictureImpl_SaveAsFile
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_SaveAsFile(LPPICTURE iface,
				LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->SaveAsFile()", This));

    //Delegate...
    return This->pStrat->SaveAsFile(pStream, fSaveMemCopy, pCbSize);
}

// ----------------------------------------------------------------------
// IPictureImpl_get_Attributes
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_Attributes(LPPICTURE iface,
				DWORD * pDwAttr)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_Attributes()", This));

    // Sanity check
    if (pDwAttr == 0)
	return E_POINTER;

    //Delegate...
    return This->pStrat->get_Attributes(pDwAttr);
}


// ----------------------------------------------------------------------
// IPictureImpl_IDispatch_QueryInterface
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_QueryInterface(LPDISPATCH iface,
				REFIID riid, LPVOID * obj)
{
    _ICOM_THIS_From_IDispatch(IPicture, iface);

    return IPictureImpl_QueryInterface(This, riid, obj);
}

// ----------------------------------------------------------------------
// IPictureImpl_IDispatch_AddRef
// ----------------------------------------------------------------------
static ULONG   WINAPI IPictureImpl_IDispatch_AddRef(LPDISPATCH iface)
{
    _ICOM_THIS_From_IDispatch(IPicture, iface);

    return IPictureImpl_AddRef(This);
}

// ----------------------------------------------------------------------
// IPictureImpl_IDispatch_Release
// ----------------------------------------------------------------------
static ULONG   WINAPI IPictureImpl_IDispatch_Release(LPDISPATCH iface)
{
    _ICOM_THIS_From_IDispatch(IPicture, iface);

    return IPictureImpl_Release(This);
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_GetTypeInfoCount(LPDISPATCH iface, 
				unsigned int* pctinfo)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_GetTypeInfo(LPDISPATCH  iface,
				UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_GetIDsOfNames(LPDISPATCH iface,
				REFIID riid, LPOLESTR* rgszNames, UINT cNames,
				LCID lcid, DISPID* rgDispId)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_Invoke(LPDISPATCH iface,
				DISPID dispIdMember, REFIID riid,
				LCID lcid, WORD wFlags,
				DISPPARAMS* pDispParams, VARIANT* pVarResult,
				EXCEPINFO* pExepInfo, UINT* puArgErr)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_QueryInterface(LPPERSISTSTREAM iface,
				REFIID riid, LPVOID * obj)
{
    _ICOM_THIS_From_IPersistStream(IPicture, iface);

    return IPictureImpl_QueryInterface(This, riid, obj);
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static ULONG   WINAPI IPictureImpl_IPersistStream_AddRef(LPPERSISTSTREAM iface)
{
    _ICOM_THIS_From_IPersistStream(IPicture, iface);

    return IPictureImpl_AddRef(This);
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static ULONG   WINAPI IPictureImpl_IPersistStream_Release(LPPERSISTSTREAM iface)
{
    _ICOM_THIS_From_IPersistStream(IPicture, iface);

    return IPictureImpl_Release(This);
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_GetClassID(LPPERSISTSTREAM iface,
				CLSID * pClassID)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_IsDirty(LPPERSISTSTREAM iface)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_Load(LPPERSISTSTREAM iface,
				LPSTREAM pLoadStream)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_Save(LPPERSISTSTREAM iface,
				LPSTREAM pOutStream, BOOL fClearDirty)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_GetSizeMax(LPPERSISTSTREAM iface,
				ULARGE_INTEGER * pcbSize)
{
    dprintf(("%s: Stub", __FUNCTION__));
    return E_NOTIMPL;
}


// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
//  OleLoadPicture
// ----------------------------------------------------------------------
HRESULT WIN32API OleLoadPicture(LPSTREAM lpstream, LONG lSize, BOOL fRunmode,
    REFIID riid, LPVOID * lplpvObj)
{
    dprintf(("OLEAUT32: OleLoadPicture - stub"));
    return S_OK;
}

// ----------------------------------------------------------------------
// OleCreatePictureIndirect
// ----------------------------------------------------------------------
HRESULT WIN32API OleCreatePictureIndirect(LPPICTDESC lpPictDesc, REFIID riid,
    BOOL fOwn, LPVOID * lplpvObj)
{
    IPictureImpl *	newPicture;
    HRESULT		hr;

    dprintf(("OLEAUT32: OleCreatePictureIndirect"));

    // Validate input pointer...
    if (lplpvObj == 0)
    	return E_POINTER;

    *lplpvObj = 0;

    // Create new picture object...
    newPicture = IPictureImpl_Constructor(lpPictDesc, fOwn);

    if (newPicture == 0)
	return E_OUTOFMEMORY;

    // Grab and return the requested interface...
    hr = IPictureImpl_QueryInterface((LPPICTURE)newPicture, riid, lplpvObj);

    // We should now have two references :-)
    // One from the constructor & one from QueryInterface
    // Release one...
    IPictureImpl_Release((LPPICTURE)newPicture);

    return hr;
}

// ----------------------------------------------------------------------
// OleLoadPictureFile
// ----------------------------------------------------------------------
HRESULT WIN32API OleLoadPictureFile(VARIANT varFileName, LPDISPATCH* lplpdispPicture)
{
    dprintf(("OLEAUT32: OleLoadPictureFile - stub"));
    return S_OK;
}

// ----------------------------------------------------------------------
// OleSavePictureFile
// ----------------------------------------------------------------------
HRESULT WIN32API OleSavePictureFile(LPDISPATCH lpdispPicture,
    BSTR bstrFileName)
{
    dprintf(("OLEAUT32: OleSavePictureFile - stub"));
    return S_OK;
}

// ----------------------------------------------------------------------
// OleLoadPicturePath
// ----------------------------------------------------------------------
HRESULT WIN32API OleLoadPicturePath
   (LPOLESTR  		szURLorPath,
    LPUNKNOWN 		punkCaller,
    DWORD     		dwReserved,
    OLE_COLOR 		clrReserved,
    REFIID    		riid,
    LPVOID *  		ppvRet )
{
    dprintf(("OLEAUT32: OleLoadPicturePath - stub"));
    return S_OK;
}


