/* -*- tab-width: 8; c-basic-offset: 4 -*- */
/* $Id: driver.c,v 1.2 2001-09-05 13:11:25 bird Exp $ */
/*
 *      MSACM32 library
 *
 *      Copyright 1998  Patrik Stridvall
 *        1999  Eric Pouech
 */

#include <stdio.h>

#include "winbase.h"
#include "winerror.h"
#include "windef.h"
#include "wingdi.h"
#include "winuser.h"
#include "debugtools.h"
#include "heap.h"
#include "mmsystem.h"
#include "msacm.h"
#include "msacmdrv.h"
#include "wineacm.h"
#include "winreg.h"

#ifdef __WIN32OS2__
#include "winnls.h"
#define snprintf wvsnprintfA
#endif

DEFAULT_DEBUG_CHANNEL(msacm);

/***********************************************************************
 *           acmDriverAddA (MSACM32.2)
 */
MMRESULT WINAPI acmDriverAddA(PHACMDRIVERID phadid, HINSTANCE hinstModule,
                  LPARAM lParam, DWORD dwPriority, DWORD fdwAdd)
{
    if (!phadid)
    return MMSYSERR_INVALPARAM;

    /* Check if any unknown flags */
    if (fdwAdd &
    ~(ACM_DRIVERADDF_FUNCTION|ACM_DRIVERADDF_NOTIFYHWND|
      ACM_DRIVERADDF_GLOBAL))
    return MMSYSERR_INVALFLAG;

    /* Check if any incompatible flags */
    if ((fdwAdd & ACM_DRIVERADDF_FUNCTION) &&
    (fdwAdd & ACM_DRIVERADDF_NOTIFYHWND))
    return MMSYSERR_INVALFLAG;

    /* FIXME: in fact, should GetModuleFileName(hinstModule) and do a
     * LoadDriver on it, to be sure we can call SendDriverMessage on the
     * hDrvr handle.
     */
    *phadid = (HACMDRIVERID) MSACM_RegisterDriver(NULL, NULL, hinstModule);

    /* FIXME: lParam, dwPriority and fdwAdd ignored */

    return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverAddW (MSACM32.3)
 * FIXME
 *   Not implemented
 */
MMRESULT WINAPI acmDriverAddW(PHACMDRIVERID phadid, HINSTANCE hinstModule,
                  LPARAM lParam, DWORD dwPriority, DWORD fdwAdd)
{
    FIXME("(%p, 0x%08x, %ld, %ld, %ld): stub\n",
      phadid, hinstModule, lParam, dwPriority, fdwAdd);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmDriverClose (MSACM32.4)
 */
MMRESULT WINAPI acmDriverClose(HACMDRIVER had, DWORD fdwClose)
{
    PWINE_ACMDRIVER pad;
    PWINE_ACMDRIVERID   padid;
    PWINE_ACMDRIVER*    tpad;

    if (fdwClose)
    return MMSYSERR_INVALFLAG;

    pad = MSACM_GetDriver(had);
    if (!pad)
    return MMSYSERR_INVALHANDLE;

    padid = pad->obj.pACMDriverID;

    /* remove driver from list */
    for (tpad = &(padid->pACMDriverList); *tpad; *tpad = (*tpad)->pNextACMDriver) {
    if (*tpad == pad) {
        *tpad = (*tpad)->pNextACMDriver;
        break;
    }
    }

    /* close driver if it has been opened */
    if (pad->hDrvr && !padid->hInstModule)
    CloseDriver(pad->hDrvr, 0, 0);

    HeapFree(MSACM_hHeap, 0, pad);

    return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverDetailsA (MSACM32.5)
 */
MMRESULT WINAPI acmDriverDetailsA(HACMDRIVERID hadid, PACMDRIVERDETAILSA padd, DWORD fdwDetails)
{
    MMRESULT mmr;
    ACMDRIVERDETAILSW   addw;

    addw.cbStruct = sizeof(addw);
    mmr = acmDriverDetailsW(hadid, &addw, fdwDetails);
    if (mmr == 0) {
    padd->fccType = addw.fccType;
    padd->fccComp = addw.fccComp;
    padd->wMid = addw.wMid;
    padd->wPid = addw.wPid;
    padd->vdwACM = addw.vdwACM;
    padd->vdwDriver = addw.vdwDriver;
    padd->fdwSupport = addw.fdwSupport;
    padd->cFormatTags = addw.cFormatTags;
    padd->cFilterTags = addw.cFilterTags;
    padd->hicon = addw.hicon;
        WideCharToMultiByte( CP_ACP, 0, addw.szShortName, -1, padd->szShortName,
                             sizeof(padd->szShortName), NULL, NULL );
        WideCharToMultiByte( CP_ACP, 0, addw.szLongName, -1, padd->szLongName,
                             sizeof(padd->szLongName), NULL, NULL );
        WideCharToMultiByte( CP_ACP, 0, addw.szCopyright, -1, padd->szCopyright,
                             sizeof(padd->szCopyright), NULL, NULL );
        WideCharToMultiByte( CP_ACP, 0, addw.szLicensing, -1, padd->szLicensing,
                             sizeof(padd->szLicensing), NULL, NULL );
        WideCharToMultiByte( CP_ACP, 0, addw.szFeatures, -1, padd->szFeatures,
                             sizeof(padd->szFeatures), NULL, NULL );
    }
    return mmr;
}

/***********************************************************************
 *           acmDriverDetailsW (MSACM32.6)
 */
MMRESULT WINAPI acmDriverDetailsW(HACMDRIVERID hadid, PACMDRIVERDETAILSW padd, DWORD fdwDetails)
{
    HACMDRIVER acmDrvr;
    MMRESULT mmr;

    if (fdwDetails)
    return MMSYSERR_INVALFLAG;

    mmr = acmDriverOpen(&acmDrvr, hadid, 0);
    if (mmr == MMSYSERR_NOERROR) {
    mmr = (MMRESULT)MSACM_Message(acmDrvr, ACMDM_DRIVER_DETAILS, (LPARAM)padd,  0);

    acmDriverClose(acmDrvr, 0);
    }

    return mmr;
}

/***********************************************************************
 *           acmDriverEnum (MSACM32.7)
 */
MMRESULT WINAPI acmDriverEnum(ACMDRIVERENUMCB fnCallback, DWORD dwInstance, DWORD fdwEnum)
{
    PWINE_ACMDRIVERID   p;
    ACMDRIVERDETAILSW   add;

    if (!fnCallback) return MMSYSERR_INVALPARAM;

    if (fdwEnum && ~(ACM_DRIVERENUMF_NOLOCAL|ACM_DRIVERENUMF_DISABLED))
    return MMSYSERR_INVALFLAG;

    add.cbStruct = sizeof(add);
    for (p = MSACM_pFirstACMDriverID; p; p = p->pNextACMDriverID) {
    if (acmDriverDetailsW((HACMDRIVERID)p, &add, 0) != MMSYSERR_NOERROR)
        continue;
    if (!p->bEnabled) {
        if (fdwEnum & ACM_DRIVERENUMF_DISABLED)
        add.fdwSupport |= ACMDRIVERDETAILS_SUPPORTF_DISABLED;
        else
        continue;
    }
    if (!(*fnCallback)((HACMDRIVERID)p, dwInstance, add.fdwSupport))
        break;
    }

    return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverID (MSACM32.8)
 */
MMRESULT WINAPI acmDriverID(HACMOBJ hao, PHACMDRIVERID phadid, DWORD fdwDriverID)
{
    PWINE_ACMOBJ pao;

    if (!phadid)
    return MMSYSERR_INVALPARAM;

    if (fdwDriverID)
    return MMSYSERR_INVALFLAG;

    pao = MSACM_GetObj(hao, WINE_ACMOBJ_DONTCARE);
    if (!pao)
    return MMSYSERR_INVALHANDLE;

    *phadid = (HACMDRIVERID) pao->pACMDriverID;

    return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverMessage (MSACM32.9)
 *
 */
LRESULT WINAPI acmDriverMessage(HACMDRIVER had, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
    if ((uMsg >= ACMDM_USER && uMsg < ACMDM_RESERVED_LOW) ||
    uMsg == ACMDM_DRIVER_ABOUT ||
    uMsg == DRV_QUERYCONFIGURE ||
    uMsg == DRV_CONFIGURE)
    return MSACM_Message(had, uMsg, lParam1, lParam2);
    return MMSYSERR_INVALPARAM;
}


/***********************************************************************
 *           acmDriverOpen (MSACM32.10)
 */
MMRESULT WINAPI acmDriverOpen(PHACMDRIVER phad, HACMDRIVERID hadid, DWORD fdwOpen)
{
    PWINE_ACMDRIVERID   padid;
    PWINE_ACMDRIVER pad;

    TRACE("(%p, %x, %08lu)\n", phad, hadid, fdwOpen);

    if (!phad)
    return MMSYSERR_INVALPARAM;

    if (fdwOpen)
    return MMSYSERR_INVALFLAG;

    padid = MSACM_GetDriverID(hadid);
    if (!padid)
    return MMSYSERR_INVALHANDLE;

    pad = HeapAlloc(MSACM_hHeap, 0, sizeof(WINE_ACMDRIVER));
    if (!pad) return MMSYSERR_NOMEM;

    pad->obj.dwType = WINE_ACMOBJ_DRIVER;
    pad->obj.pACMDriverID = padid;

    if (!(pad->hDrvr = padid->hInstModule) && padid->pszDriverAlias)
    pad->hDrvr = OpenDriverA(padid->pszDriverAlias, NULL, 0);
    if (!pad->hDrvr) {
    HeapFree(MSACM_hHeap, 0, pad);
    return MMSYSERR_ERROR;
    }

    /* insert new pad at beg of list */
    pad->pNextACMDriver = padid->pACMDriverList;
    padid->pACMDriverList = pad;

    /* FIXME: Create a WINE_ACMDRIVER32 */
    *phad = (HACMDRIVER)pad;
    TRACE("'%s' => %08lx\n", padid->pszDriverAlias, (DWORD)pad);
    return MMSYSERR_NOERROR;
}

/***********************************************************************
 *           acmDriverPriority (MSACM32.11)
 */
MMRESULT WINAPI acmDriverPriority(HACMDRIVERID hadid, DWORD dwPriority, DWORD fdwPriority)
{
    PWINE_ACMDRIVERID padid;
    CHAR szSubKey[17];
    CHAR szBuffer[256];
    LONG lBufferLength = sizeof(szBuffer);
    LONG lError;
    HKEY hPriorityKey;
    DWORD dwPriorityCounter;

    padid = MSACM_GetDriverID(hadid);
    if (!padid)
    return MMSYSERR_INVALHANDLE;

    /* Check for unknown flags */
    if (fdwPriority &
    ~(ACM_DRIVERPRIORITYF_ENABLE|ACM_DRIVERPRIORITYF_DISABLE|
      ACM_DRIVERPRIORITYF_BEGIN|ACM_DRIVERPRIORITYF_END))
    return MMSYSERR_INVALFLAG;

    /* Check for incompatible flags */
    if ((fdwPriority & ACM_DRIVERPRIORITYF_ENABLE) &&
    (fdwPriority & ACM_DRIVERPRIORITYF_DISABLE))
    return MMSYSERR_INVALFLAG;

    /* Check for incompatible flags */
    if ((fdwPriority & ACM_DRIVERPRIORITYF_BEGIN) &&
    (fdwPriority & ACM_DRIVERPRIORITYF_END))
    return MMSYSERR_INVALFLAG;

    lError = RegOpenKeyA(HKEY_CURRENT_USER,
             "Software\\Microsoft\\Multimedia\\"
             "Audio Compression Manager\\Priority v4.00",
             &hPriorityKey
             );
    /* FIXME: Create key */
    if (lError != ERROR_SUCCESS)
    return MMSYSERR_ERROR;

    for (dwPriorityCounter = 1; ; dwPriorityCounter++)  {
    snprintf(szSubKey, 17, "Priorty%ld", dwPriorityCounter);
    lError = RegQueryValueA(hPriorityKey, szSubKey, szBuffer, &lBufferLength);
    if (lError != ERROR_SUCCESS)
        break;

    FIXME("(0x%08x, %ld, %ld): stub (partial)\n",
          hadid, dwPriority, fdwPriority);
    break;
    }

    RegCloseKey(hPriorityKey);

    return MMSYSERR_ERROR;
}

/***********************************************************************
 *           acmDriverRemove (MSACM32.12)
 */
MMRESULT WINAPI acmDriverRemove(HACMDRIVERID hadid, DWORD fdwRemove)
{
    PWINE_ACMDRIVERID padid;

    padid = MSACM_GetDriverID(hadid);
    if (!padid)
    return MMSYSERR_INVALHANDLE;

    if (fdwRemove)
    return MMSYSERR_INVALFLAG;

    MSACM_UnregisterDriver(padid);

    return MMSYSERR_NOERROR;
}

