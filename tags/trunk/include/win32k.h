/* $Id: win32k.h,v 1.3 2000-12-11 06:17:20 bird Exp $
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
#define K32_QUERYOPTIONSSTATUS  0x03
#define K32_SETOPTIONS          0x04
#define K32_PROCESSREADWRITE    0x05

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
 * Options struct.
 *
 * (The function of these members are described in options.h in
 *  the src\win32k\include directory.)
 * All members of this struct (except cb ofcourse) is changable.
 */
typedef struct _K32Options
{
    ULONG   cb;

    /** @cat logging options */
    USHORT      usCom;                  /* Output port no. */
    ULONG       fLogging;               /* Logging. */

    /** @cat Options affecting the generated LX executables */
    ULONG       fPE;                    /* Flags set the type of conversion. */
    ULONG       ulInfoLevel;            /* Pe2Lx InfoLevel. */

    /** @cat Options affecting the generated ELF executables */
    ULONG       fElf;                   /* Elf flags. */

    /** @cat Options affecting the UNIX script executables */
    ULONG       fUNIXScript;            /* UNIX script flags. */

    /** @cat Options affecting the REXX script executables */
    ULONG       fREXXScript;            /* REXX script flags. */

    /** @cat Options affecting the JAVA executables */
    ULONG       fJava;                  /* Java flags. */

    /** @cat Options affecting the  executables */
    ULONG       fNoLoader;              /* No loader stuff. !FIXME! We should import / functions even if this flag is set!!! */

    /** @cat Options affecting the behaviour changes in the OS/2 loader */
    ULONG       fDllFixes;              /* Enables the long DLL name and non .DLL extention fixes. */

    /** @cat Options affecting the heap. */
    ULONG       cbSwpHeapMax;           /* Maximum heapsize. */
    ULONG       cbResHeapMax;           /* Maxiumem residentheapsize. */
} K32OPTIONS, *PK32OPTIONS;


/*
 * Status struct.
 *
 */
typedef struct _K32Status
{
    ULONG   cb;

    /** @cat Options status. */
    ULONG       fQuiet;                 /* Quiet initialization. */

    /** @cat Kernel status. */
    ULONG       fKernel;                /* Smp or uni kernel. */
    ULONG       ulBuild;                /* Kernel build. */
    USHORT      usVerMajor;             /* OS/2 major ver - 20 */
    USHORT      usVerMinor;             /* OS/2 minor ver - 30,40 */

    /** @cat Heap status. */
    ULONG       cbSwpHeapInit;          /* Initial heapsize. */
    ULONG       cbSwpHeapFree;          /* Amount of used space. */
    ULONG       cbSwpHeapUsed;          /* Amount of free space reserved. */
    ULONG       cbSwpHeapSize;          /* Amount of memory used by the heap free and used++. */
    ULONG       cSwpBlocksUsed;         /* Count of used blocks. */
    ULONG       cSwpBlocksFree;         /* Count of free blocks. */

    ULONG       cbResHeapInit;          /* Initial heapsize. */
    ULONG       cbResHeapFree;          /* Amount of free space reserved. */
    ULONG       cbResHeapUsed;          /* Amount of used space. */
    ULONG       cbResHeapSize;          /* Amount of memory used by the heap free and used++. */
    ULONG       cResBlocksUsed;         /* Count of used blocks. */
    ULONG       cResBlocksFree;         /* Count of free blocks. */

    /** @cat Win32k build, version and init stuff */
    CHAR        szBuildDate[12];        /* Date of the win32k build. (Sep 02 2000) */
    CHAR        szBuildTime[9];         /* Time of the win32k build. (11:44:21) */
    ULONG       ulVersion;              /* Win32k version */
    CHAR        szSymFile[CCHMAXPATH];  /* The name of the symbol file or sym database. */

    /** @cat Statistics */
    ULONG       cPe2LxModules;          /* Number of Pe2Lx modules currently loaded. */
    ULONG       cElf2LxModules;         /* Number of Elf2Lx modules currently loaded. */
    /*...*/
} K32STATUS, *PK32STATUS;



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

typedef struct _k32QueryOptionsStatus
{
    PK32OPTIONS     pOptions;           /* Pointer to option struct. (NULL allowed) */
    PK32STATUS      pStatus;            /* Pointer to status struct. (NULL allowed) */
    ULONG           rc;                 /* Return code. */
} K32QUERYOPTIONSSTATUS, *PK32QUERYOPTIONSSTATUS;

typedef struct _k32SetOptions
{
    PK32OPTIONS     pOptions;           /* Pointer to option struct. (NULL allowed) */
    ULONG           rc;                 /* Return code. */
} K32SETOPTIONS, *PK32SETOPTIONS;

typedef struct _k32ProcessReadWrite
{
    PID         pid;                    /* Process ID of the process to access memory in. */
    ULONG       cb;                     /* Number of bytes to read or write. */
    PVOID       pvSource;               /* Pointer to source data. */
    PVOID       pvTarget;               /* Pointer to target area. */
    BOOL        fRead;                  /* TRUE:   pvSource is within pid while pvTarget is ours. */
                                        /* FALSE:  pvTarget is within pid while pvSource is ours. */
    ULONG       rc;                     /* Return code. */
} K32PROCESSREADWRITE, *PK32PROCESSREADWRITE;



#pragma pack()

#ifndef NO_WIN32K_LIB_FUNCTIONS
/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
/* Win32k APIs */
APIRET APIENTRY  libWin32kInit(void);
APIRET APIENTRY  libWin32kTerm(void);
BOOL   APIENTRY  libWin32kInstalled(void);
APIRET APIENTRY  libWin32kQueryOptionsStatus(PK32OPTIONS pOptions, PK32STATUS pStatus);
APIRET APIENTRY  libWin32kSetOptions(PK32OPTIONS pOptions);
APIRET APIENTRY  libWin32kSetOption(PK32OPTIONS pOptions);

/* "Extra OS2 APIs" */
APIRET APIENTRY  DosAllocMemEx(PPVOID ppv, ULONG cb, ULONG flag);
APIRET APIENTRY  W32kQueryOTEs(HMODULE hMTE, PQOTEBUFFER pQOte, ULONG cbQOte);
APIRET APIENTRY  W32kProcessReadWrite(PID pid, ULONG cb, PVOID pvSource, PVOID pvTarget, BOOL fRead);

/* Helper function */
USHORT APIENTRY  libHelperGetCS(void);


#endif

#endif
