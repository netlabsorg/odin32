/* $Id: OS2KSM.h,v 1.1 2001-09-14 01:50:15 bird Exp $
 *
 * OS/2 kernel swapper manager stuff.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _OS2KSM_H_
#define _OS2KSM_H_


/**
 * Swapping enabled flag.
 * Two states:<ul>
 *      <li> 1 - Swapping enabled.
 *      <li> 0 - Swapping disabled. </ul>
 */
extern PSHORT       pSMswapping;
#define SMswapping (*pSMswapping)

/**
 * Real swapper file size in disk frames (ie. pages).
 */
extern PULONG       psmFileSize;
#define smFileSize (*psmFileSize)

/**
 * Number of broken disk frames.
 */
extern PULONG       psmcBrokenDF;
#define smcBrokenDF (*psmcBrokenDF)

/**
 * Number of disk frames (DF) currently in use.
 */
extern PULONG       pSMcDFInuse;
#define SMcDFInuse (*pSMcDFInuse)

/**
 * Number of available disk frames (DF) currently.
 */
#define SMcDFAvail (smFileSize - smcBrokenDF - SMcDFInuse)

/**
 * The minimum free space on the swap volume (in pages). (Addjusted SMCFGMinFree)
 */
extern PULONG       pSMMinFree;
#define SMMinFree (*pSMMinFree)

/**
 * The minimum free space on the swap volumen (in pages * 4) specified in config.sys.
 */
extern PULONG       pSMCFGMinFree;
#define SMCFGMinFree (*pSMCFGMinFree)

/**
 * The initial swapfile size (in pages * 4) specified in config.sys.
 */
extern PULONG       pSMCFGSwapSize;
#define SMCFGSwapSize  (*pSMCFGSwapSize)

/**
 * Number of swapper grow attempt which has failed since boot.
 */
extern PULONG       psmcGrowFails;
#define smcGrowFails (*psmcGrowFails)

/**
 * Number of pages in a in memory swap file.
 * This count isn't counted into the SMcDFInuse count I think.
 */
extern PULONG       pSMcInMemFile;
#define SMcInMemFile (*pSMcInMemFile)


#endif
