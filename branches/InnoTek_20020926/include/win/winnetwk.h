/* $Id: winnetwk.h,v 1.2 1999-11-04 10:55:46 phaller Exp $ */

#ifndef _WINNETWK_H_
#define _WINNETWK_H_

#include "windef.h"


typedef struct {
	DWORD	dwScope;
	DWORD	dwType;
	DWORD	dwDisplayType;
	DWORD	dwUsage;
	LPSTR	lpLocalName;
	LPSTR	lpRemoteName;
	LPSTR	lpComment ;
	LPSTR	lpProvider;
} NETRESOURCEA,*LPNETRESOURCEA;

typedef struct {
	DWORD	dwScope;
	DWORD	dwType;
	DWORD	dwDisplayType;
	DWORD	dwUsage;
	LPWSTR	lpLocalName;
	LPWSTR	lpRemoteName;
	LPWSTR	lpComment ;
	LPWSTR	lpProvider;
} NETRESOURCEW,*LPNETRESOURCEW;

DECL_WINELIB_TYPE_AW(NETRESOURCE)
DECL_WINELIB_TYPE_AW(LPNETRESOURCE)

typedef struct {
    DWORD cbStructure;       /* size of this structure in bytes */
    HWND hwndOwner;          /* owner window for the dialog */
    LPNETRESOURCEA lpConnRes;/* Requested Resource info    */
    DWORD dwFlags;           /* flags (see below) */
    DWORD dwDevNum;          /* number of devices connected to */
} CONNECTDLGSTRUCTA, *LPCONNECTDLGSTRUCTA;
typedef struct {
    DWORD cbStructure;       /* size of this structure in bytes */
    HWND hwndOwner;          /* owner window for the dialog */
    LPNETRESOURCEW lpConnRes;/* Requested Resource info    */
    DWORD dwFlags;           /* flags (see below) */
    DWORD dwDevNum;          /* number of devices connected to */
} CONNECTDLGSTRUCTW, *LPCONNECTDLGSTRUCTW;

DECL_WINELIB_TYPE_AW(CONNECTDLGSTRUCT)
DECL_WINELIB_TYPE_AW(LPCONNECTDLGSTRUCT)


typedef struct _DISCDLGSTRUCTA{
    DWORD cbStructure;
    HWND  hwndOwner;
    LPSTR lpLocalName;
    LPSTR lpRemoteName;
    DWORD dwFlags;
} DISCDLGSTRUCTA, *LPDISCDLGSTRUCTA;

typedef struct _DISCDLGSTRUCTW{
    DWORD  cbStructure;
    HWND   hwndOwner;
    LPWSTR lpLocalName;
    LPWSTR lpRemoteName;
    DWORD  dwFlags;
} DISCDLGSTRUCTW, *LPDISCDLGSTRUCTW;


typedef struct _NETINFOSTRUCT{
    DWORD cbStructure;
    DWORD dwProviderVersion;
    DWORD dwStatus;
    DWORD dwCharacteristics;
    ULONG_PTR dwHandle;
    WORD wNetType;
    DWORD dwPrinters;
    DWORD dwDrives;
} NETINFOSTRUCT, *LPNETINFOSTRUCT;

/**/
#define CONNDLG_RO_PATH     0x00000001 /* Resource path should be read-only    */
#define CONNDLG_CONN_POINT  0x00000002 /* Netware -style movable connection point enabled */
#define CONNDLG_USE_MRU     0x00000004 /* Use MRU combobox  */
#define CONNDLG_HIDE_BOX    0x00000008 /* Hide persistent connect checkbox  */
#define CONNDLG_PERSIST     0x00000010 /* Force persistent connection */
#define CONNDLG_NOT_PERSIST 0x00000020 /* Force connection NOT persistent */


typedef struct {
	DWORD	cbStructure;
	DWORD	dwFlags;
	DWORD	dwSpeed;
	DWORD	dwDelay;
	DWORD	dwOptDataSize;
} NETCONNECTINFOSTRUCT,*LPNETCONNECTINFOSTRUCT;


/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/

#define RESOURCETYPE_ANY             0x00000000
#define RESOURCETYPE_DISK            0x00000001
#define RESOURCETYPE_PRINT           0x00000002

#define CONNECT_UPDATE_PROFILE       0x00000001
#define CONNECT_UPDATE_RECENT        0x00000002
#define CONNECT_TEMPORARY            0x00000004
#define CONNECT_INTERACTIVE          0x00000008
#define CONNECT_PROMPT               0x00000010
#define CONNECT_NEED_DRIVE           0x00000020


