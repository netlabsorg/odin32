/* $Id: imcntl.c,v 1.2 2001-09-05 13:36:35 bird Exp $ */
/*
 * Implementation of IMediaControl for FilterGraph.
 *
 * FIXME - stub.
 *
 * hidenori@a2.ctktv.ne.jp
 */

#include "config.h"

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winerror.h"
#include "wine/obj_base.h"
#include "wine/obj_oleaut.h"
#include "strmif.h"
#include "control.h"
#include "uuids.h"

#include "debugtools.h"
DEFAULT_DEBUG_CHANNEL(quartz);

#include "quartz_private.h"
#include "fgraph.h"



static HRESULT WINAPI
IMediaControl_fnQueryInterface(IMediaControl* iface,REFIID riid,void** ppobj)
{
    CFilterGraph_THIS(iface,mediacontrol);

    TRACE("(%p)->()\n",This);

    return IUnknown_QueryInterface(This->unk.punkControl,riid,ppobj);
}

static ULONG WINAPI
IMediaControl_fnAddRef(IMediaControl* iface)
{
    CFilterGraph_THIS(iface,mediacontrol);

    TRACE("(%p)->()\n",This);

    return IUnknown_AddRef(This->unk.punkControl);
}

static ULONG WINAPI
IMediaControl_fnRelease(IMediaControl* iface)
{
    CFilterGraph_THIS(iface,mediacontrol);

    TRACE("(%p)->()\n",This);

    return IUnknown_Release(This->unk.punkControl);
}

static HRESULT WINAPI
IMediaControl_fnGetTypeInfoCount(IMediaControl* iface,UINT* pcTypeInfo)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnGetTypeInfo(IMediaControl* iface,UINT iTypeInfo, LCID lcid, ITypeInfo** ppobj)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnGetIDsOfNames(IMediaControl* iface,REFIID riid, LPOLESTR* ppwszName, UINT cNames, LCID lcid, DISPID* pDispId)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnInvoke(IMediaControl* iface,DISPID DispId, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarRes, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}


static HRESULT WINAPI
IMediaControl_fnRun(IMediaControl* iface)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnPause(IMediaControl* iface)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnStop(IMediaControl* iface)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnGetState(IMediaControl* iface,LONG lTimeOut,OAFilterState* pFilterState)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnRenderFile(IMediaControl* iface,BSTR bstrFileName)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnAddSourceFilter(IMediaControl* iface,BSTR bstrFileName,IDispatch** ppobj)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnget_FilterCollection(IMediaControl* iface,IDispatch** ppobj)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnget_RegFilterCollection(IMediaControl* iface,IDispatch** ppobj)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}

static HRESULT WINAPI
IMediaControl_fnStopWhenReady(IMediaControl* iface)
{
    CFilterGraph_THIS(iface,mediacontrol);

    FIXME("(%p)->()\n",This);

    return E_NOTIMPL;
}


static ICOM_VTABLE(IMediaControl) imediacontrol =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    /* IUnknown fields */
    IMediaControl_fnQueryInterface,
    IMediaControl_fnAddRef,
    IMediaControl_fnRelease,
    /* IDispatch fields */
    IMediaControl_fnGetTypeInfoCount,
    IMediaControl_fnGetTypeInfo,
    IMediaControl_fnGetIDsOfNames,
    IMediaControl_fnInvoke,
    /* IMediaControl fields */
    IMediaControl_fnRun,
    IMediaControl_fnPause,
    IMediaControl_fnStop,
    IMediaControl_fnGetState,
    IMediaControl_fnRenderFile,
    IMediaControl_fnAddSourceFilter,
    IMediaControl_fnget_FilterCollection,
    IMediaControl_fnget_RegFilterCollection,
    IMediaControl_fnStopWhenReady,
};


void CFilterGraph_InitIMediaControl( CFilterGraph* pfg )
{
    TRACE("(%p)\n",pfg);
    ICOM_VTBL(&pfg->mediacontrol) = &imediacontrol;
}
