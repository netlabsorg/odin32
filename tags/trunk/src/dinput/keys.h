/********************************************************************/
/* Table to translate from a PM scancode to a general Windows vkey, */
/* left-right specific Windows vkey or and extended key flag.       */
/*                                                                  */
/* NOTE: Windows uses different vkeys for the numpad depending on   */
/*       numlock state.  This table assumes numlock is ON.  To map  */
/*       this vkey to it's numlock OFF equivalent, use the table    */
/*       'abNumlockWinVKeyToWinVKey' below.                         */
/********************************************************************/
BYTE   abPMScanToWinVKey[256][3] =
/****************************************************************************/
/* Scancode                 *    General vkey     * Specific vkey    * Ext  */
/****************************************************************************/
/* 0x00                     */ { 0x00,              0x00,              FALSE
/* 0x01 PMSCAN_ESC          */ , VK_ESCAPE_W,       VK_ESCAPE_W,       FALSE
/* 0x02 PMSCAN_ONE          */ , VK_1_W,            VK_1_W,            FALSE
/* 0x03 PMSCAN_TWO          */ , VK_2_W,            VK_2_W,            FALSE
/* 0x04 PMSCAN_THREE        */ , VK_3_W,            VK_3_W,            FALSE
/* 0x05 PMSCAN_FOUR         */ , VK_4_W,            VK_4_W,            FALSE
/* 0x06 PMSCAN_FIVE         */ , VK_5_W,            VK_5_W,            FALSE
/* 0x07 PMSCAN_SIX          */ , VK_6_W,            VK_6_W,            FALSE
/* 0x08 PMSCAN_SEVEN        */ , VK_7_W,            VK_7_W,            FALSE
/* 0x09 PMSCAN_EIGHT        */ , VK_8_W,            VK_8_W,            FALSE
/* 0x0A PMSCAN_NINE         */ , VK_9_W,            VK_9_W,            FALSE
/* 0x0B PMSCAN_ZERO         */ , VK_0_W,            VK_0_W,            FALSE
/* 0x0C PMSCAN_HYPHEN       */ , VK_HYPHEN_W,       VK_HYPHEN_W,       FALSE
/* 0x0D PMSCAN_EQUAL        */ , VK_EQUAL_W,        VK_EQUAL_W,        FALSE
/* 0x0E PMSCAN_BACKSPACE    */ , VK_BACK_W,         VK_BACK_W,         FALSE
/* 0x0F PMSCAN_TAB          */ , VK_TAB_W,          VK_TAB_W,          FALSE
/* 0x10 PMSCAN_Q            */ , VK_Q_W,            VK_Q_W,            FALSE
/* 0x11 PMSCAN_W            */ , VK_W_W,            VK_W_W,            FALSE
/* 0x12 PMSCAN_E            */ , VK_E_W,            VK_E_W,            FALSE
/* 0x13 PMSCAN_R            */ , VK_R_W,            VK_R_W,            FALSE
/* 0x14 PMSCAN_T            */ , VK_T_W,            VK_T_W,            FALSE
/* 0x15 PMSCAN_Y            */ , VK_Y_W,            VK_Y_W,            FALSE
/* 0x16 PMSCAN_U            */ , VK_U_W,            VK_U_W,            FALSE
/* 0x17 PMSCAN_I            */ , VK_I_W,            VK_I_W,            FALSE
/* 0x18 PMSCAN_O            */ , VK_O_W,            VK_O_W,            FALSE
/* 0x19 PMSCAN_P            */ , VK_P_W,            VK_P_W,            FALSE
/* 0x1A PMSCAN_BRACKETLEFT  */ , VK_BRACKETLEFT_W,  VK_BRACKETLEFT_W,  FALSE
/* 0x1B PMSCAN_BRACKETRIGHT */ , VK_BRACKETRIGHT_W, VK_BRACKETRIGHT_W, FALSE
/* 0x1C PMSCAN_ENTER        */ , VK_RETURN_W,       VK_RETURN_W,       FALSE
/* 0x1D PMSCAN_CTRLLEFT     */ , VK_CONTROL_W,      VK_LCONTROL_W,     FALSE
/* 0x1E PMSCAN_A            */ , VK_A_W,            VK_A_W,            FALSE
/* 0x1F PMSCAN_S            */ , VK_S_W,            VK_S_W,            FALSE
/* 0x20 PMSCAN_D            */ , VK_D_W,            VK_D_W,            FALSE
/* 0x21 PMSCAN_F            */ , VK_F_W,            VK_F_W,            FALSE
/* 0x22 PMSCAN_G            */ , VK_G_W,            VK_G_W,            FALSE
/* 0x23 PMSCAN_H            */ , VK_H_W,            VK_H_W,            FALSE
/* 0x24 PMSCAN_J            */ , VK_J_W,            VK_J_W,            FALSE
/* 0x25 PMSCAN_K            */ , VK_K_W,            VK_K_W,            FALSE
/* 0x26 PMSCAN_L            */ , VK_L_W,            VK_L_W,            FALSE
/* 0x27 PMSCAN_SEMICOLON    */ , VK_SEMICOLON_W,    VK_SEMICOLON_W,    FALSE
/* 0x28 PMSCAN_QUOTESINGLE  */ , VK_QUOTESINGLE_W,  VK_QUOTESINGLE_W,  FALSE
/* 0x29 PMSCAN_GRAVE        */ , VK_GRAVE_W,        VK_GRAVE_W,        FALSE
/* 0x2A PMSCAN_SHIFTLEFT    */ , VK_SHIFT_W,        VK_LSHIFT_W,       FALSE
/* 0x2B PMSCAN_BACKSLASH    */ , VK_BACKSLASH_W,    VK_BACKSLASH_W,    FALSE
/* 0x2C PMSCAN_Z            */ , VK_Z_W,            VK_Z_W,            FALSE
/* 0x2D PMSCAN_X            */ , VK_X_W,            VK_X_W,            FALSE
/* 0x2E PMSCAN_C            */ , VK_C_W,            VK_C_W,            FALSE
/* 0x2F PMSCAN_V            */ , VK_V_W,            VK_V_W,            FALSE
/* 0x30 PMSCAN_B            */ , VK_B_W,            VK_B_W,            FALSE
/* 0x31 PMSCAN_N            */ , VK_N_W,            VK_N_W,            FALSE
/* 0x32 PMSCAN_M            */ , VK_M_W,            VK_M_W,            FALSE
/* 0x33 PMSCAN_COMMA        */ , VK_COMMA_W,        VK_COMMA_W,        FALSE
/* 0x34 PMSCAN_PERIOD       */ , VK_PERIOD_W,       VK_PERIOD_W,       FALSE
/* 0x35 PMSCAN_SLASH        */ , VK_SLASH_W,        VK_SLASH_W,        FALSE
/* 0x36 PMSCAN_SHIFTRIGHT   */ , VK_SHIFT_W,        VK_RSHIFT_W,       FALSE
/* 0x37 PMSCAN_PADASTERISK  */ , VK_MULTIPLY_W,     VK_MULTIPLY_W,     FALSE
/* 0x38 PMSCAN_ALTLEFT      */ , VK_MENU_W,         VK_LMENU_W,        FALSE
/* 0x39 PMSCAN_SPACE        */ , VK_SPACE_W,        VK_SPACE_W,        FALSE
/* 0x3A PMSCAN_CAPSLOCK     */ , VK_CAPITAL_W,      VK_CAPITAL_W,      FALSE
/* 0x3B PMSCAN_F1           */ , VK_F1_W,           VK_F1_W,           FALSE
/* 0x3C PMSCAN_F2           */ , VK_F2_W,           VK_F2_W,           FALSE
/* 0x3D PMSCAN_F3           */ , VK_F3_W,           VK_F3_W,           FALSE
/* 0x3E PMSCAN_F4           */ , VK_F4_W,           VK_F4_W,           FALSE
/* 0x3F PMSCAN_F5           */ , VK_F5_W,           VK_F5_W,           FALSE
/* 0x40 PMSCAN_F6           */ , VK_F6_W,           VK_F6_W,           FALSE
/* 0x41 PMSCAN_F7           */ , VK_F7_W,           VK_F7_W,           FALSE
/* 0x42 PMSCAN_F8           */ , VK_F8_W,           VK_F8_W,           FALSE
/* 0x43 PMSCAN_F9           */ , VK_F9_W,           VK_F9_W,           FALSE
/* 0x44 PMSCAN_F10          */ , VK_F10_W,          VK_F10_W,          FALSE
/* 0x45 PMSCAN_NUMLOCK      */ , VK_NUMLOCK_W,      VK_NUMLOCK_W,      TRUE
/* 0x46 PMSCAN_SCROLLLOCK   */ , VK_SCROLL_W,       VK_SCROLL_W,       FALSE
/* 0x47 PMSCAN_PAD7         */ , VK_NUMPAD7_W,      VK_NUMPAD7_W,      FALSE
/* 0x48 PMSCAN_PAD8         */ , VK_NUMPAD8_W,      VK_NUMPAD8_W,      FALSE
/* 0x49 PMSCAN_PAD9         */ , VK_NUMPAD9_W,      VK_NUMPAD9_W,      FALSE
/* 0x4A PMSCAN_PADMINUS     */ , VK_SUBTRACT_W,     VK_SUBTRACT_W,     FALSE
/* 0x4B PMSCAN_PAD4         */ , VK_NUMPAD4_W,      VK_NUMPAD4_W,      FALSE
/* 0x4C PMSCAN_PAD5         */ , VK_NUMPAD5_W,      VK_NUMPAD5_W,      FALSE
/* 0x4D PMSCAN_PAD6         */ , VK_NUMPAD6_W,      VK_NUMPAD6_W,      FALSE
/* 0x4E PMSCAN_PADPLUS      */ , VK_ADD_W,          VK_ADD_W,          FALSE
/* 0x4F PMSCAN_PAD1         */ , VK_NUMPAD1_W,      VK_NUMPAD1_W,      FALSE
/* 0x50 PMSCAN_PAD2         */ , VK_NUMPAD2_W,      VK_NUMPAD2_W,      FALSE
/* 0x51 PMSCAN_PAD3         */ , VK_NUMPAD3_W,      VK_NUMPAD3_W,      FALSE
/* 0x52 PMSCAN_PAD0         */ , VK_NUMPAD0_W,      VK_NUMPAD0_W,      FALSE
/* 0x53 PMSCAN_PADPERIOD    */ , VK_DECIMAL_W,      VK_DECIMAL_W,      FALSE
/* 0x54 PMSCAN_SYSREQ       */ , 0x00,              0x00,              FALSE
/* 0x55 PMSCAN_RESET        */ , 0x00,              0x00,              FALSE
/* 0x56 PMSCAN_EXTRA        */ , VK_EXTRA_W,        VK_EXTRA_W,        FALSE
/* 0x57 PMSCAN_F11          */ , VK_F11_W,          VK_F11_W,          FALSE
/* 0x58 PMSCAN_F12          */ , VK_F12_W,          VK_F12_W,          FALSE
/* 0x59 PMSCAN_BACKTAB      */ , 0x00,              0x00,              FALSE
/* 0x5A PMSCAN_PADENTER     */ , VK_RETURN_W,       VK_RETURN_W,       TRUE
/* 0x5B PMSCAN_CTRLRIGHT    */ , VK_CONTROL_W,      VK_RCONTROL_W,     TRUE
/* 0x5C PMSCAN_PADSLASH     */ , VK_DIVIDE_W,       VK_DIVIDE_W,       TRUE
/* 0x5D PMSCAN_PRINT        */ , VK_PRINT_W,        VK_PRINT_W,        FALSE
/* 0x5E PMSCAN_ALTRIGHT     */ , VK_MENU_W,         VK_RMENU_W,        TRUE
/* 0x5F PMSCAN_PAUSE        */ , VK_PAUSE_W,        VK_PAUSE_W,        FALSE
/* 0x60 PMSCAN_HOME         */ , VK_HOME_W,         VK_HOME_W,         TRUE
/* 0x61 PMSCAN_UP           */ , VK_UP_W,           VK_UP_W,           TRUE
/* 0x62 PMSCAN_PAGEUP       */ , VK_PRIOR_W,        VK_PRIOR_W,        TRUE
/* 0x63 PMSCAN_LEFT         */ , VK_LEFT_W,         VK_LEFT_W,         TRUE
/* 0x64 PMSCAN_RIGHT        */ , VK_RIGHT_W,        VK_RIGHT_W,        TRUE
/* 0x65 PMSCAN_END          */ , VK_END_W,          VK_END_W,          TRUE
/* 0x66 PMSCAN_DOWN         */ , VK_DOWN_W,         VK_DOWN_W,         TRUE
/* 0x67 PMSCAN_PAGEDOWN     */ , VK_NEXT_W,         VK_NEXT_W,         TRUE
/* 0x68 PMSCAN_INSERT       */ , VK_INSERT_W,       VK_INSERT_W,       TRUE
/* 0x69 PMSCAN_DELETE       */ , VK_DELETE_W,       VK_DELETE_W,       TRUE
/* 0x6A PMSCAN_F23          */ , VK_F23_W,          VK_F23_W,          FALSE
/* 0x6B PMSCAN_F24          */ , VK_F24_W,          VK_F24_W,          FALSE
/* 0x6C PMSCAN_SYSMEM       */ , 0x00,              0x00,              FALSE
/* 0x6D PMSCAN_ERASEEOF     */ , VK_EREOF_W,        VK_EREOF_W,        FALSE
/* 0x6E PMSCAN_BREAK        */ , VK_CANCEL_W,       VK_CANCEL_W,       TRUE
/* 0x6F PMSCAN_MOVEWIN      */ , 0x00,              0x00,              FALSE
/* 0x70 PMSCAN_NLS3         */ , 0x00,              0x00,              FALSE
/* 0x71 PMSCAN_HELP         */ , VK_HELP_W,         VK_HELP_W,         FALSE
/* 0x72 PMSCAN_TASKMAN      */ , 0x00,              0x00,              FALSE
/* 0x73 PMSCAN_JEXTRA       */ , VK_FFC1_W,         VK_FFC1_W,         FALSE
/* 0x73 PMSCAN_B11             , 0x00,              0x00,              FALSE */
/* 0x74 PMSCAN_JUMP         */ , 0x00,              0x00,              FALSE
/* 0x75 PMSCAN_MINWIN       */ , 0x00,              0x00,              FALSE
/* 0x76 PMSCAN_CLEAR        */ , 0x00,              0x00,              FALSE
/* 0x77 PMSCAN_77           */ , 0x00,              0x00,              FALSE
/* 0x78 PMSCAN_78           */ , 0x00,              0x00,              FALSE
/* 0x79 PMSCAN_NLS2         */ , 0x00,              0x00,              FALSE
/* 0x7a PMSCAN_SIZE         */ , 0x00,              0x00,              FALSE
/* 0x7b PMSCAN_NLS1         */ , 0x00,              0x00,              FALSE
/* 0x7c PMSCAN_APPLICATION  */ , VK_APPS_W,         VK_APPS_W,         FALSE
/* 0x7d PMSCAN_YEN          */ , VK_YEN_W,          VK_YEN_W,          FALSE
/* 0x7d PMSCAN_E13             , 0x00,              0x00,              FALSE */
/* 0x7e PMSCAN_WINLEFT      */ , VK_LWIN_W,         VK_LWIN_W,         FALSE
/* 0x7f PMSCAN_WINRIGHT     */ , VK_RWIN_W,         VK_RWIN_W,         FALSE
/* 0x80 PMSCAN_PA1          */ , VK_PA1_W,          VK_PA1_W,          FALSE
/* 0x81 PMSCAN_F13          */ , VK_F13_W,          VK_F13_W,          FALSE
/* 0x82 PMSCAN_F14          */ , VK_F14_W,          VK_F14_W,          FALSE
/* 0x83 PMSCAN_F15          */ , VK_F15_W,          VK_F15_W,          FALSE
/* 0x84 PMSCAN_PA2          */ , 0x00,              0x00,              FALSE
/* 0x85 PMSCAN_PA3          */ , 0x00,              0x00,              FALSE
/* 0x86 PMSCAN_SPACEBREAK   */ , 0x00,              0x00,              FALSE
/* 0x87 PMSCAN_TABRIGHT     */ , 0x00,              0x00,              FALSE
/* 0x88 PMSCAN_NOOP         */ , 0x00,              0x00,              FALSE
/* 0x89 PMSCAN_F16          */ , VK_F16_W,          VK_F16_W,          FALSE
/* 0x8a PMSCAN_F17          */ , VK_F17_W,          VK_F17_W,          FALSE
/* 0x8b PMSCAN_F18          */ , VK_F18_W,          VK_F18_W,          FALSE
/* 0x8c PMSCAN_F19          */ , VK_F19_W,          VK_F19_W,          FALSE
/* 0x8d PMSCAN_F20          */ , VK_F20_W,          VK_F20_W,          FALSE
/* 0x8e PMSCAN_F21          */ , VK_F21_W,          VK_F21_W,          FALSE
/* 0x8f PMSCAN_F22          */ , VK_F22_W,          VK_F22_W,          FALSE
/* 0x92                     */ , 0x00,              0x00,              FALSE
/* 0x93                     */ , 0x00,              0x00,              FALSE
/* 0x94                     */ , 0x00,              0x00,              FALSE
/* 0x95                     */ , 0x00,              0x00,              FALSE
/* 0x96                     */ , 0x00,              0x00,              FALSE
/* 0x97                     */ , 0x00,              0x00,              FALSE
/* 0x98                     */ , 0x00,              0x00,              FALSE
/* 0x99                     */ , 0x00,              0x00,              FALSE
/* 0x9A                     */ , 0x00,              0x00,              FALSE
/* 0x9B                     */ , 0x00,              0x00,              FALSE
/* 0x9C                     */ , 0x00,              0x00,              FALSE
/* 0x9D                     */ , 0x00,              0x00,              FALSE
/* 0x9E                     */ , 0x00,              0x00,              FALSE
/* 0x9F                     */ , 0x00,              0x00,              FALSE
/* 0xA0                     */ , 0x00,              0x00,              FALSE
/* 0xA1                     */ , 0x00,              0x00,              FALSE
/* 0xA2                     */ , 0x00,              0x00,              FALSE
/* 0xA3                     */ , 0x00,              0x00,              FALSE
/* 0xA4                     */ , 0x00,              0x00,              FALSE
/* 0xA5                     */ , 0x00,              0x00,              FALSE
/* 0xA6                     */ , 0x00,              0x00,              FALSE
/* 0xA7                     */ , 0x00,              0x00,              FALSE
/* 0xA8                     */ , 0x00,              0x00,              FALSE
/* 0xA9                     */ , 0x00,              0x00,              FALSE
/* 0xAA                     */ , 0x00,              0x00,              FALSE
/* 0xAB                     */ , 0x00,              0x00,              FALSE
/* 0xAC                     */ , 0x00,              0x00,              FALSE
/* 0xAD                     */ , 0x00,              0x00,              FALSE
/* 0xAE                     */ , 0x00,              0x00,              FALSE
/* 0xAF                     */ , 0x00,              0x00,              FALSE
/* 0xB0                     */ , 0x00,              0x00,              FALSE
/* 0xB1                     */ , 0x00,              0x00,              FALSE
/* 0xB2                     */ , 0x00,              0x00,              FALSE
/* 0xB3                     */ , 0x00,              0x00,              FALSE
/* 0xB4                     */ , 0x00,              0x00,              FALSE
/* 0xB5                     */ , 0x00,              0x00,              FALSE
/* 0xB6                     */ , 0x00,              0x00,              FALSE
/* 0xB7                     */ , 0x00,              0x00,              FALSE
/* 0xB8                     */ , 0x00,              0x00,              FALSE
/* 0xB9                     */ , 0x00,              0x00,              FALSE
/* 0xBA                     */ , 0x00,              0x00,              FALSE
/* 0xBB                     */ , 0x00,              0x00,              FALSE
/* 0xBC                     */ , 0x00,              0x00,              FALSE
/* 0xBD                     */ , 0x00,              0x00,              FALSE
/* 0xBE                     */ , 0x00,              0x00,              FALSE
/* 0xBF                     */ , 0x00,              0x00,              FALSE
/* 0xC0                     */ , 0x00,              0x00,              FALSE
/* 0xC1                     */ , 0x00,              0x00,              FALSE
/* 0xC2                     */ , 0x00,              0x00,              FALSE
/* 0xC3                     */ , 0x00,              0x00,              FALSE
/* 0xC4                     */ , 0x00,              0x00,              FALSE
/* 0xC5                     */ , 0x00,              0x00,              FALSE
/* 0xC6                     */ , 0x00,              0x00,              FALSE
/* 0xC7                     */ , 0x00,              0x00,              FALSE
/* 0xC8                     */ , 0x00,              0x00,              FALSE
/* 0xC9                     */ , 0x00,              0x00,              FALSE
/* 0xCA                     */ , 0x00,              0x00,              FALSE
/* 0xCB                     */ , 0x00,              0x00,              FALSE
/* 0xCC                     */ , 0x00,              0x00,              FALSE
/* 0xCD                     */ , 0x00,              0x00,              FALSE
/* 0xCE                     */ , 0x00,              0x00,              FALSE
/* 0xCF                     */ , 0x00,              0x00,              FALSE
/* 0xD0                     */ , 0x00,              0x00,              FALSE
/* 0xD1                     */ , 0x00,              0x00,              FALSE
/* 0xD2                     */ , 0x00,              0x00,              FALSE
/* 0xD3                     */ , 0x00,              0x00,              FALSE
/* 0xD4                     */ , 0x00,              0x00,              FALSE
/* 0xD5                     */ , 0x00,              0x00,              FALSE
/* 0xD6                     */ , 0x00,              0x00,              FALSE
/* 0xD7                     */ , 0x00,              0x00,              FALSE
/* 0xD8                     */ , 0x00,              0x00,              FALSE
/* 0xD9                     */ , 0x00,              0x00,              FALSE
/* 0xDA                     */ , 0x00,              0x00,              FALSE
/* 0xDB                     */ , 0x00,              0x00,              FALSE
/* 0xDC                     */ , 0x00,              0x00,              FALSE
/* 0xDD                     */ , 0x00,              0x00,              FALSE
/* 0xDE                     */ , 0x00,              0x00,              FALSE
/* 0xDF                     */ , 0x00,              0x00,              FALSE
/* 0xE0                     */ , 0x00,              0x00,              FALSE
/* 0xE1                     */ , 0x00,              0x00,              FALSE
/* 0xE2                     */ , 0x00,              0x00,              FALSE
/* 0xE3                     */ , 0x00,              0x00,              FALSE
/* 0xE4                     */ , 0x00,              0x00,              FALSE
/* 0xE5                     */ , 0x00,              0x00,              FALSE
/* 0xE6                     */ , 0x00,              0x00,              FALSE
/* 0xE7                     */ , 0x00,              0x00,              FALSE
/* 0xE8                     */ , 0x00,              0x00,              FALSE
/* 0xE9                     */ , 0x00,              0x00,              FALSE
/* 0xEA                     */ , 0x00,              0x00,              FALSE
/* 0xEB                     */ , 0x00,              0x00,              FALSE
/* 0xEC                     */ , 0x00,              0x00,              FALSE
/* 0xED                     */ , 0x00,              0x00,              FALSE
/* 0xEE                     */ , 0x00,              0x00,              FALSE
/* 0xEF                     */ , 0x00,              0x00,              FALSE
/* 0xF0                     */ , 0x00,              0x00,              FALSE
/* 0xF1                     */ , 0x00,              0x00,              FALSE
/* 0xF2                     */ , 0x00,              0x00,              FALSE
/* 0xF3                     */ , 0x00,              0x00,              FALSE
/* 0xF4                     */ , 0x00,              0x00,              FALSE
/* 0xF5                     */ , 0x00,              0x00,              FALSE
/* 0xF6                     */ , 0x00,              0x00,              FALSE
/* 0xF7                     */ , 0x00,              0x00,              FALSE
/* 0xF8                     */ , 0x00,              0x00,              FALSE
/* 0xF9                     */ , 0x00,              0x00,              FALSE
/* 0xFA                     */ , 0x00,              0x00,              FALSE
/* 0xFB                     */ , 0x00,              0x00,              FALSE
/* 0xFC                     */ , 0x00,              0x00,              FALSE
/* 0xFD                     */ , 0x00,              0x00,              FALSE
/* 0xFE                     */ , 0x00,              0x00,              FALSE
/* 0xFF                     */ , 0x00,              0x00,              FALSE
                               };

