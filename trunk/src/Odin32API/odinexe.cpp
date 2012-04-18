/* $Id: odinexe.cpp,v 1.3 1999-10-05 14:28:52 sandervl Exp $ */
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

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD _Resource_PEResTab;
}
//******************************************************************************
//******************************************************************************
int main(int argc, char *argv[])
{
#ifdef ODIN_FORCE_WIN32_TIB
  // Needed for proper __try/__except support:
  ForceWin32TIB();
#endif
  RegisterLxExe(WinMain, (PVOID)&_Resource_PEResTab);
}
//******************************************************************************
//******************************************************************************
