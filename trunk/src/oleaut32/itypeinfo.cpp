/* $Id: itypeinfo.cpp,v 1.5 2001-01-18 18:12:20 sandervl Exp $ */
/* 
 * ITypeInfo interface
 * 
 * 5/12/99
 * 
 * Copyright 1999 David J. Raison
 * Some portions based on original work on Wine
 *   Copyright 1997 Marcus Meissner
 	       1999 Rein Klazes
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
#include "oleaut32.h"
#include "olectl.h"
#include "itypelib.h"

HRESULT WINAPI LoadRegTypeLib(REFGUID rguid,
		    WORD wVerMajor, WORD wVerMinor, LCID lcid, ITypeLib * * ppTLib);
HRESULT WINAPI LoadTypeLib( LPCOLESTR szFile, ITypeLib * * ppTLib);

// ======================================================================
// Local Data
// ======================================================================

static ICOM_VTABLE(ITypeInfo2) ITypeInfo_Vtable =
{
    ITypeInfoImpl_QueryInterface,
    ITypeInfoImpl_AddRef,
    ITypeInfoImpl_Release,
    ITypeInfoImpl_GetTypeAttr,
    ITypeInfoImpl_GetTypeComp,
    ITypeInfoImpl_GetFuncDesc,
    ITypeInfoImpl_GetVarDesc,
    ITypeInfoImpl_GetNames,
    ITypeInfoImpl_GetRefTypeOfImplType,
    ITypeInfoImpl_GetImplTypeFlags,
    ITypeInfoImpl_GetIDsOfNames,
    ITypeInfoImpl_Invoke,
    ITypeInfoImpl_GetDocumentation,
    ITypeInfoImpl_GetDllEntry,
    ITypeInfoImpl_GetRefTypeInfo,
    ITypeInfoImpl_AddressOfMember,
    ITypeInfoImpl_CreateInstance,
    ITypeInfoImpl_GetMops,
    ITypeInfoImpl_GetContainingTypeLib,
    ITypeInfoImpl_ReleaseTypeAttr,
    ITypeInfoImpl_ReleaseFuncDesc,
    ITypeInfoImpl_ReleaseVarDesc,
    ITypeInfo2Impl_GetTypeKind,
    ITypeInfo2Impl_GetTypeFlags,
    ITypeInfo2Impl_GetFuncIndexOfMemId,
    ITypeInfo2Impl_GetVarIndexOfMemId,
    ITypeInfo2Impl_GetCustData,
    ITypeInfo2Impl_GetFuncCustData,
    ITypeInfo2Impl_GetParamCustData,
    ITypeInfo2Impl_GetVarCustData,
    ITypeInfo2Impl_GetImplTypeCustData,
    ITypeInfo2Impl_GetDocumentation2,
    ITypeInfo2Impl_GetAllCustData,
    ITypeInfo2Impl_GetAllFuncCustData,
    ITypeInfo2Impl_GetAllParamCustData,
    ITypeInfo2Impl_GetAllVarCustData,
    ITypeInfo2Impl_GetAllImplTypeCustData,
};

// ======================================================================
// Local Methods
// ======================================================================

// ----------------------------------------------------------------------
// ITypeInfoImpl_Constructor
// ----------------------------------------------------------------------
ITypeInfoImpl * ITypeInfoImpl_Constructor()
{
    ITypeInfoImpl *	pNew;

//    dprintf(("OLEAUT32: ITypeInfo()->Constructor()"));

    pNew = new ITypeInfoImpl;

    // Init vft.
    pNew->lpvtbl = &ITypeInfo_Vtable;
    pNew->ref = 1;

    return pNew;
}

// ----------------------------------------------------------------------
// ITypeInfoImpl_Destructor
// ----------------------------------------------------------------------
void ITypeInfoImpl_Destructor(ITypeInfoImpl * This)
{
    dprintf(("OLEAUT32: ITypeInfo(%p)->Destructor()", This));

//    delete This;		// FIXME! - release all substructures, etc.
}

// ----------------------------------------------------------------------
// ITypeInfoImpl_QueryInterface
// 
// This object supports IUnknown, ITypeInfo, ITypeInfo2
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_QueryInterface(ITypeInfo2 * iface,
				REFIID riid, VOID **ppvObject)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->QueryInterface()", This));

    // Sanity check...
    if ((This == 0) || (ppvObject == 0))
	return E_INVALIDARG;

    // Initialise return jic...
    *ppvObject = 0;

    // Go find the correct interface...
    if (IsEqualIID(&IID_IUnknown, riid))
    {
	*ppvObject = &(This->lpvtbl);
	dprintf(("          ->IUnknown"));
    }
    else if (IsEqualIID(&IID_ITypeInfo, riid))
    {
	*ppvObject = &(This->lpvtbl);
	dprintf(("          ->ITypeInfo"));
    }
    else if (IsEqualIID(&IID_ITypeInfo2, riid))
    {
	*ppvObject = &(This->lpvtbl);
	dprintf(("          ->ITypeInfo2"));
    }
    else
    {
	char	tmp[50];

	WINE_StringFromCLSID(riid, tmp);

	dprintf(("          ->E_NOINTERFACE(%s)", tmp));
	return E_NOINTERFACE; 
    }

    // Query Interface always increases the reference count by one...
    ITypeInfoImpl_AddRef((ITypeInfo2 *)This);

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_AddRef
// ----------------------------------------------------------------------
ULONG   WIN32API ITypeInfoImpl_AddRef(ITypeInfo2 * iface)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->AddRef()", This));

    return ++(This->ref);
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_Release
// ----------------------------------------------------------------------
ULONG   WIN32API ITypeInfoImpl_Release(ITypeInfo2 * iface)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->Release()\n", This));

    if (--(This->ref) == 0)
    {
	ITypeInfoImpl_Destructor(This);
	return 0;
    }
    return This->ref;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_GetTypeAttr
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetTypeAttr(ITypeInfo2 * iface,
				LPTYPEATTR  *ppTypeAttr)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetTypeAttr()\n", This));

    TYPEATTR *	pTypeAttr = (TYPEATTR *)HeapAlloc(GetProcessHeap(), 0, sizeof(TYPEATTR));

    *pTypeAttr = This->TypeAttr;

    *ppTypeAttr = pTypeAttr;
    return S_OK;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetTypeComp(ITypeInfo2 * iface,
				ITypeComp  * *ppTComp)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetTypeComp() - STUB\n", This));

    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_GetFuncDesc
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetFuncDesc(ITypeInfo2 * iface,
				UINT index, LPFUNCDESC  *ppFuncDesc)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetFuncDesc()\n", This));

    if (This->pFunctions.Count() < index) 
	return E_INVALIDARG;

    FUNCDESC *	pFuncDesc = (FUNCDESC *)HeapAlloc(GetProcessHeap(), 0, sizeof(FUNCDESC));
    *pFuncDesc = This->pFunctions[index]->funcdesc;
    *ppFuncDesc = pFuncDesc;

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_GetVarDesc
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetVarDesc(ITypeInfo2 * iface,
				UINT index, LPVARDESC  *ppVarDesc)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetVarDesc()\n", This));

    if (This->pVariables.Count() < index) 
	return E_INVALIDARG;

    VARDESC *	pVarDesc = (VARDESC *)HeapAlloc(GetProcessHeap(), 0, sizeof(VARDESC));
    *pVarDesc = This->pVariables[index]->vardesc;
    *ppVarDesc = pVarDesc;

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_GetNames
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetNames(ITypeInfo2 * iface,
				MEMBERID memid, BSTR  *rgBstrNames,
				UINT cMaxNames, UINT  *pcNames)
{
    ICOM_THIS(ITypeInfoImpl, iface);
    int		ii;

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetNames()\n", This));

    // Sanity check...
    if (pcNames == 0)
	return E_INVALIDARG;

    // Search the functions...
    oListIter<TLBFuncDesc *>	itrFunc(This->pFunctions);

    for (itrFunc.MoveStart(); itrFunc.IsValid(); itrFunc.MoveNext())
    {
	if (itrFunc.Element()->funcdesc.memid == memid)
	{
	    for (ii = 0; ii < cMaxNames && ii <= itrFunc.Element()->funcdesc.cParams; ii++)
	    {
		if (!ii)
		    rgBstrNames[ii] = DupAtoBstr(
			    itrFunc.Element()->szName);
		else
		    rgBstrNames[ii] = DupAtoBstr(
			    itrFunc.Element()->pParamDesc[ii - 1].szName);
	    }
	    *pcNames = ii;
	    return S_OK;
	}

    }

    // Search the variables...
    oListIter<TLBVarDesc *>	itrVar(This->pVariables);
    for (itrVar.MoveStart(); itrVar.IsValid(); itrVar.MoveNext())
    {
	if (itrVar.Element()->vardesc.memid == memid)
	{
	    rgBstrNames[0] = DupAtoBstr(itrVar.Element()->szName);
	    *pcNames = 1;
	    return S_OK;
	}
    }

    // Search interfaces...
    if (This->TypeAttr.typekind == TKIND_INTERFACE)
    {
	oListIter<TLBRefType *>	itrRef(This->pImplements);
	for (itrRef.MoveStart(); itrRef.IsValid(); itrRef.MoveNext())
	{
	    // recursive search
	    ITypeInfo2 *	pTInfo;
	    HRESULT		result;

	    result = This->lpvtbl->fnGetRefTypeInfo(iface, itrRef.Element()->reference, (ITypeInfo **)&pTInfo);
	    if(SUCCEEDED(result))
	    {
		result = ICOM_VTBL(pTInfo)->fnGetNames(
			    pTInfo, memid, rgBstrNames, cMaxNames, pcNames);
		ICOM_VTBL(pTInfo)->fnRelease(pTInfo);
		return result;
	    }
	    dprintf((" could not search inherited interface 0x%x", itrRef.Element()->reference));
	}
    }

    *pcNames = 0; // jic

    dprintf((" :-( no names found"));
    return TYPE_E_ELEMENTNOTFOUND;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_GetRefTypeOfImplType
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetRefTypeOfImplType(ITypeInfo2 * iface,
				UINT index, HREFTYPE  *pRefType)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetRefTypeOfImplType()\n", This));

    if (This->pImplements.Count() < index)
	return TYPE_E_ELEMENTNOTFOUND;

    *pRefType = This->pImplements[index]->reference;
    return S_OK;
}


// ----------------------------------------------------------------------
//  ITypeInfoImpl_GetImplTypeFlags
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetImplTypeFlags(ITypeInfo2 * iface,
				UINT index, INT  *pImplTypeFlags)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetImplTypeFlags()\n", This));

    if (This->pImplements.Count() < index)
	return TYPE_E_ELEMENTNOTFOUND;

    *pImplTypeFlags = This->pImplements[index]->flags;
    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_GetIDsOfNames
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetIDsOfNames(ITypeInfo2 * iface,
				LPOLESTR  *rgszNames, UINT cNames, MEMBERID  *pMemId)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetIDsOfNames()\n", This));

    PCHAR		aszName = HEAP_strdupWtoA( GetProcessHeap(), 0, rgszNames[0]);
    HRESULT		rc = S_OK;
    int			ii;
    int			jj;

    if (cNames == 0)
	return rc;

    // Search the functions...
    oListIter<TLBFuncDesc *>	itrFunc(This->pFunctions);

    for (itrFunc.MoveStart(); itrFunc.IsValid(); itrFunc.MoveNext())
    {
	// Looking for a method of matching name...
	if (!strcmp(itrFunc.Element()->szName, aszName))
	{
	    pMemId[0] = itrFunc.Element()->funcdesc.memid;

	    for (ii = 1; ii < cNames; ii++)
	    {
	    	// Now match the methods (any order)
		PCHAR		aszPar = HEAP_strdupWtoA( GetProcessHeap(), 0, rgszNames[ii]);
		for (jj = 0; jj < itrFunc.Element()->funcdesc.cParams; jj++)
		{
                    if (!strcmp(aszPar, itrFunc.Element()->pParamDesc[jj].szName))
                    {
			pMemId[ii] = jj;
			break;
		    }
		}
		// Oops - blew it...
		if (jj == itrFunc.Element()->funcdesc.cParams) 
		    rc = DISP_E_UNKNOWNNAME;
		HeapFree(GetProcessHeap(), 0, aszPar);

	    }
	    HeapFree(GetProcessHeap(), 0, aszName);
	    return rc;
	}

    }

    // Search the variables...
    oListIter<TLBVarDesc *>	itrVar(This->pVariables);
    for (itrVar.MoveStart(); itrVar.IsValid(); itrVar.MoveNext())
    {
	if (!strcmp(itrVar.Element()->szName, aszName))
	{
	    pMemId[0] = itrVar.Element()->vardesc.memid;
	    HeapFree(GetProcessHeap(), 0, aszName);
	    return rc;
	}
    }

    HeapFree(GetProcessHeap(), 0, aszName);

    // Search interfaces...
    if (This->TypeAttr.typekind == TKIND_INTERFACE)
    {
	oListIter<TLBRefType *>	itrRef(This->pImplements);
	for (itrRef.MoveStart(); itrRef.IsValid(); itrRef.MoveNext())
	{
	    // recursive search
	    ITypeInfo *	pTInfo;

	    rc = This->lpvtbl->fnGetRefTypeInfo(iface, itrRef.Element()->reference, &pTInfo);
	    if(SUCCEEDED(rc))
	    {
		rc = ICOM_VTBL(pTInfo)->fnGetIDsOfNames(pTInfo, rgszNames, cNames, pMemId );
		ICOM_VTBL(pTInfo)->fnRelease(pTInfo);
		return rc;
	    }
	    dprintf((" could not search inherited interface 0x%x", itrRef.Element()->reference));
	}
    }
    return DISP_E_UNKNOWNNAME;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_Invoke(ITypeInfo2 * iface,
				VOID  *pIUnk, MEMBERID memid, UINT16 dwFlags,
				DISPPARAMS  *pDispParams, VARIANT *pVarResult,
				EXCEPINFO  *pExcepInfo, UINT  *pArgErr)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->Invoke() STUB!\n", This));

    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_GetDocumentation
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetDocumentation(ITypeInfo2 * iface,
				MEMBERID memid, BSTR  *pBstrName, BSTR  *pBstrDocString,
				DWORD  *pdwHelpContext, BSTR  *pBstrHelpFile)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetDocumentation()\n", This));

    /* documentation for the typeinfo */
    if (memid == MEMBERID_NIL)
    {
        if (pBstrName)
            *pBstrName = DupAtoBstr(This->szName);
        if (pBstrDocString)
            *pBstrDocString = DupAtoBstr(This->szDocString);
        if (pdwHelpContext)
            *pdwHelpContext = This->lHelpContext;
        if (pBstrHelpFile)
            *pBstrHelpFile = DupAtoBstr(This->szDocString);/* FIXME */

        return S_OK;
    }

    // Search functions...
    oListIter<TLBFuncDesc *>	itrFunc(This->pFunctions);
    for (itrFunc.MoveStart(); itrFunc.IsValid(); itrFunc.MoveNext())
    {
	if (itrFunc.Element()->funcdesc.memid == memid)
	{
	    if (pBstrName)
		*pBstrName = DupAtoBstr(itrFunc.Element()->szName);
	    if (pBstrDocString)
		*pBstrDocString = DupAtoBstr(itrFunc.Element()->szHelpString);
	    if (pdwHelpContext)
		*pdwHelpContext = itrFunc.Element()->lHelpStringContext;
	    if (pBstrHelpFile)
		*pBstrHelpFile = DupAtoBstr(itrFunc.Element()->szHelpString);/* FIXME */
	    return S_OK;
	}
    }

    // Search variables...
    oListIter<TLBVarDesc *>	itrVar(This->pVariables);
    for (itrVar.MoveStart(); itrVar.IsValid(); itrVar.MoveNext())
    {
	if (itrVar.Element()->vardesc.memid == memid)
	{
	    if (pBstrName)
		*pBstrName = DupAtoBstr(itrVar.Element()->szName);
	    if (pBstrDocString)
		*pBstrDocString = DupAtoBstr(itrVar.Element()->szHelpString);
	    if (pdwHelpContext)
		*pdwHelpContext = itrVar.Element()->lHelpStringContext;
	    if (pBstrHelpFile)
		*pBstrHelpFile = DupAtoBstr(itrVar.Element()->szHelpString);/* FIXME */
	    return S_OK;
	}
    }

    return TYPE_E_ELEMENTNOTFOUND;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetDllEntry(ITypeInfo2 * iface,
				MEMBERID memid, INVOKEKIND invKind, BSTR  *pBstrDllName,
				BSTR  *pBstrName, WORD  *pwOrdinal)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetDllEntry() - STUB!\n", This));

    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetRefTypeInfo(ITypeInfo2 * iface,
				HREFTYPE hRefType, ITypeInfo  * *ppTInfo)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetRefTypeInfo()\n", This));

    HRESULT rc;
    if (HREFTYPE_INTHISFILE(hRefType))
    {
        ITypeLib *	pTLib;
        int		index;
        rc = This->lpvtbl->fnGetContainingTypeLib(iface, &pTLib, (UINT*)&index);
        if (SUCCEEDED(rc))
        {
            rc = ICOM_VTBL(pTLib)->fnGetTypeInfo(pTLib, HREFTYPE_INDEX(hRefType), ppTInfo);
            ICOM_VTBL(pTLib)->fnRelease(pTLib );
        }
        return rc;
    }

    /* imported type lib */
    oListIter<TLBRefType *>	itrRef(This->pImplements);
    for (itrRef.MoveStart(); itrRef.IsValid(); itrRef.MoveNext())
    {
	if (itrRef.Element()->reference == hRefType)
	{
	    TLBImpLib *		pImpLib;
	    ITypeLibImpl *	pTypeLib;

	    pTypeLib = itrRef.Element()->pImpTLInfo->pImpTypeLib;
	    if (pTypeLib)
	    {
		return pTypeLib->lpvtbl->fnGetTypeInfoOfGuid(
			(ITypeLib2 *)pTypeLib, &itrRef.Element()->guid, ppTInfo);
	    }
	    rc = LoadRegTypeLib( &itrRef.Element()->pImpTLInfo->guid,
		    0,0,0, /* FIXME */
		    (LPTYPELIB *)&pTypeLib);
	    if (!SUCCEEDED(rc))
	    {
		BSTR libnam = DupAtoBstr(itrRef.Element()->pImpTLInfo->name);
		rc = LoadTypeLib(libnam, (LPTYPELIB *)&pTypeLib);
		SysFreeString(libnam);
	    }
	    if (SUCCEEDED(rc))
	    {
		rc = pTypeLib->lpvtbl->fnGetTypeInfoOfGuid(
			(ITypeLib2 *)pTypeLib, &itrRef.Element()->guid, ppTInfo);
		itrRef.Element()->pImpTLInfo->pImpTypeLib = pTypeLib;
	    }
	    return rc;
	}
    }

    return TYPE_E_ELEMENTNOTFOUND; /* FIXME : correct? */
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_AddressOfMember(ITypeInfo2 * iface,
				MEMBERID memid, INVOKEKIND invKind, PVOID *ppv)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->AddressOfMember()\n", This));

    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_CreateInstance(ITypeInfo2 * iface,
				IUnknown *pUnk, REFIID riid, VOID  * *ppvObj)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->CreateInstance()\n", This));

    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetMops(ITypeInfo2 * iface,
				MEMBERID memid, BSTR  *pBstrMops)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetMops()\n", This));

    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_GetContainingTypeLib
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_GetContainingTypeLib(ITypeInfo2 * iface,
				ITypeLib  * *ppTLib, UINT  *pIndex)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetContainingTypeLib()\n", This));

    ICOM_VTBL(*ppTLib)->fnAddRef(*ppTLib);

    *ppTLib = (LPTYPELIB )(This->pTypeLib);
    *pIndex = This->index;

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_ReleaseTypeAttr
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_ReleaseTypeAttr(ITypeInfo2 * iface,
				TYPEATTR *pTypeAttr)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->ReleaseTypeAttr()\n", This));

    HeapFree(GetProcessHeap(), 0, pTypeAttr);

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_ReleaseFuncDesc
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_ReleaseFuncDesc(ITypeInfo2 * iface,
				FUNCDESC *pFuncDesc)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->ReleaseFuncDesc()\n", This));

    HeapFree(GetProcessHeap(), 0, pFuncDesc);

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfoImpl_ReleaseVarDesc
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfoImpl_ReleaseVarDesc(ITypeInfo2 * iface,
				VARDESC *pVarDesc)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->ReleaseVarDesc()\n", This));

    HeapFree(GetProcessHeap(), 0, pVarDesc);

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetTypeKind
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetTypeKind(ITypeInfo2 * iface,
				TYPEKIND *pTypeKind)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetTypeKind()\n", This));

    *pTypeKind = This->TypeAttr.typekind;
    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetTypeFlags
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetTypeFlags(ITypeInfo2 * iface,
				UINT *pTypeFlags)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetTypeFlags()\n", This));

    *pTypeFlags = This->TypeAttr.wTypeFlags;

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetFuncIndexOfMemId
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetFuncIndexOfMemId(ITypeInfo2 * iface,
				MEMBERID memid, INVOKEKIND invKind, UINT *pFuncIndex)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetFuncIndexOfMemId()\n", This));

    int		index = 0;
    oListIter<TLBFuncDesc *>	itrFunc(This->pFunctions);

    for (itrFunc.MoveStart(); itrFunc.IsValid(); itrFunc.MoveNext())
    {
	if ((itrFunc.Element()->funcdesc.memid == memid)
	    && (itrFunc.Element()->funcdesc.invkind == invKind))
	{
	    *pFuncIndex = index;
	    return S_OK;
	}
	index += 1;
    }
    *pFuncIndex = 0;
    return E_INVALIDARG;
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetVarIndexOfMemId
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetVarIndexOfMemId(ITypeInfo2 * iface,
				MEMBERID memid, UINT *pVarIndex)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetVarIndexOfMemId()\n", This));

    int		index = 0;
    oListIter<TLBVarDesc *>	itrVar(This->pVariables);

    for (itrVar.MoveStart(); itrVar.IsValid(); itrVar.MoveNext())
    {
	if (itrVar.Element()->vardesc.memid == memid)
	{
	    *pVarIndex = index;
	    return S_OK;
	}
	index += 1;
    }
    *pVarIndex = 0;
    return E_INVALIDARG;
}

