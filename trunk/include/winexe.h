/* $Id: winexe.h,v 1.6 1999-08-23 18:06:45 sandervl Exp $ */

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

class Win32Exe;
typedef Win32Exe *(* WIN32API WIN32CTOR)(char *);

class Win32Exe : public Win32Image
{
public:
        Win32Exe(HINSTANCE hinstance, int NameTableId, int Win32TableId);
	Win32Exe(char *szFileName);
       ~Win32Exe();

	void  setCommandLine(char *cline)      { cmdline = cline; };
	char *getCommandLine()                 { return cmdline;  };

	BOOL  isConsoleApp()                   { return fConsoleApp; };

virtual ULONG start();

virtual BOOL  isDll();
	
protected:
	char *cmdline;
	BOOL  fConsoleApp;

private:
};

extern Win32Exe *WinExe;

#endif