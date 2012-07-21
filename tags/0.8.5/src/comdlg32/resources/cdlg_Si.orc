/*
 * Copyright 2002 Rok Mandeljc <rok.mandeljc@gimb.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

LANGUAGE LANG_SLOVENIAN, SUBLANG_DEFAULT

OPEN_FILE DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 275, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Odpiranje"
FONT 8, "MS Sans Serif"
{
 LTEXT "Ime &datoteke:", 1090, 6, 6, 76, 9
 EDITTEXT edt1, 6, 16, 90, 12, ES_AUTOHSCROLL | ES_OEMCONVERT | WS_BORDER | WS_TABSTOP
 LISTBOX 1120, 6, 32, 90, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&Mape:", -1, 110, 6, 92, 9
 LTEXT "", 1088, 110, 18, 92, 9, SS_NOPREFIX | WS_GROUP
 LISTBOX 1121, 110, 32, 92, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&Vrsta datotek:", 1089, 6, 104, 90, 9
 COMBOBOX cmb1, 6, 114, 90, 36, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 LTEXT "Pog&oni:", 1091, 110, 104, 92, 9
 COMBOBOX cmb2, 110, 114, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "V redu", 1, 208, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Prekli�i", 2, 208, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Pomo�", pshHelp, 208, 46, 56, 14, WS_GROUP | WS_TABSTOP
 CHECKBOX "Samo za b&ranje", chx1, 208, 68, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


SAVE_FILE DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 275, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Shrani kot"
FONT 8, "MS Sans Serif"
{
 LTEXT "Ime &datoteke:", 1090, 6, 6, 76, 9
 EDITTEXT edt1, 6, 16, 90, 12, ES_AUTOHSCROLL | ES_OEMCONVERT | WS_BORDER | WS_TABSTOP
 LISTBOX 1120, 6, 32, 90, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&Mape:", -1, 110, 6, 92, 9
 LTEXT "", 1088, 110, 18, 92, 9, SS_NOPREFIX | WS_GROUP
 LISTBOX 1121, 110, 32, 92, 68, LBS_STANDARD | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL | WS_TABSTOP
 LTEXT "&Vrsta datotek:", 1089, 6, 104, 90, 9
 COMBOBOX cmb1, 6, 114, 90, 36, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 LTEXT "Pog&oni:", 1091, 110, 104, 92, 9
 COMBOBOX cmb2, 110, 114, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "V redu", 1, 208, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Prekli�i", 2, 208, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Pomo�", pshHelp, 208, 46, 56, 14, WS_GROUP | WS_TABSTOP
 CHECKBOX "Samo za b&ranje", chx1, 208, 68, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


PRINT DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 264, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Tiskanje"
FONT 8, "MS Sans Serif"
{
 LTEXT "Tiskalnik:", 1088, 6, 6, 40, 9
 LTEXT "", 1089, 60, 6, 150, 9
 GROUPBOX "Obseg tiskanja", grp1, 6, 30, 160, 65, BS_GROUPBOX
 RADIOBUTTON "&Vse", rad1, 16, 45, 60, 12
 RADIOBUTTON "I&zbor", rad2, 16, 60, 60, 12
 RADIOBUTTON "St&rani", rad3, 16, 75, 60, 12
 DEFPUSHBUTTON "V redu", 1, 206, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Prekli�i", 2, 206, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Pripr&ava ...", psh1, 206, 46, 56, 14, WS_GROUP | WS_TABSTOP
 LTEXT "&od:", 1090, 60, 80, 30, 9
 LTEXT "&do:", 1091, 120, 80, 30, 9
 LTEXT "Kakovost ti&skanja:", 1092, 6, 100, 76, 9
 COMBOBOX cmb1, 80, 100, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 CHECKBOX "&Tiskanje v datoteko", chx1, 20, 100, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "Z&biranje kopij", chx2, 160, 100, 50, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
}


PRINT_SETUP DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 264, 134
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Priprava za tiskanje"
FONT 8, "MS Sans Serif"
{
 GROUPBOX "Tiskalnik", grp1, 6, 10, 180, 65, BS_GROUPBOX
 RADIOBUTTON "Privzeti &tiskalnik", rad1, 16, 20, 80, 12
 LTEXT "(Ni privzetega tiskalnika)", 1088, 35, 35, 120, 9
 RADIOBUTTON "Dolo�eni ti&skalnik:", rad2, 16, 50, 80, 12
 COMBOBOX cmb1, 35, 65, 149, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 DEFPUSHBUTTON "V redu", IDOK, 206, 6, 56, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Prekli�i", IDCANCEL, 206, 24, 56, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Mo�nosti ...", psh1, 206, 46, 56, 14, WS_GROUP | WS_TABSTOP
 GROUPBOX "Usmerjenost", grp2, 6, 85, 100, 50, BS_GROUPBOX
 RADIOBUTTON "P&okon�na", rad3, 50, 100, 40, 12
 RADIOBUTTON "L&e�e�a", rad4, 50, 115, 40, 12
 ICON "LANDSCAP", stc10, 10, 95, 32, 32
 ICON "PORTRAIT", stc11, 10, 95, 32, 32
 GROUPBOX "Papir", grp3, 120, 85, 180, 50, BS_GROUPBOX
 LTEXT "Veli&kost:", 1089, 130, 95, 30, 9
 LTEXT "&Vir:", 1090, 130, 110, 30, 9
 COMBOBOX cmb2, 155, 95, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
 COMBOBOX cmb3, 155, 110, 92, 68, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_SORT | CBS_HASSTRINGS | WS_BORDER | WS_VSCROLL | WS_TABSTOP
}


CHOOSE_FONT DIALOG DISCARDABLE  13, 54, 264, 147
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Pisava"
FONT 8, "MS Sans Serif"
{
    LTEXT           "Pis&ava:",1088 ,6,3,40,9
    COMBOBOX        cmb1, 6,13,94,54,  CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    CBS_AUTOHSCROLL | CBS_SORT | WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE
    LTEXT           "Slog pi&save:",1089 ,108,3,44,9
    COMBOBOX        cmb2,108,13,64,54, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE
    LTEXT           "&Velikost:",1090,179,3,30,9
    COMBOBOX        cmb3,179,13,32,54, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DISABLENOSCROLL |
                    WS_VSCROLL | WS_TABSTOP | CBS_SIMPLE | CBS_SORT
    DEFPUSHBUTTON   "V redu",IDOK,218,6,40,14, WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON
    PUSHBUTTON      "Prekli�i",IDCANCEL,218,23,40,14,WS_GROUP | WS_TABSTOP
    PUSHBUTTON      "&Uporabi", psh3,218,40,40,14,WS_GROUP | WS_TABSTOP
    PUSHBUTTON      "&Pomo�" , pshHelp,218,57,40,14,WS_GROUP | WS_TABSTOP
    GROUPBOX        "U�inki",1072,6,72,84,34,WS_GROUP
    CHECKBOX	    "Pr&e�rtano", chx1, 10,82,50,10, BS_AUTOCHECKBOX | WS_TABSTOP
    CHECKBOX 	    "Po&d�rtano", chx2, 10,94,50,10, BS_AUTOCHECKBOX
    LTEXT           "&Barva:", 1091 ,6,110,30,9
    COMBOBOX        cmb4,6,120,84,100,CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS |
		    CBS_AUTOHSCROLL |  WS_BORDER | WS_VSCROLL | WS_TABSTOP
    GROUPBOX        "Vzorec",1073,98,72,160,49,WS_GROUP
    CTEXT           "AaBbYyZz",1093,104,81,149,37,SS_NOPREFIX | WS_VISIBLE
}


CHOOSE_COLOR DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 300, 185
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Barva"
FONT 8, "MS Sans Serif"
{
 LTEXT "Osno&vne barve:",   1088, 4,    4,  140, 10
 LTEXT "Name�ane &barve:",  1089, 4,   106, 140, 10
 LTEXT "Barva | T&emelj",  1090, 150, 151,  48, 10
 LTEXT   "&Rde�a:", 726 /*1094*/,249,126,24,10
 EDITTEXT 706, 275,124,21,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT   "&Zelena:",727/*1095*/,249,140,24,10
 EDITTEXT 707, 275,138,21,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT   "Modr&a:",728 /*1096*/,249,154,24,10
 EDITTEXT 708, 275,152,21,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "Od&tenek:" ,723 /*1091*/,202,126,22,10
 EDITTEXT 703, 226,124,21,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&Nasi�enost:" ,724 /*1092*/,202,140,22,10
 EDITTEXT 704, 226,138,21,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT  "&Svetlost:" ,725 /*1093*/,202,154,22,10
 EDITTEXT 705, 226,152,21,12, WS_BORDER | WS_GROUP | WS_TABSTOP
 CONTROL "" ,720,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP,4,14,140,86
 CONTROL "" ,721,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP,4,116,140,28
 CONTROL "" ,710,"STATIC",WS_BORDER|SS_SIMPLE|WS_TABSTOP|WS_GROUP, 152,4,118,116
 CONTROL "" ,702,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP, 278,4,8,116
 CONTROL "" ,709,"STATIC",SS_SIMPLE|WS_TABSTOP|WS_GROUP, 152,124,40,26
 DEFPUSHBUTTON "V redu",  1,  4, 166, 44, 14, BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Prekli�i", 2, 52, 166, 44, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Pomo�", pshHelp,100,166, 44, 14
 PUSHBUTTON "&Dodaj med name�ane barve",    712/*1024*/, 152, 166, 142, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Me�anje barv >>", 719/*1025*/,   4, 150, 142, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON  "&i",713,300,200,4,14   /* just a dummy:  'i' is  like  &i  in "sol&id"  */
}


FINDDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 236, 62
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Iskanje"
FONT 8, "MS Sans Serif"
{
 LTEXT "N&ajdi:", -1, 4, 8, 42, 8
 EDITTEXT edt1, 47, 7, 128, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 CHECKBOX "&Samo cele besede", chx1, 4, 26, 100, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "&Razlikovanje malih in velikih �rk", chx2, 4, 42, 64, 12, BS_AUTOCHECKBOX | WS_TABSTOP
 GROUPBOX "Smer iskanja", grp1, 107, 26, 68, 28
 CONTROL "&Gor", rad1, "BUTTON", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 111, 38, 20, 12
 CONTROL "&Dol", rad2, "BUTTON", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 138, 38, 30, 12

 DEFPUSHBUTTON "&Nadaljuj iskanje", IDOK, 182,  5, 50, 14, WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON
 PUSHBUTTON "Prekli�i", IDCANCEL	 , 182, 23, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Pomo�", pshHelp	 , 182, 45, 50, 14, WS_GROUP | WS_TABSTOP
}


REPLACEDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 36, 24, 230, 94
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Zamenjava"
FONT 8, "MS Sans Serif"
{
 LTEXT "N&ajdi:", -1, 4, 9, 48, 8
 EDITTEXT edt1, 54, 7, 114, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 LTEXT "Za&menjaj z:", -1, 4, 26, 48, 8
 EDITTEXT edt2, 54, 24, 114, 12, ES_AUTOHSCROLL | WS_BORDER | WS_GROUP | WS_TABSTOP
 CHECKBOX "&Samo cele besede", chx1, 5, 46, 104, 12, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
 CHECKBOX "&Razlikovanje malih in velikih �rk", chx2, 5, 62, 59, 12, BS_AUTOCHECKBOX | WS_TABSTOP

 DEFPUSHBUTTON "&Nadaljuj iskanje", IDOK, 174,  4, 50, 14, WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON
 PUSHBUTTON "&Zamenjaj", psh1	 , 174, 21, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Zamenjaj &vse", psh2 , 174, 38, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "Prekli�i", IDCANCEL	 , 174, 55, 50, 14, WS_GROUP | WS_TABSTOP
 PUSHBUTTON "&Pomo�", pshHelp	 , 174, 75, 50, 14, WS_GROUP | WS_TABSTOP
}


