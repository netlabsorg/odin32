/* $Id: itypelib.cpp,v 1.1 2001-08-10 19:24:49 sandervl Exp $ */
/* 
 * ITypelib interface
 * 
 * 1/12/99
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
#include "oList.h"	// linked list template
#include "itypelib.h"

// ======================================================================
// Local Data
// ======================================================================
static ICOM_VTABLE(ITypeLib2) ITypeLib_VTable =
{
    ITypeLibImpl_QueryInterface,
    ITypeLibImpl_AddRef,
    ITypeLibImpl_Release,
    ITypeLibImpl_GetTypeInfoCount,
    ITypeLibImpl_GetTypeInfo,
    ITypeLibImpl_GetTypeInfoType,
    ITypeLibImpl_GetTypeInfoOfGuid,
    ITypeLibImpl_GetLibAttr,
    ITypeLibImpl_GetTypeComp,
    ITypeLibImpl_GetDocumentation,
    ITypeLibImpl_IsName,
    ITypeLibImpl_FindName,
    ITypeLibImpl_ReleaseTLibAttr,
    ITypeLib2Impl_GetCustData,
    ITypeLib2Impl_GetLibStatistics,
    ITypeLib2Impl_GetDocumentation2,
    ITypeLib2Impl_GetAllCustData
};

// ======================================================================
// Local Methods
// ======================================================================

// ----------------------------------------------------------------------
// ITypeLibImpl_Constructor
// ----------------------------------------------------------------------
ITypeLibImpl * ITypeLibImpl_Constructor()
{
    ITypeLibImpl *	pNew;

//    dprintf(("OLEAUT32: ITypeLib()->Constructor()"));

    pNew = new ITypeLibImpl;

    // Initialise the vft
    pNew->lpvtbl = &ITypeLib_VTable;
    pNew->ref = 1;

    return pNew;
}

// ----------------------------------------------------------------------
// ITypeLibImpl_Destructor
// ----------------------------------------------------------------------
void ITypeLibImpl_Destructor(ITypeLibImpl * This)
{
    dprintf(("OLEAUT32: ITypeLibImpl(%p)->Destructor()", This));

    // TODO - need to relase all substructures etc.
//    delete This;
}


// ----------------------------------------------------------------------
// ITypeLibImpl_QueryInterface
//
// This object supports IUnknown, ITypeLib, ITypeLib2
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_QueryInterface(ITypeLib2 * iface,
				REFIID riid, VOID **ppvObject)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->QueryInterface()", This));

    // Sanity check...
    if ((This == 0) || (ppvObject == 0))
	return E_INVALIDARG;

    // Initialise return jic...
    *ppvObject = 0;

    // Go find the correct interface...
    if (IsEqualIID(&IID_IUnknown, riid))
    {
	dprintf(("          ->IUnknown"));
	*ppvObject = &(This->lpvtbl);
    }
    else if (IsEqualIID(&IID_ITypeLib, riid))
    {
	dprintf(("          ->ITypeLib"));
	*ppvObject = &(This->lpvtbl);
    }
    else if (IsEqualIID(&IID_ITypeLib2, riid))
    {
	dprintf(("          ->ITypeLib2"));
	*ppvObject = &(This->lpvtbl);
    }
    else
    {
	char	tmp[50];

	WINE_StringFromCLSID(riid, tmp);

	dprintf(("          ->E_NOINTERFACE(%s)", tmp));
	return E_NOINTERFACE; 
    }

    // Query Interface always increases the reference count by one...
    ITypeLibImpl_AddRef((ITypeLib2 *)This);

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeLibImpl_AddRef
// ----------------------------------------------------------------------
ULONG   WIN32API ITypeLibImpl_AddRef(ITypeLib2 * iface)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->AddRef()", This));

    return ++(This->ref);
}


// ----------------------------------------------------------------------
// ITypeLibImpl_Release
// ----------------------------------------------------------------------
ULONG   WIN32API ITypeLibImpl_Release(ITypeLib2 * iface)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->Release()\n", This));

    if (--(This->ref) == 0)
    {
	ITypeLibImpl_Destructor(This);
	return 0;
    }
    return This->ref;
}


// ----------------------------------------------------------------------
// ITypeLibImpl_GetTypeInfoCount
// ----------------------------------------------------------------------
UINT    WIN32API ITypeLibImpl_GetTypeInfoCount(ITypeLib2 * iface)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetTypeInfoCount()\n", This));

    return This->pTypeInfo.Count();
}


// ----------------------------------------------------------------------
// ITypeLibImpl_GetTypeInfo
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_GetTypeInfo(ITypeLib2 * iface,
				UINT index, ITypeInfo **ppTInfo)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetTypeInfo()\n", This));

    // Sanity check...
    if (ppTInfo == 0)
	return E_INVALIDARG;

    *ppTInfo = 0;

    if (index > (This->pTypeInfo.Count() - 1))
	return TYPE_E_ELEMENTNOTFOUND;

    // Fish out typeinfo (remembering to incr the ref count)...
    ITypeInfoImpl * pInfo = This->pTypeInfo[index];
    *ppTInfo = (ITypeInfo *)pInfo;
    pInfo->lpvtbl->fnAddRef((ITypeInfo2 *)*ppTInfo);

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeLibImpl_GetTypeInfoType
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_GetTypeInfoType(ITypeLib2 * iface,
				UINT index, TYPEKIND *pTKind)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetTypeInfoType()\n", This));

    // Sanity check...
    if (pTKind == 0)
	return E_INVALIDARG;

    if (index > (This->pTypeInfo.Count() - 1))
	return TYPE_E_ELEMENTNOTFOUND;

    // Fish out typeinfo & return typekind...
    ITypeInfoImpl * pInfo = This->pTypeInfo[index];
    *pTKind = pInfo->TypeAttr.typekind;

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeLibImpl_GetTypeInfoOfGuid
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_GetTypeInfoOfGuid(ITypeLib2 * iface,
				REFGUID guid, ITypeInfo **ppTInfo)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetTypeInfoOfGuid()\n", This));

    // Sanity check...
    if (ppTInfo == 0)
	return E_INVALIDARG;

    *ppTInfo = 0;

    oListIter<ITypeInfoImpl *>  itrTypeInfo(This->pTypeInfo);

    // loop thru them to find the one we want.
    for (itrTypeInfo.MoveStart(); itrTypeInfo.IsValid(); itrTypeInfo.MoveNext())
	{
	    if (IsEqualGUID(&itrTypeInfo.Element()->TypeAttr.guid, guid))
	    {
		// Fish out typeinfo...
		ITypeInfoImpl * pInfo = itrTypeInfo.Element();
		*ppTInfo = (ITypeInfo *)pInfo;
		pInfo->lpvtbl->fnAddRef((ITypeInfo2 *)*ppTInfo);

		return S_OK;
	    }
	}


    return TYPE_E_ELEMENTNOTFOUND;
}


// ----------------------------------------------------------------------
// ITypeLibImpl_GetLibAttr
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_GetLibAttr(ITypeLib2 * iface,
				LPTLIBATTR *ppTLibAttr)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetLibAttr()\n", This));

    TLIBATTR *	pTLibAttr = (TLIBATTR *)HeapAlloc(GetProcessHeap(), 0, sizeof(TLIBATTR));

    memcpy(pTLibAttr, &This->LibAttr, sizeof(TLIBATTR));

    *ppTLibAttr = pTLibAttr;
    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeLibImpl_GetTypeComp
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_GetTypeComp(ITypeLib2 * iface,
				ITypeComp **ppTComp)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetTypeComp()\n", This));

    // Sanity check...
    if (ppTComp == 0)
	return E_INVALIDARG;

    *ppTComp = 0;

    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// ITypeLibImpl_GetDocumentation
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_GetDocumentation(ITypeLib2 * iface,
				INT index, BSTR *pBstrName, BSTR *pBstrDocString,
				DWORD *plHelpContext, BSTR *pBstrHelpFile)
{
    HRESULT		rc;
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetDocumentation(idx = %i)\n", This, index));

    /* if index is -Ve this is a request for documentation for the typelib */
    if (index < 0)
    {
        if (pBstrName)
            *pBstrName = DupAtoBstr(This->szName);
        if (pBstrDocString)
            *pBstrDocString = DupAtoBstr(This->szDocString);
        if (plHelpContext)
            *plHelpContext = This->lHelpContext;
        if (pBstrHelpFile)
            *pBstrHelpFile = DupAtoBstr(This->szHelpFile);

	rc = S_OK;
    }
    else
    { // for specified typeinfo
	ITypeInfo2 *	pTInfo;

	rc = ITypeLibImpl_GetTypeInfo(iface, index, (ITypeInfo **)&pTInfo);

        if (SUCCEEDED(rc))
        {
	    // Delegate to typeinfo interface...
            rc = ITypeInfoImpl_GetDocumentation(pTInfo,
					    MEMBERID_NIL,
					    pBstrName,
					    pBstrDocString,
					    plHelpContext,
					    pBstrHelpFile);

            ITypeInfo_Release(pTInfo);
        }
    }

    return rc;
}

