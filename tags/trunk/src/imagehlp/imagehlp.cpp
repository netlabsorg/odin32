/* $Id: imagehlp.cpp,v 1.3 1999-11-12 11:38:39 sandervl Exp $ */
/*
 *	IMAGEHLP library
 *
 *	Copyright 1998	Patrik Stridvall
 *	Copyright 1999	Jens Wiessner
 */

#include <os2win.h>
#include <odinwrap.h>
#include <imagehlp.h>
#include <heapstring.h>

ODINDEBUGCHANNEL(imagehlp)

/***********************************************************************
 *           Data
 */

static PLOADED_IMAGE IMAGEHLP_pFirstLoadedImage=NULL;
static PLOADED_IMAGE IMAGEHLP_pLastLoadedImage=NULL;

static LOADED_IMAGE IMAGEHLP_EmptyLoadedImage = {
  NULL,       /* ModuleName */
  0xffffffff, /* hFile */
  NULL,       /* MappedAddress */
  NULL,       /* FileHeader */
  NULL,       /* LastRvaSection */
  0,          /* NumberOfSections */
  NULL,       /* Sections */
  1,          /* Characteristics */
  FALSE,      /* fSystemImage */
  FALSE,      /* fDOSImage */
  { &IMAGEHLP_EmptyLoadedImage.Links, &IMAGEHLP_EmptyLoadedImage.Links }, /* Links */
  148,        /* SizeOfImage; */
};


/**********************************************************************/

HANDLE IMAGEHLP_hHeap = (HANDLE) NULL;

static API_VERSION IMAGEHLP_ApiVersion = { 4, 0, 0, 5 };

/***********************************************************************
 *           IMAGEHLP_LibMain (IMAGEHLP.init)
 */
BOOL WINAPI IMAGEHLP_LibMain(
  HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
      IMAGEHLP_hHeap = HeapCreate(0, 0x10000, 0);
      break;
    case DLL_PROCESS_DETACH:
      HeapDestroy(IMAGEHLP_hHeap);
      IMAGEHLP_hHeap = (HANDLE) NULL;
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    default:
      break;
    }
  return TRUE;
}


/***********************************************************************
 *           BindImage (IMAGEHLP.1)
 */
BOOL WINAPI BindImage(
  LPSTR ImageName, LPSTR DllPath, LPSTR SymbolPath)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: BindImage\n"));
#endif
  return BindImageEx(0, ImageName, DllPath, SymbolPath, NULL);
}


/***********************************************************************
 *           BindImageEx (IMAGEHLP.2)
 */
BOOL WINAPI BindImageEx(
  DWORD Flags, LPSTR ImageName, LPSTR DllPath, LPSTR SymbolPath,
  PIMAGEHLP_STATUS_ROUTINE StatusRoutine)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: BindImageEx not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           CheckSumMappedFile (IMAGEHLP.3)
 */
PIMAGE_NT_HEADERS WINAPI CheckSumMappedFile(
  LPVOID BaseAddress, DWORD FileLength, 
  LPDWORD HeaderSum, LPDWORD CheckSum)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: CheckSumMappedFile not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}


/***********************************************************************
 *           EnumerateLoadedModules (IMAGEHLP.4)
 */
BOOL WINAPI EnumerateLoadedModules(
  HANDLE hProcess,
  PENUMLOADED_MODULES_CALLBACK EnumLoadedModulesCallback,
  PVOID UserContext)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: EnumerateLoadedModules not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           FindDebugInfoFile (IMAGEHLP.5)
 */
HANDLE WINAPI FindDebugInfoFile(
  LPSTR FileName, LPSTR SymbolPath, LPSTR DebugFilePath)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: FindDebugInfoFile not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HANDLE) NULL;
}


/***********************************************************************
 *           FindExecutableImage (IMAGEHLP.6)
 */
HANDLE WINAPI FindExecutableImage(
  LPSTR FileName, LPSTR SymbolPath, LPSTR ImageFilePath)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: FindExecutableImage not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (HANDLE) NULL;
}


/***********************************************************************
 *           GetImageConfigInformation (IMAGEHLP.7)
 */
