/* $Id: winkeyboard.cpp,v 1.22 2001-10-25 13:16:57 phaller Exp $ */
/*
 * Win32 <-> PM key translation
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>

#include <string.h>
#include <stdio.h>
#include <winkeyboard.h>
#include "oslibwin.h"
#include <heapstring.h>
#include <pmscan.h>
#include <winscan.h>
#include <winuser32.h>
#include "initterm.h"

#define DBG_LOCALLOG    DBG_winkeyboard
#include "dbglocal.h"




ODINDEBUGCHANNEL(USER32-WINKBD)


BOOL OPEN32API _O32_GetKeyboardState( PBYTE lpKeyState );

inline BOOL O32_GetKeyboardState(PBYTE lpKeyState)
{
 BOOL yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = _O32_GetKeyboardState(lpKeyState);
    SetFS(sel);

    return yyrc;
}


BYTE abPMScanToWinVKey[256][2] =
/****************************************************************************/
/* PM Scancode              *    Win32 vkey                 Extended Key     */
/****************************************************************************/
/* 0x00                     */ { 0x00,                      FALSE
/* 0x01 PMSCAN_ESC          */ , VK_ESCAPE                  ,FALSE
/* 0x02 PMSCAN_ONE          */ , VK_1                       ,FALSE
/* 0x03 PMSCAN_TWO          */ , VK_2                       ,FALSE
/* 0x04 PMSCAN_THREE        */ , VK_3                       ,FALSE
/* 0x05 PMSCAN_FOUR         */ , VK_4                       ,FALSE
/* 0x06 PMSCAN_FIVE         */ , VK_5                       ,FALSE
/* 0x07 PMSCAN_SIX          */ , VK_6                       ,FALSE
/* 0x08 PMSCAN_SEVEN        */ , VK_7                       ,FALSE
/* 0x09 PMSCAN_EIGHT        */ , VK_8                       ,FALSE
/* 0x0A PMSCAN_NINE         */ , VK_9                       ,FALSE
/* 0x0B PMSCAN_ZERO         */ , VK_0                       ,FALSE
/* 0x0C PMSCAN_HYPHEN       */ , VK_HYPHEN                  ,FALSE
/* 0x0D PMSCAN_EQUAL        */ , VK_EQUAL                   ,FALSE
/* 0x0E PMSCAN_BACKSPACE    */ , VK_BACK                    ,FALSE
/* 0x0F PMSCAN_TAB          */ , VK_TAB                     ,FALSE
/* 0x10 PMSCAN_Q            */ , VK_Q                       ,FALSE
/* 0x11 PMSCAN_W            */ , VK_W                       ,FALSE
/* 0x12 PMSCAN_E            */ , VK_E                       ,FALSE
/* 0x13 PMSCAN_R            */ , VK_R                       ,FALSE
/* 0x14 PMSCAN_T            */ , VK_T                       ,FALSE
/* 0x15 PMSCAN_Y            */ , VK_Y                       ,FALSE
/* 0x16 PMSCAN_U            */ , VK_U                       ,FALSE
/* 0x17 PMSCAN_I            */ , VK_I                       ,FALSE
/* 0x18 PMSCAN_O            */ , VK_O                       ,FALSE
/* 0x19 PMSCAN_P            */ , VK_P                       ,FALSE
/* 0x1A PMSCAN_BRACKETLEFT  */ , VK_BRACKETLEFT             ,FALSE
/* 0x1B PMSCAN_BRACKETRIGHT */ , VK_BRACKETRIGHT            ,FALSE
/* 0x1C PMSCAN_ENTER        */ , VK_RETURN                  ,FALSE
/* 0x1D PMSCAN_CTRLLEFT     */ , VK_LCONTROL                ,FALSE
/* 0x1E PMSCAN_A            */ , VK_A                       ,FALSE
/* 0x1F PMSCAN_S            */ , VK_S                       ,FALSE
/* 0x20 PMSCAN_D            */ , VK_D                       ,FALSE
/* 0x21 PMSCAN_F            */ , VK_F                       ,FALSE
/* 0x22 PMSCAN_G            */ , VK_G                       ,FALSE
/* 0x23 PMSCAN_H            */ , VK_H                       ,FALSE
/* 0x24 PMSCAN_J            */ , VK_J                       ,FALSE
/* 0x25 PMSCAN_K            */ , VK_K                       ,FALSE
/* 0x26 PMSCAN_L            */ , VK_L                       ,FALSE
/* 0x27 PMSCAN_SEMICOLON    */ , VK_SEMICOLON               ,FALSE
/* 0x28 PMSCAN_QUOTESINGLE  */ , VK_QUOTESINGLE             ,FALSE
/* 0x29 PMSCAN_GRAVE        */ , VK_GRAVE                   ,FALSE
/* 0x2A PMSCAN_SHIFTLEFT    */ , VK_LSHIFT                  ,FALSE
/* 0x2B PMSCAN_BACKSLASH    */ , VK_BACKSLASH               ,FALSE
/* 0x2C PMSCAN_Z            */ , VK_Z                       ,FALSE
/* 0x2D PMSCAN_X            */ , VK_X                       ,FALSE
/* 0x2E PMSCAN_C            */ , VK_C                       ,FALSE
/* 0x2F PMSCAN_V            */ , VK_V                       ,FALSE
/* 0x30 PMSCAN_B            */ , VK_B                       ,FALSE
/* 0x31 PMSCAN_N            */ , VK_N                       ,FALSE
/* 0x32 PMSCAN_M            */ , VK_M                       ,FALSE
/* 0x33 PMSCAN_COMMA        */ , VK_COMMA                   ,FALSE
/* 0x34 PMSCAN_PERIOD       */ , VK_PERIOD                  ,FALSE
/* 0x35 PMSCAN_SLASH        */ , VK_SLASH                   ,FALSE
/* 0x36 PMSCAN_SHIFTRIGHT   */ , VK_RSHIFT                  ,FALSE
/* 0x37 PMSCAN_PADASTERISK  */ , VK_MULTIPLY                ,FALSE
/* 0x38 PMSCAN_ALTLEFT      */ , VK_LMENU                   ,FALSE
/* 0x39 PMSCAN_SPACE        */ , VK_SPACE                   ,FALSE
/* 0x3A PMSCAN_CAPSLOCK     */ , VK_CAPITAL                 ,FALSE
/* 0x3B PMSCAN_F1           */ , VK_F1                      ,FALSE
/* 0x3C PMSCAN_F2           */ , VK_F2                      ,FALSE
/* 0x3D PMSCAN_F3           */ , VK_F3                      ,FALSE
/* 0x3E PMSCAN_F4           */ , VK_F4                      ,FALSE
/* 0x3F PMSCAN_F5           */ , VK_F5                      ,FALSE
/* 0x40 PMSCAN_F6           */ , VK_F6                      ,FALSE
/* 0x41 PMSCAN_F7           */ , VK_F7                      ,FALSE
/* 0x42 PMSCAN_F8           */ , VK_F8                      ,FALSE
/* 0x43 PMSCAN_F9           */ , VK_F9                      ,FALSE
/* 0x44 PMSCAN_F10          */ , VK_F10                     ,FALSE
/* 0x45 PMSCAN_NUMLOCK      */ , VK_NUMLOCK                 ,TRUE
/* 0x46 PMSCAN_SCROLLLOCK   */ , VK_SCROLL                  ,FALSE
/* 0x47 PMSCAN_PAD7         */ , VK_NUMPAD7                 ,FALSE
/* 0x48 PMSCAN_PAD8         */ , VK_NUMPAD8                 ,FALSE
/* 0x49 PMSCAN_PAD9         */ , VK_NUMPAD9                 ,FALSE
/* 0x4A PMSCAN_PADMINUS     */ , VK_SUBTRACT                ,FALSE
/* 0x4B PMSCAN_PAD4         */ , VK_NUMPAD4                 ,FALSE
/* 0x4C PMSCAN_PAD5         */ , VK_NUMPAD5                 ,FALSE
/* 0x4D PMSCAN_PAD6         */ , VK_NUMPAD6                 ,FALSE
/* 0x4E PMSCAN_PADPLUS      */ , VK_ADD                     ,FALSE
/* 0x4F PMSCAN_PAD1         */ , VK_NUMPAD1                 ,FALSE
/* 0x50 PMSCAN_PAD2         */ , VK_NUMPAD2                 ,FALSE
/* 0x51 PMSCAN_PAD3         */ , VK_NUMPAD3                 ,FALSE
/* 0x52 PMSCAN_PAD0         */ , VK_NUMPAD0                 ,FALSE
/* 0x53 PMSCAN_PADPERIOD    */ , VK_DECIMAL                 ,FALSE
/* 0x54 PMSCAN_SYSREQ       */ , 0x00                       ,FALSE
/* 0x55 PMSCAN_RESET        */ , 0x00                       ,FALSE
/* 0x56 PMSCAN_EXTRA        */ , VK_EXTRA                   ,FALSE
/* 0x57 PMSCAN_F11          */ , VK_F11                     ,FALSE
/* 0x58 PMSCAN_F12          */ , VK_F12                     ,FALSE
/* 0x59 PMSCAN_BACKTAB      */ , 0x00                       ,FALSE
/* 0x5A PMSCAN_PADENTER     */ , VK_RETURN                  ,TRUE
/* 0x5B PMSCAN_CTRLRIGHT    */ , VK_RCONTROL                ,TRUE
/* 0x5C PMSCAN_PADSLASH     */ , VK_DIVIDE                  ,TRUE
/* 0x5D PMSCAN_PRINT        */ , VK_PRINT                   ,TRUE
/* 0x5E PMSCAN_ALTRIGHT     */ , VK_RMENU                   ,TRUE
/* 0x5F PMSCAN_PAUSE        */ , VK_PAUSE                   ,FALSE
/* 0x60 PMSCAN_HOME         */ , VK_HOME                    ,TRUE
/* 0x61 PMSCAN_UP           */ , VK_UP                      ,TRUE
/* 0x62 PMSCAN_PAGEUP       */ , VK_PRIOR                   ,TRUE
/* 0x63 PMSCAN_LEFT         */ , VK_LEFT                    ,TRUE
/* 0x64 PMSCAN_RIGHT        */ , VK_RIGHT                   ,TRUE
/* 0x65 PMSCAN_END          */ , VK_END                     ,TRUE
/* 0x66 PMSCAN_DOWN         */ , VK_DOWN                    ,TRUE
/* 0x67 PMSCAN_PAGEDOWN     */ , VK_NEXT                    ,TRUE
/* 0x68 PMSCAN_INSERT       */ , VK_INSERT                  ,TRUE
/* 0x69 PMSCAN_DELETE       */ , VK_DELETE                  ,TRUE
/* 0x6A PMSCAN_F23          */ , VK_F23                     ,FALSE
/* 0x6B PMSCAN_F24          */ , VK_F24                     ,FALSE
/* 0x6C PMSCAN_SYSMEM       */ , 0x00                       ,FALSE
/* 0x6D PMSCAN_ERASEEOF     */ , VK_EREOF                   ,FALSE
/* 0x6E PMSCAN_BREAK        */ , VK_CANCEL                  ,TRUE
/* 0x6F PMSCAN_MOVEWIN      */ , 0x00                       ,FALSE
/* 0x70 PMSCAN_NLS3         */ , 0x00                       ,FALSE
/* 0x71 PMSCAN_HELP         */ , VK_HELP                    ,FALSE
/* 0x72 PMSCAN_TASKMAN      */ , 0x00                       ,FALSE
/* 0x73 PMSCAN_B11          */ , 0x00                       ,FALSE
/* 0x74 PMSCAN_JUMP         */ , 0x00                       ,FALSE
/* 0x75 PMSCAN_MINWIN       */ , 0x00                       ,FALSE
/* 0x76 PMSCAN_CLEAR        */ , 0x00                       ,FALSE
/* 0x77 PMSCAN_77           */ , 0x00                       ,FALSE
/* 0x78 PMSCAN_78           */ , 0x00                       ,FALSE
/* 0x79 PMSCAN_NLS2         */ , 0x00                       ,FALSE
/* 0x7a PMSCAN_SIZE         */ , 0x00                       ,FALSE
/* 0x7b PMSCAN_NLS1         */ , 0x00                       ,FALSE
/* 0x7c PMSCAN_APPLICATION  */ , VK_APPS                    ,TRUE
/* 0x7d PMSCAN_E13          */ , 0x00                       ,FALSE
/* 0x7e PMSCAN_WINLEFT      */ , VK_LWIN                    ,TRUE
/* 0x7f PMSCAN_WINRIGHT     */ , VK_RWIN                    ,TRUE
/* 0x80 PMSCAN_PA1          */ , VK_PA1                     ,FALSE
/* 0x81 PMSCAN_F13          */ , VK_F13                     ,FALSE
/* 0x82 PMSCAN_F14          */ , VK_F14                     ,FALSE
/* 0x83 PMSCAN_F15          */ , VK_F15                     ,FALSE
/* 0x84 PMSCAN_PA2          */ , 0x00                       ,FALSE
/* 0x85 PMSCAN_PA3          */ , 0x00                       ,FALSE
/* 0x86 PMSCAN_SPACEBREAK   */ , 0x00                       ,FALSE
/* 0x87 PMSCAN_TABRIGHT     */ , 0x00                       ,FALSE
/* 0x88 PMSCAN_NOOP         */ , 0x00                       ,FALSE
/* 0x89 PMSCAN_F16          */ , VK_F16                     ,FALSE
/* 0x8a PMSCAN_F17          */ , VK_F17                     ,FALSE
/* 0x8b PMSCAN_F18          */ , VK_F18                     ,FALSE
/* 0x8c PMSCAN_F19          */ , VK_F19                     ,FALSE
/* 0x8d PMSCAN_F20          */ , VK_F20                     ,FALSE
/* 0x8e PMSCAN_F21          */ , VK_F21                     ,FALSE
/* 0x8f PMSCAN_F22          */ , VK_F22                     ,FALSE
/* 0x90                     */ , 0x00                       ,FALSE
/* 0x91                     */ , 0x00                       ,FALSE
/* 0x92                     */ , 0x00                       ,FALSE
/* 0x93                     */ , 0x00                       ,FALSE
/* 0x94                     */ , 0x00                       ,FALSE
/* 0x95                     */ , 0x00                       ,FALSE
/* 0x96                     */ , 0x00                       ,FALSE
/* 0x97                     */ , 0x00                       ,FALSE
/* 0x98                     */ , 0x00                       ,FALSE
/* 0x99                     */ , 0x00                       ,FALSE
/* 0x9A                     */ , 0x00                       ,FALSE
/* 0x9B                     */ , 0x00                       ,FALSE
/* 0x9C                     */ , 0x00                       ,FALSE
/* 0x9D                     */ , 0x00                       ,FALSE
/* 0x9E                     */ , 0x00                       ,FALSE
/* 0x9F                     */ , 0x00                       ,FALSE
/* 0xA0                     */ , 0x00                       ,FALSE
/* 0xA1                     */ , 0x00                       ,FALSE
/* 0xA2                     */ , 0x00                       ,FALSE
/* 0xA3                     */ , 0x00                       ,FALSE
/* 0xA4                     */ , 0x00                       ,FALSE
/* 0xA5                     */ , 0x00                       ,FALSE
/* 0xA6                     */ , 0x00                       ,FALSE
/* 0xA7                     */ , 0x00                       ,FALSE
/* 0xA8                     */ , 0x00                       ,FALSE
/* 0xA9                     */ , 0x00                       ,FALSE
/* 0xAA                     */ , 0x00                       ,FALSE
/* 0xAB                     */ , 0x00                       ,FALSE
/* 0xAC                     */ , 0x00                       ,FALSE
/* 0xAD                     */ , 0x00                       ,FALSE
/* 0xAE                     */ , 0x00                       ,FALSE
/* 0xAF                     */ , 0x00                       ,FALSE
/* 0xB0                     */ , 0x00                       ,FALSE
/* 0xB1                     */ , 0x00                       ,FALSE
/* 0xB2                     */ , 0x00                       ,FALSE
/* 0xB3                     */ , 0x00                       ,FALSE
/* 0xB4                     */ , 0x00                       ,FALSE
/* 0xB5                     */ , 0x00                       ,FALSE
/* 0xB6                     */ , 0x00                       ,FALSE
/* 0xB7                     */ , 0x00                       ,FALSE
/* 0xB8                     */ , 0x00                       ,FALSE
/* 0xB9                     */ , 0x00                       ,FALSE
/* 0xBA                     */ , 0x00                       ,FALSE
/* 0xBB                     */ , 0x00                       ,FALSE
/* 0xBC                     */ , 0x00                       ,FALSE
/* 0xBD                     */ , 0x00                       ,FALSE
/* 0xBE                     */ , 0x00                       ,FALSE
/* 0xBF                     */ , 0x00                       ,FALSE
/* 0xC0                     */ , 0x00                       ,FALSE
/* 0xC1                     */ , 0x00                       ,FALSE
/* 0xC2                     */ , 0x00                       ,FALSE
/* 0xC3                     */ , 0x00                       ,FALSE
/* 0xC4                     */ , 0x00                       ,FALSE
/* 0xC5                     */ , 0x00                       ,FALSE
/* 0xC6                     */ , 0x00                       ,FALSE
/* 0xC7                     */ , 0x00                       ,FALSE
/* 0xC8                     */ , 0x00                       ,FALSE
/* 0xC9                     */ , 0x00                       ,FALSE
/* 0xCA                     */ , 0x00                       ,FALSE
/* 0xCB                     */ , 0x00                       ,FALSE
/* 0xCC                     */ , 0x00                       ,FALSE
/* 0xCD                     */ , 0x00                       ,FALSE
/* 0xCE                     */ , 0x00                       ,FALSE
/* 0xCF                     */ , 0x00                       ,FALSE
/* 0xD0                     */ , 0x00                       ,FALSE
/* 0xD1                     */ , 0x00                       ,FALSE
/* 0xD2                     */ , 0x00                       ,FALSE
/* 0xD3                     */ , 0x00                       ,FALSE
/* 0xD4                     */ , 0x00                       ,FALSE
/* 0xD5                     */ , 0x00                       ,FALSE
/* 0xD6                     */ , 0x00                       ,FALSE
/* 0xD7                     */ , 0x00                       ,FALSE
/* 0xD8                     */ , 0x00                       ,FALSE
/* 0xD9                     */ , 0x00                       ,FALSE
/* 0xDA                     */ , 0x00                       ,FALSE
/* 0xDB                     */ , 0x00                       ,FALSE
/* 0xDC                     */ , 0x00                       ,FALSE
/* 0xDD                     */ , 0x00                       ,FALSE
/* 0xDE                     */ , 0x00                       ,FALSE
/* 0xDF                     */ , 0x00                       ,FALSE
/* 0xE0                     */ , 0x00                       ,FALSE
/* 0xE1                     */ , 0x00                       ,FALSE
/* 0xE2                     */ , 0x00                       ,FALSE
/* 0xE3                     */ , 0x00                       ,FALSE
/* 0xE4                     */ , 0x00                       ,FALSE
/* 0xE5                     */ , 0x00                       ,FALSE
/* 0xE6                     */ , 0x00                       ,FALSE
/* 0xE7                     */ , 0x00                       ,FALSE
/* 0xE8                     */ , 0x00                       ,FALSE
/* 0xE9                     */ , 0x00                       ,FALSE
/* 0xEA                     */ , 0x00                       ,FALSE
/* 0xEB                     */ , 0x00                       ,FALSE
/* 0xEC                     */ , 0x00                       ,FALSE
/* 0xED                     */ , 0x00                       ,FALSE
/* 0xEE                     */ , 0x00                       ,FALSE
/* 0xEF                     */ , 0x00                       ,FALSE
/* 0xF0                     */ , 0x00                       ,FALSE
/* 0xF1                     */ , 0x00                       ,FALSE
/* 0xF2                     */ , 0x00                       ,FALSE
/* 0xF3                     */ , 0x00                       ,FALSE
/* 0xF4                     */ , 0x00                       ,FALSE
/* 0xF5                     */ , 0x00                       ,FALSE
/* 0xF6                     */ , 0x00                       ,FALSE
/* 0xF7                     */ , 0x00                       ,FALSE
/* 0xF8                     */ , 0x00                       ,FALSE
/* 0xF9                     */ , 0x00                       ,FALSE
/* 0xFA                     */ , 0x00                       ,FALSE
/* 0xFB                     */ , 0x00                       ,FALSE
/* 0xFC                     */ , 0x00                       ,FALSE
/* 0xFD                     */ , 0x00                       ,FALSE
/* 0xFE                     */ , 0x00                       ,FALSE
/* 0xFF                     */ , 0x00                       ,FALSE
                               };