PRINT32 DIALOG LOADONCALL MOVEABLE DISCARDABLE  32, 32, 288, 186
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
      DS_CONTEXTHELP | DS_3DLOOK
CAPTION "Tiskanje"
FONT 8, "MS Sans Serif"
{
    DEFPUSHBUTTON   "V redu",     IDOK,     180,164, 48,14, WS_GROUP | BS_DEFPUSHBUTTON
    PUSHBUTTON      "Prekli�i", IDCANCEL, 232,164, 48,14, WS_GROUP
    PUSHBUTTON      "&Pomo�",  pshHelp,  50, 161, 48,14, WS_GROUP

    GROUPBOX        "Tiskalnik",        grp4,   8,  4, 272,84, WS_GROUP
    CONTROL         "&Tiskanje v datoteko", chx1, "Button",BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP,212,70,64,12
    PUSHBUTTON      "&Lastnosti",    psh2, 212, 17,  60,14, WS_GROUP
    LTEXT           "&Ime:",         stc6,  16, 20,  36,8
    COMBOBOX                          cmb4,  52, 18, 152,152,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "Stanje:",        stc8,  16, 36,  36,10, SS_NOPREFIX
    LTEXT           "Dummy State",    stc12, 52, 36, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Vrsta:",          stc7,  16, 48,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Type",     stc11, 52, 48, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Kje:",         stc10, 16, 60,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Location", stc14, 52, 60, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Komentar:",       stc9,  16, 72,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Remark",   stc13, 52, 72, 152,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP

    GROUPBOX        "Kopije",         grp2, 160, 92, 120,64, WS_GROUP
    LTEXT           "�tevilo &kopij:",stc5,168,108,68,8
    ICON            "",               ico3, 162,124,  76,24, WS_GROUP | SS_CENTERIMAGE
    CONTROL         "Z&biranje",       chx2,"Button",BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP,240,130,36,12
    EDITTEXT                          edt3, 240,106,  32,12, WS_GROUP | ES_NUMBER

    GROUPBOX        "Obseg tiskanja",    grp1,   8,92,  144,64, WS_GROUP
    CONTROL         "&Vse",           rad1,"Button",BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP,16,106,64,12
    CONTROL         "St&rani",         rad3,"Button",BS_AUTORADIOBUTTON,16,122,36,12
    CONTROL         "I&zbor",     rad2,"Button",BS_AUTORADIOBUTTON,16,138,64,12
    EDITTEXT                          edt1,  74,122,  26,12, WS_GROUP | ES_NUMBER
    EDITTEXT                          edt2, 118,122,  26,12, WS_GROUP | ES_NUMBER
    RTEXT           "&od:",         stc2,  52,124,  20,8
    RTEXT           "&do:",           stc3, 100,124,  16,8
}

