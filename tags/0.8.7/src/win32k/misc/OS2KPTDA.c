/* $Id: OS2KPTDA.c,v 1.5 2001-07-08 03:09:51 bird Exp $
 *
 * PTDA access functions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_PTDA

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>


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
extern ULONG    pptda_ptdasem;
extern ULONG    pptda_handle;
extern ULONG    pptda_module;
extern ULONG    pptda_pBeginLIBPATH;


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
 * Gets the ptda_handle PTDA member. This member holds the PTDA handle for the
 * given PTDA.
 * @returns     Content of the pPTDA->ptda_handle member.
 * @param       pPTDA   PTDA Pointer. (NULL is not allowed!)
 */
HPTDA  ptdaGet_ptda_handle(PPTDA pPTDA)
{
    return *(PHPTDA)(void*)(((char*)(void*)pPTDA) + (pptda_handle - pptda_start));
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


/**
 * Gets the ptda_ptdasem PTDA member. This member holds the intra-process semaphore which
 * for example is used to serialize _LDRQAppType.
 * @returns     Content of the pPTDA->ptda_ptdasem member.
 * @param       pPTDA   PTDA Pointer. (NULL is not allowed!)
 */
HKSEMMTX    ptda_ptda_ptdasem(PPTDA pPTDA)
{
    return (HKSEMMTX)(void*)(((char*)(void*)pPTDA) + (pptda_ptdasem - pptda_start));
}


/**
 * Gets the ptda_pBeginLIBPATH PTDA member. This member may hold the begin libpath
 * extention for this process.
 * @returns     beginlibpath string pointer.
 *              NULL if member is NULL or not supported by kernel.
 * @param       pPTDA   PTDA Pointer. (NULL is not allowed!)
 */
PSZ ptdaGet_ptda_pBeginLIBPATH(PPTDA pPTDA)
{
    if (!pptda_pBeginLIBPATH)
        return NULL;
    return *(PSZ*)(void*)(((char*)(void*)pPTDA) + (pptda_pBeginLIBPATH - pptda_start));
}


/**
 * Gets the ptda_pEndLIBPATH PTDA member. This member might hold the end libpath
 * extention for this process.
 * @returns     endlibpath string pointer.
 *              NULL if member is NULL or not supported by kernel.
 * @param       pPTDA   PTDA Pointer. (NULL is not allowed!)
 */
PSZ ptdaGet_ptda_pEndLIBPATH(PPTDA pPTDA)
{
    if (!pptda_pBeginLIBPATH)
        return NULL;
    return *(PSZ*)(void*)(((char*)(void*)pPTDA) + (pptda_pBeginLIBPATH + 4 - pptda_start));
}




