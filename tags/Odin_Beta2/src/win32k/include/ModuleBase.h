/* $Id: ModuleBase.h,v 1.6 2001-02-11 14:59:20 bird Exp $
 *
 * ModuleBase - Declaration of the Basic module class.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _modulebase_h_
#define _modulebase_h_


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * Error definitions (used in addition to them in bseerr.h)
 */
#define ERROR_FAILED_TO_ADD_OBJECT          0x42000000UL
#define ERROR_INITMETHOD_NOT_INITTIME       0x42000001UL
#define ERROR_INTERNAL_PROCESSING_ERROR     0x42000002UL


/*
 * Some useful macros.
 */
#define NOREF(a) (a=a)                      /* Not referenced parameter warning fix. */
#define ALIGN(a, alignment) (((a) + (alignment - 1UL)) & ~(alignment - 1UL))
                                            /* aligns something, a,  up to nearest alignment boundrary-
                                             * Note: Aligment must be a 2**n number. */

#ifndef PAGESHIFT
#define PAGESHIFT                   12      /* bytes to pages or pages to bytes shift value. */
#endif
#ifndef PAGESIZE
#define PAGESIZE                    0x1000  /* pagesize on i386 */
#endif
#ifndef PAGEOFFSET
#define PAGEOFFSET(addr) ((addr) &  (PAGESIZE-1)) /* Gets the offset into the page addr points into. */
#endif
#ifndef PAGESTART
#define PAGESTART(addr)  ((addr) & ~(PAGESIZE-1)) /* Gets the address of the page addr points into. */
#endif


/*
 * Output macros.
 * Macros:          option   infolevel
 *      printIPE    -W1      Error
 *      printErr    -W1      Error
 *      printWar    -W2      Warning
 *      printInf    -W3      Info
 *      printInfA   -W4      InfoAll
 */
#define printIPE(a) (ModuleBase::ulInfoLevel >= ModuleBase::Error ? \
                     ModuleBase::printf("\nerror(%d:"__FUNCTION__"): !Internal Processing Error!\n\t", __LINE__), \
                     ModuleBase::printf a,  \
                     ModuleBase::printf("\n")  \
                     : (void)0,(void)0,(void)0 )
#define printErr(a) (ModuleBase::ulInfoLevel >= ModuleBase::Error ? \
                     ModuleBase::printf("error(%d:"__FUNCTION__"): ", __LINE__), \
                     ModuleBase::printf a  \
                     : (void)0,(void)0 )
#define printWar(a) (ModuleBase::ulInfoLevel >= ModuleBase::Warning ? \
                     ModuleBase::printf("warning("__FUNCTION__"): "), \
                     ModuleBase::printf a  \
                     : (void)0,(void)0 )
#define printInf(a) (ModuleBase::ulInfoLevel >= ModuleBase::Info ? \
                     ModuleBase::printf a : (void)0 )
#define printInfA(a)(ModuleBase::ulInfoLevel >= ModuleBase::InfoAll ? \
                     ModuleBase::printf a : (void)0 )


/*
 * Read macros.
 *  ReadAt:  Reads from a file, hFile, at a given offset, ulOffset, into a buffer, pvBuffer,
 *           an amount of bytes, cbToRead.
 *           RING0: Map this to ldrRead with 0UL as flFlags.
 *           RING3: Implementes this function as a static function, ReadAt.
 *  ReadAtF: Same as ReadAt but two extra parameters; an additional far pointer to the buffer and
 *           a MTE pointer. Used in the read method.
 *           RING0: Map directly to ldrRead.
 *           RING3: Map to ReadAt, ignoring the two extra parameters.
 */
#ifdef RING0
    #define ReadAt(hFile, ulOffset, pvBuffer, cbToRead) \
        ldrRead(hFile, ulOffset, pvBuffer, 0UL, cbToRead, NULL)
    #define ReadAtF(hFile, ulOffset, pvBuffer, fpBuffer, cbToRead, pMTE) \
        ldrRead(hFile, ulOffset, pvBuffer, fpBuffer, cbToRead, pMTE)
#else
    #define ReadAtF(hFile, ulOffset, pvBuffer, fpBuffer, cbToRead, pMTE) \
        ReadAt(hFile, ulOffset, pvBuffer, cbToRead)
#endif


/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
#ifndef RING0
APIRET ReadAt(SFN hFile, ULONG ulOffset, PVOID pvBuffer, ULONG cbToRead);
#endif

/*
 * Make sure that pLdrLv is defined.
 */
#if !defined(RING0) && !defined(_ldrCalls_h_)
typedef struct ldrlv_s  ldrlv_t;
#endif

/**
 * Base class for executable modules.
 * @author      knut st. osmundsen
 * @approval    not approved yet...
 */
class ModuleBase
{
public:
    /** @cat Constructor/Destructor */
    ModuleBase(SFN hFile);
    virtual ~ModuleBase();

    /** @cat Public Main methods */
    virtual ULONG   init(PCSZ pszFilename);
    virtual ULONG   read(ULONG offLXFile, PVOID pvBuffer, ULONG fpBuffer, ULONG cbToRead, PMTE pMTE) = 0;
    virtual ULONG   applyFixups(PMTE pMTE, ULONG iObject, ULONG iPageTable, PVOID pvPage,
                                ULONG ulPageAddress, PVOID pvPTDA); /*(ldrEnum32bitRelRecs)*/
    virtual ULONG   openPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *pLdrLv, PULONG pful, ULONG lLibPath); /* (ldrOpenPath) */
    #ifndef RING0
    virtual ULONG   writeFile(PCSZ pszLXFilename);
    #endif

    /** @cat public Helper methods */
    virtual VOID    dumpVirtualLxFile() = 0;
    BOOL            queryIsModuleName(PCSZ pszFilename);
    PCSZ            getFilename();
    PCSZ            getModuleName();

    /** @cat static print method */
    static VOID     printf(PCSZ pszFormat, ...);

protected:
    /** @cat private data members. */
    #ifdef DEBUG
    BOOL        fInitTime;              /* init time indicator (debug) */
    #endif
    SFN         hFile;                  /* filehandle */
    PSZ         pszFilename;            /* fullpath */
    PSZ         pszModuleName;          /* filename without path and extention. */

    /** @cat public static data. */
public:
    static ULONG ulInfoLevel;                    /* Current output message detail level. */
    enum {Quiet, Error, Warning, Info, InfoAll}; /* Output message detail levels. */
};


#endif


