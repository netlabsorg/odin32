/* $Id: kFileFormatBase.cpp,v 1.5 2001-06-22 17:07:47 bird Exp $
 *
 * kFileFormatBase - Base class for kFile<format> classes.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "kFile.h"
#include "kInterfaces.h"
#include "kFileFormatBase.h"


/**
 * Dump function.
 * @returns Successindicator.
 * @param   pOut    Output file.
 */
BOOL   kFileFormatBase::dump(kFile *pOut)
{
    pOut->printf("Sorry, dump() is not implemented for this file format.\n");
    return FALSE;
}


kFileFormatBase::~kFileFormatBase()
{

}