/***********************************************************/
/* Table to translate from a Windows vkey to a PM scancode */
/***********************************************************/
BYTE   abWinVKeyToPMScan[256] =
/****************************************/
/* Vkey                   *    Scancode */
/****************************************/
/* 0x00                   */ { 0x00
/* 0x01 VK_LBUTTON_W      */ , 0x00
/* 0x02 VK_RBUTTON_W      */ , 0x00
/* 0x03 VK_CANCEL_W       */ , PMSCAN_BREAK
/* 0x04 VK_MBUTTON_W      */ , 0x00
/* 0x05                   */ , 0x00
/* 0x06                   */ , 0x00
/* 0x07                   */ , 0x00
/* 0x08 VK_BACK_W         */ , PMSCAN_BACKSPACE
/* 0x09 VK_TAB_W          */ , PMSCAN_TAB
/* 0x0A                   */ , 0x00
/* 0x0B                   */ , 0x00
/* 0x0C VK_CLEAR_W        */ , PMSCAN_PAD5
/* 0x0D VK_RETURN_W       */ , PMSCAN_ENTER
/* 0x0E                   */ , 0x00
/* 0x0F                   */ , 0x00
/* 0x10 VK_SHIFT_W        */ , PMSCAN_SHIFTLEFT
/* 0x11 VK_CONTROL_W      */ , PMSCAN_CTRLLEFT
/* 0x12 VK_MENU_W         */ , PMSCAN_ALTLEFT
/* 0x13 VK_PAUSE_W        */ , PMSCAN_PAUSE
/* 0x14 VK_CAPITAL_W      */ , PMSCAN_CAPSLOCK
/* 0x15                   */ , 0x00
/* 0x16                   */ , 0x00
/* 0x17                   */ , 0x00
/* 0x18                   */ , 0x00
/* 0x19                   */ , 0x00
/* 0x1A                   */ , 0x00
/* 0x1B VK_ESCAPE_W       */ , PMSCAN_ESC
/* 0x1C                   */ , 0x00
/* 0x1D                   */ , 0x00
/* 0x1E                   */ , 0x00
/* 0x1F                   */ , 0x00
/* 0x20 VK_SPACE_W        */ , PMSCAN_SPACE
/* 0x21 VK_PRIOR_W        */ , PMSCAN_PAGEUP
/* 0x22 VK_NEXT_W         */ , PMSCAN_PAGEDOWN
/* 0x23 VK_END_W          */ , PMSCAN_END
/* 0x24 VK_HOME_W         */ , PMSCAN_HOME
/* 0x25 VK_LEFT_W         */ , PMSCAN_LEFT
/* 0x26 VK_UP_W           */ , PMSCAN_UP
/* 0x27 VK_RIGHT_W        */ , PMSCAN_RIGHT
/* 0x28 VK_DOWN_W         */ , PMSCAN_DOWN
/* 0x29 VK_SELECT_W       */ , 0x00
/* 0x2A VK_PRINT_W        */ , PMSCAN_PRINT
/* 0x2B VK_EXECUTE_W      */ , 0x00
/* 0x2C VK_SNAPSHOT_W     */ , 0x00
/* 0x2D VK_INSERT_W       */ , PMSCAN_INSERT
/* 0x2E VK_DELETE_W       */ , PMSCAN_DELETE
/* 0x2F VK_HELP_W         */ , PMSCAN_HELP
/* 0x30 VK_0_W            */ , PMSCAN_ZERO
/* 0x31 VK_1_W            */ , PMSCAN_ONE
/* 0x32 VK_2_W            */ , PMSCAN_TWO
/* 0x33 VK_3_W            */ , PMSCAN_THREE
/* 0x34 VK_4_W            */ , PMSCAN_FOUR
/* 0x35 VK_5_W            */ , PMSCAN_FIVE
/* 0x36 VK_6_W            */ , PMSCAN_SIX
/* 0x37 VK_7_W            */ , PMSCAN_SEVEN
/* 0x38 VK_8_W            */ , PMSCAN_EIGHT
/* 0x39 VK_9_W            */ , PMSCAN_NINE
/* 0x3A                   */ , 0x00
/* 0x3B                   */ , 0x00
/* 0x3C                   */ , 0x00
/* 0x3D                   */ , 0x00
/* 0x3E                   */ , 0x00
/* 0x3F                   */ , 0x00
/* 0x40                   */ , 0x00
/* 0x41 VK_A_W            */ , PMSCAN_A
/* 0x42 VK_B_W            */ , PMSCAN_B
/* 0x43 VK_C_W            */ , PMSCAN_C
/* 0x44 VK_D_W            */ , PMSCAN_D
/* 0x45 VK_E_W            */ , PMSCAN_E
/* 0x46 VK_F_W            */ , PMSCAN_F
/* 0x47 VK_G_W            */ , PMSCAN_G
/* 0x48 VK_H_W            */ , PMSCAN_H
/* 0x49 VK_I_W            */ , PMSCAN_I
/* 0x4A VK_J_W            */ , PMSCAN_J
/* 0x4B VK_K_W            */ , PMSCAN_K
/* 0x4C VK_L_W            */ , PMSCAN_L
/* 0x4D VK_M_W            */ , PMSCAN_M
/* 0x4E VK_N_W            */ , PMSCAN_N
/* 0x4F VK_O_W            */ , PMSCAN_O
/* 0x50 VK_P_W            */ , PMSCAN_P
/* 0x51 VK_Q_W            */ , PMSCAN_Q
/* 0x52 VK_R_W            */ , PMSCAN_R
/* 0x53 VK_S_W            */ , PMSCAN_S
/* 0x54 VK_T_W            */ , PMSCAN_T
/* 0x55 VK_U_W            */ , PMSCAN_U
/* 0x56 VK_V_W            */ , PMSCAN_V
/* 0x57 VK_W_W            */ , PMSCAN_W
/* 0x58 VK_X_W            */ , PMSCAN_X
/* 0x59 VK_Y_W            */ , PMSCAN_Y
/* 0x5A VK_Z_W            */ , PMSCAN_Z
/* 0x5B VK_LWIN_W         */ , PMSCAN_WINLEFT
/* 0x5C VK_RWIN_W         */ , PMSCAN_WINRIGHT
/* 0x5D VK_APPS_W         */ , PMSCAN_APPLICATION
/* 0x5E                   */ , 0x00
/* 0x5F                   */ , 0x00
/* 0x60 VK_NUMPAD0_W      */ , PMSCAN_PAD0
/* 0x61 VK_NUMPAD1_W      */ , PMSCAN_PAD1
/* 0x62 VK_NUMPAD2_W      */ , PMSCAN_PAD2
/* 0x63 VK_NUMPAD3_W      */ , PMSCAN_PAD3
/* 0x64 VK_NUMPAD4_W      */ , PMSCAN_PAD4
/* 0x65 VK_NUMPAD5_W      */ , PMSCAN_PAD5
/* 0x66 VK_NUMPAD6_W      */ , PMSCAN_PAD6
/* 0x67 VK_NUMPAD7_W      */ , PMSCAN_PAD7
/* 0x68 VK_NUMPAD8_W      */ , PMSCAN_PAD8
/* 0x69 VK_NUMPAD9_W      */ , PMSCAN_PAD9
/* 0x6A VK_MULTIPLY_W     */ , PMSCAN_PADASTERISK
/* 0x6B VK_ADD_W          */ , PMSCAN_PADPLUS
/* 0x6C VK_SEPARATOR_W    */ , 0x00
/* 0x6D VK_SUBTRACT_W     */ , PMSCAN_PADMINUS
/* 0x6E VK_DECIMAL_W      */ , PMSCAN_PADPERIOD
/* 0x6F VK_DIVIDE_W       */ , PMSCAN_PADSLASH
/* 0x70 VK_F1_W           */ , PMSCAN_F1
/* 0x71 VK_F2_W           */ , PMSCAN_F2
/* 0x72 VK_F3_W           */ , PMSCAN_F3
/* 0x73 VK_F4_W           */ , PMSCAN_F4
/* 0x74 VK_F5_W           */ , PMSCAN_F5
/* 0x75 VK_F6_W           */ , PMSCAN_F6
/* 0x76 VK_F7_W           */ , PMSCAN_F7
/* 0x77 VK_F8_W           */ , PMSCAN_F8
/* 0x78 VK_F9_W           */ , PMSCAN_F9
/* 0x79 VK_F10_W          */ , PMSCAN_F10
/* 0x7A VK_F11_W          */ , PMSCAN_F11
/* 0x7B VK_F12_W          */ , PMSCAN_F12
/* 0x7C VK_F13_W          */ , PMSCAN_F13
/* 0x7D VK_F14_W          */ , PMSCAN_F14
/* 0x7E VK_F15_W          */ , PMSCAN_F15
/* 0x7F VK_F16_W          */ , PMSCAN_F16
/* 0x80 VK_F17_W          */ , PMSCAN_F17
/* 0x81 VK_F18_W          */ , PMSCAN_F18
/* 0x82 VK_F19_W          */ , PMSCAN_F19
/* 0x83 VK_F20_W          */ , PMSCAN_F20
/* 0x84 VK_F21_W          */ , PMSCAN_F21
/* 0x85 VK_F22_W          */ , PMSCAN_F22
/* 0x86 VK_F23_W          */ , PMSCAN_F23
/* 0x87 VK_F24_W          */ , PMSCAN_F24
/* 0x88                   */ , 0x00
/* 0x89                   */ , 0x00
/* 0x8A                   */ , 0x00
/* 0x8B                   */ , 0x00
/* 0x8C                   */ , 0x00
/* 0x8D                   */ , 0x00
/* 0x8E                   */ , 0x00
/* 0x8F                   */ , 0x00
/* 0x90 VK_NUMLOCK_W      */ , PMSCAN_NUMLOCK
/* 0x91 VK_SCROLL_W       */ , PMSCAN_SCROLLLOCK
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
/* 0xA0 VK_LSHIFT_W       */ , PMSCAN_SHIFTLEFT
/* 0xA1 VK_RSHIFT_W       */ , PMSCAN_SHIFTRIGHT
/* 0xA2 VK_LCONTROL_W     */ , PMSCAN_CTRLLEFT
/* 0xA3 VK_RCONTROL_W     */ , PMSCAN_CTRLRIGHT
/* 0xA4 VK_LMENU_W        */ , PMSCAN_ALTLEFT
/* 0xA5 VK_RMENU_W        */ , PMSCAN_ALTRIGHT
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
/* 0xBA VK_SEMICOLON_W    */ , PMSCAN_SEMICOLON
/* 0xBB VK_EQUAL_W        */ , PMSCAN_EQUAL
/* 0xBC VK_COMMA_W        */ , PMSCAN_COMMA
/* 0xBD VK_HYPHEN_W       */ , PMSCAN_HYPHEN
/* 0xBE VK_PERIOD_W       */ , PMSCAN_PERIOD
/* 0xBF VK_SLASH_W        */ , PMSCAN_SLASH
/* 0xC0 VK_GRAVE_W        */ , PMSCAN_GRAVE
/* 0xC1 VK_FFC1_W         */ , PMSCAN_JEXTRA
/* 0xC2 VK_YEN_W          */ , PMSCAN_YEN
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
/* 0xDB VK_BRACKETLEFT_W  */ , PMSCAN_BRACKETLEFT
/* 0xDC VK_BACKSLASH_W    */ , PMSCAN_BACKSLASH
/* 0xDD VK_BRACKETRIGHT_W */ , PMSCAN_BRACKETRIGHT
/* 0xDE VK_QUOTESINGLE_W  */ , PMSCAN_QUOTESINGLE
/* 0xDF                   */ , 0x00
/* 0xE0                   */ , 0x00
/* 0xE1                   */ , 0x00
/* 0xE2 VK_EXTRA_W        */ , PMSCAN_EXTRA
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
/* 0xF6 VK_ATTN_W         */ , 0x00
/* 0xF7 VK_CRSEL_W        */ , 0x00
/* 0xF8 VK_EXSEL_W        */ , 0x00
/* 0xF9 VK_EREOF_W        */ , PMSCAN_ERASEEOF
/* 0xFA VK_PLAY_W         */ , 0x00
/* 0xFB VK_ZOOM_W         */ , 0x00
/* 0xFC VK_NONAME_W       */ , 0x00
/* 0xFD VK_PA1_W          */ , PMSCAN_PA1
/* 0xFE VK_OEM_CLEAR_W    */ , 0x00
/* 0xFF                   */ , 0x00
                             };