BOOL WINAPI GetImageConfigInformation(
  PLOADED_IMAGE LoadedImage,
  PIMAGE_LOAD_CONFIG_DIRECTORY ImageConfigInformation)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: GetImageConfigInformation not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/***********************************************************************
 *           GetImageUnusedHeaderBytes (IMAGEHLP.8)
 */
DWORD WINAPI GetImageUnusedHeaderBytes(
  PLOADED_IMAGE LoadedImage,
  LPDWORD SizeUnusedHeaderBytes)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: GetImageUnusedHeaderBytes not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           GetTimestampForLoadedLibrary (IMAGEHLP.9)
 */
DWORD WINAPI GetTimestampForLoadedLibrary(HMODULE Module)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: GetTimestampForLoadedLibrary not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}


/***********************************************************************
 *           ImageAddCertificate (IMAGEHLP.10)
 */

BOOL WINAPI ImageAddCertificate(
  HANDLE FileHandle, PWIN_CERTIFICATE Certificate, PDWORD Index)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageAddCertificate not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImageDirectoryEntryToData (IMAGEHLP.11)
 */
PVOID WINAPI ImageDirectoryEntryToData(
  PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, PULONG Size)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageDirectoryEntryToData not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}

/***********************************************************************
 *           ImageEnumerateCertificates (IMAGEHLP.12)
 */
BOOL WINAPI ImageEnumerateCertificates(
  HANDLE FileHandle, WORD TypeFilter, PDWORD CertificateCount,
  PDWORD Indices, DWORD IndexCount)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageEnumerateCertificates not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImageGetCertificateData (IMAGEHLP.13)
 */
BOOL WINAPI ImageGetCertificateData(
  HANDLE FileHandle, DWORD CertificateIndex,
  PWIN_CERTIFICATE Certificate, PDWORD RequiredLength)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageGetCertificateData not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImageGetCertificateHeader (IMAGEHLP.14)
 */
BOOL WINAPI ImageGetCertificateHeader(
  HANDLE FileHandle, DWORD CertificateIndex,
  PWIN_CERTIFICATE Certificateheader)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageGetCertificateHeader not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImageGetDigestStream (IMAGEHLP.15)
 */
BOOL WINAPI ImageGetDigestStream(
  HANDLE FileHandle, DWORD DigestLevel,
  DIGEST_FUNCTION DigestFunction, DIGEST_HANDLE DigestHandle)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageGetDigestStream not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImageLoad (IMAGEHLP.16)
 */
PLOADED_IMAGE WINAPI ImageLoad(LPSTR DllName, LPSTR DllPath)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageLoad not implemented\n"));
#endif
// FIXME-JW  PLOADED_IMAGE pLoadedImage = HeapAlloc (IMAGEHLP_hHeap, 0, sizeof(LOADED_IMAGE));
// FIXME-JW  return pLoadedImage;
  return NULL;
}

/***********************************************************************
 *           ImageNtHeader (IMAGEHLP.17)
 */
PIMAGE_NT_HEADERS WINAPI ImageNtHeader(PVOID Base)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageNtHeader\n"));
#endif
  return (PIMAGE_NT_HEADERS)
    ((LPBYTE) Base + ((PIMAGE_DOS_HEADER) Base)->e_lfanew);
}

/***********************************************************************
 *           ImageRemoveCertificate (IMAGEHLP.18)
 */
BOOL WINAPI ImageRemoveCertificate(HANDLE FileHandle, DWORD Index)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageRemoveCertificate not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           ImageRvaToSection (IMAGEHLP.19)
 */
PIMAGE_SECTION_HEADER WINAPI ImageRvaToSection(
  PIMAGE_NT_HEADERS NtHeaders, PVOID Base, ULONG Rva)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageRvaToSection not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}

/***********************************************************************
 *           ImageRvaToVa (IMAGEHLP.20)
 */
PVOID WINAPI ImageRvaToVa(
  PIMAGE_NT_HEADERS NtHeaders, PVOID Base, ULONG Rva,
  PIMAGE_SECTION_HEADER *LastRvaSection)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageRvaToVa not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}

