/* $Id: modify.cpp,v 1.2 2000-08-02 14:56:26 bird Exp $ */
/*
 *	IMAGEHLP library
 *
 *	Copyright 1998	Patrik Stridvall
 */

#include <os2win.h>
#include <odinwrap.h>
#include <imagehlp.h>
#include <heapstring.h>

ODINDEBUGCHANNEL(imagehlp)

/***********************************************************************
 *		BindImage (IMAGEHLP.@)
 */
BOOL WINAPI BindImage(
  LPSTR ImageName, LPSTR DllPath, LPSTR SymbolPath)
{
  return BindImageEx(0, ImageName, DllPath, SymbolPath, NULL);
}

/***********************************************************************
 *		BindImageEx (IMAGEHLP.@)
 */
BOOL WINAPI BindImageEx(
  DWORD Flags, LPSTR ImageName, LPSTR DllPath, LPSTR SymbolPath,
  PIMAGEHLP_STATUS_ROUTINE StatusRoutine)
{
  dprintf(("BindImageEx: stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		CheckSumMappedFile (IMAGEHLP.@)
 */
PIMAGE_NT_HEADERS WINAPI CheckSumMappedFile(
  LPVOID BaseAddress, DWORD FileLength,
  LPDWORD HeaderSum, LPDWORD CheckSum)
{
  dprintf(("(%p, %ld, %p, %p): stub\n",
    BaseAddress, FileLength, HeaderSum, CheckSum
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}

/***********************************************************************
 *		MapFileAndCheckSumA (IMAGEHLP.@)
 */
DWORD WINAPI MapFileAndCheckSumA(
  LPSTR Filename, LPDWORD HeaderSum, LPDWORD CheckSum)
{
  dprintf(("MapFileAndCheckSumA: stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return CHECKSUM_OPEN_FAILURE;
}

/***********************************************************************
 *		MapFileAndCheckSumW (IMAGEHLP.@)
 */
DWORD WINAPI MapFileAndCheckSumW(
  LPWSTR Filename, LPDWORD HeaderSum, LPDWORD CheckSum)
{
  dprintf(("MapFileAndCheckSumW: stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return CHECKSUM_OPEN_FAILURE;
}

/***********************************************************************
 *		ReBaseImage (IMAGEHLP.@)
 */
BOOL WINAPI ReBaseImage(
  LPSTR CurrentImageName, LPSTR SymbolPath, BOOL fReBase,
  BOOL fRebaseSysfileOk, BOOL fGoingDown, ULONG CheckImageSize,
  ULONG *OldImageSize, ULONG *OldImageBase, ULONG *NewImageSize,
  ULONG *NewImageBase, ULONG TimeStamp)
{
  dprintf(("ReBaseImage: stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		RemovePrivateCvSymbolic (IMAGEHLP.@)
 */
BOOL WINAPI RemovePrivateCvSymbolic(
  PCHAR DebugData, PCHAR *NewDebugData, ULONG *NewDebugSize)
{
  dprintf(("(%p, %p, %p): stub\n",
    DebugData, NewDebugData, NewDebugSize
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		RemoveRelocations (IMAGEHLP.@)
 */
VOID WINAPI RemoveRelocations(PCHAR ImageName)
{
  dprintf(("(%p): stub\n", ImageName));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}

/***********************************************************************
 *		SplitSymbols (IMAGEHLP.@)
 */
BOOL WINAPI SplitSymbols(
  LPSTR ImageName, LPSTR SymbolsPath,
  LPSTR SymbolFilePath, DWORD Flags)
{
  dprintf(("SplitSymbols: stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		UpdateDebugInfoFile (IMAGEHLP.@)
 */
BOOL WINAPI UpdateDebugInfoFile(
  LPSTR ImageFileName, LPSTR SymbolPath,
  LPSTR DebugFilePath, PIMAGE_NT_HEADERS NtHeaders)
{
  dprintf(("UpdateDebugInfoFile: stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		UpdateDebugInfoFileEx (IMAGEHLP.@)
 */
BOOL WINAPI UpdateDebugInfoFileEx(
  LPSTR ImageFileName, LPSTR SymbolPath, LPSTR DebugFilePath,
  PIMAGE_NT_HEADERS NtHeaders, DWORD OldChecksum)
{
  dprintf(("UpdateDebugInfoFileEx: stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