/* Virtual key codes */
#define VK_LBUTTON          0x01
#define VK_RBUTTON          0x02
#define VK_CANCEL           0x03
#define VK_MBUTTON          0x04
/*                          0x05-0x07  Undefined */
#define VK_BACK             0x08
#define VK_TAB              0x09
/*                          0x0A-0x0B  Undefined */
#define VK_CLEAR            0x0C
#define VK_RETURN           0x0D
/*                          0x0E-0x0F  Undefined */
#define VK_SHIFT            0x10
#define VK_CONTROL          0x11
#define VK_MENU             0x12
#define VK_PAUSE            0x13
#define VK_CAPITAL          0x14
/*                          0x15-0x19  Reserved for Kanji systems */
/*                          0x1A       Undefined */
#define VK_ESCAPE           0x1B
/*                          0x1C-0x1F  Reserved for Kanji systems */
#define VK_SPACE            0x20
#define VK_PRIOR            0x21
#define VK_NEXT             0x22
#define VK_END              0x23
#define VK_HOME             0x24
#define VK_LEFT             0x25
#define VK_UP               0x26
#define VK_RIGHT            0x27
#define VK_DOWN             0x28
#define VK_SELECT           0x29
#define VK_PRINT            0x2A /* OEM specific in Windows 3.1 SDK */
#define VK_EXECUTE          0x2B
#define VK_SNAPSHOT         0x2C
#define VK_INSERT           0x2D
#define VK_DELETE           0x2E
#define VK_HELP             0x2F
#define VK_0                0x30
#define VK_1                0x31
#define VK_2                0x32
#define VK_3                0x33
#define VK_4                0x34
#define VK_5                0x35
#define VK_6                0x36
#define VK_7                0x37
#define VK_8                0x38
#define VK_9                0x39
/*                          0x3A-0x40  Undefined */
#define VK_A                0x41
#define VK_B                0x42
#define VK_C                0x43
#define VK_D                0x44
#define VK_E                0x45
#define VK_F                0x46
#define VK_G                0x47
#define VK_H                0x48
#define VK_I                0x49
#define VK_J                0x4A
#define VK_K                0x4B
#define VK_L                0x4C
#define VK_M                0x4D
#define VK_N                0x4E
#define VK_O                0x4F
#define VK_P                0x50
#define VK_Q                0x51
#define VK_R                0x52
#define VK_S                0x53
#define VK_T                0x54
#define VK_U                0x55
#define VK_V                0x56
#define VK_W                0x57
#define VK_X                0x58
#define VK_Y                0x59
#define VK_Z                0x5A

