/* 
 * THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
 * PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
 * TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
 * INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
 * DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
 * THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
 * FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
 * 
 * USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
 * RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
 * TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
 * AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
 * SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
 * THE UNITED STATES.  
 * 
 * COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
 */

#ifndef _FXLINUX_H_
#define _FXLINUX_H_

#define FX_MAJOR_VER 0
#define FX_MINOR_VER 0

extern FxU32 PCI_VENDOR_ID_LINUX;
extern FxU32 PCI_DEVICE_ID_LINUX;
extern FxU32 PCI_COMMAND_LINUX;
extern FxU32 PCI_BASE_ADDRESS_0_LINUX;
extern FxU32 SST1_PCI_INIT_ENABLE_LINUX;
extern FxU32 SST1_PCI_BUS_SNOOP0_LINUX;
extern FxU32 SST1_PCI_BUS_SNOOP1_LINUX;

FxBool
hasDev3DfxOS2(void);

FxBool 
pciInitializeOS2(void);

FxBool
pciCloseOS2( void );

FxU8 
pioInByte(unsigned short port);

FxU16 
pioInWord(unsigned short port);

FxU32 
pioInLong(unsigned short port);

FxBool 
pioOutByte(unsigned short port, FxU8 data);

FxBool 
pioOutWord(unsigned short port, FxU16 data);

FxBool 
pioOutLong(unsigned short port, FxU32 data);

FxBool 
pciMapPhysicalToLinearOS2( FxU32 *linear_addr, FxU32 physical_addr,
			FxU32 *length );
void 
pciUnmapPhysicalOS2( FxU32 linear_addr, FxU32 length );

int
getNumDevicesOS2(void);

FxU32
pciFetchRegisterOS2( FxU32 cmd, FxU32 size, FxU32 device);

int
pciUpdateRegisterOS2(FxU32 cmd, FxU32 data, FxU32 size, FxU32 device);

#endif
