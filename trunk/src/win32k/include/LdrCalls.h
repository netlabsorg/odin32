/* $Id: LdrCalls.h,v 1.7 2000-12-11 06:45:03 bird Exp $
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

/** Additional ldrFindModule flag. Or together with the class. */
#define SEARCH_FULL_NAME    0x0001


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


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

