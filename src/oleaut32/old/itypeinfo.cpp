/* $Id: itypeinfo.cpp,v 1.1 2001-08-10 19:24:46 sandervl Exp $ */
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
#include <debugtools.h>
#include "asmutil.h"

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

    if(index==(UINT)-1)
    {
      /* only valid on dual interfaces;
         retrieve the associated TKIND_INTERFACE handle for the current TKIND_DISPATCH
      */
      if( This->TypeAttr.typekind != TKIND_DISPATCH) return E_INVALIDARG;
      
      if (This->TypeAttr.wTypeFlags & TYPEFLAG_FDISPATCHABLE &&
          This->TypeAttr.wTypeFlags & TYPEFLAG_FDUAL )
      {
        *pRefType = -1;
      }
      else
      {
        if (This->pImplements.Count() == 0) return TYPE_E_ELEMENTNOTFOUND;
        *pRefType = This->pImplements[0]->reference;
      }
    }
    else
    {
      if (This->pImplements.Count() < index)
	return TYPE_E_ELEMENTNOTFOUND;

      *pRefType = This->pImplements[index]->reference;
    }
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
/**
 * Coerce arguments and, if necessary, sorts named arguments in the order 
 * specified in the function's FUNCDESC
 *
 * NOTE: the arguments are left in reverse order by design. 
 *       (see INVOKE_InvokeStdCallFunction for more information)
 */
static HRESULT
INVOKE_AllocateCoerceAndSortArguments(FUNCDESC*      pFuncDesc,
                                      DISPPARAMS*    pDispParams, 
                                      VARIANTARG**  ppaArgsCoerced,
                                      unsigned int*  puBadArg)
{
    HRESULT     hr;
    UINT        index;        

    VARIANTARG* paArgsCoerced = (VARIANTARG*)HeapAlloc(GetProcessHeap(), 
                                          HEAP_ZERO_MEMORY, 
                                          pDispParams->cArgs * sizeof(VARIANTARG));

    if ( !paArgsCoerced )
    {
        hr = E_OUTOFMEMORY;
        goto cleanup;
    }

    if ( !pDispParams->cNamedArgs )
    {
        /* "Arguments are stored in pDispParams->rgvarg in reverse order"
         * 
         * Windows Platform SDK, IDispatch::Invoke.
         * See puArgErr argument description.
         */
        VARIANTARG* paSrcArg = &pDispParams->rgvarg[pDispParams->cArgs - 1];
        VARIANTARG* paDstArg = &paArgsCoerced[pDispParams->cArgs - 1];

        /* sanity check to verify we have enough required 
         * arguments to call the method
         */
        if ( pDispParams->cArgs < pFuncDesc->cParams - pFuncDesc->cParamsOpt )
        {
            hr = DISP_E_BADPARAMCOUNT;
            goto cleanup;
        }

        /* coerce all the arguments, if required */
        for ( index = 0; 
            index < pDispParams->cArgs;  
            ++index, --paSrcArg, --paDstArg )
        {
            /* checks if the VARTYPEs matches */
            VARTYPE vtdest = pFuncDesc->lprgelemdescParam[index].tdesc.vt;

            if ( V_VT(paSrcArg) != vtdest )
            {
                USHORT wFlags = 0;

               /* we don't use VariantChangeTypeEx 
                 * because we assume system lcid (e.g. 0)
                 */
                hr = VariantChangeType(paDstArg, paSrcArg, wFlags, vtdest); 

                if ( FAILED(hr) ) goto bad_arg_err;
            }
            else
            {
                hr = VariantCopy(paDstArg, paSrcArg);

                if ( FAILED(hr) ) goto bad_arg_err;
            }
        }
    }
    else
    {
        FIXME("Named arguments not supported!\n");
        /*
                named arguments not supported ?
                    hr = DISP_E_NONAMEDARGS
                    
                For each argument
                    Check if DISPID can be found in typeinfo
    
                    Found -> 
                        required argument?
                            yes -> increase required argument counter. next.
                            no  -> next.
                            
                    Not Found -> DISP_E_PARAMNOTFOUND, puBadArg = arg index.
                
                if function required arguments != counter
                    DISP_E_PARAMNOTOPTIONAL
    
                Coerce arguments at the right spot in the variant array
        */
    }

    hr = S_OK;
    *ppaArgsCoerced = paArgsCoerced;
    return hr;


bad_arg_err:
    *puBadArg = index;

cleanup:
    if ( paArgsCoerced )
    {
        HeapFree(GetProcessHeap(), 0, paArgsCoerced);
    }
    
    return hr;
}

