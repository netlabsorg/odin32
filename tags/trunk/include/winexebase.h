/* $Id: winexebase.h,v 1.1 1999-09-15 23:29:37 sandervl Exp $ */

/*
 * Win32 PE Exe base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINEXEBASE_H__
#define __WINEXEBASE_H__

#include <winimagebase.h>

typedef ULONG (* WIN32API WIN32EXEENTRY)();

//Base executable class
class Win32ExeBase : public virtual Win32ImageBase
{
public:
	Win32ExeBase(HINSTANCE hInstance);
virtual ~Win32ExeBase();

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

extern Win32ExeBase *WinExe;

#endif //__WINEXEBASE_H__
