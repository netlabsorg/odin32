/* $Id: probkrnl.h,v 1.3 1999-11-10 01:45:33 bird Exp $
 *
 * Include file for ProbKrnl.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _ProbKrnl_h_
#define _ProbKrnl_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define NUMBER_OF_PROCS 4   /* remeber to sync this with ldr_functions.h and dev32_start.asm */

/* indexes into the _ProcTab/ProcTab table */
#define iLDRREAD              0
#define iLDROPEN              1
#define iLDRCLOSE             2
#define iLDRQAPPTYPE          3

#define MAX_LENGTH_NAME      32

/* entry-point type flag */
#define EPT_PROC              0 /* procedure */
#define EPT_VAR               1 /* variable/non-procedure */


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
extern unsigned long int _ulBuild;          /* 'ulBuild'         in PrbKrnl.c */
extern unsigned long int _fInitSuccess;     /* 'fInitSuccess'    in PrbKrnl.c */
extern unsigned long int _usVerMajor;       /* 'usVerMajor'      in PrbKrnl.c */
extern unsigned long int _usVerMinor;       /* 'usVerMinor'      in PrbKrnl.c */

#ifdef INCL_16 /* 16-bit only */
int ProbeKernel(PRPINITIN pReqPack);
#endif

#endif

