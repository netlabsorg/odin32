/* $Id: iPicture.cpp,v 1.1 1999-11-14 01:26:35 davidr Exp $ */
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
#include "ipicture.h"

typedef struct IPictureImpl {

    // IUnknown, IPictureImpl
    ICOM_VTABLE(IPicture) *     	lpvtbl1;

    // IDispatch, IPictureImplDisp
    ICOM_VTABLE(IDispatch) *		lpvtbl2;

    // IPersistStream
    ICOM_VTABLE(IPersistStream) *	lpvtbl3;

    ULONG		ref;		// Reference counter...

//    PICTDESC		description;

    short		sType;
    HANDLE		hPicture;
    HANDLE		hPal;
    BOOL		fKeepOriginalFormat;
    BOOL		fOwn;
    HDC			hdc;
    DWORD		lAttrib;

} IPictureImpl;

// Access macros
#define _ICOM_THIS(class, name) class* This = (class*)name;
#define _ICOM_THIS_From_IDispatch(class, name) class * This = (class*)(((char*)name)-sizeof(void*));
#define _ICOM_THIS_From_IPersistStream(class, name) class * This = (class*)(((char*)name)-2*sizeof(void*));

// ======================================================================
// Prototypes.
// ======================================================================

// ----------------------------------------------------------------------
// IPictureImpl
// ----------------------------------------------------------------------
static IPictureImpl *   IPictureImpl_Constructor(LPPICTDESC description, BOOL fOwn);
static void             IPictureImpl_Destructor(IPictureImpl * obj);

static ULONG   WIN32API	IPictureImpl_AddRef(LPPICTURE iface);
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
    IPictureImpl *	newObject;

    // Allocate a new object on the heap...
    if ((newObject = (IPictureImpl *)HeapAlloc(GetProcessHeap(), 0, sizeof(IPictureImpl))) == 0)
    	return 0;

    dprintf(("OLEAUT32: IPictureImpl(%p)->Contructor()\n", newObject));

    // Initialise the vft's
    newObject->lpvtbl1 = &IPictureImpl_VTable;
    newObject->lpvtbl2 = &IPictureImpl_IDispatch_VTable;
    newObject->lpvtbl3 = &IPictureImpl_IPersistStream_VTable;

    // Initial reference count of 1...
    newObject->ref = 1;
    newObject->fOwn = fOwn;

    // If a description is not passed then leave the picture uninitialised...
    if (description == 0)
	newObject->sType = PICTYPE_NONE;
    else
    {
	// Validate size...
	if (description->cbSizeofstruct < sizeof(PICTDESC))
	{
	    dprintf(("OLEAUT32: IPictureImpl(%p)->Constructor - Invalid cbSizeofstruct!", newObject));
	    return 0;
	}
	switch(description->picType)
	{
	    case PICTYPE_BITMAP:
		dprintf(("OLEAUT32: IPictureImpl(%p)->Constructor - Creating bitmap", newObject));

		newObject->sType = PICTYPE_BITMAP;

		// Allocate picture from bitmap...

//		newObject->description.u.bmp.hbitmap = description->u.bmp.hbitmap;
//		newObject->description.u.bmp.hpal = description->u.bmp.hpal;

		break;

	    case PICTYPE_METAFILE:
		dprintf(("OLEAUT32: IPictureImpl(%p)->Constructor - Creating metafile", newObject));

		newObject->sType = PICTYPE_METAFILE;

//		newObject->description.u.wmf.hmeta = description->u.wmf.hmeta;
//		newObject->description.u.wmf.xExt = description->u.wmf.xExt;
//		newObject->description.u.wmf.yExt = description->u.wmf.yExt;
		break;

	    case PICTYPE_ICON:
		dprintf(("OLEAUT32: IPictureImpl(%p)->Constructor - Creating icon", newObject));

		newObject->sType = PICTYPE_ICON;

//		newObject->description.u.icon.hicon = description->u.icon.hicon;
		break;

	    case PICTYPE_ENHMETAFILE:
		dprintf(("OLEAUT32: IPictureImpl(%p)->Constructor - Creating enh metafile", newObject));
		newObject->sType = PICTYPE_ENHMETAFILE;

//		newObject->description.u.emf.hemf = description->u.emf.hemf;
		break;

	    default:
	    	// assume uninitialised...
		dprintf(("OLEAUT32: IPictureImpl(%p)->Constructor - Creating uninitialised", newObject));
		newObject->sType = PICTYPE_NONE;
		break;
	}
    }

    return newObject;
}

