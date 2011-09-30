/* $Id: osliblvm.h,v 1.6 2003-01-20 10:46:27 sandervl Exp $ */
/*
 * OS/2 LVM (Logical Volume Management) functions
 *
 * Copyright 2002 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBLVM_H__
#define __OSLIBLVM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <win\winioctl.h>

#ifdef OS2_INCLUDED

typedef unsigned short int CARDINAL16;
typedef unsigned long      CARDINAL32;
typedef unsigned int       CARDINAL;
typedef unsigned char      BOOLEAN;
typedef short int          INTEGER16;
typedef long  int          INTEGER32;
typedef int                INTEGER;
typedef unsigned long      DoubleWord;
typedef short unsigned int Word;
typedef unsigned long      LBA;

/* Define a Partition Sector Number.  A Partition Sector Number is 
   relative to the start of a partition. The first sector in a partition 
   is PSN 0. */
typedef unsigned long      PSN;

/* Define the size of a Partition Name.  Partition Names are user defined names given to a partition. */
#define PARTITION_NAME_SIZE  20

/* Define the size of a volume name.  Volume Names are user defined names given to a volume. */
#define VOLUME_NAME_SIZE  20

/* Define the size of a disk name.  Disk Names are user defined names given to physical disk drives in the system. */
#define DISK_NAME_SIZE    20

/* The name of the filesystem in use on a partition.  This name may be up to 12 ( + NULL terminator) characters long. */
#define FILESYSTEM_NAME_SIZE 20

/* The number of bytes in a sector on the disk. */
#define BYTES_PER_SECTOR  512

/* The following are invariant for a disk drive. */
typedef struct _Drive_Control_Record {
    CARDINAL32   Drive_Number;        /* OS/2 Drive Number for this drive. */
    CARDINAL32   Drive_Size;          /* The total number of sectors on the drive. */
    DoubleWord   Drive_Serial_Number; /* The serial number assigned to this drive.
                                         For info. purposes only. */
    ULONG        Drive_Handle;        /* Handle used for operations on the disk that
                                         this record corresponds to. */
    CARDINAL32   Cylinder_Count;      /* The number of cylinders on the drive. */
    CARDINAL32   Heads_Per_Cylinder;  /* The number of heads per cylinder for this drive. */
    CARDINAL32   Sectors_Per_Track;   /* The number of sectors per track for this drive. */
    BOOLEAN      Drive_Is_PRM;        /* Set to TRUE if this drive is a PRM. */
    BYTE         Reserved[3];         /* Alignment. */
} Drive_Control_Record;

/* The following structure is returned by the Get_Drive_Control_Data function. */
typedef struct _Drive_Control_Array{
    Drive_Control_Record *   Drive_Control_Data;       /* An array of drive control records. */
    CARDINAL32               Count;                    /* The number of entries in the array
                                                          of drive control records. */
} Drive_Control_Array;


/* The following structure defines the information that can be changed for a specific disk drive. */
typedef struct _Drive_Information_Record {
    CARDINAL32   Total_Available_Sectors;        /* The number of sectors on the disk which are not currently assigned to a partition. */
    CARDINAL32   Largest_Free_Block_Of_Sectors;  /* The number of sectors in the largest contiguous block of available sectors.  */
    BOOLEAN      Corrupt_Partition_Table;        /* If TRUE, then the partitioning information found on the drive is incorrect! */
    BOOLEAN      Unusable;                       /* If TRUE, the drive's MBR is not accessible and the drive can not be partitioned. */
    BOOLEAN      IO_Error;                       /* If TRUE, then the last I/O operation on this drive failed! */
    BOOLEAN      Is_Big_Floppy;                  /* If TRUE, then the drive is a PRM formatted as a big floppy (i.e. the old style removable media support). */
    char         Drive_Name[DISK_NAME_SIZE];     /* User assigned name for this disk drive. */
} Drive_Information_Record;

