/* $Id: OS2KSM.h,v 1.2 2001-09-26 03:52:36 bird Exp $
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
#ifdef KKRNLLIB
extern PSHORT       pSMswapping;
#define SMswapping (*pSMswapping)
#else
extern SHORT        SMswapping;
#endif


/**
 * Real swapper file size in disk frames (ie. pages).
 */
#ifdef KKRNLLIB
extern PULONG       psmFileSize;
#define smFileSize (*psmFileSize)
#else
extern ULONG        smFileSize;
#endif


/**
 * Number of broken disk frames.
 */
#ifdef KKRNLLIB
extern PULONG       psmcBrokenDF;
#define smcBrokenDF (*psmcBrokenDF)
#else
extern ULONG        smcBrokenDF;
#endif


/**
 * Number of disk frames (DF) currently in use.
 */
#ifdef KKRNLLIB
extern PULONG       pSMcDFInuse;
#define SMcDFInuse (*pSMcDFInuse)
#else
extern ULONG        SMcDFInuse;
#endif


/**
 * Number of available disk frames (DF) currently.
 */
#define SMcDFAvail (smFileSize - smcBrokenDF - SMcDFInuse)


/**
 * The minimum free space on the swap volume (in pages). (Addjusted SMCFGMinFree)
 */
#ifdef KKRNLLIB
extern PULONG       pSMMinFree;
#define SMMinFree (*pSMMinFree)
#else
extern ULONG        SMMinFree;
#endif


/**
 * The minimum free space on the swap volumen (in pages * 4) specified in config.sys.
 */
#ifdef KKRNLLIB
extern PULONG       pSMCFGMinFree;
#define SMCFGMinFree (*pSMCFGMinFree)
#else
extern ULONG        SMCFGMinFree;
#endif


/**
 * The initial swapfile size (in pages * 4) specified in config.sys.
 */
#ifdef KKRNLLIB
extern PULONG       pSMCFGSwapSize;
#define SMCFGSwapSize  (*pSMCFGSwapSize)
#else
extern ULONG        SMCFGSwapSize;
#endif


/**
 * Number of swapper grow attempt which has failed since boot.
 */
#ifdef KKRNLLIB
extern PULONG       psmcGrowFails;
#define smcGrowFails (*psmcGrowFails)
#else
extern ULONG        smcGrowFails;
#endif


/**
 * Number of pages in a in memory swap file.
 * This count isn't counted into the SMcDFInuse count I think.
 */
#ifdef KKRNLLIB
extern PULONG       pSMcInMemFile;
#define SMcInMemFile (*pSMcInMemFile)
#else
extern ULONG        SMcInMemFile;
#endif


#endif