// ----------------------------------------------------------------------
// FindCustData
// ----------------------------------------------------------------------
static HRESULT FindCustData(oList<TLBCustData *> & list, REFGUID guid, VARIANT *pVarVal)
{
    oListIter<TLBCustData *>	itrCust(list);

    for (itrCust.MoveStart(); itrCust.IsValid(); itrCust.MoveNext())
    {
	if (IsEqualGUID(&itrCust.Element()->guid, guid))
	{
	    VariantInit( pVarVal);
	    VariantCopy( pVarVal, &itrCust.Element()->data);
	    return S_OK;
	}
    }

    return E_INVALIDARG;
}

// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetCustData(ITypeInfo2 * iface,
				REFGUID guid, VARIANT *pVarVal)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetCustData()\n", This));

    return FindCustData(This->pCustData, guid, pVarVal);
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetFuncCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetFuncCustData(ITypeInfo2 * iface,
				UINT index, REFGUID guid, VARIANT *pVarVal)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetFuncCustData()\n", This));

    if (This->pFunctions.Count() < index)
	return E_INVALIDARG;

    return FindCustData(This->pFunctions[index]->pCustData, guid, pVarVal);
}


// ----------------------------------------------------------------------
//  ITypeInfo2Impl_GetParamCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetParamCustData(ITypeInfo2 * iface,
				UINT indexFunc, UINT indexParam, REFGUID guid, VARIANT *pVarVal)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetParamCustData()\n", This));

    TLBFuncDesc *	pFuncDesc;

    if (This->pFunctions.Count() < indexFunc)
	return E_INVALIDARG;

    pFuncDesc = This->pFunctions[indexFunc];

    if (pFuncDesc->funcdesc.cParams < indexParam)
	return E_INVALIDARG;

    return FindCustData(pFuncDesc->pParamDesc[indexParam].pCustData, guid, pVarVal);
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetVarCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetVarCustData(ITypeInfo2 * iface,
				UINT index, REFGUID guid, VARIANT *pVarVal)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetVarCustData()\n", This));

    if (This->pVariables.Count() < index)
	return E_INVALIDARG;

    return FindCustData(This->pVariables[index]->pCustData, guid, pVarVal);
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetImplTypeCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetImplTypeCustData(ITypeInfo2 * iface,
				UINT index, REFGUID guid, VARIANT *pVarVal)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetImplTypeCustData()\n", This));

    if (This->pImplements.Count() < index)
	return E_INVALIDARG;

    return FindCustData(This->pImplements[index]->pCustData, guid, pVarVal);
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetDocumentation2
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetDocumentation2(ITypeInfo2 * iface,
				MEMBERID memid, LCID lcid, BSTR *pbstrHelpString,
				DWORD *pdwHelpStringContext, BSTR *pbstrHelpStringDll)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetDocumentation2()\n", This));

    // Typeinfo itself...
    if (memid == MEMBERID_NIL)
    {
        if (pbstrHelpString)
            *pbstrHelpString = DupAtoBstr(This->szName);
        if (pdwHelpStringContext)
            *pdwHelpStringContext = This->lHelpStringContext;
        if (pbstrHelpStringDll)
            *pbstrHelpStringDll = DupAtoBstr(
		    ((ITypeLibImpl *)This->pTypeLib)->szHelpStringDll);/* FIXME */
        return S_OK;
    }

    // Search functions...
    oListIter<TLBFuncDesc *>	itrFunc(This->pFunctions);
    for (itrFunc.MoveStart(); itrFunc.IsValid(); itrFunc.MoveNext())
    {
	if (itrFunc.Element()->funcdesc.memid == memid)
	{
            if (pbstrHelpString)
                *pbstrHelpString = DupAtoBstr(itrFunc.Element()->szHelpString);
            if (pdwHelpStringContext)
                *pdwHelpStringContext = itrFunc.Element()->lHelpStringContext;
            if (pbstrHelpStringDll)
                *pbstrHelpStringDll = DupAtoBstr(
			((ITypeLibImpl *)This->pTypeLib)->szHelpStringDll);/* FIXME */
        return S_OK;
        }
    }

    // Search variables...
    oListIter<TLBVarDesc *>	itrVar(This->pVariables);
    for (itrVar.MoveStart(); itrVar.IsValid(); itrVar.MoveNext())
    {
	if (itrVar.Element()->vardesc.memid == memid)
	{
             if (pbstrHelpString)
                *pbstrHelpString = DupAtoBstr(itrVar.Element()->szHelpString);
            if (pdwHelpStringContext)
                *pdwHelpStringContext = itrVar.Element()->lHelpStringContext;
            if (pbstrHelpStringDll)
                *pbstrHelpStringDll = DupAtoBstr(
			((ITypeLibImpl *)This->pTypeLib)->szHelpStringDll);/* FIXME */
            return S_OK;
        }
    }
    return TYPE_E_ELEMENTNOTFOUND;
}

