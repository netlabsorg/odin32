/* $Id: shell32_Wa.orc,v 1.2 2003-11-12 14:04:55 sandervl Exp $ */
LANGUAGE LANG_WALON, SUBLANG_DEFAULT

/*
 * Si vos voloz aider avou li ratoûrnaedje des libes programes è walon,
 * vos poloz scrîre a l' adresse emile <linux-wa@chanae.alphanet.ch>
 */

SHELL_ABOUT_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 216, 170
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "Å dfait di %s"
FONT 10, "System"
{
 DEFPUSHBUTTON "OK", IDOK, 158, 167, 50, 12, WS_TABSTOP
 LISTBOX 99, 8, 105, 137, 82, LBS_NOTIFY | WS_VSCROLL | WS_BORDER
 ICON "", 1088, 194, 75, 0, 0
 LTEXT "", 100, 8, 75, 176, 20
 LTEXT "Odin %d.%d.%d (%s) a estu fwait par:", 98, 8, 95, 176, 10
 CONTROL "", 2001, "Static", SS_BITMAP, 8, 8, 0, 0
}
