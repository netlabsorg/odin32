/* $Id: kHll.cpp,v 1.10 2000-04-07 02:47:00 bird Exp $
 *
 * kHll - Implementation of the class kHll.
 *        That class is used to create HLL debuginfo.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_TYPES
#define INCL_DOSERRORS
#define FOR_EXEHDR          1           /* exe386.h flag */
#define DWORD               ULONG       /* Used by exe386.h / newexe.h */
#define WORD                USHORT      /* Used by exe386.h / newexe.h */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include <newexe.h>
#include <exe386.h>

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "hll.h"
#include "kList.h"
#include "kHll.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
signed long     fsize(FILE *phFile);




/*******************************************************************************
*                                                                              *
*   kHllBaseEntry                                                              *
*                                                                              *
*   kHllBaseEntry                                                              *
*                                                                              *
*******************************************************************************/


/**
 * Writes a list to disk.
 * @returns   Number of bytes written.
 * @returns   Count of bytes written on success. (includes 0)
 *            -3    Invalid offsets.
 *            -2    Seek error.
 *            -1    Write error.
 * @param     phFile    Output filehandle.
 * @param     pEntry    Pointer to the start of the list which is to be written.
 */
int kHllBaseEntry::writeList(FILE *phFile, kHllBaseEntry *pEntry)
{
    int             cch;
    int             cchWritten = 0;

    /*
     * Loop thru the list and write all the entries to disk.
     */
    while (pEntry != NULL)
    {
        cchWritten += cch = pEntry->write(phFile);
        if (cch < 0)
            return cch;
        if (cch == 0)
            return -1;
        pEntry = (kHllBaseEntry*)pEntry->getNext();
    }

    return cchWritten;
}






/*******************************************************************************
*                                                                              *
*   kHllPubSymEntry                                                            *
*                                                                              *
*   kHllPubSymEntry                                                            *
*                                                                              *
*******************************************************************************/



/**
 * Creates an HLL public symbol entry.
 * @param     pachName  Symbol name.
 * @param     cchName   Length of symbol name.
 * @param     off       Offset into the object.
 * @param     iObject   LX Object index.
 * @param     iType     Type index. (index into type table)
 */
kHllPubSymEntry::kHllPubSymEntry(
        const char *        pachName,
        int                 cchName,
        unsigned long       off,
        unsigned short      iObject,
        unsigned short      iType
        )
{
    pPubSym = (PHLLPUBLICSYM)malloc(cchName + sizeof(HLLPUBLICSYM));
    assert(pPubSym != NULL);

    pPubSym->cchName = cchName;
    pPubSym->achName[0] = '\0';
    strncat((char*)&pPubSym->achName[0], pachName, cchName);
    pPubSym->off = off;
    pPubSym->iObject = iObject;
    pPubSym->iType = iType;
}



/**
 * Destructor.
 */
kHllPubSymEntry::~kHllPubSymEntry()
{
    if (pPubSym != NULL)
        free(pPubSym);
    pPubSym = NULL;
}



/**
 * Write this entry to file.
 * @returns   Count of bytes written (on success).
 *            -3    Invalid offsets.
 *            -2    Seek error.
 *            -1    Write error.
 *            0     No data written. Concidered as an error!
 * @param     phFile    Filehandle.
 */
int kHllPubSymEntry::write(FILE *phFile)
{
    assert(pPubSym != NULL);
    return fwrite(pPubSym,
                  1,
                  offsetof(HLLPUBLICSYM, achName) + pPubSym->cchName,
                  phFile);
}







/*******************************************************************************
*                                                                              *
*   kHllLineNumberChunk                                                        *
*                                                                              *
*   kHllLineNumberChunk                                                        *
*                                                                              *
*******************************************************************************/




    int                 cLines;
    PHLLLINENUMBERENTRY paLines;
    HLLFIRSTENTRY       FirstEntry;

/**
 * Constructor.
 * @param     iSeg      Segment number for these linenumbers.
 * @param     offBase   Base offset for all line number offsets. (defaults to 0)
 */
kHllLineNumberChunk::kHllLineNumberChunk(
    unsigned short int iSeg,
    unsigned long int offBase /*= 0*/
    )
{
    memset(&FirstEntry, 0, sizeof(FirstEntry));
    FirstEntry.hll04.iSeg = iSeg;
    FirstEntry.hll04.u1.offBase = offBase;
    FirstEntry.hll04.uchType = 0;
}



