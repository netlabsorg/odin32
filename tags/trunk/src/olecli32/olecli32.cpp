/* $Id: olecli32.cpp,v 1.1 1999-08-29 08:15:02 sandervl Exp $ */
/*
 *	OLECLI32 library
 *
 *	Copyright 1995	Martin von Loewis
 *	Copyright 1999  Jens Wiessner
 */

/*	At the moment, these are only empty stubs.
 */

#include <os2win.h>
#include <ole.h>
#include "olecli32.h"
#include "debugtools.h"
#include <debugdefs.h>


DEFAULT_DEBUG_CHANNEL(ole)


/******************************************************************************
 *		OleDelete	[OLECLI32.2]
 */
OLESTATUS WINAPI OleDelete(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleSaveToStream	[OLECLI32.3]
 */
OLESTATUS WINAPI OleSaveToStream(LPOLEOBJECT oleob, LPOLESTREAM olest)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleLoadFromStream [OLECLI32.4]
 */
OLESTATUS WINAPI OleLoadFromStream(LPOLESTREAM olest, LPCSTR str1, LPOLECLIENT olecl, LHCLIENTDOC cldoc, LPCSTR str2, LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleClone [OLECLI32.6]
 */
OLESTATUS WINAPI OleClone(LPOLEOBJECT oleob, LPOLECLIENT olecl, LHCLIENTDOC cldoc, LPCSTR str1, LPOLEOBJECT oleob2)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleCopyFromLink [OLECLI32.7]
 */
OLESTATUS WINAPI OleCopyFromLink(LPOLEOBJECT oleob, LPCSTR str1, LPOLECLIENT olecl, LHCLIENTDOC cldoc, LPCSTR str2, LPOLEOBJECT oleob2)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleEqual	[OLECLI32.8]
 */
OLESTATUS WINAPI OleEqual(LPOLEOBJECT oleob, LPOLEOBJECT oleob2)
{
	return OLE_OK;
}


/******************************************************************************
 *           OleQueryLinkFromClip32	[OLECLI32.9]
 */
OLESTATUS WINAPI OleQueryLinkFromClip(LPCSTR name,OLEOPT_RENDER render,OLECLIPFORMAT clipformat)
{
	return OLE_OK;
}


/******************************************************************************
 *           OleQueryCreateFromClip32	[OLECLI32.10]
 */
OLESTATUS WINAPI OleQueryCreateFromClip(LPCSTR name,OLEOPT_RENDER render,OLECLIPFORMAT clipformat)
{
	return OLE_OK;
}


/******************************************************************************
 *           OleCreateLinkFromClip32	[OLECLI32.11]
 */
OLESTATUS WINAPI OleCreateLinkFromClip( 
	LPCSTR name,LPOLECLIENT olecli,LHCLIENTDOC hclientdoc,LPCSTR xname,
	LPOLEOBJECT *lpoleob,OLEOPT_RENDER render,OLECLIPFORMAT clipformat)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleCreateFromClip32	[OLECLI32.12]
 */
OLESTATUS WINAPI OleCreateFromClip(
	LPCSTR name,LPOLECLIENT olecli,LHCLIENTDOC hclientdoc,LPCSTR xname,
	LPOLEOBJECT *lpoleob,OLEOPT_RENDER render, OLECLIPFORMAT clipformat)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleCopyToClipboard	[OLECLI32.13]
 */
OLESTATUS WINAPI OleCopyToClipboard(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryType32	[OLECLI32.14]
 */
OLESTATUS WINAPI OleQueryType(LPOLEOBJECT oleob,LONG*xlong)
{
//	if (!oleob)
//		return OLE_ERROR_METAFILE;
//	TRACE("Calling OLEOBJECT.QueryType (%p) (%p,%p)\n",
//	      oleob->lpvtbl->QueryType,oleob,xlong);
	return OLE_ERROR_METAFILE;
//	return oleob->lpvtbl->QueryType(oleob,xlong);
}


/******************************************************************************
 *		OleSetHostNames32	[OLECLI32.15]
 */
OLESTATUS WINAPI OleSetHostNames(LPOLEOBJECT oleob,LPCSTR name1,LPCSTR name2)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleSetTargetDevice32	[OLECLI32.16]
 */
OLESTATUS WINAPI OleSetTargetDevice(LPOLEOBJECT oleob, HGLOBAL hglobal)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleSetBounds32	[OLECLI32.17]
 */
OLESTATUS WINAPI OleSetBounds(LPOLEOBJECT oleob, const RECT rec)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryBounds32	[OLECLI32.18]
 */
OLESTATUS WINAPI OleQueryBounds(LPOLEOBJECT oleob, RECT rec)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleDraw32	[OLECLI32.19]
 */
OLESTATUS WINAPI OleDraw(LPOLEOBJECT oleob, HDC hdc, const RECT rec, const RECT rec2, HDC hdc2)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryOpen	[OLECLI32.20]
 */
OLESTATUS WINAPI OleQueryOpen(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleActivate	[OLECLI32.21]
 */
OLESTATUS WINAPI OleActivate(LPOLEOBJECT oleob, UINT uin, BOOL boo, BOOL boo2, HWND hwnd, const RECT rec)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleUpdate	[OLECLI32.22]
 */
OLESTATUS WINAPI OleUpdate(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleReconnect	[OLECLI32.23]
 */
OLESTATUS WINAPI OleReconnect(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleGetLinkUpdateOptions	[OLECLI32.24]
 */
OLESTATUS WINAPI OleGetLinkUpdateOptions(LPOLEOBJECT oleob, OLEOPT_UPDATE oleopt)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleSetLinkUpdateOptions	[OLECLI32.25]
 */
OLESTATUS WINAPI OleSetLinkUpdateOptions(LPOLEOBJECT oleob, OLEOPT_UPDATE oleopt)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleEnumFormats		[OLECLI32.26]
 */
OLECLIPFORMAT WINAPI OleEnumFormats(LPOLEOBJECT oleob, OLECLIPFORMAT olecf)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleClose		[OLECLI32.27]
 */
OLESTATUS WINAPI OleClose(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleGetData		[OLECLI32.28]
 */
OLESTATUS WINAPI OleGetData(LPOLEOBJECT oleob, OLECLIPFORMAT olecf, HANDLE handle)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleSetData		[OLECLI32.29]
 */
OLESTATUS WINAPI OleSetData(LPOLEOBJECT oleob, OLECLIPFORMAT olecf, HANDLE handle)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryProtocol	[OLECLI32.30]
 */
void   WINAPI  OleQueryProtocol(LPOLEOBJECT oleob, LPCSTR str)
{
	return;
}


/******************************************************************************
 *		OleQueryOutOfDate	[OLECLI32.31]
 */
OLESTATUS WINAPI OleQueryOutOfDate(LPOLEOBJECT)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleObjectConvert	[OLECLI32.32]
 */
OLESTATUS WINAPI OleObjectConvert(LPOLEOBJECT oleob, LPCSTR str, LPOLECLIENT olecl, LHCLIENTDOC cldoc, LPCSTR str2, LPOLEOBJECT oleob2)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleCreateFromTemplate	[OLECLI32.33]
 */
OLESTATUS WINAPI OleCreateFromTemplate(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LHCLIENTDOC cldoc, LPCSTR str3, LPOLEOBJECT oleob, OLEOPT_RENDER optren, OLECLIPFORMAT olecf)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleCreate		[OLECLI32.34]
 */
OLESTATUS WINAPI OleCreate(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LHCLIENTDOC cldoc, LPCSTR str3, LPOLEOBJECT oleob, OLEOPT_RENDER optren, OLECLIPFORMAT olecf)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryReleaseStatus	[OLECLI32.35]
 */
OLESTATUS WINAPI OleQueryReleaseStatus(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryReleaseError	[OLECLI32.36]
 */
OLESTATUS WINAPI OleQueryReleaseError(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryReleaseMethod	[OLECLI32.37]
 */
OLE_RELEASE_METHOD WINAPI OleQueryReleaseMethod(LPOLEOBJECT oleob)
{
	return OLE_NONE;
}


/******************************************************************************
 *		OleCreateFromFile	[OLECLI32.38]
 */
OLESTATUS   WINAPI  OleCreateFromFile(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LPCSTR str3, LHCLIENTDOC cldoc, LPCSTR str4, LPOLEOBJECT oleob, OLEOPT_RENDER optren, OLECLIPFORMAT olecf)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleCreateLinkFromFile	[OLECLI32.39]
 */
OLESTATUS   WINAPI  OleCreateLinkFromFile(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LPCSTR str3, LPCSTR str4, LHCLIENTDOC cldoc, LPCSTR str5, LPOLEOBJECT oleob, OLEOPT_RENDER optren, OLECLIPFORMAT olecf)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleRelease	[OLECLI32.40]
 */
OLESTATUS WINAPI OleRelease(LPOLEOBJECT oleob)
{
	return OLE_OK;
}


/******************************************************************************
 *		OleRegisterClientDoc32	[OLECLI32.41]
 */
OLESTATUS WINAPI OleRegisterClientDoc(LPCSTR classname, LPCSTR docname,
                                        LONG reserved, LHCLIENTDOC *hRet )
{
    *hRet=++OLE_current_handle;
    return OLE_OK;
}


/******************************************************************************
 *		OleRevokeClientDoc32	[OLECLI32.42]
 */
OLESTATUS WINAPI OleRevokeClientDoc(LHCLIENTDOC hServerDoc)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleRenameClientDoc32	[OLECLI32.43]
 */
OLESTATUS WINAPI OleRenameClientDoc(LHCLIENTDOC hDoc, LPCSTR newName)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleRevertClientDoc32	[OLECLI32.44]
 */
OLESTATUS WINAPI OleRevertClientDoc(LHCLIENTDOC cldoc)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleSavedClientDoc32	[OLECLI32.45]
 */
OLESTATUS WINAPI OleSavedClientDoc(LHCLIENTDOC cldoc)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleRename	[OLECLI32.46]
 */
OLESTATUS WINAPI OleRename(LPOLEOBJECT oleob, LPCSTR str)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleEnumObjects	[OLECLI32.47]
 */
OLESTATUS WINAPI OleEnumObjects(LHCLIENTDOC cldoc, LPOLEOBJECT oleob)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleQueryName	[OLECLI32.48]
 */
OLESTATUS WINAPI OleQueryName(LPOLEOBJECT oleob, LPSTR str, UINT uin)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleSetColorScheme [OLECLI32.49]
 */
OLESTATUS WINAPI OleSetColorScheme(LPOLEOBJECT oleob, const LOGPALETTE logpal)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleRequestData	[OLECLI32.50]
 */
OLESTATUS WINAPI OleRequestData(LPOLEOBJECT oleob, OLECLIPFORMAT olecf)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleLockServer	[OLECLI32.54]
 */
OLESTATUS WINAPI OleLockServer(LPOLEOBJECT oleob, LHSERVER lhserv)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleUnLockServer	[OLECLI32.55]
 */
OLESTATUS WINAPI OleUnlockServer(LHSERVER lhserv)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleQuerySize	[OLECLI32.56]
 */
OLESTATUS WINAPI OleQuerySize(LPOLEOBJECT oleob, DWORD dw)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleExecute	[OLECLI32.57]
 */
OLESTATUS WINAPI OleExecute(LPOLEOBJECT oleob, HGLOBAL hglob, UINT uin)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleCreateInvisible	[OLECLI32.58]
 */
OLESTATUS WINAPI OleCreateInvisible(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LHCLIENTDOC cldoc, LPCSTR str3, LPOLEOBJECT oleob, OLEOPT_RENDER olere, OLECLIPFORMAT olecf, BOOL boo)
{
    return OLE_OK;
}


/******************************************************************************
 *		OleQueryClientVersion	[OLECLI32.59]
 */
DWORD WINAPI OleQueryClientVersion(void)
{
    return OLE_OK;
}



/******************************************************************************
 *		OleIsDcMeta32	[OLECLI32.60]
 */
BOOL WINAPI OleIsDcMeta(HDC hdc)
{
//        TRACE("(%04x)\n",hdc);
//	if (GDI_GetObjPtr( hdc, METAFILE_DC_MAGIC ) != 0) {
//	    GDI_HEAP_UNLOCK( hdc );
//	    return TRUE;
//	}
	return FALSE;
}
