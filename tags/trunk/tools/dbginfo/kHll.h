/* $Id: kHll.h,v 1.2 2000-03-25 23:50:11 bird Exp $
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


class kHll

/**
 * HLL Public symbol entry.
 * @author      knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
class kHllPubSymEntry : public kListEntry
{
private:
    PHLLPUBLICSYM       pPubSym;

public:
    kHLLPubSym(
        const char *        pszName,
        unsigned long       off,
        unsigned short      iObj,
        unsigned short      iType
        );
    ~kHLLPubSym();

    int         write(FILE *phFile);
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
    kList<kHllPubSymEntry>     PublicSymbols;


public:
    /** @cat
     * Constructors and Destructors.
     */
    kHll();
    ~kHll();

    /** @cat
     * Add menthods
     */
    const void *    addObject(
                        const char *        pszName,
                        unsigned long int   cb
                        );
    const void *    addModule(
                        const char *        pszName,
                        const void *        pvLib,
                        unsigned            cObject,
                        PMODOBJECT          paObjects
                        );
    const void *    addPublicSymbol(
                        const char *        pszName,
                        unsigned long int   off,
                        unsigned short int  iObject,
                        const void *        pvType
                        );
    /*
    const void *    addPublicSymbol(
                        const char *        pszName,
                        unsigned long int   off,
                        unsigned short int  iObject,
                        const char *        pszType
                            );
    */


    /** @cat
     * Output.
     */
    BOOL            write(
                        const char *pszFilename
                        );

};



#endif

