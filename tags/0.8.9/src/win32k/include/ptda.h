/* $Id: ptda.h,v 1.3 2000-12-11 06:53:52 bird Exp $
 *
 * PTDA access functions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _ptda_h_
#define _ptda_h_


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _PTDA
{
    char dummy;
} PTDA, *PPTDA;

PPTDA   ptdaGetCur(void);
PPTDA   ptdaGet_pPTDAExecChild(PPTDA pPTDA);
USHORT  ptdaGet_ptda_environ(PPTDA pPTDA);

#endif
