/* $Id: d32Win32kIOCtl.c,v 1.1.2.3 2002-04-01 09:57:11 bird Exp $
 *
 * Win32k driver IOCtl handler function.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: d32Win32kIOCtl.c,v 1.1.2.3 2002-04-01 09:57:11 bird Exp $";
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define MAX_PARAMSIZE   64              /* Maximum size of a parameter structure. */

#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_TK
#define INCL_KKL_LOG

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
 * @author    knut st. osmundsen (bird@anduin.net)
 */
USHORT _loadds _Far32 _Pascal Win32kIOCtl(PRP32GENIOCTL pRpIOCtl)
{
    KLOGENTRY1("USHORT","PRP32GENIOCTL pRpIOCtl", pRpIOCtl);
    /* validate parameter pointer */
    if (pRpIOCtl == NULL || pRpIOCtl->ParmPacket == NULL
        || pRpIOCtl->Function == 0 || pRpIOCtl->Function > K32_LASTIOCTLFUNCTION)
    {
        KLOGEXIT(STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER);
        return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
    }

    switch (pRpIOCtl->Category)
    {
        case IOCTL_W32K_K32:
            {
            APIRET rc = Win32kAPIRouter(pRpIOCtl->Function, pRpIOCtl->ParmPacket);
            if (    rc != 0xdeadbeefUL
                &&  TKSuULongNF(&((PK32HDR)pRpIOCtl->ParmPacket)->rc, SSToDS(&rc)) == NO_ERROR)
            {
                KLOGEXIT(STATUS_DONE);
                return STATUS_DONE;     /* Successfull return */
            }
            break;
            }
    }

    KLOGEXIT(STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER);
    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}
