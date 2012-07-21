/* $Id: osliblvm.cpp,v 1.5 2003-01-20 10:46:27 sandervl Exp $ */

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
#include <win/winioctl.h>
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

static void Close_LVM_Engine ( void );

//******************************************************************************
//******************************************************************************
static BOOL OSLibLVMInit()
{
    APIRET rc;
    CHAR   szModuleFailure[CCHMAXPATH];

    rc = DosLoadModule(szModuleFailure, sizeof(szModuleFailure), "LVM", (HMODULE *)&hModLVM);
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
static void Open_LVM_Engine( BOOLEAN Ignore_CHS, CARDINAL32 * Error_Code )
{
    USHORT               sel;

    //Load LVM dll
    OSLibLVMInit();

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
BOOL OSLibLVMQueryVolumeName(HANDLE hVolumeControlData, ULONG *pVolIndex,
                             LPSTR lpszVolumeName, DWORD cchBufferLength)
{
    Volume_Control_Array      *volctrl = (Volume_Control_Array *)hVolumeControlData;
    Volume_Information_Record  volinfo;
    CARDINAL32                 lasterror;

    if(volctrl == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(*pVolIndex >= volctrl->Count) {
        return FALSE;   //no more volumes
    }
    while(*pVolIndex < volctrl->Count) {
        volinfo = Get_Volume_Information(volctrl->Volume_Control_Data[*pVolIndex].Volume_Handle, &lasterror);
        if(lasterror != LVM_ENGINE_NO_ERROR) {
            DebugInt3();
            return FALSE;
        }
        //Don't report anything about LVM volumes until we support all those
        //fancy features (like spanned volumes)
        if(volinfo.Compatibility_Volume == TRUE) break;
        dprintf(("Ignoring LVM volume %s", volinfo.Volume_Name));
        (*pVolIndex)++;
    }
    if(*pVolIndex >= volctrl->Count) {
        return FALSE;   //no more volumes
    }
    strncpy(lpszVolumeName, volinfo.Volume_Name, min(sizeof(volinfo.Volume_Name), cchBufferLength)-1);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
static Volume_Information_Record OSLibLVMFindVolumeByDriveLetter(ULONG driveLetter,
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
    int i;
    for(i=0;i<volctrl->Count;i++) {
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
static Volume_Information_Record OSLibLVMFindVolumeByName(LPSTR pszVolName,
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
    int i;
    for(i=0;i<volctrl->Count;i++) {
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
BOOL OSLibLVMGetPartitionInfo(ULONG driveLetter, LPSTR lpszVolumeName, PPARTITION_INFORMATION pPartition)
{
    Volume_Information_Record  volinfo;
    Volume_Control_Record      volctrl;
    Partition_Information_Array partctrl;
    CARDINAL32                 lasterror;

    if(lpszVolumeName && lpszVolumeName[0]) {
         volinfo = OSLibLVMFindVolumeByName(lpszVolumeName, &volctrl, &lasterror);
    }
    else volinfo = OSLibLVMFindVolumeByDriveLetter(driveLetter, &volctrl, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return FALSE;
    }

    partctrl = Get_Partitions(volctrl.Volume_Handle, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR || partctrl.Count == 0) {
        return FALSE;
    }

    pPartition->StartingOffset.u.HighPart = partctrl.Partition_Array[0].Partition_Start >> 23;
    pPartition->StartingOffset.u.LowPart  = partctrl.Partition_Array[0].Partition_Start << 9;
//    pPartition->PartitionLength.u.HighPart= partctrl.Partition_Array[0].True_Partition_Size >> 23;
//    pPartition->PartitionLength.u.LowPart = partctrl.Partition_Array[0].True_Partition_Size << 9;
//    pPartition->HiddenSectors             = 0;
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
BOOL OSLibLVMGetVolumeExtents(ULONG driveLetter, LPSTR lpszVolumeName, PVOLUME_DISK_EXTENTS pVolExtent,
                              BOOL *pfLVMVolume)
{
    Volume_Information_Record  volinfo;
    Volume_Control_Record      volctrl;
    Drive_Control_Array        diskinfo;
    Partition_Information_Array partctrl;
    CARDINAL32                 lasterror;
    BOOL                       ret = TRUE;

    if(lpszVolumeName && lpszVolumeName[0]) {
         volinfo = OSLibLVMFindVolumeByName(lpszVolumeName, &volctrl, &lasterror);
    }
    else volinfo = OSLibLVMFindVolumeByDriveLetter(driveLetter, &volctrl, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return FALSE;
    }

    partctrl = Get_Partitions(volctrl.Volume_Handle, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR || partctrl.Count == 0) {
        return FALSE;
    }

    //TODO: spanned volumes
    pVolExtent->NumberOfDiskExtents = 1;
    pVolExtent->Extents[0].DiskNumber  = 0;
    pVolExtent->Extents[0].StartingOffset.u.HighPart = partctrl.Partition_Array[0].Partition_Start >> 23;;
    pVolExtent->Extents[0].StartingOffset.u.LowPart  = partctrl.Partition_Array[0].Partition_Start << 9;
//    pVolExtent->Extents[0].ExtentLength.u.HighPart   = partctrl.Partition_Array[0].True_Partition_Size >> 23;
//    pVolExtent->Extents[0].ExtentLength.u.LowPart    = partctrl.Partition_Array[0].True_Partition_Size << 9;
    pVolExtent->Extents[0].ExtentLength.u.HighPart   = partctrl.Partition_Array[0].Usable_Partition_Size >> 23;
    pVolExtent->Extents[0].ExtentLength.u.LowPart    = partctrl.Partition_Array[0].Usable_Partition_Size << 9;

    //find number of disk on which this volume is located
    diskinfo = Get_Drive_Control_Data(&lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        return FALSE;
    }
    int i;
    for(i=0;i<diskinfo.Count;i++) {
        if(diskinfo.Drive_Control_Data[i].Drive_Handle == partctrl.Partition_Array[0].Drive_Handle) {
            //win32 base = 0, os2 base = 1
            pVolExtent->Extents[0].DiskNumber = diskinfo.Drive_Control_Data[i].Drive_Number - 1;
#ifdef DEBUG
            if(diskinfo.Drive_Control_Data[i].Drive_Number == 0) DebugInt3();
#endif
            break;
        }
    }
    if(i == diskinfo.Count) {
        ret = FALSE;
    }
    dprintf(("pVolExtent->NumberOfDiskExtents       %d", pVolExtent->NumberOfDiskExtents));
    dprintf(("pVolExtent->Extents[0].DiskNumber     %d",  pVolExtent->Extents[0].DiskNumber));
    dprintf(("pVolExtent->Extents[0].StartingOffset %08x%08x", pVolExtent->Extents[0].StartingOffset.u.HighPart, pVolExtent->Extents[0].StartingOffset.u.LowPart));
    dprintf(("pVolExtent->Extents[0].ExtentLength   %08x%08x", pVolExtent->Extents[0].ExtentLength.u.HighPart, pVolExtent->Extents[0].ExtentLength.u.LowPart));

    if(pfLVMVolume) {
        *pfLVMVolume = (volinfo.Compatibility_Volume == FALSE);
    }
    Free_Engine_Memory((ULONG)diskinfo.Drive_Control_Data);
    Free_Engine_Memory((ULONG)partctrl.Partition_Array);
    return ret;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibLVMGetDriveType(LPCSTR lpszVolume)
{
    Volume_Information_Record  volinfo;
    Volume_Control_Record      volctrl;
    ULONG                      drivetype;
    CARDINAL32                 lasterror;

    volinfo = OSLibLVMFindVolumeByName((char *)lpszVolume, &volctrl, &lasterror);
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
// OSLibLVMQueryDriveFromVolumeName
//
//   Returns:
//            - drive letter corresponding to volume name
//            - -1 if volume wasn't found
//            - 0 if volume is present, but not mounted
//
//******************************************************************************
CHAR OSLibLVMQueryDriveFromVolumeName(LPCSTR lpszVolume)
{
    Volume_Information_Record  volinfo;
    ULONG                      drivetype;
    CARDINAL32                 lasterror;

    volinfo = OSLibLVMFindVolumeByName((char *)lpszVolume, NULL, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return -1;  //not found
    }
    return volinfo.Current_Drive_Letter;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibLVMQueryVolumeFS(LPSTR lpszVolume, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
    Volume_Information_Record volinfo;
    CARDINAL32                lasterror;

    volinfo = OSLibLVMFindVolumeByName(lpszVolume, NULL, &lasterror);
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

    volinfo = OSLibLVMFindVolumeByName(lpszVolume, &volctrl, &lasterror);
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
        drive = *lpszVolumeMountPoint - 'A';
    }
    else
    if('a' <= *lpszVolumeMountPoint && *lpszVolumeMountPoint <= 'z') {
        drive = *lpszVolumeMountPoint - 'a';
    }
    else {
        return FALSE;
    }
    if(lpszVolumeMountPoint[1] != ':') {
        return FALSE;
    }

    Volume_Information_Record volinfo;
    CARDINAL32                lasterror;

    volinfo = OSLibLVMFindVolumeByDriveLetter(drive, NULL, &lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return FALSE;
    }

    strncpy(lpszVolumeName, volinfo.Volume_Name, cchBufferLength-1);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibLVMStripVolumeName(LPCSTR lpszWin32VolumeName, LPSTR lpszOS2VolumeName, DWORD cchBufferLength)
{
    int length;

    //strip volume name prefix (\\\\?\\Volume\\)
    length = strlen(lpszWin32VolumeName);

    strncpy(lpszOS2VolumeName, &lpszWin32VolumeName[sizeof(VOLUME_NAME_PREFIX)-1+1], cchBufferLength-1);  //-zero term + starting '{'
    length -= sizeof(VOLUME_NAME_PREFIX)-1+1;
    if(lpszOS2VolumeName[length-2] == '}')
    {
        lpszOS2VolumeName[length-2] = 0;
        return TRUE;
    }
    else
    if(lpszOS2VolumeName[length-1] == '}')
    {
        lpszOS2VolumeName[length-1] = 0;
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibLVMGetDiskGeometry(DWORD dwDiskNr, PDISK_GEOMETRY pGeom)
{
    Drive_Control_Array   driveinfo;
    Drive_Control_Record *pDriveRec;
    CARDINAL32            lasterror;
    BOOL                  ret = FALSE;
    int                   i;

    driveinfo = Get_Drive_Control_Data(&lasterror);
    if(lasterror != LVM_ENGINE_NO_ERROR) {
        DebugInt3();
        return FALSE;
    }
    pDriveRec = driveinfo.Drive_Control_Data;
    if(pDriveRec == NULL) {
        DebugInt3();
        return FALSE;
    }
    if(dwDiskNr > driveinfo.Count) {
        DebugInt3();
        ret = FALSE;
        goto endfunc;
    }
    for(i=0;i<driveinfo.Count;i++) {
        if(pDriveRec->Drive_Number == dwDiskNr) {
            pGeom->Cylinders.u.LowPart  = pDriveRec->Cylinder_Count;
            pGeom->Cylinders.u.HighPart = 0;
            pGeom->TracksPerCylinder    = pDriveRec->Heads_Per_Cylinder;
            pGeom->SectorsPerTrack      = pDriveRec->Sectors_Per_Track;
            pGeom->BytesPerSector       = 512;
            pGeom->MediaType            = FixedMedia;

            ret = TRUE;
            break;
        }
    }
endfunc:
    Free_Engine_Memory((ULONG)driveinfo.Drive_Control_Data);
    return ret;
}
//******************************************************************************
//******************************************************************************