/**
 * Destructor.
 */
kHllLineNumberChunk::~kHllLineNumberChunk()
{
    if (paLines != NULL)
        free(paLines);
    paLines = 0;
    FirstEntry.hll04.cEntries = 0;
}



/**
 * Adds a line information.
 * @returns   Success indicator.
 * @param     iusFile       File index.
 * @param     usLine        Line number
 * @param     off           Offset into object.
 */
BOOL                kHllLineNumberChunk::addLineInfo(
                        unsigned short int  iusFile,
                        unsigned short int  usLine,
                        unsigned long int   off
                        )
{
    /*
     * Allocate more memory?
     */
    if (FirstEntry.hll04.cEntries % 20 == 0)
        {
        void *pv = realloc(paLines, (FirstEntry.hll04.cEntries + 20) * sizeof(paLines[0].hll04));
        assert(pv != NULL);
        if (pv == NULL)
            return FALSE;
        paLines = (PHLLLINENUMBERENTRY)pv;
        }

    /*
     * Add line info entry.
     */
    paLines[FirstEntry.hll04.cEntries].hll04.iusSourceFile  = iusFile;
    paLines[FirstEntry.hll04.cEntries].hll04.off            = off;
    paLines[FirstEntry.hll04.cEntries].hll04.usLine         = usLine;
    FirstEntry.hll04.cEntries++;

    return FALSE;
}


/**
 * Write this entry to file.
 * @returns   Count of bytes written (on success).
 *            -3    Invalid offsets.
 *            -2    Seek error.
 *            -1    Write error.
 *            0     No data written. Concidered as an error!
 * @param     phFile    Filehandle.
 */
int     kHllLineNumberChunk::write(FILE *phFile)
{
    int cb;
    int cbWritten;

    /*
     * First entry
     */
    cb = sizeof(paLines[0])*FirstEntry.hll04.cEntries;
    cbWritten = fwrite(&FirstEntry, 1, cb, phFile);
    if (cbWritten != cb)
        return -1;

    /*
     * Line array.
     */
    cb = sizeof(paLines[0])*FirstEntry.hll04.cEntries;
    cbWritten = fwrite(paLines, 1, cb, phFile);
    if (cbWritten != cb)
        return -1;

    return cbWritten + sizeof(FirstEntry.hll04);
}





/*******************************************************************************
*                                                                              *
*   kHllSrcEntry                                                               *
*                                                                              *
*   kHllSrcEntry                                                               *
*                                                                              *
*******************************************************************************/





/**
 * Constructor.
 */
kHllSrcEntry::kHllSrcEntry()
    :
    cFilenames(0),
    pachFilenames(NULL),
    cbFilenames(0),
    cbFilenamesAllocated(0)
{
}


/**
 * Destructor.
 */
kHllSrcEntry::~kHllSrcEntry()
{
    if (pachFilenames != NULL)
        free(pachFilenames);
    pachFilenames = NULL;
    cFilenames = cbFilenames = cbFilenamesAllocated = 0;
}



/**
 * Add/queries a Linenumber chunk.
 * A linenumber chunk is a collection of linenumber information for a
 * module segement.
 * @returns   Pointer to linenumber chunk which you may add linenumber info to.
 *            NULL on failiure.
 * @param     iSeg      Segment number for these linenumbers.
 * @param     offBase   Base offset for all line number offsets. (defaults to 0)
 */
kHllLineNumberChunk *
                    kHllSrcEntry::addLineNumberChunk(
                        unsigned short int  iSeg,
                        unsigned long int   offBase/* = 0*/
                        )
{
    kHllLineNumberChunk *pChunk;

    /*
     * Try find existing chunk.
     */
    pChunk = Lines.getFirst();
    while (pChunk != NULL && pChunk->getSeg() != iSeg)
        pChunk = (kHllLineNumberChunk*)pChunk->getNext();

    /*
     * If not found, then create a new one and add it to the list.
     */
    if (pChunk != NULL)
    {
        pChunk = new kHllLineNumberChunk(iSeg, offBase);
        assert(pChunk != NULL);
        if (pChunk != NULL)
            Lines.insert(pChunk);
    }

    return pChunk;
}



