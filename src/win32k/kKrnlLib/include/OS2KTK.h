/* $Id: OS2KTK.h,v 1.2 2001-09-26 03:52:37 bird Exp $
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

#ifdef __cplusplus
extern "C" {
#endif


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


/*
 * Scan tasks flags. (Very similar to POST_SIGNAL).
 */
#define TKST_COMMANDTREE                0
#define TKST_PROCESS                    1
#define TKST_DESCENDANTS                2
#define TKST_SCREENGROUP                3
#define TKST_ALL                        4

#define TKST_ERR_WORKER_STOP            0x00000100
#define TKST_ERR_WORKER_RET             0x00000200
#define TKST_INCLUDE_EXECCHILD          0x00000400
#define TKST_INCLUDE_ZOMBIES            0x00000800


/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/

/**
 * Copy user memory into system memory.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pv     Pointer to target (system) data area.
 * @param     pvUsr  Pointer to source (user) data area.
 * @param     cb     Count of bytes to copy.
 * @param     fl     Flags.
 */
extern ULONG KRNLCALL   TKFuBuff(PVOID pv, PVOID pvUsr, ULONG cb, ULONG fl);
extern ULONG KRNLCALL   OrgTKFuBuff(PVOID pv, PVOID pvUsr, ULONG cb, ULONG fl);


/**
 * Copy system memory to user memory.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pvUsr  Pointer to target (user) data area.
 * @param     pv     Pointer to source (system) data area.
 * @param     cb     Count of bytes to copy.
 * @param     fl     Flags.
 */
extern ULONG KRNLCALL   TKSuBuff(PVOID pvUsr, PVOID pv, ULONG cb, ULONG fl);
extern ULONG KRNLCALL   OrgTKSuBuff(PVOID pvUsr, PVOID pv, ULONG cb, ULONG fl);


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
extern ULONG KRNLCALL   OrgTKFuBufLen(PLONG pcch, PVOID pvUsr, ULONG cchMax, ULONG fl, BOOL fDblNULL);


/**
 * Copy user memory to user memory.
 * @returns   OS/2 return code. (NO_ERROR is success)
 * @param     pvTarget  Pointer to target (user) data area.
 * @param     pvSource  Pointer to source (user) data area.
 * @param     cb        Count of bytes to copy.
 * @param     fl        Flags.
 */
extern ULONG KRNLCALL   TKSuFuBuff(PVOID pvTarget, PVOID pvSource, ULONG cb, ULONG fl);
extern ULONG KRNLCALL   OrgTKSuFuBuff(PVOID pvTarget, PVOID pvSource, ULONG cb, ULONG fl);


#ifdef INCL_OS2KRNL_PTDA
/**
 * PID to PTDA pointer.
 * @returns     OS/2 return code.
 * @param       pid     Process Identifier.
 * @param       ppPTDA  Pointer to the PTDA-pointer variabel which is to receive
 *                      the PTDA pointer on successful return.
 */
extern ULONG KRNLCALL   TKPidToPTDA(PID pid, PPPTDA ppPTDA);
extern ULONG KRNLCALL   OrgTKPidToPTDA(PID pid, PPPTDA ppPTDA);
#endif


#ifdef INCL_OS2KRNL_PTDA
/**
 * Worker prototype.
 */
typedef ULONG (KRNLCALL TKSCANTASKWORKER)(PPTDA, ULONG);
typedef TKSCANTASKWORKER * PTKSCANTASKWORKER;

/**
 * Scans tasks (ie. processes) in the system calling the working for each which matches the
 * flFlags and id combination.
 * @return  NO_ERROR on success.
 *          On error, depends on flags, but appropriate error code.
 * @param   flFlags     TKST_* flags.
 * @param   id          Process id og screen group id.
 * @param   pfnWorker   Pointer (FLAT) to the worker procesdure.
 * @param   ulArg       User argument which is passed on as second parameter to worker
 */
extern ULONG KRNLCALL   TKScanTasks(ULONG flFlags, ULONG id, PTKSCANTASKWORKER pfnWorker, ULONG ulArg);
extern ULONG KRNLCALL   OrgTKScanTasks(ULONG flFlags, ULONG id, PTKSCANTASKWORKER pfnWorker, ULONG ulArg);
#endif


