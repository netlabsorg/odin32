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

#define DBG_LOCALLOG    DBG_trace
#include <dbglocal.h>

#define DBGWRAP_MODULE "KERNEL32"
#include <dbgwrap.h>


BOOL WINAPI TryEnterCriticalSection( CRITICAL_SECTION *crit );
void WINAPI ReinitializeCriticalSection( CRITICAL_SECTION *crit );
void WINAPI UninitializeCriticalSection( CRITICAL_SECTION *crit );

BOOL WINAPI CloseProfileUserMapping(void);
BOOL WINAPI OpenProfileUserMapping(void);

LPVOID WIN32API CreateFiber( DWORD dwStackSize,
                             LPVOID lpStartAddress,
                             LPVOID lpParameter);

VOID WIN32API DeleteFiber(LPVOID lpFiber);
HANDLE WIN32API BeginUpdateResourceA( LPCSTR pFileName,
                                       BOOL bDeleteExistingResources);
HANDLE WIN32API BeginUpdateResourceW( LPCWSTR pFileName,
                                       BOOL bDeleteExistingResources);

BOOL WIN32API EndUpdateResourceA(HANDLE hUpdate,BOOL fDiscard);
BOOL WIN32API EndUpdateResourceW(HANDLE hUpdate,BOOL fDiscard);
DWORD WIN32API CmdBatNotification(DWORD x1);


DWORD WIN32API ThunkConnect32(LPVOID,  LPSTR thunkfun16,
                              LPSTR module16, LPSTR module32, HMODULE hmod32,
                              DWORD dllinitarg1 );

BOOL WIN32API UpdateResourceA(HANDLE  hUpdateFile,
                                 LPCTSTR lpszType,
                                 LPCTSTR lpszName,
                                 WORD    IDLanguage,
                                 LPVOID  lpvData,
                                 DWORD   cbData);
BOOL WIN32API UpdateResourceW(HANDLE  hUpdateFile,
                              LPCWSTR lpszType,
                              LPCWSTR lpszName,
                              WORD    IDLanguage,
                              LPVOID  lpvData,
                              DWORD   cbData);
DWORD WIN32API GetVDMCurrentDirectories(DWORD x1);
BOOL WIN32API GetSystemTimeAdjustment(PDWORD lpTimeAdjustment,
                                         PDWORD lpTimeIncrement,
                                         PBOOL  lpTimeAdjustmentDisabled);
BOOL WIN32API SetSystemTimeAdjustment(DWORD dwTimeAdjustment,
                                         BOOL  bTimeAdjustmentDisabled);

BOOL WIN32API SystemTimeToTzSpecificLocalTime(LPTIME_ZONE_INFORMATION arg1,
                                              LPSYSTEMTIME arg2,
                                              LPSYSTEMTIME arg3);
FARPROC WIN32API GetProcAddress16(HMODULE hModule, LPCSTR lpszProc);


DEBUGWRAP0(AreFileApisANSI);
DEBUGWRAP8(Beep);
DEBUGWRAP28(BackupRead)
DEBUGWRAP24(BackupSeek);
DEBUGWRAP28(BackupWrite);
DEBUGWRAP4(CancelWaitableTimer);
DEBUGWRAP4(CmdBatNotification);
DEBUGWRAP4(ConvertToGlobalHandle);
DEBUGWRAP12(CreateFiber);
DEBUGWRAP16(CreateIoCompletionPort);
DEBUGWRAP16(CreateTapePartition);
DEBUGWRAP12(CreateWaitableTimerA);
DEBUGWRAP12(CreateWaitableTimerW);
DEBUGWRAP12(DefineDosDeviceA);
DEBUGWRAP12(DefineDosDeviceW);
DEBUGWRAP4(DeleteFiber);
DEBUGWRAP12(EraseTape);
DEBUGWRAP20(GetQueuedCompletionStatus);
DEBUGWRAP16(GetTapeParameters);
DEBUGWRAP20(GetTapePosition);
DEBUGWRAP4(GetTapeStatus);
//DEBUGWRAP12(InterlockedCompareExchange);
//DEBUGWRAP4(InterlockedDecrement);
//DEBUGWRAP8(InterlockedExchange);
//DEBUGWRAP8(InterlockedExchangeAdd);
//DEBUGWRAP4(InterlockedIncrement);

DEBUGWRAP_LVL2_12(MulDiv);
DEBUGWRAP16(PostQueuedCompletionStatus);
DEBUGWRAP12(PrepareTape);
DEBUGWRAP12(QueryDosDeviceA);
DEBUGWRAP12(QueryDosDeviceW);
DEBUGWRAP12(SetTapeParameters);
DEBUGWRAP24(SetTapePosition);
DEBUGWRAP16(WriteTapemark);

DEBUGWRAP4(SetErrorMode);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_registry

