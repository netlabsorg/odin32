/**************************************************************************
 *
 * SOURCE FILE NAME =  RMBASE.H
 *
 * DESCRIPTIVE NAME =  RM Base types and definitions
 *
 *
 * Copyright : COPYRIGHT IBM CORPORATION, 1994, 1995
 *             LICENSED MATERIAL - PROGRAM PROPERTY OF IBM
 *             REFER TO COPYRIGHT INSTRUCTION FORM#G120-2083
 *             RESTRICTED MATERIALS OF IBM
 *             IBM CONFIDENTIAL
 *
 * VERSION = V1.01
 *
 * DATE
 *
 * DESCRIPTION :
 *
 * Purpose:
 *
 *
 *
 * FUNCTIONS  :
 *
 * NOTES
 *
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 *
 *
 * EXTERNAL FUNCTIONS
 *
 * CHANGE ACTIVITY =
 *   DATE      FLAG        APAR   CHANGE DESCRIPTION
 *   --------  ----------  -----  --------------------------------------
 *
 ****************************************************************************/


#ifndef __RM_HEADER__
#define __RM_HEADER__


#define CMVERSION_MAJOR    0x01
#define CMVERSION_MINOR    0x01

#define CMVERSION_MAJOR_21  0x02
#define CMVERSION_MINOR_21  0x01

#define FAR         /* this will be deleted shortly */
#define NEAR        /* this will be deleted shortly */

typedef ULONG RMHANDLE, FAR *PRMHANDLE, NEAR *NPRMHANDLE;
typedef RMHANDLE HDRIVER;
typedef RMHANDLE HADAPTER;
typedef RMHANDLE HDEVICE;
typedef RMHANDLE HRESOURCE;
typedef RMHANDLE HLDEV;
typedef RMHANDLE HSYSNAME;
typedef RMHANDLE HDETECTED;

typedef HDRIVER   FAR *PHDRIVER;
typedef HDRIVER   NEAR *NPHDRIVER;
typedef HADAPTER  FAR *PHADAPTER;
typedef HADAPTER  NEAR *NPHADAPTER;
typedef HDEVICE   FAR *PHDEVICE;
typedef HDEVICE   NEAR *NPHDEVICE;
typedef HRESOURCE FAR *PHRESOURCE;
typedef HRESOURCE NEAR *NPHRESOURCE;
typedef HLDEV     FAR *PHLDEV;
typedef HLDEV     NEAR *NPHLDEV;
typedef HSYSNAME  FAR *PHSYSNAME;
typedef HSYSNAME  NEAR *NPHSYSNAME;
typedef HDETECTED FAR *PHDETECTED ;
typedef HDETECTED NEAR *NPHDETECTED ;

/*********************************/
/* pResourceStruct->ResourceType */
/*********************************/

#define RS_TYPE_IO    1
#define RS_TYPE_IRQ   2
#define RS_TYPE_MEM   3
#define RS_TYPE_DMA   4
#define RS_TYPE_TIMER 5
#define RS_TYPE_NEW   0xffff

typedef struct _hresource {
   ULONG     NumResource;
   HRESOURCE hResource[1];      /*First Entry in Array of HRESOURCE */
}AHRESOURCE, FAR *PAHRESOURCE, NEAR *NPAHRESOURCE;


/****************************************************************************/
/*                                                                          */
/* Driver Structure                                                         */
/*                                                                          */
/****************************************************************************/

typedef struct {
   USHORT Year;
   UCHAR  Month;
   UCHAR  Day;
} DATESTAMP, FAR *PDATESTAMP, NEAR *NPDATESTAMP;


/* Callback for Grant/Yield; Indicates CM resolves resource to handle */
#ifdef __IBMCPP__
typedef ULONG PFNRMCB;
#else
typedef USHORT (_cdecl FAR *PFNRMCB)(HRESOURCE hResource);
#endif