typedef struct tagWinVKeyToPMScan
{
  /* index is the VKey value */
  BYTE  bPMScanCode;
  LPSTR lpstrName;
} WINVKEYTOPMSCAN, *PWINVKEYTOPMSCAN;

WINVKEYTOPMSCAN abWinVKeyToPMScan[256] =
/**********************************************************************/
/* Vkey                   *    Scancode               * Name          */
/**********************************************************************/
/* 0x00                   */ { 0x00                   , NULL
/* 0x01 VK_LBUTTON        */ , 0x00                   , NULL
/* 0x02 VK_RBUTTON        */ , 0x00                   , NULL
/* 0x03 VK_CANCEL         */ , PMSCAN_BREAK           , "Break"
/* 0x04 VK_MBUTTON        */ , 0x00                   , NULL
/* 0x05                   */ , 0x00                   , NULL
/* 0x06                   */ , 0x00                   , NULL
/* 0x07                   */ , 0x00                   , NULL
/* 0x08 VK_BACK           */ , PMSCAN_BACKSPACE       , "Backspace"
/* 0x09 VK_TAB            */ , PMSCAN_TAB             , "Tab"
/* 0x0A                   */ , 0x00                   , NULL
/* 0x0B                   */ , 0x00                   , NULL
/* 0x0C VK_CLEAR          */ , PMSCAN_PAD5            , "Num 5"
/* 0x0D VK_RETURN         */ , PMSCAN_ENTER           , "Enter"
/* 0x0E                   */ , 0x00                   , NULL
/* 0x0F                   */ , 0x00                   , NULL
/* 0x10 VK_SHIFT          */ , PMSCAN_SHIFTLEFT       , "Left Shift"
/* 0x11 VK_CONTROL        */ , PMSCAN_CTRLLEFT        , "Left Control"
/* 0x12 VK_MENU           */ , PMSCAN_ALTLEFT         , "Left Alt"
/* 0x13 VK_PAUSE          */ , PMSCAN_PAUSE           , "Pause"
/* 0x14 VK_CAPITAL        */ , PMSCAN_CAPSLOCK        , "Caps Lock"
/* 0x15                   */ , 0x00                   , NULL
/* 0x16                   */ , 0x00                   , NULL
/* 0x17                   */ , 0x00                   , NULL
/* 0x18                   */ , 0x00                   , NULL
/* 0x19                   */ , 0x00                   , NULL
/* 0x1A                   */ , 0x00                   , NULL
/* 0x1B VK_ESCAPE         */ , PMSCAN_ESC             , "Escape"
/* 0x1C                   */ , 0x00                   , NULL
/* 0x1D                   */ , 0x00                   , NULL
/* 0x1E                   */ , 0x00                   , NULL
/* 0x1F                   */ , 0x00                   , NULL
/* 0x20 VK_SPACE          */ , PMSCAN_SPACE           , "Space"
/* 0x21 VK_PRIOR          */ , PMSCAN_PAGEUP          , "Page Up"
/* 0x22 VK_NEXT           */ , PMSCAN_PAGEDOWN        , "Page Down"
/* 0x23 VK_END            */ , PMSCAN_END             , "End"
/* 0x24 VK_HOME           */ , PMSCAN_HOME            , "Home"
/* 0x25 VK_LEFT           */ , PMSCAN_LEFT            , "Left"
/* 0x26 VK_UP             */ , PMSCAN_UP              , "Up"
/* 0x27 VK_RIGHT          */ , PMSCAN_RIGHT           , "Right"
/* 0x28 VK_DOWN           */ , PMSCAN_DOWN            , "Down"
/* 0x29 VK_SELECT         */ , 0x00                   , NULL
/* 0x2A VK_PRINT          */ , PMSCAN_PRINT           , "Print"
/* 0x2B VK_EXECUTE        */ , 0x00                   , NULL
/* 0x2C VK_SNAPSHOT       */ , PMSCAN_PRINT           , "Print"  // NT4SP6: appears to be printscreen!
/* 0x2D VK_INSERT         */ , PMSCAN_INSERT          , "Insert"
/* 0x2E VK_DELETE         */ , PMSCAN_DELETE          , "Delete"
/* 0x2F VK_HELP           */ , PMSCAN_HELP            , "Help"
/* 0x30 VK_0              */ , PMSCAN_ZERO            , "0"
/* 0x31 VK_1              */ , PMSCAN_ONE             , "1"
/* 0x32 VK_2              */ , PMSCAN_TWO             , "2"
/* 0x33 VK_3              */ , PMSCAN_THREE           , "3"
/* 0x34 VK_4              */ , PMSCAN_FOUR            , "4"
/* 0x35 VK_5              */ , PMSCAN_FIVE            , "5"
/* 0x36 VK_6              */ , PMSCAN_SIX             , "6"
/* 0x37 VK_7              */ , PMSCAN_SEVEN           , "7"
/* 0x38 VK_8              */ , PMSCAN_EIGHT           , "8"
/* 0x39 VK_9              */ , PMSCAN_NINE            , "9"
/* 0x3A                   */ , 0x00                   , NULL
/* 0x3B                   */ , 0x00                   , NULL
/* 0x3C                   */ , 0x00                   , NULL
/* 0x3D                   */ , 0x00                   , NULL
/* 0x3E                   */ , 0x00                   , NULL
/* 0x3F                   */ , 0x00                   , NULL
/* 0x40                   */ , 0x00                   , NULL
/* 0x41 VK_A              */ , PMSCAN_A               , "A"
/* 0x42 VK_B              */ , PMSCAN_B               , "B"
/* 0x43 VK_C              */ , PMSCAN_C               , "C"
/* 0x44 VK_D              */ , PMSCAN_D               , "D"
/* 0x45 VK_E              */ , PMSCAN_E               , "E"
/* 0x46 VK_F              */ , PMSCAN_F               , "F"
/* 0x47 VK_G              */ , PMSCAN_G               , "G"
/* 0x48 VK_H              */ , PMSCAN_H               , "H"
/* 0x49 VK_I              */ , PMSCAN_I               , "I"
/* 0x4A VK_J              */ , PMSCAN_J               , "J"
/* 0x4B VK_K              */ , PMSCAN_K               , "K"
/* 0x4C VK_L              */ , PMSCAN_L               , "L"
/* 0x4D VK_M              */ , PMSCAN_M               , "M"
/* 0x4E VK_N              */ , PMSCAN_N               , "N"
/* 0x4F VK_O              */ , PMSCAN_O               , "O"
/* 0x50 VK_P              */ , PMSCAN_P               , "P"
/* 0x51 VK_Q              */ , PMSCAN_Q               , "Q"
/* 0x52 VK_R              */ , PMSCAN_R               , "R"
/* 0x53 VK_S              */ , PMSCAN_S               , "S"
/* 0x54 VK_T              */ , PMSCAN_T               , "T"
/* 0x55 VK_U              */ , PMSCAN_U               , "U"
/* 0x56 VK_V              */ , PMSCAN_V               , "V"
/* 0x57 VK                */ , PMSCAN_W               , "W"
/* 0x58 VK_X              */ , PMSCAN_X               , "X"
/* 0x59 VK_Y              */ , PMSCAN_Y               , "Y"
/* 0x5A VK_Z              */ , PMSCAN_Z               , "Z"
/* 0x5B VK_LWIN           */ , PMSCAN_WINLEFT         , "Left Win"
/* 0x5C VK_RWIN           */ , PMSCAN_WINRIGHT        , "Right Win"
/* 0x5D VK_APPS           */ , PMSCAN_APPLICATION     , "Application"
/* 0x5E                   */ , 0x00                   , NULL
/* 0x5F                   */ , 0x00                   , NULL
/* 0x60 VK_NUMPAD0        */ , PMSCAN_PAD0            , "Num 0"
/* 0x61 VK_NUMPAD1        */ , PMSCAN_PAD1            , "Num 1"
/* 0x62 VK_NUMPAD2        */ , PMSCAN_PAD2            , "Num 2"
/* 0x63 VK_NUMPAD3        */ , PMSCAN_PAD3            , "Num 3"
/* 0x64 VK_NUMPAD4        */ , PMSCAN_PAD4            , "Num 4"
/* 0x65 VK_NUMPAD5        */ , PMSCAN_PAD5            , "Num 5"
/* 0x66 VK_NUMPAD6        */ , PMSCAN_PAD6            , "Num 6"
/* 0x67 VK_NUMPAD7        */ , PMSCAN_PAD7            , "Num 7"
/* 0x68 VK_NUMPAD8        */ , PMSCAN_PAD8            , "Num 8"
/* 0x69 VK_NUMPAD9        */ , PMSCAN_PAD9            , "Num 9"
/* 0x6A VK_MULTIPLY       */ , PMSCAN_PADASTERISK     , "Num *"
/* 0x6B VK_ADD            */ , PMSCAN_PADPLUS         , "Num +"
/* 0x6C VK_SEPARATOR      */ , 0x00                   , NULL
/* 0x6D VK_SUBTRACT       */ , PMSCAN_PADMINUS        , "Num -"
/* 0x6E VK_DECIMAL        */ , PMSCAN_PADPERIOD       , "Num ."
/* 0x6F VK_DIVIDE         */ , PMSCAN_PADSLASH        , "Num /"
/* 0x70 VK_F1             */ , PMSCAN_F1              , "F1"
/* 0x71 VK_F2             */ , PMSCAN_F2              , "F2"
/* 0x72 VK_F3             */ , PMSCAN_F3              , "F3"
/* 0x73 VK_F4             */ , PMSCAN_F4              , "F4"
/* 0x74 VK_F5             */ , PMSCAN_F5              , "F5"
/* 0x75 VK_F6             */ , PMSCAN_F6              , "F6"
/* 0x76 VK_F7             */ , PMSCAN_F7              , "F7"
/* 0x77 VK_F8             */ , PMSCAN_F8              , "F8"
/* 0x78 VK_F9             */ , PMSCAN_F9              , "F9"
/* 0x79 VK_F10            */ , PMSCAN_F10             , "F10"
/* 0x7A VK_F11            */ , PMSCAN_F11             , "F11"
/* 0x7B VK_F12            */ , PMSCAN_F12             , "F12"
/* 0x7C VK_F13            */ , PMSCAN_F13             , "F13"
/* 0x7D VK_F14            */ , PMSCAN_F14             , "F14"
/* 0x7E VK_F15            */ , PMSCAN_F15             , "F15"
/* 0x7F VK_F16            */ , PMSCAN_F16             , "F16"
/* 0x80 VK_F17            */ , PMSCAN_F17             , "F17"
/* 0x81 VK_F18            */ , PMSCAN_F18             , "F18"
/* 0x82 VK_F19            */ , PMSCAN_F19             , "F19"
/* 0x83 VK_F20            */ , PMSCAN_F20             , "F20"
/* 0x84 VK_F21            */ , PMSCAN_F21             , "F21"
/* 0x85 VK_F22            */ , PMSCAN_F22             , "F22"
/* 0x86 VK_F23            */ , PMSCAN_F23             , "F23"
/* 0x87 VK_F24            */ , PMSCAN_F24             , "F24"
/* 0x88                   */ , 0x00                   , NULL
/* 0x89                   */ , 0x00                   , NULL
/* 0x8A                   */ , 0x00                   , NULL
/* 0x8B                   */ , 0x00                   , NULL
/* 0x8C                   */ , 0x00                   , NULL
/* 0x8D                   */ , 0x00                   , NULL
/* 0x8E                   */ , 0x00                   , NULL
/* 0x8F                   */ , 0x00                   , NULL
/* 0x90 VK_NUMLOCK        */ , PMSCAN_NUMLOCK         , "Num Lock"
/* 0x91 VK_SCROLL         */ , PMSCAN_SCROLLLOCK      , "Scroll Lock"
/* 0x92                   */ , 0x00                   , NULL
/* 0x93                   */ , 0x00                   , NULL
/* 0x94                   */ , 0x00                   , NULL
/* 0x95                   */ , 0x00                   , NULL
/* 0x96                   */ , 0x00                   , NULL
/* 0x97                   */ , 0x00                   , NULL
/* 0x98                   */ , 0x00                   , NULL
/* 0x99                   */ , 0x00                   , NULL
/* 0x9A                   */ , 0x00                   , NULL
/* 0x9B                   */ , 0x00                   , NULL
/* 0x9C                   */ , 0x00                   , NULL
/* 0x9D                   */ , 0x00                   , NULL
/* 0x9E                   */ , 0x00                   , NULL
/* 0x9F                   */ , 0x00                   , NULL
/* 0xA0 VK_LSHIFT         */ , PMSCAN_SHIFTLEFT       , "Left Shift"
/* 0xA1 VK_RSHIFT         */ , PMSCAN_SHIFTRIGHT      , "Right Shift"
/* 0xA2 VK_LCONTROL       */ , PMSCAN_CTRLLEFT        , "Left Control"
/* 0xA3 VK_RCONTROL       */ , PMSCAN_CTRLRIGHT       , "Right Control"
/* 0xA4 VK_LMENU          */ , PMSCAN_ALTLEFT         , "Left Alt"
/* 0xA5 VK_RMENU          */ , PMSCAN_ALTRIGHT        , "Right Alt"
/* 0xA6                   */ , 0x00                   , NULL
/* 0xA7                   */ , 0x00                   , NULL
/* 0xA8                   */ , 0x00                   , NULL
/* 0xA9                   */ , 0x00                   , NULL
/* 0xAA                   */ , 0x00                   , NULL
/* 0xAB                   */ , 0x00                   , NULL
/* 0xAC                   */ , 0x00                   , NULL
/* 0xAD                   */ , 0x00                   , NULL
/* 0xAE                   */ , 0x00                   , NULL
/* 0xAF                   */ , 0x00                   , NULL
/* 0xB0                   */ , 0x00                   , NULL
/* 0xB1                   */ , 0x00                   , NULL
/* 0xB2                   */ , 0x00                   , NULL
/* 0xB3                   */ , 0x00                   , NULL
/* 0xB4                   */ , 0x00                   , NULL
/* 0xB5                   */ , 0x00                   , NULL
/* 0xB6                   */ , 0x00                   , NULL
/* 0xB7                   */ , 0x00                   , NULL
/* 0xB8                   */ , 0x00                   , NULL
/* 0xB9                   */ , 0x00                   , NULL
/* 0xBA VK_SEMICOLON      */ , PMSCAN_SEMICOLON       , ";"
/* 0xBB VK_EQUAL          */ , PMSCAN_EQUAL           , "="
/* 0xBC VK_COMMA          */ , PMSCAN_COMMA           , ","
/* 0xBD VK_HYPHEN         */ , PMSCAN_HYPHEN          , "-"
/* 0xBE VK_PERIOD         */ , PMSCAN_PERIOD          , "."
/* 0xBF VK_SLASH          */ , PMSCAN_SLASH           , "/"
/* 0xC0 VK_GRAVE          */ , PMSCAN_GRAVE           , "'"
/* 0xC1 VK_FFC1           */ , PMSCAN_JEXTRA          , "JExtra" //??
/* 0xC2 VK_YEN            */ , PMSCAN_YEN             , "Yen"
/* 0xC3                   */ , 0x00                   , NULL
/* 0xC4                   */ , 0x00                   , NULL
/* 0xC5                   */ , 0x00                   , NULL
/* 0xC6                   */ , 0x00                   , NULL
/* 0xC7                   */ , 0x00                   , NULL
/* 0xC8                   */ , 0x00                   , NULL
/* 0xC9                   */ , 0x00                   , NULL
/* 0xCA                   */ , 0x00                   , NULL
/* 0xCB                   */ , 0x00                   , NULL
/* 0xCC                   */ , 0x00                   , NULL
/* 0xCD                   */ , 0x00                   , NULL
/* 0xCE                   */ , 0x00                   , NULL
/* 0xCF                   */ , 0x00                   , NULL
/* 0xD0                   */ , 0x00                   , NULL
/* 0xD1                   */ , 0x00                   , NULL
/* 0xD2                   */ , 0x00                   , NULL
/* 0xD3                   */ , 0x00                   , NULL
/* 0xD4                   */ , 0x00                   , NULL
/* 0xD5                   */ , 0x00                   , NULL
/* 0xD6                   */ , 0x00                   , NULL
/* 0xD7                   */ , 0x00                   , NULL
/* 0xD8                   */ , 0x00                   , NULL
/* 0xD9                   */ , 0x00                   , NULL
/* 0xDA                   */ , 0x00                   , NULL
/* 0xDB VK_BRACKETLEFT    */ , PMSCAN_BRACKETLEFT     , "["
/* 0xDC VK_BACKSLASH      */ , PMSCAN_BACKSLASH       , "\\"
/* 0xDD VK_BRACKETRIGHT   */ , PMSCAN_BRACKETRIGHT    , "]"
/* 0xDE VK_QUOTESINGLE    */ , PMSCAN_QUOTESINGLE     , "'"
/* 0xDF                   */ , 0x00                   , NULL
/* 0xE0                   */ , 0x00                   , NULL
/* 0xE1                   */ , 0x00                   , NULL
/* 0xE2 VK_EXTRA          */ , PMSCAN_EXTRA           , "<"
/* 0xE3                   */ , 0x00                   , NULL
/* 0xE4                   */ , 0x00                   , NULL
/* 0xE5                   */ , 0x00                   , NULL
/* 0xE6                   */ , 0x00                   , NULL
/* 0xE7                   */ , 0x00                   , NULL
/* 0xE8                   */ , 0x00                   , NULL
/* 0xE9                   */ , 0x00                   , NULL
/* 0xEA                   */ , 0x00                   , NULL
/* 0xEB                   */ , 0x00                   , NULL
/* 0xEC                   */ , 0x00                   , NULL
/* 0xED                   */ , 0x00                   , NULL
/* 0xEE                   */ , 0x00                   , NULL
/* 0xEF                   */ , 0x00                   , NULL
/* 0xF0                   */ , 0x00                   , NULL
/* 0xF1                   */ , 0x00                   , NULL
/* 0xF2                   */ , 0x00                   , NULL
/* 0xF3                   */ , 0x00                   , NULL
/* 0xF4                   */ , 0x00                   , NULL
/* 0xF5                   */ , 0x00                   , NULL
/* 0xF6 VK_ATTN           */ , 0x00                   , NULL
/* 0xF7 VK_CRSEL          */ , 0x00                   , NULL
/* 0xF8 VK_EXSEL          */ , 0x00                   , NULL
/* 0xF9 VK_EREOF          */ , PMSCAN_ERASEEOF        , "Erase"
/* 0xFA VK_PLAY           */ , 0x00                   , NULL
/* 0xFB VK_ZOOM           */ , 0x00                   , NULL
/* 0xFC VK_NONAME         */ , 0x00                   , NULL
/* 0xFD VK_PA1            */ , PMSCAN_PA1             , "PA1"
/* 0xFE VK_OEM_CLEAR      */ , 0x00                   , NULL
/* 0xFF                   */ , 0x00                   , NULL
                             };


