/* $Id: winspool.cpp,v 1.6 2001-04-04 09:02:16 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * WINSPOOL stubs
 *
 * Copyright 1998 Patrick Haller
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Partially based on Wine code (dlls\winspool\info.c (EnumPrinters, DeviceCapabilitiesA/W)
 *
 * Copyright 1996 John Harvey
 * Copyright 1998 Andreas Mohr
 * Copyright 1999 Klaas van Gend
 * Copyright 1999, 2000 Huw D M Davies
 *
 * TODO: far from complete (although the Spl API seems to provide everything we need!)
 *
 */

/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <winspool.h>
#include <odinwrap.h>
#include <heapstring.h>
#include <win\winnls.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <misc.h>
#include "oslibspl.h"

ODINDEBUGCHANNEL(WINSPOOL)


/*****************************************************************************
 * Name      : BOOL EnumPrintersA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

ODINFUNCTION7(BOOL,    EnumPrintersA,
              DWORD,   dwType,
              LPTSTR,  lpszName,
              DWORD,   dwLevel,
              LPBYTE,  lpbPrinters,
              DWORD,   cbBuf,
              LPDWORD, lpdwNeeded,
              LPDWORD, lpdwReturned)
{
 ULONG cReturned, cTotal, cbNeeded, flType;
 int   used, nameoff;
 OSLIB_PRINTERINFO *printerInfo;
 PRINTER_INFO_1A *pi1;
 PRINTER_INFO_2A *pi2;
 PRINTER_INFO_4A *pi4;
 PRINTER_INFO_5A *pi5;
 LPSTR lpszPrinterStrings;

    if(lpbPrinters)
        memset(lpbPrinters, 0, cbBuf);

    if(lpdwReturned)
        *lpdwReturned = 0;

    if (!((dwType & PRINTER_ENUM_LOCAL) || (dwType & PRINTER_ENUM_NAME)))
    {
        dprintf(("Invalid parameters !(PRINTER_ENUM_LOCAL & PRINTER_ENUM_NAME)!!"));
        SetLastError(ERROR_INVALID_FLAGS);
        return FALSE;
    }

    if(lpszName) {//TODO:
        dprintf(("remote printer enumeration not (yet) supported!!"));
        flType = OSLIB_SPL_PR_ALL;
    }
    else flType = OSLIB_SPL_PR_LOCAL_ONLY | OSLIB_SPL_PR_ALL;

    if(OSLibSplEnumPrinters(flType, NULL, 0, &cReturned, &cTotal, &cbNeeded) == FALSE) {
        SetLastError(ERROR_FILE_NOT_FOUND); //todo wrong error
        return FALSE;
    }
    if(cTotal == 0) {//no printers installed
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    switch(dwLevel) {
    case 1:
        used = cTotal * sizeof(PRINTER_INFO_1A);
        break;

    case 2:
        used = cTotal * sizeof(PRINTER_INFO_2A);
        break;

    case 4:
        used = cTotal * sizeof(PRINTER_INFO_4A);
        break;

    case 5:
        used = cTotal * sizeof(PRINTER_INFO_5A);
        break;

    default:
        dprintf(("ERROR: EnumPrintersA: Unknown level %d!", dwLevel));
        SetLastError(ERROR_INVALID_LEVEL);
        return FALSE;
    }
    nameoff = used;
    used   += cbNeeded;   //add size of printer names

    if(used > cbBuf) {
        if(lpdwNeeded)
            *lpdwNeeded = used;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    printerInfo = (OSLIB_PRINTERINFO *)malloc(cbNeeded);
    if(OSLibSplEnumPrinters(flType, printerInfo, cbNeeded, &cReturned, &cTotal, &cbNeeded) == FALSE) {
        free(printerInfo);
        SetLastError(ERROR_FILE_NOT_FOUND);
        return FALSE;
    }
    lpszPrinterStrings = (char *)lpbPrinters + nameoff;

    for(int i=0;i<cReturned;i++) {
        switch(dwLevel) {
        case 1:
            pi1 = (PRINTER_INFO_1A *)lpbPrinters;
            lpbPrinters += sizeof(PRINTER_INFO_1A);

            pi1->Flags = PRINTER_ENUM_ICON8;
            pi1->pName = lpszPrinterStrings;
            strcpy(lpszPrinterStrings, printerInfo[i].pszPrintDestinationName);
            lpszPrinterStrings += strlen(printerInfo[i].pszPrintDestinationName)+1;

            if(printerInfo[i].pszDescription) {
                pi1->pDescription = lpszPrinterStrings;
                strcpy(lpszPrinterStrings, printerInfo[i].pszDescription);
                lpszPrinterStrings += strlen(printerInfo[i].pszDescription)+1;
            }
            //pComment empty
            break;

        case 2:
            pi2 = (PRINTER_INFO_2A *)lpbPrinters;
            lpbPrinters += sizeof(PRINTER_INFO_2A);

            pi2->pPrinterName = lpszPrinterStrings;
            strcpy(lpszPrinterStrings, printerInfo[i].pszPrintDestinationName);
            lpszPrinterStrings += strlen(printerInfo[i].pszPrintDestinationName)+1;
            pi2->pDriverName = pi2->pPrinterName;   //TODO:might not be correct!

            if(printerInfo[i].pszLocalName) {
                pi2->pPortName = lpszPrinterStrings;
                strcpy(lpszPrinterStrings, printerInfo[i].pszLocalName);
                lpszPrinterStrings += strlen(printerInfo[i].pszLocalName)+1;
            }

            if(printerInfo[i].pszComputerName) {
                pi2->pServerName = lpszPrinterStrings;
                strcpy(lpszPrinterStrings, printerInfo[i].pszComputerName);
                lpszPrinterStrings += strlen(printerInfo[i].pszComputerName)+1;
            }

            pi2->Attributes   = PRINTER_ATTRIBUTE_QUEUED;   //todo

            dprintf(("EnumPrinters level 2 NOT COMPLETE!!"));
            //todo not complete
            break;

        case 4:
            pi4 = (PRINTER_INFO_4A *)lpbPrinters;
            lpbPrinters += sizeof(PRINTER_INFO_4A);

            pi4->Attributes   = PRINTER_ATTRIBUTE_QUEUED;   //todo
            pi4->pPrinterName = lpszPrinterStrings;
            strcpy(lpszPrinterStrings, printerInfo[i].pszPrintDestinationName);
            lpszPrinterStrings += strlen(printerInfo[i].pszPrintDestinationName)+1;

            if(printerInfo[i].pszComputerName) {
                pi4->pServerName = lpszPrinterStrings;
                strcpy(lpszPrinterStrings, printerInfo[i].pszComputerName);
                lpszPrinterStrings += strlen(printerInfo[i].pszComputerName)+1;
            }
            dprintf(("EnumPrinters level 4 NOT COMPLETE!!"));
            break;

        case 5:
            pi5 = (PRINTER_INFO_5A *)lpbPrinters;
            lpbPrinters += sizeof(PRINTER_INFO_5A);

            pi5->Attributes   = PRINTER_ATTRIBUTE_QUEUED;   //todo
            pi5->pPrinterName = lpszPrinterStrings;
            strcpy(lpszPrinterStrings, printerInfo[i].pszPrintDestinationName);
            lpszPrinterStrings += strlen(printerInfo[i].pszPrintDestinationName)+1;

            if(printerInfo[i].pszLocalName) {
                pi5->pPortName = lpszPrinterStrings;
                strcpy(lpszPrinterStrings, printerInfo[i].pszLocalName);
                lpszPrinterStrings += strlen(printerInfo[i].pszLocalName)+1;
            }
            dprintf(("EnumPrinters level 5 NOT COMPLETE!!"));
            break;
        }
    }
    free(printerInfo);

    if(lpdwNeeded)
        *lpdwNeeded = used;

    if(lpdwReturned)
        *lpdwReturned = cReturned;
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}


/*****************************************************************************
 * Name      : BOOL ClosePrinter
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [09.07.98 13:39:08]
 *****************************************************************************/