DEBUGWRAP4(RegCloseKey);
DEBUGWRAP12(RegConnectRegistryA);
DEBUGWRAP12(RegConnectRegistryW);
DEBUGWRAP12(RegCreateKeyA);
DEBUGWRAP36(RegCreateKeyExA);
DEBUGWRAP36(RegCreateKeyExW);
DEBUGWRAP12(RegCreateKeyW);
DEBUGWRAP8(RegDeleteKeyA);
DEBUGWRAP8(RegDeleteKeyW);
DEBUGWRAP8(RegDeleteValueA);
DEBUGWRAP8(RegDeleteValueW);
DEBUGWRAP16(RegEnumKeyA);
DEBUGWRAP32(RegEnumKeyExA);
DEBUGWRAP32(RegEnumKeyExW);
DEBUGWRAP16(RegEnumKeyW);
DEBUGWRAP32(RegEnumValueA);
DEBUGWRAP32(RegEnumValueW);
DEBUGWRAP4(RegFlushKey);
DEBUGWRAP16(RegGetKeySecurity);
DEBUGWRAP12(RegLoadKeyA);
DEBUGWRAP12(RegLoadKeyW);
DEBUGWRAP20(RegNotifyChangeKeyValue);
DEBUGWRAP12(RegOpenKeyA);
DEBUGWRAP20(RegOpenKeyExA);
DEBUGWRAP20(RegOpenKeyExW);
DEBUGWRAP12(RegOpenKeyW);
DEBUGWRAP48(RegQueryInfoKeyA);
DEBUGWRAP48(RegQueryInfoKeyW);
DEBUGWRAP20(RegQueryMultipleValuesA);
DEBUGWRAP20(RegQueryMultipleValuesW);
DEBUGWRAP16(RegQueryValueA);
DEBUGWRAP24(RegQueryValueExA);
DEBUGWRAP24(RegQueryValueExW);
DEBUGWRAP16(RegQueryValueW);
DEBUGWRAP16(RegReplaceKeyA);
DEBUGWRAP16(RegReplaceKeyW);
DEBUGWRAP12(RegRestoreKeyA);
DEBUGWRAP12(RegRestoreKeyW);
DEBUGWRAP12(RegSaveKeyA);
DEBUGWRAP12(RegSaveKeyW);
DEBUGWRAP12(RegSetKeySecurity);
DEBUGWRAP20(RegSetValueA);
DEBUGWRAP24(RegSetValueExA);
DEBUGWRAP24(RegSetValueExW);
DEBUGWRAP20(RegSetValueW);
DEBUGWRAP8(RegUnLoadKeyA);
DEBUGWRAP8(RegUnLoadKeyW);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_thunk

DEBUGWRAP4(QT_Thunk);
DEBUGWRAP4(FT_Exit0);
DEBUGWRAP4(FT_Exit12);
DEBUGWRAP4(FT_Exit16);
DEBUGWRAP4(FT_Exit20);
DEBUGWRAP4(FT_Exit24);
DEBUGWRAP4(FT_Exit28);
DEBUGWRAP4(FT_Exit32);
DEBUGWRAP4(FT_Exit36);
DEBUGWRAP4(FT_Exit4);
DEBUGWRAP4(FT_Exit40);
DEBUGWRAP4(FT_Exit44);
DEBUGWRAP4(FT_Exit48);
DEBUGWRAP4(FT_Exit52);
DEBUGWRAP4(FT_Exit56);
DEBUGWRAP4(FT_Exit8);
DEBUGWRAP4(FT_Prolog);
DEBUGWRAP4(FT_Thunk);
DEBUGWRAP4(SMapLS);
DEBUGWRAP4(SMapLS_IP_EBP_12);
DEBUGWRAP4(SMapLS_IP_EBP_16);
DEBUGWRAP4(SMapLS_IP_EBP_20);
DEBUGWRAP4(SMapLS_IP_EBP_24);
DEBUGWRAP4(SMapLS_IP_EBP_28);
DEBUGWRAP4(SMapLS_IP_EBP_32);
DEBUGWRAP4(SMapLS_IP_EBP_36);
DEBUGWRAP4(SMapLS_IP_EBP_40);
DEBUGWRAP4(SMapLS_IP_EBP_8);
DEBUGWRAP4(SUnMapLS);
DEBUGWRAP4(SUnMapLS_IP_EBP_12);
DEBUGWRAP4(SUnMapLS_IP_EBP_16);
DEBUGWRAP4(SUnMapLS_IP_EBP_20);
DEBUGWRAP4(SUnMapLS_IP_EBP_24);
DEBUGWRAP4(SUnMapLS_IP_EBP_28);
DEBUGWRAP4(SUnMapLS_IP_EBP_32);
DEBUGWRAP4(SUnMapLS_IP_EBP_36);
DEBUGWRAP4(SUnMapLS_IP_EBP_40);
DEBUGWRAP4(SUnMapLS_IP_EBP_8);
DEBUGWRAP4(MapLS);
DEBUGWRAP4(MapSLFix);
DEBUGWRAP4(MapSL);
DEBUGWRAP4(ReleaseThunkLock);
DEBUGWRAP4(RestoreThunkLock);
DEBUGWRAP0(_ConfirmWin16Lock);
DEBUGWRAP4(K32Thk1632Epilog);
DEBUGWRAP4(K32Thk1632Prolog);
DEBUGWRAP4(UnMapLS);
DEBUGWRAP12(UnMapSLFixArray);
DEBUGWRAP24(ThunkConnect32);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_directory

DEBUGWRAP24(SearchPathA);
DEBUGWRAP24(SearchPathW);
DEBUGWRAP8(CreateDirectoryA);
DEBUGWRAP12(CreateDirectoryExA);
DEBUGWRAP12(CreateDirectoryExW);
DEBUGWRAP8(CreateDirectoryW);
DEBUGWRAP8(GetCurrentDirectoryA);
DEBUGWRAP8(GetCurrentDirectoryW);
DEBUGWRAP8(GetSystemDirectoryA);
DEBUGWRAP8(GetSystemDirectoryW);
DEBUGWRAP8(GetWindowsDirectoryA);
DEBUGWRAP8(GetWindowsDirectoryW);
DEBUGWRAP4(RemoveDirectoryA);
DEBUGWRAP4(RemoveDirectoryW);
DEBUGWRAP4(SetCurrentDirectoryA);
DEBUGWRAP4(SetCurrentDirectoryW);
DEBUGWRAP8(GetTempPathA);
DEBUGWRAP8(GetTempPathW);
DEBUGWRAP16(GetTempFileNameA);
DEBUGWRAP16(GetTempFileNameW);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG  DBG_comm

