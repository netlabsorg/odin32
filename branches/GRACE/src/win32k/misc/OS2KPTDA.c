/* $Id: OS2KPTDA.c,v 1.1.2.1 2000-08-15 07:03:27 bird Exp $
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
extern ULONG    pptda_module;


/**
 * Gets the ptda_environ PTDA member. This member holds the memory object handle
 * for the environment block of the processes.
 * @returns     Content of the pPTDA->ptda_environ member.
 * @param       pPTDA   PTDA Pointer. (NULL is not allowed!)
 */
USHORT  ptdaGet_ptda_environ(PPTDA pPTDA)
{
    return *(PUSHORT)(void*)(((char*)(void*)pPTDA) + (pptda_environ - pptda_start));
}


/**
 * Gets the ptda_module PTDA member. This member holds the MTE handle of the process's
 * executable image.
 * @returns     Content of the pPTDA->ptda_module member.
 * @param       pPTDA   PTDA Pointer. (NULL is not allowed!)
 */
USHORT  ptdaGet_ptda_module(PPTDA pPTDA)
{
    return *(PUSHORT)(void*)(((char*)(void*)pPTDA) + (pptda_module - pptda_start));
}