PRINT32_SETUP DIALOG LOADONCALL MOVEABLE DISCARDABLE  32, 32, 288, 178
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
      DS_CONTEXTHELP | DS_3DLOOK
CAPTION "Priprava za tiskanje"
FONT 8, "MS Sans Serif"
BEGIN
    DEFPUSHBUTTON   "V redu",IDOK,180,156,48,14,WS_GROUP
    PUSHBUTTON      "Prekli�i",IDCANCEL,232,156,48,14
/*    PUSHBUTTON      "Omre�je...", psh5, 284,156,48,14 */

    GROUPBOX        "Tiskalnik",        grp4,   8,  4, 272,84, WS_GROUP
    PUSHBUTTON      "&Lastnosti",    psh2, 212, 17,  60,14, WS_GROUP
    LTEXT           "&Ime:",         stc6,  16, 20,  36,8
    COMBOBOX                          cmb1,  52, 18, 152,152,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "Stanje:",        stc8,  16, 36,  36,10, SS_NOPREFIX
    LTEXT           "Dummy State",    stc12, 52, 36, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Vrsta:",          stc7,  16, 48,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Type",     stc11, 52, 48, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Kje:",         stc10, 16, 60,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Location", stc14, 52, 60, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP
    LTEXT           "Komentar:",       stc9,  16, 72,  36,10, SS_NOPREFIX
    LTEXT           "Dummy Remark",   stc13, 52, 72, 224,10, SS_NOPREFIX | SS_LEFTNOWORDWRAP

    GROUPBOX        "Papir",          grp2,   8, 92, 164,56, WS_GROUP
    LTEXT           "Veli&kost:",         stc2,  16,108,  36, 8
    COMBOBOX                          cmb2,  52,106, 112,112,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    LTEXT           "&Vir:",       stc3,  16,128,  36, 8
    COMBOBOX                          cmb3,  52,126, 112,112,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP

    GROUPBOX        "Usmerjenost",    grp1, 180, 92, 100,56, WS_GROUP
    ICON            "",               ico1, 195,112,  18,20, WS_GROUP
    CONTROL         "P&okon�na",      rad1,"Button",BS_AUTORADIOBUTTON | WS_GROUP |WS_TABSTOP,224,106,52,12
    CONTROL         "L&e�e�a",     rad2,"Button",BS_AUTORADIOBUTTON,224,126,52,12
