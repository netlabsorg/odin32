/*		DirectInput
 *
 * Copyright 1998 Marcus Meissner
 * Copyright 1998,1999 Lionel Ulmer
 *
 */
/* Status:
 *
 * - Tomb Raider 2 Demo:
 *   Playable using keyboard only.
 * - WingCommander Prophecy Demo:
 *   Doesn't get Input Focus.
 * 
 * - Fallout : works great in X and DGA mode
 *
 * FIXME: The keyboard handling needs to (and will) be merged into keyboard.c
 *	  (The current implementation is currently only a proof of concept and
 *	   an utter mess.)
 */

#include "config.h"
#include <assert.h>
#include <string.h>

#include "debugtools.h"
#include "winbase.h"
#include "winerror.h"
#include "windef.h"
#include "dinput_private.h"

DEFAULT_DEBUG_CHANNEL(dinput);

static ICOM_VTABLE(IDirectInputA) ddiavt;
static ICOM_VTABLE(IDirectInput7A) ddi7avt;

/* This array will be filled a dinput.so loading */
#define MAX_WINE_DINPUT_DEVICES 4
static dinput_device * dinput_devices[MAX_WINE_DINPUT_DEVICES];
static int nrof_dinput_devices = 0;

/* register a direct draw driver. We better not use malloc for we are in 
 * the ELF startup initialisation at this point.
 */
void dinput_register_device(dinput_device *device) {
    int	i;

    /* insert according to priority */
    for (i=0;i<nrof_dinput_devices;i++) {
	if (dinput_devices[i]->pref <= device->pref) {
	    memcpy(dinput_devices+i+1,dinput_devices+i,sizeof(dinput_devices[0])*(nrof_dinput_devices-i));
	    dinput_devices[i] = device;
	    break;
	}
    }
    if (i==nrof_dinput_devices)	/* not found, or too low priority */
	dinput_devices[nrof_dinput_devices] = device;

    nrof_dinput_devices++;

    /* increase MAX_DDRAW_DRIVERS if the line below triggers */
    assert(nrof_dinput_devices <= MAX_WINE_DINPUT_DEVICES);
}

/******************************************************************************
 *	DirectInputCreateEx (DINPUT.@)
 */
HRESULT WINAPI DirectInputCreateEx(
	HINSTANCE hinst, DWORD dwVersion, REFIID riid, LPVOID *ppDI,
	LPUNKNOWN punkOuter
) {
	IDirectInputAImpl* This;

	TRACE("(0x%08lx,%04lx,%s,%p,%p)\n",
		(DWORD)hinst,dwVersion,debugstr_guid(riid),ppDI,punkOuter
	);
	if (IsEqualGUID(&IID_IDirectInputA,riid)) {
	  This = (IDirectInputAImpl*)HeapAlloc(GetProcessHeap(),0,sizeof(IDirectInputAImpl));
	  This->ref = 1;
	  ICOM_VTBL(This) = &ddiavt;
	  *ppDI = This;
	  
	  return DI_OK;
	}
	
	if (IsEqualGUID(&IID_IDirectInput7A,riid)) {
	  This = (IDirectInputAImpl*)HeapAlloc(GetProcessHeap(),0,sizeof(IDirectInputAImpl));
	  This->ref = 1;
	  ICOM_VTBL(This) = (ICOM_VTABLE(IDirectInputA) *) &ddi7avt;
	  *ppDI = This;
	  
	  return DI_OK;
	}

	return DIERR_OLDDIRECTINPUTVERSION;
}

/******************************************************************************
 *	DirectInputCreateA (DINPUT.@)
 */
HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter)
{
	IDirectInputAImpl* This;
	TRACE("(0x%08lx,%04lx,%p,%p)\n",
		(DWORD)hinst,dwVersion,ppDI,punkOuter
	);
	This = (IDirectInputAImpl*)HeapAlloc(GetProcessHeap(),0,sizeof(IDirectInputAImpl));
	This->ref = 1;
	ICOM_VTBL(This) = &ddiavt;
	*ppDI=(IDirectInputA*)This;
	return 0;
}
/******************************************************************************
 *	IDirectInputA_EnumDevices
 */
