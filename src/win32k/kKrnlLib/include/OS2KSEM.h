/* $Id: OS2KSEM.h,v 1.3 2001-10-14 22:50:21 bird Exp $
 *
 * OS/2 kernel Semaphore functions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _OS2KSEM_h_
#define _OS2KSEM_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define KSEM_INDEFINITE_WAIT       -1UL
#define KSEM_IMMEDIATE_RETURN       0UL

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

    struct
    {
        char            ks_achSignature[4];
        char            ks_bFlags;
        char            ks_bType;
        unsigned short  ks_Owner;
        unsigned short  ks_cusPendingWriters;
        unsigned short  ks_cusNest;
        unsigned short  ks_cusReaders;
        unsigned short  ks_cusPendingReaders;
    } debug;

    struct
    {
        char            ks_bFlags;
        char            ks_bType;
        unsigned short  ks_Owner;
        unsigned short  ks_cusPendingWriters;
        unsigned short  ks_cusNest;
        unsigned short  ks_cusReaders;
        unsigned short  ks_cusPendingReaders;
    } release;

}   KSEMSHR,
   *PKSEMSHR,
   *HKSEMSHR;                           /* Handle to kernel shared semphore. */
typedef HKSEMSHR * PHKSEMSHR;


typedef union _KSEMMTX
{
    /**
     * Astrict is 12 byte while retail is 8 bytes.
     * We'll reserve 20 bytes just to be sure!
     */
    char achDummy[20];

    struct
    {
        char            ksem_achSignature[4];
        char            ksem_bFlags;
        char            ksem_bType;
        unsigned short  ksem_Owner;
        unsigned short  ksem_cusPendingWriters;
        unsigned short  ksem_cusNest;
    }debug;
    struct
    {
        char            ksem_bFlags;
        char            ksem_bType;
        unsigned short  ksem_Owner;
        unsigned short  ksem_cusPendingWriters;
        unsigned short  ksem_cusNest;
    } release;
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

    struct
    {
        char            kse_achSignature[4];
        char            kse_bFlags;
        char            kse_bType;
        unsigned short  kse_Owner;
        unsigned short  kse_cusPendingWriters;
    } debug;

    struct
    {
        char            kse_bFlags;
        char            kse_bType;
        unsigned short  kse_Owner;
        unsigned short  kse_cusPendingWriters;
    } release;

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
extern ULONG KRNLCALL OrgKSEMRequestMutex(HKSEMMTX hkmtx, ULONG ulTimeout);
extern VOID  KRNLCALL KSEMReleaseMutex(HKSEMMTX hkmtx);
extern VOID  KRNLCALL OrgKSEMReleaseMutex(HKSEMMTX hkmtx);
extern ULONG KRNLCALL KSEMQueryMutex(HKSEMMTX hkmtx, PUSHORT pcusNest);
extern ULONG KRNLCALL OrgKSEMQueryMutex(HKSEMMTX hkmtx, PUSHORT pcusNest);


/*
 * Event semaphores.
 */
extern VOID  KRNLCALL KSEMResetEvent(HKSEMEVT hkev);
extern VOID  KRNLCALL OrgKSEMResetEvent(HKSEMEVT hkev);
extern VOID  KRNLCALL KSEMPostEvent(HKSEMEVT hkev);
extern VOID  KRNLCALL OrgKSEMPostEvent(HKSEMEVT hkev);
extern ULONG KRNLCALL KSEMWaitEvent(HKSEMEVT hkev, ULONG ulTimeout);
extern ULONG KRNLCALL OrgKSEMWaitEvent(HKSEMEVT hkev, ULONG ulTimeout);


/*
 * Shared semaphores.
 */
extern ULONG KRNLCALL KSEMRequestExclusive(HKSEMSHR hkshr, ULONG ulTimeout);
extern ULONG KRNLCALL OrgKSEMRequestExclusive(HKSEMSHR hkshr, ULONG ulTimeout);
extern ULONG KRNLCALL KSEMRequestShared(HKSEMSHR hkshr, ULONG ulTimeout);
extern ULONG KRNLCALL OrgKSEMRequestShared(HKSEMSHR hkshr, ULONG ulTimeout);


/*
 * Some other KSEM prefixed functions - parameters's not that obvious...
 */
#if 0
extern ULONG KRNLCALL KSEMAlloc(PHKSEM phksem, ULONG p1, ULONG p2);
extern ULONG KRNLCALL KSEMCreate(PHKSEM phksem, ULONG type);
extern VOID  KRNLCALL KSEMDestroy(HKSEM hksem);
#endif
extern VOID  KRNLCALL KSEMInit(PKSEM pksem, ULONG fulType, ULONG fulFlags);
extern VOID  KRNLCALL OrgKSEMInit(PKSEM pksem, ULONG fulType, ULONG fulFlags);
extern VOID  KRNLCALL KSEMRelease(HKSEM hksem);
extern VOID  KRNLCALL OrgKSEMRelease(HKSEM hksem);
extern VOID  KRNLCALL KSEMQuery(HKSEM hksem, PULONG pul);
extern VOID  KRNLCALL OrgKSEMQuery(HKSEM hksem, PULONG pul);

#endif

