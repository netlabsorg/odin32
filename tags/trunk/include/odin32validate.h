/* $Id: odin32validate.h,v 1.1 2000-04-16 02:59:03 bird Exp $
 *
 * Parameter validation macros.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _odin32validate_h_
#define _odin32validate_h_

/**
 * ADDRESS_SPACE_LIMIT holds the largest possible address in the address space + 1.
 * 3GB is the higest possible address in the user address space in OS/2.
 */
#define ADDRESS_SPACE_LIMIT 0xc0000000


/**
 * Validates that the value of a pointer is within the virtual address space.
 * @returns   TRUE if valid.
 *            FALSE if invalid.
 * @param     p     Pointer value.
 */
#define VALIDFAST_PTR(p) \
            ((char*)(p) >= (char*)0x10000 \
            && (char*)(p) < (char*)ADDRESS_SPACE_LIMIT)


/**
 * Validates that a area pointed to is within the virtual address space.
 * @returns   TRUE if valid.
 *            FALSE if invalid.
 * @param     p     Pointer value.
 * @param     cb    Size of data pointer to by the pointer.
 */
#define VALIDFAST_PTRSIZE(p, cb) \
            ((char*)(p) >= (char*)0x10000 \
             && (char*)(p) < (char*)ADDRESS_SPACE_LIMIT \
             && (char*)(p) + (char*)(cb) < (char*)ADDRESS_SPACE_LIMIT)

/**
 * Validates that the value of a string pointer is within the virtual address
 * space, and that its length less or equal to the given max length.
 * @returns   TRUE if valid.
 *            FALSE if invalid.
 * @param     psz       String pointer.
 * @param     cchMax    Max string length
 */
#define VALIDFAST_PSZLENGTH(psz, cchMax) \
            (VALIDFAST_PTRSIZE(psz, 1) \
             && strlen(psz) <= cchMax)



#endif /* !defined(_odin32validate_h_) */
