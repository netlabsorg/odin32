/* $Id: kHll.h,v 1.1 2000-03-25 21:10:00 bird Exp $
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



class kHll
{
private:
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
                        const void *

                        ...);
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
}



#endif

