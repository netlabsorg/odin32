/*
 *	OLESVR32 library
 *
 *	Copyright 1995	Martin von Loewis
 *	Copyright 1999  Jens Wiessner
 */

/*	At the moment, these are only empty stubs.
 */

#include <os2win.h>
#include <ole.h>
#include "olesvr32.h"
#include "debugtools.h"
#include <debugdefs.h>

DEFAULT_DEBUG_CHANNEL(ole)

/******************************************************************************
 * OleRegisterServer32 [OLESVR32.2]
 */
OLESTATUS WINAPI OleRegisterServer(LPCSTR svrname,LPOLESERVER olesvr,LHSERVER* hRet,HINSTANCE hinst,OLE_SERVER_USE osu) {
	FIXME("(%s,%p,%p,%08x,%d): stub!\n",svrname,olesvr,hRet,hinst,osu);
    	*hRet=++OLE_current_handle;
	return OLE_OK;
}

/******************************************************************************
 *		OleRevokeServer32	[OLESVR32.3]
 */
OLESTATUS WINAPI OleRevokeServer(LHSERVER hServer)
{
    FIXME("%ld - stub\n",hServer);
    return OLE_OK;
}

/******************************************************************************
 *		OleBlockServer32	[OLESVR32.4]
 */
OLESTATUS WINAPI OleBlockServer(LHSERVER hServer)
{
    FIXME("(%ld): stub\n",hServer);
    return OLE_OK;
}


/******************************************************************************
 *		OleUnblockServer32	[OLESVR32.5]
 */
OLESTATUS WINAPI OleUnblockServer(LHSERVER hServer, BOOL *block)
{
    FIXME("(%ld): stub\n",hServer);
    /* no more blocked messages :) */
    *block=FALSE;
    return OLE_OK;
}

/******************************************************************************
 * OleRegisterServerDoc32 [OLESVR32.6]
 */
INT WINAPI OleRegisterServerDoc( LHSERVER hServer, LPCSTR docname,
                                         LPOLESERVERDOC document,
                                         LHSERVERDOC *hRet)
{
    FIXME("(%ld,%s): stub\n", hServer, docname);
    *hRet=++OLE_current_handle;
    return OLE_OK;
}

/******************************************************************************
 *		OleRevokeServerDoc32	[OLESVR32.7]
 */
OLESTATUS WINAPI OleRevokeServerDoc(LHSERVERDOC hServerDoc)
{
    FIXME("(%ld): stub\n",hServerDoc);
    return OLE_OK;
}

/******************************************************************************
 *		OleRenameServerDoc32	[OLESVR32.8]
 *
 */
OLESTATUS WINAPI OleRenameServerDoc(LHSERVERDOC hDoc, LPCSTR newName)
{
    FIXME("(%ld,%s): stub.\n",hDoc, newName);
    return OLE_OK;
}

/******************************************************************************
 *		OleRevertServerDoc	[OLESVR32.9]
 *
 */
OLESTATUS   WINAPI  OleRevertServerDoc(LHSERVERDOC hDoc)
{
    FIXME("(%ld): stub\n",hDoc);
    return OLE_OK;
}

/******************************************************************************
 *		OleSavedServerDoc	[OLESVR32.10]
 *
 */
OLESTATUS   WINAPI  OleSavedServerDoc(LHSERVERDOC hDoc)
{
    FIXME("(%ld): stub\n",hDoc);
    return OLE_OK;
}

/******************************************************************************
 *		OleRevokeObject		[OLESVR32.11]
 *
 */
OLESTATUS   WINAPI  OleRevokeObject(LPOLECLIENT oClient)
{
    FIXME("(%ld): stub\n",oClient);
    return OLE_OK;
}

/******************************************************************************
 *		OleQueryServerVersion	[OLESVR32.12]
 *
 */
DWORD       WINAPI  OleQueryServerVersion(void)
{
    return OLE_OK;
}