// ----------------------------------------------------------------------
// IsNameSub
// ----------------------------------------------------------------------
static BOOL IsNameSub(ITypeLibImpl * This, char * pName)
{
    if (!strcmp(pName, This->szName))
	return TRUE;

    oListIter<ITypeInfoImpl *>	itrTypeInfo(This->pTypeInfo);

    for (itrTypeInfo.MoveStart(); itrTypeInfo.IsValid(); itrTypeInfo.MoveNext())
    {
	if (!strcmp(pName, itrTypeInfo.Element()->szName))
	    return TRUE;

	oListIter<TLBFuncDesc *>    itrFuncDesc(itrTypeInfo.Element()->pFunctions);
	for (itrFuncDesc.MoveStart(); itrFuncDesc.IsValid(); itrFuncDesc.MoveNext())
	{
	    if (!strcmp(pName, itrFuncDesc.Element()->szName))
		return TRUE;
	}

	oListIter<TLBVarDesc *>	    itrVarDesc(itrTypeInfo.Element()->pVariables);
	for (itrVarDesc.MoveStart(); itrVarDesc.IsValid(); itrVarDesc.MoveNext())
	{
	    if (!strcmp(pName, itrVarDesc.Element()->szName))
		return TRUE;
	}
    }
    return FALSE;
}

