/* $Id: initsystem.h,v 1.1 1999-12-09 11:59:28 sandervl Exp $ */

#ifndef _INITSYSTEM_H_
#define _INITSYSTEM_H_

BOOL InitSystemEnvironment(ULONG nrCPUs);

#ifndef OS2_INCLUDED
extern LONG (WIN32API *ADVAPI32_RegCloseKey)(HKEY);
extern LONG (WIN32API *ADVAPI32_RegCreateKeyA)(HKEY,LPCSTR,LPHKEY);
extern LONG (WIN32API *ADVAPI32_RegSetValueExA)(HKEY,LPSTR,DWORD,DWORD,LPBYTE,DWORD);
#endif

#endif //_INITSYSTEM_H_