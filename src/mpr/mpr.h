/* $Id: mpr.h,v 1.3 1999-06-10 17:06:06 phaller Exp $ */

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
 * Name      : MPR.H
 * Purpose   : This module maps all Win32 functions contained in MPR.DLL
 *             to their OS/2-specific counterparts as far as possible.
 *             Basis is  5.05.97  12.00  59152 MPR.DLL (NT4SP3)
 *****************************************************************************/


#ifndef __MPR_H__
#define __MPR_H__

#define WINVER    0x400
#undef  APIENTRY
#define APIENTRY  WIN32API
#define FAR

#include <winnetwk.h>

/****************************************************************************
 * Prototypes                                                               *
 ****************************************************************************/

DWORD WIN32API OS2WNetAddConnection2A(LPNETRESOURCEA lpNetResource,
                                      LPCSTR       lpPassword,
                                      LPCSTR       lpUsername,
                                      DWORD         fdwConnection);

DWORD WIN32API OS2WNetAddConnection2W(LPNETRESOURCEW lpNetResource,
                                      LPCWSTR       lpPassword,
                                      LPCWSTR       lpUsername,
                                      DWORD         fdwConnection);

DWORD WIN32API OS2WNetAddConnection3A(HWND          hwndOwner,
                                      LPNETRESOURCEA lpNetResource,
                                      LPCSTR       lpPassword,
                                      LPCSTR       lpUsername,
                                      DWORD         fdwConnection);

DWORD WIN32API OS2WNetAddConnection3W(HWND          hwndOwner,
                                      LPNETRESOURCEW lpNetResource,
                                      LPCWSTR       lpPassword,
                                      LPCWSTR       lpUsername,
                                      DWORD         fdwConnection);

DWORD WIN32API OS2WNetAddConnectionA(LPCSTR lpRemoteName,
                                     LPCSTR lpPassword,
                                     LPCSTR lpUsername);

DWORD WIN32API OS2WNetAddConnectionW(LPCWSTR lpRemoteName,
                                     LPCWSTR lpPassword,
                                     LPCWSTR lpUsername);

DWORD WIN32API OS2WNetCancelConnection2A(LPTSTR lpszName,
                                         DWORD  fdwConnection,
                                         BOOL   fForce);

DWORD WIN32API OS2WNetCancelConnection2W(LPWSTR lpszName,
                                         DWORD  fdwConnection,
                                         BOOL   fForce);

DWORD WIN32API OS2WNetCancelConnectionA(LPTSTR lpszName,
                                        BOOL   fForce);

DWORD WIN32API OS2WNetCancelConnectionW(LPWSTR lpszName,
                                        BOOL   fForce);

DWORD WIN32API OS2WNetCloseEnum (HANDLE hEnum);

DWORD WIN32API OS2WNetConnectionDialog (HWND  hwnd,
                                        DWORD fdwResourceType);

DWORD WIN32API OS2WNetDisconnectDialog (HWND  hwnd,
                                        DWORD fdwResourceType);

DWORD WIN32API OS2WNetEnumResourceA (HANDLE  hEnum,
                                     LPDWORD lpcEntries,
                                     LPVOID  lpvBuffer,
                                     LPDWORD lpcBuffer);

DWORD WIN32API OS2WNetEnumResourceW (HANDLE  hEnum,
                                     LPDWORD lpcEntries,
                                     LPVOID  lpvBuffer,
                                     LPDWORD lpcBuffer);

DWORD WIN32API OS2WNetGetConnectionA (LPTSTR lpszLocalName,
                                      LPTSTR lpszRemoteName,
                                      LPDWORD lpcchBuffer);

DWORD WIN32API OS2WNetGetConnectionW (LPWSTR lpszLocalName,
                                      LPWSTR lpszRemoteName,
                                      LPDWORD lpcchBuffer);

DWORD WIN32API OS2WNetGetLastErrorA (LPDWORD lpdwErrorCode,
                                     LPTSTR  lpszDescription,
                                     DWORD   cchDescription,
                                     LPTSTR  lpszName,
                                     DWORD   cchName);

DWORD WIN32API OS2WNetGetLastErrorW (LPDWORD lpdwErrorCode,
                                     LPWSTR  lpszDescription,
                                     DWORD   cchDescription,
                                     LPWSTR  lpszName,
                                     DWORD   cchName);

DWORD WIN32API OS2WNetGetResourceInformationA(LPNETRESOURCEA lpNetResource,
                                              LPVOID         lpBuffer,
                                              LPDWORD        cbBuffer,
                                              LPTSTR         *lplpSystem);

DWORD WIN32API OS2WNetGetResourceInformationW(LPNETRESOURCEW lpNetResource,
                                              LPVOID         lpBuffer,
                                              LPDWORD        cbBuffer,
                                              LPWSTR         *lplpSystem);

DWORD WIN32API OS2WNetGetResourceParentA(LPNETRESOURCEA lpNetResource,
                                         LPVOID         lpBuffer,
                                         LPDWORD        lpBufferSize);

DWORD WIN32API OS2WNetGetResourceParentW(LPNETRESOURCEW lpNetResource,
                                         LPVOID         lpBuffer,
                                         LPDWORD        lpBufferSize);

DWORD WIN32API OS2WNetGetUniversalNameA(LPCSTR  lpLocalPath,
                                        DWORD   dwInfoLevel,
                                        LPVOID  lpBuffer,
                                        LPDWORD lpBufferSize);

DWORD WIN32API OS2WNetGetUniversalNameW(LPCWSTR lpLocalPath,
                                        DWORD   dwInfoLevel,
                                        LPVOID  lpBuffer,
                                        LPDWORD lpBufferSize);

DWORD WIN32API OS2WNetGetUserA(LPTSTR  lpszLocalName,
                               LPTSTR  lpszUserName,
                               LPDWORD lpcchBuffer);

DWORD WIN32API OS2WNetGetUserW(LPWSTR  lpszLocalName,
                               LPWSTR  lpszUserName,
                               LPDWORD lpcchBuffer);

DWORD WIN32API OS2WNetOpenEnumA(DWORD          fdwScope,
                                DWORD          fdwType,
                                DWORD          fdwUsage,
                                LPNETRESOURCEA lpNetResource,
                                LPHANDLE       lphEnum);

DWORD WIN32API OS2WNetOpenEnumW(DWORD          fdwScope,
                                DWORD          fdwType,
                                DWORD          fdwUsage,
                                LPNETRESOURCEW lpNetResource,
                                LPHANDLE       lphEnum);

DWORD WIN32API OS2WNetSetLastErrorA (DWORD   dwErrorCode,
                                     LPTSTR  lpszDescription,
                                     LPTSTR  lpszName);

DWORD WIN32API OS2WNetSetLastErrorW (DWORD   dwErrorCode,
                                     LPWSTR  lpszDescription,
                                     LPWSTR  lpszName);






#endif