DEBUGWRAP12(CommConfigDialogA);
DEBUGWRAP12(CommConfigDialogW);
DEBUGWRAP8(BuildCommDCBA);
DEBUGWRAP12(BuildCommDCBAndTimeoutsA);
DEBUGWRAP12(BuildCommDCBAndTimeoutsW);
DEBUGWRAP8(BuildCommDCBW);
DEBUGWRAP4(ClearCommBreak);
DEBUGWRAP12(ClearCommError);
DEBUGWRAP8(EscapeCommFunction);
DEBUGWRAP12(GetCommConfig);
DEBUGWRAP8(GetCommMask);
DEBUGWRAP8(GetCommModemStatus);
DEBUGWRAP8(GetCommProperties);
DEBUGWRAP8(GetCommState);
DEBUGWRAP8(GetCommTimeouts);
DEBUGWRAP12(GetDefaultCommConfigA);
DEBUGWRAP12(GetDefaultCommConfigW);
DEBUGWRAP8(PurgeComm);
DEBUGWRAP4(SetCommBreak);
DEBUGWRAP12(SetCommConfig);
DEBUGWRAP8(SetCommMask);
DEBUGWRAP8(SetCommState);
DEBUGWRAP8(SetCommTimeouts);
DEBUGWRAP12(SetDefaultCommConfigA);
DEBUGWRAP12(SetDefaultCommConfigW);
DEBUGWRAP12(SetupComm);
DEBUGWRAP8(TransmitCommChar);
DEBUGWRAP12(WaitCommEvent);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_heapstring

DEBUGWRAP_LVL2_12(HEAP_xalloc);
DEBUGWRAP_LVL2_16(HEAP_xrealloc);
DEBUGWRAP_LVL2_4(HEAP_malloc);
DEBUGWRAP_LVL2_8(HEAP_realloc);
DEBUGWRAP_LVL2_4(HEAP_size);
DEBUGWRAP_LVL2_4(HEAP_free);
DEBUGWRAP_LVL2_12(HEAP_strdupWtoA);
DEBUGWRAP_LVL2_12(HEAP_strdupAtoW);
DEBUGWRAP_LVL2_12(HEAP_strdupA);
DEBUGWRAP_LVL2_12(HEAP_strdupW);
DEBUGWRAP_LVL2_8(lstrcatA);
DEBUGWRAP_LVL2_8(lstrcatW);
DEBUGWRAP_LVL2_8(lstrcmpA);
DEBUGWRAP_LVL2_8(lstrcmpW);
DEBUGWRAP_LVL2_8(lstrcmpiA);
DEBUGWRAP_LVL2_8(lstrcmpiW);
DEBUGWRAP_LVL2_8(lstrcpyA);
DEBUGWRAP_LVL2_8(lstrcpyW);
DEBUGWRAP_LVL2_12(lstrcpynA);
DEBUGWRAP_LVL2_12(lstrcpynW);
DEBUGWRAP_LVL2_4(lstrlenA);
DEBUGWRAP_LVL2_4(lstrlenW);
DEBUGWRAP_LVL2_12(lstrcpynAtoW);
DEBUGWRAP_LVL2_12(lstrcpynWtoA);
DEBUGWRAP_LVL2_8(lstrcpyAtoW);
DEBUGWRAP_LVL2_8(lstrcpyWtoA);
DEBUGWRAP_LVL2_12(lstrncmpA);
DEBUGWRAP_LVL2_12(lstrncmpW);
DEBUGWRAP_LVL2_12(lstrncmpiA);
DEBUGWRAP_LVL2_12(lstrncmpiW);
DEBUGWRAP_LVL2_8(lstrlenAtoW);
DEBUGWRAP_LVL2_8(lstrlenWtoA);
DEBUGWRAP_LVL2_8(lstrtrunc);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_console

DEBUGWRAP20(WriteConsoleA);
DEBUGWRAP16(WriteConsoleInputA);
DEBUGWRAP16(WriteConsoleInputW);
DEBUGWRAP20(WriteConsoleOutputA);
DEBUGWRAP20(WriteConsoleOutputAttribute);
DEBUGWRAP20(WriteConsoleOutputCharacterA);
DEBUGWRAP20(WriteConsoleOutputCharacterW);
DEBUGWRAP20(WriteConsoleOutputW);
DEBUGWRAP20(WriteConsoleW);
DEBUGWRAP0(AllocConsole);
DEBUGWRAP20(CreateConsoleScreenBuffer);
DEBUGWRAP20(FillConsoleOutputAttribute);
DEBUGWRAP20(FillConsoleOutputCharacterA);
DEBUGWRAP20(FillConsoleOutputCharacterW);
DEBUGWRAP4(FlushConsoleInputBuffer);
DEBUGWRAP0(FreeConsole);
DEBUGWRAP8(GenerateConsoleCtrlEvent);
DEBUGWRAP0(GetConsoleCP);
DEBUGWRAP8(GetConsoleCursorInfo);
DEBUGWRAP8(GetConsoleMode);
DEBUGWRAP0(GetConsoleOutputCP);
DEBUGWRAP8(GetConsoleScreenBufferInfo);
DEBUGWRAP8(GetConsoleTitleA);
DEBUGWRAP8(GetConsoleTitleW);
DEBUGWRAP4(GetLargestConsoleWindowSize);
DEBUGWRAP8(GetNumberOfConsoleInputEvents);
DEBUGWRAP4(GetNumberOfConsoleMouseButtons);
DEBUGWRAP16(PeekConsoleInputA);
DEBUGWRAP16(PeekConsoleInputW);
DEBUGWRAP20(ReadConsoleA);
DEBUGWRAP16(ReadConsoleInputA);
DEBUGWRAP16(ReadConsoleInputW);
DEBUGWRAP20(ReadConsoleOutputA);
DEBUGWRAP20(ReadConsoleOutputAttribute);
DEBUGWRAP20(ReadConsoleOutputCharacterA);
DEBUGWRAP20(ReadConsoleOutputCharacterW);
DEBUGWRAP20(ReadConsoleOutputW);
DEBUGWRAP20(ReadConsoleW);
DEBUGWRAP20(ScrollConsoleScreenBufferA);
DEBUGWRAP20(ScrollConsoleScreenBufferW);
DEBUGWRAP4(SetConsoleCP);
DEBUGWRAP8(SetConsoleCtrlHandler);
DEBUGWRAP8(SetConsoleCursorInfo);
DEBUGWRAP8(SetConsoleCursorPosition);
DEBUGWRAP8(SetConsoleMode);
DEBUGWRAP4(SetConsoleOutputCP);
DEBUGWRAP8(SetConsoleScreenBufferSize);
DEBUGWRAP8(SetConsoleTextAttribute);
DEBUGWRAP4(SetConsoleTitleA);
DEBUGWRAP4(SetConsoleTitleW);
DEBUGWRAP12(SetConsoleWindowInfo);
DEBUGWRAP4(SetConsoleActiveScreenBuffer);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_profile

