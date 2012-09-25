/* $Id: odininst.cpp,v 1.14 2003-07-16 10:51:10 sandervl Exp $ */
/*
 * Odin WarpIn installation app
 *
 *  Creates:
 *  - SYSTEMDIR\drivers
 *  - SYSTEMDIR\drivers\etc
 *  - WINDOWSDIR\SYSTEM
 *  - WINDOWSDIR\AppData
 *  - WINDOWSDIR\Cache
 *  - WINDOWSDIR\Cookies
 *  - WINDOWSDIR\Desktop
 *  - WINDOWSDIR\Favorites
 *  - WINDOWSDIR\Fonts
 *  - WINDOWSDIR\History
 *  - WINDOWSDIR\NetHood
 *  - WINDOWSDIR\My Documents
 *  - WINDOWSDIR\PrintHood
 *  - WINDOWSDIR\Recent
 *  - WINDOWSDIR\SendTo
 *  - WINDOWSDIR\Start Menu
 *  - WINDOWSDIR\Start Menu\Programs
 *  - WINDOWSDIR\Start Menu\Programs\Startup
 *  - WINDOWSDIR\ShellNew
 *  - WINDOWSDIR\Program Files
 *  - WINDOWSDIR\Program Files\Common Files
 *  - WINDOWSDIR\Temp
 *  - and a minimal system registry
 *
 * Copyright 1999-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
#include <heapstring.h>
#include <ctype.h>
#include "winreg.h"
#include "global.h"
#include "winnt.h"
#include "winerror.h"
#include "winreg.h"
#include "winnls.h"
#include "debugtools.h"
#include <odininst.h>
#include <win/options.h>
#include <versionos2.h>
#include <time.h>

BOOL CreateSystemDirectories();
BOOL SetupControlPanelKeys();
BOOL InitSystemAndRegistry();
void SetupTimeZoneInfo();
void RegisterDLLs();

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

static char DIR_Windows[260];
static char DIR_System[260];

//******************************************************************************
//******************************************************************************
int main(int argc, char *argv[])
{
  char *installdate;
  time_t anytime;

  time(&anytime);
  installdate = asctime(localtime(&anytime));
  PROFILE_SetOdinIniString(ODINSYSTEM_SECTION, "INSTALLDATE", installdate);

  InitSystemAndRegistry();
  CreateSystemDirectories();
  SetupControlPanelKeys();

  SetupTimeZoneInfo();

  RegisterDLLs();

  return 0;
}
//******************************************************************************
//******************************************************************************
char *InternalGetWindowsDirectory()
{
 static char fInit = FALSE;

   if(fInit == FALSE) {
       GetWindowsDirectory(DIR_Windows, sizeof(DIR_Windows)-1);
       fInit = TRUE;
   }
   return DIR_Windows;
}
//******************************************************************************
//******************************************************************************
char *InternalGetSystemDirectory()
{
 static char fInit = FALSE;

   if(fInit == FALSE) {
       GetSystemDirectory(DIR_System, sizeof(DIR_System)-1);
       fInit = TRUE;
   }
   return DIR_System;
}
//******************************************************************************
//******************************************************************************
BOOL InitSystemAndRegistry()
{
 HKEY hkey, hkey1;
 char *buf;
 DWORD val;
 char  digbuf[16];
 char  shellpath[260];

    if(RegCreateKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Windows",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information\n"));
        return FALSE;
    }
    buf = InternalGetWindowsDirectory();
    RegSetValueEx(hkey,"Directory",0,REG_BINARY, (LPBYTE)buf, strlen(buf)+1);
    buf = InternalGetSystemDirectory();
    RegSetValueEx(hkey,"SystemDirectory",0,REG_BINARY, (LPBYTE)buf, strlen(buf)+1);
    val = 0;
    RegSetValueEx(hkey,"ErrorMode",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
    val = 0;
    RegSetValueEx(hkey,"NoInteractiveServices",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
    val = ODINNT_BUILD_NR;
    RegSetValueEx(hkey,"CSDVersion",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
    RegSetValueEx(hkey,"ShutdownTime",0,REG_DWORD, (LPBYTE)ShutdownTime, sizeof(ShutdownTime));
    RegCloseKey(hkey);

    //Software\Microsoft\Windows\CurrentVersion\RunOnce
    if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information (2)"));
        return FALSE;
    }
    RegCloseKey(hkey);

    //System\CurrentControlSet\Control\Session Manager
    if(RegCreateKey(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Control\\Session Manager",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information (2)"));
        return FALSE;
    }
    RegCloseKey(hkey);

    if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\iexplore.exe",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information (2)"));
        return FALSE;
    }
    strcpy(shellpath, InternalGetWindowsDirectory());
    strcat(shellpath, "\\IEXPLORE.EXE");
    RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
    RegCloseKey(hkey);


    if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information (2)"));
        return FALSE;
    }
    buf = InternalGetSystemDirectory();
    RegSetValueEx(hkey,"SystemRoot",0,REG_SZ, (LPBYTE)buf, strlen(buf)+1);
    RegSetValueEx(hkey,"PathName",0,REG_SZ, (LPBYTE)buf, strlen(buf)+1);
    sprintf(digbuf, "%d", ODINNT_BUILD_NR);
    RegSetValueEx(hkey,"CurrentBuildNumber",0,REG_SZ, (LPBYTE)digbuf, strlen(digbuf)+1);
    RegSetValueEx(hkey,"CurrentType",0,REG_SZ, (LPBYTE)ODINNT_OSTYPE_UNI, sizeof(ODINNT_OSTYPE_UNI));
    RegSetValueEx(hkey,"CSDVersion",0,REG_SZ, (LPBYTE)ODINNT_CSDVERSION, sizeof(ODINNT_CSDVERSION));
    RegSetValueEx(hkey,"SoftwareType",0,REG_SZ, (LPBYTE)ODINNT_SOFTWARE_TYPE, sizeof(ODINNT_SOFTWARE_TYPE));

    sprintf(digbuf, "%d.%d", ODINNT_MAJOR_VERSION, ODINNT_MINOR_VERSION);
    RegSetValueEx(hkey,"CurrentVersion",0,REG_SZ, (LPBYTE)digbuf, strlen(digbuf)+1);

    val = (DWORD)time(NULL); //todo: Correct format???
    RegSetValueEx(hkey,"InstallDate",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));

    RegCloseKey(hkey);
    //todo: productid, registered org/owner, sourcepath,

//#
//# Entries for OLE32 (COM/OLE base)
//#
//
//# OLE32's built-in marshaler, handles standard interfaces such as IClassFactory.
//# (PSFactoryBuffer = Proxy/Stub factory)
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00000320-0000-0000-C000-000000000046}]
//@="PSFactoryBuffer"
//
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00000320-0000-0000-C000-000000000046}\InProcServer32]
//@="ole32.dll"
//"ThreadingModel"="Both"
   #define PSFACTORYBUFFER "PSFactoryBuffer"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00000320-0000-0000-C000-000000000046}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)PSFACTORYBUFFER, sizeof(PSFACTORYBUFFER));
   RegCloseKey(hkey);
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00000320-0000-0000-C000-000000000046}\\InProcServer32",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_OLE32DLL, sizeof(CLASS_OLE32DLL));
   RegSetValueEx(hkey, COM_THREADMODEL, 0, REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey);

//# IUnknown, the superclass for everything COM/OLE.
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Interface\{00000000-0000-0000-C000-000000000046}]
//@="IUnknown"
//
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Interface\{00000000-0000-0000-C000-000000000046}\BaseInterface]
//@=""
//
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Interface\{00000000-0000-0000-C000-000000000046}\NumMethods]
//@="3"
   #define IUNKNOWN "IUnknown"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00000000-0000-0000-C000-000000000046}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)IUNKNOWN, sizeof(IUNKNOWN));
   RegCloseKey(hkey);
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00000000-0000-0000-C000-000000000046}\\BaseInterface",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)"", 1);
   RegCloseKey(hkey);
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00000000-0000-0000-C000-000000000046}\\NumMethods",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)"3", 2);
   RegCloseKey(hkey);

//# IClassFactory, standard interface for creating instances of classes.
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Interface\{00000001-0000-0000-C000-000000000046}]
//@="IClassFactory"
//
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Interface\{00000001-0000-0000-C000-000000000046}\NumMethods]
//@="5"
//
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Interface\{00000001-0000-0000-C000-000000000046}\ProxyStubClsid32]
//@="{00000320-0000-0000-C000-000000000046}"
   #define ICLASSFACTORY "IClassFactory"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00000001-0000-0000-C000-000000000046}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)ICLASSFACTORY, sizeof(ICLASSFACTORY));
   RegCloseKey(hkey);
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00000001-0000-0000-C000-000000000046}\\NumMethods",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)"5", 2);
   RegCloseKey(hkey);
   #define PSFACTORY_GUID "{00000320-0000-0000-C000-000000000046}"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00000001-0000-0000-C000-000000000046}\\ProxyStubClsid32",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)PSFACTORY_GUID, sizeof(PSFACTORY_GUID));
   RegCloseKey(hkey);

//#
//# Entries for OLEAUT32 (OLE Automation)
//#
//
//# The Universal Marshaler, also known as the Type Library Marshaler.
//# (PSOAInterface = Proxy/Stub OLE Automation interface)
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00020424-0000-0000-C000-000000000046}]
//@="PSOAInterface"
//
//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00020424-0000-0000-C000-000000000046}\InProcServer32]
//@="oleaut32.dll"
//"ThreadingModel"="Both"
   #define PSOAINTERFACE "PSOAInterface"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00020424-0000-0000-C000-000000000046}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)PSOAINTERFACE, sizeof(PSOAINTERFACE));
   RegCloseKey(hkey);
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00020424-0000-0000-C000-000000000046}\\InProcServer32",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_OLEAUT32DLL, sizeof(CLASS_OLEAUT32DLL));
   RegSetValueEx(hkey, COM_THREADMODEL, 0, REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey);


   //Shell32 & IE related keys
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021400-0000-0000-C000-000000000046}]
   //@="Desktop"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021400-0000-0000-C000-000000000046}\InProcServer32]
   //@="shell32.dll"
   //ThreadingModel="Apartment"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021400-0000-0000-C000-000000000046}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_DESKTOP, sizeof(CLASS_DESKTOP));
   RegCloseKey(hkey);
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021400-0000-0000-C000-000000000046}\\InProcServer32",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_SHELL32DLL, sizeof(CLASS_SHELL32DLL));
   RegSetValueEx(hkey, COM_THREADMODEL, 0, REG_SZ, (LPBYTE)COM_THREAD_APARTMENT, sizeof(COM_THREAD_APARTMENT));
   RegCloseKey(hkey);
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021401-0000-0000-C000-000000000046}]
   //@="Shortcut"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021401-0000-0000-C000-000000000046}\InProcServer32]
   //@="shell32.dll"
   //"ThreadingModel"="Apartment"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021401-0000-0000-C000-000000000046}\shellex\MayChangeDefaultMenu]
   //@=""
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021401-0000-0000-C000-000000000046}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_SHORTCUT, sizeof(CLASS_SHORTCUT));
   RegCloseKey(hkey);
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021401-0000-0000-C000-000000000046}\\InProcServer32",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_SHELL32DLL, sizeof(CLASS_SHELL32DLL));
   RegSetValueEx(hkey, COM_THREADMODEL, 0, REG_SZ, (LPBYTE)COM_THREAD_APARTMENT, sizeof(COM_THREAD_APARTMENT));
   RegCloseKey(hkey);
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021401-0000-0000-C000-000000000046}\\shellex\\MayChangeDefaultMenu",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)"", 0);
   RegCloseKey(hkey);

   //# Entries for IWebBrowser
   //# Used by Internet Explorer HTML-rendering control
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{8856f961-340a-11d0-a96b-00c04fd705a2}]
   //@="Shortcut"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{8856f961-340a-11d0-a96b-00c04fd705a2}\InProcServer32]
   //@="shdocvw.dll"
   //"ThreadingModel"="Apartment"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{8856f961-340a-11d0-a96b-00c04fd705a2}\shellex\MayChangeDefaultMenu]
   //@=""

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{8856f961-340a-11d0-a96b-00c04fd705a2}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_SHORTCUT, sizeof(CLASS_SHORTCUT));
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{8856f961-340a-11d0-a96b-00c04fd705a2}\\InProcServer32",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_SHDOCVW, sizeof(CLASS_SHDOCVW));
   RegSetValueEx(hkey, COM_THREADMODEL, 0, REG_SZ, (LPBYTE)COM_THREAD_APARTMENT, sizeof(COM_THREAD_APARTMENT));
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{8856f961-340a-11d0-a96b-00c04fd705a2}\\shellex\\MayChangeDefaultMenu",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)"", 1);
   RegCloseKey(hkey);

   //Now the Ddraw & dsound registry keys:
   //HKEY_CLASSES_ROOT\DirectDraw = DirectDraw Object
   //HKEY_CLASSES_ROOT\DirectDraw\CLSID = {D7B70EE0-4340-11CF-B063-0020AFC2CD35}
   //HKEY_CLASSES_ROOT\CLSID\{D7B70EE0-4340-11CF-B063-0020AFC2CD35} = DirectDraw Object
   //HKEY_CLASSES_ROOT\CLSID\{D7B70EE0-4340-11CF-B063-0020AFC2CD35}\InprocServer32 = ddraw.dll
   if(RegCreateKey(HKEY_CLASSES_ROOT,"DirectDraw",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "", 0, REG_SZ, (LPBYTE)DDRAW_DEFAULT, sizeof(DDRAW_DEFAULT));
   if(RegCreateKey(hkey,COM_CLASS_ID,&hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)DDRAW_CLASSID, sizeof(DDRAW_CLASSID));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_CLASSES_ROOT, COM_CLASS_ID"\\"DDRAW_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)DDRAW_DEFAULT, sizeof(DDRAW_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)DDRAW_DLL, sizeof(DDRAW_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //HKEY_CLASSES_ROOT\DirectDrawClipper = DirectDraw Clipper Object
   //HKEY_CLASSES_ROOT\DirectDrawClipper\CLSID = {593817A0-7DB3-11CF-A2DE-00AA00B93356}
   //HKEY_CLASSES_ROOT\CLSID\{593817A0-7DB3-11CF-A2DE-00AA00B93356} = DirectDraw Clipper Object
   //HKEY_CLASSES_ROOT\CLSID\{593817A0-7DB3-11CF-A2DE-00AA00B93356}\InprocServer32 = ddraw.dll
   if(RegCreateKey(HKEY_CLASSES_ROOT,"DirectDrawClipper",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)DDRAW_CLIPPER_DEFAULT, sizeof(DDRAW_CLIPPER_DEFAULT));
   if(RegCreateKey(hkey,COM_CLASS_ID,&hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)DDRAW_CLIPPER_CLASSID, sizeof(DDRAW_CLIPPER_CLASSID));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_CLASSES_ROOT, COM_CLASS_ID"\\"DDRAW_CLIPPER_CLASSID,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)DDRAW_CLIPPER_DEFAULT, sizeof(DDRAW_CLIPPER_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)DDRAW_DLL, sizeof(DDRAW_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //HKEY_CLASSES_ROOT\DirectSound = DirectSound Object
   //HKEY_CLASSES_ROOT\DirectSound\CLSID = {47D4D946-62E8-11cf-93BC-444553540000}
   //HKEY_CLASSES_ROOT\CLSID\{47D4D946-62E8-11cf-93BC-444553540000} = DirectSound Object
   //HKEY_CLASSES_ROOT\CLSID\{47D4D946-62E8-11cf-93BC-444553540000}\InprocServer32 = dsound.dll
   if(RegCreateKey(HKEY_CLASSES_ROOT,"DirectSound",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)DSOUND_DEFAULT, sizeof(DSOUND_DEFAULT));
   if(RegCreateKey(hkey,COM_CLASS_ID,&hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)DSOUND_CLASSID, sizeof(DSOUND_CLASSID));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_CLASSES_ROOT, COM_CLASS_ID"\\"DSOUND_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)DSOUND_DEFAULT, sizeof(DSOUND_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)DSOUND_DLL, sizeof(DSOUND_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //DirectPlay
   if(RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\"DPLAYX_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)DPLAYX_DEFAULT, sizeof(DPLAYX_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)DPLAYX_DLL, sizeof(DPLAYX_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //DirectPlay Lobby
   if(RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\"DPLAYX_LOBBY_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)DPLAYX_LOBBY_DEFAULT, sizeof(DPLAYX_LOBBY_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)DPLAYX_LOBBY_DLL, sizeof(DPLAYX_LOBBY_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

//#
//# Entries for quartz.dll
//#

   //Quartz.dll keys
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{CDA42200-BD88-11d0-BD4E-00A0C911CE86}]
   //@="Filter Mapper2"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{CDA42200-BD88-11d0-BD4E-00A0C911CE86}\InprocServer32]
   //@="G:\\WINNT\\System32\\quartz.dll"
   //"ThreadingModel"="Both"
   if(RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\"QUARTZ_FILTER_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)QUARTZ_FILTER_DEFAULT, sizeof(QUARTZ_FILTER_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)QUARTZ_DLL, sizeof(QUARTZ_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{CDBD8D00-C193-11D0-BD4E-00A0C911CE86}]
   //@="CMediaPropertyBag"
   //
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{CDBD8D00-C193-11D0-BD4E-00A0C911CE86}\InprocServer32]
   //@="G:\\WINNT\\System32\\quartz.dll"
   //"ThreadingModel"="Both"
   if(RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\"QUARTZ_MEDIAPROP_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)QUARTZ_MEDIAPROP_DEFAULT, sizeof(QUARTZ_MEDIAPROP_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)QUARTZ_DLL, sizeof(QUARTZ_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{79376820-07D0-11CF-A24D-0020AFD79767}]
   //@="DirectSound Audio Renderer"
   //
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{79376820-07D0-11CF-A24D-0020AFD79767}\InprocServer32]
   //@="G:\\WINNT\\System32\\quartz.dll"
   //"ThreadingModel"="Both"
   if(RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\"QUARTZ_DSOUNDREND_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)QUARTZ_DSOUNDREND_DEFAULT, sizeof(QUARTZ_DSOUNDREND_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)QUARTZ_DLL, sizeof(QUARTZ_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{e436ebb4-524f-11ce-9f53-0020af0ba770}]
   //@="Filter Graph Control Plug In Distributor"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{e436ebb4-524f-11ce-9f53-0020af0ba770}\InprocServer32]
   //@="G:\\WINNT\\System32\\quartz.dll"
   //"ThreadingModel"="Both"
   if(RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\"QUARTZ_FILTERGRAPHDIST_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)QUARTZ_FILTERGRAPHDIST_DEFAULT, sizeof(QUARTZ_FILTERGRAPHDIST_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)QUARTZ_DLL, sizeof(QUARTZ_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);


#if 0
   if(RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\E436EBB3-524F-11CE-9F53-0020AF0BA770" ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)QUARTZ_FILTERGRAPHDIST_DEFAULT, sizeof(QUARTZ_FILTERGRAPHDIST_DEFAULT));
   if(RegCreateKey(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueEx(hkey1,"",0,REG_SZ, (LPBYTE)QUARTZ_DLL, sizeof(QUARTZ_DLL));
   RegSetValueEx(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

# CLSID_FilterGraph
[HKEY_CLASSES_ROOT\CLSID\{E436EBB3-524F-11CE-9F53-0020AF0BA770}\InprocServer32]
@="quartz.dll"
"ThreadingModel"="Both"

# CLSID_SystemClock
[HKEY_CLASSES_ROOT\CLSID\{E436EBB1-524F-11CE-9F53-0020AF0BA770}\InprocServer32]
@="quartz.dll"
"ThreadingModel"="Both"

# CLSID_MemoryAllocator
[HKEY_CLASSES_ROOT\CLSID\{1E651CC0-B199-11D0-8212-00C04FC32C45}\InprocServer32]
@="quartz.dll"
"ThreadingModel"="Both"
#endif

   //[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion]
   //"ProgramFilesDir"="C:\Program Files"
   //"CommonFilesDir"="C:\Program Files\Common Files"
   //# This is intended for a centrally managed (server) directory where system files and e.g. fonts can reside. Most installs have this set to C:\WINDOWS, though.
   //"SharedDir"="C:\WINDOWS"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }

   //Create Program Files directory
   strcpy(shellpath, InternalGetWindowsDirectory());
   strcat(shellpath, "\\Program Files");
   CreateDirectory(shellpath, NULL);
   RegSetValueEx(hkey, DIR_PROGRAM, 0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);

   //Create Program Files\Common Files directory
   strcat(shellpath, "\\Common Files");
   CreateDirectory(shellpath, NULL);
   RegSetValueEx(hkey, DIR_PROGRAM_COMMON, 0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);

   strcpy(shellpath, InternalGetWindowsDirectory());
   RegSetValueEx(hkey, DIR_SHARED, 0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);

   RegCloseKey(hkey);

   //[HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\VIDEO]
   //"\\Device\\Video0"="\\REGISTRY\\Machine\\System\\ControlSet001\\Services\\mga64\\Device0"
   //"\\Device\\Video1"="\\REGISTRY\\Machine\\System\\ControlSet001\\Services\\VgaSave\\Device0"
   //"VgaCompatible"="\\Device\\Video1"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\VIDEO",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"\\Device\\Video0",0,REG_SZ, (LPBYTE)HARDWARE_VIDEO_GRADD, sizeof(HARDWARE_VIDEO_GRADD));
   RegSetValueEx(hkey,"\\Device\\Video1",0,REG_SZ, (LPBYTE)HARDWARE_VIDEO_VGA, sizeof(HARDWARE_VIDEO_VGA));
   RegSetValueEx(hkey, "VgaCompatible", 0,REG_SZ, (LPBYTE)HARDWARE_VIDEO_COMPATIBLE, sizeof(HARDWARE_VIDEO_COMPATIBLE));
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Gradd\\Device0",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"Device Description", 0, REG_SZ, (LPBYTE)HARDWARE_VIDEO_GRADD_DESCRIPTION, sizeof(HARDWARE_VIDEO_GRADD_DESCRIPTION));
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\VgaSave\\Device0",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"Device Description", 0, REG_SZ, (LPBYTE)HARDWARE_VIDEO_VGA_DESCRIPTION, sizeof(HARDWARE_VIDEO_VGA_DESCRIPTION));
   RegCloseKey(hkey);

   //Software\Microsoft\Multimedia\Sound Mapper
   if(RegCreateKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Multimedia\\Sound Mapper",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"Playback", 0, REG_SZ, (LPBYTE)ODIN_WINMM_PLAYBACK, sizeof(ODIN_WINMM_PLAYBACK));
   RegSetValueEx(hkey,"Record", 0, REG_SZ, (LPBYTE)ODIN_WINMM_RECORD, sizeof(ODIN_WINMM_RECORD));
   RegCloseKey(hkey);

   //Software\Microsoft\DirectX
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectX",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey,"",0,REG_SZ, (LPBYTE)"", 0);
   RegSetValueEx(hkey, "RC", 0,REG_SZ, (LPBYTE)DIRECTX_RC, sizeof(DIRECTX_RC));
   RegSetValueEx(hkey, "Version", 0,REG_SZ, (LPBYTE)DIRECTX_VERSION, sizeof(DIRECTX_VERSION));
   val = DIRECTX_INSTALLED_VERSION;
   RegSetValueEx(hkey, "InstalledVersion", 0,REG_BINARY, (LPBYTE)&val, sizeof(DWORD));
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectDraw",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   //todo
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Direct3D",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   //todo
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectPlay",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   //todo
   RegCloseKey(hkey);

#if 0
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectMusic",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   //todo
   RegCloseKey(hkey);
#endif

// [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Cdfs]
// "Type"=dword:00000002
// "Start"=dword:00000004
// "Group"="File system"
// "ErrorControl"=dword:00000001
// "DependOnGroup"=hex(7):53,43,53,49,20,43,44,52,4f,4d,20,43,6c,61,73,73,00,00

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Cdfs",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   val = 0x2;
   RegSetValueEx(hkey, KEY_DEVICE_TYPE,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x4;
   RegSetValueEx(hkey, KEY_DEVICE_START,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x1;
   RegSetValueEx(hkey, KEY_DEVICE_ERRORCONTROL,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   RegSetValueEx(hkey, KEY_DEVICE_GROUP,0,REG_SZ, (LPBYTE)DEVICE_GROUP_FILESYSTEM, sizeof(DEVICE_GROUP_FILESYSTEM));
   //todo dependongroup
   RegCloseKey(hkey);


/*
// [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Cdfs\Enum]
// "0"="Root\\LEGACY_CDFS\\0000"
// "Count"=dword:00000001
// "NextInstance"=dword:00000001
*/
// [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Cdrom]
// "Type"=dword:00000001
// "Start"=dword:00000001
// "Group"="SCSI CDROM Class"
// "ErrorControl"=dword:00000000
// "Tag"=dword:00000002
// "DependOnGroup"=hex(7):53,43,53,49,20,6d,69,6e,69,70,6f,72,74,00,00
// "Autorun"=dword:00000001

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Cdrom",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   val = 0x1;
   RegSetValueEx(hkey, KEY_DEVICE_TYPE,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x1;
   RegSetValueEx(hkey, KEY_DEVICE_START,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x0;
   RegSetValueEx(hkey, KEY_DEVICE_ERRORCONTROL,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x2;
   RegSetValueEx(hkey, KEY_DEVICE_TAG,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x0;
   RegSetValueEx(hkey, KEY_DEVICE_AUTORUN,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   RegSetValueEx(hkey, KEY_DEVICE_GROUP,0,REG_SZ, (LPBYTE)DEVICE_GROUP_SCSICDROM, sizeof(DEVICE_GROUP_SCSICDROM));
   //todo dependongroup
   RegCloseKey(hkey);

/*
[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Cdrom\Enum]
"0"="Root\\LEGACY_CDROM\\0000"
"Count"=dword:00000001
"NextInstance"=dword:00000001
*/

   //[HKEY_LOCAL_MACHINE\SYSTEM\ControlSet002\Control\FileSystem]
   //"Win31FileSystem"=dword:00000000
   //"NtfsDisable8dot3NameCreation"=dword:00000000
   //"Win95TruncatedExtensions"=dword:00000001
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\FileSystem",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   val = 0x0;
   RegSetValueEx(hkey, "Win31FileSystem",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x0;
   RegSetValueEx(hkey, "NtfsDisable8dot3NameCreation",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x1;
   RegSetValueEx(hkey, "Win95TruncatedExtensions",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   RegCloseKey(hkey);

//[HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM]
//"Serial1"="COM2"
////   if(RegCreateKey(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\SERIALCOMM",&hkey)!=ERROR_SUCCESS) {
////    goto initreg_error;
////   }
////   RegSetValueEx(hkey, "Serial1",0,REG_SZ, (LPBYTE)"COM2", 5);
////   RegCloseKey(hkey);


//[HKEY_LOCAL_MACHINE\Software\Microsoft\OLE]
//# allow cross-machine calls (RPC) (default Y)
//"EnableDCOM"="Y"
//# allow incoming connections ? (def. N)
//"EnableRemoteConnect"="N"
   if(RegCreateKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\OLE",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   digbuf[0] = 'Y';
   digbuf[1] = 0;
   RegSetValueEx(hkey, "EnableDCOM",0,REG_SZ, (LPBYTE)digbuf, 2);
   digbuf[0] = 'N';
   digbuf[1] = 0;
   RegSetValueEx(hkey, "EnableRemoteConnect",0,REG_SZ, (LPBYTE)digbuf, 2);
   RegCloseKey(hkey);

   //Add MS Sans Serif to WarpSans font conversion entry
   char temp;
   if(PROFILE_GetOdinIniString(ODINFONTSECTION, "MS Sans Serif", "", &temp,
                               0) <= 1)
   {
       PROFILE_SetOdinIniString(ODINFONTSECTION, "MS Sans Serif", "WarpSans");
   }
   //Add MS Shell Dlg
   if(PROFILE_GetOdinIniString(ODINFONTSECTION, "MS Shell Dlg", "", &temp,
                               0) <= 1)
   {
       PROFILE_SetOdinIniString(ODINFONTSECTION, "MS Shell Dlg", "WarpSans");
   }
   //Add MS Shell Dlg 2 (win2k and up) too
   if(PROFILE_GetOdinIniString(ODINFONTSECTION, "MS Shell Dlg 2", "", &temp,
                               0) <= 1)
   {
       PROFILE_SetOdinIniString(ODINFONTSECTION, "MS Shell Dlg 2", "WarpSans");
   }

   //Create system.ini with [mci] section
   strcpy(shellpath, InternalGetWindowsDirectory());
   strcat(shellpath, "\\system.ini");

   if(GetPrivateProfileStringA(szMci, szCDAudio, szMciCDA, &temp, 0, shellpath) <= 1) {
      WritePrivateProfileStringA(szMci, szCDAudio, szMciCDA, shellpath);
   }

   //Font registry keys (not complete)
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts]
   //"Courier 10,12,15 (VGA res)"="COURE.FON"
   //"MS Sans Serif 8,10,12,14,18,24 (VGA res)"="SSERIFE.FON"
   //"MS Serif 8,10,12,14,18,24 (VGA res)"="SERIFE.FON"


   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   #define FONT_COURIER    "COURE.FON"
   #define FONT_SANSSERIF  "SSERIFE.FON"
   #define FONT_SERIF      "SERIFE.FON"
   RegSetValueEx(hkey, "Courier 10,12,15 (VGA res)",0,REG_SZ, (LPBYTE)FONT_COURIER, sizeof(FONT_COURIER));
   RegSetValueEx(hkey, "MS Sans Serif 8,10,12,14,18,24 (VGA res)",0,REG_SZ, (LPBYTE)FONT_SANSSERIF, sizeof(FONT_SANSSERIF));
   RegSetValueEx(hkey, "MS Serif 8,10,12,14,18,24 (VGA res)",0,REG_SZ, (LPBYTE)FONT_SERIF, sizeof(FONT_SERIF));
   RegCloseKey(hkey);

   if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueEx(hkey, "Courier 10,12,15 (VGA res)",0,REG_SZ, (LPBYTE)FONT_COURIER, sizeof(FONT_COURIER));
   RegSetValueEx(hkey, "MS Sans Serif 8,10,12,14,18,24 (VGA res)",0,REG_SZ, (LPBYTE)FONT_SANSSERIF, sizeof(FONT_SANSSERIF));
   RegSetValueEx(hkey, "MS Serif 8,10,12,14,18,24 (VGA res)",0,REG_SZ, (LPBYTE)FONT_SERIF, sizeof(FONT_SERIF));
   RegCloseKey(hkey);
   return TRUE;

initreg_error:
   dprintf((INITREG_ERROR));
   return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL CreateSystemDirectories()
{
 char dirname[260];
 HKEY hkey;

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

    if(RegCreateKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information (3)"));
        return FALSE;
    }
    //system32\drivers dir
    strcpy(dirname, InternalGetSystemDirectory());
    strcat(dirname, "\\Drivers");
    CreateDirectory(dirname, NULL);
    strcat(dirname, "\\etc");
    CreateDirectory(dirname, NULL);

    //SYSTEM dir
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\SYSTEM");
    CreateDirectory(dirname, NULL);

    //AppData
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Application Data");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"AppData",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Cache
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Temporary Internet Files");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Cache",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Cookies
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Cookies");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Cookies",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Desktop
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Desktop");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Desktop",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Favorites
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Favorites");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Favorites",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Fonts
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Fonts");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Fonts",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //History
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\History");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"History",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //NetHood
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\NetHood");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"NetHood",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Personal
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\My Documents");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Personal",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //PrintHood
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\PrintHood");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"PrintHood",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Recent
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Recent");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Recent",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //SendTo
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\SendTo");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"SendTo",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Start Menu
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Start Menu");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Start Menu",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);

    //Temp directory
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Temp");
    CreateDirectory(dirname, NULL);

    //Programs
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\Start Menu\\Programs");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Programs",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //Startup
    strcat(dirname, "\\Startup");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Startup",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    //ShellNew
    strcpy(dirname, InternalGetWindowsDirectory());
    strcat(dirname, "\\ShellNew");
    CreateDirectory(dirname, NULL);
    RegSetValueEx(hkey,"Templates",0,REG_SZ, (LPBYTE)dirname, strlen(dirname)+1);
    RegCloseKey(hkey);

    return TRUE;
}
//******************************************************************************
//Example:
//[HKEY_USERS\.DEFAULT\Control Panel\International]
//"Locale"="00000409"
//"sLanguage"="ENU"
//"sCountry"="United States"
//"iCountry"="1"
//"sList"=","
//"iMeasure"="1"
//"sDecimal"="."
//"sThousand"=","
//"iDigits"="2"
//"iLZero"="1"
//"sCurrency"="$"
//"iCurrDigits"="2"
//"iCurrency"="0"
//"iNegCurr"="0"
//"sDate"="/"
//"sTime"=":"
//"sShortDate"="M/d/yy"
//"sLongDate"="dddd, MMMM dd, yyyy"
//"iDate"="0"
//"iTime"="0"
//"iTLZero"="0"
//"s1159"="AM"
//"s2359"="PM"
//******************************************************************************
BOOL SetupControlPanelKeys()
{
 HKEY hkey;
 LCID lcid;
 char tmp[128];

    if(RegCreateKey(HKEY_CURRENT_USER,"Control Panel\\International",&hkey)!=ERROR_SUCCESS) {
        dprintf(("SetupControlPanelKeys: Unable to create key"));
        return FALSE;
    }
    lcid = GetUserDefaultLCID();
    sprintf(tmp, "%08X", lcid);
    RegSetValueEx(hkey,"Locale",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_SLANGUAGE, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sLanguage",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_SCOUNTRY, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sCountry",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_ICOUNTRY, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iCountry",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_SLIST, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sList",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_IMEASURE, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sMeasure",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_SDECIMAL, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sDecimal",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_STHOUSAND, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sThousand",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_IDIGITS, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iDigits",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_ILZERO, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iLZero",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_SCURRENCY, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sCurrency",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_ICURRDIGITS, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iCurrDigits",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_ICURRENCY, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iCurrency",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_INEGCURR, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iNegCurr",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_SDATE, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sDate",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_STIME, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sTime",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_SSHORTDATE, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sShortDate",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_SLONGDATE, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"sLongDate",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_IDATE, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iDate",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_ITIME, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iTime",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_ITLZERO, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iTLZero",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_S1159, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"s1159",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_S2359, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"s2359",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    GetLocaleInfo(lcid, LOCALE_ICALENDARTYPE, tmp, sizeof(tmp)-1);
    RegSetValueEx(hkey,"iCalendarType",0,REG_SZ, (LPBYTE)tmp, strlen(tmp)+1);

    RegCloseKey(hkey);

    if(RegCreateKey(HKEY_CURRENT_USER,"Control Panel\\International\\Sorting Order",&hkey)!=ERROR_SUCCESS) {
        dprintf(("SetupControlPanelKeys: Unable to create key"));
        return FALSE;
    }
    RegCloseKey(hkey);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
extern "C" const char *TimeZones;
extern "C" int ProcessEmbeddedFile(const char *data, BOOL force);
void SetupTimeZoneInfo()
{
	ProcessEmbeddedFile(TimeZones, TRUE);
}
//******************************************************************************
//******************************************************************************
void RegisterDLLs()
{
    // Register a number of DLLs that perform necessary initialization in
    // DLLRegisterServer(). On a real machine this is done by regsvr32.exe
    // during installation. Since we don't have regsvr32.exe, we will call
    // this entry point manually from here.

    char buf[sizeof(DIR_System)+260];

    const char *DLLs[] = { "RSAENH.DLL" };

    for (int i = 0; i < sizeof(DLLs)/sizeof(DLLs[0]); i++)
    {
        sprintf(buf, "%s\\%s", DIR_System, DLLs[i]);

        dprintf(("RegisterDLLs: Registering %s", buf));

        HMODULE hmod = LoadLibrary(buf);
        if (!hmod)
        {
            dprintf(("RegisterDLLs: Unable to load DLL %s (error %d)",
                     buf, GetLastError()));
            continue;
        }

        typedef HRESULT WINAPI fnDllRegisterServer();

        fnDllRegisterServer *proc = (fnDllRegisterServer *)
            GetProcAddress(hmod, "DllRegisterServer");
        if (!proc)
        {
            dprintf(("RegisterDLLs: Unable to find DllRegisterServer "
                     "in %s (error %d)", buf, GetLastError()));
        }
        else
        {
            HRESULT hrc = proc();
            if (hrc)
                dprintf(("RegisterDLLs: Calling DllRegisterServer "
                         "in %s returned error %x", buf, hrc));
        }

        FreeLibrary(hmod);
    }


	ProcessEmbeddedFile(TimeZones, TRUE);
}
//******************************************************************************
//******************************************************************************

