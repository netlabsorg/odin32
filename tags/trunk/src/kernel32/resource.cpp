/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Misc resource procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 */
#include <os2win.h>
#include "unicode.h"
#include "winres.h"
#include "winimage.h"
#include "winexe.h"

//******************************************************************************
//lpszName = integer id (high word 0), else string (name or "#237")
//Can lpszType contain a pointer to a default resource type name?
//******************************************************************************
HRSRC WIN32API FindResourceA(HINSTANCE hModule, LPCSTR lpszName, LPCSTR lpszType)
{
 Win32Image *module;

    dprintf(("FindResourceA %X", hModule));
    if(hModule == 0 || hModule == -1 || (WinExe != NULL && hModule ==
       WinExe->getOS2InstanceHandle()))
    {
         module = (Win32Image *)WinExe;
    }
    else module = (Win32Image *)Win32Dll::findModule(hModule);

    if(module == NULL)
    	return(NULL);

    return module->findResourceA(lpszName, (LPSTR)lpszType);
}
//******************************************************************************
//******************************************************************************
HRSRC WIN32API FindResourceW(HINSTANCE hModule, LPCWSTR lpszName,
                    	     LPCWSTR lpszType)
{
 Win32Image *module;

    dprintf(("FindResourceW %X", hModule));
    if(hModule == 0 || hModule == -1 || (WinExe != NULL && hModule ==
       WinExe->getOS2InstanceHandle()))
    {
         module = (Win32Image *)WinExe;
    }
    else module = (Win32Image *)Win32Dll::findModule(hModule);

    if(module == NULL)
	return(NULL);

    return module->findResourceW((LPWSTR)lpszName, (LPWSTR)lpszType);
}
//******************************************************************************
//hRes returned by LoadResource
//******************************************************************************
PVOID WIN32API LockResource(HGLOBAL hRes)
{
  Win32Resource *res = (Win32Resource *)hRes;

  dprintf(("LockResource %X\n", hRes));

  /* @@@PH */
  if (res == NULL)
    return (NULL);
  else
    return res->lockResource();
}
//******************************************************************************
//******************************************************************************
HGLOBAL WIN32API LoadResource(HINSTANCE hModule, HRSRC hRes)
{
    return (HGLOBAL)hRes;
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

    return res->sizeofResource();
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
