/* $Id: kHll.h,v 1.3 2000-03-26 14:16:19 bird Exp $
 *
 * kHll - Declarations of the class kHll.
 *        That class is used to create HLL debuginfo.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _kHll_h_
#define _kHll_h_


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/**
 * Module object(=segment) description.
 */
typedef struct
{
    unsigned short  iObject;            /* LX Object index. */
    unsigned long   cb;                 /* Size of object part. */
    unsigned long   offset;             /* Offset into the LX Object. */
} MODOBJECT, *PMODOBJECT;



/**
 * HLL General entry.
 * Provided as a base class for kList entries.
 * @author      knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
class kHllBaseEntry : public kListEntry
{
public:
    /**
     * Write this HLL entry to file.
     * @returns   Count of bytes written (on success).
     *            -3    Invalid offsets.
     *            -2    Seek error.
     *            -1    Write error.
     *            0     No data written. Concidered as an error!
     * @param     phFile    Filehandle.
     */
    virtual int     write(FILE *phFile) = 0;
    static int      writeList(FILE *phFile, kHllBaseEntry *pEntry);
};



/**
 * HLL Public symbol entry.
 * @author      knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
class kHllPubSymEntry : public kHllBaseEntry
{
private:
    PHLLPUBLICSYM       pPubSym;

public:
    kHllPubSymEntry(
        const char *        pszName,
        unsigned long       off,
        unsigned short      iObj,
        unsigned short      iType
        );
    ~kHllPubSymEntry();

    int             write(FILE *phFile);
};



/**
 * HLL Module entry.
 * @author      knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
class kHllModuleEntry : public kListEntry
{
private:
    /** @cat
     * Internal data.
     *      HLL Module data.
     *      Lists of HLL elements
     *      Offsets and sizes - set by write(..).
     */
    PHLLMODULE                  pModule;

    kList<kHllPubSymEntry>      PublicSymbols;
    /*
    kList<kHllTypeEntry>        Types;
    kList<kHllSymEntry>         Symbols;
    kList<kHllSrcEntry>         Source;
    */

    BOOL                        fValidOffsetsAndSizes;
    unsigned long               offModule;
    unsigned long               cbModule;
    unsigned long               offPublicSymbols;
    unsigned long               cbPublicSymbols;
    unsigned long               offTypes;
    unsigned long               cbTypes;
    unsigned long               offSymbols;
    unsigned long               cbSymbols;
    unsigned long               offSource;
    unsigned long               cbSource;


    /** @cat
     * Internal methods.
     */
    int             writeList(FILE *phFile, kList<kHllBaseEntry> &List);


public:
    /** @cat
     * Constructor and destructor.
     */
    kHllModuleEntry(
        const char *        pszName,
        unsigned short      iLib,
        unsigned char       cObjects = 0,
        PMODOBJECT          paObjects = NULL
        );
    ~kHllModuleEntry();


    /** @cat
     * Add menthods
     */
    BOOL            addObject(
                        unsigned short int  iObject,
                        unsigned long       off,
                        unsigned long       cb
                        );

    const void *    addPublicSymbol(
                        const char *        pszName,
                        unsigned long int   off,
                        unsigned short int  iObject,
                        const void *        pvType
                        );


    /** @cat
     * Output.
     */
    int         write(FILE *phFile, unsigned long off);
    int         writeDirEntries(FILE *phFile, unsigned short iMod);
};



/**
 * HLL Debug Info generator.
 * @author      knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
class kHll
{

private:
    /** @cat
     * Internal data.
     */
    kList<kHllModuleEntry>     Modules;
    /*
    kList<kHllLibraryEntry>    Libraries;
    */

    /** @cat
     * Internal methods.
     */
    int             write(FILE *phFile);


public:
    /** @cat
     * Constructors and Destructors.
     */
    kHll();
    ~kHll();

    /** @cat
     * Add menthods
     */
    kHllModuleEntry *   addModule(
                            const char *        pszName,
                            const void *        pvLib,
                            unsigned            cObjects,
                            PMODOBJECT          paObjects
                            );


    /** @cat
     * Output.
     */
    BOOL                write(
                            const char *pszFilename
                            );
    APIRET              writeToLX(
                            const char *pszFilename
                            );


};



#endif

