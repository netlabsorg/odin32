/**
 * Dispatch API functions
 *
 * Copyright 2000  Francois Jacques, Macadamian Technologies Inc.
 *
 * ---
 *
 * TODO: Type coercion is implemented in variant.c but not called yet.
 */
#ifdef __WIN32OS2__
#define HAVE_FLOAT_H
#define WINE_LARGE_INTEGER
#include "oleaut32.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "winerror.h"
#include "winreg.h"         /* for HKEY_LOCAL_MACHINE */
#include "winnls.h"         /* for PRIMARYLANGID */
#include "ole.h"
#include "heap.h"
#include "wine/obj_oleaut.h"
#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(ole);
DECLARE_DEBUG_CHANNEL(typelib);


/******************************************************************************
 *         DispInvoke    (OLEAUT32.30)
 *
 *
 * Calls method of an object through its IDispatch interface.
 *
 * NOTES
 * 		- Defer	method invocation to ITypeInfo::Invoke()
 *
 * RETURNS
 *
 * 		S_OK on success.
 */
HRESULT WINAPI
DispInvoke(VOID*            _this,          /* object instance */
           ITypeInfo*       ptinfo,         /* object's type info */
           DISPID           dispidMember,   /* member id */
           USHORT           wFlags,         /* kind of method call */
           DISPPARAMS*      pparams,        /* array of arguments */
           VARIANT*         pvarResult,     /* result of method call */
           EXCEPINFO*       pexcepinfo,     /* information about exception */
           UINT*            puArgErr        /* index of bad argument(if any) */
          )
{
    HRESULT hr = E_FAIL;

    /**
     * TODO:
     * For each param, call DispGetParam to perform type coercion
     */
    FIXME("Coercion of arguments not implemented\n");

    hr = ICOM_CALL7(Invoke,
                    ptinfo,
                    _this,
                    dispidMember,
                    wFlags,
                    pparams, pvarResult, pexcepinfo, puArgErr);

    return (hr);
}


/******************************************************************************
 *         DispGetIDsOfNames (OLEAUT32.29)
 *
 * Convert a set of names to dispids, based on information 
 * contained in object's type library.
 * 
 * NOTES
 * 		- Defers to ITypeInfo::GetIDsOfNames()
 *
 * RETURNS
 *
 * 		S_OK on success.
 */
HRESULT WINAPI
DispGetIDsOfNames(ITypeInfo* ptinfo,
                  OLECHAR**  rgszNames,
                  UINT       cNames,
                  DISPID*    rgdispid)
{
    HRESULT hr = E_FAIL;

    hr = ICOM_CALL3(GetIDsOfNames,
                    ptinfo,
                    rgszNames,
                    cNames,
                    rgdispid);
    return (hr);
}

/******************************************************************************
 *         DispGetParam    (OLEAUT32.30)
 *
 * Retrive a parameter from a DISPPARAMS structures and coerce it to
 * specified variant type
 *
 * NOTES
 * 		Coercion is done using system (0) locale.
 *
 * RETURNS
 *
 * 		S_OK on success.
 */
HRESULT WINAPI DispGetParam(DISPPARAMS* pdispparams, UINT position,
            VARTYPE vtTarg, VARIANT* pvarResult, UINT* puArgErr)
{
    HRESULT hr = E_FAIL;

    /**
     * TODO : Call VariantChangeTypeEx with LCID 0 (system)
     */

    FIXME("Coercion of arguments not implemented\n");
    return (hr);
}