typedef struct _Partition_Information_Record {
    ULONG        Partition_Handle;                      /* The handle used to perform
                                                           operations on this partition. */
    ULONG        Volume_Handle;                         /* If this partition is part
                                                           of a volume, this will be the
                                                           handle of the volume.  If
                                                           this partition is NOT
                                                           part of a volume, then
                                                           this handle will be 0.        */
    ULONG        Drive_Handle;                          /* The handle for the drive
                                                           this partition resides on. */
    DoubleWord   Partition_Serial_Number;               /* The serial number assigned
                                                           to this partition.         */
    CARDINAL32   Partition_Start;                       /* The LBA of the first
                                                           sector of the partition. */
    CARDINAL32   True_Partition_Size;                   /* The total number of
                                                           sectors comprising the partition. */
    CARDINAL32   Usable_Partition_Size;                 /* The size of the partition
                                                           as reported to the IFSM.  This is the
                                                           size of the partition less
                                                           any LVM overhead.                  */
    CARDINAL32   Boot_Limit;                            /* The maximum number of
                                                           sectors from this block
                                                           of free space that can be
                                                           used to create a bootable
                                                           partition if you allocate
                                                           from the beginning of the block
                                                           of free space.             */
    BOOLEAN      Spanned_Volume;                        /* TRUE if this partition is
                                                           part of a multi-partition
                                                           volume.                          */
    BOOLEAN      Primary_Partition;                     /* True or False.  Any
                                                           non-zero value here indicates
                                                           that this partition is
                                                           a primary partition.  Zero
                                                           here indicates that this
                                                           partition is a "logical drive"
                                                           - i.e. it resides inside of
                                                           an extended partition. */
    BYTE         Active_Flag;                           /* 80 = Partition is marked
                                                           as being active.
                                                           0 = Partition is not
                                                           active.                   */
    BYTE         OS_Flag;                               /* This field is from the
                                                           partition table.  It is
                                                           known as the OS flag, the
                                                           Partition Type Field,
                                                           Filesystem Type, and
                                                           various other names.
                                                           Values of interest
                                                           If this field is: (values
                                                           are in hex)
                                                           07 = The partition is a
                                                                compatibility
                                                                partition formatted
                                                                for use with an
                                                                installable
                                                                filesystem, such as
                                                                HPFS or JFS.
                                                           00 = Unformatted partition
                                                           01 = FAT12 filesystem is
                                                                in use on this
                                                                partition.
                                                           04 = FAT16 filesystem is
                                                                in use on this
                                                                partition.
                                                           0A = OS/2 Boot Manager
                                                                Partition
                                                           35 = LVM partition
                                                           06 = OS/2 FAT16 partition    */
    BYTE         Partition_Type;                        /* 0 = Free Space
                                                           1 = LVM Partition (Part of
                                                               an LVM Volume.)
                                                           2 = Compatibility Partition
                                                               All other values are reserved
                                                               for future use. */
    BYTE         Partition_Status;                      /* 0 = Free Space
                                                           1 = In Use - i.e. already
                                                               assigned to a volume.
                                                           2 = Available - i.e. not
                                                               currently assigned
                                                               to a volume. */
    BOOLEAN      On_Boot_Manager_Menu;                  /* Set to TRUE if this
                                                           partition is not part of
                                                           a Volume yet is on the
                                                           Boot Manager Menu.       */
    BYTE         Reserved;                              /* Alignment. */
    char         Volume_Drive_Letter;                   /* The drive letter assigned
                                                           to the volume that this
                                                           partition is a part of. */
    char         Drive_Name[DISK_NAME_SIZE];            /* User assigned name for
                                                           this disk drive. */
    char         File_System_Name[FILESYSTEM_NAME_SIZE];/* The name of the filesystem
                                                           in use on this partition,
                                                           if it is known. */
    char         Partition_Name[PARTITION_NAME_SIZE];   /* The user assigned name for
                                                           this partition. */
    char         Volume_Name[VOLUME_NAME_SIZE];         /* If this partition is part
                                                           of a volume, then this
                                                           will be the name of the
                                                           volume that this partition
                                                           is a part of.  If this record
                                                           represents free space,
                                                           then the Volume_Name will be
                                                           "FS xx", where xx is a unique
                                                           numeric ID generated by
                                                           LVM.DLL.  Otherwise it
                                                           will be an empty string.     */
} Partition_Information_Record;

/* The following defines are for use with the Partition_Type field in the Partition_Information_Record. */
#define FREE_SPACE_PARTITION     0
#define LVM_PARTITION            1
#define COMPATIBILITY_PARTITION  2

/* The following defines are for use with the Partition_Status field in the Partition_Information_Record. */
#define PARTITION_IS_IN_USE      1
#define PARTITION_IS_AVAILABLE   2
#define PARTITION_IS_FREE_SPACE  0


/* The following structure is returned by various functions in the LVM Engine. */
typedef struct _Partition_Information_Array {
    Partition_Information_Record * Partition_Array; /* An array of Partition_Information_Records. */
    CARDINAL32                     Count;           /* The number of entries in the Partition_Array. */
} Partition_Information_Array;

