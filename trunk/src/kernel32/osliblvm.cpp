/* $Id: osliblvm.cpp,v 1.1 2002-05-09 13:55:34 sandervl Exp $ */

/*
 * OS/2 LVM (Logical Volume Management) functions
 *
 * Copyright 2002 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSQUEUES
#define INCL_DOSMODULEMGR
#define INCL_DOSEXCEPTIONS
#define INCL_DOSERRORS
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <dbglog.h>
#include <win32type.h>
#include <winconst.h>
#include <win\winioctl.h>
#include "osliblvm.h"


#define DBG_LOCALLOG	DBG_osliblvm
#include "dbglocal.h"

static void (* SYSTEM pfnOpen_LVM_Engine)( BOOLEAN Ignore_CHS, CARDINAL32 * Error_Code );
static void (* SYSTEM pfnClose_LVM_Engine) ( void );
static Drive_Control_Array (* SYSTEM pfnGet_Drive_Control_Data)( CARDINAL32 * Error_Code );
static Drive_Information_Record (* SYSTEM pfnGet_Drive_Status)( ULONG Drive_Handle, CARDINAL32 * Error_Code );
static Partition_Information_Array (* SYSTEM pfnGet_Partitions)( ULONG Handle, CARDINAL32 * Error_Code );
static ULONG (* SYSTEM pfnGet_Partition_Handle)( CARDINAL32 Serial_Number, CARDINAL32 * Error_Code );
static Partition_Information_Record (* SYSTEM pfnGet_Partition_Information)( ULONG Partition_Handle, CARDINAL32 * Error_Code );
static Volume_Control_Array (* SYSTEM pfnGet_Volume_Control_Data)( CARDINAL32 * Error_Code );
static Volume_Information_Record (* SYSTEM pfnGet_Volume_Information)( ULONG Volume_Handle, CARDINAL32 * Error_Code );
static void (* SYSTEM pfnFree_Engine_Memory)( ULONG Object );
static void (* SYSTEM pfnRead_Sectors) ( CARDINAL32          Drive_Number,
                                         LBA                 Starting_Sector,
                                         CARDINAL32          Sectors_To_Read,
                                         ULONG               Buffer,
                                         CARDINAL32 *        Error);
static void (* SYSTEM pfnWrite_Sectors) ( CARDINAL32          Drive_Number,
                                          LBA                 Starting_Sector,
                                          CARDINAL32          Sectors_To_Write,
                                          ULONG               Buffer,
                                          CARDINAL32 *        Error);

static HMODULE hModLVM    = 0;
static BOOL    fLVMOpened = FALSE;

//******************************************************************************
//******************************************************************************
static void Open_LVM_Engine( BOOLEAN Ignore_CHS, CARDINAL32 * Error_Code )
{
    USHORT               sel;

    sel = RestoreOS2FS();
    pfnOpen_LVM_Engine(Ignore_CHS, Error_Code);
    SetFS(sel);
    return;
}
//******************************************************************************
//******************************************************************************
static void Close_LVM_Engine ( void )
{
    USHORT               sel;

    sel = RestoreOS2FS();
    pfnClose_LVM_Engine();
    SetFS(sel);
    return;
}
//******************************************************************************
//******************************************************************************
static Drive_Control_Array Get_Drive_Control_Data( CARDINAL32 * Error_Code )
{
    Drive_Control_Array ret;
    USHORT              sel;

    sel = RestoreOS2FS();
    ret = pfnGet_Drive_Control_Data(Error_Code);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
static Drive_Information_Record Get_Drive_Status( ULONG Drive_Handle, CARDINAL32 * Error_Code )
{
    Drive_Information_Record ret;
    USHORT               sel;

    sel = RestoreOS2FS();
    ret = pfnGet_Drive_Status(Drive_Handle, Error_Code);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
static Partition_Information_Array Get_Partitions( ULONG Handle, CARDINAL32 * Error_Code )
{
    Partition_Information_Array ret;
    USHORT               sel;

    sel = RestoreOS2FS();
    ret = pfnGet_Partitions(Handle, Error_Code);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
static ULONG Get_Partition_Handle( CARDINAL32 Serial_Number, CARDINAL32 * Error_Code )
{
    ULONG  ret;
    USHORT sel;

    sel = RestoreOS2FS();
    ret = pfnGet_Partition_Handle(Serial_Number, Error_Code);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
static Partition_Information_Record Get_Partition_Information( ULONG Partition_Handle, CARDINAL32 * Error_Code )
{
    Partition_Information_Record ret;
    USHORT               sel;

    sel = RestoreOS2FS();
    ret = pfnGet_Partition_Information(Partition_Handle, Error_Code);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
static Volume_Control_Array Get_Volume_Control_Data( CARDINAL32 * Error_Code )
{
    Volume_Control_Array ret;
    USHORT               sel;

    sel = RestoreOS2FS();
    ret = pfnGet_Volume_Control_Data(Error_Code);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
static Volume_Information_Record Get_Volume_Information( ULONG Volume_Handle, CARDINAL32 * Error_Code )
{
    Volume_Information_Record ret;
    USHORT                    sel;

    sel = RestoreOS2FS();
    ret = pfnGet_Volume_Information(Volume_Handle, Error_Code);
    SetFS(sel);
    return ret;
}
//******************************************************************************
//******************************************************************************
static void Free_Engine_Memory( ULONG Object )
{
    USHORT               sel;

    sel = RestoreOS2FS();
    pfnFree_Engine_Memory(Object);
    SetFS(sel);
    return;
}
//******************************************************************************
//******************************************************************************
static void Read_Sectors ( CARDINAL32          Drive_Number,
                           LBA                 Starting_Sector,
                           CARDINAL32          Sectors_To_Read,
                           ULONG               Buffer,
                           CARDINAL32 *        Error)
{
    USHORT               sel;

    sel = RestoreOS2FS();
    pfnRead_Sectors(Drive_Number, Starting_Sector, Sectors_To_Read, Buffer, Error);
    SetFS(sel);
    return;
}
//******************************************************************************
//******************************************************************************
static void Write_Sectors ( CARDINAL32          Drive_Number,
                            LBA                 Starting_Sector,
                            CARDINAL32          Sectors_To_Write,
                            ULONG               Buffer,
                            CARDINAL32 *        Error)
{
    USHORT               sel;

    sel = RestoreOS2FS();
    pfnWrite_Sectors(Drive_Number, Starting_Sector, Sectors_To_Write, Buffer, Error);
    SetFS(sel);
    return;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibLVMInit()
{
    APIRET rc;
    CHAR   szModuleFailure[CCHMAXPATH];

    rc = DosLoadModule(szModuleFailure, sizeof(szModuleFailure), "LVM.DLL", (HMODULE *)&hModLVM);
    if(rc) {
        return FALSE;
    }
    rc = DosQueryProcAddr(hModLVM, 0, "Open_LVM_Engine", (PFN *)&pfnOpen_LVM_Engine);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Close_LVM_Engine", (PFN *)&pfnClose_LVM_Engine);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Get_Drive_Control_Data", (PFN *)&pfnGet_Drive_Control_Data);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Get_Drive_Status", (PFN *)&pfnGet_Drive_Status);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Get_Partitions", (PFN *)&pfnGet_Partitions);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Get_Partition_Handle", (PFN *)&pfnGet_Partition_Handle);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Get_Partition_Information", (PFN *)&pfnGet_Partition_Information);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Get_Volume_Control_Data", (PFN *)&pfnGet_Volume_Control_Data);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Get_Volume_Information", (PFN *)&pfnGet_Volume_Information);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Free_Engine_Memory", (PFN *)&pfnFree_Engine_Memory);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Read_Sectors", (PFN *)&pfnRead_Sectors);
    if(rc) goto fail;
    rc = DosQueryProcAddr(hModLVM, 0, "Write_Sectors", (PFN *)&pfnWrite_Sectors);
    if(rc) goto fail;
    
    return TRUE;

fail:
    if(hModLVM) {
        DosFreeModule(hModLVM);
        hModLVM = 0;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
void OSLibLVMExit()
{
    if(fLVMOpened) {
        Close_LVM_Engine();
    }
    if(hModLVM) {
        DosFreeModule(hModLVM);
        hModLVM = 0;
    }
}
//******************************************************************************
//******************************************************************************
HANDLE OSLibLVMQueryVolumeControlData()
{
    Volume_Control_Array *volctrl;
    CARDINAL32            lasterror;
    
    if(!hModLVM) {
        dprintf(("LVM dll not loaded -> fail"));
        return 0;
    }

    if(!fLVMOpened) {
        Open_LVM_Engine(FALSE, &lasterror);
        if(lasterror != LVM_ENGINE_NO_ERROR) {
            DebugInt3();
            return 0;
        }
        dprintf(("LVM engine opened"));
        fLVMOpened = TRUE;
    }

    volctrl = (Volume_Control_Array *)malloc(sizeof(Volume_Control_Array));
    if(volctrl == NULL) {
        DebugInt3();
        return 0;
    }
    *volctrl = Get_Volume_Control_Data(&lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return 0;
    }
    return (HANDLE)volctrl;
}
//******************************************************************************
//******************************************************************************
void OSLibLVMFreeVolumeControlData(HANDLE hVolumeControlData)
{
    Volume_Control_Array *volctrl = (Volume_Control_Array *)hVolumeControlData;

    if(volctrl == NULL) {
        DebugInt3();
        return;
    }
    Free_Engine_Memory((ULONG)volctrl->Volume_Control_Data);
    free(volctrl);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibLVMQueryVolumeName(HANDLE hVolumeControlData, ULONG volindex, 
                             LPSTR lpszVolumeName, DWORD cchBufferLength)
{
    Volume_Control_Array      *volctrl = (Volume_Control_Array *)hVolumeControlData;
    Volume_Information_Record  volinfo;
    CARDINAL32                 lasterror;

    if(volctrl == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(volindex >= volctrl->Count) {
        return FALSE;   //no more volumes
    }
    volinfo = Get_Volume_Information(volctrl->Volume_Control_Data[volindex].Volume_Handle, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return FALSE;
    }
    strncpy(lpszVolumeName, volinfo.Volume_Name, min(sizeof(volinfo.Volume_Name), cchBufferLength)-1);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
static Volume_Information_Record OSLibLVMFindVolumeFromDriveLetter(ULONG driveLetter,
                                                                   Volume_Control_Record *pVolRec,
                                                                   CARDINAL32 *lasterror) 
{
    Volume_Control_Array      *volctrl;
    Volume_Information_Record  volinfo;

    volctrl = (Volume_Control_Array *) OSLibLVMQueryVolumeControlData();
    if(volctrl == NULL) {
        DebugInt3();
        return volinfo;
    }
    for(int i=0;i<volctrl->Count;i++) {
        volinfo = Get_Volume_Information(volctrl->Volume_Control_Data[i].Volume_Handle, lasterror);
        if(*lasterror != LVM_ENGINE_NO_ERROR) {
            goto fail;
        }
        if(volinfo.Current_Drive_Letter == (char) ('A' + driveLetter)) {
            break;
        }
    }
    if(i == volctrl->Count) goto fail;

    if(pVolRec) {
        *pVolRec = volctrl->Volume_Control_Data[i];
    }
    OSLibLVMFreeVolumeControlData((HANDLE)volctrl);
    *lasterror = LVM_ENGINE_NO_ERROR;
    return volinfo;

fail:
    DebugInt3();
    OSLibLVMFreeVolumeControlData((HANDLE)volctrl);
    *lasterror = LVM_ENGINE_NO_DRIVES_FOUND;
    return volinfo;
}
//******************************************************************************
//******************************************************************************
static Volume_Information_Record OSLibLVMFindVolumeFromName(LPSTR pszVolName,
                                                            Volume_Control_Record *pVolRec,
                                                            CARDINAL32 *lasterror) 
{
    Volume_Control_Array      *volctrl;
    Volume_Information_Record volinfo;

    volctrl = (Volume_Control_Array *) OSLibLVMQueryVolumeControlData();
    if(volctrl == NULL) {
        DebugInt3();
        return volinfo;
    }
    for(int i=0;i<volctrl->Count;i++) {
        volinfo = Get_Volume_Information(volctrl->Volume_Control_Data[i].Volume_Handle, lasterror);
        if(*lasterror != LVM_ENGINE_NO_ERROR) {
            goto fail;
        }
        if(!strcmp(volinfo.Volume_Name, pszVolName)) {
            break;
        }
    }
    if(i == volctrl->Count) goto fail;

    if(pVolRec) {
        *pVolRec = volctrl->Volume_Control_Data[i];
    }
    OSLibLVMFreeVolumeControlData((HANDLE)volctrl);
    *lasterror = LVM_ENGINE_NO_ERROR;
    return volinfo;

fail:
    DebugInt3();
    OSLibLVMFreeVolumeControlData((HANDLE)volctrl);
    *lasterror = LVM_ENGINE_NO_DRIVES_FOUND;
    return volinfo;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibLVMGetPartitionInfo(ULONG driveLetter, PPARTITION_INFORMATION pPartition)
{
    Volume_Information_Record  volinfo;
    Volume_Control_Record      volctrl;
    Partition_Information_Array partctrl;
    CARDINAL32                 lasterror;

    volinfo = OSLibLVMFindVolumeFromDriveLetter(driveLetter, &volctrl, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return FALSE;
    }

    //We will not return information about LVM volumes (too dangerous as
    //they contain extra information in the volume and can be spanned)
    if(volinfo.Compatibility_Volume == FALSE) {
        return FALSE;
    }

    partctrl = Get_Partitions(volctrl.Volume_Handle, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        return FALSE;
    }

    pPartition->StartingOffset.u.HighPart = partctrl.Partition_Array[0].Partition_Start >> 23;
    pPartition->StartingOffset.u.LowPart  = partctrl.Partition_Array[0].Partition_Start << 9;
    pPartition->PartitionLength.u.HighPart= partctrl.Partition_Array[0].Usable_Partition_Size >> 23;
    pPartition->PartitionLength.u.LowPart = partctrl.Partition_Array[0].Usable_Partition_Size << 9;
    pPartition->HiddenSectors           = partctrl.Partition_Array[0].True_Partition_Size - partctrl.Partition_Array[0].Usable_Partition_Size;
    pPartition->PartitionNumber         = 0; //todo
    pPartition->PartitionType           = partctrl.Partition_Array[0].OS_Flag;
    pPartition->BootIndicator           = volinfo.Bootable;
    pPartition->RecognizedPartition     = TRUE;
    pPartition->RewritePartition        = 0;

    Free_Engine_Memory((ULONG)partctrl.Partition_Array);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibLVMGetVolumeExtents(ULONG driveLetter, PVOLUME_DISK_EXTENTS pVolExtent)
{
    Volume_Information_Record  volinfo;
    Volume_Control_Record      volctrl;
    Partition_Information_Array partctrl;
    CARDINAL32                 lasterror;

    volinfo = OSLibLVMFindVolumeFromDriveLetter(driveLetter, &volctrl, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return FALSE;
    }
    //We will not return information about LVM volumes (too dangerous as
    //they contain extra information in the volume and can be spanned)
    if(volinfo.Compatibility_Volume == FALSE) {
        return FALSE;
    }

    partctrl = Get_Partitions(volctrl.Volume_Handle, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        return FALSE;
    }

    pVolExtent->NumberOfDiskExtents = 1;
    pVolExtent->Extents[0].DiskNumber  = 0;
    pVolExtent->Extents[0].StartingOffset.u.HighPart = partctrl.Partition_Array[0].Partition_Start >> 23;;
    pVolExtent->Extents[0].StartingOffset.u.LowPart  = partctrl.Partition_Array[0].Partition_Start << 9;
    pVolExtent->Extents[0].ExtentLength.u.HighPart   = partctrl.Partition_Array[0].Usable_Partition_Size >> 23;
    pVolExtent->Extents[0].ExtentLength.u.LowPart    = partctrl.Partition_Array[0].Usable_Partition_Size << 9;

    Free_Engine_Memory((ULONG)partctrl.Partition_Array);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibLVMGetDriveType(LPCSTR lpszVolume)
{
    Volume_Information_Record  volinfo;
    Volume_Control_Record      volctrl;
    ULONG                      drivetype;
    CARDINAL32                 lasterror;

    volinfo = OSLibLVMFindVolumeFromName((char *)lpszVolume, &volctrl, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return DRIVE_NO_ROOT_DIR_W; //return value checked in NT4, SP6 (GetDriveType(""), GetDriveType("4");
    }

    switch(volctrl.Device_Type) {
    case LVM_HARD_DRIVE:
        drivetype = DRIVE_FIXED_W;
        break;
    case NON_LVM_CDROM:
        drivetype = DRIVE_CDROM_W;
        break;
    case NETWORK_DRIVE:
        drivetype = DRIVE_REMOTE_W;
        break;
    case LVM_PRM:
        drivetype = DRIVE_REMOVABLE_W;
        break;
    default:
        return DRIVE_NO_ROOT_DIR_W; //return value checked in NT4, SP6 (GetDriveType(""), GetDriveType("4");
    }
    return drivetype;
}
//******************************************************************************
//******************************************************************************
CHAR OSLibLVMQueryDriveFromVolumeName(LPCSTR lpszVolume)
{
    Volume_Information_Record  volinfo;
    ULONG                      drivetype;
    CARDINAL32                 lasterror;

    volinfo = OSLibLVMFindVolumeFromName((char *)lpszVolume, NULL, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return 0;
    }
    return volinfo.Current_Drive_Letter;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibLVMQueryVolumeFS(LPSTR lpszVolume, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
    Volume_Information_Record volinfo;
    CARDINAL32                lasterror;

    volinfo = OSLibLVMFindVolumeFromName(lpszVolume, NULL, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return ERROR_FILE_NOT_FOUND_W;
    }
    strncpy(lpFileSystemNameBuffer, volinfo.File_System_Name, nFileSystemNameSize-1);
    return ERROR_SUCCESS_W;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibLVMQueryVolumeSerialAndName(LPSTR lpszVolume, LPDWORD lpVolumeSerialNumber, 
                                       LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize)
{
    Volume_Information_Record  volinfo;
    Volume_Control_Record      volctrl;
    CARDINAL32                 lasterror;
    int                        i;

    volinfo = OSLibLVMFindVolumeFromName(lpszVolume, &volctrl, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return ERROR_FILE_NOT_FOUND_W;
    }

    if(lpVolumeSerialNumber) {
        *lpVolumeSerialNumber = volctrl.Volume_Serial_Number;
    }
    if(lpVolumeNameBuffer)
    {
        strncpy(lpVolumeNameBuffer, volinfo.Volume_Name, nVolumeNameSize-1);
    }
    return ERROR_SUCCESS_W;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibLVMGetVolumeNameForVolumeMountPoint(LPCSTR lpszVolumeMountPoint, 
                                              LPSTR lpszVolumeName, 
                                              DWORD cchBufferLength)
{
    int drive;

    //We only support drive letters as mountpoint names
    if('A' <= *lpszVolumeMountPoint && *lpszVolumeMountPoint <= 'Z') {
        drive = *lpszVolumeMountPoint - 'A' + 1;
    }
    else
    if('a' <= *lpszVolumeMountPoint && *lpszVolumeMountPoint <= 'z') {
        drive = *lpszVolumeMountPoint - 'a' + 1;
    }
    else {
        return FALSE;
    }
    if(lpszVolumeMountPoint[1] != ':') {
        return FALSE;
    }

    Volume_Information_Record volinfo;
    CARDINAL32                lasterror;

    volinfo = OSLibLVMFindVolumeFromDriveLetter(drive, NULL, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return FALSE;
    }

    strncpy(lpszVolumeName, volinfo.Volume_Name, cchBufferLength-1);
    return TRUE;
}
//******************************************************************************
//******************************************************************************

