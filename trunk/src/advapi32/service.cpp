/* $Id: service.cpp,v 1.1 1999-12-19 19:53:37 sandervl Exp $ */

/*
 * Win32 advanced API functions for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include "misc.h"
#include "advapi32.h"
#include "unicode.h"
#include "winreg.h"

ODINDEBUGCHANNEL(ADVAPI32-SERVICE)

#define SC_HANDLE HANDLE
#define SC_LOCK DWORD

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
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API OpenSCManagerA(LPCSTR lpszMachineName,
                                  LPCSTR lpszDatabaseName,
                                  DWORD   fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenSCManagerA(%s,%s,%x) not implemented.\n",
           lpszMachineName,
           lpszDatabaseName,
           fdwDesiredAccess));

  return (NULL); /* signal failure */
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
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API OpenSCManagerW(LPCWSTR lpszMachineName,
                                     LPCWSTR lpszDatabaseName,
                                     DWORD   fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenSCManagerW(%x,%x,%x) not implemented.\n",
           lpszMachineName,
           lpszDatabaseName,
           fdwDesiredAccess));

  return (NULL); /* signal failure */
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
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API OpenServiceA(SC_HANDLE schSCManager,
                                   LPCSTR   lpszServiceName,
                                   DWORD     fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenServiceA(%08xh,%s,%08xh) not implemented.\n",
           schSCManager,
           lpszServiceName,
           fdwDesiredAccess));

  return (NULL); /* signal failure */
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
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API OpenServiceW(SC_HANDLE schSCManager,
                                   LPCWSTR   lpszServiceName,
                                   DWORD     fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenServiceW(%08xh,%s,%08xh) not implemented.\n",
           schSCManager,
           lpszServiceName,
           fdwDesiredAccess));

  return (NULL); /* signal failure */
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
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define LPQUERY_SERVICE_CONFIG LPVOID
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

#define LPQUERY_SERVICE_LOCK_STATUS LPVOID
BOOL WIN32API QueryServiceLockStatusA(SC_HANDLE                   schSCManager,
                                         LPQUERY_SERVICE_LOCK_STATUS lpqslsLockStat,
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
                                         LPQUERY_SERVICE_LOCK_STATUS lpqslsLockStat,
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
  dprintf(("ADVAPI32: QueryServiceStatus(%08xh,%08xh) not implemented.\n",
           schService,
           lpssServiceStatus));

  return (FALSE); /* signal failure */
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
  dprintf(("ADVAPI32: CloseServiceHandle(%08xh) not implemented.\n",
           hSCObject));

  return (FALSE); /* signal failure */
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
  dprintf(("ADVAPI32: ControlService(%08xh,%08xh,%08xh) not implemented.\n",
           hService,
           dwControl,
           lpServiceStatus));

  return (FALSE); /* signal failure */
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
                                     LPCSTR   lpServiceName,
                                     LPCSTR   lpDisplayName,
                                     DWORD     dwDesiredAccess,
                                     DWORD     dwServiceType,
                                     DWORD     dwStartType,
                                     DWORD     dwErrorControl,
                                     LPCSTR   lpBinaryPathName,
                                     LPCSTR   lpLoadOrderGroup,
                                     LPDWORD   lpdwTagId,
                                     LPCSTR   lpDependencies,
                                     LPCSTR   lpServiceStartName,
                                     LPCSTR   lpPassword)
{
  dprintf(("ADVAPI32: CreateServiceA(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s) not implemented.\n",
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

  return (NULL); /* signal failure */
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
  dprintf(("ADVAPI32: CreateServiceW(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s) not implemented.\n",
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

  return (NULL); /* signal failure */
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
  dprintf(("ADVAPI32: DeleteService(%08xh) not implemented.\n",
           hService));

  return (FALSE); /* signal failure */
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

#define LPENUM_SERVICE_STATUS LPVOID
BOOL WIN32API EnumDependentServicesA(SC_HANDLE             hService,
                                        DWORD                 dwServiceState,
                                        LPENUM_SERVICE_STATUS lpServices,
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
                                        LPENUM_SERVICE_STATUS lpServices,
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
                                     LPENUM_SERVICE_STATUS lpServices,
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
                                     LPENUM_SERVICE_STATUS lpServices,
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
                                        LPCSTR   lpServiceName,
                                        LPTSTR    lpDisplayName,
                                        LPDWORD   lpcchBuffer)
{
  dprintf(("ADVAPI32: GetServiceDisplayNameA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           lpcchBuffer));

  return (FALSE); /* signal failure */
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
  dprintf(("ADVAPI32: GetServiceDisplayNameW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           lpcchBuffer));

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
                                    LPCSTR   lpDisplayName,
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

  return (ERROR_ACCESS_DENIED); /* signal failure */
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

  return (FALSE); /* signal failure */
}