/* The following items are invariant for a volume. */
typedef struct _Volume_Control_Record {
    DoubleWord Volume_Serial_Number;            /* The serial number assigned to this volume. */
    ULONG      Volume_Handle;                   /* The handle used to perform operations on this volume. */
    BOOLEAN    Compatibility_Volume;            /* TRUE indicates that this volume is compatible with older versions of OS/2.
                                                   FALSE indicates that this is an LVM specific volume and can not be used without OS2LVM.DMD. */
    BYTE       Device_Type;                     /* Indicates what type of device the Volume resides on:
                                                   0 = Hard Drive under LVM Control
                                                   1 = PRM under LVM Control
                                                   2 = CD-ROM
                                                   3 = Network drive
                                                   4 = Unknown device NOT under LVM Control
                                                */
    BYTE       Reserved[2];                     /* Alignment. */
} Volume_Control_Record;

/* The following define the device types used in the Device_Type field of the Volume_Control_Record. */
#define LVM_HARD_DRIVE  0
#define LVM_PRM         1
#define NON_LVM_CDROM   2
#define NETWORK_DRIVE   3
#define NON_LVM_DEVICE  4

/* The following structure is returned by the Get_Volume_Control_Data function. */
typedef struct _Volume_Control_Array{
    Volume_Control_Record *  Volume_Control_Data;      /* An array of volume control records. */
    CARDINAL32               Count;                    /* The number of entries in the array of volume control records. */
} Volume_Control_Array;


/* The following information about a volume can (and often does) vary. */
typedef struct _Volume_Information_Record {
    CARDINAL32 Volume_Size;                           /* The number of sectors comprising the volume. */
    CARDINAL32 Partition_Count;                       /* The number of partitions which comprise this volume. */
    CARDINAL32 Drive_Letter_Conflict;                 /* 0 indicates that the drive letter preference for this volume is unique.
                                                         1 indicates that the drive letter preference for this volume
                                                           is not unique, but this volume got its preferred drive letter anyway.
                                                         2 indicates that the drive letter preference for this volume
                                                           is not unique, and this volume did NOT get its preferred drive letter.
                                                         4 indicates that this volume is currently "hidden" - i.e. it has
                                                           no drive letter preference at the current time.                        */
    BOOLEAN    Compatibility_Volume;                  /* TRUE if this is for a compatibility volume, FALSE otherwise. */
    BOOLEAN    Bootable;                              /* Set to TRUE if this volume appears on the Boot Manager menu, or if it is
                                                         a compatibility volume and its corresponding partition is the first active
                                                         primary partition on the first drive.                                         */
    char       Drive_Letter_Preference;               /* The drive letter that this volume desires to be. */
    char       Current_Drive_Letter;                  /* The drive letter currently used to access this volume.  May be different than
                                                         Drive_Letter_Preference if there was a conflict ( i.e. Drive_Letter_Preference
                                                         is already in use by another volume ).                                          */
    char       Initial_Drive_Letter;                  /* The drive letter assigned to this volume by the operating system when LVM was started.
                                                         This may be different from the Drive_Letter_Preference if there were conflicts, and
                                                         may be different from the Current_Drive_Letter.  This will be 0x0 if the Volume did
                                                         not exist when the LVM Engine was opened (i.e. it was created during this LVM session). */
    BOOLEAN    New_Volume;                            /* Set to FALSE if this volume existed before the LVM Engine was opened.  Set to
                                                         TRUE if this volume was created after the LVM Engine was opened.                */
    BYTE       Status;                                /* 0 = None.
                                                         1 = Bootable
                                                         2 = Startable
                                                         3 = Installable.           */
    BYTE       Reserved_1;
    char       Volume_Name[VOLUME_NAME_SIZE];         /* The user assigned name for this volume. */
    char       File_System_Name[FILESYSTEM_NAME_SIZE];/* The name of the filesystem in use on this partition, if it is known. */
} Volume_Information_Record;


