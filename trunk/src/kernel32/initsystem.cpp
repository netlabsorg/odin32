/* $Id: initsystem.cpp,v 1.7 2000-03-04 19:52:36 sandervl Exp $ */
/*
 * Odin system initialization (registry & directories)
 *
 *
 * NOTE: Most of this has to be moved into the Odin install program!!!!!
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */

#include <os2win.h>
#include <ctype.h>
#include <string.h>
#include "winreg.h"
#include "global.h"
#include "winnt.h"
#include "winerror.h"
#include "winreg.h"
#include "debugtools.h"
#include "cpuhlp.h"
#include "initsystem.h"
#include "directory.h"
#include <versionos2.h>

#define DBG_LOCALLOG	DBG_initsystem
#include "dbglocal.h"

BOOL InitRegistry();

//******************************************************************************
//******************************************************************************
BOOL InitSystemEnvironment(ULONG nrCPUs)
{
   InitSystemInfo(nrCPUs);
   return InitRegistry();
}
//******************************************************************************
//[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Windows]
//"Directory"=hex(2):25,53,79,73,74,65,6d,52,6f,6f,74,25,00
//"SystemDirectory"=hex(2):25,53,79,73,74,65,6d,52,6f,6f,74,25,5c,73,79,73,74,65,\
//  6d,33,32,00
//"ErrorMode"=dword:00000000
//"NoInteractiveServices"=dword:00000000
//"CSDVersion"=dword:00000300
BYTE ShutdownTime[] = {0x44,0x5e,0xf2,0xbb,0x84,0x41,0xbf,0x01};
//******************************************************************************
BOOL InitRegistry()
{
 HKEY hkey;
 char *buf;
 DWORD val;
 char  digbuf[16];

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Windows",&hkey)!=ERROR_SUCCESS) {
      	dprintf(("InitRegistry: Unable to register system information\n"));
	return FALSE;
   }
   buf = InternalGetWindowsDirectoryA();
   RegSetValueExA(hkey,"Directory",0,REG_BINARY, (LPBYTE)buf, strlen(buf)+1);
   buf = InternalGetSystemDirectoryA();
   RegSetValueExA(hkey,"SystemDirectory",0,REG_BINARY, (LPBYTE)buf, strlen(buf)+1);
   val = 0;
   RegSetValueExA(hkey,"ErrorMode",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0;
   RegSetValueExA(hkey,"NoInteractiveServices",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = ODINNT_BUILD_NR;
   RegSetValueExA(hkey,"CSDVersion",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   RegSetValueExA(hkey,"ShutdownTime",0,REG_DWORD, (LPBYTE)ShutdownTime, sizeof(ShutdownTime));
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",&hkey)!=ERROR_SUCCESS) {
      	dprintf(("InitRegistry: Unable to register system information (2)"));
	return FALSE;
   }
   buf = InternalGetSystemDirectoryA();
   RegSetValueExA(hkey,"SystemRoot",0,REG_SZ, (LPBYTE)buf, strlen(buf)+1);
   RegSetValueExA(hkey,"PathName",0,REG_SZ, (LPBYTE)buf, strlen(buf)+1);
   sprintf(digbuf, "%d", ODINNT_BUILD_NR);
   RegSetValueExA(hkey,"CurrentBuildNumber",0,REG_SZ, (LPBYTE)digbuf, strlen(digbuf)+1);
   RegSetValueExA(hkey,"CurrentType",0,REG_SZ, (LPBYTE)ODINNT_OSTYPE_UNI, sizeof(ODINNT_OSTYPE_UNI));
   RegSetValueExA(hkey,"CSDVersion",0,REG_SZ, (LPBYTE)ODINNT_CSDVERSION, sizeof(ODINNT_CSDVERSION));
   RegSetValueExA(hkey,"SoftwareType",0,REG_SZ, (LPBYTE)ODINNT_SOFTWARE_TYPE, sizeof(ODINNT_SOFTWARE_TYPE));

   sprintf(digbuf, "%d.%d", ODINNT_MAJOR_VERSION, ODINNT_MINOR_VERSION);
   RegSetValueExA(hkey,"CurrentVersion",0,REG_SZ, (LPBYTE)digbuf, strlen(digbuf)+1);

   val = (DWORD)time(NULL); //todo: Correct format???
   RegSetValueExA(hkey,"InstallDate",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   
   RegCloseKey(hkey);
   //todo: productid, registered org/owner, sourcepath,    

   //Create Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders keys
   //"Favorites"="C:\WINDOWS\Favorites"
   //"StartUp"="C:\WINDOWS\Start Menu\Programs\Startup"
   //"Desktop"="C:\WINDOWS\Desktop"
   //"Programs"="C:\WINDOWS\Start Menu\Programs"
   //"Fonts"="C:\WINDOWS\Fonts"
   //"SendTo"="C:\WINDOWS\SendTo"
   //"Start Menu"="C:\WINDOWS\Start Menu"
   //"Templates"="C:\WINDOWS\ShellNew"
   //"Recent"="C:\WINDOWS\Recent"
   //"NetHood"="C:\WINDOWS\NetHood"
   //"Personal"="C:\My Documents"

   char shellpath[260];

   if(RegCreateKeyA(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",&hkey)!=ERROR_SUCCESS) {
   	dprintf(("InitRegistry: Unable to register system information (3)"));
	return FALSE;
   }
//   if(RegOpenKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", &hkey) != ERROR_SUCCESS) 
//   {
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Favorites");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Favorites",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Programs\\Startup");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Startup",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Desktop");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Desktop",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Start Menu\\Programs");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Programs",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Fonts");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Fonts",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\SendTo");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"SendTo",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Start Menu");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Start Menu",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\ShellNew");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Templates",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Recent");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Recent",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\NetHood");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"NetHood",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\My Documents");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Personal",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
//   }
   RegCloseKey(hkey);
   return TRUE;
}
//******************************************************************************
//******************************************************************************