/**
 * Adds a file for this module.
 * @returns   Filename index used when adding linenumbers.
 * @param     pszFilename  Pointer to filaname string.
 */
unsigned short      kHllSrcEntry::addFile(
                        const char *        pszFilename
                        )
{
    return addFile(pszFilename, strlen(pszFilename));
}



/**
 * Adds a file for this module.
 * @returns   Filename index used when adding linenumbers.
 *            0 on error.
 * @param     pachFilename  Pointer to filaname string (not zero terminated).
 * @param     cchFilename   Length of filename.
 */
unsigned short      kHllSrcEntry::addFile(
                        const char *        pachFilename,
                        int                 cchFilename
                        )
{
    assert(pachFilename != NULL);
    assert(cchFilename < 256);
    if (cchFilename >= 256)
        cchFilename = 255;

    /*
     * Allocate more memory?
     */
    if ((cbFilenames + cchFilename + 1) >= cbFilenamesAllocated)
    {
        void *pv = realloc(pachFilenames, cbFilenamesAllocated + 256);
        assert(pv != NULL);
        if (pv == NULL)
            return 0;
        pachFilenames = (char*)pv;
        cbFilenamesAllocated += 256;
    }


    /*
     * Add filename
     */
    pachFilenames[cbFilenames++] = cchFilename;
    memcpy(&pachFilenames[cbFilenames], pachFilename, cchFilename);
    cbFilenames += cchFilename;

    return ++cFilenames;
}



/**
 * Write this entry to file.
 * @returns   Count of bytes written (on success).
 *            -3    Invalid offsets.
 *            -2    Seek error.
 *            -1    Write error.
 *            0     No data written. Concidered as an error!
 * @param     phFile    Filehandle.
 */
int                 kHllSrcEntry::write(FILE *phFile)
{
    HLLFIRSTENTRY       FirstEntry;
    HLLFILENAMEENTRY    FilenameEntry;
    int                 cb;
    int                 cbWrote;
    int                 cbWritten;

    /*
     *  Filenames - if no filename present we'll add a dummy filename!
     *      First entry for the filenames.
     *      Filename entry header.
     *      Write filename entries.
     */
    FirstEntry.hll04.usLine = 0;
    FirstEntry.hll04.uchType = 3;       /* filename */
    FirstEntry.hll04.uchReserved = 0;
    FirstEntry.hll04.cEntries = max(cFilenames, 1);
    FirstEntry.hll04.iSeg = 0;
    FirstEntry.hll04.u1.cbFileNameTable = cbFilenames > 0 ? cbFilenames : 8;
    cb = sizeof(FirstEntry.hll04);
    cbWritten = cbWrote = fwrite(&FirstEntry, 1, cb, phFile);
    if (cb != cbWrote)
        return -1;

    FilenameEntry.cSourceFiles = max(cFilenames, 1);
    FilenameEntry.offSource = 0;
    FilenameEntry.cSourceRecords = 0;
    cb = offsetof(HLLFILENAMEENTRY, cchName);
    cbWritten += cbWrote = fwrite(&FilenameEntry, 1, cb, phFile);
    if (cbWrote != cb)
        return -1;

    if (cbFilenames > 0)
    {
        cbWritten += cbWrote = fwrite(pachFilenames, 1, cbFilenames, phFile);
        if (cbWrote != cbFilenames)
            return -1;
    }
    else
    {   /* no filename - write dummy empty */
        cbWritten += cbWrote = fwrite("\07dummy.c", 1, 8, phFile);
        if (cbWrote != 8)
            return -1;
    }


    /*
     * Write linenumbers.
     */
    if (Lines.getFirst() != NULL)
    {
        cbWritten += cbWrote = kHllBaseEntry::writeList(phFile, Lines.getFirst());
        if (cbWrote < 0)
            return cbWrote;
    }

    return cbWritten;
}





/*******************************************************************************
*                                                                              *
*   kHllModuleEntry                                                            *
*                                                                              *
*   kHllModuleEntry                                                            *
*                                                                              *
*******************************************************************************/







/**
 * Creates an HLL module entry.
 * @param     pszName       Module name. (NULL is not allowed!)
 * @param     iLib          Library index.
 * @param     cSegInfo      Number of objects in the array.
 * @param     paSegInfo     Pointer to an array of objects.
 */
