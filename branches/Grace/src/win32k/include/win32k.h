/* $Id: win32k.h,v 1.4.4.1 2000-08-29 19:47:01 bird Exp $
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
#define K32_QUERYOTES           0x02

/*
 * Elf category
 */
#define ELF_DUMMY               0x01


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)

/*
 * Object Table Entry buffer.
 */
typedef struct _QueryOTE
{
    ULONG   ote_size;                   /* Object virtual size */
    ULONG   ote_base;                   /* Object base virtual address */
    ULONG   ote_flags;                  /* Attribute flags */
    ULONG   ote_pagemap;                /* Object page map index */
    ULONG   ote_mapsize;                /* Num of entries in obj page map */
  /*ULONG   ote_reserved;*/
    USHORT  ote_sel;                    /* Object Selector */
    USHORT  ote_hob;                    /* Object Handle */
} QOTE, *PQOTE;

typedef struct _QueryOTEBuffer
{
    ULONG   cOTEs;                      /* Number of entries in aOTE. */
    QOTE    aOTE[1];                    /* Array of OTEs. */
} QOTEBUFFER, *PQOTEBUFFER;


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

typedef struct _k32QueryOTEs
{
    HMODULE     hMTE;                   /* Module handle. */
    PQOTEBUFFER pQOte;                  /* Pointer to output buffer. */
    ULONG       cbQOte;                 /* Size of the buffer pointed to by pQOte  */
    ULONG       rc;                     /* Return code. */
} K32QUERYOTES, *PK32QUERYOTES;


#pragma pack()

#ifndef NO_WIN32K_LIB_FUNCTIONS
/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
APIRET APIENTRY  libWin32kInit(void);
APIRET APIENTRY  libWin32kTerm(void);
USHORT APIENTRY  libHelperGetCS(void);
BOOL   APIENTRY  libWin32kInstalled(void);
APIRET APIENTRY  DosAllocMemEx(PPVOID ppv, ULONG cb, ULONG flag);
APIRET APIENTRY  W32kQueryOTEs(HMODULE hMTE, PQOTEBUFFER pQOte, ULONG cbQOte);


#endif

#endif
