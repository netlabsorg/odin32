/* $Id: service.cpp,v 1.3 1999-12-21 00:31:38 sandervl Exp $ */

/*
 * Win32 advanced API functions for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 *
 * NOTE: Uses registry key for service as handle
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>
#include "advapi32.h"
#include <unicode.h>
#include <win\winreg.h>
#include <win\winsvc.h>
#include <heapstring.h>

ODINDEBUGCHANNEL(ADVAPI32-SERVICE)

#define REG_SERVICE_TYPE      		"Start"
#define REG_SERVICE_TYPE_W      	(LPWSTR)L"Start"
#define REG_SERVICE_STARTTYPE 		"Type"
#define REG_SERVICE_STARTTYPE_W 	(LPWSTR)L"Type"
#define REG_SERVICE_ERRORCONTROL	"ErrorControl"
#define REG_SERVICE_ERRORCONTROL_W	(LPWSTR)L"ErrorControl"
#define REG_SERVICE_DISPLAYNAME		"DisplayName"
#define REG_SERVICE_DISPLAYNAME_W	(LPWSTR)L"DisplayName"
#define REG_SERVICE_LOADORDERGROUP	"Group" 	//???
#define REG_SERVICE_LOADORDERGROUP_W	(LPWSTR)L"Group" 	//???
#define REG_SERVICE_DEPENDENCIES        "DependOnGroup"
#define REG_SERVICE_DEPENDENCIES_W      (LPWSTR)L"DependOnGroup"
#define REG_SERVICE_IMAGEPATH		"ImagePath"
#define REG_SERVICE_IMAGEPATH_W		(LPWSTR)L"ImagePath"
#define REG_SERVICE_TAG			"Tag"
#define REG_SERVICE_TAG_W		(LPWSTR)L"Tag"
//Odin key
#define REG_SERVICE_STATUS		"ServiceStatus"
#define REG_SERVICE_STATUS_W		(LPWSTR)L"ServiceStatus"
//This key exists if DeleteService has been called for a specific service
#define REG_SERVICE_DELETEPENDING	"DeletePending"

//*****************************************************************************
//TODO: Faster way to checking this
//*****************************************************************************
BOOL CheckServiceHandle(SC_HANDLE hService) 
{
 HKEY keyThisService;

  if(RegOpenKeyA((HKEY)hService, NULL, &keyThisService) != 0) {
	return FALSE;
  }
  RegCloseKey(keyThisService);
  return TRUE;
}
/*****************************************************************************
 * Name      : OpenSCManagerA
 * Purpose   : The OpenSCManager function establishes a connection to the
 *             service control manager on the specified computer and opens the
 *             specified database.
 * Parameters: LPCSTR lpszMachineName   address of machine name string
 *             LPCSTR lpszDatabaseName  address of database name string
 *             DWORD   fdwDesiredAccess  type of access
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

SC_HANDLE WIN32API OpenSCManagerA(LPCSTR lpszMachineName,
                                  LPCSTR lpszDatabaseName,
                                  DWORD  fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenSCManagerA(%s,%s,%x) not correctly implemented.\n",
           lpszMachineName,
           lpszDatabaseName,
           fdwDesiredAccess));

  if(!lpszMachineName && !lpszDatabaseName) {
	HKEY keyServices;
  	if(RegCreateKeyA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services", &keyServices) != 0) {
		SetLastError(ERROR_INTERNAL_ERROR);
		return 0;
	}
	return keyServices;
  }
  return 0;
}


/*****************************************************************************
 * Name      : OpenSCManagerW
 * Purpose   : The OpenSCManager function establishes a connection to the
 *             service control manager on the specified computer and opens the
 *             specified database.
 * Parameters: LPCWSTR lpszMachineName   address of machine name string
 *             LPCWSTR lpszDatabaseName  address of database name string
 *             DWORD   fdwDesiredAccess  type of access
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

SC_HANDLE WIN32API OpenSCManagerW(LPCWSTR lpszMachineName,
                                  LPCWSTR lpszDatabaseName,
                                  DWORD   fdwDesiredAccess)
{
 LPSTR lpszDataBaseNameA = NULL, lpszMachineNameA = NULL;
 SC_HANDLE hService;

  dprintf(("ADVAPI32: OpenSCManagerW(%x,%x,%x) not correctly implemented.\n",
           lpszMachineName,
           lpszDatabaseName,
           fdwDesiredAccess));

  if(lpszMachineName) 
	lpszMachineNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpszMachineName);
  if(lpszDatabaseName)
	lpszDataBaseNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpszDatabaseName);

  hService = OpenSCManagerA(lpszMachineNameA, lpszDataBaseNameA, fdwDesiredAccess);

  if(lpszMachineNameA) 
	HeapFree(GetProcessHeap(), 0, lpszMachineNameA);
  if(lpszDataBaseNameA)
	HeapFree(GetProcessHeap(), 0, lpszDataBaseNameA);

  return hService;
}


/*****************************************************************************
 * Name      : OpenServiceA
 * Purpose   : The OpenService function opens a handle to an existing service.
 * Parameters: SC_HANDLE schSCManager      handle of service control manager database
 *             LPCSTR   lpszServiceName   address of name of service to start
 *             DWORD     fdwDesiredAccess  type of access to service
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

SC_HANDLE WIN32API OpenServiceA(SC_HANDLE schSCManager,
                                LPCSTR   lpszServiceName,
                                DWORD     fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenServiceA(%08xh,%s,%08xh) not correctly implemented.\n",
           schSCManager,
           lpszServiceName,
           fdwDesiredAccess));

  if(CheckServiceHandle(schSCManager) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  
  if(lpszServiceName == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
  }
  HKEY keyThisService;
  if(RegOpenKeyA((HKEY)schSCManager, lpszServiceName, &keyThisService) != 0) {
	SetLastError(ERROR_SERVICE_DOES_NOT_EXIST);
	return 0;
  }
  return keyThisService;
}


/*****************************************************************************
 * Name      : OpenServiceW
 * Purpose   : The OpenService function opens a handle to an existing service.
 * Parameters: SC_HANDLE schSCManager      handle of service control manager database
 *             LPCWSTR   lpszServiceName   address of name of service to start
 *             DWORD     fdwDesiredAccess  type of access to service
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

SC_HANDLE WIN32API OpenServiceW(SC_HANDLE schSCManager,
                                LPCWSTR   lpszServiceName,
                                DWORD     fdwDesiredAccess)
{
 LPSTR lpszServiceNameA = NULL;
 SC_HANDLE hService;

  dprintf(("ADVAPI32: OpenServiceW(%08xh,%s,%08xh) not correctly implemented.\n",
           schSCManager,
           lpszServiceName,
           fdwDesiredAccess));

  if(lpszServiceName == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
  }
  lpszServiceNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpszServiceName);
  hService = OpenServiceA(schSCManager, lpszServiceNameA, fdwDesiredAccess);
  HeapFree(GetProcessHeap(), 0, lpszServiceNameA);
  return hService;
}

/*****************************************************************************
 * Name      : QueryServiceConfigA
 * Purpose   : The QueryServiceConfig function retrieves the configuration
 *             parameters of the specified service.
 * Parameters: SC_HANDLE              schService       handle of service
 *             LPQUERY_SERVICE_CONFIG lpqscServConfig  address of service config. structure
 *             DWORD                  cbBufSize        size of service configuration buffer
 *             LPDWORD                lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

BOOL WIN32API QueryServiceConfigA(SC_HANDLE              schService,
                                     LPQUERY_SERVICE_CONFIG lpqscServConfig,
                                     DWORD                  cbBufSize,
                                     LPDWORD                lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceConfigA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schService,
           lpqscServConfig,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceConfigW
 * Purpose   : The QueryServiceConfig function retrieves the configuration
 *             parameters of the specified service.
 * Parameters: SC_HANDLE              schService       handle of service
 *             LPQUERY_SERVICE_CONFIG lpqscServConfig  address of service config. structure
 *             DWORD                  cbBufSize        size of service configuration buffer
 *             LPDWORD                lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceConfigW(SC_HANDLE              schService,
                                     LPQUERY_SERVICE_CONFIG lpqscServConfig,
                                     DWORD                  cbBufSize,
                                     LPDWORD                lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceConfigW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schService,
           lpqscServConfig,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceLockStatusA
 * Purpose   : The QueryServiceLockStatus function retrieves the lock status
 *             of the specified service control manager database.
 * Parameters: SC_HANDLE                   schSCManager     handle of svc. ctrl. mgr. database
 *             LPQUERY_SERVICE_LOCK_STATUS lpqslsLockStat   address of lock status structure
 *             DWORD                       cbBufSize        size of service configuration buffer
 *             LPDWORD                     lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceLockStatusA(SC_HANDLE                   schSCManager,
                                         LPQUERY_SERVICE_LOCK_STATUSA lpqslsLockStat,
                                         DWORD                       cbBufSize,
                                         LPDWORD                     lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceLockStatusA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schSCManager,
           lpqslsLockStat,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceLockStatusW
 * Purpose   : The QueryServiceLockStatus function retrieves the lock status
 *             of the specified service control manager database.
 * Parameters: SC_HANDLE                   schSCManager     handle of svc. ctrl. mgr. database
 *             LPQUERY_SERVICE_LOCK_STATUS lpqslsLockStat   address of lock status structure
 *             DWORD                       cbBufSize        size of service configuration buffer
 *             LPDWORD                     lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceLockStatusW(SC_HANDLE                   schSCManager,
                                         LPQUERY_SERVICE_LOCK_STATUSW lpqslsLockStat,
                                         DWORD                       cbBufSize,
                                         LPDWORD                     lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceLockStatusW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schSCManager,
           lpqslsLockStat,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : QueryServiceObjectSecurity
 * Purpose   : The QueryServiceObjectSecurity function retrieves a copy of the
 *             security descriptor protecting a service object.
 * Parameters: SC_HANDLE             schService       handle of service
 *             SECURITY_INFORMATION  fdwSecurityInfo  type of security information requested
 *             PSECURITY_DESCRIPTOR  psdSecurityDesc  address of security descriptor
 *             DWORD                 cbBufSize        size of security descriptor buffer
 *             LPDWORD               lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceObjectSecurity(SC_HANDLE             schService,
                                            SECURITY_INFORMATION  fdwSecurityInfo,
                                            PSECURITY_DESCRIPTOR  psdSecurityDesc,
                                            DWORD                 cbBufSize,
                                            LPDWORD               lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schService,
           fdwSecurityInfo,
           psdSecurityDesc,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceStatus
 * Purpose   : The QueryServiceStatus function retrieves the current status of
 *             the specified service.
 * Parameters: SC_HANDLE         schService        handle of service
 *             LPSERVICE_STATUS  lpssServiceStatus address of service status structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceStatus(SC_HANDLE         schService,
                                 LPSERVICE_STATUS  lpssServiceStatus)
{
 DWORD size, keytype;

  dprintf(("ADVAPI32: QueryServiceStatus(%08xh,%08xh) not correctly implemented.\n",
           schService,
           lpssServiceStatus));

  if(CheckServiceHandle(schService) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }

  memset(lpssServiceStatus, 0, sizeof(SERVICE_STATUS));

  size = sizeof(DWORD);
  keytype = REG_DWORD;
  RegQueryValueExA((HKEY)schService, REG_SERVICE_TYPE, 0, &keytype, (LPBYTE)&lpssServiceStatus->dwServiceType, &size);

  size = sizeof(DWORD);
  keytype = REG_DWORD;
  RegQueryValueExA((HKEY)schService, REG_SERVICE_STATUS, 0, &keytype, (LPBYTE)&lpssServiceStatus->dwCurrentState, &size);

  //TODO: Should this be set by the service once it has started?
  lpssServiceStatus->dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_PAUSE_CONTINUE|SERVICE_ACCEPT_SHUTDOWN;
  return TRUE;
}

/*****************************************************************************
 * Name      : SetServiceObjectSecurity
 * Purpose   : The SetServiceObjectSecurity function sets the security
 *             descriptor of a service object.
 * Parameters: SC_HANDLE             schService      handle of service
 *             SECURITY_INFORMATION  fdwSecurityInfo type of security information requested
 *             PSECURITY_DESCRIPTOR  psdSecurityDesc address of security descriptor
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetServiceObjectSecurity(SC_HANDLE             schService,
                                          SECURITY_INFORMATION  fdwSecurityInfo,
                                          PSECURITY_DESCRIPTOR  psdSecurityDesc)
{
  dprintf(("ADVAPI32: SetServiceObjectSecurity(%08xh,%08xh,%08xh) not implemented.\n",
           schService,
           fdwSecurityInfo,
           psdSecurityDesc));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : ChangeServiceConfigA
 * Purpose   : The ChangeServiceConfig function changes the configuration
 *             parameters of a service.
 * Parameters: SC_HANDLE hService           handle of service
 *             DWORD     dwServiceType      type of service
 *             DWORD     dwStartType        when to start service
 *             DWORD     dwErrorControl     severity if service fails to start
 *             LPCSTR   lpBinaryPathName   address of service binary file name
 *             LPCSTR   lpLoadOrderGroup   address of load ordering group name
 *             LPDWORD   lpdwTagId          address of variable to get tag identifier
 *             LPCSTR   lpDependencies     address of array of dependency names
 *             LPCSTR   lpServiceStartName address of account name of service
 *             LPCSTR   lpPassword         address of password for service account
 *             LPCSTR   lpDisplayName      address of display name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ChangeServiceConfigA(SC_HANDLE hService,
                                      DWORD     dwServiceType,
                                      DWORD     dwStartType,
                                      DWORD     dwErrorControl,
                                      LPCSTR   lpBinaryPathName,
                                      LPCSTR   lpLoadOrderGroup,
                                      LPDWORD   lpdwTagId,
                                      LPCSTR   lpDependencies,
                                      LPCSTR   lpServiceStartName,
                                      LPCSTR   lpPassword,
                                      LPCSTR   lpDisplayName)
{
  dprintf(("ADVAPI32: ChangeServiceConfigA(%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s,%s) not implemented.\n",
           hService,
           dwServiceType,
           dwStartType,
           dwErrorControl,
           lpBinaryPathName,
           lpLoadOrderGroup,
           lpdwTagId,
           lpDependencies,
           lpServiceStartName,
           lpPassword,
           lpDisplayName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ChangeServiceConfigW
 * Purpose   : The ChangeServiceConfig function changes the configuration
 *             parameters of a service.
 * Parameters: SC_HANDLE hService           handle of service
 *             DWORD     dwServiceType      type of service
 *             DWORD     dwStartType        when to start service
 *             DWORD     dwErrorControl     severity if service fails to start
 *             LPCWSTR   lpBinaryPathName   address of service binary file name
 *             LPCWSTR   lpLoadOrderGroup   address of load ordering group name
 *             LPDWORD   lpdwTagId          address of variable to get tag identifier
 *             LPCWSTR   lpDependencies     address of array of dependency names
 *             LPCWSTR   lpServiceStartName address of account name of service
 *             LPCWSTR   lpPassword         address of password for service account
 *             LPCWSTR   lpDisplayName      address of display name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ChangeServiceConfigW(SC_HANDLE hService,
                                      DWORD     dwServiceType,
                                      DWORD     dwStartType,
                                      DWORD     dwErrorControl,
                                      LPCWSTR   lpBinaryPathName,
                                      LPCWSTR   lpLoadOrderGroup,
                                      LPDWORD   lpdwTagId,
                                      LPCWSTR   lpDependencies,
                                      LPCWSTR   lpServiceStartName,
                                      LPCWSTR   lpPassword,
                                      LPCWSTR   lpDisplayName)
{
  dprintf(("ADVAPI32: ChangeServiceConfigW(%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s,%s) not implemented.\n",
           hService,
           dwServiceType,
           dwStartType,
           dwErrorControl,
           lpBinaryPathName,
           lpLoadOrderGroup,
           lpdwTagId,
           lpDependencies,
           lpServiceStartName,
           lpPassword,
           lpDisplayName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CloseServiceHandle
 * Purpose   : The CloseServiceHandle function closes a handle to a service
 *             control manager database as returned by the OpenSCManager function,
 *             or it closes a handle to a service object as returned by either
 *             the OpenService or CreateService function.
 * Parameters: SC_HANDLE  hSCObject  handle of service or service control manager database
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CloseServiceHandle(SC_HANDLE hSCObject)
{
  dprintf(("ADVAPI32: CloseServiceHandle(%08xh)",
           hSCObject));

  if(CheckServiceHandle(hSCObject) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }

  DWORD deletepending, keytype = REG_DWORD, size = sizeof(DWORD);
  if(!RegQueryValueExA((HKEY)hSCObject, REG_SERVICE_DELETEPENDING, 0, &keytype, (LPBYTE)&deletepending, &size)) {
   FILETIME filetime;
   DWORD bla, classsize;
   CHAR szClassName[64];

	HKEY keyServices;
  	if(RegCreateKeyA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services", &keyServices) != 0) {
		SetLastError(ERROR_INTERNAL_ERROR);
		return FALSE;
	}

	//NOTE: Assumes for now there are no subkeys
	//TODO: DOes not work
	classsize = sizeof(szClassName);
	RegQueryValueA((HKEY)hSCObject, NULL, szClassName, (LPLONG)&classsize);
  	RegCloseKey((HKEY)hSCObject);
	RegDeleteKeyA(keyServices, szClassName);
  }

  RegCloseKey((HKEY)hSCObject);
  return TRUE;
}

/*****************************************************************************
 * Name      : ControlService
 * Purpose   : The ControlService function sends a control code to a Win32 service.
 * Parameters: SC_HANDLE        hService        handle of service
 *             DWORD            dwControl       control code
 *             LPSERVICE_STATUS lpServiceStatus address of service status structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ControlService(SC_HANDLE        hService,
                             DWORD            dwControl,
                             LPSERVICE_STATUS lpServiceStatus)
{
  dprintf(("ADVAPI32: ControlService(%08xh,%08xh,%08xh) not correctly implemented.\n",
           hService,
           dwControl,
           lpServiceStatus));
  HKEY keyThisService;

  if(CheckServiceHandle(hService) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  return TRUE;
}


/*****************************************************************************
 * Name      : CreateServiceA
 * Purpose   : The CreateService function creates a service object and adds it
 *             to the specified service control manager database.
 * Parameters: SC_HANDLE hSCManager         handle of service control manager database
 *             LPCSTR   lpServiceName      address of name of service to start
 *             LPCSTR   lpDisplayName      address of display name
 *             DWORD     dwDesiredAccess    type of access to service
 *             DWORD     dwServiceType      type of service
 *             DWORD     dwStartType        when to start service
 *             DWORD     dwErrorControl     severity if service fails to start
 *             LPCSTR   lpBinaryPathName   address of name of binary file
 *             LPCSTR   lpLoadOrderGroup   address of name of load ordering group
 *             LPDWORD   lpdwTagId          address of variable to get tag identifier
 *             LPCSTR   lpDependencies     address of array of dependency names
 *             LPCSTR   lpServiceStartName address of account name of service
 *             LPCSTR   lpPassword         address of password for service account
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API CreateServiceA(SC_HANDLE hSCManager,
                                  LPCSTR    lpServiceName,
                                  LPCSTR    lpDisplayName,
                                  DWORD     dwDesiredAccess,
                                  DWORD     dwServiceType,
                                  DWORD     dwStartType,
                                  DWORD     dwErrorControl,
                                  LPCSTR    lpBinaryPathName,
                                  LPCSTR    lpLoadOrderGroup,
                                  LPDWORD   lpdwTagId,
                                  LPCSTR    lpDependencies,
                                  LPCSTR    lpServiceStartName,
                                  LPCSTR    lpPassword)
{
 HKEY keyServices, keyThisService;

  dprintf(("ADVAPI32: CreateServiceA(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s) not correctly implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           dwDesiredAccess,
           dwServiceType,
           dwStartType,
           dwErrorControl,
           lpBinaryPathName,
           lpLoadOrderGroup,
           lpdwTagId,
           lpDependencies,
           lpServiceStartName,
           lpPassword));

  //displayname too?
  if(lpServiceName == NULL || lpBinaryPathName == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
  }

  if(RegCreateKeyA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services", &keyServices) != 0) {
	SetLastError(ERROR_INTERNAL_ERROR);
	return 0;
  }
  if(RegCreateKeyA(keyServices, lpServiceName, &keyThisService) != 0) {
	SetLastError(ERROR_INTERNAL_ERROR);
	return 0;
  }
  RegSetValueExA(keyThisService, REG_SERVICE_TYPE, 0, REG_DWORD, (LPBYTE)&dwServiceType, sizeof(DWORD));
  RegSetValueExA(keyThisService, REG_SERVICE_STARTTYPE, 0, REG_DWORD, (LPBYTE)&dwStartType, sizeof(DWORD));
  RegSetValueExA(keyThisService, REG_SERVICE_ERRORCONTROL, 0, REG_DWORD, (LPBYTE)&dwErrorControl, sizeof(DWORD));
  if(lpDisplayName) 
	RegSetValueExA(keyThisService, REG_SERVICE_DISPLAYNAME, 0, REG_SZ, (LPBYTE)lpDisplayName, strlen(lpDisplayName)+1);
  if(lpLoadOrderGroup) 
	RegSetValueExA(keyThisService, REG_SERVICE_LOADORDERGROUP, 0, REG_SZ, (LPBYTE)lpDisplayName, strlen(lpLoadOrderGroup)+1);
  if(lpDependencies) {
	//seems to need an extra terminating '0'
	int size = strlen(lpDependencies)+2;
	char *dependencies = (char *)malloc(size);
	memset(dependencies, 0, size);
	strcpy(dependencies, lpDependencies);
	RegSetValueExA(keyThisService, REG_SERVICE_DEPENDENCIES, 0, REG_BINARY, (LPBYTE)dependencies, size);
	free(dependencies);
  }
  //TODO: %SystemRoot%
  RegSetValueExA(keyThisService, REG_SERVICE_IMAGEPATH, 0, REG_SZ, (LPBYTE)lpBinaryPathName, strlen(lpBinaryPathName)+1);

  //Pointer to a variable that receives a tag value that is unique in the group specified in the
  //lpLoadOrderGroup parameter. Specify NULL if you are not changing the existing tag. 
  DWORD tag = 1; //TODO!!
  RegSetValueExA(keyThisService, REG_SERVICE_TAG, 0, REG_DWORD, (LPBYTE)&tag, sizeof(DWORD));
  if(lpdwTagId)
	*lpdwTagId = tag;

  DWORD state = SERVICE_STOPPED;
  RegSetValueExA(keyThisService, REG_SERVICE_STATUS, 0, REG_DWORD, (LPBYTE)&state, sizeof(DWORD));

  //TODO: What else?

  RegCloseKey(keyServices);
  return keyThisService;
}


/*****************************************************************************
 * Name      : CreateServiceW
 * Purpose   : The CreateService function creates a service object and adds it
 *             to the specified service control manager database.
 * Parameters: SC_HANDLE hSCManager         handle of service control manager database
 *             LPCWSTR   lpServiceName      address of name of service to start
 *             LPCWSTR   lpDisplayName      address of display name
 *             DWORD     dwDesiredAccess    type of access to service
 *             DWORD     dwServiceType      type of service
 *             DWORD     dwStartType        when to start service
 *             DWORD     dwErrorControl     severity if service fails to start
 *             LPCWSTR   lpBinaryPathName   address of name of binary file
 *             LPCWSTR   lpLoadOrderGroup   address of name of load ordering group
 *             LPDWORD   lpdwTagId          address of variable to get tag identifier
 *             LPCWSTR   lpDependencies     address of array of dependency names
 *             LPCWSTR   lpServiceStartName address of account name of service
 *             LPCWSTR   lpPassword         address of password for service account
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API CreateServiceW(SC_HANDLE hSCManager,
                                     LPCWSTR   lpServiceName,
                                     LPCWSTR   lpDisplayName,
                                     DWORD     dwDesiredAccess,
                                     DWORD     dwServiceType,
                                     DWORD     dwStartType,
                                     DWORD     dwErrorControl,
                                     LPCWSTR   lpBinaryPathName,
                                     LPCWSTR   lpLoadOrderGroup,
                                     LPDWORD   lpdwTagId,
                                     LPCWSTR   lpDependencies,
                                     LPCWSTR   lpServiceStartName,
                                     LPCWSTR   lpPassword)
{
 LPSTR lpServiceNameA = NULL, lpDisplayNameA = NULL, lpBinaryPathNameA = NULL;
 LPSTR lpDependenciesA = NULL, lpServiceStartNameA = NULL, lpPasswordA = NULL;
 LPSTR lpLoadOrderGroupA = NULL;
 SC_HANDLE hService;

  dprintf(("ADVAPI32: CreateServiceW(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s) not correctly implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           dwDesiredAccess,
           dwServiceType,
           dwStartType,
           dwErrorControl,
           lpBinaryPathName,
           lpLoadOrderGroup,
           lpdwTagId,
           lpDependencies,
           lpServiceStartName,
           lpPassword));

  if(lpServiceName)
  	lpServiceNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpServiceName);
  if(lpDisplayName)
  	lpDisplayNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpDisplayName);
  if(lpBinaryPathName)
  	lpBinaryPathNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpBinaryPathName);
  if(lpDependencies)
  	lpDependenciesA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpDependencies);
  if(lpServiceStartName)
  	lpServiceStartNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpServiceStartName);
  if(lpPassword)
  	lpPasswordA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpPassword);
  if(lpDisplayName)
  	lpDisplayNameA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpDisplayName);
  if(lpLoadOrderGroup)
  	lpLoadOrderGroupA = HEAP_strdupWtoA(GetProcessHeap(), 0, lpLoadOrderGroup);

  hService = CreateServiceA(hSCManager,lpServiceNameA, lpDisplayNameA,
                            dwDesiredAccess, dwServiceType, dwStartType,
                            dwErrorControl, lpBinaryPathNameA, lpLoadOrderGroupA,
                            lpdwTagId, lpDependenciesA, lpServiceStartNameA,
                            lpPasswordA);

  if(lpServiceNameA) 		HeapFree(GetProcessHeap(), 0, lpServiceNameA);
  if(lpDisplayNameA) 		HeapFree(GetProcessHeap(), 0, lpDisplayNameA);
  if(lpBinaryPathNameA)		HeapFree(GetProcessHeap(), 0, lpBinaryPathNameA);
  if(lpDependenciesA) 		HeapFree(GetProcessHeap(), 0, lpDependenciesA);
  if(lpServiceStartNameA) 	HeapFree(GetProcessHeap(), 0, lpServiceStartNameA);
  if(lpPasswordA) 		HeapFree(GetProcessHeap(), 0, lpPasswordA);
  if(lpLoadOrderGroupA) 	HeapFree(GetProcessHeap(), 0, lpLoadOrderGroupA);

  return hService;
}

/*****************************************************************************
 * Name      : StartServiceA
 * Purpose   : The StartService function starts the execution of a service.
 * Parameters: SC_HANDLE schService        handle of service
 *             DWORD     dwNumServiceArgs  number of arguments
 *             LPCSTR   *lpszServiceArgs  address of array of argument string pointers
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API StartServiceA(SC_HANDLE schService,
                            DWORD     dwNumServiceArgs,
                            LPCSTR   *lpszServiceArgs)
{
  dprintf(("ADVAPI32: StartServiceA(%08xh,%08xh,%s) not implemented.\n",
           schService,
           dwNumServiceArgs,
           lpszServiceArgs));

  if(CheckServiceHandle(schService) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }

  DWORD state = SERVICE_RUNNING;
  if(!RegSetValueExA((HKEY)schService, REG_SERVICE_STATUS, 0, REG_DWORD, (LPBYTE)&state, sizeof(DWORD))) {
	return TRUE;
  }

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : StartServiceW
 * Purpose   : The StartService function starts the execution of a service.
 * Parameters: SC_HANDLE schService        handle of service
 *             DWORD     dwNumServiceArgs  number of arguments
 *             LPCWSTR   *lpszServiceArgs  address of array of argument string pointers
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API StartServiceW(SC_HANDLE schService,
                            DWORD     dwNumServiceArgs,
                            LPCWSTR   *lpszServiceArgs)
{
  dprintf(("ADVAPI32: StartServiceW(%08xh,%08xh,%s) not implemented.\n",
           schService,
           dwNumServiceArgs,
           lpszServiceArgs));

  if(CheckServiceHandle(schService) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }

  DWORD state = SERVICE_RUNNING;
  if(!RegSetValueExW((HKEY)schService, REG_SERVICE_STATUS_W, 0, REG_DWORD, (LPBYTE)&state, sizeof(DWORD))) {
	return TRUE;
  }
  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : DeleteService
 * Purpose   : The DeleteService function marks the specified service for
 *             deletion from the service control manager database.
 * Parameters: SC_HANDLE  hService  handle of service
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API DeleteService(SC_HANDLE hService)
{
  dprintf(("ADVAPI32: DeleteService(%08xh)", hService));

  if(CheckServiceHandle(hService) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  DWORD deletepending = 1;
  if(!RegSetValueExA((HKEY)hService, REG_SERVICE_DELETEPENDING, 0, REG_DWORD, (LPBYTE)&deletepending, sizeof(DWORD))) {
	return TRUE;
  }
  return FALSE;
}

/*****************************************************************************
 * Name      : GetServiceDisplayNameA
 * Purpose   : The GetServiceDisplayName function obtains the display name that
 *             is associated with a particular service name. The service name
 *             is the same as the service's registry key name.
 * Parameters: SC_HANDLE hSCManager     handle to a service control manager database
 *             LPCSTR   lpServiceName  the service name
 *             LPTSTR    lpDisplayName  buffer to receive the service's display name
 *             LPDWORD   lpcchBuffer    size of display name buffer and display name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetServiceDisplayNameA(SC_HANDLE hSCManager,
                                     LPCSTR    lpServiceName,
                                     LPTSTR    lpDisplayName,
                                     LPDWORD   lpcchBuffer)
{
 HKEY keyThisService;
 DWORD size, type;

  dprintf(("ADVAPI32: GetServiceDisplayNameA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           lpcchBuffer));

  if(!lpServiceName || !lpDisplayName) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  if(CheckServiceHandle(hSCManager) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }

  if(RegOpenKeyA((HKEY)hSCManager, lpServiceName, &keyThisService) != 0) {
	SetLastError(ERROR_SERVICE_DOES_NOT_EXIST);
	return FALSE;
  }
  size = *lpcchBuffer;
  type = REG_SZ;
  if(RegQueryValueExA(keyThisService, REG_SERVICE_DISPLAYNAME, 0, &type, (LPBYTE)lpDisplayName, &size) == ERROR_MORE_DATA)
  {
	SetLastError(ERROR_MORE_DATA);
  	*lpcchBuffer = size;
	return FALSE;
  }
  *lpcchBuffer = size;
  RegCloseKey(keyThisService);
  return TRUE;
}


/*****************************************************************************
 * Name      : GetServiceDisplayNameW
 * Purpose   : The GetServiceDisplayName function obtains the display name that
 *             is associated with a particular service name. The service name
 *             is the same as the service's registry key name.
 * Parameters: SC_HANDLE hSCManager     handle to a service control manager database
 *             LPCWSTR   lpServiceName  the service name
 *             LPWSTR    lpDisplayName  buffer to receive the service's display name
 *             LPDWORD   lpcchBuffer    size of display name buffer and display name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetServiceDisplayNameW(SC_HANDLE hSCManager,
                                     LPCWSTR   lpServiceName,
                                     LPWSTR    lpDisplayName,
                                     LPDWORD   lpcchBuffer)
{
 HKEY keyThisService;
 DWORD size, type;

  dprintf(("ADVAPI32: GetServiceDisplayNameW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           lpcchBuffer));

  if(!lpServiceName || !lpDisplayName) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  if(CheckServiceHandle(hSCManager) == FALSE) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }

  if(RegOpenKeyW((HKEY)hSCManager, lpServiceName, &keyThisService) != 0) {
	SetLastError(ERROR_SERVICE_DOES_NOT_EXIST);
	return FALSE;
  }
  size = *lpcchBuffer;
  type = REG_SZ;
  if(RegQueryValueExW(keyThisService, REG_SERVICE_DISPLAYNAME_W, 0, &type, (LPBYTE)lpDisplayName, &size) == ERROR_MORE_DATA)
  {
	SetLastError(ERROR_MORE_DATA);
  	*lpcchBuffer = size;
	return FALSE;
  }
  *lpcchBuffer = size;
  RegCloseKey(keyThisService);
  return TRUE;
}

/*****************************************************************************
 * Name      : EnumDependentServicesA
 * Purpose   : The EnumDependentServices function enumerates services that
 *             depend on another specified service; that is, the specified
 *             service must be running before the enumerated services can run.
 *             The name and status of each dependent service are provided.
 * Parameters: SC_HANDLE             hService           handle of service
 *             DWORD                 dwServiceState     state of services to enumerate
 *             LPENUM_SERVICE_STATUS lpServices         address of service status buffer
 *             DWORD                 cbBufSize          size of service status buffer
 *             LPDWORD               pcbBytesNeeded     address of variable for bytes needed
 *             LPDWORD               lpServicesReturned address of variable for number returned
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EnumDependentServicesA(SC_HANDLE             hService,
                                        DWORD                 dwServiceState,
                                        LPENUM_SERVICE_STATUSA lpServices,
                                        DWORD                 cbBufSize,
                                        LPDWORD               pcbBytesNeeded,
                                        LPDWORD               lpServicesReturned)
{
  dprintf(("ADVAPI32: EnumDependentServicesA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hService,
           dwServiceState,
           lpServices,
           cbBufSize,
           pcbBytesNeeded,
           lpServicesReturned));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : EnumDependentServicesW
 * Purpose   : The EnumDependentServices function enumerates services that
 *             depend on another specified service; that is, the specified
 *             service must be running before the enumerated services can run.
 *             The name and status of each dependent service are provided.
 * Parameters: SC_HANDLE             hService           handle of service
 *             DWORD                 dwServiceState     state of services to enumerate
 *             LPENUM_SERVICE_STATUS lpServices         address of service status buffer
 *             DWORD                 cbBufSize          size of service status buffer
 *             LPDWORD               pcbBytesNeeded     address of variable for bytes needed
 *             LPDWORD               lpServicesReturned address of variable for number returned
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EnumDependentServicesW(SC_HANDLE             hService,
                                        DWORD                 dwServiceState,
                                        LPENUM_SERVICE_STATUSW lpServices,
                                        DWORD                 cbBufSize,
                                        LPDWORD               pcbBytesNeeded,
                                        LPDWORD               lpServicesReturned)
{
  dprintf(("ADVAPI32: EnumDependentServicesW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hService,
           dwServiceState,
           lpServices,
           cbBufSize,
           pcbBytesNeeded,
           lpServicesReturned));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : EnumServicesStatusA
 * Purpose   : The EnumServicesStatus function enumerates services in the specified
 *             service control manager database. The name and status of each service are provided.
 * Parameters: SC_HANDLE             hSCManager          handle of service control manager database
 *             DWORD                 dwServiceType       type of services to enumerate
 *             DWORD                 dwServiceState      state of services to enumerate
 *             LPENUM_SERVICE_STATUS lpServices          address of service status buffer
 *             DWORD                 cbBufSize           size of service status buffer
 *             LPDWORD               pcbBytesNeeded      address of variable for bytes needed
 *             LPDWORD               lpServicesReturned  address of variable for number returned
 *             LPDWORD               lpResumeHandle      address of variable for next entry
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EnumServicesStatusA(SC_HANDLE             hSCManager,
                                     DWORD                 dwServiceType,
                                     DWORD                 dwServiceState,
                                     LPENUM_SERVICE_STATUSA lpServices,
                                     DWORD                 cbBufSize,
                                     LPDWORD               pcbBytesNeeded,
                                     LPDWORD               lpServicesReturned,
                                     LPDWORD               lpResumeHandle)
{
  dprintf(("ADVAPI32: EnumServicesStatusA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           dwServiceType,
           dwServiceState,
           lpServices,
           cbBufSize,
           pcbBytesNeeded,
           lpServicesReturned,
           lpResumeHandle));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : EnumServicesStatusW
 * Purpose   : The EnumServicesStatus function enumerates services in the specified
 *             service control manager database. The name and status of each service are provided.
 * Parameters: SC_HANDLE             hSCManager          handle of service control manager database
 *             DWORD                 dwServiceType       type of services to enumerate
 *             DWORD                 dwServiceState      state of services to enumerate
 *             LPENUM_SERVICE_STATUS lpServices          address of service status buffer
 *             DWORD                 cbBufSize           size of service status buffer
 *             LPDWORD               pcbBytesNeeded      address of variable for bytes needed
 *             LPDWORD               lpServicesReturned  address of variable for number returned
 *             LPDWORD               lpResumeHandle      address of variable for next entry
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EnumServicesStatusW(SC_HANDLE             hSCManager,
                                     DWORD                 dwServiceType,
                                     DWORD                 dwServiceState,
                                     LPENUM_SERVICE_STATUSW lpServices,
                                     DWORD                 cbBufSize,
                                     LPDWORD               pcbBytesNeeded,
                                     LPDWORD               lpServicesReturned,
                                     LPDWORD               lpResumeHandle)
{
  dprintf(("ADVAPI32: EnumServicesStatusW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           dwServiceType,
           dwServiceState,
           lpServices,
           cbBufSize,
           pcbBytesNeeded,
           lpServicesReturned,
           lpResumeHandle));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : GetServiceKeyNameA
 * Purpose   : The GetServiceKeyName function obtains the service name that is
 *             associated with a particular service's display name. The service
 *             name is the same as the service's registry key name.
 * Parameters: SC_HANDLE hSCManager     handle to a service control manager database
 *             LPCSTR   lpDisplayName  the service's display name
 *             LPTSTR    lpServiceName  buffer to receive the service name
 *             LPDWORD   lpcchBuffer    size of service name buffer and service name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetServiceKeyNameA(SC_HANDLE hSCManager,
                                 LPCSTR    lpDisplayName,
                                 LPTSTR    lpServiceName,
                                 LPDWORD   lpcchBuffer)
{
  dprintf(("ADVAPI32: GetServiceKeyNameA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpDisplayName,
           lpServiceName,
           lpcchBuffer));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetServiceKeyNameW
 * Purpose   : The GetServiceKeyName function obtains the service name that is
 *             associated with a particular service's display name. The service
 *             name is the same as the service's registry key name.
 * Parameters: SC_HANDLE hSCManager     handle to a service control manager database
 *             LPCWSTR   lpDisplayName  the service's display name
 *             LPWSTR    lpServiceName  buffer to receive the service name
 *             LPDWORD   lpcchBuffer    size of service name buffer and service name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetServiceKeyNameW(SC_HANDLE hSCManager,
                                    LPCWSTR   lpDisplayName,
                                    LPWSTR    lpServiceName,
                                    LPDWORD   lpcchBuffer)
{
  dprintf(("ADVAPI32: GetServiceKeyNameW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpDisplayName,
           lpServiceName,
           lpcchBuffer));

  return (FALSE); /* signal failure */
}

/*****************************************************************************
 * Name      : LockServiceDatabase
 * Purpose   : The LockServiceDatabase function locks a specified database.
 * Parameters: SC_HANDLE  hSCManager  handle of service control manager database
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_LOCK WIN32API LockServiceDatabase(SC_HANDLE hSCManager)
{
  dprintf(("ADVAPI32: LockServiceDatabase(%08xh) not implemented.\n",
           hSCManager));

  return 0; /* signal failure */
}

/*****************************************************************************
 * Name      : UnlockServiceDatabase
 * Purpose   : The UnlockServiceDatabase function unlocks a service control
 *             manager database by releasing the specified lock.
 * Parameters: SC_LOCK  sclLock  service control manager database lock to be released
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]

 *****************************************************************************/

BOOL WIN32API UnlockServiceDatabase(SC_LOCK sclLock)
{
  dprintf(("ADVAPI32: UnlockServiceDatabase(%08xh) not implemented.\n",
           sclLock));

  return (FALSE); /* signal failure */
}


