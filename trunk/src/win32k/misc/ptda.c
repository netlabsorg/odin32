/* $Id: ptda.c,v 1.2 2000-04-17 02:26:29 bird Exp $
 *
 * PTDA access functions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "ptda.h"


PPTDA   ptdaGetCur(void)
{
    return NULL;
}


PPTDA   ptdaGet_pPTDAExecChild(PPTDA pPTDA)
{
    pPTDA = pPTDA;
    return NULL;
}


USHORT  ptdaGet_ptda_environ(PPTDA pPTDA)
{
    pPTDA = pPTDA;
    return 0;
}

