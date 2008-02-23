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

#define DDRAW_CLASSID           "{D7B70EE0-4340-11CF-B063-0020AFC2CD35}"
#define DDRAW_DEFAULT           "DirectDraw Object"
#define DDRAW_CLIPPER_CLASSID   "{593817A0-7DB3-11CF-A2DE-00AA00B93356}"
#define DDRAW_CLIPPER_DEFAULT   "DirectDraw Clipper Object"
#define DDRAW_DLL               "ddraw.dll"
#define DSOUND_CLASSID          "{47D4D946-62E8-11cf-93BC-444553540000}"
#define DSOUND_DEFAULT          "DirectSound Object"
#define DSOUND_DLL              "dsound.dll"
#define DPLAYX_CLASSID          "{D1EB6D20-8923-11d0-9D97-00A0C90A43CB}"
#define DPLAYX_DEFAULT          "DirectPlay Object"
#define DPLAYX_DLL              "dplayx.dll"
#define DPLAYX_LOBBY_CLASSID    "{2FE8F810-B2A5-11d0-A787-0000F803ABFC}"
#define DPLAYX_LOBBY_DEFAULT    "DirectPlayLobby Object"
#define DPLAYX_LOBBY_DLL        DPLAYX_DLL

#define QUARTZ_DLL              "quartz.dll"
#define QUARTZ_FILTER_CLASSID   "{CDA42200-BD88-11d0-BD4E-00A0C911CE86}"
#define QUARTZ_FILTER_DEFAULT   "Filter Mapper2"
#define QUARTZ_MEDIAPROP_CLASSID "{CDBD8D00-C193-11D0-BD4E-00A0C911CE86}"
#define QUARTZ_MEDIAPROP_DEFAULT "CMediaPropertyBag"
#define QUARTZ_DSOUNDREND_CLASSID "{79376820-07D0-11CF-A24D-0020AFD79767}"
#define QUARTZ_DSOUNDREND_DEFAULT "DirectSound Audio Renderer"
#define QUARTZ_FILTERGRAPHDIST_CLASSID "{e436ebb4-524f-11ce-9f53-0020af0ba770}"
#define QUARTZ_FILTERGRAPHDIST_DEFAULT "Filter Graph Control Plug In Distributor"

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

