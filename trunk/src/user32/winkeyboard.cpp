/* $Id: winkeyboard.cpp,v 1.12 2001-07-04 06:39:01 sandervl Exp $ */
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
#include <pmscan.h>
#include <winuser32.h>

#define DBG_LOCALLOG	DBG_winkeyboard
#include "dbglocal.h"

BOOL OPEN32API _O32_GetKeyboardState( PBYTE lpKeyState );

inline BOOL O32_GetKeyboardState(PBYTE lpKeyState)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetKeyboardState(lpKeyState);
    SetFS(sel);

    return yyrc;
} 


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
/* 0x1D PMSCAN_CTRLLEFT     */ , VK_LCONTROL
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
/* 0x2A PMSCAN_SHIFTLEFT    */ , VK_LSHIFT
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
/* 0x36 PMSCAN_SHIFTRIGHT   */ , VK_RSHIFT
/* 0x37 PMSCAN_PADASTERISK  */ , VK_MULTIPLY
/* 0x38 PMSCAN_ALTLEFT      */ , VK_LMENU
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
/* 0x5E PMSCAN_ALTRIGHT     */ , VK_RMENU
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

BYTE abWinVKeyToPMScan[256] =
/****************************************/
/* Vkey                   *    Scancode */
/****************************************/
/* 0x00                   */ { 0x00
/* 0x01 VK_LBUTTON        */ , 0x00
/* 0x02 VK_RBUTTON        */ , 0x00
/* 0x03 VK_CANCEL         */ , PMSCAN_BREAK
/* 0x04 VK_MBUTTON        */ , 0x00
/* 0x05                   */ , 0x00
/* 0x06                   */ , 0x00
/* 0x07                   */ , 0x00
/* 0x08 VK_BACK           */ , PMSCAN_BACKSPACE
/* 0x09 VK_TAB            */ , PMSCAN_TAB
/* 0x0A                   */ , 0x00
/* 0x0B                   */ , 0x00
/* 0x0C VK_CLEAR          */ , PMSCAN_PAD5
/* 0x0D VK_RETURN         */ , PMSCAN_ENTER
/* 0x0E                   */ , 0x00
/* 0x0F                   */ , 0x00
/* 0x10 VK_SHIFT          */ , PMSCAN_SHIFTLEFT
/* 0x11 VK_CONTROL        */ , PMSCAN_CTRLLEFT
/* 0x12 VK_MENU           */ , PMSCAN_ALTLEFT
/* 0x13 VK_PAUSE          */ , PMSCAN_PAUSE
/* 0x14 VK_CAPITAL        */ , PMSCAN_CAPSLOCK
/* 0x15                   */ , 0x00
/* 0x16                   */ , 0x00
/* 0x17                   */ , 0x00
/* 0x18                   */ , 0x00
/* 0x19                   */ , 0x00
/* 0x1A                   */ , 0x00
/* 0x1B VK_ESCAPE         */ , PMSCAN_ESC
/* 0x1C                   */ , 0x00
/* 0x1D                   */ , 0x00
/* 0x1E                   */ , 0x00
/* 0x1F                   */ , 0x00
/* 0x20 VK_SPACE          */ , PMSCAN_SPACE
/* 0x21 VK_PRIOR          */ , PMSCAN_PAGEUP
/* 0x22 VK_NEXT           */ , PMSCAN_PAGEDOWN
/* 0x23 VK_END            */ , PMSCAN_END
/* 0x24 VK_HOME           */ , PMSCAN_HOME
/* 0x25 VK_LEFT           */ , PMSCAN_LEFT
/* 0x26 VK_UP             */ , PMSCAN_UP
/* 0x27 VK_RIGHT          */ , PMSCAN_RIGHT
/* 0x28 VK_DOWN           */ , PMSCAN_DOWN
/* 0x29 VK_SELECT         */ , 0x00
/* 0x2A VK_PRINT          */ , PMSCAN_PRINT
/* 0x2B VK_EXECUTE        */ , 0x00
/* 0x2C VK_SNAPSHOT       */ , 0x00
/* 0x2D VK_INSERT         */ , PMSCAN_INSERT
/* 0x2E VK_DELETE         */ , PMSCAN_DELETE
/* 0x2F VK_HELP           */ , PMSCAN_HELP
/* 0x30 VK_0              */ , PMSCAN_ZERO
/* 0x31 VK_1              */ , PMSCAN_ONE
/* 0x32 VK_2              */ , PMSCAN_TWO
/* 0x33 VK_3              */ , PMSCAN_THREE
/* 0x34 VK_4              */ , PMSCAN_FOUR
/* 0x35 VK_5              */ , PMSCAN_FIVE
/* 0x36 VK_6              */ , PMSCAN_SIX
/* 0x37 VK_7              */ , PMSCAN_SEVEN
/* 0x38 VK_8              */ , PMSCAN_EIGHT
/* 0x39 VK_9              */ , PMSCAN_NINE
/* 0x3A                   */ , 0x00
/* 0x3B                   */ , 0x00
/* 0x3C                   */ , 0x00
/* 0x3D                   */ , 0x00
/* 0x3E                   */ , 0x00
/* 0x3F                   */ , 0x00
/* 0x40                   */ , 0x00
/* 0x41 VK_A              */ , PMSCAN_A
/* 0x42 VK_B              */ , PMSCAN_B
/* 0x43 VK_C              */ , PMSCAN_C
/* 0x44 VK_D              */ , PMSCAN_D
/* 0x45 VK_E              */ , PMSCAN_E
/* 0x46 VK_F              */ , PMSCAN_F
/* 0x47 VK_G              */ , PMSCAN_G
/* 0x48 VK_H              */ , PMSCAN_H
/* 0x49 VK_I              */ , PMSCAN_I
/* 0x4A VK_J              */ , PMSCAN_J
/* 0x4B VK_K              */ , PMSCAN_K
/* 0x4C VK_L              */ , PMSCAN_L
/* 0x4D VK_M              */ , PMSCAN_M
/* 0x4E VK_N              */ , PMSCAN_N
/* 0x4F VK_O              */ , PMSCAN_O
/* 0x50 VK_P              */ , PMSCAN_P
/* 0x51 VK_Q              */ , PMSCAN_Q
/* 0x52 VK_R              */ , PMSCAN_R
/* 0x53 VK_S              */ , PMSCAN_S
/* 0x54 VK_T              */ , PMSCAN_T
/* 0x55 VK_U              */ , PMSCAN_U
/* 0x56 VK_V              */ , PMSCAN_V
/* 0x57 VK                */ , PMSCAN_W
/* 0x58 VK_X              */ , PMSCAN_X
/* 0x59 VK_Y              */ , PMSCAN_Y
/* 0x5A VK_Z              */ , PMSCAN_Z
/* 0x5B VK_LWIN           */ , PMSCAN_WINLEFT
/* 0x5C VK_RWIN           */ , PMSCAN_WINRIGHT
/* 0x5D VK_APPS           */ , PMSCAN_APPLICATION
/* 0x5E                   */ , 0x00
/* 0x5F                   */ , 0x00
/* 0x60 VK_NUMPAD0        */ , PMSCAN_PAD0
/* 0x61 VK_NUMPAD1        */ , PMSCAN_PAD1
/* 0x62 VK_NUMPAD2        */ , PMSCAN_PAD2
/* 0x63 VK_NUMPAD3        */ , PMSCAN_PAD3
/* 0x64 VK_NUMPAD4        */ , PMSCAN_PAD4
/* 0x65 VK_NUMPAD5        */ , PMSCAN_PAD5
/* 0x66 VK_NUMPAD6        */ , PMSCAN_PAD6
/* 0x67 VK_NUMPAD7        */ , PMSCAN_PAD7
/* 0x68 VK_NUMPAD8        */ , PMSCAN_PAD8
/* 0x69 VK_NUMPAD9        */ , PMSCAN_PAD9
/* 0x6A VK_MULTIPLY       */ , PMSCAN_PADASTERISK
/* 0x6B VK_ADD            */ , PMSCAN_PADPLUS
/* 0x6C VK_SEPARATOR      */ , 0x00
/* 0x6D VK_SUBTRACT       */ , PMSCAN_PADMINUS
/* 0x6E VK_DECIMAL        */ , PMSCAN_PADPERIOD
/* 0x6F VK_DIVIDE         */ , PMSCAN_PADSLASH
/* 0x70 VK_F1             */ , PMSCAN_F1
/* 0x71 VK_F2             */ , PMSCAN_F2
/* 0x72 VK_F3             */ , PMSCAN_F3
/* 0x73 VK_F4             */ , PMSCAN_F4
/* 0x74 VK_F5             */ , PMSCAN_F5
/* 0x75 VK_F6             */ , PMSCAN_F6
/* 0x76 VK_F7             */ , PMSCAN_F7
/* 0x77 VK_F8             */ , PMSCAN_F8
/* 0x78 VK_F9             */ , PMSCAN_F9
/* 0x79 VK_F10            */ , PMSCAN_F10
/* 0x7A VK_F11            */ , PMSCAN_F11
/* 0x7B VK_F12            */ , PMSCAN_F12
/* 0x7C VK_F13            */ , PMSCAN_F13
/* 0x7D VK_F14            */ , PMSCAN_F14
/* 0x7E VK_F15            */ , PMSCAN_F15
/* 0x7F VK_F16            */ , PMSCAN_F16
/* 0x80 VK_F17            */ , PMSCAN_F17
/* 0x81 VK_F18            */ , PMSCAN_F18
/* 0x82 VK_F19            */ , PMSCAN_F19
/* 0x83 VK_F20            */ , PMSCAN_F20
/* 0x84 VK_F21            */ , PMSCAN_F21
/* 0x85 VK_F22            */ , PMSCAN_F22
/* 0x86 VK_F23            */ , PMSCAN_F23
/* 0x87 VK_F24            */ , PMSCAN_F24
/* 0x88                   */ , 0x00
/* 0x89                   */ , 0x00
/* 0x8A                   */ , 0x00
/* 0x8B                   */ , 0x00
/* 0x8C                   */ , 0x00
/* 0x8D                   */ , 0x00
/* 0x8E                   */ , 0x00
/* 0x8F                   */ , 0x00
/* 0x90 VK_NUMLOCK        */ , PMSCAN_NUMLOCK
/* 0x91 VK_SCROLL         */ , PMSCAN_SCROLLLOCK
/* 0x92                   */ , 0x00
/* 0x93                   */ , 0x00
/* 0x94                   */ , 0x00
/* 0x95                   */ , 0x00
/* 0x96                   */ , 0x00
/* 0x97                   */ , 0x00
/* 0x98                   */ , 0x00
/* 0x99                   */ , 0x00
/* 0x9A                   */ , 0x00
/* 0x9B                   */ , 0x00
/* 0x9C                   */ , 0x00
/* 0x9D                   */ , 0x00
/* 0x9E                   */ , 0x00
/* 0x9F                   */ , 0x00
/* 0xA0 VK_LSHIFT         */ , PMSCAN_SHIFTLEFT
/* 0xA1 VK_RSHIFT         */ , PMSCAN_SHIFTRIGHT
/* 0xA2 VK_LCONTROL       */ , PMSCAN_CTRLLEFT
/* 0xA3 VK_RCONTROL       */ , PMSCAN_CTRLRIGHT
/* 0xA4 VK_LMENU          */ , PMSCAN_ALTLEFT
/* 0xA5 VK_RMENU          */ , PMSCAN_ALTRIGHT
/* 0xA6                   */ , 0x00
/* 0xA7                   */ , 0x00
/* 0xA8                   */ , 0x00
/* 0xA9                   */ , 0x00
/* 0xAA                   */ , 0x00
/* 0xAB                   */ , 0x00
/* 0xAC                   */ , 0x00
/* 0xAD                   */ , 0x00
/* 0xAE                   */ , 0x00
/* 0xAF                   */ , 0x00
/* 0xB0                   */ , 0x00
/* 0xB1                   */ , 0x00
/* 0xB2                   */ , 0x00
/* 0xB3                   */ , 0x00
/* 0xB4                   */ , 0x00
/* 0xB5                   */ , 0x00
/* 0xB6                   */ , 0x00
/* 0xB7                   */ , 0x00
/* 0xB8                   */ , 0x00
/* 0xB9                   */ , 0x00
/* 0xBA VK_SEMICOLON      */ , PMSCAN_SEMICOLON
/* 0xBB VK_EQUAL          */ , PMSCAN_EQUAL
/* 0xBC VK_COMMA          */ , PMSCAN_COMMA
/* 0xBD VK_HYPHEN         */ , PMSCAN_HYPHEN
/* 0xBE VK_PERIOD         */ , PMSCAN_PERIOD
/* 0xBF VK_SLASH          */ , PMSCAN_SLASH
/* 0xC0 VK_GRAVE          */ , PMSCAN_GRAVE
/* 0xC1 VK_FFC1           */ , PMSCAN_JEXTRA
/* 0xC2 VK_YEN            */ , PMSCAN_YEN
/* 0xC3                   */ , 0x00
/* 0xC4                   */ , 0x00
/* 0xC5                   */ , 0x00
/* 0xC6                   */ , 0x00
/* 0xC7                   */ , 0x00
/* 0xC8                   */ , 0x00
/* 0xC9                   */ , 0x00
/* 0xCA                   */ , 0x00
/* 0xCB                   */ , 0x00
/* 0xCC                   */ , 0x00
/* 0xCD                   */ , 0x00
/* 0xCE                   */ , 0x00
/* 0xCF                   */ , 0x00
/* 0xD0                   */ , 0x00
/* 0xD1                   */ , 0x00
/* 0xD2                   */ , 0x00
/* 0xD3                   */ , 0x00
/* 0xD4                   */ , 0x00
/* 0xD5                   */ , 0x00
/* 0xD6                   */ , 0x00
/* 0xD7                   */ , 0x00
/* 0xD8                   */ , 0x00
/* 0xD9                   */ , 0x00
/* 0xDA                   */ , 0x00
/* 0xDB VK_BRACKETLEFT    */ , PMSCAN_BRACKETLEFT
/* 0xDC VK_BACKSLASH      */ , PMSCAN_BACKSLASH
/* 0xDD VK_BRACKETRIGHT   */ , PMSCAN_BRACKETRIGHT
/* 0xDE VK_QUOTESINGLE    */ , PMSCAN_QUOTESINGLE
/* 0xDF                   */ , 0x00
/* 0xE0                   */ , 0x00
/* 0xE1                   */ , 0x00
/* 0xE2 VK_EXTRA          */ , PMSCAN_EXTRA
/* 0xE3                   */ , 0x00
/* 0xE4                   */ , 0x00
/* 0xE5                   */ , 0x00
/* 0xE6                   */ , 0x00
/* 0xE7                   */ , 0x00
/* 0xE8                   */ , 0x00
/* 0xE9                   */ , 0x00
/* 0xEA                   */ , 0x00
/* 0xEB                   */ , 0x00
/* 0xEC                   */ , 0x00
/* 0xED                   */ , 0x00
/* 0xEE                   */ , 0x00
/* 0xEF                   */ , 0x00
/* 0xF0                   */ , 0x00
/* 0xF1                   */ , 0x00
/* 0xF2                   */ , 0x00
/* 0xF3                   */ , 0x00
/* 0xF4                   */ , 0x00
/* 0xF5                   */ , 0x00
/* 0xF6 VK_ATTN           */ , 0x00
/* 0xF7 VK_CRSEL          */ , 0x00
/* 0xF8 VK_EXSEL          */ , 0x00
/* 0xF9 VK_EREOF          */ , PMSCAN_ERASEEOF
/* 0xFA VK_PLAY           */ , 0x00
/* 0xFB VK_ZOOM           */ , 0x00
/* 0xFC VK_NONAME         */ , 0x00
/* 0xFD VK_PA1            */ , PMSCAN_PA1
/* 0xFE VK_OEM_CLEAR      */ , 0x00
/* 0xFF                   */ , 0x00
                             };