/****************************************************************************
 * Prototypes                                                               *
 ****************************************************************************/

UINT      WINAPI MultinetGetErrorTextA(DWORD,DWORD,DWORD);
UINT      WINAPI MultinetGetErrorTextW(DWORD,DWORD,DWORD);
#define     MultinetGetErrorText WINELIB_NAME_AW(MultinetGetErrorText_)


DWORD WIN32API MultinetGetConnectionPerformanceA (LPNETRESOURCEA lpNetResource,
                                LPNETCONNECTINFOSTRUCT lpNetConnectInfoStruct);

DWORD WIN32API MultinetGetConnectionPerformanceW (LPNETRESOURCEW lpNetResource,
                                LPNETCONNECTINFOSTRUCT lpNetConnectInfoStruct);

DWORD WIN32API WNetAddConnection2A(LPNETRESOURCEA lpNetResource,
                                   LPCSTR       lpPassword,
                                   LPCSTR       lpUsername,
                                   DWORD         fdwConnection);

DWORD WIN32API WNetAddConnection2W(LPNETRESOURCEW lpNetResource,
                                   LPCWSTR       lpPassword,
                                   LPCWSTR       lpUsername,
                                   DWORD         fdwConnection);

DWORD WIN32API WNetAddConnection3A(HWND          hwndOwner,
                                   LPNETRESOURCEA lpNetResource,
                                   LPCSTR       lpPassword,
                                   LPCSTR       lpUsername,
                                   DWORD         fdwConnection);

DWORD WIN32API WNetAddConnection3W(HWND          hwndOwner,
                                   LPNETRESOURCEW lpNetResource,
                                   LPCWSTR       lpPassword,
                                   LPCWSTR       lpUsername,
                                   DWORD         fdwConnection);

DWORD WIN32API WNetAddConnectionA(LPCSTR lpRemoteName,
                                  LPCSTR lpPassword,
                                  LPCSTR lpUsername);

DWORD WIN32API WNetAddConnectionW(LPCWSTR lpRemoteName,
                                  LPCWSTR lpPassword,
                                  LPCWSTR lpUsername);

DWORD WIN32API WNetCancelConnection2A(LPTSTR lpszName,
                                      DWORD  fdwConnection,
                                      BOOL   fForce);

DWORD WIN32API WNetCancelConnection2W(LPWSTR lpszName,
                                      DWORD  fdwConnection,
                                      BOOL   fForce);

DWORD WIN32API WNetCancelConnectionA(LPTSTR lpszName,
                                     BOOL   fForce);

DWORD WIN32API WNetCancelConnectionW(LPWSTR lpszName,
                                     BOOL   fForce);

DWORD WIN32API WNetCloseEnum (HANDLE hEnum);

DWORD WIN32API WNetConnectionDialog (HWND  hwnd,
                                     DWORD fdwResourceType);

DWORD WIN32API WNetConnectionDialog1A(LPCONNECTDLGSTRUCTA lpConnDlgStruct);

DWORD WIN32API WNetConnectionDialog1W(LPCONNECTDLGSTRUCTW lpConnDlgStruct);

DWORD WIN32API WNetDisconnectDialog (HWND  hwnd,
                                     DWORD fdwResourceType);

DWORD WIN32API WNetDisconnectDialog1A(LPDISCDLGSTRUCTA lpDisconnectDlgStruct);

DWORD WIN32API WNetDisconnectDialog1W(LPDISCDLGSTRUCTW lpDisconnectDlgStruct);

DWORD WIN32API WNetEnumResourceA (HANDLE  hEnum,
                                  LPDWORD lpcEntries,
                                  LPVOID  lpvBuffer,
                                  LPDWORD lpcBuffer);

DWORD WIN32API WNetEnumResourceW (HANDLE  hEnum,
                                  LPDWORD lpcEntries,
                                  LPVOID  lpvBuffer,
                                  LPDWORD lpcBuffer);

DWORD WIN32API WNetGetConnectionA (LPTSTR lpszLocalName,
                                   LPTSTR lpszRemoteName,
                                   LPDWORD lpcchBuffer);

DWORD WIN32API WNetGetConnectionW (LPWSTR lpszLocalName,
                                   LPWSTR lpszRemoteName,
                                   LPDWORD lpcchBuffer);

