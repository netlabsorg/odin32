/* $Id: odinexe.cpp,v 1.1 2000-04-18 20:08:32 sandervl Exp $ */
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

//Win32 app entrypoint:
int WIN32API WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);

//******************************************************************************
//******************************************************************************
int main(int argc, char *argv[])
{
  RegisterLxExe(WinMain, NULL);
}
//******************************************************************************
//******************************************************************************