END

PAGESETUPDLGORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 32, 32, 240, 240
STYLE DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "Priprava strani"
FONT 8, "MS Sans Serif"
BEGIN
  CONTROL "", rct1, 130, 0x00020006, 80, 8, 80, 80,  SS_WHITERECT
  CONTROL "", rct2, 130, 0x00020005, 160, 12, 4, 80, SS_GRAYRECT
  CONTROL "", rct3, 130, 0x00020005, 84, 88, 80, 4,  SS_GRAYRECT
  GROUPBOX "Papir", grp2, 8, 96, 224, 56, BS_GROUPBOX
  LTEXT "Veli&kost:", stc2, 16, 112, 36, 8
  COMBOBOX cmb2, 64, 110, 160, 160, CBS_SIMPLE|CBS_DROPDOWN|CBS_SORT|WS_GROUP|WS_TABSTOP|WS_VSCROLL
  LTEXT "&Vir:", stc3, 16, 132, 36, 8
  COMBOBOX cmb3, 64, 130, 160, 160, CBS_SIMPLE|CBS_DROPDOWN|CBS_SORT|WS_GROUP|WS_TABSTOP|WS_VSCROLL
  GROUPBOX "Usmerjenost", grp1, 8, 156, 64, 56, BS_GROUPBOX
  AUTORADIOBUTTON "P&okon�na", rad1, 16, 170, 52, 12, BS_AUTORADIOBUTTON
  AUTORADIOBUTTON "L&e�e�a", rad2, 16, 190, 52, 12, BS_AUTORADIOBUTTON
  GROUPBOX "Robovi", grp4, 80, 156, 152, 56, BS_GROUPBOX
  LTEXT "&Levo:", stc15, 88, 172, 21, 8
  EDITTEXT edt4, 111, 170, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Desno:", stc16, 159, 172, 27, 8
  EDITTEXT edt6, 187, 170, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Zgoraj:", stc17, 88, 192, 21, 8
  EDITTEXT edt5, 111, 190, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  LTEXT "&Spodaj:", stc18, 159, 192, 23, 8
  EDITTEXT edt7, 187, 190, 39, 12, WS_TABSTOP|WS_GROUP|WS_BORDER
  DEFPUSHBUTTON "V redu", IDOK, 71, 220, 50, 14, BS_PUSHBUTTON
  PUSHBUTTON "Prekli�i", IDCANCEL, 126, 220, 50, 14
  PUSHBUTTON "&Tiskalnik ...", psh3, 184, 220, 48, 14
