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
*/

#define INCL_DOS
#define INCL_DOSDEVIOCTL
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSMODULEMGR
#define INCL_DOSDEVICES
#include <os2wrap.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <types.h>
////#include <unistd.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

#include "3dfx.h"
#include "fxpci.h"
#include "pcilib.h"
#include "fxos2.h"

#ifdef __WIN32OS2__
#define FX_CSTYLE __stdcall
#include <misc.h>
#else
#define FX_CSTYLE _System
#endif

#pragma pack(1)

#ifdef DEBUG
#define DEBUGMSG(x) printf x
#else
#define DEBUGMSG(x)
#endif

#define PCI_VENDOR_ID_OS2 0x0
#define PCI_DEVICE_ID_OS2 0x2
#define PCI_COMMAND_OS2 0x4
#define PCI_REVISION_ID_OS2 0x8
#define PCI_BASE_ADDRESS_0_OS2 0x10
#define PCI_BASE_ADDRESS_1_OS2 0x14
#define SST1_PCI_INIT_ENABLE_OS2 0x40
#define SST1_PCI_BUS_SNOOP0_OS2 0x44
#define SST1_PCI_BUS_SNOOP1_OS2 0x48
#define SST1_PCI_SPECIAL1_OS2 0x40
#define SST1_PCI_SPECIAL2_OS2 0x44
#define SST1_PCI_SPECIAL3_OS2 0x48
#define SST1_PCI_SPECIAL4_OS2 0x54
#define SST1_PCI_VCLK_ENABLE  0xc0
#define SST1_PCI_VCLK_DISABLE 0xe0


#define PCI_VENDOR_ID_3DFX 0x121a
#define PCI_DEVICE_ID_3DFX_VOODOO  1
#define PCI_DEVICE_ID_3DFX_VOODOO2 2
#define PCI_DEVICE_ID_3DFX_BANSHEE 3
#define PCI_DEVICE_ID_3DFX_VOODOO3 5
#define PCI_VENDOR_ID_ALLIANCE 0x1142
#define PCI_DEVICE_ID_ALLIANCE_AT3D 0x643d

static const char* pciIdentifyOS2(void);
static FxBool pciOutputStringOS2(const char *msg);
static FxBool pciInitializeOS2(void);
static FxBool pciShutdownOS2(void);
static FxBool pciMapLinearOS2(FxU32, FxU32 physical_addr, FxU32 *linear_addr,
				FxU32 *length);
static FxBool pciUnmapLinearOS2(FxU32 linear_addr, FxU32 length);
static FxBool pciSetPermissionOS2(const FxU32, const FxU32, const FxBool);
static FxU8 pciPortInByteOS2(unsigned short port);
static FxU16 pciPortInWordOS2(unsigned short port);
static FxU32 pciPortInLongOS2(unsigned short port);
static FxBool pciPortOutByteOS2(unsigned short port, FxU8 data);
static FxBool pciPortOutWordOS2(unsigned short port, FxU16 data);
static FxBool pciPortOutLongOS2(unsigned short port, FxU32 data);
static FxBool pciMsrGetOS2(MSRInfo *, MSRInfo *);
static FxBool pciMsrSetOS2(MSRInfo *, MSRInfo *);
static FxBool pciSetPassThroughBaseOS2(FxU32 *, FxU32);

const FxPlatformIOProcs ioProcsOS2 = {
  pciInitializeOS2,
  pciShutdownOS2,
  pciIdentifyOS2,

  pciPortInByteOS2,
  pciPortInWordOS2,
  pciPortInLongOS2,

  pciPortOutByteOS2,
  pciPortOutWordOS2,
  pciPortOutLongOS2,

  pciMapLinearOS2,
  pciUnmapLinearOS2,
  pciSetPermissionOS2,

  pciMsrGetOS2,
  pciMsrSetOS2,

  pciOutputStringOS2,
  pciSetPassThroughBaseOS2
};

static int linuxDevFd=-1;

