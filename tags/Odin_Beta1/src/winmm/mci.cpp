/* $Id: mci.cpp,v 1.7 2001-04-04 09:02:16 sandervl Exp $ */

/*
 * MCI functions
 *
 * Copyright 1998 Joel Troster
 * Copyright 1998/1999 Eric Pouech
 * Copyright 2000 Chris Wohlgemuth
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#define  INCL_BASE
#define  INCL_OS2MM

#include <os2win.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <string.h>
#include <stdio.h>
#include "debugtools.h" 

#include <misc.h>
#include <unicode.h>

#include "mcimm.h"

#include "winmm.h"

#define DBG_LOCALLOG	DBG_mci

#include "dbglocal.h"


/****************************************************************************
 * Local functions                                                          *
 ****************************************************************************/

/* forward declaration */
static DWORD  MCI_SendCommand(UINT mciId,
              UINT16 uMsg,
              DWORD dwParam1,
              DWORD dwParam2);

static LPWINE_MCIDRIVER	MCI_GetDriver(UINT16 wDevID) ;
static UINT	MCI_GetDriverFromString(LPCSTR lpstrName);

/****************************************************************************/

ODINDEBUGCHANNEL(WINMM-MCI)


ODINFUNCTION3(BOOL, mciDriverNotify,
              HWND, hwndCallback,
              UINT, uDeviceID,
              UINT, uStatus)
{
  TRACE("Entering mciDriverNotify (%08X, %04x, %04X)\n", hwndCallback, uDeviceID, uStatus);
  
  if (!IsWindow(hwndCallback)) {
	WARN("bad hwnd for call back (0x%04x)\n", hwndCallback);
	return FALSE;
  }
  TRACE("before PostMessage\n");
  PostMessageA(hwndCallback, MM_MCINOTIFY, uStatus, uDeviceID);
  return TRUE;
}

ODINFUNCTION1(UINT, mciDriverYield,
              UINT, uDeviceID)
{
  dprintf(("WINMM:mciDriverYield - stub\n"));
  return 0;
}

ODINFUNCTION1(BOOL, mciExecute,
              LPCSTR, pszCommand)
{
  dprintf(("WINMM:mciExecute(%s) - stub\n", pszCommand));
  return FALSE;
}

ODINFUNCTION1(BOOL, mciFreeCommandResource,
              UINT, uTable)
{
  dprintf(("WINMM:mciFreeCommandResource - stub\n"));
  return FALSE;
}

ODINFUNCTION1(HTASK, mciGetCreatorTask,
              MCIDEVICEID, mciId)
{
  dprintf(("WINMM:mciGetCreatorTask - stub\n"));
  return 0;
}

ODINFUNCTION1(MCIDEVICEID, mciGetDeviceIDA,
              LPCSTR, pszDevice)
{
  WARN(("WINMM:mciGetDeviceIDA - untested\n"));
  return MCI_GetDriverFromString(pszDevice);
}

ODINFUNCTION1(MCIDEVICEID, mciGetDeviceIDW,
              LPCWSTR, pszDevice)
{
  dprintf(("WINMM:mciGetDeviceIDW - stub\n"));
  return 0;
}

ODINFUNCTION2(MCIDEVICEID, mciGetDeviceIDFromElementIDA,
              DWORD, dwElementID,
              LPCSTR, lpstrType)
{
  dprintf(("WINMM:mciGetDeviceIDFromElementIDA - stub\n"));
  return 0;
}

ODINFUNCTION2(MCIDEVICEID, mciGetDeviceIDFromElementIDW,
              DWORD, dwElementID,
              LPCWSTR, lpstrType)
{
  dprintf(("WINMM:mciGetDeviceIDFromElementIDW - stub\n"));
  return 0;
}

/*****************************************************************************
 *             Queries driver data
 * Parameters: UINT uDeviceID
 * Variables :
 * @return   :  Pointer to driver data (as a DWORD)
 * Remark    :
 * @status   : Completely
 *
 * @author   : Chris Wohlgemuth [Sun, 2000/11/19]
 *****************************************************************************/
ODINFUNCTION1(DWORD, mciGetDriverData,
              UINT, uDeviceID)
{
  LPWINE_MCIDRIVER	wmd;
  
  wmd = MCI_GetDriver(uDeviceID);
  
  if (!wmd) {
	dprintf(("WARNING: Bad uDeviceID (mciGetDriverData (mci.cpp line %d)\n",__LINE__));
    return 0L; /* Error */
  }
    
  return wmd->dwPrivate;
}

/*****************************************************************************
 *             Converts an error to an error string
 * Parameters: MCIERROR mcierr,
 *             LPSTR pszText,
 *             UINT cchText
 * Variables :
 * @return   : API returncode (TRUE/FALSE)
 * Remark    :
 * @status   : Completely
 *
 * @author   : Wine
 *****************************************************************************/
ODINFUNCTION3(BOOL, mciGetErrorStringA,
              MCIERROR, mcierr,
              LPSTR, pszText,
              UINT, cchText)
{
  dprintf(("WINMM:mciGetErrorStringA(%d)\n", mcierr ));
  char * theMsg = getWinmmMsg( mcierr );
  if ( theMsg )
    strncpy( pszText, theMsg, cchText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", mcierr );
    strncpy( pszText, errMsg, cchText );
  }
  return TRUE;
}