// ----------------------------------------------------------------------
// ITypeLibImpl_IsName
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_IsName(ITypeLib2 * iface,
				LPOLESTR szNameBuf, ULONG lHashVal, BOOL *pfName)
{
    ICOM_THIS(ITypeLibImpl, iface);

    char *	astr = HEAP_strdupWtoA( GetProcessHeap(), 0, szNameBuf );

    dprintf(("OLEAUT32: ITypeLibImpl(%p,%s)->IsName(%s)\n", This, This->szName, astr));

    BOOL	fFound = IsNameSub(This, astr);

    *pfName = fFound;

    dprintf((" Search for %s: %s", astr, fFound ? "TRUE" : "FALSE"));

    HeapFree( GetProcessHeap(), 0, astr );
    return S_OK;
}


// ----------------------------------------------------------------------
// FindNameSub
// ----------------------------------------------------------------------
static BOOL FindNameSub(char *		pName,
			ITypeInfoImpl *	pTInfo,
			ULONG *		plMemID)
{
    if (!strcmp(pName, pTInfo->szName))
    {
	*plMemID = MEMBERID_NIL;	/* FIXME? */
	return TRUE;
    }

    oListIter<TLBFuncDesc *>    itrFuncDesc(pTInfo->pFunctions);
    for (itrFuncDesc.MoveStart(); itrFuncDesc.IsValid(); itrFuncDesc.MoveNext())
    {
	if (!strcmp(pName, itrFuncDesc.Element()->szName))
	{
	    *plMemID = itrFuncDesc.Element()->funcdesc.memid;
	    return TRUE;
	}
    }

    oListIter<TLBVarDesc *>	    itrVarDesc(pTInfo->pVariables);
    for (itrVarDesc.MoveStart(); itrVarDesc.IsValid(); itrVarDesc.MoveNext())
    {
	if (!strcmp(pName, itrVarDesc.Element()->szName))
	{
	    *plMemID = itrVarDesc.Element()->vardesc.memid;
	    return TRUE;
	}
    }
    return FALSE;
}

// ----------------------------------------------------------------------
// ITypeLibImpl_FindName
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLibImpl_FindName(ITypeLib2 * iface,
				LPOLESTR szNameBuf, ULONG lHashVal, ITypeInfo **ppTInfo,
				MEMBERID *rgMemId, UINT16 *pcFound)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->FindName()\n", This));

    char *	astr = HEAP_strdupWtoA( GetProcessHeap(), 0, szNameBuf );
    UINT16	cMax = *pcFound;
    UINT16	cFound = 0;
    ULONG	lMemID = 0;

    oListIter<ITypeInfoImpl *>	itrTypeInfo(This->pTypeInfo);

    for (itrTypeInfo.MoveStart(); (cFound < cMax) && itrTypeInfo.IsValid(); itrTypeInfo.MoveNext())
    {
	ITypeInfoImpl * pInfo = itrTypeInfo.Element();

	if (FindNameSub(astr, pInfo, &lMemID))
	{
	    ppTInfo[cFound] = (ITypeInfo *)pInfo;
	    rgMemId[cFound] = lMemID;
	    pInfo->lpvtbl->fnAddRef((ITypeInfo2 *)pInfo);
	    cFound += 1;
	}
    }
    *pcFound = cFound;

    HeapFree( GetProcessHeap(), 0, astr );
    return S_OK;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
