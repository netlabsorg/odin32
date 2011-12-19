/* $Id: MPR.CPP,v 1.7 2000-08-02 16:24:07 bird Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * MPR apis
 *
 * Copyright 1998 Patrick Haller
 */

/*****************************************************************************
 * Name      : MPR.CPP
 * Purpose   : This module maps all Win32 functions contained in MPR.DLL
 *             to their OS/2-specific counterparts as far as possible.
 *             Basis is  5.05.97  12.00  59152 MPR.DLL (NT4SP3)
 *****************************************************************************/

#include <os2win.h>
#include <wnet.h>
#include <string.h>
#include <odinwrap.h>
#include <heapstring.h>
#include <misc.h>
#include <winnetwk.h>

// Undocumented Stuff
// MPR.DLL of Windows NT

// MultinetGetErrorTextA
// MultinetGetErrorTextW
// RestoreConnectionA0
// WNetClearConnections
// WNetConnectionDialog2
// WNetDirectoryNotifyA
// WNetDirectoryNotifyW
// WNetDisconnectDialog2
// WNetFMXEditPerm         FileManager Extensions ?
// WNetFMXGetPermCaps
// WNetFMXGetPermHelp
// WNetFormatNetworkNameA
// WNetFormatNetworkNameW
// WNetGetConnection2A
// WNetGetConnection2W
// WNetGetConnection3A
// WNetGetConnection3W
// WNetGetDirectoryTypeA
// WNetGetDirectoryTypeW
// WNetGetHomeDirectoryW   NT specific
// WNetGetPropertyTextA
// WNetGetPropertyTextW
// WNetGetSearchDialog
// WNetLogonNotify
// WNetPasswordChangeNotify
// WNetPropertyDialogA
// WNetPropertyDialogW
// WNetRestoreConnectionW
// WNetSetConnectionA
// WNetSetConnectionW
// WNetSupportGlobalEnum


ODINDEBUGCHANNEL(MPR)

/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

static struct _MPRGlobals
{
  DWORD dwLastError;   /* most recent extended error code set by a net func */
} MPRGLOBALS;


/*****************************************************************************
 * Purpose:    The MultinetGetConnectionPerformance finctuion returns information
 *             about the expected performance of a connection used to access a
 *             network resource.
 * Parameters: LPNETRESOURCEA            lpNetResource
 *             LPNETCONNECTIONINFOSTRUCT lpNetConnectInfoStruct
 * Variables :
 * Result    : API-Returncode
 * Remark    : not documented
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 19:55]
 *****************************************************************************/

