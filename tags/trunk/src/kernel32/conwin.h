/* $Id: conwin.h,v 1.1 1999-06-17 18:22:43 phaller Exp $ */

#ifndef _CONWIN_H_
#define _CONWIN_H_

#ifndef _FILETIME_
#define _FILETIME_
/* 64 bit number of 100 nanoseconds intervals since January 1, 1601 */
typedef struct
{
  DWORD  dwLowDateTime;
  DWORD  dwHighDateTime;
} FILETIME, *LPFILETIME;
#endif /* _FILETIME_ */

typedef struct
{
  int dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  int dwVolumeSerialNumber;
  int nFileSizeHigh;
  int nFileSizeLow;
  int nNumberOfLinks;
  int nFileIndexHigh;
  int nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION ;


#define OFS_MAXPATHNAME 128
typedef struct
{
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    BYTE reserved[4];
    BYTE szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *LPOFSTRUCT;


/* The security attributes structure */
typedef struct
{
    DWORD   nLength;
    LPVOID  lpSecurityDescriptor;
    BOOL  bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#endif

// PH: unfortunately, ODIN/WINE headers don't mix with OS/2 headers
#define STD_INPUT_HANDLE         ((DWORD)-10)
#define STD_OUTPUT_HANDLE        ((DWORD)-11)
#define STD_ERROR_HANDLE         ((DWORD)-12)
#define GENERIC_READ                 0x80000000
#define GENERIC_WRITE                0x40000000
#define FILE_SHARE_READ              0x00000001
#define FILE_SHARE_WRITE             0x00000002

#endif /* _CONWIN_H_ */

