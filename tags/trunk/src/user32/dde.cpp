/* $Id: dde.cpp,v 1.10 2000-02-16 14:34:14 sandervl Exp $ */

/*
 * Win32 default window API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#define DBG_LOCALLOG	DBG_dde
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeAbandonTransaction( DWORD arg1, HCONV arg2, DWORD  arg3)
{
    dprintf(("USER32:  DdeAbandonTransaction\n"));
    return O32_DdeAbandonTransaction(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
PBYTE WIN32API DdeAccessData(HDDEDATA arg1, PDWORD  arg2)
{
    dprintf(("USER32:  DdeAccessData\n"));
    return O32_DdeAccessData(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDDEDATA WIN32API DdeAddData( HDDEDATA arg1, PVOID arg2, DWORD arg3, DWORD  arg4)
{
    dprintf(("USER32:  DdeAddData"));
    return O32_DdeAddData(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HDDEDATA WIN32API DdeClientTransaction(PVOID arg1, DWORD arg2, HCONV arg3, 
                                       HSZ arg4, UINT arg5, UINT arg6, DWORD arg7, 
                                       PDWORD  arg8)
{

    dprintf(("USER32:  DdeClientTransaction\n"));

    return O32_DdeClientTransaction(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
int WIN32API DdeCmpStringHandles( HSZ arg1, HSZ  arg2)
{

    dprintf(("USER32:  DdeCmpStringHandles\n"));

    return O32_DdeCmpStringHandles(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HCONV WIN32API DdeConnect( DWORD arg1, HSZ arg2, HSZ arg3, LPCONVCONTEXT  arg4)
{
 HCONV rc;

    rc = O32_DdeConnect(arg1, arg2, arg3, arg4);
    dprintf(("USER32: DdeConnect %x %x returned %d (%x)", arg2, arg3, rc, DdeGetLastError(arg1)));
    return rc;
}
//******************************************************************************
//******************************************************************************
HCONVLIST WIN32API DdeConnectList(DWORD arg1, HSZ arg2, HSZ arg3, HCONVLIST arg4, LPCONVCONTEXT  arg5)
{

    dprintf(("USER32:  DdeConnectList\n"));

    return O32_DdeConnectList(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
HDDEDATA WIN32API DdeCreateDataHandle(DWORD arg1, PVOID arg2, DWORD arg3, DWORD arg4, 
                                      HSZ arg5, UINT arg6, UINT  arg7)
{
    dprintf(("USER32:  DdeCreateDataHandle\n"));
    return O32_DdeCreateDataHandle(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
HSZ WIN32API DdeCreateStringHandleA(DWORD arg1, LPCSTR arg2, int  arg3)
{
 HSZ rc;

    rc = O32_DdeCreateStringHandle(arg1, arg2, arg3);
    dprintf(("USER32: OS2DdeCreateStringHandleA %s returned %x", arg2, rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
HSZ WIN32API DdeCreateStringHandleW(DWORD arg1, LPCWSTR arg2, int arg3)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HSZ   rc;

    dprintf(("USER32:  DdeCreateStringHandleW DOESN'T WORK\n"));
    rc = O32_DdeCreateStringHandle(arg1, astring, arg3);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeDisconnect(HCONV arg1)
{

    dprintf(("USER32:  DdeDisconnect\n"));

    return O32_DdeDisconnect(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeDisconnectList( HCONVLIST arg1)
{

    dprintf(("USER32:  DdeDisconnectList\n"));

    return O32_DdeDisconnectList(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeEnableCallback(DWORD arg1, HCONV arg2, UINT  arg3)
{

    dprintf(("USER32:  DdeEnableCallback\n"));

    return O32_DdeEnableCallback(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeFreeDataHandle( HDDEDATA arg1)
{

    dprintf(("USER32:  DdeFreeDataHandle\n"));

    return O32_DdeFreeDataHandle(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeFreeStringHandle(DWORD arg1, HSZ  arg2)
{

    dprintf(("USER32:  DdeFreeStringHandle\n"));

    return O32_DdeFreeStringHandle(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DdeGetData( HDDEDATA arg1, PVOID arg2, DWORD arg3, DWORD  arg4)
{

    dprintf(("USER32:  DdeGetData\n"));

    return O32_DdeGetData(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DdeGetLastError(DWORD arg1)
{

    dprintf(("USER32:  DdeGetLastError\n"));

    return O32_DdeGetLastError(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DdeInitializeA(PDWORD arg1, PFNCALLBACK arg2, DWORD arg3, DWORD  arg4)
{
 UINT rc;

    rc = O32_DdeInitialize(arg1, arg2, arg3, arg4);

    dprintf(("USER32:  DdeInitializeA\n"));
    return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DdeInitializeW(PDWORD arg1, PFNCALLBACK arg2, DWORD arg3, DWORD  arg4)
{

    dprintf(("USER32:  DdeInitializeW\n"));

    // NOTE: This will not work as is (needs UNICODE support)
    return O32_DdeInitialize(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeKeepStringHandle(DWORD arg1, HSZ  arg2)
{

    dprintf(("USER32:  DdeKeepStringHandle\n"));

    return O32_DdeKeepStringHandle(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDDEDATA WIN32API DdeNameService( DWORD arg1, HSZ  arg2, HSZ  arg3, UINT  arg4)
{

    dprintf(("USER32:  DdeNameService\n"));

    return O32_DdeNameService(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdePostAdvise(DWORD arg1, HSZ arg2, HSZ  arg3)
{

    dprintf(("USER32:  DdePostAdvise\n"));

    return O32_DdePostAdvise(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DdeQueryConvInfo( HCONV arg1, DWORD arg2, LPCONVINFO  arg3)
{

    dprintf(("USER32:  DdeQueryConvInfo\n"));

    return O32_DdeQueryConvInfo(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HCONV WIN32API DdeQueryNextServer( HCONVLIST arg1, HCONV  arg2)
{

    dprintf(("USER32:  DdeQueryNextServer\n"));

    return O32_DdeQueryNextServer(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DdeQueryStringA(DWORD arg1, HSZ arg2, LPSTR arg3, DWORD arg4, int  arg5)
{

    dprintf(("USER32:  DdeQueryStringA\n"));

    return O32_DdeQueryString(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DdeQueryStringW(DWORD arg1, HSZ arg2, LPWSTR arg3, DWORD arg4, int  arg5)
{
 char *astring = UnicodeToAsciiString(arg3);
 DWORD rc;

    dprintf(("USER32:  DdeQueryStringW\n"));
    rc = O32_DdeQueryString(arg1, arg2, astring, arg4, arg5);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HCONV WIN32API DdeReconnect( HCONV arg1)
{

    dprintf(("USER32:  DdeReconnect\n"));

    return O32_DdeReconnect(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeSetUserHandle( HCONV arg1, DWORD arg2, DWORD  arg3)
{

    dprintf(("USER32:  DdeSetUserHandle\n"));

    return O32_DdeSetUserHandle(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeUnaccessData( HDDEDATA arg1)
{

    dprintf(("USER32:  DdeUnaccessData\n"));

    return O32_DdeUnaccessData(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeUninitialize(DWORD arg1)
{

    dprintf(("USER32:  DdeUninitialize\n"));

    return O32_DdeUninitialize(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FreeDDElParam( UINT arg1, LONG  arg2)
{

    dprintf(("USER32:  FreeDDElParam\n"));

    return O32_FreeDDElParam(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API PackDDElParam(UINT arg1, UINT arg2, UINT  arg3)
{

    dprintf(("USER32:  PackDDElParam\n"));

    return O32_PackDDElParam(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API ReuseDDElParam( UINT arg1, UINT arg2, UINT arg3, UINT arg4, UINT arg5)
{

    dprintf(("USER32:  ReuseDDElParam\n"));

    return O32_ReuseDDElParam(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnpackDDElParam(UINT arg1, UINT arg2, LPUINT arg3, LPUINT  arg4)
{

    dprintf(("USER32:  UnpackDDElParam\n"));

    return O32_UnpackDDElParam(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
/*****************************************************************************
 * Name      : BOOL WIN32API DdeImpersonateClient
 * Purpose   : The DdeImpersonateClient function impersonates a dynamic data
 *             exchange (DDE) client application in a DDE client conversation.
 * Parameters: HCONV hConv handle of the DDE conversation
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get
 *             extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API DdeImpersonateClient(HCONV hConv)
{
  dprintf(("USER32:DdeImpersonateClient (%08x) not implemented.\n",
         hConv));

  return (TRUE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API DdeSetQualityOfService
 * Purpose   : The DdeSetQualityOfService function specifies the quality of
 *             service a raw DDE application desires for future DDE conversations
 *             it initiates.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get
 *             extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API DdeSetQualityOfService (HWND hwndClient,
                                         CONST SECURITY_QUALITY_OF_SERVICE *pqosNew,
                                         PSECURITY_QUALITY_OF_SERVICE pqosPrev)
{
  dprintf(("USER32:DdeSetQualityOfService(%08xh,%08xh,%08x) not implemented.\n",
         hwndClient,
         pqosNew,
         pqosPrev));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API ImpersonateDdeClientWindow
 * Purpose   : The ImpersonateDdeClientWindow function enables a DDE server
 *             application to impersonate a DDE client application's security
 *             context in order to protect secure server data from unauthorized
 *             DDE clients.
 * Parameters: HWND hWndClient handle of DDE client window
 *             HWND hWndServer handle of DDE server window
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

BOOL WIN32API ImpersonateDdeClientWindow(HWND hWndClient,
                                            HWND hWndServer)
{
  dprintf(("USER32:ImpersonateDdeClientWindow (%08xh,%08x) not implemented.\n",
         hWndClient,
         hWndServer));

  return (FALSE);
}
