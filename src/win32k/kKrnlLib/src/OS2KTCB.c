/* $Id: OS2KTCB.c,v 1.3 2002-04-01 13:51:17 bird Exp $
 *
 * TCB - Thread Control Block access methods.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2KTCB.h>
#include "kKLKernel.h"


/*******************************************************************************
*   External Data                                                              *
*******************************************************************************/
/*
 * Requires the following import(s):
 *      pTCBCur
 */


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static BOOL     fInited;
#if 0
typedef
{
    short   sOffset;

}
#endif
static int      offTCBFailErr = 0;
//static int      offTCB



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static  BOOL initTCB(void);


/**
 * Init the TCB offset(s).
 * @returns     Success indicator. (TRUE = success; FALSE = failure)
 */
static BOOL initTCB(void)
{
    /*
     * !!IMPORTANT!!
     * Please note that when we don't define top limits here. New fixpack may have
     * other values. But since they very seldom do, we'll gamble on no change!
     * (Applies to Warp 3 and WS4eB)
     * !!IMPORTANT!!
     *
     * These values are derived from the kernel[d].SDFs and SG24-4640-00.
     */

    /* WS4eB GA and above. */
    if (ulKernelBuild >= 14039)
    {
        offTCBFailErr = isSMPKernel() ? 0x1fa : 0x1ea;
    }
    else /* Warp 4 GA - fp12 */
        if (ulKernelBuild >= 9023 && ulKernelBuild <= 9036)
    {
        offTCBFailErr = 0x18e;
    }
    else /* Warp 3 fp32 - fp62. */
        if (ulKernelBuild >= 8255 && ulKernelBuild <= 8285)
    {
        offTCBFailErr = isSMPKernel() ? 0x1ba : 0x14e;
    }
    else
        return FALSE;
    return TRUE;
}



/**
 * Get the content of the TCB data member TCBFailErr.
 * Intended used to save and restore the value in ldrOpenPath.
 * @returns     Value of TCBFaileErr. (0xffff on error)
 * @param       pTCB    Pointer to TCB.
 * @remark      Only known kernels are supported!
 */
USHORT  tcbGetTCBFailErr(PTCB pTCB)
{
    if (offTCBFailErr == 0 || !initTCB())
        return 0xffff;

    return *(PUSHORT)(void*)((char*)(void*)pTCB + offTCBFailErr);
}



/**
 * Set the content of the TCB data member TCBFailErr.
 * Intended used to save and restore the value in ldrOpenPath.
 * @returns     New Value of TCBFaileErr. (0xffff on error)
 * @param       pTCB            Pointer to TCB.
 * @param       TCBFAileErr     New value. (or rather the old one in the save-restore case..)
 * @remark      Only known kernels are supported!
 */
USHORT  tcbSetTCBFailErr(PTCB pTCB, USHORT TCBFailErr)
{
    if (offTCBFailErr == 0 || !initTCB())
        return 0xffff;

    return *(PUSHORT)(void*)((char*)(void*)pTCB + offTCBFailErr) = TCBFailErr;
}