kHllModuleEntry::kHllModuleEntry(
        const char *        pszName,
        unsigned short      iLib,
        unsigned char       cSegInfo/*= 0 */,
        PHLLSEGINFO         paSegInfo/*= NULL */
        )
: fValidOffsetsAndSizes(FALSE)
{
    int         i;
    int         cchName;
    PHLLSEGINFO pSegInfo;

    /*
     * Debug parameter validations.
     */
    assert(pszName != NULL);
    assert(cSegInfo == 0 || paSegInfo != NULL);

    /*
     * Allocate data storage and fill HLL structure.
     */
    cchName = strlen(pszName);
    pModule = (PHLLMODULE)malloc(sizeof(HLLMODULE) + cchName +
                                 sizeof(HLLSEGINFO) * max((cSegInfo - 1), 3));
    assert(pModule != NULL);
    memset(pModule, 0, sizeof(*pModule));
    pModule->cchName = cchName;
    strcpy((char*)&pModule->achName[0], pszName);
    pModule->chVerMajor = 4;
    pModule->chVerMinor = 0;
    pModule->cSegInfo = cSegInfo;
    pModule->iLib = iLib;
    pModule->usDebugStyle = HLL_MOD_STYLE;
    pModule->overlay = 0;
    pModule->pad = 0;

    /* objects */
    if (cSegInfo > 0)
    {
        pModule->SegInfo0.iObject = paSegInfo->iObject;
        pModule->SegInfo0.cb      = paSegInfo->cb;
        pModule->SegInfo0.off     = paSegInfo->off;

        for (i = 1, pSegInfo = (PHLLSEGINFO)&pModule->achName[cchName]; i < cSegInfo; i++, pSegInfo++)
        {
            pSegInfo->iObject   = paSegInfo[i].iObject;
            pSegInfo->cb        = paSegInfo[i].cb;
            pSegInfo->off       = paSegInfo[i].off;
        }
    }
}


/**
 * Destructor - free storage.
 */
kHllModuleEntry::~kHllModuleEntry()
{
    if (pModule != NULL)
        free(pModule);
    pModule = NULL;
}



/**
 * Adds an object to the module.
 * @returns   Success indicator.
 * @param     iObject   LX Object index.
 * @param     off       Offset into the object to the module data.
 * @param     cb        Size of module data (in the object).
 */
BOOL            kHllModuleEntry::addSegInfo(
                    unsigned short int  iObject,
                    unsigned long       off,
                    unsigned long       cb
                    )
{
    assert(pModule != NULL);

    /*
     * Reallocate? (Note that we'll initially allocated space for 3 objects.)
     */
    if (pModule->cSegInfo >= 3)
    {
        void *pv = realloc(pModule, sizeof(HLLMODULE) + pModule->cchName
                           + (pModule->cSegInfo + 1) * sizeof(HLLSEGINFO));
        assert(pv != NULL);
        if (pv == NULL)
            return FALSE;
        pModule = (PHLLMODULE)pv;
    }


    /*
     * Add module.
     */
    if (pModule->cSegInfo == 0)
    {
        pModule->SegInfo0.cb = cb;
        pModule->SegInfo0.off = off;
        pModule->SegInfo0.iObject = iObject;
    }
    else
    {
        PHLLSEGINFO pSegInfo =  (PHLLSEGINFO)((pModule->cSegInfo - 1) * sizeof(HLLSEGINFO)
                                              + &pModule->achName[pModule->cchName]);
        pSegInfo->cb = cb;
        pSegInfo->off = off;
        pSegInfo->iObject = iObject;
    }
    pModule->cSegInfo++;

    return TRUE;
}



/**
 * Adds a public symbol.
 * @returns   Handle to the symbol. NULL on error.
 * @param     pszName  Symbol name.
 * @param     off      Offset into the LX Object of the symbol.
 * @param     iObject  LX Object index.
 * @param     pvType   Type handle. NULL if not type.
 */
const void *    kHllModuleEntry::addPublicSymbol(
                    const char *        pszName,
                    unsigned long int   off,
                    unsigned short int  iObject,
                    const void *        pvType
                    )
{
    assert(pszName != NULL);
    return addPublicSymbol(pszName, strlen(pszName), off, iObject, pvType);
}



