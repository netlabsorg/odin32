#ifndef __CUSTOMBUILD_H__
#define __CUSTOMBUILD_H__

#include <win\peexe.h>
#include <initdll.h>

//HKEY_LOCAL_MACHINE
#define CUSTOM_BUILD_OPTIONS_KEY  "System\\CustomBuild"
#define DISABLE_AUDIO_KEY         "DisableAudio"
#define DISABLE_ASPI_KEY          "DisableASPI"
#define ENABLE_POSTSCRIPT_PASSTHROUGH "EnablePostscriptPassthrough"

#define ODIN_WIN32_CDCLASS        "Win32CDWindowClass"
#define ODIN_WIN32_STDCLASS       "Win32WindowClass"

#define DUMMY_PREFIX 	          "DUMMY_"

#define MAX_FONT_MAPPINGS	  8
#define MAX_REGISTER_DLLS         64

#define INNOWIN_DLLNAME        	   "INNOWIN.DLL"
#define INNOWIN_REGISTRY_BASE      "Software\\InnoTek\\INNOWIN\\"
#define INNOWIN_REG_CURRENTUSER    "REGROOT_HKEY_CurrentUser"
#define INNOWIN_REG_LOCAL_MACHINE  "REGROOT_HKEY_LocalMachine"
#define INNOWIN_REG_USERS          "REGROOT_HKEY_Users"

#define MAKE_BUILDNR(major, minor)	((major << 16) | minor)
#define MAJOR_BUILDNR(buildnr)		(buildnr >> 16)
#define MINOR_BUILDNR(buildnr) 		(buildnr & 0xffff)


typedef BOOL (WIN32API *PFN_PRECUSTOMIZE)();
typedef BOOL (WIN32API *PFN_POSTCUSTOMIZE)();
typedef BOOL (WIN32API *PFN_ENDCUSTOMIZE)();
typedef BOOL (WIN32API *PFN_ISPESTUBLOADER)(char *pszProgram);

typedef struct {
  char               *szWindowsFont;
  char               *szPMFont;
} CUSTOMBUILD_FONTMAP;

typedef struct {
  char               *szName;	 //caps, including extension (e.g. "KERNEL32.DLL")
  PIMAGE_FILE_HEADER  pfh;       //PE file header
  PFN_INITDLL         pfnInitterm;
} CUSTOMBUILD_DLL;

typedef struct {
  char               *szName;
  PFN_INITDLL         pfnInitterm;
} CUSTOMBUILD_PRIVATE_INITTERM;

typedef struct {
  PFN_PRECUSTOMIZE    pfnPreCustomize;	//called after kernel32 is initialized
  PFN_POSTCUSTOMIZE   pfnPostCustomize;	//called at the end of dll load
  PFN_ENDCUSTOMIZE    pfnEndCustomize;  //called when dll is unloaded

  char               *szCustomBuildDllName;

  char               *szRegistryBase;
  char               *szMemMapName;
  char               *szEnvExceptLogDisable;
  char               *szEnvExceptLogPath;
  char               *szPMWindowClassName;
  char               *szWindowHandleSemName;

  //standard kernel32 settings
  DWORD               dwWindowsVersion;
  BOOL                fOdinIni;
  BOOL                fSMP;

  //standard user32 settings
  BOOL                fDragDrop;
  BOOL                fOdinSysMenuItems;
  DWORD               dwWindowAppearance;
  BOOL                fMonoCursor;

  //standard gdi32 settings
  //font mappings (null terminated)
  CUSTOMBUILD_FONTMAP fontMapping[MAX_FONT_MAPPINGS];
  BOOL                fFreeType;

  //winmm
  BOOL                fDirectAudio;
  BOOL                fWaveAudio;

  //first three important dlls (initialized in this order
  CUSTOMBUILD_DLL     dllNtdll;
  CUSTOMBUILD_DLL     dllKernel32;
  CUSTOMBUILD_DLL     dllUser32;
  CUSTOMBUILD_DLL     dllGdi32;

  //list of remainder of registered dlls (order is important due to dependencies!!)
  //(null terminated)
  CUSTOMBUILD_DLL     registeredDll[MAX_REGISTER_DLLS];

  //list of dummy dlls (to prevent accidental load)
  //(null terminated)
  CUSTOMBUILD_DLL     dummyDll[MAX_REGISTER_DLLS];

} CUSTOMBUILD;

