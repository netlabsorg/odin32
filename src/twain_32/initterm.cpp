/*
 * DLL entry point
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
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <odin.h>
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:18:15*/
#include <twain.h>
#include <initdll.h>

extern "C" {
//Win32 resource table (produced by wrc)
extern DWORD _Resource_PEResTab;

extern FARPROC   WINAPI GetProcAddress(HMODULE,LPCSTR);
extern HMODULE   WINAPI LoadLibraryA(LPCSTR);
extern BOOL      WINAPI FreeLibrary(HMODULE);
extern int WINAPI PROFILE_GetOdinIniInt(LPCSTR section,LPCSTR key_name,int def);
TW_UINT16 (APIENTRY *TWAINOS2_DSM_Entry)( pTW_IDENTITY, pTW_IDENTITY,
    	                                  TW_UINT32, TW_UINT16, TW_UINT16, TW_MEMREF) = 0;
static HINSTANCE hTWAIN = 0;

}
static HMODULE dllHandle = 0;

//******************************************************************************
//******************************************************************************
BOOL WINAPI LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID fImpLoad)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
	return TRUE;

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
	return TRUE;

   case DLL_PROCESS_DETACH:
	ctordtorTerm();
	return TRUE;
   }
   return FALSE;
}
/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
unsigned long SYSTEM _DLL_InitTerm(unsigned long hModule, unsigned long
                                   ulFlag)
{
   size_t i;
   APIRET rc;
   ULONG  twaintype;

   /*-------------------------------------------------------------------------*/
   /* If ulFlag is zero then the DLL is being loaded so initialization should */
   /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
   /* termination should be performed.                                        */
   /*-------------------------------------------------------------------------*/

   switch (ulFlag) {
      case 0 :
         ctordtorInit();

         CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

	twaintype = PROFILE_GetOdinIniInt("TWAIN","TwainIF",1);
	switch(twaintype) 
	{
	 case 1:
	 default:
         {
            dprintf(("TWAIN_32: Using CFM-Twain as Twain Source.\n\n"));
            hTWAIN = LoadLibraryA("TWAINOS2.DLL");
            if(hTWAIN) 
            {   
                *(VOID **)&TWAINOS2_DSM_Entry=(void*)GetProcAddress(hTWAIN, (LPCSTR)"DSM_Entry");
            }
	    else
	    {
              return 0UL;
	    }
	    break;
         }
	 case 2:
         {
            dprintf(("TWAIN_32: Using STI-Twain as Twain Source.\n\n"));
            hTWAIN = LoadLibraryA("TWAIN.DLL");
            if(hTWAIN) 
            {   
                *(VOID **)&TWAINOS2_DSM_Entry=(void*)GetProcAddress(hTWAIN, (LPCSTR)"DSM_ENTRY");
            }
	    else
	    {
              return 0UL;
	    }
	    break;
         }
	 case 3:
         {
            dprintf(("TWAIN_32: Using SANE as Twain Source  (currently not supported).\n\n"));
//            hTWAIN = LoadLibraryA("TWAINSNE.DLL");
//            if(hTWAIN) 
//            {   
//                *(VOID **)&TWAINOS2_DSM_Entry=(void*)GetProcAddress(hTWAIN, (LPCSTR)"DSM_Entry");
//            }
            return 0UL;
         }
	}

	 dllHandle = RegisterLxDll(hModule, LibMain, (PVOID)&_Resource_PEResTab);
         if(dllHandle == 0) 
		return 0UL;

         break;
      case 1 :
         if(hTWAIN) 
	 	FreeLibrary(hTWAIN);
         hTWAIN = 0;
         if(dllHandle) {
	 	UnregisterLxDll(dllHandle);
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
