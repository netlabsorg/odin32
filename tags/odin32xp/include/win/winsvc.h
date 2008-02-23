/* $Id: winsvc.h,v 1.1 1999-12-21 00:29:31 sandervl Exp $ */
#ifndef __WINSVC_H__
#define __WINSVC_H__

#include "winbase.h"

#define SERVICES_ACTIVE_DATABASEW      L"ServicesActive"
#define SERVICES_FAILED_DATABASEW      L"ServicesFailed"

#define SERVICES_ACTIVE_DATABASEA      "ServicesActive"
#define SERVICES_FAILED_DATABASEA      "ServicesFailed"

#define SC_GROUP_IDENTIFIERW           L'+'
#define SC_GROUP_IDENTIFIERA           '+'

#ifdef UNICODE
#define SERVICES_ACTIVE_DATABASE       SERVICES_ACTIVE_DATABASEW
#define SERVICES_FAILED_DATABASE       SERVICES_FAILED_DATABASEW
#define SC_GROUP_IDENTIFIER            SC_GROUP_IDENTIFIERW
#else
#define SERVICES_ACTIVE_DATABASE       SERVICES_ACTIVE_DATABASEA
#define SERVICES_FAILED_DATABASE       SERVICES_FAILED_DATABASEA
#define SC_GROUP_IDENTIFIER            SC_GROUP_IDENTIFIERA
#endif


#define SERVICE_NO_CHANGE              0xffffffff

#define SERVICE_ACTIVE                 0x00000001
#define SERVICE_INACTIVE               0x00000002
#define SERVICE_STATE_ALL              (SERVICE_ACTIVE|SERVICE_INACTIVE)

#define SERVICE_CONTROL_STOP           0x00000001
#define SERVICE_CONTROL_PAUSE          0x00000002
#define SERVICE_CONTROL_CONTINUE       0x00000003
#define SERVICE_CONTROL_INTERROGATE    0x00000004
#define SERVICE_CONTROL_SHUTDOWN       0x00000005

#define SERVICE_STOPPED                0x00000001
#define SERVICE_START_PENDING          0x00000002
#define SERVICE_STOP_PENDING           0x00000003
#define SERVICE_RUNNING                0x00000004
#define SERVICE_CONTINUE_PENDING       0x00000005
#define SERVICE_PAUSE_PENDING          0x00000006
#define SERVICE_PAUSED                 0x00000007

#define SERVICE_ACCEPT_STOP            0x00000001
#define SERVICE_ACCEPT_PAUSE_CONTINUE  0x00000002
#define SERVICE_ACCEPT_SHUTDOWN        0x00000004

#define SC_MANAGER_CONNECT             0x0001
#define SC_MANAGER_CREATE_SERVICE      0x0002
#define SC_MANAGER_ENUMERATE_SERVICE   0x0004
#define SC_MANAGER_LOCK                0x0008
#define SC_MANAGER_QUERY_LOCK_STATUS   0x0010
#define SC_MANAGER_MODIFY_BOOT_CONFIG  0x0020

#define SC_MANAGER_ALL_ACCESS          (STANDARD_RIGHTS_REQUIRED|SC_MANAGER_CONNECT| \
                                        SC_MANAGER_CREATE_SERVICE|SC_MANAGER_ENUMERATE_SERVICE| \
                                        SC_MANAGER_LOCK|SC_MANAGER_QUERY_LOCK_STATUS| \
                                        SC_MANAGER_MODIFY_BOOT_CONFIG)

#define SERVICE_QUERY_CONFIG           0x0001
#define SERVICE_CHANGE_CONFIG          0x0002
#define SERVICE_QUERY_STATUS           0x0004
#define SERVICE_ENUMERATE_DEPENDENTS   0x0008
#define SERVICE_START                  0x0010
#define SERVICE_STOP                   0x0020
#define SERVICE_PAUSE_CONTINUE         0x0040
#define SERVICE_INTERROGATE            0x0080
#define SERVICE_USER_DEFINED_CONTROL   0x0100

#define SERVICE_ALL_ACCESS             (STANDARD_RIGHTS_REQUIRED|SERVICE_QUERY_CONFIG| \
                                        SERVICE_CHANGE_CONFIG|SERVICE_QUERY_STATUS| \
                                        SERVICE_ENUMERATE_DEPENDENTS|SERVICE_START| \
                                        SERVICE_STOP|SERVICE_PAUSE_CONTINUE| \
                                        SERVICE_INTERROGATE|SERVICE_USER_DEFINED_CONTROL)