typedef struct {
   PSZ       DrvrName;
   PSZ       DrvrDescript;
   PSZ       VendorName;
   UCHAR     MajorVer;
   UCHAR     MinorVer;
   DATESTAMP Date;
   USHORT    DrvrFlags;
   USHORT    DrvrType;
   USHORT    DrvrSubType;
   PFNRMCB   DrvrCallback;     /* Event notification */
} DRIVERSTRUCT, FAR *PDRIVERSTRUCT, NEAR *NPDRIVERSTRUCT;


/********************************/
/* pDriverStruct->DrvrFlags */
/********************************/

#define DRF_STATIC   0x0000
#define DRF_DYNAMIC  0x0001
#define DRF_PREVIOUS 0x0002


/* pDriverStruct->DriverType      */
/*   pDriverStruct->DriverSubType */
#define DRT_UNDEFINED        0
  #define DRS_UNDEFINED      0

#define DRT_PCMCIA           1
  #define DRS_SOCKETSERV     1
  #define DRS_CARDSERV       2
  #define DRS_CLIENT         3

#define DRT_ADDDM            2
  #define DRS_DM             1
  #define DRS_FILTER         2
  #define DRS_ADD            3
  #define DRS_DM_TRANSPORT   4

#define DRT_OS2              3
  #define DRS_CHAR           1
  #define DRS_BLOCK          2
  #define DRS_APP_HELPER     3

#define DRT_NETWORK          4
#define DRT_VIDEO            5
#define DRT_AUDIO            6
#define DRT_SERVICE          7
  #define DRS_SNOOPER        1
  #define DRS_CONFIG         2												

/****************************************************************************/
/*                                                                          */
/* Adapter Structure  - Device Bus                                          */
/*                                                                          */
/****************************************************************************/

typedef struct {
   USHORT ADDHandle;
   USHORT UnitHandle;
}ADD_UNIT, FAR *PADD_UNIT, NEAR *NPADD_UNIT;

typedef struct {
   USHORT VolFlags;
   USHORT VolIFSType;
   ULONG  VolSize;
   ULONG  VolID;
} DASD_VOL, FAR *PDASD_VOL, NEAR *NPDASD_VOL;

typedef struct {
   USHORT Priority;
   USHORT DriverType;
   UCHAR  DriverName[12];
   UCHAR  DriverParmLine[1];  /* Embedded asciiz */
} DRIVERINFO, FAR *PDRIVERINFO, NEAR *NPDRIVERINFO;

#define DriverBase 0
#define DriverLoadable 1


typedef struct _ADJUNCT FAR *PADJUNCT;
typedef struct _ADJUNCT{
// warning all common field sizes needed
// see FIELDOFFSETOF() macro below
     PADJUNCT pNextAdj;
     USHORT   AdjLength;
     USHORT   AdjType;
   union {
     USHORT           AdjBase;
     USHORT           SCSI_Target_LUN;
     USHORT           Adapter_Number;
     USHORT           Device_Number;
     USHORT           PCI_DevFunc;
     USHORT           Model_Info;
     ADD_UNIT         Add_Unit;
     DASD_VOL         Dasd_Vol;
     DRIVERINFO       DriverInfo;
   };
}ADJUNCT, NEAR *NPADJUNCT;


/********************************/
/* pAdjunct->AdjunctType        */
/********************************/

//#define ADJ_HEADER_SIZE        sizeof(struct _ADJHEADER)
#ifndef FIELDOFFSET
#define FIELDOFFSET(type, field)    ((SHORT)&(((type *)0)->field))
#endif
#define ADJ_HEADER_SIZE        FIELDOFFSET(ADJUNCT,AdjBase)

#define ADJ_SCSI_TARGET_LUN    1
#define ADJ_ADAPTER_NUMBER     2
#define ADJ_DEVICE_NUMBER      3
#define ADJ_PCI_DEVFUNC        4
#define ADJ_MODEL_INFO         5
#define ADJ_ADD_UNIT           6
#define ADJ_DASD_VOL           7
#define ADJ_DRIVER_DATA        8