static HRESULT WINAPI IDirectInputAImpl_EnumDevices(
	LPDIRECTINPUT7A iface, DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback,
	LPVOID pvRef, DWORD dwFlags
)
{
	ICOM_THIS(IDirectInputAImpl,iface);
	DIDEVICEINSTANCEA devInstance;
	int i;

	TRACE("(this=%p,0x%04lx,%p,%p,%04lx)\n", This, dwDevType, lpCallback, pvRef, dwFlags);

	for (i = 0; i < nrof_dinput_devices; i++) {
	  if (dinput_devices[i]->enum_device(dwDevType, dwFlags, &devInstance)) {
	    if (lpCallback(&devInstance,pvRef) == DIENUM_STOP)
	      return 0;
	  }
	}
	
	return 0;
}

static ULONG WINAPI IDirectInputAImpl_AddRef(LPDIRECTINPUT7A iface)
{
	ICOM_THIS(IDirectInputAImpl,iface);
	return ++(This->ref);
}

static ULONG WINAPI IDirectInputAImpl_Release(LPDIRECTINPUT7A iface)
{
	ICOM_THIS(IDirectInputAImpl,iface);
	if (!(--This->ref)) {
		HeapFree(GetProcessHeap(),0,This);
		return 0;
	}
	return This->ref;
}

static HRESULT WINAPI IDirectInputAImpl_CreateDevice(
	LPDIRECTINPUT7A iface,REFGUID rguid,LPDIRECTINPUTDEVICEA* pdev,
	LPUNKNOWN punk
) {
	ICOM_THIS(IDirectInputAImpl,iface);
	HRESULT ret_value = DIERR_DEVICENOTREG;
	int i;
	
	TRACE("(this=%p,%s,%p,%p)\n",This,debugstr_guid(rguid),pdev,punk);

	/* Loop on all the devices to see if anyone matches the given GUID */
	for (i = 0; i < nrof_dinput_devices; i++) {
	  HRESULT ret;
	  if ((ret = dinput_devices[i]->create_device(This, rguid, NULL, pdev)) == DI_OK)
	    return DI_OK;

	  if (ret == DIERR_NOINTERFACE)
	    ret_value = DIERR_NOINTERFACE;
	}

	return ret_value;
}

static HRESULT WINAPI IDirectInputAImpl_QueryInterface(
	LPDIRECTINPUT7A iface,REFIID riid,LPVOID *ppobj
) {
	ICOM_THIS(IDirectInputAImpl,iface);

	TRACE("(this=%p,%s,%p)\n",This,debugstr_guid(riid),ppobj);
	if (IsEqualGUID(&IID_IUnknown,riid)) {
		IDirectInputA_AddRef(iface);
		*ppobj = This;
		return 0;
	}
	if (IsEqualGUID(&IID_IDirectInputA,riid)) {
		IDirectInputA_AddRef(iface);
		*ppobj = This;
		return 0;
	}
	TRACE("Unsupported interface !\n");
	return E_FAIL;
}

static HRESULT WINAPI IDirectInputAImpl_Initialize(
	LPDIRECTINPUT7A iface,HINSTANCE hinst,DWORD x
) {
	return DIERR_ALREADYINITIALIZED;
}

static HRESULT WINAPI IDirectInputAImpl_GetDeviceStatus(LPDIRECTINPUT7A iface,
							REFGUID rguid) {
  ICOM_THIS(IDirectInputAImpl,iface);
  
  FIXME("(%p)->(%s): stub\n",This,debugstr_guid(rguid));
  
  return DI_OK;
}

static HRESULT WINAPI IDirectInputAImpl_RunControlPanel(LPDIRECTINPUT7A iface,
							HWND hwndOwner,
							DWORD dwFlags) {
  ICOM_THIS(IDirectInputAImpl,iface);
  FIXME("(%p)->(%08lx,%08lx): stub\n",This, (DWORD) hwndOwner, dwFlags);
  
  return DI_OK;
}

static HRESULT WINAPI IDirectInput2AImpl_FindDevice(LPDIRECTINPUT2A iface, REFGUID rguid,
						    LPCSTR pszName, LPGUID pguidInstance) {
  ICOM_THIS(IDirectInputAImpl,iface);
  FIXME("(%p)->(%s, %s, %p): stub\n", This, debugstr_guid(rguid), pszName, pguidInstance);
  
  return DI_OK;
}

