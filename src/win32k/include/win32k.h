/* $Id: win32k.h,v 1.1 2000-02-15 16:26:06 bird Exp $
 *
 * Top level make file for the Win32k library.
 * Contains library and 32-bit IOCtl definition.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _WIN32K_H_
#define _WIN32K_H_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * IOCtls categories.
 */
#define IOCTL_W32K_K32          0xC1
#define IOCTL_W32K_ELF          0xC2

/*
 * K32 category - these are the functions found in the k32 directory.
 */
#define K32_DOSALLOCMEMEX       0x01


/*
 * Elf category
 */
#define ELF_DUMMY               0x01



#ifdef INCL_WIN32K_LIB
/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
APIRET APIENTRY  DosAllocMemEx(PPVOID ppb, ULONG cb, ULONG flag);

#endif

#endif
