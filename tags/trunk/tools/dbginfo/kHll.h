/* $Id: kHll.h,v 1.8 2000-05-27 02:15:42 bird Exp $
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

    /**
     * Dumps the HLL entry to ph in a human readable fashion.
     * @param       ph          Output file handle the dump is to be written to.
     * @param       cchIndent   Number of char to indents the output dump.
     */
    virtual void    dump(FILE *ph, int cchIndent) = 0;
    static  void    dumpList(FILE *ph, int cchIndent, kHllBaseEntry *pEntry);
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
        const char *        pachName,
        int                 cchName,
        unsigned long       off,
        unsigned short      iObject,
        unsigned short      iType
        );
    ~kHllPubSymEntry();

    int             write(FILE *phFile);
    void            dump(FILE *ph, int cchIndent);
};



/**
 * Linenumber chunk.
 * @author      knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
class kHllLineNumberChunk : public kHllBaseEntry
{
private:
    PHLLLINENUMBERENTRY paLines;
    HLLFIRSTENTRY       FirstEntry;

public:
    kHllLineNumberChunk(
        unsigned short int  iSeg,
        unsigned long int   offBase = 0
        );
    ~kHllLineNumberChunk();

    BOOL            addLineInfo(
                        unsigned short int  iusFile,
                        unsigned short int  usLine,
                        unsigned long int   off
                        );

    int             write(FILE *phFile);
    void            dump(FILE *ph, int cchIndent);

    int             getSeg()    { return FirstEntry.hll04.iSeg; }
};


/**
 * HLL Source entry.
 * @author      knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
class kHllSrcEntry
{
private:
    unsigned long int   cFilenames;
    char *              pachFilenames;
    unsigned long int   cbFilenames;
    unsigned long int   cbFilenamesAllocated;

    kList<kHllLineNumberChunk>
                        Lines;

public:
    kHllSrcEntry();
    ~kHllSrcEntry();

    kHllLineNumberChunk *
                    addLineNumberChunk(
                        unsigned short int  iSeg,
                        unsigned long int   offBase = 0
                        );
    unsigned short  addFile(
                        const char *        pszFilename
                        );
    unsigned short  addFile(
                        const char *        pachFilename,
                        int                 cchFilename
                        );
    int             write(FILE *phFile);
    void            dump(FILE *ph, int cchIndent);
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
    */
    kHllSrcEntry                Source;

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
        unsigned char       cSegInfo = 0,
        PHLLSEGINFO         paSegInfo = NULL
        );
    ~kHllModuleEntry();


    /** @cat
     * Add menthods
     */
    BOOL            addSegInfo(
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
    const void *    addPublicSymbol(
                        const char *        pachName,
                        int                 cchName,
                        unsigned long int   off,
                        unsigned short int  iObject,
                        const void *        pvType
                        );
    kHllSrcEntry *  getSourceEntry()    { return &Source; }


    /** @cat
     * Output.
     */
    int             write(FILE *phFile, unsigned long off);
    int             writeDirEntries(FILE *phFile, unsigned short iMod);


    /** @cat
     * Debug dump function.
     */
    void            dump(FILE *ph);
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
    kHll(kFile *pFile) throw (int);
    ~kHll();

    /** @cat
     * Add menthods
     */
    kHllModuleEntry *   addModule(
                            const char *        pszName,
                            const void *        pvLib,
                            unsigned            cSegInfo = 0,
                            PHLLSEGINFO         paSegInfo = NULL
                            );
    kHllModuleEntry *   addModule(
                            const char *        pachName,
                            unsigned            cchName,
                            const void *        pvLib,
                            unsigned            cSegInfo = 0,
                            PHLLSEGINFO         paSegInfo = NULL
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

    /** @cat
     * Static read function(s).
     */
    static kHll *       readLX(
                            const char *pszFilename
                            );

    /** @cat
     * Debug dump function.
     */
    void                dump();
};



#endif

