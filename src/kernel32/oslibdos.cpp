/* $Id: oslibdos.cpp,v 1.120 2003/06/02 16:25:19 sandervl Exp $ */
/*
 * Wrappers for OS/2 Dos* API
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999-2000 Edgar Buerkle (Edgar.Buerkle@gmx.net)
 * Copyright 2000 Przemyslaw Dobrowolski (dobrawka@asua.org.pl)
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
 * Copyright 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSDEVIOCTL
#define INCL_DOSDEVICES
#define INCL_NPIPES
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <win32api.h>
#include <winconst.h>
#include <win/winioctl.h>
#include <misc.h>
#include <odincrt.h>
#include "initterm.h"
#include "oslibdos.h"
#include "dosqss.h"
#include "win32k.h"

#define DBG_LOCALLOG    DBG_oslibdos
#include "dbglocal.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#ifndef DEVTYPE_OPTICAL
#define DEVTYPE_OPTICAL                    0x0008
#endif

#define IOC_CDROM_2                 0x82 /* from cdioctl.h (ddk, os2cdrom.dmd sample) */
#define IOCD_RETURN_DRIVE_LETTER    0x60


// used for input to logical disk Get device parms Ioctl
#pragma pack(1)
typedef struct
{
    UCHAR   Infotype;
    UCHAR   DriveUnit;
} DSKREQ;

/*------------------------------------------------*
 * Cat 0x82, Func 0x60:  Return Drive Letter Info *
 *------------------------------------------------*/
typedef struct DriveLetter_Data
{
    USHORT  drive_count;
    USHORT  first_drive_number;
} CDDRVLTR;
#pragma pack()


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static PROC_DosSetFileSizeL  DosSetFileSizeLProc = 0;
static PROC_DosSetFilePtrL   DosSetFilePtrLProc   = 0;
static PROC_DosSetFileLocksL DosSetFileLocksLProc = 0;
static PROC_DosOpenL         DosOpenLProc = 0;
static BOOL f64BitIO = FALSE;

/* first user queries the data */
static  CDDRVLTR    cdDrvLtr = {0xffff, 0xffff};

