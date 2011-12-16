/* $Id: new.cpp,v 1.5 2000-09-02 21:08:14 bird Exp $
 *
 * new - new and delete operators.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_NOAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "new.h"
#include "rmalloc.h"
#include "log.h"


#pragma info(none)
/**
 * New.
 * @returns   pointer to allocated memory.
 * @param     Size  Size requested.
 */
void *operator new(size_t size)
{
    return rmalloc(size);
}


/**
 * stub
 */
void *operator new(size_t size, void *location) throw()
{
    dprintf(("operator new(size,location) not implemented\n"));
    return NULL;
}


/**
 * stub
 */
void *operator new[](size_t size) throw()
{
    dprintf(("operator new[](size) not implemented\n"));
    return NULL;
}


/**
 * stub
 */
void *operator new[](size_t size, void *location) throw()
{
    dprintf(("operator new[](size,location) not implemented\n"));
    return NULL;
}

#ifndef __DEBUG_ALLOC__
/**
 * Delete.
 * @param     location  Pointer to memory block which are to be freed.
 */
void operator delete(void *location)
{
    rfree(location);
}


/**
 * stub
 */
void operator delete[](void *location) throw()
{
    dprintf(("operator delete[](location) - not implemented\n"));
}
#endif

/***
 *  debug!
 ***/

/**
 * New.
 * @returns   pointer to allocated memory.
 * @param     Size  Size requested.
 */
void *operator new(size_t size, const char *filename, size_t lineno)
{
    return rmalloc(size);
}


/**
 * stub
 */
void *operator new(size_t size, const char *filename, size_t lineno, void *location) throw()
{
    dprintf(("operator new(size,location) not implemented\n"));
    return NULL;
}


/**
 * stub
 */
void *operator new[](size_t size, const char *filename, size_t lineno) throw()
{
    dprintf(("operator new[](size) not implemented\n"));
    return NULL;
}


/**
 * stub
 */
void *operator new[](size_t size, const char *filename, size_t lineno, void *location) throw()
{
    dprintf(("operator new[](size,location) not implemented\n"));
    return NULL;
}

#ifdef __DEBUG_ALLOC__
/**
 * Delete.
 * @param     location  Pointer to memory block which are to be freed.
 */
void operator delete(void *location, const char *filename, size_t lineno)
{
    rfree(location);
}


/**
 * stub
 */
void operator delete[](void *location, const char *filename, size_t lineno)
{
    dprintf(("operator delete[](location) - not implemented\n"));
}
#endif
