/* $Id: accelerator.h,v 1.3 1999-06-10 17:08:52 phaller Exp $ */

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

void ShowAccelerator(int id, WINACCEL *accdata, int size);

#endif