typedef struct pioData_t {
  short port;
  short size;
  int device;
  void *value;
} ;

struct cardInfo_t {
        int vendor;
        int type;
        ULONG addr0;
        ULONG addr1;
        PULONG laddr0;
        PULONG laddr1;
        unsigned char bus;
        unsigned char dev;
};

struct pci_dev
{
   unsigned char bus;
   unsigned char devfn;
};

typedef struct pioData_t pioData;
typedef struct cardInfo_t cardInfo;

#define MAXCARDS 16

static HFILE hOemHlp;
static HFILE hTestCfg;
static HFILE hScreenDD;

static cardInfo cards[MAXCARDS];
static int numCards = 0;

static UINT os2_pci_query( HFILE hOemHlp, USHORT usDeviceID, USHORT usVendorID, UCHAR ucIndex, PUCHAR pucBus, PUCHAR pucDevFn )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     UCHAR  ucSubFunction;
     USHORT usDeviceID;
     USHORT usVendorID;
     UCHAR  ucIndex;
     } pcifindreq;
  struct
     {
     UCHAR ucRC;
     UCHAR ucBus;
     UCHAR ucDevFunc;
     } pcifindrep;

  pcifindreq.ucSubFunction = 1;          // find pci device
  pcifindreq.usDeviceID    = usDeviceID; // pci device
  pcifindreq.usVendorID    = usVendorID; // pci vendor
  pcifindreq.ucIndex       = ucIndex;    // entry

  memset( &pcifindrep, 0, sizeof(pcifindrep) );

  cbParmLenMax = sizeof(pcifindreq);
  cbDataLenMax = sizeof(pcifindrep);

  rc = DosDevIOCtl( hOemHlp, IOCTL_OEMHLP, OEMHLP_PCI, 
                    &pcifindreq, cbParmLenMax, &cbParmLenMax,
                    &pcifindrep, cbDataLenMax, &cbDataLenMax);

  if ( pcifindrep.ucRC == 0 )
     {
     *pucBus = pcifindrep.ucBus;
     *pucDevFn = pcifindrep.ucDevFunc;
     return TRUE;
     }

  return FALSE;
}

static void os2_pci_read( HFILE hOemHlp, UCHAR ucBus, UCHAR ucDevFn, UCHAR ucIndex, PULONG pulData, UCHAR ucSize )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     UCHAR ucSubFunction;
     UCHAR ucBus;
     UCHAR ucDevFunc;
     UCHAR ucCfgReg;
     UCHAR ucSize;
     } pcireadreq;
  struct
     {
     UCHAR ucRC;
     ULONG ulData;
     } pcireadrep;

  pcireadreq.ucSubFunction = 3;    // read pci device
  pcireadreq.ucBus     = ucBus;
  pcireadreq.ucDevFunc = ucDevFn;
  pcireadreq.ucCfgReg  = ucIndex;
  pcireadreq.ucSize    = ucSize;

  rc = DosDevIOCtl( hOemHlp, IOCTL_OEMHLP, OEMHLP_PCI, 
                    &pcireadreq, sizeof(pcireadreq), &cbParmLenMax,
                    &pcireadrep, sizeof(pcireadrep), &cbDataLenMax);

  *pulData = pcireadrep.ulData;
}

static void os2_pci_write( HFILE hOemHlp, UCHAR ucBus, UCHAR ucDevFn, UCHAR ucIndex, ULONG ulData, UCHAR ucSize )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     UCHAR ucSubFunction;
     UCHAR ucBus;
     UCHAR ucDevFunc;
     UCHAR ucCfgReg;
     UCHAR ucSize;
     ULONG ulData;
     } pciwritereq;
  struct
     {
     UCHAR ucRC;
     ULONG ulData;
     } pciwriterep;

  pciwritereq.ucSubFunction = 4;    // write pci device
  pciwritereq.ucBus     = ucBus;
  pciwritereq.ucDevFunc = ucDevFn;
  pciwritereq.ucCfgReg  = ucIndex;
  pciwritereq.ucSize    = ucSize;
  pciwritereq.ulData    = ulData;

  rc = DosDevIOCtl( hOemHlp, IOCTL_OEMHLP, OEMHLP_PCI, 
                    &pciwritereq, sizeof(pciwritereq), &cbParmLenMax,
                    &pciwriterep, sizeof(pciwriterep), &cbDataLenMax);
}