typedef struct{
   PSZ          AdaptDescriptName;
   USHORT       AdaptFlags;
   USHORT       BaseType;             /* From PCI/PNP */
   USHORT       SubType;
   USHORT       InterfaceType;
   USHORT       HostBusType;
   USHORT       HostBusWidth;
   PADJUNCT     pAdjunctList;
   ULONG        Reserved;             /* Logical Name addition? */
} ADAPTERSTRUCT, FAR *PADAPTERSTRUCT, NEAR *NPADAPTERSTRUCT;



/********************************/
/* pAdapteStruct->BaseType      */
/* pAdapteStruct->Sub           */
/* pAdapteStruct->InterfaceType */
/* From PNP/PCI Specs           */
/********************************/

#define AS_BASE_RESERVED          0x00
 #define AS_SUB_OTHER             0x80      /* Can Be used by any BASE type */
  #define AS_INTF_GENERIC         0x01      /* Can Be used by any SUB type  */

#define AS_BASE_MSD               0x01      /* Mass Storage Device          */
 #define AS_SUB_SCSI              0x01
 #define AS_SUB_IDE               0x02
 #define AS_SUB_FLPY              0x03
 #define AS_SUB_IPI               0x04

#define AS_BASE_NETWORK           0x02     /* Network Interface Controller */
 #define AS_SUB_ETHERNET          0x01
 #define AS_SUB_TOKENRING         0x02
 #define AS_SUB_FDDI              0x03

#define AS_BASE_DISPLAY           0x03     /* Display Controller           */
 #define AS_SUB_VGA               0x01
  #define AS_INTF_VGA_GEN         0x01
  #define AS_INTF_VESA_SVGA       0x02
 #define AS_SUB_XGA               0x02


#define AS_BASE_MMEDIA            0x04     /* Multi-media Controller       */
 #define AS_SUB_MM_VIDEO          0x01
 #define AS_SUB_MM_AUDIO          0x02

#define AS_BASE_MEMORY            0x05    /* Memory                       */
 #define AS_SUB_BIOS_ROM          0x01

#define AS_BASE_BRIDGE            0x06    /* Bridge Controller            */

#define AS_BASE_COMM              0x07    /* Communications Device        */
 #define AS_SUB_SERIAL            0x01
  #define AS_INTF_16450           0x01
  #define AS_INTF_16550           0x02
 #define AS_SUB_PARALLEL          0x02
  #define AS_INTF_BIDI            0x01
  #define AS_INTF_ECP             0x02
 #define AS_SUB_ISDN              0x03    /* ISDN Device */          /* @@JD01 */
  #define AS_INTF_UNKNOWN         0x01    /* Any I/F not yet defined    @@JD01 */
  #define AS_INTF_A2000FIFO       0x02    /* Active 2000 Fifo based     @@JD01 */
  #define AS_INTF_A2000SLC        0x03    /* Active 2000 Serial Link    @@JD01 */

#define AS_BASE_PERIPH            0x08    /* System Peripherals          */
 #define AS_SUB_PIC               0x01
  #define AS_INTF_ISAPIC          0x01
  #define AS_INTF_EISAPIC         0x02
 #define AS_SUB_DMA               0x02
 #define AS_SUB_TIMER             0x03
 #define AS_SUB_RTC               0x04

#define AS_BASE_INPUT             0x09    /* Input Device                */
 #define AS_SUB_KBD               0x01
 #define AS_SUB_DIGIT             0x02
 #define AS_SUB_MOUSE             0x03

#define AS_BASE_DOCK              0x0a    /* Docking Station             */

#define AS_BASE_CPU               0x0b    /* CPU                         */

#define AS_BASE_PCMCIA            0x0c    /* PCMCIA                      */
 #define AS_SUB_SOCKET_CONTROLLER 0x01    /* Socket Controller           */

#define AS_BASE_BIOS_ROM          0x0d

/********************************/
/* pAdapterStruct->HostBusType  */
/********************************/

