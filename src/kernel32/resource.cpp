/* $Id: resource.cpp,v 1.19 2002-03-22 12:51:35 sandervl Exp $ */

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
#include "winimagebase.h"
#include "winexebase.h"
#include "windllbase.h"

#define DBG_LOCALLOG	DBG_resource
#include "dbglocal.h"


//******************************************************************************
//lpszName = integer id (high word 0), else string (name or "#237")
//Can lpszType contain a pointer to a default resource type name?
//******************************************************************************
HRSRC WIN32API FindResourceA(HINSTANCE hModule, LPCSTR lpszName, LPCSTR lpszType)
{
 Win32ImageBase *module;
 WORD wLanguage = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

    module = Win32ImageBase::findModule(hModule);
    if(module == NULL) {
	  dprintf(("FindResourceA Module %X not found (%x %d)", hModule, lpszName, lpszType));
          return(NULL);
    }
    return module->findResourceA(lpszName, (LPSTR)lpszType, wLanguage);
}
//******************************************************************************
//******************************************************************************
HRSRC WIN32API FindResourceW(HINSTANCE hModule, LPCWSTR lpszName,
                             LPCWSTR lpszType)
{
 Win32ImageBase *module;
 WORD wLanguage = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

    module = Win32ImageBase::findModule(hModule);
    if(module == NULL) {
	  dprintf(("FindResourceW Module %X not found (%x %d)", hModule, lpszName, lpszType));
          return(NULL);
    }
    return module->findResourceW((LPWSTR)lpszName, (LPWSTR)lpszType, wLanguage);
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
 * Status    : fully implemented
 *
 * Author    : SvL
 *****************************************************************************/

HRSRC WIN32API FindResourceExA( HMODULE hModule, LPCSTR lpType,
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
 * Status    : fully implemented
 *
 * Author    : SvL
 *****************************************************************************/

HRSRC WIN32API FindResourceExW(HMODULE hModule, LPCWSTR lpType,
                               LPCWSTR lpName, WORD wLanguage)
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
//hRes == returned by FindResource(Ex) = PIMAGE_RESOURCE_DATA_ENTRY for resource
//******************************************************************************
HGLOBAL WIN32API LoadResource(HINSTANCE hModule, HRSRC hRes)
{
  Win32ImageBase *module;

  /* @@@PH */
  if(HIWORD(hRes) == NULL) {
      dprintf(("ERROR: LoadResource %x: invalid hRes %x", hModule, hRes));
      return 0;
  }

  dprintf(("LoadResource %x %X\n", hModule, hRes));
  if(hModule == 0 || hModule == -1 || (WinExe && hModule == WinExe->getInstanceHandle())) {
        module = (Win32ImageBase *)WinExe;
  }
  else {
        module = (Win32ImageBase *)Win32DllBase::findModule(hModule);
  }

  return (HGLOBAL)module->getResourceAddr(hRes);
}
//******************************************************************************
//hRes == returned by FindResource(Ex)
//******************************************************************************
DWORD WIN32API SizeofResource(HINSTANCE hModule, HRSRC hRes)
{
 Win32ImageBase *module;

  if(hRes == NULL) {
   	dprintf(("ERROR: SizeofResource %x: invalid hRes %x", hModule, hRes));
   	return(0);
  }

  dprintf(("SizeofResource %x %x", hModule, hRes));

  if(hModule == 0 || hModule == -1 || (WinExe && hModule == WinExe->getInstanceHandle())) {
        module = (Win32ImageBase *)WinExe;
  }
  else {
        module = (Win32ImageBase *)Win32DllBase::findModule(hModule);
  }

  return module->getResourceSize(hRes);
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
 * @status   fully implemented
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
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND); //todo: right error????
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
 * @status   fully implemented
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
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND); //todo: right error????
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
 * Status    : fully implemented
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceLanguagesA(HMODULE hModule, LPCSTR lpType,
                                     LPCSTR lpName,
                                     ENUMRESLANGPROCA lpEnumFunc,
                                     LONG lParam)
{
    Win32ImageBase *pModule;

    dprintf(("KERNEL32:EnumResourceLanguagesA(%08x,%08x,%08x,%08x,%08x)",
              hModule, lpType, lpName, lpEnumFunc, lParam));

    pModule = Win32ImageBase::findModule(hModule);
    if (pModule == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND); //todo: right error????
        return FALSE;
    }

    return pModule->enumResourceLanguagesA(hModule, lpType, lpName, lpEnumFunc, lParam);
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
 * Status    : fully implemented
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceLanguagesW(HMODULE hModule, LPCWSTR lpType,
                                     LPCWSTR lpName,
                                     ENUMRESLANGPROCW lpEnumFunc,
                                     LONG lParam)
{
    Win32ImageBase *pModule;

    dprintf(("KERNEL32:EnumResourceLanguagesW(%08x,%08x,%08x,%08x,%08x)",
              hModule, lpType, lpName, lpEnumFunc, lParam));

    pModule = Win32ImageBase::findModule(hModule);
    if (pModule == NULL)
    {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND); //todo: right error????
        return FALSE;
    }
    return pModule->enumResourceLanguagesW(hModule, lpType, lpName, lpEnumFunc, lParam);
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
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : fully implemented
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
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND); //todo: right error????
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
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : fully implemented
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
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND); //todo: right error????
        return FALSE;
    }

    return pModule->enumResourceTypesW(hModule, lpEnumFunc, lParam);
}
//******************************************************************************
//******************************************************************************
