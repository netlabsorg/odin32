/* $Id: new.cpp,v 1.1 1999-09-06 02:20:02 bird Exp $
 *
 * new - new and delete operators.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
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

#include "new.h"
#include "cout.h"
#include "malloc.h"


/**
 * New.
 * @returns   pointer to allocated memory.
 * @param     Size  Size requested.
 */
void *operator new(size_t size)
{
    return malloc(size);
}


/**
 * stub
 */
void *operator new(size_t size, void *location)
{
    cout << "operator new(size,location) not implemented"<< endl;
    return NULL;
}


/**
 * stub
 */
void *operator new[](size_t size)
{
    cout << "operator new[](size) not implemented"<< endl;
    return NULL;
}


/**
 * stub
 */
void *operator new[](size_t size, void *location)
{
    cout << "operator new[](size,location) not implemented"<< endl;
    return NULL;
}


/**
 * Delete.
 * @param     location  Pointer to memory block which are to be freed.
 */
void operator delete(void *location)
{
    free(location);
}


/**
 * stub
 */
void operator delete[](void *location)
{
    cout << "operator delete[](location) - not implemented" << endl;
}

