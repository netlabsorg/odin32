/* $Id: OS2KPTDA.h,v 1.5 2001-07-08 03:06:25 bird Exp $
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

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _PTDA
{
    struct _PTDA    *p1;
    struct _PTDA    *p2;
    struct _PTDA    *p3;
    struct _PTDA    *pPTDAExecChild;    /* This offset is fixed for all supported OS/2 version. */
} PTDA, *PPTDA, **PPPTDA;


extern  PPPTDA      ppPTDACur;          /* Please access this only thru the ptdaGetCur() function. */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/**
 * Gets the current PTDA pointer.
 * @returns     Pointer to the current PTDA.
 * @remark      Using import stuff.
 */
//PPTDA   ptdaGetCur(void);
#define ptdaGetCur()                    (*ppPTDACur)



/**
 * Gets the pPTDAExecChild member of the PTDA.
 * @returns     Pointer to child being started. NULL if none.
 * @param       pPTDA   Pointer to the ptda structure.
 * @remark      This member is fixed for all the supported kernels (all 2.x kernels too?).
 */
//PPTDA   ptdaGet_pPTDAExecChild(PPTDA pPTDA);
#define ptdaGet_pPTDAExecChild(pPTDA)   ((PPTDA)(pPTDA))->pPTDAExecChild


USHORT      ptdaGet_ptda_environ(PPTDA pPTDA);
HPTDA       ptdaGet_ptda_handle(PPTDA pPTDA);
USHORT      ptdaGet_ptda_module(PPTDA pPTDA);
HKSEMMTX    ptda_ptda_ptdasem(PPTDA pPTDA);
PSZ         ptdaGet_ptda_pBeginLIBPATH(PPTDA pPTDA);
PSZ         ptdaGet_ptda_pEndLIBPATH(PPTDA pPTDA);

#ifdef __cplusplus
}
#endif

#endif
