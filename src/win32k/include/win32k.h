/* $Id: win32k.h,v 1.3 2000-02-18 19:27:29 bird Exp $
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
#define K32_ALLOCMEMEX          0x01


/*
 * Elf category
 */
#define ELF_DUMMY               0x01


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/*
 * K32 category parameter structs
 */
typedef struct _k32AllocMemEx
{
    PVOID   pv;                         /* Pointer to allocated memory block */
                                        /* On input this holds the suggested */
                                        /* location of the block. */
    ULONG   cb;                         /* Blocksize (bytes) */
    ULONG   flFlags;                    /* Flags (equal to DosAllocMem flags) */
    ULONG   ulCS;                       /* Call CS */
    ULONG   ulEIP;                      /* Call EIP */
    ULONG   rc;                         /* Return code. */
} K32ALLOCMEMEX, *PK32ALLOCMEMEX;




#ifdef INCL_WIN32K_LIB
/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
APIRET APIENTRY  libWin32kInit(void);
APIRET APIENTRY  libWin32kTerm(void);
APIRET APIENTRY  DosAllocMemEx(PPVOID ppv, ULONG cb, ULONG flag);


#endif

#endif
