/* $Id: driver.cpp,v 1.7 2000-12-03 22:18:17 sandervl Exp $ */

/*
 * Driver multimedia apis
 *
 * Copyright 1994 Martin Ayotte
 * Copyright 1998 Marcus Meissner
 * Copyright 1998 Patrick Haller
 * Copyright 1998/1999 Eric Pouech
 * Copyright 2000 Chris Wohlgemuth
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <odinwrap.h>
#include <string.h>
#include <misc.h>
#include <unicode.h>

#include "os2timer.h"

#define DBG_LOCALLOG	DBG_driver
#include "dbglocal.h"


/* Who said goofy boy ? */
#define	WINE_DI_MAGIC	0x900F1B01

typedef struct tagWINE_DRIVER
{
    DWORD			dwMagic;
    char			szAliasName[128];
    /* as usual LPWINE_DRIVER == hDriver32 */
    HDRVR16			hDriver16;
    union {
       struct {
	  HMODULE16		hModule;
	  DRIVERPROC16          lpDrvProc;
       } d16;
       struct {
	  HMODULE		hModule;
	  DRIVERPROC		lpDrvProc;
       } d32;
    } d;
    DWORD		  	dwDriverID;
    DWORD			dwFlags;
    struct tagWINE_DRIVER*	lpPrevItem;
    struct tagWINE_DRIVER*	lpNextItem;
} WINE_DRIVER, *LPWINE_DRIVER;

static LPWINE_DRIVER	DRIVER_RegisterDriver32(LPCSTR, HMODULE,   DRIVERPROC,   LPARAM, BOOL);

static LPWINE_DRIVER	lpDrvItemList = NULL;

/* TODO list :
 *	WINE remark: - LoadModule count and clean up is not handled correctly (it's not a 
 *	  problem as long as FreeLibrary is not working correctly)
 *	- shoudln't the allocations be done on a per process basis ?
 */

static HDRVR DRIVER_OpenDriverA(LPCSTR lpDriverName, LPCSTR lpSectionName, LPARAM lParam) ;
static	LPWINE_DRIVER	DRIVER_FindFromHDrvr(HDRVR hDrvr);
static BOOL DRIVER_CloseDriver(LPWINE_DRIVER lpDrv, DWORD lParam1, DWORD lParam2);

ODINDEBUGCHANNEL(WINMM-DRIVER)


/*****************************************************************************
 *             Sends the specifiedmessage to the installable driver
 * Parameters: HDRVR hDrvr
 *             UINT  msg
 *             LONG  lParam1
 *             LONG  lParam2
 * Variables :
 * @return   : API returncode
 * Remark    :
 * @status   : Completely
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 * @author    : Chris Wohlgemuth [Sun, 2000/11/19]
 *****************************************************************************/

ODINFUNCTION4(LRESULT, SendDriverMessage,
              HDRVR, hdrvr,
              UINT, msg,
              LONG, lParam1,
              LONG, lParam2)
{
    LPWINE_DRIVER	lpDrv;
    LRESULT 		retval = 0;

    dprintf(("Entering SendDriverMessage (%04x, %04X, %08lX, %08lX)\n", hdrvr, msg, lParam1, lParam2));
    
    lpDrv = DRIVER_FindFromHDrvr(hdrvr);

    if (lpDrv != NULL) {
	if (lpDrv->dwFlags & WINE_GDF_16BIT) {
#if 0
      /*We don't use 16 bit */
      mapRet = DRIVER_MapMsg32To16(msg, &lParam1, &lParam2);
      if (mapRet >= 0) {
		TRACE("Before CallDriverProc proc=%p driverID=%08lx hDrv=%u wMsg=%04x p1=%08lx p2=%08lx\n", 
		      lpDrv->d.d16.lpDrvProc, lpDrv->dwDriverID, lpDrv->hDriver16, msg, lParam1, lParam2);		  
		retval = DRIVER_CallTo16_long_lwwll((FARPROC16)lpDrv->d.d16.lpDrvProc, lpDrv->dwDriverID, 
                                            lpDrv->hDriver16, msg, lParam1, lParam2);
		if (mapRet >= 1) {
          DRIVER_UnMapMsg32To16(msg, lParam1, lParam2);
		}
      } else {
		retval = 0;
      }
#endif
	} else {

#if 0
      dprintf(("Before func32 call proc=%p driverID=%08lx hDrv=%u wMsg=%04x p1=%08lx p2=%08lx\n", 
               lpDrv->d.d32.lpDrvProc, lpDrv->dwDriverID, hdrvr, msg, lParam1, lParam2));		  
#endif

      retval = lpDrv->d.d32.lpDrvProc(lpDrv->dwDriverID, hdrvr, msg, lParam1, lParam2);
	}
    } else {
      dprintf(("Bad driver handle %u\n", hdrvr));
    }

    dprintf(("(SendDriverMessage) Driver proc returned retval = %ld (0x%lx)\n", retval, retval));
    
    return retval;
}