DEBUGWRAP12(WritePrivateProfileSectionA);
DEBUGWRAP12(WritePrivateProfileSectionW);
DEBUGWRAP16(WritePrivateProfileStringA);
DEBUGWRAP16(WritePrivateProfileStringW);
DEBUGWRAP20(WritePrivateProfileStructA);
DEBUGWRAP20(WritePrivateProfileStructW);
DEBUGWRAP0(CloseProfileUserMapping);
DEBUGWRAP16(GetPrivateProfileIntA);
DEBUGWRAP16(GetPrivateProfileIntW);
DEBUGWRAP16(GetPrivateProfileSectionA);
DEBUGWRAP12(GetPrivateProfileSectionNamesA);
DEBUGWRAP12(GetPrivateProfileSectionNamesW);
DEBUGWRAP16(GetPrivateProfileSectionW);
DEBUGWRAP24(GetPrivateProfileStringA);
DEBUGWRAP24(GetPrivateProfileStringW);
DEBUGWRAP20(GetPrivateProfileStructA);
DEBUGWRAP20(GetPrivateProfileStructW);
DEBUGWRAP12(GetProfileIntA);
DEBUGWRAP12(GetProfileIntW);
DEBUGWRAP12(GetProfileSectionA);
DEBUGWRAP12(GetProfileSectionW);
DEBUGWRAP20(GetProfileStringA);
DEBUGWRAP20(GetProfileStringW);
DEBUGWRAP0(OpenProfileUserMapping);
DEBUGWRAP8(WriteProfileSectionA);
DEBUGWRAP8(WriteProfileSectionW);
DEBUGWRAP12(WriteProfileStringA);
DEBUGWRAP12(WriteProfileStringW);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_atom

DEBUGWRAP4(AddAtomA);
DEBUGWRAP4(AddAtomW);
DEBUGWRAP4(DeleteAtom);
DEBUGWRAP4(FindAtomA);
DEBUGWRAP4(FindAtomW);
DEBUGWRAP12(GetAtomNameA);
DEBUGWRAP12(GetAtomNameW);
DEBUGWRAP4(GlobalAddAtomA);
DEBUGWRAP4(GlobalAddAtomW);
DEBUGWRAP4(GlobalDeleteAtom);
//DEBUGWRAP4(GlobalFindAtomA);
//DEBUGWRAP4(GlobalFindAtomW);
DEBUGWRAP12(GlobalGetAtomNameA);
DEBUGWRAP12(GlobalGetAtomNameW);
DEBUGWRAP4(InitAtomTable);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_fileio