/***********************************************************************
 *           ImageUnload (IMAGEHLP.21)
 */
BOOL WINAPI ImageUnload(PLOADED_IMAGE pLoadedImage)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImageUnload\n"));
#endif
  LIST_ENTRY *pCurrent, *pFind;
  if(!IMAGEHLP_pFirstLoadedImage || !pLoadedImage)
    {
      /* No image loaded or null pointer */
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

  pFind=&pLoadedImage->Links;
  pCurrent=&IMAGEHLP_pFirstLoadedImage->Links;
  while((pCurrent != pFind) && 
    (pCurrent != NULL)) 
      pCurrent = pCurrent->Flink;
  if(!pCurrent)
    {
      /* Not found */
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

  if(pCurrent->Blink)
    pCurrent->Blink->Flink = pCurrent->Flink;
  else
    IMAGEHLP_pFirstLoadedImage = pCurrent->Flink?CONTAINING_RECORD(
      pCurrent->Flink, LOADED_IMAGE, Links):NULL;

  if(pCurrent->Flink)
    pCurrent->Flink->Blink = pCurrent->Blink;
  else
    IMAGEHLP_pLastLoadedImage = pCurrent->Blink?CONTAINING_RECORD(
      pCurrent->Blink, LOADED_IMAGE, Links):NULL;

  return FALSE;
}


/***********************************************************************
 *           ImagehlpApiVersion (IMAGEHLP.22)
 */
PAPI_VERSION WINAPI ImagehlpApiVersion()
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImagehlpApiVersion\n"));
#endif
  return &IMAGEHLP_ApiVersion;
}

/***********************************************************************
 *           ImagehlpApiVersionEx (IMAGEHLP.23)
 */
PAPI_VERSION WINAPI ImagehlpApiVersionEx(PAPI_VERSION AppVersion)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ImagehlpApiVersionEx\n"));
#endif
  if(!AppVersion)
    return NULL;

  AppVersion->MajorVersion = IMAGEHLP_ApiVersion.MajorVersion;
  AppVersion->MinorVersion = IMAGEHLP_ApiVersion.MinorVersion;
  AppVersion->Revision = IMAGEHLP_ApiVersion.Revision;
  AppVersion->Reserved = IMAGEHLP_ApiVersion.Reserved;

  return AppVersion;
}

/***********************************************************************
 *           MakeSureDirectoryPathExists (IMAGEHLP.24)
 */
BOOL WINAPI MakeSureDirectoryPathExists(LPCSTR DirPath)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: MakeSureDirectoryPathExists not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           MapAndLoad (IMAGEHLP.25)
 */
BOOL WINAPI MapAndLoad(
  LPSTR pszImageName, LPSTR pszDllPath, PLOADED_IMAGE pLoadedImage,
  BOOL bDotDll, BOOL bReadOnly)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: MapAndLoad\n"));