/*****************************************************************************
 *             Converts an error to an error string
 * Parameters: MCIERROR mcierr,
 *             LPSTR pszText,
 *             UINT cchText
 * Variables :
 * @return   : API returncode (TRUE/FALSE)
 * Remark    :
 * @status   : Completely
 *
 * @author   : Wine
 *****************************************************************************/
ODINFUNCTION3(BOOL, mciGetErrorStringW,
              MCIERROR, mcierr,
              LPWSTR, pszText,
              UINT, cchText)
{
  dprintf(("WINMM:mciGetErrorStringW(%d)\n", mcierr ));
  char * theMsg = getWinmmMsg( mcierr );
  if ( theMsg )
    AsciiToUnicode( theMsg, pszText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", mcierr );
    AsciiToUnicode( errMsg, pszText );
  }
  return TRUE;
}

/*****************************************************************************
 *             Converts an error to an error string
 * Parameters: MCIERROR mcierr,
 *             LPSTR pszText,
 *             UINT cchText
 * Variables :
 * @return    : API returncode
 * Remark    :
 * @status    : Completely
 *
 * @author    : Wine
 *****************************************************************************/
ODINFUNCTION2(YIELDPROC, mciGetYieldProc,
              MCIDEVICEID, mciId,
              LPDWORD, pdwYieldData)
{
  LPWINE_MCIDRIVER	wmd;
  
  TRACE("Entering mciGetYieldProc (%u, %p) - untested\n", mciId, pdwYieldData);

  if (!(wmd = MCI_GetDriver(mciId))) {
	WARN("Bad uDeviceID\n");
	return NULL;
  }
  if (!wmd->lpfnYieldProc) {
	WARN("No proc set\n");
	return NULL;
  }
  if (!wmd->bIs32) {
	WARN("Proc is 32 bit\n");
	return NULL;
  }
  return wmd->lpfnYieldProc;
}

ODINFUNCTION3(UINT, mciLoadCommandResource,
              HINSTANCE, hInstance,
              LPCWSTR, lpResName,
              UINT, uType)
{
  dprintf(("WINMM:mciLoadCOmmandResource - stub\n"));
  return 0;
}


ODINFUNCTION4(MCIERROR, mciSendCommandA,
              MCIDEVICEID, mciId,
              UINT, uMsg,
              DWORD, dwParam1,
              DWORD, dwParam2)
{
  DWORD dwRet;
  //  dprintf(("WINMM:mciSendCommandA - entering %X %X %X %X\n", mciId, uMsg, dwParam1, dwParam2));
  dwRet= MCI_SendCommand((UINT) mciId,  uMsg,  dwParam1, dwParam2) & 0xFFFF;      
  return(dwRet);
}


ODINFUNCTION4(MCIERROR, mciSendCommandW,
              MCIDEVICEID, mciId,
              UINT, uMsg,
              DWORD, dwParam1,
              DWORD, dwParam2)
{
  dprintf(("WINMM:mciSendCommandW - stub %X %X %X %X\n", mciId, uMsg, dwParam1, dwParam2));
  return(MMSYSERR_NODRIVER);
}

ODINFUNCTION4(MCIERROR, mciSendStringA,
              LPCSTR, lpstrCommand,
              LPSTR, lpstrReturnString,
              UINT, uReturnLength,
              HWND, hwndCallback)
{
  dprintf(("WINMM:mciSendStringA - stub\n"));
  if(lpstrCommand)
    dprintf(("WINMM:mciSendStringA command: %s\n",lpstrCommand));
  return(MMSYSERR_NODRIVER);
}

ODINFUNCTION4(MCIERROR, mciSendStringW,
              LPCWSTR, lpstrCommand,
              LPWSTR, lpstrReturnString,
              UINT, uReturnLength,
              HWND, hwndCallback)
{
  dprintf(("WINMM:mciSendStringW - stub\n"));
  return(MMSYSERR_NODRIVER);
}

ODINFUNCTION2(BOOL, mciSetDriverData,
              UINT, uDeviceID,
              DWORD, dwData)
{
  LPWINE_MCIDRIVER	wmd;
  
  wmd = MCI_GetDriver(uDeviceID);

  if (!wmd) {
	dprintf(("WARNING: Bad uDeviceID (mciSetDriverData line %d)\n",__LINE__));
	return FALSE;
  }
  
  wmd->dwPrivate = dwData;
  return TRUE;
}


ODINFUNCTION3(BOOL, mciSetYieldProc,
              MCIDEVICEID, mciId,
              YIELDPROC, fpYieldProc,
              DWORD, dwYieldData)
{
  LPWINE_MCIDRIVER	wmd;
  
  TRACE("WINMM:mciSetYieldProc (%u, %p, %08lx) - untested\n", mciId, fpYieldProc, dwYieldData);
  
  if (!(wmd = MCI_GetDriver(mciId))) {
	WARN("Bad uDeviceID\n");
	return FALSE;
  }
  
  wmd->lpfnYieldProc = fpYieldProc;
  wmd->dwYieldData   = dwYieldData;
  wmd->bIs32         = TRUE;
  
  return TRUE;
}


/**************************************************************************/
/*                       heap.c                                           */
/**************************************************************************/