DEBUGWRAP4(GetStdHandle);
DEBUGWRAP4(SetHandleCount);
DEBUGWRAP12(SetHandleInformation);
DEBUGWRAP8(SetStdHandle);
DEBUGWRAP8(GetCompressedFileSizeA);
DEBUGWRAP8(GetCompressedFileSizeW);
DEBUGWRAP0_NORET(SetFileApisToANSI);
DEBUGWRAP0_NORET(SetFileApisToOEM);
DEBUGWRAP8(SetFileAttributesA);
DEBUGWRAP8(SetFileAttributesW);
DEBUGWRAP16(SetFilePointer);
DEBUGWRAP16(SetFileTime);
DEBUGWRAP12(CopyFileA);
DEBUGWRAP24(CopyFileExA);
DEBUGWRAP24(CopyFileExW);
DEBUGWRAP12(CopyFileW);
DEBUGWRAP8(CompareFileTime);
DEBUGWRAP28(CreateFileA);
DEBUGWRAP28(CreateFileW);
DEBUGWRAP4(DeleteFileA);
DEBUGWRAP4(DeleteFileW);
DEBUGWRAP4(FindClose);
DEBUGWRAP4(FindCloseChangeNotification);
DEBUGWRAP12(FindFirstChangeNotificationA);
DEBUGWRAP12(FindFirstChangeNotificationW);
DEBUGWRAP8(FindFirstFileA);
DEBUGWRAP24(FindFirstFileExA);
DEBUGWRAP24(FindFirstFileExW);
DEBUGWRAP8(FindFirstFileW);
DEBUGWRAP4(FindNextChangeNotification);
DEBUGWRAP8(FindNextFileA);
DEBUGWRAP8(FindNextFileW);
DEBUGWRAP4(FlushFileBuffers);
DEBUGWRAP20(ReadFile);
DEBUGWRAP20(ReadFileEx);
DEBUGWRAP4(SetEndOfFile);
DEBUGWRAP20(WriteFile);
DEBUGWRAP20(WriteFileEx);
DEBUGWRAP12(_lread);
DEBUGWRAP12(_lwrite);
DEBUGWRAP4(_lclose);
DEBUGWRAP8(_lcreat);
DEBUGWRAP12(_llseek);
DEBUGWRAP8(_lopen);
DEBUGWRAP12(GetFileAttributesExA);
DEBUGWRAP12(GetFileAttributesExW);
DEBUGWRAP12(GetLongPathNameA);
DEBUGWRAP12(GetLongPathNameW);
DEBUGWRAP12(OpenFile);
DEBUGWRAP12(OpenFileMappingA);
DEBUGWRAP12(OpenFileMappingW);
DEBUGWRAP24(CreateFileMappingA);
DEBUGWRAP24(CreateFileMappingW);
DEBUGWRAP12(FileTimeToDosDateTime);
DEBUGWRAP8(FileTimeToLocalFileTime);
DEBUGWRAP8(FileTimeToSystemTime);
DEBUGWRAP8(LocalFileTimeToFileTime);
DEBUGWRAP20(LockFile);
DEBUGWRAP24(LockFileEx);
DEBUGWRAP20(MapViewOfFile);
DEBUGWRAP24(MapViewOfFileEx);
DEBUGWRAP8(MoveFileA);
DEBUGWRAP12(MoveFileExA);
DEBUGWRAP12(MoveFileExW);
DEBUGWRAP8(MoveFileW);
DEBUGWRAP20(UnlockFile);
DEBUGWRAP20(UnlockFileEx);
DEBUGWRAP4(UnmapViewOfFile);
DEBUGWRAP12(GetShortPathNameA);
DEBUGWRAP12(GetShortPathNameW);
DEBUGWRAP8(FlushViewOfFile);
DEBUGWRAP4(GetFileAttributesA);
DEBUGWRAP4(GetFileAttributesW);
DEBUGWRAP8(GetFileInformationByHandle);
DEBUGWRAP8(GetFileSize);
DEBUGWRAP16(GetFileTime);
DEBUGWRAP4(GetFileType);
DEBUGWRAP16(GetFullPathNameA);
DEBUGWRAP16(GetFullPathNameW);
DEBUGWRAP32(DeviceIoControl);
DEBUGWRAP4(CancelIo);
DEBUGWRAP4(CloseHandle);
DEBUGWRAP8(GetHandleInformation);
DEBUGWRAP16(GetOverlappedResult);
DEBUGWRAP28(DuplicateHandle);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_environ

DEBUGWRAP0(GetEnvironmentStringsA);
DEBUGWRAP0(GetEnvironmentStringsW);
DEBUGWRAP12(GetEnvironmentVariableA);
DEBUGWRAP12(GetEnvironmentVariableW);
DEBUGWRAP12(ExpandEnvironmentStringsA);
DEBUGWRAP12(ExpandEnvironmentStringsW);
DEBUGWRAP4(FreeEnvironmentStringsA);
DEBUGWRAP4(FreeEnvironmentStringsW);
DEBUGWRAP8(SetEnvironmentVariableA);
DEBUGWRAP8(SetEnvironmentVariableW);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_resource

DEBUGWRAP12(VerLanguageNameA);
DEBUGWRAP12(VerLanguageNameW);
DEBUGWRAP20(EnumResourceLanguagesA);
DEBUGWRAP20(EnumResourceLanguagesW);
DEBUGWRAP16(EnumResourceNamesA);
DEBUGWRAP16(EnumResourceNamesW);
DEBUGWRAP12(EnumResourceTypesA);
DEBUGWRAP12(EnumResourceTypesW);
DEBUGWRAP8(BeginUpdateResourceA);
DEBUGWRAP8(BeginUpdateResourceW);
DEBUGWRAP8(EndUpdateResourceA);
DEBUGWRAP8(EndUpdateResourceW);
DEBUGWRAP12(FindResourceA);
DEBUGWRAP16(FindResourceExA);
DEBUGWRAP16(FindResourceExW);
DEBUGWRAP12(FindResourceW);
DEBUGWRAP4(FreeResource);
DEBUGWRAP8(LoadResource);
DEBUGWRAP4(LockResource);
DEBUGWRAP8(SizeofResource);
DEBUGWRAP24(UpdateResourceA);
DEBUGWRAP24(UpdateResourceW);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_heap

DEBUGWRAP_LVL2_8(LocalAlloc);
DEBUGWRAP_LVL2_4(LocalCompact);
DEBUGWRAP_LVL2_4(LocalFlags);
DEBUGWRAP_LVL2_4(LocalFree);
DEBUGWRAP_LVL2_4(LocalHandle);
DEBUGWRAP_LVL2_4(LocalLock);
DEBUGWRAP_LVL2_12(LocalReAlloc);
DEBUGWRAP_LVL2_8(LocalShrink);
DEBUGWRAP_LVL2_4(LocalSize);
DEBUGWRAP_LVL2_4(LocalUnlock);
DEBUGWRAP_LVL2_8(GlobalAlloc);
DEBUGWRAP_LVL2_4(GlobalCompact);
DEBUGWRAP_LVL2_4(GlobalFix);
DEBUGWRAP_LVL2_4(GlobalFlags);
DEBUGWRAP_LVL2_4(GlobalFree);
DEBUGWRAP_LVL2_4(GlobalHandle);
DEBUGWRAP_LVL2_4(GlobalLock);
DEBUGWRAP_LVL2_4(GlobalMemoryStatus);
DEBUGWRAP_LVL2_12(GlobalReAlloc);
DEBUGWRAP_LVL2_4(GlobalSize);
DEBUGWRAP_LVL2_4(GlobalUnWire);
DEBUGWRAP_LVL2_4(GlobalUnfix);
DEBUGWRAP_LVL2_4(GlobalUnlock);
DEBUGWRAP_LVL2_4(GlobalWire);
DEBUGWRAP_LVL2_12(HeapAlloc);
DEBUGWRAP_LVL2_8(HeapCompact);
DEBUGWRAP_LVL2_12(HeapCreate);
DEBUGWRAP_LVL2_4(HeapDestroy);
DEBUGWRAP_LVL2_12(HeapFree);
DEBUGWRAP_LVL2_4(HeapLock);
DEBUGWRAP_LVL2_16(HeapReAlloc);
DEBUGWRAP_LVL2_12(HeapSize);
DEBUGWRAP_LVL2_4(HeapUnlock);
DEBUGWRAP_LVL2_12(HeapValidate);
DEBUGWRAP_LVL2_8(HeapWalk);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_npipe

