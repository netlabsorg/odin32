/* $Id: OS2KLDR.h,v 1.1 2001-09-14 01:50:15 bird Exp $
 *
 * OS/2 kernel Loader Stuff.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _OS2KLDR_H_
#define _OS2KLDR_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct OTE
{
    ULONG    ote_size;             /* Object virtual size */
    ULONG    ote_base;             /* Object base virtual address */
    ULONG    ote_flags;            /* Attribute flags */
    ULONG    ote_pagemap;          /* Object page map index */
    ULONG    ote_mapsize;          /* Num of entries in obj page map */
  /*ULONG    ote_reserved;*/
    USHORT   ote_sel;              /* Object Selector */
    USHORT   ote_hob;              /* Object Handle */
} OTE, *POTE;


/* ote_flags */
#ifndef __EXE386__
  #define OBJREAD       0x00000001L     /* Readable Object */
  #define OBJWRITE      0x00000002L     /* Writeable Object */
  #define OBJEXEC       0x00000004L     /* Executable Object */
  #define OBJRSRC       0x00000008L     /* Resource Object */
  #define OBJDISCARD    0x00000010L     /* Object is Discardable */
  #define OBJSHARED     0x00000020L     /* Object is Shared */
  #define OBJPRELOAD    0x00000040L     /* Object has preload pages */
  #define OBJINVALID    0x00000080L     /* Object has invalid pages */
#endif
#define OBJZEROFIL      0x00000100L     /* Object has zero-filled pages */
#ifndef __EXE386__
  #define OBJRESIDENT   0x00000200L     /* Object is resident */
  #define OBJCONTIG     0x00000300L     /* Object is resident and contiguous */
  #define OBJDYNAMIC    0x00000400L     /* Object is permanent and long locable */
  #define OBJTYPEMASK   0x00000700L     /* Object type mask */
  #define OBJALIAS16    0x00001000L     /* 16:16 alias required */
  #define OBJBIGDEF     0x00002000L     /* Big/Default bit setting */
  #define OBJCONFORM    0x00004000L     /* Object is conforming for code */
  #define OBJIOPL       0x00008000L     /* Object I/O privilege level */
#endif
#define OBJMADEPRIV     0x40000000L     /* Object is made private for debug (now obsolete) */
#define OBJALLOC        0x80000000L     /* Object is allocated used by loader */