DWORD WIN32API WNetGetLastErrorA (LPDWORD lpdwErrorCode,
                                  LPTSTR  lpszDescription,
                                  DWORD   cchDescription,
                                  LPTSTR  lpszName,
                                  DWORD   cchName);

DWORD WIN32API WNetGetLastErrorW (LPDWORD lpdwErrorCode,
                                  LPWSTR  lpszDescription,
                                  DWORD   cchDescription,
                                  LPWSTR  lpszName,
                                  DWORD   cchName);

DWORD WIN32API WNetGetNetworkInformationA(LPCSTR           lpProvider,
                                          LPNETINFOSTRUCT  lpNetInfoStruct);

DWORD WIN32API WNetGetNetworkInformationW(LPCWSTR          lpProvider,
                                          LPNETINFOSTRUCT  lpNetInfoStruct);

DWORD WIN32API WNetGetResourceInformationA(LPNETRESOURCEA lpNetResource,
                                           LPVOID         lpBuffer,
                                           LPDWORD        cbBuffer,
                                           LPTSTR         *lplpSystem);

DWORD WIN32API WNetGetResourceInformationW(LPNETRESOURCEW lpNetResource,
                                           LPVOID         lpBuffer,
                                           LPDWORD        cbBuffer,
                                           LPWSTR         *lplpSystem);

DWORD WIN32API WNetGetResourceParentA(LPNETRESOURCEA lpNetResource,
                                      LPVOID         lpBuffer,
                                      LPDWORD        lpBufferSize);

DWORD WIN32API WNetGetResourceParentW(LPNETRESOURCEW lpNetResource,
                                      LPVOID         lpBuffer,
                                      LPDWORD        lpBufferSize);

DWORD WIN32API WNetGetProviderNameA(DWORD    dwNetType,
                                    LPSTR    lpProviderName,
                                    LPDWORD  lpBufferSize);

DWORD WIN32API WNetGetProviderNameW(DWORD    dwNetType,
                                    LPWSTR   lpProviderName,
                                    LPDWORD  lpBufferSize);

DWORD WIN32API WNetGetUniversalNameA(LPCSTR  lpLocalPath,
                                     DWORD   dwInfoLevel,
                                     LPVOID  lpBuffer,
                                     LPDWORD lpBufferSize);

DWORD WIN32API WNetGetUniversalNameW(LPCWSTR lpLocalPath,
                                     DWORD   dwInfoLevel,
                                     LPVOID  lpBuffer,
                                     LPDWORD lpBufferSize);

DWORD WIN32API WNetGetUserA(LPTSTR  lpszLocalName,
                            LPTSTR  lpszUserName,
                            LPDWORD lpcchBuffer);

DWORD WIN32API WNetGetUserW(LPWSTR  lpszLocalName,
                            LPWSTR  lpszUserName,
                            LPDWORD lpcchBuffer);

DWORD WIN32API WNetOpenEnumA(DWORD          fdwScope,
                             DWORD          fdwType,
                             DWORD          fdwUsage,
                             LPNETRESOURCEA lpNetResource,
                             LPHANDLE       lphEnum);

DWORD WIN32API WNetOpenEnumW(DWORD          fdwScope,
                             DWORD          fdwType,
                             DWORD          fdwUsage,
                             LPNETRESOURCEW lpNetResource,
                             LPHANDLE       lphEnum);

DWORD WIN32API WNetSetLastErrorA (DWORD   dwErrorCode,
                                  LPTSTR  lpszDescription,
                                  LPTSTR  lpszName);

DWORD WIN32API WNetSetLastErrorW (DWORD   dwErrorCode,
                                  LPWSTR  lpszDescription,
                                  LPWSTR  lpszName);

DWORD WIN32API WNetUseConnectionA(HWND           hwndOwner,
                                  LPNETRESOURCEA lpNetResource,
                                  LPSTR          lpPassword,
                                  LPSTR          lpUserID,
                                  DWORD          dwFlags,
                                  LPSTR          lpAccessName,
                                  LPDWORD        lpBufferSize,
                                  LPDWORD        lpResult);

DWORD WIN32API WNetUseConnectionW(HWND           hwndOwner,
                                  LPNETRESOURCEW lpNetResource,
                                  LPWSTR         lpPassword,
                                  LPWSTR         lpUserID,
                                  DWORD          dwFlags,
                                  LPWSTR         lpAccessName,
                                  LPDWORD        lpBufferSize,
                                  LPDWORD        lpResult);




#endif /* _WINNETWK_H_ */