static struct pci_dev *pci_find_device(int vendor, int device, char index, struct pci_dev *dev)
{
  UINT bFound;

  bFound = os2_pci_query( hOemHlp, device, vendor, index, &dev->bus, &dev->devfn );

  return bFound ? dev : 0;
}

static void pci_read_config_dword(unsigned int bus, unsigned int devfn, char index, PULONG pulDest)
{
  os2_pci_read( hOemHlp, bus, devfn, index, pulDest, 4 );
}

static void pci_read_config_word(unsigned int bus, unsigned int devfn, char index, PUSHORT pusDest)
{
   ULONG ulDest;
   os2_pci_read( hOemHlp, bus, devfn, index, &ulDest, 4 );
   *pusDest = (USHORT)ulDest;
}

static void pci_read_config_byte(unsigned int bus, unsigned int devfn, char index, PUCHAR pucDest)
{
   ULONG ulDest;
   os2_pci_read( hOemHlp, bus, devfn, index, &ulDest, 4 );
   *pucDest = (UCHAR)ulDest;
}

static void pci_write_config_dword(unsigned int bus, unsigned int devfn, char index, ULONG ulDest)
{
  os2_pci_write( hOemHlp, bus, devfn, index, ulDest, 4 );
}

static FxU32 mmap_allocate( ULONG ulAddr )
{
  ULONG cbParmLenMax;
  APIRET rc;
  struct
     {
     ULONG ulPacketLength;
     ULONG ulPhysicalAddress;
     ULONG ulMemorySize;
     ULONG ulLinearAddress;
     ULONG ulLinearFlags;
     } memmapreq;

  memmapreq.ulPacketLength = sizeof(memmapreq);
  memmapreq.ulPhysicalAddress = ulAddr;
  memmapreq.ulMemorySize = 0x1000000;
  memmapreq.ulLinearAddress = 0;
  memmapreq.ulLinearFlags = 0x410;

  cbParmLenMax = sizeof(memmapreq);

  rc = DosDevIOCtl( hScreenDD, SCREENDD_CATEGORY, SCREENDD_GETLINEARACCESS, 
                    &memmapreq, cbParmLenMax, &cbParmLenMax,
                    0, 0, 0);

  return memmapreq.ulLinearAddress;
}

int _System io_init1(void)
{
   return 0;
}

int _System io_exit1(void)
{
   return 0;
}

char _System c_inb1 ( short sPort )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     USHORT usPort;
     USHORT usSize;
     } ioreq;
  struct
     {
     LONG lValue;
     } iorep;

  ioreq.usPort = (USHORT) sPort;
  ioreq.usSize = sizeof(CHAR);

  memset( &iorep, 0, sizeof(iorep) );

  cbParmLenMax = sizeof(ioreq);
  cbDataLenMax = sizeof(iorep);

  rc = DosDevIOCtl( hTestCfg, IOCTL_TESTCFG_SYS, TESTCFG_SYS_ISSUEINIOINSTR, 
                    &ioreq, cbParmLenMax, &cbParmLenMax,
                    &iorep, cbDataLenMax, &cbDataLenMax);

  return (CHAR) iorep.lValue;
}

short _System c_inw1 ( short sPort )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     USHORT usPort;
     USHORT usSize;
     } ioreq;
  struct
     {
     LONG lValue;
     } iorep;

  ioreq.usPort = (USHORT) sPort;
  ioreq.usSize = sizeof(SHORT);

  memset( &iorep, 0, sizeof(iorep) );

  cbParmLenMax = sizeof(ioreq);
  cbDataLenMax = sizeof(iorep);

  rc = DosDevIOCtl( hTestCfg, IOCTL_TESTCFG_SYS, TESTCFG_SYS_ISSUEINIOINSTR, 
                    &ioreq, cbParmLenMax, &cbParmLenMax,
                    &iorep, cbDataLenMax, &cbDataLenMax);

  return (SHORT) iorep.lValue;
}

