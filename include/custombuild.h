#ifndef __CUSTOMBUILD_H__
#define __CUSTOMBUILD_H__

//HKEY_LOCAL_MACHINE
#define CUSTOM_BUILD_OPTIONS_KEY  "System\\CustomBuild"
#define DISABLE_AUDIO_KEY         "DisableAudio"
#define DISABLE_ASPI_KEY          "DisableASPI"

extern BOOL fCustomBuild;

void InitDirectoriesCustom(char *szSystemDir, char *szWindowsDir);

void DisableOdinIni();
void DisableOdinSysMenuItems();


typedef HANDLE (* WIN32API PFNDRVOPEN)(LPVOID lpDriverData, DWORD dwAccess, DWORD dwShare, DWORD dwFlags, PVOID *ppHandleData);
typedef void   (* WIN32API PFNDRVCLOSE)(LPVOID lpDriverData, HANDLE hDevice, DWORD dwFlags, PVOID lpHandleData);
typedef BOOL   (* WIN32API PFNDRVIOCTL)(LPVOID lpDriverData, HANDLE hDevice, DWORD dwFlags, DWORD dwIoControlCode,
                                        LPVOID lpInBuffer, DWORD nInBufferSize,
                                        LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                        LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped,
                                        LPVOID lpHandleData);

typedef BOOL   (* WIN32API PFNDRVREAD)(LPVOID        lpDriverData, 
                                       HANDLE        hDevice,
                                       DWORD         dwFlags,
                                       LPCVOID       lpBuffer,
                                       DWORD         nNumberOfBytesToRead,
                                       LPDWORD       lpNumberOfBytesRead,
                                       LPOVERLAPPED  lpOverlapped,
                                       LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine,
                                       LPVOID        lpHandleData);

typedef BOOL   (* WIN32API PFNDRVWRITE)(LPVOID        lpDriverData, 
                                        HANDLE        hDevice,
                                        DWORD         dwFlags,
                                        LPCVOID       lpBuffer,
                                        DWORD         nNumberOfBytesToWrite,
                                        LPDWORD       lpNumberOfBytesWrite,
                                        LPOVERLAPPED  lpOverlapped,
                                        LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine,
                                        LPVOID        lpHandleData);

typedef BOOL   (* WIN32API PFNDRVCANCELIO)(LPVOID lpDriverData, HANDLE hDevice, DWORD dwFlags, PVOID lpHandleData);
typedef DWORD  (* WIN32API PFNDRVGETOVERLAPPEDRESULT)(LPVOID        lpDriverData, 
                                                      HANDLE        hDevice,
                                                      DWORD         dwFlags,
                                                      LPOVERLAPPED  lpOverlapped,
                                                      LPDWORD       lpcbTransfer,
                                                      BOOL          fWait,
                                                      LPVOID        lpHandleData);

BOOL WIN32API RegisterCustomDriver(PFNDRVOPEN pfnDriverOpen, PFNDRVCLOSE pfnDriverClose, 
                                   PFNDRVIOCTL pfnDriverIOCtl, PFNDRVREAD pfnDriverRead,
                                   PFNDRVWRITE pfnDriverWrite, PFNDRVCANCELIO pfnDriverCancelIo,
                                   PFNDRVGETOVERLAPPEDRESULT pfnDriverGetOverlappedResult,
                                   LPCSTR lpDriverName, LPVOID lpDriverData);

BOOL WIN32API UnRegisterCustomDriver(LPCSTR lpDriverName);


//SetDialogHook can be used by a custom Odin build to register a hook procedure
//that gets called before or after dialog creation

#define HCUSTOM_PREDIALOGCREATION	0
#define HCUSTOM_POSTDIALOGCREATION	1

BOOL WIN32API SetCustomDialogHook(HOOKPROC pfnDialogProc);
BOOL WIN32API ClearCustomDialogHook();

//Override FindResource function
typedef void (* WIN32API PFNFINDRESOURCEEXA)(HINSTANCE *phModule, LPSTR *lplpszName, LPSTR *lplpszType, WORD *lpLanguage);
typedef void (* WIN32API PFNFINDRESOURCEEXW)(HINSTANCE *phModule, LPWSTR *lplpszName, LPWSTR *lplpszType, WORD *lpLanguage);

BOOL WIN32API SetCustomFindResource(PFNFINDRESOURCEEXA pfnFindResourceA, PFNFINDRESOURCEEXW pfnFindResourceW);

//Set the default language in kernel32
void WIN32API SetDefaultLanguage(DWORD deflang);

//Override pm keyboard hook dll name
void WIN32API SetCustomPMHookDll(LPSTR pszKbdDllName);

//Turn off wave audio in winmm
void WIN32API DisableWaveAudio();

//Turn off ASPI
void WIN32API DisableASPI();

//force color to mono cursor conversion
void WIN32API CustForceMonoCursor();

#endif  /*__CUSTOMBUILD_H__*/

