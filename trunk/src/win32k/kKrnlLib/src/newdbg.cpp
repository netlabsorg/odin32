/* $Id: newdbg.cpp,v 1.2 2002-03-31 19:01:17 bird Exp $
 *
 * newdbg - new and delete operators. Debug versions.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: newdbg.cpp,v 1.2 2002-03-31 19:01:17 bird Exp $";
#endif


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
#include <kLib/kTypes.h>
#include <kLib/kLog.h>
#include "kKLnew.h"
#include "kKLrmalloc.h"


#pragma info(none)
/**
 * New.
 * @returns   pointer to allocated memory.
 * @param     Size  Size requested.
 */
void *operator new(size_t size, const char *filename, size_t lineno)
{
    KLOGENTRY3("void *","size_t size, const char * filename, size_t lineno", size, filename, lineno);
    void *pv = rmalloc(size);
    KLOGEXIT(pv);
    return pv;
}


/**
 * Called new with storage.
 * Dummy call just to get the constructor stuff going.
 */
void *operator new(size_t size, const char *filename, size_t lineno, void *location)
{
    KLOGENTRY4("void *","size_t size, const char * filename, size_t lineno, void * location", size, filename, lineno, location);
    KLOGEXIT(location);
    return location;
}


/**
 * Allocate array.
 */
void *operator new[](size_t size, const char *filename, size_t lineno)
{
    KLOGENTRY3("void *","size_t size, const char * filename, size_t lineno", size, filename, lineno);
    void *pv = rmalloc(size);
    KLOGEXIT(pv);
    return pv;
}


/**
 * Called new with storage.
 * Dummy call just to get the constructor stuff going.
 */
void *operator new[](size_t size, const char *filename, size_t lineno, void *location)
{
    KLOGENTRY4("void *","size_t size, const char * filename, size_t lineno, void * location", size, filename, lineno, location);
    KLOGEXIT(location);
    return location;
}

/**
 * Delete.
 * @param     location  Pointer to memory block which are to be freed.
 */
void operator delete(void *location, const char *filename, size_t lineno)
{
    KLOGENTRY3("void","void * location, const char * filename, size_t lineno", location, filename, lineno);
    rfree(location);
    KLOGEXITVOID();
}


/**
 * Delete array.
 */
void operator delete[](void *location, const char *filename, size_t lineno)
{
    KLOGENTRY3("void","void * location, const char * filename, size_t lineno", location, filename, lineno);
    rfree(location);
    KLOGEXITVOID();
}

