/* $Id: odin32validate.h,v 1.2 2000-04-16 04:06:11 bird Exp $
 *
 * Parameter validation macros.
 *
 * Note that these are not fully implemented yet. Pointers and memory areas
 * are not checked if valid yet, since this requires some more effort. So,
 * for the time being pointers and memory areas are checked agains the
 * maximum user virtual address space limits.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _odin32validate_h_
#define _odin32validate_h_

/**
 * ADDRESS_SPACE_LIMIT holds the largest possible address in the user virtual
 * address virtual space + 1.
 * 3GB is the higest possible address in the user virtual address space in OS/2.
 */
#define ADDRESS_SPACE_LIMIT 0xc0000000


/**
 * Validates that the specified pointer points at valid.
 * (Currently it only check that it's within the user virtual address space.)
 * @returns   TRUE if valid.
 *            FALSE if invalid.
 * @param     p     Pointer value.
 */
#define VALID_PTR(p) \
            ((char*)(p) >= (char*)0x10000 \
            && (char*)(p) < (char*)ADDRESS_SPACE_LIMIT)


/**
 * Validates that the area pointed to is valid.
 * (Currently it only check that it's within the user virtual address space.)
 * @returns   TRUE if valid.
 *            FALSE if invalid.
 * @param     p     Pointer value.
 * @param     cb    Size of data pointer to by the pointer.
 */
#define VALID_PTRSIZE(p, cb) \
            ((char*)(p) >= (char*)0x10000 \
             && (char*)(p) < (char*)ADDRESS_SPACE_LIMIT \
             && (char*)(p) + (unsigned)(cb) < (char*)ADDRESS_SPACE_LIMIT)


/**
 * Validates that a string pointer is valid and that the entire string is valid.
 * (Currently it only checks that the first byte of the string is within
 * the user virtual address space.)
 * @returns   TRUE if valid.
 *            FALSE if invalid.
 * @param     psz       String pointer.
 * @param     cchMax    Max string length
 */
#define VALID_PSZ(psz) \
            (VALID_PTRSIZE(psz, 1))

/**
 * Validates that a string less or equal to a given max length.
 * Note! the pointer isn't validated. That you'll have to do separatly using
 * VALID_PSZ(psz) which is defined right above.
 * @returns   TRUE if valid.
 *            FALSE if invalid.
 * @param     psz       String pointer.
 * @param     cchMax    Max string length
 */
#define VALID_PSZMAXSIZE(psz, cchMax) \
             (strlen(psz) <= cchMax)



#endif /* !defined(_odin32validate_h_) */
