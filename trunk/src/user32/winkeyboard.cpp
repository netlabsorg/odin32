/* $Id: winkeyboard.cpp,v 1.7 2000-10-18 20:17:00 sandervl Exp $ */
/*
 * Win32 <-> PM key translation
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <odin.h>
#include <string.h>
#include <stdio.h>
#include <winkeyboard.h>
#include "oslibwin.h"
#include <heapstring.h>

#define DBG_LOCALLOG	DBG_winkeyboard
#include "dbglocal.h"

BYTE abPMScanToWinVKey[256] =
/****************************************************************************/
/* PM Scancode              *    Win32 vkey                                 */
/****************************************************************************/
/* 0x00                     */ { 0x00
/* 0x01 PMSCAN_ESC          */ , VK_ESCAPE
/* 0x02 PMSCAN_ONE          */ , VK_1
/* 0x03 PMSCAN_TWO          */ , VK_2
/* 0x04 PMSCAN_THREE        */ , VK_3
/* 0x05 PMSCAN_FOUR         */ , VK_4
/* 0x06 PMSCAN_FIVE         */ , VK_5
/* 0x07 PMSCAN_SIX          */ , VK_6
/* 0x08 PMSCAN_SEVEN        */ , VK_7
/* 0x09 PMSCAN_EIGHT        */ , VK_8
/* 0x0A PMSCAN_NINE         */ , VK_9
/* 0x0B PMSCAN_ZERO         */ , VK_0
/* 0x0C PMSCAN_HYPHEN       */ , VK_HYPHEN
/* 0x0D PMSCAN_EQUAL        */ , VK_EQUAL
/* 0x0E PMSCAN_BACKSPACE    */ , VK_BACK
/* 0x0F PMSCAN_TAB          */ , VK_TAB
/* 0x10 PMSCAN_Q            */ , VK_Q
/* 0x11 PMSCAN_W            */ , VK_W
/* 0x12 PMSCAN_E            */ , VK_E
/* 0x13 PMSCAN_R            */ , VK_R
/* 0x14 PMSCAN_T            */ , VK_T
/* 0x15 PMSCAN_Y            */ , VK_Y
/* 0x16 PMSCAN_U            */ , VK_U
/* 0x17 PMSCAN_I            */ , VK_I
/* 0x18 PMSCAN_O            */ , VK_O
/* 0x19 PMSCAN_P            */ , VK_P
/* 0x1A PMSCAN_BRACKETLEFT  */ , VK_BRACKETLEFT
/* 0x1B PMSCAN_BRACKETRIGHT */ , VK_BRACKETRIGHT
/* 0x1C PMSCAN_ENTER        */ , VK_RETURN
/* 0x1D PMSCAN_CTRLLEFT     */ , VK_CONTROL
/* 0x1E PMSCAN_A            */ , VK_A
/* 0x1F PMSCAN_S            */ , VK_S
/* 0x20 PMSCAN_D            */ , VK_D
/* 0x21 PMSCAN_F            */ , VK_F
/* 0x22 PMSCAN_G            */ , VK_G
/* 0x23 PMSCAN_H            */ , VK_H
/* 0x24 PMSCAN_J            */ , VK_J
/* 0x25 PMSCAN_K            */ , VK_K
/* 0x26 PMSCAN_L            */ , VK_L
/* 0x27 PMSCAN_SEMICOLON    */ , VK_SEMICOLON
/* 0x28 PMSCAN_QUOTESINGLE  */ , VK_QUOTESINGLE
/* 0x29 PMSCAN_GRAVE        */ , VK_GRAVE
/* 0x2A PMSCAN_SHIFTLEFT    */ , VK_SHIFT
/* 0x2B PMSCAN_BACKSLASH    */ , VK_BACKSLASH
/* 0x2C PMSCAN_Z            */ , VK_Z
/* 0x2D PMSCAN_X            */ , VK_X
/* 0x2E PMSCAN_C            */ , VK_C
/* 0x2F PMSCAN_V            */ , VK_V
/* 0x30 PMSCAN_B            */ , VK_B
/* 0x31 PMSCAN_N            */ , VK_N
/* 0x32 PMSCAN_M            */ , VK_M
/* 0x33 PMSCAN_COMMA        */ , VK_COMMA
/* 0x34 PMSCAN_PERIOD       */ , VK_PERIOD
/* 0x35 PMSCAN_SLASH        */ , VK_SLASH
/* 0x36 PMSCAN_SHIFTRIGHT   */ , VK_SHIFT
/* 0x37 PMSCAN_PADASTERISK  */ , VK_MULTIPLY
/* 0x38 PMSCAN_ALTLEFT      */ , VK_MENU
/* 0x39 PMSCAN_SPACE        */ , VK_SPACE
/* 0x3A PMSCAN_CAPSLOCK     */ , VK_CAPITAL
/* 0x3B PMSCAN_F1           */ , VK_F1
/* 0x3C PMSCAN_F2           */ , VK_F2
/* 0x3D PMSCAN_F3           */ , VK_F3
/* 0x3E PMSCAN_F4           */ , VK_F4
/* 0x3F PMSCAN_F5           */ , VK_F5
/* 0x40 PMSCAN_F6           */ , VK_F6
/* 0x41 PMSCAN_F7           */ , VK_F7
/* 0x42 PMSCAN_F8           */ , VK_F8
/* 0x43 PMSCAN_F9           */ , VK_F9
/* 0x44 PMSCAN_F10          */ , VK_F10
/* 0x45 PMSCAN_NUMLOCK      */ , VK_NUMLOCK
/* 0x46 PMSCAN_SCROLLLOCK   */ , VK_SCROLL
/* 0x47 PMSCAN_PAD7         */ , VK_NUMPAD7
/* 0x48 PMSCAN_PAD8         */ , VK_NUMPAD8
/* 0x49 PMSCAN_PAD9         */ , VK_NUMPAD9
/* 0x4A PMSCAN_PADMINUS     */ , VK_SUBTRACT
/* 0x4B PMSCAN_PAD4         */ , VK_NUMPAD4
/* 0x4C PMSCAN_PAD5         */ , VK_NUMPAD5
/* 0x4D PMSCAN_PAD6         */ , VK_NUMPAD6
/* 0x4E PMSCAN_PADPLUS      */ , VK_ADD
/* 0x4F PMSCAN_PAD1         */ , VK_NUMPAD1
/* 0x50 PMSCAN_PAD2         */ , VK_NUMPAD2
/* 0x51 PMSCAN_PAD3         */ , VK_NUMPAD3
/* 0x52 PMSCAN_PAD0         */ , VK_NUMPAD0
/* 0x53 PMSCAN_PADPERIOD    */ , VK_DECIMAL
/* 0x54 PMSCAN_SYSREQ       */ , 0x00
/* 0x55 PMSCAN_RESET        */ , 0x00
/* 0x56 PMSCAN_EXTRA        */ , VK_EXTRA
/* 0x57 PMSCAN_F11          */ , VK_F11
/* 0x58 PMSCAN_F12          */ , VK_F12
/* 0x59 PMSCAN_BACKTAB      */ , 0x00
/* 0x5A PMSCAN_PADENTER     */ , VK_RETURN
/* 0x5B PMSCAN_CTRLRIGHT    */ , VK_CONTROL
/* 0x5C PMSCAN_PADSLASH     */ , VK_DIVIDE
/* 0x5D PMSCAN_PRINT        */ , VK_PRINT
/* 0x5E PMSCAN_ALTRIGHT     */ , VK_MENU
/* 0x5F PMSCAN_PAUSE        */ , VK_PAUSE
/* 0x60 PMSCAN_HOME         */ , VK_HOME
/* 0x61 PMSCAN_UP           */ , VK_UP
/* 0x62 PMSCAN_PAGEUP       */ , VK_PRIOR
/* 0x63 PMSCAN_LEFT         */ , VK_LEFT
/* 0x64 PMSCAN_RIGHT        */ , VK_RIGHT
/* 0x65 PMSCAN_END          */ , VK_END
/* 0x66 PMSCAN_DOWN         */ , VK_DOWN
/* 0x67 PMSCAN_PAGEDOWN     */ , VK_NEXT
/* 0x68 PMSCAN_INSERT       */ , VK_INSERT
/* 0x69 PMSCAN_DELETE       */ , VK_DELETE
/* 0x6A PMSCAN_F23          */ , VK_F23
/* 0x6B PMSCAN_F24          */ , VK_F24
/* 0x6C PMSCAN_SYSMEM       */ , 0x00
/* 0x6D PMSCAN_ERASEEOF     */ , VK_EREOF
/* 0x6E PMSCAN_BREAK        */ , VK_CANCEL
/* 0x6F PMSCAN_MOVEWIN      */ , 0x00
/* 0x70 PMSCAN_NLS3         */ , 0x00
/* 0x71 PMSCAN_HELP         */ , VK_HELP
/* 0x72 PMSCAN_TASKMAN      */ , 0x00
/* 0x73 PMSCAN_B11          */ , 0x00
/* 0x74 PMSCAN_JUMP         */ , 0x00
/* 0x75 PMSCAN_MINWIN       */ , 0x00
/* 0x76 PMSCAN_CLEAR        */ , 0x00
/* 0x77 PMSCAN_77           */ , 0x00
/* 0x78 PMSCAN_78           */ , 0x00
/* 0x79 PMSCAN_NLS2         */ , 0x00
/* 0x7a PMSCAN_SIZE         */ , 0x00
/* 0x7b PMSCAN_NLS1         */ , 0x00
/* 0x7c PMSCAN_APPLICATION  */ , VK_APPS
/* 0x7d PMSCAN_E13          */ , 0x00
/* 0x7e PMSCAN              */ , 0x00
/* 0x7f PMSCAN              */ , 0x00
/* 0x80 PMSCAN_PA1          */ , VK_PA1
/* 0x81 PMSCAN_F13          */ , VK_F13
/* 0x82 PMSCAN_F14          */ , VK_F14
/* 0x83 PMSCAN_F15          */ , VK_F15
/* 0x84 PMSCAN_PA2          */ , 0x00
/* 0x85 PMSCAN_PA3          */ , 0x00
/* 0x86 PMSCAN_SPACEBREAK   */ , 0x00
/* 0x87 PMSCAN_TABRIGHT     */ , 0x00
/* 0x88 PMSCAN_NOOP         */ , 0x00
/* 0x89 PMSCAN_F16          */ , VK_F16
/* 0x8a PMSCAN_F17          */ , VK_F17
/* 0x8b PMSCAN_F18          */ , VK_F18
/* 0x8c PMSCAN_F19          */ , VK_F19
/* 0x8d PMSCAN_F20          */ , VK_F20
/* 0x8e PMSCAN_F21          */ , VK_F21
/* 0x8f PMSCAN_F22          */ , VK_F22
/* 0x90                     */ , 0x00
/* 0x91                     */ , 0x00
/* 0x92                     */ , 0x00
/* 0x93                     */ , 0x00
/* 0x94                     */ , 0x00
/* 0x95                     */ , 0x00
/* 0x96                     */ , 0x00
/* 0x97                     */ , 0x00
/* 0x98                     */ , 0x00
/* 0x99                     */ , 0x00
/* 0x9A                     */ , 0x00
/* 0x9B                     */ , 0x00
/* 0x9C                     */ , 0x00
/* 0x9D                     */ , 0x00
/* 0x9E                     */ , 0x00
/* 0x9F                     */ , 0x00
/* 0xA0                     */ , 0x00
/* 0xA1                     */ , 0x00
/* 0xA2                     */ , 0x00
/* 0xA3                     */ , 0x00
/* 0xA4                     */ , 0x00
/* 0xA5                     */ , 0x00
/* 0xA6                     */ , 0x00
/* 0xA7                     */ , 0x00
/* 0xA8                     */ , 0x00
/* 0xA9                     */ , 0x00
/* 0xAA                     */ , 0x00
/* 0xAB                     */ , 0x00
/* 0xAC                     */ , 0x00
/* 0xAD                     */ , 0x00
/* 0xAE                     */ , 0x00
/* 0xAF                     */ , 0x00
/* 0xB0                     */ , 0x00
/* 0xB1                     */ , 0x00
/* 0xB2                     */ , 0x00
/* 0xB3                     */ , 0x00
/* 0xB4                     */ , 0x00
/* 0xB5                     */ , 0x00
/* 0xB6                     */ , 0x00
/* 0xB7                     */ , 0x00
/* 0xB8                     */ , 0x00
/* 0xB9                     */ , 0x00
/* 0xBA                     */ , 0x00
/* 0xBB                     */ , 0x00
/* 0xBC                     */ , 0x00
/* 0xBD                     */ , 0x00
/* 0xBE                     */ , 0x00
/* 0xBF                     */ , 0x00
/* 0xC0                     */ , 0x00
/* 0xC1                     */ , 0x00
/* 0xC2                     */ , 0x00
/* 0xC3                     */ , 0x00
/* 0xC4                     */ , 0x00
/* 0xC5                     */ , 0x00
/* 0xC6                     */ , 0x00
/* 0xC7                     */ , 0x00
/* 0xC8                     */ , 0x00
/* 0xC9                     */ , 0x00
/* 0xCA                     */ , 0x00
/* 0xCB                     */ , 0x00
/* 0xCC                     */ , 0x00
/* 0xCD                     */ , 0x00
/* 0xCE                     */ , 0x00
/* 0xCF                     */ , 0x00
/* 0xD0                     */ , 0x00
/* 0xD1                     */ , 0x00
/* 0xD2                     */ , 0x00
/* 0xD3                     */ , 0x00
/* 0xD4                     */ , 0x00
/* 0xD5                     */ , 0x00
/* 0xD6                     */ , 0x00
/* 0xD7                     */ , 0x00
/* 0xD8                     */ , 0x00
/* 0xD9                     */ , 0x00
/* 0xDA                     */ , 0x00
/* 0xDB                     */ , 0x00
/* 0xDC                     */ , 0x00
/* 0xDD                     */ , 0x00
/* 0xDE                     */ , 0x00
/* 0xDF                     */ , 0x00
/* 0xE0                     */ , 0x00
/* 0xE1                     */ , 0x00
/* 0xE2                     */ , 0x00
/* 0xE3                     */ , 0x00
/* 0xE4                     */ , 0x00
/* 0xE5                     */ , 0x00
/* 0xE6                     */ , 0x00
/* 0xE7                     */ , 0x00
/* 0xE8                     */ , 0x00
/* 0xE9                     */ , 0x00
/* 0xEA                     */ , 0x00
/* 0xEB                     */ , 0x00
/* 0xEC                     */ , 0x00
/* 0xED                     */ , 0x00
/* 0xEE                     */ , 0x00
/* 0xEF                     */ , 0x00
/* 0xF0                     */ , 0x00
/* 0xF1                     */ , 0x00
/* 0xF2                     */ , 0x00
/* 0xF3                     */ , 0x00
/* 0xF4                     */ , 0x00
/* 0xF5                     */ , 0x00
/* 0xF6                     */ , 0x00
/* 0xF7                     */ , 0x00
/* 0xF8                     */ , 0x00
/* 0xF9                     */ , 0x00
/* 0xFA                     */ , 0x00
/* 0xFB                     */ , 0x00
/* 0xFC                     */ , 0x00
/* 0xFD                     */ , 0x00
/* 0xFE                     */ , 0x00
/* 0xFF                     */ , 0x00
                               };

