/* $Id: ProbKrnlErrors.h,v 1.1 2001-09-14 01:55:46 bird Exp $
 *
 * Error definitions for Win32k.sys
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _errors_h_
#define _errors_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/*
 * Probkrnl error codes. (They all begin with ERROR_PROB_.)
 *
 * NOTE! ERROR_PROB_KRNL_*  <  ERROR_PROB_SYM_*  <  ERROR_PROB_SYMDB_*
 */
#define ERROR_PROB_BASE                             10000
#define ERROR_PROB_KRNL_FIRST                       ERROR_PROB_KRNL_OPEN_FAILED
#define ERROR_PROB_KRNL_LAST                        ERROR_PROB_KRNL_OTE_READ
#define ERROR_PROB_KRNL_OPEN_FAILED                 (ERROR_PROB_BASE + 0)
#define ERROR_PROB_KRNL_SEEK_SIZE                   (ERROR_PROB_BASE + 2)
#define ERROR_PROB_KRNL_SEEK_FIRST                  (ERROR_PROB_BASE + 3)
#define ERROR_PROB_KRNL_READ_FIRST                  (ERROR_PROB_BASE + 4)
#define ERROR_PROB_KRNL_READ_NEXT                   (ERROR_PROB_BASE + 5)
#define ERROR_PROB_KRNL_TAG_NOT_FOUND               (ERROR_PROB_BASE + 6)
#define ERROR_PROB_KRNL_INV_SIGANTURE               (ERROR_PROB_BASE + 7)
#define ERROR_PROB_KRNL_INV_BUILD_NBR               (ERROR_PROB_BASE + 8)
#define ERROR_PROB_KRNL_BUILD_VERSION               (ERROR_PROB_BASE + 9)
#define ERROR_PROB_KRNL_MZ_SEEK                     (ERROR_PROB_BASE + 10)
#define ERROR_PROB_KRNL_MZ_READ                     (ERROR_PROB_BASE + 11)
#define ERROR_PROB_KRNL_NEOFF_INVALID               (ERROR_PROB_BASE + 12)
#define ERROR_PROB_KRNL_NEOFF_SEEK                  (ERROR_PROB_BASE + 13)
#define ERROR_PROB_KRNL_LX_READ                     (ERROR_PROB_BASE + 14)
#define ERROR_PROB_KRNL_LX_SIGNATURE                (ERROR_PROB_BASE + 15)
#define ERROR_PROB_KRNL_OBJECT_CNT                  (ERROR_PROB_BASE + 16)
#define ERROR_PROB_KRNL_OBJECT_CNR_10               (ERROR_PROB_BASE + 17)
#define ERROR_PROB_KRNL_OTE_SEEK                    (ERROR_PROB_BASE + 18)
#define ERROR_PROB_KRNL_OTE_READ                    (ERROR_PROB_BASE + 19)
#define ERROR_PROB_KRNL_OTE_SIZE_MIS                (ERROR_PROB_BASE + 20)

