/* $Id: olesvr32.cpp,v 1.4 2000-08-02 16:32:43 bird Exp $ */
/*
 *	OLESVR32 library
 *
 *	Copyright 1995	Martin von Loewis
 *	Copyright 1999  Jens Wiessner
 *
 *	At the moment, these are only empty stubs.
 */

#include <os2win.h>
#include <odinwrap.h>
#include <ole.h>
#include "olesvr32.h"

ODINDEBUGCHANNEL(olesvr32)

/******************************************************************************
 * OleRegisterServer	[OLESVR32.2]
 */
OLESTATUS WINAPI OleRegisterServer(LPCSTR svrname,LPOLESERVER olesvr,LHSERVER* hRet,HINSTANCE hinst,OLE_SERVER_USE osu)
{
#ifdef DEBUG
        dprintf(("OLESVR32: OleRegisterServer not implemented\n"));
#endif
    	*hRet=++OLE_current_handle;
	return OLE_OK;
}

/******************************************************************************
 * OleRevokeServer	[OLESVR32.3]
 */
OLESTATUS WINAPI OleRevokeServer(LHSERVER hServer)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleRevokeServer not implemented\n"));
#endif
    return OLE_OK;
}

/******************************************************************************
 * OleBlockServer	[OLESVR32.4]
 */
OLESTATUS WINAPI OleBlockServer(LHSERVER hServer)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleBlockServer not implemented\n"));
#endif
    return OLE_OK;
}


/******************************************************************************
 * OleUnblockServer	[OLESVR32.5]
 */
OLESTATUS WINAPI OleUnblockServer(LHSERVER hServer, BOOL *block)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleUnblockServer not implemented\n"));
#endif
    *block=FALSE;
    return OLE_OK;
}

/******************************************************************************
 * OleRegisterServerDoc [OLESVR32.6]
 */
INT WINAPI OleRegisterServerDoc( LHSERVER hServer, LPCSTR docname,
                                         LPOLESERVERDOC document,
                                         LHSERVERDOC *hRet)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleRegisterServerDoc not implemented\n"));
#endif
    *hRet=++OLE_current_handle;
    return OLE_OK;
}

/******************************************************************************
 * OleRevokeServerDoc	[OLESVR32.7]
 */
OLESTATUS WINAPI OleRevokeServerDoc(LHSERVERDOC hServerDoc)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleRevokeServerDoc not implemented\n"));
#endif
    return OLE_OK;
}

/******************************************************************************
 * OleRenameServerDoc	[OLESVR32.8]
 */
OLESTATUS WINAPI OleRenameServerDoc(LHSERVERDOC hDoc, LPCSTR newName)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleRenameServerDoc not implemented\n"));
#endif
    return OLE_OK;
}

/******************************************************************************
 * OleRevertServerDoc	[OLESVR32.9]
 */
OLESTATUS   WINAPI  OleRevertServerDoc(LHSERVERDOC hDoc)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleRevertServerDoc not implemented\n"));
#endif
    return OLE_OK;
}

/******************************************************************************
 * OleSavedServerDoc	[OLESVR32.10]
 */
OLESTATUS   WINAPI  OleSavedServerDoc(LHSERVERDOC hDoc)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleSavedServerDoc not implemented\n"));
#endif
    return OLE_OK;
}

/******************************************************************************
 * OleRevokeObject	[OLESVR32.11]
 */
OLESTATUS   WINAPI  OleRevokeObject(LPOLECLIENT oClient)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleRevokeObject not implemented\n"));
#endif
    return OLE_OK;
}

/******************************************************************************
 * OleQueryServerVersion[OLESVR32.12]
 */
DWORD       WINAPI  OleQueryServerVersion(void)
{
#ifdef DEBUG
    dprintf(("OLESVR32: OleQueryServerVersion not implemented\n"));
#endif
    return OLE_OK;
}