static HRESULT WINAPI IDirectInput7AImpl_CreateDeviceEx(LPDIRECTINPUT7A iface, REFGUID rguid,
							REFIID riid, LPVOID* pvOut, LPUNKNOWN lpUnknownOuter)
{
  ICOM_THIS(IDirectInputAImpl,iface);
  HRESULT ret_value = DIERR_DEVICENOTREG;
  int i;
	
  TRACE("(%p)->(%s, %s, %p, %p)\n", This, debugstr_guid(rguid), debugstr_guid(riid), pvOut, lpUnknownOuter);

  /* Loop on all the devices to see if anyone matches the given GUID */
  for (i = 0; i < nrof_dinput_devices; i++) {
    HRESULT ret;
    if ((ret = dinput_devices[i]->create_device(This, rguid, riid, (LPDIRECTINPUTDEVICEA*) pvOut)) == DI_OK)
      return DI_OK;
    
    if (ret == DIERR_NOINTERFACE)
      ret_value = DIERR_NOINTERFACE;
  }
  
  return ret_value;
}

#if !defined(__STRICT_ANSI__) && defined(__GNUC__)
# define XCAST(fun)	(typeof(ddiavt.fun))
#else
# define XCAST(fun)	(void*)
#endif

static ICOM_VTABLE(IDirectInputA) ddiavt = 
{
	ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
	XCAST(QueryInterface)IDirectInputAImpl_QueryInterface,
	XCAST(AddRef)IDirectInputAImpl_AddRef,
	XCAST(Release)IDirectInputAImpl_Release,
	XCAST(CreateDevice)IDirectInputAImpl_CreateDevice,
	XCAST(EnumDevices)IDirectInputAImpl_EnumDevices,
	XCAST(GetDeviceStatus)IDirectInputAImpl_GetDeviceStatus,
	XCAST(RunControlPanel)IDirectInputAImpl_RunControlPanel,
	XCAST(Initialize)IDirectInputAImpl_Initialize
};
#undef XCAST

#if !defined(__STRICT_ANSI__) && defined(__GNUC__)
# define XCAST(fun)	(typeof(ddi7avt.fun))
#else
# define XCAST(fun)	(void*)
#endif

static ICOM_VTABLE(IDirectInput7A) ddi7avt = {
	ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
	XCAST(QueryInterface)IDirectInputAImpl_QueryInterface,
	XCAST(AddRef)IDirectInputAImpl_AddRef,
	XCAST(Release)IDirectInputAImpl_Release,
	XCAST(CreateDevice)IDirectInputAImpl_CreateDevice,
	XCAST(EnumDevices)IDirectInputAImpl_EnumDevices,
	XCAST(GetDeviceStatus)IDirectInputAImpl_GetDeviceStatus,
	XCAST(RunControlPanel)IDirectInputAImpl_RunControlPanel,
	XCAST(Initialize)IDirectInputAImpl_Initialize,
	XCAST(FindDevice)IDirectInput2AImpl_FindDevice,
	IDirectInput7AImpl_CreateDeviceEx
};
#undef XCAST

/***********************************************************************
 *		DllCanUnloadNow (DINPUT.@)
 */
HRESULT WINAPI DINPUT_DllCanUnloadNow(void)
{
    FIXME("(void): stub\n");

    return S_FALSE;
}

/***********************************************************************
 *		DllGetClassObject (DINPUT.@)
 */
HRESULT WINAPI DINPUT_DllGetClassObject(REFCLSID rclsid, REFIID riid,
					LPVOID *ppv)
{
    FIXME("(%p, %p, %p): stub\n", debugstr_guid(rclsid), 
	  debugstr_guid(riid), ppv);

    return CLASS_E_CLASSNOTAVAILABLE;
}

/***********************************************************************
 *		DllRegisterServer (DINPUT.@)
 */
HRESULT WINAPI DINPUT_DllRegisterServer(void)
{
    FIXME("(void): stub\n");

    return S_OK;
}

/***********************************************************************
 *		DllUnregisterServer (DINPUT.@)
 */
HRESULT WINAPI DINPUT_DllUnregisterServer(void)
{
    FIXME("(void): stub\n");

    return S_OK;
}