#ifdef __GNUC__
#define GET_EIP()    (__builtin_return_address(0))
#define SET_EIP(ptr) ((ARENA_INUSE*)(ptr) - 1)->callerEIP = GET_EIP()
#else
#define GET_EIP()    0
#define SET_EIP(ptr) /* nothing */
#endif  /* __GNUC__ */


/**************************************************************************/
/*                     mmmsystem.c                                        */
/**************************************************************************/

static LPWINE_MM_IDATA		lpFirstIData = NULL;

static	LPWINE_MM_IDATA	MULTIMEDIA_GetIDataNoCheck(void)
{
    DWORD		pid = GetCurrentProcessId();
    LPWINE_MM_IDATA	iData;

    for (iData = lpFirstIData; iData; iData = iData->lpNextIData) {
      if (iData->dwThisProcess == pid)
	    break;
    }
    return iData;
}

/**************************************************************************
 * 			MULTIMEDIA_GetIData			[internal]
 */
LPWINE_MM_IDATA	MULTIMEDIA_GetIData(void)
{
    LPWINE_MM_IDATA	iData = MULTIMEDIA_GetIDataNoCheck();

    if (!iData) {
      dprintf(("MULTIMEDIA_GetIData: IData not found for pid=%08lx. Suicide !!!\n", GetCurrentProcessId()));
      ExitProcess(0);
    }
    return iData;
}


/**************************************************************************
 * 			MULTIMEDIA_CreateIData			[internal]
 */
BOOL MULTIMEDIA_CreateIData(HINSTANCE hInstDLL)
{
    LPWINE_MM_IDATA	iData;
	
    iData = (LPWINE_MM_IDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WINE_MM_IDATA));

    if (!iData)
      return FALSE;
    iData->hWinMM32Instance = hInstDLL;
    iData->dwThisProcess = GetCurrentProcessId();
    iData->lpNextIData = lpFirstIData;
    lpFirstIData = iData;
    InitializeCriticalSection(&iData->cs);
    dprintf(("Created IData (%p) for pid %08lx\n", iData, iData->dwThisProcess));
    return TRUE;
}


/**************************************************************************
 * 			MULTIMEDIA_DeleteIData			[internal]
 */
void MULTIMEDIA_DeleteIData(void)
{
    LPWINE_MM_IDATA	iData = MULTIMEDIA_GetIDataNoCheck();
    LPWINE_MM_IDATA*	ppid;
	    
    if (iData) {
	for (ppid = &lpFirstIData; *ppid; ppid = &(*ppid)->lpNextIData) {
	    if (*ppid == iData) {
		*ppid = iData->lpNextIData;
		break;
	    }
	}
	/* FIXME: should also free content and resources allocated 
	 * inside iData */
	HeapFree(GetProcessHeap(), 0, iData);
    }
}


/**************************************************************************/
/*                     mmmsystem.c                                        */
/**************************************************************************/


static	int			MCI_InstalledCount;
static	LPSTR			MCI_lpInstallNames = NULL;


/* First MCI valid device ID (0 means error) */
#define MCI_MAGIC 0x0001


/**************************************************************************
 * 				MCI_GetDriver			[internal]
 */
static LPWINE_MCIDRIVER	MCI_GetDriver(UINT16 wDevID) 
{
    LPWINE_MCIDRIVER	wmd = 0;
    LPWINE_MM_IDATA	iData = MULTIMEDIA_GetIData();

    EnterCriticalSection(&iData->cs);
    for (wmd = iData->lpMciDrvs; wmd; wmd = wmd->lpNext) {
	if (wmd->wDeviceID == wDevID)
	    break;
    }
    LeaveCriticalSection(&iData->cs);
    return wmd;
}

/**************************************************************************
 * 				MCI_GetDriverFromString		[internal]
 */
static UINT	MCI_GetDriverFromString(LPCSTR lpstrName)
{
    LPWINE_MCIDRIVER	wmd;
    LPWINE_MM_IDATA	iData = MULTIMEDIA_GetIData();
    UINT		ret = 0;

    if (!lpstrName)
      return 0;
    
    if (!lstrcmpiA(lpstrName, "ALL"))
      return MCI_ALL_DEVICE_ID;
    
    EnterCriticalSection(&iData->cs);
    for (wmd = iData->lpMciDrvs; wmd; wmd = wmd->lpNext) {
      if (wmd->lpstrElementName && strcmp(wmd->lpstrElementName, lpstrName) == 0) {
	    ret = wmd->wDeviceID;
	    break;
      }
      
      if (wmd->lpstrDeviceType && strcmp(wmd->lpstrDeviceType, lpstrName) == 0) {
	    ret = wmd->wDeviceID;
	    break;
      }
      
      if (wmd->lpstrAlias && strcmp(wmd->lpstrAlias, lpstrName) == 0) {
	    ret = wmd->wDeviceID;
	    break;
      }
    }
    LeaveCriticalSection(&iData->cs);
    
    return ret;
}


/**************************************************************************
 * 				MCI_GetDevTypeFromFileName	[internal]
 */
