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

#define HODIN_PREDIALOGCREATION		0
#define HODIN_POSTDIALOGCREATION	1
#define HODIN_WINDOWCREATED		2

HHOOK   WIN32API SetOdinHookA(HOOKPROC proc );
BOOL    WIN32API UnhookOdinHook(HHOOK hhook);
LRESULT HOOK_CallOdinHookA(INT code, WPARAM wParam, LPARAM lParam );

//Override LoadImage function
typedef void (* WIN32API PFNLOADIMAGEW)(HINSTANCE *phinst, LPWSTR *lplpszName, UINT *lpuType);

BOOL WIN32API SetCustomLoadImage(PFNLOADIMAGEW pfnLoadImageW);

//Set the default language in kernel32
void WIN32API SetDefaultLanguage(DWORD deflang);

//Override pm keyboard hook dll name
void WIN32API SetCustomPMHookDll(LPSTR pszKbdDllName);

void WIN32API DisableDragDrop(BOOL fDisabled);

//Turn off wave audio in winmm
void WIN32API DisableWaveAudio();

//Override shared semaphore name used to synchronize global window handle 
//array access (to avoid name clash with Odin)
void WIN32API SetCustomWndHandleSemName(LPSTR pszSemName);

//Override shared semaphore name used to synchronize global memory map  
//list access (to avoid name clash with Odin)
void WIN32API SetCustomMMapSemName(LPSTR pszSemName);

//Turn off ASPI
void WIN32API DisableASPI();

//force color to mono cursor conversion
void WIN32API CustForceMonoCursor();

//force GetVolumeInformation to tell the app all partitions are FAT
void WIN32API CustForce2GBFileSize();

//Logging of exceptions:
//Override filename of exception log (expects full path)
void WIN32API SetCustomExceptionLog(LPSTR lpszLogName);
//Enable/disable exception logging
void WIN32API SetExceptionLogging(BOOL fEnable);


#endif  /*__CUSTOMBUILD_H__*/

