/* $Id: versionos2.h,v 1.15 2001-01-13 09:26:36 bird Exp $ */

#ifndef __VERSIONOS2__H__
#define __VERSIONOS2__H__

/*PLF
 *
 * PE2LX_VERSION is an internal version number used to verify that the
 * dll's are a matched set.  It doesn't have to be related at all to the
 * "version number of the product" we advertise to users.
 *
 * Each time a structural change in pe2lx.exe causes previous dll's to
 * be incompatible (for example,  when Sander removed the "OS2" prefix
 * from all exported names),  this constant should be incremented.
 *
 * The value is currently stored as a four byte EA (so the range is that
 * of an unsigned long).
 *
 *
 */
#define PE2LX_VERSION 6

/*
 * Current Odin version and build number.
 * Used for bldlevel info.
 */
#define ODIN32_VERSION_MAJOR 0
#define ODIN32_VERSION_MINOR 5
#define ODIN32_VERSION       "0.5"
#define ODIN32_BUILD_NR       305        /* This started at 300 on the 17th Des 2000. */


#ifndef SYSTEM
#define SYSTEM _System
#endif

//in kernel32.dll/wprocess.cpp, used by version.dll
BOOL  SYSTEM GetVersionStruct(char *modname, char *verstruct, ULONG bufLength);
ULONG SYSTEM GetVersionSize(char *modname);

//Used by GetVersion and GetProcessVersion
#define WIN32OS2_VERSION  (4)

#define ODINNT_VERSION          0x05650004  //returned by GetVersion

//Used by GetVersionEx:
#define ODINNT_MAJOR_VERSION    4
#define ODINNT_MINOR_VERSION    0
#define ODINNT_BUILD_NR     1381
#define ODINNT_CSDVERSION   "Service Pack 6"
#define ODINNT_CSDVERSION_W (LPWSTR)L"Service Pack 6"
#define ODINNT_SOFTWARE_TYPE    "SYSTEM"
#define ODINNT_OSTYPE_UNI       "Uniprocessor Free"
#define ODINNT_OSTYPE_SMP       "SMPprocessor Free" //??

//image version (header)
#define COMCTL32_MAJORIMAGE_VERSION 5
#define COMCTL32_MINORIMAGE_VERSION 0
#define COMDLG32_MAJORIMAGE_VERSION ODINNT_MAJOR_VERSION
#define COMDLG32_MINORIMAGE_VERSION ODINNT_MINOR_VERSION
#define DDRAW_MAJORIMAGE_VERSION    ODINNT_MAJOR_VERSION
#define DDRAW_MINORIMAGE_VERSION    ODINNT_MINOR_VERSION
#define GDI32_MAJORIMAGE_VERSION    ODINNT_MAJOR_VERSION
#define GDI32_MINORIMAGE_VERSION    ODINNT_MINOR_VERSION
#define SHELL32_MAJORIMAGE_VERSION  ODINNT_MAJOR_VERSION
#define SHELL32_MINORIMAGE_VERSION  ODINNT_MINOR_VERSION
#define USER32_MAJORIMAGE_VERSION   ODINNT_MAJOR_VERSION
#define USER32_MINORIMAGE_VERSION   ODINNT_MINOR_VERSION


//profile key in odin.ini for windows version
#define PROFILE_WINVERSION_SECTION  "WinVersion"
#define PROFILE_WINVERSION_KEY          "Version"
#define PROFILE_WINVERSION_WIN98    "Win98"
#define PROFILE_WINVERSION_NT40     "NT40"
#define PROFILE_WINVERSION_WIN2000  "Win2000"

#define WINVERSION_WIN98        0
#define WINVERSION_NT40         1   //default
#define WINVERSION_WIN2000      2
#define WINVERSION_MAX          3

#endif





