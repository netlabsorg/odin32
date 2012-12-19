/* $Id: winaccel.h,v 1.2 1999-10-08 09:52:02 sandervl Exp $ */

/*
 * PE2LX accelerator resource support code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINACCELERATOR_H_
#define __WINACCELERATOR_H_

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
    BYTE   fVirt;
    BYTE   pad0;
    WORD   key;
    WORD   cmd;
    WORD   pad1;
} WINACCEL, *LPWINACCEL;
#pragma pack()

#endif