// @@@PH
// Note: windoes uses different scancodes if numlock is pressed
// This is not (yet) reflected here!
BYTE abPMScanToWinScan[256][2] =
/****************************************************************************/
/* PM Scancode              *    Win32 Scancode             Extended Key     */
/****************************************************************************/
/* 0x00                     */ { 0x00,                      FALSE
/* 0x01 PMSCAN_ESC          */ , WINSCAN_ESC                ,FALSE
/* 0x02 PMSCAN_ONE          */ , WINSCAN_ONE                ,FALSE
/* 0x03 PMSCAN_TWO          */ , WINSCAN_TWO                ,FALSE
/* 0x04 PMSCAN_THREE        */ , WINSCAN_THREE              ,FALSE
/* 0x05 PMSCAN_FOUR         */ , WINSCAN_FOUR               ,FALSE
/* 0x06 PMSCAN_FIVE         */ , WINSCAN_FIVE               ,FALSE
/* 0x07 PMSCAN_SIX          */ , WINSCAN_SIX                ,FALSE
/* 0x08 PMSCAN_SEVEN        */ , WINSCAN_SEVEN              ,FALSE
/* 0x09 PMSCAN_EIGHT        */ , WINSCAN_EIGHT              ,FALSE
/* 0x0A PMSCAN_NINE         */ , WINSCAN_NINE               ,FALSE
/* 0x0B PMSCAN_ZERO         */ , WINSCAN_ZERO               ,FALSE
/* 0x0C PMSCAN_HYPHEN       */ , WINSCAN_HYPHEN             ,FALSE
/* 0x0D PMSCAN_EQUAL        */ , WINSCAN_EQUAL              ,FALSE
/* 0x0E PMSCAN_BACKSPACE    */ , WINSCAN_BACKSPACE          ,FALSE
/* 0x0F PMSCAN_TAB          */ , WINSCAN_TAB                ,FALSE
/* 0x10 PMSCAN_Q            */ , WINSCAN_Q                  ,FALSE
/* 0x11 PMSCAN_W            */ , WINSCAN_W                  ,FALSE
/* 0x12 PMSCAN_E            */ , WINSCAN_E                  ,FALSE
/* 0x13 PMSCAN_R            */ , WINSCAN_R                  ,FALSE
/* 0x14 PMSCAN_T            */ , WINSCAN_T                  ,FALSE
/* 0x15 PMSCAN_Y            */ , WINSCAN_Y                  ,FALSE
/* 0x16 PMSCAN_U            */ , WINSCAN_U                  ,FALSE
/* 0x17 PMSCAN_I            */ , WINSCAN_I                  ,FALSE
/* 0x18 PMSCAN_O            */ , WINSCAN_O                  ,FALSE
/* 0x19 PMSCAN_P            */ , WINSCAN_P                  ,FALSE
/* 0x1A PMSCAN_BRACKETLEFT  */ , WINSCAN_BRACKETLEFT        ,FALSE
/* 0x1B PMSCAN_BRACKETRIGHT */ , WINSCAN_BRACKETRIGHT       ,FALSE
/* 0x1C PMSCAN_ENTER        */ , WINSCAN_ENTER              ,FALSE
/* 0x1D PMSCAN_CTRLLEFT     */ , WINSCAN_CTRLLEFT           ,FALSE
/* 0x1E PMSCAN_A            */ , WINSCAN_A                  ,FALSE
/* 0x1F PMSCAN_S            */ , WINSCAN_S                  ,FALSE
/* 0x20 PMSCAN_D            */ , WINSCAN_D                  ,FALSE
/* 0x21 PMSCAN_F            */ , WINSCAN_F                  ,FALSE
/* 0x22 PMSCAN_G            */ , WINSCAN_G                  ,FALSE
/* 0x23 PMSCAN_H            */ , WINSCAN_H                  ,FALSE
/* 0x24 PMSCAN_J            */ , WINSCAN_J                  ,FALSE
/* 0x25 PMSCAN_K            */ , WINSCAN_K                  ,FALSE
/* 0x26 PMSCAN_L            */ , WINSCAN_L                  ,FALSE
/* 0x27 PMSCAN_SEMICOLON    */ , WINSCAN_SEMICOLON          ,FALSE
/* 0x28 PMSCAN_QUOTESINGLE  */ , WINSCAN_QUOTESINGLE        ,FALSE
/* 0x29 PMSCAN_GRAVE        */ , WINSCAN_GRAVE              ,FALSE
/* 0x2A PMSCAN_SHIFTLEFT    */ , WINSCAN_SHIFTLEFT          ,FALSE
/* 0x2B PMSCAN_BACKSLASH    */ , WINSCAN_BACKSLASH          ,FALSE
/* 0x2C PMSCAN_Z            */ , WINSCAN_Z                  ,FALSE
/* 0x2D PMSCAN_X            */ , WINSCAN_X                  ,FALSE
/* 0x2E PMSCAN_C            */ , WINSCAN_C                  ,FALSE
/* 0x2F PMSCAN_V            */ , WINSCAN_V                  ,FALSE
/* 0x30 PMSCAN_B            */ , WINSCAN_B                  ,FALSE
/* 0x31 PMSCAN_N            */ , WINSCAN_N                  ,FALSE
/* 0x32 PMSCAN_M            */ , WINSCAN_M                  ,FALSE
/* 0x33 PMSCAN_COMMA        */ , WINSCAN_COMMA              ,FALSE
/* 0x34 PMSCAN_PERIOD       */ , WINSCAN_PERIOD             ,FALSE
/* 0x35 PMSCAN_SLASH        */ , WINSCAN_SLASH              ,FALSE
/* 0x36 PMSCAN_SHIFTRIGHT   */ , WINSCAN_SHIFTRIGHT         ,FALSE
/* 0x37 PMSCAN_PADASTERISK  */ , WINSCAN_PADASTERISK        ,FALSE
/* 0x38 PMSCAN_ALTLEFT      */ , WINSCAN_ALTLEFT            ,FALSE
/* 0x39 PMSCAN_SPACE        */ , WINSCAN_SPACE              ,FALSE
/* 0x3A PMSCAN_CAPSLOCK     */ , WINSCAN_CAPSLOCK           ,FALSE
/* 0x3B PMSCAN_F1           */ , WINSCAN_F1                 ,FALSE
/* 0x3C PMSCAN_F2           */ , WINSCAN_F2                 ,FALSE
/* 0x3D PMSCAN_F3           */ , WINSCAN_F3                 ,FALSE
/* 0x3E PMSCAN_F4           */ , WINSCAN_F4                 ,FALSE
/* 0x3F PMSCAN_F5           */ , WINSCAN_F5                 ,FALSE
/* 0x40 PMSCAN_F6           */ , WINSCAN_F6                 ,FALSE
/* 0x41 PMSCAN_F7           */ , WINSCAN_F7                 ,FALSE
/* 0x42 PMSCAN_F8           */ , WINSCAN_F8                 ,FALSE
/* 0x43 PMSCAN_F9           */ , WINSCAN_F9                 ,FALSE
/* 0x44 PMSCAN_F10          */ , WINSCAN_F10                ,FALSE
/* 0x45 PMSCAN_NUMLOCK      */ , WINSCAN_NUMLOCK            ,TRUE
/* 0x46 PMSCAN_SCROLLLOCK   */ , WINSCAN_SCROLLLOCK         ,FALSE
/* 0x47 PMSCAN_PAD7         */ , WINSCAN_PAD7               ,FALSE
/* 0x48 PMSCAN_PAD8         */ , WINSCAN_PAD8               ,FALSE
/* 0x49 PMSCAN_PAD9         */ , WINSCAN_PAD9               ,FALSE
/* 0x4A PMSCAN_PADMINUS     */ , WINSCAN_PADMINUS           ,FALSE
/* 0x4B PMSCAN_PAD4         */ , WINSCAN_PAD4               ,FALSE
/* 0x4C PMSCAN_PAD5         */ , WINSCAN_PAD5               ,FALSE
/* 0x4D PMSCAN_PAD6         */ , WINSCAN_PAD6               ,FALSE
/* 0x4E PMSCAN_PADPLUS      */ , WINSCAN_PADPLUS            ,FALSE
/* 0x4F PMSCAN_PAD1         */ , WINSCAN_PAD1               ,FALSE
/* 0x50 PMSCAN_PAD2         */ , WINSCAN_PAD2               ,FALSE
/* 0x51 PMSCAN_PAD3         */ , WINSCAN_PAD3               ,FALSE
/* 0x52 PMSCAN_PAD0         */ , WINSCAN_PAD0               ,FALSE
/* 0x53 PMSCAN_PADPERIOD    */ , WINSCAN_PADPERIOD          ,FALSE
/* 0x54 PMSCAN_SYSREQ       */ , 0x00                       ,FALSE
/* 0x55 PMSCAN_RESET        */ , 0x00                       ,FALSE
/* 0x56 PMSCAN_EXTRA        */ , WINSCAN_EXTRA              ,FALSE
/* 0x57 PMSCAN_F11          */ , WINSCAN_F11                ,FALSE
/* 0x58 PMSCAN_F12          */ , WINSCAN_F12                ,FALSE
/* 0x59 PMSCAN_BACKTAB      */ , 0x00                       ,FALSE
/* 0x5A PMSCAN_PADENTER     */ , WINSCAN_PADENTER           ,TRUE
/* 0x5B PMSCAN_CTRLRIGHT    */ , WINSCAN_CTRLRIGHT          ,TRUE
/* 0x5C PMSCAN_PADSLASH     */ , WINSCAN_PADSLASH           ,TRUE
/* 0x5D PMSCAN_PRINT        */ , WINSCAN_PRINT              ,TRUE
/* 0x5E PMSCAN_ALTRIGHT     */ , WINSCAN_ALTRIGHT           ,TRUE
/* 0x5F PMSCAN_PAUSE        */ , WINSCAN_PAUSE              ,FALSE
/* 0x60 PMSCAN_HOME         */ , WINSCAN_HOME               ,TRUE
/* 0x61 PMSCAN_UP           */ , WINSCAN_UP                 ,TRUE
/* 0x62 PMSCAN_PAGEUP       */ , WINSCAN_PAGEUP             ,TRUE
/* 0x63 PMSCAN_LEFT         */ , WINSCAN_LEFT               ,TRUE
/* 0x64 PMSCAN_RIGHT        */ , WINSCAN_RIGHT              ,TRUE
/* 0x65 PMSCAN_END          */ , WINSCAN_END                ,TRUE
/* 0x66 PMSCAN_DOWN         */ , WINSCAN_DOWN               ,TRUE
/* 0x67 PMSCAN_PAGEDOWN     */ , WINSCAN_PAGEDOWN           ,TRUE
/* 0x68 PMSCAN_INSERT       */ , WINSCAN_INSERT             ,TRUE
/* 0x69 PMSCAN_DELETE       */ , WINSCAN_DELETE             ,TRUE
/* 0x6A PMSCAN_F23          */ , WINSCAN_F23                ,FALSE
/* 0x6B PMSCAN_F24          */ , WINSCAN_F24                ,FALSE
/* 0x6C PMSCAN_SYSMEM       */ , 0x00                       ,FALSE
/* 0x6D PMSCAN_ERASEEOF     */ , WINSCAN_ERASEEOF           ,FALSE
/* 0x6E PMSCAN_BREAK        */ , WINSCAN_BREAK              ,TRUE
/* 0x6F PMSCAN_MOVEWIN      */ , 0x00                       ,FALSE
/* 0x70 PMSCAN_NLS3         */ , 0x00                       ,FALSE
/* 0x71 PMSCAN_HELP         */ , WINSCAN_HELP               ,FALSE
/* 0x72 PMSCAN_TASKMAN      */ , 0x00                       ,FALSE
/* 0x73 PMSCAN_B11          */ , 0x00                       ,FALSE
/* 0x74 PMSCAN_JUMP         */ , 0x00                       ,FALSE
/* 0x75 PMSCAN_MINWIN       */ , 0x00                       ,FALSE
/* 0x76 PMSCAN_CLEAR        */ , 0x00                       ,FALSE
/* 0x77 PMSCAN_77           */ , 0x00                       ,FALSE
/* 0x78 PMSCAN_78           */ , 0x00                       ,FALSE
/* 0x79 PMSCAN_NLS2         */ , 0x00                       ,FALSE
/* 0x7a PMSCAN_SIZE         */ , 0x00                       ,FALSE
/* 0x7b PMSCAN_NLS1         */ , 0x00                       ,FALSE
/* 0x7c PMSCAN_APPLICATION  */ , WINSCAN_APPLICATION        ,TRUE
/* 0x7d PMSCAN_E13          */ , 0x00                       ,FALSE
/* 0x7e PMSCAN              */ , WINSCAN_WINLEFT            ,TRUE
/* 0x7f PMSCAN              */ , WINSCAN_WINRIGHT           ,TRUE
/* 0x80 PMSCAN_PA1          */ , WINSCAN_PA1                ,FALSE
/* 0x81 PMSCAN_F13          */ , WINSCAN_F13                ,FALSE
/* 0x82 PMSCAN_F14          */ , WINSCAN_F14                ,FALSE
/* 0x83 PMSCAN_F15          */ , WINSCAN_F15                ,FALSE
/* 0x84 PMSCAN_PA2          */ , 0x00                       ,FALSE
/* 0x85 PMSCAN_PA3          */ , 0x00                       ,FALSE
/* 0x86 PMSCAN_SPACEBREAK   */ , 0x00                       ,FALSE
/* 0x87 PMSCAN_TABRIGHT     */ , 0x00                       ,FALSE
/* 0x88 PMSCAN_NOOP         */ , 0x00                       ,FALSE
/* 0x89 PMSCAN_F16          */ , WINSCAN_F16                ,FALSE
/* 0x8a PMSCAN_F17          */ , WINSCAN_F17                ,FALSE
/* 0x8b PMSCAN_F18          */ , WINSCAN_F18                ,FALSE
/* 0x8c PMSCAN_F19          */ , WINSCAN_F19                ,FALSE
/* 0x8d PMSCAN_F20          */ , WINSCAN_F20                ,FALSE
/* 0x8e PMSCAN_F21          */ , WINSCAN_F21                ,FALSE
/* 0x8f PMSCAN_F22          */ , WINSCAN_F22                ,FALSE
/* 0x90                     */ , 0x00                       ,FALSE
/* 0x91                     */ , 0x00                       ,FALSE
/* 0x92                     */ , 0x00                       ,FALSE
/* 0x93                     */ , 0x00                       ,FALSE
/* 0x94                     */ , 0x00                       ,FALSE
/* 0x95                     */ , 0x00                       ,FALSE
/* 0x96                     */ , 0x00                       ,FALSE
/* 0x97                     */ , 0x00                       ,FALSE
/* 0x98                     */ , 0x00                       ,FALSE
/* 0x99                     */ , 0x00                       ,FALSE
/* 0x9A                     */ , 0x00                       ,FALSE
/* 0x9B                     */ , 0x00                       ,FALSE
/* 0x9C                     */ , 0x00                       ,FALSE
/* 0x9D                     */ , 0x00                       ,FALSE
/* 0x9E                     */ , 0x00                       ,FALSE
/* 0x9F                     */ , 0x00                       ,FALSE
/* 0xA0                     */ , 0x00                       ,FALSE
/* 0xA1                     */ , 0x00                       ,FALSE
/* 0xA2                     */ , 0x00                       ,FALSE
/* 0xA3                     */ , 0x00                       ,FALSE
/* 0xA4                     */ , 0x00                       ,FALSE
/* 0xA5                     */ , 0x00                       ,FALSE
/* 0xA6                     */ , 0x00                       ,FALSE
/* 0xA7                     */ , 0x00                       ,FALSE
/* 0xA8                     */ , 0x00                       ,FALSE
/* 0xA9                     */ , 0x00                       ,FALSE
/* 0xAA                     */ , 0x00                       ,FALSE
/* 0xAB                     */ , 0x00                       ,FALSE
/* 0xAC                     */ , 0x00                       ,FALSE
/* 0xAD                     */ , 0x00                       ,FALSE
/* 0xAE                     */ , 0x00                       ,FALSE
/* 0xAF                     */ , 0x00                       ,FALSE
/* 0xB0                     */ , 0x00                       ,FALSE
/* 0xB1                     */ , 0x00                       ,FALSE
/* 0xB2                     */ , 0x00                       ,FALSE
/* 0xB3                     */ , 0x00                       ,FALSE
/* 0xB4                     */ , 0x00                       ,FALSE
/* 0xB5                     */ , 0x00                       ,FALSE
/* 0xB6                     */ , 0x00                       ,FALSE
/* 0xB7                     */ , 0x00                       ,FALSE
/* 0xB8                     */ , 0x00                       ,FALSE
/* 0xB9                     */ , 0x00                       ,FALSE
/* 0xBA                     */ , 0x00                       ,FALSE
/* 0xBB                     */ , 0x00                       ,FALSE
/* 0xBC                     */ , 0x00                       ,FALSE
/* 0xBD                     */ , 0x00                       ,FALSE
/* 0xBE                     */ , 0x00                       ,FALSE
/* 0xBF                     */ , 0x00                       ,FALSE
/* 0xC0                     */ , 0x00                       ,FALSE
/* 0xC1                     */ , 0x00                       ,FALSE
/* 0xC2                     */ , 0x00                       ,FALSE
/* 0xC3                     */ , 0x00                       ,FALSE
/* 0xC4                     */ , 0x00                       ,FALSE
/* 0xC5                     */ , 0x00                       ,FALSE
/* 0xC6                     */ , 0x00                       ,FALSE
/* 0xC7                     */ , 0x00                       ,FALSE
/* 0xC8                     */ , 0x00                       ,FALSE
/* 0xC9                     */ , 0x00                       ,FALSE
/* 0xCA                     */ , 0x00                       ,FALSE
/* 0xCB                     */ , 0x00                       ,FALSE
/* 0xCC                     */ , 0x00                       ,FALSE
/* 0xCD                     */ , 0x00                       ,FALSE
/* 0xCE                     */ , 0x00                       ,FALSE
/* 0xCF                     */ , 0x00                       ,FALSE
/* 0xD0                     */ , 0x00                       ,FALSE
/* 0xD1                     */ , 0x00                       ,FALSE
/* 0xD2                     */ , 0x00                       ,FALSE
/* 0xD3                     */ , 0x00                       ,FALSE
/* 0xD4                     */ , 0x00                       ,FALSE
/* 0xD5                     */ , 0x00                       ,FALSE
/* 0xD6                     */ , 0x00                       ,FALSE
/* 0xD7                     */ , 0x00                       ,FALSE
/* 0xD8                     */ , 0x00                       ,FALSE
/* 0xD9                     */ , 0x00                       ,FALSE
/* 0xDA                     */ , 0x00                       ,FALSE
/* 0xDB                     */ , 0x00                       ,FALSE
/* 0xDC                     */ , 0x00                       ,FALSE
/* 0xDD                     */ , 0x00                       ,FALSE
/* 0xDE                     */ , 0x00                       ,FALSE
/* 0xDF                     */ , 0x00                       ,FALSE
/* 0xE0                     */ , 0x00                       ,FALSE
/* 0xE1                     */ , 0x00                       ,FALSE
/* 0xE2                     */ , 0x00                       ,FALSE
/* 0xE3                     */ , 0x00                       ,FALSE
/* 0xE4                     */ , 0x00                       ,FALSE
/* 0xE5                     */ , 0x00                       ,FALSE
/* 0xE6                     */ , 0x00                       ,FALSE
/* 0xE7                     */ , 0x00                       ,FALSE
/* 0xE8                     */ , 0x00                       ,FALSE
/* 0xE9                     */ , 0x00                       ,FALSE
/* 0xEA                     */ , 0x00                       ,FALSE
/* 0xEB                     */ , 0x00                       ,FALSE
/* 0xEC                     */ , 0x00                       ,FALSE
/* 0xED                     */ , 0x00                       ,FALSE
/* 0xEE                     */ , 0x00                       ,FALSE
/* 0xEF                     */ , 0x00                       ,FALSE
/* 0xF0                     */ , 0x00                       ,FALSE
/* 0xF1                     */ , 0x00                       ,FALSE
/* 0xF2                     */ , 0x00                       ,FALSE
/* 0xF3                     */ , 0x00                       ,FALSE
/* 0xF4                     */ , 0x00                       ,FALSE
/* 0xF5                     */ , 0x00                       ,FALSE
/* 0xF6                     */ , 0x00                       ,FALSE
/* 0xF7                     */ , 0x00                       ,FALSE
/* 0xF8                     */ , 0x00                       ,FALSE
/* 0xF9                     */ , 0x00                       ,FALSE
/* 0xFA                     */ , 0x00                       ,FALSE
/* 0xFB                     */ , 0x00                       ,FALSE
/* 0xFC                     */ , 0x00                       ,FALSE
/* 0xFD                     */ , 0x00                       ,FALSE
/* 0xFE                     */ , 0x00                       ,FALSE
/* 0xFF                     */ , 0x00                       ,FALSE
                               };