static	DWORD	MCI_GetDevTypeFromFileName(LPCSTR fileName, LPSTR buf, UINT len)
{
    LPSTR	tmp;

    if ((tmp = strrchr(fileName, '.'))) {
      GetProfileStringA("mci extensions", tmp + 1, "*", buf, len);
      if (strcmp(buf, "*") != 0) {
	    return 0;
      }
      dprintf(("No [mci extensions] entry for '%s' found. MCI_GetDevTypeFromFileName: line %d, file 'mci.cpp'\n", tmp, __LINE__));
    }
    return MCIERR_EXTENSION_NOT_FOUND;
}


#define	MAX_MCICMDTABLE			20
#define MCI_COMMAND_TABLE_NOT_LOADED	0xFFFE



/**************************************************************************
 * 				MCI_DefYieldProc	       	[internal]
 */
//UINT WINAPI MCI_DefYieldProc(MCIDEVICEID wDevID, DWORD data)

UINT16 WINAPI MCI_DefYieldProc(UINT16 wDevID, DWORD data)
{
  INT16	ret;

#if 0  
  dprintf(("(0x%04x, 0x%08lx)\n", wDevID, data));
#endif  
  if ((HIWORD(data) != 0 && GetActiveWindow() != HIWORD(data)) ||
      (GetAsyncKeyState(LOWORD(data)) & 1) == 0) {
	/* WINE stuff removed: UserYield16();*/
	ret = 0;
  } else {
	MSG		msg;
    
	msg.hwnd = HIWORD(data);
	while (!PeekMessageA(&msg, HIWORD(data), WM_KEYFIRST, WM_KEYLAST, PM_REMOVE));
	ret = -1;
  }
  return ret;
}


/**************************************************************************
 * 				MCI_UnLoadMciDriver		[internal]
 */
static	BOOL	MCI_UnLoadMciDriver(LPWINE_MM_IDATA iData, LPWINE_MCIDRIVER wmd)
{
    LPWINE_MCIDRIVER*		tmp;

#if 0
    dprintf(("Entering MCI_UnLoadMciDriver...\n"));
#endif
    if (!wmd)
      return TRUE;

    if (wmd->hDrv) 
      CloseDriver(wmd->hDrv, 0, 0);

    if (wmd->dwPrivate != 0)
      dprintf(("Unloading mci driver with non nul dwPrivate field\n"));

    EnterCriticalSection(&iData->cs);
    for (tmp = &iData->lpMciDrvs; *tmp; tmp = &(*tmp)->lpNext) {
      if (*tmp == wmd) {
	    *tmp = wmd->lpNext;
	    break;
      }
    }
    LeaveCriticalSection(&iData->cs);

    HeapFree(GetProcessHeap(), 0, wmd->lpstrDeviceType);
    HeapFree(GetProcessHeap(), 0, wmd->lpstrAlias);
    HeapFree(GetProcessHeap(), 0, wmd->lpstrElementName);

    HeapFree(GetProcessHeap(), 0, wmd);
    //dprintf(("Leaving MCI_UnLoadMciDriver...\n"));
    return TRUE;
}

/**************************************************************************
 * 				MCI_LoadMciDriver		[internal]
 */
static	DWORD	MCI_LoadMciDriver(LPWINE_MM_IDATA iData, LPCSTR _strDevTyp, 
				  LPWINE_MCIDRIVER* lpwmd)
{
    LPSTR			strDevTyp = CharUpperA(HEAP_strdupA(GetProcessHeap(), 0, _strDevTyp));
    LPWINE_MCIDRIVER		wmd = (LPWINE_MCIDRIVER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*wmd));
    MCI_OPEN_DRIVER_PARMSA	modp;
    DWORD			dwRet = 0;
    HDRVR			hDrv = 0;


    dprintf(("Entering MCI_LoadMciDriver...\n"));
 
    if (!wmd || !strDevTyp) {
      dwRet = MCIERR_OUT_OF_MEMORY;
      goto errCleanUp;
    }

    wmd->lpstrDeviceType = strDevTyp;
    wmd->lpfnYieldProc = MCI_DefYieldProc;
    wmd->dwYieldData = VK_CANCEL;
    wmd->hCreatorTask = NULL;


    EnterCriticalSection(&iData->cs);
    /* wmd must be inserted in list before sending opening the driver, coz' it
     * may want to lookup at wDevID
     */
    wmd->lpNext = iData->lpMciDrvs;
    iData->lpMciDrvs = wmd;

    for (modp.wDeviceID = MCI_MAGIC; 
         MCI_GetDriver(modp.wDeviceID) != 0; 
         modp.wDeviceID++);

    wmd->wDeviceID = modp.wDeviceID;

    LeaveCriticalSection(&iData->cs);

    dprintf(("wDevID=%04X strDevTyp: %s\n", modp.wDeviceID, strDevTyp));

    modp.lpstrParams = NULL;

    hDrv = OpenDriverA(strDevTyp, "mci", (LPARAM)&modp);
    
    if (!hDrv) {
      dprintf(("Couldn't load driver for type %s.\n"
               "If you don't have a windows installation accessible from Wine,\n"
               "you perhaps forgot to create a [mci] section in system.ini\n",
               strDevTyp));
      dwRet = MCIERR_DEVICE_NOT_INSTALLED;
      goto errCleanUp;
    }				

    /* FIXME: should also check that module's description is of the form
     * MODULENAME:[MCI] comment
     */

    wmd->hDrv = hDrv;
    /* some drivers will return 0x0000FFFF, some others 0xFFFFFFFF */
    wmd->uSpecificCmdTable = LOWORD(modp.wCustomCommandTable);
    wmd->uTypeCmdTable = MCI_COMMAND_TABLE_NOT_LOADED;

    dprintf(("Loaded driver %x (%s), type is %d, cmdTable=%08x\n", 
	  hDrv, strDevTyp, modp.wType, modp.wCustomCommandTable));
    
    
    wmd->wType = modp.wType;

