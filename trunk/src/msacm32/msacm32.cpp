/* $Id: msacm32.cpp,v 1.2 1999-09-23 09:38:04 sandervl Exp $ */
/*
 *      MSACM32 library
 *
 *      Copyright 1998  Patrik Stridvall
 *	Copyright 1999	Jens Wiessner
 */

#include <winbase.h>
#include <winerror.h>
#include <windef.h>
#include <winuser.h>
#include <misc.h>
#include <odinwrap.h>
#include "msacm.h"
#include "msacmdrv.h"
#include "winversion.h"
#include "winreg.h"
#include "debugtools.h"
#include <debugdefs.h>


DEFAULT_DEBUG_CHANNEL(msacm)

/**********************************************************************/

static DWORD MSACM_dwProcessesAttached = 0;

/***********************************************************************
 *           MSACM_LibMain32 (MSACM32.init) 
 */
BOOL WINAPI MSACM32_LibMain(
  HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
      if(MSACM_dwProcessesAttached == 0)
	{
	  MSACM_hHeap = HeapCreate(0, 0x10000, 0);
	  MSACM_RegisterAllDrivers();
	}
      MSACM_dwProcessesAttached++;
      break;
    case DLL_PROCESS_DETACH:
      MSACM_dwProcessesAttached--;
      if(MSACM_dwProcessesAttached == 0)
	{
	  MSACM_UnregisterAllDrivers();
	  HeapDestroy(MSACM_hHeap);
	  MSACM_hHeap = (HANDLE) NULL;
	}
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    default:
      break;
    }
  return TRUE;
}

/***********************************************************************
 *           XRegThunkEntry (MSACM32.1)
 * FIXME
 *   No documentation found.
 */

/***********************************************************************
 *           acmDriverAddA (MSACM32.2)
 */
MMRESULT WINAPI acmDriverAddA(
  PHACMDRIVERID phadid, HINSTANCE hinstModule,
  LPARAM lParam, DWORD dwPriority, DWORD fdwAdd)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverAddA\n"));
#endif
  PWINE_ACMLOCALDRIVER pld;
  if(!phadid)
    return MMSYSERR_INVALPARAM;

  /* Check if any unknown flags */
  if(fdwAdd & 
     ~(ACM_DRIVERADDF_FUNCTION|ACM_DRIVERADDF_NOTIFYHWND|
       ACM_DRIVERADDF_GLOBAL))
    return MMSYSERR_INVALFLAG;

  /* Check if any incompatible flags */
  if((fdwAdd & ACM_DRIVERADDF_FUNCTION) && 
     (fdwAdd & ACM_DRIVERADDF_NOTIFYHWND))
    return MMSYSERR_INVALFLAG;

  pld = (PWINE_ACMLOCALDRIVER) HeapAlloc(MSACM_hHeap, 0, sizeof(WINE_ACMLOCALDRIVER));
  pld->pfnDriverProc = (DRIVERPROC) 
    GetProcAddress(hinstModule, "DriverProc");
  *phadid = (HACMDRIVERID) MSACM_RegisterDriver(NULL, NULL, pld);

  /* FIXME: lParam, dwPriority and fdwAdd ignored */

  return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverAddW (MSACM32.3)
 * FIXME
 *   Not implemented
 */
MMRESULT WINAPI acmDriverAddW(
  PHACMDRIVERID phadid, HINSTANCE hinstModule,
  LPARAM lParam, DWORD dwPriority, DWORD fdwAdd)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverAddW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmDriverClose (MSACM32.4)
 */
MMRESULT WINAPI acmDriverClose(
  HACMDRIVER had, DWORD fdwClose)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverClose\n"));
#endif
  PWINE_ACMDRIVER p;

  if(fdwClose)
    return MMSYSERR_INVALFLAG;

  p = MSACM_GetDriver(had);
  if(!p)
    return MMSYSERR_INVALHANDLE;

  p->obj.pACMDriverID->pACMDriver = NULL;

  if(p->hDrvr)
    CloseDriver(p->hDrvr, 0, 0);

  HeapFree(MSACM_hHeap, 0, p);

  return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverDetailsA (MSACM32.5)
 */