/*****************************************************************************
 *             Opens an instance of an installable driver and initializes
 *             the instance using either the driver's default settings or a
 *             driver-specific value.
 * Parameters: LPCWSTR lpDriverName
 *             LPCWSTR lpSectionName
 *             LONG    lParam
 * Variables :
 * @return   : API returncode
 * Remark    :
 * @status   : Stub
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION3(HDRVR, OpenDriver,
              LPCWSTR, lpDriverName,
              LPCWSTR, lpSectionName,
              LONG, lParam)
{
  dprintf(("WINMM: OpenDriver not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Opens an instance of an installable driver and initializes
 *             the instance using either the driver's default settings or a
 *             driver-specific value.
 * Parameters: LPCTSTR lpDriverName
 *             LPCTSTR lpSectionName
 *             LONG    lParam
 * Variables :
 * @return   : API returncode
 * Remark    :
 * @status   : Partially tested
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION3(HDRVR, OpenDriverA,
              LPCSTR, lpDriverName,
              LPCSTR, lpSectionName,
              LONG, lParam)
{
  return DRIVER_OpenDriverA(lpDriverName, lpSectionName,  lParam); 
  //  return 0; /* unsuccessful return */
}



/*****************************************************************************
 *             Closes an installable driver.
 * Parameters: HDRVR hDrvr
 *             LONG  lParam1
 *             LONG  lParam2
 * Variables :
 * @return   :
 * Remark    :
 * @status   : Partially tested
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION3(LRESULT, CloseDriver,
              HDRVR, hDrvr,
              LONG, lParam1,
              LONG, lParam2)
{
  return DRIVER_CloseDriver(DRIVER_FindFromHDrvr(hDrvr), lParam1, lParam2);//return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Provides default processing for anymessages not processed by
 *             an installable driver.
 * Parameters: DWORD dwDriverID
 *             HDRVR hDrvr
 *             UINT  msg
 *             LONG  lParam1
 *             LONG  lParam2
 * Variables :
 * @return   :
 * Remark    :
 * @status   : Partially
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 * @author    : Chris Wohlgemuth [Sun, 2000/11/19]
 *****************************************************************************/

ODINFUNCTION5(LRESULT, DefDriverProc,
              DWORD, dwDriverID,
              HDRVR, hDrvr,
              UINT, msg,
              LONG, lParam1,
              LONG, lParam2)
{
  switch (msg) {
  case DRV_LOAD:
  case DRV_FREE:
  case DRV_ENABLE:
  case DRV_DISABLE:
    // return 1;
  case DRV_INSTALL:
  case DRV_REMOVE:
    //  return DRV_SUCCESS;
  default:
    return 0; /* unsuccessful return */
  }

}


/*****************************************************************************
 * Parameters: DWORD dwCallback
 *             DWORD dwFlags
 *             HDRVR hDrvr
 *             DWORD msg
 *             DWORD dwUser
 *             DWORD dwParam1
 *             DWORD dwParam2
 * Variables :
 * @return   :
 * Remark    :
 * @status   : Stub
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION7(LRESULT, DriverCallback,
              DWORD, dwCallback,
              DWORD, dwFlags,
              HDRVR, hDrvr,
              DWORD, msg,
              DWORD, dwUser,
              DWORD, dwParam1,
              DWORD, dwParam2)
{
  dprintf(("WINMM: DriverCallback not implemented.\n"));
  return FALSE; /* unsuccessful return */
}


