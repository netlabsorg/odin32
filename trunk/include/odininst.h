/*
 * Exported installation procedure in kernel32
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __ODININST_H__
#define __ODININST_H__

#define ODINFONTSECTION "Font Mapping"

#define CLASS_DESKTOP           "Desktop"
#define CLASS_SHORTCUT          "Shortcut"
#define CLASS_SHELL32DLL        "shell32.dll"
#define CLASS_SHDOCVW           "shdocvw.dll"
#define CLASS_OLE32DLL          "ole32.dll"
#define CLASS_OLEAUT32DLL       "oleaut32.dll"
#define COM_CLASS_ID            "CLSID"
#define COM_INPROCSERVER        "InprocServer32"
#define COM_THREADMODEL         "ThreadingModel"
#define COM_THREAD_APARTMENT    "Apartment"
#define THREAD_BOTH             "Both"
#define INITREG_ERROR           "InitRegistry: Unable to register system information"
#define DIR_PROGRAM             "ProgramFilesDir"
#define DIR_PROGRAM_COMMON      "CommonFilesDir"
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
#define ODIN_WINMM_RECORD       "OS/2 Dart Audio Record"

#define KEY_DEVICE_TYPE         "Type"
#define KEY_DEVICE_START        "Start"
#define KEY_DEVICE_GROUP        "Group"
#define KEY_DEVICE_ERRORCONTROL "ErrorControl"
#define KEY_DEVICE_AUTORUN      "Autorun"
#define KEY_DEVICE_TAG          "Tag"
#define DEVICE_GROUP_FILESYSTEM "File system"
#define DEVICE_GROUP_SCSICDROM  "SCSI CDROM Class"

const char szMci[] = "mci";
const char szCDAudio[] = "cdaudio";
const char szMciCDA[] = "mcicda.drv";

#endif //__ODININST_H__