/* Error codes returned by the LVM Engine. */
#define LVM_ENGINE_NO_ERROR                           0
#define LVM_ENGINE_OUT_OF_MEMORY                      1
#define LVM_ENGINE_IO_ERROR                           2
#define LVM_ENGINE_BAD_HANDLE                         3
#define LVM_ENGINE_INTERNAL_ERROR                     4
#define LVM_ENGINE_ALREADY_OPEN                       5
#define LVM_ENGINE_NOT_OPEN                           6
#define LVM_ENGINE_NAME_TOO_BIG                       7
#define LVM_ENGINE_OPERATION_NOT_ALLOWED              8
#define LVM_ENGINE_DRIVE_OPEN_FAILURE                 9
#define LVM_ENGINE_BAD_PARTITION                     10
#define LVM_ENGINE_CAN_NOT_MAKE_PRIMARY_PARTITION    11
#define LVM_ENGINE_TOO_MANY_PRIMARY_PARTITIONS       12
#define LVM_ENGINE_CAN_NOT_MAKE_LOGICAL_DRIVE        13
#define LVM_ENGINE_REQUESTED_SIZE_TOO_BIG            14
#define LVM_ENGINE_1024_CYLINDER_LIMIT               15
#define LVM_ENGINE_PARTITION_ALIGNMENT_ERROR         16
#define LVM_ENGINE_REQUESTED_SIZE_TOO_SMALL          17
#define LVM_ENGINE_NOT_ENOUGH_FREE_SPACE             18
#define LVM_ENGINE_BAD_ALLOCATION_ALGORITHM          19
#define LVM_ENGINE_DUPLICATE_NAME                    20
#define LVM_ENGINE_BAD_NAME                          21
#define LVM_ENGINE_BAD_DRIVE_LETTER_PREFERENCE       22
#define LVM_ENGINE_NO_DRIVES_FOUND                   23
#define LVM_ENGINE_WRONG_VOLUME_TYPE                 24
#define LVM_ENGINE_VOLUME_TOO_SMALL                  25
#define LVM_ENGINE_BOOT_MANAGER_ALREADY_INSTALLED    26
#define LVM_ENGINE_BOOT_MANAGER_NOT_FOUND            27
#define LVM_ENGINE_INVALID_PARAMETER                 28
#define LVM_ENGINE_BAD_FEATURE_SET                   29
#define LVM_ENGINE_TOO_MANY_PARTITIONS_SPECIFIED     30
#define LVM_ENGINE_LVM_PARTITIONS_NOT_BOOTABLE       31
#define LVM_ENGINE_PARTITION_ALREADY_IN_USE          32
#define LVM_ENGINE_SELECTED_PARTITION_NOT_BOOTABLE   33
#define LVM_ENGINE_VOLUME_NOT_FOUND                  34
#define LVM_ENGINE_DRIVE_NOT_FOUND                   35
#define LVM_ENGINE_PARTITION_NOT_FOUND               36
#define LVM_ENGINE_TOO_MANY_FEATURES_ACTIVE          37
#define LVM_ENGINE_PARTITION_TOO_SMALL               38
#define LVM_ENGINE_MAX_PARTITIONS_ALREADY_IN_USE     39
#define LVM_ENGINE_IO_REQUEST_OUT_OF_RANGE           40
#define LVM_ENGINE_SPECIFIED_PARTITION_NOT_STARTABLE 41
#define LVM_ENGINE_SELECTED_VOLUME_NOT_STARTABLE     42
#define LVM_ENGINE_EXTENDFS_FAILED                   43
#define LVM_ENGINE_REBOOT_REQUIRED                   44

#endif

#define VOLUME_NAME_PREFIX  "\\\\?\\Volume\\"

void   OSLibLVMExit();
HANDLE OSLibLVMQueryVolumeControlData();
void   OSLibLVMFreeVolumeControlData(HANDLE hVolumeControlData);
BOOL   OSLibLVMQueryVolumeName(HANDLE hVolumeControlData, ULONG *pVolIndex, 
                               LPSTR lpszVolumeName, DWORD cchBufferLength);

BOOL   OSLibLVMGetPartitionInfo(ULONG driveLetter, LPSTR lpszVolumeName, PPARTITION_INFORMATION pPartition);
BOOL   OSLibLVMGetVolumeExtents(ULONG driveLetter, LPSTR lpszVolumeName, PVOLUME_DISK_EXTENTS pVolExtent, BOOL *pfLVMVolume = NULL);

ULONG  OSLibLVMGetDriveType(LPCSTR lpszVolume);
CHAR   OSLibLVMQueryDriveFromVolumeName(LPCSTR lpszVolume);
BOOL   OSLibLVMGetVolumeNameForVolumeMountPoint(LPCSTR lpszVolumeMountPoint, LPSTR lpszVolumeName, 
                                                DWORD cchBufferLength);


DWORD  OSLibLVMQueryVolumeFS(LPSTR lpszVolume, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
DWORD  OSLibLVMQueryVolumeSerialAndName(LPSTR lpszVolume, LPDWORD lpVolumeSerialNumber, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize);

BOOL   OSLibLVMStripVolumeName(LPCSTR lpszWin32VolumeName, LPSTR lpszOS2VolumeName, DWORD cchBufferLength);
BOOL   OSLibLVMGetDiskGeometry(DWORD dwDiskNr, PDISK_GEOMETRY pGeom);

#ifdef __cplusplus
}
#endif

#endif //__OSLIBLVM_H__
