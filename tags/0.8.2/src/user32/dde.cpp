/* $Id: dde.cpp,v 1.16 2003-01-21 11:47:46 sandervl Exp $ */

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
#include <unicode.h>
#include <ddeml.h>
#include <dde.h>
#include <odinwrap.h>
#include <dbgwrap.h>

#define DBG_LOCALLOG	DBG_dde
#include "dbglocal.h"

#define DBGWRAP_MODULE "USER32"
#include <dbgwrap.h>

#ifdef DEBUG
#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_dde
DEBUGWRAP12(DdeAbandonTransaction)
DEBUGWRAP8(DdeAccessData)
DEBUGWRAP16(DdeAddData)
DEBUGWRAP32(DdeClientTransaction)
DEBUGWRAP8(DdeCmpStringHandles)
DEBUGWRAP16(DdeConnect)
DEBUGWRAP20(DdeConnectList)
DEBUGWRAP28(DdeCreateDataHandle)
DEBUGWRAP12(DdeCreateStringHandleA)
DEBUGWRAP12(DdeCreateStringHandleW)
DEBUGWRAP4(DdeDisconnect)
DEBUGWRAP4(DdeDisconnectList)
DEBUGWRAP12(DdeEnableCallback)
DEBUGWRAP4(DdeFreeDataHandle)
DEBUGWRAP8(DdeFreeStringHandle)
DEBUGWRAP16(DdeGetData)
DEBUGWRAP4(DdeGetLastError)
DEBUGWRAP4(DdeImpersonateClient)
DEBUGWRAP16(DdeInitializeA)
DEBUGWRAP16(DdeInitializeW)
DEBUGWRAP8(DdeKeepStringHandle)
DEBUGWRAP16(DdeNameService)
DEBUGWRAP12(DdePostAdvise)
DEBUGWRAP12(DdeQueryConvInfo)
DEBUGWRAP8(DdeQueryNextServer)
DEBUGWRAP20(DdeQueryStringA)
DEBUGWRAP20(DdeQueryStringW)
DEBUGWRAP4(DdeReconnect)
DEBUGWRAP12(DdeSetQualityOfService)
DEBUGWRAP12(DdeSetUserHandle)
DEBUGWRAP4(DdeUnaccessData)
DEBUGWRAP4(DdeUninitialize)
DEBUGWRAP8(FreeDDElParam)
DEBUGWRAP8(ImpersonateDdeClientWindow)
DEBUGWRAP12(PackDDElParam)
DEBUGWRAP20(ReuseDDElParam)
DEBUGWRAP16(UnpackDDElParam)
#endif

