/* $Id: resource.cpp,v 1.14 2000-01-05 19:39:56 sandervl Exp $ */

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

    module = Win32ImageBase::findModule(hModule);
    if(module == NULL) {
	  dprintf(("FindResourceA Module %X not found (%x %d)", hModule, lpszName, lpszType));
          return(NULL);
    }
    return module->findResourceA(lpszName, (LPSTR)lpszType);
}
//******************************************************************************
//******************************************************************************
HRSRC WIN32API FindResourceW(HINSTANCE hModule, LPCWSTR lpszName,
                          LPCWSTR lpszType)
{
 Win32ImageBase *module;

    module = Win32ImageBase::findModule(hModule);
    if(module == NULL) {
	  dprintf(("FindResourceW Module %X not found (%x %d)", hModule, lpszName, lpszType));
          return(NULL);
    }

    return module->findResourceW((LPWSTR)lpszName, (LPWSTR)lpszType);
}
/*****************************************************************************
 * Name      : HRSRC WIN32API FindResourceExA
 * Purpose   : The FindResourceExA function determines the location of the
 *             resource with the specified type, name, and language in the
 *             specified module.
 * Parameters: HMODULE hModule  resource-module handle
 *             LPCSTR lpType    pointer to resource type
 *             LPCSTR lpName    pointer to resource name
 *             WORD wLanguage   resource language
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to the
 *             specified resource's info block. To obtain a handle to the
 *             resource, pass this handle to the LoadResource function.
 *             If the function fails, the return value is NULL
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

HRSRC  WIN32API FindResourceExA( HMODULE hModule, LPCSTR lpType,
                                    LPCSTR lpName, WORD wLanguage)
{
 Win32ImageBase *module;

    module = Win32ImageBase::findModule(hModule);
    if(module == NULL) {
	  dprintf(("FindResourceExA Module %X not found (%x %d)", hModule, lpName, lpType));
          return(NULL);
    }

    return module->findResourceA((LPSTR)lpName, (LPSTR)lpType, wLanguage);
}

/*****************************************************************************
 * Name      : HRSRC WIN32API FindResourceExA
 * Purpose   : The FindResourceExA function determines the location of the
 *             resource with the specified type, name, and language in the
 *             specified module.
 * Parameters: HMODULE hModule  resource-module handle
 *             LPCSTR lpType    pointer to resource type
 *             LPCSTR lpName    pointer to resource name
 *             WORD wLanguage   resource language
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to the
 *             specified resource's info block. To obtain a handle to the
 *             resource, pass this handle to the LoadResource function.
 *             If the function fails, the return value is NULL
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : SvL
 *****************************************************************************/

HRSRC  WIN32API FindResourceExW(HMODULE hModule,
                                LPCWSTR lpType,
                                LPCWSTR lpName,
                                WORD    wLanguage)
{
 Win32ImageBase *module;

    module = Win32ImageBase::findModule(hModule);
    if(module == NULL) {
	  dprintf(("FindResourceExW Module %X not found (%x %d)", hModule, lpName, lpType));
          return(NULL);
    }

    return module->findResourceW((LPWSTR)lpName, (LPWSTR)lpType, wLanguage);
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

    dprintf(("KERNEL32:EnumResourceNamesA(%08x,%08x,%08x,%08x)",
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

/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceLanguagesA
 * Purpose   : The EnumResourceLanguagesA function searches a module for each
 *             resource of the specified type and name and passes the language
 *             of each resource it locates to a defined callback function
 * Parameters: HMODULE hModule                  resource-module handle
 *             LPCTSTR lpType                   pointer to resource type
 *             LPCTSTR lpName,                  pointer to resource name
 *             ENUMRESLANGPROC lpEnumFunc       pointer to callback function
 *             LONG lParam                  application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 * Remark    : The EnumResourceLanguages function continues to enumerate
 *             resource languages until the callback function returns FALSE
 *             or all resource languages have been enumerated.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceLanguagesA(HMODULE hModule, LPCSTR lpType,
                                     LPCSTR lpName,
                                     ENUMRESLANGPROCA lpEnumFunc,
                                     LONG lParam)
{

  dprintf(("KERNEL32:EnumResourceLanguagesA(%08x,%08x,%08x,%08x,%08x)\n not implemented",
           hModule, lpType, lpName, lpEnumFunc, lParam
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceLanguagesW
 * Purpose   : The EnumResourceLanguagesW function searches a module for each
 *             resource of the specified type and name and passes the language
 *             of each resource it locates to a defined callback function
 * Parameters: HMODULE hModule                  resource-module handle
 *             LPCTSTR lpType                   pointer to resource type
 *             LPCTSTR lpName,                  pointer to resource name
 *             ENUMRESLANGPROC lpEnumFunc       pointer to callback function
 *             LONG lParam                  application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 * Remark    : The EnumResourceLanguages function continues to enumerate
 *             resource languages until the callback function returns FALSE
 *             or all resource languages have been enumerated.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceLanguagesW(HMODULE hModule, LPCWSTR lpType,
                                     LPCWSTR lpName,
                                     ENUMRESLANGPROCW lpEnumFunc,
                                     LONG lParam)
{

  dprintf(("KERNEL32:EnumResourceLanguagesW(%08x,%08x,%08x,%08x,%08x)\n not implemented",
           hModule, lpType, lpName, lpEnumFunc, lParam
          ));

  return (FALSE);
}



/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceTypesA
 * Purpose   : The EnumResourceTypesA function searches a module for resources
 *             and passes each resource type it finds to an application-defined
 *             callback function
 * Parameters: HMODULE hModule,                    resource-module handle
 *             ENUMRESTYPEPROC lpEnumFunc          pointer to callback function
 *             LONG lParam                         application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 * If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceTypesA(HMODULE hModule,
                                 ENUMRESTYPEPROCA lpEnumFunc, LONG lParam)
{
    Win32ImageBase *pModule;

    dprintf(("KERNEL32:EnumResourceTypesA(%08x,%08x,%08x)\n",
              hModule, lpEnumFunc, lParam));

    pModule = Win32ImageBase::findModule(hModule);
    if (pModule == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    return pModule->enumResourceTypesA(hModule, lpEnumFunc, lParam);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceTypesW
 * Purpose   : The EnumResourceTypesW function searches a module for resources
 *             and passes each resource type it finds to an application-defined
 *             callback function
 * Parameters: HMODULE hModule,                    resource-module handle
 *             ENUMRESTYPEPROC lpEnumFunc          pointer to callback function
 *             LONG lParam                         application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 * If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceTypesW(HMODULE hModule,
                                 ENUMRESTYPEPROCW lpEnumFunc, LONG lParam)
{
    Win32ImageBase *pModule;

    dprintf(("KERNEL32:EnumResourceTypesW(%08x,%08x,%08x)\n",
              hModule, lpEnumFunc, lParam));

    pModule = Win32ImageBase::findModule(hModule);
    if (pModule == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }

    return pModule->enumResourceTypesW(hModule, lpEnumFunc, lParam);
}
