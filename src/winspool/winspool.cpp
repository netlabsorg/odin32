/* $Id: winspool.cpp,v 1.2 1999-06-10 16:19:02 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * WINSPOOL stubs
 *
 * Copyright 1998 Patrick Haller
 *
 */
#include <os2win.h>
#include <winspool.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"


/******************************************************************************/
//DWORD dwType;	/* printer type or types	*/
//LPSTR lpszName;	/* name of printer object	*/
//DWORD dwLevel;	/* structure level	*/
//LPBYTE lpbPrinters;	/* printer info array	*/
//DWORD cbBuf;	/* address of structure count of bytes in array	*/
//LPDWORD lpdwNeeded;	/* addr. of variable with no. of bytes copied (or required)	*/
//LPDWORD lpdwReturned;	/* addr. of variable with no. of printer info. structures copied	*/
/******************************************************************************/
BOOL WIN32API EnumPrintersA(DWORD dwType, LPTSTR lpszName, DWORD dwLevel, 
			       LPBYTE lpbPrinters, DWORD cbBuf, 
			       LPDWORD lpdwNeeded, LPDWORD lpdwReturned)
{
   dprintf(("EnumPrintersA\n"));

   return(O32_EnumPrinters(dwType, lpszName, dwLevel, lpbPrinters, cbBuf,
	  	           lpdwNeeded, lpdwReturned));
}
/******************************************************************************/
/******************************************************************************/
BOOL WIN32API ClosePrinter(HANDLE hPrinter)
{
  dprintf(("ClosePrinter, not implemented\n"));
  return(TRUE);
}
/******************************************************************************/
/******************************************************************************/
BOOL WIN32API OpenPrinterA(LPSTR pPrinterName, HANDLE * phPrinter, PRINTER_DEFAULTSA *pDefault)
{
  dprintf(("OpenPrinterA, not implemented\n"));
  return(FALSE);
}
/******************************************************************************/
//HWND hwnd;	/* handle of window that displays dialog box	*/
//HANDLE hPrinter;	/* handle of printer object	*/
//LPSTR pDeviceName;	/* address of device name	*/
//LPDEVMODEA pdmOutput;	/* address of modified device mode structure	*/
//LPDEVMODEA pdmInput;	/* address of original device mode structure	*/
//DWORD fMode;	/* mode flag	*/
/******************************************************************************/
LONG WIN32API DocumentPropertiesA(HWND hwnd, HANDLE hPrinter, LPSTR lpszDeviceName, 
				     LPDEVMODEA pdmOutput, LPDEVMODEA pdmInput, DWORD fMode)
{
  dprintf(("DocumentPropertiesA, not implemented\n"));
  return(-1);
}
/******************************************************************************/
/******************************************************************************/

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
BOOL WIN32API AbortPrinter(
                              HANDLE hPrinter
                              )

