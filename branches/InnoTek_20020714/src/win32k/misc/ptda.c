/* $Id: ptda.c,v 1.3 2000-12-11 06:53:54 bird Exp $
 *
 * PTDA access functions.
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