#if 0    
    dprintf(("mcidev=%d, uDevTyp=%04X wDeviceID=%04X !\n", 
             modp.wDeviceID, modp.wType, modp.wDeviceID));
#endif

    *lpwmd = wmd;
#if 0
    dprintf(("Leaving MCI_LoadMciDriver succesful...\n"));
#endif
    return 0;
errCleanUp:
    MCI_UnLoadMciDriver(iData, wmd);
    //  HeapFree(GetProcessHeap(), 0, strDevTyp);<-- done in MCI_UnloadMciDriver
    *lpwmd = 0;
    dprintf(("Leaving MCI_LoadMciDriver on error...\n"));
    return dwRet;
}


/**************************************************************************
 * 			MCI_SendCommandFrom32			[internal]
 */
static DWORD MCI_SendCommandFrom32(UINT wDevID, UINT16 wMsg, DWORD dwParam1, DWORD dwParam2)
{
    DWORD		dwRet = MCIERR_DEVICE_NOT_INSTALLED;
    LPWINE_MCIDRIVER	wmd = MCI_GetDriver(wDevID);

    if (!wmd) {
      dwRet = MCIERR_INVALID_DEVICE_ID;
    } else {
      switch (GetDriverFlags(wmd->hDrv) & (WINE_GDF_EXIST)) {
      case WINE_GDF_EXIST:
	    dwRet = SendDriverMessage(wmd->hDrv, wMsg, dwParam1, dwParam2);
	    break;
      default:
	    dprintf(("Unknown driver %u\n", wmd->hDrv));
	    dwRet = MCIERR_DRIVER_INTERNAL;
      }
    }
    return dwRet;
}
    
/**************************************************************************
 * 			MCI_FinishOpen				[internal]
 */
static	DWORD	MCI_FinishOpen(LPWINE_MCIDRIVER wmd, LPMCI_OPEN_PARMSA lpParms, 
                               DWORD dwParam)
{
  if (dwParam & MCI_OPEN_ELEMENT)
	wmd->lpstrElementName = HEAP_strdupA(GetProcessHeap(), 0, 
                                         lpParms->lpstrElementName);
  
  if (dwParam & MCI_OPEN_ALIAS)
	wmd->lpstrAlias = HEAP_strdupA(GetProcessHeap(), 0, lpParms->lpstrAlias);
  
  lpParms->wDeviceID = wmd->wDeviceID;
  
  return MCI_SendCommandFrom32(wmd->wDeviceID, MCI_OPEN_DRIVER, dwParam, 
                               (DWORD)lpParms);
}

/**************************************************************************
 * 			MCI_Open				[internal]
 */
static	DWORD MCI_Open(DWORD dwParam, LPMCI_OPEN_PARMSA lpParms)
{
    char			strDevTyp[128];
    DWORD 			dwRet; 
    LPWINE_MCIDRIVER		wmd = NULL;

    LPWINE_MM_IDATA		iData = MULTIMEDIA_GetIData();

    dprintf(("Entering MCI_OPEN...\n"));

    if (lpParms == NULL) return MCIERR_NULL_PARAMETER_BLOCK;

    /* only two low bytes are generic, the other ones are dev type specific */
#define WINE_MCIDRIVER_SUPP	(0xFFFF0000|MCI_OPEN_SHAREABLE|MCI_OPEN_ELEMENT| \
                         MCI_OPEN_ALIAS|MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID| \
                         MCI_NOTIFY|MCI_WAIT)
    if ((dwParam & ~WINE_MCIDRIVER_SUPP) != 0) {
	dprintf(("Unsupported yet dwFlags=%08lX\n", dwParam & ~WINE_MCIDRIVER_SUPP));
    }