#endif
  CHAR szFileName[MAX_PATH];
  HANDLE hFile = (HANDLE) NULL;
  HANDLE hFileMapping = (HANDLE) NULL;
  HMODULE hModule = (HMODULE) NULL;
  PIMAGE_NT_HEADERS pNtHeader = NULL;

  /* PathCombine(&szFileName, pszDllPath, pszImageName); */
  /* PathRenameExtension(&szFileName, bDotDll?:"dll":"exe"); */

  /* FIXME: Check if the file already loaded (use IMAGEHLP_pFirstLoadedImage32) */
  if(!(hFile = CreateFileA(
    szFileName, GENERIC_READ, 1, /* FIXME: FILE_SHARE_READ not defined */
    NULL, OPEN_EXISTING, 0, (HANDLE) NULL)))
    {
      SetLastError(ERROR_FILE_NOT_FOUND);
      goto Error;
    }

  if(!(hFileMapping = CreateFileMappingA(
    hFile, NULL, PAGE_READONLY | SEC_COMMIT, 0, 0, NULL)))
    {
      DWORD dwLastError = GetLastError();
#ifdef DEBUG
      dprintf(("CreateFileMapping: Error = %ld\n", dwLastError));
#endif
      SetLastError(dwLastError);
      goto Error;
    }
  CloseHandle(hFile);
  hFile = (HANDLE) NULL;

  if(!(hModule = (HMODULE) MapViewOfFile(
    hFileMapping, FILE_MAP_READ, 0, 0, 0)))
    {
      DWORD dwLastError = GetLastError();
#ifdef DEBUG
      dprintf(("MapViewOfFile: Error = %ld\n", dwLastError));
#endif
      SetLastError(dwLastError);
      goto Error;
    }

  CloseHandle(hFileMapping);
  hFileMapping=(HANDLE) NULL;

  pLoadedImage = (PLOADED_IMAGE) HeapAlloc(
    IMAGEHLP_hHeap, 0, sizeof(LOADED_IMAGE)
  );

  pNtHeader = ImageNtHeader((PVOID) hModule);

  pLoadedImage->ModuleName =
    HEAP_strdupA(IMAGEHLP_hHeap, 0, pszDllPath); /* FIXME: Correct? */
  pLoadedImage->hFile = hFile;
  pLoadedImage->MappedAddress = (PUCHAR) hModule;
  pLoadedImage->FileHeader = pNtHeader;
  pLoadedImage->Sections = (PIMAGE_SECTION_HEADER)
    ((LPBYTE) &pNtHeader->OptionalHeader +
      pNtHeader->FileHeader.SizeOfOptionalHeader);
  pLoadedImage->NumberOfSections =
    pNtHeader->FileHeader.NumberOfSections;
  pLoadedImage->SizeOfImage =
    pNtHeader->OptionalHeader.SizeOfImage;
  pLoadedImage->Characteristics =
    pNtHeader->FileHeader.Characteristics;
  pLoadedImage->LastRvaSection = pLoadedImage->Sections;

  pLoadedImage->fSystemImage = FALSE; /* FIXME */
  pLoadedImage->fDOSImage = FALSE;    /* FIXME */

  /* FIXME: Make thread safe */
  pLoadedImage->Links.Flink = NULL;
  pLoadedImage->Links.Blink = &IMAGEHLP_pLastLoadedImage->Links;
  if(IMAGEHLP_pLastLoadedImage)
    IMAGEHLP_pLastLoadedImage->Links.Flink = &pLoadedImage->Links;
  IMAGEHLP_pLastLoadedImage = pLoadedImage;
  if(!IMAGEHLP_pFirstLoadedImage)
    IMAGEHLP_pFirstLoadedImage = pLoadedImage;

  return TRUE;

Error:
  if(hModule)
    UnmapViewOfFile((PVOID) hModule);
  if(hFileMapping)
    CloseHandle(hFileMapping);
  if(hFile)
    CloseHandle(hFile);
  return FALSE;
}

/***********************************************************************
 *           MapDebugInformation (IMAGEHLP.26)
 */
PIMAGE_DEBUG_INFORMATION WINAPI MapDebugInformation(
  HANDLE FileHandle, LPSTR FileName,
  LPSTR SymbolPath, DWORD ImageBase)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: MapDebugInformation not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}

/***********************************************************************
 *           MapFileAndCheckSumA (IMAGEHLP.27)
 */
DWORD WINAPI MapFileAndCheckSumA(
  LPSTR Filename, LPDWORD HeaderSum, LPDWORD CheckSum)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: MapFileAndCheckSumA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return CHECKSUM_OPEN_FAILURE;
}

/***********************************************************************
 *           MapFileAndCheckSumW (IMAGEHLP.28)
 */
DWORD WINAPI MapFileAndCheckSumW(
  LPWSTR Filename, LPDWORD HeaderSum, LPDWORD CheckSum)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: MapFileAndCheckSumW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return CHECKSUM_OPEN_FAILURE;
}

/***********************************************************************
 *           MarkImageAsRunFromSwap (IMAGEHLP.29)
 * FIXME
 *   No documentation available.
 */

/***********************************************************************
 *           ReBaseImage (IMAGEHLP.30)
 */
