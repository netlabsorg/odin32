#ifndef __CUSTOMBUILD_H__
#define __CUSTOMBUILD_H__

//HKEY_LOCAL_MACHINE
#define CUSTOM_BUILD_OPTIONS_KEY  "System\\CustomBuild"
#define DISABLE_AUDIO_KEY         "DisableAudio"

extern BOOL fCustomBuild;

void InitDirectoriesCustom(char *szSystemDir, char *szWindowsDir);

void DisableOdinIni();
void DisableOdinSysMenuItems();


typedef HANDLE (* WIN32API PFNDRVOPEN)(DWORD dwAccess, DWORD dwShare, DWORD dwFlags, PVOID *ppDriverData);
typedef void   (* WIN32API PFNDRVCLOSE)(HANDLE hDevice, DWORD dwFlags, PVOID lpDriverData);
typedef BOOL   (* WIN32API PFNDRVIOCTL)(HANDLE hDevice, DWORD dwFlags, DWORD dwIoControlCode,
                                        LPVOID lpInBuffer, DWORD nInBufferSize,
                                        LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                        LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped,
                                        PVOID lpDriverData);

typedef BOOL   (* WIN32API PFNDRVREAD)(HANDLE        hDevice,
                                       DWORD         dwFlags,
                                       LPCVOID       lpBuffer,
                                       DWORD         nNumberOfBytesToRead,
                                       LPDWORD       lpNumberOfBytesRead,
                                       LPOVERLAPPED  lpOverlapped,
                                       LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine,
                                       PVOID         lpDriverData);

typedef BOOL   (* WIN32API PFNDRVWRITE)(HANDLE        hDevice,
                                        DWORD         dwFlags,
                                        LPCVOID       lpBuffer,
                                        DWORD         nNumberOfBytesToWrite,
                                        LPDWORD       lpNumberOfBytesWrite,
                                        LPOVERLAPPED  lpOverlapped,
                                        LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine,
                                        PVOID         lpDriverData);

typedef BOOL   (* WIN32API PFNDRVCANCELIO)(HANDLE hDevice, DWORD dwFlags, PVOID lpDriverData);
typedef DWORD  (* WIN32API PFNDRVGETOVERLAPPEDRESULT)(HANDLE        hDevice,
                                                      DWORD         dwFlags,
                                                      LPOVERLAPPED  lpOverlapped,
                                                      LPDWORD       lpcbTransfer,
                                                      BOOL          fWait,
                                                      PVOID         lpDriverData);

BOOL WIN32API RegisterCustomDriver(PFNDRVOPEN pfnDriverOpen, PFNDRVCLOSE pfnDriverClose, 
                                   PFNDRVIOCTL pfnDriverIOCtl, PFNDRVREAD pfnDriverRead,
                                   PFNDRVWRITE pfnDriverWrite, PFNDRVCANCELIO pfnDriverCancelIo,
                                   PFNDRVGETOVERLAPPEDRESULT pfnDriverGetOverlappedResult,
                                   LPCSTR lpDeviceName);


//SetDialogHook can be used by a custom Odin build to register a hook procedure
//that gets called before or after dialog creation

#define HCUSTOM_PREDIALOGCREATION	0
#define HCUSTOM_POSTDIALOGCREATION	1

BOOL WIN32API SetDialogHook(HOOKPROC pfnDialogProc);
BOOL WIN32API ClearDialogHook();


//Set the default language in kernel32
void WIN32API SetDefaultLanguage(DWORD deflang);

//Override pm keyboard hook dll name
void WIN32API SetCustomPMHookDll(LPSTR pszKbdDllName);

//Turn off wave audio in winmm
void WIN32API DisableWaveAudio();

#endif  /*__CUSTOMBUILD_H__*/

