/* $Id: OS2KPTDA.h,v 1.3 2001-09-27 03:04:12 bird Exp $
 *
 * PTDA access functions.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _OS2KPTDA_h_
#define _OS2KPTDA_h_

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


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifndef KKRNLLIB
    #if defined(__IBMC__) || defined(__IBMCPP__)
        #pragma map(pPTDACur, "_pPTDACur")
    #else
        #pragma pPTDACur       _pPTDACur
    #endif
#endif
#ifdef KKRNLLIB
extern  PPPTDA      ppPTDACur;          /* Please access this only thru the ptdaGetCur() function. */
#else
extern  PPTDA       pPTDACur;           /* Please access this only thru the ptdaGetCur() function. */
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/**
 * Gets the current PTDA pointer.
 * @returns     Pointer to the current PTDA.
 * @remark      Using import stuff.
 */
//PPTDA   ptdaGetCur(void);
#ifdef KKRNLLIB
#define ptdaGetCur()                    (*ppPTDACur)
#else
#define ptdaGetCur()                    (pPTDACur)
#endif




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
