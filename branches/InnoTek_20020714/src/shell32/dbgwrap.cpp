#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winreg.h>
#include <winnt.h>
#include <wincon.h>
#include <winthunk.h>
#include <winnls.h>
#include <ntddk.h>
#include <heapstring.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

//#define NO_SHLWAPI_STREAM
#include "shlwapi.h"

#include "undocshell.h"
#include "shell32_main.h"
#include "shellapi.h"
#include "shresdef.h"

#define DBGWRAP_MODULE "SHELL32"
#include <dbgwrap.h>


DEBUGWRAP24(SHChangeNotifyRegister)
NODEF_DEBUGWRAP8(CheckEscapesA)
DEBUGWRAP4(SHChangeNotifyDeregister)
NODEF_DEBUGWRAP8(CheckEscapesW)
NODEF_DEBUGWRAP8(CommandLineToArgvW)
NODEF_DEBUGWRAP16(Control_FillCache_RunDLL)
DEBUGWRAP8(ILGetDisplayName)
DEBUGWRAP4(ILFindLastID)
DEBUGWRAP4(ILRemoveLastID)
DEBUGWRAP4(ILClone)
DEBUGWRAP4(ILCloneFirst)
DEBUGWRAP4(ILGlobalClone)
DEBUGWRAP8(ILIsEqual)
NODEF_DEBUGWRAP16(Control_RunDLL)
DEBUGWRAP12(ILIsParent)
DEBUGWRAP8(ILFindChild)
DEBUGWRAP8(ILCombine)
DEBUGWRAP8(ILLoadFromStream)
DEBUGWRAP8(ILSaveToStream)
DEBUGWRAP12(SHILCreateFromPathAW)
DEBUGWRAP4(PathIsRootAW)
DEBUGWRAP8(PathBuildRootAW)
DEBUGWRAP4(PathFindExtensionAW)
DEBUGWRAP4(PathAddBackslashAW)
DEBUGWRAP4(PathRemoveBlanksAW)
DEBUGWRAP4(PathFindFileNameAW)
DEBUGWRAP4(PathRemoveFileSpecAW)
DEBUGWRAP8(PathAppendAW)
DEBUGWRAP12(PathCombineAW)
DEBUGWRAP4(PathStripPathAW)
DEBUGWRAP4(PathIsUNCAW)
DEBUGWRAP4(PathIsRelativeAW)
DEBUGWRAP4(PathIsExeAW)
NODEF_DEBUGWRAP8(DoEnvironmentSubstA)
DEBUGWRAP4(PathFileExistsAW)
DEBUGWRAP8(PathMatchSpecAW)
DEBUGWRAP20(PathMakeUniqueNameAW)
DEBUGWRAP12(PathSetDlgItemPathAW)
DEBUGWRAP4(PathQualifyAW)
DEBUGWRAP4(PathStripToRootAW)
DEBUGWRAP12(PathResolveAW)
DEBUGWRAP4(PathGetArgsAW)
NODEF_DEBUGWRAP8(DoEnvironmentSubstAW)
DEBUGWRAP8_NORET(DragAcceptFiles)
DEBUGWRAP4(PathQuoteSpacesAW)
DEBUGWRAP4(PathUnquoteSpacesAW)
NODEF_DEBUGWRAP4(PathGetDriveNumberAW)
NODEF_DEBUGWRAP16(ParseFieldAW)
DEBUGWRAP12(RestartDialog)
DEBUGWRAP4(ExitWindowsDialog)
DEBUGWRAP24(RunFileDlg)
DEBUGWRAP16(PickIconDlg)
DEBUGWRAP28(GetFileNameFromBrowse)
NODEF_DEBUGWRAP4(DriveType)
NODEF_DEBUGWRAP4(InvalidateDriveType)
NODEF_DEBUGWRAP4(IsNetDrive)
DEBUGWRAP24(Shell_MergeMenus)
NODEF_DEBUGWRAP12(SHGetSetSettings)
DEBUGWRAP20(SHCreateDefClassObject)
DEBUGWRAP8(Shell_GetImageList)
NODEF_DEBUGWRAP12(Shell_GetCachedImageIndexAW)
NODEF_DEBUGWRAP12(Shell_GetCachedImageIndexW)
NODEF_DEBUGWRAP12(Shell_GetCachedImageIndexA)
DEBUGWRAP12(SHShellFolderView_Message)
DEBUGWRAP12(SHCreateStdEnumFmtEtc)
NODEF_DEBUGWRAP16(PathYetAnotherMakeUniqueNameAW)
DEBUGWRAP12(SHMapPIDLToSystemImageListIndex)
NODEF_DEBUGWRAP16(OleStrToStrN)
NODEF_DEBUGWRAP16(StrToOleStrN)
DEBUGWRAP4(DragFinish)
DEBUGWRAP16(DragQueryFileA)
DEBUGWRAP16(CIDLData_CreateFromIDArray)
DEBUGWRAP8(SHRegisterDragDrop)
DEBUGWRAP4(SHRevokeDragDrop)
DEBUGWRAP20(SHDoDragDrop)
NODEF_DEBUGWRAP12(SHCloneSpecialIDList)
DEBUGWRAP8(SHFindFiles)
DEBUGWRAP8(SHFindComputer)
DEBUGWRAP4(PathGetShortPathAW)
NODEF_DEBUGWRAP4(SHLogILFromFSIL)
DEBUGWRAP16(StrRetToStrNAW)
DEBUGWRAP12(SHWaitForFileToOpen)
NODEF_DEBUGWRAP12(SHGetRealIDL)
NODEF_DEBUGWRAP8(SetAppStartingCursor)
NODEF_DEBUGWRAP4(SHRestricted)
NODEF_DEBUGWRAP16(DragQueryFileAorW)
DEBUGWRAP20(SHCoCreateInstance)
NODEF_DEBUGWRAP4(SignalFileOpen)
DEBUGWRAP4(FileMenu_DeleteAllItems)
DEBUGWRAP8(FileMenu_DrawItem)
DEBUGWRAP8(FileMenu_FindSubMenuByPidl)
DEBUGWRAP12(FileMenu_GetLastSelectedItemPidls)
DEBUGWRAP8(FileMenu_HandleMenuChar)
DEBUGWRAP4(FileMenu_InitMenuPopup)
DEBUGWRAP24(FileMenu_InsertUsingPidl)
DEBUGWRAP4(FileMenu_Invalidate)
DEBUGWRAP8(FileMenu_MeasureItem)
DEBUGWRAP20(FileMenu_ReplaceUsingPidl)
DEBUGWRAP20(FileMenu_Create)
NODEF_DEBUGWRAP24(FileMenu_AppendItemAW)
DEBUGWRAP24(FileMenu_TrackPopupMenuEx)
DEBUGWRAP8(FileMenu_DeleteItemByCmd)
DEBUGWRAP4(FileMenu_Destroy)
NODEF_DEBUGWRAP4(IsLFNDriveA)
DEBUGWRAP0_NORET(FileMenu_AbortInitMenu)
NODEF_DEBUGWRAP0(SHFlushClipboard)
NODEF_DEBUGWRAP20(RunDLL_CallEntry16)
DEBUGWRAP0_NORET(SHFreeUnusedLibraries)
DEBUGWRAP12(FileMenu_AppendFilesForPidl)
DEBUGWRAP28(FileMenu_AddFilesForPidl)
DEBUGWRAP12(SHOutOfMemoryMessageBox)
NODEF_DEBUGWRAP16(SHWinHelp)
DEBUGWRAP12(SHELL32_DllGetClassObject)
DEBUGWRAP12(DAD_AutoScroll)
DEBUGWRAP4(DAD_DragEnter)
DEBUGWRAP12(DAD_DragEnterEx)
DEBUGWRAP0(DAD_DragLeave)
DEBUGWRAP16(DragQueryFileW)
DEBUGWRAP8(DAD_DragMove)
DEBUGWRAP8(DragQueryPoint)
DEBUGWRAP8(DAD_SetDragImage)
DEBUGWRAP4(DAD_ShowDragImage)
NODEF_DEBUGWRAP4(DuplicateIcon)
NODEF_DEBUGWRAP4(Desktop_UpdateBriefcaseOnEvent)
DEBUGWRAP8(FileMenu_DeleteItemByIndex)
NODEF_DEBUGWRAP8(FileMenu_DeleteItemByFirstID)
DEBUGWRAP4(FileMenu_DeleteSeparator)
DEBUGWRAP12(FileMenu_EnableItemByCmd)
DEBUGWRAP8(FileMenu_GetItemExtent)
DEBUGWRAP8(PathFindOnPathAW)
NODEF_DEBUGWRAP0(RLBuildListOfPaths)
NODEF_DEBUGWRAP8(SHCLSIDFromStringAW)
DEBUGWRAP12(ExtractAssociatedIconA)
NODEF_DEBUGWRAP16(SHFind_InitMenuPopup)
NODEF_DEBUGWRAP12(ExtractAssociatedIconExA)
DEBUGWRAP4(SHLoadOLE)
DEBUGWRAP4(ILGetSize)
DEBUGWRAP4(ILGetNext)
NODEF_DEBUGWRAP12(ILAppend)
DEBUGWRAP4(ILFree)
DEBUGWRAP4(ILGlobalFree)
DEBUGWRAP4(ILCreateFromPathAW)
DEBUGWRAP12(PathGetExtensionAW)
DEBUGWRAP4(PathIsDirectoryAW)
NODEF_DEBUGWRAP8(SHRunControlPanel)
DEBUGWRAP4(SHSimpleIDListFromPathAW)
NODEF_DEBUGWRAP8(StrToOleStr)
NODEF_DEBUGWRAP4(Win32DeleteFile)
NODEF_DEBUGWRAP8(SHCreateDirectory)
NODEF_DEBUGWRAP12(SHAddFromPropSheetExtArray)
NODEF_DEBUGWRAP12(SHCreatePropSheetExtArray)
NODEF_DEBUGWRAP4(SHDestroyPropSheetExtArray)
NODEF_DEBUGWRAP16(SHReplaceFromPropSheetExtArray)
DEBUGWRAP8(PathCleanupSpecAW)
NODEF_DEBUGWRAP12(SHValidateUNC)
DEBUGWRAP8(SHCreateShellFolderViewEx)
NODEF_DEBUGWRAP16(SHGetSpecialFolderPathAW)
NODEF_DEBUGWRAP4(SHSetInstanceExplorer)
DEBUGWRAP8(RegisterShellHook)
DEBUGWRAP20(ArrangeWindows)
DEBUGWRAP8(SHHandleDiskFull)
NODEF_DEBUGWRAP12(ExtractAssociatedIconExW)
DEBUGWRAP12(ExtractAssociatedIconW)
DEBUGWRAP12(ExtractIconA)
DEBUGWRAP20(ExtractIconExA)
DEBUGWRAP20(ExtractIconExW)
NODEF_DEBUGWRAP4(ExtractIconResInfoA)
NODEF_DEBUGWRAP4(ExtractIconResInfoW)
DEBUGWRAP12(ExtractIconW)
DEBUGWRAP4(SHFree)
DEBUGWRAP4(SHAlloc)
NODEF_DEBUGWRAP4(SHGlobalDefect)
NODEF_DEBUGWRAP0(SHAbortInvokeCommand)
DEBUGWRAP12(FindExecutableA)
DEBUGWRAP12(FindExecutableW)
NODEF_DEBUGWRAP4(FreeIconList)
NODEF_DEBUGWRAP4(InternalExtractIconListA)
NODEF_DEBUGWRAP4(InternalExtractIconListW)
NODEF_DEBUGWRAP24(RealShellExecuteA)
NODEF_DEBUGWRAP4(RealShellExecuteExA)
NODEF_DEBUGWRAP4(RealShellExecuteExW)
NODEF_DEBUGWRAP24(RealShellExecuteW)
NODEF_DEBUGWRAP8(RegenerateUserEnvironment)
DEBUGWRAP8(SHAddToRecentDocs)
NODEF_DEBUGWRAP8(SHAppBarMessage)
DEBUGWRAP4(SHBrowseForFolderA)
DEBUGWRAP4(SHBrowseForFolderW)
NODEF_DEBUGWRAP16(SHChangeNotifyAW)
NODEF_DEBUGWRAP12(SHEmptyRecycleBinA)
NODEF_DEBUGWRAP12(SHEmptyRecycleBinW)
NODEF_DEBUGWRAP4(SHFileOperationAW)
DEBUGWRAP4(SHFileOperationA)
DEBUGWRAP4(SHFileOperationW)
NODEF_DEBUGWRAP16(SHFormatDrive)
NODEF_DEBUGWRAP4(SHFreeNameMappings)
DEBUGWRAP20(SHGetDataFromIDListA)
DEBUGWRAP20(SHGetDataFromIDListW)
DEBUGWRAP4(PathParseIconLocationAW)
DEBUGWRAP4(PathRemoveExtensionAW)
DEBUGWRAP4(PathRemoveArgsAW)
DEBUGWRAP4(SHGetDesktopFolder)
DEBUGWRAP20(SHGetFileInfoA)
DEBUGWRAP20(SHGetFileInfoW)
NODEF_DEBUGWRAP4(SHGetInstanceExplorer)
DEBUGWRAP4(SHGetMalloc)
NODEF_DEBUGWRAP20(SHGetNewLinkInfo)
DEBUGWRAP8(SHGetPathFromIDListA)
DEBUGWRAP8(SHGetPathFromIDListW)
DEBUGWRAP12(SHGetSpecialFolderLocation)
NODEF_DEBUGWRAP16(SHHelpShortcuts_RunDLL)
NODEF_DEBUGWRAP16(SHHelpShortcuts_RunDLLA)
NODEF_DEBUGWRAP16(SHHelpShortcuts_RunDLLW)
NODEF_DEBUGWRAP4(SHLoadInProc)
NODEF_DEBUGWRAP8(SHQueryRecycleBinA)
NODEF_DEBUGWRAP8(SHQueryRecycleBinW)
NODEF_DEBUGWRAP0(SHUpdateRecycleBinIcon)
NODEF_DEBUGWRAP4(SheChangeDirW)
NODEF_DEBUGWRAP8(SheConvertPathW)
NODEF_DEBUGWRAP4(SheSetCurDrive)
NODEF_DEBUGWRAP8(SheGetDirW)
NODEF_DEBUGWRAP4(SheRemoveQuotesA)
NODEF_DEBUGWRAP4(SheRemoveQuotesW)
NODEF_DEBUGWRAP8(SheShortenPathA)
NODEF_DEBUGWRAP8(SheShortenPathW)
DEBUGWRAP16(ShellAboutA)
DEBUGWRAP16(ShellAboutW)
DEBUGWRAP24(ShellExecuteA)
DEBUGWRAP4(ShellExecuteExA)
DEBUGWRAP4(ShellExecuteExW)
DEBUGWRAP24(ShellExecuteW)
DEBUGWRAP8(Shell_NotifyIconA)
DEBUGWRAP8(Shell_NotifyIconW)
NODEF_DEBUGWRAP4(SHRegCloseKey)
NODEF_DEBUGWRAP12(SHRegOpenKeyA)
NODEF_DEBUGWRAP12(SHRegOpenKeyW)
NODEF_DEBUGWRAP16(SHRegQueryValueA)
NODEF_DEBUGWRAP24(SHRegQueryValueExA)
NODEF_DEBUGWRAP24(SHRegQueryValueExW)
NODEF_DEBUGWRAP16(SHRegQueryValueW)
NODEF_DEBUGWRAP8(SHRegDeleteKeyW)
NODEF_DEBUGWRAP8(SHRegDeleteKeyA)
DEBUGWRAP12(SHAllocShared)
DEBUGWRAP8(SHInitRestricted)
DEBUGWRAP8(SHLockShared)
DEBUGWRAP4(SHUnlockShared)
DEBUGWRAP8(SHFreeShared)
NODEF_DEBUGWRAP24(NTSHChangeNotifyRegister)
NODEF_DEBUGWRAP4(NTSHChangeNotifyDeregister)
DEBUGWRAP8(PathIsSameRootAW)
NODEF_DEBUGWRAP8(ReadCabinetState)
NODEF_DEBUGWRAP4(WriteCabinetState)
DEBUGWRAP16(PathProcessCommandAW)
DEBUGWRAP4(FileIconInit)
NODEF_DEBUGWRAP0(IsUserAdmin)
DEBUGWRAP16(SHGetSpecialFolderPathA)
DEBUGWRAP8(SHGetSettings)
NODEF_DEBUGWRAP20(SHGetFolderPathA)
NODEF_DEBUGWRAP20(SHGetFolderPathW)
NODEF_DEBUGWRAP20(SHGetFolderLocation)
NODEF_DEBUGWRAP4(SHELL32_DllGetVersion)
DEBUGWRAP0(SHELL32_DllCanUnloadNow)
NODEF_DEBUGWRAP8(SHELL32_654)

//    ShellMessageBoxW                  = _ShellMessageBoxW                     @182 ;cdecl
//    ShellMessageBoxA                  = _ShellMessageBoxA                     @183 ;cdecl

