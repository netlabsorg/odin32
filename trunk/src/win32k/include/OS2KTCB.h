/* $Id: OS2KTCB.h,v 1.1 2000-07-16 22:21:18 bird Exp $
 *
 * TCB - Thread Control Block access methods.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _tcb_h_
#define _tcb_h_

#ifdef __cplusplus
extern "C" {
#endif


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
