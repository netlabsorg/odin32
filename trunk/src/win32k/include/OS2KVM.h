/* $Id: OS2KVM.h,v 1.9 2001-02-10 11:11:43 bird Exp $
 *
 * OS/2 kernel VM functions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
 * From SG24-4640-00
 * Object flags. (ob_fs)
 */
#define OB_PSEUDO           0x8000      /* Pseudo-object */
#define OB_API              0x4000      /* API allocated object */
#define OB_LOCKWAIT         0x2000      /* Some thread to wake in VMUnlock */
#define OB_LALIAS           0x1000      /* Object has aliases */
#define OB_SHARED           0x0800      /* Object's contents are shared */
#define OB_UVIRT            0x0400      /* UVirt object */
#define OB_ZEROINIT         0x0200      /* Object is zero-initialized */
#define OB_RESIDENT         0x0100      /* Initial allocation was resident */
#define OB_LOWMEM           0x0040      /* Object is in low memory */
#define OB_GUARD            0x0080      /* Page attribute/permission flags */
#define OB_EXEC             0x0020      /* Executable */
#define OB_READ             0x0010      /* Read permission */
#define OB_USER             0x0008      /* User Storage */
#define OB_WRITE            0x0004      /* Write permission */
#define OB_HUGE             0x0002      /* Object is huge */
#define OB_SHRINKABLE       0x0001      /* Object is Shrinkable */
#define OB_DHSETMEM         0x0001      /* DevHlp_VMSetMems are allowed */


/*
 * From SG24-4640-00
 * ob_xflags
 */
#define VMOB_SLOCK_WAIT     0x01        /* Waiting on short term locks to clear */
#define VMOB_LLOCK_WAIT     0x02        /* Waiting on long term locks to clear */
#define VMOB_DISC_SEG       0x04        /* Object is part of a discardable seg */
#define VMOB_HIGHMEM        0x08        /* Object was allocated via dh_vmalloc */



/*                                            v8086.h */
#define VM_PG_W            0x00000002      /* VPMPG_W       - Page Writeable. */
#define VM_PG_U            0x00000004      /* VPMPG_U       - Page User Mode Accessible.*/
#define VM_PG_X            0x00000008      /* VPMPG_X       - Page Executable. */
#define VM_PG_R            0x00000010      /* VPMPG_R       - Page Readable. */
#define VM_PG_RESERVED     0x00001000      /* VPMPG_RESERVED- Reservered */

/*                                            vpmx2.h */
#define VM_PG_SWAPPABLE    0x00000000      /* Swappable */



/*
 * VMAlloc flFlag1 guessings
 */
#define VMA_CONTIG          0x00000001UL  /* VMDHA_CONTIG */
#define VMA_WRITE           VM_PG_W     /* PAG_WRITE and OB_WRITE */
#define VMA_USER            VM_PG_U     /* OB_USER */
#define VMA_EXECUTE         VM_PG_X     /* PAG_EXECUTE and OB_READ !! */
#define VMA_READ            VM_PG_R     /* PAG_READ and OB_EXEC !! */
#define VMA_LOWMEM          0x00000020UL  /* OB_LOWMEM < 1MB physical */
#define VMA_GUARD           0x00000040UL  /* PAG_GUARD and VMDHA_16M and OB_GUARD */
#if 0
#define VMA_RESIDENT        0x00000080UL  /* OB_RESIDENT */
#define VMA_ZEROINIT        0x00000100UL  /* OB_ZEROINIT */
#define VMA_PHYS            0x00000200UL  /* VMDHA_PHYS */
#define VMA_FIXED           0x00000400UL  /* VMDHA_FIXED and OB_SHARED */
#else
#define VMA_ZEROFILL        0x00000080UL
#define VMA_SWAPONWRITE     0x00000100UL
#define VMA_UVIRT           0x00000200UL  /* VMDHA_PHYS */
#define VMA_RESIDENT        0x00000400UL  /* VMDHA_FIXED and OB_SHARED */
#endif
#define VMA_DISCARDABLE     0x00000800UL  /* discarable object */
#define VMA_SHARE           0x00001000UL  /* OBJSHARE and OBJEXEC (which implies shared) */
#define VMA_PROTECTED       0x00004000UL  /* PAG_PROTECTED */
#define VMA_LOWMEM2         0x00010000UL  /* OB_LOWMEM */
#define VMA_VDM             0x00040000UL  /* (VPMVMAC_VDM)    VDM allocation */
#define VMA_DECOMMIT        0x00080000UL  /* PAG_DECOMMIT */
#define VMA_TILE            0x00400000UL  /* OBJ_TILE */
#define VMA_SELALLOC        0x00400000UL  /* Allocates selector */
#define VMA_SHRINKABLE      0x00800000UL  /* OB_SHRINKABLE */
#define VMA_HUGH            0x01000000UL  /* OB_HUGH */