//******************************************************************************
//******************************************************************************

ODINPROCEDURE3(KeyTranslatePMToWinBuf,
               BYTE *, pmkey,
               BYTE *, winkey,
               int,    nrkeys)
{
   for(int i=1;i<nrkeys;i++) {
       if(abWinVKeyToPMScan[i].bPMScanCode) {
            winkey[i] = pmkey[OSLibWinTranslateChar(abWinVKeyToPMScan[i].bPMScanCode, TC_SCANCODETOVIRTUALKEY, 0)];
       }
   }
   winkey[VK_SHIFT]   = winkey[VK_LSHIFT] | winkey[VK_RSHIFT];
   winkey[VK_CONTROL] = winkey[VK_LCONTROL] | winkey[VK_RCONTROL];
   winkey[VK_MENU]    = winkey[VK_LMENU] | winkey[VK_RMENU];
}
//******************************************************************************
//******************************************************************************
BYTE KeyTranslateWinVKeyToPMScan(BYTE bWinVKey, BOOL fExtended)
{
  // if the extended bit didn't match, this is
  // the closest hit
  BYTE bAlmost = 0;
  
  // experiment
#if 0
  bAlmost = abWinVKeyToPMScan[bWinVKey].bPMScanCode;
#else
  for(int i = 0;
      i < 256; // sizeof(row of abPMScanToWinVKey)
      i++)
  {
    if (abPMScanToWinVKey[i][0] == bWinVKey)
    {
      // this represents the PMScan code which is used as index      
      bAlmost = i;
      
      // exact match!
      if (abPMScanToWinVKey[i][1] == fExtended)
        break;
    }
  }
#endif
  
  dprintf(("KeyTranslateWinVKeyToPMScan(%02xh,%d) = %02xh",
           bWinVKey,
           fExtended,
           bAlmost));
  
  // almost a match or no match at all.
  return bAlmost; 
}
//******************************************************************************
//******************************************************************************
BYTE KeyTranslateWinScanToPMScan(BYTE bWinScan, BOOL fExtended)
{
  // Note:
  // MapVirtualKeyA requires this function,
  // O32_MapVirtualKeyA uses PM Scancodes only!
  
  // if the extended bit didn't match, this is
  // the closest hit
  BYTE bAlmost = 0;
  
  for(int i = 0;
      i < 256; // sizeof(row of abPMScanToWinVKey)
      i++)
  {
    if (abPMScanToWinScan[i][0] == bWinScan)
    {
      // this represents the PMScan code which is used as index      
      bAlmost = i;
      
      // exact match!
      if (abPMScanToWinVKey[i][1] == fExtended)
        break;
    }
  }
  
  dprintf(("KeyTranslateWinScanToPMScan(%02xh,%d) = %02xh",
           bWinScan,
           fExtended,
           bAlmost));
  
  // almost a match or no match at all.
  return bAlmost; 
  
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(int, GetKeyboardType,
              int, nTypeFlag)
{
    return O32_GetKeyboardType(nTypeFlag);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(BOOL,  GetKeyboardState,
              PBYTE, lpKeyState)
{
 BYTE   PMKeyState[256];
 BOOL   rc;

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
ODINFUNCTION1(BOOL,  SetKeyboardState,
              PBYTE, lpKeyState)
{
  dprintf(("USER32: SetKeyboardState %x not implemented", lpKeyState));
  return(TRUE);
}
/***********************************************************************
 *           GetKeyboardLayout          (USER32.250)
 *
 * FIXME: - device handle for keyboard layout defaulted to
 *          the language id. This is the way Windows default works.
 *        - the thread identifier (dwLayout) is also ignored.
 */
// * Remark    : Based on Wine version (991031)
ODINFUNCTION1(HKL,   GetKeyboardLayout,
              DWORD, dwLayout)
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
ODINFUNCTION1(INT,   GetKeyboardLayoutNameA,
              LPSTR, pwszKLID)
{
   dprintf(("not correctly implemented"));

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
ODINFUNCTION1(INT,    GetKeyboardLayoutNameW,
              LPWSTR, pwszKLID)
{
   LPSTR buf = (LPSTR)HEAP_xalloc( GetProcessHeap(), 0, strlen("00000409")+1);

   int res = GetKeyboardLayoutNameA(buf);
   lstrcpyAtoW(pwszKLID,buf);
   HeapFree( GetProcessHeap(), 0, buf );
   return res;
}
/***********************************************************************
 *           GetKeyboardLayoutList      (USER32.251)
 *
 * FIXME: Supports only the system default language and layout and
 *          returns only 1 value.
 *
 * Return number of values available if either input parm is
 *  0, per MS documentation.
 *
 * Remark    : Based on Wine version (991031)
 */
ODINFUNCTION2(INT,   GetKeyboardLayoutList,
              INT,   nBuff,
              HKL *, layouts)
{
  dprintf(("not correctly implemented"));
  
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

ODINFUNCTION5(int,    ToAscii,
              UINT,   uVirtKey,
              UINT,   uScanCode,
              PBYTE,  lpbKeyState,
              LPWORD, lpwTransKey,
              UINT,   fuState)
{
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

ODINFUNCTION6(int,    ToAsciiEx,
              UINT,   uVirtKey,
              UINT,   uScanCode,
              PBYTE,  lpbKeyState,
              LPWORD, lpwTransKey,
              UINT,   fuState,
              HKL,    hkl)
{
  dprintf(("imcompletely implemented"));

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

ODINFUNCTION6(int,    ToUnicode,
              UINT,   uVirtKey,
              UINT,   uScanCode,
              PBYTE,  lpKeyState,
              LPWSTR, pwszBuff,
              int,    cchBuff,
              UINT,   wFlags)
{
  dprintf(("not implemented.\n"));

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

ODINFUNCTION0(UINT, GetKBCodePage)
{
  return (GetOEMCP());
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,    GetKeyNameTextA,
              LPARAM, lParam, 
              LPSTR,  lpString, 
              int,    nSize)
{
  // check the scancodes if the extended key bit is set so
  // we can easily distinguish between "left" and "right" special keys, etc.
  BOOL  fDontCare = (lParam & 0x02000000) != 0;
  BOOL  fExtended = (lParam & 0x01000000) != 0;
  UCHAR ucWinScan = (lParam & 0x00ff0000) >> 16;
  UCHAR ucWinVKey;
  UCHAR ucPMScan;
  int   result;
  
  if (fDontCare)
  {
    fExtended = FALSE;
    
    // map "right keys" to "universal keys"
    // LALT,RALT -> ALT
    // etc.
    // @@@PH
  }
  
  // Note: Open32 expects PM Scancodes, NOT Winscancodes.
  // Note: this is the only way of loss-less conversion!
  ucPMScan = KeyTranslateWinScanToPMScan(ucWinScan, fExtended);
  dprintf(("ucWinScan=%02xh, fExtended=%d translated to PMscan=%02xh\n",
           ucWinScan,
           fExtended,
           ucPMScan));
  ucWinVKey = abPMScanToWinVKey[ucPMScan][0];
  dprintf(("ucPMScan=%02xh translated to ucWinVKey=%02xh\n",
           ucPMScan,
           ucWinVKey));

  // Bug in Open32:
  // 0 - expects PMScancodes instead of WinScancodes
  // 1 - the "extended key bit" is not taken into account
  // 2 - some keys (AltGr) have not been assigned any names
  // int result = O32_GetKeyNameText(lParam,lpString,nSize);
  
  memset(lpString, 0, nSize);
  
  LPSTR lpstrKey = abWinVKeyToPMScan[ucWinVKey].lpstrName;
  if (NULL == lpstrKey)
  {
    dprintf(("ERROR: keyname for winscan=%02xh winvkey=%02xh, fExtended=%d not found.\n",
             ucWinScan, ucWinVKey, fExtended & !fDontCare));
    
    // build artificial name
    
    CHAR szName[10];
    sprintf(szName, 
            "VKey%02x%c",
            ucWinScan,
            fExtended ? '+' : 0);
    memcpy(lpString, szName, nSize);
    
    // how many chars have been returned?
    result = min(nSize, strlen(lpString));
  }
  else
  {
    result = min(nSize, strlen(lpstrKey));
    strncpy (lpString, lpstrKey, result);
  }
  
  dprintf(("keyname=[%s]\n",
           lpString));
  
  return result;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(int,    GetKeyNameTextW,
              LPARAM, lParam,
              LPWSTR, lpString,
              int,    nSize)
{
    dprintf(("USER32:  GetKeyNameTextW DOES NOT WORK (not implemented)\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_GetKeyNameText(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(SHORT, GetKeyState,
              int,   nVirtKey)
{
  return O32_GetKeyState(nVirtKey);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(WORD,  GetAsyncKeyState,
              INT,   nVirtKey)
{
  if(fVersionWarp3) 
  {
    dprintf(("WARNING: not correctly implemented for Warp 3"));
    //Not present in Warp 3's PMWINX
    return O32_GetKeyState(nVirtKey);
  }
  else  
    return O32_GetAsyncKeyState(nVirtKey);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(UINT,  MapVirtualKeyA,
              UINT,  uCode, 
              UINT,  uMapType)
{
  dprintf(("imcompletely implemented"));
  
  /* A quick fix for Commandos, very incomplete */
  switch (uMapType) 
  {
    case 2:
      if (uCode >= VK_A && uCode <= VK_Z) 
      {
        return 'A' + uCode - VK_A;
      }
    break;

    case 1:
    case 3:
      // Open32 uses PMScancodes, not Windows Scancodes!!
      // @@@PH the extended flag is to be ignored!
      BYTE bPMScan = KeyTranslateWinScanToPMScan(uCode, FALSE);
      return O32_MapVirtualKey(bPMScan, uMapType);
  }
  return O32_MapVirtualKey(uCode, uMapType);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(UINT,  MapVirtualKeyW,
              UINT,  uCode, 
              UINT,  uMapType)
{
  dprintf(("incorrectly implemented\n"));
  
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
ODINFUNCTION3(UINT,  MapVirtualKeyExA,
              UINT,  uCode, 
              UINT,  uMapType, 
              HKL,   dwhkl)
{
  dprintf(("incompletely implemented"));

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
ODINFUNCTION3(UINT, MapVirtualKeyExW,
              UINT, uCode, 
              UINT, uMapType, 
              HKL,  dwhkl)
{
  dprintf(("incompletely implemented"));

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
 * Status    : Borrowed from NT
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/
ODINFUNCTION1(DWORD, OemKeyScan,
              WORD,  wOemChar)
{
  int nVirtKey;
  
  OemToCharBuffA((LPCSTR)&wOemChar,
                 (LPSTR) &nVirtKey,
                 1);
  if (nVirtKey != 0)
  {
    SHORT sScan = VkKeyScanA(nVirtKey);
    nVirtKey = MapVirtualKeyA( (sScan & 0xff), 0);
    if (nVirtKey != 0)
    {
      return ( (nVirtKey & 0x00FF) ||
               ( (nVirtKey & 0xFF00) << 8) );
    }
  }
  
  // if not found ...
  return 0xffffffff;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(BOOL,  RegisterHotKey,
              HWND,  hwnd, 
              int,   idHotKey, 
              UINT,  fuModifiers, 
              UINT,  uVirtKey)
{
  dprintf(("not implemented"));
  
  hwnd = Win32ToOS2Handle(hwnd);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL, UnregisterHotKey,
              HWND, hwnd, 
              int,  idHotKey)
{
  dprintf(("not implemented"));
  
  hwnd = Win32ToOS2Handle(hwnd);

  return(TRUE);
}

//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
ODINFUNCTION1(WORD, VkKeyScanA,
              char, ch)
{
  return O32_VkKeyScan(ch);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(WORD,  VkKeyScanW,
              WCHAR, wch)
{
  dprintf(("incorrectly implemented"));
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
ODINFUNCTION2(WORD,  VkKeyScanExW,
              WCHAR, uChar,
              HKL,   hkl)
{
  dprintf(("partially implemented"));

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
ODINFUNCTION2(WORD, VkKeyScanExA,
              CHAR, uChar,
              HKL,  hkl)
{
  dprintf(("partially implemented"));

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
ODINPROCEDURE4(keybd_event,
               BYTE,  bVk,
               BYTE,  bScan,
               DWORD, dwFlags,
               DWORD, dwExtraInfo)
{
  INPUT i;
  
  // format input packet
  i.type           = INPUT_KEYBOARD;
  i.ki.wVk         = bVk;
  i.ki.wScan       = bScan;
  i.ki.dwFlags     = dwFlags;
  i.ki.dwExtraInfo = dwExtraInfo;
  
  // forward to more modern API
  SendInput(1, &i, sizeof(i) );
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
ODINFUNCTION2(HKL,     LoadKeyboardLayoutA,
              LPCSTR,  pwszKLID,
              UINT,    Flags)
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
ODINFUNCTION2(HKL,     LoadKeyboardLayoutW,
              LPCWSTR, pwszKLID,
              UINT,    Flags)
{
  dprintf(("USER32:LoadKeyboardLayoutW (%ls,%u) not implemented.\n",
         pwszKLID,
         Flags));

  return (NULL);
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION2(BOOL,  ActivateKeyboardLayout,
              HKL,   hkl,
              UINT,  fuFlags)
{
  dprintf(("not implemented\n"));
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
ODINFUNCTION1(BOOL, UnloadKeyboardLayout,
              HKL,  hkl)
{
  dprintf(("not implemented.\n"));

  return (TRUE);
}
//******************************************************************************
//******************************************************************************