/*
static
WINE_EXCEPTION_FILTER(INVOKE_ExceptionHandler)
{
    FIXME("Exception handler currently does nothing.\n");
    return EXCEPTION_EXECUTE_HANDLER;
}
*/

static HRESULT
INVOKE_InvokeStdCallFunction(IUnknown*   pIUnk,
                             FUNCDESC*   pFuncDesc,
                             VARIANTARG* paArgsCoerced,
                             VARIANT*    pVarResult,
                             EXCEPINFO*  pExcepInfo)
{
    typedef DWORD  (WINAPI *pDWORDRetFunc) (IUnknown*);   
    typedef double (WINAPI *pDoubleRetFunc)(IUnknown*); 

    HRESULT hr = E_FAIL;

    DWORD   dwVtblBaseAddr;               /* address of vtbl   */
    DWORD   dwFuncAddr;                   /* address of method */
    DWORD   paramsize = 0;

    int     i;                            /* index of argument */

    /* Process method arguments.
     *
     * Following stdcall convention, arguments are pushed on the stack from 
     * right to left. However, since they were stored in reverse order 
     * in DISPPARAMS structure (and left in that order during coercion)
     * we push the arguments from 0 to cArgs - 1.
     */
    DWORD *pStack    = (DWORD *)alloca(sizeof(double)*pFuncDesc->cParams + 8);
    PBYTE  pStackPtr = (PBYTE)((char *)pStack + sizeof(double)*pFuncDesc->cParams);

    //Don't include the pIUnknown parameter
    for ( i = 0; i < pFuncDesc->cParams; ++i )
    {
        /* Decision of pushing 32 or 64 bits is based on argument's VARTYPE */
        VARIANT* pVar   = &(paArgsCoerced[i]);
        PVOID    pValue = &(pVar->cVal);

        if(pVar == NULL) {//TODO: Is this correct?
            /* push 32 bits on stack */
            pStackPtr -= sizeof(DWORD);
            * (DWORD*) pStackPtr = 0;

            paramsize += sizeof(DWORD);
        }
        else 
        switch ( V_VT(pVar) )
        {
        /* QUAD-sized values */
        case VT_R8:
        case VT_DATE:
            /* push 64 bits on stack */
            pStackPtr -= sizeof(double);
            * (double*) pStackPtr = *(double*) pValue;

            paramsize += sizeof(double);
            break;

        /* DWORD-sized values */
        default:
            /* push 32 bits on stack */
            pStackPtr -= sizeof(DWORD);
            * (DWORD*) pStackPtr = * (DWORD*) pValue;

            paramsize += sizeof(DWORD);
            break;
        }
   }   
    /* assign function pointer to ITypeImpl->vtbl[index] */
   dwVtblBaseAddr =    (DWORD)  ((IUnknown*) pIUnk)->lpVtbl;
   dwFuncAddr     =  * (DWORD*) (dwVtblBaseAddr + pFuncDesc->oVft);

   /* remove comments around __TRY for SEH
   __TRY
   */
   {
        DWORD  dwRet  = 0;
        double dblRet = 0;

        //push pIUnk
        pStackPtr -= sizeof(DWORD);
        * (DWORD*) pStackPtr = * (DWORD*) pIUnk;
        paramsize += sizeof(DWORD);

        //reset to top
        pStackPtr  = (PBYTE)((char *)pStack + sizeof(double)*pFuncDesc->cParams);
        pStackPtr -= 4;

         /* invoke function */
        if ( pFuncDesc->elemdescFunc.tdesc.vt == VT_R8   ||
             pFuncDesc->elemdescFunc.tdesc.vt == VT_DATE )
        {
            dblRet = invokeStdCallDouble((PVOID)dwFuncAddr, paramsize/4, pStackPtr);
        }
        else
        {
            dwRet = invokeStdCallDword((PVOID)dwFuncAddr, paramsize/4, pStackPtr);
        }

        /* fill result variant */
        if ( pVarResult )
        {
            pVarResult->vt = pFuncDesc->elemdescFunc.tdesc.vt;

            if ( pFuncDesc->elemdescFunc.tdesc.vt == VT_R8   ||
                 pFuncDesc->elemdescFunc.tdesc.vt == VT_DATE )
            {
                V_UNION(pVarResult, dblVal) = dblRet;
            }
            else
            {
                V_UNION(pVarResult, lVal)   = dwRet;
            }
        }
        hr = S_OK;
    }
    
    /* remove comments to enable SEH
    __EXCEPT (INVOKE_ExceptionHandler);
    __ENDTRY;
    */
    
    return hr;
}