DEBUGWRAP28(GetNamedPipeHandleStateA);
DEBUGWRAP28(GetNamedPipeHandleStateW);
DEBUGWRAP28(CallNamedPipeA);
DEBUGWRAP28(CallNamedPipeW);
DEBUGWRAP8(ConnectNamedPipe);
DEBUGWRAP32(CreateNamedPipeA);
DEBUGWRAP32(CreateNamedPipeW);
DEBUGWRAP16(CreatePipe);
DEBUGWRAP4(DisconnectNamedPipe);
DEBUGWRAP20(GetNamedPipeInfo);
DEBUGWRAP24(PeekNamedPipe);
DEBUGWRAP16(SetNamedPipeHandleState);
DEBUGWRAP28(TransactNamedPipe);
DEBUGWRAP8(WaitNamedPipeA);
DEBUGWRAP8(WaitNamedPipeW);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_event

DEBUGWRAP16(CreateEventA);
DEBUGWRAP16(CreateEventW);
DEBUGWRAP12(OpenEventA);
DEBUGWRAP12(OpenEventW);
DEBUGWRAP4(PulseEvent);
DEBUGWRAP4(ResetEvent);
DEBUGWRAP4(SetEvent);
DEBUGWRAP16(WaitForMultipleObjects);
DEBUGWRAP20(WaitForMultipleObjectsEx);
DEBUGWRAP8(WaitForSingleObject);
DEBUGWRAP12(WaitForSingleObjectEx);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_mutex

DEBUGWRAP12(CreateMutexA);
DEBUGWRAP12(CreateMutexW);
DEBUGWRAP12(OpenMutexA);
DEBUGWRAP12(OpenMutexW);
DEBUGWRAP4(ReleaseMutex);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_semaphore
DEBUGWRAP12(OpenSemaphoreA);
DEBUGWRAP12(OpenSemaphoreW);
DEBUGWRAP16(CreateSemaphoreA);
DEBUGWRAP16(CreateSemaphoreW);
DEBUGWRAP12(ReleaseSemaphore);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG  DBG_disk

DEBUGWRAP16(GetDiskFreeSpaceExA);
DEBUGWRAP16(GetDiskFreeSpaceExW);
DEBUGWRAP20(GetDiskFreeSpaceA);
DEBUGWRAP20(GetDiskFreeSpaceW);
DEBUGWRAP4(GetDriveTypeA);
DEBUGWRAP4(GetDriveTypeW);
DEBUGWRAP32(GetVolumeInformationA);
DEBUGWRAP32(GetVolumeInformationW);
DEBUGWRAP8(SetVolumeLabelA);
DEBUGWRAP8(SetVolumeLabelW);
DEBUGWRAP8(GetLogicalDriveStringsA);
DEBUGWRAP8(GetLogicalDriveStringsW);
DEBUGWRAP0(GetLogicalDrives);


DEBUGWRAP8(FindFirstVolumeA);
DEBUGWRAP8(FindFirstVolumeW);
DEBUGWRAP12(FindNextVolumeA);
DEBUGWRAP12(FindNextVolumeW);
DEBUGWRAP4(FindVolumeClose);
DEBUGWRAP12(FindFirstVolumeMountPointA);
DEBUGWRAP12(FindFirstVolumeMountPointW);
DEBUGWRAP12(FindNextVolumeMountPointA);
DEBUGWRAP12(FindNextVolumeMountPointW);
DEBUGWRAP4(FindVolumeMountPointClose);
DEBUGWRAP12(GetVolumeNameForVolumeMountPointA);
DEBUGWRAP12(GetVolumeNameForVolumeMountPointW);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_mailslot
DEBUGWRAP16(CreateMailslotA);
DEBUGWRAP16(CreateMailslotW);
DEBUGWRAP20(GetMailslotInfo);
DEBUGWRAP8(SetMailslotInfo);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_thread
DEBUGWRAP28(CreateRemoteThread);
DEBUGWRAP24(CreateThread);
DEBUGWRAP4(ExitThread);
DEBUGWRAP_LVL2_0(GetCurrentThread);
DEBUGWRAP_LVL2_0(GetCurrentThreadId);
DEBUGWRAP8(GetExitCodeThread);
DEBUGWRAP8(GetThreadContext);
DEBUGWRAP0(GetThreadLocale);
DEBUGWRAP4(GetThreadPriority);
DEBUGWRAP12(GetThreadSelectorEntry);
DEBUGWRAP20(GetThreadTimes);
DEBUGWRAP4(ResumeThread);
DEBUGWRAP8(SetThreadAffinityMask);
DEBUGWRAP8(SetThreadContext);
DEBUGWRAP4(SetThreadLocale);
DEBUGWRAP8(SetThreadPriority);
DEBUGWRAP8(SetThreadPriorityBoost);
DEBUGWRAP4(SuspendThread);
DEBUGWRAP8(TerminateThread);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_process

