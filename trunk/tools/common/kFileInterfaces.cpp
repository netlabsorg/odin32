/* $Id: kFileInterfaces.cpp,v 1.1 2002-02-24 02:47:25 bird Exp $
 *
 * kFileInterface.cpp - Interface helpers and workers.
 *
 *      I've decided that this is the logical place to put many
 *      of those generic worker/helper methods which works on
 *      or with the interfaces. In java this would be impossible
 *      using the real interfaces, but in C++ when we use empty
 *      classes it very possible and quite helpful.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <malloc.h>

#include "kTypes.h"
#include "kError.h"
#include "kFileInterfaces.h"



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/**
 * XRef status and search info.
 * kRelocEntry::pv2 points to this structure.
 */
typedef struct _XRefState
{
    /* Position we wanna find references to. */
    unsigned long   ulSegment;
    unsigned long   offSegment;
} XREFSTATE, *PXREFSTATE;



/**
 * Finds the first reference to a location in the image.
 *
 * @returns Success indicator.
 *          TRUE if found anything.
 *          FALSE if nothing
 * @param   ulSegment   Segment number (0-based).
 *                      Special segment numbers are not supported.
 * @param   offSegment  Offset into the segement (ulSegment).
 * @param   preloc      The enumeration 'handle.'
 * @status  implemented.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark  No special segment numbers are supported.
 */
KBOOL    kRelocI::relocXRefFindFirst(unsigned long ulSegment, unsigned long offSegment, kRelocEntry *preloc)
{
    kASSERT(preloc);
    kASSERT(ulSegment < kRelocEntry::enmFirstSelector);

    /*
     * Save the target we are searching for references to.
     */
    PXREFSTATE  pState = (PXREFSTATE)malloc(sizeof(XREFSTATE));
    pState->ulSegment = ulSegment;
    pState->offSegment = offSegment;
    preloc->pv2 = (void*)pState;

    /*
     * Start searching.
     */
    if (relocFindFirst(0, 0, preloc))
    {
        kASSERT(ulSegment < kRelocEntry::enmFirstSelector);
        if (    preloc->isInternal()
            &&  preloc->Info.Internal.ulSegment == ulSegment
            &&  preloc->Info.Internal.offSegment == offSegment
            )
            return TRUE;

        KBOOL fRc = relocXRefFindNext(preloc);
        if (!fRc)
            relocXRefFindClose(preloc);
        return fRc;
    }

    return FALSE;
}


/**
 * Finds the next reference to the address.
 * @returns Success indicator.
 *          Returns FALSE if no more matches.
 * @param   preloc  The enumeration 'handle.'
 *                  This must be initiated by a call to relocXRefFindFirst.
 * @status  implemented.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 */
KBOOL    kRelocI::relocXRefFindNext(kRelocEntry *preloc)
{
    kASSERT(preloc->pv2 != NULL);

    while (relocFindNext(preloc))
    {
        kASSERT(preloc->ulSegment < kRelocEntry::enmFirstSelector);
        if (    preloc->isInternal()
            &&  preloc->Info.Internal.ulSegment ==  ((PXREFSTATE)preloc->pv2)->ulSegment
            &&  preloc->Info.Internal.offSegment == ((PXREFSTATE)preloc->pv2)->offSegment
            )
            return TRUE;
    }

    return FALSE;
}


/**
 * Closes a find session - this function *must* be called to
 * clean up resources used internally.
 *
 * @param   preloc  The enumeration 'handle.'
 *                  This must be initiated by a call to relocXRefFindFirst.
 * @status  implemented.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark
 */
void    kRelocI::relocXRefFindClose(kRelocEntry *preloc)
{
    kASSERT(preloc != NULL);
    relocFindClose(preloc);
    free(preloc->pv2);
    preloc->pv2 = NULL;
}

