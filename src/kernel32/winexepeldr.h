/* $Id: winexepeldr.h,v 1.7 2002-07-23 13:51:48 sandervl Exp $ */

/*
 * Win32 PE loader Exe class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINEXEPELDR_H__
#define __WINEXEPELDR_H__

#include <winexebase.h>
#include <winimagepeldr.h>

typedef BOOL (* WIN32API WIN32CTOR)(char *, char *, char *, ULONG, ULONG, BOOL, BOOL);

//Class for executables run by the ring 3 PE loader
class Win32PeLdrExe : public Win32PeLdrImage, public Win32ExeBase
{
public:
	Win32PeLdrExe(char *szFileName, BOOL fConsoleApp);
virtual ~Win32PeLdrExe();

protected:
private:
};

extern char szErrorTitle[];
extern char szMemErrorMsg[];
extern char szFileErrorMsg[];
extern char szPEErrorMsg[];
extern char szCPUErrorMsg[];
extern char szExeErrorMsg[];
extern char szInteralErrorMsg[];

extern BOOL fPeLoader;

#endif //__WINEXEPELDR_H__
