/* $Id: winexebase.h,v 1.1 2000-03-09 19:03:22 sandervl Exp $ */

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

typedef ULONG (* WIN32API WIN32EXEENTRY)(LPVOID);

//Base executable class
class Win32ExeBase : public virtual Win32ImageBase
{
public:
	Win32ExeBase(HINSTANCE hInstance);
virtual ~Win32ExeBase();

	void   setCommandLine(char *cline);
	LPSTR  getCommandLineA()                { return cmdLineA;  };
	LPWSTR getCommandLineW()                { return cmdLineW;  };

	BOOL  isConsoleApp()                   { return fConsoleApp; };

virtual ULONG start();

virtual BOOL  isDll();
	
protected:
	LPSTR  cmdLineA;
	LPWSTR cmdLineW;
	BOOL   fConsoleApp;

private:
};

extern Win32ExeBase *WinExe;

#endif //__WINEXEBASE_H__
