/* $Id: kFileInterfaces.h,v 1.1 2002-02-24 02:47:26 bird Exp $
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
class kRelocI;



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
     * @returns 0 on success. kError number on error.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      will receive the page data on the specified address.
     * @param   ulAddress   Page address. This must be page aligned.
     */
    virtual int     pageGet(char *pachPage, unsigned long ulAddress) const = 0;

    /**
     * Get a code, data or resource page from the file.
     * @returns 0 on success. kError number on error.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      will receive the page data on the specified address.
     * @param   iSegment    Segment number.  (0-based)
     * @param   offObject   Offset into the object. This must be page aligned.
     * @remark  Object = Section.
     */
    virtual int     pageGet(char *pachPage, int iSegment, int offObject) const = 0;

    /**
     * Updates or adds a code, data, or resource page to the file.
     * @returns 0 on success. kError number on error.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      holds the page data.
     * @param   ulAddress   Page address. This must be page aligned.
     */
    virtual int     pagePut(const char *pachPage, unsigned long ulAddress) = 0;

    /**
     * Updates or adds a code, data, or resource page to the file.
     * @returns 0 on success. kError number on error.
     * @param   pachPage    Pointer to a buffer of the size of a page which
     *                      holds the page data.
     * @param   iSegment    Segment number. (0-based)
     * @param   offObject   Offset into the object. This must be page aligned.
     */
    virtual int     pagePut(const char *pachPage, int iSegment, int offObject) = 0;

    /**
     * Get pagesize for the file.
     * @returns Pagesize in bytes.
     */
    virtual int     pageGetPageSize() const = 0;
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
    virtual KBOOL   exportFindFirst(kExportEntry * pExport) = 0;

    /**
     * Find the next export/public name.
     * @returns Success indicator.
     *          FALSE when no more exports (exportFindClose has been processed then).
     * @param   pExport     Communication area which has been successfully
     *                      processed by findFirstExport.
     */
    virtual KBOOL   exportFindNext(kExportEntry * pExport) = 0;

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
    virtual KBOOL   exportLookup(unsigned long ulOrdinal, kExportEntry *pExport) = 0;

    /**
     * Lookup information on a spesific export given by name.
     * @returns Success indicator.
     * @param   pExport     Communication area containing export information
     *                      on successful return.
     */
    virtual KBOOL   exportLookup(const char *  pszName, kExportEntry *pExport) = 0;
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
    virtual KBOOL   moduleGetName(char *pszBuffer, int cbBuffer = 260) = 0;
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
    virtual kDbgTypeEntry *     dbgtypeFindFirst(int flFlags) = 0;
    virtual kDbgTypeEntry *     dbgtypeFindNext(kDbgTypeEntry *kDbgTypeEntry) = 0;
    virtual void                dbgtypeFindClose(kDbgTypeEntry *kDbgTypeEntry) = 0;

    virtual kDbgTypeEntry *     dbgtypeLookup(const char *pszName, int flFlags) = 0;
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


/**
 * One reallocation (reloc for short).
 * @author      knut st. osmundsen (kosmunds@csc.com)
 */
class kRelocEntry
{
public:
    /** @cat constants and flags */
    enum
    {
        /* flags */
        enmFlagMask = 0x0000FFFF,
        enmLittleEndian = 0x0001,       /* Little endian */
        enmBigEndian    = 0x0002,       /* Big endian */
        enmAdditive     = 0x0004,       /* There is a addition (like extra offset) to the fixup. */

        /* relocation type part 1 */
        enmType1Mask= 0x000F0000,
        enmName     = 0x00010000,       /* Name import (no dll/module) */
        enmNameDLL  = 0x00020000,       /* Name import (from a given dll/module) */
        enmOrdDLL   = 0x00030000,       /* Ordinal import (from a given dll/module) */
        enmDelta    = 0x00040000,       /* Delta fixup - add the relocation delta */
        enmInternal = 0x00050000,       /* Write the pointer as enmSizeMask describes. */

