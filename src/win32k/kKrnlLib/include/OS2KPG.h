/* $Id: OS2KPG.h,v 1.1 2001-09-14 01:50:15 bird Exp $
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

/**
 * Paging enabled flag.
 * Two states:<ul>
 *      <li> 1 - Swapping enabled.
 *      <li> 0 - Swapping disabled. </ul>
 */
extern int * pPGSwapEnabled;
#define PGSwapEnabled (*pPGSwapEnabled)

/**
 * Count of physical pages - hope this is correct.
 */
extern int * ppgPhysPages;
#define pgPhysPages (*ppgPhysPages)

/**
 * Pointer to top of physical memory (physical address).
 */
extern PULONG ppgPhysMax;
#define pgPhysMax (*ppgPhysMax)

/**
 * Count of resident pages.
 */
extern int * ppgResidentPages;
#define pgResidentPages (*ppgResidentPages)

/**
 * Count of swappable pages which is currently present in memory.
 */
extern int * ppgSwappablePages;
#define pgSwappablePages (*ppgSwappablePages)

/**
* Count of discardable pages which is currently present in memory.
*/
extern int * ppgDiscardableInmem;
#define pgDiscardableInmem (*ppgDiscardableInmem)

/**
 * Count of discardable pages allocated.
 */
extern int * ppgDiscardablePages;
#define pgDiscardablePages (*ppgDiscardablePages)

/**
 * Count of page faults since bootup.
 */
extern PULONG ppgcPageFaults;
#define pgcPageFaults (*ppgcPageFaults)

/**
 * Count of page faults currently being processed.
 */
extern PULONG ppgcPageFaultsActive;
#define pgcPageFaultsActive (*ppgcPageFaultsActive)


/**
 * Returns the number of bytes of physical memory available.
 */
ULONG KRNLCALL PGPhysAvail(void);


/**
 * Returns the number of pageframes currently in use.
 */
ULONG KRNLCALL PGPhysPresent(void);

#endif