typedef struct {
  PFN_PRECUSTOMIZE    pfnPreCustomize;	//called after kernel32, user32 and gdi32 are initialized
  PFN_POSTCUSTOMIZE   pfnPostCustomize;	//called at the end of dll load
  PFN_ENDCUSTOMIZE    pfnEndCustomize;  //called when dll is unloaded
  PFN_ISPESTUBLOADER  pfnIsPeStubLoader;

  char               *szCustomBuildDllName;

  char               *szRegistryBase;
  char               *szEnvExceptLogDisable;
  char               *szEnvExceptLogPath;

  //standard kernel32 settings
  DWORD               dwWindowsVersion;
  BOOL                fOdinIni;
  BOOL                fSMP;

  //standard user32 settings
  BOOL                fDragDrop;
  BOOL                fOdinSysMenuItems;
  DWORD               dwWindowAppearance;
  BOOL                fMonoCursor;

  //standard gdi32 settings
  //font mappings (null terminated)
  CUSTOMBUILD_FONTMAP fontMapping[MAX_FONT_MAPPINGS];
  BOOL                fFreeType;

  //winmm
  BOOL                fDirectAudio;
  BOOL                fWaveAudio;

  //list of remainder of registered dlls (order is important due to dependencies!!)
  //(null terminated)
  //(NTDLL, KERNEL32, USER32 and GDI32 are always registered)
  CUSTOMBUILD_DLL     registeredDll[MAX_REGISTER_DLLS];

} CUSTOMBUILD_COMMON;

BOOL WIN32API LoadCustomEnvironment(CUSTOMBUILD_COMMON *CustomBuild);
BOOL WIN32API UnloadCustomEnvironment(BOOL fExitList);

BOOL WIN32API InitExecutableEnvironment();
BOOL WIN32API CheckCustomDllVersion(ULONG ulVersionMajor, ULONG ulVersionMinor);

typedef BOOL (* WIN32API PFN_CHECKDLLVERSION)();
BOOL WIN32API CheckDllVersion();

extern BOOL fCustomBuild;

void WIN32API SetRegistryRootKey(HKEY hRootkey, HKEY hKey);
#ifdef __cplusplus
void WIN32API SetCustomBuildName(char *lpszName, PIMAGE_FILE_HEADER pfh = NULL);
#else
void WIN32API SetCustomBuildName(char *lpszName, PIMAGE_FILE_HEADER pfh);
#endif

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
#define HODIN_PREWINDOWCREATEDA		3

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
//Turn off usage of the Uniaud DirectAudio interface
void WIN32API DisableDirectAudio();

//Override shared semaphore name used to synchronize global window handle
//array access (to avoid name clash with Odin)
void WIN32API SetCustomWndHandleSemName(LPSTR pszSemName);

//Override shared semaphore name used to synchronize global memory map
//list access (to avoid name clash with Odin)
void WIN32API SetCustomMMapSemName(LPSTR pszSemName);

//Override std class names used in Odin
void  WIN32API SetCustomStdClassName(LPSTR pszStdClassName);
char *WIN32API QueryCustomStdClassName();

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

//******************************************************************************
// ODIN_DisableFolderShellLink
//
// Disable object creation in Odin folder. Desktop shortcuts will still be
// created as WPS objects on the desktop.
//
//******************************************************************************
void WIN32API ODIN_DisableFolderShellLink();

//******************************************************************************
// ODIN_waveOutSetFixedBuffers
//
// Tell WINMM to use DART buffers of the same size as the first buffer delivered
// by waveOutWrite
//
// NOTE: This will only work in very specific cases; it is not a good general
//       purpose solution.
//
//******************************************************************************
void WIN32API ODIN_waveOutSetFixedBuffers();

#define SetFixedWaveBufferSize ODIN_waveOutSetFixedBuffers

//******************************************************************************
// ODIN_waveInSetFixedBuffers
//
// Tell WINMM to use DART buffers of the same size as the first buffer delivered
// by waveInAddBuffer
//
// NOTE: This will only work in very specific cases; it is not a good general
//       purpose solution.
//
//******************************************************************************
void WIN32API ODIN_waveInSetFixedBuffers();