#define  AS_HOSTBUS_OTHER       0x00
#define  AS_HOSTBUS_ISA         0x01
#define  AS_HOSTBUS_EISA        0x02
#define  AS_HOSTBUS_uCHNL       0x03
#define  AS_HOSTBUS_PCI         0x04
#define  AS_HOSTBUS_SCSI        0x05
#define  AS_HOSTBUS_PCMCIA      0x06
#define  AS_HOSTBUS_PARALLEL    0x07
#define  AS_HOSTBUS_PLANAR      0x08
#define  AS_HOSTBUS_UNKNOWN     0xff
/* Others SERIAL??? */

/********************************/
/* pAdapterStruct->HostBusWidth */
/********************************/

#define  AS_BUSWIDTH_8BIT       0x10
#define  AS_BUSWIDTH_16BIT      0x20
#define  AS_BUSWIDTH_32BIT      0x30
#define  AS_BUSWIDTH_64BIT      0x40
#define  AS_BUSWIDTH_UNKNOWN    0xf0

/********************************/
/* pAdapterStruct->AdapterFlags */
/********************************/

#define AS_16MB_ADDRESS_LIMIT       0x01
#define AS_NO16MB_ADDRESS_LIMIT     0x00


/****************************************************************************/
/*                                                                          */
/* Device Structure  - Object hanging off Adapter                           */
/*                                                                          */
/****************************************************************************/

typedef struct {
   PSZ      DevDescriptName;
   USHORT   DevFlags;
   USHORT   DevType;
   PADJUNCT pAdjunctList;
} DEVICESTRUCT, FAR *PDEVICESTRUCT, NEAR *NPDEVICESTRUCT;

/********************************/
/* pDeviceStruct->DevType       */
/********************************/


#define  DS_TYPE_DISK            0x0000     /* All Direct Access Devices        */
#define  DS_TYPE_TAPE            0x0001     /* Sequencial Access Devices        */
#define  DS_TYPE_PRINTER         0x0002     /* Printer Device                   */
#define  DS_TYPE_PROCESSOR       0x0003     /* Processor type device            */
#define  DS_TYPE_WORM            0x0004     /* Write Once Read Many Device      */
#define  DS_TYPE_CDROM           0x0005     /* CD ROM Device                    */
#define  DS_TYPE_SCANNER         0x0006     /* Scanner Device                   */
#define  DS_TYPE_OPT_MEM         0x0007     /* some Optical disk                */
#define  DS_TYPE_CHANGER         0x0008     /* Changer device e.g. juke box     */
#define  DS_TYPE_COMM            0x0009     /* Communication devices            */
#define  DS_TYPE_ATAPI           0x000A     /* Unspecific ATAPI protocol device */
#define  DS_TYPE_SCSI_ATT        0x000B     /* SCSI Attach                      */
#define  DS_TYPE_SOCKET          0x000C     /* PCMCIA Socket                    */
#define  DS_TYPE_SLOT            0x000D     /* Bus Slot                         */
#define  DS_TYPE_PLANAR_CHIPSET  0x000E     /* DMA/IRQ/TIMER Controllers        */
#define  DS_TYPE_IO              0x000F     /* Input/Output                     */
#define  DS_TYPE_AUDIO           0x0010     /* Audio Device                     */
#define  DS_TYPE_UNKNOWN         0xFFFF

/********************************/
/* pDeviceStruct->DevFlags      */
/********************************/

#define DS_REMOVEABLE_MEDIA  0x01
#define DS_FIXED_LOGICALNAME 0x02



typedef struct {
   ULONG     NumDevices;
   HDEVICE   hDevice[1];        /* First Entry in array of HRESOURCE */
   }AHDEVICES, FAR *PAHDEVICES, NEAR *NPAHDEVICES;


/****************************************************************************/
/*                                                                          */
/* Resource Structure                                                       */
/*                                                                          */
/****************************************************************************/

typedef struct {
   USHORT BaseIOPort;
   USHORT NumIOPorts;
   USHORT IOFlags;
   USHORT IOAddressLines;
} IORESOURCE, FAR *PIORESOURCE, NEAR *NPIORESOURCE;

/********************************/
/* pIOResource->IOFlags         */
/********************************/