long _System c_inl1 ( short sPort )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     USHORT usPort;
     USHORT usSize;
     } ioreq;
  struct
     {
     LONG lValue;
     } iorep;

  ioreq.usPort = (USHORT) sPort;
  ioreq.usSize = sizeof(LONG);

  memset( &iorep, 0, sizeof(iorep) );

  cbParmLenMax = sizeof(ioreq);
  cbDataLenMax = sizeof(iorep);

  rc = DosDevIOCtl( hTestCfg, IOCTL_TESTCFG_SYS, TESTCFG_SYS_ISSUEINIOINSTR, 
                    &ioreq, cbParmLenMax, &cbParmLenMax,
                    &iorep, cbDataLenMax, &cbDataLenMax);

  return iorep.lValue;
}

void _System c_outb1 ( short sPort, char cValue )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     USHORT usPort;
     USHORT usSize;
     } ioreq;
  struct
     {
     LONG lValue;
     } iorep;

  ioreq.usPort = (USHORT) sPort;
  ioreq.usSize = sizeof(CHAR);

  iorep.lValue = (LONG) cValue;

  cbParmLenMax = sizeof(ioreq);
  cbDataLenMax = sizeof(iorep);

  rc = DosDevIOCtl( hTestCfg, IOCTL_TESTCFG_SYS, TESTCFG_SYS_ISSUEOUTIOINSTR, 
                    &ioreq, cbParmLenMax, &cbParmLenMax,
                    &iorep, cbDataLenMax, &cbDataLenMax);
}

void _System c_outw1 ( short sPort, short sValue )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     USHORT usPort;
     USHORT usSize;
     } ioreq;
  struct
     {
     LONG lValue;
     } iorep;

  ioreq.usPort = (USHORT) sPort;
  ioreq.usSize = sizeof(SHORT);

  iorep.lValue = (LONG) sValue;

  cbParmLenMax = sizeof(ioreq);
  cbDataLenMax = sizeof(iorep);

  rc = DosDevIOCtl( hTestCfg, IOCTL_TESTCFG_SYS, TESTCFG_SYS_ISSUEOUTIOINSTR, 
                    &ioreq, cbParmLenMax, &cbParmLenMax,
                    &iorep, cbDataLenMax, &cbDataLenMax);
}

void _System c_outl1 ( short sPort, long lValue )
{
  ULONG cbParmLenMax, cbDataLenMax;
  APIRET rc;
  struct
     {
     USHORT usPort;
     USHORT usSize;
     } ioreq;
  struct
     {
     LONG lValue;
     } iorep;

  ioreq.usPort = (USHORT) sPort;
  ioreq.usSize = sizeof(LONG);

  iorep.lValue = (LONG) lValue;

  cbParmLenMax = sizeof(ioreq);
  cbDataLenMax = sizeof(iorep);

  rc = DosDevIOCtl( hTestCfg, IOCTL_TESTCFG_SYS, TESTCFG_SYS_ISSUEOUTIOINSTR, 
                    &ioreq, cbParmLenMax, &cbParmLenMax,
                    &iorep, cbDataLenMax, &cbDataLenMax);
}

static void findCardType(int vendor, int device)
{
	struct pci_dev dev = {0};
	char index = 0;

	while (numCards < MAXCARDS && pci_find_device(vendor, device, index, &dev)) {
		pci_read_config_dword(dev.bus, dev.devfn, PCI_BASE_ADDRESS_0_OS2, &cards[numCards].addr0);
		pci_read_config_dword(dev.bus, dev.devfn, PCI_BASE_ADDRESS_1_OS2, &cards[numCards].addr1);
		cards[numCards].bus = dev.bus;
		cards[numCards].dev = dev.devfn;
		cards[numCards].addr0 &= ~0xF;
		cards[numCards].addr1 &= ~0xF;
		cards[numCards].vendor = vendor;
		cards[numCards].type = device;

		DEBUGMSG(("3dfx: board vendor %x type %x located at %x/%x bus %x dev %x\n",
		    vendor, device, cards[numCards].addr0, cards[numCards].addr1, cards[numCards].bus, cards[numCards].dev));

		++numCards;
		++index;
	}
}

