/* $Id: winexepeldr.h,v 1.1 2000-03-10 16:12:02 sandervl Exp $ */

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

typedef BOOL (* WIN32API WIN32CTOR)(char *, ULONG);

//Class for executables run by the ring 3 PE loader
class Win32PeLdrExe : public Win32PeLdrImage, public Win32ExeBase
{
public:
	Win32PeLdrExe(char *szFileName);
virtual ~Win32PeLdrExe();

virtual BOOL init(ULONG reservedMem);

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

#endif //__WINEXEPELDR_H__