#define RS_IO_EXCLUSIVE         0x0001
#define RS_IO_MULTIPLEXED       0x0002
#define RS_IO_SHARED            0x0004
#define RS_IO_RECONFIGURE       0x0008
#define RS_IO_GRANT_YIELD       0x0010
#define RS_IO_FORCE_ALIAS       0x0020
#define RS_SEARCH               0x4000
#define RS_NONOWNER_ALLOWED     0x8000                              /*@V103757*/

/****************************************************************************/
/*                                                                          */
/* IRQ Resource Structure                                                   */
/*                                                                          */
/****************************************************************************/

/* Interrupt handler for Grant/Yield with RM routing interrupts       */
#ifdef __IBMCPP__
typedef ULONG PFNRMINTHANDLER;
#else
typedef USHORT (_cdecl FAR *PFNRMINTHANDLER)(VOID);
#endif

typedef struct {
   USHORT  IRQLevel;                       /* < 16 */
   USHORT  PCIIrqPin;                      /* < 5  */
   USHORT  IRQFlags;
   USHORT  Reserved;                       /* Alignment for 32 bit code */
   PFNRMINTHANDLER pfnIntHandler;
} IRQRESOURCE, FAR *PIRQRESOURCE, NEAR *NPIRQRESOURCE;

/********************************/
/* pIRQResource->PCIIrqPin      */
/********************************/
#define RS_PCI_INT_NONE 0    /* For completeness, on non-PCI box */
#define RS_PCI_INT_A    1
#define RS_PCI_INT_B    2
#define RS_PCI_INT_C    3
#define RS_PCI_INT_D    4


/********************************/
/* pIRQResource->IRQFlags       */
/********************************/

#define RS_IRQ_EXCLUSIVE     0x01
#define RS_IRQ_MULTIPLEXED   0x02
#define RS_IRQ_SHARED        0x04
#define RS_IRQ_RECONFIGURE   0x08
#define RS_IRQ_GRANT_YIELD   0x10
#define RS_IRQ_ROUTER        0x20

/****************************************************************************/
/*                                                                          */
/* MEM Resource Structure                                                   */
/*                                                                          */
/****************************************************************************/

typedef struct {
   ULONG  MemBase;
   ULONG  MemSize;
   USHORT MemFlags;
   USHORT ReservedAlign;    /* Alignment for 32-bit code */
} MEMRESOURCE, FAR *PMEMRESOURCE, NEAR *NPMEMRESOURCE;


/********************************/
/* pMemResource->MemFlags       */
/********************************/

#define RS_MEM_EXCLUSIVE     0x01
#define RS_MEM_MULTIPLEXED   0x02
#define RS_MEM_SHARED        0x04
#define RS_MEM_RECONFIGURE   0x08
#define RS_MEM_GRANT_YIELD   0x10

/****************************************************************************/
/*                                                                          */
/* DMA Resource Structure                                                   */
/*                                                                          */
/****************************************************************************/

typedef struct {
   USHORT DMAChannel;
   USHORT DMAFlags;
} DMARESOURCE, FAR *PDMARESOURCE, NEAR *NPDMARESOURCE;


/********************************/
/* pDMAResource->DMAFlags       */
/********************************/

#define RS_DMA_EXCLUSIVE     0x01
#define RS_DMA_MULTIPLEXED   0x02
#define RS_DMA_SHARED        0x04
#define RS_DMA_RECONFIGURE   0x08
#define RS_DMA_GRANT_YIELD   0x10

/****************************************************************************/
/*                                                                          */
/* Timer Resource Structure                                                 */
/*                                                                          */
/****************************************************************************/

typedef struct {
   USHORT TMRChannel;
   USHORT TMRFlags;
} TMRRESOURCE, FAR *PTMRRESOURCE, NEAR *NPTMRRESOURCE;


/********************************/
/* pTmrResource->TMRFlags       */
/********************************/

#define RS_TMR_EXCLUSIVE   0x01
#define RS_TMR_MULTIPLEXED 0x02
#define RS_TMR_SHARED      0x04


/****************************************************************************/

