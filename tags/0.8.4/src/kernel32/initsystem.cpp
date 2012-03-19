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
#include <win/options.h>
#include <win/winnls.h>
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

    // Deduce the Windows time zone from TZ
    const char *tz = getenv("TZ");
    if (tz == NULL) {
        if (RegOpenKeyA(HKEY_LOCAL_MACHINE,
                        "SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation",
                        &hkey) == ERROR_SUCCESS) {
            // use the last value
            RegCloseKey(hkey);
            return;
        }
        //default is Central European Time
        tz = "CET-1CDT";
    }

    char *tzBuf = strdup(tz);
    if (!tzBuf) {
        dprintf(("InitRegistry: Unable to allocate memory"));
        return;
    }

    TIME_ZONE_INFORMATION tzi;
    memset(&tzi, 0, sizeof(tzi));
    dprintf(("InitRegistry: TZ=%s", tzBuf));

    int state = 0;
    char *tok = strtok(tzBuf, ",");
    for (; tok && state != -1; state++, tok = strtok(NULL, ",")) {
        switch (state) {
        case 0: {
            // STD bias(sec) DST
            char *s = tok;
            while (isalpha(*s))
                ++s;
            if (s - tok == 3) {
                MultiByteToWideChar(CP_ACP, 0, tok, 3, tzi.StandardName,
                                    sizeof(tzi.StandardName));
                char *fail;
                long n = strtol(s, &fail, 10);
                s = fail;
                if (*s)
                    while (*s && isalpha(*s))
                        ++s;
                if (!*s) {
                    MultiByteToWideChar(CP_ACP, 0, fail, -1, tzi.DaylightName, sizeof(tzi.DaylightName));
                    tzi.Bias = n * 60;
                } else {
                    state = -1;
                }
            } else {
                state = -1;
            }
            break;
        }
        // the rest has the following format:
        // DST_m,DST_n,DST_w,DST_t,STD_m,STD_n,STD_w,STD_t,DST_bias
        // where _m is month, _w is day of week, _n is the occurence
        // of this day, _t is the time (sec)
        case 1:
        case 5: {
            // month
            char *fail;
            long n = strtol(tok, &fail, 10);
            if (*fail == 0 && n >= 1 && n <= 12) {
                state == 1 ? tzi.DaylightDate.wMonth = n
                           : tzi.StandardDate.wMonth = n;
            } else {
                state = -1;
            }
            break;
        }
        case 2:
        case 6: {
            // occurence
            char *fail;
            long n = strtol(tok, &fail, 10);
            if (*fail == 0 && (n >= 1 && n <= 5) || n == -1) {
                if (n == -1)
                    n = 5; // this is what Windows expect
                state == 2 ? tzi.DaylightDate.wDay = n
                           : tzi.StandardDate.wDay = n;
            } else {
                state = -1;
            }
            break;
        }
        case 3:
        case 7: {
            // day of week
            char *fail;
            long n = strtol(tok, &fail, 10);
            if (*fail == 0 && n >= 0 && n <= 6) {
                state == 3 ? tzi.DaylightDate.wDayOfWeek = n
                           : tzi.StandardDate.wDayOfWeek = n;
            } else {
                state = -1;
            }
            break;
        }
        case 4:
        case 8: {
            // time (sec)
            char *fail;
            long n = strtol(tok, &fail, 10);
            if (*fail == 0 && n >= 0) {
                SYSTEMTIME *t = state == 4 ? &tzi.DaylightDate
                                           : &tzi.StandardDate;
                t->wHour = n / 3600;
                t->wMinute = (n % 3600) / 60;
            } else {
                state = -1;
            }
            break;
        }
        case 9: {
            // daylight bias (sec)
            char *fail;
            long n = strtol(tok, &fail, 10);
            if (*fail == 0) {
                tzi.DaylightBias = - (n / 60);
            } else {
                state = -1;
            }
            break;
        }
        default:
            state = -1;
            break;
        }
    }

    free(tzBuf);
    if (state == -1 || (state != 1 && state != 10)) {
        dprintf(("InitRegistry: Unable to parse TZ"));
        return;
    }

    // apply the daylight bias to the standard start time
    // (windows expects this)
    if (tzi.DaylightBias) {
        DWORD min = tzi.StandardDate.wHour * 60 + tzi.StandardDate.wMinute;
        min -= tzi.DaylightBias;
        tzi.StandardDate.wHour = min / 60;
        tzi.StandardDate.wMinute = min % 60;
    }

    dprintf(("InitRegistry: Bias         %d", tzi.Bias));
    dprintf(("InitRegistry: StandardName '%ls'", tzi.StandardName));
    dprintf(("InitRegistry: StandardDate %d,%d,%d,%d:%d",
             tzi.StandardDate.wMonth, tzi.StandardDate.wDay,
             tzi.StandardDate.wDayOfWeek,
             tzi.StandardDate.wHour, tzi.StandardDate.wMinute));
    dprintf(("InitRegistry: StandardBias %d", tzi.StandardBias));
    dprintf(("InitRegistry: DaylightName '%ls'", tzi.DaylightName));
    dprintf(("InitRegistry: DaylightDate %d,%d,%d,%d:%d",
             tzi.DaylightDate.wMonth, tzi.DaylightDate.wDay,
             tzi.DaylightDate.wDayOfWeek,
             tzi.DaylightDate.wHour, tzi.DaylightDate.wMinute));
    dprintf(("InitRegistry: DaylightBias %d", tzi.DaylightBias));

    struct REG_TZI_FORMAT {
        LONG Bias;
        LONG StandardBias;
        LONG DaylightBias;
        SYSTEMTIME StandardDate;
        SYSTEMTIME DaylightDate;
    } rtzi, rtzitmp;

    memset(&rtzi, 0, sizeof(rtzi));
    rtzi.Bias = tzi.Bias;
    rtzi.StandardBias = tzi.StandardBias;
    rtzi.DaylightBias = tzi.DaylightBias;
    rtzi.StandardDate = tzi.StandardDate;
    rtzi.DaylightDate = tzi.DaylightDate;

    DWORD bestMatch = 0;
    if (RegOpenKeyA(HKEY_LOCAL_MACHINE,
                    "Software\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones",
                    &hkey) == ERROR_SUCCESS) {
        DWORD idx = 0;
        char name[256];
        DWORD len;
        for (;; idx++) {
            len = sizeof(name);
            if (RegEnumKeyExA(hkey, idx, name, &len, NULL, NULL, NULL,
                              NULL) != ERROR_SUCCESS)
                    break;

            HKEY hsubkey;
            DWORD len;
            if (RegOpenKeyA(hkey, name, &hsubkey) == ERROR_SUCCESS) {
                len = sizeof(rtzitmp);
                if (RegQueryValueExA(hsubkey, "TZI", NULL, NULL,
                                     (LPBYTE)&rtzitmp, &len) == ERROR_SUCCESS) {
                    DWORD match = 0;
                    if (rtzi.Bias == rtzitmp.Bias) {
                        match = 1;
                        if (rtzi.StandardBias == rtzitmp.StandardBias &&
                            rtzi.DaylightBias == rtzitmp.DaylightBias) {
                            match = 2;
                            if (memcmp(&rtzi.StandardDate, &rtzitmp.StandardDate,
                                       sizeof(rtzi.StandardDate)) == 0 &&
                                    memcmp(&rtzi.DaylightDate, &rtzitmp.DaylightDate,
                                           sizeof(rtzi.DaylightDate)) == 0) {
                                match = 3;
                            }
                        }
                    }
                    if (match > bestMatch) {
                        len = sizeof(tzi.StandardName);
                        if (RegQueryValueExW(hsubkey, (LPCWSTR)L"Std", NULL, NULL,
                                             (LPBYTE)tzi.StandardName, &len) == ERROR_SUCCESS) {
                            len = sizeof(tzi.DaylightName);
                            if (RegQueryValueExW(hsubkey, (LPCWSTR)L"Dlt", NULL, NULL,
                                                 (LPBYTE)tzi.DaylightName, &len) == ERROR_SUCCESS) {
                                tzi.StandardName[sizeof(tzi.StandardName) - 1] = '\0';
                                tzi.DaylightName[sizeof(tzi.DaylightName) - 1] = '\0';
                                // copy the found data over (it may be more precise)
                                tzi.Bias = rtzitmp.Bias;
                                tzi.StandardBias = rtzitmp.StandardBias;
                                tzi.DaylightBias = rtzitmp.DaylightBias;
                                tzi.StandardDate = rtzitmp.StandardDate;
                                tzi.DaylightDate = rtzitmp.DaylightDate;

                                bestMatch = match;
                            }
                        }
                    }
                }
                RegCloseKey(hsubkey);
            }
        }
        RegCloseKey(hkey);
    }

    if (bestMatch) {
        dprintf(("InitRegistry: Found Windows Time Zone (best match %d):", bestMatch));
        dprintf(("InitRegistry: StandardName '%ls'", tzi.StandardName));
        dprintf(("InitRegistry: DaylightName '%ls'", tzi.DaylightName));
    } else {
        dprintf(("InitRegistry: WARNING: Couldn't match TZ to Windows Time Zone."));
        dprintf(("InitRegistry: Check 'HKLM\\Software\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones' key."));
    }

    SetTimeZoneInformation (&tzi);
}
//******************************************************************************
//******************************************************************************

