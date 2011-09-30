/* $Id: devErrors.h,v 1.1 2002-12-16 01:50:05 bird Exp $
 *
 * Error definitions for kKrnlLib.
 *
 * Copyright (c) 2000-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef __devErrors_h__
#define __devErrors_h__

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/*
 * Probkrnl error codes. (They all begin with ERROR_PROB_.)
 *
 * NOTE! ERROR_PROB_KRNL_*  <  ERROR_PROB_SYM_*  <  ERROR_PROB_SYMDB_*
 */
#define ERROR_PROB_BASE                             0x3000
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
#define ERROR_PROB_KRNL_GIS                         (ERROR_PROB_BASE + 21)

#define ERROR_PROB_SYM_FIRST                        ERROR_PROB_SYM_FILE_NOT_FOUND
#define ERROR_PROB_SYM_LAST                         ERROR_PROB_SYMDB_KRNL_NOT_FOUND
#define ERROR_PROB_SYM_FILE_NOT_FOUND               (ERROR_PROB_BASE + 30)
#define ERROR_PROB_SYM_READERROR                    (ERROR_PROB_BASE + 31)
#define ERROR_PROB_SYM_INVALID_MOD_NAME             (ERROR_PROB_BASE + 32)
#define ERROR_PROB_SYM_SEGS_NE_OBJS                 (ERROR_PROB_BASE + 33)
#define ERROR_PROB_SYM_SEG_DEF_SEEK                 (ERROR_PROB_BASE + 34)
#define ERROR_PROB_SYM_SEG_DEF_READ                 (ERROR_PROB_BASE + 35)
#define ERROR_PROB_SYM_VERIFY_IOCTL                 (ERROR_PROB_BASE + 36)
/* - reserved for future errors - */

#define ERROR_PROB_SYM_IMPORTS_NOTFOUND             (ERROR_PROB_BASE + 100)
#define ERROR_PROB_SYMDB_KRNL_NOT_FOUND             (ERROR_PROB_BASE + 101)

/*
 * Init errors.
 */
#define ERROR_D16_THUNKING_FAILED                   (ERROR_PROB_BASE + 120)
#define ERROR_D16_OPEN_DEV_FAILED                   (ERROR_PROB_BASE + 121)
#define ERROR_D16_IOCTL_FAILED                      (ERROR_PROB_BASE + 122)


/*
 * 32-bit errors (from 32-bit code that is).
 */
#define ERROR_D32_FIRST                              ERROR_PROB_BASE + 256
#define ERROR_D32_LAST                               ERROR_D32_SYMDB_NOT_FOUND

/*
 * GetKernelInfo32 Errors
 */
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
 * VerifyImportTab32 Errors.
 */
#define ERROR_D32_PROC_NOT_FOUND                    (0x000a + ERROR_D32_FIRST)
#define ERROR_D32_INVALID_OBJ_OR_ADDR               (0x000b + ERROR_D32_FIRST)
#define ERROR_D32_INVALID_ADDRESS                   (0x000c + ERROR_D32_FIRST)
#define ERROR_D32_TOO_INVALID_PROLOG                (0x000d + ERROR_D32_FIRST)
#define ERROR_D32_NOT_IMPLEMENTED                   (0x000e + ERROR_D32_FIRST)

/*
 * LookupKrnlEntry32 Errors.
 */
#define ERROR_D32_SYMDB_NOT_FOUND                   (0x000f + ERROR_D32_FIRST)


/*
 * Procedure shift and mask.
 * Used to get or set the procedure number of an error code.
 */
#define ERROR_D32_PROC_SHIFT                        0x10



/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
const char *    devGetErrorMsg(int sErr);


#endif



