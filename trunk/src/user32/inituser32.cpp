/* $Id: inituser32.cpp,v 1.17 2003-11-12 14:10:19 sandervl Exp $ */
/*
 * USER32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*-------------------------------------------------------------*/
/* INITERM.C -- Source for a custom dynamic link library       */
/*              initialization and termination (_DLL_InitTerm) */
/*              function.                                      */
/*                                                             */
/* When called to perform initialization, this sample function */
/* gets storage for an array of integers, and initializes its  */
/* elements with random integers.  At termination time, it     */
/* frees the array.  Substitute your own special processing.   */
/*-------------------------------------------------------------*/


/* Include files */
#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSMISC
#define  INCL_DOSERRORS
#define  INCL_WINSHELLDATA
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
#include <kbdhook.h>
#include "pmwindow.h"
#include "win32wdesktop.h"
#include "win32wndhandle.h"
#include "syscolor.h"
#include "initterm.h"
#include <exitlist.h>
#include <initdll.h>
#include <stats.h>

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);

extern "C" {
 //Win32 resource table (produced by wrc)
 extern DWORD user32_PEResTab;
}
DWORD hInstanceUser32 = 0;

extern INT __cdecl wsnprintfA(LPSTR,UINT,LPCSTR,...);

//******************************************************************************
#define FONTSDIRECTORY "Fonts"
#define REGPATH "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"
//******************************************************************************
void MigrateWindowsFonts()
{
  HKEY  hkFonts,hkOS2Fonts;
  char  buffer[512];
  UINT  len = GetWindowsDirectoryA( NULL, 0 );
  
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
         //Check OS/2 INI profile for font entry
         if (!PrfQueryProfileString(HINI_PROFILE, "PM_Fonts", dataArray,
                            NULL, (PVOID)subKeyName, (LONG)sizeof(subKeyName)))
         {
           HDIR          hdirFindHandle = HDIR_CREATE;
           FILEFINDBUF3  FindBuffer     = {0};      
           ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
           ULONG         ulFindCount    = 1;       
           APIRET        rc             = NO_ERROR; 

           dprintf(("Migrating font %s to OS/2",dataArray));

           GetWindowsDirectoryA( buffer, len + 1 );
           wsnprintfA( buffer, sizeof(buffer), "%s\\%s\\%s", buffer, FONTSDIRECTORY, dataArray );

           rc = DosFindFirst( buffer, &hdirFindHandle, FILE_NORMAL,&FindBuffer, ulResultBufLen, &ulFindCount, FIL_STANDARD);

           //Check that file actaully exist 
  	   if ( rc == NO_ERROR  && !(FindBuffer.attrFile & FILE_DIRECTORY))
  	   {
              PrfWriteProfileString(HINI_PROFILE,"PM_Fonts",dataArray, buffer);   
	      DosFindClose(hdirFindHandle);
           }
        } 
      }
      RegCloseKey(hkFonts);
  }
}
/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
ULONG APIENTRY inittermUser32(ULONG hModule, ULONG ulFlag)
{
   size_t i;
   APIRET rc;
   ULONG  version[2];

   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :
         ParseLogStatusUSER32();

         InitializeKernel32();
         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

         hInstanceUser32 = RegisterLxDll(hModule, 0, (PVOID)&user32_PEResTab,
                                         USER32_MAJORIMAGE_VERSION, USER32_MINORIMAGE_VERSION,
                                         IMAGE_SUBSYSTEM_WINDOWS_GUI);
         if(hInstanceUser32 == 0)
                return 0UL;

         dprintf(("user32 init %s %s (%x)", __DATE__, __TIME__, inittermUser32));

         //SvL: Try to start communication with our message spy queue server
         InitSpyQueue();

         //SvL: Init win32 PM classes
         //PH:  initializes HAB!
         if (FALSE == InitPM())
           return 0UL;

         InitializeWindowHandles();

         //SvL: 18-7-'98, Register system window classes (button, listbox etc etc)
         //CB: register internal classes
         RegisterSystemClasses(hModule);

         //CB: initialize PM monitor driver
         MONITOR_Initialize(&MONITOR_PrimaryMonitor);

         //PF: migrate windows fonts
         MigrateWindowsFonts();

         break;


       case 1 :
         if(hInstanceUser32) {
            UnregisterLxDll(hInstanceUser32);
         }
         break;
      default  :
         return 0UL;
   }

   /***********************************************************/
   /* A non-zero value must be returned to indicate success.  */
   /***********************************************************/
   return 1UL;
}
//******************************************************************************
//******************************************************************************
void APIENTRY cleanupUser32(ULONG ulReason)
{
  dprintf(("user32 exit\n"));

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
}
//******************************************************************************
//******************************************************************************
