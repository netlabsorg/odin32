/* $Id: odinexe.cpp,v 1.2 1999-09-18 17:46:47 sandervl Exp $ */
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
#include <exceptions.h>

//Win32 app entrypoint:
int WIN32API WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD _Resource_PEResTab;
}
//******************************************************************************
//******************************************************************************
int main(int argc, char *argv[])
{
 WINEXCEPTION_FRAME exceptFrame;

  OS2SetExceptionHandler(&exceptFrame);
  RegisterLxExe(WinMain, (PVOID)&_Resource_PEResTab);
  OS2UnsetExceptionHandler(&exceptFrame);
}
//******************************************************************************
//******************************************************************************
