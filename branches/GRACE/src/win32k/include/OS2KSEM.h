/* $Id: OS2KSEM.h,v 1.1 2000-07-16 22:21:19 bird Exp $
 *
 * OS/2 kernel Semaphore functions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _OS2KSEM_h_
#define _OS2KSEM_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define KSEM_INDEFINITE_WAIT     -1L
#define KSEM_IMMEDIATE_RETURN     0L


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef ULONG  HKSEM;                   /* Handle to kernel semaphore. */
typedef HKSEM  PHKSEM;                  /* Pointer to kernel semaphore handle. */
typedef HKSEM  HKMTX;                   /* Handle to kernel mutex semaphore. */
typedef HKSEM  HKEV;                    /* Handle to kernel event semaphore. */


/*******************************************************************************
*   Exported Functions                                                         *
*******************************************************************************/
/*
 * Mutex semaphores.
 * NOTE! Only is KSEMRequestMutex currently is imported!
 */
extern ULONG KRNLCALL KSEMRequestMutex(HKMTX hkmtx, ULONG ulTimeout);
extern VOID  KRNLCALL KSEMReleaseMutex(HKMTX hkmtx);
extern ULONG KRNLCALL KSEMQueryMutex(HKMTX hkmtx, PUSHORT pus);


#if 0
/*
 * Event semaphores.
 */
extern VOID  KRNLCALL KSEMResetEvent(HKEV hkevent);
extern VOID  KRNLCALL KSEMPostEvent(HKEV hkevent);
extern ULONG KRNLCALL KSEMWaitEvent(HKEV hkevent);

/*
 * Some other KSEM prefixed functions - parameters's not that obvious...
 */
extern ULONG KRNLCALL KSEMAlloc(PHKSEM phksem, ULONG p1, ULONG p2);
extern ULONG KRNLCALL KSEMCreate(PHKSEM phksem, ULONG type);
extern ULONG KRNLCALL KSEMRequestExclusive(HKSEM hksem, ULONG );
extern ULONG KRNLCALL KSEMRequestShared(HKSEM hksem, ULONG);
extern VOID  KRNLCALL KSEMDestroy(HKSEM hksem);
extern VOID  KRNLCALL KSEMInit(HKSEM hksem, ULONG p1, ULONG p2);
extern VOID  KRNLCALL KSEMQuery(HKSEM hksem, ULONG p2)
extern VOID  KRNLCALL KSEMRelease(HKSEM hksem);
#endif
#endif