// ----------------------------------------------------------------------
// IPictureImpl_QueryInterface
// ----------------------------------------------------------------------
static void             IPictureImpl_Destructor(IPictureImpl * obj)
{
    dprintf(("OLEAUT32: IPictureImpl(%p)->Destructor()\n", obj));

    HeapFree(GetProcessHeap(), 0, obj);
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
    *pHandle = This->hPicture;

    return S_OK;
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
    *phPal = This->hPal;

    return S_OK;
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
    if (This->sType == PICTYPE_NONE)
	*pType = PICTYPE_UNINITIALIZED;
    else
	*pType = This->sType;

    return S_OK;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_Width(LPPICTURE iface,
				OLE_XSIZE_HIMETRIC * pWidth)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_Width()", This));

    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_Height(LPPICTURE iface,
				OLE_YSIZE_HIMETRIC * pHeight)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_Height()", This));

    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_Render(LPPICTURE iface,
				HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				LPCRECT pRcWBounds)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->Render()", This));

    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_set_hPal(LPPICTURE iface,
				OLE_HANDLE hPal)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->set_hPal()", This));

    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_CurDC(LPPICTURE iface,
				HDC * phDC)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_CurDC()", This));

    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_SelectPicture(LPPICTURE iface,
				HDC hDCIn, HDC * phDCOut, OLE_HANDLE * phBmpOut)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->SelectPicture()", This));

    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
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
// 
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
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_PictureChanged(LPPICTURE iface)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->PictureChanged()", This));

    return S_OK;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_SaveAsFile(LPPICTURE iface,
				LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->SaveAsFile()", This));

    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WIN32API	IPictureImpl_get_Attributes(LPPICTURE iface,
				DWORD * pDwAttr)
{
    _ICOM_THIS(IPictureImpl, iface);

    dprintf(("OLEAUT32: IPictureImpl(%p)->get_Attributes()", This));

    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_QueryInterface(LPDISPATCH iface,
				REFIID riid, LPVOID * obj)
{
    _ICOM_THIS_From_IDispatch(IPicture, iface);

    return IPictureImpl_QueryInterface(This, riid, obj);
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static ULONG   WINAPI IPictureImpl_IDispatch_AddRef(LPDISPATCH iface)
{
    _ICOM_THIS_From_IDispatch(IPicture, iface);

    return IPictureImpl_AddRef(This);
}

// ----------------------------------------------------------------------
// 
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
    dprintf(("Stub"));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_GetTypeInfo(LPDISPATCH  iface,
				UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    dprintf(("Stub"));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IDispatch_GetIDsOfNames(LPDISPATCH iface,
				REFIID riid, LPOLESTR* rgszNames, UINT cNames,
				LCID lcid, DISPID* rgDispId)
{
    dprintf(("Stub"));
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
    dprintf(("Stub"));
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
    dprintf(("Stub"));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_IsDirty(LPPERSISTSTREAM iface)
{
    dprintf(("Stub"));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_Load(LPPERSISTSTREAM iface,
				LPSTREAM pLoadStream)
{
    dprintf(("Stub"));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_Save(LPPERSISTSTREAM iface,
				LPSTREAM pOutStream, BOOL fClearDirty)
{
    dprintf(("Stub"));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
static HRESULT WINAPI IPictureImpl_IPersistStream_GetSizeMax(LPPERSISTSTREAM iface,
				ULARGE_INTEGER * pcbSize)
{
    dprintf(("Stub"));
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


