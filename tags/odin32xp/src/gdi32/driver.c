/* $Id: driver.c,v 1.1 2000-03-10 16:13:44 sandervl Exp $ */

/*
 * Graphics driver management functions
 *
 * Copyright 1996 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */
#include <windows.h>
#include <misc.h>

/*****************************************************************************
 *      GDI_CallDevInstall16   [GDI32.100]
 *
 * This should thunk to 16-bit and simply call the proc with the given args.
 */
INT WINAPI GDI_CallDevInstall16( FARPROC16 lpfnDevInstallProc, HWND hWnd, 
                                 LPSTR lpModelName, LPSTR OldPort, LPSTR NewPort )
{
    dprintf(("STUB: GDI_CallDevInstall16: (%p, %04x, %s, %s, %s)\n", 
	    lpfnDevInstallProc, hWnd, lpModelName, OldPort, NewPort));
    return -1;
}

/*****************************************************************************
 *      GDI_CallExtDeviceModePropSheet16   [GDI32.101]
 *
 * This should load the correct driver for lpszDevice and calls this driver's
 * ExtDeviceModePropSheet proc. 
 *
 * Note: The driver calls a callback routine for each property sheet page; these 
 * pages are supposed to be filled into the structure pointed to by lpPropSheet.
 * The layout of this structure is:
 * 
 * struct
 * {
 *   DWORD  nPages;
 *   DWORD  unknown;
 *   HPROPSHEETPAGE  pages[10];
 * };
 */
INT WINAPI GDI_CallExtDeviceModePropSheet16( HWND hWnd, LPCSTR lpszDevice, 
                                             LPCSTR lpszPort, LPVOID lpPropSheet )
{
    dprintf(("STUB: GDI_CallExtDeviceModePropSheet16 (%04x, %s, %s, %p)\n", 
      hWnd, lpszDevice, lpszPort, lpPropSheet ));
    return -1;
}

/*****************************************************************************
 *      GDI_CallExtDeviceMode16   [GDI32.102]
 *
 * This should load the correct driver for lpszDevice and calls this driver's
 * ExtDeviceMode proc.
 */
INT WINAPI GDI_CallExtDeviceMode16( HWND hwnd, 
                                    LPDEVMODEA lpdmOutput, LPSTR lpszDevice, 
                                    LPSTR lpszPort, LPDEVMODEA lpdmInput, 
                                    LPSTR lpszProfile, DWORD fwMode )
{
    dprintf(("STUB: GDI_CallExtDeviceMode16(%04x, %p, %s, %s, %p, %s, %ld)\n", 
	  hwnd, lpdmOutput, lpszDevice, lpszPort, 
	  lpdmInput, lpszProfile, fwMode ));
    return -1;
}

/****************************************************************************
 *      GDI_CallAdvancedSetupDialog16     [GDI32.103]
 *
 * This should load the correct driver for lpszDevice and calls this driver's
 * AdvancedSetupDialog proc.
 */
INT WINAPI GDI_CallAdvancedSetupDialog16( HWND hwnd, LPSTR lpszDevice,
                                          LPDEVMODEA devin, LPDEVMODEA devout )
{
    dprintf(("STUB: GDI_CallAdvancedSetupDialog16: (%04x, %s, %p, %p)\n", 
	  hwnd, lpszDevice, devin, devout ));
    return -1;
}

/*****************************************************************************
 *      GDI_CallDeviceCapabilities16      [GDI32.104]
 *
 * This should load the correct driver for lpszDevice and calls this driver's
 * DeviceCapabilities proc.
 */
DWORD WINAPI GDI_CallDeviceCapabilities16( LPCSTR lpszDevice, LPCSTR lpszPort,
                                           WORD fwCapability, LPSTR lpszOutput,
                                           LPDEVMODEA lpdm )
{
    dprintf(("STUB: GDI_CallDeviceCapabilities16(%s, %s, %d, %p, %p)\n", 
	  lpszDevice, lpszPort, fwCapability, lpszOutput, lpdm ));
    return -1;
}
