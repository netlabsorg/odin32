/* $Id: LdrCalls.h,v 1.3.4.7 2000-08-25 04:47:23 bird Exp $
 *
 * Prototypes for the loader overrided function.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _ldrCalls_h_
#define _ldrCalls_h_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define LDRCALL __stdcall


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
extern ULONG LDRCALL ldrOpenPath(   /* retd  0x10 */
    PCHAR       pachFilename,       /* ebp + 0x08 */
    USHORT      cchFilename,        /* ebp + 0x0c */
    ldrlv_t *   plv,                /* ebp + 0x10 */
    PULONG      pful                /* ebp + 0x14 */
    );

ULONG LDRCALL myldrOpenPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful);


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
ULONG LDRCALL ldrFindModule(PCHAR pachFilename, USHORT cchFilename, USHORT usClass, PPMTE ppMTE);


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
 * Pointer to the loader filename buffer.
 * Upon return from ldrOpen (and ldrOpenPath which calls ldrOpen) this is
 * set to the fully qualified filename of the file last opened (successfully).
 */
extern PSZ *pldrpFileNameBuf;
#define ldrpFileNameBuf (*pldrpFileNameBuf)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
