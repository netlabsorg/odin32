/* $Id: rcdata.cpp,v 1.1 1999-09-06 02:20:08 bird Exp $ */

/*
 * PE2LX RCDATA resources
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "pe2lx.h"
#include "rcdata.h"

//******************************************************************************
//******************************************************************************
BOOL ShowRCData(LXHeaderSuper &OS2Exe, int id, char *data, int size)
{
  ltassert((ULONG)data > MINPTR && (ULONG)data+size < MAXPTR);
  cout << "RCData/Version resource with id " << id << " , size " << size << endl;
  return OS2Exe.StoreResource(id, RT_RCDATA, size, data);
}
//******************************************************************************
//******************************************************************************
