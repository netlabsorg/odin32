/* $Id: OS2KPTDA.c,v 1.1 2000-07-16 22:21:17 bird Exp $
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
#include <OS2KPTDA.h>


/*******************************************************************************
*   External Data                                                              *
*******************************************************************************/
/*
 * Requires the following imports:
 *      pPTDACur
 *      ptda_start
 *      ptda_environ
 */
extern ULONG    pptda_start;
extern ULONG    pptda_environ;


/**
 * Gets the ptda_environ PTDA member. This member holds the memory object handle
 * for the environment block of the processes.
 * @returns     Content of the pPTDA->ptda_environ member.
 * @param       pPTDA   PTDA Pointer. (NULL is not allowed!)
 * @sketch
 */
USHORT  ptdaGet_ptda_environ(PPTDA pPTDA)
{
    return *(PUSHORT)(void*)(((char*)(void*)pPTDA) + (pptda_environ - pptda_start));
}