/**
 * Adds a public symbol.
 * @returns   Handle to the symbol. NULL on error.
 * @param     pachName   Symbol name.
 * @param     cchName    Name length.
 * @param     off        Offset into the LX Object of the symbol.
 * @param     iObject    LX Object index.
 * @param     pvType     Type handle. NULL if not type.
 */
const void *    kHllModuleEntry::addPublicSymbol(
                    const char *        pachName,
                    int                 cchName,
                    unsigned long int   off,
                    unsigned short int  iObject,
                    const void *        pvType
                    )
{
    kHllPubSymEntry *   pEntry;

    /* parameter assertion */
    assert(pachName != NULL);

    /*
     * Create a public symbol entry
     * Insert into it's list.
     * Invalidate offsets.
     */
    pEntry = new kHllPubSymEntry(
        pachName,
        cchName,
        off,
        iObject,
        pvType == NULL ? 0 : -1 //FIXME/TODO: Types->getIndex(pvType); check if 0 or -1.
        );

    PublicSymbols.insert(pEntry);

    fValidOffsetsAndSizes = FALSE;

    return pEntry;
}




/**
 * Write this HLL entry to file.
 * @returns   Count of bytes written.
 *            -3    Invalid offsets.
 *            -2    Seek error.
 *            -1    Write error.
 * @param     phFile    Filehandle.
 * @param     off       Current offset into the HLL data.
 *                      This is stored and used when making the directory
 *                      entries for this module.
 */
int         kHllModuleEntry::write(FILE *phFile, unsigned long off)
{
    int             cch;
    int             cchToWrite;
    int             cchWritten = 0;

    /* validate object state */
    assert(pModule != NULL);

    /*
     * Write module HLL data.
     */
    offModule = off;
    cchToWrite = offsetof(HLLMODULE, achName) + pModule->cchName + sizeof(HLLSEGINFO) * max(pModule->cSegInfo-1, 0);
    cch = fwrite(pModule, 1, cchToWrite, phFile);
    if (cch != cchToWrite)
        return -1;
    cchWritten += cch;
    cbModule = cch;
    off += cch;

    /*
     * Write the lists.
     *      Public Symbols
     *      Types
     *      Symbols
     *      Source
     */
    offPublicSymbols = off;
    cbPublicSymbols = cch = kHllBaseEntry::writeList(phFile, PublicSymbols.getFirst());
    if (cch < 0)
        return cch;
    cchWritten += cch;
    off += cch;

    /*
    offTypes = off;
    cbTypes = cch = kHllBaseEntry::writeList(phFile, Types.getFirst());
    if (cch < 0)
        return cch;
    cchWritten += cch;
    off += cch;


    offSymbols = off;
    cbSymbols = cch = kHllBaseEntry::writeList(phFile, Symbols.getFirst());
    if (cch < 0)
        return cch;
    cchWritten += cch;
    off += cch;
    */

    offSource = off;
    cbSource = cch = Source.write(phFile);
    if (cch < 0)
        return cch;
    cchWritten += cch;
    off += cch;

    /*
     * Marks offsets and sizes valid and returns succesfully.
     */
    fValidOffsetsAndSizes = TRUE;
    return cchWritten;
}



/**
 * Writes the directory entries for this module to file.
 * @returns   Count of bytes written on success.
 *            -3    Invalid offsets.
 *            -2    Seek error.
 *            -1    Write error.
 *            0     no data written (this is an error condition!)
 * @param     phFile    Filehandle.
 * @param     iMod      Index of this module.
 */