static int findCards(void)
{
	numCards = 0;
	findCardType(PCI_VENDOR_ID_3DFX, PCI_DEVICE_ID_3DFX_VOODOO);
	findCardType(PCI_VENDOR_ID_3DFX, PCI_DEVICE_ID_3DFX_VOODOO2);
	findCardType(PCI_VENDOR_ID_ALLIANCE, 0x643d);
	findCardType(PCI_VENDOR_ID_3DFX, PCI_DEVICE_ID_3DFX_BANSHEE);
	findCardType(PCI_VENDOR_ID_3DFX, PCI_DEVICE_ID_3DFX_VOODOO3);
	return numCards;
}

FxBool
pciPlatformInit(void)
{
  gCurPlatformIO = &ioProcsOS2;
  return FXTRUE;
}

FxBool 
hasDev3DfxOS2(void) 
{
  if (numCards==0) return FXFALSE;
  return FXTRUE;
}

static int doQueryFetchOS2(pioData *desc)
{
	int retval;
	UCHAR retchar;
	USHORT retword;
	ULONG retlong;

	if (desc->device < 0 || desc->device >= numCards)
		return -1;
	switch (desc->port) {
	case PCI_VENDOR_ID_OS2:
		if (desc->size != 2)
			return -1;
		memcpy(desc->value, &cards[desc->device].vendor, desc->size);
		return 0;
	case PCI_DEVICE_ID_OS2:
		if (desc->size != 2)
			return -1;
		memcpy(desc->value, &cards[desc->device].type, desc->size);
		return 0;
	case PCI_BASE_ADDRESS_0_OS2:
		if (desc->size != 4)
			return -1;
		memcpy(desc->value, &cards[desc->device].addr0, desc->size);
		return 0;
	case PCI_BASE_ADDRESS_1_OS2:
		if (desc->size != 4)
			return -1;
		memcpy(desc->value, &cards[desc->device].addr1, desc->size);
		return 0;
	case SST1_PCI_SPECIAL1_OS2:
		if (desc->size != 4)
			return -1;
		break;
	case PCI_REVISION_ID_OS2:
		if (desc->size != 1)
			return -1;
		break;
	case SST1_PCI_SPECIAL4_OS2:
		if (desc->size != 4)
			return -1;
		break;

	default:
		return -1;
	}
	switch (desc->size) {
	case 1:
		pci_read_config_byte(cards[desc->device].bus, cards[desc->device].dev, desc->port, &retchar);
		memcpy(desc->value, &retchar, 1);
		break;
	case 2:
		pci_read_config_word(cards[desc->device].bus, cards[desc->device].dev, desc->port, &retword);
		memcpy(desc->value, &retword, 2);
		break;
	case 4:
		pci_read_config_dword(cards[desc->device].bus, cards[desc->device].dev, desc->port, &retlong);
		memcpy(desc->value, &retlong, 4);
		break;
	default:
		return -1;
	}
	return 0;
}

FxU32
pciFetchRegisterOS2( FxU32 cmd, FxU32 size, FxU32 device) 
{
  pioData desc;
  char cval;
  short sval;
  int ival;

  if (numCards==0) return -1;
  desc.port=cmd;
  desc.size=size;
  desc.device=device;
  switch (size) {
  case 1:
    desc.value=&cval;
    break;
  case 2:
    desc.value=&sval;
    break;
  case 4:
    desc.value=&ival;
    break;
  default:
    return 0;
  }
  if (doQueryFetchOS2(&desc)==-1)
    return 0;
  switch (size) {
  case 1:
    return cval;
  case 2:
    return sval;
  case 4:
    return ival;
  default:
    return 0;
  }
}