BOOL WINAPI ReBaseImage(
  LPSTR CurrentImageName, LPSTR SymbolPath, BOOL fReBase,
  BOOL fRebaseSysfileOk, BOOL fGoingDown, ULONG CheckImageSize,
  ULONG *OldImageSize, ULONG *OldImageBase, ULONG *NewImageSize,
  ULONG *NewImageBase, ULONG TimeStamp)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: ReBaseImage not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           RemovePrivateCvSymbolic (IMAGEHLP.31)
 */
BOOL WINAPI RemovePrivateCvSymbolic(
  PCHAR DebugData, PCHAR *NewDebugData, ULONG *NewDebugSize)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: RemovePrivateCvSymbolic not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           RemoveRelocations (IMAGEHLP.32)
 */
VOID WINAPI RemoveRelocations(PCHAR ImageName)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: RemoveRelocations not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/***********************************************************************
 *           SearchTreeForFile (IMAGEHLP.33)
 */
BOOL WINAPI SearchTreeForFile(
  LPSTR RootPath, LPSTR InputPathName, LPSTR OutputPathBuffer)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SearchTreeForFile not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SetImageConfigInformation (IMAGEHLP.34)
 */
BOOL WINAPI SetImageConfigInformation(
  PLOADED_IMAGE LoadedImage,
  PIMAGE_LOAD_CONFIG_DIRECTORY ImageConfigInformation)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SetImageConfigInformation not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SplitSymbols (IMAGEHLP.35)
 */
BOOL WINAPI SplitSymbols(
  LPSTR ImageName, LPSTR SymbolsPath, 
  LPSTR SymbolFilePath, DWORD Flags)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SplitSymbols not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           StackWalk (IMAGEHLP.36)
 */
BOOL WINAPI StackWalk(
  DWORD MachineType, HANDLE hProcess, HANDLE hThread,
  PSTACKFRAME StackFrame, PVOID ContextRecord,
  PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
  PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
  PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
  PTRANSLATE_ADDRESS_ROUTINE TranslateAddress)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: StackWalk not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymCleanup (IMAGEHLP.37)
 */
BOOL WINAPI SymCleanup(HANDLE hProcess)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymCleanup not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymEnumerateModules (IMAGEHLP.38)
 */

BOOL WINAPI SymEnumerateModules(
  HANDLE hProcess, PSYM_ENUMMODULES_CALLBACK EnumModulesCallback,
  PVOID UserContext)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymEnumerateModules not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymEnumerateSymbols (IMAGEHLP.39)
 */
BOOL WINAPI SymEnumerateSymbols(
  HANDLE hProcess, DWORD BaseOfDll,
  PSYM_ENUMSYMBOLS_CALLBACK EnumSymbolsCallback, PVOID UserContext)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymEnumerateSymbols not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymFunctionTableAccess (IMAGEHLP.40)
 */
PVOID WINAPI SymFunctionTableAccess(HANDLE hProcess, DWORD AddrBase)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymFunctionTableAccess not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymGetModuleBase (IMAGEHLP.41)
 */
DWORD WINAPI SymGetModuleBase(HANDLE hProcess, DWORD dwAddr)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymGetModuleBase not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           SymGetModuleInfo (IMAGEHLP.42)
 */
BOOL WINAPI SymGetModuleInfo(
  HANDLE hProcess, DWORD dwAddr,
  PIMAGEHLP_MODULE ModuleInfo)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymGetModuleInfo not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymGetOptions (IMAGEHLP.43)
 */
DWORD WINAPI SymGetOptions()
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymGetOptions not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           SymGetSearchPath (IMAGEHLP.44)
 */
BOOL WINAPI SymGetSearchPath(
  HANDLE hProcess, LPSTR szSearchPath, DWORD SearchPathLength)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymGetSearchPath not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymGetSymFromAddr (IMAGEHLP.45)
 */
BOOL WINAPI SymGetSymFromAddr(
  HANDLE hProcess, DWORD dwAddr, 
  PDWORD pdwDisplacement, PIMAGEHLP_SYMBOL Symbol)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymGetSymFromAddr not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymGetSymFromName (IMAGEHLP.46)
 */