/*************************/
/* warp 3.0 AS SMTEs     */
/* Swap ModuleTableEntry */
/*************************/
typedef struct SMTE
{
    /* lxheader fields */
        /*- magic,border,worder,level,cpu,os,ver,mflags */
    ULONG   smte_mpages;        /* 00  Module # pages */
    ULONG   smte_startobj;      /* 04  Object # for instruction */
    ULONG   smte_eip;           /* 08  Extended instruction pointer */
    ULONG   smte_stackobj;      /* 0c  Object # for stack pointer */
    ULONG   smte_esp;           /* 10  Extended stack pointer */
        /*- pagesize*/
    ULONG   smte_pageshift;      /* 14  Page alignment shift in .EXE */
    ULONG   smte_fixupsize;     /* 18  Fixup section size */
        /*- fixupsum,ldrsize,ldrsum*/
    POTE    smte_objtab;        /* 1c  Object table offset - POINTER */
    ULONG   smte_objcnt;        /* 20  Number of objects in module */
    ULONG   smte_objmap;        /* 24  Object page map offset - POINTER */
    ULONG   smte_itermap;       /* 28  Object iterated data map offset */
    ULONG   smte_rsrctab;       /* 2c  Offset of Resource Table */
    ULONG   smte_rsrccnt;       /* 30  Number of resource entries */
    ULONG   smte_restab;        /* 34  Offset of resident name table - POINTER */
    ULONG   smte_enttab;        /* 38  Offset of Entry Table - POINTER */
    ULONG   smte_fpagetab;      /* 3c  Offset of Fixup Page Table - POINTER */
    ULONG   smte_frectab;       /* 40  Offset of Fixup Record Table - POINTER */
    ULONG   smte_impmod;        /* 44  Offset of Import Module Name Table - POINTER */
        /*- impmodcnt*/
    ULONG   smte_impproc;       /* 48  Offset of Imp Procedure Name Tab - POINTER */
        /*- pagesum*/
    ULONG   smte_datapage;      /* 4c  Offset of Enumerated Data Pages */
        /*- preload*/
    ULONG   smte_nrestab;       /* 50  Offset of Non-resident Names Table */
    ULONG   smte_cbnrestab;     /* 54  Size of Non-resident Name Table */
        /*- nressum*/
    ULONG   smte_autods;        /* 58  Object # for automatic data object */
    ULONG   smte_debuginfo;     /* 5c  Offset of the debugging info */
    ULONG   smte_debuglen;      /* 60  The len of the debug info in */
        /*- instpreload,instdemand*/
    ULONG   smte_heapsize;      /* 64  use for converted 16-bit modules */
        /*- res3*/
    /* extra */
    PCHAR   smte_path;          /* 68  full pathname - POINTER */
    USHORT  smte_semcount;      /* 6c  Count of threads waiting on MTE semaphore. 0 => semaphore is free */
    USHORT  smte_semowner;      /* 6e  Slot number of the owner of MTE semahore */
    ULONG   smte_pfilecache;    /* 70  Pointer to file cache for Dos32CacheModule */
    ULONG   smte_stacksize;     /* 74  Thread 1 Stack size from the exe header */
    USHORT  smte_alignshift;    /* 78  use for converted 16-bit modules */
    USHORT  smte_NEexpver;      /* 7a  expver from NE header */
    USHORT  smte_pathlen;       /* 7c  length of full pathname */
    USHORT  smte_NEexetype;     /* 7e  exetype from NE header */
    USHORT  smte_csegpack;      /* 80  count of segs to pack */
} SMTE, *PSMTE;








/********************/
/* warp 3.0 GA MTEs */
/* ModuleTableEntry */
/********************/
typedef struct MTE
{
   USHORT         mte_flags2;
   USHORT         mte_handle;
   PSMTE          mte_swapmte;   /* handle for swapmte */
   struct MTE *   mte_link;
   ULONG          mte_flags1;
   ULONG          mte_impmodcnt; /* number of entries in Import Module Name Table*/
   SFN            mte_sfn;       /*"filehandle"*/
   USHORT         mte_usecnt;    /* (.EXE only) - use count */
   CHAR           mte_modname[8];
} MTE,*PMTE,**PPMTE;


