/* $Id: initsystem.cpp,v 1.24 2000-12-30 10:17:00 sandervl Exp $ */
/*
 * Odin system initialization (registry, directories & environment)
 *
 * Called from the WarpIn install program to create the desktop directories and
 * to setup the registry
 *
 * InitSystemAndRegistry creates:
 *      - SYSTEMDIR\drivers
 *      - SYSTEMDIR\drivers\etc
 *  - WINDOWSDIR\SYSTEM
 *      - WINDOWSDIR\AppData
 *      - WINDOWSDIR\Cache
 *      - WINDOWSDIR\Cookies
 *  - WINDOWSDIR\Desktop
 *  - WINDOWSDIR\Favorites
 *  - WINDOWSDIR\Fonts
 *      - WINDOWSDIR\History
 *  - WINDOWSDIR\NetHood
 *  - WINDOWSDIR\My Documents
 *  - WINDOWSDIR\PrintHood
 *  - WINDOWSDIR\Recent
 *  - WINDOWSDIR\SendTo
 *  - WINDOWSDIR\Start Menu
 *  - WINDOWSDIR\Start Menu\Programs
 *  - WINDOWSDIR\Start Menu\Programs\Startup
 *  - WINDOWSDIR\ShellNew
 *      - x:\Program Files
 *      - x:\Program Files\Common Files
 *  - and a minimal system registry
 *
 * Copyright 1999-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
#include <win\options.h>
#include "directory.h"
#include <versionos2.h>

#define DBG_LOCALLOG    DBG_initsystem
#include "dbglocal.h"

#define DDRAW_CLASSID       "{D7B70EE0-4340-11CF-B063-0020AFC2CD35}"
#define DDRAW_DEFAULT       "DirectDraw Object"
#define DDRAW_CLIPPER_CLASSID   "{593817A0-7DB3-11CF-A2DE-00AA00B93356}"
#define DDRAW_CLIPPER_DEFAULT   "DirectDraw Clipper Object"
#define DDRAW_DLL       "ddraw.dll"
#define DSOUND_CLASSID      "{47D4D946-62E8-11cf-93BC-444553540000}"
#define DSOUND_DEFAULT      "DirectSound Object"
#define DSOUND_DLL      "dsound.dll"
#define DPLAYX_CLASSID          "{D1EB6D20-8923-11d0-9D97-00A0C90A43CB}"
#define DPLAYX_DEFAULT          "DirectPlay Object"
#define DPLAYX_DLL              "dplayx.dll"
#define DPLAYX_LOBBY_CLASSID    "{2FE8F810-B2A5-11d0-A787-0000F803ABFC}"
#define DPLAYX_LOBBY_DEFAULT    "DirectPlayLobby Object"
#define DPLAYX_LOBBY_DLL        DPLAYX_DLL

#define CLASS_DESKTOP       "Desktop"
#define CLASS_SHORTCUT          "Shortcut"
#define CLASS_SHELL32DLL    "shell32.dll"
#define COM_CLASS_ID        "CLSID"
#define COM_INPROCSERVER        "InprocServer32"
#define COM_THREADMODEL     "ThreadingModel"
#define COM_THREAD_APARTMENT    "Apartment"
#define THREAD_BOTH     "Both"
#define INITREG_ERROR       "InitRegistry: Unable to register system information"
#define DIR_PROGRAM             "ProgramFilesDir"
#define DIR_PROGRAM_COMMON  "CommonFilesDir"
#define DIR_SHARED              "SharedDir"
#define HARDWARE_VIDEO_GRADD    "\\REGISTRY\\Machine\\System\\CurrentControlSet\\Services\\Gradd\\Device0"
#define HARDWARE_VIDEO_GRADD_DESCRIPTION "OS/2 Display driver"
#define HARDWARE_VIDEO_VGA      "\\REGISTRY\\Machine\\System\\CurrentControlSet\\Services\\VgaSave\\Device0"
#define HARDWARE_VIDEO_VGA_DESCRIPTION   "OS/2 VGA Display driver"
#define HARDWARE_VIDEO_COMPATIBLE "\\Device\\Video1"
#define DIRECTX_RC              "0"
#define DIRECTX_VERSION         "4.04.1381.276"
#define DIRECTX_INSTALLED_VERSION 0x0004
#define ODIN_WINMM_PLAYBACK     "OS/2 Dart Audio Playback"
#define ODIN_WINMM_RECORD   "OS/2 Dart Audio Record"

#define KEY_DEVICE_TYPE     "Type"
#define KEY_DEVICE_START    "Start"
#define KEY_DEVICE_GROUP        "Group"
#define KEY_DEVICE_ERRORCONTROL "ErrorControl"
#define KEY_DEVICE_AUTORUN      "Autorun"
#define KEY_DEVICE_TAG          "Tag"
#define DEVICE_GROUP_FILESYSTEM "File system"
#define DEVICE_GROUP_SCSICDROM  "SCSI CDROM Class"

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
 char  shellpath[260];

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

    //Software\Microsoft\Windows\CurrentVersion\RunOnce
    if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information (2)"));
        return FALSE;
    }
    RegCloseKey(hkey);

    //System\CurrentControlSet\Control\Session Manager
    if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Control\\Session Manager",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information (2)"));
        return FALSE;
    }
    RegCloseKey(hkey);

    if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\iexplore.exe",&hkey)!=ERROR_SUCCESS) {
        dprintf(("InitRegistry: Unable to register system information (2)"));
        return FALSE;
    }
    strcpy(shellpath, InternalGetWindowsDirectoryA());
    strcat(shellpath, "\\IEXPLORE.EXE");
    RegSetValueExA(hkey, "", 0, REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);
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

   if(RegCreateKeyA(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",&hkey)!=ERROR_SUCCESS) {
    dprintf(("InitRegistry: Unable to register system information (3)"));
    return FALSE;
   }
//   if(RegOpenKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", &hkey) != ERROR_SUCCESS)
//   {
        //system32\drivers dir
    strcpy(shellpath, InternalGetSystemDirectoryA());
    strcat(shellpath, "\\Drivers");
    CreateDirectoryA(shellpath, NULL);
    strcat(shellpath, "\\etc");
    CreateDirectoryA(shellpath, NULL);

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

   //Shell32 & IE related keys
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021400-0000-0000-C000-000000000046}]
   //@="Desktop"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021400-0000-0000-C000-000000000046}\InProcServer32]
   //@="shell32.dll"
   //ThreadingModel="Apartment"
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021400-0000-0000-C000-000000000046}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_DESKTOP, sizeof(CLASS_DESKTOP));
   RegCloseKey(hkey);
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021400-0000-0000-C000-000000000046}\\InProcServer32",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_SHELL32DLL, sizeof(CLASS_SHELL32DLL));
   RegSetValueExA(hkey, COM_THREADMODEL, 0, REG_SZ, (LPBYTE)COM_THREAD_APARTMENT, sizeof(COM_THREAD_APARTMENT));
   RegCloseKey(hkey);
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021401-0000-0000-C000-000000000046}]
   //@="Shortcut"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021401-0000-0000-C000-000000000046}\InProcServer32]
   //@="shell32.dll"
   //"ThreadingModel"="Apartment"
   //[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{00021401-0000-0000-C000-000000000046}\shellex\MayChangeDefaultMenu]
   //@=""
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021401-0000-0000-C000-000000000046}",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_SHORTCUT, sizeof(CLASS_SHORTCUT));
   RegCloseKey(hkey);
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021401-0000-0000-C000-000000000046}\\InProcServer32",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey, "", 0, REG_SZ, (LPBYTE)CLASS_SHELL32DLL, sizeof(CLASS_SHELL32DLL));
   RegSetValueExA(hkey, COM_THREADMODEL, 0, REG_SZ, (LPBYTE)COM_THREAD_APARTMENT, sizeof(COM_THREAD_APARTMENT));
   RegCloseKey(hkey);
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\CLSID\\{00021401-0000-0000-C000-000000000046}\\shellex\\MayChangeDefaultMenu",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey, "", 0, REG_SZ, (LPBYTE)"", 0);
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

   //DirectPlay
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\"DPLAYX_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey,"",0,REG_SZ, (LPBYTE)DPLAYX_DEFAULT, sizeof(DPLAYX_DEFAULT));
   if(RegCreateKeyA(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueExA(hkey1,"",0,REG_SZ, (LPBYTE)DPLAYX_DLL, sizeof(DPLAYX_DLL));
   RegSetValueExA(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //DirectPlay Lobby
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE, "Software\\CLASSES\\CLSID\\"DPLAYX_LOBBY_CLASSID ,&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey,"",0,REG_SZ, (LPBYTE)DPLAYX_LOBBY_DEFAULT, sizeof(DPLAYX_LOBBY_DEFAULT));
   if(RegCreateKeyA(hkey,COM_INPROCSERVER, &hkey1)!=ERROR_SUCCESS) {
    RegCloseKey(hkey);
    goto initreg_error;
   }
   RegSetValueExA(hkey1,"",0,REG_SZ, (LPBYTE)DPLAYX_LOBBY_DLL, sizeof(DPLAYX_LOBBY_DLL));
   RegSetValueExA(hkey1, COM_THREADMODEL, 0,REG_SZ, (LPBYTE)THREAD_BOTH, sizeof(THREAD_BOTH));
   RegCloseKey(hkey1);
   RegCloseKey(hkey);

   //[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion]
   //"ProgramFilesDir"="C:\Program Files"
   //"CommonFilesDir"="C:\Program Files\Common Files"
   //# This is intended for a centrally managed (server) directory where system files and e.g. fonts can reside. Most installs have this set to C:\WINDOWS, though.
   //"SharedDir"="C:\WINDOWS"
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   //Create x:\Program Files directory
   strcpy(shellpath, InternalGetWindowsDirectoryA());
   shellpath[2] = 0;    //get drive
   strcat(shellpath, "\\Program Files");
   CreateDirectoryA(shellpath, NULL);
   RegSetValueExA(hkey, DIR_PROGRAM, 0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);

   //Create x:\Program Files\Common Files directory
   strcat(shellpath, "\\Common Files");
   CreateDirectoryA(shellpath, NULL);
   RegSetValueExA(hkey, DIR_PROGRAM_COMMON, 0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);

   strcpy(shellpath, InternalGetWindowsDirectoryA());
   RegSetValueExA(hkey, DIR_SHARED, 0,REG_SZ, (LPBYTE)shellpath, strlen(shellpath)+1);

   RegCloseKey(hkey);

   //[HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\VIDEO]
   //"\\Device\\Video0"="\\REGISTRY\\Machine\\System\\ControlSet001\\Services\\mga64\\Device0"
   //"\\Device\\Video1"="\\REGISTRY\\Machine\\System\\ControlSet001\\Services\\VgaSave\\Device0"
   //"VgaCompatible"="\\Device\\Video1"
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\VIDEO",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey,"\\Device\\Video0",0,REG_SZ, (LPBYTE)HARDWARE_VIDEO_GRADD, sizeof(HARDWARE_VIDEO_GRADD));
   RegSetValueExA(hkey,"\\Device\\Video1",0,REG_SZ, (LPBYTE)HARDWARE_VIDEO_VGA, sizeof(HARDWARE_VIDEO_VGA));
   RegSetValueExA(hkey, "VgaCompatible", 0,REG_SZ, (LPBYTE)HARDWARE_VIDEO_COMPATIBLE, sizeof(HARDWARE_VIDEO_COMPATIBLE));
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Gradd\\Device0",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey,"Device Description", 0, REG_SZ, (LPBYTE)HARDWARE_VIDEO_GRADD_DESCRIPTION, sizeof(HARDWARE_VIDEO_GRADD_DESCRIPTION));
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\VgaSave\\Device0",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey,"Device Description", 0, REG_SZ, (LPBYTE)HARDWARE_VIDEO_VGA_DESCRIPTION, sizeof(HARDWARE_VIDEO_VGA_DESCRIPTION));
   RegCloseKey(hkey);

   //Software\Microsoft\Multimedia\Sound Mapper
   if(RegCreateKeyA(HKEY_CURRENT_USER,"Software\\Microsoft\\Multimedia\\Sound Mapper",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey,"Playback", 0, REG_SZ, (LPBYTE)ODIN_WINMM_PLAYBACK, sizeof(ODIN_WINMM_PLAYBACK));
   RegSetValueExA(hkey,"Record", 0, REG_SZ, (LPBYTE)ODIN_WINMM_RECORD, sizeof(ODIN_WINMM_RECORD));
   RegCloseKey(hkey);

   //Software\Microsoft\DirectX
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectX",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   RegSetValueExA(hkey,"",0,REG_SZ, (LPBYTE)"", 0);
   RegSetValueExA(hkey, "RC", 0,REG_SZ, (LPBYTE)DIRECTX_RC, sizeof(DIRECTX_RC));
   RegSetValueExA(hkey, "Version", 0,REG_SZ, (LPBYTE)DIRECTX_VERSION, sizeof(DIRECTX_VERSION));
   val = DIRECTX_INSTALLED_VERSION;
   RegSetValueExA(hkey, "InstalledVersion", 0,REG_BINARY, (LPBYTE)&val, sizeof(DWORD));
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectDraw",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   //todo
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Direct3D",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   //todo
   RegCloseKey(hkey);

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectPlay",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   //todo
   RegCloseKey(hkey);

#if 0
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectMusic",&hkey)!=ERROR_SUCCESS) {
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

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Cdfs",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   val = 0x2;
   RegSetValueExA(hkey, KEY_DEVICE_TYPE,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x4;
   RegSetValueExA(hkey, KEY_DEVICE_START,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x1;
   RegSetValueExA(hkey, KEY_DEVICE_ERRORCONTROL,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   RegSetValueExA(hkey, KEY_DEVICE_GROUP,0,REG_SZ, (LPBYTE)DEVICE_GROUP_FILESYSTEM, sizeof(DEVICE_GROUP_FILESYSTEM));
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

   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Cdrom",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   val = 0x1;
   RegSetValueExA(hkey, KEY_DEVICE_TYPE,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x1;
   RegSetValueExA(hkey, KEY_DEVICE_START,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x0;
   RegSetValueExA(hkey, KEY_DEVICE_ERRORCONTROL,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x2;
   RegSetValueExA(hkey, KEY_DEVICE_TAG,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x0;
   RegSetValueExA(hkey, KEY_DEVICE_AUTORUN,0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   RegSetValueExA(hkey, KEY_DEVICE_GROUP,0,REG_SZ, (LPBYTE)DEVICE_GROUP_SCSICDROM, sizeof(DEVICE_GROUP_SCSICDROM));
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
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\FileSystem",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   val = 0x0;
   RegSetValueExA(hkey, "Win31FileSystem",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x0;
   RegSetValueExA(hkey, "NtfsDisable8dot3NameCreation",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   val = 0x1;
   RegSetValueExA(hkey, "Win95TruncatedExtensions",0,REG_DWORD, (LPBYTE)&val, sizeof(DWORD));
   RegCloseKey(hkey);


//[HKEY_LOCAL_MACHINE\Software\Microsoft\OLE]
//# allow cross-machine calls (RPC) (default Y)
//"EnableDCOM"="Y"
//# allow incoming connections ? (def. N)
//"EnableRemoteConnect"="N"
   if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\OLE",&hkey)!=ERROR_SUCCESS) {
    goto initreg_error;
   }
   digbuf[0] = 'Y';
   digbuf[1] = 0;
   RegSetValueExA(hkey, "EnableDCOM",0,REG_SZ, (LPBYTE)digbuf, 2);
   digbuf[0] = 'N';
   digbuf[1] = 0;
   RegSetValueExA(hkey, "EnableRemoteConnect",0,REG_SZ, (LPBYTE)digbuf, 2);
   RegCloseKey(hkey);

   //Add MS Sans Serif to WarpSans font conversion entry
   char temp;
   if(PROFILE_GetOdinIniString(ODINFONTSECTION, "MS Sans Serif", "", &temp,
                               0) <= 1)
   {
       PROFILE_SetOdinIniString(ODINFONTSECTION, "MS Sans Serif", "WarpSans");
   }

   return TRUE;

initreg_error:
   dprintf((INITREG_ERROR));
   return FALSE;
}
//******************************************************************************
//Environment variables created by Windows NT:
//
//COMPUTERNAME=NTBAK
//ComSpec=E:\WINNT\system32\cmd.exe
//CPU=i386
//HOMEDRIVE=E:
//HOMEPATH=\
//LOGONSERVER=\\NTBAK
//NUMBER_OF_PROCESSORS=2
//OS=Windows_NT
//PATHEXT=.COM;.EXE;.BAT;.CMD
//PROCESSOR_ARCHITECTURE=x86
//PROCESSOR_IDENTIFIER=x86 Family 6 Model 6 Stepping 5, GenuineIntel
//PROCESSOR_LEVEL=6
//PROCESSOR_REVISION=0605
//SystemDrive=E:
//SystemRoot=E:\WINNT
//USERDOMAIN=NTBAK
//USERNAME=Sander
//USERPROFILE=E:\WINNT\Profiles\Sander
//windir=E:\WINNT
//******************************************************************************
void InitEnvironment(int nrcpus)
{
 char  buffer[64];
 char  buffer1[32];
 char *windir;
 DWORD signature;

    SetEnvironmentVariableA("CPU", "i386");
    SetEnvironmentVariableA("PROCESSOR_ARCHITECTURE", "x86");
    if(SupportsCPUID()) {
        GetCPUVendorString(buffer1);
        buffer1[12] = 0;
        signature = GetCPUSignature();
        sprintf(buffer, "x86 Family %x Model %x Stepping %x, %s", (signature >> 8)&0xf, signature & 0xf, (signature >> 4)&0xf, buffer1);
        SetEnvironmentVariableA("PROCESSOR_IDENTIFIER", buffer);
        sprintf(buffer, "%x", (signature >> 8)&0xf);
        SetEnvironmentVariableA("PROCESSOR_LEVEL", buffer);
        sprintf(buffer, "%02x%02x", (signature >> 4)&0xf, signature & 0xf);
        SetEnvironmentVariableA("PROCESSOR_REVISION", buffer);
    }
    sprintf(buffer, "%d", nrcpus);
    SetEnvironmentVariableA("NUMBER_OF_PROCESSORS", buffer);
    SetEnvironmentVariableA("OS", "Windows_NT");
    SetEnvironmentVariableA("PATHEXT", ".COM;.EXE;.BAT;.CMD");
    windir = InternalGetWindowsDirectoryA();
    SetEnvironmentVariableA("windir", windir);
    SetEnvironmentVariableA("SystemRoot", windir);
    buffer[0] = windir[0];
    buffer[1] = windir[1];
    buffer[2] = 0;
    SetEnvironmentVariableA("SystemDrive", buffer);
    SetEnvironmentVariableA("HOMEDRIVE", buffer);
    SetEnvironmentVariableA("HOMEPATH", "\\");

    //TODO:
    //COMPUTERNAME=NTBAK
    //ComSpec=E:\WINNT\system32\cmd.exe
    //LOGONSERVER=\\NTBAK
    //USERDOMAIN=NTBAK
    //USERNAME=Sander
    //USERPROFILE=E:\WINNT\Profiles\Sander
}
//******************************************************************************
//Create/change keys that may change (i.e. odin.ini keys that affect windows version)
//******************************************************************************
void InitDynamicRegistry()
{
 OSVERSIONINFOA versioninfo;
 HKEY           hkey;
 char           buf[16] = "";

    versioninfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&versioninfo);

    //Set version key:
    // [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\ProductOptions]
    // "ProductType"="WinNT"
    if(RegCreateKeyA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",&hkey)!=ERROR_SUCCESS) {
      	dprintf(("InitRegistry: Unable to register system information (2)"));
	    return;
    }
    switch(versioninfo.dwPlatformId) {
    case VER_PLATFORM_WIN32_WINDOWS:
        strcpy(buf, "Win98");    //TODO: Correct???????????
        break;
    case VER_PLATFORM_WIN32_NT:
        strcpy(buf, "WinNT");	//TODO: Also correct for Windows 2000???
        break;
    default:
        DebugInt3();
        break;
    }
    RegSetValueExA(hkey,"ProductType",0,REG_SZ, (LPBYTE)buf, strlen(buf)+1);
    RegCloseKey(hkey);
}
//******************************************************************************
//******************************************************************************

