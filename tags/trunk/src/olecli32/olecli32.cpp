/* $Id: olecli32.cpp,v 1.3 1999-11-12 11:38:42 sandervl Exp $ */
/*
 *	OLECLI32 library
 *
 *	Copyright 1995	Martin von Loewis
 *	Copyright 1999  Jens Wiessner
 */

/*	At the moment, these are only empty stubs.
 */

#include <os2win.h>
#include <odinwrap.h>
#include <ole.h>
#include "olecli32.h"

ODINDEBUGCHANNEL(olecli32)

/******************************************************************************
 *		OleDelete		[OLECLI32.2]
 */
OLESTATUS WINAPI OleDelete(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleDelete not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleSaveToStream		[OLECLI32.3]
 */
OLESTATUS WINAPI OleSaveToStream(LPOLEOBJECT oleob, LPOLESTREAM olest)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleSaveToStream not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleLoadFromStream 	[OLECLI32.4]
 */
OLESTATUS WINAPI OleLoadFromStream(LPOLESTREAM olest, LPCSTR str1, LPOLECLIENT olecl, LHCLIENTDOC cldoc, LPCSTR str2, LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleLoadFromStream not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleClone 		[OLECLI32.6]
 */
OLESTATUS WINAPI OleClone(LPOLEOBJECT oleob, LPOLECLIENT olecl, LHCLIENTDOC cldoc, LPCSTR str1, LPOLEOBJECT oleob2)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleClone not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleCopyFromLink 	[OLECLI32.7]
 */
OLESTATUS WINAPI OleCopyFromLink(LPOLEOBJECT oleob, LPCSTR str1, LPOLECLIENT olecl, LHCLIENTDOC cldoc, LPCSTR str2, LPOLEOBJECT oleob2)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleCopyFromLink not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleEqual		[OLECLI32.8]
 */
OLESTATUS WINAPI OleEqual(LPOLEOBJECT oleob, LPOLEOBJECT oleob2)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleEqual not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *           OleQueryLinkFromClip	[OLECLI32.9]
 */
OLESTATUS WINAPI OleQueryLinkFromClip(LPCSTR name,OLEOPT_RENDER render,OLECLIPFORMAT clipformat)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryLinkFromClip not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *           OleQueryCreateFromClip	[OLECLI32.10]
 */
OLESTATUS WINAPI OleQueryCreateFromClip(LPCSTR name,OLEOPT_RENDER render,OLECLIPFORMAT clipformat)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryCreateFromClip not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *           OleCreateLinkFromClip	[OLECLI32.11]
 */
OLESTATUS WINAPI OleCreateLinkFromClip( 
	LPCSTR name,LPOLECLIENT olecli,LHCLIENTDOC hclientdoc,LPCSTR xname,
	LPOLEOBJECT *lpoleob,OLEOPT_RENDER render,OLECLIPFORMAT clipformat)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleCreateLinkFromClip not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleCreateFromClip	[OLECLI32.12]
 */
OLESTATUS WINAPI OleCreateFromClip(
	LPCSTR name,LPOLECLIENT olecli,LHCLIENTDOC hclientdoc,LPCSTR xname,
	LPOLEOBJECT *lpoleob,OLEOPT_RENDER render, OLECLIPFORMAT clipformat)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleCreateFromClip not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleCopyToClipboard	[OLECLI32.13]
 */
OLESTATUS WINAPI OleCopyToClipboard(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleCopyToClipboard not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryType		[OLECLI32.14]
 */
OLESTATUS WINAPI OleQueryType(LPOLEOBJECT oleob,LONG*xlong)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryType not implemented\n"));
#endif
  	if (!oleob)
  		return OLE_ERROR_METAFILE;
//	dprintf(("Calling OLEOBJECT.QueryType (%p) (%p,%p)\n",
//	      oleob->lpvtbl->QueryType,oleob,xlong));
	return OLE_ERROR_METAFILE;
//	return oleob->lpvtbl->QueryType(oleob,xlong);
}


/******************************************************************************
 *		OleSetHostNames		[OLECLI32.15]
 */
OLESTATUS WINAPI OleSetHostNames(LPOLEOBJECT oleob,LPCSTR name1,LPCSTR name2)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleSetHostNames not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleSetTargetDevice	[OLECLI32.16]
 */
OLESTATUS WINAPI OleSetTargetDevice(LPOLEOBJECT oleob, HGLOBAL hglobal)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleSetTargetDevice not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleSetBounds		[OLECLI32.17]
 */
OLESTATUS WINAPI OleSetBounds(LPOLEOBJECT oleob, const RECT rec)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleSetTargetDevice not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryBounds		[OLECLI32.18]
 */
OLESTATUS WINAPI OleQueryBounds(LPOLEOBJECT oleob, RECT rec)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryBounds not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleDraw			[OLECLI32.19]
 */
OLESTATUS WINAPI OleDraw(LPOLEOBJECT oleob, HDC hdc, const RECT rec, const RECT rec2, HDC hdc2)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleDraw not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryOpen		[OLECLI32.20]
 */
OLESTATUS WINAPI OleQueryOpen(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryOpen not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleActivate	[OLECLI32.21]
 */
OLESTATUS WINAPI OleActivate(LPOLEOBJECT oleob, UINT uin, BOOL boo, BOOL boo2, HWND hwnd, const RECT rec)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleActivate not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleUpdate	[OLECLI32.22]
 */
OLESTATUS WINAPI OleUpdate(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleUpdate not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleReconnect	[OLECLI32.23]
 */
OLESTATUS WINAPI OleReconnect(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleReconnect not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleGetLinkUpdateOptions	[OLECLI32.24]
 */
OLESTATUS WINAPI OleGetLinkUpdateOptions(LPOLEOBJECT oleob, OLEOPT_UPDATE oleopt)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleGetLinkUpdateOptions not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleSetLinkUpdateOptions	[OLECLI32.25]
 */
OLESTATUS WINAPI OleSetLinkUpdateOptions(LPOLEOBJECT oleob, OLEOPT_UPDATE oleopt)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleSetLinkUpdateOptions not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleEnumFormats		[OLECLI32.26]
 */
OLECLIPFORMAT WINAPI OleEnumFormats(LPOLEOBJECT oleob, OLECLIPFORMAT olecf)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleEnumFormats not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleClose		[OLECLI32.27]
 */
OLESTATUS WINAPI OleClose(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleClose not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleGetData		[OLECLI32.28]
 */
OLESTATUS WINAPI OleGetData(LPOLEOBJECT oleob, OLECLIPFORMAT olecf, HANDLE handle)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleGetData not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleSetData		[OLECLI32.29]
 */
OLESTATUS WINAPI OleSetData(LPOLEOBJECT oleob, OLECLIPFORMAT olecf, HANDLE handle)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleSetData not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryProtocol	[OLECLI32.30]
 */
void   WINAPI  OleQueryProtocol(LPOLEOBJECT oleob, LPCSTR str)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryProtocol not implemented\n"));
#endif
	return;
}


/******************************************************************************
 *		OleQueryOutOfDate	[OLECLI32.31]
 */
OLESTATUS WINAPI OleQueryOutOfDate(LPOLEOBJECT)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryOutOfDate not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleObjectConvert	[OLECLI32.32]
 */
OLESTATUS WINAPI OleObjectConvert(LPOLEOBJECT oleob, LPCSTR str, LPOLECLIENT olecl, LHCLIENTDOC cldoc, LPCSTR str2, LPOLEOBJECT oleob2)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleObjectConvert not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleCreateFromTemplate	[OLECLI32.33]
 */
OLESTATUS WINAPI OleCreateFromTemplate(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LHCLIENTDOC cldoc, LPCSTR str3, LPOLEOBJECT oleob, OLEOPT_RENDER optren, OLECLIPFORMAT olecf)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleCreateFromTemplate not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleCreate		[OLECLI32.34]
 */
OLESTATUS WINAPI OleCreate(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LHCLIENTDOC cldoc, LPCSTR str3, LPOLEOBJECT oleob, OLEOPT_RENDER optren, OLECLIPFORMAT olecf)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleCreate not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryReleaseStatus	[OLECLI32.35]
 */
OLESTATUS WINAPI OleQueryReleaseStatus(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryReleaseStatus not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryReleaseError	[OLECLI32.36]
 */
OLESTATUS WINAPI OleQueryReleaseError(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryReleaseError not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleQueryReleaseMethod	[OLECLI32.37]
 */
OLE_RELEASE_METHOD WINAPI OleQueryReleaseMethod(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleQueryReleaseMethod not implemented\n"));
#endif
	return OLE_NONE;
}


/******************************************************************************
 *		OleCreateFromFile	[OLECLI32.38]
 */
OLESTATUS   WINAPI  OleCreateFromFile(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LPCSTR str3, LHCLIENTDOC cldoc, LPCSTR str4, LPOLEOBJECT oleob, OLEOPT_RENDER optren, OLECLIPFORMAT olecf)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleCreateFromFile not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleCreateLinkFromFile	[OLECLI32.39]
 */
OLESTATUS   WINAPI  OleCreateLinkFromFile(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LPCSTR str3, LPCSTR str4, LHCLIENTDOC cldoc, LPCSTR str5, LPOLEOBJECT oleob, OLEOPT_RENDER optren, OLECLIPFORMAT olecf)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleCreateLinkFromFile not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleRelease	[OLECLI32.40]
 */
OLESTATUS WINAPI OleRelease(LPOLEOBJECT oleob)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleRelease not implemented\n"));
#endif
	return OLE_OK;
}


/******************************************************************************
 *		OleRegisterClientDoc  	[OLECLI32.41]
 */
OLESTATUS WINAPI OleRegisterClientDoc(LPCSTR classname, LPCSTR docname,
                                        LONG reserved, LHCLIENTDOC *hRet )
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleRegisterClientDoc not implemented\n"));
#endif
    *hRet=++OLE_current_handle;
    return OLE_OK;
}


/******************************************************************************
 *		OleRevokeClientDoc  	[OLECLI32.42]
 */
OLESTATUS WINAPI OleRevokeClientDoc(LHCLIENTDOC hServerDoc)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleRevokeClientDoc not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleRenameClientDoc  	[OLECLI32.43]
 */
OLESTATUS WINAPI OleRenameClientDoc(LHCLIENTDOC hDoc, LPCSTR newName)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleRenameClientDoc not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleRevertClientDoc  	[OLECLI32.44]
 */
OLESTATUS WINAPI OleRevertClientDoc(LHCLIENTDOC cldoc)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleRevertClientDoc not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleSavedClientDoc  	[OLECLI32.45]
 */
OLESTATUS WINAPI OleSavedClientDoc(LHCLIENTDOC cldoc)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleSavedClientDoc not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleRename	[OLECLI32.46]
 */
OLESTATUS WINAPI OleRename(LPOLEOBJECT oleob, LPCSTR str)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleRename not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleEnumObjects	[OLECLI32.47]
 */
OLESTATUS WINAPI OleEnumObjects(LHCLIENTDOC cldoc, LPOLEOBJECT oleob)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleEnumObjects not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleQueryName	[OLECLI32.48]
 */
OLESTATUS WINAPI OleQueryName(LPOLEOBJECT oleob, LPSTR str, UINT uin)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleQueryName not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleSetColorScheme [OLECLI32.49]
 */
OLESTATUS WINAPI OleSetColorScheme(LPOLEOBJECT oleob, const LOGPALETTE logpal)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleSetColorScheme not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleRequestData	[OLECLI32.50]
 */
OLESTATUS WINAPI OleRequestData(LPOLEOBJECT oleob, OLECLIPFORMAT olecf)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleRequestData not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleLockServer	[OLECLI32.54]
 */
OLESTATUS WINAPI OleLockServer(LPOLEOBJECT oleob, LHSERVER lhserv)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleLockServer not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleUnLockServer	[OLECLI32.55]
 */
OLESTATUS WINAPI OleUnlockServer(LHSERVER lhserv)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleUnlockServer not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleQuerySize	[OLECLI32.56]
 */
OLESTATUS WINAPI OleQuerySize(LPOLEOBJECT oleob, DWORD dw)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleQuerySize not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleExecute	[OLECLI32.57]
 */
OLESTATUS WINAPI OleExecute(LPOLEOBJECT oleob, HGLOBAL hglob, UINT uin)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleExecute not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleCreateInvisible	[OLECLI32.58]
 */
OLESTATUS WINAPI OleCreateInvisible(LPCSTR str, LPOLECLIENT olecl, LPCSTR str2, LHCLIENTDOC cldoc, LPCSTR str3, LPOLEOBJECT oleob, OLEOPT_RENDER olere, OLECLIPFORMAT olecf, BOOL boo)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleCreateInvisible not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 *		OleQueryClientVersion	[OLECLI32.59]
 */
DWORD WINAPI OleQueryClientVersion(void)
{
#ifdef DEBUG
    dprintf(("OLECLI32: OleQueryClientVersion not implemented\n"));
#endif
    return OLE_OK;
}



/******************************************************************************
 *		OleIsDcMeta		[OLECLI32.60]
 */
BOOL WINAPI OleIsDcMeta(HDC hdc)
{
#ifdef DEBUG
        dprintf(("OLECLI32: OleIsDcMeta not implemented\n"));
        dprintf(("(%04x)\n",hdc));
#endif
//	if (GDI_GetObjPtr( hdc, METAFILE_DC_MAGIC ) != 0) {
//	    GDI_HEAP_UNLOCK( hdc );
//	    return TRUE;
//	}
	return FALSE;
}
