/* $Id: new.h,v 1.1 1999-09-06 02:19:59 bird Exp $
 *
 * new - new and delete operators.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */

#ifdef __new_h
    #error("wrong version of new allready loaded!")
#endif

#ifdef __DEBUG_ALLOC__
    #error("__DEBUG_ALLOC__ is not supported!")
#endif


#ifndef _new_h_
#define _new_h_

/* size_t */
#ifndef __size_t
    #define  __size_t
    typedef  unsigned int size_t;
#endif

/* The standard favourites */
void *operator new(size_t size);
void *operator new(size_t size, void *location);  /* stub */

void *operator new[](size_t size);                /* stub */
void *operator new[](size_t size, void *location);/* stub */

void operator delete(void *location);
void operator delete[](void *location);           /* stub */

#endif