DEBUGWRAP8(SetPriorityClass);
DEBUGWRAP40(CreateProcessA);
DEBUGWRAP40(CreateProcessW);
DEBUGWRAP4(ExitProcess);
DEBUGWRAP0(GetCurrentProcess);
DEBUGWRAP0(GetCurrentProcessId);
DEBUGWRAP8(GetExitCodeProcess);
DEBUGWRAP12(GetProcessAffinityMask);
DEBUGWRAP8(GetProcessDword);
DEBUGWRAP4(GetProcessFlags);
DEBUGWRAP_LVL2_0(GetProcessHeap);
DEBUGWRAP8(GetProcessHeaps);
DEBUGWRAP8(GetProcessShutdownParameters);
DEBUGWRAP20(GetProcessTimes);
DEBUGWRAP4(GetProcessVersion);
DEBUGWRAP12(GetProcessWorkingSetSize);
DEBUGWRAP20(ReadProcessMemory);
DEBUGWRAP8(RegisterServiceProcess);
DEBUGWRAP8(SetProcessAffinityMask);
DEBUGWRAP12(SetProcessDword);
DEBUGWRAP8(SetProcessPriorityBoost);
DEBUGWRAP8(SetProcessShutdownParameters);
DEBUGWRAP12(SetProcessWorkingSetSize);
DEBUGWRAP8(TerminateProcess);
DEBUGWRAP12(OpenProcess);
DEBUGWRAP20(WriteProcessMemory);
DEBUGWRAP4(GetPriorityClass);
DEBUGWRAP8(FatalAppExitA);
DEBUGWRAP8(FatalAppExitW);
DEBUGWRAP4(FatalExit);
DEBUGWRAP_LVL2_4(Sleep);
DEBUGWRAP8(SleepEx);
DEBUGWRAP8(WinExec);
DEBUGWRAP4(GetStartupInfoA);
DEBUGWRAP4(GetStartupInfoW);
DEBUGWRAP0(GetCommandLineA);
DEBUGWRAP0(GetCommandLineW);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_debug