void WIN32API ITypeLibImpl_ReleaseTLibAttr(ITypeLib2 * iface,
				TLIBATTR *pTLibAttr)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->ReleaseTLibAttr()\n", This));

    HeapFree(GetProcessHeap(), 0, pTLibAttr);
}


// ----------------------------------------------------------------------
// ITypeLib2Impl_GetCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLib2Impl_GetCustData(ITypeLib2 * iface,
				REFGUID guid, VARIANT *pVarVal)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetCustData()\n", This));

    oListIter<TLBCustData *>	itrCustData(This->pCustData);

    for (itrCustData.MoveStart(); itrCustData.IsValid(); itrCustData.MoveNext())
    {
        if( IsEqualIID(guid, &itrCustData.Element()->guid))
        {
	    VariantInit( pVarVal);
	    VariantCopy( pVarVal, &itrCustData.Element()->data);
	    return S_OK;
        }
    }

    return E_INVALIDARG;
}


// ----------------------------------------------------------------------
// ITypeLib2Impl_GetLibStatistics		STUB
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLib2Impl_GetLibStatistics(ITypeLib2 * iface,
				ULONG *pcUniqueNames, ULONG *pcchUniqueNames)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetLibStatistics() - Stub\n", This));

    if (pcUniqueNames)
	*pcUniqueNames = 1;

    if (pcchUniqueNames)
	*pcchUniqueNames = 1;

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeLib2Impl_GetDocumentation2
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLib2Impl_GetDocumentation2(ITypeLib2 * iface,
				INT index, LCID lcid, BSTR *pbstrHelpString,
				DWORD *plHelpStringContext, BSTR *pbstrHelpStringDll)
{
    ICOM_THIS(ITypeLibImpl, iface);
    HRESULT	rc;

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetDocumentation2()\n", This));

    /* if index is -Ve this is a request for documentation for the typelib */
    if (index < 0)
    {
        if (pbstrHelpString)
            *pbstrHelpString = DupAtoBstr(This->szDocString);
        if (plHelpStringContext)
            *plHelpStringContext = This->lHelpContext;
        if (pbstrHelpStringDll)
            *pbstrHelpStringDll = DupAtoBstr(This->szHelpStringDll);
	rc = S_OK;
    }
    else
    { // for specified typeinfo
	ITypeInfo2 *	pTInfo;

	rc = ITypeLibImpl_GetTypeInfo(iface, index, (ITypeInfo **)&pTInfo);

        if (SUCCEEDED(rc))
        {
	    // Delegate to typeinfo interface...
            rc = ITypeInfo2Impl_GetDocumentation2(pTInfo,
					    MEMBERID_NIL,
					    lcid,
					    pbstrHelpString,
					    plHelpStringContext,
					    pbstrHelpStringDll);
            ITypeInfo_Release(pTInfo);
        }
    }

    return rc;
}


// ----------------------------------------------------------------------
// ITypeLib2Impl_GetAllCustData
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeLib2Impl_GetAllCustData(ITypeLib2 * iface,
				CUSTDATA *pCustData)
{
    ICOM_THIS(ITypeLibImpl, iface);

    dprintf(("OLEAUT32: ITypeLibImpl(%p)->GetAllCustData()\n", This));

    CUSTDATAITEM *	pItem;

    // Sanity check...
    if (pCustData == 0)
	return E_INVALIDARG;

    pCustData->prgCustData = 0;
    pCustData->cCustData = 0;

    if ((pItem = (CUSTDATAITEM *)HeapAlloc(GetProcessHeap(), 0,
			    sizeof(CUSTDATAITEM) * This->pCustData.Count())) == 0)
    {
	dprintf(("OLEAUT32: ITypeLib2Impl_GetAllCustData: E_OUTOFMEMORY"));
        return E_OUTOFMEMORY;
    }

    pCustData->prgCustData = pItem;
    pCustData->cCustData = This->pCustData.Count();

    oListIter<TLBCustData *>	itrCustData(This->pCustData);

    for (itrCustData.MoveStart(); itrCustData.IsValid(); itrCustData.MoveNext())
    {
	pItem->guid = itrCustData.Element()->guid;
	VariantCopy(&pItem->varValue, &itrCustData.Element()->data);
	pItem += 1;
    }

    return S_OK;
}
