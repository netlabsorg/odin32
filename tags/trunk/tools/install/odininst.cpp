/* $Id: odininst.cpp,v 1.1 2000-04-29 18:23:34 sandervl Exp $ */
/*
 * Win32 Odin32 Application executable entrypoint
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2.h>
#include <win32type.h>
#include <odinlx.h>
#include <odininst.h>

//******************************************************************************
//******************************************************************************
int main(int argc, char *argv[])
{
  InitSystemAndRegistry();
}
//******************************************************************************
//******************************************************************************
