/* $Id: kInterfaces.h,v 1.3 2001-04-17 04:16:02 bird Exp $
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


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define MAXEXPORTNAME   256
#define MAXDBGNAME      256


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
class kPageI;
class kExportI;
class kExportEntry;
class kModuleI;
class kExecutableI;
class kDbgTypeI;


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
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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


/**
 * Communication class/struct used by kDbgTypeI and others.
 * This is structure or union member class.
 * (This might turn out very similar to a normal variable class,
 *  and will if so be changed to that when time comes.)
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kDbgMemberEntry
{
public:
    char        szName[MAXDBGNAME];     /* Member name. */
    char        cb;                     /* Count of bytes it covers. */
    char        szTypeName[MAXDBGNAME]; /* Type name. */
    int         flFlags;                /* Type flags. One of the kDbgTypeI::enm*. optional: defaults to kDbgTypeI::enmAny */
};


/**
 * Communication class/struct used by kDbgTypeI.
 * This is the type.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kDbgTypeEntry
{
public:
    char        szName[MAXDBGNAME];     /* Name of the type. */
    char        cb;                     /* Count of bytes it covers. */
    int         flFlags;                /* Type flags. One of the kDbgTypeI::enm*. */

    int         cMembers;               /* Number of members. This is 0 if not a union or a struct. */
    kDbgMemberEntry * paMembers;        /* Pointer array of member entries if union or struct. */

public:
    /** @cat Internal use only - don't mess! */
    int         flSearchFlags;          /* Type flags which this search was started with. One of the kDbgTypeI::enm*. */
};



/**
 * Interface class (ie. virtual) which defines the interface for
 * debug typeinfo on debug module level.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
class kDbgTypeI
{
public:
    /** @cat Debug Type information methods. */
    virtual kDbgTypeEntry * dbgtypeFindFirst(int flFlags);
    virtual kDbgTypeEntry * dbgtypeFindNext(kDbgTypeEntry *kDbgTypeEntry);
    virtual void            dbgtypeFindClose(kDbgTypeEntry *kDbgTypeEntry);

    virtual kDbgTypeEntry * dbgtypeLookup(const char *pszName, int flFlags);
    enum
    {   enmAny,                         /* Any/unknown type. */
        enmStruct,                      /* Structure:       struct _somestruct {..}; */
        enmUnion,                       /* Union:           union  _someunion  {..}; */
        enmEnum,                        /* Enumeration:     enum   _someenum   {..}; */
        enmTypedef,                     /* Type definition: typedef <type expr>     sometype; */
        enmMask = 0x0000000ff,          /* Type mask. */
        enmFlagMask = 0xffffff00,       /* Flag mask. */
        enmflPointer = 0x00000100       /* This is pointer to something. (flag) */
    };
};
#endif
