/* $Id: MPR.CPP,v 1.3 1999-06-10 17:06:06 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * MPR apis
 *
 * 1998/06/12
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
#include "misc.h"
#include "mpr.h"

// Undocumented Stuff
// MPR.DLL of Windows NT

// WIN32API MultinetGetConnectionPerformanceA
// WIN32API MultinetGetConnectionPerformanceW
// WIN32API MultinetGetErrorTextA
// WIN32API MultinetGetErrorTextW
// WIN32API RestoreConnectionA0
// WIN32API WNetClearConnections
// WIN32API WNetConnectionDialog1A
// WIN32API WNetConnectionDialog1W
// WIN32API WNetConnectionDialog2
// WIN32API WNetDirectoryNotifyA
// WIN32API WNetDirectoryNotifyW
// WIN32API WNetDisconnectDialog1A
// WIN32API WNetDisconnectDialog1W
// WIN32API WNetDisconnectDialog2
// WIN32API WNetFMXEditPerm         FileManager Extensions ?
// WIN32API WNetFMXGetPermCaps
// WIN32API WNetFMXGetPermHelp
// WIN32API WNetFormatNetworkNameA
// WIN32API WNetFormatNetworkNameW
// WIN32API WNetGetConnection2A
// WIN32API WNetGetConnection2W
// WIN32API WNetGetConnection3A
// WIN32API WNetGetConnection3W
// WIN32API WNetGetDirectoryTypeA
// WIN32API WNetGetDirectoryTypeW
// WIN32API WNetGetHomeDirectoryW   NT specific
// WIN32API WNetGetNetworkInformationA
// WIN32API WNetGetNetworkInformationW
// WIN32API WNetGetPropertyTextA
// WIN32API WNetGetPropertyTextW
// WIN32API WNetGetProviderNameA
// WIN32API WNetGetProviderNameW
// WIN32API WNetGetSearchDialog
// WIN32API WNetLogonNotify
// WIN32API WNetPasswordChangeNotify
// WIN32API WNetPropertyDialogA
// WIN32API WNetPropertyDialogW
// WIN32API WNetRestoreConnectionW
// WIN32API WNetSetConnectionA
// WIN32API WNetSetConnectionW
// WIN32API WNetSetLastErrorA      PH: implemented
// WIN32API WNetSetLastErrorW      PH: implemented
// WIN32API WNetSupportGlobalEnum
// WIN32API WNetUseConnectionA
// WIN32API WNetUseConnectionW


/****************************************************************************
 * Module Global Variables                                                  *
 ****************************************************************************/

static struct _MPRGlobals
{
  DWORD dwLastError;   /* most recent extended error code set by a net func */
} MPRGLOBALS;



