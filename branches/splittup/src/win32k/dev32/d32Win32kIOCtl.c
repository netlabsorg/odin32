/* $Id: d32Win32kIOCtl.c,v 1.7.2.1 2001-09-27 03:08:13 bird Exp $
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
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include "devSegDf.h"
#include "dev1632.h"
#include "dev32.h"
#include "Win32k.h"
#include "k32.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
APIRET _Optlink Win32kAPIRouter(ULONG ulFunction, PVOID pvParam);  /* implemented in d32CallGate.asm. */


/**
 * IOCtl handler for the Win32k part of the driver.
 * @returns   Gen IOCtl return code.
 * @param     pRpIOCtl  Pointer to 32-bit request packet. (not to the original packet)
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
USHORT _loadds _Far32 _Pascal Win32kIOCtl(PRP32GENIOCTL pRpIOCtl)
{
    /* validate parameter pointer */
    if (pRpIOCtl == NULL || pRpIOCtl->ParmPacket == NULL
        || pRpIOCtl->Function == 0 || pRpIOCtl->Function > K32_LASTIOCTLFUNCTION)
        return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;

    switch (pRpIOCtl->Category)
    {
        case IOCTL_W32K_K32:
            {
            APIRET rc = Win32kAPIRouter(pRpIOCtl->Function, pRpIOCtl->ParmPacket);
            if (    rc != 0xdeadbeefUL
                &&  TKSuULongNF(&((PK32HDR)pRpIOCtl->ParmPacket)->rc, SSToDS(&rc)) == NO_ERROR)
                return STATUS_DONE;     /* Successfull return */
            break;
            }
    }

    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}
