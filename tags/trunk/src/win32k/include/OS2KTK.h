/* $Id: OS2KTK.h,v 1.4 2000-12-11 06:53:50 bird Exp $
 *
 * OS2KTK - OS/2 Kernel Task.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _OS2KTK_h_
#define _OS2KTK_h_


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define TK_FUSU_FATAL       0x3         /* Traps the task on failure. */
#define TK_FUSU_NONFATAL    0x0         /* Returns error code to caller on failure. */
#define TK_FUSU_USER_SRC    0x10        /* Source buffer is in user memory. (TKSuFuBuff) */
#define TK_FUSU_USER_DST    0x20        /* Target buffer is in user memory. (TKSuFuBuff) */



#define TKFuChar(pch, pchUsr) \
            TKFuBuff((PVOID)pch, (PVOID)pchUsr, sizeof(char), TK_FUSU_FATAL)
#define TKSuChar(pchUsr, pch) \
            TKSuBuff((PVOID)pchUsr, (PVOID)pch, sizeof(char), TK_FUSU_FATAL)

#define TKFuCharNF(pch, pchUsr) \
            TKFuBuff((PVOID)pch, (PVOID)pchUsr, sizeof(char), TK_FUSU_NONFATAL)
#define TKSuCharNF(pchUsr, pch) \
            TKSuBuff((PVOID)pchUsr, (PVOID)pch, sizeof(char), TK_FUSU_NONFATAL)


#define TKFuUShort(pus, pusUsr) \
            TKFuBuff((PVOID)pus, (PVOID)pusUsr, sizeof(USHORT), TK_FUSU_FATAL)
#define TKSuUShort(pusUsr, pus) \
            TKSuBuff((PVOID)pusUsr, (PVOID)pus, sizeof(USHORT), TK_FUSU_FATAL)

#define TKFuUShortNF(pus, pusUsr) \
            TKFuBuff((PVOID)pus, (PVOID)pusUsr, sizeof(USHORT), TK_FUSU_NONFATAL)
#define TKSuUShortNF(pusUsr, pus) \
            TKSuBuff((PVOID)pusUsr, (PVOID)pus, sizeof(USHORT), TK_FUSU_NONFATAL)


#define TKFuULong(pul, pulUsr) \
            TKFuBuff((PVOID)pul, (PVOID)pulUsr, sizeof(ULONG), TK_FUSU_FATAL)
#define TKSuULong(pulUsr, pul) \
            TKSuBuff((PVOID)pulUsr, (PVOID)pul, sizeof(ULONG), TK_FUSU_FATAL)

#define TKFuULongNF(pul, pulUsr) \
            TKFuBuff((PVOID)pul, (PVOID)pulUsr, sizeof(ULONG), TK_FUSU_NONFATAL)
#define TKSuULongNF(pulUsr, pul) \
            TKSuBuff((PVOID)pulUsr, (PVOID)pul, sizeof(ULONG), TK_FUSU_NONFATAL)


#define TKFuStrLen(pcch, pvUsr, cchMax) \
            TKFuBufLen(pcch, pvUsr, cchMax, TK_FUSU_FATAL, FALSE)
#define TKFuStrLenNZ(pcch, pvUsr, cchMax) \
            TKFuBufLenNZ(pcch, pvUsr, cchMax, TK_FUSU_NONFATAL, FALSE)

#define TKFuStrLenZ(pcch, pvUsr, cchMax) \
            TKFuBufLenZ(pcch, pvUsr, cchMax, TK_FUSU_FATAL, TRUE)
#define TKFuStrLenZNZ(pcch, pvUsr, cchMax) \
            TKFuBufLenZNZ(pcch, pvUsr, cchMax, TK_FUSU_NONFATAL, TRUE)





/**
 * Copy user memory into system memory.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pv     Pointer to target (system) data area.
 * @param     pvUsr  Pointer to source (user) data area.
 * @param     cb     Count of bytes to copy.
 * @param     fl     Flags.
 */
extern ULONG KRNLCALL   TKFuBuff(PVOID pv, PVOID pvUsr, ULONG cb, ULONG fl);


/**
 * Copy system memory to user memory.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pvUsr  Pointer to target (user) data area.
 * @param     pv     Pointer to source (system) data area.
 * @param     cb     Count of bytes to copy.
 * @param     fl     Flags.
 */
extern ULONG KRNLCALL   TKSuBuff(PVOID pvUsr, PVOID pv, ULONG cb, ULONG fl);


/**
 * String length.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pcch      Pointer to length variable.
 * @param     pvUsr     Pointer to user data to preform string length on.
 * @param     cchMax    Max string length.
 * @param     fl        Flags.
 * @param     fDblNULL  TRUE:  Double '\0' (ie. '\0\0') terminated. Usefull for scanning environments.
 *                      FALSE: Single string. (ie. one '\0').
 */
extern ULONG KRNLCALL   TKFuBufLen(PLONG pcch, PVOID pvUsr, ULONG cchMax, ULONG fl, BOOL fDblNULL);


/**
 * Copy user memory to user memory.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pvTarget  Pointer to target (user) data area.
 * @param     pvSource  Pointer to source (user) data area.
 * @param     cb        Count of bytes to copy.
 * @param     fl        Flags.
 */
extern ULONG KRNLCALL   TKSuFuBuff(PVOID pvTarget, PVOID pvSource, ULONG cb, ULONG fl);


#ifdef INCL_OS2KRNL_PTDA
/**
 * PID to PTDA pointer.
 * @returns     OS/2 return code.
 * @param       pid     Process Identifier.
 * @param       ppPTDA  Pointer to the PTDA-pointer variabel which is to receive
 *                      the PTDA pointer on successful return.
 */
extern ULONG KRNLCALL  TKPidToPTDA(PID pid, PPPTDA ppPTDA);
#endif

#endif