/*****************************************************************************
 * Name      : DWORD WIN32API WNetAddConnection2A
 * Purpose   : The WNetAddConnection2 function makes a connection to a network
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

DWORD WIN32API OS2WNetAddConnection2A(LPNETRESOURCEA lpNetResource,
                                      LPCSTR        lpPassword,
                                      LPCSTR        lpUsername,
                                      DWORD          fdwConnection)
{
  dprintf(("MPR:WNetAddConnection2A (%08x,%s,%s,%08x) not implemented.\n",
           lpNetResource,
           lpPassword,
           lpUsername,
           fdwConnection));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetAddConnection2W
 * Purpose   : The WNetAddConnection2 function makes a connection to a network
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

DWORD WIN32API OS2WNetAddConnection2W(LPNETRESOURCEW lpNetResource,
                                      LPCWSTR        lpPassword,
                                      LPCWSTR        lpUsername,
                                      DWORD          fdwConnection)
{
  dprintf(("MPR:WNetAddConnection2W (%08x,%s,%s,%08x) not implemented.\n",
           lpNetResource,
           lpPassword,
           lpUsername,
           fdwConnection));

  return (ERROR_NO_NETWORK);
}



/*****************************************************************************
 * Name      : DWORD WIN32API WNetAddConnection3A
 * Purpose   : The WNetAddConnection3 function makes a connection to a network
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

DWORD WIN32API OS2WNetAddConnection3A(HWND           hwndOwner,
                                      LPNETRESOURCEA lpNetResource,
                                      LPCSTR        lpPassword,
                                      LPCSTR        lpUsername,
                                      DWORD          fdwConnection)
{
  dprintf(("MPR:WNetAddConnection3A (%08x,%08x,%s,%s,%08x) not implemented.\n",
           hwndOwner,
           lpNetResource,
           lpPassword,
           lpUsername,
           fdwConnection));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetAddConnection3W
 * Purpose   : The WNetAddConnection3 function makes a connection to a network
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

DWORD WIN32API OS2WNetAddConnection3W(HWND           hwndOwner,
                                      LPNETRESOURCEW lpNetResource,
                                      LPCWSTR        lpPassword,
                                      LPCWSTR        lpUsername,
                                      DWORD          fdwConnection)
{
  dprintf(("MPR:WNetAddConnection3W (%08x,%08x,%s,%s,%08x) not implemented.\n",
           hwndOwner,
           lpNetResource,
           lpPassword,
           lpUsername,
           fdwConnection));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetAddConnectionA
 * Purpose   : The WNetAddConnection function makes a connection to a network
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

DWORD WIN32API OS2WNetAddConnectionA(LPCSTR lpRemoteName,
                                     LPCSTR lpPassword,
                                     LPCSTR lpUsername)
{
  dprintf(("MPR:WNetAddConnectionA (%s,%s,%s) not implemented.\n",
           lpRemoteName,
           lpPassword,
           lpUsername));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetAddConnectionW
 * Purpose   : The WNetAddConnection function makes a connection to a network
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

DWORD WIN32API OS2WNetAddConnectionW(LPCWSTR lpRemoteName,
                                     LPCWSTR lpPassword,
                                     LPCWSTR lpUsername)
{
  dprintf(("MPR:WNetAddConnectionW (%s,%s,%s) not implemented.\n",
           lpRemoteName,
           lpPassword,
           lpUsername));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetCancelConnection2A
 * Purpose   : The WNetCancelConnection2 function breaks an existing network
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

DWORD WIN32API OS2WNetCancelConnection2A(LPTSTR lpszName,
                                         DWORD  fdwConnection,
                                         BOOL   fForce)
{
  dprintf(("MPR:WNetCancelConnection2A (%s,%08x,%u) not implemented.\n",
           lpszName,
           fdwConnection,
           fForce));

  return (ERROR_NOT_CONNECTED);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetCancelConnection2W
 * Purpose   : The WNetCancelConnection2 function breaks an existing network
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

DWORD WIN32API OS2WNetCancelConnection2W(LPWSTR lpszName,
                                         DWORD  fdwConnection,
                                         BOOL   fForce)
{
  dprintf(("MPR:WNetCancelConnection2W (%s,%08x,%u) not implemented.\n",
           lpszName,
           fdwConnection,
           fForce));

  return (ERROR_NOT_CONNECTED);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetCancelConnectionA
 * Purpose   : The WNetCancelConnection2 function breaks an existing network
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

DWORD WIN32API OS2WNetCancelConnectionA(LPTSTR lpszName,
                                        BOOL   fForce)
{
  dprintf(("MPR:WNetCancelConnectionA (%s,%08x,%u) not implemented.\n",
           lpszName,
           fForce));

  return (ERROR_NOT_CONNECTED);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetCancelConnectionW
 * Purpose   : The WNetCancelConnection2 function breaks an existing network
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

DWORD WIN32API OS2WNetCancelConnectionW(LPWSTR lpszName,
                                        BOOL   fForce)
{
  dprintf(("MPR:WNetCancelConnectionW (%s,%08x,%u) not implemented.\n",
           lpszName,
           fForce));

  return (ERROR_NOT_CONNECTED);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetCloseEnum
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

DWORD WIN32API OS2WNetCloseEnum (HANDLE hEnum)
{
  dprintf(("MPR:WNetCloseEnum (%08x) not implemented.\n",
           hEnum));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetConnectionDialog
 * Purpose   : The WNetConnectionDialog function starts a general browsing
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

DWORD WIN32API OS2WNetConnectionDialog (HWND  hwnd,
                                        DWORD fdwResourceType)
{
  dprintf(("MPR:WNetConnectionDialog (%08x,%08x) not implemented.\n",
           hwnd,
           fdwResourceType));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetDisconnectDialog
 * Purpose   : The WNetDisconnectDialog function starts a general browsing
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

DWORD WIN32API OS2WNetDisconnectDialog (HWND  hwnd,
                                        DWORD fdwResourceType)
{
  dprintf(("MPR:WNetDisconnectDialog (%08x,%08x) not implemented.\n",
           hwnd,
           fdwResourceType));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetEnumResourceA
 * Purpose   : The WNetEnumResource function continues a network-resource
 *               enumeration started by the WNetOpenEnum function.
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

DWORD WIN32API OS2WNetEnumResourceA (HANDLE  hEnum,
                                     LPDWORD lpcEntries,
                                     LPVOID  lpvBuffer,
                                     LPDWORD lpcBuffer)
{
  dprintf(("MPR:WNetEnumResourceA (%08x,%08x,%08x,%08x) not implemented.\n",
           hEnum,
           lpcEntries,
           lpvBuffer,
           lpcBuffer));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetEnumResourceW
 * Purpose   : The WNetEnumResource function continues a network-resource
 *               enumeration started by the WNetOpenEnum function.
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

DWORD WIN32API OS2WNetEnumResourceW (HANDLE  hEnum,
                                     LPDWORD lpcEntries,
                                     LPVOID  lpvBuffer,
                                     LPDWORD lpcBuffer)
{
  dprintf(("MPR:WNetEnumResourceW (%08x,%08x,%08x,%08x) not implemented.\n",
           hEnum,
           lpcEntries,
           lpvBuffer,
           lpcBuffer));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetConnectionA
 * Purpose   : The WNetGetConnection function retrieves the name of the network
 *               resource associated with a local device.
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

DWORD WIN32API OS2WNetGetConnectionA (LPTSTR lpszLocalName,
                                      LPTSTR lpszRemoteName,
                                      LPDWORD lpcchBuffer)
{
  dprintf(("MPR:WNetGetConnectionA (%s,%s,%08x) not implemented.\n",
           lpszLocalName,
           lpszRemoteName,
           lpcchBuffer));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetConnectionW
 * Purpose   : The WNetGetConnection function retrieves the name of the network
 *               resource associated with a local device.
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

DWORD WIN32API OS2WNetGetConnectionW (LPWSTR lpszLocalName,
                                      LPWSTR lpszRemoteName,
                                      LPDWORD lpcchBuffer)
{
  dprintf(("MPR:WNetGetConnectionW (%s,%s,%08x) not implemented.\n",
           lpszLocalName,
           lpszRemoteName,
           lpcchBuffer));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetLastErrorA
 * Purpose   : The WNetGetLastError function retrieves the most recent extended
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

DWORD WIN32API OS2WNetGetLastErrorA (LPDWORD lpdwErrorCode,
                                     LPTSTR  lpszDescription,
                                     DWORD   cchDescription,
                                     LPTSTR  lpszName,
                                     DWORD   cchName)
{
  dprintf(("MPR:WNetGetLastErrorA (%08x,%08x,%08x,%08x,%80x).\n",
           lpdwErrorCode,
           lpszDescription,
           cchDescription,
           lpszName,
           cchName));

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
 * Name      : DWORD WIN32API WNetGetLastErrorW
 * Purpose   : The WNetGetLastError function retrieves the most recent extended
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

DWORD WIN32API OS2WNetGetLastErrorW (LPDWORD lpdwErrorCode,
                                     LPWSTR  lpszDescription,
                                     DWORD   cchDescription,
                                     LPWSTR  lpszName,
                                     DWORD   cchName)
{
  dprintf(("MPR:WNetGetLastErrorA (%08x,%08x,%08x,%08x,%80x) not correctly implemented.\n",
           lpdwErrorCode,
           lpszDescription,
           cchDescription,
           lpszName,
           cchName));

  if ( (lpdwErrorCode   == NULL) ||                     /* check parameters */
       (lpszDescription == NULL) ||
       (cchDescription  == 0) ||
       (lpszName        == NULL) ||
       (cchName         == 0) )
    return (ERROR_INVALID_ADDRESS);                     /* abort with error */

  *lpdwErrorCode = MPRGLOBALS.dwLastError;           /* set that error code */
  /* @@@PH Get error text from some net-MSG-file */
  lpszDescription[cchDescription - 1] = 0;     /* ensure string termination */

  strncpy ((LPTSTR)lpszName,                        /* return provider name */
            "OS/2 LAN",                      /* that's our default provider */
            cchName);
  lpszName[cchName - 1] = 0;                   /* ensure string termination */

  return (NO_ERROR);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetResourceInformationA
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