// ----------------------------------------------------------------------
// GetAllCustData
// ----------------------------------------------------------------------
static HRESULT GetAllCustData(oList<TLBCustData *> & list, CUSTDATA *pCustData)
{
    if (list.Count() > 0)
    {
	pCustData->prgCustData = (CUSTDATAITEM *)
		    HeapAlloc(GetProcessHeap(), 0, list.Count() * sizeof(CUSTDATAITEM));

	if (!pCustData->prgCustData)
	{
	    dprintf(("OLEAUT32: GetAllCustData: E_OUTOFMEMORY"));
	    return E_OUTOFMEMORY;
	}

	int		index = 0;
	oListIter<TLBCustData *>	itrCust(list);

	for (itrCust.MoveStart(); itrCust.IsValid(); itrCust.MoveNext())
	{
	    pCustData->prgCustData[index].guid = itrCust.Element()->guid;
	    VariantCopy( &pCustData->prgCustData[index].varValue, &itrCust.Element()->data );
	    index += 1;
	}
    }

    return S_OK;
}

// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetAllCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetAllCustData(ITypeInfo2 * iface,
				CUSTDATA *pCustData)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetAllCustData()\n", This));

    return GetAllCustData(This->pCustData, pCustData);
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetAllFuncCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetAllFuncCustData(ITypeInfo2 * iface,
				UINT index, CUSTDATA *pCustData)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetAllFuncCustData()\n", This));

    if (This->pFunctions.Count() < index)
	return E_INVALIDARG;

    return GetAllCustData(This->pFunctions[index]->pCustData, pCustData);
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetAllParamCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetAllParamCustData(ITypeInfo2 * iface,
				UINT indexFunc, UINT indexParam, CUSTDATA *pCustData)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetAllParamCustData()\n", This));

    TLBFuncDesc *	pFuncDesc;

    if (This->pFunctions.Count() < indexFunc)
	return E_INVALIDARG;

    pFuncDesc = This->pFunctions[indexFunc];

    if (pFuncDesc->funcdesc.cParams < indexParam)
	return E_INVALIDARG;

    return GetAllCustData(pFuncDesc->pParamDesc[indexParam].pCustData, pCustData);
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetAllVarCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetAllVarCustData(ITypeInfo2 * iface,
				UINT index, CUSTDATA *pCustData)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetAllVarCustData()\n", This));

    if (This->pVariables.Count() < index)
	return E_INVALIDARG;

    return GetAllCustData(This->pVariables[index]->pCustData,  pCustData);
}


// ----------------------------------------------------------------------
// ITypeInfo2Impl_GetAllImplTypeCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeInfo2Impl_GetAllImplTypeCustData(ITypeInfo2 * iface,
				UINT index, CUSTDATA *pCustData)
{
    ICOM_THIS(ITypeInfoImpl, iface);

    dprintf(("OLEAUT32: ITypeInfoImpl(%p)->GetAllImplTypeCustData()\n", This));

    if (This->pImplements.Count() < index)
	return E_INVALIDARG;

    return GetAllCustData(This->pImplements[index]->pCustData, pCustData);
}