{

  dprintf(("WINSPOOL: AbortPrinter(%u) not implemented\n"
           ,hPrinter
         ));

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
BOOL WIN32API AddFormA(
                         HANDLE hPrinter,
                         DWORD Level,
                         LPBYTE pForm
                         )

{

  dprintf(("WINSPOOL: AddFormA(%u, %d, %08x) not implemented\n"
           ,hPrinter, Level, pForm
         ));

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
BOOL WIN32API AddFormW(
                         HANDLE hPrinter,
                         DWORD Level,
                         LPBYTE pForm
                         )

{

  dprintf(("WINSPOOL: AddFormW(%u, %d, %08x) not implemented\n"
           ,hPrinter, Level, pForm
         ));

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
BOOL WIN32API AddJobA(
                        HANDLE hPrinter,
                        DWORD Level,
                        LPBYTE pData,
                        DWORD cbBuf,
                        LPDWORD pcbNeeded
                        )

{

  dprintf(("WINSPOOL: AddJobA(%u, %d, %08x, %d, %08x) not implemented\n"
           ,hPrinter, Level, pData, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API AddJobW(
                        HANDLE hPrinter,
                        DWORD Level,
                        LPBYTE pData,
                        DWORD cbBuf,
                        LPDWORD pcbNeeded
                        )

{

  dprintf(("WINSPOOL: AddJobW(%u, %d, %08x, %d, %08x) not implemented\n"
           ,hPrinter, Level, pData, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API AddMonitorA(
                             LPSTR pName,
                             DWORD Level,
                             LPBYTE pMonitors
                             )

{

  dprintf(("WINSPOOL: AddMonitorA(%s, %d, %08x) not implemented\n"
           ,pName, Level, pMonitors
         ));

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
BOOL WIN32API AddMonitorW(
                             LPWSTR pName,
                             DWORD Level,
                             LPBYTE pMonitors
                             )

{

  dprintf(("WINSPOOL: AddMonitorW(%s, %d, %08x)not implemented\n"
           ,pName, Level, pMonitors
         ));

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
BOOL WIN32API AddPortA(
                          LPSTR pName,
                          HWND hWnd,
                          LPSTR pMonitorName
                          )

{

  dprintf(("WINSPOOL: AddPortA(%s, %d, %s) not implemented\n"
           ,pName, hWnd, pMonitorName
         ));

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
BOOL WIN32API AddPortW(
                          LPWSTR pName,
                          HWND hWnd,
                          LPWSTR pMonitorName
                          )

{

  dprintf(("WINSPOOL: AddPortW(%s, %d, %s)not implemented\n"
           ,pName, hWnd, pMonitorName
         ));

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
BOOL WIN32API AddPrintProcessorA(
                                    LPSTR pName,
                                    LPSTR pEnvironment,
                                    LPSTR pPathName,
                                    LPSTR pPrintProcessorName
                                    )

{

  dprintf(("WINSPOOL: AddPrintProcessorA(%s, %s, %s, %s) not implemented\n"
           ,pName, pEnvironment, pPathName, pPrintProcessorName
         ));

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
BOOL WIN32API AddPrintProcessorW(
                                    LPWSTR pName,
                                    LPWSTR pEnvironment,
                                    LPWSTR pPathName,
                                    LPWSTR pPrintProcessorName
                                    )

{

  dprintf(("WINSPOOL: AddPrintProcessorW(%s, %s, %s, %s)not implemented\n"
           ,pName, pEnvironment, pPathName, pPrintProcessorName
         ));

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
BOOL WIN32API AddPrintProvidorA(
                                   LPSTR pName,
                                   DWORD Level,
                                   LPBYTE pProvidorInfo
                                   )

{

  dprintf(("WINSPOOL: AddPrintProvidorA(%s, %d, %08x) not implemented\n"
           ,pName, Level, pProvidorInfo
         ));

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
BOOL WIN32API AddPrintProvidorW(
                                   LPWSTR pName,
                                   DWORD Level,
                                   LPBYTE pProvidorInfo
                                   )

{

  dprintf(("WINSPOOL: AddPrintProvidorW(%s, %d, %08x)not implemented\n"
           ,pName, Level, pProvidorInfo
         ));

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
HANDLE WIN32API AddPrinterA(
                               LPSTR pName,
                               DWORD Level,
                               LPBYTE pPrinter
                               )

{

  dprintf(("WINSPOOL: AddPrinterA(%s, %d, %08x) not implemented\n"
           ,pName, Level, pPrinter
         ));

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
HANDLE WIN32API AddPrinterW(
                               LPWSTR pName,
                               DWORD Level,
                               LPBYTE pPrinter
                               )

{

  dprintf(("WINSPOOL: AddPrinterW(%s, %d, %08x)not implemented\n"
           ,pName, Level, pPrinter
         ));

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
BOOL WIN32API AddPrinterConnectionA(
                                       LPSTR pName
                                       )

{

  dprintf(("WINSPOOL: AddPrinterConnectionA(%s) not implemented\n"
           ,pName
         ));

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
BOOL WIN32API AddPrinterConnectionW(
                                       LPWSTR pName
                                       )

{

  dprintf(("WINSPOOL: AddPrinterConnectionW(%s)not implemented\n"
           ,pName
         ));

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
BOOL WIN32API AddPrinterDriverA(
                                   LPSTR pName,
                                   DWORD Level,
                                   LPBYTE pDriverInfo
                                   )

{

  dprintf(("WINSPOOL: AddPrinterDriverA(%s, %d, %08x) not implemented\n"
           ,pName, Level, pDriverInfo
         ));

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
BOOL WIN32API AddPrinterDriverW(
                                   LPWSTR pName,
                                   DWORD Level,
                                   LPBYTE pDriverInfo
                                   )

{

  dprintf(("WINSPOOL: AddPrinterDriverW(%s, %d, %08x)not implemented\n"
           ,pName, Level, pDriverInfo
         ));

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
LONG WIN32API AdvancedDocumentPropertiesA(
                                             HWND hWnd,
                                             HANDLE hPrinter,
                                             LPSTR pDeviceName,
                                             LPDEVMODEA pDevModeOutput,
                                             LPDEVMODEA pDevModeInput
                                             )

{

  dprintf(("WINSPOOL: AdvancedDocumentPropertiesA(%d, %u, %s, %08x, %08x) not implemented\n"
           ,hWnd, hPrinter, pDeviceName, pDevModeOutput, pDevModeInput
         ));

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
LONG WIN32API AdvancedDocumentPropertiesW(
                                             HWND hWnd,
                                             HANDLE hPrinter,
                                             LPWSTR pDeviceName,
                                             LPDEVMODEW pDevModeOutput,
                                             LPDEVMODEW pDevModeInput
                                             )

{

  dprintf(("WINSPOOL: AdvancedDocumentPropertiesW(%d, %u, %s, %08x, %08x)not implemented\n"
           ,hWnd, hPrinter, pDeviceName, pDevModeOutput, pDevModeInput
         ));

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
BOOL WIN32API ConfigurePortA(
                                LPSTR pName,
                                HWND hWnd,
                                LPSTR pPortName
                                )

{

  dprintf(("WINSPOOL: ConfigurePortA(%s, %d, %s) not implemented\n"
           ,pName, hWnd, pPortName
         ));

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
BOOL WIN32API ConfigurePortW(
                                LPWSTR pName,
                                HWND hWnd,
                                LPWSTR pPortName
                                )

{

  dprintf(("WINSPOOL: ConfigurePortW(%s, %d, %s)not implemented\n"
           ,pName, hWnd, pPortName
         ));

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
HANDLE WIN32API ConnectToPrinterDlg(
                                       HWND hwnd,
                                       DWORD Flags
                                       )

{

  dprintf(("WINSPOOL: ConnectToPrinterDlg(%d, %d) not implemented\n"
           ,hwnd, Flags
         ));

  return (NULL);
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
INT WIN32API DeviceCapabilitiesA(
                                      LPCSTR pDevice,
                                      LPCSTR pPort,
                                      WORD fwCapability,
                                      LPSTR pOutput,
                                      DEVMODEA *pDevMode
                                      )

{

  dprintf(("WINSPOOL: DeviceCapabilitiesA(%08x, %08x, %u, %s, %08x) not implemented\n"
           ,pDevice, pPort, fwCapability, pOutput, *pDevMode
         ));

  return (0);
}

 /*****************************************************************************
 * Name      : DWORD DeviceCapabilitiesW
 * Purpose   : 
 * Parameters: LPCWSTR pDevice  pointer to a printer-name string
 *             LPCWSTR pPort  pointer to a port-name string
 *             WORD fwCapability  device capability to query
 *             LPWSTR pOutput  pointer to the output
 *             CONST DEVMODEW *pDevMode  pointer to structure with device data
 * Variables : 
 * Result    : 
 * Remark    : 
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:27:08]
 *****************************************************************************/
INT WIN32API DeviceCapabilitiesW(
                                      LPCWSTR pDevice,
                                      LPCWSTR pPort,
                                      WORD fwCapability,
                                      LPWSTR pOutput,
                                      CONST DEVMODEW *pDevMode
                                      )

{

  dprintf(("WINSPOOL: DeviceCapabilitiesW(%08x, %08x, %u, %s, %08x)not implemented\n"
           ,pDevice, pPort, fwCapability, pOutput, *pDevMode
         ));

  return (0);
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
BOOL WIN32API DeleteFormA(
                             HANDLE hPrinter,
                             LPSTR pFormName
                             )

{

  dprintf(("WINSPOOL: DeleteFormA(%u, %s) not implemented\n"
           ,hPrinter, pFormName
         ));

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
BOOL WIN32API DeleteFormW(
                             HANDLE hPrinter,
                             LPWSTR pFormName
                             )

{

  dprintf(("WINSPOOL: DeleteFormW(%u, %s)not implemented\n"
           ,hPrinter, pFormName
         ));

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
BOOL WIN32API DeleteMonitorA(
                                LPSTR pName,
                                LPSTR pEnvironment,
                                LPSTR pMonitorName
                                )

{

  dprintf(("WINSPOOL: DeleteMonitorA(%s, %s, %s) not implemented\n"
           ,pName, pEnvironment, pMonitorName
         ));

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
BOOL WIN32API DeleteMonitorW(
                                LPWSTR pName,
                                LPWSTR pEnvironment,
                                LPWSTR pMonitorName
                                )

{

  dprintf(("WINSPOOL: DeleteMonitorW(%s, %s, %s)not implemented\n"
           ,pName, pEnvironment, pMonitorName
         ));

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
BOOL WIN32API DeletePortA(
                             LPSTR pName,
                             HWND hWnd,
                             LPSTR pPortName
                             )

{

  dprintf(("WINSPOOL: DeletePortA(%s, %d, %s) not implemented\n"
           ,pName, hWnd, pPortName
         ));

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
BOOL WIN32API DeletePortW(
                             LPWSTR pName,
                             HWND hWnd,
                             LPWSTR pPortName
                             )

{

  dprintf(("WINSPOOL: DeletePortW(%s, %d, %s)not implemented\n"
           ,pName, hWnd, pPortName
         ));

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
BOOL WIN32API DeletePrintProcessorA(
                                       LPSTR pName,
                                       LPSTR pEnvironment,
                                       LPSTR pPrintProcessorName
                                       )

{

  dprintf(("WINSPOOL: DeletePrintProcessorA(%s, %s, %s) not implemented\n"
           ,pName, pEnvironment, pPrintProcessorName
         ));

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
BOOL WIN32API DeletePrintProcessorW(
                                       LPWSTR pName,
                                       LPWSTR pEnvironment,
                                       LPWSTR pPrintProcessorName
                                       )

{

  dprintf(("WINSPOOL: DeletePrintProcessorW(%s, %s, %s)not implemented\n"
           ,pName, pEnvironment, pPrintProcessorName
         ));

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
BOOL WIN32API DeletePrintProvidorA(
                                      LPSTR pName,
                                      LPSTR pEnvironment,
                                      LPSTR pPrintProvidorName
                                      )

{

  dprintf(("WINSPOOL: DeletePrintProvidorA(%s, %s, %s) not implemented\n"
           ,pName, pEnvironment, pPrintProvidorName
         ));

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
BOOL WIN32API DeletePrintProvidorW(
                                      LPWSTR pName,
                                      LPWSTR pEnvironment,
                                      LPWSTR pPrintProvidorName
                                      )

{

  dprintf(("WINSPOOL: DeletePrintProvidorW(%s, %s, %s)not implemented\n"
           ,pName, pEnvironment, pPrintProvidorName
         ));

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
BOOL WIN32API DeletePrinter(
                               HANDLE hPrinter
                               )

{

  dprintf(("WINSPOOL: DeletePrinter(%u) not implemented\n"
           ,hPrinter
         ));

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
BOOL WIN32API DeletePrinterConnectionA(
                                          LPSTR pName
                                          )

{

  dprintf(("WINSPOOL: DeletePrinterConnectionA(%s) not implemented\n"
           ,pName
         ));

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
BOOL WIN32API DeletePrinterConnectionW(
                                          LPWSTR pName
                                          )

{

  dprintf(("WINSPOOL: DeletePrinterConnectionW(%s)not implemented\n"
           ,pName
         ));

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
BOOL WIN32API DeletePrinterDriverA(
                                      LPSTR pName,
                                      LPSTR pEnvironment,
                                      LPSTR pDriverName
                                      )

{

  dprintf(("WINSPOOL: DeletePrinterDriverA(%s, %s, %s) not implemented\n"
           ,pName, pEnvironment, pDriverName
         ));

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
BOOL WIN32API DeletePrinterDriverW(
                                      LPWSTR pName,
                                      LPWSTR pEnvironment,
                                      LPWSTR pDriverName
                                      )

{

  dprintf(("WINSPOOL: DeletePrinterDriverW(%s, %s, %s)not implemented\n"
           ,pName, pEnvironment, pDriverName
         ));

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
BOOL WIN32API EndDocPrinter(
                               HANDLE hPrinter
                               )

{

  dprintf(("WINSPOOL: EndDocPrinter(%u) not implemented\n"
           ,hPrinter
         ));

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
BOOL WIN32API EndPagePrinter(
                                HANDLE hPrinter
                                )

{

  dprintf(("WINSPOOL: EndPagePrinter(%u) not implemented\n"
           ,hPrinter
         ));

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
BOOL WIN32API EnumFormsA(
                           HANDLE hPrinter,
                           DWORD Level,
                           LPBYTE pForm,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded,
                           LPDWORD pcReturned
                           )

{

  dprintf(("WINSPOOL: EnumFormsA(%u, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,hPrinter, Level, pForm, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumFormsW(
                           HANDLE hPrinter,
                           DWORD Level,
                           LPBYTE pForm,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded,
                           LPDWORD pcReturned
                           )

{

  dprintf(("WINSPOOL: EnumFormsW(%u, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,hPrinter, Level, pForm, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumJobsA(
                          HANDLE hPrinter,
                          DWORD FirstJob,
                          DWORD NoJobs,
                          DWORD Level,
                          LPBYTE pJob,
                          DWORD cbBuf,
                          LPDWORD pcbNeeded,
                          LPDWORD pcReturned
                          )

{

  dprintf(("WINSPOOL: EnumJobsA(%u, %d, %d, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,hPrinter, FirstJob, NoJobs, Level, pJob, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumJobsW(
                          HANDLE hPrinter,
                          DWORD FirstJob,
                          DWORD NoJobs,
                          DWORD Level,
                          LPBYTE pJob,
                          DWORD cbBuf,
                          LPDWORD pcbNeeded,
                          LPDWORD pcReturned
                          )

{

  dprintf(("WINSPOOL: EnumJobsW(%u, %d, %d, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,hPrinter, FirstJob, NoJobs, Level, pJob, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumMonitorsA(
                               LPSTR pName,
                               DWORD Level,
                               LPBYTE pMonitors,
                               DWORD cbBuf,
                               LPDWORD pcbNeeded,
                               LPDWORD pcReturned
                               )

{

  dprintf(("WINSPOOL: EnumMonitorsA(%s, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,pName, Level, pMonitors, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumMonitorsW(
                               LPWSTR pName,
                               DWORD Level,
                               LPBYTE pMonitors,
                               DWORD cbBuf,
                               LPDWORD pcbNeeded,
                               LPDWORD pcReturned
                               )

{

  dprintf(("WINSPOOL: EnumMonitorsW(%s, %d, %08x, %d, %08x, %08x)not implemented\n"
           ,pName, Level, pMonitors, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPortsA(
                            LPSTR pName,
                            DWORD Level,
                            LPBYTE pPorts,
                            DWORD cbBuf,
                            LPDWORD pcbNeeded,
                            LPDWORD pcReturned
                            )

{

  dprintf(("WINSPOOL: EnumPortsA(%s, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,pName, Level, pPorts, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPortsW(
                            LPWSTR pName,
                            DWORD Level,
                            LPBYTE pPorts,
                            DWORD cbBuf,
                            LPDWORD pcbNeeded,
                            LPDWORD pcReturned
                            )

{

  dprintf(("WINSPOOL: EnumPortsW(%s, %d, %08x, %d, %08x, %08x)not implemented\n"
           ,pName, Level, pPorts, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPrintProcessorDatatypesA(
                                              LPSTR pName,
                                              LPSTR pPrintProcessorName,
                                              DWORD Level,
                                              LPBYTE pDatatypes,
                                              DWORD cbBuf,
                                              LPDWORD pcbNeeded,
                                              LPDWORD pcReturned
                                              )

{

  dprintf(("WINSPOOL: EnumPrintProcessorDatatypesA(%s, %s, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,pName, pPrintProcessorName, Level, pDatatypes, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPrintProcessorDatatypesW(
                                              LPWSTR pName,
                                              LPWSTR pPrintProcessorName,
                                              DWORD Level,
                                              LPBYTE pDatatypes,
                                              DWORD cbBuf,
                                              LPDWORD pcbNeeded,
                                              LPDWORD pcReturned
                                              )

{

  dprintf(("WINSPOOL: EnumPrintProcessorDatatypesW(%s, %s, %d, %08x, %d, %08x, %08x)not implemented\n"
           ,pName, pPrintProcessorName, Level, pDatatypes, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPrintProcessorsA(
                                      LPSTR pName,
                                      LPSTR pEnvironment,
                                      DWORD Level,
                                      LPBYTE pPrintProcessorInfo,
                                      DWORD cbBuf,
                                      LPDWORD pcbNeeded,
                                      LPDWORD pcReturned
                                      )

{

  dprintf(("WINSPOOL: EnumPrintProcessorsA(%s, %s, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,pName, pEnvironment, Level, pPrintProcessorInfo, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPrintProcessorsW(
                                      LPWSTR pName,
                                      LPWSTR pEnvironment,
                                      DWORD Level,
                                      LPBYTE pPrintProcessorInfo,
                                      DWORD cbBuf,
                                      LPDWORD pcbNeeded,
                                      LPDWORD pcReturned
                                      )

{

  dprintf(("WINSPOOL: EnumPrintProcessorsW(%s, %s, %d, %08x, %d, %08x, %08x)not implemented\n"
           ,pName, pEnvironment, Level, pPrintProcessorInfo, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPrinterDriversA(
                                     LPSTR pName,
                                     LPSTR pEnvironment,
                                     DWORD Level,
                                     LPBYTE pDriverInfo,
                                     DWORD cbBuf,
                                     LPDWORD pcbNeeded,
                                     LPDWORD pcReturned
                                     )

{

  dprintf(("WINSPOOL: EnumPrinterDriversA(%s, %s, %d, %08x, %d, %08x, %08x) not implemented\n"
           ,pName, pEnvironment, Level, pDriverInfo, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPrinterDriversW(
                                     LPWSTR pName,
                                     LPWSTR pEnvironment,
                                     DWORD Level,
                                     LPBYTE pDriverInfo,
                                     DWORD cbBuf,
                                     LPDWORD pcbNeeded,
                                     LPDWORD pcReturned
                                     )

{

  dprintf(("WINSPOOL: EnumPrinterDriversW(%s, %s, %d, %08x, %d, %08x, %08x)not implemented\n"
           ,pName, pEnvironment, Level, pDriverInfo, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API EnumPrintersW(
                               DWORD Flags,
                               LPWSTR Name,
                               DWORD Level,
                               LPBYTE pPrinterEnum,
                               DWORD cbBuf,
                               LPDWORD pcbNeeded,
                               LPDWORD pcReturned
                               )

{

  dprintf(("WINSPOOL: EnumPrintersW(%d, %s, %d, %08x, %d, %08x, %08x)not implemented\n"
           ,Flags, Name, Level, pPrinterEnum, cbBuf, pcbNeeded, pcReturned
         ));

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
BOOL WIN32API FindClosePrinterChangeNotification(
                                                    HANDLE hChange
                                                    )

{

  dprintf(("WINSPOOL: FindClosePrinterChangeNotification(%u) not implemented\n"
           ,hChange
         ));

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
HANDLE WIN32API FindFirstPrinterChangeNotification(
                                                      HANDLE hPrinter,
                                                      DWORD fdwFlags,
                                                      DWORD fdwOptions,
                                                      LPVOID pPrinterNotifyOptions
                                                      )

{

  dprintf(("WINSPOOL: FindFirstPrinterChangeNotification(%u, %d, %d, %08x) not implemented\n"
           ,hPrinter, fdwFlags, fdwOptions, pPrinterNotifyOptions
         ));

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
BOOL WIN32API FindNextPrinterChangeNotification(
                                                   HANDLE hChange,
                                                   PDWORD pdwChange,
                                                   LPVOID pPrinterNotifyOptions,
                                                   LPPRINTER_NOTIFY_INFO ppPrinterNotifyInfo
                                                   )

{

  dprintf(("WINSPOOL: FindNextPrinterChangeNotification(%u, %08x, %08x, %08x) not implemented\n"
           ,hChange, pdwChange, pPrinterNotifyOptions, *ppPrinterNotifyInfo
         ));

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
BOOL WIN32API FreePrinterNotifyInfo(
                                       LPPRINTER_NOTIFY_INFO pPrinterNotifyInfo
                                       )

{

  dprintf(("WINSPOOL: FreePrinterNotifyInfo(%08x) not implemented\n"
           ,pPrinterNotifyInfo
         ));

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
BOOL WIN32API GetFormA(
                          HANDLE hPrinter,
                          LPSTR pFormName,
                          DWORD Level,
                          LPBYTE pForm,
                          DWORD cbBuf,
                          LPDWORD pcbNeeded
                          )

{

  dprintf(("WINSPOOL: GetFormA(%u, %s, %d, %08x, %d, %08x) not implemented\n"
           ,hPrinter, pFormName, Level, pForm, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetFormW(
                          HANDLE hPrinter,
                          LPWSTR pFormName,
                          DWORD Level,
                          LPBYTE pForm,
                          DWORD cbBuf,
                          LPDWORD pcbNeeded
                          )

{

  dprintf(("WINSPOOL: GetFormW(%u, %s, %d, %08x, %d, %08x)not implemented\n"
           ,hPrinter, pFormName, Level, pForm, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetJobA(
                        HANDLE hPrinter,
                        DWORD JobId,
                        DWORD Level,
                        LPBYTE pJob,
                        DWORD cbBuf,
                        LPDWORD pcbNeeded
                        )

{

  dprintf(("WINSPOOL: GetJobA(%u, %d, %d, %08x, %d, %08x) not implemented\n"
           ,hPrinter, JobId, Level, pJob, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetJobW(
                        HANDLE hPrinter,
                        DWORD JobId,
                        DWORD Level,
                        LPBYTE pJob,
                        DWORD cbBuf,
                        LPDWORD pcbNeeded
                        )

{

  dprintf(("WINSPOOL: GetJobW(%u, %d, %d, %08x, %d, %08x) not implemented\n"
           ,hPrinter, JobId, Level, pJob, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetPrintProcessorDirectoryA(
                                             LPSTR pName,
                                             LPSTR pEnvironment,
                                             DWORD Level,
                                             LPBYTE pPrintProcessorInfo,
                                             DWORD cbBuf,
                                             LPDWORD pcbNeeded
                                             )

{

  dprintf(("WINSPOOL: GetPrintProcessorDirectoryA(%s, %s, %d, %08x, %d, %08x) not implemented\n"
           ,pName, pEnvironment, Level, pPrintProcessorInfo, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetPrintProcessorDirectoryW(
                                             LPWSTR pName,
                                             LPWSTR pEnvironment,
                                             DWORD Level,
                                             LPBYTE pPrintProcessorInfo,
                                             DWORD cbBuf,
                                             LPDWORD pcbNeeded
                                             )

{

  dprintf(("WINSPOOL: GetPrintProcessorDirectoryW(%s, %s, %d, %08x, %d, %08x)not implemented\n"
           ,pName, pEnvironment, Level, pPrintProcessorInfo, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetPrinterA(
                            HANDLE hPrinter,
                            DWORD Level,
                            LPBYTE pPrinter,
                            DWORD cbBuf,
                            LPDWORD pcbNeeded
                            )

{

  dprintf(("WINSPOOL: GetPrinterA(%u, %d, %08x, %d, %08x) not implemented\n"
           ,hPrinter, Level, pPrinter, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetPrinterW(
                            HANDLE hPrinter,
                            DWORD Level,
                            LPBYTE pPrinter,
                            DWORD cbBuf,
                            LPDWORD pcbNeeded
                            )

{

  dprintf(("WINSPOOL: GetPrinterW(%u, %d, %08x, %d, %08x) not implemented\n"
           ,hPrinter, Level, pPrinter, cbBuf, pcbNeeded
         ));

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
DWORD WIN32API GetPrinterDataA(
                                  HANDLE hPrinter,
                                  LPSTR pValueName,
                                  LPDWORD pType,
                                  LPBYTE pData,
                                  DWORD nSize,
                                  LPDWORD pcbNeeded
                                  )

{

  dprintf(("WINSPOOL: GetPrinterDataA(%u, %s, %08x, %08x, %d, %08x) not implemented\n"
           ,hPrinter, pValueName, pType, pData, nSize, pcbNeeded
         ));

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
DWORD WIN32API GetPrinterDataW(
                                  HANDLE hPrinter,
                                  LPWSTR pValueName,
                                  LPDWORD pType,
                                  LPBYTE pData,
                                  DWORD nSize,
                                  LPDWORD pcbNeeded
                                  )

{

  dprintf(("WINSPOOL: GetPrinterDataW(%u, %s, %08x, %08x, %d, %08x)not implemented\n"
           ,hPrinter, pValueName, pType, pData, nSize, pcbNeeded
         ));

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
BOOL WIN32API GetPrinterDriverA(
                                   HANDLE hPrinter,
                                   LPSTR pEnvironment,
                                   DWORD Level,
                                   LPBYTE pDriverInfo,
                                   DWORD cbBuf,
                                   LPDWORD pcbNeeded
                                   )

{

  dprintf(("WINSPOOL: GetPrinterDriverA(%u, %s, %d, %08x, %d, %08x) not implemented\n"
           ,hPrinter, pEnvironment, Level, pDriverInfo, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetPrinterDriverW(
                                   HANDLE hPrinter,
                                   LPWSTR pEnvironment,
                                   DWORD Level,
                                   LPBYTE pDriverInfo,
                                   DWORD cbBuf,
                                   LPDWORD pcbNeeded
                                   )

{

  dprintf(("WINSPOOL: GetPrinterDriverW(%u, %s, %d, %08x, %d, %08x)not implemented\n"
           ,hPrinter, pEnvironment, Level, pDriverInfo, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetPrinterDriverDirectoryA(
                                            LPSTR pName,
                                            LPSTR pEnvironment,
                                            DWORD Level,
                                            LPBYTE pDriverDirectory,
                                            DWORD cbBuf,
                                            LPDWORD pcbNeeded
                                            )

{

  dprintf(("WINSPOOL: GetPrinterDriverDirectoryA(%s, %s, %d, %08x, %d, %08x) not implemented\n"
           ,pName, pEnvironment, Level, pDriverDirectory, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API GetPrinterDriverDirectoryW(
                                            LPWSTR pName,
                                            LPWSTR pEnvironment,
                                            DWORD Level,
                                            LPBYTE pDriverDirectory,
                                            DWORD cbBuf,
                                            LPDWORD pcbNeeded
                                            )

{

  dprintf(("WINSPOOL: GetPrinterDriverDirectoryW(%s, %s, %d, %08x, %d, %08x)not implemented\n"
           ,pName, pEnvironment, Level, pDriverDirectory, cbBuf, pcbNeeded
         ));

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
BOOL WIN32API OpenPrinterW(
                              LPWSTR pPrinterName,
                              HANDLE * phPrinter,
                              PRINTER_DEFAULTSW * pDefault
                              )

{

  dprintf(("WINSPOOL: OpenPrinterW(%s, %08x, %08x)not implemented\n"
           ,pPrinterName, phPrinter, pDefault
         ));

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
HPRINTER WIN32API PrinterMessageBoxA(
                                        HANDLE hPrinter,
                                        DWORD Error,
                                        HWND hWnd,
                                        LPSTR pText,
                                        LPSTR pCaption,
                                        DWORD dwType
                                        )

{

  dprintf(("WINSPOOL: PrinterMessageBoxA(%u, %d, %d, %s, %s, %d) not implemented\n"
           ,hPrinter, Error, hWnd, pText, pCaption, dwType
         ));

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
HPRINTER WIN32API PrinterMessageBoxW(
                                        HANDLE hPrinter,
                                        DWORD Error,
                                        HWND hWnd,
                                        LPWSTR pText,
                                        LPWSTR pCaption,
                                        DWORD dwType
                                        )

{

  dprintf(("WINSPOOL: PrinterMessageBoxW(%u, %d, %d, %s, %s, %d)not implemented\n"
           ,hPrinter, Error, hWnd, pText, pCaption, dwType
         ));

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
BOOL WIN32API PrinterProperties(
                                   HWND hWnd,
                                   HANDLE hPrinter
                                   )

{

  dprintf(("WINSPOOL: PrinterProperties(%d, %u) not implemented\n"
           ,hWnd, hPrinter
         ));

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
BOOL WIN32API ReadPrinter(
                             HANDLE hPrinter,
                             LPVOID pBuf,
                             DWORD cbBuf,
                             LPDWORD pNoBytesRead
                             )

{

  dprintf(("WINSPOOL: ReadPrinter(%u, %08x, %d, %08x) not implemented\n"
           ,hPrinter, pBuf, cbBuf, pNoBytesRead
         ));

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
BOOL WIN32API ResetPrinterA(
                              HANDLE hPrinter,
                              PRINTER_DEFAULTSA * pDefault
                              )

{

  dprintf(("WINSPOOL: ResetPrinterA(%u, %08x) not implemented\n"
           ,hPrinter, pDefault
         ));

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
BOOL WIN32API ResetPrinterW(
                              HANDLE hPrinter,
                              PRINTER_DEFAULTSW * pDefault
                              )

{

  dprintf(("WINSPOOL: ResetPrinterW(%u, %08x) not implemented\n"
           ,hPrinter, pDefault
         ));

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
BOOL WIN32API ScheduleJob(
                             HANDLE hPrinter,
                             DWORD dwJobID
                             )

{

  dprintf(("WINSPOOL: ScheduleJob(%u, %d) not implemented\n"
           ,hPrinter, dwJobID
         ));

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
BOOL WIN32API SetFormA(
                          HANDLE hPrinter,
                          LPSTR pFormName,
                          DWORD Level,
                          LPBYTE pForm
                          )

{

  dprintf(("WINSPOOL: SetFormA(%u, %s, %d, %08x) not implemented\n"
           ,hPrinter, pFormName, Level, pForm
         ));

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
BOOL WIN32API SetFormW(
                          HANDLE hPrinter,
                          LPWSTR pFormName,
                          DWORD Level,
                          LPBYTE pForm
                          )

{

  dprintf(("WINSPOOL: SetFormW(%u, %s, %d, %08x)not implemented\n"
           ,hPrinter, pFormName, Level, pForm
         ));

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
BOOL WIN32API SetJobA(
                        HANDLE hPrinter,
                        DWORD JobId,
                        DWORD Level,
                        LPBYTE  pJob,
                        DWORD Command
                        )

{

  dprintf(("WINSPOOL: SetJobA(%u, %d, %d, %08x, %d) not implemented\n"
           ,hPrinter, JobId, Level, pJob, Command
         ));

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
BOOL WIN32API SetJobW(
                        HANDLE hPrinter,
                        DWORD JobId,
                        DWORD Level,
                        LPBYTE  pJob,
                        DWORD Command
                        )

{

  dprintf(("WINSPOOL: SetJobW(%u, %d, %d, %08x, %d) not implemented\n"
           ,hPrinter, JobId, Level, pJob, Command
         ));

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
BOOL WIN32API SetPrinterA(
                            HANDLE hPrinter,
                            DWORD Level,
                            LPBYTE pPrinter,
                            DWORD Command
                            )

{

  dprintf(("WINSPOOL: SetPrinterA(%u, %d, %08x, %d) not implemented\n"
           ,hPrinter, Level, pPrinter, Command
         ));

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
BOOL WIN32API SetPrinterW(
                            HANDLE hPrinter,
                            DWORD Level,
                            LPBYTE pPrinter,
                            DWORD Command
                            )

{

  dprintf(("WINSPOOL: SetPrinterW(%u, %d, %08x, %d) not implemented\n"
           ,hPrinter, Level, pPrinter, Command
         ));

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
DWORD WIN32API SetPrinterDataA(
                                  HANDLE hPrinter,
                                  LPSTR pValueName,
                                  DWORD Type,
                                  LPBYTE pData,
                                  DWORD cbData
                                  )

{

  dprintf(("WINSPOOL: SetPrinterDataA(%u, %s, %d, %08x, %d) not implemented\n"
           ,hPrinter, pValueName, Type, pData, cbData
         ));

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
DWORD WIN32API SetPrinterDataW(
                                  HANDLE hPrinter,
                                  LPWSTR pValueName,
                                  DWORD Type,
                                  LPBYTE pData,
                                  DWORD cbData
                                  )

{

  dprintf(("WINSPOOL: SetPrinterDataW(%u, %s, %d, %08x, %d)not implemented\n"
           ,hPrinter, pValueName, Type, pData, cbData
         ));

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
DWORD WIN32API StartDocPrinterA(
                                  HANDLE hPrinter,
                                  DWORD Level,
                                  LPBYTE pDocInfo
                                  )

{

  dprintf(("WINSPOOL: StartDocPrinterA(%u, %d, %08x) not implemented\n"
           ,hPrinter, Level, pDocInfo
         ));

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
DWORD WIN32API StartDocPrinterW(
                                  HANDLE hPrinter,
                                  DWORD Level,
                                  LPBYTE pDocInfo
                                  )

{

  dprintf(("WINSPOOL: StartDocPrinterW(%u, %d, %08x) not implemented\n"
           ,hPrinter, Level, pDocInfo
         ));

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
BOOL WIN32API StartPagePrinter(
                                  HANDLE hPrinter
                                  )

{

  dprintf(("WINSPOOL: StartPagePrinter(%u) not implemented\n"
           ,hPrinter
         ));

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
BOOL WIN32API WritePrinter(
                              HANDLE hPrinter,
                              LPVOID pBuf,
                              DWORD cbBuf,
                              LPDWORD pcWritten
                              )

{

  dprintf(("WINSPOOL: WritePrinter(%u, %08x, %d, %08x) not implemented\n"
           ,hPrinter, pBuf, cbBuf, pcWritten
         ));

  return (FALSE);
}

