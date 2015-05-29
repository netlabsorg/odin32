/* $Id: initterm.cpp,v 1.34 2003-01-21 11:22:08 sandervl Exp $
 *
 * USER32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSMISC
#define  INCL_DOSERRORS
#define  INCL_WINSHELLDATA
#define  INCL_WINERRORS
#define  INCL_GPILCIDS
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:29*/
#include <win32type.h>
#include <win32api.h>
#include <winconst.h>
#include <odinlx.h>
#include <spy.h>
#include <monitor.h>
#include <exitlist.h>
#include <initdll.h>
#include <stats.h>

#include "pmwindow.h"
#include "win32wdesktop.h"
#include "win32wndhandle.h"
#include "syscolor.h"
#include "initterm.h"
#include "auxthread.h"

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

extern "C" INT __cdecl wsnprintfA(LPSTR,UINT,LPCSTR,...);

// Win32 resource table (produced by wrc)
extern DWORD user32_PEResTab;

DWORD hInstanceUser32 = 0;

BOOL  fVersionWarp3 = FALSE;

#define FONTSDIRECTORY "Fonts"
#define REGPATH "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"

static void MigrateWindowsFonts()
{
  HKEY  hkFonts,hkOS2Fonts;
  char  buffer[512];
  UINT  len;

  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, REGPATH ,0, KEY_ALL_ACCESS, &hkFonts) == 0)
  {
      DWORD dwIndex, dwType;
      char subKeyName[255], dataArray[512];
      DWORD sizeOfSubKeyName = 254, sizeOfDataArray = 511;

      // loop over all values of the current key
      for (dwIndex=0;
           RegEnumValueA(hkFonts, dwIndex, subKeyName, &sizeOfSubKeyName, NULL, &dwType ,(LPBYTE)dataArray, &sizeOfDataArray) != ERROR_NO_MORE_ITEMS_W;
           ++dwIndex, sizeOfSubKeyName = 254, sizeOfDataArray = 511)
      {
         HDIR          hdirFindHandle = HDIR_CREATE;
         FILEFINDBUF3  FindBuffer     = {0};
         ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
         ULONG         ulFindCount    = 1, ret;
         APIRET        rc             = NO_ERROR;

         GetWindowsDirectoryA( buffer, sizeof(buffer) );
         wsnprintfA( buffer, sizeof(buffer), "%s\\%s\\%s", buffer, FONTSDIRECTORY, dataArray );

         rc = DosFindFirst( buffer, &hdirFindHandle, FILE_NORMAL,&FindBuffer, ulResultBufLen, &ulFindCount, FIL_STANDARD);
         //Check that file actaully exist
         if ( rc == NO_ERROR  && !(FindBuffer.attrFile & FILE_DIRECTORY))
         {
            //Check OS/2 INI profile for font entry
            len = PrfQueryProfileString(HINI_PROFILE, "PM_Fonts", dataArray,
                                        NULL, (PVOID)subKeyName, (LONG)sizeof(subKeyName));

            //If it doesn't exist OR if it's outdated
            if(len == 0 || strcmp(subKeyName, buffer))
            {
              dprintf(("Migrating font %s to OS/2",dataArray));

              ret = GpiLoadFonts(0, buffer);
              if(ret == FALSE) {
                  dprintf(("GpiLoadFonts %s failed with %x", buffer, WinGetLastError(0)));
              }
            }
         }
         DosFindClose(hdirFindHandle);
      }
      RegCloseKey(hkFonts);
  }
}

static BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            return TRUE;
        case DLL_PROCESS_DETACH:
        {
            StopAuxThread();
            return TRUE;
        }
        default:
            break;
    }
    return FALSE;
}

ULONG SYSTEM DLL_InitUser32(ULONG hModule)
{
    APIRET rc;
    __con_debug(2,"%s::%s@%08X(%08X)\n","USER32",__FUNCTION__,DLL_InitUser32,hModule);
    ULONG version[2];
    rc = DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_MINOR, version, sizeof(version));
    if (rc == 0)
    {
         if (version[0] >= 20 && version[1] <= 30)
             fVersionWarp3 = TRUE;
    }

    STATS_InitializeUSER32();

    ParseLogStatusUSER32();

    if (!InitializeKernel32())
        return -1;

    CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

    hInstanceUser32 = RegisterLxDll(hModule, DllMain, (PVOID)&user32_PEResTab,
                                    USER32_MAJORIMAGE_VERSION, USER32_MINORIMAGE_VERSION,
                                    IMAGE_SUBSYSTEM_WINDOWS_GUI);
    if (hInstanceUser32 == 0)
        return -1;

    dprintf(("user32 init %s %s (%x)", __DATE__, __TIME__, DLL_InitUser32));

    //SvL: Try to start communication with our message spy queue server
    InitSpyQueue();

    //SvL: Init win32 PM classes
    //PH:  initializes HAB!
    if (FALSE == InitPM())
        return -1;

    InitializeWindowHandles();

    //SvL: 18-7-'98, Register system window classes (button, listbox etc etc)
    //CB: register internal classes
    RegisterSystemClasses(hModule);

    //CB: initialize PM monitor driver
    MONITOR_Initialize(&MONITOR_PrimaryMonitor);

    //PF: migrate windows fonts
    MigrateWindowsFonts();

    InitClipboardFormats();

    RasEntry (RAS_EVENT_User32InitComplete, &hInstanceUser32, sizeof (hInstanceUser32));

    return EXITLIST_USER32;
}

void SYSTEM DLL_TermUser32(ULONG hModule)
{
    dprintf(("user32 exit\n"));
    __con_debug(2,"%s::%s@%08X(%08X)\n","USER32",__FUNCTION__,DLL_TermUser32,hModule);
 //SvL: Causes PM hangs on some (a lot?) machines. Reason unknown.
 ////   RestoreCursor();

    //Destroy CD notification window
    WinDestroyWindow(hwndCD);
    DestroyDesktopWindow();
    Win32BaseWindow::DestroyAll();
    UnregisterSystemClasses();
    Win32WndClass::DestroyAll();
    MONITOR_Finalize(&MONITOR_PrimaryMonitor);
    SYSCOLOR_Save();
    CloseSpyQueue();
    FinalizeWindowHandles();
    STATS_DumpStatsUSER32();
    dprintf(("user32 exit done\n"));

    if (hInstanceUser32) {
        UnregisterLxDll(hInstanceUser32);
    }
    STATS_UninitializeUSER32();
}

ULONG SYSTEM DLL_Init(ULONG hModule)
{
    if (DLL_InitDefault(hModule) == -1)
        return -1;
    return DLL_InitUser32(hModule);
}

void SYSTEM DLL_Term(ULONG hModule)
{
    DLL_TermUser32(hModule);
    DLL_TermDefault(hModule);
}