#define VK_LWIN             0x5B
#define VK_RWIN             0x5C
#define VK_APPS             0x5D
/*                          0x5E-0x5F Unassigned */
#define VK_NUMPAD0          0x60
#define VK_NUMPAD1          0x61
#define VK_NUMPAD2          0x62
#define VK_NUMPAD3          0x63
#define VK_NUMPAD4          0x64
#define VK_NUMPAD5          0x65
#define VK_NUMPAD6          0x66
#define VK_NUMPAD7          0x67
#define VK_NUMPAD8          0x68
#define VK_NUMPAD9          0x69
#define VK_MULTIPLY         0x6A
#define VK_ADD              0x6B
#define VK_SEPARATOR        0x6C
#define VK_SUBTRACT         0x6D
#define VK_DECIMAL          0x6E
#define VK_DIVIDE           0x6F
#define VK_F1               0x70
#define VK_F2               0x71
#define VK_F3               0x72
#define VK_F4               0x73
#define VK_F5               0x74
#define VK_F6               0x75
#define VK_F7               0x76
#define VK_F8               0x77
#define VK_F9               0x78
#define VK_F10              0x79
#define VK_F11              0x7A
#define VK_F12              0x7B
#define VK_F13              0x7C
#define VK_F14              0x7D
#define VK_F15              0x7E
#define VK_F16              0x7F
#define VK_F17              0x80
#define VK_F18              0x81
#define VK_F19              0x82
#define VK_F20              0x83
#define VK_F21              0x84
#define VK_F22              0x85
#define VK_F23              0x86
#define VK_F24              0x87
/*                          0x88-0x8F  Unassigned */
#define VK_NUMLOCK          0x90
#define VK_SCROLL           0x91
/*                          0x92-0x9F  Unassigned */
/*
 * differencing between right and left shift/control/alt key.
 * Used only by GetAsyncKeyState() and GetKeyState().
 */