#undef WINE_MCIDRIVER_SUPP

    strDevTyp[0] = 0;

    if (dwParam & MCI_OPEN_TYPE) {
      if (dwParam & MCI_OPEN_TYPE_ID) {
	    WORD uDevType = LOWORD((DWORD)lpParms->lpstrDeviceType);
        if (uDevType < MCI_DEVTYPE_FIRST ||
            uDevType > MCI_DEVTYPE_LAST ||
            !LoadStringA(iData->hWinMM32Instance, uDevType, strDevTyp, sizeof(strDevTyp))) /* This gets a name for the device e.g 'cdaudio' */
          {
            dwRet = MCIERR_BAD_INTEGER;
            goto errCleanUp;
          }
      } else {
	    LPSTR	ptr;
	    if (lpParms->lpstrDeviceType == NULL) {
          dwRet = MCIERR_NULL_PARAMETER_BLOCK;
		goto errCleanUp;
	    }
	    strcpy(strDevTyp, lpParms->lpstrDeviceType);
	    ptr = strchr(strDevTyp, '!');
	    if (ptr) {
		/* this behavior is not documented in windows. However, since, in
		 * some occasions, MCI_OPEN handling is translated by WinMM into
		 * a call to mciSendString("open <type>"); this code shall be correct
		 */
		if (dwParam & MCI_OPEN_ELEMENT) {
		    dprintf(("Both MCI_OPEN_ELEMENT(%s) and %s are used\n",
			lpParms->lpstrElementName, strDevTyp));
		    dwRet = MCIERR_UNRECOGNIZED_KEYWORD;
		    goto errCleanUp;
		}
		dwParam |= MCI_OPEN_ELEMENT;
		*ptr++ = 0;
		/* FIXME: not a good idea to write in user supplied buffer */
		lpParms->lpstrElementName = ptr;
	    }
		
      }
      dprintf(("MCI_OPEN (MCI_OPEN_TYPE): devType='%s' !\n", strDevTyp));
    }
   
    if (dwParam & MCI_OPEN_ELEMENT) {
      dprintf(("lpstrElementName='%s'\n", lpParms->lpstrElementName));

      if (dwParam & MCI_OPEN_ELEMENT_ID) {
	    dprintf(("Unsupported yet flag MCI_OPEN_ELEMENT_ID\n"));
	    dwRet = MCIERR_UNRECOGNIZED_KEYWORD;
	    goto errCleanUp;
      }

      if (!lpParms->lpstrElementName) {
	    dwRet = MCIERR_NULL_PARAMETER_BLOCK;
	    goto errCleanUp;
      }

#if 0
      /* Only working on my machine!! CW */
      if(lpParms->lpstrElementName[0]=='N') {
	    dprintf(("Discarding drive N:\n"));
        dwRet = MCIERR_UNRECOGNIZED_KEYWORD;
	    goto errCleanUp;
      }
#endif
      
      /* type, if given as a parameter, supersedes file extension */
      if (!strDevTyp[0] && 
          MCI_GetDevTypeFromFileName(lpParms->lpstrElementName, 
                                     strDevTyp, sizeof(strDevTyp))) {
        if (GetDriveTypeA(lpParms->lpstrElementName) != DRIVE_CDROM) {
          dwRet = MCIERR_EXTENSION_NOT_FOUND;
          goto errCleanUp;
        }
        /* FIXME: this will not work if several CDROM drives are installed on the machine */
        strcpy(strDevTyp, "CDAUDIO");
      }
    }
    
    if (strDevTyp[0] == 0) {
      dprintf(("Couldn't load driver (MCI_Open line %d)\n",__LINE__));
      dwRet = MCIERR_INVALID_DEVICE_NAME;
      goto errCleanUp;
    }

    if (dwParam & MCI_OPEN_ALIAS) {
      dprintf(("MCI_OPEN_ALIAS requested\n"));
      if (!lpParms->lpstrAlias) {
	    dwRet = MCIERR_NULL_PARAMETER_BLOCK;
	    goto errCleanUp;
      }
      dprintf(("Alias='%s' !\n", lpParms->lpstrAlias));
    }
   
    if ((dwRet = MCI_LoadMciDriver(iData, strDevTyp, &wmd))) {
      goto errCleanUp;
    }


    if ((dwRet = MCI_FinishOpen(wmd, lpParms, dwParam))) {
      dprintf(("Failed to open driver (MCI_OPEN_DRIVER) [%08lx], closing\n", dwRet));
      /* FIXME: is dwRet the correct ret code ? */
      goto errCleanUp;
    }


    /* only handled devices fall through */
    dprintf(("wDevID=%04X wDeviceID=%d dwRet=%ld\n", wmd->wDeviceID, lpParms->wDeviceID, dwRet));

    if (dwParam & MCI_NOTIFY)
      // mciDriverNotify16(lpParms->dwCallback, wmd->wDeviceID, MCI_NOTIFY_SUCCESSFUL);
      dprintf(("FIXME: MCI_NOTIFY not implemented yet! MCI_Open (line %d)\n",__LINE__));

    return 0;

errCleanUp:

    if (wmd) MCI_UnLoadMciDriver(iData, wmd);

    if (dwParam & MCI_NOTIFY)
      //  mciDriverNotify16(lpParms->dwCallback, 0, MCI_NOTIFY_FAILURE);
      dprintf(("FIXME: MCI_NOTIFY not implemented yet! MCI_Open (line %d)\n",__LINE__));
 
    dprintf(("Leaving MCI_Open on error...\n"));
    return dwRet;
}


/**************************************************************************
 * 			MCI_Close				[internal]
 */
