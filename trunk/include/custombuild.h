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

BOOL WIN32API RegisterCustomDriver(PFNDRVOPEN pfnDriverOpen, PFNDRVCLOSE pfnDriverClose, 
                                   PFNDRVIOCTL pfnDriverIOCtl, LPCSTR lpDeviceName);


//SetDialogHook can be used by a custom Odin build to register a hook procedure
//that gets called before or after dialog creation

#define HCUSTOM_PREDIALOGCREATION	0
#define HCUSTOM_POSTDIALOGCREATION	1

BOOL WIN32API SetDialogHook(HOOKPROC pfnDialogProc);
BOOL WIN32API ClearDialogHook();

#endif  /*__CUSTOMBUILD_H__*/