static int doQueryUpdateOS2(pioData *desc)
{
	ULONG retval;
	ULONG preval;
	int mask;
	UCHAR retchar;
	USHORT retword;
	ULONG retlong;

	if (desc->device < 0 || desc->device >= numCards)
		return -1;
	switch (desc->port) {
	case PCI_COMMAND_OS2:
		if (desc->size != 2)
			return -1;
		break;
	case SST1_PCI_SPECIAL1_OS2:
		if (desc->size != 4)
			return -1;
		break;
	case SST1_PCI_SPECIAL2_OS2:
		if (desc->size != 4)
			return -1;
		break;
	case SST1_PCI_SPECIAL3_OS2:
		if (desc->size != 4)
			return -1;
		break;
	case SST1_PCI_SPECIAL4_OS2:
		if (desc->size != 4)
			return -1;
		break;

	case SST1_PCI_VCLK_ENABLE:
		if (desc->size != 4)
			return -1;
		break;

	case SST1_PCI_VCLK_DISABLE:
		if (desc->size != 4)
			return -1;
		break;

	default:
		return -1;
	}
	pci_read_config_dword(cards[desc->device].bus, cards[desc->device].dev, desc->port & ~0x3, &retval);
	switch (desc->size) {
	case 1:
		memcpy(&retchar, desc->value, 1);
		preval = retchar << (8 * (desc->port & 0x3));
		mask = 0xFF << (8 * (desc->port & 0x3));
		break;
	case 2:
		memcpy(&retword, desc->value, 2);
		preval = retword << (8 * (desc->port & 0x3));
		mask = 0xFFFF << (8 * (desc->port & 0x3));
		break;
	case 4:
		memcpy(&retlong, desc->value, 4);
		preval = retlong;
		mask = ~0;
		break;
	default:
		return -1;
	}
	retval = (retval & ~mask) | preval;
	pci_write_config_dword(cards[desc->device].bus, cards[desc->device].dev, desc->port, retval);
	return 0;
}

FxBool
pciUpdateRegisterOS2(FxU32 cmd, FxU32 data, FxU32 size, FxU32 device) {
  pioData desc;

  if (numCards==0) return -1;
  desc.port=cmd;
  desc.size=size;
  desc.device=device;
  desc.value=&data;
  if (doQueryUpdateOS2(&desc)==-1)
    return FXFALSE;
  return FXTRUE;
}

int 
getNumDevicesOS2(void) 
{
  if (numCards==0) return -1;
  return numCards;
}

static const char*
pciIdentifyOS2(void)
{
  return "fxPCI for OS2";
}

static FxBool 
pciOutputStringOS2(const char *msg) 
{
  printf(msg);
  return FXTRUE;
}