typedef HANDLE      	SC_HANDLE;
typedef SC_HANDLE      *LPSC_HANDLE;
typedef LPVOID  	SC_LOCK;
typedef DWORD       	SERVICE_STATUS_HANDLE;
typedef VOID (* WINAPI LPHANDLER_FUNCTION)(DWORD dwControl);


typedef struct _SERVICE_STATUS {
    DWORD   dwServiceType;
    DWORD   dwCurrentState;
    DWORD   dwControlsAccepted;
    DWORD   dwWin32ExitCode;
    DWORD   dwServiceSpecificExitCode;
    DWORD   dwCheckPoint;
    DWORD   dwWaitHint;
} SERVICE_STATUS, *LPSERVICE_STATUS;

typedef struct _ENUM_SERVICE_STATUSA {
    LPSTR          lpServiceName;
    LPSTR          lpDisplayName;
    SERVICE_STATUS ServiceStatus;
} ENUM_SERVICE_STATUSA, *LPENUM_SERVICE_STATUSA;

typedef struct _ENUM_SERVICE_STATUSW {
    LPWSTR         lpServiceName;
    LPWSTR         lpDisplayName;
    SERVICE_STATUS ServiceStatus;
} ENUM_SERVICE_STATUSW, *LPENUM_SERVICE_STATUSW;

typedef struct _QUERY_SERVICE_LOCK_STATUSA {
    DWORD   fIsLocked;
    LPSTR   lpLockOwner;
    DWORD   dwLockDuration;
} QUERY_SERVICE_LOCK_STATUSA, *LPQUERY_SERVICE_LOCK_STATUSA;

typedef struct _QUERY_SERVICE_LOCK_STATUSW {
    DWORD   fIsLocked;
    LPWSTR  lpLockOwner;
    DWORD   dwLockDuration;
} QUERY_SERVICE_LOCK_STATUSW, *LPQUERY_SERVICE_LOCK_STATUSW;

typedef struct _QUERY_SERVICE_CONFIGA {
    DWORD   dwServiceType;
    DWORD   dwStartType;
    DWORD   dwErrorControl;
    LPSTR   lpBinaryPathName;
    LPSTR   lpLoadOrderGroup;
    DWORD   dwTagId;
    LPSTR   lpDependencies;
    LPSTR   lpServiceStartName;
    LPSTR   lpDisplayName;
} QUERY_SERVICE_CONFIGA, *LPQUERY_SERVICE_CONFIGA;

typedef struct _QUERY_SERVICE_CONFIGW {
    DWORD   dwServiceType;
    DWORD   dwStartType;
    DWORD   dwErrorControl;
    LPWSTR  lpBinaryPathName;
    LPWSTR  lpLoadOrderGroup;
    DWORD   dwTagId;
    LPWSTR  lpDependencies;
    LPWSTR  lpServiceStartName;
    LPWSTR  lpDisplayName;
} QUERY_SERVICE_CONFIGW, *LPQUERY_SERVICE_CONFIGW;

/* service main function prototype */
typedef VOID (* CALLBACK LPSERVICE_MAIN_FUNCTIONA)(DWORD,LPSTR);
typedef VOID (* CALLBACK LPSERVICE_MAIN_FUNCTIONW)(DWORD,LPWSTR);
DECL_WINELIB_TYPE_AW(LPSERVICE_MAIN_FUNCTION)

/* service start table */
typedef struct
{
    LPSTR			lpServiceName;
    LPSERVICE_MAIN_FUNCTIONA	lpServiceProc;
} *LPSERVICE_TABLE_ENTRYA, SERVICE_TABLE_ENTRYA;

typedef struct
{
    LPWSTR			lpServiceName;
    LPSERVICE_MAIN_FUNCTIONW	lpServiceProc;
} *LPSERVICE_TABLE_ENTRYW, SERVICE_TABLE_ENTRYW;

DECL_WINELIB_TYPE_AW(SERVICE_TABLE_ENTRY)
DECL_WINELIB_TYPE_AW(LPSERVICE_TABLE_ENTRY)