/***********/
/* flags 1 */
/***********/
   #define NOAUTODS            0x00000000   /* No Auto DS exists */
   #define SOLO                0x00000001   /* Auto DS is shared */
   #define INSTANCEDS          0x00000002   /* Auto DS is not shared */
   #define INSTLIBINIT         0x00000004   /* Perinstance Libinit */
   #define GINISETUP           0x00000008   /* Global Init has been setup */
   #define NOINTERNFIXUPS        0x00000010 /* internal fixups in .EXE.DLL applied */
   #define NOEXTERNFIXUPS      0x00000020   /* external fixups in .EXE.DLL applied */
   #define CLASS_PROGRAM       0x00000040   /* Program class */
   #define CLASS_GLOBAL        0x00000080   /* Global class */
   #define CLASS_SPECIFIC      0x000000C0   /* Specific class, as against global */
   #define CLASS_ALL           0x00000000   /* nonspecific class  all modules */
   #define CLASS_MASK          0x000000C0 /* */
   #define MTEPROCESSED        0x00000100   /* MTE being loaded */
   #define USED                0x00000200   /* MTE is referenced  see ldrgc.c */
   #define DOSLIB              0x00000400   /* set if DOSCALL1 */
   #define DOSMOD              0x00000800   /* set if DOSCALLS */
   #define MTE_MEDIAFIXED      0x00001000   /* File Media permits discarding */
   #define LDRINVALID          0x00002000   /* module not loadable */
   #define PROGRAMMOD          0x00000000   /* program module */
   #define DEVDRVMOD           0x00004000   /* device driver module */
   #define LIBRARYMOD          0x00008000   /* DLL module */
   #define VDDMOD              0x00010000   /* VDD module */
   #define MVDMMOD             0x00020000   /* Set if VDD Helper MTE (MVDM.DLL) */
   #define INGRAPH             0x00040000   /* In Module Graph  see ldrgc.c */
   #define GINIDONE            0x00080000   /* Global Init has finished */
   #define MTEADDRALLOCED      0x00100000   /* Allocate specific or not */
   #define FSDMOD              0x00200000   /* FSD MTE */
   #define FSHMOD              0x00400000   /* FS helper MTE */
   #define MTELONGNAMES        0x00800000   /* Module supports longnames */
   #define MTE_MEDIACONTIG     0x01000000   /* File Media contiguous memory req */
   #define MTE_MEDIA16M        0x02000000   /* File Media requires mem below 16M */
   #define MTESWAPONLOAD       0x04000000   /* make code pages swap on load */
   #define MTEPORTHOLE         0x08000000   /* porthole module */
   #define MTEMODPROT          0x10000000   /* Module has shared memory protected */
   #define MTENEWMOD           0x20000000   /* Newly added module */
   #define MTEDLLTERM          0x40000000   /* Gets instance termination */
   #define MTESYMLOADED        0x80000000   /* Set if debugger symbols loaded */


/***********/
/* flags 2 */
/***********/
   #define MTEFORMATMASK         0x0003     /* Module format mask */
   #define MTEFORMATR1           0x0000     /* Module format reserved */
   #define MTEFORMATNE           0x0001     /* Module format NE */
   #define MTEFORMATLX           0x0002     /* Module format LX */
   #define MTEFORMATR2           0x0003     /* Module format reserved */
   #define MTESYSTEMDLL          0x0004     /* DLL exists in system list */
   #define MTELOADORATTACH       0x0008     /* Module under load or attach  for init */
   #define MTECIRCLEREF          0x0010     /* Module circular reference detection */
   #define MTEFREEFIXUPS         0x0020     /* Free system mte's fixup flag d#98488 */
   #define MTEPRELOADED          0x0040     /* MTE Preload completed */
   #define MTEGETMTEDONE         0x0080     /* GetMTE already resolved */
   #define MTEPACKSEGDONE        0x0100     /* Segment packed memory allocated */
   #define MTE20LIELIST          0x0200     /* Name present in version20 lie list */
   #define MTESYSPROCESSED       0x0400     /* System DLL already processed */
   #define MTEDLLONEXTLST        0x1000     /* DLL has term routine on exit list #74177 */


#pragma pack()


#ifndef INCL_OS2KRNL_LDR_NOAPIS         /* Flag to exclude prototypes. */
#ifndef INCL_16     /* 16-bit test - function prototypes for 32-bit code only! */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define LDRCALL __stdcall

/** Additional ldrFindModule flag. Or together with the class. */
#define SEARCH_FULL_NAME    0x0001


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern PSZ*    pLdrLibPath;             /* Pointer to the libpath pointer. */
extern PPMTE   pmte_h;
extern PPMTE   pprogram_h;
extern PPMTE   pprogram_l;
extern PPMTE   pglobal_h;
extern PPMTE   pglobal_l;
extern PPMTE   pspecific_h;
extern PPMTE   pspecific_l;

/*******************************************************************************
*   Function Prototypes                                                        *
*******************************************************************************/
/**
 * _ldrClose
 */
extern ULONG LDRCALL ldrClose(  /* retd  0x04 */
    SFN p1                      /* ebp + 0x08 */
    );

