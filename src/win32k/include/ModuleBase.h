/* $Id: ModuleBase.h,v 1.2 2000-01-22 18:20:58 bird Exp $
 *
 * ModuleBase - Declaration of the Basic module class.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _MODULEBASE_H_
#define _MODULEBASE_H_


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
 * Misc
 */
#define PAGESIZE                    0x1000




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
    virtual ULONG  init(PCSZ pszFilename);
    virtual ULONG  read(ULONG offLXFile, PVOID pvBuffer, ULONG cbToRead, ULONG flFlags, PMTE pMTE) = 0;
    virtual ULONG  applyFixups(PMTE pMTE, ULONG iObject, ULONG iPageTable, PVOID pvPage,
                               ULONG ulPageAddress, PVOID pvPTDA); /*(ldrEnum32bitRelRecs)*/
    #ifndef RING0
    virtual ULONG  writeFile(PCSZ pszLXFilename);
    #endif

    /** @cat public Helper methods */
    virtual VOID   dumpVirtualLxFile() = 0;
    BOOL           queryIsModuleName(PCSZ pszFilename);

    /** @cat static print method */
    static VOID     printf(PCSZ pszFormat, ...);

protected:
    /** @cat private data members. */
    #ifdef DEBUG
    BOOL        fInitTime;              /* init time indicator (debug) */
    #endif
    SFN         hFile;                  /* filehandle */
    PSZ         pszFilename;            /* fullpath */
    PSZ         pszModuleName;          /* filename without extention. */

    /** @cat public static data. */
public:
    static ULONG ulInfoLevel;                    /* Current output message detail level. */
    enum {Quiet, Error, Warning, Info, InfoAll}; /* Output message detail levels. */
};


#endif


