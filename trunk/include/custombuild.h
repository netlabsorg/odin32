#ifndef __CUSTOMBUILD_H__
#define __CUSTOMBUILD_H__

//HKEY_LOCAL_MACHINE
#define CUSTOM_BUILD_OPTIONS_KEY  "System\\CustomBuild"
#define DISABLE_AUDIO_KEY         "DisableAudio"
#define DISABLE_ASPI_KEY          "DisableASPI"

extern BOOL fCustomBuild;

void WIN32API InitDirectoriesCustom(char *szSystemDir, char *szWindowsDir);

void WIN32API DisableOdinIni();

void WIN32API DisableOdinSysMenuItems();


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

//Call to tell winmm to expect simple fixed size buffers, so
//it doesn't have to use very small DART buffers; this will
//only work in very specific cases; it is not a good general
//purpose solution)
void WIN32API SetFixedWaveBufferSize();

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

//Disable OS2CDROM.DMD aspi support
void WIN32API DisableCDIo();

//Force DirectDraw to report only one fullscreen mode
void WIN32API SetCustomFullScreenMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);

//Force DirectSurface Lock & Unlock methods to hide & show the mouse cursor
BOOL WIN32API SetCustomHideCursorOnLock(BOOL state);

//Call to enable access to physical disks or volumes (default is disabled)
void WIN32API EnablePhysicalDiskAccess(BOOL fEnable);

//Override a system color without forcing a desktop repaint (which
//SetSysColors does)
BOOL WIN32API ODIN_SetSysColors(INT nChanges, const INT *lpSysColor,
                                const COLORREF *lpColorValues);

//Override loader names (PEC, PE, W16ODIN)
BOOL WIN32API ODIN_SetLoaders(LPCSTR pszPECmdLoader, LPCSTR pszPEGUILoader,
                              LPCSTR pszNELoader);

//Returns path of loaders
BOOL WIN32API ODIN_QueryLoaders(LPSTR pszPECmdLoader, INT cchPECmdLoader,
                                LPSTR pszPEGUILoader, INT cchPEGUILoader,
                                LPSTR pszNELoader, INT cchNELoader);

//Checks whether program is LX or PE
BOOL WIN32API ODIN_IsWin32App(LPSTR lpszProgramPath);

//Custom build function to disable loading of LX dlls
void WIN32API ODIN_DisableLXDllLoading();

/**
 * LoadLibrary*() callback function registered using ODIN_SetLxDllLoadCallback().
 * This is called for all LX modules which are loaded by LoadLibrary.
 *
 * @returns TRUE loading should succeed. The DLL is now registered.
 * @returns FALSE loading should fail.
 * @param   hmodOS2     The OS/2 module handle.
 * @param   hInstance   The Odin instance handle.
 *                      If not NULL this means that the DLL is registered.
 */
typedef BOOL (* WIN32API PFNLXDLLLOAD)(HMODULE hmodOS2, HINSTANCE hInstance);

/** Custombuild API for registering a callback for LX Dll loading thru LoadLibrary*(). */
BOOL WIN32API ODIN_SetLxDllLoadCallback(PFNLXDLLLOAD pfn);

/**
 * FreeLibrary*() callback function registered using ODIN_SetLxDllUnLoadCallback().
 * This is called when an LX which was previously loaded using LoadLibrary*() is
 * finally freed from Odin. The callback servers mainly the purpose of cleaning
 * up any data associated with the DLL.
 *
 * It is called *AFTER* the module is freed from Odin but before it's actually
 * unloaded from the OS/2 process. This means that the OS/2 handle is valid while
 * the Odin handle is invalid.
 *
 * @param   hmodOS2     The OS/2 module handle.
 * @param   hInstance   The Odin instance handle (invalid!).
 * @remark  It likely that this callback will be called for more DLLs than the
 *          load callback. Do check handles properly.
 */
typedef void (* WIN32API PFNLXDLLUNLOAD)(HMODULE hmodOS2, HINSTANCE hInstance);

BOOL WIN32API ODIN_SetLxDllUnLoadCallback(PFNLXDLLUNLOAD pfn);


//******************************************************************************
//Install a handler that is called before the entrypoint of a dll (DLL_PROCESS_ATTACH)
//******************************************************************************
typedef void (* WIN32API ODINPROC_DLLLOAD)(HMODULE hModule);

BOOL    WIN32API ODIN_SetDllLoadCallback(ODINPROC_DLLLOAD pfnMyDllLoad);


//******************************************************************************
// ODIN_SetProcAddress: Override a dll export
//
// Parameters:
//      HMODULE hModule		Module handle
//      LPCSTR  lpszProc	Export name or ordinal
//      FARPROC pfnNewProc	New export function address
//
// Returns: Success -> old address of export
//          Failure -> -1
//
//******************************************************************************
FARPROC WIN32API ODIN_SetProcAddress(HMODULE hModule, LPCSTR lpszProc, FARPROC pfnNewProc);

//******************************************************************************
// ODIN_SetTIBSwitch: override TIB switching
//
// Parameters:
//      BOOL fSwitchTIB
//              FALSE  -> no TIB selector switching
//              TRUE   -> force TIB selector switching
//
//******************************************************************************
void WIN32API ODIN_SetTIBSwitch(BOOL fSwitchTIB);

#endif  /*__CUSTOMBUILD_H__*/