ULONG LDRCALL myldrClose(SFN);


/**
 * _ldrOpen
 */
extern ULONG LDRCALL ldrOpen(   /* retd  0x0c */
    PSFN    p1,                 /* ebp + 0x08 */
    PCHAR   p2,                 /* ebp + 0x0c */
    PULONG  p3                  /* ebp + 0x10 */
    );

ULONG LDRCALL myldrOpen(PSFN phFile, PSZ pszFilename, PULONG pfl);


/**
 * _ldrRead
 */
extern ULONG LDRCALL ldrRead(   /* retd  0x18 */
    SFN     hFile,              /* ebp + 0x08 */
    ULONG   ulOffset,           /* ebp + 0x0c */
    PVOID   pvBuffer,           /* ebp + 0x10 */
    ULONG   fpBuffer,           /* ebp + 0x14 */
    ULONG   cbToRead,           /* ebp + 0x18 */
    PMTE    pMTE                /* ebp + 0x1c */
    );

ULONG LDRCALL myldrRead(
    SFN     hFile,
    ULONG   ulOffset,
    PVOID   pvBuffer,
    ULONG   fpBuffer,
    ULONG   cbToRead,
    PMTE    pMTE
    );



/**
 * _LDRQAppType
 */
extern ULONG LDRCALL LDRQAppType(   /* retd  0x08 */
    ULONG p1,                       /* ebp + 0x08 */
    ULONG p2                        /* ebp + 0x0c */
    );

ULONG LDRCALL myLDRQAppType(ULONG,ULONG);


/**
 * ldrEnum32bitRelRecs
 * @param  pMTE           Pointer to MTE for this module.
 * @param  iObject        Object index. 0-based!
 * @param  iPageTabl      Page index. 0-based!
 * @param  pvPage         Pointer to page buffer.
 * @param  ulPageAddress  Note! Page is not present.
 * @param  pPTDA
 *
 */
extern ULONG LDRCALL ldrEnum32bitRelRecs( /* retd 0x20 */
    PMTE pMTE,                      /* ebp + 0x08 */
    ULONG iObject,                  /* ebp + 0x0c */
    ULONG iPageTable,               /* ebp + 0x10 */
    PVOID pvPage,                   /* ebp + 0x14 */
    ULONG ulPageAddress,            /* ebp + 0x18 */
    PVOID pvPTDA                    /* ebp + 0x1c */
    );

ULONG LDRCALL myldrEnum32bitRelRecs(PMTE, ULONG, ULONG, PVOID, ULONG, PVOID);



/**
 * Loader local variables from KERNEL.SDF.
 */
typedef struct ldrlv_s /* #memb 12 size 39 (0x027) */
{
    PMTE        lv_pmte;                /* Pointer to mte. (ldrCreateMte/ldrXXX) */
    ULONG       lv_lbufaddr;
    ULONG       lv_sbufaddr;
    ULONG       lv_lrecbufaddr;
    ULONG       lv_srecbufaddr;
    ULONG       lv_new_exe_off;
    USHORT      lv_sfn;                 /* Handle to the module being loaded */
    USHORT      lv_hobmte;
    ULONG       lv_objnum;
    ULONG       lv_csmte;               /* size of the swappable mte heap block. (ldrCreateMte) */
    USHORT      lv_class;               /* Object class CLASS_* defines in OS2Krnl (mteflags1) it seems. */
                                        /* CLASS_PROGRAM    Program class. */
                                        /* CLASS_GLOBAL     Global class. */
                                        /* CLASS_SPECIFIC   Specific class, as against global. */
                                        /* CLASS_ALL (0)    Nonspecific class  all modules. */
                                        /* CLASS_MASK       Class mask. */
    UCHAR       lv_type;                /* Type of executable image expected loaded. */
} ldrlv_t;


/*
 * Values of the lv_type byte. (Qualified guesses.)
 */