MMRESULT WINAPI acmDriverDetailsA(
  HACMDRIVERID hadid, PACMDRIVERDETAILSA padd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverDetailsA\n"));
#endif
  PWINE_ACMDRIVERID p;
  MMRESULT mmr;
  BOOL bOpenTemporary;

  p = MSACM_GetDriverID(hadid);
  if(!p)
    return MMSYSERR_INVALHANDLE;
  
  if(fdwDetails)
    return MMSYSERR_INVALFLAG;

  bOpenTemporary = !p->pACMDriver;
  if(bOpenTemporary) {
    bOpenTemporary = TRUE;
    acmDriverOpen((PHACMDRIVER) &p->pACMDriver, hadid, 0);
  }
  
  /* FIXME
   *   How does the driver know if the ANSI or 
   *   the UNICODE variant of PACMDRIVERDETAILS is used?
   *   It might check cbStruct or does it only accept ANSI.
   */
  mmr = (MMRESULT) acmDriverMessage(
    (HACMDRIVER) p->pACMDriver, ACMDM_DRIVER_DETAILS, 
    (LPARAM) padd,  0
  );

  if(bOpenTemporary) {
    acmDriverClose((HACMDRIVER) p->pACMDriver, 0);
    p->pACMDriver = NULL;
  }

  return mmr;
}

/***********************************************************************
 *           acmDriverDetailsW (MSACM32.6)
 * FIXME
 *   Not implemented
 */