typedef struct {
   ULONG ResourceType;
   union {
      IORESOURCE   IOResource;
      IRQRESOURCE  IRQResource;
      MEMRESOURCE  MEMResource;
      DMARESOURCE  DMAResource;
      TMRRESOURCE  TMRResource;
     };
   ULONG  Reserved;
} RESOURCESTRUCT, FAR *PRESOURCESTRUCT, NEAR *NPRESOURCESTRUCT;


/********************************/
/* RMModifyResource Command     */
/********************************/
#define RM_MODIFY_ADD    0
#define RM_MODIFY_DELETE 1


/****************************************************************************/
/*                                                                          */
/* LDEV - Logical Device Structure                                          */
/*                                                                          */
/****************************************************************************/

typedef struct {
   PSZ      LDevDescriptName;
   USHORT   LDevFlags;
   USHORT   LDevClass;
   HDEVICE  LDevHDevice;
   PADJUNCT pAdjunctList;
} LDEVSTRUCT, FAR *PLDEVSTRUCT, NEAR *NPLDEVSTRUCT;

/*********************************/
/* pLDevStruct->LDevFlags        */
/*********************************/
/*- TBD -*/

#define LDEV_CLASS_BASE         0
#define LDEV_CLASS_ROOT         (LDEV_CLASS_BASE+1)
#define LDEV_CLASS_DASD         (LDEV_CLASS_BASE+2)
#define LDEV_CLASS_CDROM        (LDEV_CLASS_BASE+3)
#define LDEV_CLASS_SERIAL       (LDEV_CLASS_BASE+4)
#define LDEV_CLASS_PARALLEL     (LDEV_CLASS_BASE+5)
#define LDEV_CLASS_TAPE         (LDEV_CLASS_BASE+6)

#define NUM_LDEV_CLASSES        6
#define MAX_LDEV_CLASSES        10
#define NUM_DDCFG_TYPES 	     2

/****************************************************************************/
/*                                                                          */
/* SYSNAME - System Name                                                    */
/*                                                                          */
/****************************************************************************/

typedef struct {
   PSZ      SysDescriptName;
   PADJUNCT pAdjunctList;
   USHORT   SysFlags;
   USHORT   Reserved;
} SYSNAMESTRUCT, FAR *PSYSNAMESTRUCT, NEAR *NPSYSNAMESTRUCT;



/****************************************************************************/
/*                                                                          */
/* DETECTED - Detected HW Function                                          */
/*                                                                          */
/****************************************************************************/
typedef ULONG IDTYPE;

#define   RM_IDTYPE_RM      0        /* Resource Manager Internal */
#define   RM_IDTYPE_EISA    1        /* EISA (Also ISA PNP) */
#define   RM_IDTYPE_PCI     2        /* PCI                 */
#define   RM_IDTYPE_LEGACY  3        /* LEGACY              */

typedef ULONG DEVID;
typedef ULONG VENDID;
typedef ULONG SERNUM;

typedef struct {
   PSZ      DetectDescriptName;/* Ascii name       */
   USHORT   DetectFlags;       /* ???              */
   IDTYPE   IDType;            /* EISA, PCI, etc.. */
   DEVID    DeviceID;          /* EISA, PCI, etc.. ID   */
   DEVID    FunctionID;        /* For multi-function hw */
   DEVID    CompatibleID;      /* Compatible Device     */
   PADJUNCT pAdjunctList;
	VENDID	VendorID;
	SERNUM	SerialNumber;
} DETECTEDSTRUCT, FAR *PDETECTEDSTRUCT, NEAR *NPDETECTEDSTRUCT;

/********************************/
/* pDetectStruct->DetectFlags   */
/********************************/

#define DS_PREVIOUS_BOOT  0x01     /* Note: not settable externally */




/****************************************************************************/
/*                                                                          */
/* Handle List Structure                                                    */
/*                                                                          */
/****************************************************************************/

typedef struct
{
  USHORT        cMaxHandles;
  USHORT        cHandles;
  HADAPTER      Handles[1];

} HANDLELIST, FAR *PHANDLELIST, NEAR *NPHANDLELIST;