ODINFUNCTION2(DWORD,                  MultinetGetConnectionPerformanceA,
              LPNETRESOURCEA,         lpNetResource,
              LPNETCONNECTINFOSTRUCT, lpNetConnectInfoStruct)
{
  dprintf(("MPR:MultinetGetConnectionPerformanceA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Purpose:    The MultinetGetConnectionPerformance finctuion returns information
 *             about the expected performance of a connection used to access a
 *             network resource.
 * Parameters: LPNETRESOURCEW            lpNetResource
 *             LPNETCONNECTIONINFOSTRUCT lpNetConnectInfoStruct
 * Variables :
 * Result    : API-Returncode
 * Remark    : not documented
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 19:55]
 *****************************************************************************/

ODINFUNCTION2(DWORD,                  MultinetGetConnectionPerformanceW,
              LPNETRESOURCEW,         lpNetResource,
              LPNETCONNECTINFOSTRUCT, lpNetConnectInfoStruct)
{
  dprintf(("MPR:MultinetGetConnectionPerformanceW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             WNetAddConnection2, makes a connection to a network
 *             resource. The function can redirect a local device to the network
 *             resource.
 * Parameters: LPNETRESOURCE lpNetResource points to structure that specifies
 *                                         connection details
 *             LPCSTR       lpPassword    points to password string
 *             LPCSTR       lpUsername    points to user name string
 *             DWORD         fdwConnection set of bit flags that specify
 *                                         connection options
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION4(DWORD, WNetAddConnection2A,
              LPNETRESOURCEA, lpNetResource,
              LPCSTR, lpPassword,
              LPCSTR, lpUsername,
              DWORD, fdwConnection)
{
  dprintf(("MPR:WNetAddConnection2A not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             WNetAddConnection2 makes a connection to a network
 *             resource. The function can redirect a local device to the network
 *             resource.
 * Parameters: LPNETRESOURCE lpNetResource points to structure that specifies
 *                                         connection details
 *             LPCSTR       lpPassword    points to password string
 *             LPCSTR       lpUsername    points to user name string
 *             DWORD         fdwConnection set of bit flags that specify
 *                                         connection options
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION4(DWORD, WNetAddConnection2W,
              LPNETRESOURCEW, lpNetResource,
              LPCWSTR, lpPassword,
              LPCWSTR, lpUsername,
              DWORD, fdwConnection)
{
  dprintf(("MPR:WNetAddConnection2W not implemented.\n"));
  return (ERROR_NO_NETWORK);
}



/*****************************************************************************
 *             WNetAddConnection3 makes, aconnection to a network
 *             resource. The function can redirect a local device to the network
 *             resource.
 * Parameters: HWND          hwndOwner     handle to an owner window for dialog
 *             LPNETRESOURCE lpNetResource points to structure that specifies
 *                                         connection details
 *             LPCSTR       lpPassword    points to password string
 *             LPCSTR       lpUsername    points to user name string
 *             DWORD         fdwConnection set of bit flags that specify
 *                                         connection options
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION5(DWORD, WNetAddConnection3A,
              HWND, hwndOwner,
              LPNETRESOURCEA, lpNetResource,
              LPCSTR, lpPassword,
              LPCSTR, lpUsername,
              DWORD, fdwConnection)
{
  dprintf(("MPR:WNetAddConnection3A not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             WNetAddConnection3 makes, connection to a network
 *             resource. The function can redirect a local device to the network
 *             resource.
 * Parameters: HWND          hwndOwner     handle to an owner window for dialog
 *             LPNETRESOURCE lpNetResource points to structure that specifies
 *                                         connection details
 *             LPCSTR       lpPassword    points to password string
 *             LPCSTR       lpUsername    points to user name string
 *             DWORD         fdwConnection set of bit flags that specify
 *                                         connection options
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION5(DWORD, WNetAddConnection3W,
              HWND, hwndOwner,
              LPNETRESOURCEW, lpNetResource,
              LPCWSTR, lpPassword,
              LPCWSTR, lpUsername,
              DWORD, fdwConnection)
{
  dprintf(("MPR:WNetAddConnection3W not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             WNetAddConnectionA makes a connection to a network
 *             resource. The function can redirect a local device to the network
 *             resource.
 * Parameters: LPCSTR lpRemoteName  address of network device name
 *             LPCSTR lpPassword    points to password string
 *             LPCSTR lpUsername    points to user name string
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetAddConnectionA,
              LPCSTR, lpRemoteName,
              LPCSTR, lpPassword,
              LPCSTR, lpUsername)
{
  dprintf(("MPR:WNetAddConnectionA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             WNetAddConnection makes a connection to a network
 *             resource. The function can redirect a local device to the network
 *             resource.
 * Parameters: LPCWSTR lpRemoteName  address of network device name
 *             LPCWSTR lpPassword    points to password string
 *             LPCWSTR lpUsername    points to user name string
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetAddConnectionW,
              LPCWSTR, lpRemoteName,
              LPCWSTR, lpPassword,
              LPCWSTR, lpUsername)
{
  dprintf(("MPR:WNetAddConnectionW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetCancelConnection2 function breaks an existing network
 *             connection. It can also be used to remove remembered network
 *             connections that are not currently connected. This function
 *             supersedes WNetCancelConnection.
 * Parameters: LPTSTR lpszName      address of resource name to disconnect
 *             DWORD  fdwConnection connection type flags
 *             BOOL   fForce        flag for unconditional disconnect
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetCancelConnection2A,
              LPTSTR, lpszName,
              DWORD, fdwConnection,
              BOOL, fForce)
{
  dprintf(("MPR:WNetCancelConnection2A not implemented.\n"));
  return (ERROR_NOT_CONNECTED);
}


/*****************************************************************************
 *             The WNetCancelConnection2 function breaks an existing network
 *             connection. It can also be used to remove remembered network
 *             connections that are not currently connected. This function
 *             supersedes WNetCancelConnection.
 * Parameters: LPTSTR lpszName      address of resource name to disconnect
 *             DWORD  fdwConnection connection type flags
 *             BOOL   fForce        flag for unconditional disconnect
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetCancelConnection2W,
              LPWSTR, lpszName,
              DWORD, fdwConnection,
              BOOL, fForce)
{
  dprintf(("MPR:WNetCancelConnection2W not implemented.\n"));
  return (ERROR_NOT_CONNECTED);
}


/*****************************************************************************
 *             The WNetCancelConnection2 function breaks an existing network
 *             connection.
 * Parameters: LPTSTR lpszName      address of resource name to disconnect
 *             BOOL   fForce        flag for unconditional disconnect
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION2(DWORD, WNetCancelConnectionA,
              LPTSTR, lpszName,
              BOOL, fForce)
{
  dprintf(("MPR:WNetCancelConnectionA not implemented.\n"));
  return (ERROR_NOT_CONNECTED);
}


/*****************************************************************************
 *             The WNetCancelConnection2 function breaks an existing network
 *             connection.
 * Parameters: LPTSTR lpszName      address of resource name to disconnect
 *             BOOL   fForce        flag for unconditional disconnect
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION2(DWORD, WNetCancelConnectionW,
              LPWSTR, lpszName,
              BOOL, fForce)
{
  dprintf(("MPR:WNetCancelConnectionW not implemented.\n"));
  return (ERROR_NOT_CONNECTED);
}


/*****************************************************************************
 *
 * Purpose   : The WNetCloseEnum function ends a network resource enumeration
 *               started by the WNetOpenEnum function.
 * Parameters: HANDLE hEnum
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION1(DWORD, WNetCloseEnum,
              HANDLE, hEnum)
{
  dprintf(("MPR:WNetCloseEnum not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetConnectionDialog function starts a general browsing
 *             dialog box for connecting to network resources.
 * Parameters: HWND  hwnd            handle of window owning dialog box
 *             DWORD fdwResourceType resource type to allow connections to
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION2(DWORD, WNetConnectionDialog,
              HWND, hwnd,
              DWORD, fdwResourceType)
{
  dprintf(("MPR:WNetConnectionDialog not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetConnectionDialog1 function starts a general browsing
 *             dialog box for connecting to network resources.
 * Parameters: LPCONNECTDLGSTRUCT lpConnDlgStruct
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION1(DWORD,               WNetConnectionDialog1A,
              LPCONNECTDLGSTRUCTA, lpConnDlgStruct)
{
  dprintf(("MPR:WNetConnectionDialog1A not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetConnectionDialog1 function starts a general browsing
 *             dialog box for connecting to network resources.
 * Parameters: LPCONNECTDLGSTRUCT lpConnDlgStruct
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION1(DWORD,               WNetConnectionDialog1W,
              LPCONNECTDLGSTRUCTW, lpConnDlgStruct)
{
  dprintf(("MPR:WNetConnectionDialog1W not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetDisconnectDialog function starts a general browsing
 *             dialog box for disconnecting from network resources.
 * Parameters: HWND  hwnd            handle of window owning dialog box
 *             DWORD fdwResourceType resource type to disconnect from
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION2(DWORD, WNetDisconnectDialog,
              HWND, hwnd,
              DWORD, fdwResourceType)
{
  dprintf(("MPR:WNetDisconnectDialog not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetDisconnectDialog1 function starts a general browsing
 *             dialog box for disconnecting from network resources.
 * Parameters: LPDISCDLGSTRUCTA lpDisconnectDlgStruct
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION1(DWORD,            WNetDisconnectDialog1A,
              LPDISCDLGSTRUCTA, lpDisconnectDlgStruct)
{
  dprintf(("MPR:WNetDisconnectDialog1A not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetDisconnectDialog1 function starts a general browsing
 *             dialog box for disconnecting from network resources.
 * Parameters: LPDISCDLGSTRUCTW lpDisconnectDlgStruct
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION1(DWORD,            WNetDisconnectDialog1W,
              LPDISCDLGSTRUCTW, lpDisconnectDlgStruct)
{
  dprintf(("MPR:WNetDisconnectDialog1W not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetEnumResource function continues a network-resource
 *             enumeration started by the WNetOpenEnum function.
 * Parameters: HANDLE  hEnum      handle of enumeration
 *             LPDWORD lpcEntries address of entries to list
 *             LPVOID  lpvBuffer  address of buffer for results
 *             LPDWORD lpcbBuffer address of buffer size variable
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION4(DWORD, WNetEnumResourceA,
              HANDLE, hEnum,
              LPDWORD, lpcEntries,
              LPVOID, lpvBuffer,
              LPDWORD, lpcBuffer)
{
  dprintf(("MPR:WNetEnumResourceA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetEnumResource, functioncontinues a network-resource
 *             enumeration started by the WNetOpenEnum function.
 * Parameters: HANDLE  hEnum      handle of enumeration
 *             LPDWORD lpcEntries address of entries to list
 *             LPVOID  lpvBuffer  address of buffer for results
 *             LPDWORD lpcbBuffer address of buffer size variable
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION4(DWORD, WNetEnumResourceW,
              HANDLE, hEnum,
              LPDWORD, lpcEntries,
              LPVOID, lpvBuffer,
              LPDWORD, lpcBuffer)
{
  dprintf(("MPR:WNetEnumResourceW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetGetConnection function retrieves the name of the network
 *             resource associated with a local device.
 * Parameters: LPTSTR  lpszLocalName  address of local name
 *             LPTSTR  lpszRemoteName address of buffer for remote name
 *             LPDWORD lpcchBuffer    address of buffer size, in characters
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetGetConnectionA,
              LPTSTR, lpszLocalName,
              LPTSTR, lpszRemoteName,
              LPDWORD, lpcchBuffer)
{
  dprintf(("MPR:WNetGetConnectionA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetGetConnection function retrieves the name of the network
 *             resource associated with a local device.
 * Parameters: LPWSTR  lpszLocalName  address of local name
 *             LPWSTR  lpszRemoteName address of buffer for remote name
 *             LPDWORD lpcchBuffer    address of buffer size, in characters
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetGetConnectionW,
              LPWSTR, lpszLocalName,
              LPWSTR, lpszRemoteName,
              LPDWORD, lpcchBuffer)
{
  dprintf(("MPR:WNetGetConnectionW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *            The WNetGetLastError function retrieves, themost recent extended
 *               error code set by a Windows network function.
 * Parameters: LPDWORD lpdwErrorCode   address of error code
 *             LPTSTR  lpszDescription address of string describing error
 *             DWORD   cchDescription  size of description buffer, in characters
 *             LPTSTR  lpszName        address of buffer for provider name
 *             DWORD   cchName         size of provider name buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION5(DWORD, WNetGetLastErrorA,
              LPDWORD, lpdwErrorCode,
              LPTSTR, lpszDescription,
              DWORD, cchDescription,
              LPTSTR, lpszName,
              DWORD, cchName)
{
  if ( (lpdwErrorCode   == NULL) ||                     /* check parameters */
       (lpszDescription == NULL) ||
       (cchDescription  == 0) ||
       (lpszName        == NULL) ||
       (cchName         == 0) )
    return (ERROR_INVALID_ADDRESS);                     /* abort with error */

  *lpdwErrorCode = MPRGLOBALS.dwLastError;           /* set that error code */
  /* @@@PH Get error text from some net-MSG-file */
  lpszDescription[cchDescription - 1] = 0;     /* ensure string termination */

  strncpy (lpszName,                                /* return provider name */
           "OS/2 LAN",                       /* that's our default provider */
           cchName);
  lpszName[cchName - 1] = 0;                   /* ensure string termination */

  return (NO_ERROR);
}


/*****************************************************************************
 *             The WNetGetNetworkInformation function returns extended
 *             information about a specific network provider whose name was
 *             returned by a previous network enumeration.
 * Parameters: LPCSTR lpProvider
 *             LPNETINFOSTRUCT lpNetInfoStruct
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION2(DWORD,           WNetGetNetworkInformationA,
              LPCSTR,          lpProvider,
              LPNETINFOSTRUCT, lpNetInfoStruct)
{
  dprintf(("MPR:WNetGetNetworkInformationA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetGetNetworkInformation function returns extended
 *             information about a specific network provider whose name was
 *             returned by a previous network enumeration.
 * Parameters: LPCWSTR lpProvider
 *             LPNETINFOSTRUCT lpNetInfoStruct
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION2(DWORD,           WNetGetNetworkInformationW,
              LPCWSTR,         lpProvider,
              LPNETINFOSTRUCT, lpNetInfoStruct)
{
  dprintf(("MPR:WNetGetNetworkInformationW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *            The WNetGetLastError function retrieves, themost recent extended
 *               error code set by a Windows network function.
 * Parameters: LPDWORD lpdwErrorCode   address of error code
 *             LPWSTR  lpszDescription address of string describing error
 *             DWORD   cchDescription  size of description buffer, in characters
 *             LPWSTR  lpszName        address of buffer for provider name
 *             DWORD   cchName         size of provider name buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION5(DWORD, WNetGetLastErrorW,
              LPDWORD, lpdwErrorCode,
              LPWSTR, lpszDescription,
              DWORD, cchDescription,
              LPWSTR, lpszName,
              DWORD, cchName)
{
  if ( (lpdwErrorCode   == NULL) ||                     /* check parameters */
       (lpszDescription == NULL) ||
       (cchDescription  == 0) ||
       (lpszName        == NULL) ||
       (cchName         == 0) )
    return (ERROR_INVALID_ADDRESS);                     /* abort with error */

  *lpdwErrorCode = MPRGLOBALS.dwLastError;           /* set that error code */
  /* @@@PH Get error text from some net-MSG-file */
  lpszDescription[cchDescription - 1] = 0;     /* ensure string termination */

  lstrcpynW(lpszName,                               /* return provider name */
            (LPCWSTR)L"OS/2 LAN",            /* that's our default provider */
            cchName);
  lpszName[cchName - 1] = 0;                   /* ensure string termination */

  return (NO_ERROR);
}


/*****************************************************************************
 * Purpose   : The WNetGetResourceInformation function retrieves enumeration
 *             information for a network resource. You typically use this
 *             function when the user specifies an object. Call this function
 *             in conjunction with WNetGetResourceParent to determine the
 *             placement and nature of the resource in the browse hierarchy.
 *             Unlike WNetGetResourceParent, WNetGetResourceInformation always
 *             tries to determine the network provider that owns the resource
 *             and the type of the resource, although it might not currently
 *             be accessible (or even exist if the type of the resource was
 *             specified by the caller).
 * Parameters: LPNETRESOURCEA lpNetResource specifies the network resource
 *             LPVOID lpBuffer             pointer to a buffer for results
 *             LPDWORD cbBuffer            size of the buffer
 *             LPTSTR *lplpSystem          pointer to a string in the buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION4(DWORD, WNetGetResourceInformationA,
              LPNETRESOURCEA, lpNetResource,
              LPVOID, lpBuffer,
              LPDWORD, cbBuffer,
              LPTSTR *, lplpSystem)
{
  dprintf(("MPR:WNetGetResourceInformationA not implemented.\n"));
  return (WN_NO_NETWORK);
}


/*****************************************************************************
 * Purpose   : The WNetGetResourceInformation function retrieves enumeration
 *             information for a network resource. You typically use this
 *             function when the user specifies an object. Call this function
 *             in conjunction with WNetGetResourceParent to determine the
 *             placement and nature of the resource in the browse hierarchy.
 *             Unlike WNetGetResourceParent, WNetGetResourceInformation always
 *             tries to determine the network provider that owns the resource
 *             and the type of the resource, although it might not currently
 *             be accessible (or even exist if the type of the resource was
 *             specified by the caller).
 * Parameters: LPNETRESOURCEW lpNetResource specifies the network resource
 *             LPVOID lpBuffer             pointer to a buffer for results
 *             LPDWORD cbBuffer            size of the buffer
 *             LPWSTR *lplpSystem          pointer to a string in the buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION4(DWORD, WNetGetResourceInformationW,
              LPNETRESOURCEW, lpNetResource,
              LPVOID, lpBuffer,
              LPDWORD, cbBuffer,
              LPWSTR *, lplpSystem)
{
  dprintf(("MPR:WNetGetResourceInformationW (%08x,%08x,%08x,%08x) not implemented.\n",
           lpNetResource,
           lpBuffer,
           cbBuffer,
           lplpSystem));

  return (WN_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetGetResourceParent function lets you navigate up from a
 *             resource. It enables browsing to commence based on the name of
 *             a network resource. This function also allows navigating up from
 *             a browsed resource to find connectable resources.
 * Parameters: LPNETRESOURCEA lpNetResource specifies the network resource
 *             LPVOID         lpBuffer      pointer to a buffer for results
 *             LPDWORD        lpBufferSize  size, in bytes, of the buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetGetResourceParentA,
              LPNETRESOURCEA, lpNetResource,
              LPVOID, lpBuffer,
              LPDWORD, lpBufferSize)
{
  dprintf(("MPR:WNetGetResourceParentA not implemented.\n"));
  return (WN_ACCESS_DENIED);
}


/*****************************************************************************
 *             The WNetGetResourceParent function lets you navigate up from a
 *             resource. It enables browsing to commence based on the name of
 *             a network resource. This function also allows navigating up from
 *             a browsed resource to find connectable resources.
 * Parameters: LPNETRESOURCEW lpNetResource specifies the network resource
 *             LPVOID         lpBuffer      pointer to a buffer for results
 *             LPDWORD        lpBufferSize  size, in bytes, of the buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetGetResourceParentW,
              LPNETRESOURCEW, lpNetResource,
              LPVOID, lpBuffer,
              LPDWORD, lpBufferSize)
{
  dprintf(("MPR:WNetGetResourceParentW not implemented.\n"));
  return (WN_ACCESS_DENIED);
}


/*****************************************************************************
 *             The WNetGetProviderName function obtains the provider name for
 *             a specific type of network.
 * Parameters: DWORD   dwNetType
 *             LPSTR   lpProviderName
 *             LPDWORD lpBufferSize
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD,   WNetGetProviderNameA,
              DWORD,   dwNetType,
              LPSTR,   lpProviderName,
              LPDWORD, lpBufferSize)
{
  dprintf(("MPR:WNetGetProviderNameA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetGetProviderName function obtains the provider name for
 *             a specific type of network.
 * Parameters: DWORD   dwNetType
 *             LPWSTR  lpProviderName
 *             LPDWORD lpBufferSize
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD,   WNetGetProviderNameW,
              DWORD,   dwNetType,
              LPWSTR,  lpProviderName,
              LPDWORD, lpBufferSize)
{
  dprintf(("MPR:WNetGetProviderNameW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *            The WNetGetUniversalName, functiontakes a drive-based path for
 *             a network resource and obtains a data structure that contains a
 *             more universal form of the name.
 * Parameters: LPCSTR lpLocalPath  address of drive-based path for a network resource
 *             DWORD   dwInfoLevel  specifies form of universal name to be obtained
 *             LPVOID  lpBuffer     address of buffer that receives universal name data structure
 *             LPDWORD lpBufferSize address of variable that specifies size of buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION4(DWORD, WNetGetUniversalNameA,
              LPCSTR, lpLocalPath,
              DWORD, dwInfoLevel,
              LPVOID, lpBuffer,
              LPDWORD, lpBufferSize)
{
  dprintf(("MPR:WNetGetUniversalNameA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetGetUniversalName, functiontakes a drive-based path for
 *             a network resource and obtains a data structure that contains a
 *             more universal form of the name.
 * Parameters: LPCWSTR lpLocalPath  address of drive-based path for a network resource
 *             DWORD   dwInfoLevel  specifies form of universal name to be obtained
 *             LPVOID  lpBuffer     address of buffer that receives universal name data structure
 *             LPDWORD lpBufferSize address of variable that specifies size of buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION4(DWORD, WNetGetUniversalNameW,
              LPCWSTR, lpLocalPath,
              DWORD, dwInfoLevel,
              LPVOID, lpBuffer,
              LPDWORD, lpBufferSize)
{
  dprintf(("MPR:WNetGetUniversalNameW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetGetUser function retrieves the current default user name
 *             or the user name used to establish a network connection.
 * Parameters: LPTSTR  lpszLocalName address of local name to get user name for
 *             LPTSTR  lpszUserName  address of buffer for user name
 *             LPDWORD lpcchBuffer   address of buffer size variable
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetGetUserA,
              LPTSTR, lpszLocalName,
              LPTSTR, lpszUserName,
              LPDWORD, lpcchBuffer)
{
  dprintf(("MPR:WNetGetUserA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetGetUser function retrieves the current default user name
 *             or the user name used to establish a network connection.
 * Parameters: LPWSTR  lpszLocalName address of local name to get user name for
 *             LPWSTR  lpszUserName  address of buffer for user name
 *             LPDWORD lpcchBuffer   address of buffer size variable
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetGetUserW,
              LPWSTR, lpszLocalName,
              LPWSTR, lpszUserName,
              LPDWORD, lpcchBuffer)
{
  dprintf(("MPR:WNetGetUserW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetOpenEnum function starts, anenumeration of network
 *             resources or existing connections.
 * Parameters: DWORD          fdwScope      scope of enumeration
 *             DWORD          fdwType       resource types to list
 *             DWORD          fdwUsage      resource usage to list
 *             LPNETRESOURCEA lpNetResource address of resource structure
 *             LPHANDLE       lphEnum       address of enumeration handle buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 14:41]
 *****************************************************************************/

ODINFUNCTION5(DWORD, WNetOpenEnumA,
              DWORD, fdwScope,
              DWORD, fdwType,
              DWORD, fdwUsage,
              LPNETRESOURCEA, lpNetResource,
              LPHANDLE, lphEnum)
{
  dprintf(("MPR:WNetOpenEnumA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetOpenEnum function starts, anenumeration of network
 *             resources or existing connections.
 * Parameters: DWORD          fdwScope      scope of enumeration
 *             DWORD          fdwType       resource types to list
 *             DWORD          fdwUsage      resource usage to list
 *             LPNETRESOURCEW lpNetResource address of resource structure
 *             LPHANDLE       lphEnum       address of enumeration handle buffer
 * Variables :
 * Result    : API-Returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 14:41]
 *****************************************************************************/

ODINFUNCTION5(DWORD, WNetOpenEnumW,
              DWORD, fdwScope,
              DWORD, fdwType,
              DWORD, fdwUsage,
              LPNETRESOURCEW, lpNetResource,
              LPHANDLE, lphEnum)
{
  dprintf(("MPR:WNetOpenEnumW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 *             The WNetSetLastError function is used to set the error code status
 *             of this module.
 * Parameters: DWORD   dwErrorCode     error code
 *             LPTSTR  lpszDescription address of string describing error
 *             LPTSTR  lpszName        address of buffer for provider name
 * Variables :
 * Result    : API-Returncode
 * Remark    : not documented
 * Status    : PARTIALLY IMPLEMENTED
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetSetLastErrorA,
              DWORD, dwErrorCode,
              LPTSTR, lpszDescription,
              LPTSTR, lpszName)
{
  dprintf(("MPR:WNetSetLastErrorA not implemented correctly.\n"));
  MPRGLOBALS.dwLastError = dwErrorCode;
  return (NO_ERROR);
}



/*****************************************************************************
 *             The WNetSetLastError function is used to set the error code status
 *             of this module.
 * Parameters: DWORD   dwErrorCode     error code
 *             LPWSTR  lpszDescription address of string describing error
 *             LPWSTR  lpszName        address of buffer for provider name
 * Variables :
 * Result    : API-Returncode
 * Remark    : not documented
 * Status    : PARTIALLY IMPLEMENTED
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

ODINFUNCTION3(DWORD, WNetSetLastErrorW,
              DWORD, dwErrorCode,
              LPWSTR, lpszDescription,
              LPWSTR, lpszName)
{
  dprintf(("MPR:WNetSetLastErrorW not implemented correctly.\n"));
  MPRGLOBALS.dwLastError = dwErrorCode;
  return (NO_ERROR);
}


/*****************************************************************************
 * Purpose:    The WNetUSeConnectionA function is used to establish a connection
 *             to a network object, especially for browsing. The function may
 *             optionally prompt the user for login or authentification.
 * Parameters: HWND           hwndOwner
 *             LPNETRESOURCEA lpNetResource
 *             LPSTR          lpPassword
 *             LPSTR          lpUserID
 *             DWORD          dwFlags
 *             LPSTR          lpAccessName
 *             LPDWORD        lpBufferSize
 *             LPDWORD        lpResult
 * Variables :
 * Result    : API-Returncode
 * Remark    : not documented
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 19:55]
 *****************************************************************************/

ODINFUNCTION8(DWORD,         WNetUseConnectionA,
              HWND,          hwndOwner,
              LPNETRESOURCEA,lpNetResource,
              LPSTR,         lpPassword,
              LPSTR,         lpUserID,
              DWORD,         dwFlags,
              LPSTR,         lpAccessName,
              LPDWORD,       lpBufferSize,
              LPDWORD,       lpResult)
{
  dprintf(("MPR:WNetUseConnectionA not implemented.\n"));
  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Purpose:    The WNetUSeConnectionW function is used to establish a connection
 *             to a network object, especially for browsing. The function may
 *             optionally prompt the user for login or authentification.
 * Parameters: HWND           hwndOwner
 *             LPNETRESOURCEW lpNetResource
 *             LPWSTR         lpPassword
 *             LPWSTR         lpUserID
 *             DWORD          dwFlags
 *             LPWSTR         lpAccessName
 *             LPDWORD        lpBufferSize
 *             LPDWORD        lpResult
 * Variables :
 * Result    : API-Returncode
 * Remark    : not documented
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/04 19:55]
 *****************************************************************************/

ODINFUNCTION8(DWORD,         WNetUseConnectionW,
              HWND,          hwndOwner,
              LPNETRESOURCEW,lpNetResource,
              LPWSTR,        lpPassword,
              LPWSTR,        lpUserID,
              DWORD,         dwFlags  ,
              LPWSTR,        lpAccessName,
              LPDWORD,       lpBufferSize,
              LPDWORD,       lpResult)
{
  dprintf(("MPR:WNetUseConnectionW not implemented.\n"));
  return (ERROR_NO_NETWORK);
}

