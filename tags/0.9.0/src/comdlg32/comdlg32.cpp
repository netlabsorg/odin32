/* $Id: comdlg32.cpp,v 1.32 2001-08-02 14:49:55 sandervl Exp $ */

/*
 * COMDLG32 implementation
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1999 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <odinwrap.h>
#include <winuser32.h>
#include <unicode.h>
#include <wingdi32.h>
#include <codepage.h>

ODINDEBUGCHANNEL(COMDLG32-COMDLG32)


#define COMDLG32_CHECKHOOK(a,b,c)           \
  if(a->Flags & b)                          \
  {                                         \
    a->lpfnHook = 0; \
  } \
  a->hwndOwner = Win32ToOS2Handle(a->hwndOwner);

#define COMDLG32_CHECKHOOK2(a,b,c,d)        \
  if(a->Flags & b)                          \
  {                                         \
    a->d = 0; \
  } \
  a->hwndOwner = Win32ToOS2Handle(a->hwndOwner);

// from gdi32/devcontext.cpp
BOOL WIN32API GetPMQueueName(LPSTR pDeviceName, LPSTR lpszPMQueue, INT cbPMQueue);

// USING_OPEN32 seems to be not necessary since the WGSS GlobalAlloc API appears
// to be in line with the Odin32 GlobalAlloc API (and the current version of
// WGSS doesn't export this API anyway)

#ifdef USING_OPEN32

#define FLAG_TO_OPEN32	     0
#define FLAG_FROM_OPEN32     1

static HGLOBAL GlobalCopy(HGLOBAL hDest, HGLOBAL hSource, BOOL fToOpen32)
{
    LPVOID src;
    LPVOID dest;
    ULONG  size;

    if (fToOpen32 == FLAG_TO_OPEN32)
    {
        src = GlobalLock(hSource);
        if (src)
        {
            size = GlobalSize(hSource);
            if(hDest == NULL)
            {
                hDest = O32_GlobalAlloc(GHND, size);
            }
            dest = O32_GlobalLock(hDest);
            memcpy(dest, src, size);
            O32_GlobalUnlock(hDest);
        }
        GlobalUnlock(hSource);
    }
    else
    {
        src = O32_GlobalLock(hSource);
        if (src)
        {
            size = O32_GlobalSize(hSource);
            if(hDest == NULL)
            {
                hDest = GlobalAlloc(GHND, size);
            }
            dest  = GlobalLock(hDest);
            memcpy(dest, src, size);
            GlobalUnlock(hDest);
        }
        O32_GlobalUnlock(hSource);
    }
    return hDest;
}

#define MyGlobalAlloc   O32_GlobalAlloc
#define MyGlobalLock    O32_GlobalLock
#define MyGlobalUnlock  O32_GlobalUnlock
#define MyGlobalFree    O32_GlobalFree
#define MyGlobalSize    O32_GlobalSize

#else

#define MyGlobalAlloc   GlobalAlloc
#define MyGlobalLock    GlobalLock
#define MyGlobalUnlock  GlobalUnlock
#define MyGlobalFree    GlobalFree
#define MyGlobalSize    GlobalSize

#endif

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PrintDlgA,
              LPPRINTDLGA, lppd)
{
    BOOL      ret;
#ifdef USING_OPEN32
    HGLOBAL   hOrgDevMode = 0, hOrgDevNames = 0;
#endif
    LPSTR     origDevice = NULL;

    COMDLG32_CHECKHOOK2(lppd, PD_ENABLEPRINTHOOK, LPPRINTHOOKPROC,lpfnPrintHook)
    COMDLG32_CHECKHOOK2(lppd, PD_ENABLESETUPHOOK, LPSETUPHOOKPROC,lpfnSetupHook)

#ifdef USING_OPEN32
    if (lppd->hDevMode)
    {
        hOrgDevMode = lppd->hDevMode;
        lppd->hDevMode = GlobalCopy(NULL, hOrgDevMode, FLAG_TO_OPEN32);
    }
    if (lppd->hDevNames)
    {
        hOrgDevNames = lppd->hDevNames;
        lppd->hDevNames = GlobalCopy(NULL, hOrgDevNames, FLAG_TO_OPEN32);
    }
#endif

    if (lppd->hDevNames)
    {
        LPDEVNAMES devNames = (LPDEVNAMES)MyGlobalLock(lppd->hDevNames);

        LPSTR driver = (LPSTR)devNames + devNames->wDriverOffset;
        LPSTR device = (LPSTR)devNames + devNames->wDeviceOffset;
        LPSTR output = (LPSTR)devNames + devNames->wOutputOffset;
        int driverLen = lstrlenA(driver) + 1;
        int deviceLen = lstrlenA(device) + 1;
        int outputLen = lstrlenA(output) + 1;

        // get the PM queue name for the device (which is usually the full
        // PM printer's WPS object name): WGSS expects it and will correctly
        // pre-select the printer specified in hDevNames. Note that we don't
        // perform the opposite conversion from the PM queue to the device name
        // after calling O32_PrintDlg() since this seems to work as it is:
        // the dialog returns the correct DC when OK is pressed despite the name
        // mismatch

        CHAR queue[256];
        if (GetPMQueueName(device, queue, sizeof(queue)))
        {
            dprintf(("PrintDlgA: renamed device {%s} to PM queue {%s}",
                     device, queue));
            device = queue;
            deviceLen = lstrlenA(device) + 1;

            HGLOBAL hDevNames2 =
                MyGlobalAlloc(GHND, MyGlobalSize(lppd->hDevNames) +
                              lstrlenA(queue) - (deviceLen - 1));

            LPDEVNAMES devNames2 = (LPDEVNAMES)MyGlobalLock(hDevNames2);

            devNames2->wDriverOffset = sizeof(DEVNAMES);
            memcpy((LPSTR)devNames2 + devNames2->wDriverOffset, driver, driverLen);
            devNames2->wDeviceOffset = devNames2->wDriverOffset + driverLen;
            memcpy((LPSTR)devNames2 + devNames2->wDeviceOffset, device, deviceLen);
            devNames2->wOutputOffset = devNames2->wDeviceOffset + deviceLen;
            memcpy((LPSTR)devNames2 + devNames2->wOutputOffset, output, outputLen);
            devNames2->wDefault = devNames->wDefault;

            MyGlobalUnlock(hDevNames2);

            MyGlobalUnlock(lppd->hDevNames);
            MyGlobalFree(lppd->hDevNames);
            lppd->hDevNames = hDevNames2;
        }
        else
            MyGlobalUnlock(lppd->hDevNames);
    }

    HWND hwndOwner = lppd->hwndOwner;
    if (lppd->hwndOwner)
    {
        lppd->hwndOwner = Win32ToOS2Handle(lppd->hwndOwner);
    }

    ret = O32_PrintDlg(lppd);

    lppd->hwndOwner = hwndOwner;

#ifdef USING_OPEN32
    if (ret == TRUE)
    {
        if (lppd->hDevMode)
        {
            HGLOBAL hDevMode = lppd->hDevMode;
            lppd->hDevMode = GlobalCopy(hOrgDevMode, lppd->hDevMode,
                                        FLAG_FROM_OPEN32);
            O32_GlobalFree(hDevMode);
        }
        if (lppd->hDevNames)
        {
            HGLOBAL hDevNames = lppd->hDevNames;
            lppd->hDevNames = GlobalCopy(hOrgDevNames, lppd->hDevNames,
                                         FLAG_FROM_OPEN32);
            O32_GlobalFree(hDevNames);
        }
    }
    else
    {
        if (lppd->hDevMode)
        {
            O32_GlobalFree(lppd->hDevMode);
            lppd->hDevMode = hOrgDevMode;
        }
        if (lppd->hDevNames)
        {
            O32_GlobalFree(lppd->hDevNames);
            lppd->hDevNames = hOrgDevNames;
        }
    }
#endif

    if(lppd->hDC)
    {
        OSLibGpiSetCp(lppd->hDC, GetDisplayCodepage());
    }

    return ret;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PrintDlgW,
              LPPRINTDLGW, lppd)
{
    PRINTDLGA pd;
    BOOL      bResult;

    memcpy(&pd,          // make binary copy first to save all the fields
           lppd,
           sizeof(pd));

    if (lppd->hwndOwner)
    {
        pd.hwndOwner = Win32ToOS2Handle(lppd->hwndOwner);
    }

    if (lppd->hDevMode)
    {
        LPDEVMODEW devMode = (LPDEVMODEW)GlobalLock(lppd->hDevMode);

        dprintf(("PrintDlgW: in devMode{%.*ls,%.*ls}",
                 CCHDEVICENAME, devMode->dmDeviceName,
                 CCHFORMNAME, devMode->dmFormName));

        pd.hDevMode = MyGlobalAlloc(GHND, sizeof(DEVMODEA));

        LPDEVMODEA devModeA = (LPDEVMODEA)MyGlobalLock(pd.hDevMode);

        // convert to ASCII string
        UnicodeToAsciiN((LPWSTR)devMode->dmDeviceName, (LPSTR)devModeA->dmDeviceName,
                        CCHDEVICENAME);
        UnicodeToAsciiN((LPWSTR)devMode->dmFormName, (LPSTR)devModeA->dmFormName,
                        CCHFORMNAME);

        MyGlobalLock(pd.hDevMode);
        GlobalLock(lppd->hDevMode);
    }

    if (lppd->hDevNames)
    {
        LPDEVNAMES devNames = (LPDEVNAMES)GlobalLock(lppd->hDevNames);

        dprintf(("PrintDlgW: in devNames{%ls,%ls,%ls}",
                 (LPCWSTR)devNames + devNames->wDriverOffset,
                 (LPCWSTR)devNames + devNames->wDeviceOffset,
                 (LPCWSTR)devNames + devNames->wOutputOffset));

        // convert to ASCII string
        LPSTR driver = UnicodeToAsciiString((LPCWSTR)devNames + devNames->wDriverOffset);
        LPSTR device = UnicodeToAsciiString((LPCWSTR)devNames + devNames->wDeviceOffset);
        LPSTR output = UnicodeToAsciiString((LPCWSTR)devNames + devNames->wOutputOffset);
        int driverLen = lstrlenA(driver) + 1;
        int deviceLen = lstrlenA(device) + 1;
        int outputLen = lstrlenA(output) + 1;

        // get the PM queue name for the device (which is usually the full
        // PM printer's WPS object name): WGSS expects it and will correctly
        // pre-select the printer specified in hDevNames. Note that we don't
        // perform the opposite conversion from the PM queue to the device name
        // after calling O32_PrintDlg() since this seems to work as it is:
        // the dialog returns the correct DC when OK is pressed despite the name
        // mismatch

        CHAR queue[256];
        if (GetPMQueueName(device, queue, sizeof(queue)))
        {
            dprintf(("PrintDlgW: renamed device {%s} to PM queue {%s}",
                     device, queue));
            device = queue;
            deviceLen = lstrlenA(device) + 1;
        }

        pd.hDevNames = MyGlobalAlloc(GHND, sizeof(DEVNAMES) +
                                     driverLen + deviceLen + outputLen);

        LPDEVNAMES devNamesA = (LPDEVNAMES)MyGlobalLock(pd.hDevNames);

        devNamesA->wDriverOffset = sizeof(DEVNAMES);
        memcpy((LPSTR)devNamesA + devNamesA->wDriverOffset, driver, driverLen);
        devNamesA->wDeviceOffset = devNamesA->wDriverOffset + driverLen;
        memcpy((LPSTR)devNamesA + devNamesA->wDeviceOffset, device, deviceLen);
        devNamesA->wOutputOffset = devNamesA->wDeviceOffset + deviceLen;
        memcpy((LPSTR)devNamesA + devNamesA->wOutputOffset, output, outputLen);
        devNamesA->wDefault = devNames->wDefault;

        FreeAsciiString(output);
        if (device != queue)
            FreeAsciiString(device);
        FreeAsciiString(driver);

        MyGlobalUnlock(pd.hDevNames);
        GlobalUnlock(lppd->hDevNames);
    }

    // convert to ASCII string
    if ((lppd->Flags & PD_ENABLEPRINTTEMPLATE) &&
        (lppd->lpPrintTemplateName != NULL))
        pd.lpPrintTemplateName = UnicodeToAsciiString((WCHAR*)lppd->lpPrintTemplateName);
    else
        pd.lpPrintTemplateName = NULL;

    if ((lppd->Flags & PD_ENABLESETUPTEMPLATE) &&
        (lppd->lpSetupTemplateName != NULL))
        pd.lpSetupTemplateName = UnicodeToAsciiString((WCHAR*)lppd->lpSetupTemplateName);
    else
        pd.lpSetupTemplateName = NULL;

    COMDLG32_CHECKHOOK2((&pd), PD_ENABLEPRINTHOOK, LPPRINTHOOKPROC, lpfnPrintHook)
    COMDLG32_CHECKHOOK2((&pd), PD_ENABLESETUPHOOK, LPSETUPHOOKPROC, lpfnSetupHook)

    bResult = O32_PrintDlg(&pd); // call ASCII API

    if (pd.lpPrintTemplateName != NULL)
        FreeAsciiString((char*)pd.lpPrintTemplateName);
    if (pd.lpSetupTemplateName != NULL)
        FreeAsciiString((char*)pd.lpSetupTemplateName);

    if (pd.hDevNames)
    {
        LPDEVNAMES devNames = (LPDEVNAMES)MyGlobalLock(pd.hDevNames);

        dprintf(("PrintDlgW: out devNames{%s,%s,%s}",
                 (LPCSTR)devNames + devNames->wDriverOffset,
                 (LPCSTR)devNames + devNames->wDeviceOffset,
                 (LPCSTR)devNames + devNames->wOutputOffset));

        LPSTR realDevice = (LPSTR)devNames + devNames->wDeviceOffset;

        // convert from ASCII string
        LPWSTR driver = AsciiToUnicodeString((LPCSTR)devNames + devNames->wDriverOffset);
        LPWSTR device = AsciiToUnicodeString(realDevice);
        LPWSTR output = AsciiToUnicodeString((LPCSTR)devNames + devNames->wOutputOffset);
        int driverLen = (lstrlenW(driver) + 1) * sizeof(WCHAR);
        int deviceLen = (lstrlenW(device) + 1) * sizeof(WCHAR);
        int outputLen = (lstrlenW(output) + 1) * sizeof(WCHAR);

        if (lppd->hDevNames)
            lppd->hDevNames = GlobalReAlloc(lppd->hDevNames,
                                            sizeof(DEVNAMES) +
                                            driverLen + deviceLen + outputLen,
                                            GHND);
        else
            lppd->hDevNames = GlobalAlloc(GHND, sizeof(DEVNAMES) +
                                          driverLen + deviceLen + outputLen);

        LPDEVNAMES devNamesW = (LPDEVNAMES)GlobalLock(lppd->hDevNames);

        devNamesW->wDriverOffset = sizeof(DEVNAMES) / sizeof(WCHAR);
        memcpy((LPWSTR)devNamesW + devNamesW->wDriverOffset, driver, driverLen);
        devNamesW->wDeviceOffset = devNamesW->wDriverOffset + (driverLen / sizeof(WCHAR));
        memcpy((LPWSTR)devNamesW + devNamesW->wDeviceOffset, device, deviceLen);
        devNamesW->wOutputOffset = devNamesW->wDeviceOffset + (deviceLen / sizeof(WCHAR));
        memcpy((LPWSTR)devNamesW + devNamesW->wOutputOffset, output, outputLen);
        devNamesW->wDefault = devNames->wDefault;

        FreeAsciiString(output);
        FreeAsciiString(device);
        FreeAsciiString(driver);

        GlobalUnlock(lppd->hDevNames);
        MyGlobalUnlock(pd.hDevNames);

        MyGlobalFree(pd.hDevNames);
    }
    else
    {
         if (lppd->hDevNames)
            MyGlobalFree(lppd->hDevNames);
         lppd->hDevNames = NULL;
    }

    if (pd.hDevMode)
    {
        LPDEVMODEA devMode = (LPDEVMODEA)MyGlobalLock(pd.hDevMode);

        dprintf(("PrintDlgW: out devMode{%.*s,%.*s}",
                 CCHDEVICENAME, devMode->dmDeviceName,
                 CCHFORMNAME, devMode->dmFormName));

        if (!lppd->hDevMode)
            lppd->hDevMode = GlobalAlloc(GHND, sizeof(DEVMODEW));

        LPDEVMODEW devModeW = (LPDEVMODEW)GlobalLock(lppd->hDevMode);

        // convert from ASCII string
        AsciiToUnicodeN((LPSTR)devMode->dmDeviceName, (LPWSTR)devModeW->dmDeviceName,
                        CCHDEVICENAME);
        AsciiToUnicodeN((LPSTR)devMode->dmFormName, (LPWSTR)devModeW->dmFormName,
                        CCHFORMNAME);

        memcpy(&devModeW->dmSpecVersion, &devMode->dmSpecVersion,
               offsetof(DEVMODEW, dmFormName) - offsetof(DEVMODEW, dmSpecVersion));
        memcpy(&devModeW->dmLogPixels, &devMode->dmLogPixels,
               sizeof(DEVMODEW) - offsetof(DEVMODEW, dmLogPixels));

        GlobalUnlock(lppd->hDevMode);
        MyGlobalUnlock(pd.hDevMode);

        MyGlobalFree(pd.hDevMode);
    }
    else
    {
         if (lppd->hDevNames)
            MyGlobalFree(lppd->hDevNames);
         lppd->hDevNames = NULL;
    }

    // copy back the rest
    lppd->hDC       = pd.hDC;
    lppd->Flags     = pd.Flags;
    lppd->nFromPage = pd.nFromPage;
    lppd->nToPage   = pd.nToPage;
    lppd->nMinPage  = pd.nMinPage;
    lppd->nMaxPage  = pd.nMaxPage;
    lppd->nCopies   = pd.nCopies;

    if (lppd->hDC)
    {
        OSLibGpiSetCp(lppd->hDC, GetDisplayCodepage());
    }

    return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PageSetupDlgA,
              LPPAGESETUPDLGA, lppsd)
{

  dprintf(("COMDLG32: PageSetupDlgA not implemented.\n"));

  //COMDLG32_CHECKHOOK2(lppsd, PSD_ENABLESETUPHOOK, LPPAGESETUPHOOK, lpfnPageSetupHook)

  return(FALSE);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PageSetupDlgW,
              LPPAGESETUPDLGW, lppsd)
{

  dprintf(("COMDLG32: PageSetupDlgW(%08xh) not implemented.\n"));

  //COMDLG32_CHECKHOOK2(lppsd, PSD_ENABLESETUPHOOK, LPPAGESETUPHOOK, lpfnPageSetupHook)

  return(FALSE);
}

