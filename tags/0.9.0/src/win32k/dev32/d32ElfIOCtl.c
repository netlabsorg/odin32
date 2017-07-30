/* $Id: d32ElfIOCtl.c,v 1.3 2000-12-11 06:53:48 bird Exp $
 *
 * Elf driver IOCtl handler function.
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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"
#include "dev1632.h"
#include "dev32.h"



/**
 * IOCtl handler for the ELF part of the driver.
 * @returns   Gen IOCtl return code.
 * @param     pRpIOCtl  Pointer to 32-bit request packet. (not to the original packet)
 * @sketch
 * @status    stub.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
USHORT _loadds _Far32 _Pascal ElfIOCtl(PRP32GENIOCTL pRpIOCtl)
{
    pRpIOCtl = pRpIOCtl;
    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}

