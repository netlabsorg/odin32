/* $Id: APIImport.h,v 1.2 2000-02-11 18:35:53 bird Exp $ */
/*
 * APIImportPE - imports a DLL with functions into the Odin32 database. Header.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _APIImport_h_
#define _APIImport_h_

/******************************************************************************
*   Defined Constants                                                         *
******************************************************************************/
#define MAJOR_VER 0
#define MINOR_VER 5

/******************************************************************************
*   Structures and Typedefs                                                   *
******************************************************************************/
typedef struct _options
{
    BOOL fIgnoreOrdinals;
} OPTIONS, *POPTIONS;

#endif

