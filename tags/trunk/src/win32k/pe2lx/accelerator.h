/* $Id: accelerator.h,v 1.1 1999-09-06 02:20:03 bird Exp $ */

/*
 * PE2LX accelerator resource support code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __ACCELERATOR_H_
#define __ACCELERATOR_H_

/*
 * Defines for the fVirt field of the Accelerator table structure.
 */
#define FVIRTKEY  TRUE          /* Assumed to be == TRUE */
#define FNOINVERT 0x02
#define FSHIFT    0x04
#define FCONTROL  0x08
#define FALT      0x10

#pragma pack(1)
typedef struct tagWINACCEL {
    BYTE   fVirt;               /* Also called the flags field */
    WORD   key;
    WORD   cmd;
    BYTE   align[3];
} WINACCEL, *LPWINACCEL;
#pragma pack()

BOOL  ShowAccelerator(LXHeaderSuper &OS2Exe, int id, WINACCEL *accdata, int size);
void *ConvertAccelerator(WINACCEL *accdata, int size, PULONG pulSize);
ULONG QuerySizeAccelerator(WINACCEL accdata, int size);

#endif
