/* $Id: errors.h,v 1.1.2.2 2000-08-15 06:16:03 bird Exp $
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
#define ERROR_PROB_SYM_VERIFY_FIRST                 (ERROR_PROB_BASE + 36)
#define ERROR_PROB_SYM_V_PROC_NOT_FND               (ERROR_PROB_SYM_VERIFY_FIRST + ERROR_D32_PROC_NOT_FOUND      - ERROR_D32_VERIFYIMPORTTAB_FIRST)
#define ERROR_PROB_SYM_V_OBJ_OR_ADDR                (ERROR_PROB_SYM_VERIFY_FIRST + ERROR_D32_INVALID_OBJ_OR_ADDR - ERROR_D32_VERIFYIMPORTTAB_FIRST)
#define ERROR_PROB_SYM_V_ADDRESS                    (ERROR_PROB_SYM_VERIFY_FIRST + ERROR_D32_INVALID_ADDRESS     - ERROR_D32_VERIFYIMPORTTAB_FIRST)
#define ERROR_PROB_SYM_V_PROLOG                     (ERROR_PROB_SYM_VERIFY_FIRST + ERROR_D32_TOO_INVALID_PROLOG  - ERROR_D32_VERIFYIMPORTTAB_FIRST)
#define ERROR_PROB_SYM_V_NOT_IMPL                   (ERROR_PROB_SYM_VERIFY_FIRST + ERROR_D32_NOT_IMPLEMENTED     - ERROR_D32_VERIFYIMPORTTAB_FIRST)
/* - reserved for future errors - *
#define ERROR_PROB_SYM_                             (ERROR_PROB_BASE + 41)
#define ERROR_PROB_SYM_                             (ERROR_PROB_BASE + 42) */
#define ERROR_PROB_SYM_IMPORTS_NOTFOUND             (ERROR_PROB_BASE + 43)
#define ERROR_PROB_SYMDB_KRNL_NOT_FOUND             (ERROR_PROB_BASE + 44)



/*
 * D32init Errors
 */
#define ERROR_D32_GETOS2KRNL_FAILED                 0x01
#define ERROR_D32_NO_SWAPMTE                        0x02
#define ERROR_D32_TOO_MANY_OBJECTS                  0x03
#define ERROR_D32_NO_OBJECT_TABLE                   0x04
#define ERROR_D32_BUILD_INFO_NOT_FOUND              0x05
#define ERROR_D32_INVALID_BUILD                     0x06

#define ERROR_D32_VERIFYIMPORTTAB_FIRST             0x10
#define ERROR_D32_PROC_NOT_FOUND                    (ERROR_D32_VERIFYIMPORTTAB_FIRST + 0)
#define ERROR_D32_INVALID_OBJ_OR_ADDR               (ERROR_D32_VERIFYIMPORTTAB_FIRST + 1)
#define ERROR_D32_INVALID_ADDRESS                   (ERROR_D32_VERIFYIMPORTTAB_FIRST + 2)
#define ERROR_D32_TOO_INVALID_PROLOG                (ERROR_D32_VERIFYIMPORTTAB_FIRST + 3)
#define ERROR_D32_NOT_IMPLEMENTED                   (ERROR_D32_VERIFYIMPORTTAB_FIRST + 4)
#define ERROR_D32_VERIFYIMPORTTAB_LAST              0x1F

#define ERROR_D32_VERIFY_FAILED                     0x20


#endif