static BOOL
INVOKE_RetrieveFuncDesc(oListIter<TLBFuncDesc *> itrFunc,
                        MEMBERID     memid,
                        UINT16       wFlags,
                        FUNCDESC**   ppFuncDesc)
{
    BOOL  bFound = FALSE;

    // Search functions...
    for (itrFunc.MoveStart(); itrFunc.IsValid(); itrFunc.MoveNext())
    {
	if (itrFunc.Element()->funcdesc.memid == memid && 
            itrFunc.Element()->funcdesc.invkind == wFlags)
	{
            *ppFuncDesc = &(itrFunc.Element()->funcdesc);
	    return TRUE;
	}
    }

    return(bFound);
}

static HRESULT
INVOKE_DeferToParentTypeInfos(ITypeInfo2*     iface,
                              VOID*           pIUnk,
                              MEMBERID        memid,
                              UINT16          wFlags,
                              DISPPARAMS*     pDispParams,
                              VARIANT*        pVarResult,
                              EXCEPINFO*      pExcepInfo,
                              UINT*           puArgErr)
{
    HRESULT hr = E_FAIL;
    
    unsigned short index;
    ITypeInfo*     pRefTypeInfo = NULL;

    /* retrieve ITypeInfoImpl ptr from ITypeInfo ptr */
    ICOM_THIS( ITypeInfoImpl, iface);
    /* call Invoke on implemented type */
    for ( index = 0; index < This->TypeAttr.cImplTypes ; ++index )
    {
        HREFTYPE    hRef;

        hr = ITypeInfo_GetRefTypeOfImplType(iface, index, &hRef);

        if ( FAILED(hr) )
        {
            hr = DISP_E_MEMBERNOTFOUND;
            goto cleanup;
        }

        hr = ITypeInfo_GetRefTypeInfo(iface, hRef, &pRefTypeInfo);

        if ( FAILED(hr) )
        {
            hr = DISP_E_MEMBERNOTFOUND;
            goto cleanup;
        }

        hr = ITypeInfo_Invoke(pRefTypeInfo, 
                              pIUnk,
                              memid,
                              wFlags,
                              pDispParams,
                              pVarResult,
                              pExcepInfo,
                              puArgErr);

        if ( hr == S_OK )
            break;
    }
cleanup:
    if (pRefTypeInfo)
        ITypeInfo2_Release(pRefTypeInfo);
    
    return hr;
}
/**
 * ITypeInfo::Invoke
 * 
 * Invokes a method, or accesses a property of an object, that implements the
 * interface described by the type description.
 *
 *  Handles all the "magic" between your Automation controller
 *  and components.
 *
 * 1. Find the method's position in the vtable based on
 *    the dispid and the information contained in the type library.
 *
 * 2. Push all the arguments on the stack, accordingly to the __stdcall 
 *    calling convention, e.g. right to left.
 *
 * 3. Coerce the retrieved value as the variant type stored in the type library.
 *
 * 4. Store return value in VarResult variant
 * 
 * 5. Return to the Invoke() caller the return value for dispid method.
 *
 * NOT SUPPORTED : inherited types
 *
 */
