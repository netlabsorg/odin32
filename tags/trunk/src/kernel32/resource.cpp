/* $Id: resource.cpp,v 1.10 1999-11-29 00:04:05 bird Exp $ */

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
    module = Win32ImageBase::findModule(hModule);
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
    module = Win32ImageBase::findModule(hModule);
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



/**
 * The EnumResourceNames function searches a module for each
 * resource of the specified type and passes the name of each
 * resource it locates to an application-defined callback function
 *
 * @returns  If the function succeeds, the return value is nonzero.
 *           If the function fails, the return value is zero
 * @param    hModule       resource-module handling
 * @param    lpszType      pointer to resource type
 * @param    lpEnumFunc    pointer to callback function
 * @param    lParam        application-defined parameter
 * @status   stub
 * @author   knut st. osmundsen
 * @remark   The EnumResourceNames function continues to enumerate resource
 *           names until the callback function returns FALSE or all resource
 *           names have been enumerated
 */
BOOL WIN32API EnumResourceNamesA(HINSTANCE        hModule,
                                 LPCTSTR          lpszType,
                                 ENUMRESNAMEPROCA lpEnumFunc,
                                 LONG             lParam)
{
    Win32ImageBase *pModule;

    dprintf(("KERNEL32:EnumResourceNamesA(%08x,%08x,%08x,%08x) not implemented\n",
              hModule, lpszType, lpEnumFunc, lParam
             ));

    pModule = Win32ImageBase::findModule(hModule);
    if (pModule == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    return pModule->enumResourceNamesA(hModule, lpszType, lpEnumFunc, lParam);
}


/**
 * The EnumResourceNames function searches a module for each
 * resource of the specified type and passes the name of each
 * resource it locates to an application-defined callback function
 *
 * @returns  If the function succeeds, the return value is nonzero.
 *           If the function fails, the return value is zero
 * @param    hModule       resource-module handling
 * @param    lpszType      pointer to resource type
 * @param    lpEnumFunc    pointer to callback function
 * @param    lParam        application-defined parameter
 * @status   stub
 * @author   knut st. osmundsen
 * @remark   The EnumResourceNames function continues to enumerate resource
 *           names until the callback function returns FALSE or all resource
 *           names have been enumerated
 */
BOOL WIN32API EnumResourceNamesW(HMODULE          hModule,
                                 LPCWSTR          lpszType,
                                 ENUMRESNAMEPROCW lpEnumFunc,
                                 LONG             lParam)
{
    Win32ImageBase *pModule;

    dprintf(("KERNEL32:EnumResourceNamesW(%08x,%08x,%08x,%08x)\n",
              hModule, lpszType, lpEnumFunc, lParam));

    pModule = Win32ImageBase::findModule(hModule);
    if (pModule == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    return pModule->enumResourceNamesW(hModule, lpszType, lpEnumFunc, lParam);
}