#define LVTYPE_EXE      0               /* Executable program. */
#define LVTYPE_DLL      1               /* Dynamic Link Library. */
#define LVTYPE_DD       2               /* Device Driver. */
#define LVTYPE_IFS      3               /* Installable Filesystem. */
#define LVTYPE_VDD      4               /* Virtual Device Driver (for VDMs). */



/**
 * ldrOpenPath
 *  pre 14053.
 * @returns   OS2 return code.
 *            pLdrLv->lv_sfn  is set to filename handle.
 * @param     pachModname   Pointer to modulename. Not zero terminated!
 * @param     cchModname    Modulename length.
 * @param     pLdrLv        Loader local variables? (Struct from KERNEL.SDF)
 * @param     pfl           Pointer to flags which are passed on to ldrOpen.
 * @sketch
 *  if !CLASS_GLOBAL or miniifs then
 *      ldrOpen(pachModName)
 *  else
 *      loop until no more libpath elements
 *          get next libpath element and add it to the modname.
 *          try open the modname
 *          if successfull then break the loop.
 *      endloop
 *  endif
 */
extern ULONG LDRCALL ldrOpenPath_old(   /* retd  0x10 */
    PCHAR       pachFilename,       /* ebp + 0x08 */
    USHORT      cchFilename,        /* ebp + 0x0c */
    ldrlv_t *   plv,                /* ebp + 0x10 */
    PULONG      pful                /* ebp + 0x14 */
    );

ULONG LDRCALL myldrOpenPath_old(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful);


/**
 * ldrOpenPath - ldrOpenPath for build 14053 and above.
 *
 * @returns   OS2 return code.
 *            plv->lv_sfn  is set to filename handle.
 * @param     pachFilename  Pointer to modulename. Not zero terminated!
 * @param     cchFilename   Modulename length.
 * @param     plv           Loader local variables? (Struct from KERNEL.SDF)
 * @param     pful          Pointer to flags which are passed on to ldrOpen.
 * @param     lLibPath      New parameter in build 14053.
 *                          ldrGetMte calls with 1
 *                          ldrOpenNewExe calls with 3
 *                          This is compared to the initial libpath index.
 *                              The libpath index is:
 *                                  BEGINLIBPATH    1
 *                                  LIBPATH         2
 *                                  ENDLIBPATH      3
 *                              The initial libpath index is either 1 or 2.
 *
 * @sketch
 * This is roughly what the original ldrOpenPath does:
 *      Save pTCBCur->TCBFailErr.
 *      if !CLASS_GLOBAL or miniifs then
 *          ldrOpen(pachFilename)
 *      else
 *          if beglibpath != NULL then path = 1 else path = 2
 *          if (lLibPath < path)
 *              return ERROR_FILE_NOT_FOUND; (2)
 *          Allocate buffer.
 *          loop until no more libpath elements
 *              get next libpath element and add it to the modname.
 *              try open the modname
 *              if successfull then break the loop.
 *          endloop
 *          Free buffer.
 *      endif
 *      Restore pTCBCur->TCBFailErr.
 */
extern ULONG LDRCALL ldrOpenPath(  /* retd  0x14 */
    PCHAR       pachFilename,      /* ebp + 0x08 */
    USHORT      cchFilename,       /* ebp + 0x0c */
    ldrlv_t *   plv,               /* ebp + 0x10 */
    PULONG      pful,              /* ebp + 0x14 */
    ULONG       lLibPath           /* ebp + 0x18 */
    );

ULONG LDRCALL myldrOpenPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful, ULONG lLibPath);


/**
 * Finds a module if it's loaded.
 * @returns     NO_ERROR on success.
 *              OS/2 errorcode on error.
 * @param       pachFilename    Pointer to module filename.
 * @param       cchFilename     Length of modulefilename.
 * @param       usClass         Module class. (CLASS_*)
 * @param       ppMTE           Pointer to pMTE found.
 * @sketch
 */
