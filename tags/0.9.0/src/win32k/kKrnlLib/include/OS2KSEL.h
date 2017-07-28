/* $Id: OS2KSEL.h,v 1.1 2001-09-26 03:52:36 bird Exp $
 *
 * Selector Manager prototypes, structures and definitions.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _OS2KSEL_h_
#define _OS2KSEL_h_


/**
 * Converts 16:32 address to FLAT 32-bits address.
 * @returns Flat 32-bits address.
 *          -1 on error.
 * @param   ulOffset    Offset.
 * @param   usSel       Selector.
 */
extern PVOID KRNLCALL   SELVirtToLin(ULONG ulOffset, USHORT usSel);
extern PVOID KRNLCALL   OrgSELVirtToLin(ULONG ulOffset, USHORT usSel);


/**
 * Converts selector to linar address.
 * @returns Flat 32-bits address.
 *          -1 on error.
 * @param   usSel       Selector.
 */
extern PVOID KRNLCALL   SELConvertToLinear(USHORT usSel, PPTDA pPTDA);
extern PVOID KRNLCALL   OrgSELConvertToLinear(USHORT usSel, PPTDA pPTDA);


/**
 * Converts a 32-bit flat pointer to an selector if possible.
 * @returns Selector (eax), offset (edx).
 * @param   pv      Flat 32-bit address.
 * @param   ulRPL   Requested privilege level of the selector.
 * @param   hPTDA   PTDA handle. NULLHANDLE means current PTDA.
 */
extern USHORT KRNLCALL  SELConvertToSelector(PVOID pv, ULONG ulRPL, HPTDA hPTDA);
extern USHORT KRNLCALL  OrgSELConvertToSelector(PVOID pv, ULONG ulRPL, HPTDA hPTDA);


/**
 * Allocates a GDT selector.
 * @returns OS2 return code. (I.e. NO_ERROR on success.)
 * @param   pusSel  Where to put the selector on successful return.
 */
extern ULONG  KRNLCALL  SELAllocGDT(PUSHORT pusSel);
extern ULONG  KRNLCALL  OrgSELAllocGDT(PUSHORT pusSel);


#endif

