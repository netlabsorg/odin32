/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
** 
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.  
** 
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
**
**
** $Revision: 1.1 $ 
** $Date: 2000-02-18 10:45:12 $ 
**
*/

#ifndef _FXPCI_H_
#define _FXPCI_H_

#define _3DFX_PCI_ID 0x121A

/* 
    16 Busses ( of possible 256, I am making the 
                assumption that busses are numbered 
                in increasing order and that no
                PC will have more than 16 busses )
    32 Slots Per Bus

    Device Number = Bus Number * 32 + Slot Number
*/
#define MAX_PCI_DEVICES 512

typedef int PciMemType;
#define PciMemTypeUncacheable           0
#define PciMemTypeWriteCombining        1
#define PciMemTypeWriteThrough          4
#define PciMemTypeWriteProtected        5
#define PciMemTypeWriteback             6

typedef enum {
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
} PciIOFlag;

typedef struct {
    FxU32              regAddress;
    FxU32              sizeInBytes;
    PciIOFlag          rwFlag;
} PciRegister;

#ifndef KERNEL
extern PciRegister PCI_VENDOR_ID;     
extern PciRegister PCI_DEVICE_ID;     
extern PciRegister PCI_COMMAND;       
extern PciRegister PCI_STATUS;
extern PciRegister PCI_REVISION_ID;
extern PciRegister PCI_CLASS_CODE;
extern PciRegister PCI_CACHE_LINE_SIZE;
extern PciRegister PCI_LATENCY_TIMER;
extern PciRegister PCI_HEADER_TYPE;
extern PciRegister PCI_BIST;
extern PciRegister PCI_BASE_ADDRESS_0;
extern PciRegister PCI_BASE_ADDRESS_1;
extern PciRegister PCI_BASE_ADDRESS_2;
extern PciRegister PCI_BASE_ADDRESS_3;
extern PciRegister PCI_BASE_ADDRESS_4;
extern PciRegister PCI_BASE_ADDRESS_5;
extern PciRegister PCI_INTERRUPT_LINE;
extern PciRegister PCI_INTERRUPT_PIN;
extern PciRegister PCI_MIN_GNT;
extern PciRegister PCI_MAX_LAT;

extern PciRegister PCI_SST1_INIT_ENABLE; // 0x40
extern PciRegister PCI_SST1_BUS_SNOOP_0; // 0x44
extern PciRegister PCI_SST1_BUS_SNOOP_1; // 0x48
extern PciRegister PCI_SST1_CFG_STATUS;  // 0x4C
#endif /* #ifndef KERNEL */

FX_ENTRY const char* FX_CALL
pciGetErrorString( void );

FX_ENTRY FxU32 FX_CALL
pciGetErrorCode( void );

FX_ENTRY FxBool FX_CALL 
pciOpen( void );

FX_ENTRY FxBool FX_CALL 
pciClose( void );

FX_ENTRY FxBool FX_CALL 
pciDeviceExists( FxU32 device_number );

FX_ENTRY FxBool FX_CALL 
pciGetConfigData( PciRegister reg, FxU32 device_number, FxU32 *data );

FX_ENTRY FxBool FX_CALL 
pciSetConfigData( PciRegister reg, FxU32 device_number, FxU32 *data );

FX_ENTRY FxBool FX_CALL 
pciMapPhysicalToLinear(FxU32 *linear_addr, FxU32 physical_addr,FxU32 *length);

FX_ENTRY void   FX_CALL 
pciUnmapPhysical( FxU32 linear_addr, FxU32 length );

const char *
pciGetVendorName( FxU16 vendor_id );
const char *
pciGetClassName( FxU32 class_code , FxU32 deviceID);

FX_ENTRY FxBool FX_CALL 
pciFindCard(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum);

FX_ENTRY FxBool FX_CALL 
pciFindCardMulti(FxU32 vID, FxU32 dID, FxU32 *devNum, FxU32 cardNum);

FX_ENTRY FxU32 * FX_CALL 
pciMapCard(FxU32 vID, FxU32 dID, FxI32 len, FxU32 *devNo, FxU32 addrNo);

FX_ENTRY FxU32 * FX_CALL
pciMapCardMulti(FxU32 vID,FxU32 dID,FxI32 l,FxU32 *dNo,FxU32 cNo,FxU32 aNo);

FX_ENTRY FxBool FX_CALL
pciFindMTRRMatch(FxU32 pBaseAddrs, FxU32 psz, PciMemType type, FxU32 *mtrrNum);

FX_ENTRY FxBool FX_CALL
pciFindFreeMTRR(FxU32 *mtrrNum);

FX_ENTRY FxBool FX_CALL
pciSetMTRR(FxU32 mtrrNo, FxU32 pBaseAddr, FxU32 psz, PciMemType type);

FX_ENTRY FxBool FX_CALL
pciSetPassThroughBase(FxU32* pBaseAddr, FxU32 baseAddrLen);

FX_ENTRY FxBool FX_CALL
pciOutputDebugString(const char* debugMsg);

FX_ENTRY FxBool FX_CALL
pciLinearRangeSetPermission(const FxU32 addrBase, const FxU32 addrLen, const FxBool writeableP);

#define PCI_ERR_NOERR           0
#define PCI_ERR_WINRTINIT       1
#define PCI_ERR_MEMMAPVXD       2
#define PCI_ERR_MAPMEMDRV       3
#define PCI_ERR_GENPORT         4
#define PCI_ERR_NO_BUS          5
#define PCI_ERR_NOTOPEN         6
#define PCI_ERR_NOTOPEN2        7
#define PCI_ERR_NOTOPEN3        8
#define PCI_ERR_OUTOFRANGE      9
#define PCI_ERR_NODEV           10
#define PCI_ERR_NODEV2          11
#define PCI_ERR_WRITEONLY       12
#define PCI_ERR_READONLY        13
#define PCI_ERR_PHARLAP         14
#define PCI_ERR_WRONGVXD        15
#define PCI_ERR_MEMMAP          16
#define PCI_ERR_MAPMEM          17
#define PCI_ERR_WINRT           18
#define PCI_ERR_VXDINUSE        19
#define PCI_ERR_NO_IO_PERM	20
#define PCI_ERR_NO_MEM_PERM	21


#endif /* _FXPCI_H_ */