ODINFUNCTION1(BOOL, ClosePrinter,
              HANDLE, hPrinter)
{
  dprintf(("ClosePrinter, not implemented\n"));
  return(TRUE);
}


/*****************************************************************************
 * Name      : BOOL OpenPrinterA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [09.07.98 13:39:08]
 *****************************************************************************/

ODINFUNCTION3(BOOL, OpenPrinterA,
              LPSTR, pPrinterName,
              HANDLE *, phPrinter,
              PRINTER_DEFAULTSA *, pDefault)
{
  dprintf(("OpenPrinterA, not implemented\n"));
  return(FALSE);
}


/*****************************************************************************
 * Name      : LONG DocumentPropertiesA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [09.07.98 13:39:08]
 *****************************************************************************/

ODINFUNCTION6(LONG, DocumentPropertiesA,
              HWND, hwnd,
              HANDLE, hPrinter,
              LPSTR, lpszDeviceName,
              LPDEVMODEA, pdmOutput,
              LPDEVMODEA, pdmInput,
              DWORD, fMode)
{
  dprintf(("DocumentPropertiesA, not implemented\n"));
  return(-1);
}


/*****************************************************************************
 * Name      : BOOL AbortPrinter
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.01 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 13:39:08]
 *****************************************************************************/

ODINFUNCTION1(BOOL, AbortPrinter,
              HANDLE, hPrinter)

{
  dprintf(("WINSPOOL: AbortPrinter not implemented\n"));
  return (FALSE);
}



/*****************************************************************************
 * Name      : BOOL AddFormA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             DWORD  Level     data-structure level
 *             LPBYTE pForm     pointer to form info. data structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.01 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 13:47:45]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddFormA,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pForm)
{
  dprintf(("WINSPOOL: AddFormA not implemented\n"));
  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL AddFormW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             DWORD  Level     data-structure level
 *             LPBYTE pForm     pointer to form info. data structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.01 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 13:47:45]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddFormW,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pForm)
{
  dprintf(("WINSPOOL: AddFormW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddJobA
 * Purpose   :
 * Parameters: HANDLE hPrinter  specifies printer for the print job
 *             DWORD Level  specifies version of print job information data structure
 *             LPBYTE pData  pointer to buffer to receive print job information data
 *             DWORD cbBuf  specifies size of buffer pointed to by pData
 *             LPDWORD pcbNeeded  pointer to variable to receive size of print job information data
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.01 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 13:50:28]
 *****************************************************************************/

ODINFUNCTION5(BOOL, AddJobA,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pData,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: AddJobA not implemented\n"));
  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL AddJobW
 * Purpose   :
 * Parameters: HANDLE  hPrinter   specifies printer for the print job
 *             DWORD   Level      specifies version of print job information
 *                                data structure
 *             LPBYTE  pData      pointer to buffer to receive print job
 *                                information data
 *             DWORD   cbBuf      specifies size of buffer pointed to by pData
 *             LPDWORD pcbNeeded  pointer to variable to receive size of print
 *                                job information data
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.01 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 13:50:28]
 *****************************************************************************/