MMRESULT WINAPI acmDriverDetailsW(
  HACMDRIVERID hadid, PACMDRIVERDETAILSW padd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverDetailsW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmDriverEnum (MSACM32.7)
 */
MMRESULT WINAPI acmDriverEnum(
  ACMDRIVERENUMCB fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverEnum\n"));
#endif
  PWINE_ACMDRIVERID p;

  if(!fnCallback)
    {
      return MMSYSERR_INVALPARAM;
    }

  if(fdwEnum && ~(ACM_DRIVERENUMF_NOLOCAL|ACM_DRIVERENUMF_DISABLED))
    {
      return MMSYSERR_INVALFLAG;
    }

  p = MSACM_pFirstACMDriverID;
  while(p)
    {
      (*fnCallback)((HACMDRIVERID) p, dwInstance, ACMDRIVERDETAILS_SUPPORTF_CODEC);
      p = p->pNextACMDriverID;
    }

  return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverID (MSACM32.8)
 */
MMRESULT WINAPI acmDriverID(
  HACMOBJ hao, PHACMDRIVERID phadid, DWORD fdwDriverID)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverID\n"));
#endif
  PWINE_ACMOBJ pao;

  pao = MSACM_GetObj(hao);
  if(!pao)
    return MMSYSERR_INVALHANDLE;

  if(!phadid)
    return MMSYSERR_INVALPARAM;

  if(fdwDriverID)
    return MMSYSERR_INVALFLAG;

  *phadid = (HACMDRIVERID) pao->pACMDriverID;

  return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverMessage (MSACM32.9)
 * FIXME
 *   Not implemented
 */
LRESULT WINAPI acmDriverMessage(
  HACMDRIVER had, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverMessage not implemented\n"));
#endif
  PWINE_ACMDRIVER pad = MSACM_GetDriver(had);
  if(!pad)
    return MMSYSERR_INVALPARAM;

  /* FIXME: Check if uMsg legal */

  if(!SendDriverMessage(pad->hDrvr, uMsg, lParam1, lParam2))
    return MMSYSERR_NOTSUPPORTED;

  return MMSYSERR_NOERROR;
}


/***********************************************************************
 *           acmDriverOpen (MSACM32.10)
 */
MMRESULT WINAPI acmDriverOpen(
  PHACMDRIVER phad, HACMDRIVERID hadid, DWORD fdwOpen)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverOpen\n"));
#endif
  PWINE_ACMDRIVERID padid;

  if(!phad)
    return MMSYSERR_INVALPARAM;

  padid = MSACM_GetDriverID(hadid); 
  if(!padid)
    return MMSYSERR_INVALHANDLE;

  if(fdwOpen)
    return MMSYSERR_INVALFLAG;

  if(padid->pACMDriver)
    {
      /* FIXME: Is it allowed? */
      return MMSYSERR_ERROR;
    }

  padid->pACMDriver = (PWINE_ACMDRIVER) HeapAlloc(
    MSACM_hHeap, 0, sizeof(WINE_ACMDRIVER)
  );
  padid->pACMDriver->obj.pACMDriverID = padid;
  
  if(!padid->pACMLocalDriver)
      padid->pACMDriver->hDrvr =
        OpenDriverA(padid->pszDriverAlias, "drivers32", 0);
  else
    {
      padid->pACMDriver->hDrvr = MSACM_OpenDriverProc(
	padid->pACMLocalDriver->pfnDriverProc
      );
    }

  if(!padid->pACMDriver->hDrvr)
    return MMSYSERR_ERROR;
 
  /* FIXME: Create a WINE_ACMDRIVER32 */
  *phad = (HACMDRIVER) NULL;

  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmDriverPriority (MSACM32.11)
 */
MMRESULT WINAPI acmDriverPriority(
  HACMDRIVERID hadid, DWORD dwPriority, DWORD fdwPriority)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverPriority\n"));
#endif
  PWINE_ACMDRIVERID padid;
  CHAR szSubKey[17];
  CHAR szBuffer[256];
  LONG lBufferLength = sizeof(szBuffer);
  LONG lError;
  HKEY hPriorityKey;
  DWORD dwPriorityCounter;

  padid = MSACM_GetDriverID(hadid);
  if(!padid)
    return MMSYSERR_INVALHANDLE;

  /* Check for unknown flags */
  if(fdwPriority & 
     ~(ACM_DRIVERPRIORITYF_ENABLE|ACM_DRIVERPRIORITYF_DISABLE|
       ACM_DRIVERPRIORITYF_BEGIN|ACM_DRIVERPRIORITYF_END))
    return MMSYSERR_INVALFLAG;

  /* Check for incompatible flags */
  if((fdwPriority & ACM_DRIVERPRIORITYF_ENABLE) &&
    (fdwPriority & ACM_DRIVERPRIORITYF_DISABLE))
    return MMSYSERR_INVALFLAG;

  /* Check for incompatible flags */
  if((fdwPriority & ACM_DRIVERPRIORITYF_BEGIN) &&
    (fdwPriority & ACM_DRIVERPRIORITYF_END))
    return MMSYSERR_INVALFLAG;

  lError = RegOpenKeyA(HKEY_CURRENT_USER, 
    "Software\\Microsoft\\Multimedia\\"
      "Audio Compression Manager\\Priority v4.00",
    &hPriorityKey
  );
  /* FIXME: Create key */
  if(lError != ERROR_SUCCESS)
    return MMSYSERR_ERROR;

  for(dwPriorityCounter = 1; ; dwPriorityCounter++)
  {
//
// FIXME ODIN   wsnprintfA(szSubKey, 17, "Priorty%ld", dwPriorityCounter);
//
    lError = RegQueryValueA(hPriorityKey, szSubKey, szBuffer, &lBufferLength);
    if(lError != ERROR_SUCCESS)
      break;

    break;
  }

  RegCloseKey(hPriorityKey);

  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmDriverRemove (MSACM32.12)
 */
MMRESULT WINAPI acmDriverRemove(
  HACMDRIVERID hadid, DWORD fdwRemove)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverRemove\n"));
#endif
  PWINE_ACMDRIVERID padid;

  padid = MSACM_GetDriverID(hadid);
  if(!padid)
    return MMSYSERR_INVALHANDLE;

  if(fdwRemove)
    return MMSYSERR_INVALFLAG;

  MSACM_UnregisterDriver(padid);

  return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmFilterChooseA (MSACM32.13)
 */
MMRESULT WINAPI acmFilterChooseA(
  PACMFILTERCHOOSEA pafltrc)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverChooseA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterChooseW (MSACM32.14)
 */
MMRESULT WINAPI acmFilterChooseW(
  PACMFILTERCHOOSEW pafltrc)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmDriverChooseW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterDetailsA (MSACM32.15)
 */
MMRESULT WINAPI acmFilterDetailsA(
  HACMDRIVER had, PACMFILTERDETAILSA pafd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFilterDetailsA\n"));
#endif
  if(fdwDetails & ~(ACM_FILTERDETAILSF_FILTER))
    return MMSYSERR_INVALFLAG;

  /* FIXME
   *   How does the driver know if the ANSI or
   *   the UNICODE variant of PACMFILTERDETAILS is used?
   *   It might check cbStruct or does it only accept ANSI.
   */
  return (MMRESULT) acmDriverMessage(
    had, ACMDM_FILTER_DETAILS,
    (LPARAM) pafd,  (LPARAM) fdwDetails
  );
}

/***********************************************************************
 *           acmFilterDetailsW (MSACM32.16)
 */
MMRESULT WINAPI acmFilterDetailsW(
  HACMDRIVER had, PACMFILTERDETAILSW pafd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFilterDetailsW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterEnumA (MSACM32.17)
 */
MMRESULT WINAPI acmFilterEnumA(
  HACMDRIVER had, PACMFILTERDETAILSA pafd, 
  ACMFILTERENUMCBA fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFilterEnumA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterEnumW (MSACM32.18)
 */
MMRESULT WINAPI acmFilterEnumW(
  HACMDRIVER had, PACMFILTERDETAILSW pafd, 
  ACMFILTERENUMCBW fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFilterEnumW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterTagDetailsA (MSACM32.19)
 */
MMRESULT WINAPI acmFilterTagDetailsA(
  HACMDRIVER had, PACMFILTERTAGDETAILSA paftd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFilterTagDetailsA\n"));
#endif
  if(fdwDetails & 
     ~(ACM_FILTERTAGDETAILSF_FILTERTAG|
       ACM_FILTERTAGDETAILSF_LARGESTSIZE))
    return MMSYSERR_INVALFLAG;

  /* FIXME
   *   How does the driver know if the ANSI or
   *   the UNICODE variant of PACMFILTERTAGDETAILS is used?
   *   It might check cbStruct or does it only accept ANSI.
   */
  return (MMRESULT) acmDriverMessage(
    had, ACMDM_FILTERTAG_DETAILS,
    (LPARAM) paftd,  (LPARAM) fdwDetails
  );
}

/***********************************************************************
 *           acmFilterTagDetailsW (MSACM32.20)
 */
MMRESULT WINAPI acmFilterTagDetailsW(
  HACMDRIVER had, PACMFILTERTAGDETAILSW paftd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFilterTagDetailsW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterTagEnumA (MSACM32.21)
 */
MMRESULT WINAPI acmFilterTagEnumA(
  HACMDRIVER had, PACMFILTERTAGDETAILSA  paftd,
  ACMFILTERTAGENUMCBA fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFilterTagEnumA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFilterTagEnumW (MSACM32.22)
 */
MMRESULT WINAPI acmFilterTagEnumW(
  HACMDRIVER had, PACMFILTERTAGDETAILSW paftd,
  ACMFILTERTAGENUMCBW fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFilterTagEnumW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatChooseA (MSACM32.23)
 */
MMRESULT WINAPI acmFormatChooseA(
  PACMFORMATCHOOSEA pafmtc)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatChooseA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatChooseW (MSACM32.24)
 */
MMRESULT WINAPI acmFormatChooseW(
  PACMFORMATCHOOSEW pafmtc)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatChooseW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatDetailsA (MSACM32.25)
 */
MMRESULT WINAPI acmFormatDetailsA(
  HACMDRIVER had, PACMFORMATDETAILSA pafd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatDetailsA\n"));
#endif
  if(fdwDetails & ~(ACM_FORMATDETAILSF_FORMAT))
    return MMSYSERR_INVALFLAG;

  /* FIXME
   *   How does the driver know if the ANSI or
   *   the UNICODE variant of PACMFORMATDETAILS is used?
   *   It might check cbStruct or does it only accept ANSI.
   */
  return (MMRESULT) acmDriverMessage(
    had, ACMDM_FORMAT_DETAILS,
    (LPARAM) pafd,  (LPARAM) fdwDetails
  );
}

/***********************************************************************
 *           acmFormatDetailsW (MSACM32.26)
 */
MMRESULT WINAPI acmFormatDetailsW(
  HACMDRIVER had, PACMFORMATDETAILSW pafd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatDetailsW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatEnumA (MSACM32.27)
 */
MMRESULT WINAPI acmFormatEnumA(
  HACMDRIVER had, PACMFORMATDETAILSA pafd,
  ACMFORMATENUMCBA fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatEnumA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatEnumW (MSACM32.28)
 */
MMRESULT WINAPI acmFormatEnumW(
  HACMDRIVER had, PACMFORMATDETAILSW pafd,
  ACMFORMATENUMCBW fnCallback, DWORD dwInstance,  DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatEnumW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatSuggest (MSACM32.29)
 */
MMRESULT WINAPI acmFormatSuggest(
  HACMDRIVER had, PWAVEFORMATEX pwfxSrc, PWAVEFORMATEX pwfxDst,
  DWORD cbwfxDst, DWORD fdwSuggest)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatSuggest not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatTagDetailsA (MSACM32.30)
 */
MMRESULT WINAPI acmFormatTagDetailsA(
  HACMDRIVER had, PACMFORMATTAGDETAILSA paftd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatTagDetailsA\n"));
#endif
  if(fdwDetails & 
     ~(ACM_FORMATTAGDETAILSF_FORMATTAG|ACM_FORMATTAGDETAILSF_LARGESTSIZE))
    return MMSYSERR_INVALFLAG;

  /* FIXME
   *   How does the driver know if the ANSI or
   *   the UNICODE variant of PACMFORMATTAGDETAILS is used?
   *   It might check cbStruct or does it only accept ANSI.
   */
  return (MMRESULT) acmDriverMessage(
    had, ACMDM_FORMATTAG_DETAILS,
    (LPARAM) paftd,  (LPARAM) fdwDetails
  );
}

/***********************************************************************
 *           acmFormatTagDetailsW (MSACM32.31)
 */
MMRESULT WINAPI acmFormatTagDetailsW(
  HACMDRIVER had, PACMFORMATTAGDETAILSW paftd, DWORD fdwDetails)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatTagDetailsW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatTagEnumA (MSACM32.32)
 */
MMRESULT WINAPI acmFormatTagEnumA(
  HACMDRIVER had, PACMFORMATTAGDETAILSA paftd,
  ACMFORMATTAGENUMCBA fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatTagEnumA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmFormatTagEnumW (MSACM32.33)
 */
MMRESULT WINAPI acmFormatTagEnumW(
  HACMDRIVER had, PACMFORMATTAGDETAILSW paftd,
  ACMFORMATTAGENUMCBW fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmFormatTagEnumW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmGetVersion (MSACM32.34)
 */
DWORD WINAPI acmGetVersion()
{
#ifdef DEBUG
  dprintf(("MSACM32: acmGetVersion\n"));
#endif
  switch(GetVersion()) 
    {
    default: 
// 
    case WIN95:
      return 0x04000000; /* 4.0.0 */
    case NT40:
      return 0x04000565; /* 4.0.1381 */
    }
}

/***********************************************************************
 *           acmMessage (MSACM32.35)
 * FIXME
 *   No documentation found.
 */

/***********************************************************************
 *           acmMetrics (MSACM32.36)
 */
MMRESULT WINAPI acmMetrics(
  HACMOBJ hao, UINT uMetric, LPVOID  pMetric)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmMetrics\n"));
#endif
  PWINE_ACMOBJ pao = MSACM_GetObj(hao);
  BOOL bLocal = TRUE;

  switch(uMetric)
    {
    case ACM_METRIC_COUNT_DRIVERS:
      bLocal = FALSE;
    case ACM_METRIC_COUNT_LOCAL_DRIVERS:
      if(!pao)
	return MMSYSERR_INVALHANDLE;  
      return MMSYSERR_NOTSUPPORTED;
    case ACM_METRIC_COUNT_CODECS:
      bLocal = FALSE;
    case ACM_METRIC_COUNT_LOCAL_CODECS:
      return MMSYSERR_NOTSUPPORTED;
    case ACM_METRIC_COUNT_CONVERTERS:
      bLocal = FALSE;
    case ACM_METRIC_COUNT_LOCAL_CONVERTERS:
      return MMSYSERR_NOTSUPPORTED;
    case ACM_METRIC_COUNT_FILTERS:
      bLocal = FALSE;
    case ACM_METRIC_COUNT_LOCAL_FILTERS:
      return MMSYSERR_NOTSUPPORTED;
    case ACM_METRIC_COUNT_DISABLED:
      bLocal = FALSE;
    case ACM_METRIC_COUNT_LOCAL_DISABLED:
      if(!pao)
	return MMSYSERR_INVALHANDLE;  
      return MMSYSERR_NOTSUPPORTED;
    case ACM_METRIC_COUNT_HARDWARE:
    case ACM_METRIC_HARDWARE_WAVE_INPUT:
    case ACM_METRIC_HARDWARE_WAVE_OUTPUT:
    case ACM_METRIC_MAX_SIZE_FORMAT:
    case ACM_METRIC_MAX_SIZE_FILTER:
    case ACM_METRIC_DRIVER_SUPPORT:
    case ACM_METRIC_DRIVER_PRIORITY:
    default:
      return MMSYSERR_NOTSUPPORTED;
    }
  return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmStreamClose (MSACM32.37)
 */
MMRESULT WINAPI acmStreamClose(
  HACMSTREAM has, DWORD fdwClose)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmStreamClose not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmStreamConvert (MSACM32.38)
 */
MMRESULT WINAPI acmStreamConvert(
  HACMSTREAM has, PACMSTREAMHEADER pash, DWORD fdwConvert)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmStreamConvert not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmStreamMessage (MSACM32.39)
 */
MMRESULT WINAPI acmStreamMessage(
  HACMSTREAM has, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmStreamMessage not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmStreamOpen (MSACM32.40)
 */
MMRESULT WINAPI acmStreamOpen(
  PHACMSTREAM phas, HACMDRIVER had, PWAVEFORMATEX pwfxSrc,
  PWAVEFORMATEX pwfxDst, PWAVEFILTER pwfltr, DWORD dwCallback,
  DWORD dwInstance, DWORD fdwOpen)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmStreamOpen not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}


/***********************************************************************
 *           acmStreamPrepareHeader (MSACM32.41)
 */
MMRESULT WINAPI acmStreamPrepareHeader(
  HACMSTREAM has, PACMSTREAMHEADER pash, DWORD fdwPrepare)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmStreamPrepareHeader not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmStreamReset (MSACM32.42)
 */
MMRESULT WINAPI acmStreamReset(
  HACMSTREAM has, DWORD fdwReset)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmStreamReset not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmStreamSize (MSACM32.43)
 */
MMRESULT WINAPI acmStreamSize(
  HACMSTREAM has, DWORD cbInput, 
  LPDWORD pdwOutputBytes, DWORD fdwSize)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmStreamSize not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmStreamUnprepareHeader (MSACM32.44)
 */
MMRESULT WINAPI acmStreamUnprepareHeader(
  HACMSTREAM has, PACMSTREAMHEADER pash, DWORD fdwUnprepare)
{
#ifdef DEBUG
  dprintf(("MSACM32: acmStreamUnprepareHeader not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return MMSYSERR_ERROR;
}