extern "C" {

/*******************************************************************************
*   Functions Prototypes.                                                      *
*******************************************************************************/
BOOL WINAPI CharToOemA( LPCSTR s, LPSTR d );
BOOL WINAPI OemToCharA( LPCSTR s, LPSTR d );

static ULONG sdbm(const char *str);
static ULONG crc32str(const char *psz);

LPCSTR ODINHelperStripUNC(LPCSTR strUNC)
{
    LPCSTR retStr = strUNC;

    if (!strUNC) return NULL;

    /* first possible case */
    if (strUNC[0] == '\\' &&
        (strUNC[1] == '?' || (strUNC[1] >= 'A' && strUNC[1] <= 'Z' )) &&
        strUNC[2] == '\\')
    {

        retStr = &strUNC[3];
    }
    /* second possible case */
    if (strUNC[0] == '\\' && strUNC[1] == '\\' &&
        (strUNC[2] == '?' || (strUNC[2] >= 'A' && strUNC[2] <= 'Z' )) &&
        strUNC[3] == '\\')
    {

        retStr = &strUNC[4];
    }
    return retStr;
}

//******************************************************************************
//******************************************************************************
void OSLibInitWSeBFileIO()
{
 HMODULE hDoscalls;

  if(DosQueryModuleHandleStrict("DOSCALLS", &hDoscalls) != NO_ERROR) {
    return;
  }
  if(DosQueryProcAddr(hDoscalls, 989, NULL, (PFN *)&DosSetFileSizeLProc) != NO_ERROR) {
    return;
  }
  if(DosQueryProcAddr(hDoscalls, 988, NULL, (PFN *)&DosSetFilePtrLProc) != NO_ERROR) {
    return;
  }
  if(DosQueryProcAddr(hDoscalls, 986, NULL, (PFN *)&DosSetFileLocksLProc) != NO_ERROR) {
    return;
  }
  if(DosQueryProcAddr(hDoscalls, 981, NULL, (PFN *)&DosOpenLProc) != NO_ERROR) {
    return;
  }
  f64BitIO = TRUE;
}
//******************************************************************************
//******************************************************************************
APIRET OdinDosSetFileSizeL(HFILE hFile, LONGLONG cbSize)
{
 APIRET yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFileSizeLProc(hFile, cbSize);
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
APIRET OdinDosSetFilePtrL(HFILE hFile, LONGLONG ib, ULONG method, PLONGLONG ibActual)
{
 APIRET yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFilePtrLProc(hFile, ib, method, ibActual);
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
APIRET OdinDosSetFileLocksL(HFILE hFile, PFILELOCKL pflUnlock, PFILELOCKL pflLock,
                            ULONG timeout, ULONG flags)
{
 APIRET yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = DosSetFileLocksLProc(hFile, pflUnlock, pflLock, timeout, flags);
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
//******************************************************************************
APIRET APIENTRY OdinDosOpenL(PCSZ  pszFileName,
                             PHFILE phf,
                             PULONG pulAction,
                             LONGLONG cbFile,
                             ULONG ulAttribute,
                             ULONG fsOpenFlags,
                             ULONG fsOpenMode,
                             PEAOP2 peaop2)
{
 APIRET yyrc;
 USHORT sel = RestoreOS2FS();
 char OemFileName[260];
 CharToOemA(ODINHelperStripUNC((char*)pszFileName), OemFileName);

    if(DosOpenLProc) {
        yyrc = DosOpenLProc(OemFileName, phf, pulAction, cbFile, ulAttribute, fsOpenFlags,
                            fsOpenMode, peaop2);
    }
    else yyrc = DosOpen(OemFileName, phf, pulAction, cbFile.ulLo, ulAttribute, fsOpenFlags,
                        fsOpenMode, peaop2);
    SetFS(sel);

    return yyrc;
}
//******************************************************************************
// translate OS/2 error codes to Windows codes
// NOTE: add all codes you need, list is not complete!
//******************************************************************************
DWORD error2WinError(APIRET rc,DWORD defaultCode)
{
  switch (rc)
  {
    case NO_ERROR: //0
        return ERROR_SUCCESS_W;

    case ERROR_INVALID_FUNCTION: //1
        return ERROR_INVALID_FUNCTION_W;

    case ERROR_FILE_NOT_FOUND: //2
        return ERROR_FILE_NOT_FOUND_W;

    case ERROR_PATH_NOT_FOUND: //3
        return ERROR_PATH_NOT_FOUND_W;

    case ERROR_TOO_MANY_OPEN_FILES: //4
        return ERROR_TOO_MANY_OPEN_FILES_W;

    case ERROR_ACCESS_DENIED: //5
        return ERROR_ACCESS_DENIED_W;

    case ERROR_INVALID_HANDLE: //6
        return ERROR_INVALID_HANDLE_W;

    case ERROR_NOT_ENOUGH_MEMORY: //8
        return ERROR_NOT_ENOUGH_MEMORY_W;

    case ERROR_BAD_FORMAT: //11
        return ERROR_BAD_FORMAT_W;

    case ERROR_INVALID_ACCESS: //12
        return ERROR_INVALID_ACCESS_W;

    case ERROR_INVALID_DATA: //13
      return ERROR_INVALID_DATA_W;

    case ERROR_INVALID_DRIVE: //15
      return ERROR_INVALID_DRIVE_W;

    case ERROR_CURRENT_DIRECTORY: // 16
        return ERROR_CURRENT_DIRECTORY_W;

    case ERROR_NO_MORE_FILES: //18
        return ERROR_NO_MORE_FILES_W;

    case ERROR_WRITE_PROTECT: //19
        return ERROR_WRITE_PROTECT_W;

    case ERROR_BAD_UNIT: //20
        return ERROR_BAD_UNIT_W;

    case ERROR_NOT_READY: //21
        return ERROR_NOT_READY_W;

    case ERROR_CRC: //23
        return ERROR_CRC_W;

    case ERROR_SEEK:
        return ERROR_SEEK_W;

    case ERROR_NOT_DOS_DISK: //26
        return ERROR_NOT_DOS_DISK_W;

    case ERROR_WRITE_FAULT: //29
        return ERROR_WRITE_FAULT_W;

    case ERROR_GEN_FAILURE: //31
        return ERROR_GEN_FAILURE_W;

    case ERROR_SHARING_VIOLATION: //32
        return ERROR_SHARING_VIOLATION_W;

    case ERROR_LOCK_VIOLATION: //32
        return ERROR_LOCK_VIOLATION_W;

    case ERROR_WRONG_DISK: //34
        return ERROR_WRONG_DISK_W;

    case ERROR_SHARING_BUFFER_EXCEEDED: //36
        return ERROR_SHARING_BUFFER_EXCEEDED_W;

    case ERROR_BAD_NETPATH: //53
        return ERROR_BAD_NETPATH_W;

    case ERROR_CANNOT_MAKE: //82
        return ERROR_CANNOT_MAKE_W;

    case ERROR_OUT_OF_STRUCTURES: //84
        return ERROR_OUT_OF_STRUCTURES_W;

    case ERROR_INVALID_PARAMETER: //87
        return ERROR_INVALID_PARAMETER_W;

    case ERROR_INTERRUPT: //95
        return ERROR_INVALID_AT_INTERRUPT_TIME_W; //CB: right???

    case ERROR_DEVICE_IN_USE: //99
        return ERROR_DEVICE_IN_USE_W;

    case ERROR_TOO_MANY_SEMAPHORES: //100
        return ERROR_TOO_MANY_SEMAPHORES_W;

    case ERROR_DISK_CHANGE: // 107
        return ERROR_DISK_CHANGE_W;

    case ERROR_DRIVE_LOCKED: //108
        return ERROR_DRIVE_LOCKED_W;

    case ERROR_BROKEN_PIPE: //109
        return ERROR_BROKEN_PIPE_W;

    case ERROR_OPEN_FAILED: //110
        return ERROR_OPEN_FAILED_W;

    case ERROR_BUFFER_OVERFLOW: //111
        return ERROR_BUFFER_OVERFLOW_W;

    case ERROR_DISK_FULL: //112
        return ERROR_DISK_FULL_W;

    case ERROR_NO_MORE_SEARCH_HANDLES: //113
        return ERROR_NO_MORE_SEARCH_HANDLES_W;

    case ERROR_SEM_TIMEOUT: //121
        return ERROR_SEM_TIMEOUT_W;

    case ERROR_INVALID_NAME:
        return ERROR_INVALID_NAME_W;

    case ERROR_DIRECT_ACCESS_HANDLE: //130
        return ERROR_DIRECT_ACCESS_HANDLE_W;

    case ERROR_NEGATIVE_SEEK: //131
        return ERROR_NEGATIVE_SEEK;

    case ERROR_SEEK_ON_DEVICE: //132
        return ERROR_SEEK_ON_DEVICE_W;

    case ERROR_DISCARDED: //157
        return ERROR_DISCARDED_W;

    case ERROR_INVALID_MODULETYPE:
        return ERROR_INVALID_MODULETYPE_W;

    case ERROR_INVALID_EXE_SIGNATURE: // 191
        return ERROR_INVALID_EXE_SIGNATURE_W;

    case ERROR_EXE_MARKED_INVALID: // 192
        return ERROR_EXE_MARKED_INVALID_W;

    case ERROR_BAD_EXE_FORMAT: // 193
        return ERROR_BAD_EXE_FORMAT_W;

    case ERROR_FILENAME_EXCED_RANGE: //206
        return ERROR_FILENAME_EXCED_RANGE_W;

    case ERROR_META_EXPANSION_TOO_LONG: //208
        return ERROR_META_EXPANSION_TOO_LONG_W;

    case ERROR_BAD_PIPE: //230
        return ERROR_BAD_PIPE_W;

    case ERROR_PIPE_BUSY: //231
        return ERROR_PIPE_BUSY_W;

    case ERROR_NO_DATA: //232
        return ERROR_NO_DATA_W;

    case ERROR_PIPE_NOT_CONNECTED: //233
        return ERROR_PIPE_NOT_CONNECTED_W;

    case ERROR_MORE_DATA: //234
        return ERROR_MORE_DATA_W;

    case ERROR_INVALID_PATH: //253
        return ERROR_INVALID_NAME_W;

    case ERROR_INVALID_EA_NAME: //254
        return ERROR_INVALID_EA_NAME_W;

    case ERROR_EA_LIST_INCONSISTENT: //255
        return ERROR_EA_LIST_INCONSISTENT_W;

    case ERROR_EAS_DIDNT_FIT: //275
        return ERROR_EAS_DIDNT_FIT;

    case ERROR_INIT_ROUTINE_FAILED: //295
        return ERROR_DLL_INIT_FAILED_W;

    // device driver specific error codes (I24)
    case ERROR_USER_DEFINED_BASE + ERROR_I24_WRITE_PROTECT:
       return ERROR_WRITE_PROTECT_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_BAD_UNIT:
       return ERROR_BAD_UNIT_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_NOT_READY:
       return ERROR_NOT_READY_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_BAD_COMMAND:
       return ERROR_BAD_COMMAND_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_CRC:
       return ERROR_CRC_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_BAD_LENGTH:
       return ERROR_BAD_LENGTH_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_SEEK:
       return ERROR_SEEK_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_NOT_DOS_DISK:
       return ERROR_NOT_DOS_DISK_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_SECTOR_NOT_FOUND:
       return ERROR_SECTOR_NOT_FOUND_W;

// @@@PH this error code is not defined in winconst.h
//    case ERROR_USER_DEFINED_BASE + ERROR_I24_OUT_OF_PAPER:
//       return 28;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_WRITE_FAULT:
       return ERROR_WRITE_FAULT_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_READ_FAULT:
       return ERROR_READ_FAULT_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_GEN_FAILURE:
       return ERROR_GEN_FAILURE_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_DISK_CHANGE:
       return ERROR_DISK_CHANGE_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_WRONG_DISK:
       return ERROR_WRONG_DISK_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_INVALID_PARAMETER:
       return ERROR_INVALID_PARAMETER_W;

    case ERROR_USER_DEFINED_BASE + ERROR_I24_DEVICE_IN_USE:
       return ERROR_DEVICE_IN_USE_W;

    default:
        dprintf(("WARNING: error2WinError: error %d not included!!!!", rc));
        return defaultCode;
  }
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosOpen(LPCSTR lpszFileName, DWORD flags)
{
 APIRET rc;
 HFILE  hFile;
 ULONG  ulAction;
 DWORD  os2flags = OPEN_FLAGS_NOINHERIT; //default is not inherited by child processes
 char lOemFileName[260];

  CharToOemA(ODINHelperStripUNC(lpszFileName), lOemFileName);

  if(flags & OSLIB_ACCESS_READONLY)
        os2flags |= OPEN_ACCESS_READONLY;
  else
  if(flags & OSLIB_ACCESS_READWRITE)
        os2flags |= OPEN_ACCESS_READWRITE;

  if(flags & OSLIB_ACCESS_SHAREDENYNONE)
        os2flags |= OPEN_SHARE_DENYNONE;
  else
  if(flags & OSLIB_ACCESS_SHAREDENYREAD)
        os2flags |= OPEN_SHARE_DENYREAD;
  else
  if(flags & OSLIB_ACCESS_SHAREDENYWRITE)
        os2flags |= OPEN_SHARE_DENYWRITE;

tryopen:
  rc = DosOpen(lOemFileName,                     /* File path name */
               &hFile,                   /* File handle */
               &ulAction,                      /* Action taken */
               0L,                             /* File primary allocation */
               0L,                     /* File attribute */
               OPEN_ACTION_FAIL_IF_NEW |
               OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
               os2flags,
               0L);                            /* No extended attribute */
  if(rc)
  {
    if(rc == ERROR_TOO_MANY_OPEN_FILES)
    {
      ULONG CurMaxFH;
      LONG  ReqCount = 32;

      rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
      if(rc)
      {
        dprintf(("DosSetRelMaxFH returned %d", rc));
        return 0;
      }
      dprintf(("DosOpen failed -> increased nr open files to %d", CurMaxFH));
      goto tryopen;
    }

    SetLastError(error2WinError(rc));
    return 0;
  }

  // OK, file was opened
  SetLastError(ERROR_SUCCESS_W);
  return hFile;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosClose(DWORD hFile)
{
 APIRET rc;

  rc = DosClose(hFile);
  SetLastError(error2WinError(rc));
  return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosCopyFile(LPCSTR lpszOldFile, LPCSTR lpszNewFile, BOOL fFailIfExist)
{
 APIRET rc;
 char lOemOldFile[260], lOemNewFile[260];

  CharToOemA(ODINHelperStripUNC((char*)lpszOldFile), lOemOldFile);
  CharToOemA(ODINHelperStripUNC((char*)lpszNewFile), lOemNewFile);

  rc = DosCopy((PSZ)lOemOldFile, (PSZ)lOemNewFile, fFailIfExist ? 0: DCPY_EXISTING);
  SetLastError(error2WinError(rc));
  return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosMoveFile(LPCSTR lpszOldFile, LPCSTR lpszNewFile)
{
 APIRET rc;
 char lOemOldFile[260], lOemNewFile[260];

  CharToOemA(ODINHelperStripUNC((char*)lpszOldFile), lOemOldFile);
  CharToOemA(ODINHelperStripUNC((char*)lpszNewFile), lOemNewFile);

  // we need to know the current drive for relative paths
  ULONG diskNum, logicalBitmap;
  DosQueryCurrentDisk(&diskNum, &logicalBitmap);
  char currentDrive = 'A' + diskNum - 1;

  // first determine whether source and target paths are absolute
  // (i.e. contain a drive letter)
  BOOL sourcePathAbsolute = FALSE;
  char sourceDrive = currentDrive;
  if ((strlen(lOemOldFile) > 2) && (lOemOldFile[1] == ':'))
  {
      sourcePathAbsolute = TRUE;
      sourceDrive = toupper(lOemOldFile[0]);
  }
  BOOL targetPathAbsolute = FALSE;
  char targetDrive = currentDrive;
  if ((strlen(lOemNewFile) > 2) && (lOemNewFile[1] == ':'))
  {
      targetPathAbsolute = TRUE;
      targetDrive = toupper(lOemNewFile[0]);
  }

  // if the source and target drives are different, we have to do take
  // the copy and delete path
  if (sourceDrive != targetDrive)
  {
      dprintf(("OSLibDosMoveFile - different drives, doing the copy/delete approach (WARNING: non atomic file system operation!)"));
      // @@@AH TODO: this is not atomic. Maybe it is on Windows - we have to check this
      rc = DosCopy((PSZ)lOemOldFile, (PSZ)lOemNewFile, 0);
      if (rc == NO_ERROR)
      {
          rc = DosDelete(lOemOldFile);
      }
      SetLastError(error2WinError(rc));
  } else
  {
      // atomic DosMove will do fine
      rc = DosMove((PSZ)lOemOldFile, (PSZ)lOemNewFile);
      SetLastError(error2WinError(rc));
  }
  return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosRemoveDir(LPCSTR lpszDir)
{
 APIRET rc;
 char lOemDir[260];

  CharToOemA(ODINHelperStripUNC((char*)lpszDir), lOemDir);

  rc = DosDeleteDir((PSZ)lOemDir);
  SetLastError(error2WinError(rc));
  return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosCreateDirectory(LPCSTR lpszDir)
{
 APIRET rc;
 char lOemDir[260];

  CharToOemA(ODINHelperStripUNC((char*)lpszDir), lOemDir);

  rc = DosCreateDir((PSZ)lOemDir, NULL);
  SetLastError(error2WinError(rc));
  return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosChangeMaxFileHandles()
{
 ULONG  CurMaxFH;
 LONG   ReqCount = 0;
 APIRET rc;

        rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
        if(rc) {
                dprintf(("DosSetRelMaxFH returned %d", rc));
                return rc;
        }
        if(ReqCount + ODIN_INCREMENT_MAX_FILEHANDLES > CurMaxFH) {
                ReqCount = CurMaxFH + ODIN_INCREMENT_MAX_FILEHANDLES;
                rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
                if(rc) {
                        dprintf(("DosSetRelMaxFH returned %d", rc));
                        return rc;
                }
        }
        return 0;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetInitialMaxFileHandles(DWORD maxhandles)
{
 ULONG  CurMaxFH;
 LONG   ReqCount = 0;
 APIRET rc;

        rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
        if(rc) {
                dprintf(("DosSetRelMaxFH returned %d", rc));
                return rc;
        }
        if(CurMaxFH < maxhandles) {
                rc = DosSetMaxFH(maxhandles);
                if(rc) {
                        dprintf(("DosSetMaxFH returned %d", rc));
                        return rc;
                }
        }
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosRead(DWORD hFile, LPVOID lpBuffer, DWORD size, DWORD *nrBytesRead)
{
 APIRET rc;

  rc = DosRead(hFile, lpBuffer, size, nrBytesRead);
  SetLastError(error2WinError(rc));
  return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosWrite(DWORD hFile, LPVOID lpBuffer, DWORD size, DWORD *nrBytesWritten)
{
 APIRET rc;

  rc = DosWrite(hFile, lpBuffer, size, nrBytesWritten);
  SetLastError(error2WinError(rc));
  return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetFilePtr(DWORD hFile, DWORD offset, DWORD method)
{
 DWORD  os2method;
 DWORD  newoffset;
 APIRET rc;

  switch(method)
  {
    case OSLIB_SETPTR_FILE_CURRENT:
      os2method = FILE_CURRENT;
      break;
    case OSLIB_SETPTR_FILE_BEGIN:
      os2method = FILE_BEGIN  ;
      break;
    case OSLIB_SETPTR_FILE_END:
      os2method = FILE_END;
      break;
    default:
      return OSLIB_ERROR_INVALID_PARAMETER;
  }

  rc = DosSetFilePtr(hFile, offset, os2method, &newoffset);
  if(rc)
  {
      SetLastError(error2WinError(rc));
      return -1;
  }
  SetLastError(ERROR_SUCCESS_W);
  return newoffset;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosDelete(LPCSTR lpszFileName)
{
    APIRET rc;
    char lOemFileName[260];

    CharToOemA(ODINHelperStripUNC(lpszFileName), lOemFileName);

    rc = DosDelete(lOemFileName);
    if(rc) {
        SetLastError(error2WinError(rc));
        return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL pmDateTimeToFileTime(FDATE *pDate,FTIME *pTime,FILETIME *pFT)
{
    USHORT   dosTime, dosDate;
    BOOL     ret;
    FILETIME dummy;

    dosTime = *(USHORT*)pTime;
    dosDate = *(USHORT*)pDate;

    //time we get from OS2 is local time; win32 expects file time (UTC)
    ret = DosDateTimeToFileTime(dosDate, dosTime, &dummy);
    if(ret) ret = LocalFileTimeToFileTime(&dummy, pFT);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL pmFileTimeToDateTime(FILETIME *pFT, FDATE *pDate, FTIME *pTime)
{
    BOOL 	ret;
    FILETIME dummy;
    //time we get from win32 is file time (UTC); OS2 expects local time
    ret = FileTimeToLocalFileTime(pFT, &dummy);
    if (ret) ret = FileTimeToDosDateTime(&dummy, (WORD*)pDate, (WORD*)pTime);
    return ret;
}
//******************************************************************************
//******************************************************************************
#define NOT_NORMAL (FILE_ATTRIBUTE_READONLY_W | \
                    FILE_ATTRIBUTE_HIDDEN_W |   \
                    FILE_ATTRIBUTE_SYSTEM_W |   \
                    FILE_ATTRIBUTE_ARCHIVE_W)

inline DWORD pm2WinFileAttributes(DWORD attrFile)
{
  DWORD res = 0;

  if (!(attrFile & NOT_NORMAL))
    res |= FILE_ATTRIBUTE_NORMAL_W;
  if (attrFile & FILE_READONLY)
    res |= FILE_ATTRIBUTE_READONLY_W;
  if (attrFile & FILE_HIDDEN)
    res |= FILE_ATTRIBUTE_HIDDEN_W;
  if (attrFile & FILE_SYSTEM)
    res |= FILE_ATTRIBUTE_SYSTEM_W;
  if (attrFile & FILE_DIRECTORY)
    res |= FILE_ATTRIBUTE_DIRECTORY_W;
  if (attrFile & FILE_ARCHIVED)
    res |= FILE_ATTRIBUTE_ARCHIVE_W;

  //CB: not used: FILE_ATTRIBUTE_COMPRESSED_W
  //PH: NT server will serve appropriate sizes for compressed files
  //    over network. So if realSize < allocatedSize, the file must
  //    be compressed.

  return res;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosGetFileAttributesEx(PSZ   pszName,
                                 ULONG ulDummy,
                                 PVOID pBuffer)
{
  APIRET      rc;                                         /* API return code */
  FILESTATUS3 fs3;                             /* file information structure */
  LPWIN32_FILE_ATTRIBUTE_DATA lpFad = (LPWIN32_FILE_ATTRIBUTE_DATA) pBuffer;
  char        pszOemName[CCHMAXPATH];

  //Convert file name from Windows to OS/2 codepage
  CharToOemA(ODINHelperStripUNC(pszName), pszOemName);

  // Treat 'x:' as 'x:\' and 'some\path\' as 'some\path' -- this is what
  // GetFileAttributesEx() does on Win32 -- instead of returning an error as
  // DosQueryPathInfo() does
  int len = strlen(pszOemName);
  if (len == 2 && pszOemName[1] == ':')
  {
      pszOemName[2] = '\\';
      pszOemName[3] = '\0';
  }
  else if (len > 1 && (len != 3 || pszOemName[1] != ':'))
  {
      // note: len > 1 above leaves '\' (i.e. the root dir) as is
      // and the second condition prevents 'x:\' from being cut
      while (pszOemName[len-1] == '\\' || pszOemName[len-1] == '/')
          --len;
      pszOemName[len] = '\0';
  }

  // Note: we only handle standard "GetFileExInfoStandard" requests
  rc = DosQueryPathInfo(pszOemName,            /* query the file information */
                        FIL_STANDARD,
                        &fs3,
                        sizeof(fs3));
  if (rc != NO_ERROR)                                    /* check for errors */
    return FALSE;                                   /* raise error condition */

  // convert structure
  lpFad->dwFileAttributes = pm2WinFileAttributes(fs3.attrFile);
  pmDateTimeToFileTime(&fs3.fdateCreation,   &fs3.ftimeCreation,   &lpFad->ftCreationTime);
  pmDateTimeToFileTime(&fs3.fdateLastAccess, &fs3.ftimeLastAccess, &lpFad->ftLastAccessTime);
  pmDateTimeToFileTime(&fs3.fdateLastWrite,  &fs3.ftimeLastWrite,  &lpFad->ftLastWriteTime);

  /* @@@PH we might add Aurora support ...
  lpFad->nFileSizeHigh    = info.nFileSizeHigh;
  */
  lpFad->nFileSizeHigh    = 0;
  lpFad->nFileSizeLow     = fs3.cbFile;

  return TRUE;
}
//******************************************************************************
DWORD WIN32API GetEnvironmentVariableA(LPCSTR, LPSTR, DWORD );
//******************************************************************************
DWORD OSLibDosSearchPath(DWORD cmd, LPCSTR path, LPCSTR name,
                         LPSTR full_name, DWORD length_fullname)
{
  switch(cmd) {
  case OSLIB_SEARCHDIR:
        if(DosSearchPath(SEARCH_IGNORENETERRS, ODINHelperStripUNC(path),
                         name, full_name, length_fullname) != 0) {
                return 0;
        }
        return strlen(full_name);


  case OSLIB_SEARCHCURDIR:
        if(DosSearchPath(SEARCH_IGNORENETERRS | SEARCH_CUR_DIRECTORY, ODINHelperStripUNC(path),
                         name, full_name, length_fullname) != 0) {
                return 0;
        }
        return strlen(full_name);

  case OSLIB_SEARCHFILE:
  {
    FILESTATUS3 fileinfo;

        if(DosQueryPathInfo(ODINHelperStripUNC(name), FIL_STANDARD, &fileinfo, sizeof(fileinfo)) != 0) {
                return 0;
        }
        strncpy(full_name, name, length_fullname);
  full_name[length_fullname-1] = 0;
        return strlen(full_name);
  }

  case OSLIB_SEARCHENV:
  {
   LPSTR envstring;
   int   envsize;
   CHAR  szResult[CCHMAXPATH];

        envsize = GetEnvironmentVariableA(path, NULL, 0);
        envstring = (LPSTR)malloc(envsize+1);
        GetEnvironmentVariableA(path, envstring, envsize);
        if(DosSearchPath(SEARCH_IGNORENETERRS, envstring,
                         name, szResult, sizeof(szResult)) != 0) {
                free(envstring);
                return 0;
        }
        free(envstring);
        strcpy(full_name, szResult);
        return strlen(full_name);
  }
  }
  return 0;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosQueryPathInfo(CHAR *pszPathName,
                            ULONG ulInfoLevel,
                            PVOID pInfoBuf,
                            ULONG cbInfoBuf)
{
    APIRET      rc;
    char        pszOemPathName[CCHMAXPATH];

    //Convert file name from Windows to OS/2 codepage
    CharToOemA(ODINHelperStripUNC(pszPathName), pszOemPathName);

    rc = DosQueryPathInfo(pszOemPathName, ulInfoLevel,
                          pInfoBuf, cbInfoBuf);

    if(rc == ERROR_TOO_MANY_OPEN_FILES)
    {
        LONG  reqCount = 2;
        ULONG maxFiles;

        if(DosSetRelMaxFH(&reqCount, &maxFiles) == NO_ERROR)
          rc = DosQueryPathInfo(pszOemPathName, ulInfoLevel,
                                pInfoBuf, cbInfoBuf);
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosCreateFile(CHAR *lpszFile,
                         DWORD fuAccess,
                         DWORD fuShare,
                         LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                         DWORD fuCreate,
                         DWORD fuAttrFlags,
                         HANDLE hTemplateFile)
{
   HFILE   hFile;
   ULONG   actionTaken = 0;
   LONGLONG fileSize = {0};
   ULONG   fileAttr = FILE_NORMAL;
   ULONG   openFlag = 0;
   ULONG   openMode = OPEN_FLAGS_NOINHERIT; //default is not inherited by child processes
   APIRET  rc = ERROR_NOT_ENOUGH_MEMORY;

   LPCSTR lpszFileLoc = ODINHelperStripUNC(lpszFile);

   //TODO: lpSecurityAttributes (inheritance)

   if(fuAttrFlags & FILE_ATTRIBUTE_ARCHIVE_W)
        fileAttr |= FILE_ARCHIVED;
   if(fuAttrFlags & FILE_ATTRIBUTE_HIDDEN_W)
        fileAttr |= FILE_HIDDEN;
   if(fuAttrFlags & FILE_ATTRIBUTE_SYSTEM_W)
        fileAttr |= FILE_SYSTEM;
   if(fuAttrFlags & FILE_ATTRIBUTE_READONLY_W)
        fileAttr |= FILE_READONLY;
   // TODO: FILE_ATTRIBUTE_TEMPORARY_W

   switch(fuCreate)
   {
   case CREATE_NEW_W:
        openFlag |= OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS;
        break;
   case CREATE_ALWAYS_W:
       /* kso 2000-11-26: Not sure if OPEN_ACTION_REPLACE_IF_EXISTS is correct here! It is correct according to
        *   MSDN, but not according to "The Win32 API SuperBible". Anyway I haven't got time to check it out in
        *   NT now.
        *   The problem is that OPEN_ACTION_REPLACE_IF_EXISTS requires write access. It failes with
        *   rc = ERROR_ACCESS_DENIED (5). Quick fix, use OPEN_IF_EXIST if readonly access.
        */
       if (fuAccess & GENERIC_WRITE_W)
           openFlag |= OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS;
       else
           openFlag |= OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        break;
   case OPEN_EXISTING_W:
        openFlag |= OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        break;
   case OPEN_ALWAYS_W:
        openFlag |= OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        break;
   case TRUNCATE_EXISTING_W:
        openFlag |= OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS;
        break;
   }

   if(fuAttrFlags & FILE_FLAG_WRITE_THROUGH_W)   openMode |= OPEN_FLAGS_WRITE_THROUGH;
   if(fuAttrFlags & FILE_FLAG_NO_BUFFERING_W)    openMode |= OPEN_FLAGS_NO_CACHE;
   if(fuAttrFlags & FILE_FLAG_RANDOM_ACCESS_W)   openMode |= OPEN_FLAGS_RANDOM;
   if(fuAttrFlags & FILE_FLAG_SEQUENTIAL_SCAN_W) openMode |= OPEN_FLAGS_SEQUENTIAL;
   // TODO: FILE_FLAG_BACKUP_SEMANTICS_W
   //       FILE_FLAG_POSIX_SEMANTICS_W are not supported

   //TODO: FILE_SHARE_DELETE
   if((fuShare & (FILE_SHARE_READ_W | FILE_SHARE_WRITE_W)) == 0 )
        openMode |= OPEN_SHARE_DENYREADWRITE;
   else
   if((fuShare & (FILE_SHARE_READ_W | FILE_SHARE_WRITE_W)) == (FILE_SHARE_READ_W | FILE_SHARE_WRITE_W))
        openMode |= OPEN_SHARE_DENYNONE;
   else
   if(fuShare & FILE_SHARE_READ_W)
        openMode |= OPEN_SHARE_DENYWRITE;
   else
   if(fuShare & FILE_SHARE_WRITE_W)
        openMode |= OPEN_SHARE_DENYREAD;

   if(fuAccess == (GENERIC_READ_W | GENERIC_WRITE_W))
        openMode |= OPEN_ACCESS_READWRITE;
   else
   if(fuAccess & GENERIC_READ_W)
        openMode |= OPEN_ACCESS_READONLY;
   else
   //mgp: There seems to be a problem where OPEN_ACCESS_WRITEONLY gives an
   //     Access Error (0x05) if the file is opened with
   //     OPEN_ACTION_OPEN_IF_EXISTS.  So, in that case, change it to
   //     OPEN_ACCESS_READWRITE
   if(fuAccess & GENERIC_WRITE_W)
      if  (openFlag & OPEN_ACTION_OPEN_IF_EXISTS)
        openMode |= OPEN_ACCESS_READWRITE;
      else
         openMode |= OPEN_ACCESS_WRITEONLY;

#if 0
   //SvL: Not true; verified in NT! (also messed up access of files on
   //     readonly volumes)
   //     CreateFile with OPEN_ALWAYS & GENERIC_READ on non-existing file
   //     -> creates 0 size file, WriteFile is not allowed
   //     Same in OS/2.
   /* if creating a file, access cannot be readonly! */
   if (openFlag & OPEN_ACTION_CREATE_IF_NEW &&
       (!(openMode & OPEN_ACCESS_READWRITE) &&
        !(openMode & OPEN_ACCESS_WRITEONLY))) {
        openMode |= OPEN_ACCESS_READWRITE;
   }
#endif

   if(strlen(lpszFileLoc) == 2 && lpszFileLoc[1] == ':') {
        //app tries to open logical volume/partition
        openMode |= OPEN_FLAGS_DASD;
   }

   int retry = 0;
   while (retry < 3)
   {
        dprintf(("DosOpen %s openFlag=%x openMode=%x", lpszFileLoc, openFlag, openMode));
        rc = OdinDosOpenL((PSZ)lpszFileLoc,
                           &hFile,
                           &actionTaken,
                           fileSize,
                           fileAttr,
                           openFlag,
                           openMode,
                           NULL);
    if (rc == ERROR_TOO_MANY_OPEN_FILES)
    {
        ULONG CurMaxFH;
        LONG  ReqCount = 32;

        rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
        if(rc) {
            dprintf(("DosSetRelMaxFH returned %d", rc));
            SetLastError(ERROR_TOO_MANY_OPEN_FILES_W);
            return INVALID_HANDLE_VALUE_W;
        }
        dprintf(("DosOpen failed -> increased nr open files to %d", CurMaxFH));
    }
    #if 0
    else if (rc == ERROR_ACCESS_DENIED && (openFlag & OPEN_ACTION_REPLACE_IF_EXISTS))
    {   /* kso: I have great problems with the REPLACE not working
         *      So, I guess this emulation may help...
         *      This problem exist on WS4eB SMP FP1 and Warp4 FP14/Kernel 14059 at least.
         */
        rc = DosOpen((PSZ)lOemFile,
                      &hFile,
                      &actionTaken,
                      fileSize,
                      fileAttr,
                      (openFlag & ~OPEN_ACTION_REPLACE_IF_EXISTS) | OPEN_ACTION_OPEN_IF_EXISTS,
                      openMode,
                      NULL);
        if (rc == NO_ERROR)
        {
            rc = DosSetFileSize(hFile, 0);
            if (!rc && fileSize > 0)
                rc = DosSetFileSize(hFile, fileSize);
            if (rc)
            {
                DosClose(hFile);
                hFile = NULLHANDLE;

                if (rc != ERROR_TOO_MANY_OPEN_FILES)
                    break;
            }
            else break;
        }
    }
    #endif
    else break;
    retry++;
   }

   if(rc)
   {
      // @@@AH 2001-06-02 Win2k SP2 returns error 2 in this case
      int winError = error2WinError(rc);
      if (winError == ERROR_OPEN_FAILED_W || winError == ERROR_PATH_NOT_FOUND_W)
      {
          //Windows returns ERROR_FILE_EXISTS if create new & file exists
          if(fuCreate == CREATE_NEW_W) {
                winError = ERROR_FILE_EXISTS_W;
          }
          else  winError = ERROR_FILE_NOT_FOUND_W;
      }
      SetLastError(winError);
      return INVALID_HANDLE_VALUE_W;
   }
   SetLastError(ERROR_SUCCESS_W);
   return hFile;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosOpenFile(CHAR *lpszFile, UINT fuMode)
{
   ULONG   actionTaken = 0;
   LONGLONG fileSize = {0};
   ULONG   fileAttr = FILE_NORMAL;
   ULONG   openFlag = 0;
   ULONG   openMode = OPEN_FLAGS_NOINHERIT; //default is not inherited by child processes
   APIRET  rc = ERROR_NOT_ENOUGH_MEMORY;
   HFILE   hFile;

   if(!(fuMode & (OF_CREATE_W | OF_READWRITE_W | OF_WRITE_W)))
   {
        openMode |= OPEN_ACCESS_READONLY;
        openFlag |= OPEN_ACTION_OPEN_IF_EXISTS;
   }
   else
   {
        if(fuMode & OF_CREATE_W) {
            openFlag |= OPEN_ACTION_CREATE_IF_NEW |
                        OPEN_ACTION_REPLACE_IF_EXISTS;
   }
   else openFlag |= OPEN_ACTION_OPEN_IF_EXISTS;

   if(fuMode & OF_READWRITE_W)
        openMode |= OPEN_ACCESS_READWRITE;
   else
   if(fuMode & OF_WRITE_W)
        openMode |= OPEN_ACCESS_WRITEONLY;
   else
   if(fuMode & OF_CREATE_W)
        openMode |= OPEN_ACCESS_READWRITE;
   }

   if((fuMode & OF_SHARE_DENY_WRITE_W) ||
      !(fuMode & (OF_SHARE_DENY_READ_W | OF_SHARE_DENY_NONE_W | OF_SHARE_EXCLUSIVE_W)))
        openMode |= OPEN_SHARE_DENYWRITE;
   else
   if (fuMode & OF_SHARE_DENY_NONE_W)
        openMode |= OPEN_SHARE_DENYNONE;
   else
   if (fuMode & OF_SHARE_DENY_READ_W)
        openMode |= OPEN_SHARE_DENYREAD;
   else
   if (fuMode & OF_SHARE_EXCLUSIVE_W)
        openMode |= OPEN_SHARE_DENYREADWRITE;

   rc = OdinDosOpenL((PSZ)lpszFile,
                     &hFile,
                     &actionTaken,
                     fileSize,
                     fileAttr,
                     openFlag,
                     openMode,
                     NULL);

   if(rc != NO_ERROR)
   {
        if(fuMode & OF_EXIST_W)
        {
            if(rc == ERROR_OPEN_FAILED || rc == ERROR_FILE_NOT_FOUND)
            {
                SetLastError(ERROR_FILE_NOT_FOUND_W);
                return HFILE_ERROR_W;
            }
        }
        if((rc == ERROR_OPEN_FAILED) && (openFlag & OPEN_ACTION_OPEN_IF_EXISTS))
        {
             SetLastError(ERROR_FILE_NOT_FOUND_W);
        }
        else SetLastError(error2WinError(rc));

        return HFILE_ERROR_W;
   }
   SetLastError(ERROR_SUCCESS_W);
   return hFile;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosLockFile(DWORD hFile, DWORD dwFlags,
                      DWORD OffsetLow, DWORD OffsetHigh,
                      DWORD nNumberOfBytesToLockLow, DWORD nNumberOfBytesToLockHigh,
                      LPOVERLAPPED lpOverlapped)
{
  APIRET   rc;

   // Set 5 secs timeout for locking file and no other can access this
   // file region

   if(lpOverlapped) {//TODO:
        dprintf(("OSLibDosLockFile: overlapped lock not yet implemented!!"));
   }
   //TODO: Locking region crossing end of file is permitted. Works in OS/2??
   if(f64BitIO)
   {
        FILELOCKL lockRangeL;

        lockRangeL.lOffset.ulLo = OffsetLow;
        lockRangeL.lOffset.ulHi = OffsetHigh;
        lockRangeL.lRange.ulLo  = nNumberOfBytesToLockLow;
        lockRangeL.lRange.ulHi  = nNumberOfBytesToLockHigh;

        rc = OdinDosSetFileLocksL(hFile, NULL, &lockRangeL,
                                  (dwFlags & LOCKFILE_FAIL_IMMEDIATELY_W) ? 0 : 5000, 0);
        //SvL: 64 bits values are only supported by JFS
        //     Try the 32 bits DosSetFileLocks if it fails
        //     (TODO: should check the partition type instead)
        if(rc == ERROR_INVALID_PARAMETER && (OffsetHigh || nNumberOfBytesToLockHigh)) {
            goto oldlock;
        }
   }
   else
   {
oldlock:
        FILELOCK lockRange = { OffsetLow, nNumberOfBytesToLockLow };

        rc = DosSetFileLocks(hFile, NULL, &lockRange,
                             (dwFlags & LOCKFILE_FAIL_IMMEDIATELY_W) ? 0 : 5000, 0);
   }
   if(rc) {
        SetLastError(error2WinError(rc));
        return FALSE;
   }
   SetLastError(ERROR_SUCCESS_W);
   return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosUnlockFile(DWORD hFile, DWORD OffsetLow, DWORD OffsetHigh,
                        DWORD nNumberOfBytesToLockLow, DWORD nNumberOfBytesToLockHigh,
                        LPOVERLAPPED lpOverlapped)
{
  APIRET   rc;

   // Set 5 secs timeout for unlocking file and no other can access this
   // file region

   if(lpOverlapped) {//TODO:
        dprintf(("OSLibDosUnlockFile: overlapped unlock not yet implemented!!"));
   }
   if(f64BitIO)
   {
        FILELOCKL unlockRangeL;

        unlockRangeL.lOffset.ulLo = OffsetLow;
        unlockRangeL.lOffset.ulHi = OffsetHigh;
        unlockRangeL.lRange.ulLo  = nNumberOfBytesToLockLow;
        unlockRangeL.lRange.ulHi  = nNumberOfBytesToLockHigh;

        rc = OdinDosSetFileLocksL(hFile, &unlockRangeL, NULL, 5000, 0);
        //SvL: 64 bits values are only supported by JFS
        //     Try the 32 bits DosSetFileLocks if it fails
        //     (TODO: should check the partition type instead)
        if(rc == ERROR_INVALID_PARAMETER && (OffsetHigh || nNumberOfBytesToLockHigh)) {
            goto oldlock;
        }
   }
   else
   {
oldlock:
        FILELOCK unlockRange = { OffsetLow, nNumberOfBytesToLockLow };

        rc = DosSetFileLocks(hFile, &unlockRange, NULL,  5000, 0);
   }
   if(rc) {
        SetLastError(error2WinError(rc));
        return FALSE;
   }
   SetLastError(ERROR_SUCCESS_W);
   return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosFlushFileBuffers(DWORD hFile)
{
  APIRET   rc;

   rc = DosResetBuffer(hFile);
   SetLastError(error2WinError(rc));
   return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosGetFileSize(DWORD hFile, LPDWORD lpdwFileSizeHigh)
{
 APIRET rc;
 ULONG  sizeLow;

   if(f64BitIO)
   {
        FILESTATUS3L fsts3ConfigInfoL = {{0}};
        ULONG        ulBufSize       = sizeof(FILESTATUS3L);

        rc = DosQueryFileInfo(hFile, FIL_STANDARDL, &fsts3ConfigInfoL, ulBufSize);
        if(lpdwFileSizeHigh) {
            *lpdwFileSizeHigh = fsts3ConfigInfoL.cbFile.ulHi;
        }
        sizeLow = fsts3ConfigInfoL.cbFile.ulLo;
   }
   else
   {
        FILESTATUS3 fsts3ConfigInfo = {{0}};
        ULONG       ulBufSize       = sizeof(FILESTATUS3);

        if(lpdwFileSizeHigh) {
            *lpdwFileSizeHigh = 0;
        }
        rc = DosQueryFileInfo(hFile, FIL_STANDARD, &fsts3ConfigInfo, ulBufSize);
        sizeLow = fsts3ConfigInfo.cbFile;
   }
   if(rc) {
        SetLastError(error2WinError(rc));
        return -1;
   }
   SetLastError(ERROR_SUCCESS_W);
   return sizeLow;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetFilePointer(DWORD hFile, DWORD OffsetLow, DWORD *OffsetHigh, DWORD method)
{
  LONGLONG offsetL;
  LONGLONG newoffsetL;
  APIRET   rc;
  DWORD    newoffset;

  switch(method)
  {
    case FILE_BEGIN_W:
      method = FILE_BEGIN;
      break;

    case FILE_CURRENT_W:
      method = FILE_CURRENT;
      break;

    case FILE_END_W:
      method = FILE_END;
      break;
  }

  // PH Note: for a negative 32-bit seek, the OS/2 64-bit version
  // needs to be skipped.
#if 0 /* doesnt works at all with flash 10 */
  if( (f64BitIO) && (OffsetHigh) && ((DWORD)OffsetHigh != 0xAAAAAAAA) &&
     (*OffsetHigh != 0xAAAAAAAA) &&
     (*OffsetHigh != 0)) //workaround for flash10 video
  {
        offsetL.ulLo = OffsetLow;
        offsetL.ulHi = (OffsetHigh) ? *OffsetHigh : 0;
        rc = OdinDosSetFilePtrL(hFile, offsetL, method, &newoffsetL);
        if(OffsetHigh) {
            *OffsetHigh = newoffsetL.ulHi;
        }
        newoffset = newoffsetL.ulLo;
  }
  else
#endif
      rc = DosSetFilePtr(hFile, OffsetLow, method, &newoffset);

  dprintf(("OSLibDosSetFilePointer. method: %08x wanted: %08x moved to %08x",
           method, OffsetLow, newoffset));
  if(rc)
  {
        SetLastError(error2WinError(rc));
        return -1;
  }
  SetLastError(ERROR_SUCCESS_W);
  return newoffset;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosSetEndOfFile(DWORD hFile)
{
 ULONG    newFilePos;
 LONGLONG FilePosL = {0,0};
 LONGLONG newFilePosL;
 APIRET   rc;

   if(f64BitIO) {
        rc = OdinDosSetFilePtrL(hFile, FilePosL, FILE_CURRENT, &newFilePosL);
        if(rc == 0) {
            rc = OdinDosSetFileSizeL(hFile, newFilePosL);
        }
   }
   else {
        rc = DosSetFilePtr(hFile, 0, FILE_CURRENT, &newFilePos);
        if(rc == 0) {
            rc = DosSetFileSize(hFile, newFilePos);
        }
   }
   if(rc) {
        SetLastError(error2WinError(rc));
        return FALSE;
   }
   SetLastError(ERROR_SUCCESS_W);
   return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosGetFileInformationByHandle(LPCSTR lpFileName, DWORD hFile, BY_HANDLE_FILE_INFORMATION* pInfo)
{
   APIRET      rc;

   if (lpFileName == NULL && hFile == INVALID_HANDLE_VALUE_W)
   {
       SetLastError(ERROR_INVALID_PARAMETER_W);
       return FALSE;
   }

   // NOTE: On HPFS386, doing FIL_QUERYEASIZE on a write-only file will
   // fail with ERROR_ACCESS_DENIED. Since we don't actually care about EAs
   // here, we will simply use FIL_STANDARD instead.

   if(f64BitIO)
   {
        FILESTATUS3L statusL = { 0 };

        rc = hFile != INVALID_HANDLE_VALUE_W ?
                    DosQueryFileInfo(hFile, FIL_STANDARDL,
                                     &statusL, sizeof(statusL)) :
                    DosQueryPathInfo(lpFileName, FIL_STANDARDL,
                                     &statusL, sizeof(statusL));
        if(rc == NO_ERROR)
        {
            pInfo->dwFileAttributes = 0;
            if(!(statusL.attrFile & NOT_NORMAL))
                pInfo->dwFileAttributes |= FILE_ATTRIBUTE_NORMAL_W;
            if(statusL.attrFile & FILE_READONLY)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_READONLY_W;
            if(statusL.attrFile & FILE_HIDDEN)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN_W;
            if(statusL.attrFile & FILE_SYSTEM)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_SYSTEM_W;
            if(statusL.attrFile & FILE_DIRECTORY)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY_W;
            if(statusL.attrFile & FILE_ARCHIVED)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE_W;

            pmDateTimeToFileTime(&statusL.fdateCreation,
                                 &statusL.ftimeCreation,
                                 &pInfo->ftCreationTime);
            pmDateTimeToFileTime(&statusL.fdateLastAccess,
                                 &statusL.ftimeLastAccess,
                                 &pInfo->ftLastAccessTime);
            pmDateTimeToFileTime(&statusL.fdateLastWrite,
                                 &statusL.ftimeLastWrite,
                                 &pInfo->ftLastWriteTime);

            pInfo->nFileSizeHigh = statusL.cbFile.ulHi;
            pInfo->nFileSizeLow  = statusL.cbFile.ulLo;
            pInfo->dwVolumeSerialNumber = 0;
            pInfo->nNumberOfLinks = 1;
            pInfo->nFileIndexHigh = 0;
            pInfo->nFileIndexLow  = 0;
        }
   }
   else
   {
        FILESTATUS3  status  = { 0 };

        rc = hFile != INVALID_HANDLE_VALUE_W ?
                    DosQueryFileInfo(hFile, FIL_STANDARD, &status,
                                     sizeof(status)) :
                    DosQueryPathInfo(lpFileName, FIL_STANDARD, &status,
                                     sizeof(status));
        if(rc == NO_ERROR)
        {
            pInfo->dwFileAttributes = 0;
            if(!(status.attrFile & NOT_NORMAL))
                pInfo->dwFileAttributes |= FILE_ATTRIBUTE_NORMAL_W;
            if(status.attrFile & FILE_READONLY)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_READONLY_W;
            if(status.attrFile & FILE_HIDDEN)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN_W;
            if(status.attrFile & FILE_SYSTEM)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_SYSTEM_W;
            if(status.attrFile & FILE_DIRECTORY)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY_W;
            if(status.attrFile & FILE_ARCHIVED)
                    pInfo->dwFileAttributes |= FILE_ATTRIBUTE_ARCHIVE_W;

            pmDateTimeToFileTime(&status.fdateCreation,
                                 &status.ftimeCreation,
                                 &pInfo->ftCreationTime);
            pmDateTimeToFileTime(&status.fdateLastAccess,
                                 &status.ftimeLastAccess,
                                 &pInfo->ftLastAccessTime);
            pmDateTimeToFileTime(&status.fdateLastWrite,
                                 &status.ftimeLastWrite,
                                 &pInfo->ftLastWriteTime);

            pInfo->nFileSizeHigh = 0;
            pInfo->nFileSizeLow  = status.cbFile;
            pInfo->dwVolumeSerialNumber = 0;
            pInfo->nNumberOfLinks = 1;
            pInfo->nFileIndexHigh = 0;
            pInfo->nFileIndexLow  = 0;
        }
   }
   if(rc) {
        SetLastError(error2WinError(rc));
        return FALSE;
   }

   if(lpFileName &&
      // must be the full path
      *lpFileName && lpFileName[1] == ':') {
       // get the volume serial
       ULONG disk = toupper(*lpFileName) - 'A' + 1;
       FSINFO fsinfo;
       rc = DosQueryFSInfo(disk, FSIL_VOLSER, &fsinfo, sizeof(fsinfo));
       if(!rc) {
           pInfo->dwVolumeSerialNumber = *(USHORT *)&fsinfo.ftimeCreation;
           pInfo->dwVolumeSerialNumber <<= 16;
           pInfo->dwVolumeSerialNumber |= *(USHORT *)&fsinfo.fdateCreation;
       }
       // fake the index number
       pInfo->nFileIndexHigh = crc32str(lpFileName);
       pInfo->nFileIndexLow = sdbm(lpFileName);
   }

   SetLastError(ERROR_SUCCESS_W);
   return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosSetFileTime(DWORD hFile,  LPFILETIME pFT1,
                         LPFILETIME pFT2, LPFILETIME pFT3)
{
  FILESTATUS3 fileInfo;
  APIRET      rc;

  rc = DosQueryFileInfo(hFile, FIL_STANDARD, &fileInfo, sizeof(fileInfo));

  if (rc == NO_ERROR)
  {
	if (pFT1) pmFileTimeToDateTime(pFT1, &fileInfo.fdateCreation,  &fileInfo.ftimeCreation);
	if (pFT2) pmFileTimeToDateTime(pFT2, &fileInfo.fdateLastAccess,&fileInfo.ftimeLastAccess);
	if (pFT3) pmFileTimeToDateTime(pFT3, &fileInfo.fdateLastWrite, &fileInfo.ftimeLastWrite);

    rc = DosSetFileInfo(hFile, FIL_STANDARD, &fileInfo, sizeof(fileInfo));
  }

  if(rc)
  {
    SetLastError(error2WinError(rc));
    return FALSE;
  }
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosGetFileTime(DWORD hFile, LPFILETIME pFT1,
                         LPFILETIME pFT2, LPFILETIME pFT3)
{
  FILESTATUS3 fileInfo;
  APIRET      rc;

  rc = DosQueryFileInfo(hFile, FIL_STANDARD, &fileInfo, sizeof(fileInfo));

  if(rc == NO_ERROR)
  {
  	if (pFT1) pmDateTimeToFileTime(&fileInfo.fdateCreation,  &fileInfo.ftimeCreation,  pFT1);
  	if (pFT2) pmDateTimeToFileTime(&fileInfo.fdateLastAccess,&fileInfo.ftimeLastAccess,pFT2);
  	if (pFT3) pmDateTimeToFileTime(&fileInfo.fdateLastWrite, &fileInfo.ftimeLastWrite, pFT3);
  }
  if(rc)
  {
    SetLastError(error2WinError(rc));
    return FALSE;
  }
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetFilePtr2(DWORD hFile, DWORD offset, DWORD method)
{
 DWORD  newoffset;
 APIRET rc;


   rc = DosSetFilePtr(hFile, offset, method, &newoffset);
   if(rc) {
      dprintf(("DosSetFilePtr Error rc:%d", rc));
      return -1;
   }
   else  return newoffset;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosDupHandle(DWORD hFile, DWORD *hNew)
{
   DWORD dwNewHandle = -1, ret;

   //Use the stack for storing the new handle; DosDupHandle doesn't like high
   //addresses
   ret = DosDupHandle(hFile, &dwNewHandle);
   *hNew = dwNewHandle;
   return ret;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetFHState(DWORD hFile, DWORD dwFlags)
{
   DWORD  ulMode;
   APIRET rc;

   rc = DosQueryFHState(hFile, &ulMode);
   if(rc != NO_ERROR) return error2WinError(rc);

   //turn off non-participating bits
   ulMode &= 0x7F88;

   if(dwFlags & HANDLE_FLAG_INHERIT_W) {
       ulMode &= ~OPEN_FLAGS_NOINHERIT;
   }
   else
       ulMode |= OPEN_FLAGS_NOINHERIT;

   rc = DosSetFHState(hFile, ulMode);
   return error2WinError(rc);
}
//******************************************************************************
//Returns time spent in kernel & user mode in milliseconds
//******************************************************************************
BOOL OSLibDosQueryProcTimes(DWORD procid, ULONG *kerneltime, ULONG *usertime)
{
 APIRET rc;
 char *buf;
 ULONG size;
 ULONG nrthreads = 4;

tryagain:
  size = sizeof(QTOPLEVEL)+sizeof(QGLOBAL)+sizeof(QPROCESS) + nrthreads*sizeof(QTHREAD);
  buf = (char *)malloc(size);
  rc = DosQuerySysState(0x1, RESERVED, procid, RESERVED, (PCHAR)buf, size);

  if(rc) {
        free(buf);
        if(rc == ERROR_BUFFER_OVERFLOW) {
                nrthreads += 4;
                goto tryagain;
        }
        return FALSE;
  }
  PQTOPLEVEL top = (PQTOPLEVEL)buf;

  *kerneltime = 0;
  *usertime = 0;
  for(int i=0;i<top->procdata->threadcnt;i++) {
        *kerneltime += top->procdata->threads[i].systime;
        *usertime   += top->procdata->threads[i].usertime;
  }
  free(buf);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
// TODO: implement SecurityAttributes parameter
DWORD OSLibDosCreateNamedPipe(LPCTSTR lpName,
                               DWORD   dwOpenMode,
                               DWORD   dwPipeMode,
                               DWORD   nMaxInstances,
                               DWORD   nOutBufferSize,
                               DWORD   nInBufferSize,
                               DWORD   nDefaultTimeOut,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{  DWORD dwOS2Mode     = NP_NOINHERIT; //default is not inherited by child processes
   DWORD dwOS2PipeMode = 0;
   LPSTR lpOS2Name;
   DWORD hPipe;
   DWORD rc, ulAction;
   char  lpOemName[CCHMAXPATH];

  //Convert file name from Windows to OS/2 codepage
  CharToOemA(lpName, lpOemName);

  if ((dwOpenMode & PIPE_ACCESS_DUPLEX_W) == PIPE_ACCESS_DUPLEX_W)
    dwOS2Mode |= NP_ACCESS_DUPLEX;
  else
  if (dwOpenMode & PIPE_ACCESS_INBOUND_W)
    dwOS2Mode |= NP_ACCESS_INBOUND;
  else
  if (dwOpenMode & PIPE_ACCESS_OUTBOUND_W)
    dwOS2Mode |= NP_ACCESS_OUTBOUND;
  // TODO:
  // if(dwOpenMode & FILE_FLAG_OVERLAPPED)
  // if(dwOpenMode & WRITE_DAC)
  // if(dwOpenMode & WRITE_OWNER)
  // if(dwOpenMode & ACCESS_SYSTEM_SECURITY)
  if(dwOpenMode & FILE_FLAG_WRITE_THROUGH_W)
    dwOS2Mode |= NP_WRITEBEHIND; // FIXME: I'm not sure!

  if (dwPipeMode & PIPE_WAIT_W)
    dwOS2PipeMode |= NP_WAIT;
  if (dwPipeMode & PIPE_NOWAIT_W)
    dwOS2PipeMode |= NP_NOWAIT;
  if (dwPipeMode & PIPE_READMODE_BYTE_W)
    dwOS2PipeMode |= NP_READMODE_BYTE;
  if (dwPipeMode & PIPE_READMODE_MESSAGE_W)
    dwOS2PipeMode |= NP_READMODE_MESSAGE;
  if (dwPipeMode & PIPE_TYPE_BYTE_W)
    dwOS2PipeMode |= NP_TYPE_BYTE;
  if (dwPipeMode & PIPE_TYPE_MESSAGE_W)
    dwOS2PipeMode |= NP_TYPE_MESSAGE;

  if (nMaxInstances>0xff)
  {
    SetLastError(ERROR_INVALID_PARAMETER_W); // ERROR_INVALID_PARAMETER
    return -1; // INVALID_HANDLE_VALUE
  }
  dwOS2PipeMode |= nMaxInstances;

  if (strstr(lpOemName,"\\\\."))
  {
    // If pipe is created on the local machine
    // we must delete string \\. because
    // in Windows named pipes scheme is a \\.\PIPE\pipename
    // but in OS/2 only \PIPE\pipename
    lpOS2Name = (LPSTR)lpOemName + 3;
  }
  else lpOS2Name = (LPSTR)lpOemName;

  dprintf(("DosCreateNPipe(%s,%x,%x,%x,%x,%x)",lpOS2Name,dwOS2Mode,dwOS2PipeMode,nInBufferSize,nOutBufferSize,nDefaultTimeOut));

  //if the windows app tries to open another instance of an existing pipe, then
  //we must use DosOpen here. So first try DosOpen, if that fails then we can
  //create the named pipe
  rc = DosOpen(lpOS2Name, &hPipe, &ulAction, 0, FILE_NORMAL, FILE_OPEN,
               ((dwOpenMode & PIPE_ACCESS_INBOUND_W) ? OPEN_ACCESS_READWRITE : OPEN_ACCESS_READONLY) |
               OPEN_SHARE_DENYNONE | OPEN_FLAGS_NOINHERIT, NULL);

  if(rc == NO_ERROR) {
#if 0
      //TODO:
      if(dwOpenMode & FILE_FLAG_FIRST_PIPE_INSTANCE_W) {
          DosClose(hPipe);
          SetLastError(ERROR_ALREADY_EXISTS_W);
          return -1;
      }
      else {
#endif
          dprintf(("Opening of existing named pipe succeeded"));
          SetLastError(ERROR_SUCCESS_W);
          return hPipe;
//      }
  }

  rc=DosCreateNPipe(lpOS2Name,
                    &hPipe,
                    dwOS2Mode,
                    dwOS2PipeMode,
                    nInBufferSize,
                    nInBufferSize,
                    nDefaultTimeOut); // Timeouts must be tested!

  dprintf(("DosCreateNPipe rc=%d",rc));
  if (rc)
  {
    SetLastError(error2WinError(rc,ERROR_INVALID_PARAMETER_W));
    return -1; // INVALID_HANDLE_VALUE
  }
  SetLastError(ERROR_SUCCESS_W);
  return hPipe;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibSetNamedPipeState(DWORD hNamedPipe, DWORD dwPipeMode)
{
 ULONG  dwOS2PipeMode = 0;
 APIRET rc;

  if (dwPipeMode & PIPE_WAIT_W)
    dwOS2PipeMode |= NP_WAIT;
  if (dwPipeMode & PIPE_NOWAIT_W)
    dwOS2PipeMode |= NP_NOWAIT;
  if (dwPipeMode & PIPE_READMODE_BYTE_W)
    dwOS2PipeMode |= NP_READMODE_BYTE;
  if (dwPipeMode & PIPE_READMODE_MESSAGE_W)
    dwOS2PipeMode |= NP_READMODE_MESSAGE;

  rc = DosSetNPHState(hNamedPipe, dwOS2PipeMode);
  if(rc) {
    SetLastError(error2WinError(rc, ERROR_INVALID_PARAMETER_W));
    return FALSE;
  }
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosOpenPipe(LPCTSTR lpName,
                       DWORD fuAccess,
                       DWORD fuShare,
                       LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                       DWORD fuCreate,
                       DWORD fuAttrFlags)
{
  LPSTR lpOS2Name;
  ULONG hPipe;
  ULONG rc, ulAction;
  ULONG openFlag = 0;
  ULONG openMode = OPEN_FLAGS_NOINHERIT; //default is not inherited by child processes
  char  lpOemName[CCHMAXPATH];

   //Convert file name from Windows to OS/2 codepage
   CharToOemA(lpName, lpOemName);

   switch(fuCreate)
   {
   case CREATE_NEW_W:
        openFlag |= OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS;
        break;
   case CREATE_ALWAYS_W:
       /* kso 2000-11-26: Not sure if OPEN_ACTION_REPLACE_IF_EXISTS is correct here! It is correct according to
        *   MSDN, but not according to "The Win32 API SuperBible". Anyway I haven't got time to check it out in
        *   NT now.
        *   The problem is that OPEN_ACTION_REPLACE_IF_EXISTS requires write access. It failes with
        *   rc = ERROR_ACCESS_DENIED (5). Quick fix, use OPEN_IF_EXIST if readonly access.
        */
       if (fuAccess & GENERIC_WRITE_W)
           openFlag |= OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS;
       else
           openFlag |= OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        break;
   case OPEN_EXISTING_W:
        openFlag |= OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        break;
   case OPEN_ALWAYS_W:
        openFlag |= OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        break;
   case TRUNCATE_EXISTING_W:
        openFlag |= OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS;
        break;
   }

   if(fuAttrFlags & FILE_FLAG_WRITE_THROUGH_W)   openMode |= OPEN_FLAGS_WRITE_THROUGH;
   if(fuAttrFlags & FILE_FLAG_NO_BUFFERING_W)    openMode |= OPEN_FLAGS_NO_CACHE;
   if(fuAttrFlags & FILE_FLAG_RANDOM_ACCESS_W)   openMode |= OPEN_FLAGS_RANDOM;
   if(fuAttrFlags & FILE_FLAG_SEQUENTIAL_SCAN_W) openMode |= OPEN_FLAGS_SEQUENTIAL;
   // TODO: FILE_FLAG_BACKUP_SEMANTICS_W
   //       FILE_FLAG_POSIX_SEMANTICS_W are not supported

   //TODO: FILE_SHARE_DELETE
   if((fuShare & (FILE_SHARE_READ_W | FILE_SHARE_WRITE_W)) == 0 )
        openMode |= OPEN_SHARE_DENYREADWRITE;
   else
   if((fuShare & (FILE_SHARE_READ_W | FILE_SHARE_WRITE_W)) == (FILE_SHARE_READ_W | FILE_SHARE_WRITE_W))
        openMode |= OPEN_SHARE_DENYNONE;
   else
   if(fuShare & FILE_SHARE_READ_W)
        openMode |= OPEN_SHARE_DENYWRITE;
   else
   if(fuShare & FILE_SHARE_WRITE_W)
        openMode |= OPEN_SHARE_DENYREAD;

   if(fuAccess == (GENERIC_READ_W | GENERIC_WRITE_W))
        openMode |= OPEN_ACCESS_READWRITE;
   else
   if(fuAccess & GENERIC_READ_W)
        openMode |= OPEN_ACCESS_READONLY;
   else
   if(fuAccess & GENERIC_WRITE_W)
        openMode |= OPEN_ACCESS_WRITEONLY;

  if (strstr(lpOemName,"\\\\."))
  {
       // If pipe is created on the local machine
       // we must delete string \\. because
       // in Windows named pipes scheme is a \\.\PIPE\pipename
       // but in OS/2 only \PIPE\pipename
       lpOS2Name = (LPSTR)lpOemName + 3;
  }
  else lpOS2Name = (LPSTR)lpOemName;

  rc = DosOpen(lpOS2Name, &hPipe, &ulAction, 0, 0,
               openFlag, openMode, NULL);

  if(rc == NO_ERROR) {
      dprintf(("Opening of existing named pipe succeeded"));
      SetLastError(ERROR_SUCCESS_W);
      return hPipe;
  }

  SetLastError(error2WinError(rc,ERROR_INVALID_PARAMETER_W));
  return -1; // INVALID_HANDLE_VALUE
}

//******************************************************************************
//******************************************************************************
// TODO: implement lpOverlapped parameter!
BOOL OSLibDosConnectNamedPipe(DWORD hNamedPipe, LPOVERLAPPED lpOverlapped)
{
 DWORD rc;

  rc=DosConnectNPipe(hNamedPipe);
  dprintf(("DosConnectNPipe rc=%d",rc));

  if(rc == NO_ERROR) {
      SetLastError(ERROR_SUCCESS_W);
      return (TRUE);
  }
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));
  return (FALSE);
}

//******************************************************************************
//******************************************************************************
BOOL OSLibDosCallNamedPipe( LPCTSTR lpNamedPipeName,
                            LPVOID  lpInBuffer,
                            DWORD   nInBufferSize,
                            LPVOID  lpOutBuffer,
                            DWORD   nOutBufferSize,
                            LPDWORD lpBytesRead,
                            DWORD   nTimeOut )
{
  LPSTR lpOS2Name;
  DWORD rc;
  char  lpOemNamedPipeName[CCHMAXPATH];

  //Convert file name from Windows to OS/2 codepage
  CharToOemA(lpNamedPipeName, lpOemNamedPipeName);

  if (strstr(lpOemNamedPipeName,"\\\\."))
  {
    // If pipe is created on the local machine
    // we must delete string \\. because
    // in Windows named pipes scheme is a \\.\PIPE\pipename
    // but in OS/2 only \PIPE\pipename
    lpOS2Name = (LPSTR)lpOemNamedPipeName + 3;
  }
  else lpOS2Name = (LPSTR)lpOemNamedPipeName;

  rc=DosCallNPipe(lpOS2Name,
                  lpInBuffer,
                  nInBufferSize,
                  lpOutBuffer,
                  nOutBufferSize,
                  lpBytesRead,
                  nTimeOut );


  if(rc == NO_ERROR) {
      SetLastError(ERROR_SUCCESS_W);
      return (TRUE);
  }
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));
  return (FALSE);
}

//******************************************************************************
//******************************************************************************
BOOL OSLibDosTransactNamedPipe( DWORD  hNamedPipe,
                                LPVOID lpInBuffer,
                                DWORD nInBufferSize,
                                LPVOID lpOutBuffer,
                                DWORD nOutBufferSize,
                                LPDWORD lpBytesRead,
                                LPOVERLAPPED lpOverlapped)
{
  DWORD rc;

  rc=DosTransactNPipe(hNamedPipe,
                      lpOutBuffer,
                      nOutBufferSize,
                      lpInBuffer,
                      nInBufferSize,
                      lpBytesRead);

  dprintf(("DosTransactNPipe returned rc=%d");)
  if(rc == NO_ERROR) {
      SetLastError(ERROR_SUCCESS_W);
      return (TRUE);
  }
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));
  return (FALSE);
}

//******************************************************************************
//******************************************************************************
BOOL OSLibDosPeekNamedPipe(DWORD   hPipe,
                           LPVOID  lpvBuffer,
                           DWORD   cbBuffer,
                           LPDWORD lpcbRead,
                           LPDWORD lpcbAvail,
                           LPDWORD lpcbMessage)
{
  DWORD     rc;
  AVAILDATA availData ={0};
  ULONG     ulState, ulRead, ulMessage;
  char      buffer;

  if(lpcbRead == NULL) {
      lpcbRead = &ulRead;
  }
  if(lpcbMessage == NULL) {
      lpcbMessage = &ulMessage;
  }
  if(lpvBuffer == NULL) {
      lpvBuffer = &buffer;
      cbBuffer = 0;
  }

  rc = DosPeekNPipe(hPipe,lpvBuffer,cbBuffer,lpcbRead,&availData,&ulState);

  dprintf(("DosPeekNPipe returned rc=%d",rc));

  //OS/2 doesn't support DosPeekNPipe for unnamed pipes; win32 does
  if (rc == 1) {
     dprintf(("WARNING: Pretend broken pipe for PeekNamedPipe with unnamed pipe handle"));
     SetLastError(ERROR_BROKEN_PIPE_W);
     return FALSE;
  }
  if (rc == NO_ERROR)
  {
    if(lpcbAvail) {
        *lpcbAvail   = availData.cbpipe;
    }
    if(lpcbMessage) {
        *lpcbMessage = availData.cbmessage;
    }
    SetLastError(ERROR_SUCCESS_W);
    return (TRUE);
  }
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));
  return (FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosDisconnectNamedPipe(DWORD hPipe)
{
  DWORD     rc;

  rc=DosDisConnectNPipe(hPipe);

  dprintf(("DosDisConnectNPipe returned rc=%d",rc));

  if(rc == NO_ERROR) {
      SetLastError(ERROR_SUCCESS_W);
      return (TRUE);
  }
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));
  return (FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosWaitNamedPipe(LPCSTR lpszNamedPipeName,
                            DWORD   dwTimeout)
{
  LPSTR lpOS2Name;
  DWORD rc;
  char  lpszOemNamedPipeName[CCHMAXPATH];

  //Convert file name from Windows to OS/2 codepage
  CharToOemA(lpszNamedPipeName, lpszOemNamedPipeName);

  if (strstr(lpszOemNamedPipeName,"\\\\."))
  {
    // If pipe is created on the local machine
    // we must delete string \\. because
    // in Windows named pipes scheme is a \\.\PIPE\pipename
    // but in OS/2 only \PIPE\pipename
    lpOS2Name = (LPSTR)lpszOemNamedPipeName + 3;
  }
  else lpOS2Name = (LPSTR)lpszOemNamedPipeName;

  rc=DosWaitNPipe(lpOS2Name,dwTimeout);

  dprintf(("DosWaitNPipe returned rc=%d",rc));

  if(rc == NO_ERROR) {
      SetLastError(ERROR_SUCCESS_W);
      return (TRUE);
  }
  SetLastError(error2WinError(rc,ERROR_PIPE_NOT_CONNECTED_W));

  return (FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL isRoot(CHAR* name)
{
  if (name[1] == ':')
  {
    //local name (x:\)
    return (name[2] == 0) || !strchr(&name[3],'\\');
  } else if (name[0] == '\\')
  {
    //UNC name (\\resource\drive\)
    CHAR *drive,*dir;

    drive = strchr(&name[2],'\\');
    if (!drive) return FALSE;
    dir = strchr(&drive[1],'\\');
    if (!dir) return TRUE;
    return !strchr(&dir[1],'\\');
  } else return FALSE; //unknown
}
//******************************************************************************
//******************************************************************************
inline CHAR system2DOSCharacter(CHAR ch)
{
  switch(ch)
  {
    case ' ':
    case '.':
    case '~':
      return '_';

    default:
      return toupper(ch);
  }
}

// TODO: not finished nor correct!!!!
VOID long2ShortName(CHAR* longName, CHAR* shortName)
{
  // check for uplink / root: "." and ".."
  if (longName[0] == '.')
  {
    // if ((strcmp(longName,".") == 0) || (strcmp(longName,"..") == 0))
    if (longName[1] == 0) // "."
    {
      shortName[0] = '.';
      shortName[1] = 0;
      return;
    }

    if (longName[1] == '.' && longName[2] == 0) // ".."
    {
      shortName[0] = '.';
      shortName[1] = '.';
      shortName[2] = 0;
      return;
    }
  }
  else
    // check for empty name
    if(longName[0] == 0)
    {
      shortName[0] = 0;
      return;
    }

  INT x;
  CHAR *source = longName;

  // Test if longName is 8:3 compliant and simply copy
  // the filename.
  BOOL flag83 = TRUE;

  // verify forbidden characters
  for (x = 0;
       (x < 8) &&
       (flag83 == TRUE);
       x++)
  {
    switch (*source)
    {
      case '.': // a period will cause the loop to abort!
        x=1000;
        break;

      case '/':      case '?':
      case '*':      case ':':
      case '\\':     case '"':
      case ' ':
        flag83 = FALSE;
        break;
      default:
        source++;
        break;
    }
  }

  // verify we're on a period now
  if (flag83 == TRUE)
    if (*source != '.')
         flag83 = FALSE;
    else source++;

  // verify extension
  if (flag83 == TRUE)
    for (INT y = 0;
         (y < 3) && (flag83 == TRUE);
         y++)
    {
      switch (*source)
      {
        case '/':      case '?':
        case '*':      case ':':
        case '\\':     case '"':
        case ' ':      case '.':
          flag83 = FALSE;
          break;
      }
      source++;
    }

  // verify we're at the end of the string now
  if (flag83 == TRUE)
    if (*source != 0)
      flag83 = FALSE;

  // OK, done
  if (flag83 == TRUE)
  {
    // we might not alter any character here, since
    // an app opening a specific file with an 8:3 "alias",
    // would surely fail.
    strcpy(shortName, longName);

    return; // Done
  }


  // @@@PH
  shortName[0] = 0; // this function is disabled anyway ...
  return;

  CHAR *dest = shortName;
  CHAR *ext = strrchr(longName,'.');

  //CB: quick and dirty, real FILE~12.EXT is too slow
  //PH: We'd have to count the number of non-8:3-compliant files
  //    within a directory. Or simpler: the number of files within
  //    the current directory.

  //8 character file name
  for (x = 0;x < 8;x++)
  {
    if ((source == ext) || (source[0] == 0)) break;
    dest[0] = system2DOSCharacter(source[0]);
    source++;
    dest++;
  }

  if (source[0] == 0)
  {
    dest[0] = 0;
    return;
  }

  if (source != ext)
  {
    //longName > 8 characters, insert ~1
    shortName[6] = '~';
    shortName[7] = '1';
  }

  if (ext)
  {
    //add extension, 3 characters
    dest[0] = ext[0];
    dest++;
    ext++;
    for (x = 0;x < 3;x++)
    {
      if (ext[0] == 0) break;
      dest[0] = system2DOSCharacter(ext[0]);
      ext++;
      dest++;
    }
  }
  dest[0] = 0;
}
//******************************************************************************
//******************************************************************************
VOID translateFileResults(FILESTATUS3 *pResult,LPWIN32_FIND_DATAA pFind,CHAR* achName)
{
  CHAR* name;

  pFind->dwReserved0 = pFind->dwReserved1 = 0;
  pFind->dwFileAttributes = pm2WinFileAttributes(pResult->attrFile);

  pmDateTimeToFileTime(&pResult->fdateCreation,&pResult->ftimeCreation,&pFind->ftCreationTime);
  pmDateTimeToFileTime(&pResult->fdateLastAccess,&pResult->ftimeLastAccess,&pFind->ftLastAccessTime);
  pmDateTimeToFileTime(&pResult->fdateLastWrite,&pResult->ftimeLastWrite,&pFind->ftLastWriteTime);

  pFind->nFileSizeHigh = 0; //CB: fixme
  pFind->nFileSizeLow = pResult->cbFile;
  name = strrchr(achName,'\\');
  if (name)
  {
    name++;
//    strcpy(pFind->cFileName,name);
    OemToCharA( name, pFind->cFileName );
  }
  else
    pFind->cFileName[0] = 0;

  long2ShortName(pFind->cFileName,pFind->cAlternateFileName);
}


VOID translateFindResults(FILEFINDBUF3 *pResult,LPWIN32_FIND_DATAA pFind)
{
  pFind->dwFileAttributes = pm2WinFileAttributes(pResult->attrFile);
  pFind->dwReserved0 = pFind->dwReserved1 = 0;

  pmDateTimeToFileTime(&pResult->fdateCreation,&pResult->ftimeCreation,&pFind->ftCreationTime);
  pmDateTimeToFileTime(&pResult->fdateLastAccess,&pResult->ftimeLastAccess,&pFind->ftLastAccessTime);
  pmDateTimeToFileTime(&pResult->fdateLastWrite,&pResult->ftimeLastWrite,&pFind->ftLastWriteTime);

  pFind->nFileSizeHigh = 0; //CB: fixme
  pFind->nFileSizeLow = pResult->cbFile;
//  strcpy(pFind->cFileName,pResult->achName);
  OemToCharA( pResult->achName, pFind->cFileName );
  long2ShortName(pFind->cFileName,pFind->cAlternateFileName);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosFindFirst(LPCSTR lpFileName,WIN32_FIND_DATAA* lpFindFileData)
{
  HDIR hDir = -1;
  ULONG attrs;
  FILEFINDBUF3 result;
  ULONG searchCount = 1;

  attrs = FILE_READONLY | FILE_HIDDEN | FILE_SYSTEM | FILE_DIRECTORY | FILE_ARCHIVED;
  result.achName[0] = 0;

  ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS_W);
  char    lOemFileName[260];

  CharToOemA(ODINHelperStripUNC((char*)lpFileName), lOemFileName);

  APIRET rc = DosFindFirst((PSZ)lOemFileName,&hDir,attrs,&result,sizeof(result),&searchCount,FIL_STANDARD);

  //check root: skip "." and ".." (HPFS, not on FAT)
  //check in OSLibDosFindNext not necessary: "." and ".." are the first two entries
  if ((rc == 0) && isRoot((LPSTR)ODINHelperStripUNC((char*)lpFileName)))
  {
    while ((strcmp(result.achName,".") == 0) ||
           (strcmp(result.achName,"..") == 0))
    {
      result.achName[0] = 0;
      searchCount = 1;
      APIRET rc = DosFindNext(hDir,&result,sizeof(result),&searchCount);
      if (rc)
      {
        DosFindClose(hDir);
        SetLastError(error2WinError(rc));

        SetErrorMode(oldmode);
        return INVALID_HANDLE_VALUE_W;
      }
    }
  }

  // enable i/o kernel exceptions again
  SetErrorMode(oldmode);

  if(rc)
  {
    DosFindClose(hDir);

    //Windows returns ERROR_FILE_NOT_FOUND if the file/directory is not present
    if(rc == ERROR_NO_MORE_FILES || rc == ERROR_PATH_NOT_FOUND) {
         SetLastError(ERROR_FILE_NOT_FOUND_W);
    }
    else SetLastError(error2WinError(rc));
    return INVALID_HANDLE_VALUE_W;
  }
  translateFindResults(&result,lpFindFileData);
  SetLastError(ERROR_SUCCESS_W);
  return hDir;
}
//******************************************************************************
// NOTE: returns "." and ".." in root
//******************************************************************************
DWORD OSLibDosFindFirstMulti(LPCSTR lpFileName,WIN32_FIND_DATAA *lpFindFileData,DWORD *count)
{
  HDIR hDir = -1;
  ULONG attrs;
  FILEFINDBUF3 *result,*data;
  ULONG searchCount = *count;

  attrs = FILE_READONLY | FILE_HIDDEN | FILE_SYSTEM | FILE_DIRECTORY | FILE_ARCHIVED;
  result = (FILEFINDBUF3*)malloc(searchCount*sizeof(FILEFINDBUF3));

  ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS_W);
  char  lOemFileName[260];

  CharToOemA(ODINHelperStripUNC((char*)lpFileName), lOemFileName);
  APIRET rc = DosFindFirst((PSZ)lOemFileName,&hDir,attrs,result,searchCount*sizeof(FILEFINDBUF3),&searchCount,FIL_STANDARD);
  SetErrorMode(oldmode);
  if (rc)
  {
    free(result);
    *count = 0;
    SetLastError(error2WinError(rc));

    return INVALID_HANDLE_VALUE_W;
  }

  data = result;
  for (int x = 0;x < searchCount;x++)
  {
    translateFindResults(data,&lpFindFileData[x]);
    data = (FILEFINDBUF3*)(((DWORD)data)+data->oNextEntryOffset);
  }
  free(result);
  *count = searchCount;

  SetLastError(ERROR_SUCCESS_W);
  return hDir;
}
//******************************************************************************
//******************************************************************************
BOOL  OSLibDosFindNext(DWORD hFindFile,WIN32_FIND_DATAA *lpFindFileData)
{
  FILEFINDBUF3 result;
  ULONG searchCount = 1;

  APIRET rc = DosFindNext((HDIR)hFindFile,&result,sizeof(result),&searchCount);
  if (rc)
  {
    SetLastError(error2WinError(rc));

    return FALSE;
  }

  translateFindResults(&result,lpFindFileData);
  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL  OSLibDosFindNextMulti(DWORD hFindFile,WIN32_FIND_DATAA *lpFindFileData,DWORD *count)
{
  FILEFINDBUF3 *result,*data;
  ULONG searchCount = *count;

  result = (FILEFINDBUF3*)malloc(searchCount*sizeof(FILEFINDBUF3));
  APIRET rc = DosFindNext((HDIR)hFindFile,result,searchCount*sizeof(FILEFINDBUF3),&searchCount);
  if (rc)
  {
    free(result);
    *count = 0;
    SetLastError(error2WinError(rc));

    return FALSE;
  }

  data = result;
  for (int x = 0;x < searchCount;x++)
  {
    translateFindResults(data,&lpFindFileData[x]);
    data = (FILEFINDBUF3*)(((DWORD)data)+data->oNextEntryOffset);
  }
  free(result);
  *count = searchCount;

  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosFindClose(DWORD hFindFile)
{
  APIRET rc = DosFindClose((HDIR)hFindFile);
  if (rc)
  {
    SetLastError(error2WinError(rc));
    return FALSE;
  }

  SetLastError(ERROR_SUCCESS_W);
  return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibGetFileAttributes(LPSTR lpFileName)
{
   FILESTATUS3 statusBuf;
   char        lOemFileName[CCHMAXPATH];
   char       *lpszBackslash, *lpszColon;
   APIRET      rc;

//testestest
   if(strlen(lpFileName) > CCHMAXPATH) DebugInt3();
//testestset

   //Convert file name from Windows to OS/2 codepage
   CharToOemA(ODINHelperStripUNC(lpFileName), lOemFileName);
   lpszBackslash = CharPrevA(lOemFileName, lOemFileName + strlen(lOemFileName));
   if(lpszBackslash)
   {
       if(*lpszBackslash == '\\')
       {
           lpszColon = CharPrevA(lOemFileName, lpszBackslash);
           if(lpszColon && *lpszColon != ':')
           {//only rootdir is allowed to have terminating backslash
               *lpszBackslash = 0;
           }
       }
       else
       if(*lpszBackslash == ':')
       {//root dir must end with backslash
           strcat(lOemFileName, "\\");
       }
   }

   rc = DosQueryPathInfo(lOemFileName, FIL_STANDARD, &statusBuf, sizeof(statusBuf));
   if(rc == ERROR_TOO_MANY_OPEN_FILES)
   {
       LONG  reqCount = 2;
       ULONG maxFiles;

       if(DosSetRelMaxFH(&reqCount, &maxFiles) == NO_ERROR)
           rc = DosQueryPathInfo(lOemFileName, FIL_STANDARD, &statusBuf, sizeof(statusBuf));
   }

   if(rc == NO_ERROR)
   {
      DWORD status = 0;
      if(!(statusBuf.attrFile & NOT_NORMAL))
          status |= FILE_ATTRIBUTE_NORMAL_W;
      if(statusBuf.attrFile & FILE_READONLY)
          status |= FILE_ATTRIBUTE_READONLY_W;
      if(statusBuf.attrFile & FILE_HIDDEN)
          status |= FILE_ATTRIBUTE_HIDDEN_W;
      if(statusBuf.attrFile & FILE_SYSTEM)
          status |= FILE_ATTRIBUTE_SYSTEM_W;
      if(statusBuf.attrFile & FILE_DIRECTORY)
          status |= FILE_ATTRIBUTE_DIRECTORY_W;
      if(statusBuf.attrFile & FILE_ARCHIVED)
          status |= FILE_ATTRIBUTE_ARCHIVE_W;

      SetLastError(ERROR_SUCCESS_W);
      return status;
   }
   SetLastError(error2WinError(rc));
   return -1;
}
//******************************************************************************
#define FSATTACH_SIZE 256
//******************************************************************************
DWORD OSLibDosQueryVolumeFS(int drive, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
 PFSQBUFFER2 fsinfo = (PFSQBUFFER2) alloca(FSATTACH_SIZE);
 ULONG       cb     = FSATTACH_SIZE;
 char        drv[3] = "A:";
 char       *fsname;
 APIRET      rc;

   if(lpFileSystemNameBuffer == NULL) {
        DebugInt3();
        return ERROR_INVALID_PARAMETER_W;
   }
   drv[0] = (char)('A' + drive - 1);

   ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS_W);
   rc = DosQueryFSAttach(drv, 1, FSAIL_QUERYNAME, fsinfo, &cb);
   SetErrorMode(oldmode);

   switch(rc) {
   case ERROR_INVALID_DRIVE:
        return ERROR_INVALID_DRIVE_W;
   case ERROR_NO_VOLUME_LABEL:
        return ERROR_NO_VOLUME_LABEL_W;
   case NO_ERROR:
        break;
   default:
        return ERROR_NOT_ENOUGH_MEMORY; //whatever
   }

   fsname = (char *)&fsinfo->szName[0] + fsinfo->cbName + 1;

  /* @@@PH 2000/08/10 CMD.EXE querys with nFileSystemNameSize == 0
   * however does NOT expect to receive an error.
   */
   strncpy(lpFileSystemNameBuffer,
           fsname,
           nFileSystemNameSize);
  /*
   if(strlen(fsname) < nFileSystemNameSize) {
        strcpy(lpFileSystemNameBuffer, fsname);
   }
   else return ERROR_BUFFER_OVERFLOW_W;
   */
   return ERROR_SUCCESS_W;
}
//******************************************************************************
typedef struct _FSINFOBUF
{
  ULONG     ulVolser;          /* Volume serial number            */
  VOLUMELABEL vol;               /* Volume lable                    */
} FSINFOBUF;
//******************************************************************************
DWORD OSLibDosQueryVolumeSerialAndName(int drive, LPDWORD lpVolumeSerialNumber,
                                       LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize)
{
 FSINFOBUF fsi;
 APIRET    rc;

   ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS_W);
   rc = DosQueryFSInfo(drive, FSIL_VOLSER, &fsi, sizeof(fsi));
   SetErrorMode(oldmode);

   switch(rc) {
   case ERROR_INVALID_DRIVE:
        return ERROR_INVALID_DRIVE_W;
   case ERROR_NO_VOLUME_LABEL:
        return ERROR_NO_VOLUME_LABEL_W;
   case NO_ERROR:
        break;
   default:
        return ERROR_NOT_ENOUGH_MEMORY; //whatever
   }

   if(lpVolumeSerialNumber) {
         *lpVolumeSerialNumber = fsi.ulVolser;
   }
   if(lpVolumeNameBuffer)
   {
        if(nVolumeNameSize > fsi.vol.cch) {
            strcpy(lpVolumeNameBuffer, (PCHAR)fsi.vol.szVolLabel);
        }
        else return ERROR_BUFFER_OVERFLOW_W;
   }

   return ERROR_SUCCESS_W;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibGetDiskFreeSpace(LPSTR lpRootPathName, LPDWORD lpSectorsPerCluster,
                           LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters,
                           LPDWORD lpTotalNumberOfClusters)
{
 ULONG diskNum;
 FSALLOCATE fsAlloc;
 APIRET rc;

   if(lpRootPathName == 0)
      diskNum = 0;
   else
   if('A' <= *lpRootPathName && *lpRootPathName <= 'Z' )
      diskNum = *lpRootPathName - 'A' + 1;
   else
   if('a' <= *lpRootPathName && *lpRootPathName <= 'z' )
      diskNum = *lpRootPathName - 'a' + 1;
   else
      diskNum = 0;

   ULONG oldmode = SetErrorMode(SEM_FAILCRITICALERRORS_W);
   rc = DosQueryFSInfo(diskNum, FSIL_ALLOC, &fsAlloc, sizeof(fsAlloc));
   SetErrorMode(oldmode);

   if(rc == 0)
   {
        *lpSectorsPerCluster     = fsAlloc.cSectorUnit;
        *lpBytesPerSector        = fsAlloc.cbSector;
        *lpNumberOfFreeClusters  = fsAlloc.cUnitAvail;
        *lpTotalNumberOfClusters = fsAlloc.cUnit;
        SetLastError(ERROR_SUCCESS_W);
        return TRUE;
   }
   SetLastError(error2WinError(rc));
   return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosGetDiskGeometry(HANDLE hDisk, DWORD cDisk, PVOID pdiskgeom)
{
   PDISK_GEOMETRY pGeom = (PDISK_GEOMETRY)pdiskgeom;
   BYTE  param[2] = {0, 0};
   ULONG parsize = 2;
   BIOSPARAMETERBLOCK bpb;
   ULONG datasize = sizeof(bpb);
   APIRET rc;

   dprintf(("OSLibDosGetDiskGeometry %x %d %x", hDisk, cDisk, pdiskgeom));
   param[1] = cDisk - 'A';
   rc = DosDevIOCtl((hDisk) ? hDisk : -1, IOCTL_DISK, DSK_GETDEVICEPARAMS, param, 2, &parsize, &bpb, sizeof(bpb), &datasize);
   if(rc == 0)
   {
            pGeom->SectorsPerTrack   = bpb.usSectorsPerTrack;
            pGeom->BytesPerSector    = bpb.usBytesPerSector;
            pGeom->TracksPerCylinder = 80;  //TODO:!!!!!
            pGeom->Cylinders.u.LowPart  = bpb.cCylinders;
            pGeom->Cylinders.u.HighPart = 0;
            switch(bpb.bDeviceType) {
            case DEVTYPE_48TPI:
                pGeom->MediaType = F5_360_512;  //?????
                break;
            case DEVTYPE_96TPI:
                pGeom->MediaType = F5_1Pt2_512; //?????
                break;
            case DEVTYPE_35:
                pGeom->MediaType = F3_720_512;
                break;
            case DEVTYPE_8SD:
            case DEVTYPE_8DD:
                pGeom->MediaType = RemovableMedia;
                break;
            case DEVTYPE_FIXED:
                pGeom->MediaType = FixedMedia;
                break;
            case DEVTYPE_TAPE:
                pGeom->MediaType = RemovableMedia;
                break;
            case DEVTYPE_UNKNOWN: //others, include 1.44 3.5 inch disk drive
                pGeom->MediaType = F3_1Pt44_512;
                break;
            case 8: //RW optical disk
                pGeom->MediaType = RemovableMedia;
                break;
            case 9: //2.88 3.5 inch disk
                pGeom->MediaType = F3_2Pt88_512;
                break;
            }
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
   }
   dprintf(("OSLibDosGetDiskGeometry: error %d -> %d", rc, error2WinError(rc)));
   SetLastError(error2WinError(rc));
   return FALSE;
}
//******************************************************************************

#define CDType         0x8000
#define FloppyType     0x4000
#define LanType        0x2000
#define LogicalType    0x1000
#define VDISKType      0x0800
#define OpticalType    0x0400
#define NonRemovable   0x01

#define FirstDrive             0
#define B_Drive_0_Based        1
#define Base_1_offset          1
#define Binary_to_Printable 0x41
#define LastDrive             26
#define OpticalSectorsPerCluster 4

#define DisketteCylinders 80



/**
 * Determin the type of a specific drive or the current drive.
 *
 * @returns DRIVE_UNKNOWN
 *          DRIVE_NO_ROOT_DIR
 *          DRIVE_CANNOTDETERMINE
 *          DRIVE_DOESNOTEXIST
 *          DRIVE_REMOVABLE
 *          DRIVE_FIXED
 *          DRIVE_REMOTE
 *          DRIVE_CDROM
 *          DRIVE_RAMDISK
 *
 * @param   ulDrive     Index of the drive which type we query.
 *
 * @status  completely implemented and tested
 * @author  Vitali Pelenyov <sunlover@anduin.net>
 * @author  bird
 */
ULONG OSLibGetDriveType(ULONG ulDrive)
{
    ULONG       cbParm;
    ULONG       cbData;
    ULONG       ulDriveNum = 0;
    ULONG       ulDriveMap = 0;
    APIRET      rc;
    ULONG       ulDriveType;


    /*
     * Check if drive is present in the logical drive mask,
     */
    if (    DosQueryCurrentDisk(&ulDriveNum, &ulDriveMap)
        || !(ulDriveMap & (1 << ulDrive))
            )
        return DRIVE_DOESNOTEXIST_W;


    /*
     * Optimize floppy queries for A: and B:.
     * (These aren't currently a subject of change.)
     */
    static ULONG ulFloppyMask = 0;
    if (ulFloppyMask & (1 << ulDrive))
        return DRIVE_REMOVABLE_W;

    /*
     * Optimize for CDROM requests.
     * (These aren't currently a subject of change.)
     */
    static ULONG ulCDROMMask = 0;
    if (ulCDROMMask & (1 << ulDrive))
        return DRIVE_CDROM_W;

    /*
     * Check for CD drives
     * We don't have to this everytime. I mean, the os2cdrom.dmd is
     * exactly very dynamic when it comes to this info.
     */
    if (cdDrvLtr.drive_count == 0xffff)
    {
        HFILE           hCDRom2;
        ULONG           ulAction = 0;

        if (DosOpen("\\DEV\\CD-ROM2$", &hCDRom2, &ulAction, 0,
                    FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
                    OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY | OPEN_FLAGS_NOINHERIT, NULL)
            == NO_ERROR)
        {
            cbData = sizeof(cdDrvLtr);
            rc = DosDevIOCtl(hCDRom2,
                             IOC_CDROM_2,
                             IOCD_RETURN_DRIVE_LETTER,
                             NULL, 0, NULL,
                             (PVOID)&cdDrvLtr, sizeof(cdDrvLtr), &cbData);
            DosClose(hCDRom2);
        }
        else
            cdDrvLtr.drive_count = 0;
    }


    /*
     * Is the drive present?? Lets the the BPB to check this.
     * this should be a pretty speedy call if I'm not much mistaken.
     */
    BIOSPARAMETERBLOCK  bpb = {0};
    DSKREQ              DskReq;
    DskReq.Infotype  = 0;
    DskReq.DriveUnit = (UCHAR)ulDrive;
    cbParm = sizeof(DskReq);
    cbData = sizeof(bpb);
    if (DosDevIOCtl(-1, IOCTL_DISK, DSK_GETDEVICEPARAMS,
                    (PVOID)&DskReq, sizeof(DskReq), &cbParm,
                    (PVOID)&bpb, sizeof(BIOSPARAMETERBLOCK), &cbData)
        == NO_ERROR)
    {
        if (bpb.bDeviceType == 0 && bpb.fsDeviceAttr == 0)
            bpb.fsDeviceAttr = LanType;  // fix for LAN type drives
    }
    else
        bpb.fsDeviceAttr = LanType; //could be a LAN drive - could it??


    /*
     * This is a CDROM/DVD drive
     *
     * Fix: When the cdrom is not last drive letter that
     *      sometimes reports wrong index, have to check
     *      that device type can be a CDROM one.
     */
    if (    cdDrvLtr.drive_count > 0
        &&  (bpb.bDeviceType == DEVTYPE_UNKNOWN || bpb.bDeviceType == DEVTYPE_OPTICAL) // Other or RW optical
        &&  ulDrive >= cdDrvLtr.first_drive_number
        &&  ulDrive <  cdDrvLtr.first_drive_number + cdDrvLtr.drive_count
            )
        bpb.fsDeviceAttr |= CDType;


    if (bpb.bDeviceType || bpb.fsDeviceAttr == LanType)
    {
        if ((bpb.fsDeviceAttr & (CDType | NonRemovable)) == 0)
        {   // if the device is removable and NOT a CD
            CHAR        szDevName[4] = "A:";
            PFSQBUFFER2 pfsqbuf2;

            szDevName[0] += ulDrive;
            cbData = sizeof(PFSQBUFFER2) + 3 * CCHMAXPATH;
            pfsqbuf2 = (PFSQBUFFER2)malloc(cbData);
            DosError(FERR_DISABLEEXCEPTION | FERR_DISABLEHARDERR); // disable error popups
            rc = DosQueryFSAttach(&szDevName[0], 0L, FSAIL_QUERYNAME, pfsqbuf2, &cbData);
            if (rc == NO_ERROR)
            {
                if (pfsqbuf2->iType == FSAT_REMOTEDRV)
                    bpb.fsDeviceAttr |= LanType;
                else if (pfsqbuf2->iType == FSAT_LOCALDRV)
                {
                    PSZ pszFSName = (PSZ)pfsqbuf2->szName + pfsqbuf2->cbName + 1;
                    if (!strcmp(pszFSName, "FAT"))
                    {
                        // device is a removable FAT drive, so it MUST be diskette
                        // as Optical has another name as does LAN and SRVIFS
                        if (bpb.bSectorsPerCluster != OpticalSectorsPerCluster)
                            bpb.fsDeviceAttr |= FloppyType;
                        else
                            bpb.fsDeviceAttr |= OpticalType;
                    }
                    /*
                     * CDROM detection fix
                     */
                    else
                    {
                        // device is removable non-FAT, maybe it is CD?
                        if (    bpb.bDeviceType == DEVTYPE_UNKNOWN
                            &&  bpb.bMedia == 5
                            &&  bpb.usBytesPerSector > 512
                                )
                            bpb.fsDeviceAttr |= CDType;
                    }
                    // CDROM detection fix
                }
            }
            else // must be no media or audio only (for CDs at least)
            {
                if (bpb.cCylinders <= DisketteCylinders) // floppies will always be 80
                {
                    // or less cylinders
                    if (bpb.bSectorsPerCluster != OpticalSectorsPerCluster)
                        bpb.fsDeviceAttr |= FloppyType;
                    else
                        bpb.fsDeviceAttr |= OpticalType;
                }
            }

            //Enable error popups again
            DosError(FERR_ENABLEEXCEPTION | FERR_ENABLEHARDERR);

            free(pfsqbuf2);
        }
        else
        {// non removable or CD type. maybe RAM disk
            if (!(bpb.fsDeviceAttr & CDType))       // if NOT CD
            {
                if (bpb.cFATs == 1)                 // is there only one FAT?
                    bpb.fsDeviceAttr |= VDISKType;  // -> RAM disk
            }
        }
    }


    /*
     * Determin return value.
     */
    if (bpb.fsDeviceAttr & FloppyType)
    {
        ulDriveType = DRIVE_REMOVABLE_W;
        /* update floppy cache if A or B */
        if (ulDrive <= 1)
            ulFloppyMask |= 1 << ulDrive;
    }
    else if (bpb.fsDeviceAttr & CDType)
    {
        ulDriveType = DRIVE_CDROM_W;
        /* update cdrom cache */
        ulCDROMMask |= 1 << ulDrive;
    }
    else if (bpb.fsDeviceAttr & LanType)
        ulDriveType = DRIVE_REMOTE_W;
    else if (bpb.fsDeviceAttr & VDISKType)
        ulDriveType = DRIVE_RAMDISK_W;
    else if (bpb.fsDeviceAttr & OpticalType || bpb.bDeviceType == DEVTYPE_OPTICAL)
        ulDriveType = DRIVE_FIXED_W;
    else if (bpb.bDeviceType == DEVTYPE_FIXED)
        ulDriveType = DRIVE_FIXED_W;
    else
        ulDriveType = DRIVE_UNKNOWN_W;

    return ulDriveType;
}
//******************************************************************************
//Returns bit map where with the mapping of the logical drives
//******************************************************************************
ULONG OSLibGetLogicalDrives()
{
 ULONG   ulDriveNum   = 0;      /* Drive number (A=1, B=2, C=3, ...)    */
 ULONG   ulDriveMap   = 0;      /* Mapping of valid drives              */
 APIRET  rc;

    rc = DosQueryCurrentDisk(&ulDriveNum, &ulDriveMap);
    if(rc) {
        DebugInt3();
        SetLastError(error2WinError(rc));
        return 0;
    }
    return ulDriveMap;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibDosQueryCurrentDisk()

{
 ULONG   ulDriveNum   = 0;      /* Drive number (A=1, B=2, C=3, ...)    */
 ULONG   ulDriveMap   = 0;      /* Mapping of valid drives              */
 APIRET  rc;

    rc = DosQueryCurrentDisk(&ulDriveNum, &ulDriveMap);
    if(rc) {
        DebugInt3();
        SetLastError(error2WinError(rc));
        return 0;
    }
    return ulDriveNum;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosCreatePipe(PHANDLE phfRead,
                        PHANDLE phfWrite,
                        LPSECURITY_ATTRIBUTES lpsa,
                        DWORD   dwSize)
{
  APIRET rc;

  dprintf(("DosCreatePipe(%08xh,%08xh,%08xh)\n",
           phfRead,
           phfWrite,
           dwSize));

  // select default buffer size
  if (dwSize == 0)
    dwSize = 4096;

  rc = DosCreatePipe(phfRead,
                     phfWrite,
                     dwSize);
  dprintf(("DosCreatePipe rc=%d",rc));
  if (rc)
  {
    SetLastError(error2WinError(rc,ERROR_INVALID_PARAMETER_W));
    return -1; // INVALID_HANDLE_VALUE
  }
  SetLastError(ERROR_SUCCESS_W);
  return NO_ERROR;
}
//******************************************************************************
//******************************************************************************
DWORD SYSTEM OSLibDosDevIOCtl( DWORD hFile, DWORD dwCat, DWORD dwFunc,
                               PVOID pParm, DWORD dwParmMaxLen, DWORD *pdwParmLen,
                               PVOID pData, DWORD dwDataMaxLen, DWORD *pdwDataLen)
{
  APIRET rc;
  PVOID pTiledParm    = pParm;
  BOOL  flagTiledParm = FALSE;
  PVOID pTiledData    = pData;
  BOOL  flagTiledData = FALSE;

#if 1
    /*
     * Quick and Dirty Fix!
     * TO BE REMOVED!
     *
     * On some VPC installation without CDROM we seem to
     * use a concidrable amount of time during Win2k shutdown.
     * No idea why, but it has to do with CDROM we think.
     *
     * So, let's just fail all IOCtls to CD01 if there aren't any
     * CDROMs in the system.
     *
     */

    /*
     * Check for CD drives
     * We don't have to this everytime. I mean, the os2cdrom.dmd is
     * exactly very dynamic when it comes to this info.
     */
    if (cdDrvLtr.drive_count == 0xffff)
    {
        HFILE           hCDRom2;
        ULONG           ulAction = 0;

        if (DosOpen("\\DEV\\CD-ROM2$", &hCDRom2, &ulAction, 0,
                    FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
                    OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY | OPEN_FLAGS_NOINHERIT, NULL)
            == NO_ERROR)
        {
            ULONG cbData = sizeof(cdDrvLtr);
            rc = DosDevIOCtl(hCDRom2,
                             IOC_CDROM_2,
                             IOCD_RETURN_DRIVE_LETTER,
                             NULL, 0, NULL,
                             (PVOID)&cdDrvLtr, sizeof(cdDrvLtr), &cbData);
            DosClose(hCDRom2);
        }
        else
            cdDrvLtr.drive_count = 0;
    }

    if (    cdDrvLtr.drive_count == 0
        &&  (dwCat == IOCTL_CDROMDISK
             || (dwCat == IOCTL_CDROMAUDIO
                 && dwParmMaxLen >= 4 && strncmp((char*)pParm, "CD01", 4))
             )
       )
    {
        /* just return some error code */
        return ERROR_BAD_COMMAND;
    }

#endif

#define MEM_TILED_CEILING 0x1fffffff

  // bounce buffer support
  // make sure no parameter or data buffer can pass the tiled memory region
  // barrier (usually 512MB). OS/2 kernel does not correctly translate these
  // addresses to a 16:16 address used in device driver land. In fact,
  // DosDevIOCtl is not a high memory enabled API!

  if (pTiledParm && (((DWORD)pTiledParm + max(dwParmMaxLen, *pdwParmLen)) > MEM_TILED_CEILING))
  {
    rc = DosAllocMem(&pTiledParm, dwParmMaxLen, PAG_READ | PAG_WRITE);
    if (rc)
      goto _exit_ioctl;

    flagTiledParm = TRUE;
  }

  if (pTiledData && (((DWORD)pTiledData + max(dwDataMaxLen, *pdwDataLen)) > MEM_TILED_CEILING))
  {
    rc = DosAllocMem(&pTiledData, dwDataMaxLen, PAG_READ | PAG_WRITE);
    if (rc)
        goto _exit_ioctl;

    flagTiledData = TRUE;
  }

  // copy data from real buffers to
  // bounce buffers if necessary
  if (pTiledParm != pParm)
    memcpy(pTiledParm, pParm, *pdwParmLen);

  if (pTiledData != pData)
    memcpy(pTiledData, pData, *pdwDataLen);


  rc = DosDevIOCtl( (HFILE)hFile, dwCat, dwFunc,
                     pParm, dwParmMaxLen, pdwParmLen,
                     pData, dwDataMaxLen, pdwDataLen);

  // copy data from bounce buffers to real
  // target buffers if necessary
  if (pTiledParm != pParm)
    memcpy(pParm, pTiledParm, *pdwParmLen);

  if (pTiledData != pData)
    memcpy(pData, pTiledData, *pdwDataLen);


  _exit_ioctl:

  // deallocate bounce buffers
  if (flagTiledParm)
    DosFreeMem(pTiledParm);

  if (flagTiledData)
    DosFreeMem(pTiledData);

  rc = error2WinError(rc, ERROR_INVALID_HANDLE);
  SetLastError(rc);
  return (DWORD)rc;
}

ULONG OSLibDosQueryModuleName(ULONG hModule, int cchName, char *pszName)
{
    char pszOemName[CCHMAXPATH];
    APIRET rc;

    rc = DosQueryModuleName(hModule, sizeof(pszOemName), pszOemName);
    if(rc == NO_ERROR)
    {
       //Convert file name from OS/2 to Windows
       if (!OemToCharBuffA(pszOemName, pszName, cchName))
           return GetLastError();
    }

    rc = error2WinError(rc,ERROR_INVALID_HANDLE);
    SetLastError(rc);
    return rc;
}

HINSTANCE OSLibDosLoadModule(LPSTR szModName)
{
 APIRET  rc;
 HMODULE hModule = NULLHANDLE;
 char    name[ CCHMAXPATH ];
 char    szOemModName[CCHMAXPATH];

  //Convert file name from Windows to OS/2 codepage
  CharToOemA(ODINHelperStripUNC(szModName), szOemModName);

  rc = DosLoadModule(name, CCHMAXPATH, szOemModName, &hModule);
  if(rc) {
      dprintf(("DosLoadModule([%s]) failed with %d and [%s]",
               szModName, rc, name));
      if (rc == ERROR_FILE_NOT_FOUND &&
          stricmp(szOemModName, name) != 0) {
          // the DLL could not be loaded due to some missing import module;
          // Win32 seems to report this with ERROR_MOD_NOT_FOUND, mimic this
          // behavior
          SetLastError(ERROR_MOD_NOT_FOUND);
      } else {
          SetLastError(error2WinError(rc,ERROR_FILE_NOT_FOUND));
      }
      return 0;
  }
  SetLastError(ERROR_SUCCESS_W);
  return hModule;
}
//******************************************************************************
//******************************************************************************
void OSLibDosFreeModule(HINSTANCE hinst)
{
  DosFreeModule(hinst);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibDosQuerySysInfo(ULONG iStart, ULONG iLast, PVOID pBuf, ULONG cbBuf)
{
  APIRET rc;

  rc = DosQuerySysInfo(iStart, iLast, pBuf, cbBuf);
  SetLastError(error2WinError(rc,ERROR_INVALID_HANDLE));
  return rc;
}
//******************************************************************************
//returned length is number of characters required or used for current dir
//*excluding* terminator
//******************************************************************************
ULONG OSLibDosQueryDir(DWORD length, LPSTR lpszCurDir)
{
  ULONG  drivemap, currentdisk, len;
  char  *lpszCurDriveAndDir = lpszCurDir +3;
  APIRET rc;

   len = (length > 3) ? length - 3 : 0;

   rc = DosQueryCurrentDir(0, lpszCurDriveAndDir, &len);
   if(rc != ERROR_BUFFER_OVERFLOW)
   {
        if(rc)
        {
             SetLastError(error2WinError(rc,ERROR_INVALID_PARAMETER));
             return 0;
        }
        len = strlen(lpszCurDriveAndDir) + 3;

        // Dir returned by DosQueryCurDir doesn't include drive, so add it
        DosQueryCurrentDisk(&currentdisk, &drivemap);

        if(isupper(lpszCurDir[3])) {
             lpszCurDir[0] = (char)('A' - 1 + currentdisk);
        }
        else lpszCurDir[0] = (char)('a' - 1 + currentdisk);

        lpszCurDir[1] = ':';
        lpszCurDir[2] = '\\';
   }
   else len += 3;   // + 3 since DosQueryCurDir doesn't include drive

   SetLastError(ERROR_SUCCESS_W);
   return len;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosBeep(DWORD ulFreq, DWORD ulDuration)
{
   APIRET   rc;

   if (ulDuration == -1)
   {
      SetLastError(ERROR_NOT_SUPPORTED_W);
      return FALSE;
   }
   rc = DosBeep(ulFreq, ulDuration);
   SetLastError(error2WinError(rc,ERROR_INVALID_HANDLE));

   return (rc == 0);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibDosGetModuleFileName(HMODULE hModule, LPTSTR lpszPath, DWORD cchPath)
{
   PTIB pTIB;
   PPIB pPIB;
   APIRET rc;
   char lpszOemPath[CCHMAXPATH];

   if(hModule == -1 || hModule == 0)
   {
      DosGetInfoBlocks(&pTIB, &pPIB);
      hModule = pPIB->pib_hmte;
   }
   *lpszPath = 0;
   rc = DosQueryModuleName(hModule, sizeof(lpszOemPath), lpszOemPath);

   if(rc == NO_ERROR)
   {
      //Convert file name from OS/2 to Windows
      if (!OemToCharBuffA(lpszOemPath, lpszPath, cchPath))
          return 0;
   }

   DWORD len = strlen(lpszPath);
   SetLastError(error2WinError(rc,ERROR_INVALID_HANDLE));
   return len;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibDosGetProcAddress(HMODULE hModule, LPCSTR lpszProc)
{
   APIRET  rc;
   PFN     pfn = NULL;
   ULONG   ordinal = (((ULONG)lpszProc) <= 0xFFFF) ? (ULONG)lpszProc : 0;
   HMODULE hmod = (HMODULE)hModule;

   rc = DosQueryProcAddr( hmod, ordinal, (PSZ)lpszProc, &pfn );
   if(rc == ERROR_INVALID_HANDLE && pfn == NULL) {
      CHAR pszError[32], pszModule[CCHMAXPATH];

      if(DosQueryModuleName( hmod, CCHMAXPATH, pszModule ) == 0 &&
         DosLoadModule( pszError, 32, pszModule, &hmod ) == 0)
      {
          rc = DosQueryProcAddr(hmod, ordinal, (PSZ)lpszProc, &pfn);
      }
   }
   SetLastError(error2WinError(rc,ERROR_INVALID_HANDLE));

   return (ULONG)pfn;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosSetThreadAffinity(DWORD dwThreadAffinityMask)
{
  static PROC_DosSetThreadAffinity pfnDosSetThreadAffinity = NULL;
  static BOOL fInit = FALSE;
  MPAFFINITY  mask;
  APIRET      rc;

    if(fInit == FALSE && pfnDosSetThreadAffinity == NULL)
    {
        ULONG nrCPUs;

        rc = DosQuerySysInfo(QSV_NUMPROCESSORS, QSV_NUMPROCESSORS, &nrCPUs, sizeof(nrCPUs));
        if (rc != 0 || nrCPUs == 1)
        {//not supported
            fInit = TRUE;
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
        }

        HMODULE hDoscalls;
        if(DosQueryModuleHandleStrict("DOSCALLS", &hDoscalls) == NO_ERROR) {
            DosQueryProcAddr(hDoscalls, 564, NULL, (PFN *)&pfnDosSetThreadAffinity);
        }
        fInit = TRUE;
    }
    if(fInit && pfnDosSetThreadAffinity == NULL) {
        SetLastError(ERROR_SUCCESS_W);
        return TRUE;
    }
    USHORT sel = RestoreOS2FS();

    mask.mask[0] = dwThreadAffinityMask;
    mask.mask[1] = 0; //TODO: this might not be a good idea, but then again, not many people have > 32 cpus

    rc = pfnDosSetThreadAffinity(&mask);
    SetFS(sel);

    SetLastError(error2WinError(rc,ERROR_INVALID_PARAMETER));
    return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibDosQueryAffinity(DWORD fMaskType, DWORD *pdwThreadAffinityMask)
{
  static PROC_DosQueryThreadAffinity pfnDosQueryThreadAffinity = NULL;
  static BOOL fInit = FALSE;
  MPAFFINITY  mask;
  APIRET      rc;

    if(fInit == FALSE && pfnDosQueryThreadAffinity == NULL)
    {
        ULONG nrCPUs;

        rc = DosQuerySysInfo(QSV_NUMPROCESSORS, QSV_NUMPROCESSORS, &nrCPUs, sizeof(nrCPUs));
        if (rc != 0 || nrCPUs == 1)
        {//not supported
            fInit = TRUE;
            *pdwThreadAffinityMask = 1;
            SetLastError(ERROR_SUCCESS_W);
            return TRUE;
        }

        HMODULE hDoscalls;
        if(DosQueryModuleHandleStrict("DOSCALLS", &hDoscalls) == NO_ERROR) {
            DosQueryProcAddr(hDoscalls, 563, NULL, (PFN *)&pfnDosQueryThreadAffinity);
        }
        fInit = TRUE;
    }
    if(fInit && pfnDosQueryThreadAffinity == NULL) {
        *pdwThreadAffinityMask = 1;
        SetLastError(ERROR_SUCCESS_W);
        return TRUE;
    }

    ULONG scope = (fMaskType == MASK_SYSTEM) ? AFNTY_SYSTEM : AFNTY_THREAD;

    USHORT sel = RestoreOS2FS();

    rc = pfnDosQueryThreadAffinity(scope, &mask);
    SetFS(sel);

    if(rc == NO_ERROR) {
         *pdwThreadAffinityMask = mask.mask[0];
    }

    SetLastError(error2WinError(rc,ERROR_INVALID_PARAMETER));
    return (rc == NO_ERROR);
}
//******************************************************************************
//******************************************************************************
void  OSLibDosSleep(ULONG msecs)
{
  DosSleep(msecs);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosDevConfig(PVOID pdevinfo,
                         ULONG item)
{
  // DosDevConfig() is not high-mem safe, use a low-mem stack variable
  // (according to CPREF, "All returned device information is BYTE-sized,
  // so [the argument] should be the address of a BYTE variable"
  BYTE devinfo;
  DWORD rc = (DWORD)DosDevConfig(&devinfo, item);
  *((PBYTE)pdevinfo) = devinfo;
  return rc;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosGetNumPhysDrives()
{
    USHORT  usNumDrives  = 0;                  /* Data return buffer        */
    ULONG   ulDataLen    = sizeof(USHORT);     /* Data return buffer length */
    APIRET  rc           = NO_ERROR;           /* Return code               */

    /* Request a count of the number of partitionable disks in the system */

    rc = DosPhysicalDisk(INFO_COUNT_PARTITIONABLE_DISKS,
                         &usNumDrives,
                         ulDataLen,
                         NULL,         /* No parameter for this function */
                         0L);

    if (rc != NO_ERROR) {
        dprintf(("DosPhysicalDisk error: return code = %u\n", rc));
        return 0;
    }
    else {
        dprintf(("DosPhysicalDisk:  %u partitionable disk(s)\n",usNumDrives));
    }
    return usNumDrives;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosQueryHType(ULONG handle, PULONG pAttr)
{
    ULONG type, attr;
    APIRET rc = DosQueryHType(handle, &type, pAttr ? pAttr : &attr);
    if (rc != NO_ERROR) {
        dprintf(("DosQueryHType error: return code = %u\n", rc));
        return -1;
    }
    return type;
}
//******************************************************************************
//******************************************************************************

/* sdbm:
   This algorithm was created for sdbm (a public-domain reimplementation of
   ndbm) database library. it was found to do well in scrambling bits,
   causing better distribution of the keys and fewer splits. it also happens
   to be a good general hashing function with good distribution. the actual
   function is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below
   is the faster version used in gawk. [there is even a faster, duff-device
   version] the magic constant 65599 was picked out of thin air while
   experimenting with different constants, and turns out to be a prime.
   this is one of the algorithms used in berkeley db (see sleepycat) and
   elsewhere. */
static ULONG sdbm(const char *str)
{
    ULONG hash = 0;
    ULONG c;

    while ((c = *(unsigned const char *)str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

/*-
 *  COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or
 *  code or tables extracted from it, as desired without restriction.
 *
 *  First, the polynomial itself and its table of feedback terms.  The
 *  polynomial is
 *  X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0
 *
 *  Note that we take it "backwards" and put the highest-order term in
 *  the lowest-order bit.  The X^32 term is "implied"; the LSB is the
 *  X^31 term, etc.  The X^0 term (usually shown as "+1") results in
 *  the MSB being 1
 *
 *  Note that the usual hardware shift register implementation, which
 *  is what we're using (we're merely optimizing it by doing eight-bit
 *  chunks at a time) shifts bits into the lowest-order term.  In our
 *  implementation, that means shifting towards the right.  Why do we
 *  do it this way?  Because the calculated CRC must be transmitted in
 *  order from highest-order term to lowest-order term.  UARTs transmit
 *  characters in order from LSB to MSB.  By storing the CRC this way
 *  we hand it to the UART in the order low-byte to high-byte; the UART
 *  sends each low-bit to hight-bit; and the result is transmission bit
 *  by bit from highest- to lowest-order term without requiring any bit
 *  shuffling on our part.  Reception works similarly
 *
 *  The feedback terms table consists of 256, 32-bit entries.  Notes
 *
 *      The table can be generated at runtime if desired; code to do so
 *      is shown later.  It might not be obvious, but the feedback
 *      terms simply represent the results of eight shift/xor opera
 *      tions for all combinations of data and CRC register values
 *
 *      The values must be right-shifted by eight bits by the "updcrc
 *      logic; the shift must be unsigned (bring in zeroes).  On some
 *      hardware you could probably optimize the shift in assembler by
 *      using byte-swap instructions
 *      polynomial $edb88320
 *
 *
 * CRC32 code derived from work by Gary S. Brown.
 */

static const ULONG crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static ULONG
crc32str(const char *psz)
{
	const UCHAR *p;
	ULONG crc;

	p = (const UCHAR *)psz;
	crc = ~0U;

	while (*p)
		crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

	return crc ^ ~0U;
}

} // extern "C"