/** Enter odin context with this thread. */
USHORT WIN32API ODIN_ThreadEnterOdinContext(void *pExceptionRegRec, BOOL fForceFSSwitch);
/** Leave odin context with this thread. */
void   WIN32API ODIN_ThreadLeaveOdinContext(void *pExceptionRegRec, USHORT selFSOld);

/** Leave odin context to call back into OS/2 code. */
USHORT WIN32API ODIN_ThreadLeaveOdinContextNested(void *pExceptionRegRec, BOOL fRemoveOdinExcpt);
/** Re-enter Odin context after being back in OS/2 code. */
void   WIN32API ODIN_ThreadEnterOdinContextNested(void *pExceptionRegRec, BOOL fRestoreOdinExcpt, USHORT selFSOld);

void   WIN32API ODIN_SetExceptionHandler(void *pExceptionRegRec);
void   WIN32API ODIN_UnsetExceptionHandler(void *pExceptionRegRec);

/* Turn on CD Polling (window with 2 second timer to check CD disk presence) */
void WIN32API CustEnableCDPolling();

void WIN32API SetFreeTypeIntegration(BOOL fEnabled);

// Special function to change dwords in the startup structure of a process
void WINAPI ODIN_SetProcessDword( DWORD dwProcessID, INT offset, DWORD value );

void WIN32API ODIN_SetPostscriptPassthrough(BOOL fEnable);

//PE headers of system dlls
#ifdef __cplusplus
extern "C" {
#endif
extern IMAGE_FILE_HEADER nt_ntdll_header;
extern IMAGE_FILE_HEADER nt_gdi32_header;
extern IMAGE_FILE_HEADER nt_kernel32_header;
extern IMAGE_FILE_HEADER nt_user32_header;
extern IMAGE_FILE_HEADER nt_advapi32_header;
extern IMAGE_FILE_HEADER nt_version_header;
extern IMAGE_FILE_HEADER nt_wsock32_header;
extern IMAGE_FILE_HEADER nt_ws2_32_header;
extern IMAGE_FILE_HEADER nt_winmm_header;
extern IMAGE_FILE_HEADER nt_ole32_header;
extern IMAGE_FILE_HEADER nt_comctl32_header;
extern IMAGE_FILE_HEADER nt_shell32_header;
extern IMAGE_FILE_HEADER nt_comdlg32_header;
extern IMAGE_FILE_HEADER nt_winspool_header;
extern IMAGE_FILE_HEADER nt_ddraw_header;
extern IMAGE_FILE_HEADER nt_oleaut32_header;
extern IMAGE_FILE_HEADER nt_msvfw32_header;
extern IMAGE_FILE_HEADER nt_imm32_header;
extern IMAGE_FILE_HEADER nt_mpr_header;
extern IMAGE_FILE_HEADER nt_iphlpapi_header;
extern IMAGE_FILE_HEADER nt_olepro32_header;
extern IMAGE_FILE_HEADER nt_msvcrt_header;
extern IMAGE_FILE_HEADER nt_lz32_header;
extern IMAGE_FILE_HEADER nt_oledlg_header;
extern IMAGE_FILE_HEADER nt_riched32_header;
extern IMAGE_FILE_HEADER nt_psapi_header;
extern IMAGE_FILE_HEADER nt_rpcrt4_header;
extern IMAGE_FILE_HEADER nt_shlwapi_header;
extern IMAGE_FILE_HEADER nt_shfolder_header;
extern IMAGE_FILE_HEADER nt_wininet_header;
extern IMAGE_FILE_HEADER nt_olepro32_header;
extern IMAGE_FILE_HEADER nt_avifil32_header;
extern IMAGE_FILE_HEADER nt_dinput_header;
extern IMAGE_FILE_HEADER nt_dsound_header;
extern IMAGE_FILE_HEADER nt_uxtheme_header;
extern IMAGE_FILE_HEADER nt_mciwave_header;
extern IMAGE_FILE_HEADER nt_urlmon_header;
extern IMAGE_FILE_HEADER nt_netapi32_header;
#ifdef __cplusplus
}
#endif

#endif  /*__CUSTOMBUILD_H__*/