int         kHllModuleEntry::writeDirEntries(FILE *phFile, unsigned short iMod)
{
    HLLDIRENTRY     hllDirEntry;
    int             cch;
    int             cchWritten = 0;

    /*
     * Check that offsets are valid!
     */
    assert(fValidOffsetsAndSizes);
    if (!fValidOffsetsAndSizes)
        return -3;

    /*
     * Write Directory Entries.
     *      Module.
     *      Public Symbols. (if any)
     *      Types. (if any)
     *      Symbols. (if any)
     *      Source. (if any)
     */
    hllDirEntry.usType  = HLL_DE_MODULES;
    hllDirEntry.cb      = cbModule;
    hllDirEntry.off     = offModule;
    hllDirEntry.iMod    = iMod;
    cch = fwrite(&hllDirEntry, 1, sizeof(hllDirEntry), phFile);
    if (cch != sizeof(hllDirEntry))
        return -1;
    cchWritten += cch;

    if (cbPublicSymbols > 0)
    {
        hllDirEntry.usType  = HLL_DE_PUBLICS;
        hllDirEntry.cb      = cbPublicSymbols;
        hllDirEntry.off     = offPublicSymbols;
        hllDirEntry.iMod    = iMod;
        cch = fwrite(&hllDirEntry, 1, sizeof(hllDirEntry), phFile);
        if (cch != sizeof(hllDirEntry))
            return -1;
        cchWritten += cch;
    }

    /*
    if (cbTypes > 0)
    {
        hllDirEntry.usType  = HLL_DE_TYPES;
        hllDirEntry.cb      = cbTypes;
        hllDirEntry.off     = offTypes;
        hllDirEntry.iMod    = iMod;
        cch = fwrite(&hllDirEntry, 1, sizeof(hllDirEntry), phFile);
        if (cch != sizeof(hllDirEntry))
            return -1;
        cchWritten += cch;
    }

    if (cbSymbols > 0)
    {
        hllDirEntry.usType  = HLL_DE_SYMBOLS;
        hllDirEntry.cb      = cbSymbols;
        hllDirEntry.off     = offSymbols;
        hllDirEntry.iMod    = iMod;
        cch = fwrite(&hllDirEntry, 1, sizeof(hllDirEntry), phFile);
        if (cch != sizeof(hllDirEntry))
            return -1;
        cchWritten += cch;
    }
    */

    if (cbSource > 0)
    {
        hllDirEntry.usType  = HLL_DE_IBMSRC;
        hllDirEntry.cb      = cbSource;
        hllDirEntry.off     = offSource;
        hllDirEntry.iMod    = iMod;
        cch = fwrite(&hllDirEntry, 1, sizeof(hllDirEntry), phFile);
        if (cch != sizeof(hllDirEntry))
            return -1;
        cchWritten += cch;
    }


    return cchWritten;
}












/*******************************************************************************
*                                                                              *
*   kHll                                                                       *
*                                                                              *
*   kHll                                                                       *
*                                                                              *
*******************************************************************************/



/**
 * Writes HLL debuginfo to the given file at the current position.
 * The file should be opened in write mode.
 * @returns   Number of bytes written.
 * @param     phFile    Filehandle to output file. Starts writing at current pos.
 */
int         kHll::write(FILE *phFile)
{
    HLLHDR              hllHdr;
    HLLDIR              hllDir;
    kHllModuleEntry *   pModule;
    int                 cch;            /* Number of bytes written to the file in an operation. */
    int                 cchWritten = 0; /* Number of bytes written to the file. */
    long int            lPosStart;      /* Starting position. */
    long int            lPosDir;        /* Directory position. */
    long int            lPos;           /* A file position. */
    int                 iMod;           /* Module index (passed in to writeDirEntries) */

    /* Get starting position. */
    lPosStart = ftell(phFile);

    /* Make temporary header and write it */
    memcpy(hllHdr.achSignature, "NB04", 4);
    hllHdr.offDirectory = 0;
    cch = fwrite(&hllHdr, 1, sizeof(hllHdr), phFile);
    if (cch != sizeof(hllHdr))
        return -1;
    cchWritten += cch;


    /*
     * Start writing modules
     */
    pModule = (kHllModuleEntry*)Modules.getFirst();
    while (pModule != NULL)
    {
        cch = pModule->write(phFile, cchWritten);
        if (cch <= 0)
            return cch;
        cchWritten += cch;
        pModule = (kHllModuleEntry *)pModule->getNext();
    }


    /*
     * Write libraries.
     */
    //Not implemented yet - TODO/FIXME!


    /*
     * Write directory.
     *      Make and write temporary directory header.
     *      Write directory entries per module.
     *      Write directory entry for libraries.
     *      Remake and rewrite directory header. (correct cEntries)
     */
    lPosDir = ftell(phFile);
    hllDir.cEntries = 0;
    hllDir.cb       = offsetof(HLLDIR, aEntries);
    hllDir.cbEntry  = sizeof(HLLDIRENTRY);
    cch = fwrite(&hllDir, 1, offsetof(HLLDIR, aEntries), phFile);
    if (cch != offsetof(HLLDIR, aEntries))
        return -1;
    cchWritten += cch;

    iMod = 1;
    pModule = (kHllModuleEntry*)Modules.getFirst();
    while (pModule != NULL)
    {
        cch = pModule->writeDirEntries(phFile, iMod);
        if (cch == -1)
            return -1;
        cchWritten += cch;
        pModule = (kHllModuleEntry *)pModule->getNext();
        iMod++;
    }

    //Library - TODO/FIXME

    lPos = ftell(phFile);
    hllDir.cEntries = (lPos - lPosDir - offsetof(HLLDIR, aEntries)) / sizeof(HLLDIRENTRY);
    if (fseek(phFile, lPosDir, SEEK_SET) != 0)
        return -2;
    cch = fwrite(&hllDir, 1, offsetof(HLLDIR, aEntries), phFile);
    if (cch != offsetof(HLLDIR, aEntries))
        return -1;

    /*
     * Rewrite HLL header (with correct directory offset).
     */
    hllHdr.offDirectory = lPosDir - lPosStart;
    if (fseek(phFile, lPosStart, SEEK_SET) != 0)
        return -2;
    cch = fwrite(&hllHdr, 1, sizeof(hllHdr), phFile);
    if (cch != sizeof(hllHdr))
        return -1;

    /*
     * Hacking:
     * Writing an extra HLL header pointing to an non-existing directory
     * staring at the last byte of this header. This is present when linking
     * with ilink...
     */
    if (fseek(phFile, lPosStart + cchWritten, SEEK_SET) != 0)
        return -2;
    hllHdr.offDirectory = cchWritten + sizeof(hllHdr) - 1;
    cch = fwrite(&hllHdr, 1, sizeof(hllHdr), phFile);
    if (cch != sizeof(hllHdr))
        return -1;
    cchWritten += cch;

    return cchWritten;
}