#define ERROR_PROB_SYM_FIRST                        ERROR_PROB_SYM_FILE_NOT_FOUND
#define ERROR_PROB_SYM_LAST                         ERROR_PROB_SYMDB_KRNL_NOT_FOUND
#define ERROR_PROB_SYM_FILE_NOT_FOUND               (ERROR_PROB_BASE + 30)
#define ERROR_PROB_SYM_READERROR                    (ERROR_PROB_BASE + 31)
#define ERROR_PROB_SYM_INVALID_MOD_NAME             (ERROR_PROB_BASE + 32)
#define ERROR_PROB_SYM_SEGS_NE_OBJS                 (ERROR_PROB_BASE + 33)
#define ERROR_PROB_SYM_SEG_DEF_SEEK                 (ERROR_PROB_BASE + 34)
#define ERROR_PROB_SYM_SEG_DEF_READ                 (ERROR_PROB_BASE + 35)
#define ERROR_PROB_SYM_VERIFY_IOCTL                 (ERROR_PROB_BASE + 36)
#define ERROR_PROB_SYM_D32_FIRST                    (ERROR_PROB_BASE + 37)
#define ERROR_PROB_SYM_V_GETOS2KRNL                 (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_GETOS2KRNL_FAILED     - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_NO_SWAPMTE                 (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_NO_SWAPMTE            - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_OBJECTS                    (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_TOO_MANY_OBJECTS      - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_OBJECT_TABLE               (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_NO_OBJECT_TABLE       - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_BUILD_INFO                 (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_BUILD_INFO_NOT_FOUND  - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_INVALID_BUILD              (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_INVALID_BUILD         - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_VERIFY                     (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_VERIFY_FAILED         - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_IPE                        (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_IPE                   - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_HEAPINIT                   (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_HEAPINIT_FAILED       - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_PROC_NOT_FND               (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_PROC_NOT_FOUND        - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_OBJ_OR_ADDR                (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_INVALID_OBJ_OR_ADDR   - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_ADDRESS                    (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_INVALID_ADDRESS       - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_PROLOG                     (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_TOO_INVALID_PROLOG    - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_NOT_IMPL                   (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_NOT_IMPLEMENTED       - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_V_D32_LDR_INIT               (ERROR_PROB_SYM_D32_FIRST + ERROR_D32_LDR_INIT_FAILED       - ERROR_D32_FIRST)
#define ERROR_PROB_SYM_D32_LAST                     ERROR_PROB_SYM_V_D32_LDR_INIT
/* - reserved for future errors - */
#define ERROR_PROB_SYM_IMPORTS_NOTFOUND             (ERROR_PROB_BASE + 100)
#define ERROR_PROB_SYMDB_KRNL_NOT_FOUND             (ERROR_PROB_BASE + 101)



/*
 * GetKernelInfo32 Errors
 */
#define ERROR_D32_FIRST                              0x0001
#define ERROR_D32_LAST                               ERROR_D32_NOT_IMPLEMENTED
#define ERROR_D32_GETOS2KRNL_FAILED                 (0x0000 + ERROR_D32_FIRST)
#define ERROR_D32_NO_SWAPMTE                        (0x0001 + ERROR_D32_FIRST)
#define ERROR_D32_TOO_MANY_OBJECTS                  (0x0002 + ERROR_D32_FIRST)
#define ERROR_D32_NO_OBJECT_TABLE                   (0x0003 + ERROR_D32_FIRST)
#define ERROR_D32_BUILD_INFO_NOT_FOUND              (0x0004 + ERROR_D32_FIRST)
#define ERROR_D32_INVALID_BUILD                     (0x0005 + ERROR_D32_FIRST)

/*
 * R0Init32 Errors
 */
#define ERROR_D32_VERIFY_FAILED                     (0x0006 + ERROR_D32_FIRST)
#define ERROR_D32_IPE                               (0x0007 + ERROR_D32_FIRST)
#define ERROR_D32_HEAPINIT_FAILED                   (0x0008 + ERROR_D32_FIRST)
#define ERROR_D32_LDR_INIT_FAILED                   (0x0009 + ERROR_D32_FIRST)

/*
 * VerifyImportTab32 errors.
 */
#define ERROR_D32_PROC_NOT_FOUND                    (0x000a + ERROR_D32_FIRST)
#define ERROR_D32_INVALID_OBJ_OR_ADDR               (0x000b + ERROR_D32_FIRST)
#define ERROR_D32_INVALID_ADDRESS                   (0x000c + ERROR_D32_FIRST)
#define ERROR_D32_TOO_INVALID_PROLOG                (0x000d + ERROR_D32_FIRST)
#define ERROR_D32_NOT_IMPLEMENTED                   (0x000e + ERROR_D32_FIRST)

/*
 * Procedure shift and mask.
 * Used to get or set the procedure number of an error code.
 */
#define ERROR_D32_ERROR_MASK                        0xff00
#define ERROR_D32_PROC_MASK                         0x7f00
#define ERROR_D32_PROC_FLAG                         0x8000
#define ERROR_D32_PROC_SHIFT                        0x8


#endif