/*****************************************************************************
 * Purpose   : Retrieves the instance handle of the module that contains the
 *             installable driver.
 * Parameters: HDRVR hDriver
 * Variables :
 * @return   :
 * Remark    :
 * @status   : Stub
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION1(HMODULE, DrvGetModuleHandle,
              HDRVR, hDriver)
{
  dprintf(("WINMM: DrvGetModuleHandle not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Sends the specified message to the installable driver
 * Parameters: HDRVR hDrvr
 *             UINT  msg
 *             LONG  lParam1
 *             LONG  lParam2
 * Variables :
 * @return   : API returncode
 * Remark    : Ehm, what's the difference to SendDriverMessage() ?
 * @status   : Stub
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION4(LRESULT, DrvSendMessage,
              HDRVR, hdrvr,
              UINT, msg,
              LONG, lParam1,
              LONG, lParam2)
{
  dprintf(("WINMM: DrvSendMessage not implemented.\n"));
  return 0; /* unsuccessful return */
}


/*****************************************************************************
 *             Retrieves the instance handle of the module that contains the
 *             installable driver.
 * Parameters: HDRVR hDriver
 * Variables :
 * @return   :
 * Remark    : What's the difference to DrvGetModuleHandle() ?
 * @status   : Completely
 *
 * @author    : Patrick Haller [Tue, 1998/05/05 10:44]
 * @author    : Chris Wohlgemuth [Sun, 2000/11/19]
 *****************************************************************************/

ODINFUNCTION1(HMODULE, GetDriverModuleHandle,
              HDRVR, hDriver)
{
  LPWINE_DRIVER 	lpDrv;
  HMODULE		hModule = 0;
  
#if 0
  dprintf(("(%04x);\n", hDriver));
#endif

  lpDrv = DRIVER_FindFromHDrvr(hDriver);
  if (lpDrv != NULL && !(lpDrv->dwFlags & WINE_GDF_16BIT)) {
	hModule = lpDrv->d.d32.hModule;
  
}
#if 0
  dprintf(("=> %d\n", hModule));
#endif
  return hModule; //return 0; /* unsuccessful return */
}



/**************************************************************************
 *				GetDriverFlags		[WINMM.13]
 * Parameters: HDRVR hDrvr
 * Variables :
 * @return   :	0x00000000 if hDrvr is an invalid handle
 *             	0x80000000 if hDrvr is a valid 32 bit driver
 * Remark    :
 * @status   : Completely
 *
 * @author    : Chris Wohlgemuth [Sun, 2000/11/19] (ported from WINE)
 */
ODINFUNCTION1(DWORD, GetDriverFlags,
              HDRVR, hDrvr)
{
    LPWINE_DRIVER 	lpDrv;
    DWORD		ret = 0;

    //TRACE("(%04x)\n", hDrvr);

    if ((lpDrv = DRIVER_FindFromHDrvr(hDrvr)) != NULL) {
      ret = lpDrv->dwFlags;
    }
    return ret;
}


/**************************************************************************/
/*              Implementation                                            */
/**************************************************************************/

/**************************************************************************
 *			DRIVER_GetNumberOfModuleRefs		[internal]
 *
 * Returns the number of open drivers which share the same module.
 */
static	WORD	DRIVER_GetNumberOfModuleRefs(LPWINE_DRIVER lpNewDrv)
{
    LPWINE_DRIVER	lpDrv;
    WORD		count = 0;
    
    for (lpDrv = lpDrvItemList; lpDrv; lpDrv = lpDrv->lpNextItem) {
	if (lpDrv->dwFlags & WINE_GDF_16BIT) {
	    if (lpDrv->d.d16.hModule == lpNewDrv->d.d16.hModule) {
		count++;
	    }
	} else {
	    if (lpDrv->d.d32.hModule == lpNewDrv->d.d32.hModule) {
		count++;
	    }
	}
    }
    return count;
}


/**************************************************************************
 *				DRIVER_FindFromHDrvr		[internal]
 * 
 * From a hDrvr (being 16 or 32 bits), returns the WINE internal structure.
 */
static	LPWINE_DRIVER	DRIVER_FindFromHDrvr(HDRVR hDrvr)
{    
  if (!IsBadWritePtr((void*)hDrvr, sizeof(WINE_DRIVER)) && 
      ((LPWINE_DRIVER)hDrvr)->dwMagic == WINE_DI_MAGIC) {
	return (LPWINE_DRIVER)hDrvr;
  }
  return 0;
}

/**************************************************************************
 *				DRIVER_RemoveFromList		[internal]
 *
 * Generates all the logic to handle driver closure / deletion
 * Removes a driver struct to the list of open drivers.
 *
 */
