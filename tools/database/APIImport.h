/* $Id: APIImport.h,v 1.1 1999-09-05 02:53:04 bird Exp $ */
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
#define MINOR_VER 4

/******************************************************************************
*   Structures and Typedefs                                                   *
******************************************************************************/
typedef struct _options
{
    BOOL fIgnoreOrdinals;
} OPTIONS, *POPTIONS;

#endif

