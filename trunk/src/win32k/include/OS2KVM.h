/* $Id: OS2KVM.h,v 1.2 2000-02-15 23:39:19 bird Exp $
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
#define VMA_CONTIG          0x00000001 /* VMDHA_CONTIG */
#define VMA_WRITE           0x00000002 /* PAG_WRITE and OB_WRITE */
#define VMA_USER            0x00000004 /* OB_USER */
#define VMA_EXECUTE         0x00000008 /* PAG_EXECUTE and OB_READ !! */
#define VMA_READ            0x00000010 /* PAG_READ and OB_EXEC !! */
#define VMA_LOWMEM          0x00000020 /* OB_LOWMEM */
#define VMA_GUARD           0x00000040 /* PAG_GUARD and VMDHA_16M and OB_GUARD */
#define VMA_RESIDENT        0x00000080 /* OB_RESIDENT */
#define VMA_ZEROINIT        0x00000100 /* OB_ZEROINIT */
#define VMA_PHYS            0x00000200 /* VMDHA_PHYS */
#define VMA_FIXED           0x00000400 /* VMDHA_FIXED and OB_SHARED */
#define VMA_SHARE           0x00001000 /* OBJSHARE and OBJEXEC (which implies shared) */
#define VMA_PROTECTED       0x00004000 /* PAG_PROTECTED */
#define VMA_LOWMEM2         0x00010000 /* OB_LOWMEM */
#define VMA_DECOMMIT        0x00080000 /* PAG_DECOMMIT */
#define VMA_TILE            0x00400000 /* OBJ_TILE */
#define VMA_SHRINKABLE      0x00800000 /* OB_SHRINKABLE */
#define VMA_HUGH            0x01000000 /* OB_HUGH */


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



/*******************************************************************************
*   Exported Functions                                                         *
*******************************************************************************/
APIRET KRNLCALL VMAllocMem(
    ULONG   cbSize,
    ULONG   SomeArg1,
    ULONG   flFlags1,
    USHORT  hPTDA,
    USHORT  usVMOwnerId,
    USHORT  hMTE,
    ULONG   flFlags2,
    ULONG   SomeArg2,
    PVMAC   pvmac);



#endif