        /* relocation type part 2 */
        enmType2Mask= 0x00F00000,
        enmOffset   = 0x00100000,       /* Offset fixup. */
        enmRelStart = 0x00200000,       /* The fixup is relative to the fixup location. */
        enmRelEnd   = 0x00300000,       /* The fixup is relative to the fixup location + size. */

        /* size */
        enmSizeMask = 0xFF000000,       /* Size mask */
        enm8        = 0x01000000,       /* 8 bit fixup */
        enm16       = 0x02000000,       /* 16 bit fixup */
        enm32       = 0x03000000,       /* 32 bit fixup */
        enm64       = 0x04000000,       /* 64 bit fixup */
        enm16_00    = 0x05000000,       /* 16:00 fixup (selector only) */
        enm16_16    = 0x06000000,       /* 16:16 fixup (selector:offset) */
        enm16_32    = 0x07000000,       /* 16:32 fixup (selector:offset) */
        enm16_48    = 0x08000000,       /* 16:48 fixup (selector:offset) */
        enm16hb     = 0x09000000,       /* 16 bit fixup - high byte */
        enm32hw     = 0x0a000000,       /* 32 bit fixup - high word */
        enm64hdw    = 0x0b000000,       /* 64 bit fixup - high dword */

        /* defaults */
        enmFirstSelector= 0xfffffffe,   /* Selector used if the offset is a Relative Virtual Address. */
        enmRVASelector  = 0xffffffff,   /* Selector used if the offset is a Relative Virtual Address. */
        enmVASelector   = 0xfffffffe    /* Selector used if the offset is a FLAT address. */
    };

    /** @cat public data (speed/size) */
    unsigned long           offSegment; /* The address of the fixup. (page offset, segment offset, RVA or FLAT address) */
    unsigned long           ulSegment;  /* Segment number the offSegment is relative to. Special selectors may be used. (0-based) */
    unsigned long           fFlags;     /* Combination of the above flags. Don't mess with this member. */
    unsigned long           ulAdd;      /* Value to add if additiv relocation. */

    union _RelocInfo
    {
        struct InternalInfo {
            unsigned long   offSegment; /* Offset into ulSegment of the 'target'. */
            unsigned long   ulSegment;  /* Segemnt number of the 'traget'. Special selectors may be used. (0-based) */
        } Internal;

        struct NameImportInfo {
            char *          pszName;    /* Pointer to string or NULL. Don't mess with this pointer! */
            char *          pszModule;  /* Pointer to module name or NULL. Don't mess with this pointer! */
            unsigned long   ulOrdinal;  /* Ordinal value if applicable. */
        } Name;
    } Info;

    /** @cat helper functions */
    KBOOL   isName() const              { return (fFlags & enmType1Mask) <= enmOrdDLL; };
    KBOOL   isInternal() const          { return (fFlags & enmType1Mask) > enmOrdDLL; };


public:
    /** @cat Internal use only - don't mess! */
    void *                  pv1;        /* some pointer - used by relocFind*() implementations. */
    void *                  pv2;        /* some pointer - used by relocXRef*(). */
};



/**
 * Relocation interface.
 * @author      knut st. osmundsen (kosmunds@csc.com)
 * @remark      This interface will not return any address by using the special selectors
 *              since there is no pulic way of converting them - yet.
 *              relocFindFirst will accept special selectors as input.
 *              relocXRefFindFirst will not.
 */
class kRelocI
{
public:
    /** @cat Relocation enumeration menthods */
    virtual KBOOL   relocFindFirst(unsigned long ulSegment, unsigned long offSegment, kRelocEntry *preloc) = 0;
    virtual KBOOL   relocFindFirst(unsigned long ulAddress, kRelocEntry *preloc) = 0;
    virtual KBOOL   relocFindNext(kRelocEntry *preloc) = 0;
    virtual void    relocFindClose(kRelocEntry *preloc) = 0;

    /** @cat Common worker methods. */
    KBOOL           relocXRefFindFirst(unsigned long ulSegment, unsigned long offSegment, kRelocEntry *preloc);
    KBOOL           relocXRefFindNext(kRelocEntry *preloc);
    void            relocXRefFindClose(kRelocEntry *preloc);

};



#endif