HRESULT WIN32API ITypeInfoImpl_Invoke(ITypeInfo2 * iface,
				VOID  *pIUnk, MEMBERID memid, UINT16 wFlags,
				DISPPARAMS  *pDispParams, VARIANT *pVarResult,
				EXCEPINFO  *pExcepInfo, UINT  *puArgErr)
{
    ICOM_THIS(ITypeInfoImpl, iface);
    HRESULT       hr = E_FAIL;
    FUNCDESC*     pFuncDesc;
    VARIANT*      paArgsCoerced = NULL;       /* coerced arguments */
    BOOL          bFound;                     /* function found    */


    dprintf(("TypeInfo_fnInvoke: (%p) (%p, memid=0x%08lx, 0x%08x, %p, %p, %p, %p)",
          This, pIUnk, memid, wFlags, 
          pDispParams, pVarResult, pExcepInfo, puArgErr ));

//    dump_TypeInfo(This);

    if ( pDispParams )
    {
//        dump_DispParms(pDispParams);

        if ( pDispParams->cNamedArgs != pDispParams->cArgs )
        {
            WARN("named arguments count differs from number of arguments!\n");
        }
    }

    if ( memid < 0 )
    {
        FIXME("standard MEMBERID resolution not fully implemented\n");

        switch ( memid )
        {
        case DISPID_CONSTRUCTOR:
            memid = This->TypeAttr.memidConstructor;
            break;
        case DISPID_DESTRUCTOR:
            memid = This->TypeAttr.memidDestructor;
            break;
        case DISPID_UNKNOWN:
        default:
            return DISP_E_MEMBERNOTFOUND;
        }
    }

    /* REMINDER:
     *
     * "properties are an equivalent expression of get and put functions 
     *   for data members"
     *  
     *  (Brockschmidt - Inside OLE, 2nd Edition,
     *                  Chap.14 # The Mechanics of OLE Automation)
     *
     * Thus, the following points to the list of methods AND properties
     */

    /* find FUNCDESC by memid */
    FIXME("possible collision between property and method names\n");

    bFound = INVOKE_RetrieveFuncDesc(This->pFunctions, 
                                     memid, 
                                     wFlags,
                                    &pFuncDesc);

    if ( !bFound )
    {
        if ( !(This->TypeAttr.cImplTypes) )     return DISP_E_MEMBERNOTFOUND;

        /* indirect recursion : following function calls ITypeInfo::Invoke */
        hr = INVOKE_DeferToParentTypeInfos(iface,
                                           pIUnk,
                                           memid,
                                           wFlags,
                                           pDispParams,
                                           pVarResult,
                                           pExcepInfo,
                                           puArgErr );
    }
    else
    {
        /* Validate invoke context. No return value on property put/putref */
        if ( wFlags & DISPATCH_PROPERTYPUTREF )
        {
            if ( pFuncDesc->lprgelemdescParam[0].tdesc.vt != VT_BYREF )
            {
                TRACE("Property put by reference not supported by object\n");
                return DISP_E_MEMBERNOTFOUND;
            }

            pVarResult = NULL;
        }
        else if ( wFlags & DISPATCH_PROPERTYPUT )
        {
            pVarResult = NULL;
        }
        else if ( wFlags & DISPATCH_PROPERTYGET ||
                  wFlags & DISPATCH_METHOD )
        {
            /* verify if property isn't read-only */
        }
        else
        {
            TRACE("Invalid method call context\n");
            return DISP_E_MEMBERNOTFOUND;
        }

        if ( pDispParams && pDispParams->cArgs > 0 )
        {
            /* this allocates an array of VARIANTARG 
             * in their final form for invocation
             */
            hr = INVOKE_AllocateCoerceAndSortArguments(pFuncDesc, 
                                                       pDispParams, 
                                                      &paArgsCoerced,
                                                       puArgErr);
            if ( FAILED(hr) ) return hr;
        }

        switch ( pFuncDesc->callconv )
        {
        case CC_STDCALL:
            hr = INVOKE_InvokeStdCallFunction((IUnknown *)pIUnk,    
                                              pFuncDesc,
                                              paArgsCoerced,
                                              pVarResult,
                                              pExcepInfo);
            break;
        default:
            FIXME("Calling convention not supported!\n");
            return E_FAIL;
        }
    }
    
    return hr;
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
    else 
    if (hRefType == -1 && 
       (((ITypeInfoImpl*) This)->TypeAttr.typekind   == TKIND_DISPATCH) &&
       (((ITypeInfoImpl*) This)->TypeAttr.wTypeFlags &  TYPEFLAG_FDUAL))
    {
	  /* when we meet a DUAL dispinterface, we must create the interface 
	  * version of it.
	  */
	  ITypeInfoImpl* pTypeInfoImpl = (ITypeInfoImpl*) ITypeInfoImpl_Constructor();
	
	  /* the interface version contains the same information as the dispinterface
	   * copy the contents of the structs.
	   */
	  *pTypeInfoImpl = *This;
	  pTypeInfoImpl->ref = 1;
		
	  /* change the type to interface */
	  pTypeInfoImpl->TypeAttr.typekind = TKIND_INTERFACE;
		
          *ppTInfo = (ITypeInfo*) pTypeInfoImpl;

	  ITypeInfo_AddRef((ITypeInfo*) pTypeInfoImpl);

          return S_OK;
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



