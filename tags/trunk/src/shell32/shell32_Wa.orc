/* $Id: shell32_Wa.orc,v 1.2 2000-03-26 16:34:49 cbratschi Exp $ */
LANGUAGE LANG_WALON, SUBLANG_DEFAULT

/*
 * Si vos voloz aider avou li rato�rnaedje des libes programes � walon,
 * vos poloz scr�re a l' adresse emile <linux-wa@chanae.alphanet.ch>
 */

SHELL_ABOUT_MSGBOX DIALOG LOADONCALL MOVEABLE DISCARDABLE 15, 40, 210, 152
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "� dfait di %s"
FONT 10, "System"
{
 DEFPUSHBUTTON "I Va", IDOK, 153, 130, 50, 12
 LISTBOX 99, 8, 65, 137, 82, LBS_NOTIFY | WS_VSCROLL | WS_BORDER
 ICON "", 1088, 189, 10, 14, 16
 LTEXT "", 100, 8, 10, 137, 33
 LTEXT "Odin a estu fwait par:", 98, 8, 55, 137, 10
}