//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeAbandonTransaction( DWORD arg1, HCONV arg2, DWORD  arg3)
{
    dprintf(("USER32: DdeAbandonTransaction %x %x %x", arg1, arg2, arg3));
    return O32_DdeAbandonTransaction(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
PBYTE WIN32API DdeAccessData(HDDEDATA arg1, PDWORD  arg2)
{
    dprintf(("USER32: DdeAccessData %x %x", arg1, arg2));
    return O32_DdeAccessData(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDDEDATA WIN32API DdeAddData( HDDEDATA arg1, LPBYTE arg2, DWORD arg3, DWORD  arg4)
{
    dprintf(("USER32: DdeAddData %x %x %x %x", arg1, arg2, arg3, arg4));
    return O32_DdeAddData(arg1, (PVOID)arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HDDEDATA WIN32API DdeClientTransaction(LPBYTE arg1, DWORD arg2, HCONV arg3, 
                                       HSZ arg4, UINT arg5, UINT arg6, DWORD arg7, 
                                       LPDWORD arg8)
{
    dprintf(("USER32: DdeClientTransaction %x %x %x %x %x %x %x %x", arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));

    return O32_DdeClientTransaction((PVOID)arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
int WIN32API DdeCmpStringHandles( HSZ arg1, HSZ  arg2)
{
    dprintf(("USER32: DdeCmpStringHandles %x %x", arg1, arg2));

    return O32_DdeCmpStringHandles(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HCONV WIN32API DdeConnect(DWORD idInst, HSZ hszService, HSZ hszTopic, 
                          LPCONVCONTEXT pCC)
{
 HCONV rc;

    rc = O32_DdeConnect(idInst, hszService, hszTopic, pCC);
    dprintf(("USER32: DdeConnect %x %x %x %x returned %d (%x)", idInst, hszService, hszTopic, pCC, rc, DdeGetLastError(idInst)));
    return rc;
}
//******************************************************************************
//******************************************************************************
HCONVLIST WIN32API DdeConnectList(DWORD arg1, HSZ arg2, HSZ arg3, HCONVLIST arg4, LPCONVCONTEXT  arg5)
{

    dprintf(("USER32: DdeConnectList %x %x %x %x %x", arg1, arg2, arg3, arg4, arg5));

    return O32_DdeConnectList(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
HDDEDATA WIN32API DdeCreateDataHandle(DWORD arg1, LPBYTE arg2, DWORD arg3, DWORD arg4, 
                                      HSZ arg5, UINT arg6, UINT  arg7)
{
    dprintf(("USER32: DdeCreateDataHandle %x %x %x %x %x %x %x", arg1, arg2, arg3, arg4, arg5, arg6, arg7));
    return O32_DdeCreateDataHandle(arg1, (PVOID)arg2, arg3, arg4, arg5, arg6, arg7);
}
//******************************************************************************
//******************************************************************************
HSZ WIN32API DdeCreateStringHandleA(DWORD idInst, LPCSTR lpszString, int codepage)
{
 HSZ rc;

    rc = O32_DdeCreateStringHandle(idInst, lpszString, codepage);
    dprintf(("USER32: DdeCreateStringHandleA %x %s %x returned %x", idInst, lpszString, codepage, rc));
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
    dprintf(("USER32: DdeDisconnect %x", arg1));

    return O32_DdeDisconnect(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeDisconnectList( HCONVLIST arg1)
{
    dprintf(("USER32: DdeDisconnectList %x", arg1));

    return O32_DdeDisconnectList(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeEnableCallback(DWORD arg1, HCONV arg2, UINT  arg3)
{
    dprintf(("USER32: DdeEnableCallback %x %x %x", arg1, arg2, arg3));

    return O32_DdeEnableCallback(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeFreeDataHandle( HDDEDATA arg1)
{
    dprintf(("USER32: DdeFreeDataHandle %x", arg1));

    return O32_DdeFreeDataHandle(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeFreeStringHandle(DWORD arg1, HSZ  arg2)
{
    dprintf(("USER32: DdeFreeStringHandle %x %x", arg1, arg2));

    return O32_DdeFreeStringHandle(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DdeGetData( HDDEDATA arg1, LPBYTE arg2, DWORD arg3, DWORD  arg4)
{
    dprintf(("USER32: DdeGetData %x %x %x %x", arg1, arg2, arg3, arg4));

    return O32_DdeGetData(arg1, (PVOID)arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DdeGetLastError(DWORD arg1)
{
    dprintf(("USER32: DdeGetLastError %x", arg1));

    return O32_DdeGetLastError(arg1);
}
#ifdef USING_OPEN32
//******************************************************************************
//******************************************************************************
HDDEDATA EXPENTRY_O32 DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, 
                                  HSZ hsz2, HDDEDATA hdata, DWORD dwData1, 
                                  DWORD dwData2) 
{
   dprintf(("DdeCallback %x %x %x %x %x %x %x %x", uType, uFmt, hconv, hsz1, hsz2,
            hdata, dwData1, dwData2));
   return 0;
}
#endif
//******************************************************************************
//******************************************************************************
UINT WIN32API DdeInitializeA(PDWORD lpidInst, PFNCALLBACK pfnCallback, 
                             DWORD afCmd, DWORD ulRes)
{
 UINT ret;

#ifdef USING_OPEN32
    ret = O32_DdeInitialize(lpidInst, (PFNCALLBACK_O32)DdeCallback, afCmd, ulRes);
#else
    ret = O32_DdeInitialize(lpidInst, (PFNCALLBACK_O32)pfnCallback, afCmd, ulRes);
#endif
    dprintf(("USER32: DdeInitialize %x %x %x %x returned %x", lpidInst, pfnCallback, afCmd, ulRes, ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API DdeInitializeW(PDWORD lpidInst, PFNCALLBACK pfnCallback, 
                             DWORD afCmd, DWORD ulRes)
{
 UINT ret;

    // NOTE: This will not work as is (needs UNICODE support)
#ifdef USING_OPEN32
    ret = O32_DdeInitialize(lpidInst, (PFNCALLBACK_O32)DdeCallback, afCmd, ulRes);
#else
    ret = O32_DdeInitialize(lpidInst, (PFNCALLBACK_O32)pfnCallback, afCmd, ulRes);
#endif
    dprintf(("USER32: DdeInitializeW %x %x %x %x returned %x", lpidInst, pfnCallback, afCmd, ulRes, ret));
    return ret;
//    return O32_DdeInitialize(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DdeKeepStringHandle(DWORD arg1, HSZ arg2)
{
    dprintf(("USER32: DdeKeepStringHandle %x %x", arg1, arg2));

    return O32_DdeKeepStringHandle(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HDDEDATA WIN32API DdeNameService( DWORD arg1, HSZ  arg2, HSZ  arg3, UINT  arg4)
{
    dprintf(("USER32: DdeNameService %x %x %x %x", arg1, arg2, arg3, arg4));

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
LPARAM WIN32API ReuseDDElParam(LPARAM arg1, UINT arg2, UINT arg3, UINT arg4, UINT arg5)
{
    dprintf(("USER32:  ReuseDDElParam\n"));

    return O32_ReuseDDElParam(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnpackDDElParam(UINT arg1, LPARAM arg2, PUINT arg3, PUINT arg4)
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