static	BOOL	DRIVER_RemoveFromList(LPWINE_DRIVER lpDrv)
{
    lpDrv->dwDriverID = 0;
    if (DRIVER_GetNumberOfModuleRefs(lpDrv) == 1) {
      SendDriverMessage((HDRVR)lpDrv, DRV_DISABLE, 0L, 0L);
      SendDriverMessage((HDRVR)lpDrv, DRV_FREE,    0L, 0L);
    }
    
    if (lpDrv->lpPrevItem)
	lpDrv->lpPrevItem->lpNextItem = lpDrv->lpNextItem;
    else
	lpDrvItemList = lpDrv->lpNextItem;
    if (lpDrv->lpNextItem)
	lpDrv->lpNextItem->lpPrevItem = lpDrv->lpPrevItem;

    return TRUE;
}

/**************************************************************************
 *				DRIVER_AddToList		[internal]
 *
 * Adds a driver struct to the list of open drivers.
 * Generates all the logic to handle driver creation / open.
 *
 */
static	BOOL	DRIVER_AddToList(LPWINE_DRIVER lpNewDrv, LPARAM lParam, BOOL bCallFrom32)
{
  lpNewDrv->dwMagic = WINE_DI_MAGIC;
  /* First driver to be loaded for this module, need to load correctly the module */

  if (DRIVER_GetNumberOfModuleRefs(lpNewDrv) == 0) {
	if (SendDriverMessage((HDRVR)lpNewDrv, DRV_LOAD, 0L, 0L) != DRV_SUCCESS) {
      dprintf(("DRV_LOAD failed on driver 0x%08lx\n", (DWORD)lpNewDrv));
      return FALSE;
	}
	if (SendDriverMessage((HDRVR)lpNewDrv, DRV_ENABLE, 0L, 0L) != DRV_SUCCESS) {
      dprintf(("DRV_ENABLE failed on driver 0x%08lx\n", (DWORD)lpNewDrv));
      return FALSE;
	}
  }
  
  lpNewDrv->lpNextItem = NULL;
  if (lpDrvItemList == NULL) {
	lpDrvItemList = lpNewDrv;
	lpNewDrv->lpPrevItem = NULL;
  } else {
	LPWINE_DRIVER	lpDrv = lpDrvItemList;	/* find end of list */
	while (lpDrv->lpNextItem != NULL)
      lpDrv = lpDrv->lpNextItem;
	
	lpDrv->lpNextItem = lpNewDrv;
	lpNewDrv->lpPrevItem = lpDrv;
  }
  /* Now just open a new instance of a driver on this module */
  
  lpNewDrv->dwDriverID = SendDriverMessage((HDRVR)lpNewDrv, DRV_OPEN, 0L, lParam);
  
  if (lpNewDrv->dwDriverID == 0) {
	dprintf(("DRV_OPEN failed on driver 0x%08lx\n", (DWORD)lpNewDrv));
	DRIVER_RemoveFromList(lpNewDrv);
	return FALSE;
  }
    
  return TRUE;
}

/**************************************************************************
 *				DRIVER_RegisterDriver32		[internal]
 *
 * Creates all the WINE internal representations for a 32 bit driver.
 * The driver is also open by sending the correct messages.
 *
 */
static LPWINE_DRIVER DRIVER_RegisterDriver32(LPCSTR lpName, HMODULE hModule, DRIVERPROC lpProc, 
                                      LPARAM lParam, BOOL bCallFrom32)
{
  LPWINE_DRIVER	lpDrv;
  
  lpDrv = (LPWINE_DRIVER)HeapAlloc(GetProcessHeap(), 0, sizeof(WINE_DRIVER));
  if (lpDrv != NULL) {
	lpDrv->dwFlags          = WINE_GDF_EXIST;
	lpDrv->dwDriverID       = 0;
    lpDrv->hDriver16        = 0;  /* We don't support 16 bit */
	lstrcpynA(lpDrv->szAliasName, lpName, sizeof(lpDrv->szAliasName));
	lpDrv->d.d32.hModule    = hModule;
	lpDrv->d.d32.lpDrvProc  = lpProc;

	if (!DRIVER_AddToList(lpDrv, lParam, bCallFrom32)) {
      HeapFree(GetProcessHeap(), 0, lpDrv);
      lpDrv = NULL;
	}

  }
  return lpDrv;
}