ULONG LDRCALL ldrFindModule(        /* retd  0x10 */
    PCHAR       pachFilename,       /* ebp + 0x08 */
    USHORT      cchFilename,        /* ebp + 0x0c */
    USHORT      usClass,            /* ebp + 0x10 */
    PPMTE       ppMTE               /* ebp + 0x14 */
    );

ULONG LDRCALL myldrFindModule(PCHAR pachFilename, USHORT cchFilename, USHORT usClass, PPMTE ppMTE);


/**
 * Checks if a module was loaded using DosLoadModule.
 * This is called from LDRGetProcAddr and LDRFreeModule.
 * @returns NO_ERROR if the module was LoadModuled or executable.
 *          ERROR_INVALID_HANDLE if not LoadModuled.
 * @param   hmte    MTE handle.
 * @param   pptda   Pointer to the PTDA of the process calling. (current)
 * @param   pcUsage Pointer to usage variable. (output)
 *                  The usage count is returned.
 * @sketch
 */
#ifdef _ptda_h_
ULONG LDRCALL ldrWasLoadModuled(        /* retd 0x0c */
    HMTE        hmte,                   /* ebp + 0x08 */
    PPTDA       pptda,                  /* ebp + 0x0c */
    PULONG      pcUsage);               /* ebp + 0x10 */

ULONG LDRCALL myldrWasLoadModuled(HMTE hmte, PPTDA pptda, PULONG pcUsage);
#endif


/**
 * LDRGetProcAddr gets address and proctype for a entry point to a module.
 * @returns NO_ERROR if the module was LoadModuled.
 *          ERROR_INVALID_HANDLE if not LoadModuled.
 * @param   hmte            Handle of module.
 * @param   ulOrdinal       Procedure ordinal.
 * @param   pszName         Pointer to procedure name.
 *                          NULL is allowed. Ignored if ulOrdinal is not zero.
 * @param   pulAddress      Pointer to address variable. (output)
 * @param   fFlat           TRUE if a flat 0:32 address is to be returned.
 *                          FALSE if a far 16:16 address is to be returned.
 * @param   pulProcType     Pointer to procedure type variable. (output)
 *                          NULL is allowed. (DosQueryProcAddr uses NULL)
 *                          In user space.
 * @sketch
 */
ULONG LDRCALL LDRGetProcAddr(           /* retd 0x14 */
    HMTE        hmte,                   /* ebp + 0x08 */
    ULONG       ulOrdinal,              /* ebp + 0x0c */
    PCSZ        pszName,                /* ebp + 0x10 */
    PULONG      pulAddress,             /* ebp + 0x14 */
    BOOL        fFlat,                  /* ebp + 0x18 */
    PULONG      pulProcType);           /* ebp + 0x1c */

ULONG LDRCALL myLDRGetProcAddr(HMTE hmte, ULONG ulOrdinal, PCSZ pszName, PULONG pulAddress, BOOL fFlat, PULONG pulProcType);



/**
 * LDRClearSem - Clears the loader semaphore.
 * (It does some garbage collection on release.)
 * @returns   NO_ERROR on success.
 *            OS/2 error on failure. (ERROR_INTERRUPT?)
 */
extern ULONG LDRCALL LDRClearSem(void);


/**
 * LDRRequestSem - Requests the loader semaphore..
 * @returns   NO_ERROR if succesfully.
 *            OS2 errorcode on failure. (ERROR_INTERRUPT?)
 */
#define LDRRequestSem()   KSEMRequestMutex(pLdrSem, (ULONG)-1)


/*
 * Pointer to the loader semaphore.
 */
#ifdef _OS2KSEM_h_
extern PKSEMMTX    pLdrSem;
#endif


/**
 * Validates an hMTE and gets the MTE pointer.
 * @returns   Pointer to MTE on success.
 *            NULL on error.
 * @param     hMTE  MTE handle.
 * @remark    If you wan't to this faster:
 *              Use the hMTE as a HOB and get the HOB address (by using VMGetHandleInfo).
 */