static FxBool 
pciInitializeOS2(void)
{
  ULONG  ulAction;
  APIRET rc;

  io_init1();

tryopen1:
  rc = DosOpen("OEMHLP$", &hOemHlp, &ulAction, 0, FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS, OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYREADWRITE, 0L);
  if(rc == ERROR_TOO_MANY_OPEN_FILES) {
   ULONG CurMaxFH;
   LONG  ReqCount = 32;

	rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
	if(rc) {
		dprintf(("DosSetRelMaxFH returned %d", rc));
     		pciErrorCode = PCI_ERR_NO_IO_PERM;
		return FXFALSE;
	}
	dprintf(("DosOpen failed -> increased nr open files to %d", CurMaxFH));
	goto tryopen1;
  }
  else
  if(rc) {
     pciErrorCode = PCI_ERR_NO_IO_PERM;
     return FXFALSE;
  }
tryopen2:
  rc = DosOpen("TESTCFG$", &hTestCfg, &ulAction, 0, FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS, OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE, 0L );
  if(rc == ERROR_TOO_MANY_OPEN_FILES) {
   ULONG CurMaxFH;
   LONG  ReqCount = 32;

	rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
	if(rc) {
		dprintf(("DosSetRelMaxFH returned %d", rc));
     		pciErrorCode = PCI_ERR_NO_IO_PERM;
		return FXFALSE;
	}
	dprintf(("DosOpen failed -> increased nr open files to %d", CurMaxFH));
	goto tryopen2;
  }
  else
  if(rc) {
     pciErrorCode = PCI_ERR_NO_IO_PERM;
     return FXFALSE;
  }
tryopen3:
  rc = DosOpen("SCREEN$", &hScreenDD, &ulAction, 0, FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS, OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE, 0L );
  if(rc == ERROR_TOO_MANY_OPEN_FILES) {
   ULONG CurMaxFH;
   LONG  ReqCount = 32;

	rc = DosSetRelMaxFH(&ReqCount, &CurMaxFH);
	if(rc) {
		dprintf(("DosSetRelMaxFH returned %d", rc));
     		pciErrorCode = PCI_ERR_NO_IO_PERM;
		return FXFALSE;
	}
	dprintf(("DosOpen failed -> increased nr open files to %d", CurMaxFH));
	goto tryopen3;
  }
  else
  if(rc) {
     pciErrorCode = PCI_ERR_NO_IO_PERM;
     return FXFALSE;
  }
  findCards();
  if (numCards==0) {
     pciErrorCode = PCI_ERR_NO_IO_PERM;
     return FXFALSE;
  }
  return FXTRUE;
}

static FxBool 
pciShutdownOS2(void)
{
  DosClose( hTestCfg );
  DosClose( hScreenDD );
  DosClose( hOemHlp );
  io_exit1();
  return FXTRUE;
}

static FxBool 
pciMapLinearOS2(FxU32 bus, FxU32 physical_addr, FxU32 *linear_addr, FxU32 *length) 
{
  *linear_addr = mmap_allocate(physical_addr);
  return FXTRUE;
}

static FxBool
pciUnmapLinearOS2(FxU32 linear_addr, FxU32 length) 
{
  return FXTRUE;
}

static FxBool
pciSetPermissionOS2(const FxU32 addrBase, const FxU32 addrLen, const FxBool writePermP)
{
  return FXTRUE;
}

static FxU8 
pciPortInByteOS2(unsigned short port) 
{
  return c_inb1(port);
}

static FxU16 
pciPortInWordOS2(unsigned short port) 
{
  return c_inw1(port);
}

static FxU32 
pciPortInLongOS2(unsigned short port) 
{
  return c_inl1(port);
}

static FxBool 
pciPortOutByteOS2(unsigned short port, FxU8 data) 
{
  c_outb1(port,data);
  return FXTRUE;
}

static FxBool 
pciPortOutWordOS2(unsigned short port, FxU16 data) 
{
  c_outw1(port,data);
  return FXTRUE;
}

static FxBool 
pciPortOutLongOS2(unsigned short port, FxU32 data) 
{
  c_outl1(port,data);
  return FXTRUE;
}

static FxBool
pciMsrGetOS2(MSRInfo *in, MSRInfo *out)
{
  return FXTRUE;
}

static FxBool
pciMsrSetOS2(MSRInfo *in, MSRInfo *out)
{
  return FXTRUE;
}

static FxBool
pciSetPassThroughBaseOS2(FxU32 *baseAddr, FxU32 baseAddrLen)
{
  return FXTRUE;
}

FxBool FX_CSTYLE pciFindFreeMTRR(FxU32 *mtrrNum)
{
  return FXFALSE;
}

FxBool FX_CSTYLE pciSetMTRR(FxU32 mtrrNum, FxU32 physBaseAddr, FxU32 physSize, PciMemType type)
{
  return FXTRUE;
}

FxBool FX_CSTYLE pciFindMTRRMatch(FxU32 physBaseAddress, FxU32 physSize, PciMemType type, FxU32 *mtrrNum)
{
  return FXFALSE;
}