//******************************************************************************
//******************************************************************************
BYTE WIN32API KeyTranslatePMToWin(BYTE key)
{
   return abPMScanToWinVKey[key];
}
//******************************************************************************
//******************************************************************************
void WIN32API KeyTranslatePMToWinBuf(BYTE *pmkey, BYTE *winkey, int nrkeys)
{
   for(int i=0;i<nrkeys;i++) {
    	winkey[i] = abPMScanToWinVKey[pmkey[i]];
   }
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetKeyboardState(PBYTE lpKeyState)
{
 BYTE   PMKeyState[256];
 BOOL   rc;

  dprintf(("USER32: GetKeyboardState %x", lpKeyState));
  rc = OSLibWinGetKeyboardStateTable((PBYTE)&PMKeyState );

  if(rc == TRUE) 
  {
    	KeyTranslatePMToWinBuf((BYTE *)&PMKeyState, lpKeyState, 256);
    	return TRUE;
  }
  return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetKeyboardState(PBYTE lpKeyState)
{
  dprintf(("USER32: SetKeyboardState %x not implemented", lpKeyState));
  return(TRUE);
}
/***********************************************************************
 *           GetKeyboardLayout			(USER32.250)
 *
 * FIXME: - device handle for keyboard layout defaulted to 
 *          the language id. This is the way Windows default works.
 *        - the thread identifier (dwLayout) is also ignored.
 */
// * Remark    : Based on Wine version (991031)
HKL WIN32API GetKeyboardLayout(DWORD dwLayout)
{
        HKL layout;
        layout = GetSystemDefaultLCID(); /* FIXME */
        layout |= (layout<<16);          /* FIXME */
        dprintf(("GetKeyboardLayout returning %08x\n",layout));
        return layout;
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetKeyboardLayoutNameA
 * Purpose   : The GetKeyboardLayoutName function retrieves the name of the
 *             active keyboard layout.
 * Parameters: LPTSTR pwszKLID address of buffer for layout name
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    : Based on Wine version (991031)
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
INT WIN32API GetKeyboardLayoutNameA(LPTSTR pwszKLID)
{
   dprintf(("USER32:GetKeyboardLayoutNameA"));

   sprintf(pwszKLID, "%08x",GetKeyboardLayout(0));
   return 1;
}
/*****************************************************************************
 * Name      : BOOL WIN32API GetKeyboardLayoutNameW
 * Purpose   : The GetKeyboardLayoutName function retrieves the name of the
 *             active keyboard layout.
 * Parameters: LPTSTR pwszKLID address of buffer for layout name
 * Variables :
 * Result    : If the function succeeds, the return value is TRUE.
 *             If the function fails, the return value is FALSE. To get extended
 *               error information, call GetLastError.
 * Remark    : Based on Wine version (991031)
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
INT WIN32API GetKeyboardLayoutNameW(LPWSTR pwszKLID)
{
   LPSTR buf = (LPSTR)HEAP_xalloc( GetProcessHeap(), 0, strlen("00000409")+1);

   dprintf(("USER32:GetKeyboardLayoutNameA"));
   int res = GetKeyboardLayoutNameA(buf);
   lstrcpyAtoW(pwszKLID,buf);
   HeapFree( GetProcessHeap(), 0, buf );
   return res;
}
/***********************************************************************
 *           GetKeyboardLayoutList		(USER32.251)
 *
 * FIXME: Supports only the system default language and layout and 
 *          returns only 1 value.
 *
 * Return number of values available if either input parm is 
 *  0, per MS documentation.
 *
 * Remark    : Based on Wine version (991031)
 */
INT WINAPI GetKeyboardLayoutList(INT nBuff,HKL *layouts)
{
        dprintf(("GetKeyboardLayoutList(%d,%p)\n",nBuff,layouts));
        if (!nBuff || !layouts)
            return 1;
	if (layouts)
		layouts[0] = GetKeyboardLayout(0);
	return 1;
}
/*****************************************************************************
 * Name      : int WIN32API ToAscii
 * Purpose   : The ToAscii function translates the specified virtual-key code
 *             and keyboard state to the corresponding Windows character or characters.
 * Parameters: UINT   uVirtKey    virtual-key code
 *             UINT   uScanCode   scan code
 *             PBYTE  lpbKeyState address of key-state array
 *             LPWORD lpwTransKey buffer for translated key
 *             UINT   fuState     active-menu flag
 * Variables :
 * Result    : 0 The specified virtual key has no translation for the current
 *               state of the keyboard.
 *             1 One Windows character was copied to the buffer.
 *             2 Two characters were copied to the buffer. This usually happens
 *               when a dead-key character (accent or diacritic) stored in the
 *               keyboard layout cannot be composed with the specified virtual
 *               key to form a single character.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Based on Wine code (windows\x11drv\keyboard.c
 * Copyright 1993 Bob Amstadt
 * Copyright 1996 Albrecht Kleine 
 * Copyright 1997 David Faure
 * Copyright 1998 Morten Welinder
 * Copyright 1998 Ulrich Weigand
 * Copyright 1999 Ove K�ven
 *
 *****************************************************************************/
int WIN32API ToAscii(UINT   uVirtKey,
                     UINT   uScanCode,
                     PBYTE  lpbKeyState,
                     LPWORD lpwTransKey,
                     UINT   fuState)
{
  dprintf(("USER32:ToAscii (%u,%u,%08xh,%08xh,%u) not implemented.\n",
         uVirtKey,
         uScanCode,
         lpbKeyState,
         lpwTransKey,
         fuState));

  INT ret;

  if (uScanCode == 0) {
        /* This happens when doing Alt+letter : a fake 'down arrow' key press
           event is generated by windows. Just ignore it. */
        dprintf2(("scanCode=0, doing nothing"));
        return 0;
  }
  if (uScanCode & 0x8000)
  {
        dprintf2(("Key UP, doing nothing"));
        return 0;
  }
  /* We have a special case to handle : Shift + arrow, shift + home, ...
     X returns a char for it, but Windows doesn't. Let's eat it. */
  if(!(lpbKeyState[VK_NUMLOCK] & 0x01)  /* NumLock is off */
       && (lpbKeyState[VK_SHIFT] & 0x80) /* Shift is pressed */
       && (uVirtKey >= VK_0) && (uVirtKey >= VK_9))
  {
        *(char*)lpwTransKey = 0;
        ret = 0;
  }
  else        
  /* We have another special case for delete key (XK_Delete) on an
     extended keyboard. X returns a char for it, but Windows doesn't */
  if (uVirtKey == VK_DELETE)
  {
       *(char*)lpwTransKey = 0;
       ret = 0;
  }
  else {
       *(char*)lpwTransKey = uVirtKey;
       ret = 1;
  }
  return ret;
}
/*****************************************************************************
 * Name      : int WIN32API ToAsciiEx
 * Purpose   : The ToAscii function translates the specified virtual-key code
 *             and keyboard state to the corresponding Windows character or characters.
 * Parameters: UINT   uVirtKey    virtual-key code
 *             UINT   uScanCode   scan code
 *             PBYTE  lpbKeyState address of key-state array
 *             LPWORD lpwTransKey buffer for translated key
 *             UINT   fuState     active-menu flag
 *             HLK    hlk         keyboard layout handle
 * Variables :
 * Result    : 0 The specified virtual key has no translation for the current
 *               state of the keyboard.
 *             1 One Windows character was copied to the buffer.
 *             2 Two characters were copied to the buffer. This usually happens
 *               when a dead-key character (accent or diacritic) stored in the
 *               keyboard layout cannot be composed with the specified virtual
 *               key to form a single character.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
int WIN32API ToAsciiEx(UINT   uVirtKey,
                       UINT   uScanCode,
                       PBYTE  lpbKeyState,
                       LPWORD lpwTransKey,
                       UINT   fuState,
                       HKL    hkl)
{
  dprintf(("USER32:ToAsciiEx (%u,%u,%08xh,%08xh,%u,%08x) not implemented.\n",
         uVirtKey,
         uScanCode,
         lpbKeyState,
         lpwTransKey,
         fuState,
         hkl));

  return (0);
}
//******************************************************************************
//******************************************************************************
