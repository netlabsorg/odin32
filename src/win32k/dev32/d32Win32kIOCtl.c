/* $Id: d32Win32kIOCtl.c,v 1.6 2001-02-20 05:02:40 bird Exp $
 *
 * Win32k driver IOCtl handler function.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define MAX_PARAMSIZE   64              /* Maximum size of a parameter structure. */

#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_TK

#define NO_WIN32K_LIB_FUNCTIONS


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"
#include "dev1632.h"
#include "dev32.h"
#include "OS2Krnl.h"
#include "Win32k.h"
#include "k32.h"
#include "log.h"
#include "asmutils.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
APIRET _Optlink Win32kAPIRouter(ULONG ulFunction, PVOID pvParam);  /* called from d32CallGate.asm too. */



/**
 * IOCtl handler for the Win32k part of the driver.
 * @returns   Gen IOCtl return code.
 * @param     pRpIOCtl  Pointer to 32-bit request packet. (not to the original packet)
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
USHORT _loadds _Far32 _Pascal Win32kIOCtl(PRP32GENIOCTL pRpIOCtl)
{
    /* validate parameter pointer */
    if (pRpIOCtl == NULL || pRpIOCtl->ParmPacket == NULL || pRpIOCtl->Function == 0 || pRpIOCtl->Function > K32_LASTIOCTLFUNCTION)
        return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;

    switch (pRpIOCtl->Category)
    {
        case IOCTL_W32K_K32:
            {
            APIRET rc = Win32kAPIRouter(pRpIOCtl->Function, pRpIOCtl->ParmPacket);
            if (    rc != 0xdeadbeefUL
                &&  TKSuULongNF(pRpIOCtl->ParmPacket, SSToDS(&rc)) == NO_ERROR)
                return STATUS_DONE;     /* Successfull return */
            break;
            }
    }

    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}


/**
 * Internal function router which calls the correct function.
 * Called from IOCtl worker and callgate in d32CallGate.asm.
 * @returns function return code.
 *          0xdeadbeef if invalid function number.
 * @param   ulFunction  Function number to call.
 * @param   pvParam     Parameter package for that function.
 * @sketch
 * @status  partially implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  This could be reimplemented in assembly.
 *          Make generic parameter layout to limit amount of memory copied back.
 */
APIRET _Optlink Win32kAPIRouter(ULONG ulFunction, PVOID pvParam)
{
    static ULONG acbParams[] =
    {
        0,                              /* Not used - ie. invalid */
        sizeof(K32ALLOCMEMEX),          /* K32_ALLOCMEMEX          0x01 */
        sizeof(K32QUERYOTES),           /* K32_QUERYOTES           0x02 */
        sizeof(K32QUERYOPTIONSSTATUS),  /* K32_QUERYOPTIONSSTATUS  0x03 */
        sizeof(K32SETOPTIONS),          /* K32_SETOPTIONS          0x04 */
        sizeof(K32PROCESSREADWRITE),    /* K32_PROCESSREADWRITE    0x05 */
        sizeof(K32HANDLESYSTEMEVENT),   /* K32_HANDLESYSTEMEVENT   0x06 */
        sizeof(K32QUERYSYSTEMMEMINFO)   /* K32_QUERYSYSTEMMEMINFO  0x07 */
    };
    APIRET  rc;
    char    achBuffer[MAX_PARAMSIZE];
    PVOID   pv = SSToDS(&achBuffer[0]);

    /*
     * Validate the function number.
     * Fetch parameters from user buffer onto our Ring-0 stack.
     */
    if (ulFunction == 0 || ulFunction > sizeof(acbParams) / sizeof(acbParams[0]))
        return 0xdeadbeaf;
    rc = TKFuBuff(pv, pvParam, acbParams[ulFunction], TK_FUSU_NONFATAL);
    if (rc)
    {
        kprintf(("Win32kAPIRouter: Failed to fetch user parameters rc=%d.\n", rc));
        return rc;                      /* This can't happen when called from by IOCtl (I hope). */
    }


    /*
     * Call the actual function.
     */
    switch (ulFunction)
    {
        case K32_ALLOCMEMEX:
        {
            PK32ALLOCMEMEX pParm = (PK32ALLOCMEMEX)pv;
            return k32AllocMemEx(pParm->ppv, pParm->cb, pParm->flFlags, pParm->ulCS, pParm->ulEIP);
        }

        case K32_QUERYOTES:
        {
            PK32QUERYOTES pParm = (PK32QUERYOTES)pv;
            return k32QueryOTEs((HMTE)pParm->hMTE, pParm->pQOte, pParm->cbQOte);
        }

        case K32_QUERYOPTIONSSTATUS:
        {
            PK32QUERYOPTIONSSTATUS pParm = (PK32QUERYOPTIONSSTATUS)pv;
            return k32QueryOptionsStatus(pParm->pOptions, pParm->pStatus);
        }

        case K32_SETOPTIONS:
        {
            PK32SETOPTIONS pParm = (PK32SETOPTIONS)pv;
            return k32SetOptions(pParm->pOptions);
        }

        case K32_PROCESSREADWRITE:
        {
            PK32PROCESSREADWRITE pParm = (PK32PROCESSREADWRITE)pv;
            return k32ProcessReadWrite(pParm->pid, pParm->cb, pParm->pvSource, pParm->pvTarget, pParm->fRead);
        }

        case K32_HANDLESYSTEMEVENT:
        {
            //PK32HANDLESYSTEMEVENT pParm = (PK32HANDLESYSTEMEVENT)pv;
            //return k32HandleSystemEvent(pParm->ulEvent, pParm->hev, pParm->fHandle);
            return ERROR_NOT_SUPPORTED;
        }

        case K32_QUERYSYSTEMMEMINFO:
        {
            PK32QUERYSYSTEMMEMINFO pParm = (PK32QUERYSYSTEMMEMINFO)pv;
            return k32QuerySystemMemInfo(pParm->pMemInfo);
        }
    }


    /*
     * This will never happen.
     */
    kprintf(("Win32kAPIRouter: Internal processing error\n"));
    Int3();

    return 0xdeadbeaf;
}