/**
 * Post signal to one or more processes.
 * @returns NO_ERROR on success.
 *          On error ERROR_NOT_DESCENDANT, ERROR_SIGNAL_REFUSED,
 *          ERROR_INVALID_PROCID, ERROR_ZOMBIE_PROCESS, ERROR_SIGNAL_PENDING. (it seems)
 * @param   usSignal       Signal number.
 * @param   usAction       Action.
 *                         0 - the process and all children.
 *                         1 - only the process
 *                         2 - the process and all it's descendants.
 *                         3 - all processes in that screen group.
 * @param   usSignalArg    Signal argument.
 * @param   usPIDSGR       Process Id or Screen Group Id.
 * @remark  Win32k Internal call.
 */
extern ULONG _Optlink POST_SIGNAL32(USHORT usSignal, USHORT usAction, USHORT usSignalArg, USHORT usPIDSGR);

#ifdef __cplusplus
}
#endif


#ifdef INCL_OS2KRNL_TCB
/**
 * Force a thread to be made ready.
 * @param   flFlag  Which flag(s?) to set.
 * @param   pTCB    Pointer to the thread control block of the thread to be forced.
 */
extern void KRNLCALL TKForceThread(ULONG flFlag, PTCB pTCB);
extern void KRNLCALL OrgTKForceThread(ULONG flFlag, PTCB pTCB);
#endif

#ifdef INCL_OS2KRNL_PTDA
/**
 * Set force flag on a task.
 * @param   flFlag  Which flag(s?) to set.
 * @param   pPTDA   Pointer to the PTDA of the task to be processed.
 * @param   fForce  FALSE   Just set the flag on all threads.
 *                  TRUE    Force all threads ready by calling TKForceThread.
 */
extern void KRNLCALL  TKForceTask(ULONG flFlag, PPTDA pPTDA, BOOL fForce);
extern void KRNLCALL  OrgTKForceTask(ULONG flFlag, PPTDA pPTDA, BOOL fForce);
#endif

#ifdef INCL_OS2KRNL_TCB
/**
 * Get priotity of a thread.
 * @returns Thread priority.
 * @param   pTCB    pointer to the TCB of the thread in question.
 */
extern ULONG KRNLCALL TKGetPriority(PTCB pTCB);
extern ULONG KRNLCALL OrgTKGetPriority(PTCB pTCB);
#endif

/**
 * Make current thread sleep.
 * @returns NO_ERROR on success.
 *          ERROR_INTERRUPT if a signal is forced on the thread.
 *          ERROR_TIMEOUT if we timeout.
 * @param   ulSleepId   32-bit sleep id which TKWakeup will be called with.
 * @param   ulTimeout   Number of milliseconds to sleep. (0 is not yield)
 *                      -1 means forever or till wakeup.
 * @param   fUnint      TRUE:  may not interrupt sleep.
 *                      FALSE: may interrupt sleep.
 * @param   flSleepType ???
 */
extern ULONG KRNLCALL TKSleep(ULONG ulSleepId, ULONG ulTimeout, ULONG fUnInterruptable, ULONG flWakeupType);
extern ULONG KRNLCALL OrgTKSleep(ULONG ulSleepId, ULONG ulTimeout, ULONG fUnInterruptable, ULONG flWakeupType);

/**
 * Wakeup sleeping thread(s).
 * @returns NO_ERROR on succes.
 *          Appropriate error code on failure.
 * @param   ulSleepId       32-bit sleep id which threads are sleeping on.
 * @param   flWakeUpType    How/what to wakeup.
 * @param   pcWakedUp       Pointer to variable which is to hold the count of
 *                          thread waked up.
 */
extern ULONG KRNLCALL TKWakeup(ULONG ulSleepId, ULONG flWakeupType, PULONG cWakedUp);
extern ULONG KRNLCALL OrgTKWakeup(ULONG ulSleepId, ULONG flWakeupType, PULONG cWakedUp);

#ifdef INCL_OS2KRNL_TCB
/**
 * Wake up a single thread.
 * @returns NO_ERROR on succes.
 *          Appropriate error code on failure.
 * @param   pTCB    Pointer to the TCB of the thread to be waken.
 */
extern ULONG KRNLCALL TKWakeThread(PTCB pTCB);
extern ULONG KRNLCALL OrgTKWakeThread(PTCB pTCB);
#endif

#ifdef INCL_OS2KRNL_TCB
/**
 * See which thread will be Wakeup.
 * @returns Pointer to TCB of the thread on success.
 *          NULL on failure or no threads.
 * @param   ulSleepId       32-bit sleep id which threads are sleeping on.
 * @param   flWakeUpType    How/what to wakeup.
 */
extern PTCB  KRNLCALL TKQueryWakeup(ULONG ulSleepId, ULONG flWakeupType);
extern PTCB  KRNLCALL OrgTKQueryWakeup(ULONG ulSleepId, ULONG flWakeupType);
#endif

#endif