DEBUGWRAP4(DebugActiveProcess);
DEBUGWRAP0_NORET(DebugBreak);
DEBUGWRAP4(OutputDebugStringA);
DEBUGWRAP4(OutputDebugStringW);
DEBUGWRAP8(WaitForDebugEvent);
DEBUGWRAP0(IsDebuggerPresent);
DEBUGWRAP12(ContinueDebugEvent);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_nls
DEBUGWRAP0(GetACP);
DEBUGWRAP8(GetCPInfo);
DEBUGWRAP4(ConvertDefaultLocale);
DEBUGWRAP4(IsValidCodePage);
DEBUGWRAP8(IsValidLocale);
DEBUGWRAP0(GetSystemDefaultLCID);
DEBUGWRAP0(GetSystemDefaultLangID);
DEBUGWRAP24(GetCurrencyFormatA);
DEBUGWRAP24(GetCurrencyFormatW);
DEBUGWRAP24(GetDateFormatA);
DEBUGWRAP24(GetDateFormatW);
DEBUGWRAP16(EnumCalendarInfoA);
DEBUGWRAP16(EnumCalendarInfoW);
DEBUGWRAP12(EnumDateFormatsA);
DEBUGWRAP12(EnumDateFormatsW);
DEBUGWRAP8(EnumSystemCodePagesA);
DEBUGWRAP8(EnumSystemCodePagesW);
DEBUGWRAP8(EnumSystemLocalesA);
DEBUGWRAP8(EnumSystemLocalesW);
DEBUGWRAP12(EnumTimeFormatsA);
DEBUGWRAP12(EnumTimeFormatsW);
DEBUGWRAP24(GetTimeFormatA);
DEBUGWRAP24(GetTimeFormatW);
DEBUGWRAP0(GetUserDefaultLCID);
DEBUGWRAP0(GetUserDefaultLangID);
DEBUGWRAP24(GetNumberFormatA);
DEBUGWRAP24(GetNumberFormatW);
DEBUGWRAP0(GetOEMCP);
DEBUGWRAP12(SetLocaleInfoA);
DEBUGWRAP12(SetLocaleInfoW);
DEBUGWRAP16(GetLocaleInfoA);
DEBUGWRAP16(GetLocaleInfoW);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_time
DEBUGWRAP4(GetTimeZoneInformation);
DEBUGWRAP12(DosDateTimeToFileTime);
DEBUGWRAP4(GetSystemTime);
DEBUGWRAP12(GetSystemTimeAdjustment);
DEBUGWRAP4_NORET(GetSystemTimeAsFileTime);
DEBUGWRAP4_NORET(GetLocalTime);
DEBUGWRAP4(SetLocalTime);
DEBUGWRAP4_NORET(SetSystemTime);
DEBUGWRAP8(SetSystemTimeAdjustment);
DEBUGWRAP4(SetTimeZoneInformation);
DEBUGWRAP8(SystemTimeToFileTime);
DEBUGWRAP12(SystemTimeToTzSpecificLocalTime);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG  DBG_wintls
DEBUGWRAP_LVL2_0(TlsAlloc);
DEBUGWRAP_LVL2_4(TlsFree);
DEBUGWRAP_LVL2_4(TlsGetValue);
DEBUGWRAP_LVL2_8(TlsSetValue);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_critsection
DEBUGWRAP_LVL2_4(DeleteCriticalSection);
DEBUGWRAP_LVL2_4(EnterCriticalSection);
DEBUGWRAP_LVL2_4(InitializeCriticalSection);
DEBUGWRAP_LVL2_4(LeaveCriticalSection);
DEBUGWRAP_LVL2_4(MakeCriticalSectionGlobal);
DEBUGWRAP_LVL2_4(ReinitializeCriticalSection);
DEBUGWRAP_LVL2_4(TryEnterCriticalSection);
DEBUGWRAP_LVL2_4(UninitializeCriticalSection);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_module
DEBUGWRAP4(LoadLibrary16);
DEBUGWRAP4(FreeLibrary16);
DEBUGWRAP8(GetProcAddress16);
DEBUGWRAP4(LoadLibraryA);
DEBUGWRAP12(LoadLibraryExA);
DEBUGWRAP12(LoadLibraryExW);
DEBUGWRAP4(LoadLibraryW);
DEBUGWRAP8(LoadModule);
DEBUGWRAP4(DisableThreadLibraryCalls);
DEBUGWRAP4(FreeLibrary);
DEBUGWRAP8(FreeLibraryAndExitThread);
DEBUGWRAP12(GetModuleFileNameA);
DEBUGWRAP12(GetModuleFileNameW);
DEBUGWRAP4(GetModuleHandleA);
DEBUGWRAP4(GetModuleHandleW);
DEBUGWRAP8(GetProcAddress);
DEBUGWRAP8(GetBinaryTypeA);
DEBUGWRAP8(GetBinaryTypeW);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_memory
DEBUGWRAP16(VirtualAlloc);
DEBUGWRAP12(VirtualFree);
DEBUGWRAP8(VirtualLock);
DEBUGWRAP16(VirtualProtect);
DEBUGWRAP20(VirtualProtectEx);
DEBUGWRAP12(VirtualQuery);
DEBUGWRAP16(VirtualQueryEx);
DEBUGWRAP8(VirtualUnlock);
DEBUGWRAP4(IsBadCodePtr);
DEBUGWRAP8(IsBadHugeReadPtr);
DEBUGWRAP8(IsBadHugeWritePtr);
DEBUGWRAP8(IsBadReadPtr);
DEBUGWRAP8(IsBadStringPtrA);
DEBUGWRAP8(IsBadStringPtrW);
DEBUGWRAP8(IsBadWritePtr);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_system
DEBUGWRAP4(GetSystemInfo);
DEBUGWRAP4(GetSystemPowerStatus);
DEBUGWRAP8(GetComputerNameA);
DEBUGWRAP8(GetComputerNameW);
DEBUGWRAP0(GetVersion);
DEBUGWRAP4(GetVersionExA);
DEBUGWRAP4(GetVersionExW);
DEBUGWRAP4(SetComputerNameA);
DEBUGWRAP4(SetComputerNameW);
DEBUGWRAP_LVL2_0(GetTickCount);
DEBUGWRAP4(GetVDMCurrentDirectories);
DEBUGWRAP_LVL2_4(QueryPerformanceCounter);
DEBUGWRAP4(QueryPerformanceFrequency);
DEBUGWRAP4(IsProcessorFeaturePresent);
DEBUGWRAP28(FormatMessageA);
DEBUGWRAP28(FormatMessageW);
DEBUGWRAP12(FlushInstructionCache);
DEBUGWRAP8(SetSystemPowerState);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_string
DEBUGWRAP24(CompareStringA);
DEBUGWRAP24(CompareStringW);
DEBUGWRAP20(FoldStringA);
DEBUGWRAP20(FoldStringW);
DEBUGWRAP24(LCMapStringA);
DEBUGWRAP24(LCMapStringW);
DEBUGWRAP_LVL2_24(MultiByteToWideChar);
DEBUGWRAP20(GetStringTypeA);
DEBUGWRAP20(GetStringTypeExA);
DEBUGWRAP20(GetStringTypeExW);
DEBUGWRAP16(GetStringTypeW);
DEBUGWRAP_LVL2_4(IsDBCSLeadByte);
DEBUGWRAP_LVL2_8(IsDBCSLeadByteEx);
DEBUGWRAP_LVL2_32(WideCharToMultiByte);


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_exceptions
DEBUGWRAP16(RaiseException);
DEBUGWRAP16(RtlUnwind);
DEBUGWRAP4(SetUnhandledExceptionFilter);
DEBUGWRAP4(UnhandledExceptionFilter);

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_char
DEBUGWRAP_LVL2_4(CharLowerA)
DEBUGWRAP_LVL2_8(CharLowerBuffA)
DEBUGWRAP_LVL2_8(CharLowerBuffW)
DEBUGWRAP_LVL2_4(CharLowerW)
DEBUGWRAP_LVL2_4(CharNextA)
DEBUGWRAP_LVL2_12(CharNextExA)
DEBUGWRAP_LVL2_12(CharNextExW)
DEBUGWRAP_LVL2_4(CharNextW)
DEBUGWRAP_LVL2_8(CharPrevA)
DEBUGWRAP_LVL2_16(CharPrevExA)
DEBUGWRAP_LVL2_16(CharPrevExW)
DEBUGWRAP_LVL2_8(CharPrevW)
DEBUGWRAP_LVL2_8(CharToOemA)
DEBUGWRAP_LVL2_12(CharToOemBuffA)
DEBUGWRAP_LVL2_12(CharToOemBuffW)
DEBUGWRAP_LVL2_8(CharToOemW)
DEBUGWRAP_LVL2_4(CharUpperA)
DEBUGWRAP_LVL2_8(CharUpperBuffA)
DEBUGWRAP_LVL2_8(CharUpperBuffW)
DEBUGWRAP_LVL2_4(CharUpperW)
DEBUGWRAP_LVL2_4(IsCharAlphaA)
DEBUGWRAP_LVL2_4(IsCharAlphaNumericA)
DEBUGWRAP_LVL2_4(IsCharAlphaNumericW)
DEBUGWRAP_LVL2_4(IsCharAlphaW)
DEBUGWRAP_LVL2_4(IsCharLowerA)
DEBUGWRAP_LVL2_4(IsCharLowerW)
DEBUGWRAP_LVL2_4(IsCharUpperA)
DEBUGWRAP_LVL2_4(IsCharUpperW)
DEBUGWRAP_LVL2_8(OemToCharA)
DEBUGWRAP_LVL2_12(OemToCharBuffA)
DEBUGWRAP_LVL2_12(OemToCharBuffW)
DEBUGWRAP_LVL2_8(OemToCharW)