/**
 * Constructor - Creates an empty HLL object.
 */
kHll::kHll()
{
}



/**
 * Destructor.
 */
kHll::~kHll()
{
}



/**
 * Adds a module.
 * @returns  Pointer to the module object added. NULL on error.
 * @param    pszName    Module name
 * @param    pvLib      Library module handle.
 * @param    cSegInfo   Number of objects in the array.
 * @param    paSegInfo  Pointer to an array of objects.
 */
kHllModuleEntry *   kHll::addModule(
                        const char *        pszName,
                        const void *        pvLib,
                        unsigned            cSegInfo,
                        PHLLSEGINFO         paSegInfo)
{
    kHllModuleEntry *   pEntry;
    assert(pszName != NULL);

    pEntry = new kHllModuleEntry(
        pszName,
        pvLib == NULL ? 0 : -1, //FIXME/TODO: Libs->getIndex(pvLib); check if 0 or -1;
        cSegInfo,
        paSegInfo);

    Modules.insert(pEntry);
    return pEntry;
}



/**
 * Adds a module.
 * @returns  Pointer to the module object added. NULL on error.
 * @param    pachName   Module name
 * @param    cchName    Length of modulename
 * @param    pvLib      Library module handle.
 * @param    cSegInfo   Number of objects in the array.
 * @param    paSegInfo  Pointer to an array of objects.
 */
kHllModuleEntry *   kHll::addModule(
                        const char *        pachName,
                        unsigned            cchName,
                        const void *        pvLib,
                        unsigned            cSegInfo,
                        PHLLSEGINFO         paSegInfo)
{
    char szModName[256];
    kHllModuleEntry *   pEntry;
    assert(pachName != NULL && cchName > 0);

    szModName[0] = '\0';
    strncat(szModName, pachName, min(cchName, 255));
    pEntry = new kHllModuleEntry(
        szModName,
        pvLib == NULL ? 0 : -1, //FIXME/TODO: Libs->getIndex(pvLib); check if 0 or -1;
        cSegInfo,
        paSegInfo);

    Modules.insert(pEntry);
    return pEntry;
}



/**
 * Writes the HLL info to a file. (Not LX?)
 * @returns   Success indicator.
 * @param     pszFilename   Name of the output file.
 * @remark    IMPORTANT! This is mostly for debugging!
 *            It completely overwrites the file if it exists!
 */
