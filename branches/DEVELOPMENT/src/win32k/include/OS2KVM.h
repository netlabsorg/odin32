/* $Id: OS2KVM.h,v 1.4 2000-04-17 01:56:49 bird Exp $
 *
 * OS/2 kernel VM functions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _OS2KVM_h_
#define _OS2KVM_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * VMAlloc flFlag1 guessings
 */
#define VMA_CONTIG          0x00000001  /* VMDHA_CONTIG */
#define VMA_WRITE           0x00000002  /* PAG_WRITE and OB_WRITE */
#define VMA_USER            0x00000004  /* OB_USER */
#define VMA_EXECUTE         0x00000008  /* PAG_EXECUTE and OB_READ !! */
#define VMA_READ            0x00000010  /* PAG_READ and OB_EXEC !! */
#define VMA_LOWMEM          0x00000020  /* OB_LOWMEM < 1MB physical */
#define VMA_GUARD           0x00000040  /* PAG_GUARD and VMDHA_16M and OB_GUARD */
#if 0
#define VMA_RESIDENT        0x00000080  /* OB_RESIDENT */
#define VMA_ZEROINIT        0x00000100  /* OB_ZEROINIT */
#define VMA_PHYS            0x00000200  /* VMDHA_PHYS */
#define VMA_FIXED           0x00000400  /* VMDHA_FIXED and OB_SHARED */
#else
#define VMA_ZEROFILL        0x00000080
#define VMA_SWAPONWRITE     0x00000100
#define VMA_UVIRT           0x00000200  /* VMDHA_PHYS */
#define VMA_RESIDENT        0x00000400  /* VMDHA_FIXED and OB_SHARED */
#endif
#define VMA_DISCARDABLE     0x00000800  /* discarable object */
#define VMA_SHARE           0x00001000  /* OBJSHARE and OBJEXEC (which implies shared) */
#define VMA_PROTECTED       0x00004000  /* PAG_PROTECTED */
#define VMA_LOWMEM2         0x00010000  /* OB_LOWMEM */
#define VMA_VDM             0x00040000  /* VDM */
#define VMA_DECOMMIT        0x00080000  /* PAG_DECOMMIT */
#define VMA_TILE            0x00400000  /* OBJ_TILE */
#define VMA_SELALLOC        0x00400000  /* Allocates selector */
#define VMA_SHRINKABLE      0x00800000  /* OB_SHRINKABLE */
#define VMA_HUGH            0x01000000  /* OB_HUGH */

#define VMA_ARENASHARED     0x04000000  /* Shared Arena */
#define VMA_ARENASYSTEM     0x00000000  /* System Arena */
#define VMA_ARENAPRIVATE    0x02000000  /* Private Arena */
#define VMA_ARENAHEAP       0x06000000  /* Heap Arena */
#define VMA_ARENAHIGHA      0x00008000  /* High shared arena (Warp >= fp13) */
#define VMA_ARENAHIGH   (options.ulBuild >= MERLINFP13 ? VMA_ARENAHIGHA : 0UL)
#define VMA_ARENAMASKW      0x06000000  /* Warp < fp13 Arena Mask */
#define VMA_ARENAMASKA      0x06008000  /* Aurora Arena Mask */
#define VMA_ARENAMASK   (options.ulBuild >= MERLINFP13 ? VMA_ARENAMASKA : VMA_ARENAMASKW)

#define VMA_LOCMASK         0xC0000000  /* Location mask */
#define VMA_LOCSPECIFIC     0x80000000  /* Specific location */
#define VMA_LOCABOVE        0x40000000  /* Above or equal to specified location */
#define VMA_LOCANY          0x00000000  /* Anywhere */



/*
 * VMAlloc flFlags2 guessings
 */
#define VMAF2_WRITE         0x00000002 /* PAG_WRITE */


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _vmac
{
    ULONG       ac_va;                  /* off=0 cb=4 Virtual Address. */
    USHORT      ac_sel;                 /* off=4 cb=2 Selector. */
    USHORT      ac_hob;                 /* off=6 cb=2 Object handle. */
} VMAC, *PVMAC;


typedef USHORT HMTE, *PHMTE;
typedef USHORT HPTDA, *PHPTDA;

/*******************************************************************************
*   Exported Functions                                                         *
*******************************************************************************/
HMTE KRNLCALL VMGetOwner(
    ULONG ulCS,
    ULONG ulEIP);

APIRET KRNLCALL VMAllocMem(
    ULONG   cbSize,
    ULONG   cbCommit,
    ULONG   flFlags1,
    HPTDA   hPTDA,
    USHORT  usVMOwnerId,
    HMTE    hMTE,
    ULONG   flFlags2,
    ULONG   SomeArg2,
    PVMAC   pvmac);

APIRET KRNLCALL VMObjHandleInfo(
    USHORT  usHob,
    PULONG  pulAddr,
    PUSHORT pushPTDA);

#endif