//******************************************************************************
//******************************************************************************
void WIN32API KeyTranslatePMToWinBuf(BYTE *pmkey, BYTE *winkey, int nrkeys)
{
   for(int i=1;i<nrkeys;i++) {
       if(abWinVKeyToPMScan[i]) {
            winkey[i] = pmkey[OSLibWinTranslateChar(abWinVKeyToPMScan[i], TC_SCANCODETOVIRTUALKEY, 0)];
       }
   }
   winkey[VK_SHIFT]   = winkey[VK_LSHIFT] | winkey[VK_RSHIFT];
   winkey[VK_CONTROL] = winkey[VK_LCONTROL] | winkey[VK_RCONTROL];
   winkey[VK_MENU]    = winkey[VK_LMENU] | winkey[VK_RMENU];
}
//******************************************************************************
//******************************************************************************
int WIN32API GetKeyboardType( int nTypeFlag)
{
    dprintf(("USER32: GetKeyboardType %x", nTypeFlag));
    return O32_GetKeyboardType(nTypeFlag);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetKeyboardState(PBYTE lpKeyState)
{
 BYTE   PMKeyState[256];
 BOOL   rc;

  dprintf(("USER32: GetKeyboardState %x", lpKeyState));
  memset(PMKeyState, 0, sizeof(PMKeyState));
  memset(lpKeyState, 0, 256);
  rc = OSLibWinGetKeyboardStateTable((PBYTE)&PMKeyState[0] );
////  rc = O32_GetKeyboardState(lpKeyState);
  if(rc == TRUE) 
  {
    	KeyTranslatePMToWinBuf((BYTE *)&PMKeyState[0], lpKeyState, 256);
#ifdef DEBUG
        for(int i=0;i<256;i++) {
            if(PMKeyState[i] & 0x80) {
                dprintf2(("PM  key 0x%0x = %x", i, PMKeyState[i]));
            }
        }
        for(i=0;i<256;i++) {
            if(lpKeyState[i]) {
                dprintf2(("Win key 0x%0x = %x", i, lpKeyState[i]));
            }
        }
#endif
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
 * Author    : SvL
 *****************************************************************************/
int WIN32API ToAscii(UINT   uVirtKey,
                     UINT   uScanCode,
                     PBYTE  lpbKeyState,
                     LPWORD lpwTransKey,
                     UINT   fuState)
{
  dprintf(("USER32:ToAscii (%x,%x,%08xh,%08xh,%x) partially implemented",
           uVirtKey, uScanCode, lpbKeyState, lpwTransKey,  fuState));

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

  /* We have another special case for delete key (XK_Delete) on an
     extended keyboard. X returns a char for it, but Windows doesn't */
  if (uVirtKey == VK_DELETE)
  {
       *(char*)lpwTransKey = 0;
       ret = 0;
  }
  else
  if (uVirtKey == VK_ESCAPE) {
       //NT returns VK_ESCAPE here
       *(char*)lpwTransKey = VK_ESCAPE;
       ret = 1;
  }
  else {
       ULONG shiftstate = 0;

       //TODO: multiple characters returned (DBCS??)

       if(lpbKeyState[VK_LSHIFT]   & 0x80) shiftstate |= TCF_LSHIFT;
       if(lpbKeyState[VK_RSHIFT]   & 0x80) shiftstate |= TCF_RSHIFT;
       if(lpbKeyState[VK_SHIFT]    & 0x80) shiftstate |= TCF_SHIFT;
       if(lpbKeyState[VK_LCONTROL] & 0x80) shiftstate |= TCF_LCONTROL;
       if(lpbKeyState[VK_RCONTROL] & 0x80) shiftstate |= TCF_RCONTROL;
       if(lpbKeyState[VK_CONTROL]  & 0x80) shiftstate |= TCF_CONTROL;
       if(lpbKeyState[VK_LMENU]    & 0x80) shiftstate |= TCF_ALT;
       if(lpbKeyState[VK_RMENU]    & 0x80) shiftstate |= TCF_ALTGR;
       if(lpbKeyState[VK_MENU]     & 0x80) shiftstate |= TCF_ALT;
       if(lpbKeyState[VK_CAPITAL]  & 1)    shiftstate |= TCF_CAPSLOCK;
       if(lpbKeyState[VK_NUMLOCK]  & 1)    shiftstate |= TCF_NUMLOCK;

       //NT only modifies the bytes it returns
       *(char *)lpwTransKey = OSLibWinTranslateChar(uScanCode, TC_SCANCODETOCHAR, shiftstate);

       if(shiftstate & TCF_CONTROL) {
           if(uVirtKey >= VK_A && uVirtKey <= VK_Z) {
               //NT returns key-0x60 (or so it seems) for ctrl-(shift-)-a..z
               if(shiftstate & (TCF_SHIFT|TCF_CAPSLOCK)) {
                    *(char *)lpwTransKey -= 0x40;
               }
               else *(char *)lpwTransKey -= 0x60;
           }
           else
           if(uVirtKey == VK_SPACE) {
               if(shiftstate & TCF_SHIFT) {
                   //NT returns 0 for ctrl-shift-space
                   *lpwTransKey = 0;
               }
           }
           else {
               //NT returns 0 for ctrl-0, ctrl-.
               *lpwTransKey = 0;
           }
       }

       if(*(char *)lpwTransKey == 0) {
            ret = 0;
       }
       else ret = 1;
  }
  dprintf2(("USER32:ToAscii returned %x, len %d", *lpwTransKey, ret));
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
  dprintf(("USER32:ToAsciiEx (%u,%u,%08xh,%08xh,%u,%08x) partially implemented",
         uVirtKey,
         uScanCode,
         lpbKeyState,
         lpwTransKey,
         fuState,
         hkl));

  return ToAscii(uVirtKey, uScanCode, lpbKeyState, lpwTransKey, fuState);
}
/*****************************************************************************
 * Name      : int WIN32API ToUnicode
 * Purpose   : The ToUnicode function translates the specified virtual-key code
 *             and keyboard state to the corresponding Unicode character or characters.
 * Parameters: UINT   wVirtKey   virtual-key code
 *             UINT   wScanCode  scan code
 *             PBYTE  lpKeyState address of key-state array
 *             LPWSTR pwszBuff   buffer for translated key
 *             int    cchBuff    size of translated key buffer
 *             UINT   wFlags     set of function-conditioning flags
 * Variables :
 * Result    : - 1 The specified virtual key is a dead-key character (accent or
 *                 diacritic). This value is returned regardless of the keyboard
 *                 layout, even if several characters have been typed and are
 *                 stored in the keyboard state. If possible, even with Unicode
 *                 keyboard layouts, the function has written a spacing version of
 *                 the dead-key character to the buffer specified by pwszBuffer.
 *                 For example, the function writes the character SPACING ACUTE
 *                 (0x00B4), rather than the character NON_SPACING ACUTE (0x0301).
 *               0 The specified virtual key has no translation for the current
 *                 state of the keyboard. Nothing was written to the buffer
 *                 specified by pwszBuffer.
 *               1 One character was written to the buffer specified by pwszBuffer.
 *       2 or more Two or more characters were written to the buffer specified by
 *                 pwszBuff. The most common cause for this is that a dead-key
 *                 character (accent or diacritic) stored in the keyboard layout
 *                 could not be combined with the specified virtual key to form a
 *                 single character.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
int WIN32API ToUnicode(UINT   uVirtKey,
                          UINT   uScanCode,
                          PBYTE  lpKeyState,
                          LPWSTR pwszBuff,
                          int    cchBuff,
                          UINT   wFlags)
{
  dprintf(("USER32:ToUnicode (%u,%u,%08xh,%08xh,%u,%08x) not implemented.\n",
         uVirtKey,
         uScanCode,
         lpKeyState,
         pwszBuff,
         cchBuff,
         wFlags));

  return (0);
}
/*****************************************************************************
 * Name      : UINT WIN32API GetKBCodePage
 * Purpose   : The GetKBCodePage function is provided for compatibility with
 *             earlier versions of Windows. In the Win32 application programming
 *             interface (API) it just calls the GetOEMCP function.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is an OEM code-page
 *             identifier, or it is the default identifier if the registry
 *             value is not readable. For a list of OEM code-page identifiers,
 *             see GetOEMCP.
 * Remark    :
 * Status    : COMPLETELY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

UINT WIN32API GetKBCodePage(VOID)
{
  return (GetOEMCP());
}
//******************************************************************************
//******************************************************************************
int WIN32API GetKeyNameTextA( LPARAM lParam, LPSTR lpString, int  nSize)
{
    dprintf(("USER32:  GetKeyNameTextA\n"));
    return O32_GetKeyNameText(lParam,lpString,nSize);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetKeyNameTextW( LPARAM lParam, LPWSTR lpString, int  nSize)
{
    dprintf(("USER32:  GetKeyNameTextW DOES NOT WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_GetKeyNameText(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
SHORT WIN32API GetKeyState( int nVirtKey)
{
    dprintf2(("USER32: GetKeyState %x", nVirtKey));
    return O32_GetKeyState(nVirtKey);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API GetAsyncKeyState(INT nVirtKey)
{
    dprintf2(("USER32: GetAsyncKeyState %x", nVirtKey));
    return O32_GetAsyncKeyState(nVirtKey);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API MapVirtualKeyA( UINT uCode, UINT  uMapType)
{
    dprintf(("USER32: MapVirtualKeyA %x %x", uCode, uMapType));
    /* A quick fix for Commandos, very incomplete */
    switch (uMapType) {
    case 2:
      if (uCode >= VK_A && uCode <= VK_Z) {
         return 'A' + uCode - VK_A;
      }
      break;
    }
    return O32_MapVirtualKey(uCode,uMapType);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API MapVirtualKeyW( UINT uCode, UINT  uMapType)
{
    dprintf(("USER32:  MapVirtualKeyW\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_MapVirtualKey(uCode,uMapType);
}
/*****************************************************************************
 * Name      : UINT WIN32API MapVirtualKeyExA
 * Purpose   : The MapVirtualKeyEx function translates (maps) a virtual-key
 *             code into a scan code or character value, or translates a scan
 *             code into a virtual-key code. The function translates the codes
 *             using the input language and physical keyboard layout identified
 *             by the given keyboard layout handle.
 * Parameters:
 * Variables :
 * Result    : The return value is either a scan code, a virtual-key code, or
 *             a character value, depending on the value of uCode and uMapType.
 *             If there is no translation, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
UINT WIN32API MapVirtualKeyExA(UINT uCode, UINT uMapType, HKL  dwhkl)
{
  dprintf(("USER32:MapVirtualKeyExA (%u,%u,%08x) partially implemented",
         uCode,
         uMapType,
         dwhkl));

  return MapVirtualKeyA(uCode, uMapType);
}
/*****************************************************************************
 * Name      : UINT WIN32API MapVirtualKeyExW
 * Purpose   : The MapVirtualKeyEx function translates (maps) a virtual-key
 *             code into a scan code or character value, or translates a scan
 *             code into a virtual-key code. The function translates the codes
 *             using the input language and physical keyboard layout identified
 *             by the given keyboard layout handle.
 * Parameters:
 * Variables :
 * Result    : The return value is either a scan code, a virtual-key code, or
 *             a character value, depending on the value of uCode and uMapType.
 *             If there is no translation, the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
UINT WIN32API MapVirtualKeyExW(UINT uCode, UINT uMapType, HKL  dwhkl)
{
  dprintf(("USER32:MapVirtualKeyExW (%u,%u,%08x) partially implemented",
          uCode, uMapType, dwhkl));

  return MapVirtualKeyW(uCode, uMapType);
}
/*****************************************************************************
 * Name      : DWORD WIN32API OemKeyScan
 * Purpose   : The OemKeyScan function maps OEM ASCII codes 0 through 0x0FF
 *             into the OEM scan codes and shift states. The function provides
 *             information that allows a program to send OEM text to another
 *             program by simulating keyboard input.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
DWORD WIN32API OemKeyScan(WORD wOemChar)
{
  dprintf(("USER32:OemKeyScan (%u) not implemented.\n",
         wOemChar));

  return (wOemChar);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RegisterHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT uVirtKey)
{
  dprintf(("USER32:  RegisterHotKey %x %d %x %x not implemented", hwnd, idHotKey, fuModifiers, uVirtKey));
  hwnd = Win32ToOS2Handle(hwnd);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnregisterHotKey(HWND hwnd, int idHotKey)
{
  dprintf(("USER32:  UnregisterHotKey %x %d not implemented", hwnd, idHotKey));
  hwnd = Win32ToOS2Handle(hwnd);

  return(TRUE);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
WORD WIN32API VkKeyScanA( char ch)
{
    dprintf(("USER32: VkKeyScanA %x", ch));
    return O32_VkKeyScan(ch);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API VkKeyScanW( WCHAR wch)
{
    dprintf(("USER32:  VkKeyScanW %x", wch));
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_VkKeyScan((char)wch);
}
/*****************************************************************************
 * Name      : SHORT WIN32API VkKeyScanExW
 * Purpose   : The VkKeyScanEx function translates a character to the
 *             corresponding virtual-key code and shift state. The function
 *             translates the character using the input language and physical
 *             keyboard layout identified by the given keyboard layout handle.
 * Parameters: UINT uChar character to translate
 *             HKL  hkl   keyboard layout handle
 * Variables :
 * Result    : see docs
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
WORD WIN32API VkKeyScanExW(WCHAR uChar,
                           HKL   hkl)
{
  dprintf(("USER32:VkKeyScanExW (%u,%08x) partially implemented",
          uChar,hkl));

  return VkKeyScanW(uChar);
}
/*****************************************************************************
 * Name      : SHORT WIN32API VkKeyScanExA
 * Purpose   : The VkKeyScanEx function translates a character to the
 *             corresponding virtual-key code and shift state. The function
 *             translates the character using the input language and physical
 *             keyboard layout identified by the given keyboard layout handle.
 * Parameters: UINT uChar character to translate
 *             HKL  hkl   keyboard layout handle
 * Variables :
 * Result    : see docs
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
WORD WIN32API VkKeyScanExA(CHAR uChar,
                           HKL  hkl)
{
  dprintf(("USER32:VkKeyScanExA (%u,%08x) partially implemented",
         uChar, hkl));

  return VkKeyScanA(uChar);
}
/*****************************************************************************
 * Name      : VOID WIN32API keybd_event
 * Purpose   : The keybd_event function synthesizes a keystroke. The system
 *             can use such a synthesized keystroke to generate a WM_KEYUP or
 *             WM_KEYDOWN message. The keyboard driver's interrupt handler calls
 *             the keybd_event function.
 * Parameters: BYTE  bVk         virtual-key code

 *             BYTE  bScan       hardware scan code
 *             DWORD dwFlags     flags specifying various function options
 *             DWORD dwExtraInfo additional data associated with keystroke
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
VOID WIN32API keybd_event (BYTE bVk,
                           BYTE bScan,
                           DWORD dwFlags,
                           DWORD dwExtraInfo)
{
  dprintf(("USER32:keybd_event (%u,%u,%08xh,%08x) not implemented.\n",
         bVk,
         bScan,
         dwFlags,
         dwExtraInfo));
}
/*****************************************************************************
 * Name      : HLK WIN32API LoadKeyboardLayoutA
 * Purpose   : The LoadKeyboardLayout function loads a new keyboard layout into
 *             the system. Several keyboard layouts can be loaded at a time, but
 *             only one per process is active at a time. Loading multiple keyboard
 *             layouts makes it possible to rapidly switch between layouts.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *               keyboard layout.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HKL WIN32API LoadKeyboardLayoutA(LPCTSTR pwszKLID,
                                 UINT    Flags)
{
  dprintf(("USER32:LeadKeyboardLayoutA (%s,%u) not implemented.\n",
         pwszKLID,
         Flags));

  return (NULL);
}
/*****************************************************************************
 * Name      : HLK WIN32API LoadKeyboardLayoutW
 * Purpose   : The LoadKeyboardLayout function loads a new keyboard layout into
 *             the system. Several keyboard layouts can be loaded at a time, but
 *             only one per process is active at a time. Loading multiple keyboard
 *             layouts makes it possible to rapidly switch between layouts.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *               keyboard layout.
 *             If the function fails, the return value is NULL. To get extended
 *               error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
HKL WIN32API LoadKeyboardLayoutW(LPCWSTR pwszKLID,
                                 UINT    Flags)
{
  dprintf(("USER32:LeadKeyboardLayoutW (%s,%u) not implemented.\n",
         pwszKLID,
         Flags));

  return (NULL);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ActivateKeyboardLayout(HKL hkl, UINT fuFlags)
{
  dprintf(("USER32:  ActivateKeyboardLayout, not implemented\n"));
  return(TRUE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API UnloadKeyboardLayout
 * Purpose   : The UnloadKeyboardLayout function removes a keyboard layout.
 * Parameters: HKL hkl handle of keyboard layout
 * Variables :
 * Result    : If the function succeeds, the return value is the handle of the
 *             keyboard layout; otherwise, it is NULL. To get extended error
 *             information, use the GetLastError function.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
BOOL WIN32API UnloadKeyboardLayout (HKL hkl)
{
  dprintf(("USER32:UnloadKeyboardLayout (%08x) not implemented.\n",
         hkl));

  return (0);
}
//******************************************************************************
//******************************************************************************
