/* $Id: kHll.cpp,v 1.3 2000-03-26 14:16:18 bird Exp $
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
 * @param     pszName   Symbol name.
 * @param     off       Offset into the object.
 * @param     iObj      LX Object index.
 * @param     iType     Type index. (index into type table)
 */
kHllPubSymEntry::kHllPubSymEntry(
        const char *        pszName,
        unsigned long       off,
        unsigned short      iObj,
        unsigned short      iType
        )
{
    pPubSym = (PHLLPUBLICSYM)malloc(strlen(pszName) + sizeof(HLLPUBLICSYM));
    assert(pPubSym != NULL);

    pPubSym->cchName = strlen(pszName);
    strcpy((char*)&pPubSym->achName[0], pszName);
    pPubSym->off = off;
    pPubSym->iObj = iObj;
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
 * @returns   Number of bytes written.
 * @param     phFile    File handle.
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
*   kHllModuleEntry                                                            *
*                                                                              *
*   kHllModuleEntry                                                            *
*                                                                              *
*******************************************************************************/







/**
 * Creates an HLL module entry.
 * @param     pszName       Module name. (NULL is not allowed!)
 * @param     iLib          Library index.
 * @param     cObjects      Number of objects in the array.
 * @param     paObjects     Pointer to an array of objects.
 */
kHllModuleEntry::kHllModuleEntry(
        const char *        pszName,
        unsigned short      iLib,
        unsigned char       cObjects/*= 0 */,
        PMODOBJECT          paObjects/*= NULL */
        )
: fValidOffsetsAndSizes(FALSE)
{
    int         i;
    int         cchName;
    PHLLOBJECT  pObj;

    /*
     * Debug parameter validations.
     */
    assert(pszName != NULL);
    assert(cObjects == 0 || paObjects != NULL);

    /*
     * Allocate data storage and fill HLL structure.
     */
    cchName = strlen(pszName);
    pModule = (PHLLMODULE)malloc(sizeof(HLLMODULE) + cchName +
                                 sizeof(HLLOBJECT) * min((cObjects - 1), 3));
    assert(pModule != NULL);
    memset(pModule, 0, sizeof(*pModule));
    pModule->cchName = cchName;
    strcpy((char*)&pModule->achName[0], pszName);
    pModule->chVerMajor = 4;
    pModule->chVerMinor = 0;
    pModule->cObjects = cObjects;
    pModule->iLib = iLib;
    pModule->usDebugStyle = HLL_MOD_STYLE;
    pModule->overlay = 0;
    pModule->pad = 0;

    /* objects */
    if (cObjects > 0)
    {
        pModule->Object.cb   = paObjects->cb;
        pModule->Object.iObj = paObjects->iObject;
        pModule->Object.off  = paObjects->offset;

        for (i = 1, pObj = (PHLLOBJECT)&pModule->achName[cchName]; i < cObjects; i++, pObj++)
        {
            pObj->cb   = paObjects[i].cb;
            pObj->iObj = paObjects[i].iObject;
            pObj->off  = paObjects[i].offset;
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
BOOL            kHllModuleEntry::addObject(
                    unsigned short int  iObject,
                    unsigned long       off,
                    unsigned long       cb
                    )
{
    assert(pModule != NULL);

    /*
     * Reallocate? (Note that we'll initially allocated space for 3 objects.)
     */
    if (pModule->cObjects >= 3)
    {
        void *pv = realloc(pModule, sizeof(HLLMODULE) + pModule->cchName
                           + (pModule->cObjects + 1) * sizeof(HLLOBJECT));
        assert(pv != NULL);
        if (pv == NULL)
            return FALSE;
        pModule = (PHLLMODULE)pv;
    }


    /*
     * Add module.
     */
    if (pModule->cObjects == 0)
    {
        pModule->Object.cb = cb;
        pModule->Object.off = off;
        pModule->Object.iObj = iObject;
    }
    else
    {
        PHLLOBJECT pObject =  (PHLLOBJECT)(pModule->cObjects * sizeof(HLLOBJECT)
                                           + pModule->achName[pModule->cchName]);
        pObject->cb = cb;
        pObject->off = off;
        pObject->iObj = iObject;
    }
    pModule->cObjects++;

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
    kHllPubSymEntry *   pEntry;

    /* parameter assertion */
    assert(pszName != NULL);

    /*
     * Create a public symbol entry
     * Insert into it's list.
     * Invalidate offsets.
     */
    pEntry = new kHllPubSymEntry(
        pszName,
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
 * @returns   Count of bytes written. -1 on error.
 * @param     phFile    Filehandle.
 * @param     off       Current offset into the HLL data.
 *                      This is stored and used when making the directory
 *                      entries for this module.
 */
int         kHllModuleEntry::write(FILE *phFile, unsigned long off)
{
    int             cch;
    int             cchWritten = 0;

    /* validate object state */
    assert(pModule != NULL);

    /*
     * Write module HLL data.
     */
    offModule = off;
    cch = fwrite(pModule, 1, offsetof(HLLMODULE, achName) + pModule->cchName, phFile);
    if (cch != offsetof(HLLMODULE, achName) + pModule->cchName)
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

    offSource = off;
    cbSource = cch = kHllBaseEntry::writeList(phFile, Source.getFirst());
    if (cch < 0)
        return cch;
    cchWritten += cch;
    off += cch;
    */

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

    */

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
    hllHdr.ulReserved = 1;
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
    hllDir.ulReserved = 0; //FIXME/TODO - not quite sure what this is or should be!
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
    cch = fwrite(&hllHdr, 1, sizeof(hllHdr), phFile);
    if (cch != sizeof(hllHdr))
        return -1;


    return cch;
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
 * @param    cObjects   Number of objects in the array.
 * @param    paObjects  Pointer to an array of objects.
 */
kHllModuleEntry *   kHll::addModule(
                        const char *        pszName,
                        const void *        pvLib,
                        unsigned            cObjects,
                        PMODOBJECT          paObjects)
{
    kHllModuleEntry *   pEntry;
    assert(pszName != NULL);

    pEntry = new kHllModuleEntry(
        pszName,
        pvLib == NULL ? 0 : -1, //FIXME/TODO: Libs->getIndex(pvLib); check if 0 or -1;
        cObjects,
        paObjects);

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

        /*
         * Read exe header
         */
        cch = fread(&ehdr, 1, sizeof(ehdr), phFile);
        if (cch == sizeof(ehdr))
        {
            if (ehdr.e_magic == NEMAGIC)
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
                        if (e32.e32_debuginfo > 0 && e32.e32_debuginfo > 0)
                        {
                            long lPosDebug;

                            /*
                             * Go to end of file and write debug info.
                             */
                            if (fseek(phFile, 0, SEEK_END) == 0
                                &&
                                (lPosDebug = ftell(phFile)) != -1
                                )
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



