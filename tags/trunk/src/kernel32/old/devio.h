/* $Id: devio.h,v 1.1 1999-09-15 23:32:54 sandervl Exp $ */

#ifndef __DEVIO_H__
#define __DEVIO_H__

#ifndef _OS2WIN_H
#define GENERIC_READ                 0x80000000
#define GENERIC_WRITE                0x40000000
#define GENERIC_EXECUTE              0x20000000
#define GENERIC_ALL                  0x10000000

#define FILE_SHARE_READ              0x00000001
#define FILE_SHARE_WRITE             0x00000002
#define FILE_ATTRIBUTE_READONLY      0x00000001
#define FILE_ATTRIBUTE_HIDDEN        0x00000002
#define FILE_ATTRIBUTE_SYSTEM        0x00000004
#define FILE_ATTRIBUTE_DIRECTORY     0x00000010
#define FILE_ATTRIBUTE_ARCHIVE       0x00000020
#define FILE_ATTRIBUTE_NORMAL        0x00000080
#define FILE_ATTRIBUTE_TEMPORARY     0x00000100
#define FILE_ATTRIBUTE_ATOMIC_WRITE  0x00000200
#define FILE_ATTRIBUTE_XACTION_WRITE 0x00000400
#define FILE_ATTRIBUTE_COMPRESSED    0x00000800
#define FILE_ATTRIBUTE_HAS_EMBEDDING 0x00001000

#define FILE_SHARE_READ                  0x00000001
#define FILE_SHARE_WRITE                 0x00000002

#define IOCTL_GPD_READ_PORT_UCHAR   0x900
#define IOCTL_GPD_READ_PORT_USHORT  0x901
#define IOCTL_GPD_READ_PORT_ULONG   0x902
#define IOCTL_GPD_WRITE_PORT_UCHAR  0x910
#define IOCTL_GPD_WRITE_PORT_USHORT 0x911
#define IOCTL_GPD_WRITE_PORT_ULONG  0x912

#define IOCTL_MAPMEM_MAP_USER_PHYSICAL_MEMORY   0x800
#define IOCTL_MAPMEM_UNMAP_USER_PHYSICAL_MEMORY 0x801

typedef struct  _GENPORT_WRITE_INPUT {
    ULONG   PortNumber;
    union   {
        ULONG   LongData;
        USHORT  ShortData;
        UCHAR   CharData;
    };
}   GENPORT_WRITE_INPUT;

typedef struct _tagOVERLAPPED {
    DWORD    Internal;
    DWORD    InternalHigh;
    DWORD    Offset;
    DWORD    OffsetHigh;
    HANDLE   hEvent;
} OVERLAPPED, *POVERLAPPED, *LPOVERLAPPED;

typedef BOOL (* WINIOCTL)(HANDLE hDevice, DWORD dwIoControlCode,
              LPVOID lpInBuffer, DWORD nInBufferSize,
              LPVOID lpOutBuffer, DWORD nOutBufferSize,
              LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

typedef struct {
    char     szWin32Name[32];
    char     szOS2Name[32];
    BOOL     fCreateFile;
    DWORD    hDevice;
    WINIOCTL devIOCtl;
} WIN32DRV;

typedef enum _INTERFACE_TYPE {
    InterfaceTypeUndefined = -1,
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    ProcessorInternal,
    InternalPowerBus,
    PNPISABus,
    MaximumInterfaceType
}INTERFACE_TYPE, *PINTERFACE_TYPE;

typedef LARGE_INTEGER PHYSICAL_ADDRESS;

typedef struct
{
    INTERFACE_TYPE   InterfaceType;
    ULONG            BusNumber;
    PHYSICAL_ADDRESS BusAddress;
    ULONG            AddressSpace;
    ULONG            Length;

} PHYSICAL_MEMORY_INFO, *PPHYSICAL_MEMORY_INFO;

typedef struct _MapDevRequest
{
    DWORD   mdr_ServiceID;
    LPVOID  mdr_PhysicalAddress;
    DWORD   mdr_SizeInBytes;
    LPVOID  mdr_LinearAddress;
    WORD    mdr_Selector;
    WORD    mdr_Status;
} MAPDEVREQUEST, *PMAPDEVREQUEST;

#endif

#ifdef __cplusplus
extern "C" {
#endif

HANDLE OS2CreateFile(char *szName, DWORD fdwAccess, DWORD fdwShareMode);
void   OS2CloseFile(HANDLE hDevice);

#ifdef __cplusplus
}
#endif

#endif