#define VMA_ARENASHARED     0x04000000UL  /* Shared Arena */
#define VMA_ARENASYSTEM     0x00000000UL  /* System Arena */
#define VMA_ARENAPRIVATE    0x02000000UL  /* (VPMVMAC_ARENAPRV) Private Arena */
#define VMA_ARENAHEAP       0x06000000UL  /* Heap Arena */
#define VMA_ARENAHIGHA      0x00008000UL  /* High shared arena (Warp >= fp13) */
#define VMA_ARENAHIGH   (options.ulBuild >= AURORAGA ? VMA_ARENAHIGHA : 0UL)
#define VMA_ARENAMASKW      0x06000000UL  /* Warp < fp13 Arena Mask */
#define VMA_ARENAMASKA      0x06008000UL  /* Aurora Arena Mask */
#define VMA_ARENAMASK   (options.ulBuild >= AURORAGA ? VMA_ARENAMASKA : VMA_ARENAMASKW)

#define VMA_ALIGNSEL        0x10000000UL  /* Selector aligment */
#define VMA_ALIGNPAGE       0x18000000UL  /* (VPMVMAC_ALIGNPAGE)      Page alignment */

#define VMA_LOCMASK         0xC0000000UL  /* Location mask */
#define VMA_LOCSPECIFIC     0x80000000UL  /* (VPMVMAC_LOCSPECIFIC)    Specific location */
#define VMA_LOCABOVE        0x40000000UL  /* (VPMVMAC_LOCABOVE)       Above or equal to specified location */
#define VMA_LOCANY          0x00000000UL  /* Anywhere */



/*
 * VMAlloc flFlags2 guessings
 */
#if 0 /*???*/
#define VDHAM_FIXED         0x0000UL
#define VDHAM_SWAPPABLE     0x0001UL
#define VPMVMFM_VDM         0x00000004UL  /* Request on behalf of VDM */
#endif

#define VMAF2_WRITE         0x00000002UL  /* PAG_WRITE */


/*
 * VMMapDebugAlias flags.
 */
#define VMMDA_ARENAPRIVATE  0           /* Create alias in private arena */
#define VMMDA_ARENASYSTEM   4           /* Create alias in system arena */
#define VMMDA_READONLY      1           /* Create readonly alias */



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _vmac
{
    ULONG       ac_va;                  /* off=0 cb=4 Virtual Address. */
    USHORT      ac_sel;                 /* off=4 cb=2 Selector. */
    USHORT      ac_hob;                 /* off=6 cb=2 Object handle. */
} VMAC, *PVMAC;


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

APIRET KRNLCALL VMFreeMem(
    ULONG   ulAddress,
    HPTDA   hPTDA,
    ULONG   flFlags);

APIRET KRNLCALL VMMapDebugAlias(
    ULONG   flVMFlags,
    ULONG   ulAddress,
    ULONG   cbSize,
    HPTDA   hPTDA,
    PVMAC   pvmac);

APIRET KRNLCALL VMObjHandleInfo(
    USHORT  usHob,
    PULONG  pulAddr,
    PUSHORT pushPTDA);

#ifdef _OS2KLDR_H_
PMTE KRNLCALL VMPseudoHandleMap(
    HMTE    hMTE);
#endif

#endif
