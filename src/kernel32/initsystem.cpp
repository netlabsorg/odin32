/* $Id: initsystem.cpp,v 1.28 2001-06-27 13:35:46 sandervl Exp $ */
/*
 * Odin system initialization (registry, directories & environment)
 *
 * Called from the WarpIn install program to create the desktop directories and
 * to setup the registry
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


//******************************************************************************
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
        sprintf(buffer, "x86 Family %x Model %x Stepping %x, %s", (signature >> 8)&0xf, (signature >> 4) & 0xf, signature & 0xf, buffer1);
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