#define VK_LSHIFT           0xA0
#define VK_RSHIFT           0xA1
#define VK_LCONTROL         0xA2
#define VK_RCONTROL         0xA3
#define VK_LMENU            0xA4
#define VK_RMENU            0xA5
/*                          0xA6-0xB9  Unassigned */
#define VK_OEM_1            0xBA
#define VK_OEM_PLUS         0xBB
#define VK_OEM_COMMA        0xBC
#define VK_OEM_MINUS        0xBD
#define VK_OEM_PERIOD       0xBE
#define VK_OEM_2            0xBF
#define VK_OEM_3            0xC0
/*                          0xC1-0xDA  Unassigned */
#define VK_OEM_4            0xDB
#define VK_OEM_5            0xDC
#define VK_OEM_6            0xDD
#define VK_OEM_7            0xDE
/*                          0xDF-0xE4  OEM specific */

#define VK_PROCESSKEY       0xE5

/*                          0xE6       OEM specific */
/*                          0xE7-0xE8  Unassigned */
/*                          0xE9-0xF5  OEM specific */

#define VK_ATTN             0xF6
#define VK_CRSEL            0xF7
#define VK_EXSEL            0xF8
#define VK_EREOF            0xF9
#define VK_PLAY             0xFA
#define VK_ZOOM             0xFB
#define VK_NONAME           0xFC
#define VK_PA1              0xFD
#define VK_OEM_CLEAR        0xFE