/**************************************************************************
 *				DRIVER_TryOpenDriver32		[internal]
 *
 * Tries to load a 32 bit driver whose DLL's (module) name is lpFileName.
 *
 */
static	HDRVR	DRIVER_TryOpenDriver32(LPCSTR lpFileName, LPARAM lParam, BOOL bCallFrom32)
{
    LPWINE_DRIVER 	lpDrv = NULL;
    LPCSTR		lpSFN;
    HMODULE		hModule;
    DRIVERPROC		lpProc;
    
    dprintf(("'Entering DRIVER_TryOpenDriver32: %s', %08lX, %d);\n", lpFileName, lParam, bCallFrom32));
    
    if (strlen(lpFileName) < 1) 
      return 0;
    
    lpSFN = strrchr(lpFileName, '\\');
    lpSFN = (lpSFN) ? (lpSFN + 1) : lpFileName;
    
    if ((hModule = LoadLibraryA(lpFileName)) != 0) {
      if ((lpProc = (DRIVERPROC)GetProcAddress(hModule, "DriverProc")) != NULL) {
        lpDrv = DRIVER_RegisterDriver32(lpSFN, hModule, lpProc, lParam, bCallFrom32);
      } else {
	    FreeLibrary(hModule);
	    lpDrv = 0;
	    dprintf(("DRIVER_TryOpenDriver32: No DriverProc found (line %d)\n",__LINE__));
      }
    } else {
      dprintf(("DRIVER_TryOpenDriver32: Unable to load 32 bit module \"%s\" (line %d)\n", lpFileName,__LINE__));
    }

    dprintf(("(DRIVER_TryOpenDriver32) driver handle => %p\n", lpDrv));

    return (HDRVR)lpDrv;
}

/**************************************************************************
 *				DRIVER_OpenDriverA		        [internal]
 * (0,1,DRV_LOAD  ,0       ,0)
 * (0,1,DRV_ENABLE,0       ,0)
 * (0,1,DRV_OPEN  ,buf[256],0)
 *
 */
static HDRVR  DRIVER_OpenDriverA(LPCSTR lpDriverName, LPCSTR lpSectionName, LPARAM lParam) 
{
  HDRVR 		hDriver = 0;
  char 		drvName[128];
  
  dprintf(("Entering DRIVER_OpenDriverA: lpDriverName: %s lpSectionName: %s\n",lpDriverName,lpSectionName));

  if (lpSectionName == NULL) {
	lstrcpynA(drvName, lpDriverName, sizeof(drvName));
	hDriver = DRIVER_TryOpenDriver32(lpDriverName, lParam, TRUE);
	if (!hDriver) {
      if (GetPrivateProfileStringA("Drivers32", lpDriverName, "", drvName,
                                   sizeof(drvName), "SYSTEM.INI")) {

		hDriver = DRIVER_TryOpenDriver32(drvName, lParam, TRUE);		
      }
	}
  } else {/* of if (lpSectionName == NULL) */
    //dprintf(("driver name %x '%s'\n",drvName,drvName));
    drvName[0]=0;    

	if (GetPrivateProfileStringA(lpSectionName, lpDriverName, "", drvName,
                                 sizeof(drvName)-1, "SYSTEM.INI")) {
#if 0
      dprintf(("driver name %x '%s'\n",drvName,drvName));
#endif      
      hDriver = DRIVER_TryOpenDriver32(drvName, lParam, TRUE);
	}/* GetPrivate... */
  }
  if (!hDriver)
	dprintf(("OpenDriverA: Failed to open driver %s from section %s\n", lpDriverName, lpSectionName));

  else 
	dprintf(("OpenDriverA success: Driver handle => %08x\n", hDriver));

  return hDriver;
}

/**************************************************************************
 *				DRIVER_CloseDriver		[internal]
 *
 *
 */
static BOOL DRIVER_CloseDriver(LPWINE_DRIVER lpDrv, DWORD lParam1, DWORD lParam2)
{
  if (lpDrv != NULL) {
	SendDriverMessage((HDRVR)lpDrv, DRV_CLOSE, lParam1, lParam2);
    
	if (DRIVER_RemoveFromList(lpDrv)) {
      HeapFree(GetProcessHeap(), 0, lpDrv);
      return TRUE;
	}
  }
  dprintf(("Failed to close driver\n"));
  return FALSE;
}