DWORD WIN32API OS2WNetGetResourceInformationA(LPNETRESOURCEA lpNetResource,
                                              LPVOID         lpBuffer,
                                              LPDWORD        cbBuffer,
                                              LPTSTR         *lplpSystem)
{
  dprintf(("MPR:WNetGetResourceInformationA (%08x,%08x,%08x,%08x) not implemented.\n",
           lpNetResource,
           lpBuffer,
           cbBuffer,
           lplpSystem));

  return (WN_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetResourceInformationW
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

DWORD WIN32API OS2WNetGetResourceInformationW(LPNETRESOURCEW lpNetResource,
                                              LPVOID         lpBuffer,
                                              LPDWORD        cbBuffer,
                                              LPWSTR         *lplpSystem)
{
  dprintf(("MPR:WNetGetResourceInformationW (%08x,%08x,%08x,%08x) not implemented.\n",
           lpNetResource,
           lpBuffer,
           cbBuffer,
           lplpSystem));

  return (WN_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetResourceParentA
 * Purpose   : The WNetGetResourceParent function lets you navigate up from a
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

DWORD WIN32API OS2WNetGetResourceParentA(LPNETRESOURCEA lpNetResource,
                                         LPVOID         lpBuffer,
                                         LPDWORD        lpBufferSize)
{
  dprintf(("MPR:WNetGetResourceParentA (%08x,%08x,%u) not implemented.\n",
           lpNetResource,
           lpBuffer,
           lpBufferSize));

  return (WN_ACCESS_DENIED);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetResourceParentW
 * Purpose   : The WNetGetResourceParent function lets you navigate up from a
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

DWORD WIN32API OS2WNetGetResourceParentW(LPNETRESOURCEW lpNetResource,
                                         LPVOID         lpBuffer,
                                         LPDWORD        lpBufferSize)
{
  dprintf(("MPR:WNetGetResourceParentW (%08x,%08x,%u) not implemented.\n",
           lpNetResource,
           lpBuffer,
           lpBufferSize));

  return (WN_ACCESS_DENIED);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetUniversalNameA
 * Purpose   : The WNetGetUniversalName function takes a drive-based path for
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

DWORD WIN32API OS2WNetGetUniversalNameA(LPCSTR lpLocalPath,
                                        DWORD   dwInfoLevel,
                                        LPVOID  lpBuffer,
                                        LPDWORD lpBufferSize)
{
  dprintf(("MPR:WNetGetUniversalNameA (%s,%08x,%08x,%08x) not implemented.\n",
           lpLocalPath,
           dwInfoLevel,
           lpBuffer,
           lpBufferSize));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetUniversalNameW
 * Purpose   : The WNetGetUniversalName function takes a drive-based path for
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

DWORD WIN32API OS2WNetGetUniversalNameW(LPCWSTR lpLocalPath,
                                        DWORD   dwInfoLevel,
                                        LPVOID  lpBuffer,
                                        LPDWORD lpBufferSize)
{
  dprintf(("MPR:WNetGetUniversalNameW (%s,%08x,%08x,%08x) not implemented.\n",
           lpLocalPath,
           dwInfoLevel,
           lpBuffer,
           lpBufferSize));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetUserA
 * Purpose   : The WNetGetUser function retrieves the current default user name
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

DWORD WIN32API OS2WNetGetUserA(LPTSTR  lpszLocalName,
                               LPTSTR  lpszUserName,
                               LPDWORD lpcchBuffer)
{
  dprintf(("MPR:WNetGetUserA (%s,%s,%08x) not implemented.\n",
           lpszLocalName,
           lpszUserName,
           lpcchBuffer));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetGetUserW
 * Purpose   : The WNetGetUser function retrieves the current default user name
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

DWORD WIN32API OS2WNetGetUserW(LPWSTR  lpszLocalName,
                               LPWSTR  lpszUserName,
                               LPDWORD lpcchBuffer)
{
  dprintf(("MPR:WNetGetUserW (%s,%s,%08x) not implemented.\n",
           lpszLocalName,
           lpszUserName,
           lpcchBuffer));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetOpenEnumA
 * Purpose   : The WNetOpenEnum function starts an enumeration of network
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

DWORD WIN32API OS2WNetOpenEnumA(DWORD          fdwScope,
                                DWORD          fdwType,
                                DWORD          fdwUsage,
                                LPNETRESOURCEA lpNetResource,
                                LPHANDLE       lphEnum)
{
  dprintf(("MPR:WNetOpenEnumA (%08x,%08x,%08x,%08x,%08x) not implemented.\n",
           fdwScope,
           fdwType,
           fdwUsage,
           lpNetResource,
           lphEnum));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetOpenEnumW
 * Purpose   : The WNetOpenEnum function starts an enumeration of network
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

DWORD WIN32API OS2WNetOpenEnumW(DWORD          fdwScope,
                                DWORD          fdwType,
                                DWORD          fdwUsage,
                                LPNETRESOURCEW lpNetResource,
                                LPHANDLE       lphEnum)
{
  dprintf(("MPR:WNetOpenEnumW (%08x,%08x,%08x,%08x,%08x) not implemented.\n",
           fdwScope,
           fdwType,
           fdwUsage,
           lpNetResource,
           lphEnum));

  return (ERROR_NO_NETWORK);
}


/*****************************************************************************
 * Name      : DWORD WIN32API WNetSetLastErrorA
 * Purpose   : The WNetSetLastError function is used to set the error code status
 *             of this module.
 * Parameters: DWORD   dwErrorCode     error code
 *             LPTSTR  lpszDescription address of string describing error
 *             LPTSTR  lpszName        address of buffer for provider name
 * Variables :
 * Result    : API-Returncode
 * Remark    : not documented
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

DWORD WIN32API OS2WNetSetLastErrorA (DWORD   dwErrorCode,
                                     LPTSTR  lpszDescription,
                                     LPTSTR  lpszName)
{
  dprintf(("MPR:WNetSetLastErrorA (%08x,%s,%s) not implemented correctly.\n",
           dwErrorCode,
           lpszDescription,
           lpszName));

  MPRGLOBALS.dwLastError = dwErrorCode;

  return (NO_ERROR);
}



/*****************************************************************************
 * Name      : DWORD WIN32API WNetSetLastErrorW
 * Purpose   : The WNetSetLastError function is used to set the error code status
 *             of this module.
 * Parameters: DWORD   dwErrorCode     error code
 *             LPWSTR  lpszDescription address of string describing error
 *             LPWSTR  lpszName        address of buffer for provider name
 * Variables :
 * Result    : API-Returncode
 * Remark    : not documented
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Fri, 1998/02/27 11:55]
 *****************************************************************************/

DWORD WIN32API OS2WNetSetLastErrorW (DWORD   dwErrorCode,
                                     LPWSTR  lpszDescription,
                                     LPWSTR  lpszName)
{
  dprintf(("MPR:WNetSetLastErrorW (%08x,%s,%s) not implemented correctly.\n",
           dwErrorCode,
           lpszDescription,
           lpszName));

  MPRGLOBALS.dwLastError = dwErrorCode;

  return (NO_ERROR);
}

