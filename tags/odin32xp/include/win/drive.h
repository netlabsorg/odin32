/*
 * DOS drive handling declarations
 *
 * Copyright 1995 Alexandre Julliard
 */

#ifndef __WINE_DRIVE_H
#define __WINE_DRIVE_H

#include "windef.h"
#include <direct.h>

#define MAX_DOS_DRIVES  26

typedef enum
{
    TYPE_FLOPPY,
    TYPE_HD,
    TYPE_CDROM,
    TYPE_NETWORK,
    TYPE_INVALID
} DRIVETYPE;

/* Drive flags */

#define DRIVE_DISABLED        0x0001  /* Drive is disabled */
#define DRIVE_SHORT_NAMES     0x0002  /* Drive fs has 8.3 file names */
#define DRIVE_CASE_SENSITIVE  0x0004  /* Drive fs is case sensitive */
#define DRIVE_CASE_PRESERVING 0x0008  /* Drive fs is case preserving */

inline int DRIVE_SetCurrentDrive(char drive)
{
   int ret;
   USHORT sel = GetFS();
   ret = _chdrive(drive);
   SetFS(sel);
   return ret;
}

inline int DRIVE_GetCurrentDrive()     
{
   int ret;
   USHORT sel = GetFS();
   ret = _getdrive();
   SetFS(sel);
   return ret;
}

#define DRIVE_Chdir(a,b)             SetCurrentDirectoryA(b)

inline char *DRIVE_GetDosCwd (char *buffer, int drive, int size)
{
   char * ret;
   USHORT sel = GetFS();
   ret = _getdcwd(drive, buffer, size);
   SetFS(sel);
   return ret;
}

#if 0
extern int DRIVE_Init(void);
extern int DRIVE_IsValid( int drive );
extern int DRIVE_GetCurrentDrive(void);
extern int DRIVE_SetCurrentDrive( int drive );
extern int DRIVE_FindDriveRoot( const char **path );
extern const char * DRIVE_GetRoot( int drive );
extern const char * DRIVE_GetDosCwd( int drive );
extern const char * DRIVE_GetUnixCwd( int drive );
extern const char * DRIVE_GetLabel( int drive );
extern DWORD DRIVE_GetSerialNumber( int drive );
extern int DRIVE_SetSerialNumber( int drive, DWORD serial );
extern DRIVETYPE DRIVE_GetType( int drive );
extern UINT DRIVE_GetFlags( int drive );
extern int DRIVE_Chdir( int drive, const char *path );
extern int DRIVE_Disable( int drive  );
extern int DRIVE_Enable( int drive  );
extern int DRIVE_SetLogicalMapping ( int existing_drive, int new_drive );
extern int DRIVE_OpenDevice( int drive, int flags );
extern int DRIVE_RawRead(BYTE drive, DWORD begin, DWORD length, BYTE *dataptr, BOOL fake_success );
extern int DRIVE_RawWrite(BYTE drive, DWORD begin, DWORD length, BYTE *dataptr, BOOL fake_success );
#endif

#endif  /* __WINE_DRIVE_H */