static	DWORD MCI_Close(UINT16 wDevID, DWORD dwParam, LPMCI_GENERIC_PARMS lpParms)
{
  DWORD		dwRet;
  LPWINE_MCIDRIVER	wmd;
  LPWINE_MM_IDATA	iData = MULTIMEDIA_GetIData();
  
  //dprintf(("(%04x, %08lX, %p)\n", wDevID, dwParam, lpParms));

  if (wDevID == MCI_ALL_DEVICE_ID) {
	LPWINE_MCIDRIVER	next;

	EnterCriticalSection(&iData->cs);
	/* FIXME: shall I notify once after all is done, or for 
	 * each of the open drivers ? if the latest, which notif
	 * to return when only one fails ?
	 */
	for (wmd = iData->lpMciDrvs; wmd; ) {
      next = wmd->lpNext;
      MCI_Close(wmd->wDeviceID, dwParam, lpParms);
      wmd = next;
	}	
	LeaveCriticalSection(&iData->cs);
	return 0;
  }

  if (!(wmd = MCI_GetDriver(wDevID))) {
	return MCIERR_INVALID_DEVICE_ID;
  }

  dwRet = MCI_SendCommandFrom32(wDevID, MCI_CLOSE_DRIVER, dwParam, (DWORD)lpParms);
  
  MCI_UnLoadMciDriver(iData, wmd);
  
  if (dwParam & MCI_NOTIFY)
    dprintf(("FIXME: MCI_NOTIFY not implemented yet! MCI_Close (line %d)\n",__LINE__));
    //	mciDriverNotify16(lpParms->dwCallback, wDevID,
    //                (dwRet == 0) ? MCI_NOTIFY_SUCCESSFUL : MCI_NOTIFY_FAILURE);
  
  return dwRet;
}


/**************************************************************************
 * 			MCI_WriteString				[internal]
 */
DWORD	MCI_WriteString(LPSTR lpDstStr, DWORD dstSize, LPCSTR lpSrcStr)
{
    DWORD	ret = 0;

    if (lpSrcStr) {
	if (dstSize <= strlen(lpSrcStr)) {
	    lstrcpynA(lpDstStr, lpSrcStr, dstSize - 1);
	    ret = MCIERR_PARAM_OVERFLOW;
	} else {
	    strcpy(lpDstStr, lpSrcStr);
	}	
    } else {
	*lpDstStr = 0;
    }
    return ret;
}


/**************************************************************************
 * 			MCI_Sysinfo				[internal]
 */
static	DWORD MCI_SysInfo(UINT uDevID, DWORD dwFlags, LPMCI_SYSINFO_PARMSA lpParms)
{
    DWORD		ret = MCIERR_INVALID_DEVICE_ID;
    LPWINE_MCIDRIVER	wmd;
    LPWINE_MM_IDATA	iData = MULTIMEDIA_GetIData();

    if (lpParms == NULL)			return MCIERR_NULL_PARAMETER_BLOCK;

    TRACE("(%08x, %08lX, %08lX[num=%ld, wDevTyp=%u])\n", 
	  uDevID, dwFlags, (DWORD)lpParms, lpParms->dwNumber, lpParms->wDeviceType);
    
    switch (dwFlags & ~MCI_SYSINFO_OPEN) {
    case MCI_SYSINFO_QUANTITY:
      {
	    DWORD	cnt = 0;
	    
	    if (lpParms->wDeviceType < MCI_DEVTYPE_FIRST || 
            lpParms->wDeviceType > MCI_DEVTYPE_LAST) {
          if (dwFlags & MCI_SYSINFO_OPEN) {
		    TRACE("MCI_SYSINFO_QUANTITY: # of open MCI drivers\n");
		    EnterCriticalSection(&iData->cs);
		    for (wmd = iData->lpMciDrvs; wmd; wmd = wmd->lpNext) {
              cnt++;
		    }
		    LeaveCriticalSection(&iData->cs);
          } else {
		    TRACE("MCI_SYSINFO_QUANTITY: # of installed MCI drivers\n");
		    cnt = MCI_InstalledCount;
          }
	    } else {
          if (dwFlags & MCI_SYSINFO_OPEN) {
		    TRACE("MCI_SYSINFO_QUANTITY: # of open MCI drivers of type %u\n", 
                  lpParms->wDeviceType);
		    EnterCriticalSection(&iData->cs);
		    for (wmd = iData->lpMciDrvs; wmd; wmd = wmd->lpNext) {
              if (wmd->wType == lpParms->wDeviceType)
			    cnt++;
		    }
		    LeaveCriticalSection(&iData->cs);
          } else {
		    TRACE("MCI_SYSINFO_QUANTITY: # of installed MCI drivers of type %u\n", 
                  lpParms->wDeviceType);
		    FIXME("Don't know how to get # of MCI devices of a given type\n");
		    cnt = 1;
          }
	    }
	    *(DWORD*)lpParms->lpstrReturn = cnt;
      }
      TRACE("(%ld) => '%ld'\n", lpParms->dwNumber, *(DWORD*)lpParms->lpstrReturn);
      ret = MCI_INTEGER_RETURNED;
      break;
    case MCI_SYSINFO_INSTALLNAME:
      TRACE("MCI_SYSINFO_INSTALLNAME \n");
      if ((wmd = MCI_GetDriver(uDevID))) {
	    ret = MCI_WriteString(lpParms->lpstrReturn, lpParms->dwRetSize, 
                              wmd->lpstrDeviceType);
      } else {
	    *lpParms->lpstrReturn = 0;
	    ret = MCIERR_INVALID_DEVICE_ID;
      }
      TRACE("(%ld) => '%s'\n", lpParms->dwNumber, lpParms->lpstrReturn);
      break;
    case MCI_SYSINFO_NAME:
      TRACE("MCI_SYSINFO_NAME\n");
      if (dwFlags & MCI_SYSINFO_OPEN) {
	    FIXME("Don't handle MCI_SYSINFO_NAME|MCI_SYSINFO_OPEN (yet)\n");
	    ret = MCIERR_UNRECOGNIZED_COMMAND;
      } else if (lpParms->dwNumber > MCI_InstalledCount) {
	    ret = MCIERR_OUTOFRANGE;
      } else {
	    DWORD	count = lpParms->dwNumber;
	    LPSTR	ptr = MCI_lpInstallNames;
        
	    while (--count > 0) ptr += strlen(ptr) + 1;
	    ret = MCI_WriteString(lpParms->lpstrReturn, lpParms->dwRetSize, ptr);
      }
      TRACE("(%ld) => '%s'\n", lpParms->dwNumber, lpParms->lpstrReturn);
      break;
    default:
      TRACE("Unsupported flag value=%08lx\n", dwFlags);
      ret = MCIERR_UNRECOGNIZED_COMMAND;
    }
    return ret;
}


