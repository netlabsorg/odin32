/* $Id: winexe.h,v 1.2 1999-05-27 15:17:59 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 PE Exe class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __WINEXE_H__
#define __WINEXE_H__

#include "winimage.h"
#include "windll.h"

typedef ULONG (* WIN32API WIN32EXEENTRY)();

class Win32Exe : public Win32Image
{
public:
        Win32Exe(HINSTANCE hinstance, int NameTableId, int Win32TableId);
	Win32Exe(char *szFileName);
       ~Win32Exe();

	void  setCommandLine(char *cline)      { cmdline = cline; };
	char *getCommandLine()                 { return cmdline;  };

	void  setOS2InstanceHandle(ULONG handle) { OS2InstanceHandle = handle; };
	ULONG getOS2InstanceHandle()             { return OS2InstanceHandle; };

	BOOL  isConsoleApp()                   { return fConsoleApp; };

virtual ULONG start();
	
protected:
	char *cmdline;
	BOOL  fConsoleApp;

	ULONG OS2InstanceHandle;

private:
};

extern Win32Exe *WinExe;

#endif