typedef struct _ADJINFO
{
    HADAPTER    hAdapter;
    USHORT      AdjIndex;

} ADJINFO;

typedef struct
{
  USHORT        cMaxHandles;
  USHORT        cHandles;
  ADJINFO       Adj[1];

} ADJHANDLELIST, FAR *PADJHANDLELIST, NEAR *NPADJHANDLELIST;

/****************************************************************************/
/*                                                                          */
/* PreAssigned Node Handles                                                 */
/*                                                                          */
/****************************************************************************/

#define HANDLE_PHYS_TREE         0xffff8000
#define HANDLE_SYS_TREE          0xffff8001
#define HANDLE_DEFAULT_SYSBUS    0xffff8002
#define HANDLE_X_BUS             0xffff8003
#define HANDLE_PCI_BUS           0xffff8004
#define HANDLE_PREVIOUS_DETECTED 0xffff8005
#define HANDLE_CURRENT_DETECTED  0xffff8006

/****************************************************************************/
/*                                                                          */
/* Handle Types                                                             */
/*                                                                          */
/****************************************************************************/

#define HANDLE_TYPE_INVALID     0
#define HANDLE_TYPE_DRIVER      1
#define HANDLE_TYPE_ADAPTER     2
#define HANDLE_TYPE_DEVICE      3
#define HANDLE_TYPE_RESOURCE    4
#define HANDLE_TYPE_LOGDEV      5
#define HANDLE_TYPE_SYSDEV      6
#define HANDLE_TYPE_DETECTED    7

/****************************************************************************/
/*                                                                          */
/* Search ID Flags                                                          */
/*                                                                          */
/****************************************************************************/
typedef ULONG SEARCHIDFLAGS;

#define   SEARCH_ID_DEVICEID      1
#define   SEARCH_ID_FUNCTIONID    2
#define   SEARCH_ID_COMPATIBLEID  4
#define   SEARCH_ID_VENDOR			 8
#define   SEARCH_ID_SERIAL  		16

/****************************************************************************/
/*                                                                          */
/* Convert ID                                                               */
/*                                                                          */
/****************************************************************************/

#define   RM_CONVERT_TO_TEXT      1
#define   RM_CONVERT_TO_ID        2

/****************************************************************************/
/*                                                                          */
/* SNOOPLEVEL definitions                                                   */
/*                                                                          */
/****************************************************************************/

typedef enum {SNP_NO_RISK,SNP_LOW_RISK,SNP_MEDIUM_RISK,SNP_HIGH_RISK,SNP_WARP_RISK } SNOOPLEVEL;

#define SNP_FLG_NEXTBOOT  1
#define SNP_FLG_DEFAULT   2

/****************************************************************************/
/*                                                                          */
/* RMINFO Flag definitions                                                  */
/*                                                                          */
/****************************************************************************/

#define RM_SAVE_DEFAULT 0
#define RM_DELETE_DEFAULT 0

#endif /* __RM_HEADER__ */


/****************************************************************************/
/*                                                                          */
/* Comon Keys                                                               */
/*                                                                          */
/****************************************************************************/

#define KEY_FIXDISK      "DISK_#      "
#define KEY_TAPE         "TAPE_#      "
#define KEY_PRINTER      "PRINTER_#      "
#define KEY_CPU          "CPU_#      "
#define KEY_WORM         "WORM_#      "
#define KEY_CDROM        "CDROM_#      "
#define KEY_SCANNER      "SCANNER_#      "
#define KEY_OPTICAL_MEM  "OPTICAL_MEM_#      "
#define KEY_CHANGER      "CHANGER_#      "
#define KEY_COMM         "COMM_#      "
#define KEY_SCSI_UNKNOWN "UNKNOWN_#      "

/****************************************************************************/
/*                                                                          */
/* DDtype for RMGetCommandLine()                                            */
/*                                                                          */
/****************************************************************************/

#define RM_CMD_BASE		0
#define RM_CMD_DEVICE	1