BOOL            kHll::write(
                     const char *pszFilename
                     )
{
    FILE *      phFile;

    phFile = fopen(pszFilename, "wb");
    if (phFile != NULL)
    {
        int cch = write(phFile);
        if (cch > 0)
        {
            fclose(phFile);
            return TRUE;
        }
        else
            fprintf(stderr, "write failed with cch=%d\n", cch);
        fclose(phFile);
    }

    return FALSE;
}



/**
 * Writes the HLL info to a file. (Not LX?)
 * Failes if there is debuginfo in the file.
 * No backup is made. (sorry)
 * @returns   OS2 return code.
 * @param     pszFilename   Name of the output file.
 */
APIRET          kHll::writeToLX(
                    const char *pszFilename
                    )
{
    APIRET  rc;
    FILE *  phFile;

    phFile = fopen(pszFilename, "rb+");
    if (phFile != NULL)
    {
        struct exe_hdr      ehdr;
        struct e32_exe      e32;
        int                 cch;
        long                lPosLXHdr;
        long                cbLXFile;

        /*
         * Read exe header
         */
        cch = fread(&ehdr, 1, sizeof(ehdr), phFile);
        if (cch == sizeof(ehdr))
        {
            cbLXFile = fsize(phFile);
            if (ehdr.e_magic == EMAGIC)
                lPosLXHdr = ehdr.e_lfanew;
            else
                lPosLXHdr = 0;
            if (fseek(phFile, lPosLXHdr, SEEK_SET) == 0)
            {
                cch = fread(&e32, 1, sizeof(e32), phFile);
                if (cch == sizeof(e32))
                {
                    if (*(unsigned short*)&e32.e32_magic[0] == E32MAGIC)
                    {
                        /*
                         * Found exeheader.
                         * Check if there is any debug info.
                         */
                        if ((e32.e32_debuginfo == 0 && e32.e32_debuginfo == 0)
                            || (cbLXFile == e32.e32_debuglen + e32.e32_debuginfo)
                            )
                        {
                            long lPosDebug;

                            if (e32.e32_debuginfo != 0 && e32.e32_debuglen != 0)
                                lPosDebug = e32.e32_debuginfo;
                            else
                                lPosDebug = cbLXFile;

                            /*
                             * Go to debug info position in the file and write debug info.
                             */
                            if (fseek(phFile, lPosDebug, SEEK_SET) == 0)
                            {
                                /*
                                 * Write the HLL data to disk.
                                 */
                                cch = write(phFile);
                                if (cch > 0)
                                {
                                    /*
                                     * Update exeheader.
                                     */
                                    e32.e32_debuglen = cch;
                                    e32.e32_debuginfo = lPosDebug;
                                    if (fseek(phFile, lPosLXHdr, SEEK_SET) == 0)
                                    {
                                        /*
                                         * Write the updated header to disk.
                                         */
                                        cch = fwrite(&e32, 1, sizeof(e32), phFile);
                                        if (cch == sizeof(e32))
                                            rc = NO_ERROR;
                                        else
                                            rc = ERROR_WRITE_FAULT;
                                    }
                                    else
                                        rc = ERROR_SEEK;
                                }
                                else
                                {
                                    fprintf(stderr, "error - write failed with cch=%d\n", cch);
                                    rc = ERROR_WRITE_FAULT;
                                }
                            }
                            else
                                rc = ERROR_SEEK;
                        }
                        else
                        {
                            fprintf(stderr, "error - debuginfo exists\n");
                            rc = ERROR_BAD_EXE_FORMAT;
                        }

                    }
                    else
                        rc = ERROR_INVALID_EXE_SIGNATURE;
                }
                else
                    rc = ERROR_BAD_EXE_FORMAT;
            }
            else
                rc = ERROR_BAD_EXE_FORMAT;
        }
        else
            rc = ERROR_READ_FAULT;

        fclose(phFile);
    }
    else
        rc = ERROR_ACCESS_DENIED; //?


    return rc;
}


/**
 * Find the size of a file.
 * @returns   Size of file. -1 on error.
 * @param     phFile  File handle.
 */
signed long fsize(FILE *phFile)
{
    int ipos;
    signed long cb;

    if ((ipos = ftell(phFile)) < 0
        ||
        fseek(phFile, 0, SEEK_END) != 0
        ||
        (cb = ftell(phFile)) < 0
        ||
        fseek(phFile, ipos, SEEK_SET) != 0
        )
        cb = -1;
    return cb;
}

