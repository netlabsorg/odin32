/* $Id: probkrnl.h,v 1.6 2000-02-18 19:27:29 bird Exp $
 *
 * Include file for ProbKrnl.
 *
 * Copyright (c) 1998-2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _ProbKrnl_h_
#define _ProbKrnl_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define NUMBER_OF_PROCS      13
#define MAX_LENGTH_NAME      32

/* entry-point type flag */
#define EPT_PROC              0 /* procedure - overload procedure*/
#define EPT_PROCIMPORT        1 /* procedure - import only */
#define EPT_VAR               2 /* variable/non-procedure - not implemented yet */


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)
typedef struct tagPROCS
{
   unsigned char       fFound;
   unsigned char       iObject;
   unsigned short int  cchName;
   unsigned char       achName[MAX_LENGTH_NAME];
   unsigned long  int  offObject;
   unsigned long  int  ulAddress;
   unsigned char       fType;
} PROCS;
#pragma pack()



/*******************************************************************************
*   Global Variables                                                           *
*   NOTE! These are only available at init time!                               *
*******************************************************************************/
extern PROCS _aProcTab[NUMBER_OF_PROCS];    /* 'aProcTab'        in PrbKrnl.c */
extern unsigned long int  _ulBuild;         /* 'ulBuild'         in PrbKrnl.c */
extern unsigned short int _usVerMajor;      /* 'usVerMajor'      in PrbKrnl.c */
extern unsigned short int _usVerMinor;      /* 'usVerMinor'      in PrbKrnl.c */

#ifdef INCL_16 /* 16-bit only */
int ProbeKernel(PRPINITIN pReqPack);
#endif

#endif

