/* $Id: k32AllocMemEx.cpp,v 1.1 2000-02-15 23:39:58 bird Exp $
 *
 * k32AllocMemEx - Equivalent to DosAllocMem, but this one
 *                 uses the address in ppv.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS
#define INCL_OS2KRNL_VM


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "OS2Krnl.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"
#include "log.h"




/**
 * Extended edition of DosAllocMem. Allows you to suggest an address where you
 * want the memory allocated.
 * @returns   OS2 returncode.
 * @param     ppv    Pointer to a pointer. This points to a pointer which points to the
 *                   suggested address of the memory block.
 * @param     cb     Size of the memory block.
 * @param     flag   Flags just as for DosAllocMem.
 * @param     ulCS   CS where the memoryobject were requested.
 * @param     ulEIP  EIP where the memoryobject were requested.
 * @sketch
 * @status    stub
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
APIRET k32AllocMemEx(PPVOID ppv, ULONG cb, ULONG flag, ULONG ulCS, ULONG ulEIP)
{

    ppv = ppv;
    cb = cb;
    flag = flag;
    ulCS = ulCS;
    ulEIP = ulEIP;

    return ERROR_NOT_SUPPORTED;
}