#ifdef UNICODE
typedef QUERY_SERVICE_LOCK_STATUSW 	QUERY_SERVICE_LOCK_STATUS;
typedef LPQUERY_SERVICE_LOCK_STATUSW 	LPQUERY_SERVICE_LOCK_STATUS;
typedef ENUM_SERVICE_STATUSW 	ENUM_SERVICE_STATUS;
typedef LPENUM_SERVICE_STATUSW 	LPENUM_SERVICE_STATUS;
typedef QUERY_SERVICE_CONFIGW 	QUERY_SERVICE_CONFIG;
typedef LPQUERY_SERVICE_CONFIGW LPQUERY_SERVICE_CONFIG;
#else
typedef ENUM_SERVICE_STATUSA 	ENUM_SERVICE_STATUS;
typedef LPENUM_SERVICE_STATUSA 	LPENUM_SERVICE_STATUS;
typedef QUERY_SERVICE_LOCK_STATUSA 	QUERY_SERVICE_LOCK_STATUS;
typedef LPQUERY_SERVICE_LOCK_STATUSA 	LPQUERY_SERVICE_LOCK_STATUS;
typedef QUERY_SERVICE_CONFIGA 	QUERY_SERVICE_CONFIG;
typedef LPQUERY_SERVICE_CONFIGA LPQUERY_SERVICE_CONFIG;
#endif

HANDLE    WINAPI OpenSCManagerA(LPCSTR,LPCSTR,DWORD);
HANDLE    WINAPI OpenSCManagerW(LPCWSTR,LPCWSTR,DWORD);
#define     OpenSCManager WINELIB_NAME_AW(OpenSCManager)
HANDLE    WINAPI OpenServiceA(SC_HANDLE,LPCSTR,DWORD);
HANDLE    WINAPI OpenServiceW(SC_HANDLE,LPCWSTR,DWORD);
#define     OpenService WINELIB_NAME_AW(OpenService)

BOOL      WINAPI CloseServiceHandle(SC_HANDLE);
BOOL      WINAPI ControlService(SC_HANDLE,DWORD,LPSERVICE_STATUS);
BOOL      WINAPI DeleteService(SC_HANDLE);

BOOL      WINAPI StartServiceA(SC_HANDLE,DWORD,LPCSTR*);
BOOL      WINAPI StartServiceW(SC_HANDLE,DWORD,LPCWSTR*);
#define     StartService WINELIB_NAME_AW(StartService)


BOOL WINAPI QueryServiceConfigA(SC_HANDLE              schService,
                                     LPQUERY_SERVICE_CONFIG lpqscServConfig,
                                     DWORD                  cbBufSize,
                                     LPDWORD                lpcbBytesNeeded);

BOOL WINAPI QueryServiceConfigW(SC_HANDLE              schService,
                                     LPQUERY_SERVICE_CONFIG lpqscServConfig,
                                     DWORD                  cbBufSize,
                                     LPDWORD                lpcbBytesNeeded);

#define     QueryServiceConfig WINELIB_NAME_AW(QueryServiceConfig)


BOOL WINAPI QueryServiceLockStatusA(SC_HANDLE                   schSCManager,
                                         LPQUERY_SERVICE_LOCK_STATUSA lpqslsLockStat,
                                         DWORD                       cbBufSize,
                                         LPDWORD                     lpcbBytesNeeded);

BOOL WINAPI QueryServiceLockStatusW(SC_HANDLE                   schSCManager,
                                         LPQUERY_SERVICE_LOCK_STATUSW lpqslsLockStat,
                                         DWORD                       cbBufSize,
                                         LPDWORD                     lpcbBytesNeeded);

#define     QueryServiceLockStatus WINELIB_NAME_AW(QueryServiceLockStatus)

BOOL WINAPI QueryServiceObjectSecurity(SC_HANDLE             schService,
                                            SECURITY_INFORMATION  fdwSecurityInfo,
                                            PSECURITY_DESCRIPTOR  psdSecurityDesc,
                                            DWORD                 cbBufSize,
                                            LPDWORD               lpcbBytesNeeded);

BOOL WINAPI QueryServiceStatus(SC_HANDLE         schService,
                                 LPSERVICE_STATUS  lpssServiceStatus);

BOOL WINAPI SetServiceObjectSecurity(SC_HANDLE             schService,
                                          SECURITY_INFORMATION  fdwSecurityInfo,
                                          PSECURITY_DESCRIPTOR  psdSecurityDesc);

