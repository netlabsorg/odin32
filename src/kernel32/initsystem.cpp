/* $Id: initsystem.cpp,v 1.9 2000-04-29 18:26:58 sandervl Exp $ */
/*
 * Odin system initialization (registry & directories)
 *
 * Called from the WarpIn install program to create the desktop directories and 
 * to setup the registry
 *
 * InitSystemAndRegistry creates:
 *	- WINDOWSDIR\SYSTEM
 *      - WINDOWSDIR\AppData
 *      - WINDOWSDIR\Cache
 *      - WINDOWSDIR\Cookies
 *	- WINDOWSDIR\Desktop
 *	- WINDOWSDIR\Favorites
 *	- WINDOWSDIR\Fonts
 *      - WINDOWSDIR\History
 *	- WINDOWSDIR\NetHood
 *	- WINDOWSDIR\My Documents
 *	- WINDOWSDIR\PrintHood
 *	- WINDOWSDIR\Recent
 *	- WINDOWSDIR\SendTo
 *	- WINDOWSDIR\Start Menu
 *	- WINDOWSDIR\Start Menu\Programs
 *	- WINDOWSDIR\Start Menu\Programs\Startup
 *	- WINDOWSDIR\ShellNew
 *	- and a minimal system registry
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
#include <odininst.h>
#include "directory.h"
#include <versionos2.h>

#define DBG_LOCALLOG	DBG_initsystem
#include "dbglocal.h"

#define DDRAW_CLASSID 		"{D7B70EE0-4340-11CF-B063-0020AFC2CD35}"
#define DDRAW_DEFAULT		"DirectDraw Object"
#define DDRAW_CLIPPER_CLASSID	"{593817A0-7DB3-11CF-A2DE-00AA00B93356}"
#define DDRAW_CLIPPER_DEFAULT	"DirectDraw Clipper Object"
#define DDRAW_DLL		"ddraw.dll"
#define DSOUND_CLASSID		"{47D4D946-62E8-11cf-93BC-444553540000}"
#define DSOUND_DEFAULT		"DirectSound Object"
#define DSOUND_DLL		"dsound.dll"
#define COM_CLASS_ID		"CLSID"
#define COM_INPROCSERVER        "InprocServer32"
#define COM_THREADMODEL		"ThreadingModel"
#define THREAD_BOTH		"Both"
#define INITREG_ERROR		"InitRegistry: Unable to register system information"

//******************************************************************************
//******************************************************************************
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
BOOL InitSystemAndRegistry()
{
 HKEY hkey, hkey1;
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
	//SYSTEM dir
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\SYSTEM");
   	CreateDirectoryA(shellpath, NULL);

	//AppData
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Application Data");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"AppData",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Cache
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Temporary Internet Files");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Cache",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Cookies
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Cookies");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Cookies",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Desktop
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Desktop");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Desktop",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Favorites
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Favorites");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Favorites",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Fonts
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Fonts");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Fonts",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//History
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\History");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"History",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//NetHood
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\NetHood");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"NetHood",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Personal
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\My Documents");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Personal",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//PrintHood
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\PrintHood");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"PrintHood",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Recent
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Recent");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Recent",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//SendTo
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\SendTo");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"SendTo",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Start Menu
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Start Menu");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Start Menu",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Programs
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\Start Menu\\Programs");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Programs",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//Startup
	strcat(shellpath, "\\Startup");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Startup",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
	//ShellNew
   	strcpy(shellpath, InternalGetWindowsDirectoryA());
   	strcat(shellpath, "\\ShellNew");
   	CreateDirectoryA(shellpath, NULL);
   	RegSetValueExA(hkey,"Templates",0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
//   }
   RegCloseKey(hkey);

   //Now the Ddraw & dsound registry keys:
   //HKEY_CLASSES_ROOT\DirectDraw = DirectDraw Object
   //HKEY_CLASSES_ROOT\DirectDraw\CLSID = {D7B70EE0-4340-11CF-B063-0020AFC2CD35}
   //HKEY_CLASSES_ROOT\CLSID\{D7B70EE0-4340-11CF-B063-0020AFC2CD35} = DirectDraw Object
   //HKEY_CLASSES_ROOT\CLSID\{D7B70EE0-4340-11CF-B063-0020AFC2CD35}\InprocServer32 = ddraw.dll
   if(RegCreateKeyA(HKEY_CLASSES_ROOT,"DirectDraw",&hkey)!=ERROR_SUCCESS) {
   	goto initreg_error;
   }
   RegSetValueExA(hkey, "", 0, REG_SZ, (LPBYTE)DDRAW_DEFAULT, sizeof(DDRAW_DEFAULT));
   if(RegCreateKeyA(hkey,COM_CLASS_ID,&hkey1)!=ERROR_SUCCESS) {
   	RegCloseKey(hkey);
   	goto initreg_error;
   }
   RegSetValueExA(hkey1,"",0,REG_SZ, (LPBYTE)DDRAW_CLASSID, sizeof(DDRAW_CLASSID));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_CLASSES_ROOT, COM_CLASS_ID"\\"DDRAW_CLASSID ,&hkey)!=ERROR_SUCCESS) {
   	goto initreg_error;
   }
   RegSetValueExA(hkey,"",0,REG_SZ, (LPBYTE)DDRAW_DEFAULT, sizeof(DDRAW_DEFAULT));
   if(RegCreateKeyA(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
   	RegCloseKey(hkey);
   	goto initreg_error;
   }
   RegSetValueExA(hkey1,"",0,REG_SZ, (LPBYTE)DDRAW_DLL, sizeof(DDRAW_DLL));
   RegSetValueExA(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //HKEY_CLASSES_ROOT\DirectDrawClipper = DirectDraw Clipper Object
   //HKEY_CLASSES_ROOT\DirectDrawClipper\CLSID = {593817A0-7DB3-11CF-A2DE-00AA00B93356}
   //HKEY_CLASSES_ROOT\CLSID\{593817A0-7DB3-11CF-A2DE-00AA00B93356} = DirectDraw Clipper Object
   //HKEY_CLASSES_ROOT\CLSID\{593817A0-7DB3-11CF-A2DE-00AA00B93356}\InprocServer32 = ddraw.dll
   if(RegCreateKeyA(HKEY_CLASSES_ROOT,"DirectDrawClipper",&hkey)!=ERROR_SUCCESS) {
   	goto initreg_error;
   }
   RegSetValueExA(hkey,"",0,REG_SZ, (LPBYTE)DDRAW_CLIPPER_DEFAULT, sizeof(DDRAW_CLIPPER_DEFAULT));
   if(RegCreateKeyA(hkey,COM_CLASS_ID,&hkey1)!=ERROR_SUCCESS) {
   	RegCloseKey(hkey);
   	goto initreg_error;
   }
   RegSetValueExA(hkey1,"",0,REG_SZ, (LPBYTE)DDRAW_CLIPPER_CLASSID, sizeof(DDRAW_CLIPPER_CLASSID));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_CLASSES_ROOT, COM_CLASS_ID"\\"DDRAW_CLIPPER_CLASSID,&hkey)!=ERROR_SUCCESS) {
   	goto initreg_error;
   }
   RegSetValueExA(hkey,"",0,REG_SZ, (LPBYTE)DDRAW_CLIPPER_DEFAULT, sizeof(DDRAW_CLIPPER_DEFAULT));
   if(RegCreateKeyA(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
   	RegCloseKey(hkey);
   	goto initreg_error;
   }
   RegSetValueExA(hkey1,"",0,REG_SZ, (LPBYTE)DDRAW_DLL, sizeof(DDRAW_DLL));
   RegSetValueExA(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //HKEY_CLASSES_ROOT\DirectSound = DirectSound Object
   //HKEY_CLASSES_ROOT\DirectSound\CLSID = {47D4D946-62E8-11cf-93BC-444553540000}
   //HKEY_CLASSES_ROOT\CLSID\{47D4D946-62E8-11cf-93BC-444553540000} = DirectSound Object
   //HKEY_CLASSES_ROOT\CLSID\{47D4D946-62E8-11cf-93BC-444553540000}\InprocServer32 = dsound.dll
   if(RegCreateKeyA(HKEY_CLASSES_ROOT,"DirectSound",&hkey)!=ERROR_SUCCESS) {
   	goto initreg_error;
   }
   RegSetValueExA(hkey,"",0,REG_SZ, (LPBYTE)DSOUND_DEFAULT, sizeof(DSOUND_DEFAULT));
   if(RegCreateKeyA(hkey,COM_CLASS_ID,&hkey1)!=ERROR_SUCCESS) {
   	RegCloseKey(hkey);
   	goto initreg_error;
   }
   RegSetValueExA(hkey1,"",0,REG_SZ, (LPBYTE)DSOUND_CLASSID, sizeof(DSOUND_CLASSID));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_CLASSES_ROOT, COM_CLASS_ID"\\"DSOUND_CLASSID ,&hkey)!=ERROR_SUCCESS) {
   	goto initreg_error;
   }
   RegSetValueExA(hkey,"",0,REG_SZ, (LPBYTE)DSOUND_DEFAULT, sizeof(DSOUND_DEFAULT));
   if(RegCreateKeyA(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
   	RegCloseKey(hkey);
   	goto initreg_error;
   }
   RegSetValueExA(hkey1,"",0,REG_SZ, (LPBYTE)DSOUND_DLL, sizeof(DSOUND_DLL));
   RegSetValueExA(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);
   return TRUE;

initreg_error:
   dprintf((INITREG_ERROR));
   return FALSE;
}
//******************************************************************************
//******************************************************************************