/**************************************************************************
 * 			MCI_Break				[internal]
 */
static	DWORD MCI_Break(UINT wDevID, DWORD dwFlags, LPMCI_BREAK_PARMS lpParms)
{
    DWORD	dwRet = 0;
    
    if (lpParms == NULL)	return MCIERR_NULL_PARAMETER_BLOCK;

    if (dwFlags & MCI_NOTIFY)
      dprintf(("FIXME: MCI_NOTIFY not implemented yet! MCI_Break (line %d)\n",__LINE__));
    //	mciDriverNotify16(lpParms->dwCallback, wDevID,
    //	  (dwRet == 0) ? MCI_NOTIFY_SUCCESSFUL : MCI_NOTIFY_FAILURE);

    return dwRet;
}

    
/**************************************************************************
 * 			MCI_SendCommand				[internal]
 */
static DWORD	MCI_SendCommand(UINT wDevID, UINT16 wMsg, DWORD dwParam1, 
			DWORD dwParam2)
{
    DWORD		dwRet = MCIERR_UNRECOGNIZED_COMMAND;

    switch (wMsg) {
    case MCI_OPEN:
      dwRet = MCI_Open(dwParam1, (LPMCI_OPEN_PARMSA)dwParam2);
      break;
    case MCI_CLOSE:
      dwRet = MCI_Close(wDevID, dwParam1, (LPMCI_GENERIC_PARMS)dwParam2);
      break;
    case MCI_SYSINFO:
      dwRet = MCI_SysInfo(wDevID, dwParam1, (LPMCI_SYSINFO_PARMSA)dwParam2);
      break;
    case MCI_BREAK:
      dwRet = MCI_Break(wDevID, dwParam1, (LPMCI_BREAK_PARMS)dwParam2);
      break;
      // case MCI_SOUND:
      /* FIXME: it seems that MCI_SOUND needs the same handling as MCI_BREAK
       * but I couldn't get any doc on this MCI message
       */
      // break;
    default:
      if (wDevID == MCI_ALL_DEVICE_ID) {
	    dprintf(("MCI_SendCommand: unhandled MCI_ALL_DEVICE_ID\n"));
	    dwRet = MCIERR_CANNOT_USE_ALL;
      } else {
        dwRet=MCI_SendCommandFrom32(wDevID, wMsg, dwParam1, dwParam2);
      }	    
      break;
    }
    return dwRet;
}




/**************************************************************************
 * 			MULTIMEDIA_MciInit			[internal]
 *
 * Initializes the MCI internal variables.
 *
 */
BOOL MULTIMEDIA_MciInit(void)
{
    LPSTR	ptr1, ptr2;
    HKEY	hWineConf;
    HKEY	hkey;
    DWORD	err;
    DWORD 	type;
    DWORD 	count = 2048;

    MCI_InstalledCount = 0;
    ptr1 = MCI_lpInstallNames = (char*) HeapAlloc(GetProcessHeap(), 0, count);

    if (!MCI_lpInstallNames)
      return FALSE;

#if 0
    /* FIXME: should do also some registry diving here ? */
    if (!(err = RegOpenKeyA(HKEY_LOCAL_MACHINE, "Software\\Wine\\Wine\\Config", &hWineConf)) &&
        !(err = RegOpenKeyA(hWineConf, "options", &hkey))) {
      err = RegQueryValueExA(hkey, "mci", 0, &type, MCI_lpInstallNames, &count);
      RegCloseKey(hkey);
      FIXME("Registry handling for mci drivers not changed for odin yet. Verbatim copy from WINE (line %d)",__LINE__);
    }
#endif
    FIXME("No Registry querying for mci drivers yet! (line %d)",__LINE__);
    err=1;
    if (!err) {
      TRACE("Wine => '%s' \n", ptr1);
      while ((ptr2 = strchr(ptr1, ':')) != 0) {
	    *ptr2++ = 0;
	    TRACE("---> '%s' \n", ptr1);
	    MCI_InstalledCount++;
	    ptr1 = ptr2;
      }
      MCI_InstalledCount++;
      TRACE("---> '%s' \n", ptr1);
      ptr1 += strlen(ptr1) + 1;
    } else {
      GetPrivateProfileStringA("mci", NULL, "", MCI_lpInstallNames, count, "SYSTEM.INI");
      while (strlen(ptr1) > 0) {
	    TRACE("---> '%s' \n", ptr1);
	    ptr1 += strlen(ptr1) + 1;
	    MCI_InstalledCount++;
      }
    }
    //RegCloseKey(hWineConf);
    return TRUE;
}
