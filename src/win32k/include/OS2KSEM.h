/* $Id: OS2KSEM.h,v 1.1.2.1 2000-08-22 03:00:18 bird Exp $
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
#define KSEM_INDEFINITE_WAIT       -1L
#define KSEM_IMMEDIATE_RETURN       0L

/*
 * Semaphore type. ( Used with the generic KSEM routines. )
 */
#define KSEM_EVENT                  0
#define KSEM_MUTEX                  1
#define KSEM_SHARED                 2

/*
 * Sempahore flags. (according to SG24-4640-00)
 */
#define KSEM_DEFAULT                0
#define KSEM_NOINTERRUPT            1
#define KSEM_WRITE                  2
#define KSEM_DISPLAYID              4
#define KSEM_NOBLOCKED              8


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/* (This is according to SG24-4640-00.) */

typedef struct _KSEMSHR
{
    /**
     * Astrict 16 is byte while retail 12 is bytes.
     * We'll reserve 20 bytes just to be sure!.
     */
    char achDummy[20];
}   KSEMSHR,
   *PKSEMSHR,
   *HKSEMSHR;                           /* Handle to kernel shared semphore. */
typedef HKSEMSHR * PHKSEMSHR;


typedef struct _KSEMMTX
{
    /**
     * Astrict is 12 byte while retail is 8 bytes.
     * We'll reserve 20 bytes just to be sure!
     */
    char achDummy[20];
}   KSEMMTX,
   *PKSEMMTX,
   *HKSEMMTX;                           /* Handle to kernel mutex semaphore. */
typedef HKSEMMTX * PHKSEMMTX;


typedef struct _KSEMEVT
{
    /**
     * Astrict is 16 byte while retail is 12 bytes.
     * We'll reserve 20 bytes just to be sure!
     */
    char achDummy[20];
}   KSEMEVT,
   *PKSEMEVT,
   *HKSEMEVT;                           /* Handle to kernel event sempahore. */
typedef HKSEMEVT * PHKSEMEVT;


typedef union _KSEM
{
    KSEMSHR shr;
    KSEMMTX mtx;
    KSEMEVT evt;
} KSEM, *PKSEM, *HKSEM;                 /* Generic kernel semaphore handle. */
typedef HKSEM * PHKSEM;


/*******************************************************************************
*   Exported Functions                                                         *
*******************************************************************************/
/*
 * Mutex semaphores.
 */
extern ULONG KRNLCALL KSEMRequestMutex(HKSEMMTX hkmtx, ULONG ulTimeout);
extern VOID  KRNLCALL KSEMReleaseMutex(HKSEMMTX hkmtx);
extern ULONG KRNLCALL KSEMQueryMutex(HKSEMMTX hkmtx, PUSHORT pcusNest);


#if 0
/*
 * Event semaphores.
 */
extern VOID  KRNLCALL KSEMResetEvent(HKEV hkevent);
extern VOID  KRNLCALL KSEMPostEvent(HKEV hkevent);
extern ULONG KRNLCALL KSEMWaitEvent(HKEV hkevent);
#endif

/*
 * Some other KSEM prefixed functions - parameters's not that obvious...
 */
#if 0
extern ULONG KRNLCALL KSEMAlloc(PHKSEM phksem, ULONG p1, ULONG p2);
extern ULONG KRNLCALL KSEMCreate(PHKSEM phksem, ULONG type);
extern ULONG KRNLCALL KSEMRequestExclusive(HKSEM hksem, ULONG );
extern ULONG KRNLCALL KSEMRequestShared(HKSEM hksem, ULONG);
extern VOID  KRNLCALL KSEMDestroy(HKSEM hksem);
#endif
extern VOID  KRNLCALL KSEMInit(PKSEM pksem, ULONG fulType, ULONG fulFlags);
#if 0
extern VOID  KRNLCALL KSEMQuery(HKSEM hksem, ULONG p2)
extern VOID  KRNLCALL KSEMRelease(HKSEM hksem);
#endif
#endif