BOOL WINAPI SymGetSymFromName(
  HANDLE hProcess, LPSTR Name, PIMAGEHLP_SYMBOL Symbol)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymGetSymFromName not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymGetSymNext (IMAGEHLP.47)
 */
BOOL WINAPI SymGetSymNext(
  HANDLE hProcess, PIMAGEHLP_SYMBOL Symbol)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymGetSymNext not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymGetSymPrev (IMAGEHLP.48)
 */

BOOL WINAPI SymGetSymPrev(
  HANDLE hProcess, PIMAGEHLP_SYMBOL Symbol)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymGetSymPrev not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymInitialize (IMAGEHLP.49)
 */
BOOL WINAPI SymInitialize(
  HANDLE hProcess, LPSTR UserSearchPath, BOOL fInvadeProcess)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymInitialize not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymLoadModule (IMAGEHLP.50)
 */

BOOL WINAPI SymLoadModule(
  HANDLE hProcess, HANDLE hFile, LPSTR ImageName, LPSTR ModuleName,
  DWORD BaseOfDll, DWORD SizeOfDll)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymLoadModule not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymRegisterCallback (IMAGEHLP.51)
 */
BOOL WINAPI SymRegisterCallback(
  HANDLE hProcess, PSYMBOL_REGISTERED_CALLBACK CallbackFunction,
  PVOID UserContext)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymRegisterCallback not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymSetOptions (IMAGEHLP.52)
 */
DWORD WINAPI SymSetOptions(DWORD SymOptions)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymSetOptions not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           SymSetSearchPath (IMAGEHLP.53)
 */
BOOL WINAPI SymSetSearchPath(HANDLE hProcess, LPSTR szSearchPath)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymSetSearchPath not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymUnDName (IMAGEHLP.54)
 */
BOOL WINAPI SymUnDName(
  PIMAGEHLP_SYMBOL sym, LPSTR UnDecName, DWORD UnDecNameLength)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymUnDName not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           SymUnloadModule (IMAGEHLP.55)
 */
BOOL WINAPI SymUnloadModule(
  HANDLE hProcess, DWORD BaseOfDll)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: SymUnloadModule not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           TouchFileTimes (IMAGEHLP.56)
 */
BOOL WINAPI TouchFileTimes(
  HANDLE FileHandle, LPSYSTEMTIME lpSystemTime)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: TouchFileTimes not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           UnDecorateSymbolName (IMAGEHLP.57)
 */
DWORD WINAPI UnDecorateSymbolName(
  LPCSTR DecoratedName, LPSTR UnDecoratedName,
  DWORD UndecoratedLength, DWORD Flags)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: UnDecorateSymbolName not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           UnMapAndLoad (IMAGEHLP.58)
 */
BOOL WINAPI UnMapAndLoad(PLOADED_IMAGE LoadedImage)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: UnMapAndLoad not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           UnmapDebugInformation (IMAGEHLP.59)
 */

BOOL WINAPI UnmapDebugInformation(
  PIMAGE_DEBUG_INFORMATION DebugInfo)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: UnmapDebugInformation not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           UpdateDebugInfoFile (IMAGEHLP.60)
 */
BOOL WINAPI UpdateDebugInfoFile(
  LPSTR ImageFileName, LPSTR SymbolPath,
  LPSTR DebugFilePath, PIMAGE_NT_HEADERS NtHeaders)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: UpdateDebugInfoFile not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           UpdateDebugInfoFileEx (IMAGEHLP.?)
 * FIXME
 *   Function has no ordinal.
 */
BOOL WINAPI UpdateDebugInfoFileEx(
  LPSTR ImageFileName, LPSTR SymbolPath, LPSTR DebugFilePath,
  PIMAGE_NT_HEADERS NtHeaders, DWORD OldChecksum)
{
#ifdef DEBUG
  dprintf(("IMAGEHLP: UpdateDebugInfoFileEx not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