extern PMTE LDRCALL ldrValidateMteHandle(HMTE hMTE);


/**
 * Gets the pMTE from a hMTE. No checks.
 * @returns     Pointer to the pMTE for a given hMTE.
 * @param       hMTE    Module handle.
 * @sketch
 */
extern PMTE KRNLCALL ldrASMpMTEFromHandle(HMTE  hMTE);


/**
 * Translates a relative filename to a full qualified filename.
 * @returns NO_ERROR on success.
 *          Errorcode on error.
 * @param   pszFilename     Pointer to nullterminated filename.
 */
extern ULONG LDRCALL ldrTransPath(PSZ pszFilename);


/**
 * Sets the VM flags for an executable object.
 * @returns     void
 * @param       pMTE        Pointer to the module table entry.
 * @param       flObj       LX Object flags.
 * @param       pflFlags1   Pointer to the flFlags1 of VMAllocMem (out).
 * @param       pflFlags2   Pointer to the flFlags2 of VMAllocMem (out).
 */
extern VOID LDRCALL  ldrSetVMflags( /* retd  0x10 */
    PMTE        pMTE,               /* ebp + 0x08 */
    ULONG       flObj,              /* ebp + 0x0c */
    PULONG      pflFlags1,          /* ebp + 0x10 */
    PULONG      pflFlags2           /* ebp + 0x14 */
    );

VOID LDRCALL myldrSetVMflags(PMTE pMTE, ULONG flObj, PULONG pflFlags1, PULONG pflFlags2);


/**
 * Checks if the internal name (first name in the resident nametable) matches
 * the filename.
 * @returns     NO_ERROR on success (the name matched).
 *              ERROR_INVALID_NAME if mismatch.
 * @param       pMTE    Pointer to the MTE of the module to check.<br>
 *                      Assumes! that the filename for this module is present in ldrpFileNameBuf.
 */
extern ULONG LDRCALL    ldrCheckInternalName( /* retd  0x04 */
    PMTE        pMTE                /* ebp + 0x08 */
    );

ULONG LDRCALL myldrCheckInternalName(PMTE pMTE);


/**
 * Parses a filename to find the name and extention.
 * @returns Length of the filename without the extention.
 * @param   pachFilename    Pointer to filename to parse - must have path!
 * @param   ppachName       Pointer to pointer which should hold the name pointer upon successfull return.
 * @param   ppachExt        Pointer to pointer which should hold the extention pointer upon successfull return.
 */
extern ULONG LDRCALL    ldrGetFileName(PSZ pszFilename, PCHAR *ppchName, PCHAR *ppchExt);


/**
 * Parses a filename to find the name and extention.
 * @returns Length of the filename without the extention.
 * @param   pachFilename    Pointer to filename to parse - path not needed.
 * @param   ppachName       Pointer to pointer which should hold the name pointer upon successfull return.
 * @param   ppachExt        Pointer to pointer which should hold the extention pointer upon successfull return.
 */
extern ULONG LDRCALL    ldrGetFileName2(PSZ pszFilename, PCHAR *ppchName, PCHAR *ppchExt);


/**
 * Uppercase a string.
 * @returns void
 * @param   pach    String to uppercase.
 * @param   cch     Length of string. (may include terminator)
 */
extern VOID LDRCALL     ldrUCaseString(PCHAR pch, unsigned cch);


/**
 * Pointer to the loader filename buffer.
 * Upon return from ldrOpen (and ldrOpenPath which calls ldrOpen) this is
 * set to the fully qualified filename of the file last opened (successfully).
 */
extern PSZ *pldrpFileNameBuf;
#define ldrpFileNameBuf (*pldrpFileNameBuf)



#endif /* INCL_16 (16-bit test - function prototypes for 32-bit code only!) */
#endif /* no loader api test */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
