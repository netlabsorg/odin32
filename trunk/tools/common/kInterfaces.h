/* $Id: kInterfaces.h,v 1.2 2001-04-17 00:26:12 bird Exp $
 *
 * This headerfile contains interfaces for the common tools classes.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _kInterfaces_h_
#define _kInterfaces_h_


class kPageI;
class kExportI;
class kExportEntry;
class kModuleI;


/**
 * Interface class (ie. virtual) which defines the interface for
 * executables (resources and object too perhaps) files used to
 * get and put pages.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kPageI
{
public:
    /** @cat Get and put page methods. */

    /**
     * Get a code, data or resource page from the file.
     * @returns Success indicator.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      will receive the page data on the specified address.
     * @param   ulAddress   Page address. This will be page aligned.
     */
    virtual BOOL    pageGet(char *pachPage, ULONG ulAddress) = 0;

    /**
     * Get a code, data or resource page from the file.
     * @returns Success indicator.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      will receive the page data on the specified address.
     * @param   iObject     Object number.  (0-based)
     * @param   offObject   Offset into the object.
     * @remark  Object = Section.
     */
    virtual BOOL    pageGet(char *pachPage, int iObject, int offObject) = 0;

    /**
     * Updates or adds a code, data, or resource page to the file.
     * @returns Success indicator.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      holds the page data.
     * @param   ulAddress   Page address. This will be page aligned.
     */
    virtual BOOL    pagePut(const char *pachPage, ULONG ulAddress) = 0;

    /**
     * Updates or adds a code, data, or resource page to the file.
     * @returns Success indicator.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      holds the page data.
     * @param   iObject     Object number. (0-based)
     * @param   offObject   Offset into the object.
     */
    virtual BOOL    pagePut(const char *pachPage, int iObject, int offObject) = 0;

    /**
     * Get pagesize for the file.
     * @returns Pagesize in bytes.
     */
    virtual int     pageGetPageSize() = 0;
};


/**
 * ExportEntry used by the findFirstExport/findNextExport functions
 */
class kExportEntry
{
#define MAXEXPORTNAME 256
public:
    unsigned long   ulOrdinal;                  /* Ordinal of export. 0 if invalid. */
    char            achName[MAXEXPORTNAME];     /* Public or exported name. */
    char            achIntName[MAXEXPORTNAME];  /* Optional. not used by PEFile */

    unsigned long   ulOffset;                   /* Offset. -1 if invalid. */
    unsigned long   iObject;                    /* Object number. -1 if invalid. */
    unsigned long   ulAddress;                  /* Address of symbol. -1 if invalid. */

public:
    /** @cat Internal use - don't mess! */
    void *          pv;                          /* Internal pointer. */
};

/**
 * Interface class (ie. virtual) which defines the interface for executables
 * (objects and libraries to perhaps) files used to enumerate exports and
 * public exported names.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kExportI
{
public:
    /** @cat Export enumeration methods. */

    /**
     * Find the first export/public name.
     * @returns Success indicator.
     * @param   pExport     Communication area.
     */
    virtual BOOL    exportFindFirst(kExportEntry * pExport) = 0;

    /**
     * Find the next export/public name.
     * @returns Success indicator.
     *          FALSE when no more exports (exportFindClose has been processed then).
     * @param   pExport     Communication area which has been successfully
     *                      processed by findFirstExport.
     */
    virtual BOOL    exportFindNext(kExportEntry * pExport) = 0;

    /**
     * Frees resources associated with the communicatin area.
     * It's not necessary to call this when exportFindNext has return FALSE.
     * @param   pExport     Communication area which has been successfully
     *                      processed by findFirstExport.
     */
    virtual void    exportFindClose(kExportEntry * pExport) = 0;


    /** @cat Export Search methods */

    /**
     * Lookup information on a spesific export given by ordinal number.
     * @returns Success indicator.
     * @param   pExport     Communication area containing export information
     *                      on successful return.
     */
    virtual BOOL    exportLookup(unsigned long ulOrdinal, kExportEntry *pExport) = 0;

    /**
     * Lookup information on a spesific export given by name.
     * @returns Success indicator.
     * @param   pExport     Communication area containing export information
     *                      on successful return.
     */
    virtual BOOL    exportLookup(const char *  pszName, kExportEntry *pExport) = 0;
};


/**
 * Interface class (ie. virtual) which defines the interface for executables
 * (objects and libraries to perhaps) files used to enumerate exports and
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kModuleI
{
public:
    /** @cat Module information methods. */
    virtual BOOL    moduleGetName(char *pszBuffer, int cbBuffer = 260) = 0;
};


/**
 * Interface class (ie. virtual) which defines the interface for
 * executables files.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kExecutableI : public kModuleI, public kExportI
{
public:
    /** @cat Executable information methods. */
    #if 0
    virtual BOOL execIsDLL(void) = 0;
    virtual BOOL execIsProgram(void) = 0;
    virtual BOOL execIsDriver(void) = 0;
    #endif
};

#endif
