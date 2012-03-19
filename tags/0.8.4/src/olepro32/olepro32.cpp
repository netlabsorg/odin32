/* $Id: olepro32.cpp,v 1.3 2003-07-28 11:29:30 sandervl Exp $ */
/*
 * OlePro32 Stubs
 *
 * Copyright 1999 Corel Corporation
 *
 * Sean Langley
 */

#include "olepro32.h"
#include "olectl.h"
#include <odinwrap.h>


ODINDEBUGCHANNEL(olepro32)

/*****************************************************
 * OleIconToCursor
 */
HCURSOR WINAPI OleIconToCursor( HINSTANCE hinstExe, HICON hicon)
{
	dprintf(("(%x,%x), not implemented (olepro32.dll)\n",hinstExe,hicon));
	return S_OK;
}

/*****************************************************
 * OleCreatePropertyFrameIndirect
 */
HRESULT WINAPI OleCreatePropertyFrameIndirect( LPOCPFIPARAMS lpParams)
{
	dprintf(("(%p), not implemented (olepro32.dll)\n",lpParams));
	return S_OK;
}

/*****************************************************
 * OleCreatePropertyFrame
 */
HRESULT WINAPI OleCreatePropertyFrame( HWND hwndOwner, UINT x, UINT y,
			LPCOLESTR lpszCaption,ULONG cObjects, LPUNKNOWN* ppUnk,
			ULONG cPages, LPCLSID pPageClsID, LCID lcid,
			DWORD dwReserved, LPVOID pvReserved )
{
	dprintf(("(%x,%d,%d,%p,%ld,%p,%ld,%p,%x,%ld,%p), not implemented (olepro32.dll)\n",
		hwndOwner,x,y,lpszCaption,cObjects,ppUnk,cPages,pPageClsID,
		(int)lcid,dwReserved,pvReserved));
	return S_OK;
}

/*****************************************************
 * OleLoadPicture
 */
HRESULT WINAPI OleLoadPicture( LPSTREAM lpstream, LONG lSize, BOOL fRunmode,
		            REFIID reed, LPVOID *lplpvObj )
{
	dprintf(("(%p,%ld,%d,%p,%p), not implemented (olepro32.dll) \n",
		lpstream,lSize,fRunmode,reed,lplpvObj
	));
	return S_OK;
}

HRESULT WINAPI OLEPRO32_DllUnregisterServer()
{
	dprintf(("not implemented (olepro32.dll) \n"));
	return S_OK;
}

HRESULT WINAPI OLEPRO32_DllRegisterServer()
{
	dprintf(("not implemented (olepro32.dll) \n"));
	return S_OK;
}

HRESULT WINAPI OLEPRO32_DllCanUnloadNow( )
{
	dprintf(("not implemented (olepro32.dll) \n"));
	return S_OK;
}

HRESULT WINAPI OLEPRO32_DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID* ppv )
{
	dprintf(("not implemented (olepro32.dll) \n"));
	return S_OK;
}