END



NEWFILEOPENORD DIALOG LOADONCALL MOVEABLE DISCARDABLE 0, 0, 287, 165
STYLE DS_MODALFRAME | DS_CONTEXTHELP | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Odpiranje"
FONT 8, "MS Sans Serif"
{
    LTEXT	"&I��i v",IDC_LOOKINSTATIC,4,6,43,8, SS_NOTIFY
    COMBOBOX	IDC_LOOKIN,49,3,132,100,CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP

    LTEXT	"" , IDC_TOOLBARSTATIC, 181, 2, 102, 17, NOT WS_GROUP | NOT WS_VISIBLE
    LISTBOX	IDC_SHELLSTATIC,4,20,272,85, LBS_SORT | LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | WS_HSCROLL | NOT WS_VISIBLE

    LTEXT	"Ime &datoteke:",IDC_FILENAMESTATIC,5,112,46,8, SS_NOTIFY
    EDITTEXT	IDC_FILENAME,54,110,155,12,ES_AUTOHSCROLL

    LTEXT	"Vrsta da&totek:",IDC_FILETYPESTATIC,5,128,42,8, SS_NOTIFY
    COMBOBOX	IDC_FILETYPE,54,126,155,53,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP

    CONTROL	"&Samo za branje",IDC_OPENREADONLY,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,54,145,100,10

    DEFPUSHBUTTON	"&Odpri",		 IDOK,222,110,50,14
    PUSHBUTTON		"Prekli�i",		 IDCANCEL,222,128,50,14
    PUSHBUTTON		"&Pomo�",		 pshHelp,222,145,50,14
}

STRINGTABLE DISCARDABLE
{
    IDS_ABOUTBOX            "&O FolderPicker Test"
    IDS_DOCUMENTFOLDERS     "Mape z dokumenti"
    IDS_PERSONAL            "Moji dokumenti"
    IDS_FAVORITES           "Priljubljene"
    IDS_PATH                "Sistemska pot"
    IDS_DESKTOP             "Namizje"
    IDS_FONTS               "Pisave"
    IDS_MYCOMPUTER          "Moj ra�unalnik"
}

STRINGTABLE DISCARDABLE
{
    IDS_SYSTEMFOLDERS       "System Folders"
    IDS_LOCALHARDRIVES      "Local Hard Drives"
    IDS_FILENOTFOUND        "File not found"
    IDS_VERIFYFILE          "Please verify if the correct file name was given"
    IDS_CREATEFILE          "File does not exist\nDo you want to create file"
    IDL_OVERWRITEFILE       "File does already exist.\nDo you want to replace it?"
    IDS_INVALID_FILENAME_TITLE "Invalid character(s) in path"
    IDS_INVALID_FILENAME    "A filename cannot contain any of the following characters: \n                          / : < > |"
    IDS_PATHNOTEXISTING     "Path does not exist"
    IDS_FILENOTEXISTING     "File does not exist"
}

STRINGTABLE DISCARDABLE
{
    IDS_UPFOLDER         "V nadrejeno mapo"
    IDS_NEWFOLDER        "Ustvari novo mapo"
    IDS_LISTVIEW         "Seznam"
    IDS_REPORTVIEW       "Podrobnosti"
    IDS_TODESKTOP        "Pogled namizja"
}

