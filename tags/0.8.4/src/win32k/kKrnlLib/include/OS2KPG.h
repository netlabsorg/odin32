/* $Id: OS2KPG.h,v 1.3 2001-09-27 03:04:12 bird Exp $
 *
 * OS/2 kernel page manager stuff.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _OS2KPG_H_
#define _OS2KPG_H_

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifndef KKRNLLIB
    #if defined(__IBMC__) || defined(__IBMCPP__)
        #pragma map(PGSwapEnabled       , "_PGSwapEnabled")
        #pragma map(pgPhysPages         , "_pgPhysPages")
        #pragma map(pgPhysMax           , "_pgPhysMax")
        #pragma map(pgResidentPages     , "_pgResidentPages")
        #pragma map(pgSwappablePages    , "_pgSwappablePages")
        #pragma map(pgDiscardableInmem  , "_pgDiscardableInmem")
        #pragma map(pgDiscardablePages  , "_pgDiscardablePages")
        #pragma map(pgcPageFaults       , "_pgcPageFaults")
        #pragma map(pgcPageFaultsActive , "_pgcPageFaultsActive")
    #else
        #pragma PGSwapEnabled            _PGSwapEnabled
        #pragma pgPhysPages              _pgPhysPages
        #pragma pgPhysMax                _pgPhysMax
        #pragma pgResidentPages          _pgResidentPages
        #pragma pgSwappablePages         _pgSwappablePages
        #pragma pgDiscardableInmem       _pgDiscardableInmem
        #pragma pgDiscardablePages       _pgDiscardablePages
        #pragma pgcPageFaults            _pgcPageFaults
        #pragma pgcPageFaultsActive      _pgcPageFaultsActive
    #endif
#endif

/**
 * Paging enabled flag.
 * Two states:<ul>
 *      <li> 1 - Swapping enabled.
 *      <li> 0 - Swapping disabled. </ul>
 */
#ifdef KKRNLLIB
extern int * pPGSwapEnabled;
#define PGSwapEnabled (*pPGSwapEnabled)
#else
extern int   PGSwapEnabled;
#endif


/**
 * Count of physical pages - hope this is correct.
 */
#ifdef KKRNLLIB
extern int * ppgPhysPages;
#define pgPhysPages (*ppgPhysPages)
#else
extern int   pgPhysPages;
#endif


/**
 * Pointer to top of physical memory (physical address).
 */
#ifdef KKRNLLIB
extern PULONG ppgPhysMax;
#define pgPhysMax (*ppgPhysMax)
#else
extern ULONG  pgPhysMax;
#endif


/**
 * Count of resident pages.
 */
#ifdef KKRNLLIB
extern int * ppgResidentPages;
#define pgResidentPages (*ppgResidentPages)
#else
extern int   pgResidentPages;
#endif


/**
 * Count of swappable pages which is currently present in memory.
 */
#ifdef KKRNLLIB
extern int * ppgSwappablePages;
#define pgSwappablePages (*ppgSwappablePages)
#else
extern int   pgSwappablePages;
#endif


/**
* Count of discardable pages which is currently present in memory.
*/
#ifdef KKRNLLIB
extern int * ppgDiscardableInmem;
#define pgDiscardableInmem (*ppgDiscardableInmem)
#else
extern int   pgDiscardableInmem;
#endif


/**
 * Count of discardable pages allocated.
 */
#ifdef KKRNLLIB
extern int * ppgDiscardablePages;
#define pgDiscardablePages (*ppgDiscardablePages)
#else
extern int   pgDiscardablePages;
#endif


/**
 * Count of page faults since bootup.
 */
#ifdef KKRNLLIB
extern PULONG ppgcPageFaults;
#define pgcPageFaults (*ppgcPageFaults)
#else
extern ULONG  pgcPageFaults;
#endif


/**
 * Count of page faults currently being processed.
 */
#ifdef KKRNLLIB
extern PULONG ppgcPageFaultsActive;
#define pgcPageFaultsActive (*ppgcPageFaultsActive)
#else
extern ULONG  pgcPageFaultsActive;
#endif


/**
 * Returns the number of bytes of physical memory available.
 */
ULONG KRNLCALL PGPhysAvail(void);
ULONG KRNLCALL OrgPGPhysAvail(void);


/**
 * Returns the number of pageframes currently in use.
 */
ULONG KRNLCALL PGPhysPresent(void);
ULONG KRNLCALL OrgPGPhysPresent(void);


/**
 * This function returns a bit vector for the dirty pages of a specified context
 * for a specified range of pages.
 *
 * The dirty bits for the range are reset.
 *
 * @returns OS/2 return code. (NO_ERROR on success)
 * @param   ulVirtualPageNo     Virtual page fram number.
 * @param   pPTDA               Pointer to the context.
 * @param   cPages              Number of pages. (max 32-bit due to ulDirtyBits.)
 * @param   ulDirtyBits         Dirty bits.
 * @remark  VDM function... Not imported any longer.
 */
/*ULONG KRNLCALL PGGetDirty(ULONG ulVirtualPageNo, PPTDA pPTDA, ULONG cPages, PULONG ulDirtyBits);*/
/*ULONG KRNLCALL OrgPGGetDirty(ULONG ulVirtualPageNo, PPTDA pPTDA, ULONG cPages, PULONG ulDirtyBits);*/


#endif

