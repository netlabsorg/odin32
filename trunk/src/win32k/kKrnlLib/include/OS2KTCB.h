/* $Id: OS2KTCB.h,v 1.1 2001-09-14 01:50:16 bird Exp $
 *
 * TCB - Thread Control Block access methods.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _OS2KTCB_h_
#define _OS2KTCB_h_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * sg24-4640-00:    TCBForcedActions flag definitions:
 */
#define TK_FF_BUF           0x00000001  /* Buffer must be released */
#define TK_FF_EXIT          0x00000002  /* Call TKExit (old FF_DES) */
#define TK_FF_CRITSEC       0x00000004  /* Enter Per-task critical section */
#define TK_FF_ICE           0x00000008  /* Freeze thread */
#define TK_FF_NPX           0x00000010  /* NPX Error */
#define TK_FF_TIB           0x00000020  /* Update the TIB */
#define TK_FF_TRC           0x00000040  /* Enter Debug */
#define TK_FF_SIG           0x00000080  /* Signal pending */
#define TK_FF_CTXH          0x00000100  /* Pending local context hooks */
#define TK_FF_STIH          0x00000200  /* Execute STI hooks */
#define TK_FF_VDMBP         0x00000400  /* Execute VDM BP hooks */
#define TK_FF_RTRY          0x00000800  /* Retry V86 system call */
#define TK_FF_PIB           0x00001000  /* Update the PIB */
#define TK_FF_SCH           0x00002000  /* Do Scheuler Processing */
#define TK_FF_TFBIT         0x00004000  /* Validate user eflags TF bit */
#define TK_FF_TIBPRI        0x00008000  /* Update only the priority fields in TIB */


/*
 * sg24-4640-00:    TCBEntryActions flag definitions:
 */
#define TK_EF_PFCLI             1       /* Page fault inside CLI */
#define TK_EF_TRC               2       /* DosDebug action pending */


/*
 * sg24-4640-00: TCBWakeFlags flag definitions:
 */
#define TK_WF_INTERRUPTED       0x01    /* Sleep was interrupted */
#define TK_WF_TIMEEXP           0x02    /* Timeout expired */
#define TK_WF_INTPENDING        0x04    /* Interrupt pending */
#define TK_WF_SINGLEWAKEUP      0x08    /* Thread wants single wakeup */
#define TK_WF_INTERRUPTIBLE     0x10    /* Thread blocked interruptibly */
#define TK_WF_TIMEOUT           0x20    /* Thread blocked with timeout */
#define TK_WF_SLEEPING          0x40    /* In TKSleep() */


/*
 * sg24-4640-00: TCBState and TCBQState definitions:
 */
#define STATE_VOID              0       /* Uninitialized */
#define STATE_READY             1       /* Ready to run */
#define STATE_BLOCKED           2       /* Blocked on an ID */
#define STATE_SUSPENDED         3       /* Suspended (DosSuspendThread) */
#define STATE_CRITSEC           4       /* Blocked by another CritSec thread */
#define STATE_RUNNING           5       /* Thread currently running */
#define STATE_READYBOOST        6       /* Ready, but apply an IO boost */
#define STATE_TSD               7       /* Thread waiting for TSD */
#define STATE_DELAYED           8       /* Delayed TKWakeup (Almost Ready) */
#define STATE_FROZEN            9       /* Frozen Thread (FF_ICE) */
#define STATE_GETSTACK          10      /* Incomming TSD */
#define STATE_BADSTACK          11      /* TSD failed to swap in */


/*
 * sg24-4640-00: TCBPriClassMod definitions:
 */
#define CLASSMOD_KEYBOARD       0x04    /* Keyboard boost */
#define CLASSMOD_STARVED        0x08    /* Starvation boost */
#define CLASSMOD_DEVICE         0x10    /* Device I/O Done Boost */
#define CLASSMOD_FOREGROUND     0x20    /* Foreground boost */
#define CLASSMOD_WINDOW         0x40    /* Window Boost */
#define CLASSMOD_VDM_INTERRUPT  0x80    /* VDM simulated interrupt boost */


/*
 * sg24-4640-00: TCBPriClass definitions:
 */
#define CLASS_NOCHANGE          0x00    /* No priority class change */
#define CLASS_IDLE_TIME         0x01    /* Idle - Time class */
#define CLASS_REGULAR           0x02    /* Regular class */
#define CLASS_TIME_CRITICAL     0x03    /* Time-Critical class */
#define CLASS_SERVER            0x04    /* Client-Server Server class */


/*
 * sg24-4640-00: TCBSchFlg flag definitions:
 */
#define SCH_PROTECTED_PRI       0x0001  /* Only Intra-process SetPri allowed */
#define SCH_WINDOWBOOST_LOCK    0x0002  /* Lock out windoboost changes */
#define SCH_MINSLICE            0x0004  /* Use minimum timeslice */
#define SCH_PAGE_FAULT          0x0008  /* Dynamic timeslicing ### */
#define SCH_PAGE_FAULT_BIT      0x03    /* Dynamic timeslicing P728371 */


/*
 * sg24-4640-00: TCBfSwapping flag definitions:
 */
#define SM_TCB_SWAPPING         0x01    /* swap i/o underway */
#define SM_TCB_RESIZING         0x02    /* data structures are growing */


/*
 * sg24-4640-00: TCBMiscFlags flag definitions:
 */
#define TMF_CMapFailed          (0x01)  /* Set if alloc/realloc failed on a cluster */
                                        /*  map (mft_selCMap). */
#define TMF_IGNORE_HE           (0x02)  /* If set, ignore (auto fail) hard error */
#define TMF_MULT_XCPT           (0x04)  /* Set if multiple ring 0 exceptions */
#define TMF_NoFwd               (0x08)  /* Set if inhibiting forwarders */
#define TMF_EXIT_TERM           (0x10)  /* TK_FF_EXIT means TKTermThread */
#define TMF_NO_EXCEPT           (0x20)  /* Indicates TIB exception field invalid */
#define TMF_XCPT_HE             (0x40)  /* Indicates an exception harderr is */
                                        /* pending */


/*
 * sg24-4640-00: TCBMSpareFlags flag definitions:
 */
#define SPFLAGS_FGND_DISKIO     0x0080  /* Foreground Disk I/O */


/*
 * sg24-4640-00: TCBReqPktFlg flag definitions:
 */
#define TK_RP_ALLOCATED         0x01
#define TK_RP_INUSE             0x02



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _TCB
{
    char    dummy;
} TCB, *PTCB, **PPTCB;

extern PPTCB    ppTCBCur;


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/**
 * Gets pointer to the current TCB.
 * @returns     Pointer to the current thread control block (TCB).
 *              NULL if not task-time.
 */
#define tcbGetCur()     *ppTCBCur


USHORT  tcbGetTCBFailErr(PTCB pTCB);
USHORT  tcbSetTCBFailErr(PTCB pTCB, USHORT TCBFailErr);



#ifdef __cplusplus
}
#endif

#endif
