/* $Id: resource.cpp,v 1.9 1999-09-15 23:38:01 sandervl Exp $ */

/*
 * Misc resource procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <unicode.h>
#include "winres.h"
#include <winimagebase.h>
#include <winexebase.h>
#include <windllbase.h>

//******************************************************************************
//lpszName = integer id (high word 0), else string (name or "#237")
//Can lpszType contain a pointer to a default resource type name?
//******************************************************************************
HRSRC WIN32API FindResourceA(HINSTANCE hModule, LPCSTR lpszName, LPCSTR lpszType)
{
 Win32ImageBase *module;

    dprintf(("FindResourceA %X", hModule));
    if(hModule == 0 || hModule == -1 || (WinExe != NULL && hModule ==
       WinExe->getInstanceHandle()))
    {
         module = (Win32ImageBase *)WinExe;
    }
    else module = (Win32ImageBase *)Win32DllBase::findModule(hModule);

    if(module == NULL)
    	return(NULL);

    return module->findResourceA(lpszName, (LPSTR)lpszType);
}
//******************************************************************************
//******************************************************************************
HRSRC WIN32API FindResourceW(HINSTANCE hModule, LPCWSTR lpszName,
                    	     LPCWSTR lpszType)
{
 Win32ImageBase *module;

    dprintf(("FindResourceW %X", hModule));
    if(hModule == 0 || hModule == -1 || (WinExe != NULL && hModule ==
       WinExe->getInstanceHandle()))
    {
         module = (Win32ImageBase *)WinExe;
    }
    else module = (Win32ImageBase *)Win32DllBase::findModule(hModule);

    if(module == NULL)
	return(NULL);

    return module->findResourceW((LPWSTR)lpszName, (LPWSTR)lpszType);
}
//******************************************************************************
//hRes returned by LoadResource
//******************************************************************************
PVOID WIN32API LockResource(HGLOBAL hRes)
{
    return (PVOID)hRes;
}
//******************************************************************************
//hRes == returned by FindResource(Ex)
//******************************************************************************
HGLOBAL WIN32API LoadResource(HINSTANCE hModule, HRSRC hRes)
{
  Win32Resource *res = (Win32Resource *)hRes;

  dprintf(("LoadResource %x %X\n", hModule, hRes));

  /* @@@PH */
  if (HIWORD(res) == NULL) {
	dprintf(("LoadResource %x: invalid hRes %x", hModule, hRes));
	return 0;
  }
  else  return (HGLOBAL)res->lockResource();
}
//******************************************************************************
//hRes == returned by FindResource(Ex)
//******************************************************************************
DWORD WIN32API SizeofResource(HINSTANCE hModule, HRSRC hRes)
{
 Win32Resource *res = (Win32Resource *)hRes;

    dprintf(("OS2SizeofResource\n"));
    if(res == NULL)
    return(0);

    return res->getSize();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumResourceNamesA(HINSTANCE hModule, LPCTSTR lpszType,
                    ENUMRESNAMEPROCA lpEnumFunc, LONG lParam)
{
    dprintf(("OS2EnumResourceNamesA - stub\n"));
    return(FALSE);
}
//******************************************************************************
//******************************************************************************
