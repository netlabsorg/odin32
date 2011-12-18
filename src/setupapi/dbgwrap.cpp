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
#include <commctrl.h>
#include <setupapi.h>

#include <heapstring.h>

#define DBGWRAP_MODULE "SETUPAPI"
#include <dbgwrap.h>


DEBUGWRAP4(SetupCloseFileQueue)
DEBUGWRAP4(SetupCloseInfFile)
DEBUGWRAP16(SetupCommitFileQueueA)
DEBUGWRAP16(SetupCommitFileQueueW)
DEBUGWRAP16(SetupDefaultQueueCallbackA)
DEBUGWRAP16(SetupDefaultQueueCallbackW)
NODEF_DEBUGWRAP4(SetupDiDestroyDeviceInfoList)
NODEF_DEBUGWRAP12(SetupDiEnumDeviceInfo)
NODEF_DEBUGWRAP20(SetupDiEnumDeviceInterfaces)
DEBUGWRAP16(SetupDiGetClassDevsA)
DEBUGWRAP16(SetupDiGetClassDevsW)
NODEF_DEBUGWRAP28(SetupDiGetDeviceRegistryPropertyA)
NODEF_DEBUGWRAP24(SetupDiGetDeviceInterfaceDetailA)
NODEF_DEBUGWRAP24(SetupDiGetDeviceInterfaceDetailW)
DEBUGWRAP16(SetupFindFirstLineA)
DEBUGWRAP16(SetupFindFirstLineW)
DEBUGWRAP8(SetupFindNextLine)
DEBUGWRAP12(SetupFindNextMatchLineA)
DEBUGWRAP12(SetupFindNextMatchLineW)
DEBUGWRAP20(SetupGetBinaryField)
DEBUGWRAP4(SetupGetFieldCount)
DEBUGWRAP12(SetupGetFileQueueCount)
DEBUGWRAP8(SetupGetFileQueueFlags)
DEBUGWRAP12(SetupGetIntField)
DEBUGWRAP16(SetupGetLineByIndexA)
DEBUGWRAP16(SetupGetLineByIndexW)
DEBUGWRAP8(SetupGetLineCountA)
DEBUGWRAP8(SetupGetLineCountW)
DEBUGWRAP28(SetupGetLineTextA)
DEBUGWRAP28(SetupGetLineTextW)
DEBUGWRAP20(SetupGetMultiSzFieldA)
DEBUGWRAP20(SetupGetMultiSzFieldW)
DEBUGWRAP20(SetupGetStringFieldA)
DEBUGWRAP20(SetupGetStringFieldW)
DEBUGWRAP4(SetupInitDefaultQueueCallback)
DEBUGWRAP20(SetupInitDefaultQueueCallbackEx)
DEBUGWRAP24(SetupInstallFilesFromInfSectionA)
DEBUGWRAP24(SetupInstallFilesFromInfSectionW)
DEBUGWRAP44(SetupInstallFromInfSectionA)
DEBUGWRAP44(SetupInstallFromInfSectionW)
NODEF_DEBUGWRAP16(SetupIterateCabinetA)
NODEF_DEBUGWRAP16(SetupIterateCabinetW)
DEBUGWRAP12(SetupOpenAppendInfFileA)
DEBUGWRAP12(SetupOpenAppendInfFileW)
DEBUGWRAP0(SetupOpenFileQueue)
DEBUGWRAP16(SetupOpenInfFileA)
DEBUGWRAP16(SetupOpenInfFileW)
DEBUGWRAP36(SetupQueueCopyA)
DEBUGWRAP4(SetupQueueCopyIndirectA)
DEBUGWRAP4(SetupQueueCopyIndirectW)
DEBUGWRAP24(SetupQueueCopySectionA)
DEBUGWRAP24(SetupQueueCopySectionW)
DEBUGWRAP36(SetupQueueCopyW)
DEBUGWRAP24(SetupQueueDefaultCopyA)
DEBUGWRAP24(SetupQueueDefaultCopyW)
DEBUGWRAP12(SetupQueueDeleteA)
DEBUGWRAP16(SetupQueueDeleteSectionA)
DEBUGWRAP16(SetupQueueDeleteSectionW)
DEBUGWRAP12(SetupQueueDeleteW)
DEBUGWRAP20(SetupQueueRenameA)
DEBUGWRAP16(SetupQueueRenameSectionA)
DEBUGWRAP16(SetupQueueRenameSectionW)
DEBUGWRAP20(SetupQueueRenameW)
DEBUGWRAP24(SetupScanFileQueueA)
DEBUGWRAP24(SetupScanFileQueueW)
DEBUGWRAP12(SetupSetDirectoryIdA)
DEBUGWRAP12(SetupSetDirectoryIdW)
DEBUGWRAP4(SetupTermDefaultQueueCallback)


NODEF_DEBUGWRAP8(CM_Connect_MachineW)
NODEF_DEBUGWRAP4(CM_Disconnect_Machine)
NODEF_DEBUGWRAP24(SetupDiBuildClassInfoListExW)
NODEF_DEBUGWRAP24(SetupDiClassGuidsFromNameExW)
NODEF_DEBUGWRAP24(SetupDiClassNameFromGuidExW)
NODEF_DEBUGWRAP16(SetupDiCreateDeviceInfoListExW)
DEBUGWRAP12(SetupSetFileQueueFlags)
NODEF_DEBUGWRAP8(SetupDiGetDeviceInfoListDetailW)
NODEF_DEBUGWRAP28(SetupDiGetClassDevsExW)
NODEF_DEBUGWRAP20(SetupDiOpenClassRegKeyExW)
NODEF_DEBUGWRAP24(SetupDiGetClassDescriptionExW)
