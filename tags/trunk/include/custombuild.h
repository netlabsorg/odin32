#ifndef __CUSTOMBUILD_H__
#define __CUSTOMBUILD_H__

//HKEY_LOCAL_MACHINE
#define CUSTOM_BUILD_OPTIONS_KEY  "System\\CustomBuild"
#define DISABLE_AUDIO_KEY         "DisableAudio"

extern BOOL fCustomBuild;

void InitDirectoriesCustom(char *szSystemDir, char *szWindowsDir);

void DisableOdinIni();
void DisableOdinSysMenuItems();


typedef HANDLE (* WIN32API PFNDRVOPEN)(DWORD dwAccess, DWORD dwShare);
typedef void   (* WIN32API PFNDRVCLOSE)(HANDLE hDevice);
typedef BOOL   (* WIN32API PFNDRVIOCTL)(HANDLE hDevice, DWORD dwIoControlCode,
                                        LPVOID lpInBuffer, DWORD nInBufferSize,
                                        LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                        LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

typedef BOOL   (* WIN32API PFNDRVREAD)(HANDLE        hDevice,
                                       LPCVOID       lpBuffer,
                                       DWORD         nNumberOfBytesToRead,
                                       LPDWORD       lpNumberOfBytesRead,
                                       LPOVERLAPPED  lpOverlapped,
                                       LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);

typedef BOOL   (* WIN32API PFNDRVWRITE)(HANDLE        hDevice,
                                        LPCVOID       lpBuffer,
                                        DWORD         nNumberOfBytesToWrite,
                                        LPDWORD       lpNumberOfBytesWrite,
                                        LPOVERLAPPED  lpOverlapped,
                                        LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine);


BOOL WIN32API RegisterCustomDriver(PFNDRVOPEN pfnDriverOpen, PFNDRVCLOSE pfnDriverClose, 
                                   PFNDRVIOCTL pfnDriverIOCtl, PFNDRVREAD pfnDriverRead,
                                   PFNDRVWRITE pfnDriverWrite, LPCSTR lpDeviceName);


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

#endif  /*__CUSTOMBUILD_H__*/

