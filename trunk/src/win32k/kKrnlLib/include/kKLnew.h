/* $Id: kKLnew.h,v 1.2 2001-10-23 02:12:43 bird Exp $
 *
 * new - new and delete operators.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _new_h_
#define _new_h_

/* check for IBMCPP new.h */
#ifdef __new_h
    #error("A different version of new.h has allready been loaded!")
#endif
#define __new_h /* Defined to prevent IBMCPP new.h from being loaded. */

/* size_t */
#ifndef __size_t
    #define  __size_t
    typedef  unsigned int size_t;
#endif

#ifndef __DEBUG_ALLOC__
/* The standard favourites */
void *operator new(size_t size);
void *operator new(size_t size, void *location);

void *operator new[](size_t size);
void *operator new[](size_t size, void *location);

void operator delete(void *location);
void operator delete[](void *location);
#endif

#ifdef __DEBUG_ALLOC__
void *operator new(size_t size, const char *filename, size_t lineno);
void *operator new(size_t size, const char *filename, size_t lineno, void *location);

void *operator new[](size_t size, const char *filename, size_t lineno);
void *operator new[](size_t size, const char *filename, size_t lineno, void *location);

void operator delete(void *location, const char *filename, size_t lineno);
void operator delete[](void *location, const char *filename, size_t lineno);
#endif

#endif