ODINFUNCTION5(BOOL, AddJobW,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pData,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: AddJobW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddMonitorA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             DWORD Level  monitor info. structure level
 *             LPBYTE pMonitors  pointer to monitor info. structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:15:55]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddMonitorA,
              LPSTR, pName,
              DWORD, Level,
              LPBYTE, pMonitors)
{
  dprintf(("WINSPOOL: AddMonitorA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddMonitorW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             DWORD Level  monitor info. structure level
 *             LPBYTE pMonitors  pointer to monitor info. structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:15:55]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddMonitorW,
              LPWSTR, pName,
              DWORD, Level,
              LPBYTE, pMonitors)
{
  dprintf(("WINSPOOL: AddMonitorW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPortA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to a server name
 *             HWND hWnd  handle to parent window
 *             LPSTR pMonitorName  pointer to a monitor name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:16:31]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddPortA,
              LPSTR, pName,
              HWND, hWnd,
              LPSTR, pMonitorName)
{
  dprintf(("WINSPOOL: AddPortA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPortW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to a server name
 *             HWND hWnd  handle to parent window
 *             LPWSTR pMonitorName  pointer to a monitor name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:16:31]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddPortW,
              LPWSTR, pName,
              HWND, hWnd,
              LPWSTR, pMonitorName)
{
  dprintf(("WINSPOOL: AddPortW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPrintProcessorA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             LPSTR pEnvironment  pointer to environment name
 *             LPSTR pPathName  pointer to path
 *             LPSTR pPrintProcessorName  pointer to print-processor name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:17:46]
 *****************************************************************************/

ODINFUNCTION4(BOOL, AddPrintProcessorA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              LPSTR, pPathName,
              LPSTR, pPrintProcessorName)
{
  dprintf(("WINSPOOL: AddPrintProcessorA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPrintProcessorW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             LPWSTR pEnvironment  pointer to environment name
 *             LPWSTR pPathName  pointer to path
 *             LPWSTR pPrintProcessorName  pointer to print-processor name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:17:46]
 *****************************************************************************/

ODINFUNCTION4(BOOL, AddPrintProcessorW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              LPWSTR, pPathName,
              LPWSTR, pPrintProcessorName)
{
  dprintf(("WINSPOOL: AddPrintProcessorW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPrintProvidorA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             DWORD Level  provider information structure level
 *             LPBYTE pProvidorInfo  pointer to provider information structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:18:16]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddPrintProvidorA,
              LPSTR, pName,
              DWORD, Level,
              LPBYTE, pProvidorInfo)
{
  dprintf(("WINSPOOL: AddPrintProvidorA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPrintProvidorW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             DWORD Level  provider information structure level
 *             LPBYTE pProvidorInfo  pointer to provider information structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:18:16]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddPrintProvidorW,
              LPWSTR, pName,
              DWORD, Level,
              LPBYTE, pProvidorInfo)
{
  dprintf(("WINSPOOL: AddPrintProvidorW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : HANDLE AddPrinterA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             DWORD Level  printer info. structure level
 *             LPBYTE pPrinter  pointer to structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:18:56]
 *****************************************************************************/

ODINFUNCTION3(HANDLE, AddPrinterA,
              LPSTR, pName,
              DWORD, Level,
              LPBYTE, pPrinter)
{
  dprintf(("WINSPOOL: AddPrinterA not implemented\n"));
  return (NULL);
}


/*****************************************************************************
 * Name      : HANDLE AddPrinterW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             DWORD Level  printer info. structure level
 *             LPBYTE pPrinter  pointer to structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:18:56]
 *****************************************************************************/

ODINFUNCTION3(HANDLE, AddPrinterW,
              LPWSTR, pName,
              DWORD, Level,
              LPBYTE, pPrinter)
{
  dprintf(("WINSPOOL: AddPrinterW not implemented\n"));
  return (NULL);
}


/*****************************************************************************
 * Name      : BOOL AddPrinterConnectionA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to printer name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:19:22]
 *****************************************************************************/

ODINFUNCTION1(BOOL, AddPrinterConnectionA,
              LPSTR, pName)
{
  dprintf(("WINSPOOL: AddPrinterConnectionA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPrinterConnectionW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to printer name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:19:22]
 *****************************************************************************/

ODINFUNCTION1(BOOL, AddPrinterConnectionW,
              LPWSTR, pName)
{
  dprintf(("WINSPOOL: AddPrinterConnectionW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPrinterDriverA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             DWORD Level  printer info. structure level
 *             LPBYTE pDriverInfo  pointer to printer info. structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:20:04]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddPrinterDriverA,
              LPSTR, pName,
              DWORD, Level,
              LPBYTE, pDriverInfo)
{
  dprintf(("WINSPOOL: AddPrinterDriverA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL AddPrinterDriverW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             DWORD Level  printer info. structure level
 *             LPBYTE pDriverInfo  pointer to printer info. structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:20:04]
 *****************************************************************************/

ODINFUNCTION3(BOOL, AddPrinterDriverW,
              LPWSTR, pName,
              DWORD, Level,
              LPBYTE, pDriverInfo)
{
  dprintf(("WINSPOOL: AddPrinterDriverW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : LONG AdvancedDocumentPropertiesA
 * Purpose   :
 * Parameters: HWND hWnd  handle to dialog box's parent window
 *             HANDLE hPrinter  handle to printer object
 *             LPSTR pDeviceName  pointer to driver name
 *             LPDEVMODEA pDevModeInput  pointer to modified device mode structure
 *             LPDEVMODEA pDevModeInput  pointer to original device mode structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:21:59]
 *****************************************************************************/

ODINFUNCTION5(LONG, AdvancedDocumentPropertiesA,
              HWND, hWnd,
              HANDLE, hPrinter,
              LPSTR, pDeviceName,
              LPDEVMODEA, pDevModeOutput,
              LPDEVMODEA, pDevModeInput)
{
  dprintf(("WINSPOOL: AdvancedDocumentPropertiesA not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : LONG AdvancedDocumentPropertiesW
 * Purpose   :
 * Parameters: HWND hWnd  handle to dialog box's parent window
 *             HANDLE hPrinter  handle to printer object
 *             LPWSTR pDeviceName  pointer to driver name
 *             LPDEVMODEW pDevModeOutput  pointer to modified device mode structure
 *             LPDEVMODEW pDevModeInput  pointer to original device mode structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:21:59]
 *****************************************************************************/

ODINFUNCTION5(LONG, AdvancedDocumentPropertiesW,
              HWND, hWnd,
              HANDLE, hPrinter,
              LPWSTR, pDeviceName,
              LPDEVMODEW, pDevModeOutput,
              LPDEVMODEW, pDevModeInput)
{
  dprintf(("WINSPOOL: AdvancedDocumentPropertiesW not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : BOOL ConfigurePortA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             HWND hWnd  handle to parent window of the dialog box
 *             LPSTR pPortName  pointer to port name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:24:42]
 *****************************************************************************/

ODINFUNCTION3(BOOL, ConfigurePortA,
              LPSTR, pName,
              HWND, hWnd,
              LPSTR, pPortName)
{
  dprintf(("WINSPOOL: ConfigurePortA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL ConfigurePortW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             HWND hWnd  handle to parent window of the dialog box
 *             LPWSTR pPortName  pointer to port name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:24:43]
 *****************************************************************************/

ODINFUNCTION3(BOOL, ConfigurePortW,
              LPWSTR, pName,
              HWND, hWnd,
              LPWSTR, pPortName)
{
  dprintf(("WINSPOOL: ConfigurePortW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : HANDLE ConnectToPrinterDlg
 * Purpose   :
 * Parameters: HWND hwnd  handle to parent window of dialog box
 *             DWORD Flags  reserved for future use, must be zero
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:25:13]
 *****************************************************************************/

ODINFUNCTION2(HANDLE, ConnectToPrinterDlg,
              HWND, hwnd,
              DWORD, Flags)
{
  dprintf(("WINSPOOL: ConnectToPrinterDlg not implemented\n"));
  return (NULL);
}


/***********************************************************
 *      DEVMODEdupWtoA
 * Creates an ascii copy of supplied devmode on heap
 */
static LPDEVMODEA DEVMODEdupWtoA(HANDLE heap, const DEVMODEW *dmW)
{
    LPDEVMODEA dmA;
    DWORD size;
    BOOL Formname;
    ptrdiff_t off_formname = (char *)dmW->dmFormName - (char *)dmW;

    if(!dmW) return NULL;
    Formname = (dmW->dmSize > off_formname);
    size = dmW->dmSize - CCHDEVICENAME - (Formname ? CCHFORMNAME : 0);
    dmA = (LPDEVMODEA)HeapAlloc(heap, HEAP_ZERO_MEMORY, size + dmW->dmDriverExtra);
    WideCharToMultiByte(CP_ACP, 0, dmW->dmDeviceName, -1, (LPSTR)dmA->dmDeviceName,
            CCHDEVICENAME, NULL, NULL);
    if(!Formname) {
      memcpy(&dmA->dmSpecVersion, &dmW->dmSpecVersion,
         dmW->dmSize - CCHDEVICENAME * sizeof(WCHAR));
    } else {
      memcpy(&dmA->dmSpecVersion, &dmW->dmSpecVersion,
         off_formname - CCHDEVICENAME * sizeof(WCHAR));
      WideCharToMultiByte(CP_ACP, 0, dmW->dmFormName, -1, (LPSTR)dmA->dmFormName,
              CCHFORMNAME, NULL, NULL);
      memcpy(&dmA->dmLogPixels, &dmW->dmLogPixels, dmW->dmSize -
         (off_formname + CCHFORMNAME * sizeof(WCHAR)));
    }
    dmA->dmSize = size;
    memcpy((char *)dmA + dmA->dmSize, (char *)dmW + dmW->dmSize,
       dmW->dmDriverExtra);
    return dmA;
}

/*****************************************************************************
 * Name      : DWORD DeviceCapabilitiesA
 * Purpose   :
 * Parameters: LPCSTR pDevice  pointer to a printer-name string
 *             LPCSTR pPort  pointer to a port-name string
 *             WORD fwCapability  device capability to query
 *             LPSTR pOutput  pointer to the output
 *             CONST DEVMODE *pDevMode  pointer to structure with device data
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:27:08]
 *****************************************************************************/

ODINFUNCTION5(INT, DeviceCapabilitiesA,
              LPCSTR, pDevice,
              LPCSTR, pPort,
              WORD, fwCapability,
              LPSTR, pOutput,
              DEVMODEA *, pDevMode)
{
  dprintf(("WINSPOOL: DeviceCapabilitiesA %s %s %x %x %x", pDevice, pPort, fwCapability, pOutput, pDevMode));
  return O32_DeviceCapabilities(pDevice, pPort, fwCapability, pOutput, pDevMode);
}


/*****************************************************************************
 *          DeviceCapabilitiesW        [WINSPOOL.152]
 *
 * Call DeviceCapabilitiesA since we later call 16bit stuff anyway
 *
 */
ODINFUNCTION5(INT, DeviceCapabilitiesW, LPCWSTR, pDevice, LPCWSTR, pPort,
                                  WORD, fwCapability, LPWSTR, pOutput,
                                 CONST DEVMODEW *, pDevMode)
{
    LPDEVMODEA dmA = DEVMODEdupWtoA(GetProcessHeap(), pDevMode);
    LPSTR pDeviceA = HEAP_strdupWtoA(GetProcessHeap(),0,pDevice);
    LPSTR pPortA = HEAP_strdupWtoA(GetProcessHeap(),0,pPort);
    INT ret;

    if(pOutput && (fwCapability == DC_BINNAMES ||
           fwCapability == DC_FILEDEPENDENCIES ||
           fwCapability == DC_PAPERNAMES)) {
      /* These need A -> W translation */
        INT size = 0, i;
    LPSTR pOutputA;
        ret = DeviceCapabilitiesA(pDeviceA, pPortA, fwCapability, NULL,
                  dmA);
    if(ret == -1)
        return ret;
    switch(fwCapability) {
    case DC_BINNAMES:
        size = 24;
        break;
    case DC_PAPERNAMES:
    case DC_FILEDEPENDENCIES:
        size = 64;
        break;
    }
    pOutputA = (LPSTR)HeapAlloc(GetProcessHeap(), 0, size * ret);
    ret = DeviceCapabilitiesA(pDeviceA, pPortA, fwCapability, pOutputA,
                  dmA);
    for(i = 0; i < ret; i++)
        MultiByteToWideChar(CP_ACP, 0, pOutputA + (i * size), -1,
                pOutput + (i * size), size);
    HeapFree(GetProcessHeap(), 0, pOutputA);
    } else {
        ret = DeviceCapabilitiesA(pDeviceA, pPortA, fwCapability,
                  (LPSTR)pOutput, dmA);
    }
    HeapFree(GetProcessHeap(),0,pPortA);
    HeapFree(GetProcessHeap(),0,pDeviceA);
    HeapFree(GetProcessHeap(),0,dmA);
    return ret;
}


/*****************************************************************************
 * Name      : BOOL DeleteFormA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             LPSTR pFormName  pointer to form name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:28:10]
 *****************************************************************************/

ODINFUNCTION2(BOOL, DeleteFormA,
              HANDLE, hPrinter,
              LPSTR, pFormName)
{
  dprintf(("WINSPOOL: DeleteFormA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeleteFormW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             LPWSTR pFormName  pointer to form name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:28:10]
 *****************************************************************************/

ODINFUNCTION2(BOOL, DeleteFormW,
              HANDLE, hPrinter,
              LPWSTR, pFormName)
{
  dprintf(("WINSPOOL: DeleteFormW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeleteMonitorA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             LPSTR pEnvironment  pointer to environment string
 *             LPSTR pMonitorName  pointer to monitor name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:28:35]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeleteMonitorA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              LPSTR, pMonitorName)
{
  dprintf(("WINSPOOL: DeleteMonitorA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeleteMonitorW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             LPWSTR pEnvironment  pointer to environment string
 *             LPWSTR pMonitorName  pointer to monitor name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:28:35]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeleteMonitorW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              LPWSTR, pMonitorName)
{
  dprintf(("WINSPOOL: DeleteMonitorW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePortA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             HWND hWnd  handle to window that displays dialog box
 *             LPSTR pPortName  pointer to port name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:28:58]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeletePortA,
              LPSTR, pName,
              HWND, hWnd,
              LPSTR, pPortName)
{
  dprintf(("WINSPOOL: DeletePortA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePortW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             HWND hWnd  handle to window that displays dialog box
 *             LPWSTR pPortName  pointer to port name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:28:58]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeletePortW,
              LPWSTR, pName,
              HWND, hWnd,
              LPWSTR, pPortName)
{
  dprintf(("WINSPOOL: DeletePortW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrintProcessorA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             LPSTR pEnvironment  pointer to environment string
 *             LPSTR pPrintProcessorName  pointer to processor name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:30:08]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeletePrintProcessorA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              LPSTR, pPrintProcessorName)
{
  dprintf(("WINSPOOL: DeletePrintProcessorA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrintProcessorW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             LPWSTR pEnvironment  pointer to environment string
 *             LPWSTR pPrintProcessorName  pointer to processor name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:30:08]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeletePrintProcessorW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              LPWSTR, pPrintProcessorName)
{
  dprintf(("WINSPOOL: DeletePrintProcessorW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrintProvidorA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             LPSTR pEnvironment  pointer to environment string
 *             LPSTR pPrintProvidorName  pointer to provider name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:30:29]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeletePrintProvidorA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              LPSTR, pPrintProvidorName)
{
  dprintf(("WINSPOOL: DeletePrintProvidorA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrintProvidorW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             LPWSTR pEnvironment  pointer to environment string
 *             LPWSTR pPrintProvidorName  pointer to provider name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:30:29]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeletePrintProvidorW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              LPWSTR, pPrintProvidorName)
{
  dprintf(("WINSPOOL: DeletePrintProvidorW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrinter
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:30:50]
 *****************************************************************************/

ODINFUNCTION1(BOOL, DeletePrinter,
              HANDLE, hPrinter)
{
  dprintf(("WINSPOOL: DeletePrinter not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrinterConnectionA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to printer name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:31:48]
 *****************************************************************************/

ODINFUNCTION1(BOOL, DeletePrinterConnectionA,
              LPSTR, pName)
{
  dprintf(("WINSPOOL: DeletePrinterConnectionA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrinterConnectionW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to printer name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:31:48]
 *****************************************************************************/

ODINFUNCTION1(BOOL, DeletePrinterConnectionW,
              LPWSTR, pName)
{
  dprintf(("WINSPOOL: DeletePrinterConnectionW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrinterDriverA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             LPSTR pEnvironment  pointer to environment
 *             LPSTR pDriverName  pointer to driver name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:32:38]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeletePrinterDriverA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              LPSTR, pDriverName)
{
  dprintf(("WINSPOOL: DeletePrinterDriverA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL DeletePrinterDriverW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             LPWSTR pEnvironment  pointer to environment
 *             LPWSTR pDriverName  pointer to driver name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:32:38]
 *****************************************************************************/

ODINFUNCTION3(BOOL, DeletePrinterDriverW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              LPWSTR, pDriverName)
{
  dprintf(("WINSPOOL: DeletePrinterDriverW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EndDocPrinter
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:34:09]
 *****************************************************************************/

ODINFUNCTION1(BOOL, EndDocPrinter,
              HANDLE, hPrinter)
{
  dprintf(("WINSPOOL: EndDocPrinter not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EndPagePrinter
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:34:27]
 *****************************************************************************/

ODINFUNCTION1(BOOL, EndPagePrinter,
              HANDLE, hPrinter)
{
  dprintf(("WINSPOOL: EndPagePrinter not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumFormsA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             DWORD Level  data-structure level
 *             LPBYTE pForm  points to buffer that receives form info. structure array
 *             DWORD cbBuf  count of bytes in buffer
 *             LPDWORD pcbNeeded  points to variable to receive count of bytes copied or required
 *             LPDWORD pcReturned  points to variable to receive count of structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:35:07]
 *****************************************************************************/

ODINFUNCTION6(BOOL, EnumFormsA,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pForm,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumFormsA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumFormsW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             DWORD Level  data-structure level
 *             LPBYTE pForm  points to buffer that receives form info. structure array
 *             DWORD cbBuf  count of bytes in buffer
 *             LPDWORD pcbNeeded  points to variable to receive count of bytes copied or required
 *             LPDWORD pcReturned  points to variable to receive count of structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:35:07]
 *****************************************************************************/

ODINFUNCTION6(BOOL, EnumFormsW,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pForm,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumFormsW not implemented\n"));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumJobsA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             DWORD FirstJob  location of first job in print queue to enumerate
 *             DWORD NoJobs  number of jobs to enumerate
 *             DWORD Level  structure level
 *             LPBYTE pJob  pointer to structure array
 *             DWORD cbBuf  size of array, in bytes
 *             LPDWORD pcbNeeded  addr. of variable with no. of bytes copied (or required)
 *             LPDWORD pcReturned  addr. of variable with no. of job info. structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:36:24]
 *****************************************************************************/

ODINFUNCTION8(BOOL, EnumJobsA,
              HANDLE, hPrinter,
              DWORD, FirstJob,
              DWORD, NoJobs,
              DWORD, Level,
              LPBYTE, pJob,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumJobsA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumJobsW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             DWORD FirstJob  location of first job in print queue to enumerate
 *             DWORD NoJobs  number of jobs to enumerate
 *             DWORD Level  structure level
 *             LPBYTE pJob  pointer to structure array
 *             DWORD cbBuf  size of array, in bytes
 *             LPDWORD pcbNeeded  addr. of variable with no. of bytes copied (or required)
 *             LPDWORD pcReturned  addr. of variable with no. of job info. structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:36:24]
 *****************************************************************************/

ODINFUNCTION8(BOOL, EnumJobsW,
              HANDLE, hPrinter,
              DWORD, FirstJob,
              DWORD, NoJobs,
              DWORD, Level,
              LPBYTE, pJob,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumJobsW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumMonitorsA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             DWORD Level  structure level
 *             LPBYTE pMonitors  pointer to structure array
 *             DWORD cbBuf  size, in bytes, of buffer
 *             LPDWORD pcbNeeded  addr. of variable with no. of bytes copied (or required)
 *             LPDWORD pcReturned  addr. of variable with no. of job info. structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:38:50]
 *****************************************************************************/

ODINFUNCTION6(BOOL, EnumMonitorsA,
              LPSTR, pName,
              DWORD, Level,
              LPBYTE, pMonitors,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumMonitorsA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumMonitorsW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             DWORD Level  structure level
 *             LPBYTE pMonitors  pointer to structure array
 *             DWORD cbBuf  size, in bytes, of buffer
 *             LPDWORD pcbNeeded  addr. of variable with no. of bytes copied (or required)
 *             LPDWORD pcReturned  addr. of variable with no. of job info. structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:38:50]
 *****************************************************************************/

ODINFUNCTION6(BOOL, EnumMonitorsW,
              LPWSTR, pName,
              DWORD, Level,
              LPBYTE, pMonitors,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumMonitorsW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPortsA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             DWORD Level  specifies type of port info structure
 *             LPBYTE pPorts  pointer to buffer to receive array of port info. structures
 *             DWORD cbBuf  specifies size, in bytes, of buffer
 *             LPDWORD pcbNeeded  buffer size)
 *             LPDWORD pcReturned  pointer to number of PORT_INFO_*. structures stored into buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:39:09]
 *****************************************************************************/

ODINFUNCTION6(BOOL, EnumPortsA,
              LPSTR, pName,
              DWORD, Level,
              LPBYTE, pPorts,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPortsA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPortsW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             DWORD Level  specifies type of port info structure
 *             LPBYTE pPorts  pointer to buffer to receive array of port info. structures
 *             DWORD cbBuf  specifies size, in bytes, of buffer
 *             LPDWORD pcbNeeded  buffer size)
 *             LPDWORD pcReturned  pointer to number of PORT_INFO_*. structures stored into buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:39:09]
 *****************************************************************************/

ODINFUNCTION6(BOOL, EnumPortsW,
              LPWSTR, pName,
              DWORD, Level,
              LPBYTE, pPorts,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPortsW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPrintProcessorDatatypesA
 * Purpose   :
 * Parameters: LPSTR pName  points to server name string
 *             LPSTR pPrintProcessorName  points to print processor name string
 *             DWORD Level  specifies version of print processor data type structures
 *             LPBYTE pDatatypes  points to buffer to receive print processor data type structures
 *             DWORD cbBuf  specifies size, in bytes, of buffer
 *             LPDWORD pcbNeeded  points to number of bytes copied (or required)
 *             LPDWORD pcReturned  points to number of data structures obtained
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:39:29]
 *****************************************************************************/

ODINFUNCTION7(BOOL, EnumPrintProcessorDatatypesA,
              LPSTR, pName,
              LPSTR, pPrintProcessorName,
              DWORD, Level,
              LPBYTE, pDatatypes,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPrintProcessorDatatypesA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPrintProcessorDatatypesW
 * Purpose   :
 * Parameters: LPWSTR pName  points to server name string
 *             LPWSTR pPrintProcessorName  points to print processor name string
 *             DWORD Level  specifies version of print processor data type structures
 *             LPBYTE pDatatypes  points to buffer to receive print processor data type structures
 *             DWORD cbBuf  specifies size, in bytes, of buffer
 *             LPDWORD pcbNeeded  points to number of bytes copied (or required)
 *             LPDWORD pcReturned  points to number of data structures obtained
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:39:30]
 *****************************************************************************/

ODINFUNCTION7(BOOL, EnumPrintProcessorDatatypesW,
              LPWSTR, pName,
              LPWSTR, pPrintProcessorName,
              DWORD, Level,
              LPBYTE, pDatatypes,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPrintProcessorDatatypesW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPrintProcessorsA
 * Purpose   :
 * Parameters: LPSTR pName  points to server name
 *             LPSTR pEnvironment  points to environment name
 *             DWORD Level  structure level
 *             LPBYTE pPrintProcessorInfo  points to structure array
 *             DWORD cbBuf  array length in bytes
 *             LPDWORD pcbNeeded  points to number of bytes copied (or required)
 *             LPDWORD pcReturned  points to number of job info. structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:39:52]
 *****************************************************************************/

ODINFUNCTION7(BOOL, EnumPrintProcessorsA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pPrintProcessorInfo,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPrintProcessorsA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPrintProcessorsW
 * Purpose   :
 * Parameters: LPWSTR pName  points to server name
 *             LPWSTR pEnvironment  points to environment name
 *             DWORD Level  structure level
 *             LPBYTE pPrintProcessorInfo  points to structure array
 *             DWORD cbBuf  array length in bytes
 *             LPDWORD pcbNeeded  points to number of bytes copied (or required)
 *             LPDWORD pcReturned  points to number of job info. structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:39:52]
 *****************************************************************************/

ODINFUNCTION7(BOOL, EnumPrintProcessorsW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pPrintProcessorInfo,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPrintProcessorsW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPrinterDriversA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             LPSTR pEnvironment  pointer to environment name
 *             DWORD Level  structure level
 *             LPBYTE pDriverInfo  pointer to an array of structures
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  pointer to number of bytes copied (or required)
 *             LPDWORD pcReturned  pointer to number of DRIVER_INFO. structures
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:40:23]
 *****************************************************************************/

ODINFUNCTION7(BOOL, EnumPrinterDriversA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pDriverInfo,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPrinterDriversA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPrinterDriversW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             LPWSTR pEnvironment  pointer to environment name
 *             DWORD Level  structure level
 *             LPBYTE pDriverInfo  pointer to an array of structures
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  pointer to number of bytes copied (or required)
 *             LPDWORD pcReturned  pointer to number of DRIVER_INFO. structures
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:40:24]
 *****************************************************************************/

ODINFUNCTION7(BOOL, EnumPrinterDriversW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pDriverInfo,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPrinterDriversW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL EnumPrintersW
 * Purpose   :
 * Parameters: DWORD Flags  types of printer objects to enumerate
 *             LPWSTR Name  name of printer object
 *             DWORD Level  specifies type of printer info structure
 *             LPBYTE pPrinterEnum  pointer to buffer to receive printer info structures
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  pointer to variable with no. of bytes copied (or required)
 *             LPDWORD pcReturned  pointer to variable with no. of printer info. structures copied
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:40:55]
 *****************************************************************************/

ODINFUNCTION7(BOOL, EnumPrintersW,
              DWORD, Flags,
              LPWSTR, Name,
              DWORD, Level,
              LPBYTE, pPrinterEnum,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded,
              LPDWORD, pcReturned)
{
  dprintf(("WINSPOOL: EnumPrintersW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL FindClosePrinterChangeNotification
 * Purpose   :
 * Parameters: HANDLE hChange  handle to change notification object to close
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:41:51]
 *****************************************************************************/

ODINFUNCTION1(BOOL, FindClosePrinterChangeNotification,
              HANDLE, hChange)
{
  dprintf(("WINSPOOL: FindClosePrinterChangeNotification not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : HANDLE FindFirstPrinterChangeNotification
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer or print server to monitor for changes
 *             DWORD fdwFlags  flags that specify the conditions to monitor
 *             DWORD fdwOptions  reserved, must be zero
 *             LPVOID pPrinterNotifyOptions  pointer to structure specifying printer information to monitor
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:42:05]
 *****************************************************************************/

ODINFUNCTION4(HANDLE, FindFirstPrinterChangeNotification,
              HANDLE, hPrinter,
              DWORD, fdwFlags,
              DWORD, fdwOptions,
              LPVOID, pPrinterNotifyOptions)
{
  dprintf(("WINSPOOL: FindFirstPrinterChangeNotification not implemented\n"));
  return (NULL);
}


/*****************************************************************************
 * Name      : BOOL FindNextPrinterChangeNotification
 * Purpose   :
 * Parameters: HANDLE hChange  handle to change notification object of interest
 *             PDWORD pdwChange  pointer to a value that indicates the condition that changed
 *             LPVOID pPrinterNotifyOptions  pointer to a structure that specifies a refresh flag
 *             LPVOID *ppPrinterNotifyInfo  pointer to a pointer that receives printer information buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:42:20]
 *****************************************************************************/

ODINFUNCTION4(BOOL, FindNextPrinterChangeNotification,
              HANDLE, hChange,
              PDWORD, pdwChange,
              LPVOID, pPrinterNotifyOptions,
              LPPRINTER_NOTIFY_INFO, ppPrinterNotifyInfo)
{
  dprintf(("WINSPOOL: FindNextPrinterChangeNotification not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL FreePrinterNotifyInfo
 * Purpose   :
 * Parameters: PPRINTER_NOTIFY_INFO pPrinterNotifyInfo  pointer to a PRINTER_NOTIFY_INFO buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:43:02]
 *****************************************************************************/

ODINFUNCTION1(BOOL, FreePrinterNotifyInfo,
              LPPRINTER_NOTIFY_INFO, pPrinterNotifyInfo)
{
  dprintf(("WINSPOOL: FreePrinterNotifyInfo not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetFormA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer
 *             LPSTR pFormName  address of form name
 *             DWORD Level  structure level
 *             LPBYTE pForm  address of structure array
 *             DWORD cbBuf  count of bytes in array
 *             LPDWORD pcbNeeded  addr. of variable with count of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:43:32]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetFormA,
              HANDLE, hPrinter,
              LPSTR, pFormName,
              DWORD, Level,
              LPBYTE, pForm,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetFormA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetFormW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer
 *             LPWSTR pFormName  address of form name
 *             DWORD Level  structure level
 *             LPBYTE pForm  address of structure array
 *             DWORD cbBuf  count of bytes in array
 *             LPDWORD pcbNeeded  addr. of variable with count of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:43:32]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetFormW,
              HANDLE, hPrinter,
              LPWSTR, pFormName,
              DWORD, Level,
              LPBYTE, pForm,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetFormW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetJobA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer
 *             DWORD JobId  job identifier value
 *             DWORD Level  data-structure level
 *             LPBYTE pJob  address of data-structure array
 *             DWORD cbBuf  count of bytes in array
 *             LPDWORD pcbNeeded  address of value that contains count of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:43:50]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetJobA,
              HANDLE, hPrinter,
              DWORD, JobId,
              DWORD, Level,
              LPBYTE, pJob,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetJobA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetJobW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer
 *             DWORD JobId  job identifier value
 *             DWORD Level  data-structure level
 *             LPBYTE pJob  address of data-structure array
 *             DWORD cbBuf  count of bytes in array
 *             LPDWORD pcbNeeded  address of value that contains count of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:43:50]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetJobW,
              HANDLE, hPrinter,
              DWORD, JobId,
              DWORD, Level,
              LPBYTE, pJob,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetJobW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetPrintProcessorDirectoryA
 * Purpose   :
 * Parameters: LPSTR pName  address of server name
 *             LPSTR pEnvironment  address of environment
 *             DWORD Level  structure level
 *             LPBYTE pPrintProcessorInfo  address of structure array
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:45:14]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetPrintProcessorDirectoryA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pPrintProcessorInfo,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrintProcessorDirectoryA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetPrintProcessorDirectoryW
 * Purpose   :
 * Parameters: LPWSTR pName  address of server name
 *             LPWSTR pEnvironment  address of environment
 *             DWORD Level  structure level
 *             LPBYTE pPrintProcessorInfo  address of structure array
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:45:14]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetPrintProcessorDirectoryW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pPrintProcessorInfo,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrintProcessorDirectoryW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetPrinterA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer of interest
 *             DWORD Level  version of printer info data structure
 *             LPBYTE pPrinter  pointer to array of bytes that receives printer info. structure
 *             DWORD cbBuf  size, in bytes, of array of bytes
 *             LPDWORD pcbNeeded  pointer to variable with count of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:45:36]
 *****************************************************************************/

ODINFUNCTION5(BOOL, GetPrinterA,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pPrinter,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrinterA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetPrinterW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer of interest
 *             DWORD Level  version of printer info data structure
 *             LPBYTE pPrinter  pointer to array of bytes that receives printer info. structure
 *             DWORD cbBuf  size, in bytes, of array of bytes
 *             LPDWORD pcbNeeded  pointer to variable with count of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:45:36]
 *****************************************************************************/

ODINFUNCTION5(BOOL, GetPrinterW,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pPrinter,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrinterW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD GetPrinterDataA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             LPSTR pValueName  address of data type
 *             LPDWORD pType  reserved
 *             LPBYTE pData  address of array of bytes that receives data
 *             DWORD nSize  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:46:35]
 *****************************************************************************/

ODINFUNCTION6(DWORD, GetPrinterDataA,
              HANDLE, hPrinter,
              LPSTR, pValueName,
              LPDWORD, pType,
              LPBYTE, pData,
              DWORD, nSize,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrinterDataA not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : DWORD GetPrinterDataW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             LPWSTR pValueName  address of data type
 *             LPDWORD pType  reserved
 *             LPBYTE pData  address of array of bytes that receives data
 *             DWORD nSize  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:46:35]
 *****************************************************************************/

ODINFUNCTION6(DWORD, GetPrinterDataW,
              HANDLE, hPrinter,
              LPWSTR, pValueName,
              LPDWORD, pType,
              LPBYTE, pData,
              DWORD, nSize,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrinterDataW not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : BOOL GetPrinterDriverA
 * Purpose   :
 * Parameters: HANDLE hPrinter  printer object
 *             LPSTR pEnvironment  address of environment
 *             DWORD Level  structure level
 *             LPBYTE pDriverInfo  address of structure array
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:47:01]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetPrinterDriverA,
              HANDLE, hPrinter,
              LPSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pDriverInfo,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrinterDriverA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetPrinterDriverW
 * Purpose   :
 * Parameters: HANDLE hPrinter  printer object
 *             LPWSTR pEnvironment  address of environment
 *             DWORD Level  structure level
 *             LPBYTE pDriverInfo  address of structure array
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:47:01]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetPrinterDriverW,
              HANDLE, hPrinter,
              LPWSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pDriverInfo,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrinterDriverW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetPrinterDriverDirectoryA
 * Purpose   :
 * Parameters: LPSTR pName  address of server name
 *             LPSTR pEnvironment  address of environment
 *             DWORD Level  address of structure
 *             LPBYTE pDriverDirectory  address of structure array that receives path
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:47:16]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetPrinterDriverDirectoryA,
              LPSTR, pName,
              LPSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pDriverDirectory,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrinterDriverDirectoryA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetPrinterDriverDirectoryW
 * Purpose   :
 * Parameters: LPWSTR pName  address of server name
 *             LPWSTR pEnvironment  address of environment
 *             DWORD Level  address of structure
 *             LPBYTE pDriverDirectory  address of structure array that receives path
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:47:16]
 *****************************************************************************/

ODINFUNCTION6(BOOL, GetPrinterDriverDirectoryW,
              LPWSTR, pName,
              LPWSTR, pEnvironment,
              DWORD, Level,
              LPBYTE, pDriverDirectory,
              DWORD, cbBuf,
              LPDWORD, pcbNeeded)
{
  dprintf(("WINSPOOL: GetPrinterDriverDirectoryW not implemented\n"));
  return (FALSE);
}



/*****************************************************************************
 * Name      : BOOL OpenPrinterW
 * Purpose   :
 * Parameters: LPWSTR pPrinterName  pointer to printer or server name
 *             HANDLE * phPrinter  pointer to printer or server handle
 *             PRINTER_DEFAULTSW * pDefault  pointer to printer defaults structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:48:19]
 *****************************************************************************/

ODINFUNCTION3(BOOL, OpenPrinterW,
              LPWSTR, pPrinterName,
              HANDLE *, phPrinter,
              PRINTER_DEFAULTSW *, pDefault)
{
  dprintf(("WINSPOOL: OpenPrinterW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : HPRINTER PrinterMessageBoxA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer
 *             DWORD Error  error identifier
 *             HWND hWnd  handle of owner window
 *             LPSTR pText  pointer to message box text
 *             LPSTR pCaption  pointer to message box title
 *             DWORD dwType  style of message box
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:50:13]
 *****************************************************************************/

ODINFUNCTION6(HPRINTER, PrinterMessageBoxA,
              HANDLE, hPrinter,
              DWORD, Error,
              HWND, hWnd,
              LPSTR, pText,
              LPSTR, pCaption,
              DWORD, dwType)
{
  dprintf(("WINSPOOL: PrinterMessageBoxA not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : HPRINTER PrinterMessageBoxW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer
 *             DWORD Error  error identifier
 *             HWND hWnd  handle of owner window
 *             LPWSTR pText  pointer to message box text
 *             LPWSTR pCaption  pointer to message box title
 *             DWORD dwType  style of message box
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:50:13]
 *****************************************************************************/

ODINFUNCTION6(HPRINTER, PrinterMessageBoxW,
              HANDLE, hPrinter,
              DWORD, Error,
              HWND, hWnd,
              LPWSTR, pText,
              LPWSTR, pCaption,
              DWORD, dwType)
{
  dprintf(("WINSPOOL: PrinterMessageBoxW not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : BOOL PrinterProperties
 * Purpose   :
 * Parameters: HWND hWnd  handle of parent window
 *             HANDLE hPrinter  handle of printer object
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:50:49]
 *****************************************************************************/

ODINFUNCTION2(BOOL, PrinterProperties,
              HWND, hWnd,
              HANDLE, hPrinter)
{
  dprintf(("WINSPOOL: PrinterProperties not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL ReadPrinter
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             LPVOID pBuf  address of array of bytes that receives data
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pNoBytesRead  address of variable with number of bytes retrieved
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:51:17]
 *****************************************************************************/
ODINFUNCTION4(BOOL, ReadPrinter,
              HANDLE, hPrinter,
              LPVOID, pBuf,
              DWORD, cbBuf,
              LPDWORD, pNoBytesRead)
{
  dprintf(("WINSPOOL: ReadPrinter not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL ResetPrinterA
 * Purpose   :
 * Parameters: HANDLE hPrinter  printer handle
 *             PRINTER_DEFAULTSA * pDefault  address of printer defaults structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:51:52]
 *****************************************************************************/

ODINFUNCTION2(BOOL, ResetPrinterA,
              HANDLE, hPrinter,
              PRINTER_DEFAULTSA *, pDefault)
{
  dprintf(("WINSPOOL: ResetPrinterA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL ResetPrinterW
 * Purpose   :
 * Parameters: HANDLE hPrinter  printer handle
 *             PRINTER_DEFAULTSW * pDefault  address of printer defaults structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:51:52]
 *****************************************************************************/

ODINFUNCTION2(BOOL, ResetPrinterW,
              HANDLE, hPrinter,
              PRINTER_DEFAULTSW *, pDefault)
{
  dprintf(("WINSPOOL: ResetPrinterW not implemented\n"));
  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL ScheduleJob
 * Purpose   :
 * Parameters: HANDLE hPrinter  specifies printer for the print job
 *             DWORD dwJobID  print job identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:52:39]
 *****************************************************************************/

ODINFUNCTION2(BOOL, ScheduleJob,
              HANDLE, hPrinter,
              DWORD, dwJobID)
{
  dprintf(("WINSPOOL: ScheduleJob not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetFormA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             LPSTR pFormName  address of form name
 *             DWORD Level  structure level
 *             LPBYTE pForm  address of structure array
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:53:20]
 *****************************************************************************/

ODINFUNCTION4(BOOL, SetFormA,
              HANDLE, hPrinter,
              LPSTR, pFormName,
              DWORD, Level,
              LPBYTE, pForm)
{
  dprintf(("WINSPOOL: SetFormA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetFormW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             LPWSTR pFormName  address of form name
 *             DWORD Level  structure level
 *             LPBYTE pForm  address of structure array
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:53:20]
 *****************************************************************************/

ODINFUNCTION4(BOOL, SetFormW,
              HANDLE, hPrinter,
              LPWSTR, pFormName,
              DWORD, Level,
              LPBYTE, pForm)
{
  dprintf(("WINSPOOL: SetFormW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetJobA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             DWORD JobId  print job identifier
 *             DWORD Level  specifies type of job information structure
 *             LPBYTE  pJob  pointer to job information structure
 *             DWORD Command  job command value
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:53:47]
 *****************************************************************************/

ODINFUNCTION5(BOOL, SetJobA,
              HANDLE, hPrinter,
              DWORD, JobId,
              DWORD, Level,
              LPBYTE, pJob,
              DWORD, Command)
{
  dprintf(("WINSPOOL: SetJobA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetJobW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             DWORD JobId  print job identifier
 *             DWORD Level  specifies type of job information structure
 *             LPBYTE  pJob  pointer to job information structure
 *             DWORD Command  job command value
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:53:47]
 *****************************************************************************/

ODINFUNCTION5(BOOL, SetJobW,
              HANDLE, hPrinter,
              DWORD, JobId,
              DWORD, Level,
              LPBYTE, pJob,
              DWORD, Command)
{
  dprintf(("WINSPOOL: SetJobW not implemented\n"));
  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL SetPrinterA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             DWORD Level  structure level
 *             LPBYTE pPrinter  address of array containing printer data
 *             DWORD Command  printer-state command
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:55:03]
 *****************************************************************************/

ODINFUNCTION4(BOOL, SetPrinterA,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pPrinter,
              DWORD, Command)
{
  dprintf(("WINSPOOL: SetPrinterA not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetPrinterW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             DWORD Level  structure level
 *             LPBYTE pPrinter  address of array containing printer data
 *             DWORD Command  printer-state command
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:55:03]
 *****************************************************************************/

ODINFUNCTION4(BOOL, SetPrinterW,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pPrinter,
              DWORD, Command)
{
  dprintf(("WINSPOOL: SetPrinterW not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD SetPrinterDataA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             LPSTR pValueName  address of data type
 *             DWORD Type  flag for value type
 *             LPBYTE pData  address of array that specifies printer data
 *             DWORD cbData  size, in bytes, of array
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:55:52]
 *****************************************************************************/

ODINFUNCTION5(DWORD, SetPrinterDataA,
              HANDLE, hPrinter,
              LPSTR, pValueName,
              DWORD, Type,
              LPBYTE, pData,
              DWORD, cbData)
{
  dprintf(("WINSPOOL: SetPrinterDataA not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : DWORD SetPrinterDataW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             LPWSTR pValueName  address of data type
 *             DWORD Type  flag for value type
 *             LPBYTE pData  address of array that specifies printer data
 *             DWORD cbData  size, in bytes, of array
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:55:52]
 *****************************************************************************/

ODINFUNCTION5(DWORD, SetPrinterDataW,
              HANDLE, hPrinter,
              LPWSTR, pValueName,
              DWORD, Type,
              LPBYTE, pData,
              DWORD, cbData)
{
  dprintf(("WINSPOOL: SetPrinterDataW not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : DWORD StartDocPrinterA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             DWORD Level  structure level
 *             LPBYTE pDocInfo  address of structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:56:45]
 *****************************************************************************/

ODINFUNCTION3(DWORD, StartDocPrinterA,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pDocInfo)
{
  dprintf(("WINSPOOL: StartDocPrinterA not implemented\n"));
  return (0);
}


/*****************************************************************************
 * Name      : DWORD StartDocPrinterW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 *             DWORD Level  structure level
 *             LPBYTE pDocInfo  address of structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:56:45]
 *****************************************************************************/

ODINFUNCTION3(DWORD, StartDocPrinterW,
              HANDLE, hPrinter,
              DWORD, Level,
              LPBYTE, pDocInfo)
{
  dprintf(("WINSPOOL: StartDocPrinterW not implemented\n"));
  return (0);
}

/*****************************************************************************
 * Name      : BOOL StartPagePrinter
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle of printer object
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:57:38]
 *****************************************************************************/

ODINFUNCTION1(BOOL, StartPagePrinter,
              HANDLE, hPrinter)
{
  dprintf(("WINSPOOL: StartPagePrinter not implemented\n"));
  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WritePrinter
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 *             LPVOID pBuf  pointer to array that contains printer data
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcWritten  addr. of variable with count of bytes written
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:58:49]
 *****************************************************************************/

ODINFUNCTION4(BOOL, WritePrinter,
              HANDLE, hPrinter,
              LPVOID, pBuf,
              DWORD, cbBuf,
              LPDWORD, pcWritten)
{
  dprintf(("WINSPOOL: WritePrinter not implemented\n"));
  return (FALSE);
}


/*********************************************************************
 *                  DeletePrinterDataA   (WINSPOOL.152)
 */
ODINFUNCTION2(DWORD, DeletePrinterDataA,
          HANDLE, hPrinter,
          LPSTR, pValueName)
{
  dprintf(("WINSPOOL: DeletePrinterDataA not implemented\n"));
  return (FALSE);
}


/*********************************************************************
 *                  DeletePrinterDataW   (WINSPOOL.153)
 */
ODINFUNCTION2(DWORD, DeletePrinterDataW,
          HANDLE, hPrinter,
          LPWSTR, pValueName)
{
  dprintf(("WINSPOOL: DeletePrinterDataW not implemented\n"));
  return (FALSE);
}


/*********************************************************************
 *                  DocumentPropertiesW   (WINSPOOL.166)
 */
ODINFUNCTION6(LONG, DocumentPropertiesW,
          HWND,      hWnd,
          HANDLE,    hPrinter,
          LPWSTR,    pDeviceName,
          PDEVMODEW, pDevModeOutput,
          PDEVMODEW, pDevModeInput,
          DWORD,     fMode)
{
  dprintf(("WINSPOOL: DocumentPropertiesW not implemented\n"));
  return (FALSE);
}


/*********************************************************************
 *                  EnumPrinterDataA   (WINSPOOL.183)
 */
ODINFUNCTION9(DWORD, EnumPrinterDataA,
    HANDLE,   hPrinter,
    DWORD,    dwIndex,
    LPSTR,    pValueName,
    DWORD,    cbValueName,
    LPDWORD,  pcbValueName,
    LPDWORD,  pType,
    LPBYTE,   pData,
    DWORD,    cbData,
    LPDWORD,  pcbData)
{
  dprintf(("WINSPOOL: EnumPrinterDataA not implemented\n"));
  return (FALSE);
}


/*********************************************************************
 *                  EnumPrinterDataW   (WINSPOOL.184)
 */
ODINFUNCTION9(DWORD, EnumPrinterDataW,
    HANDLE,   hPrinter,
    DWORD,    dwIndex,
    LPWSTR,   pValueName,
    DWORD,    cbValueName,
    LPDWORD,  pcbValueName,
    LPDWORD,  pType,
    LPBYTE,   pData,
    DWORD,    cbData,
    LPDWORD,  pcbData)
{
  dprintf(("WINSPOOL: EnumPrinterDataW not implemented\n"));
  return (FALSE);
}


/*********************************************************************
 *                  SetPortA   (WINSPOOL.226)
 */
ODINFUNCTION4(BOOL, SetPortA,
    LPSTR,     pName,
    LPSTR,     pPortName,
    DWORD,     dwLevel,
    LPBYTE,    pPortInfo)
{
  dprintf(("WINSPOOL: SetPortA not implemented\n"));
  return (FALSE);
}


/*********************************************************************
 *                  SetPortW   (WINSPOOL.227)
 */
ODINFUNCTION4(BOOL, SetPortW,
    LPWSTR,     pName,
    LPWSTR,     pPortName,
    DWORD,      dwLevel,
    LPBYTE,     pPortInfo)
{
  dprintf(("WINSPOOL: SetPortW not implemented\n"));
  return (FALSE);
}