STRINGTABLE DISCARDABLE
{
    PD32_PRINT_TITLE       "Tiskalnik:"

    PD32_VALUE_UREADABLE                  "Velikost mora biti izra�ena s �tevilko."
    PD32_INVALID_PAGE_RANGE "Ta vrednost ni znotraj dovoljenega obsega.\n\
Vnesite �tevilo med %d in %d."
    PD32_FROM_NOT_ABOVE_TO                "Vrednost 'od' ne more biti ve�ja od vrednosti 'do'."
    PD32_MARGINS_OVERLAP                  "Robovi se ne prekrivajo ali pa niso na listu papirja.\nVnesite nove velikosti robov."
    PD32_NR_OF_COPIES_EMPTY               "�tevilo kopij ne more biti 0 in mora biti pozitivno."
    PD32_TOO_LARGE_COPIES                 "Ta tiskalnik ne omogo�a tolik�nega \
�tevila kopij.\nVnesite �tevilo med 1 in %d."
    PD32_PRINT_ERROR                      "Napaka pri tiskanju"
    PD32_NO_DEFAULT_PRINTER               "Preden lahko tiskate, morate namestiti tiskalnik."
    PD32_CANT_FIND_PRINTER                "Tiskalnika ni bilo mogo�e najti."
    PD32_OUT_OF_MEMORY                    "Na voljo ni dovolj pomnilnika, da bi bilo mogo�e dokon�ati operacijo."
    PD32_GENERIC_ERROR                    "Med operacijo je pri�lo do napake."
    PD32_DRIVER_UNKNOWN                   "Neznan gonilnik tiskalnika."

    PD32_DEFAULT_PRINTER                  "Privzeti tiskalnik; "
    PD32_NR_OF_DOCUMENTS_IN_QUEUE         "%d �akajo�ih dok."
    PD32_PRINT_ALL_X_PAGES                "&Vse strani -- %d"
    PD32_MARGINS_IN_INCHES                "Robovi [palci]"
    PD32_MARGINS_IN_MILIMETERS            "Robovi [mm]"
    PD32_MILIMETERS                       "mm"

    PD32_PRINTER_STATUS_READY             "Pripravljen"
    PD32_PRINTER_STATUS_PAUSED            "Za�asno zaustavljen; "
    PD32_PRINTER_STATUS_ERROR             "Napaka; "
    PD32_PRINTER_STATUS_PENDING_DELETION  "Pri�lo bo do brisanja; "
    PD32_PRINTER_STATUS_PAPER_JAM         "Papir se je zagozdil; "
    PD32_PRINTER_STATUS_PAPER_OUT         "Ni ve� papirja; "
    PD32_PRINTER_STATUS_MANUAL_FEED       "Ro�no podajanje; "
    PD32_PRINTER_STATUS_PAPER_PROBLEM     "Te�ave s papirjem; "
    PD32_PRINTER_STATUS_OFFLINE           "Ne sprejema podatkov; "
    PD32_PRINTER_STATUS_IO_ACTIVE         "V/I aktiven; "
    PD32_PRINTER_STATUS_BUSY              "Zaposlen; "
    PD32_PRINTER_STATUS_PRINTING          "Tiska; "
    PD32_PRINTER_STATUS_OUTPUT_BIN_FULL   "Izhodni predal je poln; "
    PD32_PRINTER_STATUS_NOT_AVAILABLE     "Ni na voljo; "
    PD32_PRINTER_STATUS_WAITING           "�aka; "
    PD32_PRINTER_STATUS_PROCESSING        "Obdeluje; "
    PD32_PRINTER_STATUS_INITIALIZING      "Se inicializira; "
    PD32_PRINTER_STATUS_WARMING_UP        "Se ogreva; "
    PD32_PRINTER_STATUS_TONER_LOW         "Zmanjkuje tonerja; "
    PD32_PRINTER_STATUS_NO_TONER          "Ni tonerja; "
    PD32_PRINTER_STATUS_PAGE_PUNT         "Predmet strani; "
    PD32_PRINTER_STATUS_USER_INTERVENTION "Poseg uporabnika; "
    PD32_PRINTER_STATUS_OUT_OF_MEMORY     "Zmanjkalo je pomnilnika; "
    PD32_PRINTER_STATUS_DOOR_OPEN         "Odprto ohi�je; "
    PD32_PRINTER_STATUS_SERVER_UNKNOWN    "Neznan stre�nik; " // not
    PD32_PRINTER_STATUS_POWER_SAVE        "V pripravljenosti; " // not
}