BOOL WINAPI ChangeServiceConfigA(SC_HANDLE hService,
                                      DWORD     dwServiceType,
                                      DWORD     dwStartType,
                                      DWORD     dwErrorControl,
                                      LPCSTR   lpBinaryPathName,
                                      LPCSTR   lpLoadOrderGroup,
                                      LPDWORD   lpdwTagId,
                                      LPCSTR   lpDependencies,
                                      LPCSTR   lpServiceStartName,
                                      LPCSTR   lpPassword,
                                      LPCSTR   lpDisplayName);


BOOL WINAPI ChangeServiceConfigW(SC_HANDLE hService,
                                      DWORD     dwServiceType,
                                      DWORD     dwStartType,
                                      DWORD     dwErrorControl,
                                      LPCWSTR   lpBinaryPathName,
                                      LPCWSTR   lpLoadOrderGroup,
                                      LPDWORD   lpdwTagId,
                                      LPCWSTR   lpDependencies,
                                      LPCWSTR   lpServiceStartName,
                                      LPCWSTR   lpPassword,
                                      LPCWSTR   lpDisplayName);

#define     ChangeServiceConfig WINELIB_NAME_AW(ChangeServiceConfig)

SC_HANDLE WINAPI CreateServiceA(SC_HANDLE hSCManager,
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
                                  LPCSTR    lpPassword);

SC_HANDLE WINAPI CreateServiceW(SC_HANDLE hSCManager,
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
                                     LPCWSTR   lpPassword);

#define     CreateService WINELIB_NAME_AW(CreateService)

BOOL WINAPI GetServiceDisplayNameA(SC_HANDLE hSCManager,
                                     LPCSTR    lpServiceName,
                                     LPTSTR    lpDisplayName,
                                     LPDWORD   lpcchBuffer);

BOOL WINAPI GetServiceDisplayNameW(SC_HANDLE hSCManager,
                                     LPCWSTR   lpServiceName,
                                     LPWSTR    lpDisplayName,
                                     LPDWORD   lpcchBuffer);

#define     GetServiceDisplayName WINELIB_NAME_AW(GetServiceDisplayName)

BOOL WINAPI EnumDependentServicesA(SC_HANDLE             hService,
                                        DWORD                 dwServiceState,
                                        LPENUM_SERVICE_STATUSA lpServices,
                                        DWORD                 cbBufSize,
                                        LPDWORD               pcbBytesNeeded,
                                        LPDWORD               lpServicesReturned);

BOOL WINAPI EnumDependentServicesW(SC_HANDLE             hService,
                                        DWORD                 dwServiceState,
                                        LPENUM_SERVICE_STATUSW lpServices,
                                        DWORD                 cbBufSize,
                                        LPDWORD               pcbBytesNeeded,
                                        LPDWORD               lpServicesReturned);

#define     EnumDependentServices WINELIB_NAME_AW(EnumDependentServices)

BOOL WINAPI EnumServicesStatusA(SC_HANDLE             hSCManager,
                                     DWORD                 dwServiceType,
                                     DWORD                 dwServiceState,
                                     LPENUM_SERVICE_STATUSA lpServices,
                                     DWORD                 cbBufSize,
                                     LPDWORD               pcbBytesNeeded,
                                     LPDWORD               lpServicesReturned,
                                     LPDWORD               lpResumeHandle);

BOOL WINAPI EnumServicesStatusW(SC_HANDLE             hSCManager,
                                     DWORD                 dwServiceType,
                                     DWORD                 dwServiceState,
                                     LPENUM_SERVICE_STATUSW lpServices,
                                     DWORD                 cbBufSize,
                                     LPDWORD               pcbBytesNeeded,
                                     LPDWORD               lpServicesReturned,
                                     LPDWORD               lpResumeHandle);

#define     EnumServicesStatus WINELIB_NAME_AW(EnumServicesStatus)

BOOL WINAPI GetServiceKeyNameA(SC_HANDLE hSCManager,
                                 LPCSTR    lpDisplayName,
                                 LPTSTR    lpServiceName,
                                 LPDWORD   lpcchBuffer);

BOOL WINAPI GetServiceKeyNameW(SC_HANDLE hSCManager,
                                    LPCWSTR   lpDisplayName,
                                    LPWSTR    lpServiceName,
                                    LPDWORD   lpcchBuffer);

#define     GetServiceKeyName WINELIB_NAME_AW(GetServiceKeyName)

SC_LOCK WINAPI LockServiceDatabase(SC_HANDLE hSCManager);

BOOL WINAPI UnlockServiceDatabase(SC_LOCK sclLock);

#endif // __WINSVC_H__
