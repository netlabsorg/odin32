/*
 * Internal functions exported by odin dlls
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __ODINAPI_H__
#define __ODINAPI_H__

void WIN32API SetRegistryRootKey(HKEY hRootkey, HKEY hKey);
void WIN32API SetCustomBuildName(char *lpszName, DWORD ordinalbase);

#endif //__WINUSER32_H__
