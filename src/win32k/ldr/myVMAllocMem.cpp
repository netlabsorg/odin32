/* $Id: myVMAllocMem.cpp,v 1.3.2.1 2001-09-27 03:08:25 bird Exp $
 *
 * Debug module - overloads VMAllocMem to analyse input parameters....
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_LDR
#define INCL_OS2KRNL_VM
#define INCL_KKL_AVL
#define INCL_KKL_LOG

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include "ldr.h"



/**
 *
 * @returns
 * @param     p1    dword ptr  ebp+8
 * @param     p2    dword ptr  ebp+0Ch
 * @param     p3    dword ptr  ebp+10h
 * @param     p4    word  ptr  ebp+14h
 * @param     p5    word  ptr  ebp+18h
 * @param     p6    word  ptr  ebp+1Ch
 * @param     p7    dword ptr  ebp+20h
 * @param     p8    dword ptr  ebp+24h
 * @param     p9    dword ptr  ebp+28h
 * @equiv
 * @time
 * @sketch
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
APIRET KRNLCALL myVMAllocMem(ULONG p1, ULONG p2, ULONG p3, USHORT p4, USHORT p5, USHORT p6, ULONG p7, ULONG p8, ULONG p9)
{
    APIRET rc;

    rc = VMAllocMem(p1,p2,p3,p4,p5,p6,p7,p8,(PVMAC)p9);

    return rc;
}

